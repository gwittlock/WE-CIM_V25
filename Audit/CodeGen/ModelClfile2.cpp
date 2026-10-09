 
#include "stdafx.h"
#include "MathConst.h"
#include "StringConst.h"
#include "ClfileConsts.h"
#include "cmn_resource.h"

#include "Return.h"
#include "VarList.h"
#include "3dCoord.h"
#include "GeoCurve.h"
#include "GeoArc.h"
#include "DbEntity.h"
#include "DbCommand.h"
#include "SpeedCalc.h"
#include "Model.h"
#include "ModelClfile.h"

static int DEBUGSPEED = 0;

static bool IsSlowArc( const CGeoCurve* geoCurve, double minRadius, double sharp );



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn
CModelClfile::AdditionalFormatting()
{
	CReturn status;

	if ( m_speedramp )
		status = SpeedRampingApply();

	return status;
}

CReturn
CModelClfile::SpeedRampingApply()
{
	CReturn status;

	CSpeedCalc calc;

	double sharp = 0.0;
	double minRadius = UNDEFINED;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Preprocessing loop.
	// Mark the sequences requiring speed ramping.

	ClfileRec* recA = NULL;
	ClfileRec* recB = NULL;
	const CVarList* sfparams = NULL;

	int indx = 0;
	while (1)
	{
		if (indx >= m_clfile.Count())
			break;

		recB = m_clfile[indx];

		if (recB != NULL)
		{
			if (recB->RecType() == EV_SPEED_RAMP_INFO)
			{
				sfparams = recB->Attrib();
				// sfparams->Dump();
				minRadius = sfparams->getReal( "Slowdown_Radius", 0. );
				sharp = sfparams->getReal( "Slowdown_Angle", 0. ) * DEG2RAD;
			}
			
			if ((recA != NULL) && (sfparams != NULL))
			{
				SpeedPreproc( sharp, minRadius, recA, recB );
			}
		}

		recA = recB;
		++indx;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Postprocessing loop.
	// Expand the sequences requiring speed ramping.
	indx = 0;
	recA = NULL;
	recB = NULL;
	while (1)
	{
		if (indx >= m_clfile.Count())
			break;

		recA = m_clfile[indx];

		if (recA != NULL)
		{
			if (recA->RecType() == EV_SPEED_RAMP_INFO)
			{
				sfparams = recA->Attrib();
				calc.Init( (*sfparams) );
			}
			else if ( calc.IsOk() )
			{
				indx = SpeedPostproc( indx, calc );
			}
		}

		++indx;
	}

	return status;
}

void
CModelClfile::SpeedPreproc( double sharp, double minRadius, ClfileRec* recA, ClfileRec* recB )
{
	CGeoCurve* curveA = dynamic_cast<CGeoCurve*>( recA->Elem() );
	if (curveA != NULL)
	{
		CGeoCurve* curveB = dynamic_cast<CGeoCurve*>( recB->Elem() );
		if (curveB != NULL)
		{
			const C3dCoord& ptA = curveA->EndPt();
			const C3dCoord& ptB = curveB->StartPt();

			if ( ptA.WithinTol( ptB, SMALL ) )
			{
				CString spramp;

				C2dUnitVec vecA = curveA->EndTan();
				C2dUnitVec vecB = curveB->StartTan();

				bool isSlowArcA = IsSlowArc( curveA, minRadius, sharp );
				bool isSlowArcB = IsSlowArc( curveB, minRadius, sharp );

				double dot = vecA * vecB;
				double angle = ((dot > (1. - SMALL)) ? PI : PI - acos( dot ));

				bool slowdown = ((angle < sharp || isSlowArcB) && !isSlowArcA);
				bool speedup  = ((angle < sharp || isSlowArcA) && !isSlowArcB);

				if ( slowdown )
				{
					spramp = (curveA->StringGet( "spramp", "" ) + "-");
					curveA->StringSet( "spramp", spramp );
				}

				if ( speedup )
				{
					spramp = (curveB->StringGet( "spramp", "" ) + "+");
					curveB->StringSet( "spramp", spramp );
				}
			}
		}
	}
}

// Replaces the Ith clfile record (representing a curve requiring speed ramping)
// with N clfile records (representing the tabulated counterparts with speed
// ramping parameters applied).
//
// Returns the indx of the next clfile record that should be processed.
int
CModelClfile::SpeedPostproc( int indx, const CSpeedCalc& calc )
{
	ClfileRec* rec = m_clfile[indx];
	if (rec == NULL)
		return indx;

	CGeoCurve* curve = dynamic_cast<CGeoCurve*>( rec->Elem() );
	if (curve == NULL)
		return indx;

	const CVarList* attribs = curve->pAttrib();
	if (attribs == NULL)
		return indx;

	CString spramp = curve->StringGet( "spramp", "" );
	if ( spramp.IsEmpty() )
		return indx;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Replace the current record with the expanded set
	// that has all of the speed ramping applied.
	bool accel = (spramp.Find( '+' ) >= 0);
	bool decel = (spramp.Find( '-' ) >= 0);

	int nIntervals = (int) (curve->Length2d() / calc.IntervalLength());

	if (accel && decel)
	{
		if (nIntervals > (2 * calc.Divisions()))
			nIntervals = calc.Divisions();
		else
			nIntervals /= 2;
	}
	else
	{
		if (nIntervals > calc.Divisions())
			nIntervals = calc.Divisions();
	}

	if ( accel )
		Accel( nIntervals, calc, &indx );

	if ( decel )
		Decel( nIntervals, calc, &indx );

	return indx;
}

// Given the curve A and the speed ramping parameters,
// replaces A with B C D A'
CReturn
CModelClfile::Accel( int ndivs, const CSpeedCalc& calc, int* indx )
{
	CReturn status;

	int jndx = (*indx);

	ClfileRec* rec = m_clfile[jndx];

	CGeoCurve* curve = dynamic_cast<CGeoCurve*>( rec->Elem() );
	const CDbEntity* dbEntity = rec->Entity();

	CVarList attribs( (*(rec->Attrib())) );

	int rectype = rec->RecType();

	for (int cnt = 0; cnt < ndivs; ++cnt)
	{
		C3dCoord pt = curve->PointAtDist( calc.IntervalLength(), TRUE );

		CGeoCurve* interval = dynamic_cast<CGeoCurve*>( curve->Clone( false ) );

		interval->EndPt( pt );
		curve->StartPt( pt );

		status = RecInsertBefore( jndx, rectype, &rec );
		if ( !status.IsOk() )
			break;

		rec->Elem( interval );
		rec->Entity( dbEntity );

		attribs.setReal( STR_FEED, calc.Feed( cnt+1 ) );
		rec->AttribsCopy( attribs );

		if ( DEBUGSPEED ) CondSpeedRampShow( pt, calc, (cnt+1) );

		++jndx;
	}

	(*indx) = jndx;

	return status;
}

// Given the curve A and the speed ramping parameters,
// replaces A with A' D C B
CReturn
CModelClfile::Decel( int ndivs, const CSpeedCalc& calc, int* indx )
{
	CReturn status;

	int jndx = (*indx);

	ClfileRec* rec = m_clfile[jndx];

	CGeoCurve* curve = dynamic_cast<CGeoCurve*>( rec->Elem() );
	const CDbEntity* dbEntity = rec->Entity();
	
	CVarList attribs( (*(rec->Attrib())) );

	int rectype = rec->RecType();

	for (int cnt = 0; cnt < ndivs; ++cnt)
	{
		C3dCoord pt = curve->PointAtDist( -calc.IntervalLength(), FALSE );

		CGeoCurve* interval = dynamic_cast<CGeoCurve*>( curve->Clone( false ) );

		interval->StartPt( pt );
		curve->EndPt( pt );

		status = RecInsertAfter( (*indx), rectype, &rec );
		if ( !status.IsOk() )
			break;

		rec->Elem( interval );
		rec->Entity( dbEntity );

		attribs.setReal( STR_FEED, calc.Feed( cnt ) );
		rec->AttribsCopy( attribs );

		if ( DEBUGSPEED ) CondSpeedRampShow( pt, calc, cnt );

		++jndx;
	}

	(*indx) = jndx;

	return status;
}

void
CModelClfile::CondSpeedRampShow( const C3dCoord& pt, const CSpeedCalc& calc, int indx )
{
	CString text;
	CDbCommand* dbcmd;
	((CModel&) m_model).EntityCreate( DBCOMMAND, (CDbEntity**) &dbcmd );
	text.Format( "-%-d", (int) calc.Feed( indx ) );
	dbcmd->Init( m_model->ActiveTool(), m_model->ActiveWorkplane(), pt, text );
}

bool IsSlowArc( const CGeoCurve* geoCurve, double minRadius, double sharp )
{
	const CGeoArc* arc = dynamic_cast<const CGeoArc*>( geoCurve );
	if (arc == NULL)
		return FALSE;

	double radius = arc->Radius();
	if (radius > minRadius)
		return FALSE;

	double angle = arc->IncludedAngle();
	return (angle > sharp);
}

