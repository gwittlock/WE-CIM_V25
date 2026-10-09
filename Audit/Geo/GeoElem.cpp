
#include <vector>

#include "stdafx.h"
#include "GeoElem.h"


CBaseGeoRenderer* g_geo_renderer = NULL;

CBaseGeoRenderer& GeoRendererGet()
{
	return (*g_geo_renderer);
}

void GeoRendererSet( CBaseGeoRenderer& renderer )
{
	g_geo_renderer = &renderer;
}



#if TRACE_GEO_HEAP
	#define GEO_HEAP_DELTA 1024;
	int geo_heap_size = 0;
	int* geo_heap = NULL;
	int g_id = -1;

	void geo_heap_dump( int count )
	{
		if (geo_heap != NULL)
		{
			for (int indx = 0; indx < geo_heap_size; ++indx)
			{
				if (geo_heap[indx] > 0)
				{
					TRACE( "%d\n", indx );

					--count;
					if (count <= 0)
						break;
				}

			}

			TRACE("\n");
		}
	}
#else
	void geo_heap_dump( int count )  {  /* do nothing */ }
#endif

//////////////////////////////////////////////////////////////////////

CGeoElem::CGeoElem()
	: m_box(),
	  m_attribs( NULL ),
	  m_userdata( NULL )
{
#if TRACE_GEO_HEAP
	++g_id;
	if (geo_heap == NULL)
	{
		geo_heap_size = GEO_HEAP_DELTA;
		geo_heap = (int*) calloc( geo_heap_size, sizeof(int) );
	}
	else if (g_id >= geo_heap_size)
	{
		geo_heap_size += GEO_HEAP_DELTA;
		geo_heap = (int*) realloc( geo_heap, (geo_heap_size * sizeof(int)) );
	}

	geo_heap[g_id] = g_id;
	m_id = g_id;
#endif
}

CGeoElem::~CGeoElem()
{
	if (m_attribs != NULL)
		delete m_attribs;
#if TRACE_GEO_HEAP
	geo_heap[m_id] = 0;
#endif
}

const C3dBox& CGeoElem::Box() const
{
	return m_box;
}

void CGeoElem::Box( const C3dBox& box )
{
	m_box = box;
}

const CVarList& CGeoElem::Attrib() const
{
	return ((m_attribs == NULL) ? CVarList::Bogus() : (*m_attribs));
}

CVarList* CGeoElem::pAttrib()
{
	if (m_attribs == NULL)
		m_attribs = new CVarList();

	return m_attribs;
}

int CGeoElem::AttribCount() const
{
	return ((m_attribs == NULL) ? 0 : m_attribs->countVar());
}

int CGeoElem::IntGet( const CString& name, int defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getInt( name, defval ));
}

double CGeoElem::DoubleGet( const CString& name, double defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getReal( name, defval ));
}

CString CGeoElem::StringGet( const CString& name, const CString& defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getString( name, defval ));
}

void CGeoElem::IntSet( const CString& name, int ival )
{
	pAttrib()->setInt( name, ival );
}

void CGeoElem::DoubleSet( const CString& name, double dval )
{
	pAttrib()->setReal( name, dval );
}

void CGeoElem::StringSet( const CString& name, const CString& sval )
{
	pAttrib()->setString( name, sval );
}

void CGeoElem::AttribDelete( const CString& name )
{
	if (m_attribs != NULL)
		m_attribs->deleteVar( name );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For debugging.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void CGeoElem::Draw() const
{
	GeoRendererGet().DrawGeo( *this );
}

void CGeoElemArray::Draw() const
{
	GeoRendererGet().DrawGeo( *this );
}

void CGeoElemList::Draw() const
{
	GeoRendererGet().DrawGeo( *this );
}

