//ImageMatch
//����:
//			ʵ��ͼƬ�ؼ���ļ��, �����ӵ���ȡ, �Ͷ���ͼƬ��ƥ��
//			��ƥ����չʾ����Ӧ��uiLabel��
//�ӿ�:
//			������������㷨,���ӿڽ��з���,��ʵ��Ŀ���Ϊ��������
//ע���:
//			��ʱֻ����˵�ͼ���
#pragma once
#include <QObject>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <xfeatures2d.hpp>//SIFT��
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
	//���ܺ���:���������ķ�����ֵת���ɱ�ǩ 
	void _dpmLabelInt2String(cv::String text, int ID);
public:
	ImageMatch(QObject *parent);
	ImageMatch();
	~ImageMatch();
	//ͼƬ·��
	QString _trainImgPath;	//ѵ��ͼƬor��һ֡ͼƬ
	QString _matchImgPath;	//ƥ��/����ͼƬor��һ֡ͼƬ
	//DPM���
	std::vector<std::string> _fileNames;
	std::vector<std::string> _classNames;
	//�ؼ���
	std::vector<cv::KeyPoint> _haarKeypoints; //��ͼƬ�ؼ���
	std::vector<cv::KeyPoint> _shitomasiKeypoints;
	std::vector<cv::KeyPoint> _siftKeypoints;
	std::vector<cv::KeyPoint> _surfKeypoints;
	//std::vector<cv::KeyPoint> _lbpKeypoints;
	std::vector<cv::KeyPoint> _hogKeypoints;
	std::vector<cv::KeyPoint> _dpmKeypoints;

	std::vector<cv::KeyPoint> _haarKeypoints2;		//2��ʾ�������ͼƬ�Ĺؼ���
	std::vector<cv::KeyPoint> _shitomasiKeypoints2;
	std::vector<cv::KeyPoint> _siftKeypoints2;
	std::vector<cv::KeyPoint> _surfKeypoints2;
	//std::vector<cv::KeyPoint> _lbpKeypoints2;
	std::vector<cv::KeyPoint> _hogKeypoints2;
	std::vector<cv::KeyPoint> _dpmKeypoints2;
	//������
	//cv::Mat _haarDescriptor;
	//cv::Mat _shitomasiDescriptor;
	cv::Mat _siftDescriptor;
	cv::Mat _surfDescriptor;
	//cv::Mat _lbpDescriptor;
	cv::HOGDescriptor _hogDescriptor;
	cv::Mat _dpmDescriptor;

	cv::Mat _siftDescriptor2;	//2��ʾ��ͼƬ,���ƥ��ͼƬ
	cv::Mat _surfDescriptor2;
	//cv::Mat _lbpDescriptor2;
	cv::HOGDescriptor _hogDescriptor2;
	cv::Mat _dpmDescriptor2;
	//�����̱�־
	bool _flagHaar = false; //�����ⲿ������Ӧ�㷨�Ĺ��ܺ���
	bool _flagShitomasi = false;
	bool _flagSift = false;
	bool _flagSurf = false;
	//bool _flagLbp = false;
	bool _flagHog = false;
	bool _flagDpm = false;
	bool _flagTemplate = false;

	//���ܽӿ�
	//ʹ�ò�ͬ�㷨���: �ؼ���, ������, �����ƥ��,����ƥ����
	void imgMatchHaar(); 
	void imgMatchShitomasi();
	void imgMatchSift();
	void imgMatchSurf();
	//void imgMatchLbp();
	void imgMatchHog();
	void imgMatchDpm();
	void imgTemplateMatch();
	//���ݲ���
	//ui
	QLabel* _uiLabelShowImg;//������ʾ����
	QLabel* _uiLabelStatus; //����״̬��

};
