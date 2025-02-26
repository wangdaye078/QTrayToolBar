#include "QIconBar.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QListWidgetItem>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QtDebug>

QDraggableListWidget::QDraggableListWidget(QWidget* _parent) :
	QListWidget(_parent), m_MouseDown(false), m_MouseMove(false), m_activeItem(NULL),
	m_activeItemNext(NULL), m_previousItem(NULL)
{
}
QDraggableListWidget::~QDraggableListWidget()
{

}
void QDraggableListWidget::paintEvent(QPaintEvent* _event)
{
	QListWidget::paintEvent(_event);
	if (m_MouseMove && m_previousItem != NULL)
	{
		QWidget* t_viewport = this->viewport();
		QPainter p(t_viewport);
		QPen t_pen(QColor("black"));
		t_pen.setWidth(2);
		p.setPen(t_pen);
		p.setBrush(QColor("black"));
		int t_x = visualItemRect(m_previousItem).left();
		p.drawLine(QPointF(t_x, 0), QPointF(t_x, t_viewport->height()));

		QPoint t_triangle[3];
		t_triangle[0] = QPoint(t_x - 2, 0);
		t_triangle[1] = QPoint(t_x, 2);
		t_triangle[2] = QPoint(t_x + 2, 0);
		p.drawPolygon(t_triangle, 3);
		t_triangle[0] = QPoint(t_x - 2, t_viewport->height());
		t_triangle[1] = QPoint(t_x, t_viewport->height() - 2);
		t_triangle[2] = QPoint(t_x + 2, t_viewport->height());
		p.drawPolygon(t_triangle, 3);
		_event->accept();
	}
}
void QDraggableListWidget::mouseMoveEvent(QMouseEvent* _event)
{
	QListWidgetItem* t_activeItem = itemAt(_event->pos());
	if (m_MouseDown)
	{
		if (t_activeItem != m_activeItem)
			m_MouseMove = true;
	}
	if (m_MouseMove)
	{
		QRect t_rect = visualItemRect(t_activeItem);
		QPoint t_pos = _event->pos();
		QListWidgetItem* t_previousItem;
		if (t_pos.x() < t_rect.left() + t_rect.width() / 2)
			t_previousItem = t_activeItem;
		else
			t_previousItem = itemAt(t_pos + QPoint(t_rect.width(), 0));
		if (m_previousItem != t_previousItem)
		{
			QListWidgetItem* t_tmpItem = m_previousItem;
			m_previousItem = t_previousItem;
			updateItemLeftLine(m_previousItem);
			updateItemLeftLine(t_tmpItem);
			setCursor(m_previousItem == m_activeItem || m_previousItem == m_activeItemNext ? Qt::ForbiddenCursor : Qt::ArrowCursor);
		}
		return _event->accept();
	}
	QListWidget::mouseMoveEvent(_event);
}
void QDraggableListWidget::mousePressEvent(QMouseEvent* _event)
{
	if (_event->button() != Qt::LeftButton)
		return QListWidget::mousePressEvent(_event);

	m_activeItem = itemAt(_event->pos());
	if (m_activeItem != NULL)
	{
		QRect t_rect = visualItemRect(m_activeItem);
		m_activeItemNext = itemAt(_event->pos() + QPoint(t_rect.width(), 0));
		m_previousItem = NULL;
		m_MouseDown = true;
		m_MouseMove = false;
		return _event->accept();
	}
	QListWidget::mousePressEvent(_event);
}
void QDraggableListWidget::mouseReleaseEvent(QMouseEvent* _event)
{
	if (_event->button() != Qt::LeftButton)
		return QListWidget::mousePressEvent(_event);

	m_MouseDown = false;
	setCursor(Qt::ArrowCursor);
	bool t_MouseMove = m_MouseMove;
	m_MouseMove = false;
	if (m_activeItem != NULL)
	{
		if (t_MouseMove)
		{
			//在m_previousAction的左面插入
			if (m_previousItem != NULL && m_previousItem != m_activeItem && m_previousItem != m_activeItemNext)
				emit moveItem_signal(row(m_activeItem), row(m_previousItem));
		}
		updateItemLeftLine(m_previousItem);
		m_previousItem = NULL;
		m_activeItemNext = NULL;
		m_MouseMove = false;
		return _event->accept();
	}

	QListWidget::mousePressEvent(_event);
}
void QDraggableListWidget::keyPressEvent(QKeyEvent* _event)
{
	QListWidget::keyPressEvent(_event);
	if (_event == QKeySequence::Delete)
	{
		QPoint t_globalPos = QCursor::pos();
		QPoint t_widgetPos = mapFromGlobal(t_globalPos);
		QModelIndex t_idx = indexAt(t_widgetPos);

		if (t_idx.row() >= 0)
			emit deleteItem_signal(t_idx.row());
	}
}
void QDraggableListWidget::updateItemLeftLine(QListWidgetItem* _Item)
{
	if (_Item == NULL)
		return;
	QRect t_rect = visualItemRect(_Item);
	t_rect.setLeft(t_rect.left() - 3);
	t_rect.setWidth(7);
	t_rect.setTop(0);
	t_rect.setHeight(height());
	this->viewport()->update(t_rect);
}

