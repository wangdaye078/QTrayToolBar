//********************************************************************
//	filename: 	F:\mygit\QIconBar\QIconBar.h
//	desc:		
//
//	created:	hying 25:2:2025   10:13
//********************************************************************
#ifndef QIconBar_h__
#define QIconBar_h__

#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>

class QToolButton;
class QResizeEvent;
class QKeyEvent;

static const char* const tb_arrow_left_xpm[] = {
	"8 5 3 1",
	"            c None",
	".            c #000000",
	"+            c none",
	"..++..++",
	"+..++..+",
	"++..++..",
	"+..++..+",
	"..++..++",
};
static const char* const tb_arrow_right_xpm[] = {
	"8 5 3 1",
	"            c None",
	".            c #000000",
	"+            c none",
	"++..++..",
	"+..++..+",
	"..++..++",
	"+..++..+",
	"++..++..",
};
class QDraggableListWidget : public QListWidget
{
	Q_OBJECT
public:
	QDraggableListWidget(QWidget* parent = nullptr);
	~QDraggableListWidget();
protected:
	virtual void paintEvent(QPaintEvent* _event);
	virtual void mouseMoveEvent(QMouseEvent* _event);
	virtual void mousePressEvent(QMouseEvent* _event);
	virtual void mouseReleaseEvent(QMouseEvent* _event);
	virtual void keyPressEvent(QKeyEvent* _event);
private:
	void updateItemLeftLine(QListWidgetItem* _Item);
signals:
	//把_move移动到_before之前
	void moveItem_signal(int _moveidx, int _beforeidx);
	void deleteItem_signal(int _idx);
private:
	bool m_MouseDown;
	bool m_MouseMove;
	QListWidgetItem* m_activeItem;
	QListWidgetItem* m_activeItemNext;	//m_activeAction下面的一个
	QListWidgetItem* m_previousItem;
};
class QIconBar : public QWidget
{
	Q_OBJECT
public:
	QIconBar(QWidget* parent = nullptr);
	~QIconBar();
	void setHeight(int _h);
	QListWidgetItem* addIcon(const QPixmap& _pix);
	void clear(void);
	void deleteIcon(int _idx);
private:
	void CreateControl(void);
	void checkShowExtensionButton(void);
protected:
	virtual void resizeEvent(QResizeEvent* _event);
private slots:
	void scrollIcon_slot(void);
	void iconListMoveItem_slot(int _moveidx, int _beforeidx);
	void iconListDeleteItem_slot(int _idx);
signals:
	//把_move移动到_before之前
	void moveIcon_signal(int _moveidx, int _beforeidx);
	void deleteIcon_signal(int _idx);
private:
	QToolButton* m_TB_left;
	QToolButton* m_TB_right;
	QDraggableListWidget* m_LW_IconList;
};
#endif // QIconBar_h__
