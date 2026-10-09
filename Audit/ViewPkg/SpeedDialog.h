#if !defined(_SPEEDDIALOG_H)
#define _SPEEDDIALOG_H

// ==================================================================
//		Speed Dialog
//
//	View animation control
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "ViewBase.h"

// ==================================================================

class CSpeedDialog : public CDialog
{
public:
	CSpeedDialog( CViewBase* view, int speed );

	int Speed( void );
	BOOL Pause( void );

	// ------------------------
private:

	CViewBase*		m_view;
	CSliderCtrl*	m_speed;
	CButton*		m_pause;
};


#endif