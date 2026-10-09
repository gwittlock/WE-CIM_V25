#if !defined(_LEADDATA_H)
#define _LEADDATA_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common.h"
#include "ToolConst.h"
#include "DaoDb.h"


// ==================================================================

// LEAD_CONTEXT_NUM is the number of permutations
// of inside/outside, line/arc contexts.
#define LEAD_CONTEXT_NUM	8

enum eLeadContext
{
	LEAD_CONTEXT_INVALID   = -1,
	LEAD_IN_INTERNAL_LINE  = 0,		// 000
	LEAD_IN_INTERNAL_ARC   = 1,		// 001
	LEAD_IN_EXTERNAL_LINE  = 2,		// 010
	LEAD_IN_EXTERNAL_ARC   = 3,		// 011
	LEAD_OUT_INTERNAL_LINE = 4,		// 100
	LEAD_OUT_INTERNAL_ARC  = 5,		// 101
	LEAD_OUT_EXTERNAL_LINE = 6,		// 110
	LEAD_OUT_EXTERNAL_ARC  = 7		// 111
};

// LEAD_CONTEXT_MASK masks out all bits except
// internal/external, line/arc and in/out.
#define LEAD_CONTEXT_MASK	0x07

// Context bits
#define LEAD_X_ARC			0x01
#define LEAD_X_LINE			0x00

#define LEAD_X_EXTERNAL		0x02
#define LEAD_X_INTERNAL		0x00

#define LEAD_X_OUT			0x04
#define LEAD_X_IN			0x00

// Off-index context bits
//#define LEAD_X_CCW		0x08
//#define LEAD_X_CW			0x00

#define LEAD_X_RIGHT		0x10
#define LEAD_X_LEFT			0x00

// ==================================================================

enum eLeadType
{
	LEAD_NONE = 0,
	LEAD_LINE,
	LEAD_ARC,
	LEAD_LINELINE,
	LEAD_LINEARC,
	LEAD_RAMP
};

enum eLeadJunction
{
	LEAD_EXACT = 0,
	LEAD_GAP,
	LEAD_OVERLAP,
	LEAD_TAB
};

// Use protractor convention.
enum eLeadSide
{
	LEAD_RIGHT = -1,
	LEAD_LEFT  = +1
};

enum ePierceType
{
	PIERCE_NONE,	// 0
	PIERCE_LEAD,	// 1
	PIERCE_ONLY		// 2
};

// ==================================================================

class dllExport CLeadData
{
public:

	CLeadData();
	CLeadData( const CLeadData& );

	virtual ~CLeadData();

	const CLeadData& operator = ( const CLeadData& );

	CReturn	Load( const CString& configdb, int index );
	CReturn	Load( CDaoDB& db, int table, int index );

	// Access Methods

	const CString& Name() const						{ return m_name; }

	eLeadContext Context() const					{ return m_context; }
	void Context( eLeadContext context )			{ m_context = context; }

	eLeadType		Type( void ) const				{ return m_type; }
	void			Type( eLeadType type )			{ m_type = type; }

	eLeadJunction	Junction( void ) const			{ return m_junction; }
	void			Junction( eLeadJunction junc)	{ m_junction = junc; }

	double			LineLen( void ) const			{ return m_length; }
	void			LineLen( double len )			{ m_length = len; }

	double			LineAng( void ) const			{ return m_angle; }
	void			LineAng( double ang )			{ m_angle = ang; }

	double			ArcRad( void ) const			{ return m_radius; }
	void			ArcRad( double rad )			{ m_radius = rad; }

	double			ArcAng( void ) const			{ return m_included; }
	void			ArcAng( double ang )			{ m_included = ang; }

	double			Distance( void ) const			{ return m_distance; }
	void			Distance( double dist )			{ m_distance = dist; }

	double			TabThick( void ) const			{ return m_tabthick; }
	void			TabThick( double thick )		{ m_tabthick = thick; }

	ePierceType		UsePierce( void ) const			{ return m_usepierce; }
	void			UsePierce( ePierceType pierce )	{ m_usepierce = pierce; }

	double			PierceOffset( void ) const		{ return m_pierceoffset; }
	void			PierceOffset( double off )		{ m_pierceoffset = off; }

	double			PierceDiameter( void ) const	{ return m_piercediam; }
	void			PierceDiameter( double diam )	{ m_piercediam = diam; }

	eToolType		PierceType( void ) const		{ return m_piercetype; }
	void			PierceType( eToolType type )	{ m_piercetype = type; }

	int				PierceID(void) const			{ return m_pierceid; }
	void			PierceID(int id)				{ m_pierceid = id; }

	int				ClockPosition() const			{ return m_clock; }
	void			ClockPosition( int pos )		{ m_clock = pos; }

	double			SplitUParam() const				{ return m_usplit; }
	void			SplitUParam( double u )			{ m_usplit = u; }

	int				Location( void ) const			{ return m_locate; }
	void			Location( int loc )				{ m_locate = loc; }

	double			Tolerance( void ) const			{ return m_tolerance; }
	void			Tolerance( double tol )			{ m_tolerance = tol; }

	bool			UseTilt( void ) const			{ return m_usetilt; }
	void			UseTilt( bool tilt )			{ m_usetilt = tilt; }

	double			MinOverlap( void ) const		{ return m_minoverlap; }
	void			MinOverlap( double min )		{ m_minoverlap = min; }


	double			LeadWidth() const;

private:

	double SplitValue( int cmdb_value ) const;

private:
	// Disabled.
	int operator == ( const CLeadData& ) const;
	int operator != ( const CLeadData& ) const;

private:

	CString			m_name;  // Lead Parameter name in CMDB
	eLeadContext	m_context;

	eLeadType		m_type;
	eLeadJunction	m_junction;

	int		m_clock;  // (0..11) (0 aka 12)
	double	m_usplit;

	int			m_locate;

	double		m_length;
	double		m_angle;
	double		m_radius;
	double		m_included;
	double		m_distance;
	double		m_tabthick;
	double		m_tolerance;

	ePierceType	m_usepierce;
	double		m_pierceoffset;		// distance; 0.0 at edge, 1/2 diameter at center
	double		m_piercediam;
	eToolType	m_piercetype;		// Pierce Tool Type
	int			m_pierceid;			// Or, specify the tool by ID

	bool		m_usetilt;

	double		m_minoverlap;
};

#endif

