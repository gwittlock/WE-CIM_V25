//========================================================================
//		SmartCAM PM4 File Interface
//
//		Read Methods
//
// ============================================================================

#include "stdafx.h"
#include "cmn_resource.h"

#include "mypm4.h"
#include "mypm4_def.h"
#include "token.h"

#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbLine.h"
#include "DbPoint.h"
#include "DbArc.h"
#include "DbHole.h"
#include "DbCommand.h"
#include "DbIterator.h"

void Dump( BYTE* pool, int size );

// ============================================================================

#define LOCAL_EOF_TAG		0xff

CDbEntity* g_lastEntity = NULL;

// ============================================================================

int
CMyPm4::do_read_codesize( BYTE code )
{
	bool	okay;
	UINT	size = 0;
	UINT	got_size;
	short	count;

	switch (code)
	{
		case PM4_MATRIX_TAG:
			size = PM4_MATRIX_SIZE;
			break;
		case PM4_VIEW_TAG:
			size = PM4_VIEW_SIZE*16;
			break;
		case PM4_SUBDEF_TAG:
			size = PM4_SUBDEF_SIZE;
			break;

		case PM4_MAP_TAG:
			got_size = m_file.Read( (LPVOID)&count, 2 );
			okay = (got_size == 2);
			size = count * 2;
			break;

		case PM4_NAME_TAG:
			got_size = m_file.Read( (LPVOID)&count, 2 );
			okay = (got_size == 2);
			size = count*9;
			break;

		default:
			got_size = m_file.Read( (LPVOID)&size, 4 );
			okay = (got_size == 4);
			break;
	}

	if (okay)
		return size;
	
	return 0;
}

// ============================================================================

CReturn
CMyPm4::do_read_refplane( 
	CModel*		model )
{
	CReturn	ret;
	BYTE	code;
	BYTE*	pool;
	UINT	size;
	UINT	got_size;
	bool	okay;

	m_refname.RemoveAll();

	// -------------------------------------------
	// Skip the klunky header...
	// ... remembering a few pieces as needed.
	//
try
{
	m_file.SeekToBegin();

	// Header
	{
		BYTE* header = new BYTE[ PM4_HEADER_LEN ];
		got_size = m_file.Read( (LPVOID)header, PM4_HEADER_LEN );
		if (got_size != PM4_HEADER_LEN)
		{
			delete[] header;

			ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_refplane - header" );
			return ret;
		}

		CString text = (char*)&header[PM4_OFF_VERSION];
		if (text.Left(6).CompareNoCase("pm4.00") )
		{
			delete[] header;

			ret.Internal( IDS_PM4_BAD_VERSION, text.Left(6) );
			return ret;
		}
		delete[] header;
	}

	// -------------------------------------------
	// Now, zip through a bunch of crap to get the refplanes
	//
	okay = TRUE;
	while (okay)
	{
		// --------------------------------------------
		// Basics... read the coding and get the size
		code = 0;
		got_size = m_file.Read( (LPVOID)&code, 1 );
		okay = (got_size == 1);

		// Well-defined exit conditions!
		if (code == LOCAL_EOF_TAG)
			return ret;

		if (okay)
			size = do_read_codesize( code );
		else
			size = 0;

		if (!okay)
			ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_refplane() - sizes" );

		// --------------------------------------------
		// Big pool...
		//
		pool = NULL;
		if ( okay
			&& (size>0) )
		{
			pool = new BYTE[size];

			if (pool)
			{
				got_size = m_file.Read( (LPVOID)pool, size );
				okay = (got_size == size);
			}
		}   
		else
			pool= NULL;

		if (!pool)
		{
			ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_refplane - pool" );
			okay = FALSE;
		}

		// --------------------------------------------
		// Interpret!
		//
		if (okay)
		{
			switch (code)
			{
				case PM4_MATRIX_TAG:
					ret += read_refplane( model, pool );
					break;

				default:
					break;
			}
			okay = ret.isOkay();
		}
		else
		{
			// Died a natural death, end of file.
			okay = TRUE;
			break;
		}

		if (!okay)
		{
			ret.Internal( IDS_INTERNAL_ERROR, "CPM4::do_read_refplane - element" );
		}
		if (pool) delete[] pool; pool = NULL;
	}
}

	catch( CFileException* e )
	{
		ret.SystemError();

		m_file.Close();
		e->Delete();
	}


	return ret;
}

// ============================================================================

