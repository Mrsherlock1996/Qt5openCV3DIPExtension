//
//FaceRec类实现的功能:
//检测功能:
//	识别所有人脸,标注红框
//	识别身份人脸,标注绿框
//	识别结果显示在调用的ui->QLabel中
//
//训练功能:
//	由于调用FaceRecognizer派生类来训练模型需要耗时很久,所以将训练和检测分为两个接口,
//	使用haarcascadeclassifier训练同样耗时较久,因此:
//		训练后生成xml文件,命名格式"指定的路径+模型所属类名.XML"
//		检测时直接加载给出的xml文件,进行检测
//
//
//
//人脸识别的两种方法
//方法1:
//		通过CascadeClassifier检测到人脸,再加载训练好的包含"身份"信息的xml, 即CascadeClassifier的训练信息, 创建第二个Cascade对象, 对该人脸识别确定对应身份
//方法2:
//		先使用cv::face::FaceReconizer创建对象,对"身份"人脸进行训练, 再利用cascade classifier检测人脸, 使用FaceRecognizer对象进行预测
//
//cv::face模块
//cv::face::EigenFaceRecognizer and cv::face::FisherFaceRecognizer都继承于cv::face::BasicFaceRecognizer,
//	二者都基于一个premise assumption :
//		    输入的图片检测和训练时相同尺寸的equal size(暂定800,800),并且是灰度图,
//	二者都不可以更新对象
//cv::Algorithm::FaceRecognizer::BasicFaceRecognizer
//对于BasicFaceRecognizer而言其输入图片并没有上述的要求,只需要和往常一样image和label对应即可
//	BasicFaceRecognizer可以更新对象
//
//txt文件的格式:
//	图片1路径
//	标签
//	图片2路径
//	标签
//
//
#pragma once

#include <QObject>
#include <opencv2/face.hpp>
#include <opencv2/opencv.hpp>
#include <qlabel.h>
#include "ConvertMatQImage.h"
#include <iostream>
#include <vector>
#include <QThread>
class FaceRec : public QObject
{
	Q_OBJECT

public:
	FaceRec(QObject *parent);
	FaceRec();
	~FaceRec();

	std::vector<cv::Mat> _images;//存储train set
	std::vector<int> _labels;//存储train set labels

	cv::Size _size; //用于记录EigenFaceRec时所需要的尺寸,初始化默认为800,800,保证train和predict时size一致
	QSize _labelSize;
	cv::VideoCapture _camera; //摄像头

	//必要标志
	bool _trainState = false;//训练开始标志开关
	bool _trainResult = false;//训练结果标志
	bool _recState=false;//是否开始人脸检测标志, true时识别, false不进行识别
	bool _cameraState = false; //摄像头状态控制帧循环, 外部开启摄像头时,先将其置为true
	//_cameraState是将摄像头控制权给予了外部,避免程序自行调用

	//必要参数
	QString _trainSetTxtFilePathQStr; //训练集信息文件路径
	QString _resultXMLFilePath;//训练结果保存路径
	std::string _haarFaceDataPath; //opencv给出的训练好的haarcascade的xml文件路径,用于大众人脸识别
	std::string _trainSetTxtFilePathStr; //训练集信息文件路径"C:/a.txt"
	QString _modelXmlAbsPath; //模型xml文件
	QLabel* _uiLabel; //显示组件
	QLabel* _uiLabelCom;//大众显示组件
	QLabel* _uiStatusBarLabel;//更新ui界面状态栏后台Label

	//训练
	void startTrain();//训练模型,产生xml
	//检测
	void begainToRec();//直接将结果显示到ui中
	void begainToCommonFaceRec();//大众人脸识别
	//只实时显示
	void onlyRealTimeShow();
};
