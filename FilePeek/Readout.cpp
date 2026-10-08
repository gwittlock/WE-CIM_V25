// Readout.cpp : implementation file
//

#include "stdafx.h"
#include "FilePeek.h"
#include "FilePeekMsgs.h"
#include "Readout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReadout

CReadout::CReadout()
{
}

CReadout::~CReadout()
{
}


BEGIN_MESSAGE_MAP(CReadout, CEdit)
	//{{AFX_MSG_MAP(CReadout)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CReadout::TypeSet( EReadoutType type )
{
	m_type = type;
}

/////////////////////////////////////////////////////////////////////////////
// CReadout message handlers

void CReadout::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int	bits;

	switch (m_type)
	{
	case EBYTE:		bits =  8;	break;
	case ESHORT:	bits = 16;	break;
	case EINTEGER:	bits = 32;	break;
	case ELONG:		bits = 16;	break;
	case EHANDLE:	bits = 16;	break;
	default:		bits =  0;	break;
	}

	if (bits > 0)
	{
		GetOwner()->SendMessage( DATA_HIGHLIGHT, (WPARAM) bits, 0 );
	}

	CEdit::OnLButtonUp(nFlags, point);
}
