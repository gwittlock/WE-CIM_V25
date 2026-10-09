
#ifndef _SHAPE_H
#define _SHAPE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DynamicArray.h"
#include "VarList.h"
#include "2dCoord.h"
#include "2dUnitVec.h"
#include "2dBox.h"
#include "3x4Matrix.h"
#include "GeoCurve.h"

// Currently recognized shapes.
enum EShape
{
	SHAPE_ROUND,
	SHAPE_SQUARE,
	SHAPE_RECTANGLE,
	SHAPE_DIAMOND,
	SHAPE_OBROUND,
	SHAPE_SINGLE_D,
	SHAPE_DOUBLE_D,
	SHAPE_TRAPEZOID,
	SHAPE_HEXAGON,
	SHAPE_KEYHOLE,
	SHAPE_TERMINAL
};



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CShape
{
public:

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Creates an 'empty' shape object.
	//
	CShape();

	CShape( const CShape& shape );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Shape initialization methods, generating the attributes that
	// describe this shape.
	//

	// Generate the shape using the given curves.
	// void Init( const CGeoCurveList& curves, bool reduce );
	void Init( const CGeoCurveArray& curves, bool reduce );

	// The ctor behavior sets this to true.
	// Introduced to resolve a bug in CAutoIndex::Offset().
	void FullNormalization( bool fully_normalize );

	// Generate the shape using the owned curves.
	// This is a companion method to pCurves()
	void Init( bool reduce );

	const CShape& operator = ( const CShape& shape );

	EShape Type() const						{ return m_type; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Attribute access methods.
	//

	int AttribCount() const;

	int IntGet( const CString& name, int defval ) const;
	double DoubleGet( const CString& name, double defval  ) const;
	CString StringGet( const CString& name, const CString& defval ) const;

	void IntSet( const CString& name, int ival );
	void DoubleSet( const CString& name, double dval );
	void StringSet( const CString& name, const CString& sval );

	void AttribsDelete();
	void AttribDelete( const CString& name );

	// Low-level methods (caution).
	const CVarList& Attribs() const			{ return m_attribs; }
	CVarList* pAttribs()					{ return &m_attribs; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Geometry access methods.
	//
	// Gets the count of curves in this poly.
	int Count() const									{ return m_curves.Count(); }

	// Curve access operator.
	const CGeoCurve* operator[] ( int indx ) const		{ return m_curves[indx]; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Geometric properties methods.
	//
	const C2dCoord& Center() const;

	C2dBox Box2d() const;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Clones this shape object.
	//
	CShape* Clone() const;

	// Apply the given transformation to the curves of this poly.
	void Transform( const C3x4Matrix& xform );

	// Reduce this poly to those curves whose normals
	// dot positive with the given vector.
	void Cull(
				const C2dUnitVec&	vec,
				int					offsetDir );

	// Copy to another poly, the curves of this poly whose
	// normals dot positive with the given vector.
	void Cull(
				const C2dBox&	box,
				int				down );

	// Set the winding direction of this shape.
	bool Direction( int dir );

	int LineCount() const		{ return m_lineCount; }
	int ArcCount() const		{ return m_arcCount; }

	virtual ~CShape();

protected:

private:  // Methods

	void Normalize();

	void Encode();

	void Parameterize();

	void ParameterizeParallelogram();
	void ParameterizeObround();
	void ParameterizeRound();
	void ParameterizeSingleD();
	void ParameterizeDoubleD();
	void ParameterizeHexagon();
	void ParameterizeKeyhole();

	double Turn( const C2dUnitVec& vecA, const C2dUnitVec& vecB );

	void UpdateBox() const;

	void Reverse();

private:  // Disabled

	int operator == ( const CShape& ) const;
	int operator != ( const CShape& ) const;

private:  // Data

	EShape	m_type;

	// stores the attributes describing this shape
	// and any other attributes attached by the client.
	CVarList m_attribs;

	// stores the geometric representation of this shape.
	// CGeoCurveList m_curves;
	CGeoCurveArray m_curves;

	C2dBox m_box;
	C2dCoord m_pc;

	int m_lineCount;
	int m_arcCount;

	bool	m_fully_normalize;
};


typedef CDynamicArray<CShape*> TShapeArray;
typedef TShapeArray  TSortedShapes[SHAPE_TERMINAL];

#endif

