#include "QTrayToolMenu.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QListWidget>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QMouseEvent>
#include <QtDebug>
#include "struct.h"

QTrayToolMenu::QTrayToolMenu(QWidget* _parent)
	: QMenu(_parent), m_LnkFile(NULL), m_MouseDown(false)
{
	setStyleSheet("QMenu{padding-left: 2px;border:1px solid #bbbbbb;background-color:#ffffff;}\n \
		QMenu::item:selected{background-color:#f9f9f9;color:#000000;}\n \
		QMenu::item{padding: 2px;padding-right: 20px;}");
}

QTrayToolMenu::~QTrayToolMenu()
{
}
void QTrayToolMenu::setLnkFile(TLnkFile* _LnkFile)
{
	m_LnkFile = _LnkFile;
}
void QTrayToolMenu::paintEvent(QPaintEvent* _event)
{
	QMenu::paintEvent(_event);
	if (m_MouseMove && m_previousAction != NULL)
	{
		QPainter p(this);
		QPen t_pen(QColor("black"));
		t_pen.setWidth(2);
		p.setPen(t_pen);
		p.setBrush(QColor("black"));
		int t_y = actionGeometry(m_previousAction).top() - 1;
		p.drawLine(QPointF(0, t_y), QPointF(width(), t_y));
		QPoint t_triangle[3];
		t_triangle[0] = QPoint(0, t_y - 2);
		t_triangle[1] = QPoint(2, t_y);
		t_triangle[2] = QPoint(0, t_y + 2);
		p.drawPolygon(t_triangle, 3);
		t_triangle[0] = QPoint(width(), t_y - 2);
		t_triangle[1] = QPoint(width() - 2, t_y);
		t_triangle[2] = QPoint(width(), t_y + 2);
		p.drawPolygon(t_triangle, 3);
	}
}
void QTrayToolMenu::mouseMoveEvent(QMouseEvent* _event)
{
	QAction* t_activeAction = activeAction();
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
		if (t_pos.y() < t_rect.top() + t_rect.height() / 2)
			t_previousAction = t_activeAction;
		else
			t_previousAction = actionAt(t_pos + QPoint(0, t_rect.height()));
		if (m_previousAction != t_previousAction)
		{
			QAction* t_tmpAction = m_previousAction;
			m_previousAction = t_previousAction;
			updateActionTopLine(m_previousAction);
			updateActionTopLine(t_tmpAction);
			setCursor(m_previousAction == m_activeAction || m_previousAction == m_activeActionNext ? Qt::ForbiddenCursor : Qt::ArrowCursor);
		}
		return _event->accept();
	}
	QMenu::mouseMoveEvent(_event);
}
void QTrayToolMenu::mousePressEvent(QMouseEvent* _event)
{
	if (_event->button() != Qt::LeftButton)
		return QMenu::mousePressEvent(_event);

	m_activeAction = activeAction();
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
void QTrayToolMenu::mouseReleaseEvent(QMouseEvent* _event)
{
	if (_event->button() != Qt::LeftButton)
		return QMenu::mouseReleaseEvent(_event);

	m_MouseDown = false;
	setCursor(Qt::ArrowCursor);
	bool t_MouseMove = m_MouseMove;
	m_MouseMove = false;
	if (m_activeAction != NULL)
	{
		if (!t_MouseMove)
		{
			QString t_path = m_activeAction->property("path").toString();
			QUrl t_url = QUrl::fromLocalFile(t_path);
			QDesktopServices::openUrl(t_url);
		}
		else
		{
			//在m_previousAction的上面插入
			if (m_previousAction != NULL && m_previousAction != m_activeAction && m_previousAction != m_activeActionNext)
			{
				removeAction(m_activeAction);
				insertAction(m_previousAction, m_activeAction);
				moveLnkFile(m_activeAction->property("path").toString(), m_previousAction->property("path").toString());
			}
		}
		updateActionTopLine(m_previousAction);
		m_previousAction = NULL;
		m_activeActionNext = NULL;
		m_MouseMove = false;
	}
	_event->accept();
}
void QTrayToolMenu::hideEvent(QHideEvent* _event)
{
	QMenu::hideEvent(_event);
	m_MouseDown = false;
	m_MouseMove = false;
	m_activeAction = NULL;
	m_previousAction = NULL;
}
void QTrayToolMenu::updateActionTopLine(QAction* _Action)
{
	if (_Action == NULL)
		return;
	QRect t_rect = actionGeometry(_Action);
	t_rect.setTop(t_rect.top() - 3);
	t_rect.setHeight(7);
	t_rect.setLeft(0);
	t_rect.setWidth(width());
	update(t_rect);
}
void QTrayToolMenu::moveLnkFile(const QString& _movePath, const QString& _beforePath)
{
	TLnkFile* t_LnkFile = NULL;
	for (QList<TLnkFile*>::iterator i = m_LnkFile->subLnk.begin(); i != m_LnkFile->subLnk.end(); ++i)
	{
		if ((*i)->path == _movePath)
		{
			t_LnkFile = *i;
			m_LnkFile->subLnk.erase(i);
			break;
		}
	}
	for (QList<TLnkFile*>::iterator i = m_LnkFile->subLnk.begin(); i != m_LnkFile->subLnk.end(); ++i)
	{
		if ((*i)->path == _beforePath)
		{
			m_LnkFile->subLnk.insert(i, t_LnkFile);
			break;
		}
	}
	emit LnkFileChanged_signal();
}
