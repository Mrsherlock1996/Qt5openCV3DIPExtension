#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	this->resize(1080, 720);
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
	//ImageMatch����
	iniImgMatch();
	//���п�ʶ�𲿷�

	//CNN����

}
//ʱ����ʾ
void MainWindow::showCurrentTime()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString timestr = currentTime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ�ĸ�ʽ
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
	connect(this, &MainWindow::_FRStartCommonRecByLBPH, _faceRec, &FaceRec::begainToCommonFaceRecLBPH, Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartRec, _faceRec, &FaceRec::begainToRec, Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartTrain, _faceTrain, &FaceRec::startTrain,Qt::QueuedConnection);
	connect(this, &MainWindow::_FROnlyShow, _faceRec, &FaceRec::onlyRealTimeShow, Qt::QueuedConnection);

}


//ѵ��ģ��, ����XML�ļ�����

//����: 
		//ѵ��model,����FaceRecognizer�����ģ��״̬xml�ļ�
		//��ѵ�������uiչʾ
//�������:
		//_faceTrain : FaceRec ʹ�õ��ǵ�����FaceRec����
		//���е�txt�ļ�·��,  ����xml����·��, 
		//ui״̬����ǩ_backGroundProcess
//���±�־:
		//_flagGenXML
//ע���:
//�������:
		//
