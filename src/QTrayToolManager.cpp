#include "QTrayToolManager.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#include <QSettings>
#include <QProcessEnvironment>
#include "QTrayToolControl.h"
#include "QTrayToolSetupDialog.h"

QTrayToolManager::QTrayToolManager(QObject* parent)
	: QObject(parent)
{
	m_SetupDialog = new QTrayToolSetupDialog(NULL);
	connect(m_SetupDialog, SIGNAL(filtersChanged_signal(const QStringList&)), this, SLOT(onFiltersChanged_slot(const QStringList&)));
	connect(m_SetupDialog, SIGNAL(foldersChanged_signal(const QMap<QString, TFolderInfo>&)), this, SLOT(onFoldersChanged_slot(const QMap<QString, TFolderInfo>&)));
	connect(m_SetupDialog, SIGNAL(setAutoStartup_signal(bool)), this, SLOT(setAutoStartup_slot(bool)));

	QProcessEnvironment t_SysEnvironment = QProcessEnvironment::systemEnvironment();
	QString t_appDataPath = t_SysEnvironment.value("APPDATA");
	m_startupLnkPath = t_appDataPath + "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\" +
		QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName() + ".lnk";

	loadSetting();
	if (m_TrayToolControls.size() == 0)
	{
		QTrayToolControl* t_TrayToolControl = new QTrayToolControl(this);
		connect(t_TrayToolControl, SIGNAL(openSetupDialog_signal()), this, SLOT(openSetupDialog_slot()));
		connect(t_TrayToolControl, SIGNAL(lnkFileChanged_signal()), this, SLOT(onLnkFileChanged_slot()));
		t_TrayToolControl->setNameFilters(&m_nameFilters);
		QString t_qlPath;
		if (!t_appDataPath.isEmpty())
			t_qlPath = t_appDataPath + "\\Microsoft\\Internet Explorer\\Quick Launch";
		t_TrayToolControl->setPath(t_qlPath, QPixmap(":/icons/ico/TrayIcon.ico"));
		m_TrayToolControls.insert(t_qlPath, t_TrayToolControl);
	}
}

