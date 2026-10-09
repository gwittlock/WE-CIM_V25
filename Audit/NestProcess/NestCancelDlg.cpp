// CNestCancelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NestMgr.h"

#include "NestCancelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CNestCancelDlg::CNestCancelDlg( 
	CString&	label )
{
//	Create( IDD_NESTCANCEL, AfxGetApp()->GetMainWnd() );
	Create( IDD_NESTCANCEL );

//	SetDlgItemText( IDC_LABEL, label );
}


BEGIN_MESSAGE_MAP(CNestCancelDlg, CDialog)
	//{{AFX_MSG_MAP(CNestCancelDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNestCancelDlg message handlers


BOOL CNestCancelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNestCancelDlg::OnOK() 
{
	CDialog::OnOK();

	CNestMgr::g_nest_cancel = true;
}