CReturn
CMyPm4::do_read_geometry( 
	CModel*		model )
{
	CReturn	ret;
	BYTE	code;
	BYTE*	pool;
	UINT	size;
	UINT	got_size;
	bool	okay;

	// -------------------------------------------
	// Skip the klunky header...
	// ... remembering a few pieces as needed.
	//
	m_orderid = 1;

try
{
	m_file.SeekToBegin();

	{
		BYTE* header = new BYTE[ PM4_HEADER_LEN ];
		got_size = m_file.Read( (LPVOID)header, PM4_HEADER_LEN );
		if (got_size != PM4_HEADER_LEN)
		{
			delete[] header;

			ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_geometry - header" );
			return ret;
		}

		CString text = (char*)&header[PM4_OFF_VERSION];
		if (text.Left(6).CompareNoCase("pm4.00") )
		{
			delete[] header;

			ret.Internal( IDS_PM4_BAD_VERSION, text.Left(6) );
			return ret;
		}
//		m_varset->setVar( "Version", text );

#ifdef USE_JOBPLAN
		text = (char*)&header[PM4_OFF_JOBPLAN];
		if (text.GetLength() == 15)
		{

			// If jplan is exactly 15 chars, assume long filename,
			// which will fail, so use part's name instead.
			if (slash < 0)
				text = m_filename;
			else
			{
				CString	temp;
				int		dot;

				temp = m_filename.Right( m_filename.GetLength() - (slash+1) );
				dot = temp.ReverseFind( '.' );
				if (dot > 0)
					text = temp.Left( dot );
				else
					text = temp;
				text += ".jof";
			}
		}
		text = path+text;
		m_varset->setVar( "Setup", text );
#endif
		
		delete[] header;
	}

	// -------------------------------------------
	// Now, read a buncha chunks...
	//
#ifdef PM4_SUBS
	in_sub = FALSE;
	sub_cnt = -1;
#endif

	okay = TRUE;
	while (okay)
	{
		// --------------------------------------------
		// Basics... read the coding and get the size
		code = 0;
		got_size = m_file.Read( (LPVOID)&code, 1 );
		okay = (got_size == 1);

		// Well-defined exit conditions!
		if (code == LOCAL_EOF_TAG)
		{
#ifdef PM4_SUBS
			join_blocks();
#endif
			return ret;
		}

		if (okay)
			size = do_read_codesize( code );
		else
			size = 0;

		if (!okay)
			ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_geometry() - sizes" );

		// --------------------------------------------
		// Big pool...
		//
		pool = NULL;
		if ( okay
			&& (size>0) )
		{
			pool = new BYTE[size];

			if (pool)
			{
				got_size = m_file.Read( (LPVOID)pool, size );
				okay = (got_size == size);
			}
		}   
		else
			pool= NULL;

		if (!pool)
		{
			ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_geometry - pool" );
			okay = FALSE;
		}

		// --------------------------------------------
		// Interpret!
		//
		if (okay)
		{
			switch (code)
			{
				case PM4_POINT_TAG:
					ret += read_point( model, pool );
					break;

				case PM4_LINE_TAG:
					ret += read_line( model, pool );
					break;

				case PM4_ARC_TAG:
					ret += read_arc( model, pool );
					break;

				case PM4_USER_TAG:
					ret += read_user( model, pool );
					break;

//            case 0x07: // Ellipse
//					elem = new CLineElement;
//					if (!elem)
//						okay = FALSE;
//					break;

//            case 0x09: // Helix
//					elem = new CLineElement;
//					if (!elem)
//						okay = FALSE;
//					break;

				case PM4_POLY_TAG:
					Dump( pool, size );
					ret += read_poly( model, pool );
					break;

//            case 0x0c: // Spline
//					elem = new CLineElement;
//					if (!elem)
//						okay = FALSE;
//					break;

#ifdef PM4_SUBS
				case PM4_SUBCALL_TAG:
					okay = readSubCall( pool );
					break;
#endif

//				case PM4_TEXT_TAG:
//					okay = readText( pool );
//					break;

//				case PM4_MAP_TAG:
//					elem = new CLineElement;
//					if (!elem)
//						okay = FALSE;
//					break;

//            case PM4_NAME_TAG: // Name
//					if (m_names) delete[] m_names;
//					if (size)
//						m_names = new char[size];
//					else
//						m_names = NULL;
//					memcpy( m_names, pool, size );
//					break;

//				case PM4_VIEW_TAG:
//					break;

				default:
					break;
			}
			okay = ret.isOkay();
		}
		else
		{
			// Died a natural death, end of file.
			okay = TRUE;
			break;
		}

		if (!okay)
		{
			ret.Internal( IDS_INTERNAL_ERROR, "CPM4::do_read_geometry - element" );
		}
#ifdef PM4_SUBS
		else // okay
		{
			// Special behavior if we have a subdef element...
			// ... find how many elements it wants.
			if (code == PM4_SUBDEF_TAG)
				sub_cnt = readSubDef( pool );

			if (okay)
			{
				// Special behavior for subdefs...
				// ... add to the subdef list, which also switches
				// internal flags affecting where subsequent elements go.
				if (code == PM4_SUBDEF_TAG)
				{
					CBlock*	new_block;
					int		sub_idx;

					new_block = new CBlock;
					new_block->setName( m_varset->getValue( "Name" ) );
					sub_idx = m_part->addBlock( new_block );
					m_part->openBlock( sub_idx );
					in_sub = TRUE;
				}
				else
				{
					if (in_sub)
					{
						sub_cnt--;
						if (!sub_cnt)
						{
							m_part->closeBlock();
							in_sub = FALSE;
						}
					}
				}
			}
		  }
#endif

		if (pool) delete[] pool; pool = NULL;
	}
}

	catch( CFileException* e )
	{
		ret.SystemError();

		m_file.Close();
		e->Delete();
	}


	return ret;
}


// ============================================================================

