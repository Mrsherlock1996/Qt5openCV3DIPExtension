#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <qfiledialog.h>
#include "FaceRec.h"
#include <fstream>
#include <qmessagebox.h>
#include <qthread.h>
#include <QLabel>
#include <qtimer.h>
#include <qdatetime.h>
#include "ImageMatch.h"
#include <qinputdialog.h>
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
	Ui::MainWindowClass ui;
	//状态栏部分
	QTimer* _timer;
	QLabel* _timeLabel; //时间
	QLabel* _welcom; //欢迎
	QLabel* _backGroundProcess; //后台程序
	QLabel* _nowProcess; //当前程序
public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	
	//FaceRec部分
	FaceRec* _faceRec;//用于识别和显示
	FaceRec* _faceTrain;//专门用于训练
	QThread* _threadFR; //用于人脸识别的线程
	QThread* _threadFRTrain;
	
	bool _flagGenXML = false; //如下标志为避免检测时QLabel为空,引发bug
	bool _flagSelecteXML = false;

	//ImageMatch部分
	ImageMatch* _imgMatch;
	QThread* _threadIM;
	//银行卡识别部分

	//CNN部分
signals:
	//FaceRec部分
	void _FRStartTrain(); //开始训练信号
	void _FRStartRec(); //开始检测信号
	void _FRStartCommonRec(); //开始大众检测信号
	void _FROnlyShow();//开始只显示信号
	void _FRStopRec(); //停止检测信号
	void _FRStartCommonRecByLBPH(); //LBPH大众人脸识别

	//图像特征匹配部分
	void _IMStartHaar();
	void _IMStartShitomasi();
	void _IMStartSIFT();
	void _IMStartSURF();
	void _IMStartLBP();
	void _IMStartHOG();
	void _IMStartDPM();

	//银行卡识别部分

	//CNN部分


public slots:
	//时间显示
	void showCurrentTime();

	//FaceRec
	void iniFaceRec();
	void on_pushButtonFRGenXML_clicked();//生成xml文件(训练)
	void on_pushButtonFRSelectXML_clicked();//选择已有的xml文件
	void on_pushButtonFRGenTxt_clicked(); //生成txt文件(文件遍历)
	void on_pushButtonFRSelectTxt_clicked();//选择已有txt文件

	void on_pushButtonFRRunRecByGen_clicked();//基于生成的xml识别
	void on_pushButtonFRRunRecBySelecte_clicked();  //基于选择的xml识别
	void on_pushButtonFRCommonFaceRec_clicked();//普通人脸识别
	void on_pushButtonFROnlyCamera_clicked();//只输出视频流
	void on_pushButtonFRComFaceRecLBPH_clicked();
	void on_pushButtonFRStop_clicked(); //停止识别工作


	//ImageMatch
	void iniImgMatch();//初始化
	void on_pushButtonIMAddOldImg_clicked();  //选择训练图片
	void on_pushButtonIMAddNewImg_clicked();	//选择测试图片
	void on_pushButtonIMOriginOld_clicked();	//显示训练图片到大图
	void on_pushButtonIMOriginNew_clicked();	//显示检测图片到大图
	void on_pushButtonIMClearAll_clicked();	//清空全部

	void on_pushButtonIMHaaris_clicked(); //harris角点检测
	void on_pushButtonIMShitomasi_clicked();
	void on_pushButtonIMSift_clicked();
	void on_pushButtonIMSurf_clicked();
	void on_pushButtonIMHog_clicked();
	void on_pushButtonIMDpmStart_clicked(); //DPM
	void on_pushButtonIMDpmXml_clicked(); //DPM 加载xml文件
	void on_pushButtonIMDpmClassName_clicked(); //DPM加载xml文件对应名字
	void on_pushButtonIMTemplate_clicked();


	//CNN
};
