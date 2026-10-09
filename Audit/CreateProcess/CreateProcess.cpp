// ==================================================================
// CreateProcess.cpp : Defines the initialization routines for the DLL.
//
// ==================================================================

#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"

#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbPoint.h"
#include "DbHole.h"
#include "DbLine.h"
#include "DbArc.h"  
#include "DbCommand.h"  

#include "DbCurve.h"
#include "DbCurveList.h"

#include "DbContainer.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "DbSequence.h"
#include "DbPattern.h"

#include "DbIterator.h"

#include "Model.h"
#include "ModelUtil.h"
#include "ViewMgr.h"

#include "CreateProcess.h"

CRouteList CCreateProcessApp::m_createRouter;
CRouteList CCreateProcessApp::m_profileRouter;
CRouteList CCreateProcessApp::m_featureRouter;
CRouteList CCreateProcessApp::m_patternRouter;
CRouteList CCreateProcessApp::m_toolpathRouter;
CRouteList CCreateProcessApp::m_editRouter;
CRouteList CCreateProcessApp::m_transformRouter;
CRouteList CCreateProcessApp::m_leadRouter;
CRouteList CCreateProcessApp::m_fontRouter;
CRouteList CCreateProcessApp::m_zoneRouter;


// ==================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ==================================================================
//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" bool PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CCreateProcessApp