CReturn
CMyPm4::do_read_layer( void )
{
	CReturn	ret;
	BYTE	code;
	BYTE*	pool;
	UINT	size;
	UINT	got_size;
	bool	okay;

	// -------------------------------------------
	// Skip the klunky header...
	//
	try
	{
		m_file.SeekToBegin();

		{
			BYTE* header = new BYTE[ PM4_HEADER_LEN ];
			got_size = m_file.Read( (LPVOID)header, PM4_HEADER_LEN );
			if (got_size != PM4_HEADER_LEN)
			{
				delete[] header;

				ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_layer - header" );
				return ret;
			}

			CString text = (char*)&header[PM4_OFF_VERSION];
			if (text.Left(6).CompareNoCase("pm4.00") )
			{
				delete[] header;

				ret.Internal( IDS_PM4_BAD_VERSION, text.Left(6) );
				return ret;
			}
			
			delete[] header;
		}

		// -------------------------------------------
		// Now, read a buncha chunks... just sucking layers
		//
		okay = TRUE;
		while (okay)
		{
			// --------------------------------------------
			// Basics... read the coding and get the size
			code = 0;
			got_size = m_file.Read( (LPVOID)&code, 1 );
			okay = (got_size == 1);

			// Well-defined exit conditions!
			if (code == LOCAL_EOF_TAG)
			{
				return ret;
			}

			if (okay)
				size = do_read_codesize( code );
			else
				size = 0;

			if (!okay)
				ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_layer() - sizes" );

			// --------------------------------------------
			// Big pool...
			//
			pool = NULL;
			if ( okay
				&& (size>0) )
			{
				pool = new BYTE[size];

				if (pool)
				{
					got_size = m_file.Read( (LPVOID)pool, size );
					okay = (got_size == size);
				}
			}   
			else
				pool= NULL;

			if (!pool)
			{
				ret.Internal( IDS_INTERNAL_ERROR, "CPM4:do_read_layer - pool" );
				okay = FALSE;
			}

			// --------------------------------------------
			// Interpret!
			//
			if (okay)
			{
				switch (code)
				{
					case PM4_POINT_TAG:
					case PM4_LINE_TAG:
					case PM4_ARC_TAG:
						{
							CString layername = read_layername( pool );

							int idx, num = m_importUtil.LayerCount();
							for (idx=0; idx<num; idx++)
							{
								if (!layername.CompareNoCase( m_importUtil.LayerName(idx) ) )
									break;
							}

							if (idx >= num)
								m_importUtil.LayerAdd( layername );
						}
						break;


					default:
						break;
				}
				okay = ret.isOkay();
			}
			else
			{
				// Died a natural death, end of file.
				okay = TRUE;
				break;
			}

			if (!okay)
			{
				ret.Internal( IDS_INTERNAL_ERROR, "CPM4::do_read_layer - element" );
			}
			if (pool) delete[] pool; pool = NULL;
		}
	}
	catch( CFileException* e )
	{
		ret.SystemError();

		m_file.Close();
		e->Delete();
	}


	return ret;
}


// ============================================================================
//		read_layername
//
CString	
CMyPm4::read_layername( 
	BYTE* io_pool )
{
	CString	name;

	WORD group = io_pool[COM_OFF_GROUP];
	short val = get2int( &io_pool[COM_OFF_TOOL] );
	if ( (val & PM4_TOOL_GROUP)
		|| (group & PM4_TOOL_GROUP) )
	{
		val = val & (short)(~PM4_TOOL_GROUP);
		name.Format( "step_%d", val );
	}
	else
	{
		// name.Format( "Layer_%d", val );
		name = ((val == 99) ? "Layer_99" : "Reference");
	}

	return name;
}


// ============================================================================
//		read_common
//
CReturn
CMyPm4::read_common(
	CModel*			model, 
	BYTE*			io_pool,
	const CString&	in_tool,
	CDbEntity*		db_ent )
{
	CReturn			ret;
	short			val;
	CString			name;
	CDbTool*		tool = NULL;
	CDbWorkplane*	plane = NULL;
	int count;

	// Find the referenced workplane
	val = get2int( &io_pool[COM_OFF_PLANE] );

	// Hacks to prevent crashing when reading a Building Blocks generated file :-(
	count = m_refname.GetSize();
	if (count < 1)
	{
		XY_PlaneCreate();
		++count;
	}

	if ((val < 0) || (val >= count))
		val = 0;  // The XY_PLANE

	name = m_refname[val];
	ret += model->EntityFind( name, (CDbEntity**)&plane, DBWORKPLANE, DBWORKPLANE );
	db_ent->Workplane( plane );

	// Offset
	val = -get2int( &io_pool[COM_OFF_OFFSET] );
	// db_ent->IntSet( "offset", val );
	db_ent->IntSet( "cutside", val );

	tool = GetTool( in_tool );
	db_ent->Tool( tool );

	// Some attributes I just don't want to lose
	db_ent->DoubleSet( "check", CheckGet( io_pool ) );
	db_ent->DoubleSet( "clear", ClearGet( io_pool ) );

	db_ent->IntSet( "orderid", m_orderid++ );

	// Color?
	// TODO: m_importUtil.Color( db_ent );

	return ret;
}

