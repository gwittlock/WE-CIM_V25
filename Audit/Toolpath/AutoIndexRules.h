
#ifndef _AUTOINDEXRULES_H
#define _AUTOINDEXRULES_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DYNAMICARRAY_H
#include "DynamicArray.h"
#endif

#ifndef _3DVEC_H
#include "3dVec.h"
#endif

#ifndef _WORM_H
#include "Worm.h"
#endif

#ifndef _SHAPE_H
#include "Shape.h"
#endif

class CGeoArc;
class CAutoIndex;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CAutoIndexRule
{
	friend class CAutoIndex;

public:

	CAutoIndexRule(
				int				partIndx,
				int				toolIndx,
				double			toolRotation,
				const C3dVec&	partBackEdgeTrans,
				double			partBackEdgeRot,
				const C3dVec&	toolBackEdgeTrans,
				double			toolBackEdgeRot,
				const C3dVec&	partFrontEdgeTrans,
				double			partFrontEdgeRot,
				const C3dVec&	toolFrontEdgeTrans,
				double			toolFrontEdgeRot,
				bool			oversizedToolFace );

	virtual ~CAutoIndexRule()		{ };

protected:

	int		m_partIndx;
	int		m_toolIndx;

	double	m_toolRotation;

	C3dVec	m_partBackEdgeTrans;
	double	m_partBackEdgeRot;
	C3dVec	m_toolBackEdgeTrans;
	double	m_toolBackEdgeRot;

	C3dVec	m_partFrontEdgeTrans;
	double	m_partFrontEdgeRot;
	C3dVec	m_toolFrontEdgeTrans;
	double	m_toolFrontEdgeRot;

	bool	m_oversizedToolFace;

private:  // Disabled

	CAutoIndexRule();
	CAutoIndexRule( const CAutoIndexRule& );
	const CAutoIndexRule& operator = ( const CAutoIndexRule& );
	int operator == ( const CAutoIndexRule& ) const;
	int operator != ( const CAutoIndexRule& ) const;
};

typedef CDynamicArray<CAutoIndexRule*> CAutoIndexRuleArray;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CRotPair
{
public:

	CRotPair( int index, double radians )
		: m_index( index ), m_radians( radians )  { }

	const CRotPair& operator = ( const CRotPair& p )
		{ m_index = p.m_index; m_radians = p.m_radians;  return (*this); }
	
	int Index() const		{ return m_index; }

	double Radians() const	{ return m_radians; }

	virtual ~CRotPair( )  { };

public:

	static int Compare( const void* ptrA, const void* ptrB );

private:  // Disabled

	CRotPair();
	CRotPair( const CRotPair& );
	int operator == ( const CRotPair& ) const;
	int operator != ( const CRotPair& ) const;

private:

	int m_index;
	double m_radians;
};

typedef CDynamicArray<CRotPair*> CRotPairArray;



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CAutoIndexRules
{
public:

	CAutoIndexRules();

	void Init(
			const CShape&	part,
			const CShape&	tool,
			int				offsetDir,
			bool			use_long_side );

	// Gets the count of rules required to offset the part.
	int Count() const;

	// Gets the Ith rule.
	const CAutoIndexRule* operator [] ( int indx ) const;

	virtual ~CAutoIndexRules();

protected:

private:  // Methods

	void ToolFacesFind(
					const CGeoCurve&	pCurve,
					const CShape&		tool,
					int					offsetDir,
					int*				bestFace,
					int*				bcnt );

	void ToolFacesOrder(
					const CGeoCurve&		pCurve,
					const CShape&	tool,
					int						offsetDir,
					const int*				bestFace,
					int						bestCount,
					int*					tndx,
					double*					trot );

	bool OutsideCut( const CGeoArc* pArc, int offsetDir );

	double CandidateFaceMeasure(
					const CGeoCurve*	partCurve,
					const CGeoCurve*	toolCurve,
					int					offsetDir );

	double ToolRotation(
					const CGeoCurve*	partCurve,
					const CGeoCurve*	toolCurve,
					int					offsetDir );

	void RulesCreate(
					const CShape&	part,
					int				pndx,
					const CShape&	tool,
					int				tndx,
					double			trot,
					int				offsetDir );

	void PartXformParams(
					EEdge				orient,
					const CGeoCurve*	curve,
					C3dVec*				trans,
					double*				radians );

	void ToolXformParams(
					EEdge				orient,
					const CGeoCurve*	pCurve,
					const CGeoCurve*	tCurve,
					int					offsetDir,
					double				pRotation,
					double				tRotation,
					C3dVec*				trans,
					double*				radians );

	double Rotation( const C2dUnitVec& partTan, const C2dUnitVec& toolTan );

	bool Comparable(
					const CGeoCurve&	partCurve,
					const CGeoCurve&	toolCurve,
					int					offsetDir,
					int					lineCount );

private:  // Disabled

	CAutoIndexRules( const CAutoIndexRules& );
	const CAutoIndexRules& operator = ( const CAutoIndexRules& );
	int operator == ( const CAutoIndexRules& ) const;
	int operator != ( const CAutoIndexRules& ) const;

private:  // Data

	CAutoIndexRuleArray m_rules;

	bool	m_use_long_side;
};

#endif

