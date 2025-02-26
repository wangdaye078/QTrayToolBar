#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QFileInfo>
#include "src/QTrayToolManager.h"
#include "src/QNotifyIconMap.h"
#include "src/basicDefine.h"
int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QString t_settingFile = app.applicationDirPath() + "\\StyleSheet.txt";
	QFile t_file(t_settingFile);
	if (t_file.exists() && t_file.open(QIODevice::ReadOnly))
		app.setStyleSheet(UTF8ToQString(t_file.readAll().constData()));

	QApplication::setQuitOnLastWindowClosed(false);
	QTranslator translator;
	if (translator.load(QLocale(), QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName(),
		"_", QCoreApplication::applicationDirPath()))
		QCoreApplication::installTranslator(&translator);
	QTrayToolManager t_tray(&app);
	return app.exec();
}
