#include "QDraggableToolBar.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtDebug>

QDraggableToolBar::QDraggableToolBar(QWidget* _parent)
	: QToolBar(_parent)
{
	//setMovable(false);
	//setFloatable(false);
}

QDraggableToolBar::~QDraggableToolBar()
{
}
void QDraggableToolBar::paintEvent(QPaintEvent* _event)
{
	QToolBar::paintEvent(_event);
	if (m_MouseMove && m_previousAction != NULL)
	{
		QPainter p(this);
		QPen t_pen(QColor("black"));
		t_pen.setWidth(2);
		p.setPen(t_pen);
		p.setBrush(QColor("black"));
		int t_x = actionGeometry(m_previousAction).left();
		p.drawLine(QPointF(t_x, 0), QPointF(t_x, height()));
		QPoint t_triangle[3];
		t_triangle[0] = QPoint(t_x - 2, 0);
		t_triangle[1] = QPoint(t_x, 2);
		t_triangle[2] = QPoint(t_x + 2, 0);
		p.drawPolygon(t_triangle, 3);
		t_triangle[0] = QPoint(t_x - 2, height());
		t_triangle[1] = QPoint(t_x, height() - 2);
		t_triangle[2] = QPoint(t_x + 2, height());
		p.drawPolygon(t_triangle, 3);
	}
}
void QDraggableToolBar::mouseMoveEvent(QMouseEvent* _event)
{
	QAction* t_activeAction = actionAt(_event->pos());
	if (m_MouseDown)
	{
		if (t_activeAction != m_activeAction)
			m_MouseMove = true;
	}
	if (m_MouseMove)
	{
		QRect t_rect = actionGeometry(t_activeAction);
		QPoint t_pos = _event->pos();
		QAction* t_previousAction;
		if (t_pos.x() < t_rect.left() + t_rect.width() / 2)
			t_previousAction = t_activeAction;
		else
			t_previousAction = actionAt(t_pos + QPoint(t_rect.width(), 0));
		if (m_previousAction != t_previousAction)
		{
			QAction* t_tmpAction = m_previousAction;
			m_previousAction = t_previousAction;
			updateActionLeftLine(m_previousAction);
			updateActionLeftLine(t_tmpAction);
		}
		return _event->accept();
	}
	QToolBar::mouseMoveEvent(_event);

}
void QDraggableToolBar::mousePressEvent(QMouseEvent* _event)
{
	if (_event->button() != Qt::LeftButton)
		return QToolBar::mousePressEvent(_event);

	m_activeAction = actionAt(_event->pos());
	if (m_activeAction != NULL)
	{
		QRect t_rect = actionGeometry(m_activeAction);
		m_activeActionNext = actionAt(_event->pos() + QPoint(0, t_rect.height()));
		m_previousAction = NULL;
		m_MouseDown = true;
		m_MouseMove = false;
	}
	_event->accept();
}
void QDraggableToolBar::mouseReleaseEvent(QMouseEvent* _event)
{
	if (_event->button() != Qt::LeftButton)
		return QToolBar::mouseReleaseEvent(_event);

	m_MouseDown = false;
	bool t_MouseMove = m_MouseMove;
	m_MouseMove = false;
	if (m_activeAction != NULL)
	{
		if (t_MouseMove)
		{
			//在m_previousAction的左面插入
			if (m_previousAction != NULL && m_previousAction != m_activeAction && m_previousAction != m_activeActionNext)
			{
				removeAction(m_activeAction);
				insertAction(m_previousAction, m_activeAction);
				emit moveAction_signal(m_activeAction, m_previousAction);
			}
		}
		updateActionLeftLine(m_previousAction);
		m_previousAction = NULL;
		m_activeActionNext = NULL;
		m_MouseMove = false;
	}
	_event->accept();
}
void QDraggableToolBar::updateActionLeftLine(QAction* _Action)
{
	if (_Action == NULL)
		return;
	QRect t_rect = actionGeometry(_Action);
	t_rect.setLeft(t_rect.left() - 3);
	t_rect.setWidth(7);
	t_rect.setTop(0);
	t_rect.setHeight(height());
	update(t_rect);
}