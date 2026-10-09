
#include "stdafx.h"

#include "ActionConst.h"
#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "Portal.h"
#include "dbPoint.h"
#include "dbLine.h"
#include "dbArc.h"
#include "Solution.h"
#include "ViewMgr.h"

#include "CreateProcess.h"


// Cloned from weGeoLine.cpp.
// NOTE: We may need to move these to some common area
// so as to avoid dependencies between weng & ci.
typedef enum
{
	LINE_2PT		= 101,
	LINE_LEN_ANG	= 102,
	LINE_PS_TAN		= 103,
	LINE_TAN_PE		= 104,
	LINE_TAN_TAN	= 105
} eLineDefn;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create a new line
//		Create:Line: action=WECREATE, sncs=LINE_2PT,
//					xs=%f, ys=%f, zs=%f, xe=%f, ye=%f, ze=%f
//		Create:Line: action=WECREATE, sncs=LINE_LEN_ANG,
//					xs=%f, ys=%f, zs=%f, len=%f, ang=%f
//		Create:Line: action=WECREATE, sncs=LINE_PS_TAN,
//					xs=%f, ys=%f, zs=%f, te=%d, xe=%f, ye=%f, ze=%f
//		Create:Line: action=WECREATE, sncs=LINE_TAN_PE,
//					ts=%d, xs=%f, ys=%f, zs=%f, xe=%f, ye=%f, ze=%f
//		Create:Line: action=WECREATE, sncs=LINE_TAN_TAN,
//					ts=%d, xs=%f, ys=%f, zs=%f, te=%d, xe=%f, ye=%f, ze=%f
//		Returns (s) id
//
// Update an existing line
//		commands are same a those for creating lines except:
//			Create:Line: action=WEUPDATE, id=%d, ...
//		Returns (s) id
//
// Query a line
//		Create:Line: action=WEQUERY, id=%d
//		Returns (s) id
//				(d) ang (radians)
//
CReturn 
CCreateProcessApp::Line( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	eWeAction	action;
	eLineDefn	sncs;

	action = (eWeAction) io_cmd->VarList().getInt( "action", IUNDEFINED );
	sncs = (eLineDefn) io_cmd->VarList().getInt( "sncs", IUNDEFINED );

	if (action == WECREATE || action == WEUPDATE)
	{
		switch (sncs)
		{
		case LINE_2PT:
			status = line_2pt( io_cmd );
			break;

		case LINE_LEN_ANG:
			status = line_len_ang( io_cmd );
			break;

		case LINE_PS_TAN:
			status = line_ps_tan( io_cmd );
			break;

		case LINE_TAN_PE:
			status = line_tan_pe( io_cmd );
			break;

		case LINE_TAN_TAN:
			status = line_tan_tan( io_cmd );
			break;

		default:
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Line(#1)" );
			io_cmd->setInt( "id", 0 );
		}
	}
	else if (action == WEQUERY)
	{
		status = line_query( io_cmd );
	}
	else
	{
		// status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Line(#2)" );
		status = OldLine( io_cmd );  // punt
		// io_cmd->setInt( "id", 0 );  // failed.
	}

	return status;
}

//	Create:Line: action=WECREATE, sncs=LINE_2PT,
//				xs=%f, ys=%f, zs=%f, xe=%f, ye=%f, ze=%f
CReturn 
CCreateProcessApp::line_2pt( CCommand* io_cmd )
{
	CReturn		status;

	CDbLine*	dbLine;
	C3dCoord	ps, pe;
	ID			id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure

	dbLine = GetLine( io_cmd );

	if (dbLine == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_2pt(#1)" );
	}
	else
	{
		ps = GetPt( io_cmd, 's' );
		pe = GetPt( io_cmd, 'e' );

		status = dbLine->Init( ps, pe );

		id = dbLine->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}

	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Line: action=WECREATE, sncs=LINE_LEN_ANG,
//				xs=%f, ys=%f, zs=%f, len=%f, ang=%f
CReturn
CCreateProcessApp::line_len_ang( CCommand* io_cmd )
{
	CReturn		status;

	CDbLine*	dbLine;
	C3dCoord	ps, pe;
	double		len, ang;
	ID			id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure

	dbLine = GetLine( io_cmd );

	if (dbLine == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_len_ang(#1)" );
	}
	else
	{
		ps = GetPt( io_cmd, 's' );

		len = io_cmd->VarList().getReal( "len", 0. );
		ang = io_cmd->VarList().getReal( "ang", 0. ) * DEG2RAD;

		pe.XYZ( (ps.X() + (len * cos(ang))), (ps.Y() + (len * sin(ang))), ps.Z() );

		status = dbLine->Init( ps, pe );

		id = dbLine->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}

	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Line: action=WECREATE, sncs=LINE_PS_TAN,
//				xs=%f, ys=%f, zs=%f,
//				te=%d, xe=%f, ye=%f, ze=%f
CReturn
CCreateProcessApp::line_ps_tan( CCommand* io_cmd )
{
	CReturn		status;

	CDbLine*	dbLine;
	CDbArc*		dbArc;
	C3dCoord	ps, pe, ph;
	CGeoArc*	geoArc;
	int			nsoln;
	ID			te, id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure

	dbLine = GetLine( io_cmd );

	if (dbLine == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_ps_tan(#1)" );
	}
	else
	{
		te = io_cmd->VarList().getInt( "te", 0 );

		model->EntityFind( te, (CDbEntity**) &dbArc, DBARC, DBARC );

		if (dbArc == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_ps_tan(#2)" );
		}
		else
		{
			ps = GetPt( io_cmd, 's' );
			ph = GetPt( io_cmd, 'e' );  // hint for soln.
			geoArc = dbArc->Arc();

			nsoln = CSolution::line_ps_tan( ps, (*geoArc), ph, &pe );
			if (nsoln <= 0)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_ps_tan(#3)" );
			}
			else
			{
				status = dbLine->Init( ps, pe );
			}

			delete geoArc;
		}

		id = dbLine->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Line: action=WECREATE, sncs=LINE_TAN_PE,
//				ts=%d, xs=%f, ys=%f, zs=%f,
//				xe=%f, ye=%f, ze=%f
CReturn
CCreateProcessApp::line_tan_pe( CCommand* io_cmd )
{
	CReturn		status;

	CDbLine*	dbLine;
	CDbArc*		dbArc;
	C3dCoord	ps, pe, ph;
	CGeoArc*	geoArc;
	ID			ts, id;
	int			nsoln;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure

	dbLine = GetLine( io_cmd );

	if (dbLine == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_tan_pe(#1)" );
	}
	else
	{
		ts = io_cmd->VarList().getInt( "ts", 0 );

		model->EntityFind( ts, (CDbEntity**) &dbArc, DBARC, DBARC );

		if (dbArc == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_tan_pe(#2)" );
		}
		else
		{
			ph = GetPt( io_cmd, 's' );  // hint for soln.
			pe = GetPt( io_cmd, 'e' );
			geoArc = dbArc->Arc();

			nsoln = CSolution::line_ps_tan( pe, (*geoArc), ph, &ps );
			if (nsoln <= 0)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_ps_tan(#3)" );
			}
			else
			{
				status = dbLine->Init( ps, pe );
			}

			delete geoArc;
		}

		id = dbLine->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Line: action=WECREATE, sncs=LINE_TAN_TAN,
//				ts=%d, xs%f, ys%f, zs%f,
//				te=%d, xe%f, ye%f, ze%f
CReturn
CCreateProcessApp::line_tan_tan( CCommand* io_cmd )
{
	CReturn		status;

	CDbLine*	dbLine;
	CDbArc*		dbArcA;
	CDbArc*		dbArcB;
	CGeoArc*	geoArcA;
	CGeoArc*	geoArcB;
	C3dCoord	ps, pe;
	C3dCoord	hintA, hintB;
	ID			ts, te, id;
	int			nsoln;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure

	dbLine = GetLine( io_cmd );

	if (dbLine == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_tan_tan(#1)" );
	}
	else
	{
		ts = io_cmd->VarList().getInt( "ts", 0 );
		hintA = GetPt( io_cmd, 's' );  // hint for soln.
		te = io_cmd->VarList().getInt( "te", 0 );
		hintB = GetPt( io_cmd, 'e' );  // hint for soln.

		model->EntityFind( ts, (CDbEntity**) &dbArcA, DBARC, DBARC );
		model->EntityFind( te, (CDbEntity**) &dbArcB, DBARC, DBARC );

		if (dbArcA == NULL || dbArcB == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_tan_tan(#2)" );
		}
		else
		{
			geoArcA = dbArcA->Arc();
			geoArcB = dbArcB->Arc();

			nsoln = CSolution::line_tan_tan(
					 (*geoArcA), hintA, (*geoArcB), hintB, &ps, &pe );

			if (nsoln <= 0)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_tan_tan(#3)" );
			}
			else
			{
				status = dbLine->Init( ps, pe );
			}

			delete geoArcA;
			delete geoArcB;
		}

		id = dbLine->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

CDbLine*
CCreateProcessApp::GetLine( CCommand* io_cmd )
{
	eWeAction	action;
	ID			id;

	CModel* model = CPortal::Model();

	action = (eWeAction) io_cmd->VarList().getInt( "action", IUNDEFINED );

	if (action == WECREATE)
	{
		CDbLine* dbLine;
		model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
		if (dbLine != NULL)
		{
			dbLine->Tool( model->ActiveTool() );
			dbLine->Workplane( model->ActiveWorkplane() );

			if (model->ActivePattern() != NULL)
				model->ActivePattern()->Append( dbLine );
		}
		return dbLine;
	}
	else
	{
		CDbLine* dbLine;

		id = io_cmd->VarList().getInt( "id", 0 );
		model->EntityFind( id, (CDbEntity**) &dbLine, DBLINE, DBLINE );

		return dbLine;
	}
}

CReturn
CCreateProcessApp::line_query( CCommand* io_cmd )
{
	CReturn		status;
	CDbLine*	dbLine;

	dbLine = GetLine( io_cmd );

	if (dbLine == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::line_query(#1)" );
		io_cmd->setInt( "id", 0 );  // failed.
	}
	else
	{
		CGeoLine* geoLine = dbLine->Line();
		io_cmd->setReal( "ang", geoLine->StartTan().Radians() );
		delete geoLine;
	}

	return status;
}

C3dCoord
CCreateProcessApp::GetPt( CCommand* io_cmd, char what )
{
	CString	param;
	double	xp, yp, zp;

	param.Format( "x%c", what );
	xp = io_cmd->VarList().getReal( param, 0. );

	param.Format( "y%c", what );
	yp = io_cmd->VarList().getReal( param, 0. );

	param.Format( "z%c", what );
	zp = io_cmd->VarList().getReal( param, 0. );

	return C3dCoord( xp, yp, zp );

}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// The old form of the line portal commands.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// ==================================================================
// Line command forms:
//
// Create:Line: id = %d, assoc = %d, sx = %f, sy = %f, sz = %f, ex = %f, ey = %f, ez = %f
// Create:Line: id = %d, assoc = %d, sx = %f, sy = %f, sz = %f, ep = %d
// Create:Line: id = %d, assoc = %d, sp = %d, ex = %f, ey = %f, ez = %f
// Create:Line: id = %d, assoc = %d, sp = %d, ep = %d
//
//			sx, sy, sz	OR  sp
//			ex, ey, ez  OR  ep
//			[assoc]
//			[id]
//
CReturn 
CCreateProcessApp::OldLine( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	// Initialize all important data to invalid values.
	ID sp = 0;
	ID ep = 0;
	double sx = UNDEFINED;
	double sy = UNDEFINED;
	double sz = UNDEFINED;
	double ex = UNDEFINED;
	double ey = UNDEFINED;
	double ez = UNDEFINED;
	CDbPoint* spt = NULL;
	CDbPoint* ept = NULL;

	bool modify;
	ID id = 0;

	CModel&	model = io_cmd->getModel();

	// By default, associativity is active,
	// but only applies to existing entities.
	int assoc = 1;

	// -----------------------------------------------------
	//		Extract command
	//
	//	Polymorphic -- by ordinates or reference IDs
	//
	io_cmd->getInt( "id", (int*)&id );
	io_cmd->getInt( "assoc", &assoc );

	if (io_cmd->getInt( "sp", (int*)&sp ).isOkay())
	{
		ret += model.EntityFind( sp, (CDbEntity**)&spt, DBPOINT, DBPOINT );

		if ( ret.IsOk() )
		{
			C3dCoord ps = spt->Coord();
			sx = ps.X();
			sy = ps.Y();
			sz = ps.Z();
		}
	}
	else
	{
		ret += io_cmd->getReal( "sx", &sx );
		ret += io_cmd->getReal( "sy", &sy );
		ret += io_cmd->getReal( "sz", &sz );
	}

	if (io_cmd->getInt( "ep", (int*)&ep ).isOkay())
	{
		ret += model.EntityFind( ep, (CDbEntity**)&ept, DBPOINT, DBPOINT );

		if ( ret.IsOk() )
		{
			C3dCoord pe = ept->Coord();
			ex = pe.X();
			ey = pe.Y();
			ez = pe.Z();
		}
	}
	else
	{
		ret += io_cmd->getReal( "ex", &ex );
		ret += io_cmd->getReal( "ey", &ey );
		ret += io_cmd->getReal( "ez", &ez );
	}

	if (!ret.isOkay())
	{
		ret.Internal( IDS_CREATE_PARAM_MISSING );
		return ret;
	}

	C3dVec	line_vec = C3dCoord( sx, sy, sz ) - C3dCoord( ex, ey, ez );
	if (EQUAL( line_vec.Length(), 0.0 ))
	{
		ret.Internal( IDS_CREATE_DEGENERATE );
		return ret;
	}

	// -----------------------------------------------------
	//		Check for modify
	//
	CDbLine* line = NULL;

	if (id)
	{
		ret = model.EntityFind( id, (CDbEntity**)&line, DBLINE, DBLINE );
		ASSERT( line != NULL);
		if ( line->Type() != DBLINE )
		{
			ret.Internal( IDS_MODIFY_TYPE );
			return ret;
		}
	}
	modify = (line != NULL);

	// -----------------------------------------------------
	//		Now, get the line
	//
	CDbWorkplane* work = NULL;
	CDbTool* tool = NULL;

	if (modify)
	{
		tool = line->Tool();
		work = line->Workplane();
	}
	else
	{
		tool = model.ActiveTool();
		work = model.ActiveWorkplane();
		if (!tool || !work)
			return ret;

		ret += model.EntityCreate( DBLINE, (CDbEntity**)&line );

		if (line != NULL && model.ActivePattern() != NULL)
			model.ActivePattern()->Append( line );
	}

	// -----------------------------------------------------
	// Create/Modify end points as necessary.
	//
	if ( ret.IsOk() )
	{
		if (modify && assoc)
		{
			spt = line->DbStartPt();

			if (sp == 0)
			{
				line->ConditionalDisassociate( spt );

				// Update an existing end point.
				spt->Init( tool, work, sx, sy, sz );
			}
			else
			{
				// This end of the line will be associated
				// with some other existing point.
			}
		}
		else if (spt == NULL)
		{
			// Create a new end point
			ret += model.EntityCreate( DBPOINT, (CDbEntity**)&spt );
			if ( ret.IsOk() )
				spt->Init( tool, work, C3dCoord( sx, sy, sz ) );
		}
	}

	if ( ret.IsOk() )
	{
		if (modify && assoc)
		{
			ept = line->DbEndPt();

			if (ep == 0)
			{
				line->ConditionalDisassociate( ept );

				// Update an existing end point.
				ept->Init( tool, work, ex, ey, ez );
			}
			else
			{
				// This end of the line will be associated
				// with some other existing point.
			}
		}
		else if (ept == NULL)
		{
			// Create a new end point
			ret += model.EntityCreate( DBPOINT, (CDbEntity**)&ept );
			if ( ret.IsOk() )
				ept->Init( tool, work, C3dCoord( ex, ey, ez ) );
		}
	}

	// -----------------------------------------------------
	// Fill the contents
	//
	if (ret.isOkay())
	{
		line->Init( tool, work, spt, ept );

		if ( modify )
			line->ModifyFlag( true );

		id = line->Id();

		io_cmd->setInt( "id", id );

		io_cmd->getViewMgr().ModelSet( model );
		io_cmd->getViewMgr().Refresh( id, TRUE );
	}

	return ret;
}

