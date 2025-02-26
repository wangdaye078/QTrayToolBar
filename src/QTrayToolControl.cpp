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
bool QTrayToolControl::update(TLnkFileEx* _parent)
{
	bool t_changed = false;
	if (!_parent->isDir)
		return false;
	QMap<QString, TLnkFileEx*> t_Paths;
	foreach(TLnkFile * t_lnk, _parent->subLnk)
		t_Paths.insert(t_lnk->path, static_cast<TLnkFileEx*>(t_lnk));

	QFileInfoList t_fileList = QDir(_parent->path).entryInfoList(*m_nameFilters, QDir::NoDotAndDotDot | QDir::AllEntries | QDir::System, QDir::DirsFirst);
	QSet<QString> t_currentPaths;
	foreach(const QFileInfo & t_file, t_fileList)
	{
		QString t_path = QDir::toNativeSeparators(t_file.absoluteFilePath());
		bool t_isDir = t_file.isDir();
		t_currentPaths.insert(t_path);
		TLnkFileEx* t_LnkFile = NULL;
		if (!t_Paths.contains(t_path))
		{
			//如果不包含，则需要先创建数据
			_parent->subLnk.append(new TLnkFileEx());
			t_LnkFile = static_cast<TLnkFileEx*>(_parent->subLnk.last());
			if (t_file.suffix() == "lnk")
				t_LnkFile->name = lnkFileReader(NULL, t_path).getName();
			else if (t_file.suffix() == "url")
				t_LnkFile->name = t_file.completeBaseName();
			else
				t_LnkFile->name = t_file.fileName();
			t_LnkFile->path = QDir::toNativeSeparators(t_path);
			t_LnkFile->icon = getPixmapFromFile(t_LnkFile->path);
			t_LnkFile->isDir = t_isDir;
			//再创建菜单项
			createAction(_parent->menu, t_LnkFile);
			t_Paths.insert(t_LnkFile->path, t_LnkFile);
			if (t_LnkFile->isDir)
				m_DirMap.insert(t_LnkFile->path, t_LnkFile);
			t_changed = true;
		}
		if (t_LnkFile == NULL)
			t_LnkFile = t_Paths[t_path];

		if (t_isDir)
			t_changed |= update(t_LnkFile);
	}
	for (QMap<QString, TLnkFileEx*>::iterator i = t_Paths.begin(); i != t_Paths.end(); ++i)
	{
		if (!t_currentPaths.contains(i.key()))
		{
			TLnkFileEx* t_LnkFile = i.value();
			if (t_LnkFile->isDir)
			{
				update(t_LnkFile);
				m_folderWatcher->removePath(t_LnkFile->path);
				m_DirMap.remove(t_LnkFile->path);
			}
			t_LnkFile->menuParent->removeAction(t_LnkFile->action);

			_parent->removeSubPath(t_LnkFile->path);
			t_changed = true;
		}
	}
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
	foreach(TLnkFile * t_lnk, _LnkFile->subLnk)
	{
		TLnkFileEx* t_lnkEx = static_cast<TLnkFileEx*>(t_lnk);
		createAction(_LnkFile->menu, t_lnkEx);
	}
}
void QTrayToolControl::createLnkList(const QString& _Path, TLnkFileEx* _parent)
{
	QFileInfoList t_fileList = QDir(_Path).entryInfoList(*m_nameFilters, QDir::NoDotAndDotDot | QDir::AllEntries | QDir::System, QDir::DirsFirst);
	foreach(const QFileInfo & t_file, t_fileList)
	{
		_parent->subLnk.append(new TLnkFileEx());
		TLnkFileEx* t_LnkFile = static_cast<TLnkFileEx*>(_parent->subLnk.last());
		if (t_file.suffix() == "lnk")
			t_LnkFile->name = lnkFileReader(NULL, t_file.absoluteFilePath()).getName();
		else if (t_file.suffix() == "url")
			t_LnkFile->name = t_file.completeBaseName();
		else
			t_LnkFile->name = t_file.fileName();
		t_LnkFile->path = QDir::toNativeSeparators(t_file.absoluteFilePath());
		t_LnkFile->icon = getPixmapFromFile(t_LnkFile->path);
		t_LnkFile->isDir = t_file.isDir();

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