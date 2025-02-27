#include "QTrayToolControl.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QMenu>
#include <QDir>
#include <QFileInfo>
#include <QBuffer>
#include <QUrl>
#include <QMap>
#include <QSet>


#include "QTrayToolMenu.h"
#include "lnkFileReader.h"
#include "struct.h"
#include "basicDefine.h"


QTrayToolControl::QTrayToolControl(QObject* parent)
	: QObject(parent), m_nameFilters(NULL)
{
	m_MainTrayIcon = new QSystemTrayIcon(this);
	m_ToolMenu = new QTrayToolMenu(NULL);
	m_mainLnkList = new TLnkFileEx();
	m_folderWatcher = new QFileSystemWatcher(this);
	createTrayMenu();
	RetranslateUi();
	connect(m_MainTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onSysTrayIconActivated_slot(QSystemTrayIcon::ActivationReason)));
	connect(m_folderWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onFolderChanged_slot(const QString&)));
}
QTrayToolControl::~QTrayToolControl()
{
	delete m_folderWatcher;
	delete m_mainLnkList;
	delete m_ToolMenu;
	delete m_MainTrayIcon;
}
void QTrayToolControl::setNameFilters(const QStringList* _nameFilters)
{
	m_nameFilters = _nameFilters;
}
void QTrayToolControl::setPath(const QString& _Path, const QPixmap& _pix)
{
	QString t_nativePath = QDir::toNativeSeparators(_Path);
	if (m_mainLnkList->icon.cacheKey() == _pix.cacheKey() && m_mainLnkList->path == t_nativePath)
		return;
	m_mainLnkList->icon = _pix;
	if (m_mainLnkList->path != t_nativePath)
	{
		reset();
		m_folderWatcher->removePaths(m_folderWatcher->directories());
		m_mainLnkList->path = t_nativePath;
		m_mainLnkList->icon = _pix;		//经历过reset，需要再设置一遍
		m_mainLnkList->isDir = QFileInfo(t_nativePath).isDir();
		if (m_mainLnkList->isDir)
		{
			m_mainLnkList->name = QFileInfo(t_nativePath).completeBaseName();
			createLnkList(m_mainLnkList->path, m_mainLnkList);
			m_mainLnkList->menuParent = NULL;
			m_mainLnkList->menu = m_ToolMenu;
			creadeToolMenu(m_mainLnkList);
		}
	}
	emit lnkFileChanged_signal();
	m_MainTrayIcon->setIcon(QIcon(_pix));
	m_MainTrayIcon->show();
}
const QString& QTrayToolControl::getPath(void)
{
	return m_mainLnkList->path;
}
const QPixmap& QTrayToolControl::getPixmap(void)
{
	return m_mainLnkList->icon;
}
void QTrayToolControl::reset(void)
{
	delete m_mainLnkList;
	delete m_ToolMenu;
	m_ToolMenu = new QTrayToolMenu(NULL);
	m_mainLnkList = new TLnkFileEx();
}
void QTrayToolControl::load(const QByteArray& _buff)
{
	QDataStream in(_buff);
	in >> *m_mainLnkList;

	m_folderWatcher->removePaths(m_folderWatcher->directories());
	m_MainTrayIcon->setIcon(QIcon(m_mainLnkList->icon));
	m_mainLnkList->menuParent = NULL;
	m_mainLnkList->menu = m_ToolMenu;
	creadeToolMenu(m_mainLnkList);
	if (update(m_mainLnkList))
		emit lnkFileChanged_signal();
	m_MainTrayIcon->show();
}
QByteArray QTrayToolControl::toByteArray(void)
{
	QByteArray t_buff;
	QDataStream out(&t_buff, QIODevice::WriteOnly);
	out << *m_mainLnkList;
	return t_buff;
}
TLnkFileEx* QTrayToolControl::fileInfoToLnkFileEx(const QFileInfo& _fileInfo)
{
	TLnkFileEx* t_LnkFile = new TLnkFileEx();
	if (_fileInfo.suffix() == "lnk")
		t_LnkFile->name = lnkFileReader(NULL, _fileInfo.absoluteFilePath()).getName();
	else if (_fileInfo.suffix() == "url")
		t_LnkFile->name = _fileInfo.completeBaseName();
	else
		t_LnkFile->name = _fileInfo.fileName();
	t_LnkFile->path = QDir::toNativeSeparators(_fileInfo.absoluteFilePath());
	t_LnkFile->icon = getPixmapFromFile(t_LnkFile->path);
	t_LnkFile->isDir = _fileInfo.isDir();
	return t_LnkFile;
}
bool QTrayToolControl::update(TLnkFileEx* _parent)
{
	bool t_changed = false;
	if (!_parent->isDir)
		return false;

	QFileInfoList t_fileList = QDir(_parent->path).entryInfoList(*m_nameFilters, QDir::NoDotAndDotDot | QDir::AllEntries | QDir::System, QDir::DirsFirst);
	//创建新旧文件列表的集合，和用于快速定位的表
	QSet<QString> t_oldPathSet;
	QMap<QString, TLnkFileEx*> t_oldPath2Lnkfile;
	for (TLnkFile* t_lnk : _parent->subLnk)
	{
		t_oldPathSet.insert(t_lnk->path);
		t_oldPath2Lnkfile.insert(t_lnk->path, static_cast<TLnkFileEx*>(t_lnk));
	}
	QSet<QString> t_newPathSet;
	QMap<QString, QFileInfo*> t_newPath2Fileinfo;
	for (QFileInfo& i : t_fileList)
	{
		t_newPathSet.insert(QDir::toNativeSeparators(i.absoluteFilePath()));
		t_newPath2Fileinfo.insert(QDir::toNativeSeparators(i.absoluteFilePath()), &i);
	}
	//使用集合差获得需要添加和删除的文件和文件夹
	QSet<QString> t_needdelete = t_oldPathSet - t_newPathSet;
	QSet<QString> t_needadd = t_newPathSet - t_oldPathSet;
	//添加、删除、并对现存的文件夹递归更新
	for (const QString& t_filePath : t_needadd)
	{
		TLnkFileEx* t_LnkFile = fileInfoToLnkFileEx(*t_newPath2Fileinfo[t_filePath]);
		_parent->subLnk.append(t_LnkFile);
		//再创建菜单项
		createAction(_parent->menu, t_LnkFile);
		t_oldPath2Lnkfile.insert(t_LnkFile->path, t_LnkFile);
		if (t_LnkFile->isDir)
			m_DirMap.insert(t_LnkFile->path, t_LnkFile);
	}
	for (const QString& t_filePath : t_needdelete)
	{
		TLnkFileEx* t_LnkFile = t_oldPath2Lnkfile[t_filePath];
		if (t_LnkFile->isDir)
		{
			update(t_LnkFile);
			m_folderWatcher->removePath(t_LnkFile->path);
			m_DirMap.remove(t_LnkFile->path);
		}
		t_LnkFile->menuParent->removeAction(t_LnkFile->action);
		_parent->removeSubPath(t_LnkFile->path);
	}
	for (QMap<QString, QFileInfo*>::iterator i = t_newPath2Fileinfo.begin(); i != t_newPath2Fileinfo.end(); ++i)
	{
		if (i.value()->isDir())
			t_changed |= update(t_oldPath2Lnkfile[i.key()]);
	}
	t_changed |= (t_needdelete.size() > 0) | (t_needadd.size() > 0);

	return t_changed;
}
void QTrayToolControl::createTrayMenu(void)
{
	m_AC_Option = new QAction(this);
	m_AC_Option->setObjectName(QString::fromUtf8("m_AC_Option"));

	m_AC_OpenFolder = new QAction(this);
	m_AC_OpenFolder->setObjectName(QString::fromUtf8("m_AC_OpenFolder"));

	m_AC_Quit = new QAction(this);
	m_AC_Quit->setObjectName(QString::fromUtf8("m_AC_Quit"));

	m_trayContextMenu = new QMenu(NULL);
	m_trayContextMenu->addAction(m_AC_Option);
	m_trayContextMenu->addAction(m_AC_OpenFolder);
	m_trayContextMenu->addAction(m_AC_Quit);

	m_MainTrayIcon->setContextMenu(m_trayContextMenu);
	//------------------------------------------------
	connect(m_AC_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(m_AC_OpenFolder, SIGNAL(triggered()), this, SLOT(onOpenFolderTrigger_slot()));
	connect(m_AC_Option, SIGNAL(triggered()), this, SLOT(onOptionTrigger_slot()));
}
void QTrayToolControl::RetranslateUi(void)
{
	m_AC_Option->setText(tr("(&O)ption..."));
	m_AC_OpenFolder->setText(tr("Open (&F)older"));
	m_AC_Quit->setText(tr("(&Q)uit"));
}
void QTrayToolControl::createAction(QTrayToolMenu* _menu, TLnkFileEx* _LnkFile)
{
	QAction* t_Action = NULL;
	if (_LnkFile->isDir)
	{
		QTrayToolMenu* t_SubMenu = new QTrayToolMenu(NULL);
		t_SubMenu->setTitle(_LnkFile->name);
		t_SubMenu->setIcon(QIcon(_LnkFile->icon));
		t_Action = _menu->addMenu(t_SubMenu);
		t_Action->setProperty("path", _LnkFile->path);
		_LnkFile->menuParent = _menu;
		_LnkFile->menu = t_SubMenu;
		creadeToolMenu(_LnkFile);
	}
	else
	{
		t_Action = new QAction(_menu);
		t_Action->setObjectName(_LnkFile->name);
		t_Action->setText(_LnkFile->name);
		t_Action->setProperty("path", _LnkFile->path);
		t_Action->setIcon(QIcon(_LnkFile->icon));
		_menu->addAction(t_Action);
		_LnkFile->menuParent = _menu;
	}
	_LnkFile->action = t_Action;
}
void QTrayToolControl::creadeToolMenu(TLnkFileEx* _LnkFile)
{
	m_folderWatcher->addPath(_LnkFile->path);
	m_DirMap.insert(_LnkFile->path, _LnkFile);
	_LnkFile->menu->setLnkFile(_LnkFile);
	_LnkFile->menu->setProperty("path", _LnkFile->path);
	connect(_LnkFile->menu, SIGNAL(LnkFileChanged_signal()), this, SLOT(onLnkFileChanged_slot()));
	for (TLnkFile* t_lnk : _LnkFile->subLnk)
	{
		TLnkFileEx* t_lnkEx = static_cast<TLnkFileEx*>(t_lnk);
		createAction(_LnkFile->menu, t_lnkEx);
	}
}
void QTrayToolControl::createLnkList(const QString& _Path, TLnkFileEx* _parent)
{
	QFileInfoList t_fileList = QDir(_Path).entryInfoList(*m_nameFilters, QDir::NoDotAndDotDot | QDir::AllEntries | QDir::System, QDir::DirsFirst);
	for (QFileInfoList::iterator i = t_fileList.begin(); i != t_fileList.end(); ++i)
	{
		TLnkFileEx* t_LnkFile = fileInfoToLnkFileEx(*i);
		_parent->subLnk.append(t_LnkFile);
		if (t_LnkFile->isDir)
			createLnkList(t_LnkFile->path, t_LnkFile);
	}
}
void QTrayToolControl::onSysTrayIconActivated_slot(QSystemTrayIcon::ActivationReason _reason)
{
	switch (_reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
	{
		if (m_mainLnkList->subLnk.size() > 0)
		{
			QRect t_taskbarRect = getToolbarRect();
			QSize t_hintSize = m_ToolMenu->sizeHint();
			m_ToolMenu->popup(QPoint(QCursor::pos().x(), t_taskbarRect.y()) - QPoint(t_hintSize.width(), t_hintSize.height()));
		}
		break;
	}
	default:
		;
	}
}
void QTrayToolControl::onFolderChanged_slot(const QString& _path)
{
	//如果直接删除一个子文件夹，可能会先调用父目录的onFolderChanged_slot，再调用子目录的，
	//这会导致调用子目录事件的时候，这个子目录的数据已经都没有了。
	if (m_DirMap.contains(_path) && update(m_DirMap[_path]))
		emit lnkFileChanged_signal();
}
void QTrayToolControl::onLnkActionTrigger_slot(void)
{
	QObject* t_Sender = sender();
	QString t_path = t_Sender->property("path").toString();
	QUrl t_url = QUrl::fromLocalFile(t_path);
	QDesktopServices::openUrl(t_url);
}
void QTrayToolControl::onLnkFileChanged_slot(void)
{
	emit lnkFileChanged_signal();
}
void QTrayToolControl::onOpenFolderTrigger_slot(void)
{
	QUrl t_url = QUrl::fromLocalFile(m_mainLnkList->path);
	QDesktopServices::openUrl(t_url);
}
void QTrayToolControl::onOptionTrigger_slot(void)
{
	emit openSetupDialog_signal();
}