//ImageMatch
//功能:
//			实现图片关键点的检测, 描述子的提取, 和对新图片的匹配
//			将匹配结果展示在相应的uiLabel中
//接口:
//			根据特征检测算法,将接口进行分类,但实现目标均为上述功能
//注意点:
//			暂时只设计了单图检测
#pragma once
#include <QObject>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <xfeatures2d.hpp>//SIFT等
#include <features2d.hpp>
#include <opencv2/xfeatures2d.hpp>//SURF
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <iostream>
#include <vector>
#include <qlabel.h>
#include "ConvertMatQImage.h"
#include <opencv2/objdetect.hpp>//HOG
#include <opencv2/dpm.hpp>//DPM
#include <string>
class ImageMatch : public QObject
{
	Q_OBJECT
private:
	ConvertMatQImage* _cvtImg;
	//功能函数:将分类器的分类数值转换成标签 
	void _dpmLabelInt2String(cv::String text, int ID);
public:
	ImageMatch(QObject *parent);
	ImageMatch();
	~ImageMatch();
	//图片路径
	QString _trainImgPath;	//训练图片or上一帧图片
	QString _matchImgPath;	//匹配/测试图片or下一帧图片
	//DPM相关
	std::vector<std::string> _fileNames;
	std::vector<std::string> _classNames;
	//关键点
	std::vector<cv::KeyPoint> _haarKeypoints; //旧图片关键点
	std::vector<cv::KeyPoint> _shitomasiKeypoints;
	std::vector<cv::KeyPoint> _siftKeypoints;
	std::vector<cv::KeyPoint> _surfKeypoints;
	//std::vector<cv::KeyPoint> _lbpKeypoints;
	std::vector<cv::KeyPoint> _hogKeypoints;
	std::vector<cv::KeyPoint> _dpmKeypoints;

	std::vector<cv::KeyPoint> _haarKeypoints2;		//2表示输入的新图片的关键点
	std::vector<cv::KeyPoint> _shitomasiKeypoints2;
	std::vector<cv::KeyPoint> _siftKeypoints2;
	std::vector<cv::KeyPoint> _surfKeypoints2;
	//std::vector<cv::KeyPoint> _lbpKeypoints2;
	std::vector<cv::KeyPoint> _hogKeypoints2;
	std::vector<cv::KeyPoint> _dpmKeypoints2;
	//描述子
	//cv::Mat _haarDescriptor;
	//cv::Mat _shitomasiDescriptor;
	cv::Mat _siftDescriptor;
	cv::Mat _surfDescriptor;
	//cv::Mat _lbpDescriptor;
	cv::HOGDescriptor _hogDescriptor;
	cv::Mat _dpmDescriptor;

	cv::Mat _siftDescriptor2;	//2表示新图片,或待匹配图片
	cv::Mat _surfDescriptor2;
	//cv::Mat _lbpDescriptor2;
	cv::HOGDescriptor _hogDescriptor2;
	cv::Mat _dpmDescriptor2;
	//检测进程标志
	bool _flagHaar = false; //用于外部开启相应算法的功能函数
	bool _flagShitomasi = false;
	bool _flagSift = false;
	bool _flagSurf = false;
	//bool _flagLbp = false;
	bool _flagHog = false;
	bool _flagDpm = false;
	bool _flagTemplate = false;

	//功能接口
	//使用不同算法获得: 关键点, 描述子, 并完成匹配,绘制匹配结果
	void imgMatchHaar(); 
	void imgMatchShitomasi();
	void imgMatchSift();
	void imgMatchSurf();
	//void imgMatchLbp();
	void imgMatchHog();
	void imgMatchDpm();
	void imgTemplateMatch();
	//传递参数
	//ui
	QLabel* _uiLabelShowImg;//更新显示界面
	QLabel* _uiLabelStatus; //更新状态栏

};
