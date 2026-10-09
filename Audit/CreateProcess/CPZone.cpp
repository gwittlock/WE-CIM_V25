// ==================================================================
// CPZone.cpp :
//
// ==================================================================

#include "stdafx.h"

#include "StringConst.h"
#include "cmn_resource.h"

#include "Return.h"
#include "DbFeature.h"
#include "DbSequence.h"
#include "Model.h"
#include "DbIterator.h"
#include "DisplayEntity.h"

#include "CreateProcess.h"

const int MAX_CLAMP = 4;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Zone:Count:
//    Returns the count of work-zones.  That is, the count of
//    all features having a 'type' attribute of '_zone'.
//
// returns (i) count
//
CReturn
CCreateProcessApp::ZonesCount( CCommand* io_cmd )
{
	CReturn	status;

	CDbIterator	iter;
	CDbFeature*	dbFeature;
	int			count;

	CModel&	model = io_cmd->getModel();

	count = 0;

	iter.Init( model.Db(), DBFEATURE );
	while (1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsWorkZone() )
			++count;

		iter.Next();
	}

	io_cmd->setInt( "count", count );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Gets a zone given its zone number [1..n]
//    Zone:Get: _zone_num=%d
//       where:
//          zonenum -- is the value [1..n] as obtained from the
//                     _zone_num attribute.
//
// returns
//    (i) id -- (0) failure / (?) success
//    (i) _zone_num (iff success)
//    (r) _zone_bottom (iff success)
//    (r) _zone_left (iff success)
//    (r) _zone_top (iff success)
//    (r) _zone_right (iff success)
//    (r) _hold_x (iff success)
//    (r) _hold_y (iff success)
//    (r) _zone_repo (iff success)
//
// NOTE: The Portal command parameter _zone_num is simply used
// for the sake of consistency with the appearance of all other
// zone number type stuff.
//
// NOTE: If needed, we could speed up the search if Zone:Count:
// creates a temporary containing only work-zone features.
//
CReturn
CCreateProcessApp::ZoneGet( CCommand* io_cmd )
{
	CReturn	status;

	CDbIterator	iter;
	CDbFeature*	dbFeature;
	int			zoneNum, tmp;
	ID			id;

	CModel&	model = io_cmd->getModel();

	zoneNum = io_cmd->VarList().getInt( "_zone_num", 0 );
	if (zoneNum == 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneGet()" );
		return status;
	}

	id = 0;

	iter.Init( model.Db(), DBFEATURE );
	while (1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsWorkZone() )
		{
			tmp = dbFeature->IntGet( "_zone_num", 0 );
			if (tmp == zoneNum)
			{
				id = dbFeature->Id();
				break;
			}
		}

		iter.Next();
	}

	io_cmd->setInt( "id", id );
	if (id > 0)
	{
		const CVarList& attribs = dbFeature->Attrib();
		io_cmd->setInt( "_zone_num", attribs.getInt( "_zone_num", 0 ) );
		io_cmd->setReal( "_zone_bottom", attribs.getReal( "_zone_bottom", UNDEFINED ) );
		io_cmd->setReal( "_zone_left", attribs.getReal( "_zone_left", UNDEFINED ) );
		io_cmd->setReal( "_zone_top", attribs.getReal( "_zone_top", UNDEFINED ) );
		io_cmd->setReal( "_zone_right", attribs.getReal( "_zone_right", UNDEFINED ) );

		io_cmd->setReal( "_zone_repo", attribs.getReal( "_zone_repo", UNDEFINED ) );
		io_cmd->setReal( "_hold_x", attribs.getReal( "_hold_x", UNDEFINED ) );
		io_cmd->setReal( "_hold_y", attribs.getReal( "_hold_y", UNDEFINED ) );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create a new work-zone.
//    Zone:Update: _zone_num=%d
//                 ,_zone_bottom=%f,_zone_left=%f
//                 ,_zone_right=%f,_zone_top=%f
//
// Update an existing work-zone.
//    Zone:Update: id=%d, _zone_num=%d,
//                 [,_zone_bottom=%f][,_zone_left=%f]
//                 [,_zone_right=%f][,_zone_top=%f]
//                 [,_zone_repo=%f]
//                 [,_hold=%d][,_hold_x=%f][,_hold_y=%f]
//
// Remove a work-zone.
//    Zone:Update: id=%d, delete=1
//
// where:
//    zonenum -- a value [1..n]
//
// returns (i) id
//
// NOTE: If needed, we could speed up the search if Zone:Count:
// creates a temporary containing only work-zone features.
//
#if BEFORE_V17_0_176_1
CReturn
CCreateProcessApp::ZoneUpdate( CCommand* io_cmd )
{
	CReturn	status;

	CString			name;
	CDbSequence*	dbSequence;
	CDbFeature*		dbFeature;
	CVarList*		attribs;
	double			bottom;
	double			left;
	double			top;
	double			right;
	double			zoneRepo;
	double			holdDiam;
	double			holdX;
	double			holdY;
	double			holdC1DX;
	double			holdC1DY;
	double			holdC2DX;
	double			holdC2DY;
	int				zoneNum;
	int				hold;
	bool			remove;
	ID				id;

	CModel&	model = io_cmd->getModel();

	id = io_cmd->VarList().getInt( "id", 0 );
	zoneNum = io_cmd->VarList().getInt( "_zone_num", 0 );
	remove = io_cmd->VarList().getInt( "delete", FALSE );

	io_cmd->setInt( "id", 0 );  // the default return value

	if ( remove )
	{
		CDbIterator	iter;

		// Removing an existing work-zone.

		model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
		if (dbFeature == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneUpdate()" );
			return status;
		}

		zoneNum = dbFeature->IntGet( "_zone_num", 0 );

		// Find any associated sequence object.
		iter.Init( model.Db(), DBSEQUENCE );
		while (1)
		{
			dbSequence = dynamic_cast<CDbSequence*>( iter() );
			if (dbSequence == NULL)
				break;

			if (zoneNum == dbSequence->IntGet( "_zone_num", 0 ))
				break;

			iter.Next();
		}

		if (dbSequence != NULL)
			dbSequence->Delete();

		dbFeature->BenignFlush();
		dbFeature->Delete();
	}
	else
	{
		// Creating new / Updating existing work-zone.

		if (zoneNum == 0)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneUpdate()" );
			return status;
		}

		bottom		= io_cmd->VarList().getReal( "_zone_bottom", UNDEFINED );
		left		= io_cmd->VarList().getReal( "_zone_left", UNDEFINED );
		top			= io_cmd->VarList().getReal( "_zone_top", UNDEFINED );
		right		= io_cmd->VarList().getReal( "_zone_right", UNDEFINED );

		zoneRepo	= io_cmd->VarList().getReal( "_zone_repo", UNDEFINED );
		hold		= io_cmd->VarList().getInt( "_hold", IUNDEFINED );
		holdDiam	= io_cmd->VarList().getReal( "_hold_dia", UNDEFINED );
		holdX		= io_cmd->VarList().getReal( "_hold_x", UNDEFINED );
		holdY		= io_cmd->VarList().getReal( "_hold_y", UNDEFINED );
		holdC1DX	= io_cmd->VarList().getReal( "_hold_c1dx", UNDEFINED );
		holdC1DY	= io_cmd->VarList().getReal( "_hold_c1dy", UNDEFINED );
		holdC2DX	= io_cmd->VarList().getReal( "_hold_c2dx", UNDEFINED );
		holdC2DY	= io_cmd->VarList().getReal( "_hold_c2dy", UNDEFINED );

		if (id == 0)
		{
			// Creating a new work-zone.

			if ((bottom >= UNDEFINED)	||
				(left >= UNDEFINED)		||
				(top >= UNDEFINED)		||
				(right >= UNDEFINED) )
			{
				status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneUpdate()" );
				return status;
			}

			model.EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );
			dbFeature->ColorSet( DCOLOR_BLUE );
			dbFeature->StringSet( STR_TYPE, "_zone" );

#if REQUIRED
			// Unfortunately, setting the system flag prevents the
			// work-zone features from appearing in the entity list.

			// By setting the system flag, we prevent work-zones features
			// from being inadvertantly selected and deleted.
			dbFeature->SystemFlag( true );
#endif
		}
		else
		{
			// Updating an existing work-zone.
			model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
		}

		if (dbFeature == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneUpdate()" );
			return status;
		}

		attribs = dbFeature->pAttrib();

		dbFeature->IntSet( "_zone_num", zoneNum );

		// name.Format( "WorkZone%d", zoneNum );
		// dbFeature->Name( name );
		name.Format( "_repo_zone_%d", zoneNum );
		dbFeature->SystemName( name );

		if (bottom < UNDEFINED)
			attribs->setReal( "_zone_bottom", bottom );

		if (left < UNDEFINED)
			attribs->setReal( "_zone_left", left );

		if (top < UNDEFINED)
			attribs->setReal( "_zone_top", top );

		if (right < UNDEFINED)
			attribs->setReal( "_zone_right", right );

		if (zoneRepo < UNDEFINED)
			attribs->setReal( "_zone_repo", zoneRepo );
		
		if (hold < IUNDEFINED)
			attribs->setInt( "_hold", hold );

		if (holdDiam < UNDEFINED)
			attribs->setReal( "_hold_dia", holdDiam );

		if (holdX < UNDEFINED)
			attribs->setReal( "_hold_x", holdX );

		if (holdY < UNDEFINED)
			attribs->setReal( "_hold_y", holdY );

		if (holdC1DX < UNDEFINED)
			attribs->setReal( "_hold_c1dx", holdC1DX );

		if (holdC1DY < UNDEFINED)
			attribs->setReal( "_hold_c1dy", holdC1DY );

		if (holdC2DX < UNDEFINED)
			attribs->setReal( "_hold_c2dx", holdC2DX );

		if (holdC2DY < UNDEFINED)
			attribs->setReal( "_hold_c2dy", holdC2DY );

		ClampsUpdate( model, dbFeature, HaveClamps( model ) );
	}

	io_cmd->setInt( "id", ((dbFeature == NULL) ? 0 : dbFeature->Id()) );

	return status;
}
#else
CReturn
CCreateProcessApp::ZoneUpdate( CCommand* io_cmd )
{
	CReturn	status;

	CString			name;
	CDbSequence*	dbSequence;
	CDbFeature*		dbFeature;
	CVarList*		attribs;
	double			bottom;
	double			left;
	double			top;
	double			right;
	double			zoneRepo;
	double			hold_dia;
	double			holdX, holdY;
	double			hold_x1, hold_x2;
	double			hold_y2;
	int				zoneNum;
	int				hold;
	bool			remove;
	ID				id;

	CModel&	model = io_cmd->getModel();

	id = io_cmd->VarList().getInt( "id", 0 );
	zoneNum = io_cmd->VarList().getInt( "_zone_num", 0 );
	remove = io_cmd->VarList().getInt( "delete", FALSE );

	io_cmd->setInt( "id", 0 );  // the default return value

	if ( remove )
	{
		CDbIterator	iter;

		// Removing an existing work-zone.

		model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
		if (dbFeature == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneUpdate()" );
			return status;
		}

		zoneNum = dbFeature->IntGet( "_zone_num", 0 );

		// Find any associated sequence object.
		iter.Init( model.Db(), DBSEQUENCE );
		while (1)
		{
			dbSequence = dynamic_cast<CDbSequence*>( iter() );
			if (dbSequence == NULL)
				break;

			if (zoneNum == dbSequence->IntGet( "_zone_num", 0 ))
				break;

			iter.Next();
		}

		if (dbSequence != NULL)
			dbSequence->Delete();

		dbFeature->BenignFlush();
		dbFeature->Delete();
	}
	else
	{
		// Creating new / Updating existing work-zone.

		if (zoneNum == 0)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneUpdate()" );
			return status;
		}

		bottom		= io_cmd->VarList().getReal( "_zone_bottom", UNDEFINED );
		left		= io_cmd->VarList().getReal( "_zone_left", UNDEFINED );
		top			= io_cmd->VarList().getReal( "_zone_top", UNDEFINED );
		right		= io_cmd->VarList().getReal( "_zone_right", UNDEFINED );

		zoneRepo	= io_cmd->VarList().getReal( "_zone_repo", UNDEFINED );

		// hold		= model.Header().getInt( "hold_type", IUNDEFINED );
		hold		= io_cmd->VarList().getInt( "hold_type", IUNDEFINED );
		holdX		= io_cmd->VarList().getReal( "_hold_x", UNDEFINED );
		holdY		= io_cmd->VarList().getReal( "_hold_y", UNDEFINED );

		if (id == 0)
		{
			// Creating a new work-zone.

			if ((bottom >= UNDEFINED)	||
				(left >= UNDEFINED)		||
				(top >= UNDEFINED)		||
				(right >= UNDEFINED) )
			{
				status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneUpdate()" );
				return status;
			}

			model.EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );
			dbFeature->ColorSet( DCOLOR_BLUE );
			dbFeature->StringSet( STR_TYPE, "_zone" );

#if REQUIRED
			// Unfortunately, setting the system flag prevents the
			// work-zone features from appearing in the entity list.

			// By setting the system flag, we prevent work-zones features
			// from being inadvertantly selected and deleted.
			dbFeature->SystemFlag( true );
#endif
		}
		else
		{
			// Updating an existing work-zone.
			model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
		}

		if (dbFeature == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ZoneUpdate()" );
			return status;
		}

		attribs = dbFeature->pAttrib();

		dbFeature->IntSet( "_zone_num", zoneNum );

		// name.Format( "WorkZone%d", zoneNum );
		// dbFeature->Name( name );
		name.Format( "_repo_zone_%d", zoneNum );
		dbFeature->SystemName( name );

		if (bottom < UNDEFINED)
			attribs->setReal( "_zone_bottom", bottom );

		if (left < UNDEFINED)
			attribs->setReal( "_zone_left", left );

		if (top < UNDEFINED)
			attribs->setReal( "_zone_top", top );

		if (right < UNDEFINED)
			attribs->setReal( "_zone_right", right );

		if (zoneRepo < UNDEFINED)
			attribs->setReal( "_zone_repo", zoneRepo );
		
		if (hold < IUNDEFINED)
		{
			if ((holdX < UNDEFINED) && (holdY < UNDEFINED))
			{
				// Something is VERY wrong if we don't have _zone_left
				// because the workzone must be exist before you can
				// access the hold downs.
				left = attribs->getReal( "_zone_left", 0. );

				attribs->setInt( "_hold", hold );
#if BEFORE_V18_0
				attribs->setReal( "_hold_x", (holdX - left) );
#else
				// 2006.09.27 (PE) -- According to conversation with Gary,
				// the coordinate of a hold position is treated just like
				// the coordinate of a hole. This kinda makes sense because
				// the hold down mechanism typically travels with the head.
				// So the previous implementation was wrong (for a very long time).
				//
				// Moreover, Sunflower reported that the X ordinate would change
				// everytime you moved a workzone in the Edit/Zones/Manage dialog.
#endif
				attribs->setReal( "_hold_x", holdX );
				attribs->setReal( "_hold_y", holdY );

				hold_dia = model.Header().getReal( "hold_dia", 0. );
				if (hold_dia < UNDEFINED)
					attribs->setReal( "_hold_dia", hold_dia );

				if (hold == HOLD_2CIRCLE)
				{
					hold_x1 = model.Header().getReal( "hold_x1", UNDEFINED );
					if (hold_x1 < UNDEFINED)
						attribs->setReal( "_hold_c1dx", hold_x1 );

					hold_x2 = model.Header().getReal( "hold_x2", UNDEFINED );
					if (hold_x2 < UNDEFINED)
						attribs->setReal( "_hold_c2dx", hold_x2 );

					hold_y2 = model.Header().getReal( "hold_y", UNDEFINED );
					if (hold_y2 < UNDEFINED)
					{
						attribs->setReal( "_hold_c1dy", hold_y2 );
						attribs->setReal( "_hold_c2dy", hold_y2 );
					}
				}
				else if (hold == HOLD_RECTANGLE)
				{
					hold_x1 = model.Header().getReal( "hold_x1", UNDEFINED );
					if (hold_x1 < UNDEFINED)
						attribs->setReal( "_hold_tldx", hold_x1 );

					hold_x2 = model.Header().getReal( "hold_x2", UNDEFINED );
					if (hold_x2 < UNDEFINED)
						attribs->setReal( "_hold_brdx", hold_x2 );

					hold_y2 = model.Header().getReal( "hold_y", UNDEFINED );
					if (hold_y2 < UNDEFINED)
					{
						// NOTE: hold_y in the model header represents the Y offset.
						attribs->setReal( "_hold_tldy", (hold_y2 + (0.5 * -hold_dia)) );
						attribs->setReal( "_hold_brdy", (hold_y2 + (0.5 * hold_dia)) );
					}
				}
			}
			else
			{
				attribs->deleteVar( "_hold" );
				attribs->deleteVar( "_hold_x" );
				attribs->deleteVar( "_hold_y" );
				attribs->deleteVar( "_hold_dia" );
				attribs->deleteVar( "_hold_c1dx" );
				attribs->deleteVar( "_hold_c1dy" );
				attribs->deleteVar( "_hold_c2dx" );
				attribs->deleteVar( "_hold_c2dy" );
			}
		}

		ClampsUpdate( model, dbFeature, HaveClamps( model ) );
	}

	io_cmd->setInt( "id", ((dbFeature == NULL) ? 0 : dbFeature->Id()) );

	return status;
}
#endif

