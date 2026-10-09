
#include "DwgReader.h"

CString ROOTDIR( "c:\\_Weng\\DwgReader\\" );

int NamesCompare( void* ptrA, void* ptrB );
int HandlesCompare( void* ptrA, void* ptrB );

CString file( const CString& name );
void dumpDoubles( double* val, int count );

BYTE g_fodder[1024];

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// BEGIN INNER CLASS DEFINITIONS
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CSectionLocator
	{
	public:

		CSectionLocator( UINT8 recno, INT32 start, INT32 size )
		{
			m_recno = recno;
			m_start = start;
			m_size  = size;
		}

		UINT8 RecNo()	{ return m_recno; }
		INT32 Start()	{ return m_start; }
		INT32 Size()	{ return m_size; }

	private:

		UINT8	m_recno;
		INT32	m_start;
		INT32	m_size;
	};
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CObjectLocator
	{
	public:

		CObjectLocator( int objHandle, long objOffset )
		{
			m_objHandle	= objHandle;
			m_objOffset	= objOffset;
			m_type		= NULL;
		}

		INT16 Handle()	{ return m_objHandle; }
		INT32 Offset()	{ return m_objOffset; }
		char* Type()	{ return m_type; }

		void Type( char* type )		{ m_type = type; }

	private:

		int		m_objHandle;
		long	m_objOffset;
		char*	m_type;
	};
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CClassMapper
	{
	public:

		CClassMapper( const CString& dxfname )
		{
			Init( dxfname );
		}

		CString Name()	{ return m_name; }
		short	Type()	{ return m_type; }

	private:

		void Init( const CString& dxfname )
		{
			m_name = dxfname;
			m_type = 0;		// unknown

			if (m_name.CompareNoCase("LWPOLYLINE") == 0)
				m_type = DWG_POLYLINE_2D;
		}

	private:

		CString	m_name;
		short	m_type;
	};
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CCommonEntityData
	{
	public:

		CCommonEntityData()
		{
		}

		void Init( int objHandle, short objType, long objSize )
		{
			m_handle	= objHandle;
			m_type		= objType;
			m_bytes		= objSize;
		}

		void Update(
				BYTE entmode,
				long numreact,
				BOOL isByLayerLT,
				long lineTypeFlags,
				long plotTypeFlags )
		{
			m_entmode		= entmode;
			m_numreact		= numreact;
			m_isByLayerLT	= isByLayerLT;
			m_lineTypeFlags	= lineTypeFlags;
			m_plotTypeFlags	= plotTypeFlags;
		}

		int		Handle() const		{ return m_handle; }
		short	Type() const		{ return m_type; }
		long	Size() const		{ return m_bytes; }

		BYTE	EntMode() const		{ return m_entmode; }
		long	NumReact() const	{ return m_numreact; }
		BOOL	IsByLayerLT() const	{ return m_isByLayerLT; }

		long	LineTypeFlags() const	{ return m_lineTypeFlags; }
		long	PlotTypeFlags() const	{ return m_plotTypeFlags; }

	private:

		int		m_handle;
		short	m_type;
		long	m_bytes;

		BYTE	m_entmode;
		long	m_numreact;
		BOOL	m_isByLayerLT;

		long	m_lineTypeFlags;
		long	m_plotTypeFlags;
	};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// END INNER CLASS DEFINITIONS
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


CDwgReader::CDwgReader()
	: m_version( 0 ),
	  m_layersOnly( FALSE ),
	  m_dump( FALSE )
{
}

CDwgReader::~CDwgReader()
{
	m_sections.DestructiveFlush();
	m_objects.DestructiveFlush();
	m_classes.DestructiveFlush();
	m_layers.DestructiveFlush();
}

CReturn
CDwgReader::ReadModel( const CString& full_path, const CDynamicArray<CString*>& acceptedLayers )
{
	CReturn status;

	status = m_file.Open( full_path, FILEMODE_READ );

	if ( status.IsOk() )
		status = ReadTables();

	if ( status.IsOk() )
		status = DrawingObjectsRead( TRUE );

	if ( status.IsOk() )
	{
		LayersCull( acceptedLayers );

		status = DrawingObjectsRead( FALSE );
	}

	m_file.Close();

	return status;
}

CReturn
CDwgReader::ReadLayers( const CString& full_path )
{
	CReturn status;

	status = m_file.Open( full_path, FILEMODE_READ );

	if ( status.IsOk() )
		status = ReadTables();

	if ( status.IsOk() )
		status = DrawingObjectsRead( TRUE );

	m_file.Close();

	return status;
}

const CLayerList&
CDwgReader::Layers()
{
	return m_layers;
}

CReturn
CDwgReader::ReadTables()
{
	CReturn status;

#if REQUIRED
	// For decoding dwg files, prints floating point numbers as
	// a series of zeros and ones.  Can be used to locate patterns
	// in a dwg file.
	double dbls[] = {-1., 1., 2., 3., 4., 5., 6., 7., 8.};
	dumpDoubles( dbls, (sizeof( dbls ) / sizeof( double )) );
#endif

	if ( status.IsOk() )
		status = VersionRead();

	if ( status.IsOk() )
		status = SectionTableRead();

	if ( status.IsOk() )
		status = ClassSectionRead();

	if ( status.IsOk() )
		status = ObjectMapRead();

	return status;
}

CReturn
CDwgReader::VersionRead()
{
	CReturn status;
	char version[8];

	m_file.Seek( 0L, CFile::begin );
	m_file.ReadBytes( (BYTE*) version, 8 );

	if (stricmp( version, "AC1013" ) == 0)
		m_version = 13;
	else if (stricmp( version, "AC1014" ) == 0)
		m_version = 14;
	else if (stricmp( version, "AC1015" ) == 0)
		m_version = 15;
	else
		status.Internal( IDS_INTERNAL_ERROR, "CDwgReader::VersionRead() -- Unrecognized file tpye." );

	return status;
}

