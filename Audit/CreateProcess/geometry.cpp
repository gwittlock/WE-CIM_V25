// ==================================================================
// Geometry.cpp : Create Geometry
//
// ==================================================================

#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"

#include "dbPoint.h"
#include "dbHole.h"
#include "dbLine.h"
#include "dbArc.h"
#include "dbWorkplane.h"
#include "DbTool.h"
#include "dbCommand.h"

#include "ViewMgr.h"

#include "DbProfile.h"
#include "DbFeature.h"

#include "Profile.h"
#include "Worm.h"
#include "Conversion.h"

#include "CreateProcess.h"

// ==================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ==================================================================
// Create:Point: [id], x, y, z
// Returns id = %d of created/modified point
//
CReturn 
CCreateProcessApp::Point( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn	ret;
	ID		id = 0;
	bool	modify;

	// -----------------------------------------------------
	//		Extract command
	//
	double	px, py, pz;
	
	io_cmd->getInt( "id", (int*) &id );

	ret += io_cmd->getReal( "x", &px );
	ret += io_cmd->getReal( "y", &py );
	ret += io_cmd->getReal( "z", &pz );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_CREATE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Check for modify
	//
	CDbPoint* point = NULL;

	if (id)
	{
		ret += model.EntityFind( id, (CDbEntity**)&point, DBPOINT, DBPOINT );
		ASSERT( point != NULL);
		if ( point->Type() != DBPOINT )
		{
			ret.Internal( IDS_MODIFY_TYPE );
			return ret;
		}
	}
	modify = (point != NULL);

	// -----------------------------------------------------
	//		Now, get the point
	//
	CDbWorkplane* work = NULL;
	CDbTool* tool = NULL;

	if (modify)
	{
		id = point->Id();

		tool = (CDbTool*)point->Tool();
		work = (CDbWorkplane*)point->Workplane();
	}
	else
	{
		tool = model.ActiveTool();
		work = model.ActiveWorkplane();
		if (!tool || !work)
			return ret;

		ret += model.EntityCreate( DBPOINT, (CDbEntity**)&point );

		if (point != NULL && model.ActivePattern() != NULL)
			model.ActivePattern()->Append( point );
	}

	id = point->Id();

	// -----------------------------------------------------
	// Fill the contents
	//
	if (ret.isOkay())
	{
		point->Init( tool, work, C3dCoord( px, py, pz ) );
		point->SystemFlag( false );

		if ( modify )
			point->ModifyFlag( true );

		io_cmd->setInt( "id", id );
//#if OKAY
		io_cmd->getViewMgr().ModelSet( model );
		io_cmd->getViewMgr().Refresh( id, TRUE );
//#endif
	}

	return ret;
}


// ==================================================================
// Create:Command: [id=%d], x=%g, y=%g, z=%g, cmd=%s, angle=%g, side=%d
// Returns id = %d of created/modified point
//
CReturn 
CCreateProcessApp::Command( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn	ret;
	ID		id = 0;
	bool	modify;

	// -----------------------------------------------------
	//		Extract command
	//
	double	px, py, pz;
	CString	text;
	
	io_cmd->getInt( "id", (int*) &id );

	ret += io_cmd->getReal( "x", &px );
	ret += io_cmd->getReal( "y", &py );
	ret += io_cmd->getReal( "z", &pz );
	ret += io_cmd->getString( "cmd", &text );

	double angle = 0.0;
	io_cmd->getReal( "angle", &angle );

	eDisplayTextPos pos = TEXTPOS_DEFAULT;
	io_cmd->getInt( "pos", (int*)&pos );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_CREATE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Check for modify
	//
	CDbCommand* cmd = NULL;

	if (id)
	{
		ret += model.EntityFind( id, (CDbEntity**)&cmd, DBCOMMAND, DBCOMMAND );
		ASSERT( cmd != NULL);
		if ( cmd->Type() != DBCOMMAND )
		{
			ret.Internal( IDS_MODIFY_TYPE );
			return ret;
		}
	}
	modify = (cmd != NULL);

	// -----------------------------------------------------
	//		Now, get the command
	//
	CDbWorkplane* work = NULL;
	CDbTool* tool = NULL;

	if (modify)
	{
		id = cmd->Id();

		tool = (CDbTool*)cmd->Tool();
		work = (CDbWorkplane*)cmd->Workplane();
	}
	else
	{
		tool = model.ActiveTool();
		work = model.ActiveWorkplane();
		if (!tool || !work)
			return ret;

		ret += model.EntityCreate( DBCOMMAND, (CDbEntity**)&cmd );

		if (cmd != NULL && model.ActivePattern() != NULL)
			model.ActivePattern()->Append( cmd );
	}

	id = cmd->Id();

	// -----------------------------------------------------
	// Fill the contents
	//
	if (ret.isOkay())
	{
		cmd->Init( tool, work, C3dCoord( px, py, pz ), text );
		cmd->SystemFlag( false );

		cmd->DoubleSet( "angle", angle );
		cmd->IntSet( "pos", pos );

		if ( modify )
			cmd->ModifyFlag( true );

		io_cmd->setInt( "id", id );
//#if OKAY
		io_cmd->getViewMgr().ModelSet( model );
		io_cmd->getViewMgr().Refresh( id, TRUE );
//#endif
	}

	return ret;
}


