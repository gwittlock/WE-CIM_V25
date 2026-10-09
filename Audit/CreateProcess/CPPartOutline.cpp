
#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"

#include "dbPoint.h"
#include "dbHole.h"
#include "dbLine.h"
#include "dbArc.h"
#include "dbWorkplane.h"
#include "dbCommand.h"

#include "ViewMgr.h"

#include "DbProfile.h"
#include "DbFeature.h"

#include "Profile.h"
#include "Worm.h"
#include "Conversion.h"
#include "ModelUtil.h"

#include "CreateProcess.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create:BBPartOutline:
// Returns id = %d of part-outline profile
//
CReturn 
CCreateProcessApp::BBPartOutline( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbEntityArray	entities;
	CDbProfile*		dbProfile;
	int				count, indx;

	CModel&	model = io_cmd->getModel();
	CSelector& selector = model.SelectorStack()();

	dbProfile = NULL;  // assume failure

	count = selector.Count();
	if (count > 0)
	{
		for (indx = 0; indx < count; ++indx)
		{
			entities.Append( selector[indx] );
		}

		dbProfile = CModelUtil::BBPartOutlineCreate( entities, &model );
	}

	io_cmd->setInt( "id", ((dbProfile == NULL) ? 0 : dbProfile->Id()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create:CHPartOutline:
// Returns id = %d of part-outline profile
//
CReturn 
CCreateProcessApp::CHPartOutline( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbEntityArray	entities;
	CDbProfile*		dbProfile;
	int				count, indx;

	CModel&	model = io_cmd->getModel();
	CSelector& selector = model.SelectorStack()();

	dbProfile = NULL;  // assume failure

	count = selector.Count();
	if (count > 0)
	{
		for (indx = 0; indx < count; ++indx)
		{
			entities.Append( selector[indx] );
		}

		dbProfile = CModelUtil::CHPartOutlineCreate( entities, 1.e-3, &model );
	}

	io_cmd->setInt( "id", ((dbProfile == NULL) ? 0 : dbProfile->Id()) );

	return status;
}
