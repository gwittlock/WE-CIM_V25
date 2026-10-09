
#ifndef _IMPORTUTIL_H
#define _IMPORTUTIL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _STRGLIST_H
#include "StrgList.h"
#endif

#include "CadEntity.h"
#include "CadLayer.h"

class CReturn;
class C3dVec;
class C3dBox;
class C3dCoord;
class CGeoElem;
class CDbEntity;
class CDbTool;
class CDbWorkplane;
class CModel;
class CAutoModDb;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CTokenRec
{
public:

	CTokenRec( const char* tokenName, const char* tokenValue )
		{ m_name = tokenName;  m_value = tokenValue; }

	CString Name()  { return m_name; }

	CString Value()  { return m_value; }

	~CTokenRec()  {};

private:

	CString m_name;
	CString m_value;
};

typedef CIndxList<CTokenRec*> CTokenList;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CImportUtil
{
public:

	CImportUtil();

	virtual ~CImportUtil();

	void Init( CModel* model, CAutoModDb* autoModDb = NULL );

	CReturn	CadEntitiesConvert( const CCadEntityList& cadEntities );

	CReturn AcadPostProc( double rotation, bool raw );

	static CReturn LayersStrip( CModel* model );

	// Determine whether it is okay to process the named layer.
	// Patrick:  I've put in a default FALSE value here, but you may
	// want to update DWG and DXF code for explicit FALSE and remove
	// the default... eww 29 sept 99
	bool IsLayerOk( const CString& layerName, bool force_visible=FALSE );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// BEGIN Layer Mapping (Data Access Methods which wrap CAutoModDb)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Locates a named layer.
	int LayerRecFind( const CString& sourceLayerName );

	// Sets the current record for data retrieval.
	CReturn CurrentRecordSet( int layerRecIndx );

	// Gets the CAM layer name associated with the current layer record.
	CDbTool* TargetTool();

	// Assigns a 'color attribute' to the given entity.
	// When in 'preview mode', always returns yellow
	// When importing, returns the color associated with the current layer record
	// Returns off-white when some sort of failure occurs.
	void Color( CDbEntity* dbEntity );

	// Gets the Z-level associated with the current layer record.
	void ZLevel( double* zLevel );

	// Adjusts the elevation of the point based on the data
	// associated with the current layer record.
	void PointTransform(
					const C3dVec&	normal,
					double			depth,
					C3dCoord*		pt );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// END Layer Mapping (Data Access Methods which wrap CAutoModDb)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Gets the 'cut direction' associated with the target layer named 'stock'.
	// The default direction of CW direction is returned when the either the
	// stock layer is undefined, or when no tool is associated with it.
	int StockDirection();

	// Given a 'standard plane' normal vector, get the target workplane.
	CDbWorkplane* TargetWork( const C3dVec& normal );
	CDbWorkplane* StandardTargetWork( const C3dVec& normal );

	// Create the temporary workplanes required to import AutoDesk files.
	CReturn AutoDeskPlanesCreate();
	// Destroy the temporary workplanes required to import AutoDesk files.
	CReturn AutoDeskPlanesDestroy();
	// Transform all entities from their AutoDesk workplanes
	// to their corresponding MM2 workplanes.
	CReturn AutoDeskPlanesTransform( double dx, double dy, double dz );

	// Create the temporary workplanes required to import ASCII files.
	CReturn AsciiPlanesCreate( double length, double width, double zStock, int wpDefn );
	// Destroy the temporary workplanes required to import ASCII files.
	CReturn AsciiPlanesDestroy();
	// Transform all entities from their ASCII workplanes
	// to their corresponding MM2 workplanes.
	CReturn AsciiPlanesTransform( double dx, double dy, double dz );

	//void PointTransform(
	//				const CString&	layerName,
	//				const C3dVec&	normal,
	//				double			depth,
	//				C3dCoord*		pt );

	// Transform all entities from the source workplane
	// to the target workplane.
	CReturn Transform( const CString& source, const CString& target, double dx, double dy, double dz );

	// Update the model header with the length, width & thickness of the stock.
	// Also, for Morbidelli and custom workplanes, shift the origin of the
	// target workplanes by the stock dimensions as necessary.
	//
	// Stock thickness now comes from the configuration database.
	//
	CReturn HeaderUpdate(
					const CString&	planeName,
					int				stockDir,
					double			thick,
					C3dBox*			box,
					double*			zStock );

	CReturn ViewPlanesCreate( double length, double width, double thick );

	// Create the standard MM2 workplanes (WORLD, TOP, RIGHT, LEFT, FRONT, BACK)
	// using the given workplane definition code wpDefn. (1) router / (2) point-to-point.
	CReturn StandardPlanesCreate( double length, double width, double zStock, int workType, int machType );

	// Destroy the temporary workplanes required to import AutoDesk files.
	CReturn SmartCAMPlanesDestroy( const CStringArray& in_names );

	// Transform all entities from their AutoDesk workplanes
	// to their corresponding MM2 workplanes.
	CReturn SmartCAMPlanesTransform( const CStringArray& in_names, double dx, double dy, double dz );

	///////////////////////////////////////////////////////////
	// Create profiles (ie. Sequence/Chain) contiguous curves
	// on each layer specified in the database.
	// See also CAutoMod::ProfilesCreate()
	CReturn ProfilesCreate();

	void StockPointsOrder(
					int				stockDir,
					double			xmin,
					double			xmax,
					double			ymin,
					double			ymax,
					double			zStock,
					C3dCoord*		pts );

	// Create the stock layer geoemtry (use when none exists).
	CReturn StockCreate(
					const CString&	planeName,
					int				stockDir,
					double			xmin,
					double			xmax,
					double			ymin,
					double			ymax,
					double			zStock );

	// Changes the stock dimensions and updates the workplane and view planes accordingly.
	CReturn StockUpdate( double dx, double dy, double dz, int workType, int machType );

	// Set the 'system flag' on entities on the stock layer.
	CReturn SystemFlagsSet();

	// Create a work-zone and put the imported entities into it.
	CReturn WorkZoneCreate( const C3dBox& box, int zoneNum, double delta );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// BEWARE!  These methods are restricted to analyzing files.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Return the count of layers.
	int LayerCount();

	// Get the Ith layer name.
	CString LayerName( int index );

	// Add a named layer to the list of layers that can be processed.
	void LayerAdd( const CString& layerName );

public:

	// Get the token value of the named token from the given string.
	static CString TokenValue( const CString& string, const CString& myTokenName );

	static void	AcceptedLayersGet(
						const CAutoModDb&	autoModDb,
						CCadLayerList*		acceptedLayers );

protected:

private:  // Methods

	CReturn	EntityConvert(
						const CGeoElem*	geoElem,
						const C3dVec&	normal,
						int				autoModDbRecIndx );

	CDbWorkplane* PlaneFindCreate( const CString& name );
	CReturn StockLayerProcess( const CDbTool& stockLayer, C3dBox* box );

	void ClampDataSet( CVarList* header );

private:  // Disabled

	CImportUtil( const CImportUtil& );
	const CImportUtil& operator = ( const CImportUtil& );
	int operator == ( const CImportUtil& ) const;
	int operator != ( const CImportUtil& ) const;

private:  // Data

	CModel*		m_model;
	CAutoModDb* m_autoModDb;

	CStrgList	m_layers;  // Restricted to analyzing files.

	CString m_currLayerName;
	int m_currLayerRecIndx;
};

#endif