// Experimental
//   See also http://en.wikipedia.org/wiki/Elliptical
/*
	if (steps == null)
		steps = 36;
	var points = [];

	// Angle is given by Degree Value
	var beta = -angle * (Math.PI / 180); //(Math.PI/180) converts Degree Value into Radians
	var sinbeta = Math.sin(beta);
	var cosbeta = Math.cos(beta);

	for (var i = 0; i < 360; i += 360 / steps) 
	{
		var alpha = i * (Math.PI / 180) ;
		var sinalpha = Math.sin(alpha);
		var cosalpha = Math.cos(alpha);

		var X = x + (a * cosalpha * cosbeta - b * sinalpha * sinbeta);
		var Y = y + (a * cosalpha * sinbeta + b * sinalpha * cosbeta);

		points.push(new OpenLayers.Geometry.Point(X, Y));
	}

	return points;
 */
 // Create:Ellipse: xc=%f. yc=%f, a=%f, b=%f, orient=%f, count=%d
CReturn 
CCreateProcessApp::Ellipse( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	const CVarList& params = io_cmd->VarList();

	// Center point coordinates.
	double xc = params.getReal( "xc", 0. );
	double yc = params.getReal( "yc", 0. );

	// Major axis dimension.
	double a = params.getReal( "a", UNDEFINED );

	// Minor axis dimension.
	double b = params.getReal( "b", UNDEFINED );

	// Orientation of the major axis.
	double orient = params.getReal( "orient", 0. );

	// Number of segments in one quadrant
	int count = params.getInt( "count", 0 );

	if ((a < UNDEFINED) && (b < UNDEFINED) && (count > 0))
	{
		C3dCoordArray pts;
		int indx;

		double beta = orient * DEG2RAD;
		double sinbeta = sin(beta);
		double cosbeta = cos(beta);

		double delta = HALFPI / (double) count;  // one quadrant
		double alpha = 0.;
		// count *= 4;
		for (indx = 0; indx <= count; ++indx)
		{
			double sinalpha = sin(alpha);
			double cosalpha = cos(alpha);

			double xp = xc + (a * cosalpha * cosbeta - b * sinalpha * sinbeta);
			double yp = yc + (a * cosalpha * sinbeta + b * sinalpha * cosbeta);

			pts.Append( new C3dCoord( xp, yp, 0. ) );

			alpha += delta;  // or alpha = (delta * (indx+1));
		}

		model.UndoBufferPrepare();

		CDbWorkplane* dbWork = model.ActiveWorkplane();

		CDbTool* dbTool;
		model.EntityFind( "Ellipse", (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if (dbTool == NULL)
		{
			model.EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
			dbTool->Name("Ellipse");
			dbTool->Workplane( dbWork );
			dbTool->ColorSet( DCOLOR_GREEN );
		}
	
		CDbProfile* dbProfile;
		model.EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );

		count = pts.Count() - 1;
		for (indx = 0; indx < count; ++indx)
		{
			const C3dCoord& ps = *pts.GetAt(indx);
			const C3dCoord& pe = *pts.GetAt(indx+1);
			if ( !ps.WithinTolXY( pe, SMALL ) )
			{
				CDbLine* dbLine;
				model.EntityCreate( DBLINE, (CDbEntity**) &dbLine );

				dbLine->Init( dbTool, dbWork, ps, pe );

				dbProfile->Append( dbLine );
			}
		}

		model.UndoBufferCommit();

		io_cmd->getViewMgr().Refresh( true );

		pts.DestructiveFlush();

#if 0
		io_cmd->setInt( "method", 0 );
		io_cmd->setString( "name", "ArcApprox" );
		io_cmd->setInt( "color", DCOLOR_YELLOW );
		status = EllipseApproximate( io_cmd );
#endif
		io_cmd->setInt( "method", 1 );
		io_cmd->setString( "name", "LChordApprox" );
		io_cmd->setInt( "color", DCOLOR_MAGENTA );
		status = EllipseApproximate( io_cmd );

		io_cmd->setInt( "method", 2 );
		io_cmd->setString( "name", "SubdivApprox" );
		io_cmd->setInt( "color", DCOLOR_CYAN );
		status = EllipseApproximate( io_cmd );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Ellipse(#1)" );
	}

	return status;
}

CReturn
CCreateProcessApp::EllipseApproximate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	const CVarList& params = io_cmd->VarList();

	// Center point coordinates.
	double xc = params.getReal( "xc", 0. );
	double yc = params.getReal( "yc", 0. );

	// Major axis dimension.
	double a = params.getReal( "a", UNDEFINED );

	// Minor axis dimension.
	double b = params.getReal( "b", UNDEFINED );

	// Orientation of the major axis.
	double orient = params.getReal( "orient", 0. );

	int method = params.getInt( "method", 0 );
	int color = params.getInt( "color", DCOLOR_RED );
	CString name = params.getString( "name", "FOOBAR" );
	double maxdev = params.getReal( "maxdev", 5.e-3 );

	if ((a < UNDEFINED) && (b < UNDEFINED))
	{
		C3dCoordArray pts;
		C3dCoord pc( xc, yc, 0. );

		if (method == 0)
			status = EllipseApproximate( pc, a, b, orient, 8, &pts );
		else
			status = EllipseApproximate( method, pc, a, b, maxdev, orient, &pts );

		model.UndoBufferPrepare();

		CDbWorkplane* dbWork = model.ActiveWorkplane();

		CDbTool* dbTool;
		model.EntityFind( name, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if (dbTool == NULL)
		{
			model.EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
			dbTool->Name( name );
			dbTool->Workplane( dbWork );
			dbTool->ColorSet( color );
		}
	
		CDbProfile* dbProfile;
		model.EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );

		int count = pts.Count() - 1;
		for (int indx = 0; indx < count; ++indx)
		{
			const C3dCoord& ps = *pts.GetAt(indx);
			const C3dCoord& pe = *pts.GetAt(indx+1);
			if ( !ps.WithinTolXY( pe, SMALL ) )
			{
				CDbLine* dbLine;
				model.EntityCreate( DBLINE, (CDbEntity**) &dbLine );

				dbLine->Init( dbTool, dbWork, ps, pe );

				dbProfile->Append( dbLine );
			}
		}

		model.UndoBufferCommit();

		io_cmd->getViewMgr().Refresh( true );

		pts.DestructiveFlush();
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::EllipseApproximate(#1)" );
	}

	return status;
}

CReturn
CCreateProcessApp::EllipseApproximate(
	const C3dCoord&	pc,
	double			a,
	double			b,
	double			orient,
	int				narcs,
	C3dCoordArray*	pts )
{
	CReturn	status;

	C3dCoord*	pt;
	int n;

	double beta = orient * DEG2RAD;
	double sinbeta = sin(beta);
	double cosbeta = cos(beta);

	// We generate p[n+2] end points in the interval (0 <= n <= pi/2),
	// where p[0] = (a,0) and p[n-1] = (0,b). We subsequently prepend
	// p[0] = (p[1].x, -p[1].y) and append p[n] = (-p[n-1].x, p[n-1].y)

	double curvA0 = a / (b * b);
	double curv0B = b / (a * a );
	double invN = 1. / (double) narcs;

	double a2 = a * a;
	double b2 = b * b;
	double denom = a2 - b2;
	double exp = 2. / 3.;

	double curvN, lambda;
	double term, xp, yp;

	for (n = 0; n <= narcs; ++n)
	{
		curvN = ((1. - (n * invN)) * curvA0) + ((n * invN) * curv0B);

		lambda = pow( ((a * b) / curvN), exp );

		term = fabs( (lambda - a2) / denom );
		xp = a * sqrt( term );

		term = fabs( (lambda - b2) / denom );
		yp = b * sqrt( term );

		pts->Append( new C3dCoord( xp, yp, 0. ) );
	}

	pt = pts->GetAt( narcs-1 );
	pts->Append( new C3dCoord( -pt->X(), pt->Y(), pt->Z() ) );

	pt = pts->GetAt( 1 );
	pts->Prepend( new C3dCoord( pt->X(), -pt->Y(), pt->Z() ) );

	return status;
}

CReturn
CCreateProcessApp::EllipseApproximate(
	int				method,
	const C3dCoord&	pc,
	double			a,
	double			b,
	double			maxdev,
	double			orient,
	C3dCoordArray*	pts )
{
	CReturn	status;

	if (method == 1)
	{
		double xe, ye;

		double xs = 0.;
		double ys = b;

		pts->Prepend( new C3dCoord( xs, ys, 0. ) );
		while (1)
		{
			ChordCalc( a, b, maxdev, xs, ys, &xe, &ye );
			pts->Prepend( new C3dCoord( xe, ye, 0. ) );

			double delta = a - xs;
			if ((delta < SMALL) || (delta > a))
				break;

			xs = xe;
			ys = ye;
		}
	}
	else if (method == 2)
	{
		C3dCoord result;

		// Subdivision solution.
		pts->Append( new C3dCoord( 0., b, 0. ) );
		pts->Append( new C3dCoord( a, 0., 0. ) );

		int indx = 0;
		while (1)
		{
			int count = pts->Count() - 1;
			if (indx >= count)
				break;

			C3dCoord* ps = pts->GetAt(indx);
			C3dCoord* pe = pts->GetAt(indx+1);
			DeviationCalc( a, b, ps->X(), ps->Y(), pe->X(), pe->Y(), &result );
			if (result.Z() <= maxdev)
			{
				++indx;
			}
			else
			{
				result.Z(0.);
				pts->InsertAfter( indx, new C3dCoord( result ) );
			}
		}
	}

	return status;
}

void
CCreateProcessApp::ChordCalc(
	double a, double b, double maxdev,
	double xs, double ys, double* xe, double* ye )
{
	C3dCoord result;
	double dx;

	double a2 = a * a;
	double b2 = b * b;

	(*xe) = a;
	dx = (*xe) - xs;
	while (1)
	{
		double xe2 = (*xe) * (*xe);
		(*ye) = sqrt(b2 * (1. - xe2 / a2));

		DeviationCalc( a, b, xs, ys, (*xe), (*ye), &result );

		if (dx < SMALL)
		{
			// The normal terminal condition.
			break;
		}

		dx /= 2.;

		double xtmp = (*xe) + ((result.Z() > maxdev) ? -dx : dx);
		if (xtmp > a)
		{
			// A boundary terminal condition.
			// ASSUMPTION: This is the terminal chord and we arrived
			// here because 'dev' can never approach 'maxdev'.
			(*xe) = a;
			(*ye) = 0.;
			break;
		}

		(*xe) = xtmp;
	}
}

void
CCreateProcessApp::DeviationCalc(
	double a, double b, double xs, double ys, double xe, double ye, C3dCoord* result )
{
	double a2 = a * a;
	double b2 = b * b;

	double dy = ye - ys;
	double dx = xe - xs;

	double dx2 = dx * dx;
	double dy2 = dy * dy;
	double dydx = dy * dx;

	double denom = sqrt( (b2 * dx2) + (a2 * dy2) );
	double x0 = -(a2 * dy) / denom;
	double y0 = (b2 * dx) / denom;

	denom = dx2 + dy2;
	double xp = ((y0 - ys) * dydx + dy2 * xs + dx2 * x0) / denom;
	double yp = ((x0 - xs) * dydx + dy2 * y0 + dx2 * ys) / denom;

	dy = yp - y0;
	dx = xp - x0;

	result->XYZ( x0, y0, sqrt( dx*dx + dy*dy ) );
}
