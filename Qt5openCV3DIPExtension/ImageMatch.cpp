#include "ImageMatch.h"
using namespace cv;
using namespace std;
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