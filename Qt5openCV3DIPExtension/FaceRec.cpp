#include "FaceRec.h"

FaceRec::FaceRec(QObject *parent)
	: QObject(parent)
{
	_size = cv::Size(800, 800); //保证train和predict时size一致
}
FaceRec::FaceRec()
{
	_size = cv::Size(800, 800);
}
FaceRec::~FaceRec()
{
}
//训练模型函数
//输入训练集的txt信息文件的读取路径 和 模型的xml文件的保存所属文件夹路径
//调用该函数应先打开标志开关为true
//形参2, 不是文件路径"a/b/aa.xml"而是文件夹路径"a/b"
void FaceRec::startTrain()
{
	using namespace std;
	using namespace cv;
	if (_trainState == true) //训练标志开关
	{
		//人脸检测使用的opencv训练好的cascade分类器的xml文件:
		string _haarFaceDataPath = "haarcascade_frontalface_alt_tree.xml"; //haarcascade_frontalface_alt_tree.xml 已放到项目文件中
		string _trainSetTxtFilePathStr = _trainSetTxtFilePathQStr.toStdString(); //fileName是txt文件
		ifstream file(_trainSetTxtFilePathStr, ifstream::in);//file是存储身份人脸的txt文件,

		//打开文件,读取图片和标签
		if (file)
		{
			//获取身份人脸train set

			string samplePath, sampleLabel;//用于存储单个样本的路径和对应标签
			Mat resizeScaleAndGray;
			while (getline(file, samplePath)) //读取第一行,即样本路径
			{
				getline(file, sampleLabel);//读取第二行,即样本标签
				if (!sampleLabel.empty() && !samplePath.empty())
				{
					stringstream temp;//将标签转换成int
					int labelInt = 0;
					temp << sampleLabel;
					temp >> labelInt;

					//建立trains set 
					_labels.push_back(labelInt);//填充数据集annotation
					//固定尺寸EigenFaceRecognizer需要训练和检测时固定尺寸且为灰度图
					resizeScaleAndGray = imread(samplePath, IMREAD_GRAYSCALE);//灰度化本帧
					resize(resizeScaleAndGray, resizeScaleAndGray, _size);//使用初始化时的尺寸
					_images.push_back(resizeScaleAndGray);//填充到数据集images中

					//debug使用
					//int i = 0;
					//cout << "samplePath:" << samplePath << endl;
					//cout << "sampleLabel:" << sampleLabel << endl;
					//cout << "labels[" << i << "]: " << labels[i] << endl;
					//cout << "images[" << i << "]: " << images[i] << endl;
					//i++;
				}
			}

			////建立简单交叉验证数据debug使用
			////看一下样本的宽高
			//int height = images[0].rows;
			//int width = images[0].cols;
			//cout << "imageHeight= " << height << endl;
			//cout << "imageWidth= " << width << endl;
			////简单的交叉验证, 取前S-1个样本, 保留最后一个样本作为test set来检测model预测能力
			//Mat testSample = images[int(images.size()) - 1];
			//int testLabel = labels[labels.size() - 1];
			//cout << "testLabel=" << testLabel << endl;
			//namedWindow("testImage", WINDOW_AUTOSIZE);
			//imshow("testImage", testSample);
			//cv::waitKey(60);
			////弹出test 样本(即最后一个sample,避免纳入训练)
			//images.pop_back();
			//labels.pop_back();

			//创建cv::face::BasicFaceRecognizer对象
			Ptr<cv::face::BasicFaceRecognizer> model = cv::face::EigenFaceRecognizer::create();
			model->train(_images, _labels);
			//训练后生成xml文件, 命名格式"指定的路径+模型所属类名.XML"
			model->save(_resultXMLFilePath.toStdString() + "/EigenFaceRec.xml");
			_trainResult = true;//训练ok结果更新
		}
		else
		{
			_trainResult = false;//没有进行训练, 更新训练结果
		}
	}
}

