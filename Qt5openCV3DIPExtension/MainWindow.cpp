#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	_faceRec = new FaceRec; //��ʼ��һ�����������
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
	_faceRec->_recState = true;//�޸�״̬����,����ʹ��
	_faceRec->begainToRec(filePath, ui.labelShowFace);//����xml�ļ�
}

void MainWindow::on_pushButtonGenTxtFile_clicked()
{
	//TrainSetImage
	QFileDialog fileDlg; //�����ļ��Ի����ȡѵ����·��
	QString trainSetRootPath = fileDlg.getExistingDirectory(
		nullptr, "Select train set root path", QDir::currentPath());
	QDir path;
	path.setCurrent(trainSetRootPath);
	QStringList fileNameList = path.entryList(); //����ֻ��ѵ��ͼƬ�ļ�����,��·����Ϣ
	ui.labelShowFaceTrainSetRootPath->setText(trainSetRootPath);//��ѵ�������ڸ�Ŀ¼��ʾ

	//TrainSetImageAnnotation
	QString txtPath = fileDlg.getExistingDirectory(		
		nullptr, "Select txt stored path", trainSetRootPath);//��ȡtrain set ����Ϣ�ļ�txt����·��
	ui.labelShowFaceTxtFilePath->setText(txtPath + "/trainSetAnnotation.txt"); //��ѵ������Ϣ�ļ�·����ʾ
	std::fstream file;
	file.open((txtPath.toStdString()+"/"+"trainSetAnnotation.txt"),
		std::ios::in|std::ios::out|std::ios::trunc);//��txt�ļ�,û���򴴽�
	for (int i = 2; i < fileNameList.size(); i++) //��2��ʼ���ų���һ�еĵڶ��е�.��.. 
	{
		ui.plainTextEditTrainSetFileNames->appendPlainText(trainSetRootPath+"/"+fileNameList[i]); 
		//���ո�ʽ��txt�ļ�����������
		file <<(trainSetRootPath.toStdString() + "/" + fileNameList[i].toStdString()+"\n");
		((i + 1) == fileNameList.size()) ? (file << "1228") : (file << "1228\n");//��ȥ�ı����һ���ǿ���
	}
	file.close();//�ر��ļ�
}

void MainWindow::on_pushButtonSelectTxtFile_clicked()
{
	QFileDialog fileDlg;
	QString txtFileAbsPath = fileDlg.getOpenFileName(
		nullptr, "Select a  txt file", QDir::currentPath(), "txt(*.txt);;all(*.*)");
	ui.labelShowFaceTxtFilePath->setText(txtFileAbsPath);
}
