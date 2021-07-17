#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <qfiledialog.h>
#include "FaceRec.h"
#include <fstream>
#include <qmessagebox.h>
#include <qthread.h>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	FaceRec* _faceRec;//用于识别和显示
	FaceRec* _faceTrain;//专门用于训练
	QThread* _threadFR; //用于人脸识别的线程
	QThread* _threadFRTrain;
signals:
	void _FRStartTrain(); //开始训练信号
	void _FRStartRec(); //开始检测信号
	void _FRStartCommonRec(); //开始大众检测信号
	void _FROnlyShow();//开始只显示信号
	void _FRStopRec(); //停止检测信号
private:
    Ui::MainWindowClass ui;

public slots:
	//FaceRec
	void iniFaceRec();
	void on_pushButtonFRGenXML_clicked();//生成xml文件(训练)
	void on_pushButtonFRSelectXML_clicked();//选择已有的xml文件
	void on_pushButtonFRGenTxt_clicked(); //生成txt文件(文件遍历)
	void on_pushButtonFRSelectTxt_clicked();//选择已有txt文件

	void on_pushButtonFRRunRecByGen_clicked();//基于生成的xml识别
	void on_pushButtonFRRunRecBySelecte_clicked();  //基于选择的xml识别

	void on_pushButtonFRStop_clicked(); //停止识别工作

	void on_pushButtonFRCommonFaceRec_clicked();//普通人脸识别

	void on_pushButtonFROnlyCamera_clicked();
	//ObjectDetect

	//银行卡识别

	//CNN
};
