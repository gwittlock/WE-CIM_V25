
#ifndef _PROFILE_H
#define _PROFILE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Return.h"
#include "3dBox.h"
#include "GeoCurve.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CProfile
{
public:

	// Create an empty profile.
	CProfile();

	// Obtain the number of elements in this profile.
	int Count() const;

	// Determine whether this profile is closed within system tolerance.
	bool IsClosed() const;

	// Determine whether this profile is closed within the given tolerance.
	bool IsClosed( double tol ) const;

	const CGeoCurveArray& Curves() const		{ return m_list; }

	// Access the ith element of this profile.
	CGeoCurve* GetAt( int indx ) const;
	
	// Attribute managment methods.
	int AttribCount() const;

	int IntGet( const CString& name, int defval ) const;
	double DoubleGet( const CString& name, double defval  ) const;
	CString StringGet( const CString& name, const CString& defval ) const;

	void IntSet( const CString& name, int ival );
	void DoubleSet( const CString& name, double dval );
	void StringSet( const CString& name, const CString& sval );

	void AttribDelete( const CString& name );

	bool HasAttrib() const			{ return (m_attribs != NULL); }

	// Low-level methods (caution).
	const CVarList& Attrib() const;
	CVarList* pAttrib();

	void AttribsPropogate( bool propogate )  { m_propogate = propogate; }
	bool AttribsPropogate() const            { return m_propogate; }

	// Obtain the profile end point.
	const C3dCoord* StartPt() const;
	const C3dCoord* EndPt() const;

	// Add an element to the end of this profile.
	CReturn Append( CGeoCurve* curve );
	CReturn CopyAppend( const CGeoCurve& curve );

	// Add references to profile elements to this profile.
	// NOTE: When using Append(), be very careful with issues
	// of element ownership ( see also ~CProfile() ).
	//
	// Upon failure, CopyAppend() restores this profile to its
	// original state.
	//
	CReturn Append( CProfile* prof );
	CReturn CopyAppend( const CProfile& prof );

	CReturn Prepend( CGeoCurve* curve );

	// Reverse the order and direction of the elements in the profile.
	// NOTE: Be careful with issues of element ownership.
	void Reverse();

	void Split( int index );
	void Reorder( int startIndex );

	// Set the Z ordinate of all elements in the profile.
	void ZSet( double elevation );

	// Remove all elements from this profile without calling
	// their destructor.  This method should be used when
	// the elements are owned by another object.
	void BenignFlush();

	// Delete the all elements in the indicated range [startIndex..count] inclusive.
	void DestructiveFlush( int startIndex = 0 );

	// Close an open profile by connecting the ends with a line segment.
	int ConditionalClose();

	// Apply colinear point elimination and merge adjacent 'like' arcs.
	void Reduce( double tol );
	void ArcsReduce();
	void LinesReduce( double tol );

	void Clean();

	C3dBox Box3d() const;
	double Area() const;

	void GapsClose();

	// For debugging
	void Dump() const;

	// NOTE: The destructor calls DestructiveFlush() which,
	// in turn, calls the destructor of each element in the
	// profile.  If you do not want to delete the elements,
	// call BenignFlush() prior to using this destructor.
	virtual ~CProfile();

private:

	// Disabled.
	const CProfile& operator = ( const CProfile& );
	int operator == ( const CProfile& );
	int operator != ( const CProfile& );
	
private:
	
	CGeoCurveArray m_list;
	CVarList* m_attribs;
	bool m_propogate;
};


typedef CIndxList<CProfile*> CProfileList;
typedef CDynamicArray<CProfile*> CProfileArray;

#endif
