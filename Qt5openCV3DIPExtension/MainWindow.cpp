#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	_faceRec = new FaceRec; //初始化一个人脸检测器
}

void MainWindow::on_pushButtonFRAddPath_clicked()
{
	QFileDialog fileDlg;
	QString txtPath = fileDlg.getOpenFileName(nullptr,
		"Select train set annotation file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	ui.labelShowFaceTxtFilePath->setText(txtPath);  
}

void MainWindow::on_pushButtonFRRunRec_clicked()
{
	QString filePath = ui.labelShowFaceTxtFilePath->text();
	_faceRec->_recState = true;//修改状态变量,将其使能
	_faceRec->begainToRec(filePath, ui.labelShowFace);//加载xml文件
}

void MainWindow::on_pushButtonGenTxtFile_clicked()
{
	//TrainSetImage
	QFileDialog fileDlg; //创建文件对话框获取训练集路径
	QString trainSetRootPath = fileDlg.getExistingDirectory(
		nullptr, "Select train set root path", QDir::currentPath());
	QDir path;
	path.setCurrent(trainSetRootPath);
	QStringList fileNameList = path.entryList(); //这里只是训练图片文件名字,无路径信息
	ui.labelShowFaceTrainSetRootPath->setText(trainSetRootPath);//将训练集所在根目录显示

	//TrainSetImageAnnotation
	QString txtPath = fileDlg.getExistingDirectory(		
		nullptr, "Select txt stored path", trainSetRootPath);//获取train set 的信息文件txt保存路径
	ui.labelShowFaceTxtFilePath->setText(txtPath + "/trainSetAnnotation.txt"); //将训练集信息文件路径显示
	std::fstream file;
	file.open((txtPath.toStdString()+"/"+"trainSetAnnotation.txt"),
		std::ios::in|std::ios::out|std::ios::trunc);//打开txt文件,没有则创建
	for (int i = 2; i < fileNameList.size(); i++) //从2开始是排出第一行的第二行的.和.. 
	{
		ui.plainTextEditTrainSetFileNames->appendPlainText(trainSetRootPath+"/"+fileNameList[i]); 
		//按照格式向txt文件中输入内容
		file <<(trainSetRootPath.toStdString() + "/" + fileNameList[i].toStdString()+"\n");
		((i + 1) == fileNameList.size()) ? (file << "1228") : (file << "1228\n");//排去文本最后一行是空行
	}
	file.close();//关闭文件
}

void MainWindow::on_pushButtonSelectTxtFile_clicked()
{
	QFileDialog fileDlg;
	QString txtFileAbsPath = fileDlg.getOpenFileName(
		nullptr, "Select a  txt file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	ui.labelShowFaceTxtFilePath->setText(txtFileAbsPath);
}
