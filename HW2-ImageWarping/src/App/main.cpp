#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
	//std::cout << "initiate done!" << std::endl;
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
