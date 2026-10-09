
#ifndef _CLFILE_H
#define _CLFILE_H

#ifndef _VARLIST_H
#include "VarList.h"
#endif


class ClfileRec;

class dllExport CClfile  
{
public:

	CClfile() : m_attribs()  { };

	/////////////////////////////////////////////
	// Obtain the total number of records.
	virtual int Count() const = 0;

	virtual int CurrRecNo() const = 0;

	virtual const ClfileRec* Fetch( int recNo ) const = 0;

	/////////////////////////////////////////////
	// Read a record and transfer its 
	// values to the given buffers.
	// 
	// The default calling convention Read( recNo < 0 )
	// simply reads the next record.
	//
	// The other calling convention Read( recNo >= 0 )
	// reads the specified record.
	//
	// NOTE: Some arguments are 'long' instead of 'int'
	// because Java's 'jint' is a 'long'.
	//
	virtual int Read( long         recNo,
					  long*        recInfo,
					  long*        intArray,
					  long         intArraySize,
					  double*      dblArray,
					  long         dblArraySize ) const = 0;

	const CVarList& Attrib() const		{ return m_attribs; }
	CVarList* pAttrib()					{ return &m_attribs; }

	virtual ~CClfile()  { };

private:

	// A varlist for tracking all attribute information,
	// whether attached to an entity or to the model header.
	// TODO: We must be careful to ensure there are no name
	// collisions between the two name spaces.
	CVarList m_attribs;
};

#endif
