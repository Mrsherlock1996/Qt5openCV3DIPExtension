#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	//时间显示部分
	 _timeLabel = new QLabel; //显示时间的标签
	_timer = new QTimer;
	ui.statusBar->addPermanentWidget(_timeLabel);//状态栏添加label
	_timer->start(1000);
	connect(_timer, &QTimer::timeout, this, &MainWindow::showCurrentTime);

	//状态栏初始化
	_welcom = new QLabel; 
	_backGroundProcess = new QLabel; 
	_nowProcess = new QLabel;
	_welcom->setText("WELCOM");
	_backGroundProcess->setText(" BG: nothing ");
	_nowProcess->setText(" Now: nothing ");
	ui.statusBar->addWidget(_welcom);
	ui.statusBar->addWidget(_nowProcess);
	ui.statusBar->addWidget(_backGroundProcess);

	//FaceRec部分
	iniFaceRec();
	//目标检测部分

	//银行卡识别部分

	//CNN部分

}
//时间显示
void MainWindow::showCurrentTime()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString timestr = currentTime.toString("yyyy-MM-dd- hh:mm:ss"); //设置显示的格式
	_timeLabel->setText(timestr);
}

//FaceRec部分

//FaceRec初始化
void MainWindow::iniFaceRec()
{
	//清空标签
	ui.labelFRGenTxtPath->clear();
	ui.labelFRGenXMLPath->clear();
	ui.labelFRSelectTxtPath->clear();
	ui.labelFRSelectXMLPath->clear();
	ui.labelShowFace->clear();
	ui.labelFRTrainSetRootPath->clear();
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
	//关联
	connect(this, &MainWindow::_FRStartCommonRec, _faceRec, &FaceRec::begainToCommonFaceRec,Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartRec, _faceRec, &FaceRec::begainToRec, Qt::QueuedConnection);
	connect(this, &MainWindow::_FRStartTrain, _faceTrain, &FaceRec::startTrain,Qt::QueuedConnection);
	connect(this, &MainWindow::_FROnlyShow, _faceRec, &FaceRec::onlyRealTimeShow, Qt::QueuedConnection);
}

//训练模型, 生成XML文件函数

//功能: 
		//训练model,生成FaceRecognizer检测器模型状态xml文件
		//将训练结果在ui展示
//所需参数:
		//已有的txt文件路径,  生成xml保存路径
//更新标志:
		//_flagGenXML
//注意点:
//互锁组件:
		//
void MainWindow::on_pushButtonFRGenXML_clicked()
{
	_flagGenXML = true;
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

	//结果
	if (_faceTrain->_trainResult)//训练完成后FaceRec::_trainResult为true
	{
		QMessageBox::information(
			this, "Success", ("Your .XML has been saved into :\n" + xmlPath), QMessageBox::Ok);//弹出对话框
		ui.labelFRGenXMLPath->setText(xmlPath);//将xml文件生成路径显示到ui中
	}
	else
	{
		ui.labelFRGenXMLPath->setText("TRAINING");//更新ui界面
	}
}


//选择XML文件函数

//功能: 
		//直接选择xml文件
		//将选择结果在ui展示
//所需参数:
		//选择的xml所在路径
//更新标志:
		//_flagSelecteXMl
//注意点:
//互锁组件:
		//
void MainWindow::on_pushButtonFRSelectXML_clicked()
{
	_flagSelecteXML = true;
	QFileDialog fileDlg;
	QString xmlPath = fileDlg.getOpenFileName(
		this,"Select a .xml",QDir::currentPath(),"xml(*.xml);;all(*.*)");
	ui.labelFRSelectXMLPath->setText(xmlPath);
}


//生成annotation文件函数

//功能: 
		//遍历训练集, 形成固定格式的txt文件,
		//将生成结果在ui展示
//所需参数:
		//已有的训练集文件根目录,  生成txt的保存路径
