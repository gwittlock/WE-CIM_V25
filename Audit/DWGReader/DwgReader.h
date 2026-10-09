#ifndef _DWGREADER_H
#define _DWGREADER_H

#include "stdafx.h"
#include "DwgTypes.h"
#include "Return.h"
#include "DynamicArray.h"
#include "dwg_file.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CSectionLocator;
class CObjectLocator;
class CClassMapper;

class dllExport CDwgReader
{
public:

	CDwgReader( void );

	CReturn	Read( const CString& full_path );

	~CDwgReader( void );

private:

	CReturn VersionRead();
	CReturn SectionTableRead();
	CReturn DrawingObjectsRead();
	CReturn ClassSectionRead();
	CReturn	ObjectMapRead();
	CReturn ObjectRead( int objHandle, short objType, long objSize );
	CReturn CommonEntityDataRead( int objHandle, short objType, long objSize );

	CReturn ReadLayer( int objHandle, long objSize );
	CReturn ReadLine( int objHandle, long objSize );
	CReturn ReadArc( int objHandle, long objSize );
	CReturn ReadCircle( int objHandle, long objSize );
	CReturn ReadPoly2d( int objHandle, long objSize );
	CReturn ReadPoly3d( int objHandle, long objSize );
	CReturn ReadVertex3d( int objHandle, long objSize );

	int		ReadHandleReference();
	void	ReadExtendedEntityData();
	void	ReadGraphicsData();

	CString	ReadString();

	BOOL	MustProcess( short objType );
	char*	ObjectString( short objType );

	short	ObjectTypeConvert( short objType );

	void DumpObjectTable();

private:

	CDWGFile	m_file;
	int			m_version;
	BOOL		m_dump;

	CDynamicArray<CSectionLocator*>	m_sections;
	CDynamicArray<CObjectLocator*>	m_objects;
	CDynamicArray<CClassMapper*>	m_classes;
};

#endif