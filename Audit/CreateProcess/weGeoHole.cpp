
#include "stdafx.h"

#include "StringConst.h"
#include "ColorConst.h"
#include "cmn_resource.h"
#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbCurve.h"
#include "DbHole.h"
#include "Model.h"

#include "CreateProcess.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create:Hole: id=%d, fid=%d, tool_id=%d, xc=%f, yc=%f, orient=%f
//
CReturn 
CCreateProcessApp::Hole( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbWorkplane* dbWork;
	CDbTool*	dbTool;
	CDbHole*	dbHole;
	CDbFeature*	dbFeature;
	double		xc, yc, zc;
	double		orient, diam;
	double		depth;
	int			feature_id;
	int			tool_id;
	int			id;  //, color;
	int			sncs;
	bool		modify;

	CModel&	model = io_cmd->getModel();

	const CVarList&	attribs = io_cmd->VarList();

	id = attribs.getInt( "id", 0 );
	feature_id = attribs.getInt( "fid", 0 );
	tool_id = attribs.getInt( "tool_id", 0 );

	xc = attribs.getReal( "xc", 0. );
	yc = attribs.getReal( "yc", 0. );

	orient = attribs.getReal( "orient", 0. );

	sncs = attribs.getInt( "sncs", 0 );

	// Obsolete/Optional parameters (?)
	zc = attribs.getReal( "zc", 0. );
	diam = attribs.getReal( "diam", 0.001 );
	depth = model.Header().getReal( STR_THICKNESS, 0.0 );

	dbHole = NULL;
	if (id > 0)
	{
		model.EntityFind( id, (CDbEntity**) &dbHole, DBHOLE, DBHOLE );
		if (dbHole == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Hole(#1)" );
			return status;
		}
	}

	modify = (dbHole != NULL);

	if (modify)
	{
		dbTool = dbHole->Tool();
		dbWork = dbHole->Workplane();
	}
	else
	{
		// dbTool = model.ActiveTool();
		dbWork = model.ActiveWorkplane();
		model.EntityFind( tool_id, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if ((dbTool == NULL) || (dbWork == NULL))
		{
			// NOTE: Can not use InternalError() or UserWarn() because
			// either sets status to STATUS_ERROR.
			status.Diagnostic( "CCreateProcessApp::Hole(#2)" );
			dbTool = model.ActiveTool();
		}

		status += model.EntityCreate( DBHOLE, (CDbEntity**) &dbHole );

		if (model.ActivePattern() != NULL)
			model.ActivePattern()->Append( dbHole );
	}

	id = dbHole->Id();

	if (status.isOkay())
	{
		dbHole->Init( dbTool, dbWork, C3dCoord( xc, yc, zc ), diam, depth );

		HoleAttribsApply( attribs, dbHole );

		if ( modify )
			dbHole->ModifyFlag( true );

		if (feature_id > 0)
		{
			model.EntityFind( feature_id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
			if (dbFeature != NULL)
				dbFeature->Append( dbHole );
		}

		io_cmd->setInt( "id", id );
	}

	return status;
}

// NOTE: In order to simplify matters for rendering and code generation,
// we express the step-along distance as a positive value and the direction
// as a signed value. This means, however, that the ui must map these values
// back to those present in any dialog.
void
CCreateProcessApp::HoleAttribsApply( const CVarList& attribs, CDbHole* dbHole )
{
	double	dst, ang;
	int		sncs;

	// All holes have (at minimum) the "orient" attribute (for the tool).
	dbHole->DoubleSet( "orient", attribs.getReal( "orient", 0. ) );

	sncs = attribs.getInt( "sncs", 0 );
	if (sncs > 0)
	{
		// NOTE: The sncs is critical to rendering.
		dbHole->IntSet( "sncs", sncs );

		switch (sncs)
		{
		case 302:  // row in x

			ang = 0.;
			dst = attribs.getReal( "dst", 0. );
			if (dst < 0.)
			{
				ang = 180.;
				dst = -dst;
			}

			dbHole->DoubleSet( "ang", ang );
			dbHole->DoubleSet( "dst", dst );
			dbHole->IntSet( "cnt", attribs.getInt( "cnt", 0 ) );
			break;

		case 303:  // row in y

			ang = 90.;
			dst = attribs.getReal( "dst", 0. );
			if (dst < 0.)
			{
				ang = 270.;
				dst = -dst;
			}

			dbHole->DoubleSet( "ang", ang );
			dbHole->DoubleSet( "dst", dst );
			dbHole->IntSet( "cnt", attribs.getInt( "cnt", 0 ) );
			break;

		case 304:  // laa / dist normal
		case 305:  // laa / dist parallel
			dbHole->DoubleSet( "ang", attribs.getReal( "ang", 0. ) );
			dbHole->DoubleSet( "dst", attribs.getReal( "dst", 0. ) );
			dbHole->IntSet( "cnt", attribs.getInt( "cnt", 0 ) );
			break;

		case 306:  // grid
			dbHole->IntSet( "prim", attribs.getInt( "prim", 0 ) );
			dbHole->DoubleSet( "ang", attribs.getReal( "ang", 0. ) );
			dbHole->DoubleSet( "xdst", attribs.getReal( "xdst", 0. ) );
			dbHole->IntSet( "xcnt", attribs.getInt( "xcnt", 0 ) );
			dbHole->DoubleSet( "ydst", attribs.getReal( "ydst", 0. ) );
			dbHole->IntSet( "ycnt", attribs.getInt( "ycnt", 0 ) );
			break;

		case 307:  // bhc
			dbHole->DoubleSet( "as", attribs.getReal( "as", 0. ) );
			dbHole->DoubleSet( "ai", attribs.getReal( "ai", 0. ) );
			dbHole->DoubleSet( "rad", attribs.getReal( "rad", 0. ) );
			dbHole->IntSet( "cnt", attribs.getInt( "cnt", 0 ) );
			break;
		}
	}
}