//注意点:
//互锁组件:
		//
void MainWindow::on_pushButtonFRGenTxt_clicked()
{
	//TrainSetImage
	QFileDialog fileDlg; //创建文件对话框获取训练集路径
	QString trainSetRootPath = fileDlg.getExistingDirectory(
		this, "Select train set root path", QDir::currentPath());
	QDir path;
	path.setCurrent(trainSetRootPath);
	QStringList fileNameList = path.entryList(); //这里只是训练图片文件名字,无路径信息
	ui.labelFRTrainSetRootPath->setText(trainSetRootPath);//将训练集所在根目录显示

	//TrainSetImageAnnotation
	QString txtPath = fileDlg.getExistingDirectory(		
		this, "Select txt stored path", trainSetRootPath);//获取train set 的信息文件txt保存路径

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

//选择annotation文件

//功能: 
		//选择已经存在的txt文件,	
		//将结果显示到ui中
//所需参数:
		//选择的txt所在路径
//互锁组件:
		//
void MainWindow::on_pushButtonFRSelectTxt_clicked()
{
	QFileDialog fileDlg;
	QString txtFileAbsPath = fileDlg.getOpenFileName(
		nullptr, "Select a txt file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	ui.labelFRSelectTxtPath->setText(txtFileAbsPath);
}


//人脸检测身份识别 启动函数

//功能:
		//通过生成的XML文件加载模型
		//调用FaceRec::begainToRec()方法,实现人脸检测和身份识别
//参数传递:
		//xml文件路径, ui标签地址
//状态标志:
		//_recState,_cameraState,_flagGenXML
//互锁组件:
		//大众人脸检测+只视频+选择XML检测+生成XML检测 互锁
		//通过STOP解锁
void MainWindow::on_pushButtonFRRunRecByGen_clicked()
{

	if (_flagGenXML)
	{	//状态栏更新
		_nowProcess->setText(" Now: FaceRecByGenXML ");
		//互锁
		ui.pushButtonFRRunRecByGen->setDisabled(false);
		ui.pushButtonFRRunRecBySelecte->setDisabled(true);
		ui.pushButtonFRCommonFaceRec->setDisabled(true);
		ui.pushButtonFROnlyCamera->setDisabled(true);

		QString filePath = ui.labelFRGenXMLPath->text();
		//更新使能标志
		_faceRec->_recState = true;//修改状态变量,将其使能
		_faceRec->_cameraState = true; //使能摄像头,使之可以进入帧循环
		//加载必要文件
		_faceRec->_modelXmlAbsPath = filePath;
		_faceRec->_uiLabel = ui.labelShowFace;
		//_faceRec->begainToRec(filePath, ui.labelShowFace);//加载xml文件,单线程
		emit _FRStartRec();
	}
	else
	{
		QMessageBox::warning(this, "No XML File",
			"Please Give XML File At First! ", QMessageBox::Ok);
	}

}


//人脸检测身份识别 启动函数

//功能:
		//通过选择的XML文件加载模型
		//调用FaceRec::begainToRec()方法,实现人脸检测和身份识别
//参数传递:
		//xml文件路径, ui标签地址
//状态标志:
		//_recState,_cameraState,_flagSelecteXML
//互锁组件:
		//大众人脸检测+只视频+选择XML检测+生成XML检测 互锁
		//通过STOP解锁
void MainWindow::on_pushButtonFRRunRecBySelecte_clicked()
{


	if (_flagSelecteXML)
	{	//状态栏更新
		_nowProcess->setText(" Now: FaceRecBySelectedXML ");
		//互锁
		ui.pushButtonFRRunRecByGen->setDisabled(true);
		ui.pushButtonFRRunRecBySelecte->setDisabled(false);
		ui.pushButtonFRCommonFaceRec->setDisabled(true);
		ui.pushButtonFROnlyCamera->setDisabled(true);
		QString filePath = ui.labelFRSelectXMLPath->text();
		_faceRec->_recState = true;//修改状态变量,将其使能
		_faceRec->_cameraState = true; //使能摄像头,使之可以进入帧循环
		//加载必要文件
		_faceRec->_modelXmlAbsPath = filePath;
		_faceRec->_uiLabel = ui.labelShowFace;
		emit _FRStartRec();
		//_faceRec->begainToRec(filePath, ui.labelShowFace);//加载xml文件,单线程
	}
	else
	{
		QMessageBox::warning(this, "No XML File",
			"Please Give XML File At First! ", QMessageBox::Ok);
	}

}


//大众人脸检测 启动函数

//功能:
		//通过opencv提供的haar人脸xml文件来实现普通人脸的检测
//参数传递:
		//ui标签地址
//状态标志:
		//_recState,_cameraState
//互锁组件:
		//大众人脸检测+只视频+选择XML检测+生成XML检测 互锁
		//通过STOP解锁
void MainWindow::on_pushButtonFRCommonFaceRec_clicked()
{
	//状态栏更新
	_nowProcess->setText(" Now: CommonFaceRec ");
	//互锁
	ui.pushButtonFRRunRecByGen->setDisabled(true);
	ui.pushButtonFRRunRecBySelecte->setDisabled(true);
	ui.pushButtonFRCommonFaceRec->setDisabled(false);
	ui.pushButtonFROnlyCamera->setDisabled(true);

	_faceRec->_recState = true;//修改状态变量,将其使能
	_faceRec->_cameraState = true; //使能摄像头,使之可以进入帧循环
	//加载必要文件
	_faceRec->_uiLabelCom = ui.labelShowFace;
	emit _FRStartCommonRec();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//单线程
}


//视频流输出 启动函数

//功能:
		//视频流输出
//参数传递:
		//ui标签地址
//状态标志:
		//_cameraState
//互锁组件:
		//大众人脸检测+只视频+选择XML检测+生成XML检测 互锁
		//通过STOP解锁
void MainWindow::on_pushButtonFROnlyCamera_clicked()
{
	//更新状态栏
	_nowProcess->setText(" Now: OnlyVideo ");
	//互锁
	ui.pushButtonFRRunRecByGen->setDisabled(true);
	ui.pushButtonFRRunRecBySelecte->setDisabled(true);
	ui.pushButtonFRCommonFaceRec->setDisabled(true);
	ui.pushButtonFROnlyCamera->setDisabled(false);


	//_faceRec->_recState = true;//只显示不检测,无需使能
	_faceRec->_recState = false;
	_faceRec->_cameraState = true; //使能摄像头,使之可以进入帧循环
	//加载必要文件
	_faceRec->_uiLabelCom = ui.labelShowFace;
	qDebug() << "Mainwindow thread: " << QThread::currentThread() << endl;
	emit _FROnlyShow();
	//_faceRec->begainToCommonFaceRec(ui.labelShowFace);//单线程
}


//停止函数

//功能:
		//解锁互锁组件, 释放必要变量
//参数传递:
		//
//状态标志:
		//_recState,_cameraState, _camera
//注意点:
		//无法结束训练
void MainWindow::on_pushButtonFRStop_clicked()
{
	//解除互锁
	ui.pushButtonFRRunRecByGen->setDisabled(false);
	ui.pushButtonFRRunRecBySelecte->setDisabled(false);
	ui.pushButtonFROnlyCamera->setDisabled(false);
	ui.pushButtonFRCommonFaceRec->setDisabled(false);

	//控制使能开关, 不终止线程
	_faceRec->_cameraState = false;//关闭帧循环
	_faceRec->_recState = false;//关闭检测
	//释放摄像头
	_faceRec->_camera.release();//释放摄像头
	//清空label
	ui.labelShowFace->clear();
	//更新状态栏
	_nowProcess->setText(" Now: nothing ");
}


