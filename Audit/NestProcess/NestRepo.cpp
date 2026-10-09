// NestRepo.cpp : The REPO support for NestProcess
//
// ==================================================================
//
#include "stdafx.h"

#include "MathConst.h"
#include "CommonFlags.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "Path.h"

#include "NestConfig.h"

#include "Model.h"
#include "ModelUtil.h"
#include "ViewMgr.h"
#include "db.h"
#include "DbIterator.h"
#include "DbTool.h"

#include "DaoQuery.h"

#include "Reposition.h"

#include "MM2.h"

#include "NestProcess.h"

// ==================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ==================================================================
// Nest:Repo:config=%s, override=%f, overlap=%f, late=%d, small=%d
//
// nest:repo:config="c:/work/wittlock/advmach/debug/database/cmdb.mdb"
// nest:repo:config="c:/work/wittlock/advmach/_bug/2003-09-16 Ben Demo/cmdb.mdb"
//
CReturn CNestProcessApp::NestRepo( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	CReturn		ret;
	CString		note;

	CString configdb_name;
	ret += io_cmd->getString("config", &configdb_name);

	double override = 0;
	ret += io_cmd->getReal("override", &override);

	double overlap = 0;
	ret += io_cmd->getReal("overlap", &overlap);

	int late = 1;
	ret += io_cmd->getInt("late", &late);

	int small_repo = 1;
	ret += io_cmd->getInt("small", &small_repo);

	int repo_burn = 0;
	io_cmd->getInt("repoburn", &repo_burn);

	if (!ret.isOkay())
	{
		ret.Internal( IDS_NEST_PARAM_MISSING );
		return ret;
	}

	EWMNesting( "NEST REPOSITION" );

	// ------------------------------------------
	//
	// Configuration
	//
	int mach_key = -1;
	CDaoDB config_db;
	ret += config_db.Open( configdb_name );
	if (ret.isOkay())
	{
		CDaoQuery	query;
		CString		sql;

		sql.Format( "SELECT [Machine ID] FROM [Tool Setups] WHERE ([Description]='%s')", 
//		sql.Format( "Select [Tool Setups].* From [Tool Setups] Where ([Tool Setups].Description = '%s')", 
						model.Header().getString("MachCfg", "") );
		ret += query.Init( config_db.Database(), sql );

		int fieldCount = query.FieldCount();
		if (fieldCount > 0)
		{ mach_key = query.IntGet(0); }
	}
	config_db.Close();

	CNestConfig& config = NestConfigGet();
	config.NestConfigInit();

	config.CmdbPath(configdb_name);
	config.PassInit(1);
	config.Pass(0);
	config.MachKey(mach_key);
	config.LoadMachine();
	config.YNegative(model.Header().getInt("WorkplaneType", 0) == 2);
	config.LeadSetup(-1);

	config.MachineTravelX(override);
	config.RepoOverlap(overlap);
	config.RepoLate(late!=0);
	config.RepoSmall(small_repo!=0);
	config.RepoToBurn(repo_burn!=0);
	//
	// (config clamps)
	for (int indx = 0; indx < config.ClampCountGet(); indx++)
	{
		CString name;
		name.Format( "Clamp%dPos", indx+1 );

		double pos = model.Header().getReal(name, 0.0);
		config.UseClamp( indx, (pos > SMALL) );
		config.ClampPos( indx, pos );
	}

	// ------------------------------------------

	ret += repo_explode(model);

	CReposition repos(&model, &config);
	repos.Create();

	// ------------------------------------------

	return ret;
}

// ==================================================================
//		repo_explode
//
//	For *all* existing reposition zones, flush their contents and remove 
// them.
//
CReturn 
CNestProcessApp::repo_explode(
	CModel& model )
{
	CReturn ret;

	CDbIterator	iter;
	CDbFeature*	dbFeature;

	iter.Init( model.Db(), DBFEATURE );
	while (true)
	{
		dbFeature = dynamic_cast<CDbFeature*>(iter());
		if (dbFeature == NULL)
			break;
		iter.Next();

		if ( dbFeature->IsWorkZone() )
		{ 
			dbFeature->BenignFlush();
			dbFeature->Delete();
		}
	}


	return ret;
}

