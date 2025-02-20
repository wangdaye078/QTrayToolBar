#include "lnkFileReader.h"
#include <QFileInfo>
#include <QFile>
#include "basicDefine.h"
#include <windows.h>

const wchar_t* MuiStringKey[] = { L"", L"LocalizedString" };

WCharArray::WCharArray(qsizetype _size, wchar_t _ch) :
	size(_size), pbuff(NULL)
{
	if (_size > 0)
		pbuff = new WCHAR[_size];
	for (qsizetype i = 0; i < size; ++i)
		pbuff[i] = _ch;
}
WCharArray::~WCharArray(void)
{
	if (pbuff != NULL)
		delete[] pbuff;
}
wchar_t* WCharArray::data(void)
{
	return pbuff;
}

lnkFileReader::lnkFileReader(QObject* parent, const QString& _path)
	: QObject(parent)
{
	if (!_path.isEmpty())
	{
		m_path = _path;
		read(m_path);
	}
}

lnkFileReader::~lnkFileReader()
{
}

lnkFileReader& lnkFileReader::read(const QString& _path)
{
	QFile t_file(_path);
	if (!t_file.open(QFile::ReadOnly))
		return *this;
	QByteArray t_buff = t_file.readAll();
	if (t_buff.size() <= sizeof(ShellLinkHeader))
		return *this;
	const char* t_pBuff = t_buff.constData();
	const ShellLinkHeader* t_pHead = reinterpret_cast<const ShellLinkHeader*>(t_pBuff);
	if (t_pHead->HeaderSize != sizeof(ShellLinkHeader))
		return *this;
	if (ReadGUID(t_pHead->LinkCLSID) != QString("{00021401-0000-0000-C000-000000000046}"))
		return *this;
	m_head = *t_pHead;
	if (!(m_head.sLinkFlags.HasLinkTargetIDList && !m_head.sLinkFlags.HasLinkInfo && m_head.sFileAttributes.UFileAttributes == 0))
		return *this;
	t_pBuff += m_head.HeaderSize;
	if (*reinterpret_cast<const qint16*>(t_pBuff) != 0x16 || *reinterpret_cast<const qint8*>(t_pBuff + 4) != 0x1F)
		return *this;
	QString t_guid = ReadGUID(*reinterpret_cast<const BGUID*>(t_pBuff + 6));
	QString t_subkeyPath = QString("SOFTWARE\\Classes\\CLSID\\%1").arg(t_guid);
	HKEY t_hKey = 0;
	LSTATUS t_status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, QStringToWCHAR(t_subkeyPath), 0, KEY_READ, &t_hKey);
	if (t_status != ERROR_SUCCESS)
		return *this;
	for (int i = 0; i < _countof(MuiStringKey); ++i)
	{
		DWORD t_size = 0;
		t_status = RegLoadMUIString(t_hKey, MuiStringKey[i], NULL, 0, &t_size, 0, NULL);
		if (t_status == ERROR_INVALID_DATA)
			continue;
		if (t_status != ERROR_MORE_DATA)
			break;
		WCharArray t_lptStr(t_size + 1, 0);
		t_status = RegLoadMUIString(t_hKey, MuiStringKey[i], t_lptStr.data(), (t_size + 1) * 2, &t_size, 0, NULL);
		if (t_status != ERROR_SUCCESS)
			break;
		m_name = WCHARToQString(t_lptStr.data());
		break;
	}
	RegCloseKey(t_hKey);
	return *this;
}
const QString& lnkFileReader::getName(void)
{
	if (m_name.isEmpty())
		m_name = QFileInfo(m_path).completeBaseName();
	return m_name;
}
QString lnkFileReader::ReadGUID(const BGUID& _id)
{
	return QString::asprintf("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		_id[3], _id[2], _id[1], _id[0], _id[5], _id[4], _id[7], _id[6], _id[8], _id[9], _id[10], _id[11],
		_id[12], _id[13], _id[14], _id[15]);
}
