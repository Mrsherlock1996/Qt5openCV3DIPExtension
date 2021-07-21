#include "ImageMatch.h"
using namespace cv;
using namespace std;


//DPM分类器结果值转换函数
void ImageMatch::_dpmLabelInt2String(cv::String text, int ID)
{
	//转换依赖于xml和classNames
	switch(ID)
	{
	case 0:	text = "aeroplane";
	case 1:	text = "bicycle";
	case 2:	text = "bird";
	case 3:	text = "boat";
	case 4:	text = "bottle";
	case 5:	text = "bus";
	case 6:	text = "car";
	case 7:	text = "cat";
	case 8:	text = "chair";
	case 9:	text = "cow";
	case 10:	text = "diningtable";
	case 11:	text = "dog";
	case 12:	text = "horse";
	case 13:	text = "inriaperson";
	case 14:	text = "motorbike";
	case 15:	text = "person";
	case 16:	text = "pottedplant";
	case 17:	text = "sheep";
	case 18:	text = "sofa";
	case 19:	text = "train";
	case 20:	text = "tvmonitor";
	}
}
ImageMatch::ImageMatch(QObject *parent)
	: QObject(parent)
{
	_cvtImg = new ConvertMatQImage;

}

ImageMatch::ImageMatch()
{
	_cvtImg = new ConvertMatQImage;
}

ImageMatch::~ImageMatch()
{
	delete _cvtImg;
}


