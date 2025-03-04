//********************************************************************
//	filename: 	F:\mygit\QTrayToolbar\src\QTrayToolSetupDialog.h
//	desc:		
//
//	created:	wangdaye 17:2:2025   13:06
//********************************************************************
#ifndef QTrayToolSetupDialog_h__
#define QTrayToolSetupDialog_h__
#include <QDialog>
#include <QSet>
#include <QMap>
#include "struct.h"
class QTableView;
class QGraphicsView;
class QLineEdit;
class QToolButton;
class QLabel;
class QCheckBox;
class QStandardItemModel;
class QShortcut;
class QNotifyIconMap;
class QIconBar;
enum PathCheck
{
	CheckSuccess,
	CheckInvalid,
	CheckRepeat,
};
class QTrayToolSetupDialog : public QDialog
{
	Q_OBJECT

public:
	QTrayToolSetupDialog(QWidget* _parent);
	~QTrayToolSetupDialog();
	void clean(void);
	void setFilters(const QStringList& _filters);
	void addFolder(const QString& _folder, const QPixmap& _icon);
	void isAutoStartup(bool _b);
	void refreshAllTrayIcon(void);
	void RetranslateUi(void);
private:
	void CreateControl(void);
	PathCheck checkPath(const QString& _Path);
private slots:
	void onDeleteReleased_slot(void);
	void onAddReleased_slot(void);
	void onModifyReleased_slot(void);
	void onApplyReleased_slot(void);
	void onIconReleased_slot(void);
	void onOpenReleased_slot(void);
	void onStartupStateChanged(int _status);
	void onCurrentFolderChanged_slot(const QModelIndex& _Current, const QModelIndex& _Previous);
	void onTrayIconMove_slot(int _moveidx, int _beforeidx);
	void onTrayIconDelete_slot(int _idx);
signals:
	void filtersChanged_signal(const QStringList& _filters);
	void foldersChanged_signal(const QMap<QString, TFolderInfo>& _folders);
	void setAutoStartup_signal(bool _autoStartup);
private:
	QTableView* m_TV_folder;
	QToolButton* m_TB_icon;
	QLineEdit* m_LE_path;
	QToolButton* m_TB_open;
	QToolButton* m_TB_delete;
	QToolButton* m_TB_add;
	QToolButton* m_TB_modify;
	QLabel* m_LB_nameFilters;
	QLineEdit* m_LE_fileFilters;
	QCheckBox* m_CB_startup;
	QIconBar* m_TB_iconList;
	QToolButton* m_TB_apply;
	QStandardItemModel* m_folderModel;
	QShortcut* m_deleteShortcut;
private:
	QNotifyIconMap* m_NotifyIconMap;
	QPixmap m_iconPixmap;
	QList<quint64> m_ulOrderList;
	QList<quint64> m_ulDeletedOrderList;

	QByteArray m_UIOrderList;
	QString m_nameFilter;
	QMap<QString, TFolderInfo> m_MenuSet;		//��ǰ������
	QMap<QString, TFolderInfo> m_MenuInfoMap;	//֮ǰ������
};
#endif // QTrayToolSetupDialog_h__