QIconBar::QIconBar(QWidget* parent)
	: QWidget(parent)
{
	CreateControl();
}

QIconBar::~QIconBar()
{
}
void QIconBar::CreateControl(void)
{
	QHBoxLayout* t_mainLayout = new QHBoxLayout(this);
	t_mainLayout->setSpacing(0);
	t_mainLayout->setObjectName(QString::fromUtf8("t_mainLayout"));
	t_mainLayout->setContentsMargins(0, 0, 0, 0);

	m_TB_left = new QToolButton(this);
	m_TB_left->setObjectName(QString::fromUtf8("m_TB_left"));
	m_TB_left->setAutoRaise(true);
	m_TB_left->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	m_TB_left->setMaximumSize(QSize(14, 16777215));
	m_TB_left->setAutoRepeat(true);
	t_mainLayout->addWidget(m_TB_left);

	m_LW_IconList = new QDraggableListWidget(this);
	m_LW_IconList->setObjectName(QString::fromUtf8("m_LW_IconList"));
	m_LW_IconList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_LW_IconList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_LW_IconList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_LW_IconList->setFlow(QListView::LeftToRight);
	m_LW_IconList->setViewMode(QListView::ListMode);
	m_LW_IconList->setSelectionMode(QAbstractItemView::SingleSelection);
	m_LW_IconList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	t_mainLayout->addWidget(m_LW_IconList);

	m_TB_right = new QToolButton(this);
	m_TB_right->setObjectName(QString::fromUtf8("m_TB_right"));
	m_TB_right->setAutoRaise(true);
	m_TB_right->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	m_TB_right->setMaximumSize(QSize(14, 16777215));
	m_TB_right->setAutoRepeat(true);
	t_mainLayout->addWidget(m_TB_right);

	//------------------------------------------------------
	m_TB_left->setIcon(QPixmap::fromImage(QImage(tb_arrow_left_xpm).convertToFormat(QImage::Format_ARGB32).mirrored(true, false)));
	m_TB_left->setProperty("inc", -1);
	m_TB_right->setIcon(QPixmap::fromImage(QImage(tb_arrow_right_xpm).convertToFormat(QImage::Format_ARGB32).mirrored(true, false)));
	m_TB_right->setProperty("inc", 1);
	connect(m_TB_left, SIGNAL(released()), this, SLOT(scrollIcon_slot()));
	connect(m_TB_right, SIGNAL(released()), this, SLOT(scrollIcon_slot()));
	connect(m_LW_IconList, SIGNAL(moveItem_signal(int, int)), this, SLOT(iconListMoveItem_slot(int, int)));
	connect(m_LW_IconList, SIGNAL(deleteItem_signal(int)), this, SLOT(iconListDeleteItem_slot(int)));
}
void QIconBar::setHeight(int _h)
{
	m_LW_IconList->setFixedHeight(_h);
	setFixedHeight(_h);
}
QListWidgetItem* QIconBar::addIcon(const QPixmap& _pix)
{
	QListWidgetItem* t_qlistwidgetitem = new QListWidgetItem(m_LW_IconList);
	t_qlistwidgetitem->setIcon(QIcon(_pix));
	checkShowExtensionButton();
	return t_qlistwidgetitem;
}
void QIconBar::clear(void)
{
	m_LW_IconList->clear();
	checkShowExtensionButton();
}
void QIconBar::deleteIcon(int _idx)
{
	delete m_LW_IconList->takeItem(_idx);
	checkShowExtensionButton();
}
void QIconBar::checkShowExtensionButton(void)
{
	QModelIndex t_idx = m_LW_IconList->indexAt(QPoint(0, 0));
	bool t_show = t_idx.isValid() && t_idx.row() != 0;
	m_TB_left->setVisible(t_show);
	if (!t_show)
		m_TB_left->setDown(false);

	t_idx = m_LW_IconList->indexAt(m_LW_IconList->viewport()->rect().bottomRight());
	t_show = t_idx.isValid();
	m_TB_right->setVisible(t_show);
	if (!t_show)
		m_TB_right->setDown(false);
}
void QIconBar::resizeEvent(QResizeEvent* _event)
{
	QWidget::resizeEvent(_event);
	checkShowExtensionButton();
}
void QIconBar::scrollIcon_slot(void)
{
	QObject* t_Sender = sender();
	int t_scroll = t_Sender->property("inc").toInt();
	QModelIndex t_idx = m_LW_IconList->indexAt(QPoint(0, 0));
	QListWidgetItem* t_item = m_LW_IconList->item(t_idx.row() + t_scroll);
	m_LW_IconList->scrollToItem(t_item, QAbstractItemView::PositionAtTop);
	checkShowExtensionButton();
}
void QIconBar::iconListMoveItem_slot(int _moveidx, int _beforeidx)
{
	emit moveIcon_signal(_moveidx, _beforeidx);
	QListWidgetItem* t_item = m_LW_IconList->takeItem(_moveidx);
	m_LW_IconList->insertItem(_moveidx < _beforeidx ? _beforeidx - 1 : _beforeidx, t_item);
}
void QIconBar::iconListDeleteItem_slot(int _idx)
{
	emit deleteIcon_signal(_idx);
	deleteIcon(_idx);
}
