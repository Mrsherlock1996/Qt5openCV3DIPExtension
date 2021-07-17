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
	FaceRec* _faceRec;//����ʶ�����ʾ
	FaceRec* _faceTrain;//ר������ѵ��
	QThread* _threadFR; //��������ʶ����߳�
	QThread* _threadFRTrain;
signals:
	void _FRStartTrain(); //��ʼѵ���ź�
	void _FRStartRec(); //��ʼ����ź�
	void _FRStartCommonRec(); //��ʼ���ڼ���ź�
	void _FROnlyShow();//��ʼֻ��ʾ�ź�
	void _FRStopRec(); //ֹͣ����ź�
private:
    Ui::MainWindowClass ui;

public slots:
	//FaceRec
	void iniFaceRec();
	void on_pushButtonFRGenXML_clicked();//����xml�ļ�(ѵ��)
	void on_pushButtonFRSelectXML_clicked();//ѡ�����е�xml�ļ�
	void on_pushButtonFRGenTxt_clicked(); //����txt�ļ�(�ļ�����)
	void on_pushButtonFRSelectTxt_clicked();//ѡ������txt�ļ�

	void on_pushButtonFRRunRecByGen_clicked();//�������ɵ�xmlʶ��
	void on_pushButtonFRRunRecBySelecte_clicked();  //����ѡ���xmlʶ��

	void on_pushButtonFRStop_clicked(); //ֹͣʶ����

	void on_pushButtonFRCommonFaceRec_clicked();//��ͨ����ʶ��

	void on_pushButtonFROnlyCamera_clicked();
	//ObjectDetect

	//���п�ʶ��

	//CNN
};
