
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "cmn_resource.h"
#include "Return.h"
#include "ErrSys.h"


static bool m_errSysIsActive = false;
static int m_count = 0;

int DLL_DECL ErrSysInit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_errSysIsActive = TRUE;
	m_count = 0;

	return 1;
}

void DLL_DECL ErrSysTerm()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_errSysIsActive = FALSE;
}

BOOL DLL_DECL ErrSysIsActive()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return (m_errSysIsActive == true);
}

int DLL_DECL ErrSysAdd( const char* errMsg )
{
	if (errMsg == NULL)
		return -1;

	CReturn status;
	status.User( IDS_INTERNAL_ERROR, errMsg );

	++m_count;

	return 0;
}

int DLL_DECL ErrSysCount()
{
	return m_count;
}
