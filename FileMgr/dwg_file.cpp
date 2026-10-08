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

CDWGFile::CDWGFile()
	: CBitstreamFile(),
	  m_msgs( FALSE )
{
}

CDWGFile::~CDWGFile()
{
}

// ==================================================================
//	ReadBit
//
//	B: Read one bit... return it as a boolean flag
//
bool
CDWGFile::ReadBit()
{
	BYTE byte;
	CBitstreamFile::ReadBit( &byte );
	return (byte & 0x01);
}

// ==================================================================
//	ReadCode
//
//	BB: Read 2 bits, return as a byte
//
BYTE	
CDWGFile::ReadCode()
{
	BYTE code;
	CBitstreamFile::ReadBits( &code, 2 );

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
	BYTE byte;
	short val;
	BYTE bits;
	
	CBitstreamFile::ReadBits( &bits, 2 );
	switch (bits)
	{
	case 0:
		CBitstreamFile::ReadBytes( (BYTE*) &val, 2 );
		break;
	case 1:
		CBitstreamFile::ReadByte( &byte );
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
	long val = 0;
	int	bits = 0;
	bool swap = false;

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

	// CBitstreamFile::ReadBits( (BYTE*) &val, 2 );
	CBitstreamFile::ReadBits( (BYTE*) &val, bits );

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
	double val = 0;

	BYTE code = ReadCode();
	switch (code)
	{
		case 1:
			return 1.0;
		case 2:
			return 0.0;
	}

	CBitstreamFile::ReadBytes( (BYTE*) &val, 8 );

	return val;
}

/*
BitDouble With Default
This is a 2 bit opcode followed optionally by data, and it requires a default value.  The different opcodes are described as follows:
00	No more data present, use the value of the default double.
01	4 bytes of data are present.  The result is the default double,
	with the 4 data bytes patched in replacing the first 4 bytes of
	the default double (assuming little endian).
10	6 bytes of data are present.  The result is the default double,
	with the first 2 data bytes patched in replacing bytes 5 and 6 of
	the default double, and the last 4 data bytes patched in replacing
	the first 4 bytes of the default double (assuming little endian).
11	A full RD follows.
*/
// ==================================================================
//	ReadBitDoubleDef
//
double
CDWGFile::ReadBitDoubleDef( double def )
{
	double	val;
	BYTE	bytes[8];
	BYTE	code;
	BYTE*	ptr;

	m_used = 0;

	code = ReadCode();
	switch (code)
	{
	case 1:
		val = def;
		ptr = ((BYTE*) &val);
		CBitstreamFile::ReadBytes( ptr, 4 );
		break;
	case 2:
		val = def;
		CBitstreamFile::ReadBytes( bytes, 6 );
		ptr = ((BYTE*) &val);
		ptr[4] = bytes[0];
		ptr[5] = bytes[1];
		ptr[0] = bytes[2];
		ptr[1] = bytes[3];
		ptr[2] = bytes[4];
		ptr[3] = bytes[5];
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
CDWGFile::ReadBitExtrusion( int version, double val[3] )
{
	if (version == 15)
	{
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
	}
	else
	{
		val[0] = ReadBitDouble();
		val[1] = ReadBitDouble();
		val[2] = ReadBitDouble();
	}
}

double
CDWGFile::ReadBitThickess( int version )
{
	if (version == 15)
	{
		return (( ReadBit() ) ? 0 : ReadBitDouble());
	}
	else
	{
		return ReadBitDouble();
	}
}


// ==================================================================
//	ReadModularChar
//
//	MC: some number of bytes, compressed into a long
//
//	Freaking crazed AutoDesk bastards.
//	
long
CDWGFile::ReadModularChar( bool negationRequired )
{
	BYTE source[5];
	BYTE src;
	bool negate = FALSE;
	int	snum = 0;

	m_used = 0;

	// What if we have leftover bits from a bit compression?
	// Do we incorporate them here, or do we skip them and synchronize
	// on byte boundaries?
	//
	// I say... synchronize.  If this fails, then we need to replace the ReadByte()
	// with a bit stream shift for 8 bits.
	//
	// CBitstreamFile::SyncByte();		// "They consist of a stream of bytes..." .. assume bit sync

	while (TRUE)
	{
		m_used++;

		if ( !CBitstreamFile::ReadByte( &src ).isOkay() )
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

	if (negationRequired && (src & 0x40))
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

	return (( negate ) ? -val : val);
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
	// CBitstreamFile::SyncByte();		// "Modular shorts are just like modular chars..."... hah!

	while (TRUE)
	{
		m_used += 2;

		if ( !CBitstreamFile::ReadBytes( (BYTE*) &src, 2 ).IsOk() )
		{ return 0; }

		source[snum++] = src;
		if (!(src&0x8000))
		{ break; }
	}
	//
	// Check the last byte to see if we are negative
	//
/*	bool negate = FALSE;
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
dwgHandle
CDWGFile::ReadHandle( BYTE* outcode )
{
	dwgHandle val = 0;

	BYTE code = ReadNibble();
	if (outcode)
	{ *outcode = code; }

	BYTE count = ReadNibble();
	if (count > 0)
	{
		// It seems handles can be 8 bytes.  Long allows this.
		if (count <= 8)
		{
			BYTE	buf[8];
			BYTE*	ptr;

			CBitstreamFile::ReadBytes( buf, count );

			ptr = ((BYTE*) &val);

			while (count > 0)
			{
				--count;
				(*ptr) = buf[count];
				++ptr;
			}
		}
		else if ( m_msgs )
		{
			CString msg;
			msg.Format( "CDWGFile::ReadHandle() -- count = %d", count );
			MessageBox( NULL, msg, "Fatal Error", MB_OK );
		}
	}

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

	if ( !CBitstreamFile::ReadByte( &val ).IsOk() )
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
	CBitstreamFile::ReadBits( &val, 4 );
	return val;
}



// ==================================================================
//		ReadINT16
//
//		Read one 16-bit integer, shortcut
//
INT16
CDWGFile::ReadINT16( bool bigend )
{
	INT16 val;

	m_used = 2;

	if ( !CBitstreamFile::ReadBytes( (BYTE*) &val, 2 ).isOkay() )
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
CDWGFile::ReadINT32( bool bigend )
{
	INT32 val;

	m_used = 4;

	if ( !CBitstreamFile::ReadBytes( (BYTE*) &val, 4 ).isOkay() )
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
CDWGFile::ReadDOUBLE( bool bigend )
{
	DOUBLE val;

	m_used = 8;

	if ( !CBitstreamFile::ReadBytes( (BYTE*) &val, 8 ).isOkay() )
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

	if ( !CBitstreamFile::ReadBytes( (BYTE*) &val, 1 ).IsOk() )
		return 0;

	return val;
}

short
CDWGFile::ReadRawShort()
{
	short val;

	m_used = 2;

	if ( !CBitstreamFile::ReadBytes( (BYTE*) &val, 2 ).IsOk() )
		return 0;

	return val;
}

double
CDWGFile::ReadRawDouble()
{
	double val;

	m_used = 8;

	if ( !CBitstreamFile::ReadBytes( (BYTE*) &val, 8 ).IsOk() )
		return 0.;

	return val;
}

long
CDWGFile::ReadRawLong()
{
	long val;

	m_used = 4;

	if ( !CBitstreamFile::ReadBytes( (BYTE*) &val, 4 ).IsOk() )
		return 0;

	return val;
}



#include "assert.h"

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

		assert( ReadModularChar( TRUE ) == 4610 );
		assert( ReadModularChar( TRUE ) == 112823273 );
		assert( ReadModularChar( TRUE ) == -1413 );
	}

	{
		BYTE test5[] = {0x31, 0xf4, 0x8d, 0x00, 0x00};
		TestBuffer( test5, sizeof(test5) );

		assert( ReadModularShort() == 4650033 );
	}

}
