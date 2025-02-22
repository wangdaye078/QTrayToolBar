//********************************************************************
//	filename: 	F:\mygit\QTrayToolbar\src\QNotifyIconMap.h
//	desc:		
//
//	created:	hying 22:2:2025   12:08
//********************************************************************
#ifndef QNotifyIconMap_h__
#define QNotifyIconMap_h__

#include <QPixmap>
#include <QMap>

class QNotifyIconMap : public QObject
{
	Q_OBJECT
public:
	QNotifyIconMap(QObject* _parent);
	~QNotifyIconMap();
	QPixmap getNotifyIcon(const QString& _guid);
	void init(void);
private:
	QMap<QString, QPixmap> m_iconMap;
};
#endif // QNotifyIconMap_h__
