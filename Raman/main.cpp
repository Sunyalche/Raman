#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QTextCodec *codec = QTextCodec::codecForName("GBK");
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