CReturn
CDwgReader::SectionTableRead()
{
	CReturn status;

	int		section_count;
	int		indx;
	UINT8	recno;
	INT32	start;
	INT32	size;

	m_file.Seek( 0x15, CFile::begin );

	section_count = m_file.ReadINT32();;

	for (indx = 0; indx < section_count; ++indx)
	{
		recno = m_file.ReadBYTE();
		start = m_file.ReadINT32();
		size  = m_file.ReadINT32();

		m_sections.Append( new CSectionLocator( recno, start, size ) );
	}

	return status;
}

CReturn
CDwgReader::DrawingObjectsRead(  BOOL layersOnly )
{
	CReturn status;
	
	int		count, indx;
	int		objHandle;
	long	objSize;
	short	objType;

	m_layersOnly = layersOnly;

	count = m_objects.Count();
	for (indx = 0; indx < count; ++indx)
	{
		m_file.Seek( m_objects[indx]->Offset(), CFile::begin );

		m_file.BitCounter( TRUE );

		objSize = m_file.ReadModularShort();
		objType = m_file.ReadBitShort();

		objType = ObjectTypeConvert( objType );

		m_objects[indx]->Type( ObjectString( objType ) );

		if ( MustProcess( objType ) )
		{
			objHandle = m_objects[indx]->Handle();
			ObjectRead( objHandle, objType, objSize );
		}

		m_file.BitCounter( FALSE );
	}

	DumpObjectTable( "_objtable.txt" );

	return status;
}

/*
5) CLASS DEFINITIONS
This section contains the defined classes for the drawing.

	SN	:	0x8D 0xA1 0xC4 0xB8 0xC4 0xA9 0xF8 0xC5 0xC0 0xDC 0xF4 0x5F 0xE7 0xCF 0xB6 0x8A.
	RL	:	size of class data area.
Then follow the class data:

	BS	:	classnum
	BS	:	version - in R14, becomes a flag indicating whether objects can be moved, edited, etc.  We are still examining this.
 	T	:	appname
 	T	:	cplusplusclassname
 	T	:	classdxfname
 	B	:	wasazombie
	BS	:	itemclassid -- 0x1F2 for classes which produce entities, 0x1F3 for classes which produce objects.
We read sets of these until we exhaust the data.

	RS	:	CRC
This following 16-byte sentinel appears after the CRC:
0x72,0x5L,0x3B,0x47,0x3B,0x56,0x07,0x3A,0x3F,0x23,0x0B,0xA0,0x18,0x30,0x49,0x75
*/
CReturn
CDwgReader::ClassSectionRead()
{
	CReturn status;

	CString	appname;
	CString	cppname;
	CString	dxfname;
	short	classnum;
	short	version;
	short	classid;

	m_file.Seek( m_sections[1]->Start(), CFile::begin );

	m_file.ReadBytes( g_fodder, 16 );
	m_file.ReadRawLong();

	while (1)
	{
		classnum = m_file.ReadBitShort();
		if (classnum < 500)
			break;

		version = m_file.ReadBitShort();
		appname = ReadString();
		cppname = ReadString();
		dxfname = ReadString();

		m_classes.Append( new CClassMapper( dxfname ) );

		m_file.ReadBit();	// was a zombie

		classid = m_file.ReadBitShort();
	}

	return status;
}

CReturn
CDwgReader::ObjectMapRead()
{
	CReturn status;

	INT16	CRC;
	short	sectionSize;

	m_file.Seek( m_sections[2]->Start(), CFile::begin );

	m_file.DumpBits( file("_objmap.txt"), (40*2) );  // bytes
	m_file.BitCounter( TRUE );

	int objHandle = 0;
	long objOffset = 0L;

	while (TRUE)
	{
		sectionSize = m_file.ReadINT16(true);
		if (sectionSize == 2)
		{ break; }

		objOffset = 0L;  // TODO: Verify this is correct (if not here, offset gets too big)

		while (sectionSize > 2)
		{
			objHandle += m_file.ReadModularChar();
			sectionSize -= m_file.Used();

			objOffset += m_file.ReadModularChar();
			sectionSize -= m_file.Used();

			m_objects.Append( new CObjectLocator( objHandle, objOffset ) );
		}

		CRC = m_file.ReadINT16(false);
	}

	m_file.BitCounter( FALSE );

	DumpObjectTable( "_objtable0.txt" );

	return status;
}

