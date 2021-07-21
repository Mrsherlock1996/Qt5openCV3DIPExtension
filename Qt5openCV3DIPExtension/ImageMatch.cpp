#include "ImageMatch.h"
using namespace cv;
using namespace std;


//DPM���������ֵת������
void ImageMatch::_dpmLabelInt2String(cv::String text, int ID)
{
	//ת��������xml��classNames
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


//Haaris�ǵ���
void ImageMatch::imgMatchHaar()
{

	if (_flagHaar)
	{
		string path = _matchImgPath.toStdString();
		Mat trainImg=imread(path);//�ǵ�������ƥ��
		Mat gray;
		Mat result;
		QImage resultImg;
		cvtColor(trainImg, gray, COLOR_BGR2GRAY);

		////�ǵ���
		////�ǵ���һ��ʹ�øú���
		//goodFeaturesToTrack(gray, _haarKeypoints, 1000, 0.01,10,Mat(),3,true);//trueʹ��haar�㷨,false��shitomasi
		////�������㷨,��ϸȥCSDN�鿴�ʼ�
		////һ��Ϊq�ҵ�һ���µ�λ�ã��㷨��ʹ�ø�ֵ��Ϊ��ʼ����е�����
		////	��ֱ���ﵽ�û�ָ������ֹ��׼��
		////	�����׼������cv::TermCriteria::MAX��ITER����
		////	��cv::TermCriteria::EPS���ͣ������ߣ�
		////	ʹ��cv::TermCriteria::EPS����Ч��ʾ����Ҫ��������ֵ��׼ȷ�ԡ�
		////	��ˣ����ָ��0.10����Ҫ�������ؾ��������ص�1��10���ڡ�
		////	ͨ����cv::TermCriteria()�������졣
		//Size subPixWinSize(10, 10);
		//TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
		//cornerSubPix(gray, _haarKeypoints, subPixWinSize, Size(-1, -1),termcrit);

		//haar�ǵ���
		cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(1000, 0.3, 5.0, 3, true);//trueʹ��haar�㷨,false��shitomasi
		gftt->detect(gray, _haarKeypoints);

		//���Ʋ���ʾ
		drawKeypoints(gray, _haarKeypoints, result);
		resultImg = _cvtImg->matToQImage(&result);
		//����
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize,Qt::KeepAspectRatio);//ͼƬԭ�������ŵ�label��
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//��ʾ
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
	else
	{
		_uiLabelShowImg->setText("Please set flagHaar=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}

}

//Shi-Tomasi�ǵ���
void ImageMatch::imgMatchShitomasi()
{
	if (_flagShitomasi)
	{
		string path = _matchImgPath.toStdString();
		Mat trainImg = imread(path);//�ǵ�������ƥ��
		Mat gray;
		Mat result;
		QImage resultImg;
		cvtColor(trainImg, gray, COLOR_BGR2GRAY);
		cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(1000, 0.3, 5.0, 3, false);//trueʹ��haar�㷨,false��shitomasi
		gftt->detect(gray, _haarKeypoints);

		//���Ʋ���ʾ
		drawKeypoints(gray, _haarKeypoints, result);
		resultImg = _cvtImg->matToQImage(&result);
		//����
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//ͼƬԭ�������ŵ�label��
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//��ʾ
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagShitomasi=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}

}


//SIFT���
void ImageMatch::imgMatchSift()
{
	if(_flagSift)
	{
		string srcPath = _trainImgPath.toStdString();//��ȡ����ͼƬ·��
		string testPath = _matchImgPath.toStdString();//
		Mat trainImg = imread(srcPath);//����ͼƬMat����
		Mat testImg = imread(testPath);
		//Mat gray;//�ݲ���Ҫ�Ҷ�ͼ
		Mat result;
		//cvtColor(trainImg, gray, COLOR_BGR2GRAY);

		int numFeatures = 100;
		int nOctaveLayers = 3;

		//����ͼƬ����SIFT�������
		Ptr<SIFT>detector = SIFT::create(numFeatures, nOctaveLayers);
		detector->detectAndCompute(trainImg, Mat(), _siftKeypoints, _siftDescriptor, false);//falseΪ�Զ����ؼ���
		detector->detectAndCompute(testImg, Mat(), _siftKeypoints2, _siftDescriptor2, false);
		//FLANNƥ��
		vector<vector<DMatch>>matches;
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);//ʹ��FLANN����ƥ��
		matcher->knnMatch(_siftDescriptor, _siftDescriptor2, matches, 2);

		//ƥ��ֵ����
		//-- Filter matches using the Lowe's ratio test, SIFT's author
		vector<DMatch>goodMatches; //��õĽϺõ�ƥ���
		const float ratioThresh = 0.7f;
		for (int i= 0; i < matches.size(); i++)
		{
			if (matches[i][0].distance < ratioThresh*matches[i][1].distance)
			{
				goodMatches.push_back(matches[i][0]);
			}
		}

		//����ƥ����
		drawMatches(trainImg, _siftKeypoints, testImg, _siftKeypoints2, goodMatches, result);

		QImage resultImg = _cvtImg->matToQImage(&result);
		//������ʾ
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//ͼƬԭ�������ŵ�label��
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//��ʾ
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagSift=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
}


//SURF���
void ImageMatch::imgMatchSurf()
{
	using namespace cv::xfeatures2d; //SURF��Ҫ
	if (_flagSurf)
	{
		string srcPath = _trainImgPath.toStdString();//��ȡ����ͼƬ·��
		string testPath = _matchImgPath.toStdString();//
		Mat trainImg = imread(srcPath);//����ͼƬMat����
		Mat testImg = imread(testPath);
		//Mat gray;//�ݲ���Ҫ�Ҷ�ͼ
		Mat result;
		//cvtColor(trainImg, gray, COLOR_BGR2GRAY);

		int numFeatures = 100;
		int nOctaveLayers = 3;

		//����ͼƬ����SIFT�������
		Ptr<SURF>detector = SURF::create(numFeatures, nOctaveLayers);
		detector->detectAndCompute(trainImg, Mat(), _surfKeypoints, _surfDescriptor, false);//falseΪ�Զ����ؼ���
		detector->detectAndCompute(testImg, Mat(), _surfKeypoints2, _surfDescriptor2, false);
		//FLANNƥ��
		vector<vector<DMatch>>matches;
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);//ʹ��FLANN����ƥ��
		matcher->knnMatch(_surfDescriptor, _surfDescriptor2, matches, 2);

		//ƥ��ֵ����
		//-- Filter matches using the Lowe's ratio test, SIFT's author
		vector<DMatch>goodMatches; //��õĽϺõ�ƥ���
		const float ratioThresh = 0.7f;
		for (int i = 0; i < matches.size(); i++)
		{
			if (matches[i][0].distance < ratioThresh*matches[i][1].distance)
			{
				goodMatches.push_back(matches[i][0]);
			}
		}

		//����ƥ����
		drawMatches(trainImg, _surfKeypoints, testImg, _surfKeypoints2, goodMatches, result);

		QImage resultImg = _cvtImg->matToQImage(&result);
		//������ʾ
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//ͼƬԭ�������ŵ�label��
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//��ʾ
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagSurf=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
}


//HOG+SVM���˼��
void ImageMatch::imgMatchHog()
{
	if (_flagHog)
	{
		//��ȡ���ͼ
		string testPath = _matchImgPath.toStdString();//
		Mat testImg = imread(testPath);
		//HOG+SVM���������˼��, ��ȡHOG������,��SVM������
		//����SVM�����,Ĭ��ѡ����
		_hogDescriptor.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
		//Ĭ�Ϲ����൱��HOGDescriptor(Size(64,128), Size(16,16), Size(8,8), Size(8,8), 9, 1 )
		//�����
		vector<Rect> found;
		_hogDescriptor.detectMultiScale(testImg, found, 0, Size(8, 8), Size(), 1.05, 2, false);
		//��������������ʾ
		for (int i = 0; i < found.size(); i++)
		{
			//����Ԥ���
			found[i].x += cvRound(found[i].width*0.1);
			found[i].width = cvRound(found[i].width*0.8);
			found[i].y += cvRound(found[i].height*0.07);
			found[i].height = cvRound(found[i].height*0.8);
			//����
			rectangle(testImg, found[i], Scalar(0, 255, 0));
			putText(testImg, "pedestrian", Point(found[i].x, found[i].y),
				FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255),1);
		}
		//���ת������ʾ
		QImage resultImg = _cvtImg->matToQImage(&testImg);
		//������ʾ
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//ͼƬԭ�������ŵ�label��
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//��ʾ
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagHOG=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}

}


//DPM������
void ImageMatch::imgMatchDpm()
{
	using namespace dpm; //DPM�ռ�, �Ҳ�����ȥ.hpp�ļ��鿴

	if (_flagDpm)
	{
		_uiLabelStatus->setText(" BG:DPM runing "); //��ʼѵ��, ����ui״̬��
		std::string path = _matchImgPath.toStdString();  //����ͼƬ

		Mat srcImg = imread(path);//�ǵ�������ƥ��
		Mat testImg = imread(path);//srcImg�ڼ����������޸�,�øö��������ʾ����
		//ʹ��opencv_extraѵ���õ�xml�ļ�
		//�޸�xml������ʱ,����޸�_dpmLabelInt2String()����
		//����xml�ļ�
		//DPM+SVM��ʱ�Զ�����໹��Ҫdebug, ��ʱֻ��è����ʶ��
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
		////��������
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
		//���������
		Ptr<DPMDetector> detector = DPMDetector::create(_fileNames,_classNames);
		//����Ŀ��洢
		//ObjectDetection�а�����:rectangulars, scores and class IDs.
		vector<DPMDetector::ObjectDetection> objects;
		detector->detect(srcImg, objects);
		cv::String label;//���ǩ
		//������ʾ����
		for (int i = 0; i < objects.size(); i++)
		{
			//��ʱֻʵ�ֶ�è��ʶ��
			//if (objects[i].score>0) //����ʱ, ֻ��ʾ���ִ���0��
			//{
				rectangle(testImg, objects[i].rect, Scalar(0, 255, 0));		//����Ŀ���
				//_dpmLabelInt2String(label, objects[i].classID);		//������ֵת��
				label = _classNames[0] + " score: "+ to_string(objects[i].score);
				putText(testImg, label, Size(objects[i].rect.x, objects[i].rect.y),  //Ŀ����ϱ�ע����
					FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255));
			//}

		}
		//namedWindow("img");
		//imshow("img", testImg);
		QImage resultImg = _cvtImg->matToQImage(&testImg);
		//����
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//ͼƬԭ�������ŵ�label��
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//��ʾ
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
		_uiLabelStatus->setText(" BG:nothing "); //��ʼѵ��, ����ui״̬��

	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagShitomasi=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
}


//ģ��ƥ��
void ImageMatch::imgTemplateMatch()
{
	if (_flagTemplate)
	{
		string srcPath = _trainImgPath.toStdString();//��ȡģ��ͼƬ
		string testPath = _matchImgPath.toStdString();//��ȡƥ��ͼƬ
		Mat tempImg = imread(srcPath);//����ͼƬMat����
		Mat testImg = imread(testPath);
		Mat result;
		Point minLoc;
		double minValue;
		matchTemplate(testImg, tempImg, result, TM_SQDIFF_NORMED);
		//����ʹ�÷����һ��ƥ��,��СֵΪ����ƥ���
		//��ƥ����ȥ������Сֵɨ��
		minMaxLoc(result, &minValue, 0, &minLoc, 0);
		//���䶨λ
		Size size = tempImg.size();
		rectangle(testImg, Rect(minLoc, size), Scalar(0,255,0), 2, 8);
		QImage resultImg = _cvtImg->matToQImage(&testImg);
		//������ʾ
		QSize labelSize = _uiLabelShowImg->size();
		resultImg = resultImg.scaled(labelSize, Qt::KeepAspectRatio);//ͼƬԭ�������ŵ�label��
		_uiLabelShowImg->setPixmap(QPixmap::fromImage(resultImg));//��ʾ
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
	else
	{
		_uiLabelShowImg->setText("Please set _flagTemplate=true !");
		_uiLabelShowImg->setAlignment(Qt::AlignCenter);//����
	}
}
