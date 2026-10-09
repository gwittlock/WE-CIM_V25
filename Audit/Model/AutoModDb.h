
#ifndef _AUTOMODDB_H
#define _AUTOMODDB_H

#include "Return.h"
#include "DynamicArray.h"
#include "StrgList.h"
#include "VarList.h"
#include "2dBox.h"
#include "ToolSetup.h"
#include "DaoDB.h"


enum EAutoModMode
{
	AUTOMOD_USE_CAD_Z	= 0,
	AUTOMOD_USE_CMDB_Z	= 1
};


#if YEARGH  // MSVC 6.0 spews chunks when trying to use std::map

	#include <map>
	typedef std::map<int,double> TIntDoubleMap;

#else

	typedef CMap<int,int,double,double> TIntDoubleMap;

#endif


// ============================================================================

class dllExport CAutoModDb
{
public:

	CAutoModDb();

	virtual ~CAutoModDb();

	// Build a table of database records associated with the given layer
	// mapping configuration.  If the given toolSetupId is less-than 1,
	// this object will be used as if for previewing a file.  That is,
	// all machining database information is ignored!
	CReturn Init(
				const CString&	dbPath,
				int				toolSetupId,
				int				layerSetupId,
				int				materialId,
				bool			buildToolSetup );

	CReturn Init(
				const CString&	dbPath,
				CString&		machineName,
				CString&		toolSetupName,
				CString&		materialName,
				bool			buildToolSetup );

	// Determines whether this object is being used for previewing a file.
	bool IsPreview() const;

	const TArrayOfAttribLists& TooledStations() const	{ return m_toolSetup.TooledStations(); }
	const TArrayOfAttribLists& Stations() const			{ return m_toolSetup.Stations(); }

	const CVarList& MachineAttributes()	const		{ return m_machineAttribs; }
	const CVarList& MaterialAttributes() const		{ return m_materialAttribs; }
	const CVarList& LayerSetupAttributes() const	{ return m_layerSetupAttribs; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// BEGIN Layer Mapping (Global Data Access Methods)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Machine Attributes
	int Units() const;
	int WorkplaneType() const;
	int MachineType() const;
	int NumberOfClamps() const;
	double HoleMinusTol() const;
	double HolePlusTol() const;
	double SpaceTol() const;
	bool CodePartProf() const;
	C2dBox MachineLimits() const;
	CString MachineName() const;

	// Material Attributes
	CString MaterialDesc() const;
	double MaterialLength() const;
	double MaterialWidth() const;
	double MaterialThick() const;

	// Layer Setup Attributes
	EAutoModMode ZLevelMode() const;
	double GapTol() const;
	double CleanTol() const;
	double SharpAngle() const;
	double FilterTol() const;
	bool RestrictOffset() const;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// END Layer Mapping (Global Data Access Methods)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// BEGIN Layer Mapping (Layer Data Access Methods)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Obtain the count of extracted records.
	int Count() const;

	// Sets the current record for data retrieval.
	CReturn CurrentRecordSet( int recIndx ) const;

	// Helps determine whether it is okay to process the named layer.
	// Returns the record index (<0) not okay (>=0) is okay
	int SourceLayerFind( const CString& layerName ) const;

	// Gets the CAM layer name associated with the current record.
	const CString TargetName() const;

	// Gets the named integer value from the current record.
	int IntGet( const CString& fieldName ) const;

	// Gets the named boolean value from the current record.
	bool BoolGet( const CString& fieldName ) const;

	// Gets the named double value from the current record.
	double DoubleGet( const CString& fieldName ) const;

	// Gets the named string value from the current record.
	CString StringGet( const CString& fieldName ) const;

	CToolSetup& ToolSetup() { return m_toolSetup; };

	const CStrgList& LayerNames() const	{ return m_layerNames; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// END Layer Mapping (Layer Data Access Methods)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CReturn SpeedFeed( int toolTypeId, double diam, CVarList* attribs ) const;

	// Last minute PM4 conversion stuff...
	int MachineID( const CString& machineName );
	CReturn MachineValuesLoad( int machineID );  // aka Init()

	// These methods were made public solely for DXF/DWG import!
	CReturn DbOpen( const CString& dbFilePath  );
	void DbClose();

	// Don't really want this stuff here ... but it is convenient.
	bool AllowTorchPiercing() const  { return m_allow_torch_piercing; }
	double KerfGet() const;

private:  // Methods.


	CReturn MachineValuesGet( int toolSetupId );
	CReturn LayerSetupValuesGet( int layerSetupId );
	CReturn MaterialValuesGet( int materialIndex );

	int MachineId( int toolSetupId );
	CString ToolTypeDescription( int toolTypeId );
	int ToolSetupId( const CString& machineName, const CString& toolSetupName );
	int MaterialId( const CString& materialName );

	// Don't really want this stuff here ... but it is convenient.
	void AllowTorchPiercingInit();

	void ToolKerfMapInit() const;

#if (_NST)
	CReturn NstLayerSetupValuesGet( int cad_global_id );
#endif

private:

	static int QSortCompare( const void* ptrA, const void* ptrB );
	static int BinarySearchCompare( const void* myItem, const void* arrayItem );
	
	static CString CadLayerName( const CVarList* attribs );
	static CString CamLayerName( const CVarList* attribs );
	
	static int g_cadLayerIndx;
	static int g_camLayerIndx;

private:  // Disabled.

	CAutoModDb( const CAutoModDb& );
	const CAutoModDb& operator = ( const CAutoModDb& );
	int operator == ( const CAutoModDb& ) const;
	int operator != ( const CAutoModDb& ) const;

private:  // Data.

	CDaoDB		m_db;
	bool		m_dbIsOpen;

	CDynamicArray<CVarList*> m_table;

	CStrgList	m_layerNames;

	CToolSetup	m_toolSetup;

	CString m_cachedSourceName;
	CString m_cachedTargetName;
	CString m_cachedMoveToName;

	CVarList m_machineAttribs;
	CVarList m_materialAttribs;
	CVarList m_layerSetupAttribs;

	CString m_toolSetupName;

	int m_toolSetupId;
	int m_materialId;  // required for SpeedFeed()

	int m_currRecIndx;

	bool m_allow_torch_piercing;
	TIntDoubleMap m_tool_kerf_map;
};


#endif
