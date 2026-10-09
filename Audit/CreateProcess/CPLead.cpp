// ==================================================================
// CPLead.cpp : Create Leads Command Interface
//
// ==================================================================

#include "stdafx.h"
#include "cmn_resource.h"

#include "DbPoint.h"
#include "Lead.h"
#include "LeadData.h"
#include "StringConst.h"

#include "CreateProcess.h"
#include "ViewMgr.h"

// ==================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ==================================================================
//	Lead:Point:id=%d, type=%d, direction=%d, side=%d, junction=%d 
//			[, length=%f] [, angle=%f] [, included=%f] [, radius=%f] [, distance=%f]
//			[, poff=%f] [, ptype=%d] [, pdiam=%f] [, ptool=%d]
//
//	Where:
//		id = (long) the start element id of the profile
//		type = (long) the lead type (0=none, 1=line, 2=arc, 3=line/line, 4=line/arc, 5=ramp)
//		direction = (long) what lead(s) to apply (1=lead in only, 2=lead out only, 3=both lead in and out)
//		side = (long) what type of lead to apply (0=outside lead, 1=inside lead)
//		junction = (long) how to treat the coincident point (1=no gap, 2=gap, 3=overlap)
//		length = (double) line length
//		angle = (double) line angle or ramp inclination
//		included = (double) arc included angle
//		radius = (double) arc radius
//		distance = (double) gap or overlap distance
//		tabthick = (double) thickness of Tab junction
//
//	if Pierce hole desired:
//		poff = (double) pierce hole offset
//		ptype = (long) pierce tool type ID
//		pdiam = (double) pierce tool diameter
//	OR:
//		ptool = (long) pierce tool ID
//
//	Parameters in brackets are optional based upon type.
//
//	Applies leads at the given point, using the given parameters.
//
CReturn CCreateProcessApp::LeadPoint( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CLeadData data;
	CReturn status;

	const CVarList& args = io_cmd->VarList();

	// ID of reference curve where the lead goes...
	ID id = args.getInt( "id", 0 );

	// (1) In / (2) Out / (3) Both
	int dir = args.getInt( "direction", 0 );

	// (+1) left / (-1) right
	// NOTE: 'side' is an optional (fallback) argument. By default,
	// manual leads use the cut-side attribute of the toolpath.
	int side = args.getInt( "side", 0 );

	int ival;
	status += io_cmd->getInt( "type", &ival);
	data.Type( (eLeadType)ival );

	status += io_cmd->getInt( "junction", &ival );
	data.Junction( (eLeadJunction)ival );

	if(!status.isOkay())
	{
		status.Internal( IDS_CREATE_PARAM_MISSING );
		return status;
	}

	// Other parameters, optional
	double dval;

	dval = args.getReal( "length", 0. );
	data.LineLen( dval );

	dval = args.getReal( "angle", 0. );
	data.LineAng( dval );

	dval = args.getReal( "included", 0. );
	data.ArcAng( dval );

	dval = args.getReal( "radius", 0. );
	data.ArcRad( dval );

	dval = args.getReal( "distance", 0. );
	data.Distance( dval );

	dval = args.getReal( "tabthick", 0. );
	data.TabThick( dval );

	// ---------------------------

	ival = args.getInt( "usepierce", 0 );
	data.UsePierce((ePierceType)ival);

	dval = args.getReal( "p_offset", 0. );
	data.PierceOffset(dval);

	dval = args.getReal( "p_diam", 0. );
	data.PierceDiameter(dval);

	ival = args.getInt( "p_type", 0 );
	data.PierceType( (eToolType) ival);

	ival = args.getInt( "p_id", -1 );
	data.PierceID(ival);

	// ----------------------------

	CModel&	model = io_cmd->getModel();
	CLead	lead(model);

	CLeadData*	lead_in = (dir&1)?&data:NULL;
	CLeadData*	lead_out = (dir&2)?&data:NULL;

	status = lead.Manual( id, side, lead_in, lead_out );

	// TODO:  Make this more specific, to refresh only the profile and leads in question?
	io_cmd->getViewMgr().ModelSet( model );
	io_cmd->getViewMgr().Refresh( true );

	return status;
}

// ==================================================================
//	Lead:Selection:configdb=%s, setup=%d
//
//		where:
//			configdb is the CMDB database
//			setup is the Lead Setup parameter table ID
//
//	Automatically applies leads to the current selection set, using
//	the information and parameters in the given lead setup table
//
CReturn CCreateProcessApp::LeadSelection( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	const CVarList& params = io_cmd->VarList();

	ID setup = params.getInt( "setup", -1 );
	CString configdb = params.getString( "configdb", "" );

	if ((setup < 0) || configdb.IsEmpty())
	{
		status.Internal( IDS_CREATE_PARAM_MISSING );
	}
	else
	{
		CModel&	model = io_cmd->getModel();
		CSelectorStack& selectorStack = model.SelectorStack();
		CSelector& selector = selectorStack();

		// (0) None / (1) Closed Profiles Only
		int restrictions = io_cmd->VarList().getInt( "restrict", 0 );

		CLead lead( model );
#if ORIGINAL_CODE
	if (restrictions == 0)
		status += lead.AutoOpen( configdb, setup, false, &selector );
#endif
		status += lead.Auto( configdb, setup, selector, TRUE, TRUE, restrictions );

		CString msg = status.LastStatusMsg();
		io_cmd->setString( "status", msg );
		status.LastStatusMsgSet( "" );

		// TODO:  Make more specific to refresh only the selection in question?
		io_cmd->getViewMgr().ModelSet( model );
		io_cmd->getViewMgr().Refresh( true );
	}

	return status;
}

// ==================================================================
//	Lead:Model:configdb=%s, setup=%d, restrict=%d
//
//		where:
//			configdb is the CMDB database
//			setup is the Lead Setup parameter table ID
//
//	Automatically applies leads to the entire model, using
//	the information and parameters in the given lead setup table
//
CReturn CCreateProcessApp::LeadModel( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;
	CString	configdb;
	ID		setup;
	int		restrictions;

	ret += io_cmd->getInt( "setup", (int*)&setup );
	ret += io_cmd->getString( "configdb", &configdb );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_CREATE_PARAM_MISSING );
		return ret;
	}

	CModel&	model = io_cmd->getModel();
	CSelector selector(model);

	selector.All( 0 );
	selector.Filter( DBPROFILE, 1 );
	selector.Restrictions( TRUE );
	selector.SelectAll( TRUE );

	CLead	lead(model);

	// restrictions -- (0) None / (1) Closed Profiles Only
	restrictions = io_cmd->VarList().getInt( "restrict", 0 );

	ret += lead.Auto( configdb, setup, selector, TRUE, TRUE, restrictions );

	selector.Clear();

	io_cmd->getViewMgr().ModelSet( model );
	io_cmd->getViewMgr().Refresh( true );

	return ret;
}