// ============================================================================
//		read_point
//
CReturn
CMyPm4::read_point(
	CModel*	model, 
	BYTE*	io_pool )
{
	CReturn		ret;
	C3dCoord	pt;
	double		check;
	double		clear;
	CDbPoint*	dbPoint = NULL;
	CDbHole*	dbHole = NULL;

	CString layername = read_layername( io_pool );
	// if (!m_importUtil.IsLayerOk( layername ))
	// 	return ret;

	if (layername.Find("step_") == 0)
	{
		clear = ClearGet( io_pool );
		if (clear < UNDEFINED)			// otherwise, it is a rapid pt (?)
		{
			model->EntityCreate( DBHOLE, (CDbEntity**) &dbHole );

			ret += read_common( model, io_pool, layername, dbHole );

			check = dbHole->DoubleGet( "check", 0. );
			// dbHole->DoubleSet( "depth", check );
			dbHole->DoubleSet( "depth", 0. );

			clear = dbHole->DoubleGet( "clear", 0. );  // the orientation angle

			pt = C3dCoord(	get8double( &io_pool[POINT_OFF_X] ),
							get8double( &io_pool[POINT_OFF_Y] ),
							get8double( &io_pool[POINT_OFF_Z] ) );

			dbHole->Init( dbHole->Tool(), dbHole->Workplane(), pt, 0.1, 0.1 );
			dbHole->SystemFlag( false );
		}
	}
	else
	{
		model->EntityCreate( DBPOINT, (CDbEntity**) &dbPoint );

		ret += read_common( model, io_pool, layername, dbPoint );

		check = dbPoint->DoubleGet( "check", 0. );
		dbPoint->DoubleSet( "depth", check );

		pt = C3dCoord(	get8double( &io_pool[POINT_OFF_X] ),
						get8double( &io_pool[POINT_OFF_Y] ),
						get8double( &io_pool[POINT_OFF_Z] ) );

		dbPoint->Init( dbPoint->Tool(), dbPoint->Workplane(), pt );
		dbPoint->SystemFlag( false );

		if ( !m_didReadOrigin )
		{
			// there should be only 1 at this time!
			int count = model->Db().Count( DBPOINT );

			model->EntityDelete( dbPoint->Id() );
			m_didReadOrigin = true;
		}
	}

#ifdef CUT_THIS_OUT
	// --------------
	switch (m_app)
	{
		case PM4_MILL:
			if (elem->getCheckZ() < 1000000)
			{
				elem->setOp( HOLE_DRILL );

				elem->setDepth( elem->getCheckZ() );
				elem->setCheckZ( pnt.getZ() );
			}
			else
				elem->setOp( HOLE_NONE );
			break;

		case PM4_FAB:
			if (elem->getClearZ() > TWOPI)
				elem->setOp( HOLE_NONE );
			else
			{
				elem->setOp( HOLE_DRILL );

				elem->setAngle( elem->getClearZ() );
				elem->setCheckZ( pnt.getZ() );
			}
			break;
	}
#endif

	// elem->SystemFlag( false );

	return ret;
}

// ============================================================================
//		read_line
//
CReturn
CMyPm4::read_line(
	CModel*	model, 
	BYTE*	io_pool )
{
	CReturn			ret;
	CDbLine*	elem = NULL;

	CString layername = read_layername( io_pool );
	// if (!m_importUtil.IsLayerOk( layername ))
	// 	return ret;

	// Hack!  Ick ack ptui...
	if (layername.CompareNoCase( Stock() ) == 0)
		layername = "Stock";

	C3dCoord	st(	get8double( &io_pool[LINE_OFF_SX] ),
					get8double( &io_pool[LINE_OFF_SY] ),
					get8double( &io_pool[LINE_OFF_SZ] ) );

	C3dCoord	en(	get8double( &io_pool[LINE_OFF_EX] ),
					get8double( &io_pool[LINE_OFF_EY] ),
					get8double( &io_pool[LINE_OFF_EZ] ) );

	if ( !st.WithinTol( en, SMALL ) )
	{
		model->EntityCreate( DBLINE, (CDbEntity**)&elem );
		ret += read_common( model, io_pool, layername, elem );

		elem->Init( elem->Tool(), elem->Workplane(), st, en );

		g_lastEntity = elem;  // in case we need to attach a user command
	}

	return ret;
}

