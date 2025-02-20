//********************************************************************
//	filename: 	F:\mygit\QTrayToolbar\src\QTrayToolbar.h
//	desc:		
//
//	created:	hying 15:2:2025   14:55
//********************************************************************
#ifndef QTrayToolbar_h__
#define QTrayToolbar_h__
#include <QtWidgets/QMenu>
class QListWidget;
class TLnkFile;

class QTrayToolMenu : public QMenu
{
	Q_OBJECT
public:
	QTrayToolMenu(QWidget* _parent = nullptr);
	~QTrayToolMenu();
	void setLnkFile(TLnkFile* _LnkFile);
protected:
	virtual void paintEvent(QPaintEvent* _event);
	virtual void mouseMoveEvent(QMouseEvent* _event);
	virtual void mousePressEvent(QMouseEvent* _event);
	virtual void mouseReleaseEvent(QMouseEvent* _event);
	virtual void hideEvent(QHideEvent* event);
private:
	void updateActionTopLine(QAction* _Action);
	void moveLnkFile(const QString& _movePath, const QString& _beforePath);
signals:
	void LnkFileChanged_signal(void);
private:
	TLnkFile* m_LnkFile;
	bool m_MouseDown;
	bool m_MouseMove;
	QAction* m_activeAction;
	QAction* m_activeActionNext;	//m_activeAction下面的一个
	QAction* m_previousAction;
};
#endif // QTrayToolbar_h__