//Haaris角点检测
void ImageMatch::imgMatchHaar()
{

	if (_flagHaar)
	{
		string path = _matchImgPath.toStdString();
		Mat trainImg=imread(path);//角点检测无需匹配
		Mat gray;
		Mat result;
		QImage resultImg;
		cvtColor(trainImg, gray, COLOR_BGR2GRAY);

		////角点检测
		////角点检测一般使用该函数
		//goodFeaturesToTrack(gray, _haarKeypoints, 1000, 0.01,10,Mat(),3,true);//true使用haar算法,false则shitomasi
		////亚像素算法,详细去CSDN查看笔记
		////一旦为q找到一个新的位置，算法将使用该值作为起始点进行迭代，
		////	并直到达到用户指定的终止标准。
		////	这个标准可以是cv::TermCriteria::MAX＿ITER类型
		////	或cv::TermCriteria::EPS类型（或两者）
		////	使用cv::TermCriteria::EPS将有效表示你需要的亚像素值的准确性。
		////	因此，如果指定0.10，则要求亚像素精度在像素的1／10以内。
		////	通常用cv::TermCriteria()函数构造。
		//Size subPixWinSize(10, 10);
		//TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
		//cornerSubPix(gray, _haarKeypoints, subPixWinSize, Size(-1, -1),termcrit);

		//haar角点检测
		cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(1000, 0.3, 5.0, 3, true);//true使用haar算法,false则shitomasi
		gftt->detect(gray, _haarKeypoints);

		//绘制并显示
		drawKeypoints(gray, _haarKeypoints, result);
		resultImg = _cvtImg->matToQImage(&result);
		//缩放
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize,Qt::KeepAspectRatio);//图片原比例缩放到label中
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//显示
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
	else
	{
		_uiLabelShowImg->setText("Please set flagHaar=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}

}

//Shi-Tomasi角点检测
void ImageMatch::imgMatchShitomasi()
{
	if (_flagShitomasi)
	{
		string path = _matchImgPath.toStdString();
		Mat trainImg = imread(path);//角点检测无需匹配
		Mat gray;
		Mat result;
		QImage resultImg;
		cvtColor(trainImg, gray, COLOR_BGR2GRAY);
		cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(1000, 0.3, 5.0, 3, false);//true使用haar算法,false则shitomasi
		gftt->detect(gray, _haarKeypoints);

		//绘制并显示
		drawKeypoints(gray, _haarKeypoints, result);
		resultImg = _cvtImg->matToQImage(&result);
		//缩放
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//图片原比例缩放到label中
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//显示
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagShitomasi=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}

}


//SIFT检测
void ImageMatch::imgMatchSift()
{
	if(_flagSift)
	{
		string srcPath = _trainImgPath.toStdString();//获取两张图片路径
		string testPath = _matchImgPath.toStdString();//
		Mat trainImg = imread(srcPath);//创建图片Mat对象
		Mat testImg = imread(testPath);
		//Mat gray;//暂不需要灰度图
		Mat result;
		//cvtColor(trainImg, gray, COLOR_BGR2GRAY);

		int numFeatures = 100;
		int nOctaveLayers = 3;

		//对两图片进行SIFT特征检测
		Ptr<SIFT>detector = SIFT::create(numFeatures, nOctaveLayers);
		detector->detectAndCompute(trainImg, Mat(), _siftKeypoints, _siftDescriptor, false);//false为自动检测关键点
		detector->detectAndCompute(testImg, Mat(), _siftKeypoints2, _siftDescriptor2, false);
		//FLANN匹配
		vector<vector<DMatch>>matches;
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);//使用FLANN方法匹配
		matcher->knnMatch(_siftDescriptor, _siftDescriptor2, matches, 2);

		//匹配值过滤
		//-- Filter matches using the Lowe's ratio test, SIFT's author
		vector<DMatch>goodMatches; //获得的较好的匹配点
		const float ratioThresh = 0.7f;
		for (int i= 0; i < matches.size(); i++)
		{
			if (matches[i][0].distance < ratioThresh*matches[i][1].distance)
			{
				goodMatches.push_back(matches[i][0]);
			}
		}

		//绘制匹配结果
		drawMatches(trainImg, _siftKeypoints, testImg, _siftKeypoints2, goodMatches, result);

		QImage resultImg = _cvtImg->matToQImage(&result);
		//缩放显示
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//图片原比例缩放到label中
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//显示
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagSift=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
}


//SURF检测
void ImageMatch::imgMatchSurf()
{
	using namespace cv::xfeatures2d; //SURF需要
	if (_flagSurf)
	{
		string srcPath = _trainImgPath.toStdString();//获取两张图片路径
		string testPath = _matchImgPath.toStdString();//
		Mat trainImg = imread(srcPath);//创建图片Mat对象
		Mat testImg = imread(testPath);
		//Mat gray;//暂不需要灰度图
		Mat result;
		//cvtColor(trainImg, gray, COLOR_BGR2GRAY);

		int numFeatures = 100;
		int nOctaveLayers = 3;

		//对两图片进行SIFT特征检测
		Ptr<SURF>detector = SURF::create(numFeatures, nOctaveLayers);
		detector->detectAndCompute(trainImg, Mat(), _surfKeypoints, _surfDescriptor, false);//false为自动检测关键点
		detector->detectAndCompute(testImg, Mat(), _surfKeypoints2, _surfDescriptor2, false);
		//FLANN匹配
		vector<vector<DMatch>>matches;
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);//使用FLANN方法匹配
		matcher->knnMatch(_surfDescriptor, _surfDescriptor2, matches, 2);

		//匹配值过滤
		//-- Filter matches using the Lowe's ratio test, SIFT's author
		vector<DMatch>goodMatches; //获得的较好的匹配点
		const float ratioThresh = 0.7f;
		for (int i = 0; i < matches.size(); i++)
		{
			if (matches[i][0].distance < ratioThresh*matches[i][1].distance)
			{
				goodMatches.push_back(matches[i][0]);
			}
		}

		//绘制匹配结果
		drawMatches(trainImg, _surfKeypoints, testImg, _surfKeypoints2, goodMatches, result);

		QImage resultImg = _cvtImg->matToQImage(&result);
		//缩放显示
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//图片原比例缩放到label中
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//显示
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagSurf=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
}


//HOG+SVM行人检测
void ImageMatch::imgMatchHog()
{
	if (_flagHog)
	{
		//获取检测图
		string testPath = _matchImgPath.toStdString();//
		Mat testImg = imread(testPath);
		//HOG+SVM常用于行人检测, 提取HOG描述子,给SVM分类器
		//关联SVM检测器,默认选择人
		_hogDescriptor.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
		//默认构函相当于HOGDescriptor(Size(64,128), Size(16,16), Size(8,8), Size(8,8), 9, 1 )
		//检测结果
		vector<Rect> found;
		_hogDescriptor.detectMultiScale(testImg, found, 0, Size(8, 8), Size(), 1.05, 2, false);
		//检测结果框调整并显示
		for (int i = 0; i < found.size(); i++)
		{
			//调整预测框
			found[i].x += cvRound(found[i].width*0.1);
			found[i].width = cvRound(found[i].width*0.8);
			found[i].y += cvRound(found[i].height*0.07);
			found[i].height = cvRound(found[i].height*0.8);
			//绘制
			rectangle(testImg, found[i], Scalar(0, 255, 0));
			putText(testImg, "pedestrian", Point(found[i].x, found[i].y),
				FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255),1);
		}
		//结果转换并显示
		QImage resultImg = _cvtImg->matToQImage(&testImg);
		//缩放显示
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//图片原比例缩放到label中
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//显示
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagHOG=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}

}


//DPM多类检测
void ImageMatch::imgMatchDpm()
{
	using namespace dpm; //DPM空间, 找不到就去.hpp文件查看

	if (_flagDpm)
	{
		_uiLabelStatus->setText(" BG:DPM runing "); //开始训练, 更新ui状态栏
		std::string path = _matchImgPath.toStdString();  //加载图片

		Mat srcImg = imread(path);//角点检测无需匹配
		Mat testImg = imread(path);//srcImg在检测完后会出现修改,用该对象进行显示操作
		//使用opencv_extra训练好的xml文件
		//修改xml和类名时,务必修改_dpmLabelInt2String()函数
		//引入xml文件
		//DPM+SVM暂时对多类分类还需要debug, 暂时只对猫进行识别
		//vector<string> fileNames =
		//{
		//	//"DPM/aeroplane.xml",
		//	//"DPM/bicycle.xml",
		//	//"DPM/bird.xml",
		//	//"DPM/boat.xml",
		//	//"DPM/bottle.xml",
		//	//"DPM/bus.xml",
		//	//"DPM/car.xml",
		//	//"DPM/cat.xml",
		//	//"DPM/chair.xml",
		//	//"DPM/cow.xml",
		//	//"DPM/diningtable.xml",
		//	//"DPM/dog.xml",
		//	//"DPM/horse.xml",
		//	//"DPM/inriaperson.xml",
		//	//"DPM/motorbike.xml",
		//	//"DPM/person.xml",
		//	//"DPM/pottedplant.xml",
		//	//"DPM/sheep.xml",
		//	//"DPM/sofa.xml",
		//	//"DPM/train.xml",
		//	//"DPM/tvmonitor.xml"
		//};
		////引入类名
		//vector<string> classNames =
		//{
		//	//"aeroplane",
		//	//"bicycle",
		//	//"bird",
		//	//"boat",
		//	//"bottle",
		//	//"bus",
		//	//"car",
		//	//"cat",
		//	//"chair",
		//	//"cow",
		//	//"diningtable",
		//	//"dog",
		//	//"horse",
		//	//"inriaperson",
		//	//"motorbike",
		//	//"person",
		//	//"pottedplant",
		//	//"sheep",
		//	//"sofa",
		//	//"train",
		//	//"tvmonitor"
		//};
		//创建检测器
		Ptr<DPMDetector> detector = DPMDetector::create(_fileNames,_classNames);
		//创建目标存储
		//ObjectDetection中包含了:rectangulars, scores and class IDs.
		vector<DPMDetector::ObjectDetection> objects;
		detector->detect(srcImg, objects);
		cv::String label;//类标签
		//绘制显示内容
		for (int i = 0; i < objects.size(); i++)
		{
			//暂时只实现对猫的识别
			//if (objects[i].score>0) //对人时, 只显示评分大于0的
			//{
				rectangle(testImg, objects[i].rect, Scalar(0, 255, 0));		//绘制目标框
				//_dpmLabelInt2String(label, objects[i].classID);		//分类结果值转换
				label = _classNames[0] + " score: "+ to_string(objects[i].score);
				putText(testImg, label, Size(objects[i].rect.x, objects[i].rect.y),  //目标框上标注文字
					FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255));
			//}

		}
		//namedWindow("img");
		//imshow("img", testImg);
		QImage resultImg = _cvtImg->matToQImage(&testImg);
		//缩放
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//图片原比例缩放到label中
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//显示
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
		_uiLabelStatus->setText(" BG:nothing "); //开始训练, 更新ui状态栏

	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagShitomasi=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
}


//模板匹配
void ImageMatch::imgTemplateMatch()
{
	if (_flagTemplate)
	{
		string srcPath = _trainImgPath.toStdString();//获取模板图片
		string testPath = _matchImgPath.toStdString();//获取匹配图片
		Mat tempImg = imread(srcPath);//创建图片Mat对象
		Mat testImg = imread(testPath);
		Mat result;
		Point minLoc;
		double minValue;
		matchTemplate(testImg, tempImg, result, TM_SQDIFF_NORMED);
		//这里使用方差归一化匹配,最小值为最优匹配点
		//对匹配结果去进行最小值扫描
		minMaxLoc(result, &minValue, 0, &minLoc, 0);
		//将其定位
		Size size = tempImg.size();
		rectangle(testImg, Rect(minLoc, size), Scalar(0,255,0), 2, 8);
		QImage resultImg = _cvtImg->matToQImage(&testImg);
		//缩放显示
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//图片原比例缩放到label中
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//显示
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagTemplate=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//居中
	}
}
