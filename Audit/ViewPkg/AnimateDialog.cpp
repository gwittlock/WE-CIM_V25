// AnimateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "viewpkg.h"
#include "AnimateDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AnimateDialog dialog


//AnimateDialog::AnimateDialog(CWnd* pParent /*=NULL*/)
//	: CDialog(AnimateDialog::IDD, pParent)
//{
//	//{{AFX_DATA_INIT(AnimateDialog)
//		// NOTE: the ClassWizard will add member initialization here
//	//}}AFX_DATA_INIT
//}

static BOOL g_fast_nibble = FALSE;
static BOOL g_show_hits = FALSE;

AnimateDialog::AnimateDialog( 
	int			speed,
	CString&	label )
{
	m_speed = speed;

	Create( IDD_ANIMATE, AfxGetApp()->GetMainWnd() );
	SetDlgItemText( IDC_LABEL, label );
}

void AnimateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AnimateDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AnimateDialog, CDialog)
	//{{AFX_MSG_MAP(AnimateDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AnimateDialog message handlers


int
AnimateDialog::Speed( void )
{
	CSliderCtrl* speed = (CSliderCtrl*)GetDlgItem( IDC_SPEED );
	if (speed)
		return speed->GetPos();
	return 0;
}


BOOL
AnimateDialog::Pause( void )
{
	return IsDlgButtonChecked( IDC_PAUSE );
}

BOOL
AnimateDialog::FastNibble( void )
{
	g_fast_nibble = IsDlgButtonChecked( IDC_NIBBLE );
	return g_fast_nibble;
}

BOOL
AnimateDialog::ShowHits( void )
{
	g_show_hits = IsDlgButtonChecked( IDC_HITS );
	return g_show_hits;
}



BOOL AnimateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CSliderCtrl* speed = (CSliderCtrl*)GetDlgItem( IDC_SPEED );
	if (speed)
	{
		speed->SetTicFreq( 100 );
		speed->SetRange( 0, 1000, TRUE );
		speed->SetPos( m_speed );
	}

	CheckDlgButton( IDC_PAUSE, 1 );
	CheckDlgButton( IDC_NIBBLE, g_fast_nibble );
	CheckDlgButton( IDC_HITS, g_show_hits );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AnimateDialog::OnOK() 
{
	CSliderCtrl* speed = (CSliderCtrl*)GetDlgItem( IDC_SPEED );
	if (speed)
		speed->SetPos(0);
	CheckDlgButton( IDC_PAUSE, 0 );
	
	CDialog::OnOK();
}
