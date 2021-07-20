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
	
	bool _flagGenXML = false; //���±�־Ϊ������ʱQLabelΪ��,����bug
	bool _flagSelecteXML = false;

	//ImageMatch����
	ImageMatch* _imgMatch;
	QThread* _threadIM;
	//���п�ʶ�𲿷�

	//CNN����
signals:
	//FaceRec����
	void _FRStartTrain(); //��ʼѵ���ź�
	void _FRStartRec(); //��ʼ����ź�
	void _FRStartCommonRec(); //��ʼ���ڼ���ź�
	void _FROnlyShow();//��ʼֻ��ʾ�ź�
	void _FRStopRec(); //ֹͣ����ź�

	//ͼ������ƥ�䲿��
	void _IMStartHaar();
	void _IMStartShitomasi();
	void _IMStartSIFT();
	void _IMStartSURF();
	void _IMStartLBP();
	void _IMStartHOG();
	void _IMStartDPM();

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


	//ImageMatch
	void iniImgMatch();//��ʼ��
	void on_pushButtonIMAddOldImg_clicked();  //ѡ��ѵ��ͼƬ
	void on_pushButtonIMAddNewImg_clicked();	//ѡ�����ͼƬ
	void on_pushButtonIMOriginOld_clicked();	//��ʾѵ��ͼƬ����ͼ
	void on_pushButtonIMOriginNew_clicked();	//��ʾ���ͼƬ����ͼ
	void on_pushButtonIMClearAll_clicked();	//���ȫ��

	void on_pushButtonIMHaaris_clicked(); //harris�ǵ���
	void on_pushButtonIMShitomasi_clicked();
	void on_pushButtonIMSift_clicked();
	void on_pushButtonIMSurf_clicked();

	//CNN
};
