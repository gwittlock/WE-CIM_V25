
#ifndef _PUNCHMATCHER_H
#define _PUNCHMATCHER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _SHAPEMATCHER_H
#include "ShapeMatcher.h"
#endif

class CToolSetup;



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Abstract base class.
//
class dllExport CPunchMatcher : public CShapeMatcher
{
public:

	CPunchMatcher();

	void Tolerances( double plus, double minus );

	virtual int ExactMatch( const CToolSetup& toolSetup, TShapeArray& exemplars ) const;

	virtual int AdequateMatch( const CToolSetup& toolSetup, TShapeArray& exemplars ) const;

	virtual CShape* ExactMatch( const TSortedShapes& catalog, const CShape& exemplar ) const;

	virtual CShape* AdequateMatch( const TSortedShapes& catalog, const CShape& exemplar ) const;

	virtual ~CPunchMatcher();

protected:

private:  // Methods

	CShape* ExactMatch( const TShapeArray& candidateShapes, const CShape& exemplar ) const;

	void MatchingShapesFind(
				const TShapeArray&	candidateShapes,
				const CString&		exemplarParams,
				TShapeArray*		matchingShapes ) const;

	bool ParamMatch(
				const CString&	type,
				const CString&	lowerBound,
				const CString&	upperBound,
				const CString&	value ) const;

	CShape* BestShapeFind(
						const TShapeArray&	candidateShapes,
						double				orientation ) const;

	void AttribsSet( const CShape* matchingShape, CShape* exemplar ) const;

private:  // Disabled

	CPunchMatcher( const CPunchMatcher& );
	const CPunchMatcher& operator = ( const CPunchMatcher& );
	int operator == ( const CPunchMatcher& ) const;
	int operator != ( const CPunchMatcher& ) const;

private:  // Data

	double	m_ptol;  // 'plus' matching tolerance
	double	m_mtol;  // 'minus' matching tolerance
};

#endif

