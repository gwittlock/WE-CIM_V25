// ==================================================================
//		AutoDesk DWG File Reader
//
//	Based on CBinaryFile
//
//	Note:  None of these calls return an error code... we assume
//	you are checking isEOF() and the file is open and stuff...
//	Failure means returning a default value, typically 0 or its
//	equivalent.
//
// ==================================================================

#include "stdafx.h"
#include "dwg_file.h"

// ==================================================================

const int OUTBUF_SIZE = sizeof(double)*3;

// ==================================================================

CDWGFile::CDWGFile()
{
	m_bitidx = -1;
	m_byte = 0;
}

CDWGFile::~CDWGFile()
{
}

// ==================================================================

void	
CDWGFile::SyncByte()
{
	// A bitidx of 0 means we are on a byte boundary anyway
	// A bitidx of -1 means we are off the bit stream entirely
	// Any other bitidx means we need to skip ahead... by marking it
	//	at -1, we discard the current byte and step out of the bitstream
	//
	if (m_bitidx != 0)
	{ m_bitidx = -1; }
}


// ==================================================================
//	ReadBit
//
//	B: Read one bit... return it as a boolean flag
//
BOOL
CDWGFile::ReadBit()
{
	return (do_read_bit != 0);
}

void
CDWGFile::first_bit()
{ 
	m_bitidx=7;
}

BYTE
CDWGFile::shift_bit()
{
	BYTE bit = (m_byte&0x80)?0x01:0x00;	// Roll bit to LSB, for easy shifting
	m_bitidx--; 
	m_byte<<=1; 
	return bit;
}

BYTE
CDWGFile::do_read_bit()
{
	if (m_bitidx < 0)
	{
		if (!ReadByte( &m_byte ).isOkay())
		{ return 0x00; }

		first_bit();
	}

	return shift_bit();
}

// ==================================================================
//	ReadCode
//
//	BB: Read 2 bits, return as a byte
//
BYTE	
CDWGFile::ReadCode()
{
	BYTE code = do_read_bit();
	code <<= 1;
	code |= do_read_bit();

	return code;
}

// ==================================================================
//	ReadBitShort
//
//	BS:  Read 2 to 18 bits, depending on the code
//
//	Code
//	00: 2 bytes follow
//	01: 1 byte follows
//	10: value = 0
//	11: value = 256
//	
short	
CDWGFile::ReadBitShort()
{
	short val = 0;
	int	bits = 0;

	BYTE code = ReadCode();
	switch (code)
	{
		case 0:
			bits = 16;
			break;
		case 1:
			bits = 8;
			break;
		case 3:
			return 256;
	}

	// This could be made more efficient.. BUT, this is easy
	while (bits--)
	{
		val<<=1;
		val |= do_read_bit();
	}

	return val;
}

// ==================================================================
//	ReadBitLong
//
//	BL:  Read 2 to 34 bits, depending on the code
//
//	Code
//	00: 4 bytes follow
//	01: 1 byte follows
//	10: value = 0
//	11: not used
//	
long
CDWGFile::ReadBitLong()
{
	long val = 0;
	int	bits = 0;

	BYTE code = ReadCode();
	switch (code)
	{
		case 0:
			bits = 32;
			break;
		case 1:
			bits = 8;
			break;
	}

	// This could be made more efficient.. BUT, this is easy
	while (bits--)
	{
		val<<=1;
		val |= do_read_bit();
	}

	return val;
}


// ==================================================================
//	ReadBitDouble
//
//	BD:  Read 2 to 66 bits, depending on the code
//
//	Code
//	00: 8 bytes Double follow
//	01: 1.0
//	10: 0.0
//	11: not used
//	
double
CDWGFile::ReadBitDouble()
{
	double val = 0;

	BYTE code = ReadCode();
	switch (code)
	{
		case 1:
			return 1.0;
		case 2:
			return 0.0;
	}

	// This could be made WAY more efficient.. BUT, this is easy
	// We know we need 64 bits... or 8 bytes
	BYTE* buffer = (BYTE*)&val;
	for (int idx=7; idx>=0; idx--)
	{
		BYTE* byte = &buffer[idx];
		*byte = 0;

		int bits = 8;
		while (bits--)
		{
			*byte <<= 1;
			*byte |= do_read_bit();
		}
	}

	return val;
}