CReturn
CDwgReader::ObjectRead( int objHandle, short objType, long objSize )
{
	CReturn status;

	CCommonEntityData ced;
	char* type;  // for debugging
	
	ced.Init( objHandle, objType, objSize );


	type = ObjectString( objType );

	switch( objType )
	{
	case DWG_BLOCK_HEADER:
		m_file.DumpBits( file("_block_header.txt"), objSize );
		status = ReadBlockHeader( ced );
		break;
	case DWG_BLOCK:
		m_file.DumpBits( file("_block.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		status = ReadBlock( ced );
		break;
	case DWG_ENDBLK:
		m_file.DumpBits( file("_end_block.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		status = ReadEndBlock( ced );
		break;
	case DWG_VERTEX_2D:
		m_file.DumpBits( file("_vertex2d.txt"), objSize );
		break;
	case DWG_VERTEX_3D:
		m_file.DumpBits( file("_vertex3d.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		status = ReadVertex3d( ced );
		break;
	case DWG_POLYLINE_2D:
	case DWG_POLYGON:
		m_file.DumpBits( file("_poly2d.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		status = ReadPoly2d( ced );
		break;
	case DWG_POLYLINE_3D:
		m_file.DumpBits( file("_poly3d.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		status = ReadPoly3d( ced );
		break;
	case DWG_ARC:
		m_file.DumpBits( file("_arc.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		status = ReadArc( ced );
		break;
	case DWG_CIRCLE:
		m_file.DumpBits( file("_circ.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		status = ReadCircle( ced );
		break;
	case DWG_LINE:
		m_file.DumpBits( file("_line.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		status = ReadLine( ced );
		break;
	case DWG_POINT:
		break;
	case DWG_LAYER:
		m_file.DumpBits( file("_layer.txt"), objSize );
		status = ReadLayer( ced );
		break;
	case DWG_LAYER_CTRL_OBJ:
		m_file.DumpBits( file("_layctrl.txt"), objSize );
		break;
	default:
		m_file.DumpBits( file("_unknown.txt"), objSize );
		status = ReadCommonEntityData( &ced );
		break;
	}

	return status;
}

/*
Common Entity Data
Drawing entities, which are of course objects, have the same format as objects,
with some additional standard items:

	MS	:	Size of object, not including the CRC
	BS	:	Object type
R2000 Only:
	RL	:	Size of object data in bits
Common:
	 H	:	Object's handle
	BS	:	Size of extended object data, if any
	 X	:	Extended object data, if any
	 B	:	Flag indicating presence of graphic image.
			if (graphicimageflag is 1) {
			  RL: Size of graphic image in bytes
			   X: The graphic image
			}
R13-R14 Only:
	RL	:	Size of object data in bits
	6B	:	Flags
	6B	:	Common parameters
R2000 Only:
	B	:	0 if the previous and next linkers are present;
			1 if they are BOTH defaults (1 back and 1 forward).
	BS	:	Entity color
	BD	:	Linetype Scale
	BB	:	Line type flags
			00 - BYLAYER linetype
			01 - BYBLOCK linetype
			10 - CONTINUOUS linetype
			11 - Indicates that a linetype handle will be stored
				in the handles section of the entity.
	BB	:	Plotstyle flags:
			00 - BYLAYER plotstyle
			01 - BYBLOCK plotstyle
			10 - CONTINUOUS plotstyle
			11 - Indicates that a plotstyle handle will be stored
				in the handles section of the entity.
	BS	:	Invisible flag
	RC	:	Entity lineweight flag
Common:
	 X	:	Object data (varies by type of object)
	 X	:	Handles associated with this object
	RS	:	CRC
*/
CReturn
CDwgReader::ReadCommonEntityData( CCommonEntityData* ced )
{
	CReturn status;

	double	lineTypeScale;
	int		color;
	int		invisible;
	int		lineTypeFlags;
	int		plotStyleFlags;
	int		lineWeightFlag;
	int		handle;
	long	numreact;
	long	nbits;
	BYTE	entmode;
	BOOL	isByLayerLT;

	lineTypeFlags = 0;
	plotStyleFlags = 0;

	if (m_version == 15)
	{
		// Get the object size in bits.
		// NOTE: assert( (bits / 8) < objSize )
		nbits = m_file.ReadRawLong();
	}

	// Get the handle of this drawing object.  This handle
	// better match the handle that we have been given.
	handle = ReadHandleReference();

	// Just skip the "extended entity data"
	ReadExtendedEntityData();

	// Skip this stuff.
	ReadGraphicsData();
	
	if (m_version == 13 || m_version == 14)
	{
		// Get the object size in bits.
		// NOTE: assert( (bits / 8) < objSize )
		nbits = m_file.ReadRawLong();
	}

	entmode = m_file.ReadCode();

	numreact = m_file.ReadBitLong();
	
	if (m_version == 13 || m_version == 14)
	{
		isByLayerLT = m_file.ReadBit();
	}

	// "Linkers present" bit
	m_file.ReadBit();

	color = m_file.ReadBitShort();

	lineTypeScale = m_file.ReadBitDouble();

	if (m_version == 15)
	{
		lineTypeFlags = 0;
		m_file.ReadBits( (BYTE*) &lineTypeFlags, 2 );

		plotStyleFlags = 0;
		m_file.ReadBits( (BYTE*) &plotStyleFlags, 2 );
	}

	invisible = m_file.ReadBitShort();

	if (m_version == 15)
	{
		lineWeightFlag = m_file.ReadRawChar();
	}

	ced->Update( entmode, numreact, isByLayerLT, lineTypeFlags, plotStyleFlags );

	return status;
}

/*
BLOCK HEADER (49)
	Length	MS	---	Object length (not counting itself or CRC).
	Type	BS	0&2	49 (internal DWG type code).
R2000 Only:
	Obj size	RL		size of object in bits, not including end handles
Common:
	Handle	H	5	code 0, length followed by the handle bytes.
	EED	X	-3	See EED section.
R13-R14 Only:
	Obj size	RL		size of object in bits, not including end handles
Common:
	Numreactors	L		Number of persistent reactors attached to this obj
	Entry name	T	2
	64-flag	B	70	The 64-bit of the 70 group.
	xrefindex+1	BS	70	subtract one from this value when read.  After that, -1 indicates that this reference did not come from an xref, otherwise this value indicates the index of the blockheader for the xref from which this came.
	Xdep	B	70	block is dependent on an xref. (16 bit)
	Anonymous	B	1	if this is an anonymous block  (1 bit)
	Hasatts	B	1	if block contains attdefs    (2 bit)
	Blkisxref	B	1	if block is xref             (4 bit)
	Xrefoverlaid	B	1	if an overlaid xref          (8 bit)
R2000 Only:
	Loaded Bit	B		0 indicates loaded for an xref
Common:
	Base pt	3BD	10	Base point of block.
	Xref pname    	T	1	Xref pathname.  That's right: DXF 1 AND 3!
			3	1 appears in a tblnext/search elist; 3 appears in an entget.
R2000 Only:
	Insert Count	RC		A sequence of zero or more non-zero RC's, followed by a terminating 0 RC.  The total number of these indicates how many insert handles will be present.
	Block Description	T	4	Block description.
	Size of preview data	BL		Indicates number of bytes of data following. 
	Binary Preview Data	N*RC	310	 
Common:
	Handle refs	H		Block control handle (CODE 4)
				[Reactors (CODE 4)]
				xdicobjhandle (CODE 3)
				NULL (CODE 5)
				BLOCK entity. (CODE 3)
				if (!blkisxref && !xrefisoverlaid) {
				  first entity in the def. (CODE 4)
				  last entity in the def. (CODE 4)
				}
				ENDBLK entity. (CODE 3)
R2000 Only:
	Insert Handles	H		N insert handles, where N corresponds to the number of insert count entries above.
	Layout Handle	H	
Common:
	CRC	X	---
*/
CReturn
CDwgReader::ReadBlockHeader( const CCommonEntityData& ced )
{
	CReturn status;

	CString	entryName;
	double	lineTypeScale;
	int		color;
	int		invisible;
	int		lineTypeFlags;
	int		plotStyleFlags;
	int		lineWeightFlag;
	int		handle;
	long	numreact;
	long	nbits;
	BYTE	entmode;
	BOOL	isByLayerLT;

	lineTypeFlags = 0;
	plotStyleFlags = 0;

	if (m_version == 15)
	{
		// Get the object size in bits.
		// NOTE: assert( (bits / 8) < objSize )
		nbits = m_file.ReadRawLong();
	}

	// Get the handle of this drawing object.  This handle
	// better match the handle that we have been given.
	handle = ReadHandleReference();

	// Just skip the "extended entity data"
	ReadExtendedEntityData();
	
	if (m_version == 13 || m_version == 14)
	{
		// Get the object size in bits.
		// NOTE: assert( (bits / 8) < objSize )
		nbits = m_file.ReadRawLong();
	}

	numreact = m_file.ReadBitLong();

	entryName = ReadString();

	return status;
}

/*
BLOCK (4)
	Common Entity Data
	Block name	T	2
	Common Entity Handle Data
	CRC	X	---
*/
CReturn
CDwgReader::ReadBlock( const CCommonEntityData& ced )
{
	CReturn	status;

	CString	name;
	long	layerHandle;

	name = ReadString();

	layerHandle = ReadCommonEntityHandleData( ced );

	return status;
}

/*
ENDBLK (5)
	Common Entity Data
	Common Entity Handle Data
	CRC	X	---
*/
CReturn
CDwgReader::ReadEndBlock( const CCommonEntityData& ced )
{
	CReturn	status;

	long	layerHandle;

	layerHandle = ReadCommonEntityHandleData( ced );

	return status;
}

/*
LAYER (51)
	Length	MS	---	Object length (not counting itself or CRC).
	Type	BS	0&2	51 (internal DWG type code).
R2000 Only:
	Obj size	RL		size of object in bits, not including end handles
Common:
	Handle	H	5	code 0, length followed by the handle bytes.
	EED	X	-3	See EED section.
R13-R14 Only:
	Obj size	RL		size of object in bits, not including end handles
Common:
	Numreactors	BL		Number of persistent reactors attached to this obj
	Entry name	T	2
	64-flag	B	70	The 64-bit of the 70 group.
	xrefindex+1	BS	70	subtract one from this value when read.  After that, -1 indicates that this reference did not come from an xref, otherwise this value indicates the index of the blockheader for the xref from which this came.
	Xdep	B	70 	dependent on an xref.  (16 bit)
R13-R14 Only:
	Frozen	B	70	if frozen  (1 bit)
	On	B		if on.  Normal Autodesk (and OpenDWG Toolkit) policy is not to report this per se, but rather to negate the color if the layer is off.
	Frz in new	B	70	if frozen by default in new viewports (2 bit)
	Locked	B	70	if locked (4 bit)
R2000 Only:
	Values	BS	 70,290,370	contains frozen (1 bit), on (2 bit), frozen by default in new viewports (4 bit), locked (8 bit), plotting flag (16 bit), and lineweight (mask with 0x03E0)
Common:	
	Color	BS	62
	Handle refs	H		Layer control (CODE 4)
				[Reactors (CODE 4)]
				xdicobjhandle (CODE 3)
				NULL (CODE 5)
R2000 Only:
		H	390	Plotstyle (CODE 5)
Common:
			6	linetype (CODE 5)
	CRC	X	---
*/
CReturn
CDwgReader::ReadLayer( const CCommonEntityData& ced )
{
	CReturn status;

	CString	name;
	long	numReactors;
	long	nbits;
	int		handle;

	if (m_version == 13 || m_version ==14)
	{
		// Get the handle of this drawing object.  This handle
		// better match the handle that we have been given.
		handle = ReadHandleReference();

		// Just skip the "extended entity data"
		ReadExtendedEntityData();

		nbits = m_file.ReadRawLong();	// object size in bits

		numReactors = m_file.ReadBitLong();

		// Get the layer name.
		name = ReadString();

		m_layers.Append( new CDWGLayer( name, handle ) );
	}
	else if (m_version == 15)
	{
		nbits = m_file.ReadRawLong();	// object size in bits

		// Get the handle of this drawing object.  This handle
		// better match the handle that we have been given.
		handle = ReadHandleReference();

		// Just skip the "extended entity data"
		ReadExtendedEntityData();

		numReactors = m_file.ReadBitLong();

		// Get the layer name.
		name = ReadString();

		m_layers.Append( new CDWGLayer( name, handle ) );
	}

	return status;
}

/*
LINE (19)
	Common Entity Data
R13-R14 Only:
	Start pt	3BD	10
	End   pt	3BD	11
R2000 Only:
	Z's are zero bit	B	
	Start Point x	RD	10
	End Point x	DD	11	Use 10 value for default
	Start Point y	RD	20
	End Point y	DD	21	Use 20 value for default
	Start Point z	RD	30	Present only if "Z's are zero bit" is 0
	End Point z	DD	31	Present only if "Z's are zero bit" is 0, use 30 value for default.

Common:
	Thickness	BT	39
	Extrusion	BE	210
	Common Entity Handle Data
	CRC	X	---
*/
CReturn
CDwgReader::ReadLine( const CCommonEntityData& ced )
{
	CReturn status;

	BYTE	zZeroBit;
	double	xs, xe;
	double	ys, ye;
	double	zs, ze;
	double	thickness;
	double	vec[3];
	long	layerHandle;

	if (m_version == 13 || m_version == 14)
	{
		xs = m_file.ReadBitDouble();
		ys = m_file.ReadBitDouble();
		zs = m_file.ReadBitDouble();

		xe = m_file.ReadBitDouble();
		ye = m_file.ReadBitDouble();
		ze = m_file.ReadBitDouble();

		thickness = m_file.ReadBitThickess();

		m_file.ReadBitExtrusion( vec );
	}
	else if (m_version == 15)
	{
		zZeroBit = m_file.ReadBit();

		xs = m_file.ReadRawDouble();
		xe = m_file.ReadBitDoubleDef( xs );

		ys = m_file.ReadRawDouble();
		ye = m_file.ReadBitDoubleDef( ys );

		if ( zZeroBit )
		{
			zs = 0.;
			ze = 0.;
		}
		else
		{
			zs = m_file.ReadRawDouble();
			ze = m_file.ReadBitDoubleDef( zs );
		}

		thickness = m_file.ReadBitThickess();

		m_file.ReadBitExtrusion( vec );
	}

	layerHandle = ReadCommonEntityHandleData( ced );

	if ( IsAcceptedLayer( layerHandle ) )
	{
	}

	return status;
}

/*
ARC (17)
	Common Entity Data
	Center	3BD	10
	Radius	BD	40
	Thickness	BT	39
	Extrusion	BE	210
	Start angle	BD	50
	End   angle	BD	51
	Common Entity Handle Data
	CRC	X	---
*/
CReturn
CDwgReader::ReadArc( const CCommonEntityData& ced )
{
	CReturn status;

	double	xc, yc, zc;
	double	thickness;
	double	radius;
	double	as, ae;
	double	vec[3];
	long	layerHandle;

	if (m_version == 13 || m_version == 14 || m_version == 15)
	{
		xc = m_file.ReadBitDouble();
		yc = m_file.ReadBitDouble();
		zc = m_file.ReadBitDouble();

		radius = m_file.ReadBitDouble();

		thickness = m_file.ReadBitThickess();

		m_file.ReadBitExtrusion( vec );
	
		as = m_file.ReadBitDouble();
		ae = m_file.ReadBitDouble();

		layerHandle = ReadCommonEntityHandleData( ced );
	}

	return status;
}

/*
CIRCLE (18)
	Common Entity Data
	Center	3BD	10
	Radius	BD	40
	Thickness	BT	39
	Extrusion	BE	210
	Common Entity Handle Data
	CRC	X	---
*/
CReturn
CDwgReader::ReadCircle( const CCommonEntityData& ced )
{
	CReturn status;

	double	xc, yc, zc;
	double	thickness;
	double	radius;
	double	vec[3];
	long	layerHandle;

	if (m_version == 13 || m_version == 14 || m_version == 15)
	{
		xc = m_file.ReadBitDouble();
		yc = m_file.ReadBitDouble();
		zc = m_file.ReadBitDouble();

		radius = m_file.ReadBitDouble();

		thickness = m_file.ReadBitThickess();

		m_file.ReadBitExtrusion( vec );

		layerHandle = ReadCommonEntityHandleData( ced );
	}

	return status;
}

/*
2D POLYLINE (15)
	Common Entity Data
	Flags	BS	70
	Curve type	BS	 75	Curve and smooth surface type.
	Start width	BD	40	Default start width
	End width	BD	41	Default end width
	Thickness	BT	39
	Elevation	BD	10	The 10-pt is (0,0,elev)
	Extrusion	BE	210
	Common Entity Handle Data
		H		1st  VERTEX (CODE 4)
		H		last VERTEX (CODE 4)
		H		SEQEND (CODE 3)
	CRC	X	---
*/
CReturn
CDwgReader::ReadPoly2d( const CCommonEntityData& ced )
{
	CReturn status;

	short	flags;
	double	startWidth;
	double	endWidth;
	double	thickness;
	double	elevation;
	double	vec[3];
	double	xpt, ypt;
	double	type;
	int		handle;
	BYTE	count, indx;

	if (m_version == 13 || m_version == 14 || m_version == 15)
	{
#if ACAD_POLY2D
		flags = m_file.ReadBitShort();

		startWidth = m_file.ReadBitDouble();
		endWidth = m_file.ReadBitDouble();

		thickness = m_file.ReadBitThickess();
		elevation = m_file.ReadBitDouble();

		m_file.ReadBitExtrusion( vec );

		handle = ReadHandleReference();
		handle = ReadHandleReference();
		handle = ReadHandleReference();
#else
		flags = m_file.ReadBitShort();

		startWidth = m_file.ReadBitDouble();

		count = m_file.ReadBYTE();

		int count2 = 0;
		if (flags != 0x00 && flags != 0x200)
		{
			count2 = m_file.ReadBitShort();  // who knows?
		}

		// The starting point.
		xpt = m_file.ReadRawDouble();
		ypt = m_file.ReadRawDouble();

		// Subsequent points.
		if (m_version == 15)
		{
			for (indx = 1; indx < count; ++indx)
			{
				xpt = m_file.ReadBitDoubleDef( xpt );
				ypt = m_file.ReadBitDoubleDef( ypt );
			}
		}
		else
		{
			for (indx = 1; indx < count; ++indx)
			{
				xpt = m_file.ReadRawDouble();
				ypt = m_file.ReadRawDouble();
			}
		}

		for (indx=0; indx<count2; ++indx)
		{
			type = m_file.ReadBitDouble();	// (-1) cw / (0) pt / (+) ccw
		}
#endif
	}

	return status;
}

/*
3D POLYLINE (16)
	Common Entity Data
	Flags	RC	70	NOT DIRECTLY THE 75.  Bit-coded (76543210):
			75	0 : Splined (75 value is 5)
				1 : Splined (75 value is 6)
				(If either is set, set 70 bit 2 (4) to indicate splined.)
	Flags	RC	70	NOT DIRECTLY THE 70.  Bit-coded (76543210):
				0 : Closed (70 bit 0 (1))
				(Set 70 bit 3 (8) because this is a 3D POLYLINE.)
	Common Entity Handle Data
		H		first VERTEX (CODE 4)
		H		last  VERTEX (CODE 4)
		H		SEQEND (CODE 3)
	CRC	X	---
*/
CReturn
CDwgReader::ReadPoly3d( const CCommonEntityData& ced )
{
	CReturn status;

	char	flags;
	int		handle;
	long	layerHandle;
	long	firstVertexHandle;
	long	lastVertexHandle;
	long	seqEndHandle;

	if (m_version == 13 || m_version == 14 || m_version == 15)
	{
		flags = m_file.ReadRawChar();

		flags = m_file.ReadRawChar();

		layerHandle = ReadCommonEntityHandleData( ced );

		firstVertexHandle = ReadHandleReference();
		lastVertexHandle = ReadHandleReference();
		seqEndHandle = ReadHandleReference();
	}

	return status;
}

/*
VERTEX (3D) (11)
	Common Entity Data
	Flags	EC	70	NOT bit-pair-coded.
	Point	3BD	10
	Common Entity Handle Data
	CRC	X	---
*/
CReturn
CDwgReader::ReadVertex3d( const CCommonEntityData& ced )
{
	CReturn status;

	BYTE	flags;
	double	xpt, ypt, zpt;
	long	layerHandle;

	if (m_version == 13 || m_version == 14 || m_version == 15)
	{
		flags = m_file.ReadBYTE();

		xpt = m_file.ReadBitDouble();
		ypt = m_file.ReadBitDouble();
		zpt = m_file.ReadBitDouble();

		layerHandle = ReadCommonEntityHandleData( ced );
	}

	return status;
}

int
CDwgReader::ReadHandleReference()
{
	int hrCode	= 0;	// handle reference code
	int hrCount	= 0;	// handle reference byte count
	int handle	= 0;	// handle of this drawing object
						// should match handle encountered in object table

	m_file.ReadBits( (BYTE*) &hrCode, 4 );
	m_file.ReadBits( (BYTE*) &hrCount, 4 );
	m_file.ReadBytes( (BYTE*) &handle, hrCount );

	return handle;
}

/*
13) EXTENDED ENTITY DATA 
      (EXTENDED OBJECT DATA)
EED directly follows the entity handle.

Each application's data is structured as follows:

    |Length|Application handle|Data items|

Length is a bitshort indicating the length of the data for an app, not including itself,
the bit-pair, or the app table handle. The above format repeats until a length of zero is found.

The application handle is a standard table handle reference:

	|0101|4-bit length|handle bytes|

Each data item has a 1-byte code (DXF group code minus 1000) followed by the value.
It looks like there's no bit-pair coding within the data; that would throw off the length
value (it would need to count bits, too).  The form of the value is listed below for each type:
    0 (1000)   String.  1st byte of value is the length; this is followed by a 2-byte short
				indicating the codepage.
 
    1 (1001)   This one seems to be invalid; can't even use as a string inside braces.
			   This would be a registered application that this data relates to, but we've
			   already had that above, so it would be redundant or irrelevant here.

    2 (1002)   A '{' or '}'; 1 byte; ASCII 0 means '{', ASCII 1 means '}'

    3 (1003)   A layer table reference.  The value is the handle of the layer; it's
			   8 bytes -- even if the leading ones are 0.  It's not a string; read it
			   as hex, as usual for handles.  (There's no length specifier this time.)
			   Even layer 0 is referred to by handle here.

    4 (1004)   Binary chunk.  The first byte of the value is a char giving the length;
			   the bytes follow.

    5 (1005)   An entity handle reference.  The value is given as 8 bytes -- even if
			   the leading ones are 0. It's not a string; read it as hex, as usual for
			   handles.  (There's no length specifier this time.)

    10 - 13 (1010 - 1013)
               Points; 24 bytes (XYZ) -- 3 doubles

    40 - 42 (1040 - 1042)
               Reals; 8 bytes (double)

    70 (1070)  A short int; 2 bytes
    71 (1071)  A long  int; 4 bytes
*/
void
CDwgReader::ReadExtendedEntityData()
{
	short eedSize;
	short length;
	int handle;

	// Extended entity data size.
	eedSize = m_file.ReadBitShort();
	if (eedSize > 0)
	{
		length = m_file.ReadBitShort();
		while (length > 0)
		{
			handle = ReadHandleReference();
			length = m_file.ReadBitShort();
		}
	}
}

void
CDwgReader::ReadGraphicsData()
{
	long nbytes;
	long nbits;

	if ( m_file.ReadBit() )
	{
		nbytes = m_file.ReadRawLong();
		nbits = m_file.ReadRawLong();
		m_file.ReadBits( g_fodder, nbits );
	}
}

/*
Common Entity Handle Data
The following data appears in the handles section of each entity,
and will be referred to as Common Entity Handle Data in the subsequent entity descriptions.
	Handle refs	H		[Subentity ref handle (CODE 3)]
						[Reactors (CODE 4)]
						xdicobjhandle  (CODE 3)
R13-R14 Only:
						8	LAYER (CODE 5)
						6	[LTYPE (CODE 5)] (present if Isbylayerlt is 0)
Common:
						[PREVIOUS ENTITY (CODE 4)]
						[NEXT ENTITY (CODE 4)]
R2000 Only:
						8 LAYER (CODE 5)
						6 [LTYPE (CODE 5)] present if linetype flags 
						were 11
						PLOTSTYLE (CODE 5) present if plotstyle flags 
						were 11

Additional notes:
The R13-R14 FLAGS area (6 bits) indicates which handle references are present in the HANDLE REFS area.  They are as follows:
FEDCBA
	FE	:	Entity mode (entmode).  Generally, this indicates whether or not the subentity relative handle reference is present.  The values go as follows:
			00 : The subentity relative handle reference is present.
                    Applies to the following:
                       VERTEX, ATTRIB, and SEQEND.
                       BLOCK, ENDBLK, and the defining entities in all
                       block defs except *MODEL_SPACE and *PAPER_SPACE.
			01 : PSPACE entity without a relative handle ref.
			10 : MSPACE entity without a relative handle ref.
			11 : Not used.

	DC	:	This is the number of reactors attached to an entity as a bitshort. This feature may have been dormant in R13, but it appears in R14, and in files saved as R13 by R14.
	B	:	0 if a linetype reference is present; 1 if it's not (the default being BYLAYER -- even though there IS a BYLAYER linetype entity and it has a handle).
	A	:	0 if the previous and next linkers are present; 1 if they are BOTH defaults (1 back and 1 forward).

*/
long
CDwgReader::ReadCommonEntityHandleData( const CCommonEntityData& ced )
{
	long	xdicobj_handle;
	long	subent_handle;
	long	reactor_handle;
	long	layerLT;
	long	prev_handle;
	long	next_handle;
	long	ltype_handle;
	long	ptype_handle;
	long	layer_handle;
	BYTE	code;

	layer_handle = 0;

	if (ced.EntMode() == 0)
	{ subent_handle = m_file.ReadHandle(&code); }	// Code => 3

	if (ced.NumReact() > 0)
	{ reactor_handle = m_file.ReadHandle(&code); }	// Code => 4

	xdicobj_handle = m_file.ReadHandle(&code);		// Code => 3

	if (m_version == 13 || m_version == 14)
	{
		layer_handle = m_file.ReadHandle(&code);	// Code => 5

		if ( !ced.IsByLayerLT() )
		{  layerLT = m_file.ReadHandle(&code); }	// Code => 5
	}

	prev_handle = m_file.ReadHandle(&code);	// Code => 4
	next_handle = m_file.ReadHandle(&code);	// Code => 4

	if (m_version == 15)
	{
		layer_handle = m_file.ReadHandle(&code);	// Code => 5

		if (ced.LineTypeFlags() == 3)
		{ ltype_handle = m_file.ReadHandle(&code); } // Code => 5

		if (ced.PlotTypeFlags() == 3)
		{ ptype_handle = m_file.ReadHandle(&code); } // Code => 5
	}

	return layer_handle;
}

CString
CDwgReader::ReadString()
{
	int length;

	length = m_file.ReadBitShort();
	m_file.ReadBytes( g_fodder, length );
	g_fodder[length] = '\0';

	return CString( g_fodder );
}

// NOTE: The switch statement should match that in CDwgReader::ObjectRead()
BOOL
CDwgReader::MustProcess( short objType )
{
	char* type = ObjectString( objType );  // for debugging

	if ( m_layersOnly )
	{
		return (objType == DWG_LAYER);
	}
	else
	{
		switch( objType )
		{
		// case DWG_LAYER_CTRL_OBJ:
		// case DWG_LAYER:
		case DWG_BLOCK_HEADER:
		case DWG_ENDBLK:
		case DWG_BLOCK:
		case DWG_VERTEX_2D:
		case DWG_VERTEX_3D:
		case DWG_POLYLINE_2D:
		case DWG_POLYLINE_3D:
		case DWG_ARC:
		case DWG_CIRCLE:
		case DWG_LINE:
		case DWG_POINT:
		case DWG_POLYGON:
			return TRUE;
		default:
			return FALSE;
		}
	}
}

char*
CDwgReader::ObjectString( short objType )
{
	switch( objType )
	{
	case DWG_UNUSED:				return "DWG_UNUSED";
	case DWG_TEXT:					return "DWG_TEXT";
	case DWG_ATTRIB:				return "DWG_ATTRIB";
	case DWG_ATTDEF:				return "DWG_ATTDEF";
	case DWG_BLOCK:					return "DWG_BLOCK";
	case DWG_ENDBLK:				return "DWG_ENDBLK";
	case DWG_SEQEND:				return "DWG_SEQEND";
	case DWG_INSERT:				return "DWG_INSERT";
	case DWG_MINSERT:				return "DWG_MINSERT";
	case DWG_0x09:					return "DWG_0x09";
	case DWG_VERTEX_2D:				return "DWG_VERTEX_2D";
	case DWG_VERTEX_3D:				return "DWG_VERTEX_3D";
	case DWG_VERTEX_MESH:			return "DWG_VERTEX_MESH";
	case DWG_VERTEX_PFACE:			return "DWG_VERTEX_PFACE";
	case DWG_VERTEX_FACE:			return "DWG_VERTEX_FACE";
	case DWG_POLYLINE_2D:			return "DWG_POLYLINE_2D";
	case DWG_POLYLINE_3D:			return "DWG_POLYLINE_3D";
	case DWG_ARC:					return "DWG_ARC";
	case DWG_CIRCLE:				return "DWG_CIRCLE";
	case DWG_LINE:					return "DWG_LINE";
	case DWG_DIM_ORDINATE:			return "DWG_DIM_ORDINATE";
	case DWG_DIM_LINEAR:			return "DWG_DIM_LINEAR";
	case DWG_DIM_ALIGNED:			return "DWG_DIM_ALIGNED";
	case DWG_DIM_ANG_3PT:			return "DWG_DIM_ANG_3PT";
	case DWG_DIM_ANG_2LN:			return "DWG_DIM_ANG_2LN";
	case DWG_DIM_RADIUS:			return "DWG_DIM_RADIUS";
	case DWG_DIM_DIAMETER:			return "DWG_DIM_DIAMETER";
	case DWG_POINT:					return "DWG_POINT";
	case DWG_3DFACE:				return "DWG_3DFACE";
	case DWG_POLYLINE_PFACE:		return "DWG_POLYLINE_PFACE";
	case DWG_POLYLINE_MESH:			return "DWG_POLYLINE_MESH";
	case DWG_SOLID:					return "DWG_SOLID";
	case DWG_TRACE:					return "DWG_TRACE";
	case DWG_SHAPE:					return "DWG_SHAPE";
	case DWG_VIEWPORT: 				return "DWG_VIEWPORT";
	case DWG_ELLIPSE:				return "DWG_ELLIPSE";
	case DWG_SPLINE: 				return "DWG_SPLINE";
	case DWG_REGION:				return "DWG_REGION";
	case DWG_3DSOLID:				return "DWG_3DSOLID";
	case DWG_BODY:					return "DWG_BODY";
	case DWG_RAY:					return "DWG_RAY";
	case DWG_XLINE:					return "DWG_XLINE";
	case DWG_DICTIONARY:			return "DWG_DICTIONARY";
	case DWG_0x2B:					return "DWG_0x2B";
	case DWG_MTEXT:					return "DWG_MTEXT";
	case DWG_LEADER:				return "DWG_LEADER";
	case DWG_TOLERANCE:				return "DWG_TOLERANCE";
	case DWG_MLINE:					return "DWG_MLINE";
	case DWG_BLOCK_CTRL_OBJ:		return "DWG_BLOCK_CTRL_OBJ";
	case DWG_BLOCK_HEADER:			return "DWG_BLOCK_HEADER";
	case DWG_LAYER_CTRL_OBJ:		return "DWG_LAYER_CTRL_OBJ";
	case DWG_LAYER:					return "DWG_LAYER";
	case DWG_STYLE_CTRL_OBJ:		return "DWG_STYLE_CTRL_OBJ";
	case DWG_STYLE:					return "DWG_STYLE";
	case DWG_0x36:					return "DWG_0x36";
	case DWG_0x37:					return "DWG_0x37";
	case DWG_LTYPE_CTRL_OBJ:		return "DWG_LTYPE_CTRL_OBJ";
	case DWG_LTYPE:					return "DWG_LTYPE";
	case DWG_0x3A:					return "DWG_0x3A";
	case DWG_0x3B:					return "DWG_0x3B";
	case DWG_VIEW_CTRL_OBJ:			return "DWG_VIEW_CTRL_OBJ";
	case DWG_VIEW:					return "DWG_VIEW";
	case DWG_UCS_CTRL_OBJ:			return "DWG_UCS_CTRL_OBJ";
	case DWG_UCS:					return "DWG_UCS";
	case DWG_VPORT_CTRL_OBJ:		return "DWG_VPORT_CTRL_OBJ";
	case DWG_VPORT:					return "DWG_VPORT";
	case DWG_APPID_CTRL_OBJ:		return "DWG_APPID_CTRL_OBJ";
	case DWG_APPID:					return "DWG_APPID";
	case DWG_DIMSTYLE_CTRL_OBJ:		return "DWG_DIMSTYLE_CTRL_OBJ";
	case DWG_DIMSTYLE:				return "DWG_DIMSTYLE";
	case DWG_VP_ENT_HDR_CTRL_OBJ:	return "DWG_VP_ENT_HDR_CTRL_OBJ";
	case DWG_VP_ENT_HDR:			return "DWG_VP_ENT_HDR";
	case DWG_GROUP:					return "DWG_GROUP";
	case DWG_MLINESTYLE:			return "DWG_MLINESTYLE";
	case DWG_POLYGON:				return "DWG_POLYGON(?)";
	default:						return "UNKNOWN";
	}
}

short
CDwgReader::ObjectTypeConvert( short objType )
{
	if (objType >= 500)
	{
		short indx = objType - 500;
		if (indx < m_classes.Count())
			objType = m_classes[indx]->Type();
	}

	return objType;
}

void
CDwgReader::LayersCull( const CDynamicArray<CString*>& acceptedLayers )
{
	CString	name;
	bool	found;
	int		indx, jndx;

	if (acceptedLayers.Count() == 1)
	{
		if (acceptedLayers[0]->CompareNoCase("*default*") == 0)
			return;  // all layers are acceptable
	}

	indx = 0;
	while (indx < m_layers.Count())
	{
		name = m_layers[indx]->Name();

		found = acceptedLayers.BinarySearch( (void*) &name, &NamesCompare, &jndx );

		if ( found )
		{
			++indx;
		}
		else
		{
			delete m_layers.Remove( indx );
		}
	}
}

bool
CDwgReader::IsAcceptedLayer( long layerHandle )
{
	int indx;
	return ( m_layers.BinarySearch( (void*) &layerHandle, &HandlesCompare, &indx ) );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int OffsetsCompare( const void* ptrA, const void* ptrB )
{
	CObjectLocator* objA = (*(CObjectLocator**) ptrA);
	CObjectLocator* objB = (*(CObjectLocator**) ptrB);

	int diff = objA->Offset() - objB->Offset();

	return diff;
}

int HandlesCompare( void* ptrA, void* ptrB )
{
	long handleA = (*((long*) ptrA));
	CDWGLayer* objB = ((CDWGLayer*) ptrB);

	int diff = (int) (objB->Handle() - handleA);

	return diff;
}

int NamesCompare( void* ptrA, void* ptrB )
{
	CString* stringA = ((CString*) ptrA);
	CString* stringB = ((CString*) ptrB);

	int diff = stringB->CompareNoCase( (*stringA) );

	return diff;
}

void
CDwgReader::DumpObjectTable( const CString& filename )
{
	CObjectLocator* rec;
	FILE* f;
	int count, indx;

	f = fopen( file( filename ), "w" );
	if (f != NULL)
	{
		m_objects.Qsort( &OffsetsCompare );

		fprintf( f, "indx handle   offset type\n" );
		fprintf( f, "-------------------------------------------\n" );
		count = m_objects.Count();
		for (indx = 0; indx < count; ++indx)
		{
			rec = m_objects[indx];
			fprintf( f, "%4d  %5d  %7d %s\n",
				indx, rec->Handle(), rec->Offset(), rec->Type() );
		}
		fclose( f );
	}
}

void
dumpDoubles( double* val, int count )
{
	FILE* f;
	char* ptr;
	int indx, jndx;

	f = fopen( file("_vals.txt"), "w" );
	if (f != NULL)
	{
		for (indx = 0; indx < count; ++indx)
		{
			ptr = (char*) &(val[indx]);

			fprintf( f, "\n\n" );
			fprintf( f, "val: %-10.6f\n\n", val[indx] );

			for (jndx = 0; jndx < sizeof( double ); ++jndx)
			{
				fprintf( f, "%d", (((*ptr) & 0x80) ? 1 : 0) );
				fprintf( f, "%d", (((*ptr) & 0x40) ? 1 : 0) );
				fprintf( f, "%d", (((*ptr) & 0x20) ? 1 : 0) );
				fprintf( f, "%d", (((*ptr) & 0x10) ? 1 : 0) );
				fprintf( f, " " );
				fprintf( f, "%d", (((*ptr) & 0x08) ? 1 : 0) );
				fprintf( f, "%d", (((*ptr) & 0x04) ? 1 : 0) );
				fprintf( f, "%d", (((*ptr) & 0x02) ? 1 : 0) );
				fprintf( f, "%d", (((*ptr) & 0x01) ? 1 : 0) );
				fprintf( f, " " );

				++ptr;
			}
		}
		fflush( f );
	}
	fclose( f );
}

CString file( const CString& name )
{
	return (ROOTDIR + name);
}
