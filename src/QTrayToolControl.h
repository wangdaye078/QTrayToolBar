//********************************************************************
//	filename: 	F:\mygit\QTrayToolbar\src\QtTrayToolControl.h
//	desc:		
//
//	created:	wangdaye 16:2:2025   20:19
//********************************************************************
#ifndef QtTrayToolControl_h__
#define QtTrayToolControl_h__
#include <QObject>
#include <QSystemTrayIcon>
#include <QtDebug>
#include "struct.h"
class QTrayToolMenu;
class QMenu;
class QAction;
class QFileSystemWatcher;
class QFileInfo;

class TLnkFileEx : public TLnkFile
{
public:
	QAction* action;
	QTrayToolMenu* menuParent;	//对于每一项，它是这一项所在的菜单
	QTrayToolMenu* menu;	//对于子菜单，他就是子菜单本身
	TLnkFileEx() :TLnkFile(), action(NULL), menuParent(NULL), menu(NULL) {}
	virtual ~TLnkFileEx() {
	};
	virtual TLnkFile* creatSub()
	{
		return new TLnkFileEx();
	}
	void removeSubPath(const QString& _path)
	{
		for (QList<TLnkFile*>::iterator i = subLnk.begin(); i != subLnk.end(); ++i)
		{
			if ((*i)->path == _path)
			{
				delete (*i);
				subLnk.erase(i);
				break;
			}
		}
	}
};

class QTrayToolControl : public QObject
{
	Q_OBJECT
public:
	QTrayToolControl(QObject* parent);
	~QTrayToolControl();
	void setNameFilters(const QStringList* _nameFilters);
	void setPath(const QString& _path, const QPixmap& _pix);
	const QString& getPath(void);
	const QPixmap& getPixmap(void);
	void reset(void);
	void load(const QByteArray& _buff);
	QByteArray toByteArray(void);
	void RetranslateUi(void);
private:
	TLnkFileEx* fileInfoToLnkFileEx(const QFileInfo& _fileInfo);
	bool update(TLnkFileEx* _parent);	//返回是否有改变
	void createTrayMenu(void);
	void createAction(QTrayToolMenu* _menu, TLnkFileEx* _LnkFile);
	void creadeToolMenu(TLnkFileEx* _LnkFile);
	void createLnkList(const QString& _Path, TLnkFileEx* _parent);
private slots:
	void onSysTrayIconActivated_slot(QSystemTrayIcon::ActivationReason _reason);
	void onFolderChanged_slot(const QString& _path);
	void onLnkActionTrigger_slot(void);
	void onLnkFileChanged_slot(void);
	void onOpenFolderTrigger_slot(void);
	void onOptionTrigger_slot(void);
signals:
	void openSetupDialog_signal(void);
	void lnkFileChanged_signal(void);
private:
	const QStringList* m_nameFilters;
	QSystemTrayIcon* m_MainTrayIcon;
	QTrayToolMenu* m_ToolMenu;
	TLnkFileEx* m_mainLnkList;
	QFileSystemWatcher* m_folderWatcher;
	QMap<QString, TLnkFileEx*> m_DirMap;
private:
	QAction* m_AC_Option;
	QAction* m_AC_OpenFolder;
	QAction* m_AC_Quit;
	QMenu* m_trayContextMenu;
};


#endif // QtTrayToolControl_h__
