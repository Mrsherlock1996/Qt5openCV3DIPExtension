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