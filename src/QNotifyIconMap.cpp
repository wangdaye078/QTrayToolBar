#include "QNotifyIconMap.h"
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector>
#include <QtWin>
#include <windows.h>
#include "basicDefine.h"

QNotifyIconMap::QNotifyIconMap(QObject* _parent)
	: QObject(_parent)
{
}
QNotifyIconMap::~QNotifyIconMap()
{
}
QPixmap QNotifyIconMap::getNotifyIcon(const QString& _guid)
{
	if (m_iconMap.contains(_guid))
		return m_iconMap[_guid];
	return getPixmapFromGUID(_guid);
}
void QNotifyIconMap::init(void)
{
	QFile t_jsonFile(QCoreApplication::applicationDirPath() + "/setting/NotifyIcon.json");
	if (!t_jsonFile.open(QIODevice::ReadOnly))
		return;
	QByteArray t_jsonData = t_jsonFile.readAll();
	QJsonArray t_items = QJsonDocument::fromJson(t_jsonData).array();
	for (int i = 0; i < t_items.size(); ++i) {
		QJsonObject t_item = t_items[i].toObject();
		QString t_guid = t_item.value("guid").toString();
		QString t_module = t_item.value("module").toString();
		int t_iconId = t_item.value("id").toInt();

		HICON t_hiconLarge;
		if (ExtractIconExW(QStringToWCHAR(t_module), -t_iconId, &t_hiconLarge, NULL, 1) != 1)
			continue;
		QPixmap t_icon = QtWin::fromHICON(t_hiconLarge);
		m_iconMap.insert(t_guid, t_icon);
		DestroyIcon(t_hiconLarge);
	}
	t_jsonFile.close();
}
