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
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
	Ui::MainWindowClass ui;
	//״̬������
	QTimer* _timer;
	QLabel* _timeLabel; //ʱ��
	QLabel* _welcom; //��ӭ
	QLabel* _backGroundProcess; //��̨����
	QLabel* _nowProcess; //��ǰ����
public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	
	//FaceRec����
	FaceRec* _faceRec;//����ʶ�����ʾ
	FaceRec* _faceTrain;//ר������ѵ��
	QThread* _threadFR; //��������ʶ����߳�
	QThread* _threadFRTrain;
		//��Ҫ��־
	bool _flagGenXML = false; //���±�־Ϊ������ʱQLabelΪ��,����bug
	bool _flagSelecteXML = false;

	//Ŀ���ⲿ��

	//���п�ʶ�𲿷�

	//CNN����
signals:
	//FaceRec����
	void _FRStartTrain(); //��ʼѵ���ź�
	void _FRStartRec(); //��ʼ����ź�
	void _FRStartCommonRec(); //��ʼ���ڼ���ź�
	void _FROnlyShow();//��ʼֻ��ʾ�ź�
	void _FRStopRec(); //ֹͣ����ź�

	//Ŀ���ⲿ��

	//���п�ʶ�𲿷�

	//CNN����


public slots:
	//ʱ����ʾ
	void showCurrentTime();
	//FaceRec
	void iniFaceRec();
	void on_pushButtonFRGenXML_clicked();//����xml�ļ�(ѵ��)
	void on_pushButtonFRSelectXML_clicked();//ѡ�����е�xml�ļ�
	void on_pushButtonFRGenTxt_clicked(); //����txt�ļ�(�ļ�����)
	void on_pushButtonFRSelectTxt_clicked();//ѡ������txt�ļ�

	void on_pushButtonFRRunRecByGen_clicked();//�������ɵ�xmlʶ��
	void on_pushButtonFRRunRecBySelecte_clicked();  //����ѡ���xmlʶ��
	void on_pushButtonFRCommonFaceRec_clicked();//��ͨ����ʶ��
	void on_pushButtonFROnlyCamera_clicked();//ֻ�����Ƶ��

	void on_pushButtonFRStop_clicked(); //ֹͣʶ����


	//ObjectDetect

	//���п�ʶ��

	//CNN
};