// ============================================================================
//		readArc
//
//		Maps to ArcElement
//
CReturn
CMyPm4::read_arc(
	CModel*	model, 
	BYTE*	io_pool )
{
	CReturn			ret;
	CDbArc*	elem = NULL;

	CString layername = read_layername( io_pool );
	// if (!m_importUtil.IsLayerOk( layername ))
	// 	return ret;

	model->EntityCreate( DBARC, (CDbEntity**)&elem );
	ret += read_common( model, io_pool, layername, elem );

	C3dCoord	st(	get8double( &io_pool[ARC_OFF_SX] ),
					get8double( &io_pool[ARC_OFF_SY] ),
					get8double( &io_pool[ARC_OFF_SZ] ) );

	C3dCoord	en(	get8double( &io_pool[ARC_OFF_EX] ),
					get8double( &io_pool[ARC_OFF_EY] ),
					get8double( &io_pool[ARC_OFF_EZ] ) );

	C3dCoord	ct(	get8double( &io_pool[ARC_OFF_CX] ),
					get8double( &io_pool[ARC_OFF_CY] ),
					get8double( &io_pool[ARC_OFF_CZ] ) );

	short dir = get2int( &io_pool[ARC_OFF_DIR] );

	if ( !st.WithinTol( en, SMALL ) )
		ret += adjust_arc( st, en, &ct );

	elem->Init( elem->Tool(), elem->Workplane(), st, en, ct, dir );

	g_lastEntity = elem;  // in case we need to attach a user command

	return ret;
}


CReturn
CMyPm4::adjust_arc( 
	C3dCoord&	st, 
	C3dCoord&	en, 
	C3dCoord*	ct )
{
	CReturn ret;

	// Given an arc -- adjust the CENTER point to provide perfect arc
	//	alignment, using the ArcSER math out of SolveProcess.
	//
	C2dVec vecA = st - *ct;
	C2dVec vecB = en - *ct;

	double lenA = vecA.Length();
	double lenB = vecB.Length();

	double radius = (lenA + lenB) / 2.0; // What's the radius?  Who the hell knows; take 'em both

	// -----------------------------------------------------
	// Create intersection of two equal arcs to get
	//	potential centers...
	//
	double		chord_dist;
	double		chord_angle;
	double		angle;
	double		dx, dy;
	double		quotient;
	double		cx[2], cy[2];

	dx = en.X() - st.X();
	dy = en.Y() - st.Y();
	chord_dist = sqrt( dx*dx + dy*dy ) / 2.0;
	chord_angle = atan2( dy, dx );

	if ((chord_dist - radius) > SMALL)
	{
		ret.Internal( IDS_SOLVE_NO_SOLUTION );
		return ret;
	}

	// Textron reported arc failure.  For whatever reason,
	// acos( chord_dist / radius ) was returning an angle
	// of -1.#IND00 when the quotient was 1.0.
	quotient = chord_dist / radius;
	if (fabs(quotient) > (1. - VECTOR_SMALL))
	{
		angle = ((quotient > 0) ? 0. : PI);
	}
	else
	{
		angle = acos( quotient );
	}

	cx[0] = st.X() + cos( chord_angle + angle) * radius;
	cy[0] = st.Y() + sin( chord_angle + angle) * radius;

	cx[1] = st.X() + cos( chord_angle - angle) * radius;
	cy[1] = st.Y() + sin( chord_angle - angle) * radius;

	// I could have found the angle between the start and the current center to
	//	decide which solution to take, OR I could just pick the center that is
	//	closest to our current center... like this...

	vecA = *ct - C2dCoord(cx[0], cy[0]);
	vecB = *ct - C2dCoord(cx[1], cy[1]);

	lenA = vecA.Length();
	lenB = vecB.Length();

	ct->X( cx[lenB < lenA] );
	ct->Y( cy[lenB < lenA] );

	return ret;
}

// ============================================================================
//		readUser
//
//		Maps to CommandElement, except for certain cases where we interpret:
//
//				ELEMENT COMMENT <text>  -> CommentElement
//
//		Note that the User's Mode and Tool information is lost.
//
CReturn
CMyPm4::read_user(
	CModel*	model, 
	BYTE* io_pool )
{
	CReturn status;

//	CDbCommand*	dbCommand;
	CToken		token;
	C3dCoord	pnt;
	CString		layername;
	CString		text;
	CString		cmdText;

	text = getNascii( &io_pool[USER_OFF_TEXT], 66 );

	// -------------------------------------
	// Initialize parser... way cooool...
	//
	token.addDelimit( ' ' );
	token.addDelimit( '\t' );
	token.addDelimit( ',' );
	token.addDelimit( '=' );
	token.setBreak( TOKEN_BREAK_STRINGS );
	token.setLine( text );

	// -------------------------------------
	// Dumb, fixed parsing.  Make it smart when we care.
	//
	cmdText = token.getToken();
	if (!cmdText.CompareNoCase("ELEMENT"))
	{
		cmdText = token.getToken();
		if (!cmdText.CompareNoCase("COMMENT"))
		{
			// Simply ignore.
			return status;
		}
	}

	
	// -------------------------------------
	// Okay, nothing special, just dump across as a Command
	//
#if CREATE_COMMAND
	layername = read_layername( io_pool );

	model->EntityCreate( DBCOMMAND, (CDbEntity**) &dbCommand );
	status += read_common( model, io_pool, layername, dbCommand );

	pnt = C3dCoord(
				get8double( &io_pool[USER_OFF_X] ),
				get8double( &io_pool[USER_OFF_Y] ),
				get8double( &io_pool[USER_OFF_Z] ) );

	dbCommand->Init( dbCommand->Layer(), dbCommand->Workplane(), pnt, text );
#endif

	if (g_lastEntity != NULL)
	{
		text.MakeUpper();
		if (text.Find("@TRAPDOOR") == 0)
		{
			g_lastEntity->StringSet( "@DROPSTOP", "@DROP: x=0, y=0, dx=0, dy=0" );
		}
	}

	return status;
}

