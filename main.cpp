#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QFileInfo>
#include "src/QTrayToolManager.h"
int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QApplication::setQuitOnLastWindowClosed(false);
	QTranslator translator;
	if (translator.load(QLocale(), QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName(),
		"_", QCoreApplication::applicationDirPath()))
		QCoreApplication::installTranslator(&translator);
	QTrayToolManager t_tray(&app);
	return app.exec();
}
