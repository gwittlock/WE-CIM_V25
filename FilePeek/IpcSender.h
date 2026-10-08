
#ifndef _IPCSENDER_H
#define _IPCSENDER_H

/////////////////////////////////////////////////////////////////////
// message commands

#define	MSGCMD_TEXT		101
#define	MSGCMD_EXIT		102
#define	MSGCMD_TIME		103

// message command structure

#define	MSGCMD_TEXTSIZE	255


typedef struct
{
	int command;
	// int line;
	// COLORREF fgcolor;
	// COLORREF bgcolor;
	char text[MSGCMD_TEXTSIZE+1];
} MsgCmd;

/////////////////////////////////////////////////////////////////////////////
// Interprocess Communication (IPC)
//
class CIpcSender
{
public:

	CIpcSender();

	int SendMsg( int command, const CString& text );

	~CIpcSender();

private:

	HWND Init();

private:

	HWND m_hWndRecv;
};

#endif
