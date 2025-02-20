//********************************************************************
//	filename: 	F:\mygit\QTrayToolbar\src\lnkFileReader.h
//	desc:		
//
//	created:	wangdaye 16:2:2025   10:40
//********************************************************************
#ifndef lnkFileReader_h__
#define lnkFileReader_h__
#include <QObject>
#pragma pack(push, 1)
typedef struct
{
	bool HasLinkTargetIDList : 1;
	bool HasLinkInfo : 1;
	bool HasName : 1;
	bool HasRelativePath : 1;
	bool HasWorkingDir : 1;
	bool HasArguments : 1;
	bool HasIconLocation : 1;
	bool IsUnicode : 1;
	bool ForceNoLinkInfo : 1;
	bool HasExpString : 1;
	bool RunInSeparateProcess : 1;
	bool Unused1 : 1;
	bool HasDarwinID : 1;
	bool RunAsUser : 1;
	bool HasExpIcon : 1;
	bool NoPidlAlias : 1;
	bool Unused2 : 1;
	bool RunWithShimLayer : 1;
	bool ForceNoLinkTrack : 1;
	bool EnableTargetMetadata : 1;
	bool DisableLinkPathTracking : 1;
	bool DisableKnownFolderTracking : 1;
	bool DisableKnownFolderAlias : 1;
	bool AllowLinkToLink : 1;
	bool UnaliasOnSave : 1;
	bool PreferEnvironmentPath : 1;
	bool KeepLocalIDListForUNCTarget : 1;
	bool Unused : 5;
} LinkFlags;
typedef union {
	typedef struct
	{
		quint32 sFILE_ATTRIBUTE_READONLY : 1;
		quint32 sFILE_ATTRIBUTE_HIDDEN : 1;
		quint32 sFILE_ATTRIBUTE_SYSTEM : 1;
		quint32 sFILE_ATTRIBUTE_VOLUME_LABEL : 1;
		quint32 sFILE_ATTRIBUTE_DIRECTORY : 1;
		quint32 sFILE_ATTRIBUTE_ARCHIVE : 1;
		quint32 sFILE_ATTRIBUTE_NORMAL : 1;
		quint32 sFILE_ATTRIBUTE_TEMPORARY : 1;
		quint32 sFILE_ATTRIBUTE_SPARSE_FILE : 1;
		quint32 sFILE_ATTRIBUTE_REPARSE_POINT : 1;
		quint32 sFILE_ATTRIBUTE_COMPRESSED : 1;
		quint32 sFILE_ATTRIBUTE_OFFLINE : 1;
		quint32 sFILE_ATTRIBUTE_NOT_CONTENT_INDEXED : 1;
		quint32 sFILE_ATTRIBUTE_ENCRYPTED : 1;
		quint32 sFILE_ATTRIBUTE_INTEGRITY_STREAM : 1;
		quint32 sFILE_ATTRIBUTE_VIRTUAL : 1;
		quint32 sFILE_ATTRIBUTE_NO_SCRUB_DATA : 1;
		quint32 Unused : 15;
	} TFileAttributes;
	quint32 UFileAttributes;
} FileAttributes;

typedef quint8 BGUID[16];

enum class EShowCommand :quint32 {
	eSW_SHOWNORMAL = 0x00000001,
	eSW_SHOWMAXIMIZED = 0x00000003,
	eSW_SHOWMINNOACTIVE = 0x00000007
};
typedef struct
{
	quint32 HeaderSize;	/* Must be 0x0000004C */
	BGUID LinkCLSID; 	    	/* This value MUST be 00021401-0000-0000-C000-000000000046. */
	LinkFlags sLinkFlags;
	FileAttributes sFileAttributes;
	quint64 CreationTime;
	quint64 AccessTime;
	quint64 WriteTime;
	quint32 FileSize;
	quint32 IconIndex;
	EShowCommand ShowCommand;
	quint16 HotKey;
	quint16 Reserved1;
	quint32 Reserved2;
	quint32 Reserved3;

} ShellLinkHeader;
#pragma pack(pop)

class WCharArray
{
public:
	WCharArray(qsizetype _size, wchar_t _ch);
	~WCharArray(void);
	wchar_t* data(void);
private:
	qsizetype size;
	wchar_t* pbuff;
};
class lnkFileReader : public QObject
{
	Q_OBJECT

public:
	lnkFileReader(QObject* _parent, const QString& _path);
	~lnkFileReader();
	lnkFileReader& read(const QString& _path);
	const QString& getName(void);
	static QString ReadGUID(const BGUID& _id);
private:
	QString m_path;
	QString m_name;
	ShellLinkHeader m_head;
};

#endif // lnkFileReader_h__
