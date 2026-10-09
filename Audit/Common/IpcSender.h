#pragma once

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////
// message commands

#define	MSGCMD_TEXT		101
#define	MSGCMD_EXIT		102
#define	MSGCMD_TIME		103

// message command structure

#define	MSGCMD_TEXTSIZE	255


struct MsgCmd
{
	int command;
	// int line;
	// COLORREF fgcolor;
	// COLORREF bgcolor;
	char text[MSGCMD_TEXTSIZE+1];
};

/////////////////////////////////////////////////////////////////////////////
// Interprocess Communication (IPC)
//
class dllExport CIpcSender
{
public:

	CIpcSender();

	// Returns: (true) success / (false) failure.
	bool Initialize();

	bool IsInitialized()  { return (m_hWndRecv != 0); }

	void Terminate();

	int SendMsg( int command, const char* text );

	~CIpcSender();

private:

	HWND m_hWndRecv;
	bool m_try;
};
