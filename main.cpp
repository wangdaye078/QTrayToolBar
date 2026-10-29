#include "src/basicDefine.h"
#include "src/QNotifyIconMap.h"
#include "src/QTrayToolManager.h"
#include <QFileInfo>
#include <QTranslator>
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

	QApplication app(argc, argv);

	QString t_settingFile = app.applicationDirPath() + "/setting/StyleSheet.txt";
	QFile t_file(t_settingFile);
	if (t_file.exists() && t_file.open(QIODevice::ReadOnly))
	{
		app.setStyleSheet(UTF8ToQString(t_file.readAll().constData()));
		t_file.close();
	}

	QApplication::setQuitOnLastWindowClosed(false);
	QTranslator translator;
	if (translator.load(QLocale(), QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName(),
		"_", QCoreApplication::applicationDirPath() + "/translations"))
		QCoreApplication::installTranslator(&translator);
	QTrayToolManager t_tray(&app);
	return app.exec();
}
