#include "QTrayToolSetupDialog.h"
#include <QGridLayout>
#include <QTableView>
#include <QCoreApplication>
#include <QGraphicsView>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <QRegExp> 
#include <QSettings>
#include <QCheckBox>
#include <QShortcut>
#include "QNotifyIconMap.h"
#include "QIconBar.h"
#include "basicDefine.h"

QTrayToolSetupDialog::QTrayToolSetupDialog(QWidget* _parent)
	: QDialog(_parent)
{
	CreateControl();
	RetranslateUi();
}

QTrayToolSetupDialog::~QTrayToolSetupDialog()
{
}
void QTrayToolSetupDialog::CreateControl(void)
{
	setObjectName(QString::fromUtf8("Form"));
	resize(482, 252);

	QGridLayout* t_mainLayout = new QGridLayout(this);
	t_mainLayout->setObjectName(QString::fromUtf8("t_mainLayout"));
	m_TV_folder = new QTableView(this);
	m_TV_folder->setObjectName(QString::fromUtf8("m_TV_folder"));
	m_TV_folder->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_TV_folder->setSelectionMode(QAbstractItemView::SingleSelection);
	m_TV_folder->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_TV_folder->horizontalHeader()->setVisible(false);
	m_TV_folder->horizontalHeader()->setStretchLastSection(true);
	m_TV_folder->verticalHeader()->setVisible(false);
	m_TV_folder->verticalHeader()->setMinimumSectionSize(18);
	m_TV_folder->verticalHeader()->setDefaultSectionSize(18);

	t_mainLayout->addWidget(m_TV_folder, 0, 0, 1, 7);

	m_TB_icon = new QToolButton(this);
	m_TB_icon->setObjectName(QString::fromUtf8("m_TB_icon"));
	m_TB_icon->setMinimumSize(QSize(48, 48));
	m_TB_icon->setMaximumSize(QSize(48, 48));
	QIcon icon;
	icon.addFile(QString::fromUtf8("ico/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
	m_TB_icon->setIcon(icon);
	m_TB_icon->setIconSize(QSize(48, 48));

	t_mainLayout->addWidget(m_TB_icon, 1, 0, 2, 1);

	m_LE_path = new QLineEdit(this);
	m_LE_path->setObjectName(QString::fromUtf8("m_LE_path"));
	m_LE_path->setReadOnly(true);

	t_mainLayout->addWidget(m_LE_path, 1, 1, 1, 5);

	m_TB_open = new QToolButton(this);
	m_TB_open->setObjectName(QString::fromUtf8("m_TB_open"));

	t_mainLayout->addWidget(m_TB_open, 1, 6, 1, 1);

	QSpacerItem* t_horizontalSpacer1 = new QSpacerItem(157, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	t_mainLayout->addItem(t_horizontalSpacer1, 2, 1, 1, 1);

	m_TB_delete = new QToolButton(this);
	m_TB_delete->setObjectName(QString::fromUtf8("m_TB_delete"));
	m_TB_delete->setMinimumSize(QSize(50, 0));

	t_mainLayout->addWidget(m_TB_delete, 2, 2, 1, 1);

	m_TB_add = new QToolButton(this);
	m_TB_add->setObjectName(QString::fromUtf8("m_TB_add"));
	m_TB_add->setMinimumSize(QSize(50, 0));

	t_mainLayout->addWidget(m_TB_add, 2, 3, 1, 1);

	m_TB_modify = new QToolButton(this);
	m_TB_modify->setObjectName(QString::fromUtf8("m_TB_modify"));
	m_TB_modify->setMinimumSize(QSize(50, 0));

	t_mainLayout->addWidget(m_TB_modify, 2, 4, 1, 1);

	m_TB_applyFolder = new QToolButton(this);
	m_TB_applyFolder->setObjectName(QString::fromUtf8("m_TB_applyFolder"));
	m_TB_applyFolder->setMinimumSize(QSize(50, 0));

	t_mainLayout->addWidget(m_TB_applyFolder, 2, 5, 1, 2);

	m_LB_nameFilters = new QLabel(this);
	m_LB_nameFilters->setObjectName(QString::fromUtf8("m_LB_nameFilters"));

	t_mainLayout->addWidget(m_LB_nameFilters, 3, 0, 1, 1);

	m_LE_fileFilters = new QLineEdit(this);
	m_LE_fileFilters->setObjectName(QString::fromUtf8("m_LE_fileFilters"));

	t_mainLayout->addWidget(m_LE_fileFilters, 3, 1, 1, 4);

	m_TB_applyFilter = new QToolButton(this);
	m_TB_applyFilter->setObjectName(QString::fromUtf8("m_TB_applyFilter"));
	m_TB_applyFilter->setMinimumSize(QSize(50, 0));

	t_mainLayout->addWidget(m_TB_applyFilter, 3, 5, 1, 2);

	m_CB_startup = new QCheckBox(this);
	m_CB_startup->setObjectName(QString::fromUtf8("m_CB_startup"));

	t_mainLayout->addWidget(m_CB_startup, 4, 0, 1, 1);

	m_TB_iconList = new QIconBar(this);
	m_TB_iconList->setObjectName(QString::fromUtf8("m_TB_iconList"));
	t_mainLayout->addWidget(m_TB_iconList, 4, 1, 1, 4);

	m_TB_applyIconOrder = new QToolButton(this);
	m_TB_applyIconOrder->setObjectName(QString::fromUtf8("m_TB_applyIconOrder"));
	m_TB_applyIconOrder->setMinimumSize(QSize(50, 0));

	t_mainLayout->addWidget(m_TB_applyIconOrder, 4, 5, 1, 2);

	//--------------------------------------------
	m_TB_open->setIcon(QIcon(":/icons/ico/open.png"));
	m_TB_iconList->setHeight(24);

	m_folderModel = new QStandardItemModel(0, 2, this);
	m_folderModel->setObjectName(QString::fromUtf8("m_folderModel"));
	m_TV_folder->setModel(m_folderModel);
	QHeaderView* t_HorizontalHeader = m_TV_folder->horizontalHeader();
	t_HorizontalHeader->resizeSection(0, 20);


	m_NotifyIconMap = new QNotifyIconMap(this);
	m_NotifyIconMap->init();
	//--------------------------------------------
	connect(m_TB_delete, SIGNAL(released()), this, SLOT(onDeleteReleased_slot()));
	connect(m_TB_add, SIGNAL(released()), this, SLOT(onAddReleased_slot()));
	connect(m_TB_modify, SIGNAL(released()), this, SLOT(onModifyReleased_slot()));
	connect(m_TB_applyFilter, SIGNAL(released()), this, SLOT(onApplyFilterReleased_slot()));
	connect(m_TB_applyFolder, SIGNAL(released()), this, SLOT(onApplyFolderReleased_slot()));
	connect(m_TB_applyIconOrder, SIGNAL(released()), this, SLOT(onApplyIconOrderReleased_slot()));
	connect(m_TB_icon, SIGNAL(released()), this, SLOT(onIconReleased_slot()));
	connect(m_TB_open, SIGNAL(released()), this, SLOT(onOpenReleased_slot()));
	connect(m_CB_startup, SIGNAL(stateChanged(int)), this, SLOT(onStartupStateChanged(int)));
	connect(m_TV_folder->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onCurrentFolderChanged_slot(const QModelIndex&, const QModelIndex&)));
	connect(m_TB_iconList, SIGNAL(moveIcon_signal(int, int)), this, SLOT(onTrayIconMove_slot(int, int)));
	connect(m_TB_iconList, SIGNAL(deleteIcon_signal(int)), this, SLOT(onTrayIconDelete_slot(int)));
}
void QTrayToolSetupDialog::RetranslateUi(void)
{
	setWindowTitle(tr("SetupDialog"));
	m_TB_delete->setText(tr("delete"));
	m_TB_add->setText(tr("add"));
	m_TB_modify->setText(tr("modify"));
	m_LB_nameFilters->setText(tr("nameFilters"));
	m_TB_applyFilter->setText(tr("apply"));
	m_TB_applyFolder->setText(tr("apply"));
	m_TB_applyIconOrder->setText(tr("apply"));
	m_CB_startup->setText(tr("AutoStartup"));
	m_folderModel->setHeaderData(0, Qt::Horizontal, tr("Icon"), Qt::DisplayRole);
	m_folderModel->setHeaderData(1, Qt::Horizontal, tr("Path"), Qt::DisplayRole);
}
PathCheck QTrayToolSetupDialog::checkPath(const QString& _Path)
{
	if (!QFileInfo(_Path).isDir())
		return CheckInvalid;
	if (m_paths.contains(_Path))
		return CheckRepeat;
	return CheckSuccess;
}
void QTrayToolSetupDialog::clean(void)
{
	m_folderModel->removeRows(0, m_folderModel->rowCount());
	m_LE_path->setText("");
	m_LE_path->setToolTip("");
	m_paths.clear();
}
void QTrayToolSetupDialog::setFilters(const QStringList& _filters)
{
	m_LE_fileFilters->setText(_filters.join(";"));
}
void QTrayToolSetupDialog::addFolder(const QString& _folder, const QPixmap& _icon)
{
	int t_RowCount = m_folderModel->rowCount();
	m_folderModel->insertRow(t_RowCount);
	QStandardItem* t_item = m_folderModel->itemFromIndex(m_folderModel->index(t_RowCount, 0));
	t_item->setIcon(_icon);
	m_folderModel->setData(m_folderModel->index(t_RowCount, 0), _icon, Qt::UserRole);
	m_folderModel->setData(m_folderModel->index(t_RowCount, 1), _folder, Qt::DisplayRole);
	m_folderModel->setData(m_folderModel->index(t_RowCount, 1), _folder, Qt::UserRole);
	m_paths.insert(_folder);
}
void QTrayToolSetupDialog::isAutoStartup(bool _b)
{
	m_CB_startup->setChecked(_b);
}
QByteArray QSettingReadBinary(const QSettings& _settings, const QString& _key)
{
	QString t_sTmp = _settings.value(_key, QString()).toString();
	return QByteArray((const char*)t_sTmp.utf16(), t_sTmp.length() * 2);
}
void optimizationPixmap(QPixmap& _pixmap)
{
	//有的系统图像只有几条白线，实在看不清楚，所以把这种图像的白线变成黑线
	bool t_onlyWhite = true;
	QImage t_img = _pixmap.toImage().convertToFormat(QImage::Format_ARGB32); // 转换为ARGB格式以支持透明度
	int t_min = qMin(t_img.width(), t_img.height());
	for (int i = 0; i < t_min; ++i) {
		QRgb t_pixel = t_img.pixel(i, i) & 0xFFFFFF;	//取颜色，去掉透明
		if (t_pixel > 0 && t_pixel < 0xFFFFFF)
		{
			t_onlyWhite = false;
			break;
		}
	}
	if (t_onlyWhite)
	{
		QPainter t_painter(&_pixmap);
		t_painter.setCompositionMode(QPainter::CompositionMode_SourceIn); // 只影响图像的透明部分
		t_painter.fillRect(_pixmap.rect(), Qt::black);
	}
}
void QTrayToolSetupDialog::refreshAllTrayIcon(void)
{
	m_TB_iconList->clear();
	m_ulDeletedOrderList.clear();
	m_ulOrderList.clear();

	QString t_appPath = QCoreApplication::applicationFilePath();
	QSettings t_sNotifyIcon("HKEY_CURRENT_USER\\Control Panel\\NotifyIconSettings\\", QSettings::Registry64Format);
	QByteArray t_baOrderList = QSettingReadBinary(t_sNotifyIcon, "UIOrderList");
	for (int i = 0; i < t_baOrderList.size() / 8; ++i)
		m_ulOrderList.append(*reinterpret_cast<quint64*>(t_baOrderList.data() + i * 8));

	struct TTrayInfo
	{
		QPixmap pixmap;
		QString path;
		TTrayInfo() {}
	};
	QMap<quint64, TTrayInfo> t_ulTrayIcons;
	QStringList t_subNotifys = t_sNotifyIcon.childGroups();
	for (QString& t_subNotifys : t_subNotifys)
	{
		t_sNotifyIcon.beginGroup(t_subNotifys);
		QByteArray t_baIcon = QSettingReadBinary(t_sNotifyIcon, "IconSnapshot");
		TTrayInfo t_trayInfo;
		if (t_baIcon.length() > 0)
			t_trayInfo.pixmap.loadFromData(t_baIcon, "PNG");
		else
			//加载到的图像有问题，不是系统托盘显示的那个，但是不知道怎么找到正确的那个
			t_trayInfo.pixmap = m_NotifyIconMap->getNotifyIcon(t_sNotifyIcon.value("IconGuid", QString()).toString());
		//t_trayInfo.pixmap = getPixmapFromGUID(t_sNotifyIcon.value("IconGuid", QString()).toString());
		optimizationPixmap(t_trayInfo.pixmap);
		t_trayInfo.path = t_sNotifyIcon.value("ExecutablePath", QString()).toString();
		t_ulTrayIcons.insert(t_subNotifys.toULongLong(), t_trayInfo);
		t_sNotifyIcon.endGroup();
	}
	for (quint64 _ulIconID : m_ulOrderList)
	{
		if (!t_ulTrayIcons.contains(_ulIconID))
			qDebug() << "unknow id " << _ulIconID;
		QString t_sIconID = QString::number(_ulIconID);
		TTrayInfo& t_TrayInfo = t_ulTrayIcons[_ulIconID];
		QListWidgetItem* t_item = m_TB_iconList->addIcon(t_TrayInfo.pixmap);
		t_item->setToolTip(t_sIconID + ":" + t_TrayInfo.path);
		t_item->setData(Qt::UserRole, _ulIconID);
	}
}
void QTrayToolSetupDialog::onDeleteReleased_slot()
{
	int t_Row = m_TV_folder->currentIndex().row();
	QAbstractItemModel* t_Model = m_TV_folder->model();
	QString t_oldPath = t_Model->data(t_Model->index(t_Row, 1), Qt::DisplayRole).toString();
	t_Model->removeRow(t_Row);
	m_paths.remove(t_oldPath);
}
void QTrayToolSetupDialog::onAddReleased_slot()
{
	int t_RowCount = m_folderModel->rowCount();
	if (t_RowCount == 1 && m_folderModel->data(m_folderModel->index(0, 1), Qt::DisplayRole).toString().isEmpty())
		return onModifyReleased_slot();
	QString t_newPath = m_LE_path->text();
	switch (checkPath(t_newPath))
	{
	case CheckInvalid:
		QMessageBox::warning(this, tr("warning"), tr("folder is Invalid!"));
		return;
	case CheckRepeat:
		QMessageBox::warning(this, tr("warning"), tr("folder is Repeated!"));
		return;
	}
	m_folderModel->insertRow(t_RowCount);
	QStandardItem* t_item = m_folderModel->itemFromIndex(m_folderModel->index(t_RowCount, 0));
	t_item->setIcon(m_iconPixmap);
	m_folderModel->setData(m_folderModel->index(t_RowCount, 0), m_iconPixmap, Qt::UserRole);
	m_folderModel->setData(m_folderModel->index(t_RowCount, 1), t_newPath, Qt::DisplayRole);
	m_folderModel->setData(m_folderModel->index(t_RowCount, 1), t_newPath, Qt::UserRole);
	m_paths.insert(t_newPath);
}
void QTrayToolSetupDialog::onModifyReleased_slot()
{
	QString t_newPath = m_LE_path->text();
	QStandardItemModel* t_Model = reinterpret_cast<QStandardItemModel*>(m_TV_folder->model());
	int t_Row = m_TV_folder->currentIndex().row();
	QString t_oldPath = t_Model->data(t_Model->index(t_Row, 1), Qt::DisplayRole).toString();
	if (t_newPath != t_oldPath)
	{
		switch (checkPath(t_newPath))
		{
		case CheckInvalid:
			QMessageBox::warning(this, tr("warning"), tr("folder is Invalid!"));
			return;
		case CheckRepeat:
			QMessageBox::warning(this, tr("warning"), tr("folder is Repeated!"));
			return;
		}
	}
	QStandardItem* t_item = t_Model->itemFromIndex(t_Model->index(t_Row, 0));
	t_item->setIcon(m_iconPixmap);
	t_Model->setData(t_Model->index(t_Row, 0), m_iconPixmap, Qt::UserRole);
	t_Model->setData(t_Model->index(t_Row, 1), m_LE_path->text(), Qt::DisplayRole);
	m_paths.remove(t_oldPath);
	m_paths.insert(t_newPath);
}
void QTrayToolSetupDialog::onApplyFilterReleased_slot()
{
	QStringList t_filters = m_LE_fileFilters->text().split(QRegExp("[ ;,]"), Qt::SkipEmptyParts);
	emit filtersChanged_signal(t_filters);
}
void QTrayToolSetupDialog::onApplyFolderReleased_slot(void)
{
	QMap<QString, TFolderInfo> t_folders;
	for (int i = 0; i < m_folderModel->rowCount(); ++i)
	{
		QPixmap t_icon = m_folderModel->data(m_folderModel->index(i, 0), Qt::UserRole).value<QPixmap>();
		QString t_newPath = m_folderModel->data(m_folderModel->index(i, 1), Qt::DisplayRole).toString();
		QString t_OldPath = m_folderModel->data(m_folderModel->index(i, 1), Qt::UserRole).toString();
		t_folders.insert(t_OldPath, TFolderInfo(t_newPath, t_icon));
		m_folderModel->setData(m_folderModel->index(i, 1), t_newPath, Qt::UserRole);
	}
	emit foldersChanged_signal(t_folders);
}
void QTrayToolSetupDialog::onApplyIconOrderReleased_slot(void)
{
	QByteArray t_baOrderList(m_ulOrderList.size() * 8, '\0');
	for (int i = 0; i < m_ulOrderList.size(); ++i)
	{
		*reinterpret_cast<quint64*>(t_baOrderList.data() + i * 8) = m_ulOrderList[i];
	}
	regWriteBinary("Control Panel\\NotifyIconSettings\\", "UIOrderList", t_baOrderList);

	for (quint64 t_id : m_ulDeletedOrderList)
	{
		QSettings t_sNotifyIcon("HKEY_CURRENT_USER\\Control Panel\\NotifyIconSettings\\", QSettings::Registry64Format);
		t_sNotifyIcon.remove(QString::number(t_id));
	}
	QMessageBox::information(this, tr("information"), tr("need to restart the system to apply modifications!"));
}
static QString imageFilter()
{
	QList<QByteArray> supportedImageFormats = QImageReader::supportedImageFormats();
	QStringList t_filters;
	for (QByteArray& t_format : supportedImageFormats)
		t_filters.append(QString("*.%1").arg(t_format.constData()).toLower());
	if (t_filters.contains("*.jepg"))
		t_filters.append("*.jpg");
	return t_filters.join(" ");
}
void QTrayToolSetupDialog::onIconReleased_slot()
{
	QString t_filePath = QFileDialog::getOpenFileName(this, tr("Choose a Pixmap"), ".", tr("All Pixmaps (%1)").arg(imageFilter()));
	if (!t_filePath.isEmpty())
	{
		m_iconPixmap = QPixmap::fromImage(QImageReader(t_filePath).read().scaled(48, 48));
		m_TB_icon->setIcon(m_iconPixmap);
	}
}
void QTrayToolSetupDialog::onOpenReleased_slot()
{
	QString t_dirPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, "Choose a Folder ", QCoreApplication::applicationDirPath(), QFileDialog::ShowDirsOnly));
	if (!t_dirPath.isEmpty())
	{
		m_LE_path->setText(t_dirPath);
		m_LE_path->setToolTip(m_LE_path->text());
	}
}
void QTrayToolSetupDialog::onStartupStateChanged(int _status)
{
	emit setAutoStartup_signal(_status != Qt::Unchecked);
}
void QTrayToolSetupDialog::onCurrentFolderChanged_slot(const QModelIndex& _Current, const QModelIndex& _Previous)
{
	int t_Row = _Current.row();
	QAbstractItemModel* t_Model = m_TV_folder->model();
	m_iconPixmap = t_Model->data(t_Model->index(t_Row, 0), Qt::UserRole).value<QPixmap>();
	m_TB_icon->setIcon(m_iconPixmap);
	m_LE_path->setText(t_Model->data(t_Model->index(t_Row, 1), Qt::DisplayRole).toString());
	m_LE_path->setToolTip(m_LE_path->text());
	m_LE_path->setCursorPosition(0);
}
void QTrayToolSetupDialog::onTrayIconMove_slot(int _moveidx, int _beforeidx)
{
	quint64 t_OrderID = m_ulOrderList[_moveidx];
	m_ulOrderList.removeOne(t_OrderID);
	m_ulOrderList.insert(_moveidx < _beforeidx ? _beforeidx - 1 : _beforeidx, t_OrderID);
}
void QTrayToolSetupDialog::onTrayIconDelete_slot(int _idx)
{
	quint64 t_OrderID = m_ulOrderList[_idx];
	m_ulOrderList.removeOne(t_OrderID);
	m_ulDeletedOrderList.append(t_OrderID);
}