//开始进行人脸检测,及身份识别
//通过传递的xml文件路径来身份识别, 视频显示在传递的QLabel指针中
//应在识别前打开_cameraState和_recState
void FaceRec::begainToRec()
{
	using namespace std;
	using namespace cv;
	if (_recState == true) //应在调用函数前将标志开关使能
	{
		//人脸检测使用的opencv训练好的cascade分类器的xml文件:
		//			haarcascade_frontalface_alt_tree.xml 已放到项目文件中
		string _haarFaceDataPath = "haarcascade_frontalface_alt_tree.xml";
		string xmlFilePath = _modelXmlAbsPath.toStdString();


		//创建cv::face::BasicFaceRecognizer对象
		Ptr<cv::face::BasicFaceRecognizer> model = cv::face::EigenFaceRecognizer::create();
		model->read(xmlFilePath); //加载训练好的身份识别模型



		//调用摄像头, 识别人脸
		//注意需要将帧数转换到QLabel中进行显示,需要Mat和QImage的转换
		//使用opencv提供好的CascadeClassifierXML文件识别大众人脸

		//Open Camera 
		//需要外部指令将_cameraState设置成tru才能进入帧识别
		_camera = VideoCapture (0);//调用默认摄像头
		if (_camera.isOpened())
		{
			Mat frame; //帧
			vector<Rect> faces; //检测大众人脸结果
			Mat recFace; //识别的人脸
			CascadeClassifier detector(_haarFaceDataPath); //加载级联分类器
			ConvertMatQImage cvt; //用于Mat和QImage转换
			QImage img;
			_camera.read(frame);
			//对每一帧进行识别检测
			while (_camera.read(frame)&&_cameraState)  //这样可以通过外部指令将_cameraState写为false就能停止帧识别
			{
				flip(frame, frame, 1);//图片左右翻转
				//对当前帧frame进行多尺度haar人脸检测
				Mat resizeAndGray;
				cvtColor(frame, resizeAndGray, COLOR_BGR2GRAY);
				resize(resizeAndGray, resizeAndGray, _size);//使用初始化时的尺寸,保证train和predict时size一致
				detector.detectMultiScale(resizeAndGray, faces, 1.1, 3, 0, Size(50, 50), Size(1000, 1000)); //对本帧的resize副本进行检测

				
				int label = _labels[0];//由于身份标签中每个labels都是一样,所以随机取第一个
				int recLabel;//预测标签值

				//框出所有人脸到该帧
				for (int i = 0; i < faces.size(); i++)
				{
					Mat face = frame(faces[i]);//从当前帧中根据Rect坐标裁剪出定位到的人脸

					//使用BasicFaceRecognizer对象进行predict
					stringstream temp;
					temp << model->predict(face);
					temp >> recLabel;

					//对检测到的人脸进行身份识别,就是预测值和标签值匹配
					//对应绘制目标框
					if (recLabel == label)
					{
						//绘制green框,文字
						rectangle(frame, faces[i], Scalar(0, 255, 0), 1, 8, 0);
						putText(frame, string("master"), faces[i].tl(), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0), 2, 8);
					}
					else
					{
						rectangle(frame, faces[i], Scalar(0, 0, 255), 1, 8, 0);
						putText(frame, string("passerby"), faces[i].tl(), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0), 2, 8);
					}
				}
				//显示到指定ui->label中,跳出for循环是因为可能存在多个无身份人脸
				img = cvt.matToQImage(&frame);
				_uiLabel->setPixmap(QPixmap::fromImage(img));
			}
		}
		else
		{
			qDebug() << "camera open failed\n";
			_cameraState = false; //没有打开摄像头,自动置为false
		}
	}
}


