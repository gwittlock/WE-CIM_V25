// OffsetWindow.cpp : implementation file
//

#include "stdafx.h"
#include "DspConsts.h"
#include "FilePeek.h"
#include "OffsetWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// COffsetWindow

COffsetWindow::COffsetWindow()
{
}

COffsetWindow::~COffsetWindow()
{
}


BEGIN_MESSAGE_MAP(COffsetWindow, CEdit)
	//{{AFX_MSG_MAP(COffsetWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
COffsetWindow::OffsetDisplay( int base )
{
	CString	offset;
	CString	all;
	int		indx, jndx;
	int		bndx, row;

	row = 0;
	while (row < MAXROWS)
	{
		offset.Format( "%6d:\r\n", (base + ((row / 2) * BYTES_PER_ROW)) );
		offset.Replace( ' ', '0' );
		all += offset;
		++row;

		all += "\r\n";
		++row;
	}

	SetWindowText( all );
}

/////////////////////////////////////////////////////////////////////////////
// COffsetWindow message handlers
