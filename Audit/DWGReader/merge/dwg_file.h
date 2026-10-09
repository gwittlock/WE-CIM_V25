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

#include "binary_file.h"
#include "Types.h"
#include "Return.h"

// ==================================================================

class dllExport CDWGFile : public CBinaryFile
{
public:

	CDWGFile();

	void	SyncByte();			// Synchronize the bitstream, to start again on and even byte

	BOOL	ReadBit();			// B
	BYTE	ReadCode();			// BB
	short	ReadBitShort();		// BS
	long	ReadBitLong();		// BL
	double	ReadBitDouble();	// BD

	long	ReadModularChar();	// MC -- assumes never has more than 32 bits worth of data
	long	ReadModularShort();	// MS -- assumes never has more than 32 bits worth of data

	BYTE	ReadBYTE();
	INT16	ReadINT16();
	INT32	ReadINT32();
	DOUBLE	ReadDOUBLE();

	~CDWGFile();

private:

	// Low-level helpers
	void	first_bit();
	BYTE	shift_bit();

	// Slightly more advanced helpers
	BYTE	do_read_bit();

private:

	int		m_bitidx;		// -1 for off the bitstream, otherwise the bit index, LSB first
	BYTE	m_bitflag;		// Alternate form of bitidx... the bit mask
	BYTE	m_byte;			// Current byte being disected by bitidx; not relevant if bitidx < 0

	BYTE*	m_outbuf;		// Buffer to shift into; NOT owned by this object (an alias)
};

#endif