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
class FaceRec : public QObject
{
	Q_OBJECT

public:
	FaceRec(QObject *parent);
	FaceRec();
	~FaceRec();
	std::string _haarFaceDataPath; //opencv给出的训练好的haarcascade的xml文件路径,用于大众人脸识别
	std::string _trainSetTxtFilePath; //训练集信息文件路径"C:/a.txt"

	
	std::vector<cv::Mat> _images;//存储train set
	std::vector<int> _labels;//存储train set labels
	//只需要标签和图片信息是对应的即可完成多人身份识别

	cv::Size _size; //用于记录EigenFaceRec时所需要的尺寸,初始化默认为800,800,保证train和predict时size一致
	bool _trainState = false;//训练开始标志开关
	bool _recState=false;//是否开始识别的标志, true时识别, false不进行识别

	void begainToRec(QString modelXmlAbsPath,QLabel* uiLabel );//直接将结果显示到ui中

	void startTrain(QString trainSetTxtFilePath, QString resultXMLFilePath);
};
