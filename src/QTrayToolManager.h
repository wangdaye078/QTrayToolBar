//********************************************************************
//	filename: 	F:\mygit\QTrayToolbar\src\QTrayToolManager.h
//	desc:		
//
//	created:	wangdaye 15:2:2025   14:28
//********************************************************************
#ifndef QTrayToolManager_h__
#define QTrayToolManager_h__
#include <QMap>
#include <QSystemTrayIcon> 
#include "struct.h"
class QAction;
class QMenu;
class QTrayToolControl;
class QTrayToolSetupDialog;

class QTrayToolManager : public QObject
{
	Q_OBJECT

public:
	QTrayToolManager(QObject* parent);
	~QTrayToolManager();
private:
	void loadSetting(void);
	void saveSetting(void);
private slots:
	void openSetupDialog_slot(void);
	void onLnkFileChanged_slot(void);
	void onFiltersChanged_slot(const QStringList& _filters);
	void onFoldersChanged_slot(const QMap<QString, TFolderInfo>& _folders);
	void setAutoStartup_slot(bool _autoRun);
private:
	QString m_startupLnkPath;
	QStringList m_nameFilters;
	QMap<QString, QTrayToolControl*> m_TrayToolControls;
	QTrayToolSetupDialog* m_SetupDialog;
};

#endif // QTrayToolManager_h__