// ============================================================================
//		read_poly
//
CReturn
CMyPm4::read_poly(
	CModel*	model, 
	BYTE*	io_pool )
{
	CReturn		ret;
	C3dCoord	ps;
	C3dCoord	pe;
	CDbLine*	elem;
	int			count;
	int			indx;
	int			offset;
	int			incr;

	CString layername = read_layername( io_pool );

	// Hack!  Ick ack ptui...
	if (layername.CompareNoCase( Stock() ) == 0)
		layername = "Stock";

	count = get2int( &io_pool[PM4_POLY_PT_COUNT] );

	ps = C3dCoord(	get8double( &io_pool[LINE_OFF_SX] ),
					get8double( &io_pool[LINE_OFF_SY] ),
					get8double( &io_pool[LINE_OFF_SZ] ) );

	offset = PM4_POLY_PT_BASE;
	incr = 3 * sizeof(double);

	for (indx = 0; indx < count; ++indx)
	{
		pe = C3dCoord(	get8double( &io_pool[offset] ),
						get8double( &io_pool[offset+8] ),
						get8double( &io_pool[offset+16] ) );

		if ( !ps.WithinTol( pe, SMALL ) )
		{
			model->EntityCreate( DBLINE, (CDbEntity**)&elem );
			ret += read_common( model, io_pool, layername, elem );

			elem->Init( elem->Tool(), elem->Workplane(), ps, pe );

			ps = pe;
		}

		offset += incr;

		g_lastEntity = elem;  // in case we need to attach a user command
	}

	return ret;
}

#ifdef PM4_SUBS
// ============================================================================
//		readSubCall
//
//		Maps to BlockElement
//
bool
CMyPm4::readSubCall(
	BYTE* io_pool )
{
	CBlockElement*	elem;
	CPoint3			pnt;
	WORD				flag;

	elem = new CBlockElement;

	get24point( &io_pool[SUBCALL_OFF_START], &pnt );
	elem->setStart( &pnt );

	get24point( &io_pool[SUBCALL_OFF_END], &pnt);
	elem->setEnd( &pnt );

	get24point( &io_pool[SUBCALL_OFF_HANDLE], &pnt );
	elem->setAnchor( &pnt );

	get24point( &io_pool[SUBCALL_OFF_DELTA], &pnt );
	elem->setShift( pnt );

	elem->setStartTan( RAD_TO_DEG( get8double( &io_pool[SUBCALL_OFF_STTAN] ) ) );
	elem->setEndTan( RAD_TO_DEG( get8double( &io_pool[SUBCALL_OFF_ENTAN] ) ) );

	elem->setAngle( RAD_TO_DEG( get8double( &io_pool[SUBCALL_OFF_ANGLE] ) ) );
	elem->setSize( get8double( &io_pool[SUBCALL_OFF_SCALE] ) );

	// Copy quantity and type
	flag = get2int( &io_pool[SUBCALL_OFF_COPY] );
	elem->setCopy( flag & 0x3fff );
	switch (flag & 0xc000)
	{
		case 0x4000:
			elem->setCopyMode( COPY_SHIFT );
			break;
		case 0x8000:
			elem->setCopyMode( COPY_ROTATE );
			break;
		default:
			elem->setCopyMode( COPY_NONE );
			break;
	}

	// Various operating flags and modes
	flag = get2int( &io_pool[SUBCALL_OFF_FLAGS] );
	switch (flag & 0x6000)
	{
		case 0x4000:
			elem->setImageMode( IMAGE_X );
			break;
		case 0x2000:
			elem->setImageMode( IMAGE_Y );
			break;
		case 0x6000:
			elem->setImageMode( IMAGE_XY );
			break;
		default:
			elem->setImageMode( IMAGE_NONE );
			break;
	}

	switch (flag & 0x1800)
	{
		case 0x0000:
			elem->setCornerMode( CORNER_TOP_RIGHT );
			break;
		case 0x0800:
			elem->setCornerMode( CORNER_TOP_LEFT );
			break;
		case 0x1000:
			elem->setCornerMode( CORNER_BOTTOM_LEFT );
			break;
		case 0x1800:
			elem->setCornerMode( CORNER_BOTTOM_RIGHT );
			break;
	}

	
	if ((flag & 0x0600) == 0x0400)
		elem->setDirMode( DIR_HORZ );
	else
	if ((flag & 0x0600) == 0x0200)
		elem->setDirMode( DIR_VERT );

	elem->setCopyX( get2int( &io_pool[SUBCALL_OFF_XCNT] ) );
	elem->setCopyY( get2int( &io_pool[SUBCALL_OFF_YCNT] ) );

	// When reading the file, the block definitions come AFTER the
	// calls... so we will place a stand-in value giving the 1-based
	// index of the block being called.  Post-process to translate.
	//
	flag = get2int( &io_pool[SUBCALL_OFF_SUBDIR] );
	elem->setBlock( flag+1 );

	m_part->addElement( elem );

	return TRUE;
}

