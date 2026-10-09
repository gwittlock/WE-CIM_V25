
#ifndef _AUTOTOOL_H
#define _AUTOTOOL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _SHAPE_H
#include "Shape.h"
#endif

#ifndef _SHAPEMATCHER_H
#include "ShapeMatcher.h"
#endif

#ifndef _SHAPEDECOMPOSER_H
#include "ShapeDecomposer.h"
#endif

#ifndef _TOOLSETUP_H
#include "ToolSetup.h"
#endif

class CModel;

enum EMatchingRigour
{
	RIGOUR_EXACT,
	RIGOUR_ADEQUATE,
	RIGOUR_DECOMPOSE
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CAutoTool
{
public:

	CAutoTool();

	CReturn ToolIt(
					CToolSetup*				toolSetup,
					bool					buildSetup,
					EMatchingRigour			rigour,
					const CShape&			theShape,
					const CShapeMatcher&	shapeMatcher,
					const CShapeDecomposer&	shapeDecomposer,
					CGeoElemList*			geometry );

	virtual ~CAutoTool();

protected:

private:  // Methods

	CReturn ToolSetupModify( CToolSetup* toolSetup, const TShapeArray& exemplars );

	CReturn RealizeShape( const TShapeArray& exemplars, CGeoElemList* geometry );

	CShape* MatchingShape( const CShape& shape );

	void HitCreate(
					const C3dCoord&		pc,
					const CShape&		theShape,
					const CShape&		theMatchingShape,
					CGeoElemList*		result );

	void HitsCreate(
					const CShape&		theShape,
					const CShape&		theMatchingShape,
					double				du,
					double				dv,
					CGeoElemList*		result );

	CShape* Collate( CToolSetup* toolSetup, CShape* station, CShape* tool );

	bool IsReusable( const CShape& station );

	EShape ShapeType( int typeID );

private:  // Disabled

	CAutoTool( const CAutoTool& );
	const CAutoTool& operator = ( const CAutoTool& );
	int operator == ( const CAutoTool& ) const;
	int operator != ( const CAutoTool& ) const;

private:  // Data

};

#endif

