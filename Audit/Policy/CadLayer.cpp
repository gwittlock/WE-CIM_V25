
#include "stdafx.h"
#include "CadLayer.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CCadLayerList::CCadLayerList()
	: m_acceptAll( FALSE )
{
}

CCadLayerList::~CCadLayerList()
{
	CDynamicArray<CCadLayer*>::DestructiveFlush();
}

void
CCadLayerList::Append( const CString& layerName )
{
	CDynamicArray<CCadLayer*>::Append( new CCadLayer( layerName, 0 ) );
	if (Count() == 1)
	{
		m_acceptAll = (layerName.CompareNoCase("*default*") == 0);
	}
}

void
CCadLayerList::Append( const CString& layerName, long handle )
{
	CDynamicArray<CCadLayer*>::Append( new CCadLayer( layerName, handle ) );
}

void
CCadLayerList::Sort()
{
	Qsort( &NamesSort );
}

// Returns index of layer name.
int
CCadLayerList::Find( const CString& layerName )
{
	int indx;

	if ( m_acceptAll )
	{
		indx = 0;
	}
	else
	{
		CCadLayer layer( layerName, 0 );

		bool found = BinarySearch( (void*) &layer, &NamesCompare, &indx );
		if ( !found )
			indx = -1;
	}

	return indx;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int
CCadLayerList::NamesSort( const void* ptrA, const void* ptrB )
{
	CCadLayer* layerA = (*(CCadLayer**) ptrA);
	CCadLayer* layerB = (*(CCadLayer**) ptrB);

	int diff = layerA->Name().CompareNoCase( layerB->Name() );

	return diff;
}

int
CCadLayerList::NamesCompare( const void* ptrA, const void* ptrB )
{
	CCadLayer* layerA = ((CCadLayer*) ptrA);
	CCadLayer* layerB = ((CCadLayer*) ptrB);

	int diff = layerB->Name().CompareNoCase( layerA->Name() );

	return diff;
}