// ============================================================================
//		readSubDef
//
//		Does nothing more than identify the name and element count of the
//		Subroutine Definition... which will be created as a new element block
//		in the calling method.
//
int
CMyPm4::readSubDef(
	BYTE* io_pool )
{
	CString	name;

	name = getNascii( &io_pool[SUBDEF_OFF_NAME], 10 );
	m_varset->setVar( "Name", name );

	return get2int( &io_pool[SUBDEF_OFF_COUNT] );
}



// ============================================================================
//		joinBlocks
//
//		A post-process on reading a file.  Go through all of the block
//		elements and hook them up with the block pointer (eliminating the
//		index assigned by the PM4)
//
void
CMyPm4::joinBlocks( void )
{
	CElement*		elem;
	CBlockElement*	block;
	int				num;
	int				idx;
	int				blocknum;

	num = m_part->countElement();
	for (idx=0; idx<num; idx++)
	{
		elem = m_part->getElement( idx );
		if(elem->getType() == ELEM_BLOCK)
		{
			block = (CBlockElement*)elem;

			blocknum = block->getBlockIdx();
			if (blocknum >= 0)
			{
				block->setBlock( m_part->getBlock( blocknum ) );
			}
		}
	}
}
#endif

#ifdef PM4_TEXT
// ============================================================================
//		readText
//
//		Maps to TextElement
//
bool CMyPm4::readText( BYTE* io_pool )
{
	CTextElement*	elem;
	CPoint3			pnt;
	short				length;

	elem = new CTextElement;
	pnt.Set(	get8double( &io_pool[TEXT_OFF_X] ),
				get8double( &io_pool[TEXT_OFF_Y] ),
				get8double( &io_pool[TEXT_OFF_Z] ) );
	elem->setStart( &pnt );

	length = get2int( &io_pool[TEXT_OFF_LEN] );
	elem->setText( getNascii( &io_pool[TEXT_OFF_TEXT], length ) );

	elem->setHeight( get8double( &io_pool[TEXT_OFF_HEIGHT + length] ) );
	elem->setAngle( RAD_TO_DEG(get8double( &io_pool[TEXT_OFF_ANGLE + length] ) ) );

	m_part->addElement( elem );

	return TRUE;
}
#endif


// ============================================================================
//		readRefPlane
//
//		Maps to Plane
//
CReturn
CMyPm4::read_refplane(
	CModel*	model, 
	BYTE*	io_pool )
{
	CReturn			ret;
	CDbWorkplane*	ref;

	model->EntityCreate( DBWORKPLANE, (CDbEntity**)&ref );

	C3dVec ivec(	get8double( &io_pool[MATRIX_OFF_WI + MATRIX_OFF_X] ),
					get8double( &io_pool[MATRIX_OFF_WI + MATRIX_OFF_Y] ),
					get8double( &io_pool[MATRIX_OFF_WI + MATRIX_OFF_Z] ) );

	C3dVec jvec(	get8double( &io_pool[MATRIX_OFF_WJ + MATRIX_OFF_X] ),
					get8double( &io_pool[MATRIX_OFF_WJ + MATRIX_OFF_Y] ),
					get8double( &io_pool[MATRIX_OFF_WJ + MATRIX_OFF_Z] ) );

	C3dVec kvec(	get8double( &io_pool[MATRIX_OFF_WK + MATRIX_OFF_X] ),
					get8double( &io_pool[MATRIX_OFF_WK + MATRIX_OFF_Y] ),
					get8double( &io_pool[MATRIX_OFF_WK + MATRIX_OFF_Z] ) );

	C3dCoord tpnt(	get8double( &io_pool[MATRIX_OFF_WT + MATRIX_OFF_X] ),
					get8double( &io_pool[MATRIX_OFF_WT + MATRIX_OFF_Y] ),
					get8double( &io_pool[MATRIX_OFF_WT + MATRIX_OFF_Z] ) );

	ref->Init( ivec, jvec, kvec, tpnt, TRUE );

	CString name = getNascii( &io_pool[MATRIX_OFF_NAME], 9 );
	ref->Name( name );

	m_refname.Add( name );

	return ret;
}



// =======================================================================
//		get2int
//
//		Retrieve a 2-byte integer from a Morbidelli pool
//
int		
CMyPm4::get2int( 
	BYTE *in_pool )
{
   WORD  val;
   BYTE  *val_ptr;

   val = 0;
   val_ptr = (BYTE *)&val;

   val_ptr[0] = in_pool[0];
   val_ptr[1] = in_pool[1];

   return val;
}

// =======================================================================
//		get4int
//
//		Retrieve a 4-byte integer from a Morbidelli pool
//
int		
CMyPm4::get4int( 
	BYTE *in_pool )
{
   int   val;
   BYTE  *val_ptr;
	int	idx;

   val = 0;
   val_ptr = (BYTE *)&val;

	for (idx=0; idx<4; idx++)
		val_ptr[idx] = in_pool[idx];

   return val;
}

// =======================================================================
//		get4float
//
//		Retrieve a 4-byte float from a data pool
//
float		
CMyPm4::get4float( 
	BYTE *in_pool )
{
   float val;
   BYTE  *val_ptr;
	int	idx;

   val = (float)0.0;
   val_ptr = (BYTE *)&val;

	for (idx=0; idx<4; idx++)
		val_ptr[idx] = in_pool[idx];

   return val;
}

