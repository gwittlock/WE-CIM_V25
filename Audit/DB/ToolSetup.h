
#ifndef _TOOLSETUP_H
#define _TOOLSETUP_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _DYNAMICARRAY_H
#include "DynamicArray.h"
#endif

#ifndef _VARLIST_H
#include "VarList.h"
#endif

#ifndef _SHAPE_H
#include "Shape.h"
#endif

#ifndef _DAODB_H
#include "DaoDB.h"
#endif

#define TS_NO_FLAGS					0x000
#define TS_GET_TOOLS				0x001
#define TS_GET_STATIONS				0x002
#define TS_GET_TOOLED_STATIONS		0x004
#define TS_COLLATE					0x008
#define TS_SHAPES_INIT				0x010
#define TS_ALL_FLAGS				0x01F

class CEntityDb;
class CDbTool;

typedef CDynamicArray<CVarList*> TArrayOfAttribLists;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CToolSetup
{
public:

	CToolSetup();

	CReturn Init( const CString& dbPath, int toolSetupID, int flags );

	CReturn InitShapesFromDb( const CEntityDb& db );

	void ToolReferencesUpdate( const CEntityDb& db );

	CShape* EmptyFind( double reqdStationSize, double orientation, bool reqAutoIndex );

	CShape* UnrefdFind( double reqdStationSize, double orientation, bool reqAutoIndex );

	void Divorce( CShape* tooledStation );

	CShape* Marry( CShape* station, CShape* tool );

	const TArrayOfAttribLists& Tools() const
			{ return m_tools; }

	const TArrayOfAttribLists& Stations() const
			{ return m_stations; }

	const TArrayOfAttribLists& TooledStations() const
			{ return m_tooledStations; }

	const TSortedShapes& TooledStationShapes() const
			{ return m_tooledStationShapes; }

	const TSortedShapes& UnpairedToolShapes() const
			{ return m_unpairedToolShapes; }

	const TSortedShapes& UnpairedStationShapes() const
			{ return m_unpairedStationShapes; }

	CString	Name() const
			{ return m_toolSetupName; }

	virtual ~CToolSetup();

protected:

private:  // Methods

	CReturn Open( const CString& dbPath );

	void Close();

	CString ToolSetupNameGet( int toolSetupID );

	int MachineGet( int toolSetupID );

	CReturn StationsGet();

	CReturn ToolsGet();

	CReturn TooledStationsGet();

	CReturn ToolSetupInfoAppend();

	CReturn Reduce();

	CReturn SortedShapesInit();

	CReturn SpecificToolAttributesAdd(
							int			toolTypeID,
							CDaoQuery&	toolQuery,
							bool		filter,
							CVarList*	attribs );


	CString ToolTypeDescription( int toolTypeID );
	CReturn ToolDescriptionsGet();

	void DumpSortedShapes() const;
	void Dump() const;

	void ToolsDump( const CString& title, const TSortedShapes& tools ) const;
	void AttribsDump( const CString& title, const TArrayOfAttribLists& taal ) const;
	void AttribsDump( const CVarList& attribs ) const;

	void StationAttribsGet( CShape* tooledStation, int flags, CVarList* toolParams );
	bool IsStationAttrib( const CVar& attrib );

	CShape* ShapeCreate( const CVarList& attribs );

	void ShapeDispose( CShape* shape );
	CShape* ShapeRemove( CShape* shape );
	CShape* ShapeRemove( CShape* shape, TSortedShapes* magazine );

	CDbTool* ToolFind( const CEntityDb& db, const CString& field, int value );

	bool IsMatchingStation(
						const CShape&	station,
						double			reqdStationSize,
						double			reqdOrientation,
						bool			reqdAutoIndex );

	void ShapesDestroy( TSortedShapes* catalog );

private:  // Disabled

	CToolSetup( const CToolSetup& );
	const CToolSetup& operator = ( const CToolSetup& );
	int operator == ( const CToolSetup& ) const;
	int operator != ( const CToolSetup& ) const;

private:  // Data

	TArrayOfAttribLists	m_stations;
	TArrayOfAttribLists m_tooledStations;
	TArrayOfAttribLists m_tools;

	TSortedShapes	m_tooledStationShapes;
	TSortedShapes	m_unpairedToolShapes;
	TSortedShapes	m_unpairedStationShapes;

	TShapeArray		m_garbage;

	CDynamicArray<CString*>	m_desc;


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Transient data, used only during data extraction.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CDaoDB		m_db;
	bool		m_dbIsOpen;

	int			m_machineID;
	int			m_toolSetupID;
	int			m_materialID;  // required for SpeedFeed()

	CString		m_toolSetupName;
};

#endif

