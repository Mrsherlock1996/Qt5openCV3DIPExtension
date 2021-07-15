#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <qfiledialog.h>
#include "FaceRec.h"
#include <fstream>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	FaceRec* _faceRec;

private:
    Ui::MainWindowClass ui;

public slots:
	void on_pushButtonFRAddPath_clicked();
	void on_pushButtonFRRunRec_clicked();
	void on_pushButtonGenTxtFile_clicked();
	void on_pushButtonSelectTxtFile_clicked();
};
