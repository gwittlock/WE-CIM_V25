
#ifndef _GEOELEM_H
#define _GEOELEM_H

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// An abstract base class representing a geometric entity.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IndxList.h"
#include "DynamicArray.h"
#include "VarList.h"
#include "3dCoord.h"
#include "3x4Matrix.h"
#include "3dBox.h"

// #define TRACE_GEO_HEAP 1

enum ElemType
{
	GEOPOINT =	0,
	GEOLINE =	1,
	GEOARC =	2,
	GEOPOLY =	3
};


class dllExport CGeoElem
{
public:

	CGeoElem();

	virtual ~CGeoElem();

	virtual ElemType Type() const = 0;

	virtual CGeoElem* Clone( bool attribs_copy ) const = 0;

	virtual const C3dCoord& StartPt() const = 0;

	virtual void StartPt( const C3dCoord& pt ) = 0;
	virtual void StartPt( double xs, double ys, double zs ) = 0;

	virtual const C3dCoord& EndPt() const = 0;

	virtual void EndPt( const C3dCoord& pt ) = 0;
	virtual void EndPt( double xe, double ye, double ze ) = 0;

	virtual double Length2d() const = 0;

	const C3dBox& Box() const;

	virtual void Xform( const C3x4Matrix& xform ) = 0;
	virtual void Shift( const C3dVec& delta ) = 0;

	// Attribute managment methods.
	int AttribCount() const;

	int IntGet( const CString& name, int defval ) const;
	double DoubleGet( const CString& name, double defval  ) const;
	CString StringGet( const CString& name, const CString& defval ) const;

	void IntSet( const CString& name, int ival );
	void DoubleSet( const CString& name, double dval );
	void StringSet( const CString& name, const CString& sval );

	void AttribDelete( const CString& name );

	bool HasAttrib() const			{ return (m_attribs != NULL); }

	// Low-level methods (caution).
	const CVarList& Attrib() const;
	CVarList* pAttrib();

	void* UserData() const			{ return m_userdata; }
	void UserData( void* data )		{ m_userdata = data; }

	// For debugging.
	void Draw() const;

protected:

	// Update the bounding box of an element.
	void Box( const C3dBox& box );

private:

	// Disabled.
	CGeoElem( const CGeoElem& );
	const CGeoElem& operator = ( const CGeoElem& );
	int operator == ( const CGeoElem& ) const;
	int operator != ( const CGeoElem& ) const;

private:

	C3dBox m_box;

	// Neither the Varlist or Userdata are copied when the geo elem is
	// assigned or cloned.
	CVarList*	m_attribs;
	void*		m_userdata;
#if TRACE_GEO_HEAP
	int m_id;
#endif
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CGeoElemArray : public CDynamicArray<CGeoElem*>
{
public:

	CGeoElemArray()  { }
	virtual ~CGeoElemArray()  { }

	void Draw() const;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CGeoElemList : public CIndxList<CGeoElem*>
{
public:

	CGeoElemList()  { }
	virtual ~CGeoElemList()  { }

	void Draw() const;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// CBaseGeoRenderer is an interface class. Its methods are
// defined as being pure-virtual so as to prevent inavertant
// instation of a CBaseGeoRenderer object.
//
// TODO: Perhaps this should be moved to a header where it
//  has access to all of the class declarations the API
// references(?)

class CGeoCurveList;
class CGeoCurveArray;

class dllExport CBaseGeoRenderer
{
public:

	CBaseGeoRenderer()  { }
	virtual ~CBaseGeoRenderer()  { }

	virtual void DrawGeo( const CGeoElem& geo ) = 0;
	virtual void DrawGeo( const CGeoElemList& list ) = 0;
	virtual void DrawGeo( const CGeoElemArray& array ) = 0;
};

dllExport void GeoRendererSet( CBaseGeoRenderer& renderer );
dllExport CBaseGeoRenderer& GeoRendererGet();

#endif

