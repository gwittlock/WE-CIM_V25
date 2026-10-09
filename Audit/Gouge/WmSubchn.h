
#ifndef _WMSUBCHN_H
#define _WMSUBCHN_H

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _WMNODE_H
#include "WmNode.h"
#endif

class C2dCoord;
class C3dCoord;
class CGeoElem;
class CWmElem;
class CWmChain;
class CWmChainIterator;

class CWmIntRec;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmSubchn : public CWmNode
{
	friend class CWmChainIterator;
	friend class CWmChainAssigner;
	friend class CWmChain;
	friend class CWmElem;

public:

	CWmSubchn( CWmChain* owner );

	virtual ~CWmSubchn();

	/////////////////////////////////////////////////////////////////
	// Get the chain that contains this subchain node.
	CWmChain* Owner() const;
	void Owner( CWmChain* owner );

	bool IsAssigned() const;

	/////////////////////////////////////////////////////////////////
	// Get the interference index of this subchain.
	int Iindex() const;
	void Iindex( int iindex );

	int Count();

	bool IsTerminal() const;

	/////////////////////////////////////////////////////////////////
	// Get the subchain from an intersecting chain that
	// has the same interference index as this subchain.
	CWmSubchn* Other() const;
	CWmSubchn* Other( int iindex ) const;

	CWmSubchn* Sister() const;

	/////////////////////////////////////////////////////////////////
	// Get the start point of this subchain.
	C3dCoord StartPt() const;
	
	/////////////////////////////////////////////////////////////////
	// Get the end point of this subchain.
	C3dCoord EndPt() const;

	CWmSubchn* SubchnPrev() const;
	CWmSubchn* SubchnNext() const;

	void SubchnPrev( CWmSubchn* node );
	void SubchnNext( CWmSubchn* node );

	virtual void Unlink();

	void IntRec( const CWmIntRec* intrec )		{ m_intrec = intrec; }

	void OwnerShipUpdate();

	void Purge();

	// For debugging.
	bool AnyElemAttribs() const;

protected:

	CWmElem* First() const;
	void First( const CWmElem* elem );

	CWmElem* Last() const;
	void Last( const CWmElem* elem );

private:

	// Disabled.
	CWmSubchn();
	CWmSubchn( const CWmSubchn& subchn );
	const CWmSubchn& operator = ( const CWmSubchn& subchn );
	int operator == ( const CWmSubchn& subchn ) const;
	int operator != ( const CWmSubchn& subchn ) const;

private:  // Methods.

	void Append( CWmElem* node );
	void Prepend( CWmElem* node );

private:  // Data.

	CWmSubchn* m_prev;
	CWmSubchn* m_next;

	// The parent chain.
	CWmChain* m_owner;

	// The interference index of this subchain.
	int m_iindex;

	const CWmIntRec* m_intrec;
};

typedef CIndxList<CWmSubchn*> CWmSubchnList;

#endif