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
const double DEFAULT_DOUBLE = 0.;

// ==================================================================

CDWGFile::CDWGFile()
	: m_file()
{
}

CDWGFile::~CDWGFile()
{
}

// ==================================================================
//	TestBuffer
//
//	Feed this data into the file buffer, for testing
void	
CDWGFile::TestBuffer( BYTE* data, int num )
{
#if OKAY
	if (m_buf) delete m_buf;
	m_buf = new BYTE[num];
	m_buflen = num;
	m_byteidx = 0;

	m_bitidx = -1;

	for (int idx=0; idx<num; idx++)
	{
		m_buf[idx] = data[idx];
	}

	m_mode = FILEMODE_READ;
#endif
}

// ==================================================================
//	ReadBit
//
//	B: Read one bit... return it as a boolean flag
//
BOOL
CDWGFile::ReadBit()
{
	BYTE byte;
	m_file.ReadBit( &byte );
	return (byte & 0x01);
	// return (do_read_bit() != 0);
}

#if REQUIRED

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
		m_used++;

		if ( !m_file.ReadByte( &m_byte ).isOkay() )
		{ return 0x00; }

		first_bit();
	}

	return shift_bit();
}

#endif

// ==================================================================
//	ReadCode
//
//	BB: Read 2 bits, return as a byte
//
BYTE	
CDWGFile::ReadCode()
{
#if ORIGINAL
	BYTE code = do_read_bit();
	code <<= 1;
	code |= do_read_bit();
#else
	BYTE code;
	m_file.ReadBits( &code, 2 );
#endif
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
	m_used = 0;

	BYTE byte;
	short val;
	BYTE bits;
	
	m_file.ReadBits( &bits, 2 );
	switch (bits)
	{
	case 0:
		m_file.ReadBytes( (BYTE*) &val, 2 );
		break;
	case 1:
		m_file.ReadByte( &byte );
		val = byte;
		break;
	case 3:
		val = 256;
		break;
	default:
		val = 0;
		break;
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
	m_used = 0;

	long val = 0;
	int	bits = 0;
	BOOL swap = false;

	BYTE code = ReadCode();
	switch (code)
	{
	case 0:
		bits = 32;
		swap = true;
		break;
	case 1:
		bits = 8;
		break;
	default:		// including "case2:"
		return 0;
	}

	// This could be made more efficient.. BUT, this is easy
#if ORIGINAL
	while (bits--)
	{
		val<<=1;
		val |= do_read_bit();
	}
#else
	m_file.ReadBits( (BYTE*) &val, 2 );
#endif

	// Swap to big-endian
	if (swap)
	{
		WORD* ptr = (WORD*)&val;
		WORD tmp = ptr[0];
		ptr[0] = ptr[1];
		ptr[1] = tmp;
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
	m_used = 0;

	double val = 0;

	BYTE code = ReadCode();
	switch (code)
	{
		case 1:
			return 1.0;
		case 2:
			return 0.0;
	}

#if ORIGINAL
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
#else
	m_file.ReadBytes( (BYTE*) &val, 8 );
#endif

	return val;
}


// ==================================================================
//	ReadBitDoubleDef
//
double
CDWGFile::ReadBitDoubleDef( double def )
{
	double val;
	BYTE bytes[6];
	BYTE code;

	m_used = 0;

	code = ReadCode();
	switch (code)
	{
	case 1:
		m_file.ReadBytes( bytes, 4 );	// TODO:
		break;
	case 2:
		m_file.ReadBytes( bytes, 6 );	// TODO:
		break;
	case 3:
		val = ReadRawDouble();
		break;
	default:
		val = def;
		break;
	}

	return val;
}

void
CDWGFile::ReadBitExtrusion( double val[3] )
{
#if R13_R14		// TODO:
	val[0] = ReadBitDouble();
	val[1] = ReadBitDouble();
	val[2] = ReadBitDouble();
#else
	if ( ReadBit() )
	{
		val[0] = 0.;
		val[1] = 0.;
		val[2] = 1.;
	}
	else
	{
		val[0] = ReadBitDouble();
		val[1] = ReadBitDouble();
		val[2] = ReadBitDouble();
	}
#endif
}

double
CDWGFile::ReadBitThickess()
{
#if R13_R14		// TODO:
	ReadBitDouble();
#else
	if ( ReadBit() )
		return 0.;
	else
		return ReadBitDouble();
#endif
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
	BYTE source[5];
	BYTE src;
	int	snum = 0;

	m_used = 0;

	// What if we have leftover bits from a bit compression?
	// Do we incorporate them here, or do we skip them and synchronize
	// on byte boundaries?
	//
	// I say... synchronize.  If this fails, then we need to replace the ReadByte()
	// with a bit stream shift for 8 bits.
	//
	// m_file.SyncByte();		// "They consist of a stream of bytes..." .. assume bit sync

	while (TRUE)
	{
		m_used++;

		if ( !m_file.ReadByte( &src ).isOkay() )
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

//	while (--snum >= 0)
	for (int idx=0; idx<snum; idx++)
	{
		BYTE byte = source[idx];
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
	WORD source[2];
	WORD src;
	int	snum = 0;

	m_used = 0;

	// What if we have leftover bits from a bit compression?
	// Do we incorporate them here, or do we skip them and synchronize
	// on byte boundaries?
	//
	// I say... synchronize.  If this fails, then we need to replace the ReadByte()
	// with a bit stream shift for 8 bits.
	//
	// m_file.SyncByte();		// "Modular shorts are just like modular chars..."... hah!

	while (TRUE)
	{
		m_used += 2;

		if ( !m_file.ReadBytes( (BYTE*) &src, 2 ).IsOk() )
		{ return 0; }

		source[snum++] = src;
		if (!(src&0x8000))
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

//	while (--snum >= 0)
	for (int idx=0; idx<snum; idx++)
	{
		WORD word = source[idx];

		for (int bit=0; bit<15; bit++)
		{
			if (word & 0x01)
			{ val |= mask; }

			word >>= 1;
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
//	ReadHandle
//
//	Read handle data
//
long
CDWGFile::ReadHandle( BYTE* outcode )
{
	long val = 0;

	BYTE code = ReadNibble();
	if (outcode)
	{ *outcode = code; }

	BYTE count = ReadNibble();
	if (count > 0)
	{ m_file.ReadBytes( (BYTE*)&val, count ); }

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

	m_used = 1;

	if ( !m_file.ReadByte( &val ).IsOk() )
	{ return 0; }

	return val;
}

// ==================================================================
//	ReadNibble
//
//	Read 4 bits, return as a byte (used for handle codes and counts)
//
BYTE	
CDWGFile::ReadNibble()
{
	BYTE val;
	m_file.ReadBits( &val, 4 );
	return val;
}



// ==================================================================
//		ReadINT16
//
//		Read one 16-bit integer, shortcut
//
INT16
CDWGFile::ReadINT16( BOOL bigend )
{
	INT16 val;

	m_used = 2;

	if ( !m_file.ReadBytes( (BYTE*) &val, 2 ).isOkay() )
	{ return 0; }

	if (bigend)
	{
		INT16 n;
		char *s,*d;

		d = (char*) &n;
		s = (char*) &val;
		s += 1;
		*d++ = (*s--)&0xff;
		*d = (*s)&0xff;

		return( n );
	}

	return val;
}

// ==================================================================
//		ReadINT32
//
//		Read one 32-bit integer, shortcut
//
INT32
CDWGFile::ReadINT32( BOOL bigend )
{
	INT32 val;

	m_used = 4;

	if ( !m_file.ReadBytes( (BYTE*) &val, 4 ).isOkay() )
	{ return 0; }

	if (bigend)
	{
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
	}

	return val;
}

// ==================================================================
//		ReadINT32
//
//		Read one 32-bit double, shortcut
//
DOUBLE
CDWGFile::ReadDOUBLE( BOOL bigend )
{
	DOUBLE val;

	m_used = 8;

	if ( !m_file.ReadBytes( (BYTE*) &val, 8 ).isOkay() )
	{ return 0; }

	if (bigend)
	{
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
	}

	return val;
}

char
CDWGFile::ReadRawChar()
{
	char val;

	m_used = 1;

	if ( !m_file.ReadBytes( (BYTE*) &val, 1 ).IsOk() )
		return 0;

	return val;
}

short
CDWGFile::ReadRawShort()
{
	short val;

	m_used = 2;

	if ( !m_file.ReadBytes( (BYTE*) &val, 2 ).IsOk() )
		return 0;

	return val;
}

double
CDWGFile::ReadRawDouble()
{
	double val;

	m_used = 8;

	if ( !m_file.ReadBytes( (BYTE*) &val, 8 ).IsOk() )
		return 0.;

	return val;
}

long
CDWGFile::ReadRawLong()
{
	long val;

	m_used = 4;

	if ( !m_file.ReadBytes( (BYTE*) &val, 4 ).IsOk() )
		return 0;

	return val;
}



#include "assert.h"

void	
CDWGFile::Test()
{
	// Remember to append 0x00 to each buffer, so the Read doesn't try to re-fill the cache
	{
		BYTE test1[] = {0x00, 0x40, 0x6d, 0x0f, 0x80, 0x00};
		TestBuffer( test1, sizeof(test1) );

		assert( ReadBitShort() == 257 );
		assert( ReadBitShort() == 0 );
		assert( ReadBitShort() == 256 );
		assert( ReadBitShort() == 15 );
		assert( ReadBitShort() == 0 );
	}

	{
		BYTE test2[] = {0x00, 0x40, 0x40, 0x00, 0x24, 0x3e, 0x00};
		TestBuffer( test2, sizeof(test2) );

		assert( ReadBitLong() == 257 );
		assert( ReadBitLong() == 0 );
		assert( ReadBitLong() == 15 );
		assert( ReadBitLong() == 0 );
	}

/*	
	{
		double d = 1.5;
		BYTE* pd = (BYTE*)&d;

		BYTE test3[] = {0x64, pd[0], pd[1], pd[2], pd[3], pd[4], pd[5], pd[6], pd[7], 0x90, 0x00};
		TestBuffer( test3, sizeof(test3) );

		assert( ReadBitDouble() == 1.0 );
		assert( ReadBitDouble() == 0.0 );
		assert( ReadBitDouble() == 1.0 );
		assert( ReadBitDouble() == 1.5 );
		assert( ReadBitDouble() == 0.0 );
		assert( ReadBitDouble() == 1.0 );
	}
*/

	{
		BYTE test4[] = {0x82, 0x24, 0xe9, 0x97, 0xe6, 0x35, 0x85, 0x4b, 0x00};
		TestBuffer( test4, sizeof(test4) );

		assert( ReadModularChar() == 4610 );
		assert( ReadModularChar() == 112823273 );
		assert( ReadModularChar() == -1413 );
	}

	{
		BYTE test5[] = {0x31, 0xf4, 0x8d, 0x00, 0x00};
		TestBuffer( test5, sizeof(test5) );

		assert( ReadModularShort() == 4650033 );
	}

}
