// DataWindow.cpp : implementation file
//

#include "stdafx.h"
#include "DspConsts.h"
#include "FilePeek.h"
#include "DataWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int DISPWID = ((8 + BYTEPAD) * BYTES_PER_ROW) + EOLWID;


/////////////////////////////////////////////////////////////////////////////
// CDataWindow

CDataWindow::CDataWindow()
{
}

CDataWindow::~CDataWindow()
{
}


BEGIN_MESSAGE_MAP(CDataWindow, CEdit)
	//{{AFX_MSG_MAP(CDataWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void
CDataWindow::DataDisplay( BYTE* data, int maxbuf, int base )
{
	CString	bin( ' ', DISPWID );
	CString	hex( ' ', DISPWID );
	CString	line;
	CString	all;
	CString	tmp;
	CString	tmp2;
	int		indx, jndx;
	int		bndx, row;
	BYTE	byte;

	int		mask[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	row = 0;
	for (indx = 0; indx < maxbuf; ++indx)
	{
		if (row >= MAXROWS)
			break;

		byte = data[indx];

		// Format this line and add to the final buffer.
		if ((indx % BYTES_PER_ROW) == 0)
		{
			if (indx > 0)
			{
				bin.TrimRight(' ');
				hex.TrimRight(' ');

				line.Format( "%s\r\n", bin );
				all += line;
				++row;

				line.Format( "  %s\r\n", hex );
				all += line;
				++row;
			}

			jndx = 0;

			hex.Empty();
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Generate the binary string for this byte.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		bin.SetAt( jndx, ' ' );
		++jndx;

		for (bndx = 0; bndx < 8; ++bndx)
		{
			bin.SetAt( jndx, ((byte & mask[bndx]) ? '1' : '0') );
			++jndx;

			if (bndx == 3)
			{
				bin.SetAt( jndx, ' ' );
				++jndx;
			}
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Generate the hex string for this byte.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		tmp.Format( "0x%2x", byte );
		tmp.Replace( ' ', '0' );
		tmp2.Format( "  %s    ", tmp );
		hex += tmp2;
	}

	SetWindowText( all );
}

void
CDataWindow::Highlight( int byteidx, int bitidx, int selBits )
{
	int	lineCount;
	int bitCount;
	int	residual;
	int	indxA, indxB;

	lineCount = byteidx / BYTES_PER_ROW;
	residual  = byteidx - (lineCount * BYTES_PER_ROW);

	// Calculate offset to start of binary line.

	indxA  = (2 * lineCount * DISPWID);		// the data are displayed as bin / hex pairs
	indxA -= (2 * lineCount);				// a hex line is 2 chars shorter than bin line

	// Calculate offset to starting bit.

	indxA += ((8 + BYTEPAD) * residual);	// 
	indxA += bitidx;
	indxA += ((bitidx < 4) ? 1 : 2);

	indxB = indxA + selBits;

	SetSel( indxA, indxB, FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CDataWindow message handlers