BEGIN_MESSAGE_MAP(CCreateProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CCreateProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateProcessApp construction

CCreateProcessApp::CCreateProcessApp()
{
	// TO_DO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCreateProcessApp object

CCreateProcessApp theApp;


CReturn 
CCreateProcessApp::RegisterProcess( 
	CRouteList*	io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	//
	// TODO:  Split Create into... Create and Edit?
	//		This is easier said than done; some things like Transform
	//		are obvious edit functions.  But what about Profile and
	//		Feature?  Each of those has a single create function and
	//		then a bunch of editing functions... does it make sense to
	//		split up that code?
	//		I don't know... so I continue to clutter CreateProcess until
	//		such time as we achieve enlightenment on the subject.
	//
	ret += io_route->addSubrouter( "Create", &m_createRouter );

	// ---

	// Defined in Ephemeral.cpp
	ret += m_createRouter.addProcess( CString("Plane"), Plane );
	ret += m_createRouter.addProcess( CString("Layer"), Layer );
	ret += m_createRouter.addProcess( CString("Tool"),  Tool );

	// Defined in Geometry.cpp
	ret += m_createRouter.addProcess( CString("Point"), Point );
	ret += m_createRouter.addProcess( CString("Hole"), Hole );
	ret += m_createRouter.addProcess( CString("Command"), Command );

	// EXPERIMENTAL
	ret += m_createRouter.addProcess( CString("Ellipse"), Ellipse );

	// Defined in PartOutline.cpp
	ret += m_createRouter.addProcess( CString("BBPartOutline"), BBPartOutline );
	ret += m_createRouter.addProcess( CString("CHPartOutline"), CHPartOutline );

	// Defined in weGeoLine.cpp
	ret += m_createRouter.addProcess( CString("Line"), Line );

	// Defined in weGeoArc.cpp
	ret += m_createRouter.addProcess( CString("Arc"), Arc );

	// Defined in weGeoCurve.cpp
	ret += m_createRouter.addProcess( CString("Split"), Split );

	ret += m_createRouter.addProcess( CString("Delete"), Delete );
	
	ret += m_createRouter.addProcess( CString("Empty"), Empty );

	ret += m_createRouter.addProcess( CString("Orphan"), Orphan );
	ret += m_createRouter.addProcess( "UpdateOrphan", UpdateOrphan );

	ret += m_createRouter.addProcess( CString("Extract"), Extract );
	ret += m_createRouter.addProcess( "OffsetCurve", OffsetCurve );
	ret += m_createRouter.addProcess( "OffsetProfile", OffsetProfile );

	// Debug method for CGeoPoly, in CPCreate.cpp
	ret += m_createRouter.addProcess( "Boolean", Boolean );
	ret += m_createRouter.addProcess( CString("Clamp"), Clamp );

	// Defined in Transform.cpp
	ret += io_route->addSubrouter( "Transform", &m_transformRouter );
	ret += m_transformRouter.addProcess( "Move", Move );
	ret += m_transformRouter.addProcess( "Rotate", Rotate );
	ret += m_transformRouter.addProcess( "Scale", Scale );
	ret += m_transformRouter.addProcess( "Mirror", Mirror );
	ret += m_transformRouter.addProcess( "MirrorLine", MirrorLine );
	ret += m_transformRouter.addProcess( "Grid", Grid );

	// Defined in CPProfile.cpp
	ret += io_route->addSubrouter( "Profile", &m_profileRouter );
	ret += m_profileRouter.addProcess( "Create",   ProfileCreate );
	ret += m_profileRouter.addProcess( "Modify",   ProfileModify );
	ret += m_profileRouter.addProcess( "IsClosed", ProfileIsClosed );
	ret += m_profileRouter.addProcess( "Grow",     ProfileGrow );
	ret += m_profileRouter.addProcess( "Explode",  ProfileExplode );
	ret += m_profileRouter.addProcess( "Selected", ProfileSelected );
	ret += m_profileRouter.addProcess( "Reverse",  ProfileReverse );
	ret += m_profileRouter.addProcess( "Split",    ProfileSplit );
	ret += m_profileRouter.addProcess( "Blend",		ProfileBlend );
	ret += m_profileRouter.addProcess( "Chamfer",	ProfileChamfer );
	ret += m_profileRouter.addProcess( "Count",    ProfileCount );
	ret += m_profileRouter.addProcess( "Associate",     ProfileAssociate );
	ret += m_profileRouter.addProcess( "Disassociate",  ProfileDisassociate );
	ret += m_profileRouter.addProcess( "Curve",    ProfileCurve );
	ret += m_profileRouter.addProcess( "IsUsableCurve", IsUsableCurve );


	// Defined in CPFeature.cpp
	ret += io_route->addSubrouter( "Feature", &m_featureRouter );
	ret += m_featureRouter.addProcess( "Create",	FeatureCreate );
	ret += m_featureRouter.addProcess( "Modify",	FeatureModify );
	ret += m_featureRouter.addProcess( "Reorder",	FeatureReorder );
	ret += m_featureRouter.addProcess( "Count",		FeatureCount );
	ret += m_featureRouter.addProcess( "Entity",	FeatureEntity );
	ret += m_featureRouter.addProcess( "ToolAssociate",	FeatureToolAssociate );

	// Defined in CPPattern.cpp
	ret += io_route->addSubrouter( "Pattern", &m_patternRouter );
	ret += m_patternRouter.addProcess( "Create",	PatternCreate );
	ret += m_patternRouter.addProcess( "Modify",	PatternModify );
	ret += m_patternRouter.addProcess( "Count",		PatternCount );
	ret += m_patternRouter.addProcess( "Entity",	PatternEntity );
	ret += m_patternRouter.addProcess( "Edit",		PatternEdit );
	ret += m_patternRouter.addProcess( "Instance",	PatternInstance );
	ret += m_patternRouter.addProcess( "Explode",	PatternExplode);

	// Defined in CPToolpath.cpp
	ret += io_route->addSubrouter( "Toolpath", &m_toolpathRouter );
	ret += m_toolpathRouter.addProcess( "AutoTool",	AutoTool );
	ret += m_toolpathRouter.addProcess( "Disassociate",	Disassociate );
	ret += m_toolpathRouter.addProcess( "ConvexHullOffset",	ConvexHullOffset );
	ret += m_toolpathRouter.addProcess( "AutoIndexOffset",	AutoIndexOffset );
	ret += m_toolpathRouter.addProcess( "AutoPunch",	AutoPunch );
	ret += m_toolpathRouter.addProcess( "Spiral",  ToolpathSpiral );
	ret += m_toolpathRouter.addProcess( "PunchShapesCreate",	PunchShapesCreate );
	ret += m_toolpathRouter.addProcess( "Raster",	Raster );
	ret += m_toolpathRouter.addProcess( "Slit",	Slit );
	ret += m_toolpathRouter.addProcess( "Nibble",	Nibble );

	// Defined in CPToolpath.cpp
	ret += io_route->addSubrouter( "Edit", &m_editRouter );
	ret += m_editRouter.addProcess( "Drop",	DropStop );

	// Defined in Lead.cpp
	ret += io_route->addSubrouter( "Lead", &m_leadRouter );
	ret += m_leadRouter.addProcess( "Point", LeadPoint );
	ret += m_leadRouter.addProcess( "Selection", LeadSelection );
	ret += m_leadRouter.addProcess( "Model", LeadModel );

	// Defined in CPFont.cpp
	ret += io_route->addSubrouter( "Font", &m_fontRouter );
	ret += m_fontRouter.addProcess( "Load", FontLoad );
	ret += m_fontRouter.addProcess( "Unload", FontUnload );
	ret += m_fontRouter.addProcess( "Text", FontText );

	// Defined in CPZone.cpp
	ret += io_route->addSubrouter( "Zone",		&m_zoneRouter );
	ret += m_zoneRouter.addProcess( "Count",	ZonesCount );
	ret += m_zoneRouter.addProcess( "Get",		ZoneGet );
	ret += m_zoneRouter.addProcess( "Update",	ZoneUpdate );

	return ret;
}

/////////////////////////////////////////////////////////////////////////////

CReturn 
CCreateProcessApp::Create( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_createRouter.Dispatch( io_cmd );
}

CReturn 
CCreateProcessApp::Transform( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_transformRouter.Dispatch( io_cmd );
}

CReturn 
CCreateProcessApp::Profile( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_profileRouter.Dispatch( io_cmd );
}

CReturn 
CCreateProcessApp::Feature( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_featureRouter.Dispatch( io_cmd );
}

CReturn 
CCreateProcessApp::Pattern( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_patternRouter.Dispatch( io_cmd );
}


CReturn 
CCreateProcessApp::Toolpath( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	return m_toolpathRouter.Dispatch( io_cmd );
}


CReturn 
CCreateProcessApp::Lead( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_leadRouter.Dispatch( io_cmd );
}

CReturn 
CCreateProcessApp::Font( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_fontRouter.Dispatch( io_cmd );
}


// ==================================================================
// Create:Extract: id=%d
//
//			type
//			owner
//			sequence
//			toolid
//			sx, sy, sz
//			ex, ey, ez
//			cx, cy, cz
//			dir
//			text
//
CReturn 
CCreateProcessApp::Extract( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn		ret;
	int			id, wz;
	CDbEntity*	entity;
	CDbEntity*	tmp;

	ret = io_cmd->getInt( "id", &id );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_CREATE_EXTRACT_NO_ID );
		return ret;
	}

	ret += model.EntityFind( id, &entity );

	if (entity != NULL && entity->IsDeleted())
	{
		ret.Diagnostic( "CCreateProcessApp::Extract() -- entity already deleted" );
		return ret;
	}

	// -----------------------------------------------------
	// Suck the contents
	//
	if (ret.isOkay())
	{
		io_cmd->setInt( "type", entity->Type() );

		tmp = entity->Owner();
		io_cmd->setInt( "owner", ((tmp == NULL) ? 0 : tmp->Id()) );

		io_cmd->setInt( "toolid", entity->ToolId() );
		io_cmd->setInt( "layerid", entity->ToolId() );
#ifdef LAYER  // eliminate reference to layerid ?
#endif
		io_cmd->setInt( "workid", entity->WorkplaneId() );

		// TODO: Is the *really* the way to do this????
		switch (entity->Type())
		{
			case DBPOINT:
				{
					CDbPoint* point = (CDbPoint*)entity;
					if (point)
					{
						const C3dCoord& pnt = point->Coord();

						io_cmd->setReal( "x", pnt.X() );
						io_cmd->setReal( "y", pnt.Y() );
						io_cmd->setReal( "z", pnt.Z() );

						// By using the same attribute name as curves
						// the client does not need to check entity type
						io_cmd->setReal( "sx", pnt.X() );
						io_cmd->setReal( "sy", pnt.Y() );
						io_cmd->setReal( "sz", pnt.Z() );
					}
				}
				break;

			case DBCOMMAND:
				{
					CDbCommand* cmd = (CDbCommand*)entity;
					if (cmd)
					{
						const C3dCoord& pnt = cmd->Coord();

						io_cmd->setReal( "x", pnt.X() );
						io_cmd->setReal( "y", pnt.Y() );
						io_cmd->setReal( "z", pnt.Z() );

						// By using the same attribute name as curves
						// the client does not need to check entity type
						io_cmd->setReal( "sx", pnt.X() );
						io_cmd->setReal( "sy", pnt.Y() );
						io_cmd->setReal( "sz", pnt.Z() );

						io_cmd->setInt( "pos",
							cmd->IntGet( "pos", TEXTPOS_TOPCTR ) );

						// degrees
						io_cmd->setReal( "angle",
							cmd->DoubleGet( "angle", 0. ) );

						io_cmd->setString( "text", cmd->Text() );

						io_cmd->setString( "label", cmd->InstanceLabel() );
					}
				}
				break;

			case DBHOLE:
				{
					CDbHole* hole = (CDbHole*)entity;
					if (hole)
					{
						const C3dCoord& pnt = hole->Coord();

						io_cmd->setReal( "x", pnt.X() );
						io_cmd->setReal( "y", pnt.Y() );
						io_cmd->setReal( "z", pnt.Z() );

						io_cmd->setReal( "dia", hole->Diam() );
						io_cmd->setReal( "depth", hole->Depth() );

						// By using the same attribute name as curves
						// the client does not need to check entity type
						io_cmd->setReal( "sx", pnt.X() );
						io_cmd->setReal( "sy", pnt.Y() );
						io_cmd->setReal( "sz", pnt.Z() );
					}
				}
				break;


			case DBLINE:
				{
					CDbLine*	line = (CDbLine*)entity;
					if (line)
					{
						const C3dCoord& st = line->StartPt();
						const C3dCoord& en = line->EndPt();

						io_cmd->setReal( "sx", st.X() );
						io_cmd->setReal( "sy", st.Y() );
						io_cmd->setReal( "sz", st.Z() );

						io_cmd->setReal( "ex", en.X() );
						io_cmd->setReal( "ey", en.Y() );
						io_cmd->setReal( "ez", en.Z() );
					}
				}
				break;

			case DBARC:
				{
					CDbArc*	arc = (CDbArc*)entity;
					if (arc)
					{
						const C3dCoord& st = arc->StartPt();
						const C3dCoord& en = arc->EndPt();
						const C3dCoord& ct = arc->CenterPt();

						io_cmd->setReal( "sx", st.X() );
						io_cmd->setReal( "sy", st.Y() );
						io_cmd->setReal( "sz", st.Z() );

						io_cmd->setReal( "ex", en.X() );
						io_cmd->setReal( "ey", en.Y() );
						io_cmd->setReal( "ez", en.Z() );

						io_cmd->setReal( "cx", ct.X() );
						io_cmd->setReal( "cy", ct.Y() );
						io_cmd->setReal( "cz", ct.Z() );

						CDbWorkplane* work = arc->Workplane();

						io_cmd->setInt( "dir", arc->Dir() * work->ToolUp() );
					}
				}
				break;

			default:
				break;
		}

		wz = CModelUtil::WorkZoneNum( entity );
		io_cmd->setInt( "wz", wz );
	}

	return ret;
}

