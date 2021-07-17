#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	iniFaceRec();


}
void MainWindow::iniFaceRec()
{
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
	//�������̺߳�����෽��
	connect(this, &MainWindow::_FRStartCommonRec, _faceRec, &FaceRec::begainToCommonFaceRec,Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartRec, _faceRec, &FaceRec::begainToRec, Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartTrain, _faceTrain, &FaceRec::startTrain,Qt::QueuedConnection);
	connect(this, &MainWindow::_FROnlyShow, _faceRec, &FaceRec::onlyRealTimeShow, Qt::QueuedConnection);
}



//ѵ��model,����FaceRecognizer�����ģ��״̬xml�ļ�
//����FaceRec::startTrain����,��Ҫ�������е�txt�ļ�·��������xml����·��
//�������uiչʾ
void MainWindow::on_pushButtonFRGenXML_clicked()
{
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

	//�����ʾ
	if (_faceTrain->_trainResult)
	{
		QMessageBox::information(
			this, "Success", ("Your .XML has been saved into :\n" + xmlPath), QMessageBox::Ok);//�����Ի���
		ui.labelFRGenXMLPath->setText(xmlPath);//����ui����
		ui.pushButtonFRRunRecByGen->setDisabled(false);//�ɹ�����xml�����ⰴť��
	}
	else
	{
		QMessageBox::critical(
			this, "ERROR", "NO XML GENERATED!!!", QMessageBox::Ok);
		ui.labelFRGenXMLPath->setText("NO XML GENERATED!");//����ui����
		ui.pushButtonFRRunRecByGen->setDisabled(true);  //û�м���XMLʱ����ť�ر�,����Ϊ�˶������xmlʧ�ܵ����
	}
}

//ѡ�����е�FaceRecognizer�����ģ��״̬xml�ļ�
void MainWindow::on_pushButtonFRSelectXML_clicked()
{
	QFileDialog fileDlg;
	QString xmlPath = fileDlg.getOpenFileName(
		this,"Select a .xml",QDir::currentPath(),"xml(*.xml);;all(*.*)");
	ui.labelFRSelectXMLPath->setText(xmlPath);
	ui.pushButtonFRRunRecByGen->setDisabled(false);//�ɹ�����xml�����ⰴť��
}

//����ѵ������annotation�ļ�
//����ѵ������ȡͼƬ����·����txt�ļ�
void MainWindow::on_pushButtonFRGenTxt_clicked()
{
	//TrainSetImage
	QFileDialog fileDlg; //�����ļ��Ի����ȡѵ����·��
	QString trainSetRootPath = fileDlg.getExistingDirectory(
		nullptr, "Select train set root path", QDir::currentPath());
	QDir path;
	path.setCurrent(trainSetRootPath);
	QStringList fileNameList = path.entryList(); //����ֻ��ѵ��ͼƬ�ļ�����,��·����Ϣ
	ui.labelFRTrainSetRootPath->setText(trainSetRootPath);//��ѵ�������ڸ�Ŀ¼��ʾ

	//TrainSetImageAnnotation
	QString txtPath = fileDlg.getExistingDirectory(		
		nullptr, "Select txt stored path", trainSetRootPath);//��ȡtrain set ����Ϣ�ļ�txt����·��

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

//ѡ�����е�ѵ����annotation�ļ�
void MainWindow::on_pushButtonFRSelectTxt_clicked()
{
	QFileDialog fileDlg;
	QString txtFileAbsPath = fileDlg.getOpenFileName(
		nullptr, "Select a txt file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	ui.labelFRSelectTxtPath->setText(txtFileAbsPath);
}


//��ʼ���(ͨ��ѡ���XML)
//ʹ��FaceRec::begainToRec()
//����xml�ļ���ui��ǩ��ַ
void MainWindow::on_pushButtonFRRunRecByGen_clicked()
{
	QString filePath = ui.labelFRGenXMLPath->text();
	_faceRec->_recState = true;//�޸�״̬����,����ʹ��
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_modelXmlAbsPath = filePath;
	_faceRec->_uiLabel = ui.labelShowFace;
	//_faceRec->begainToRec(filePath, ui.labelShowFace);//����xml�ļ�,���߳�
	emit _FRStartRec();
}

//��ʼ���(ͨ��ѡ���XML)
void MainWindow::on_pushButtonFRRunRecBySelecte_clicked()
{
	QString filePath = ui.labelFRSelectXMLPath->text();
	_faceRec->_recState = true;//�޸�״̬����,����ʹ��
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_modelXmlAbsPath = filePath;
	_faceRec->_uiLabel = ui.labelShowFace;
	emit _FRStartRec();
	//_faceRec->begainToRec(filePath, ui.labelShowFace);//����xml�ļ�,���߳�
}

void MainWindow::on_pushButtonFRStop_clicked()
{
	_faceRec->_cameraState = false;//�ر�֡ѭ��
	_faceRec->_recState = false;//�رռ��
	_faceRec->_camera.release();//�ͷ�����ͷ
}

void MainWindow::on_pushButtonFRCommonFaceRec_clicked()
{
	_faceRec->_recState = true;//�޸�״̬����,����ʹ��
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_uiLabelCom= ui.labelShowFace;
	emit _FRStartCommonRec();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//���߳�
}

void MainWindow::on_pushButtonFROnlyCamera_clicked()
{
	//_faceRec->_recState = true;//ֻ��ʾ�����,����ʹ��
	_faceRec->_recState = false;
	_faceRec->_cameraState = true; //ʹ������ͷ,ʹ֮���Խ���֡ѭ��
	//���ر�Ҫ�ļ�
	_faceRec->_uiLabelCom = ui.labelShowFace; 
	qDebug() << "Mainwindow thread: " << QThread::currentThread() << endl;
	emit _FROnlyShow();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//���߳�
}