void MainWindow::on_pushButtonFRGenXML_clicked()
{
	_flagGenXML = true;
	//����txt��xml·��
	//��ȡtxt��xml���·��
	QFileDialog fileDlg;
	QString txtPath = fileDlg.getOpenFileName(
		nullptr, "select a trainSet annotation file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	//ע�������xmlPath�����ļ�·��"a/b/aa.xml"�����ļ���·��"a/b"
	QString xmlPath = fileDlg.getExistingDirectory(
		nullptr, "Select a path to save .XML of your model", QDir::currentPath());

	//��ֹ���ݿ�ָ��,�����ж�
	if (!txtPath.isEmpty())
	{	
		ui.labelFRGenTxtPath->setText(txtPath);
		if (!xmlPath.isEmpty())
		{
			ui.labelFRGenXMLPath->setText(xmlPath);
			//����״̬��labelָ��
			_faceTrain->_uiStatusBarLabel = _backGroundProcess;
			//����FaceRec��ʼѵ������
			_faceTrain->_trainState = true;
			
			_faceTrain->_trainSetTxtFilePathQStr = ui.labelFRGenTxtPath->text();
			_faceTrain->_resultXMLFilePath = ui.labelFRGenXMLPath->text();
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
		else
		{
			QMessageBox::warning(this, "WARNING", "NOTHING SELECTED", QMessageBox::Ok);

		}
	}
	else
	{
		QMessageBox::warning(this, "WARNING", "NOTHING SELECTED", QMessageBox::Ok);
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
		ui.pushButtonFRComFaceRecLBPH->setDisabled(true);

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
		ui.pushButtonFRComFaceRecLBPH->setDisabled(true);

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
//Haar+Cascade
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
	ui.pushButtonFRComFaceRecLBPH->setDisabled(true);

	_faceRec->_recState = true;//�޸�״̬����,����ʹ��
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_uiLabelCom = ui.labelShowFace;
	emit _FRStartCommonRec();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//���߳�
}

//�����������LBPH
void MainWindow::on_pushButtonFRComFaceRecLBPH_clicked()
{
	//״̬������
	_nowProcess->setText(" Now: CommonFaceRecByLBPH ");
	//����
	ui.pushButtonFRRunRecByGen->setDisabled(true);
	ui.pushButtonFRRunRecBySelecte->setDisabled(true);
	ui.pushButtonFRCommonFaceRec->setDisabled(true);
	ui.pushButtonFROnlyCamera->setDisabled(true);
	ui.pushButtonFRComFaceRecLBPH->setDisabled(false);

	_faceRec->_recState = true;//�޸�״̬����,����ʹ��
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_uiLabelCom = ui.labelShowFace;
	emit _FRStartCommonRecByLBPH();
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
	ui.pushButtonFRComFaceRecLBPH->setDisabled(true);


	//_faceRec->_recState = true;//ֻ��ʾ�����,����ʹ��
	_faceRec->_recState = false;
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_uiLabelCom = ui.labelShowFace;
	//qDebug() << "Mainwindow thread: " << QThread::currentThread() << endl;
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
	ui.pushButtonFRComFaceRecLBPH->setDisabled(false);

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




//ImageMatch����

void MainWindow::iniImgMatch()
{
	_imgMatch = new ImageMatch;		//���������
	
	//�����ʼ��
	ui.pushButtonIMOriginNew->setDisabled(true);//���ͼƬ�����
	ui.pushButtonIMOriginOld->setDisabled(true);
	ui.labelShowIMNewImg->clear();
	ui.labelShowIMOldImg->clear();
	ui.labelShowIMResult->clear();
	ui.labelIMNewImgPath->clear();
	ui.labelIMOldImgPath->clear();
	ui.labelIMDpmXmlPath->clear();
	ui.labelIMDpmClassName->clear();

	ui.labelShowIMResult->setText("Please add A image to process!");
	ui.labelShowIMResult->setAlignment(Qt::AlignCenter);//����
	//�߳�
	_threadIM = new QThread;
	_imgMatch->moveToThread(_threadIM);
	connect(this, &MainWindow::_IMStartDPM, _imgMatch, &ImageMatch::imgMatchDpm);
	_threadIM->start();
}

//��ʾoldͼƬ
//����: ѡ��ͼƬ,������������ʾ
//flag: ��
void MainWindow::on_pushButtonIMAddOldImg_clicked()
{
	//��ȡ�ļ�·��
	QFileDialog fileDlg;
	QString imgPath = fileDlg.getOpenFileName(this, "Select A original image",
		QDir::currentPath(), "jpg(*.jpg);;png(*.png);;all(*.*)");
	//��ʾͼƬ·��
	if (!imgPath.isEmpty()) //�����ѡ
	{
		ui.labelIMOldImgPath->setText(imgPath);//��ʾ·��
		QImage oldImg(imgPath);
		QSize oldImgSize = ui.labelShowIMOldImg->size();
		oldImg = oldImg.scaled(oldImgSize,Qt::KeepAspectRatio); //������С
		ui.labelShowIMOldImg->setPixmap(QPixmap::fromImage(oldImg));	//��ʾ
		ui.labelShowIMOldImg->setAlignment(Qt::AlignCenter);//����
		ui.pushButtonIMOriginOld->setDisabled(false);//�������
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"You select nothing", QMessageBox::Ok);
	}
}


//��ʾnewͼƬ
//����: ѡ��ͼƬ,������������ʾ
//flag: ��
void MainWindow::on_pushButtonIMAddNewImg_clicked()
{
	//��ȡ�ļ�·��
	QFileDialog fileDlg;
	QString imgPath = fileDlg.getOpenFileName(this, "Select A original image",
		QDir::currentPath(), "jpg(*.jpg);;png(*.png);;all(*.*)");
	//��ʾͼƬ·��
	if (!imgPath.isEmpty()) //�����ѡ
	{
		ui.labelIMNewImgPath->setText(imgPath);//��ʾ·��
		QImage newImg(imgPath);
		QSize newImgSize = ui.labelShowIMNewImg->size();
		newImg = newImg.scaled(newImgSize, Qt::KeepAspectRatio); //������С
		ui.labelShowIMNewImg->setPixmap(QPixmap::fromImage(newImg));	//��ʾ
		ui.labelShowIMNewImg->setAlignment(Qt::AlignCenter);//����
		ui.pushButtonIMOriginNew->setDisabled(false);//�������
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"You select nothing", QMessageBox::Ok);
	}
}


//��ʾѵ��ԭͼ
void MainWindow::on_pushButtonIMOriginOld_clicked()
{
	//��ȡ�ļ�·��
	QString imgPath = ui.labelIMOldImgPath->text();
	//��ʾͼƬ·��
	if (!imgPath.isEmpty()) //�����ѡ
	{
		QImage oldImg(imgPath);
		QSize oldImgSize = ui.labelShowIMResult->size();
		oldImg = oldImg.scaled(oldImgSize, Qt::KeepAspectRatio); //������С
		ui.labelShowIMResult->setPixmap(QPixmap::fromImage(oldImg));	//��ʾ
		ui.labelShowIMResult->setAlignment(Qt::AlignCenter);//����
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Add A Old/Train Image At First ", QMessageBox::Ok);
	}
}


//��ʾ���ԭͼ
void MainWindow::on_pushButtonIMOriginNew_clicked()
{
	//��ȡ�ļ�·��
	QString imgPath = ui.labelIMNewImgPath->text();
	//��ʾͼƬ·��
	if (!imgPath.isEmpty()) //�����ѡ
	{
		QImage newImg(imgPath);
		QSize newImgSize = ui.labelShowIMResult->size();
		newImg = newImg.scaled(newImgSize, Qt::KeepAspectRatio); //������С
		ui.labelShowIMResult->setPixmap(QPixmap::fromImage(newImg));	//��ʾ
		ui.labelShowIMResult->setAlignment(Qt::AlignCenter);//����
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Add A New/Test Image At First ", QMessageBox::Ok);
	}
}


//���ȫ��
//����:���ȫ��������������
void MainWindow::on_pushButtonIMClearAll_clicked()
{
	ui.labelShowIMNewImg->clear();
	ui.labelShowIMOldImg->clear();
	ui.labelShowIMResult->clear();
	ui.labelIMNewImgPath->clear();
	ui.labelIMOldImgPath->clear();
	ui.labelIMDpmXmlPath->clear();
	ui.labelIMDpmClassName->clear();

	ui.pushButtonIMOriginNew->setDisabled(true);
	ui.pushButtonIMOriginOld->setDisabled(true);

	ui.labelShowIMResult->setText("Please add A image to process!");
	ui.labelShowIMResult->setAlignment(Qt::AlignCenter);//����

}


//Haaris�ǵ���
//����:	ʵ�ֵ�ͼ�Ľǵ���(ֻ���)
//����:
		//_uiLabelShowImg,
//��־:
		//_flagHaar
void MainWindow::on_pushButtonIMHaaris_clicked()
{
	//�Ȼ�ȡͼƬ·��,��ֹ��ͼ
	QString imgPath = ui.labelIMNewImgPath->text();
	if (!imgPath.isEmpty())
	{
		_imgMatch->_uiLabelShowImg = ui.labelShowIMResult;
		_imgMatch->_flagHaar = true;
		_imgMatch->_matchImgPath = imgPath;
		//emit _IMStartHaar;	//��������
		_imgMatch->imgMatchHaar();
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Add A New/Test Image At First ", QMessageBox::Ok);
	}

}


//Shi-Tomasi�ǵ���
//����:	ʵ�ֵ�ͼ�Ľǵ���(ֻ���)
//����:
		//_uiLabelShowImg,
//��־:
		//_flagShitomasi
void MainWindow::on_pushButtonIMShitomasi_clicked()
{
	//�Ȼ�ȡͼƬ·��,��ֹ��ͼ
	QString imgPath = ui.labelIMNewImgPath->text();
	if (!imgPath.isEmpty())
	{
		_imgMatch->_uiLabelShowImg = ui.labelShowIMResult;
		_imgMatch->_flagShitomasi = true;
		_imgMatch->_matchImgPath = imgPath;
		//emit _IMStartHaar;	//��������
		_imgMatch->imgMatchShitomasi();
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Add A New/Test Image At First ", QMessageBox::Ok);
	}

}



//SIFT�������
//����:	ʵ������ͼ���������,��ƥ�����������Ϣ
//����:
		//_uiLabelShowImg,
//��־:
		//_flagSift
void MainWindow::on_pushButtonIMSift_clicked()
{
	QString trainPath = ui.labelIMOldImgPath->text();
	QString testPath = ui.labelIMNewImgPath->text();
	if ((!trainPath.isEmpty()) && (!testPath.isEmpty()))
	{
		_imgMatch->_uiLabelShowImg = ui.labelShowIMResult;
		_imgMatch->_flagSift = true;
		_imgMatch->_trainImgPath = trainPath;
		_imgMatch->_matchImgPath = testPath;
		//emit IMStartSIFT;
		_imgMatch->imgMatchSift();
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Add Train/Test Images At First ", QMessageBox::Ok);
	}

}

//SURF�������
//����:	ʵ������ͼ���������,��ƥ�����������Ϣ
//����:
		//_uiLabelShowImg,
//��־:
		//_flagSurf
void MainWindow::on_pushButtonIMSurf_clicked()
{
	QString trainPath = ui.labelIMOldImgPath->text();
	QString testPath = ui.labelIMNewImgPath->text();
	if ((!trainPath.isEmpty()) && (!testPath.isEmpty()))
	{
		_imgMatch->_uiLabelShowImg = ui.labelShowIMResult;
		_imgMatch->_flagSurf = true;
		_imgMatch->_trainImgPath = trainPath;
		_imgMatch->_matchImgPath = testPath;
		//emit IMStartSIFT;
		_imgMatch->imgMatchSurf();
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Add Train/Test Images At First ", QMessageBox::Ok);
	}

}


//HOG+SVM���
//����: ���ż��,(��ʱû�����Ƶ֡���)
//ע��: Ӧ����Ӽ��ͼƬ
void MainWindow::on_pushButtonIMHog_clicked()
{
	QString imgPath = ui.labelIMNewImgPath->text();
	if (!imgPath.isEmpty())
	{
		_imgMatch->_uiLabelShowImg = ui.labelShowIMResult;
		_imgMatch->_flagHog = true;
		_imgMatch->_matchImgPath = imgPath; 
		//emit _IMStartHOG;	//��������
		_imgMatch->imgMatchHog();
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Add A New/Test Image At First ", QMessageBox::Ok);
	}
}


//ѡ��XML
void MainWindow::on_pushButtonIMDpmXml_clicked()
{
	QFileDialog fileDlg;
	QString xmlPath = fileDlg.getOpenFileName(nullptr, "Select A XML",
		QDir::currentPath(), "xml(*.xml);;all(*.*)");
	ui.labelIMDpmXmlPath->setText(xmlPath);
}


//��������
void MainWindow::on_pushButtonIMDpmClassName_clicked()
{
	QInputDialog inputDlg;
	QString name = inputDlg.getText(this, "Input Dialog", "Input Class Name by English",
		QLineEdit::Normal, "people");
	ui.labelIMDpmClassName->setText(name);
}


//DPM������
//ʹ�����߳�
void MainWindow::on_pushButtonIMDpmStart_clicked()
{
	QString imgPath = ui.labelIMNewImgPath->text();
	QString name = ui.labelIMDpmClassName->text();
	QString xmlPath = ui.labelIMDpmXmlPath->text();

	if (!xmlPath.isEmpty())
	{
		if (!name.isEmpty())
		{
			if (!imgPath.isEmpty())
			{
				_imgMatch->_uiLabelShowImg = ui.labelShowIMResult; //����ui��ǩ
				_imgMatch->_flagDpm = true;
				_imgMatch->_matchImgPath = imgPath; //���ݼ��ͼƬ·��
				_imgMatch->_uiLabelStatus = _backGroundProcess; //����״̬��
				_imgMatch->_fileNames.push_back(xmlPath.toStdString());
				_imgMatch->_classNames.push_back(name.toStdString());
				emit _IMStartDPM();	//��������
				ui.labelShowIMResult->setText("DPM is runing, please wait...");
				ui.labelShowIMResult->setAlignment(Qt::AlignCenter);
				//_imgMatch->imgMatchDpm();
			}
			else
			{
				QMessageBox::warning(nullptr, "NO IMAGE PATH",
					"Add A New/Test Image At First !", QMessageBox::Ok);
			}
		}
		else
		{
			QMessageBox::warning(nullptr, "NO IMAGE PATH",
				"Please Input Class Name !!", QMessageBox::Ok);
		}
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Need Load XML File At First!!!", QMessageBox::Ok);
	}


}


//ģ��ƥ��
//����:	����ģ��, ������ͼƬ�е�λ��
//����:
		//_uiLabelShowImg,
//��־:
		//_flagTemplate
void MainWindow::on_pushButtonIMTemplate_clicked()
{
	QString trainPath = ui.labelIMOldImgPath->text();
	QString testPath = ui.labelIMNewImgPath->text();
	if ((!trainPath.isEmpty()) && (!testPath.isEmpty()))
	{
		_imgMatch->_uiLabelShowImg = ui.labelShowIMResult;
		_imgMatch->_flagTemplate = true;
		_imgMatch->_trainImgPath = trainPath;
		_imgMatch->_matchImgPath = testPath;

		_imgMatch->imgTemplateMatch();
	}
	else
	{
		QMessageBox::warning(nullptr, "NO IMAGE PATH",
			"Add Train/Test Images At First ", QMessageBox::Ok);
	}

}

