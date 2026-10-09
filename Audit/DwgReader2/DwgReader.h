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
class CCommonEntityData;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CDWGLayer
	{
	public:

		CDWGLayer( const CString& name, long handle )
		{
			m_name = name;
			m_handle = handle;
		}

		const CString& Name()		{ return m_name; }
		long Handle()				{ return m_handle; }

	private:

		CString	m_name;
		long	m_handle;
	};

	typedef CDynamicArray<CDWGLayer*> CLayerList;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CDwgReader
{
public:

	CDwgReader( void );

	CReturn	ReadModel( const CString& full_path, const CDynamicArray<CString*>& acceptedLayers );

	CReturn	ReadLayers( const CString& full_path );

	const CLayerList& Layers();

	~CDwgReader( void );

private:

	CReturn	ReadTables();

	CReturn VersionRead();
	CReturn SectionTableRead();
	CReturn DrawingObjectsRead( BOOL layersOnly );
	CReturn ClassSectionRead();
	CReturn	ObjectMapRead();
	CReturn ObjectRead( int objHandle, short objType, long objSize );
	CReturn ReadCommonEntityData( CCommonEntityData* ced );

	CReturn	ReadBlockHeader( const CCommonEntityData& ced );
	CReturn	ReadBlock( const CCommonEntityData& ced );
	CReturn	ReadEndBlock( const CCommonEntityData& ced );

	CReturn ReadLayer( const CCommonEntityData& ced );
	CReturn ReadLine( const CCommonEntityData& ced );
	CReturn ReadArc( const CCommonEntityData& ced );
	CReturn ReadCircle( const CCommonEntityData& ced );
	CReturn ReadPoly2d( const CCommonEntityData& ced );
	CReturn ReadPoly3d( const CCommonEntityData& ced );
	CReturn ReadVertex3d( const CCommonEntityData& ced );

	int		ReadHandleReference();
	void	ReadExtendedEntityData();
	void	ReadGraphicsData();

	long	ReadCommonEntityHandleData( const CCommonEntityData& ced );

	CString	ReadString();

	BOOL	MustProcess( short objType );
	char*	ObjectString( short objType );

	short	ObjectTypeConvert( short objType );

	void	LayersCull( const CDynamicArray<CString*>& acceptedLayers );
	BOOL	IsAcceptedLayer( long layerHandle );

	void DumpObjectTable( const CString& filename );

private:

	CDWGFile	m_file;
	int			m_version;
	BOOL		m_layersOnly;
	BOOL		m_dump;

	CDynamicArray<CSectionLocator*>	m_sections;
	CDynamicArray<CObjectLocator*>	m_objects;
	CDynamicArray<CClassMapper*>	m_classes;
	CLayerList						m_layers;
};

#endif