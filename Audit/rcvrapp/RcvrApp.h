
#ifdef _RCVRAPP_H
#error repeated include of this file
#else
#define _RCVRAPP_H
#endif

#if BEFORE
/////////////////////////////////////////////////////////////////////
// message commands

#define	MSGCMD_TEXT		101
#define	MSGCMD_EXIT		102
#define	MSGCMD_TIME		103

// message command structure

#define	MSGCMD_TEXTSIZE	79

#define MSGCMD_MAXLINES	6		// lines of text


typedef struct
	{
	int			command;
	int			line;
	COLORREF	fgcolor;
	COLORREF	bgcolor;
	char		text[MSGCMD_TEXTSIZE+1];
	} MsgCmd;
#else
#include "IpcSender.h"
#define MSGCMD_MAXLINES	6		// lines of text
#endif

/////////////////////////////////////////////////////////////////////
// CRcvrApp - implementation of this class is in RcvrApp.cpp
//
#include <afxmt.h>  // for CMutex

class CRcvrApp : public CWinApp
{
public:
	CRcvrApp();

	char	m_IniFile[MAX_PATH];

	CMutex * m_AppMutex;
	CSingleLock *m_AppLock;

	int		GetAppType( int stringid );

// Overrides
	//{{AFX_VIRTUAL(CRcvrApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRcvrApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////

#ifdef MAIN
#define Global
#else
#define Global extern
#endif


// instance of CRcvrApp object

Global CRcvrApp theApp;