QTrayToolManager::~QTrayToolManager()
{
	delete m_SetupDialog;
	for (QMap<QString, QTrayToolControl*>::iterator i = m_TrayToolControls.begin(); i != m_TrayToolControls.end(); ++i)
	{
		delete i.value();
	}
}
void QTrayToolManager::loadSetting(void)
{
	QFileInfo t_appPath(QCoreApplication::applicationFilePath());
	QString t_settingFile = t_appPath.absolutePath() + "/setting/" + t_appPath.completeBaseName() + ".dat";
	QFile t_file(t_settingFile);
	if (t_file.exists() && t_file.open(QIODevice::ReadOnly))
	{
		QDataStream in(&t_file);
		qint32 t_Traycount;
		in >> m_nameFilters >> t_Traycount;
		for (int i = 0; i < t_Traycount; ++i)
		{
			char* t_pBuff;
			uint t_readLen;
			in.readBytes(t_pBuff, t_readLen);
			QByteArray t_arr(t_pBuff, t_readLen);
			delete[] t_pBuff;
			QTrayToolControl* t_TrayToolControl = new QTrayToolControl(this);
			t_TrayToolControl->setNameFilters(&m_nameFilters);
			t_TrayToolControl->load(t_arr);
			m_TrayToolControls.insert(t_TrayToolControl->getPath(), t_TrayToolControl);
			connect(t_TrayToolControl, SIGNAL(openSetupDialog_signal()), this, SLOT(openSetupDialog_slot()));
			connect(t_TrayToolControl, SIGNAL(lnkFileChanged_signal()), this, SLOT(onLnkFileChanged_slot()));
		}
		t_file.close();
	}
}
void QTrayToolManager::saveSetting(void)
{
	QFileInfo t_appPath(QCoreApplication::applicationFilePath());
	QString t_settingFile = t_appPath.absolutePath() + "/setting/" + t_appPath.completeBaseName() + ".dat";
	QFile t_file(t_settingFile);
	if (t_file.open(QIODevice::WriteOnly)) {
		QDataStream out(&t_file);
		out << m_nameFilters;
		out << qint32(m_TrayToolControls.size());
		for (QMap<QString, QTrayToolControl*>::iterator i = m_TrayToolControls.begin(); i != m_TrayToolControls.end(); ++i)
		{
			QByteArray t_Buff = i.value()->toByteArray();
			out.writeBytes(t_Buff.constData(), t_Buff.size());
		}
		t_file.flush();
		t_file.close();
	}
}
void QTrayToolManager::openSetupDialog_slot(void)
{
	m_SetupDialog->setFilters(m_nameFilters);
	m_SetupDialog->clean();
	for (QMap<QString, QTrayToolControl*>::iterator i = m_TrayToolControls.begin(); i != m_TrayToolControls.end(); ++i)
		m_SetupDialog->addFolder(i.value()->getPath(), i.value()->getPixmap());
	m_SetupDialog->isAutoStartup(QFileInfo(m_startupLnkPath).exists());
	m_SetupDialog->refreshAllTrayIcon();
	m_SetupDialog->show();
}
void QTrayToolManager::onLnkFileChanged_slot(void)
{
	saveSetting();
}
void QTrayToolManager::onFiltersChanged_slot(const QStringList& _filters)
{
	m_nameFilters = _filters;
	saveSetting();
	for (QMap<QString, QTrayToolControl*>::iterator i = m_TrayToolControls.begin(); i != m_TrayToolControls.end(); ++i)
		i.value()->setNameFilters(&m_nameFilters);
}
void QTrayToolManager::onFoldersChanged_slot(const QMap<QString, TFolderInfo>& _folders)
{
	QSet<QString> t_newPaths;
	for (QMap<QString, TFolderInfo>::ConstIterator i = _folders.begin(); i != _folders.end(); ++i)
	{
		t_newPaths.insert(i.value().Path);
		if (m_TrayToolControls.contains(i.key()))
		{
			QTrayToolControl* t_pTrayToolControl = m_TrayToolControls[i.key()];
			t_pTrayToolControl->setPath(i.value().Path, i.value().Icon);
			if (i.key() != i.value().Path)
			{
				m_TrayToolControls.remove(i.key());
				m_TrayToolControls.insert(t_pTrayToolControl->getPath(), t_pTrayToolControl);
			}
		}
		else
		{
			QTrayToolControl* t_pTrayToolControl = new QTrayToolControl(this);
			connect(t_pTrayToolControl, SIGNAL(openSetupDialog_signal()), this, SLOT(openSetupDialog_slot()));
			connect(t_pTrayToolControl, SIGNAL(lnkFileChanged_signal()), this, SLOT(onLnkFileChanged_slot()));
			t_pTrayToolControl->setNameFilters(&m_nameFilters);
			t_pTrayToolControl->setPath(i.value().Path, i.value().Icon);
			m_TrayToolControls.insert(t_pTrayToolControl->getPath(), t_pTrayToolControl);
		}
	}
	for (QMap<QString, QTrayToolControl*>::iterator i = m_TrayToolControls.begin(); i != m_TrayToolControls.end(); )
	{
		if (t_newPaths.contains(i.key()))
		{
			++i;
			continue;
		}
		delete i.value();
		i = m_TrayToolControls.erase(i);
	}
	if (m_TrayToolControls.size() == 0)
	{
		QTrayToolControl* t_TrayToolControl = new QTrayToolControl(this);
		connect(t_TrayToolControl, SIGNAL(openSetupDialog_signal()), this, SLOT(openSetupDialog_slot()));
		connect(t_TrayToolControl, SIGNAL(lnkFileChanged_signal()), this, SLOT(onLnkFileChanged_slot()));
		t_TrayToolControl->setNameFilters(&m_nameFilters);
		t_TrayToolControl->setPath("", QPixmap(":/icons/ico/TrayIcon.ico"));
		m_TrayToolControls.insert(t_TrayToolControl->getPath(), t_TrayToolControl);
		m_SetupDialog->addFolder(t_TrayToolControl->getPath(), t_TrayToolControl->getPixmap());
	}
	saveSetting();
}
void QTrayToolManager::setAutoStartup_slot(bool _autoRun)
{
	if (!_autoRun)
		QFile(m_startupLnkPath).remove();
	else if (!QFileInfo(m_startupLnkPath).exists())
		QFile(QCoreApplication::applicationFilePath()).link(m_startupLnkPath);
}
