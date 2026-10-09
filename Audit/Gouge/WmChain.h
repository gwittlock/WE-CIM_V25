#pragma once

#include "Return.h"
#include "IndxList.h"
#include "3dCoord.h"
#include "WmSubchn.h"
#include "3dBox.h"
#include "3x4Matrix.h"
#include "VarList.h"

class CWmChainIterator;

class CWmChain;
typedef CIndxList<CWmChain*> CWmChainList;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmChain
{
	friend class CWmChainIterator;

public:

	CWmChain();

	CWmChain( const CWmChain& chain );

	virtual ~CWmChain();

	void Reinit();

	// Determines whether the chain is closed (really only checks
	// for coincidence of the end points of the chain).
	bool IsClosed() const;

	// Obtain the count of geometric entities in the chain.
	int Count() const;

	C3dBox Box() const;

	// Get the end point coordinate of the chain.
	C3dCoord StartPt() const;
	C3dCoord EndPt() const;

	// Add the given entity to the end of this chain.
	// This chain takes ownership of the entity.
	void Append( CGeoElem* elem );

	// Add a copy of the given entity to the end of this chain.
	void CopyAppend( const CGeoElem& elem );
	void CopyAppend( const CWmChain& chain );

	// Add the given entity to the beginning of this chain.
	// This chain takes ownership of the entity.
	void Prepend( CGeoElem* elem );

	// Add a copy of the given entity to the beginning of this chain.
	void CopyPrepend( const CGeoElem& elem );

	// Reverse() should be applied only to 'virgin' chains, ie.
	// a chain that has just been created and that has no subchains,
	// especially subchains that have valid interference indices.
	void Reverse();

	void Xform( const C3x4Matrix& xform );

	// Create the offset chain.  Note: large offsets can return a
	// birfurcated results, that is, the mapping between the parent
	// chain and its offset is not necessary one-to-one.
	CReturn Offset( int offsetDir, double offsetAmt, double sharpAngle, CWmChainList* results ) const;

	CReturn Degouge( int offsetDir, CWmChainList* degougedChains );

	// Calculate the signed area of a closed chain.
	// A negative area implies clockwise curve orientation.
	double Area() const;

	void CopyAppend( const CWmSubchn& subchn );

	// Verifies that this chain is C0 continuous within the given tolerance.
	bool IsC0Continuous( double tol, bool checkForClosure );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Written specifically for the Degouge()ing process.
	//
	//     +-->  When offset to the right by a distance
	//     |     greater than the segment length, the last
	//     |     element in the offset may appear to the
	//  +--+     left of the other offset elements and
	//  |        may also be C0 discontinuous.  Therefore,
	//  |        we ignore the C0 discontinuous elements.
	void C0ContinuousCopyAppend( const CWmSubchn& subchn, double tol );

	int SetNo() const;
	void SetNo( int setNo );

	void Debug( const char* caption ) const;

	void DegenerateFilter();

	bool IsAbhorrentChain( const CWmChain& chain );

	bool Encloses( const CWmChain& chain );

	void PurgeSubchns();

	// Attribute management methods.

	void AttribsPropogate( bool propogate )  { m_propogate = propogate; }

	int AttribCount() const;

	int IntGet( const CString& name, int defval ) const;
	double DoubleGet( const CString& name, double defval  ) const;
	CString StringGet( const CString& name, const CString& defval ) const;

	void IntSet( const CString& name, int ival );
	void DoubleSet( const CString& name, double dval );
	void StringSet( const CString& name, const CString& sval );

	void AttribDelete( const CString& name );

	bool HasAttrib() const			{ return (m_attribs != NULL); }

	bool AnyElemAttribs() const;

	// Low-level methods (caution).
	const CVarList* Attrib() const;
	CVarList* pAttrib();

	// For debugging.
	void Draw() const;
	void Dump() const;

public:

	// WARNING: Very low-level methods (use judiciously)!
	// In fact, should treat as read-only.
	CWmSubchn* First() const;
	CWmSubchn* Last() const;

private:  // Disabled.

	const CWmChain& operator = ( const CWmChain& chain );
	int operator == ( const CWmChain& chain );
	int operator != ( const CWmChain& chain );

private:  // Methods.

	void CondInit();

	CReturn RawOffset( int offsetDir, double offsetAmt, double sharpAngle, CWmChain* result ) const;

	bool Int2dSegSeg(
		const C2dCoord& psA,
		const C2dUnitVec& vecA,
		const C2dCoord& psB,
		const C2dUnitVec& vecB,
		C2dCoord* pt ) const;

private:  // Data.

	CWmSubchn*	m_head;
	CWmSubchn*	m_tail;
	CVarList*	m_attribs;
	int			m_setNo;

	bool m_propogate;
};

typedef CIndxList<CWmChain*> CWmChainList;

