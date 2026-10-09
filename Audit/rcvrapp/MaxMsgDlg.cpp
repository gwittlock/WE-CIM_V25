// MaxMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RcvrApp.h"
#include "MaxMsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMaxMsgDlg dialog


CMaxMsgDlg::CMaxMsgDlg( int currMax, CWnd* pParent /*=NULL*/)
	: CDialog(CMaxMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMaxMsgDlg)
	m_max = currMax;
	//}}AFX_DATA_INIT
}


void CMaxMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMaxMsgDlg)
	DDX_Text(pDX, IDC_EDIT1, m_max);
	DDV_MinMaxInt(pDX, m_max, 10, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMaxMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CMaxMsgDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CMaxMsgDlg::MaxMsgs()
{
	return m_max;
}

/////////////////////////////////////////////////////////////////////////////
// CMaxMsgDlg message handlers
