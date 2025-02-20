//********************************************************************
//	filename: 	F:\mygit\QTrayToolbar\src\QDraggableToolBar.h
//	desc:		
//
//	created:	wangdaye 19:2:2025   23:58
//********************************************************************
#ifndef QDraggableToolBar_h__
#define QDraggableToolBar_h__

#include <QToolBar>
#include <QToolButton>
#include <QPoint>
#include <QMouseEvent>

class QPaintEvent;

class QMoveAbleToolButton : public QToolButton
{
	Q_OBJECT
public:
	QMoveAbleToolButton(QWidget* _parent = NULL) :
		QToolButton(_parent) {
		setIconSize(QSize(16, 16));
	};
	~QMoveAbleToolButton() {
	};
protected:
	virtual void mouseMoveEvent(QMouseEvent* _event)
	{
		_event->ignore();
	}
	virtual void mousePressEvent(QMouseEvent* _event)
	{
		_event->ignore();
	}
	virtual void mouseReleaseEvent(QMouseEvent* _event)
	{
		_event->ignore();
	}
};
class QDraggableToolBar : public QToolBar
{
	Q_OBJECT
public:
	QDraggableToolBar(QWidget* _parent);
	~QDraggableToolBar();
protected:
	virtual void paintEvent(QPaintEvent* _event);
	virtual void mouseMoveEvent(QMouseEvent* _event);
	virtual void mousePressEvent(QMouseEvent* _event);
	virtual void mouseReleaseEvent(QMouseEvent* _event);
private:
	void updateActionLeftLine(QAction* _Action);
signals:
	//把_move移动到_before之前
	void moveAction_signal(QAction* _move, QAction* _before);
private:
	bool m_MouseDown;
	bool m_MouseMove;
	QAction* m_activeAction;
	QAction* m_activeActionNext;	//m_activeAction下面的一个
	QAction* m_previousAction;
};


#endif // QDraggableToolBar_h__
