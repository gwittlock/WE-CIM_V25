
#ifndef _PROFILEBUILDER_H
#define _PROFILEBUILDER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _DBCURVE_H
#include "DbCurve.h"
#endif

#ifndef _DBCURVELIST_H
#include "DbCurveList.h"
#endif

#ifndef _DBPROFILE_H
#include "DbProfile.h"
#endif

#ifndef _MODEL_H
#include "Model.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CProfileBuilder
{
public:

	// Find the sequence of curves that is contiguous with
	// the given seed curve, regardless of curve 'properties'.
	static CReturn ProfileGrow(
						CDbCurve* dbSeedCurve,
						double gapTol,
						CDbCurveList* inputCurves,
						CDbCurveList* outputCurves );

	// Create profiles through curves in the given list
	// that have the same layer and owner as the seed curve.
	static CReturn ProfileGrow(
						CModel* model,
						double gapTol,
						CDbCurve* dbSeedCurve,
						CDbCurveList* dbCandidateCurves,
						CDbProfile** dbProfile );

	// Create a profile through the given selection set of curves, stopping
	// the chaining process at the first curve that is too far away.
	// CRITICAL ASSUMPTION: curve[0] has the correct direction.
	static CReturn ProfileSelectedGrow(
		CModel*			model,
		double			gapTol,
		CDbCurveList*	inputCurves,
		CDbProfile**	dbProfile );

	// Create profiles through the curves on the given layer.
	static CReturn ProfileLayer(
						CModel*		model,
						CDbTool*	dbLayer,
						double		gapTol,
						double		cleanTol,
						bool		associate,
						int			dir,
						bool		sortByID);

	// For each layer in the model, create
	// profiles through the curves on the layer.
	static CReturn ProfileAll(
						CModel*		model,
						double		gapTol,
						double		cleanTol,
						bool		associate,
						bool		sortByID);

	// Moved over (and combined) from CreateProcess and NestConfig
	static CReturn Transform( 
						CModel*				model,
						const C3x4Matrix&	in_xform, 
						int					in_copies,
						BYTE				control );

	static int DuplicatesFilter(
					CModel*			model,
					double			gap_tol,
					CDbCurveList*	dbCurves );

private:

	static bool IsDuplicate( double gap_tol, double dist, double uparam );

	static bool CProfileBuilder::CurvesAdjust(
		double		gap_tol,
		CDbCurve*	dbCurveA,
		CDbCurve*	dbCurveB );

private:  // Disabled.

	CProfileBuilder();
	CProfileBuilder( const CProfileBuilder& );
	virtual ~CProfileBuilder();
	const CProfileBuilder& operator = ( const CProfileBuilder& );
	int operator == ( const CProfileBuilder& ) const;
	int operator != ( const CProfileBuilder& ) const;

private:

};

#endif

