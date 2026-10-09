
#ifndef _RECVDLG_H
#define _RECVDLG_H

#include "MsgListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CIpcRecvDlg dialog

class CIpcRecvDlg : public CDialog
{
// Construction
public:

	CIpcRecvDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIpcRecvDlg)
	enum { IDD = IDD_RECV_DLG };
	CMsgListBox m_msgs;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CIpcRecvDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	HICON m_hIcon;

	//{{AFX_MSG(CIpcRecvDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void MessageDisplay( const char* text );

};

#endif
