#ifndef _CADLAYER_H
#define _CADLAYER_H

#include "DynamicArray.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CCadLayer
{
public:

	CCadLayer( const CString& name, long handle )
	{
		m_name = name;
		m_handle = handle;
	}

	const CString& Name() const		{ return m_name; }
	long Handle() const				{ return m_handle; }

private:

	CString	m_name;
	long	m_handle;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CCadLayerList : public CDynamicArray<CCadLayer*>
{
public:

	CCadLayerList();
	
	void	Append( const CString& layerName );

	void	Append( const CString& layerName, long handle );

	void	Sort();

	int		Find( const CString& layerName );

	~CCadLayerList();

private:

	static int	NamesSort( const void* ptrA, const void* ptrB );
	static int	NamesCompare( const void* ptrA, const void* ptrB );

private:

	bool	m_acceptAll;
};

#endif