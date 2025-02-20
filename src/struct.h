//********************************************************************
//	filename: 	F:\mygit\QTrayToolbar\src\struct.h
//	desc:		
//
//	created:	wangdaye 18:2:2025   10:32
//********************************************************************
#ifndef struct_h__
#define struct_h__

#include <QDataStream>
#include <QPixmap>
class TLnkFile
{
public:
	QString name;
	QString path;
	QPixmap icon;
	QList<TLnkFile*> subLnk;

	TLnkFile() {};
	virtual ~TLnkFile()
	{
		foreach(TLnkFile * _sub, subLnk)
		{
			delete _sub;
		}
	}
	virtual TLnkFile* creatSub()
	{
		return new TLnkFile();
	}
	friend QDataStream& operator<<(QDataStream& _stream, const TLnkFile& _lnkFile)
	{
		// 将数据输入流对象中
		_stream << _lnkFile.name;
		_stream << _lnkFile.path;
		_stream << _lnkFile.icon;
		_stream << qint32(_lnkFile.subLnk.size());
		foreach(TLnkFile * t_lnk, _lnkFile.subLnk)
			_stream << *t_lnk;

		return _stream;
	}
	friend QDataStream& operator>>(QDataStream& _stream, TLnkFile& _lnkFile)
	{
		// 将流对象中输出数据
		_stream >> _lnkFile.name;
		_stream >> _lnkFile.path;
		_stream >> _lnkFile.icon;
		qint32 t_count;
		_stream >> t_count;
		for (int i = 0; i < t_count; ++i)
		{
			TLnkFile* t_lnk = _lnkFile.creatSub();
			_stream >> *t_lnk;
			_lnkFile.subLnk.append(t_lnk);
		}
		return _stream;
	}
};

struct TFolderInfo
{
	QString Path;
	QPixmap Icon;
	TFolderInfo(const QString& _path = "", const QPixmap& _icon = QPixmap()) :
		Path(_path), Icon(_icon)
	{
	};
	TFolderInfo(const TFolderInfo& _other) :
		Path(_other.Path), Icon(_other.Icon)
	{
	}
};

#endif // struct_h__
