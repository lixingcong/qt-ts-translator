#include <QApplication>

#include "MainWindow.h"
#include "TranslateLoader.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	TranslateLoader::loadAtRun(); // 多国语言

	MainWindow w;
	w.show();

	return a.exec();
}
