#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	iniFaceRec();


}
void MainWindow::iniFaceRec()
{
	_faceRec = new FaceRec; //初始化一个人脸检测器对象
	_faceTrain = new FaceRec;//初始化一个专门用于训练的FaceRec
	ui.pushButtonFRRunRecByGen->setDisabled(true);  //没有加载XML时将按钮关闭
	ui.pushButtonFRRunRecBySelecte->setDisabled(true);
	//创建子线程
	_threadFR = new QThread;
	_threadFRTrain = new QThread;
	//开启子线程
	_threadFR->start();
	_threadFRTrain->start();
	//移动到子线程
	_faceRec->moveToThread(_threadFR);
	_faceTrain->moveToThread(_threadFRTrain);
	//关联子线程和相关类方法
	connect(this, &MainWindow::_FRStartCommonRec, _faceRec, &FaceRec::begainToCommonFaceRec,Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartRec, _faceRec, &FaceRec::begainToRec, Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartTrain, _faceTrain, &FaceRec::startTrain,Qt::QueuedConnection);
	connect(this, &MainWindow::_FROnlyShow, _faceRec, &FaceRec::onlyRealTimeShow, Qt::QueuedConnection);
}



//训练model,生成FaceRecognizer检测器模型状态xml文件
//借助FaceRec::startTrain对象,需要给出已有的txt文件路径和生成xml保存路径
//将结果在ui展示
void MainWindow::on_pushButtonFRGenXML_clicked()
{
	//获取txt和xml相关路径
	QFileDialog fileDlg;
	QString txtPath = fileDlg.getOpenFileName(
		nullptr, "select a trainSet annotation file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	//注意这里的xmlPath不是文件路径"a/b/aa.xml"而是文件夹路径"a/b"
	QString xmlPath = fileDlg.getExistingDirectory(
		nullptr, "Select a path to save .XML of your model", QDir::currentPath());

	//调用FaceRec::startTrain训练检测模型
	//_faceTrain->startTrain(txtPath, xmlPath);//非多线程
	emit _FRStartTrain();//多线程

	//结果提示
	if (_faceTrain->_trainResult)
	{
		QMessageBox::information(
			this, "Success", ("Your .XML has been saved into :\n" + xmlPath), QMessageBox::Ok);//弹出对话框
		ui.labelFRGenXMLPath->setText(xmlPath);//更新ui界面
		ui.pushButtonFRRunRecByGen->setDisabled(false);//成功生成xml将其检测按钮打开
	}
	else
	{
		QMessageBox::critical(
			this, "ERROR", "NO XML GENERATED!!!", QMessageBox::Ok);
		ui.labelFRGenXMLPath->setText("NO XML GENERATED!");//更新ui界面
		ui.pushButtonFRRunRecByGen->setDisabled(true);  //没有加载XML时将按钮关闭,这是为了多次生成xml失败的情况
	}
}

//选择已有的FaceRecognizer检测器模型状态xml文件
void MainWindow::on_pushButtonFRSelectXML_clicked()
{
	QFileDialog fileDlg;
	QString xmlPath = fileDlg.getOpenFileName(
		this,"Select a .xml",QDir::currentPath(),"xml(*.xml);;all(*.*)");
	ui.labelFRSelectXMLPath->setText(xmlPath);
	ui.pushButtonFRRunRecByGen->setDisabled(false);//成功生成xml将其检测按钮打开
}

//生成训练集的annotation文件
//遍历训练集读取图片绝对路径到txt文件
void MainWindow::on_pushButtonFRGenTxt_clicked()
{
	//TrainSetImage
	QFileDialog fileDlg; //创建文件对话框获取训练集路径
	QString trainSetRootPath = fileDlg.getExistingDirectory(
		nullptr, "Select train set root path", QDir::currentPath());
	QDir path;
	path.setCurrent(trainSetRootPath);
	QStringList fileNameList = path.entryList(); //这里只是训练图片文件名字,无路径信息
	ui.labelFRTrainSetRootPath->setText(trainSetRootPath);//将训练集所在根目录显示

	//TrainSetImageAnnotation
	QString txtPath = fileDlg.getExistingDirectory(		
		nullptr, "Select txt stored path", trainSetRootPath);//获取train set 的信息文件txt保存路径

	ui.labelFRGenTxtPath->setText(txtPath + "/trainSetAnnotation.txt"); //将训练集信息txt文件路径显示

	std::fstream file; 
	file.open((txtPath.toStdString()+"/"+"trainSetAnnotation.txt"), 
		std::ios::in|std::ios::out|std::ios::trunc);		//打开txt文件,没有则创建
	for (int i = 2; i < fileNameList.size(); i++)		//从2开始是排出第一行的第二行的.和.. 
	{
		ui.plainTextEditTrainSetFilesNames->appendPlainText(trainSetRootPath+"/"+fileNameList[i]); 
		//按照格式向txt文件中输入内容
		file <<(trainSetRootPath.toStdString() + "/" + fileNameList[i].toStdString()+"\n");
		((i + 1) == fileNameList.size()) ? (file << "1228") : (file << "1228\n");//排去文本最后一行是空行
	}
	file.close();//关闭文件
}

//选择已有的训练集annotation文件
void MainWindow::on_pushButtonFRSelectTxt_clicked()
{
	QFileDialog fileDlg;
	QString txtFileAbsPath = fileDlg.getOpenFileName(
		nullptr, "Select a txt file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	ui.labelFRSelectTxtPath->setText(txtFileAbsPath);
}


//开始检测(通过选择的XML)
//使用FaceRec::begainToRec()
//传递xml文件和ui标签地址
void MainWindow::on_pushButtonFRRunRecByGen_clicked()
{
	QString filePath = ui.labelFRGenXMLPath->text();
	_faceRec->_recState = true;//修改状态变量,将其使能
	_faceRec->_cameraState = true; //使能摄像头,使之可以进入帧循环
	//加载必要文件
	_faceRec->_modelXmlAbsPath = filePath;
	_faceRec->_uiLabel = ui.labelShowFace;
	//_faceRec->begainToRec(filePath, ui.labelShowFace);//加载xml文件,单线程
	emit _FRStartRec();
}

//开始检测(通过选择的XML)
void MainWindow::on_pushButtonFRRunRecBySelecte_clicked()
{
	QString filePath = ui.labelFRSelectXMLPath->text();
	_faceRec->_recState = true;//修改状态变量,将其使能
	_faceRec->_cameraState = true; //使能摄像头,使之可以进入帧循环
	//加载必要文件
	_faceRec->_modelXmlAbsPath = filePath;
	_faceRec->_uiLabel = ui.labelShowFace;
	emit _FRStartRec();
	//_faceRec->begainToRec(filePath, ui.labelShowFace);//加载xml文件,单线程
}

void MainWindow::on_pushButtonFRStop_clicked()
{
	_faceRec->_cameraState = false;//关闭帧循环
	_faceRec->_recState = false;//关闭检测
	_faceRec->_camera.release();//释放摄像头
}

void MainWindow::on_pushButtonFRCommonFaceRec_clicked()
{
	_faceRec->_recState = true;//修改状态变量,将其使能
	_faceRec->_cameraState = true; //使能摄像头,使之可以进入帧循环
	//加载必要文件
	_faceRec->_uiLabelCom= ui.labelShowFace;
	emit _FRStartCommonRec();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//单线程
}

void MainWindow::on_pushButtonFROnlyCamera_clicked()
{
	//_faceRec->_recState = true;//只显示不检测,无需使能
	_faceRec->_recState = false;
	_faceRec->_cameraState = true; //使能摄像头,使之可以进入帧循环
	//加载必要文件
	_faceRec->_uiLabelCom = ui.labelShowFace; 
	qDebug() << "Mainwindow thread: " << QThread::currentThread() << endl;
	emit _FROnlyShow();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//单线程
}