void FaceRec::begainToCommonFaceRec()
{
	using namespace std;
	using namespace cv;
	if (_recState == true) //应在调用函数前将标志开关使能
	{
		//人脸检测使用的opencv训练好的cascade分类器的xml文件:
		//			haarcascade_frontalface_alt_tree.xml 已放到项目文件中
		string _haarFaceDataPath = "haarcascade_frontalface_alt_tree.xml";

		//调用摄像头, 识别人脸
		//注意需要将帧数转换到QLabel中进行显示,需要Mat和QImage的转换
		//使用opencv提供好的CascadeClassifierXML文件识别大众人脸

		//Open Camera 
		//需要外部指令将_cameraState设置成tru才能进入帧识别

		_camera = VideoCapture(0);//调用默认摄像头
		if (_camera.isOpened())
		{
			Mat frame; //帧
			vector<Rect> faces; //检测大众人脸结果
			Mat recFace; //识别的人脸
			CascadeClassifier detector(_haarFaceDataPath); //加载级联分类器
			ConvertMatQImage cvt; //用于Mat和QImage转换

			QImage img;
			_camera.read(frame);
			//对每一帧进行识别检测
			while (_camera.read(frame) && _cameraState)  //这样可以通过外部指令将_cameraState写为false就能停止帧识别
			{
				flip(frame, frame, 1);//图片左右翻转
				//对当前帧frame进行多尺度haar人脸检测
				//Mat resizeAndGray;
				//cvtColor(frame, resizeAndGray, COLOR_BGR2GRAY);
				//resize(resizeAndGray, resizeAndGray, _size);//使用初始化时的尺寸,保证train和predict时size一致
				detector.detectMultiScale(frame, faces, 1.1, 3, 0, Size(50, 50), Size(1000, 1000)); //对本帧的resize副本进行检测

				//框出所有人脸到该帧
				for (int i = 0; i < faces.size(); i++)
				{
					Mat face = frame(faces[i]);//从当前帧中根据Rect坐标裁剪出定位到的人脸
					rectangle(frame, faces[i], Scalar(255, 0, 0), 1, 8, 0);
					putText(frame, string("FACE"), faces[i].tl(), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0), 2, 8);					
				}
				//显示到指定ui->label中,跳出for循环是因为可能存在多个无身份人脸

				img = cvt.matToQImage(&frame);
				_uiLabelCom->setPixmap(QPixmap::fromImage(img));

			}
		}
		else
		{
			qDebug() << "camera open failed\n";
			_cameraState = false; //没有打开摄像头,自动置为false
		}
	}
}

void FaceRec::onlyRealTimeShow()
{
	using namespace std;
	using namespace cv;
	//if (_recState == true)//仅检测,无需使能
	if (true) 
	{
		//人脸检测使用的opencv训练好的cascade分类器的xml文件:
		//			haarcascade_frontalface_alt_tree.xml 已放到项目文件中
		string _haarFaceDataPath = "haarcascade_frontalface_alt_tree.xml";

		//调用摄像头, 识别人脸
		//注意需要将帧数转换到QLabel中进行显示,需要Mat和QImage的转换
		//使用opencv提供好的CascadeClassifierXML文件识别大众人脸

		//Open Camera 
		//需要外部指令将_cameraState设置成tru才能进入帧识别
		qDebug() << "FaceRec thread: " << QThread::currentThread() << endl;
		_camera = VideoCapture(0);//调用默认摄像头
		if (_camera.isOpened())
		{
			Mat frame; //帧
			ConvertMatQImage cvt; //用于Mat和QImage转换


			QImage img;
			_camera.read(frame);
			//显示每一帧
			while (_camera.read(frame) && _cameraState)  //这样可以通过外部指令将_cameraState写为false就能停止帧识别
			{
				flip(frame, frame, 1);//图片左右翻转
				img = cvt.matToQImage(&frame);
				_uiLabelCom->setPixmap(QPixmap::fromImage(img));
			}
		}
		else
		{
			qDebug() << "camera open failed\n";
			_cameraState = false; //没有打开摄像头,自动置为false
		}
	}
}

