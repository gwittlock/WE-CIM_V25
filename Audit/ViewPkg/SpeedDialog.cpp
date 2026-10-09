// ==================================================================
//		Speed Dialog
//
//	View animation control
//
// ==================================================================

#include "stdafx.h"

#include "resource.h"
#include "SpeedDialog.h"


CSpeedDialog::CSpeedDialog( 
	CViewBase*	view,
	int			speed )
{
	m_view = view;

	Create( IDD_ANIMATE, AfxGetApp()->GetMainWnd() );

	m_speed = (CSliderCtrl*)GetDlgItem( IDC_SPEED );
	if (m_speed)
	{
		m_speed->SetTicFreq( 100 );
		m_speed->SetRange( 0, 1000, TRUE );
		m_speed->SetPos( speed );
	}

	m_pause = (CButton*)GetDlgItem( IDC_PAUSE );
	if (m_pause)
		m_pause->SetCheck(1);
}


int
CSpeedDialog::Speed( void )
{
	if (m_speed)
		return m_speed->GetPos();
	return 0;
}


BOOL
CSpeedDialog::Pause( void )
{
	if (m_pause)
	{
		int val = m_pause->GetState();
		val = m_pause->GetCheck();
		val = m_pause->GetButtonStyle();
		m_pause->SetCheck( !m_pause->GetCheck() );
		return (m_pause->GetCheck());
	}
	return 0;
}