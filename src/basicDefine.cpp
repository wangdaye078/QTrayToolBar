#include "basicDefine.h"
#include <QtWin>
#include <QtDebug>
#include <QUuid>
#include <windows.h>
#include <commctrl.h>
#include <Setupapi.h>

QPixmap getPixmapFromGUID(const QString& _guid)
{
	//��Щͼ�궼�Ǵ�taskbarcpl.dll�ﵼ���ġ�
	/*
	if (_guid == "{7820AE73-23E3-4229-82C1-E41CB67D5B9C}")
		return QPixmap(":/icons/ico/sound.png");
	if (_guid == "{7820AE74-23E3-4229-82C1-E41CB67D5B9C}")
		return QPixmap(":/icons/ico/network.png");
	if (_guid == "{7820AE77-23E3-4229-82C1-E41CB67D5B9C}")
		return QPixmap(":/icons/ico/locate.png");*/
	QPixmap t_pixmap;
	GUID t_guid = GUID(QUuid(_guid));
	HICON t_hicon;
	int t_MiniIconIndex;
	if (SetupDiLoadClassIcon(&t_guid, &t_hicon, &t_MiniIconIndex))
	{
		t_pixmap = QtWin::fromHICON(t_hicon);
		DestroyIcon(t_hicon);
	}
	return t_pixmap;
}
QPixmap getPixmapFromFile(const QString& _path)
{
	SHFILEINFOW shfi;
	UINT_PTR dwRet = SHGetFileInfo(QStringToWCHAR(_path), 0, &shfi, sizeof(shfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
	if (dwRet == 0)
		return QPixmap();
	HICON t_hicon = ImageList_GetIcon(HIMAGELIST(dwRet), shfi.iIcon, ILD_NORMAL);
	QPixmap t_pix = QtWin::fromHICON(t_hicon);
	DestroyIcon(t_hicon);
	return t_pix;
}
QRect getToolbarRect(void)
{
	RECT t_taskbarRect;
	GetWindowRect(FindWindow(L"Shell_TrayWnd", NULL), &t_taskbarRect);
	return QRect(QPoint(t_taskbarRect.left, t_taskbarRect.top), QPoint(t_taskbarRect.right, t_taskbarRect.bottom));
}
void regWriteBinary(const QString& _subkey, const QString& _valueName, const QByteArray& _value)
{
	HKEY t_hKey;
	if (LONG t_result = RegOpenKeyEx(HKEY_CURRENT_USER, QStringToWCHAR(_subkey), 0, KEY_SET_VALUE, &t_hKey) != ERROR_SUCCESS)
	{
		qDebug() << "tray icon move leftmost open key error. " << t_result;
		return;
	}
	if (LONG t_result = RegSetValueEx(t_hKey, QStringToWCHAR(_valueName), 0, REG_BINARY, reinterpret_cast<const BYTE*>(_value.constData()), _value.size()) != ERROR_SUCCESS)
		qDebug() << "Failed to set registry value. tray icon move leftmost failed." << t_result;
	RegCloseKey(t_hKey);
}