// ==================================================================
// Create:Delete: [id=%d] [selected=%b]
//
CReturn 
CCreateProcessApp::Delete( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			ret;

	CModel&	model = io_cmd->getModel();

	ID id = io_cmd->VarList().getInt( "ID", 0 );
	bool selected = io_cmd->VarList().getInt( "selected", FALSE );

	if ( selected )
	{
		// For efficiency, we try to delete top-down.
		CSelector& selector = model.SelectorStack()();
		int	count = selector.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* dbEntity = selector[indx];
			CDbEntity* owner = dbEntity->Owner();

			if (owner == NULL)
			{
				if ( !dbEntity->IsDeleted() )
					dbEntity->Delete();
			}
			else if ( !owner->IsSelected() )
			{
				if ( !dbEntity->IsDeleted() )
					dbEntity->Delete();
			}
		}
	}
	else if (id > 0)
	{
		ret += model.EntityDelete( id );
	}
	else
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Delete()" );
	}

#if NEEDED  // View management is now done on the VB side
	io_cmd->getViewMgr().Clear();
	io_cmd->getViewMgr().Regenerate( model, id );
	io_cmd->getViewMgr().Refresh();
#endif

	return ret;
}


// ==================================================================
// Create:Empty:
// Deletes all empty containers in the model.
//
CReturn 
CCreateProcessApp::Empty( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn status;

	status = CModelUtil::EmptyContainers( model, false );

	return status;
}

