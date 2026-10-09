
#include "stdafx.h"
#include "StringConst.h"
#include "GeoPoint.h"
#include "DbHole.h"
#include "ClfileRec.h"
#include "CodeGeoXref.h"


// =======================================================================

CCodeGeoXref::CCodeGeoXref()
	: m_clfile( NULL ),
	  m_rec( NULL )
{
}

CCodeGeoXref::~CCodeGeoXref( void )
{
	m_list.DestructiveFlush();
}

void
CCodeGeoXref::Init(	const CModelClfile* clfile )
{
	m_clfile = clfile;
	m_rec = NULL;
}

int
CCodeGeoXref::Count()
{
	return m_list.Count();
}

const CCodeGeoRec*
CCodeGeoXref::GetAt( int recNo )
{
	bool okay = ((recNo >= 0) && (recNo < m_list.Count()));

	return (okay ? m_list[recNo] : NULL);
}

bool
CCodeGeoXref::Seek( int recNo )
{
	bool okay = (recNo >= 0 && recNo < m_list.Count());

	if ( okay ) 
		m_rec = m_list[recNo];

	return okay;
}

int
CCodeGeoXref::RecNo( ID dbEntityId )
{
	int	count, indx;
	
	count = m_list.Count();

	for (indx = 0; indx < count; ++indx)
	{
		if (m_list[indx]->Id() == dbEntityId)
			return indx;  // success
	}

	return -1;  // failure
}

ID
CCodeGeoXref::Id() const
{
	return ((m_rec == NULL) ? 0 : m_rec->Id());
}

int
CCodeGeoXref::RecType() const
{
	return ((m_rec == NULL) ? -1 : m_rec->RecType());
}

const CString&
CCodeGeoXref::Text() const
{
	return ((m_rec == NULL) ? STR_EMPTY : m_rec->Text());
}

const CDbEntity*
CCodeGeoXref::Entity() const
{
	return ((m_rec == NULL) ? NULL : m_rec->Entity());
}

int
CCodeGeoXref::Append( const char* text )
{
	const ClfileRec*	clfileRec;
	CCodeGeoRec*		codeRec;
	const CDbEntity*	dbEntity;
	int					recNo;

	recNo = m_clfile->CurrRecNo();
	clfileRec = m_clfile->Fetch( recNo );

	dbEntity = clfileRec->Entity();

	if (text[0] == '\1')
	{
		// Special case processing, initially introduced to display
		// clamp avoidance movements. Requires the text to be
		// specially formatted (eg. "\1the_attribs\2the_cnc_block").
		//   See also GeoElemCreate().
		CString	tmp = text;

		int indx = tmp.Find('\2');
		if (indx > 0)
		{
			CGeoElem* elem = GeoElemCreate( tmp.Mid( 1, (indx - 1) ) );
			if (elem != NULL)
			{
				// Get the associated cnc block.
				tmp = tmp.Mid( indx + 1 );

				codeRec = new CCodeGeoRec( clfileRec->RecType(), dbEntity, tmp );
				codeRec->GeoElemSet( elem );
			}
		}
	}
	else
	{
		codeRec = new CCodeGeoRec( clfileRec->RecType(), dbEntity, text );
	}

	m_list.Append( codeRec );

	return 0;
}

void
CCodeGeoXref::Flush()
{
	m_list.DestructiveFlush();
}

// For use in displaying things like clamp avoidance movements.
CGeoElem* 
CCodeGeoXref::GeoElemCreate( const CString& params )
{
	CCommand	cmd;
	CGeoElem*	elem;
	CReturn		status;

	elem = NULL;

	// Parse the attribute string
	status = cmd.setCommand( params, NULL, NULL );
	if ( status.IsOk() )
	{
		const CVarList& attribs = cmd.VarList();

		int skip = attribs.getInt( "skip", FALSE );
		if ( skip )
		{
			// cmd format: "skip=1"
			// Provide a sentinel for CCodeGenProcessApp::Highlight()
			elem = new CGeoPoint( 0., 0., 0. );
			elem->IntSet( "skip", skip );

		}
		else
		{
			int rapid = attribs.getInt( "rapid", FALSE );
			int door = attribs.getInt( "door", FALSE );

			if ( rapid )
			{
				// cmd format: "rapid=%d,xs=%f,ys=%f,ye=%f,ye=%f"
				double xs, ys, xe, ye;

				status  = attribs.getReal( "xs", &xs );
				status += attribs.getReal( "ys", &ys );
				status += attribs.getReal( "xe", &xe );
				status += attribs.getReal( "ye", &ye );

				if ( status.IsOk() )
				{
					elem = new CGeoLine( xs, ys, xe, ye );
					elem->IntSet( "rapid", rapid );
				}
			}
			else if ( door )
			{
				// cmd format: "door=1,xp=%f,yp=%f"
				double xp, yp;

				status  = attribs.getReal( "xp", &xp );
				status += attribs.getReal( "yp", &yp );

				if ( status.IsOk() )
				{
					elem = new CGeoPoint( xp, yp, 0. );
					elem->IntSet( "door", door );
				}
			}
		}
	}

	return elem;
}
