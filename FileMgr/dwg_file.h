#ifndef _DWG_FILE_H
#define _DWG_FILE_H

// ==================================================================
//		AutoDesk DWG File Reader
//
//	Based on CBinaryFile
//
// ==================================================================

#include "stdafx.h"
#include <afx.h>

#include "BitstreamFile.h"
#include "DwgTypes.h"
#include "Return.h"

// ==================================================================

typedef __int64 dwgHandle;

class dllExport CDWGFile : public CBitstreamFile
{
public:

	CDWGFile();

	bool	ReadBit();			// B
	BYTE	ReadCode();			// BB
	short	ReadBitShort();		// BS
	long	ReadBitLong();		// BL
	double	ReadBitDouble();	// BD
	double	ReadBitDoubleDef( double def );	// DD

	void	ReadBitExtrusion( int version, double val[3] );	// BE
	double	ReadBitThickess( int version );	// BT

	long	ReadModularChar( bool negationRequired );	// MC -- assumes never has more than 32 bits worth of data
	long	ReadModularShort();	// MS -- assumes never has more than 32 bits worth of data

	dwgHandle ReadHandle( BYTE* outcode );

	BYTE	ReadBYTE();
	BYTE	ReadNibble();
	INT16	ReadINT16( bool bigend=false );
	INT32	ReadINT32( bool bigend=false );
	DOUBLE	ReadDOUBLE( bool bigend=false );

	char	ReadRawChar();
	short	ReadRawShort();
	double	ReadRawDouble();
	long	ReadRawLong();

	int		BytesUsed()		{ return m_used; }

	void	TestBuffer( BYTE* data, int num );
	void	Test();

	void	MessagesActivate( bool activate )		{ m_msgs = activate; }

	~CDWGFile();

private:

	int		m_used;
	bool	m_msgs;
};

#endif