// ==================================================================
//	ReadModularChar
//
//	MC: some number of bytes, compressed into a long
//
//	Freaking crazed AutoDesk bastards.
//	
long
CDWGFile::ReadModularChar()
{
	// What if we have leftover bits from a bit compression?
	// Do we incorporate them here, or do we skip them and synchronize
	// on byte boundaries?
	//
	// I say... synchronize.  If this fails, then we need to replace the ReadByte()
	// with a bit stream shift for 8 bits.
	//
	SyncByte();		// "They consist of a stream of bytes..." .. assume bit sync

	BYTE source[5];
	int	snum = 0;

	BYTE src;
	while (TRUE)
	{
		if (!ReadByte( &src ).isOkay())
		{ return 0; }

/* Alternate, if we can't Sync()
		int bits = 8;
		src = 0x00;
		while (bits--)
		{
			src<<=1;
			src|= do_read_bit();
		}
*/

		source[snum++] = src;
		if (!(src&0x80))
		{ break; }
	}
	//
	// Check the last byte to see if we are negative
	//
	BOOL negate = FALSE;
	if (src & 0x40)
	{
		src &= ~0x40;
		source[snum-1] = src;
		negate = TRUE;
	}
	//
	// Okay, we have snum bytes in our array;  shift them to our value backwards,
	//	skipping the high bits
	//
	long val = 0;
	DWORD mask = 0x01;

	while (--snum >= 0)
	{
		BYTE byte = source[snum];
		for (int bit=0; bit<7; bit++)
		{
			if (byte & 0x01)
			{ val |= mask; }

			byte >>= 1;
			mask <<= 1;
		}
	}
	//
	//
	//
	if (negate)
	{ return -val; }

	return val;
}

// ==================================================================
//	ReadModularShort
//
//	MS: some number of shorts, compressed into a long
//
//	Freaking crazed AutoDesk bastards, wasn't ModularChar ENOUGH!
//	
long
CDWGFile::ReadModularShort()
{
	// What if we have leftover bits from a bit compression?
	// Do we incorporate them here, or do we skip them and synchronize
	// on byte boundaries?
	//
	// I say... synchronize.  If this fails, then we need to replace the ReadByte()
	// with a bit stream shift for 8 bits.
	//
	SyncByte();		// "Modular shorts are just like modular chars..."... hah!

	WORD source[2];
	int	snum = 0;

	WORD src;
	while (TRUE)
	{
		if (!ReadWord( &src ).isOkay())
		{ return 0; }

		source[snum++] = src;
		if (!(src&0x0080))
		{ break; }
	}
	//
	// Check the last byte to see if we are negative
	//
/*	BOOL negate = FALSE;
	if (src & 0x4000)
	{
		src &= ~0x40;
		source[snum-1] = src;
		negate = TRUE;
	}
*/
	//
	// Okay, we have snum words in our array;  shift them to our value
	//
	long val = 0;
	DWORD mask = 0x01;

	while (--snum >= 0)
	{
		WORD word = source[snum];
		BYTE* subsrc = (BYTE*) &word;  // PE -- 07/02/02 Added (BYTE*) cast

		// Swap the bytes in the short... bastards
		BYTE byte = subsrc[1];
		for (int bit=0; bit<8; bit++)
		{
			if (byte & 0x01)
			{ val |= mask; }

			byte >>= 1;
			mask <<= 1;
		}

		byte = subsrc[0];
		for (bit=0; bit<7; bit++)
		{
			if (byte & 0x01)
			{ val |= mask; }

			byte >>= 1;
			mask <<= 1;
		}
	}
	//
	//
	//
//	if (negate)
//	{ return -val; }

	return val;
}

// ==================================================================
//		ReadBYTE
//
//		Read one byte, shortcut
//
BYTE
CDWGFile::ReadBYTE()
{
	BYTE val;

	if ( !ReadByte( &val ).IsOk() )
	{ return 0; }

	return val;
}

// ==================================================================
//		ReadINT16
//
//		Read one 16-bit integer, shortcut
//
INT16
CDWGFile::ReadINT16()
{
	INT16 val;

	if (!Read( (BYTE*) &val, 2 ).isOkay())
	{ return 0; }

#if BIG_ENDIAN
	INT16 n;
	char *s,*d;

	d = (char*) &n;
	s = (char*) &val;
	s += 1;
	*d++ = (*s--)&0xff;
	*d = (*s)&0xff;

	return( n );
#else
	return val;
#endif
}

// ==================================================================
//		ReadINT32
//
//		Read one 32-bit integer, shortcut
//
INT32
CDWGFile::ReadINT32()
{
	INT32 val;

	if (!Read( (BYTE*) &val, 4 ).isOkay())
	{ return 0; }

#if BIG_ENDIAN
	INT32 n;
	char *s,*d;

	d = (char*) &n;
	s = (char*) &val;
	s += 3;
	*d++ = *s--;
	*d++ = *s--;
	*d++ = *s--;
	*d = *s;

	return( n );
#else
	return val;
#endif
}

// ==================================================================
//		ReadINT32
//
//		Read one 32-bit double, shortcut
//
DOUBLE
CDWGFile::ReadDOUBLE()
{
	DOUBLE val;

	if (!Read( (BYTE*) &val, 8 ).isOkay())
	{ return 0; }

#if BIG_ENDIAN
	DOUBLE n;
	char* s;
	char* d;

	d = (char*) &n;
	s = (char*) &val;
	s += 7;
	*d++ = *s--;	/* long winded but faster than for loop */
	*d++ = *s--;
	*d++ = *s--;
	*d++ = *s--;
	*d++ = *s--;
	*d++ = *s--;
	*d++ = *s--;
	*d = *s;

	return( n );
#else
	return val;
#endif
}
