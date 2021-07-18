#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	//ʱ����ʾ����
	 _timeLabel = new QLabel; //��ʾʱ��ı�ǩ
	_timer = new QTimer;
	ui.statusBar->addPermanentWidget(_timeLabel);//״̬�����label
	_timer->start(1000);
	connect(_timer, &QTimer::timeout, this, &MainWindow::showCurrentTime);

	//״̬����ʼ��
	_welcom = new QLabel; 
	_backGroundProcess = new QLabel; 
	_nowProcess = new QLabel;
	_welcom->setText("WELCOM");
	_backGroundProcess->setText(" BG: nothing ");
	_nowProcess->setText(" Now: nothing ");
	ui.statusBar->addWidget(_welcom);
	ui.statusBar->addWidget(_nowProcess);
	ui.statusBar->addWidget(_backGroundProcess);

	//FaceRec����
	iniFaceRec();
	//Ŀ���ⲿ��

	//���п�ʶ�𲿷�

	//CNN����

}
//ʱ����ʾ
void MainWindow::showCurrentTime()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString timestr = currentTime.toString("yyyy-MM-dd- hh:mm:ss"); //������ʾ�ĸ�ʽ
	_timeLabel->setText(timestr);
}

//FaceRec����

//FaceRec��ʼ��
void MainWindow::iniFaceRec()
{
	//��ձ�ǩ
	ui.labelFRGenTxtPath->clear();
	ui.labelFRGenXMLPath->clear();
	ui.labelFRSelectTxtPath->clear();
	ui.labelFRSelectXMLPath->clear();
	ui.labelShowFace->clear();
	ui.labelFRTrainSetRootPath->clear();
	_faceRec = new FaceRec; //��ʼ��һ���������������
	_faceTrain = new FaceRec;//��ʼ��һ��ר������ѵ����FaceRec
	ui.pushButtonFRRunRecByGen->setDisabled(true);  //û�м���XMLʱ����ť�ر�
	ui.pushButtonFRRunRecBySelecte->setDisabled(true);
	//�������߳�
	_threadFR = new QThread;
	_threadFRTrain = new QThread;
	//�������߳�
	_threadFR->start();
	_threadFRTrain->start();
	//�ƶ������߳�
	_faceRec->moveToThread(_threadFR);
	_faceTrain->moveToThread(_threadFRTrain);
	//����
	connect(this, &MainWindow::_FRStartCommonRec, _faceRec, &FaceRec::begainToCommonFaceRec,Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartRec, _faceRec, &FaceRec::begainToRec, Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartTrain, _faceTrain, &FaceRec::startTrain,Qt::QueuedConnection);
	connect(this, &MainWindow::_FROnlyShow, _faceRec, &FaceRec::onlyRealTimeShow, Qt::QueuedConnection);
}

//ѵ��ģ��, ����XML�ļ�����

//����: 
		//ѵ��model,����FaceRecognizer�����ģ��״̬xml�ļ�
		//��ѵ�������uiչʾ
//�������:
		//���е�txt�ļ�·��,  ����xml����·��
//���±�־:
		//_flagGenXML
//ע���:
//�������:
		//
