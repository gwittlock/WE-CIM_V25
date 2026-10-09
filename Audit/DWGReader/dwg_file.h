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

#include "MyBinaryFile.h"
#include "DwgTypes.h"
#include "Return.h"

// ==================================================================

class dllExport CDWGFile
{
public:

	CDWGFile();

	CReturn	Open( const CString& in_name, eFileMode in_mode )
		{ return m_file.Open( in_name, in_mode ); }

	CReturn Close( void )
		{ return m_file.Close(); }

	CReturn	ReadBytes( BYTE* buf, int num )
		{ return m_file.ReadBytes( buf, num ); }

	CReturn Seek( LONG lOff, UINT from )
		{ return m_file.Seek( lOff, from ); }

	BOOL	ReadBit();			// B
	BYTE	ReadCode();			// BB
	short	ReadBitShort();		// BS
	long	ReadBitLong();		// BL
	double	ReadBitDouble();	// BD
	double	ReadBitDoubleDef( double def );	// DD

	void	ReadBitExtrusion( double val[3] );	// BE
	double	ReadBitThickess();	// BT

	long	ReadModularChar();	// MC -- assumes never has more than 32 bits worth of data
	long	ReadModularShort();	// MS -- assumes never has more than 32 bits worth of data

	long	ReadHandle( BYTE* outcode );

	void	Test();
	void	TestBuffer( BYTE* data, int num );

	BYTE	ReadBYTE();
	BYTE	ReadNibble();
	INT16	ReadINT16( BOOL bigend=false );
	INT32	ReadINT32( BOOL bigend=false );
	DOUBLE	ReadDOUBLE( BOOL bigend=false );

	char	ReadRawChar();
	short	ReadRawShort();
	double	ReadRawDouble();
	long	ReadRawLong();

	void	ReadBits( BYTE* buf, int nbits )
		{ m_file.ReadBits( buf, nbits ); }

	int		Used( void ) const		{ return m_used; }

	void	BitCounter( BOOL on )
		{ m_file.BitCounter( on ); }

	int		BitCountGet()
		{ return m_file.BitCountGet(); }

	void	TestBufferSet( BYTE* bytes, int nbytes )
		{ m_file.TestBufferSet( bytes, nbytes ); }

	void DumpBytes( const CString& path, int nbytes )
		{ m_file.DumpBytes( path, nbytes ); }

	void DumpBits( const CString& path, int nbytes )
		{ m_file.DumpBits( path, nbytes ); }

	~CDWGFile();

private:

#if REQUIRED

	// Low-level helpers
	void	first_bit();
	BYTE	shift_bit();

	// Slightly more advanced helpers
	BYTE	do_read_bit();

#endif

private:

	CMyBinaryFile	m_file;

	int		m_used;			// How many bytes were used in the last operation
};

#endif