// ==================================================================
//	Create:Clamp:num=%d, x1=%f, y1=%f, ... , lldx=%f, lldy=%f, trdx=%f, trdy=%f
//
//	Where:
//		num		The number of clamps to set
//		x1, y1	The position of clamp 1. Y should be at 0.0 for most cases.
//		x2, y2	The position of clamp 2
//		...
//		lldx	The offset of the lower-left corner of the clamp avoidance area
//				from the clamp's X position (min X).
//		lldy	Lower-left delta in Y (min Y).
//		trdx	Top-right dX (max X).
//		trdy	Top-right dY (max Y).
//
//	Create/Update clamp attributes on all _zone Features.
//
CReturn CCreateProcessApp::Clamp( CCommand* io_cmd )
{
	CReturn		ret;
	CDbIterator iter;
	CString		name;
	double		xc;
	int			num, indx;

	CModel& model = io_cmd->getModel();

	num = io_cmd->VarList().getInt( "num", 0 );
	num = min(num, MAX_CLAMP);
	
	model.pHeader()->setInt( "Number_Of_Clamps", num );

	// Because ClampsUpdate() gets Clamp1Pos, Clamp2Pos, ...
	// from the model header.
	for (indx = 1; indx <= num; ++indx)
	{
		name.Format( "x%d", indx );

		xc = io_cmd->VarList().getReal( name, UNDEFINED );
		if (xc < UNDEFINED)
		{
			name.Format( "Clamp%dPos", indx );
			model.pHeader()->setReal( name, xc );
		}
	}

	if ( !ret.IsOk() )
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Clamp(), Create:Clamp: missing parameter" );
		return ret;
	}

	iter.Init( model.Db(), DBFEATURE );
	while (1)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsWorkZone() )
		{
			ClampsUpdate( model, dbFeature, TRUE );
		}

		iter.Next();
	}
	
	return ret;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// The paths to this method are:
//
// Zone:Update:
//    In this case, the user is adding/editing a work-zone.
//    Regardless, the clamps should appear/move with changes to the work-zone.
//
// Create:Clamp:
//    In this case, the user is adding/editing the clamps.
//
// The information for updating the clamps is extracted from the model header.
// This technique is used because the Portal commands are very different.  In
// particular, Zone:Update: does not contain any clamps information.  The model
// header is the only persistent place for storage of the parameter.
//
void CCreateProcessApp::ClampsUpdate( const CModel& model, CDbFeature* workZone, bool updateClampNum )
{
	CString		xname;
	CString		yname;
	CString		cname;
	CVarList*	attribs;
	double		zoneLeft, zoneTop, zoneBottom;
	double		dx, dy, xc;
	int			indx, nclamps;
	int			workType;

	// WorkplaneType (1) 1st quad / (2) 4th quad
	workType = model.Header().getInt( "WorkplaneType", 2 );

	dx = model.Header().getReal( "Clamp_Length", 0. ) / 2;
	dy = model.Header().getReal( "Clamp_Width", 0. );
	
	attribs = workZone->pAttrib();

	zoneLeft = attribs->getReal( "_zone_left", 0. );
	zoneTop = attribs->getReal( "_zone_top", 0. );
	zoneBottom = attribs->getReal( "_zone_bottom", 0. );

	attribs->setReal( "_clamp_lldx", -dx );
	attribs->setReal( "_clamp_trdx", dx );

	if (workType == 2)
	{
		attribs->setReal( "_clamp_lldy", -dy );
		attribs->setReal( "_clamp_trdy", 0. );
	}
	else
	{
		attribs->setReal( "_clamp_lldy", 0. );
		attribs->setReal( "_clamp_trdy", dy );
	}

	nclamps = model.Header().getInt( "Number_Of_Clamps", 0 );

	for (indx = 1; indx <= MAX_CLAMP; ++indx)
	{
		xname.Format( "_clamp%d_x", indx );
		yname.Format( "_clamp%d_y", indx );

		if (indx <= nclamps)
		{
			cname.Format( "Clamp%dPos", indx );
			xc = model.Header().getReal( cname, UNDEFINED );

			// 2018.01.27 -- Removed this guard per conversation with Gary
			// because Rittal has a Trump that allows the first clamp to be
			// positioned in negative X.
			//   if (xc >= SMALL && xc < UNDEFINED)
			{
				attribs->setReal( xname, (zoneLeft + xc) );
				attribs->setReal( yname, ((workType == 2) ? zoneBottom : zoneTop) );
			}
		}
		else
		{
			attribs->deleteVar( xname );
			attribs->deleteVar( yname );
		}
	}

	if ( updateClampNum )
	{
		if (nclamps > 0)
			attribs->setInt( "_clamp_num", nclamps );
		else
			attribs->deleteVar("_clamp_num");
	}

	// So that the new clamps are drawn :-)
	bool allocated;
	CDisplayEntity* dispent = workZone->DisplayEntityGet( &allocated );
	dispent->Flush();
}

bool
CCreateProcessApp::HaveClamps( const CModel& model )
{
	CString	name;
	double	xc;
	int		indx;

	for (indx = 1; indx <= MAX_CLAMP; ++indx)
	{
		name.Format( "Clamp%dPos", indx );
		xc = model.Header().getReal( name, UNDEFINED );
		if (xc < UNDEFINED)
			return TRUE;
	}

	return FALSE;
}