// =======================================================================
//		get8double
//
//		Retrieve a 8-byte double from a data pool
//
double
CMyPm4::get8double( 
	BYTE *in_pool )
{
   double	val;
   BYTE*		val_ptr;
	int		idx;

   val = (double)0.0;
   val_ptr = (BYTE *)&val;

	for (idx=0; idx<8; idx++)
		val_ptr[idx] = in_pool[idx];

   if (fabs(val) < .00001)
		val = 0.0;

	return val;
}

// =======================================================================
//		get24point
//
//		Retrieve 3, 8-byte double from a data pool
//
void
CMyPm4::get24point( 
	BYTE*		in_pool,
	C3dCoord*	io_point )
{
	io_point->X( get8double( &in_pool[0] ) );
	io_point->Y( get8double( &in_pool[8] ) );
	io_point->Z( get8double( &in_pool[16] ) );
}

// =======================================================================
//		getNascii
//
//		Retrieve a multi-byte string from a Morbidelli pool
//
CString
CMyPm4::getNascii( 
	BYTE *in_pool, 
	int in_num )
{
	CString	text;
	char		ch;
   int		idx;

   text.Empty();
	for (idx=0; idx<in_num; idx++)
   {
		ch = (char)in_pool[idx];
		if (!ch)
			break;
		text += ch;
   }
	return text;
}

// =======================================================================
//		getNbin
//
//		Retrieve a multi-byte buffer from a Morbidelli pool
//
void		
CMyPm4::getNbin( 
	BYTE *in_pool, 
	BYTE *out_buf, 
	int in_num )
{
   int   idx;

   for (idx=0; idx<in_num; idx++)
      out_buf[idx] = in_pool[idx];
}




// =======================================================================
int
CMyPm4::LayerCount()
{
	return m_importUtil.LayerCount();
}

CString
CMyPm4::LayerName( int index )
{
	return m_importUtil.LayerName( index );
}

double
CMyPm4::CheckGet( BYTE *in_pool )
{
	return (double)get4float( &in_pool[COM_OFF_MAT_TOP] );
}

double
CMyPm4::ClearGet( BYTE *in_pool )
{
	return (double)get4float( &in_pool[COM_OFF_CLEAR_Z] );
}

void Dump( BYTE* pool, int size )
{
	CReturn	status;
	CString	tmp;
	CString	buf;
	int	indx;
	int	jndx;
	int INCR = 16;

	// double val = 1.0;
	// int ival = 6;
	short val = 6;

	BYTE* bval = (BYTE*) &val;

	for (indx = 0; indx < sizeof(val); ++indx)
	{
		tmp.Format( " %d", (int) bval[indx] );
		buf += tmp;
	}

	status.Diagnostic( buf );
	status.Diagnostic( "-----------------------" );

	for (indx = 0; indx < size; indx+=INCR)
	{
		buf.Empty();

		for (jndx = 0; jndx < INCR; ++jndx)
		{
			if ((indx + jndx) >= size)
				break;

			tmp.Format( " %d", (int) pool[indx+jndx] );
			buf += tmp;
		}

		status.Diagnostic( buf );
	}
}

CDbTool*
CMyPm4::ToolFind( const CModel& model, const CString& name )
{
	CDbIterator	iter;

	int num = name.GetLength() - 1;
	CString tmp = name.Mid( 5, num );

	num = atoi( tmp );

	iter.Init( model.Db(), DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if (dbTool->IntGet( "step.id", 0 ) == num)
			return dbTool;

		iter.Next();
	}

	return NULL;
}

CDbTool*
CMyPm4::GetTool( const CString&	in_tool )
{
	CDbTool*	dbTool;
	CString		temp;
	// int			tassign_num;		

#if BEFORE_2004_09_09  // Seaberg crashed
	if (in_tool.Left(4).CompareNoCase("step") == 0)
	{
		temp = in_tool.Mid( 5, (in_tool.GetLength() - 5) );

		tassign_num = m_jof->ToolAssignmentGet( atoi(temp) );
		temp.Format( "step_%d", tassign_num );
	}
	else
	{
		temp = in_tool;
	}
#else
	temp = in_tool;
#endif

	if (!m_model->EntityFind( temp, (CDbEntity**) &dbTool, DBTOOL, DBTOOL ).isOkay())
	{
		dbTool = ToolFind( (*m_model), temp ); 

		dbTool->Name( temp );
	}

	return dbTool;
}

void
CMyPm4::XY_PlaneCreate()
{
	CReturn			ret;
	CDbWorkplane*	ref;

	m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&ref );

	C3dVec ivec( 1., 0., 0.);

	C3dVec jvec( 0., 1., 0.);

	C3dVec kvec( 0., 0., 1.);

	C3dCoord tpnt( 0., 0., 0. );

	ref->Init( ivec, jvec, kvec, tpnt, TRUE );

	ref->Name( "XY_PLANE" );

	m_refname.Add( "XY_PLANE" );

}
