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
class TLnkFileEx : public TLnkFile
{
public:
	QAction* action;
	QTrayToolMenu* menuParent;
	TLnkFileEx() :TLnkFile(), action(NULL), menuParent(NULL) {}
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
	void update(const QString& _path, TLnkFileEx* _parent);
	void createTrayMenu(void);
	void createAction(QTrayToolMenu* _parent, TLnkFileEx* _LnkFile, bool _createSub);
	void creadeToolMenu(QTrayToolMenu* _parent, TLnkFileEx* _LnkFile);
	void createLnkList(const QString& _Path, TLnkFileEx* _parent);
private slots:
	void onSysTrayIconaActivated_slot(QSystemTrayIcon::ActivationReason _reason);
	void onLnkActionTrigger_slot(void);
	void onLnkFileChanged_slot(void);
	void onOpenFolderTrigger_slot(void);
	void onUpdateTrigger_slot(void);
	void onOptionTrigger_slot(void);
signals:
	void openSetupDialog_signal(void);
	void lnkFileChanged_signal(void);
private:
	const QStringList* m_nameFilters;
	QSystemTrayIcon* m_MainTrayIcon;
	QTrayToolMenu* m_ToolMenu;
	TLnkFileEx* m_mainLnkList;
private:
	QAction* m_AC_Option;
	QAction* m_AC_OpenFolder;
	QAction* m_AC_Update;
	QAction* m_AC_Quit;
	QMenu* m_trayContextMenu;
};


#endif // QtTrayToolControl_h__