void MainWindow::on_pushButtonFRGenXML_clicked()
{
	_flagGenXML = true;
	//��ȡtxt��xml���·��
	QFileDialog fileDlg;
	QString txtPath = fileDlg.getOpenFileName(
		nullptr, "select a trainSet annotation file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	//ע�������xmlPath�����ļ�·��"a/b/aa.xml"�����ļ���·��"a/b"
	QString xmlPath = fileDlg.getExistingDirectory(
		nullptr, "Select a path to save .XML of your model", QDir::currentPath());

	//����FaceRec::startTrainѵ�����ģ��
	//_faceTrain->startTrain(txtPath, xmlPath);//�Ƕ��߳�
	emit _FRStartTrain();//���߳�

	//���
	if (_faceTrain->_trainResult)//ѵ����ɺ�FaceRec::_trainResultΪtrue
	{
		QMessageBox::information(
			this, "Success", ("Your .XML has been saved into :\n" + xmlPath), QMessageBox::Ok);//�����Ի���
		ui.labelFRGenXMLPath->setText(xmlPath);//��xml�ļ�����·����ʾ��ui��
	}
	else
	{
		ui.labelFRGenXMLPath->setText("TRAINING");//����ui����
	}
}


//ѡ��XML�ļ�����

//����: 
		//ֱ��ѡ��xml�ļ�
		//��ѡ������uiչʾ
//�������:
		//ѡ���xml����·��
//���±�־:
		//_flagSelecteXMl
//ע���:
//�������:
		//
void MainWindow::on_pushButtonFRSelectXML_clicked()
{
	_flagSelecteXML = true;
	QFileDialog fileDlg;
	QString xmlPath = fileDlg.getOpenFileName(
		this,"Select a .xml",QDir::currentPath(),"xml(*.xml);;all(*.*)");
	ui.labelFRSelectXMLPath->setText(xmlPath);
}


//����annotation�ļ�����

//����: 
		//����ѵ����, �γɹ̶���ʽ��txt�ļ�,
		//�����ɽ����uiչʾ
//�������:
		//���е�ѵ�����ļ���Ŀ¼,  ����txt�ı���·��
//ע���:
//�������:
		//
void MainWindow::on_pushButtonFRGenTxt_clicked()
{
	//TrainSetImage
	QFileDialog fileDlg; //�����ļ��Ի����ȡѵ����·��
	QString trainSetRootPath = fileDlg.getExistingDirectory(
		this, "Select train set root path", QDir::currentPath());
	QDir path;
	path.setCurrent(trainSetRootPath);
	QStringList fileNameList = path.entryList(); //����ֻ��ѵ��ͼƬ�ļ�����,��·����Ϣ
	ui.labelFRTrainSetRootPath->setText(trainSetRootPath);//��ѵ�������ڸ�Ŀ¼��ʾ

	//TrainSetImageAnnotation
	QString txtPath = fileDlg.getExistingDirectory(		
		this, "Select txt stored path", trainSetRootPath);//��ȡtrain set ����Ϣ�ļ�txt����·��

	ui.labelFRGenTxtPath->setText(txtPath + "/trainSetAnnotation.txt"); //��ѵ������Ϣtxt�ļ�·����ʾ

	std::fstream file; 
	file.open((txtPath.toStdString()+"/"+"trainSetAnnotation.txt"), 
		std::ios::in|std::ios::out|std::ios::trunc);		//��txt�ļ�,û���򴴽�
	for (int i = 2; i < fileNameList.size(); i++)		//��2��ʼ���ų���һ�еĵڶ��е�.��.. 
	{
		ui.plainTextEditTrainSetFilesNames->appendPlainText(trainSetRootPath+"/"+fileNameList[i]); 
		//���ո�ʽ��txt�ļ�����������
		file <<(trainSetRootPath.toStdString() + "/" + fileNameList[i].toStdString()+"\n");
		((i + 1) == fileNameList.size()) ? (file << "1228") : (file << "1228\n");//��ȥ�ı����һ���ǿ���
	}
	file.close();//�ر��ļ�
}

//ѡ��annotation�ļ�

//����: 
		//ѡ���Ѿ����ڵ�txt�ļ�,	
		//�������ʾ��ui��
//�������:
		//ѡ���txt����·��
//�������:
		//
void MainWindow::on_pushButtonFRSelectTxt_clicked()
{
	QFileDialog fileDlg;
	QString txtFileAbsPath = fileDlg.getOpenFileName(
		nullptr, "Select a txt file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	ui.labelFRSelectTxtPath->setText(txtFileAbsPath);
}


//����������ʶ�� ��������

//����:
		//ͨ�����ɵ�XML�ļ�����ģ��
		//����FaceRec::begainToRec()����,ʵ�������������ʶ��
//��������:
		//xml�ļ�·��, ui��ǩ��ַ
//״̬��־:
		//_recState,_cameraState,_flagGenXML
//�������:
		//�����������+ֻ��Ƶ+ѡ��XML���+����XML��� ����
		//ͨ��STOP����
void MainWindow::on_pushButtonFRRunRecByGen_clicked()
{

	if (_flagGenXML)
	{	//״̬������
		_nowProcess->setText(" Now: FaceRecByGenXML ");
		//����
		ui.pushButtonFRRunRecByGen->setDisabled(false);
		ui.pushButtonFRRunRecBySelecte->setDisabled(true);
		ui.pushButtonFRCommonFaceRec->setDisabled(true);
		ui.pushButtonFROnlyCamera->setDisabled(true);

		QString filePath = ui.labelFRGenXMLPath->text();
		//����ʹ�ܱ�־
		_faceRec->_recState = true;//�޸�״̬����,����ʹ��
		_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
		//���ر�Ҫ�ļ�
		_faceRec->_modelXmlAbsPath = filePath;
		_faceRec->_uiLabel = ui.labelShowFace;
		//_faceRec->begainToRec(filePath, ui.labelShowFace);//����xml�ļ�,���߳�
		emit _FRStartRec();
	}
	else
	{
		QMessageBox::warning(this, "No XML File",
			"Please Give XML File At First! ", QMessageBox::Ok);
	}

}


//����������ʶ�� ��������

//����:
		//ͨ��ѡ���XML�ļ�����ģ��
		//����FaceRec::begainToRec()����,ʵ�������������ʶ��
//��������:
		//xml�ļ�·��, ui��ǩ��ַ
//״̬��־:
		//_recState,_cameraState,_flagSelecteXML
//�������:
		//�����������+ֻ��Ƶ+ѡ��XML���+����XML��� ����
		//ͨ��STOP����
void MainWindow::on_pushButtonFRRunRecBySelecte_clicked()
{


	if (_flagSelecteXML)
	{	//״̬������
		_nowProcess->setText(" Now: FaceRecBySelectedXML ");
		//����
		ui.pushButtonFRRunRecByGen->setDisabled(true);
		ui.pushButtonFRRunRecBySelecte->setDisabled(false);
		ui.pushButtonFRCommonFaceRec->setDisabled(true);
		ui.pushButtonFROnlyCamera->setDisabled(true);
		QString filePath = ui.labelFRSelectXMLPath->text();
		_faceRec->_recState = true;//�޸�״̬����,����ʹ��
		_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
		//���ر�Ҫ�ļ�
		_faceRec->_modelXmlAbsPath = filePath;
		_faceRec->_uiLabel = ui.labelShowFace;
		emit _FRStartRec();
		//_faceRec->begainToRec(filePath, ui.labelShowFace);//����xml�ļ�,���߳�
	}
	else
	{
		QMessageBox::warning(this, "No XML File",
			"Please Give XML File At First! ", QMessageBox::Ok);
	}

}


//����������� ��������

//����:
		//ͨ��opencv�ṩ��haar����xml�ļ���ʵ����ͨ�����ļ��
//��������:
		//ui��ǩ��ַ
//״̬��־:
		//_recState,_cameraState
//�������:
		//�����������+ֻ��Ƶ+ѡ��XML���+����XML��� ����
		//ͨ��STOP����
void MainWindow::on_pushButtonFRCommonFaceRec_clicked()
{
	//״̬������
	_nowProcess->setText(" Now: CommonFaceRec ");
	//����
	ui.pushButtonFRRunRecByGen->setDisabled(true);
	ui.pushButtonFRRunRecBySelecte->setDisabled(true);
	ui.pushButtonFRCommonFaceRec->setDisabled(false);
	ui.pushButtonFROnlyCamera->setDisabled(true);

	_faceRec->_recState = true;//�޸�״̬����,����ʹ��
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_uiLabelCom = ui.labelShowFace;
	emit _FRStartCommonRec();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//���߳�
}


//��Ƶ����� ��������

//����:
		//��Ƶ�����
//��������:
		//ui��ǩ��ַ
//״̬��־:
		//_cameraState
//�������:
		//�����������+ֻ��Ƶ+ѡ��XML���+����XML��� ����
		//ͨ��STOP����
void MainWindow::on_pushButtonFROnlyCamera_clicked()
{
	//����״̬��
	_nowProcess->setText(" Now: OnlyVideo ");
	//����
	ui.pushButtonFRRunRecByGen->setDisabled(true);
	ui.pushButtonFRRunRecBySelecte->setDisabled(true);
	ui.pushButtonFRCommonFaceRec->setDisabled(true);
	ui.pushButtonFROnlyCamera->setDisabled(false);


	//_faceRec->_recState = true;//ֻ��ʾ�����,����ʹ��
	_faceRec->_recState = false;
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_uiLabelCom = ui.labelShowFace;
	qDebug() << "Mainwindow thread: " << QThread::currentThread() << endl;
	emit _FROnlyShow();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//���߳�
}


//ֹͣ����

//����:
		//�����������, �ͷű�Ҫ����
//��������:
		//
//״̬��־:
		//_recState,_cameraState, _camera
//ע���:
		//�޷�����ѵ��
void MainWindow::on_pushButtonFRStop_clicked()
{
	//�������
	ui.pushButtonFRRunRecByGen->setDisabled(false);
	ui.pushButtonFRRunRecBySelecte->setDisabled(false);
	ui.pushButtonFROnlyCamera->setDisabled(false);
	ui.pushButtonFRCommonFaceRec->setDisabled(false);

	//����ʹ�ܿ���, ����ֹ�߳�
	_faceRec->_cameraState = false;//�ر�֡ѭ��
	_faceRec->_recState = false;//�رռ��
	//�ͷ�����ͷ
	_faceRec->_camera.release();//�ͷ�����ͷ
	//���label
	ui.labelShowFace->clear();
	//����״̬��
	_nowProcess->setText(" Now: nothing ");
}


