
#ifndef _TOOLSHAPE_H
#define _TOOLSHAPE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _TOOLCONST_H
#include "ToolConst.h"
#endif

#ifndef _VARLIST_H
#include "VarList.h"
#endif

#ifndef _3DCOORD_H
#include "3dCoord.h"
#endif

#ifndef _GEOCURVE_H
#include "GeoCurve.h"
#endif

#include "GeoPoly.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Creates an 'attributed point' represention of a tool shape.  Also
// provides for conversion to a geometric representation.
//
//  TODO:  May be merged with GeoPoly now, since GeoPoly is using the same
//		attributed point data structure (and stores true arcs)
//
class dllExport CToolShape
{
public:

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Creates an 'empty' tool shape object.
	//
	CToolShape();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Initializes the attributed point representation of this tool shape
	// based on the given attributes.  The recognized shapes and associated
	// attributes are tightly coupled with the machining database
	// representation of tools.
	//
	// Initialization will fail to yield a proper shape if the tool type
	// id attribute <Type_ID> is not recognized, and will also fail if the
	// required associated attributes are not present or appropriate.
	//
	void Init( const CVarList& attribs );
	void InitXZ( const CVarList& attribs );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// In support of custom tool graphics.
	//
	void Init(
			CArray<int,int>& type,
			CArray<C3dCoord, C3dCoord>& pt );

	CReturn InitFromCTG( const CString& ctg_path );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Appends the curves representing this shape to the given curve list.
	//
	void Convert( CGeoCurveArray* geoCurves ) const;
	void Convert( CGeoPoly* geoPoly ) const;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Determines if the attributed points have been initialized.
	//
	bool IsInited() const						{ return (Count() > 0); }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Sets the count of attributes points to zero.
	//
	void Invalidate();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Point representation access methods
	//
	int Count() const							{ return m_type.GetSize(); }

	int Type( int indx ) const					{ return m_type[indx]; }

	const C3dCoord Point( int indx ) const		{ return m_pt[indx]; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Utility methods.
	//
	static double EffectiveDiameter( const CVarList& attribs );
	static double EffectiveLength( const CVarList& attribs );
	static bool IsHoleTool( const CVarList& attribs );
	static bool IsRoundTool( const CVarList& attribs );
	static bool IsFormTool( const CVarList& attribs );
	static bool IsProfilingTool( const CVarList& attribs );

	static void LineOffset( C3dCoord* ptA, C3dCoord* ptB, double dist );

	static void Convert(
			const CArray<int,int>& pt_type,
			const CArray<C3dCoord, C3dCoord>& pt,
			CGeoCurveArray* geoCurves );

	static void Convert(
			const CArray<int,int>& pt_type,
			const CArray<C3dCoord, C3dCoord>& pt,
			CGeoPoly* geoPoly );

	virtual ~CToolShape();

protected:

private:  // Methods

	void gen_2d_round( const CVarList& attribs );
	void gen_2d_square( const CVarList& attribs );
	void gen_2d_rect( const CVarList& attribs );
	void gen_2d_obround( const CVarList& attribs );
	void gen_2d_diamond( const CVarList& attribs );
	void gen_2d_crad( const CVarList& attribs );
	void gen_2d_d( const CVarList& attribs );
	void gen_2d_dd( const CVarList& attribs );
	void gen_2d_trapezoid( const CVarList& attribs );
	void gen_2d_keyhole( const CVarList& attribs );
	void gen_2d_hexagon( const CVarList& attribs );
	void gen_2d_custom( const CVarList& attribs );

	void gen_xz_straight( const CVarList& attribs );
	void gen_xz_core_box( const CVarList& attribs );
	void gen_xz_round_over( const CVarList& attribs );
	void gen_xz_pt_round_over( const CVarList& attribs );
	void gen_xz_ogee( const CVarList& attribs );
	void gen_xz_raised_panel( const CVarList& attribs );
	void gen_xz_v_groove( const CVarList& attribs );
	void gen_xz_drill( const CVarList& attribs );
	void gen_xz_saw( const CVarList& attribs );
	void gen_xz_custom( const CVarList& attribs );
	void gen_xz_round( const CVarList& attribs );

private:  // Disabled

	CToolShape( const CToolShape& );
	const CToolShape& operator = ( const CToolShape& );
	int operator == ( const CToolShape& ) const;
	int operator != ( const CToolShape& ) const;

private:  // Data
	//
	// type:
	//		1 == line
	//		2 == G02 (CW Arc)
	//		3 == G03 (CC Arc)
	//
	CArray<int,int> m_type;

	CArray<C3dCoord, C3dCoord> m_pt;
};

#endif

