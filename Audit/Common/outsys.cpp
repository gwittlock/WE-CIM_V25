
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "OutSys.h"

COutputHandler* m_outputHandler = NULL;

int DLL_DECL
OutSysInit( COutputHandler* outputHandler )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (outputHandler == NULL)
		return -1;

	m_outputHandler = outputHandler;

	return 0;
}

int DLL_DECL
OutSysTerm()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_outputHandler == NULL)
		return -1;

	return ( m_outputHandler->Term() );
}

int DLL_DECL
OutSysHandler()
{
	return (int) m_outputHandler;
}


