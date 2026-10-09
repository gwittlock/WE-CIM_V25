// Msg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Msg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool m_msgIsActive = FALSE;

void DLL_DECL MsgInit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_msgIsActive = TRUE;
}

void DLL_DECL MsgTerm()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_msgIsActive = FALSE;
}

BOOL DLL_DECL MsgIsActive()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_msgIsActive;
}

void DLL_DECL MsgDisplay( const char* msg )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	MessageBox( NULL, msg, "Warning!", (MB_OK|MB_ICONEXCLAMATION) );
}

int DLL_DECL FatalMsg( int status, const char* msgTemplate, const char* arg )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString msg;

	if (arg != NULL)
		msg.Format( msgTemplate, arg );
	else
		msg = msgTemplate;

	MessageBox( NULL, msg, "Fatal Error!", (MB_OK|MB_ICONSTOP) );

	return status;
}