// ==================================================================
// Create:Orphan:
//
CReturn 
CCreateProcessApp::Orphan( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CModel&	model = io_cmd->getModel();
	ID id = 0;

	status += io_cmd->getInt( "id", (int*)&id );
	if (!status.isOkay())
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Orphan()" );
		return status;
	}

	CDbEntity* dbEntity;
	status += model.EntityFind( id, &dbEntity );

	status += do_orphan( model, dbEntity );

	return status;
}

// ==================================================================
// Create:UpdateOrphan:
//
//	Orphan all selected entities
//
CReturn 
CCreateProcessApp::UpdateOrphan( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	CReturn			ret;


	// -----------------------------------------------------
	//		Set the attribute... everywhere!
	//
	CDbEntity*	db_ent;
	int num = selector.Count();
	for (int idx = 0; idx<num; idx++)
	{
		db_ent = selector[idx];
		if (db_ent)
			ret += do_orphan( model, db_ent );
	}

	return ret;
}

CReturn
CCreateProcessApp::do_orphan(
	const CModel&	model,
	CDbEntity*		dbEntity )
{
	CReturn	status;

	if (dbEntity == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::do_orphan()" );
	else
	{
		//
		// ONLY orphan entites whose parent is a Feature... so verify this.
		//
		CDbFeature*	feature = dynamic_cast<CDbFeature*>( dbEntity->Owner() );
		if (feature)
			dbEntity->Owner( NULL );
	}

	return status;
}
