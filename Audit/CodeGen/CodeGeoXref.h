
#ifndef _CODEGEOXREF_H
#define _CODEGEOXREF_H

// ============================================================================
// A quick/temporary solution to providing bidirectional geometric/text editing.
//
// A list of 'blocks of NC code' is created for a given part via Regenerate().
// At first implementation, the format of the blocks is restricted to that
// required by Morbidelli machine tools.
//
// As the blocks of NC code are generated in a list, a given block can be
// obtained via RecGet() by providing the appropriate index into the list.
// The record returned by this call has three data members, the text,
// associate element id (guid) and the text type.
//
// There is a many-to-one association between an element and blocks of code.
// These blocks are contiguous.  The first block can be located via RecIndexGet().
//
// You can manually append CCodeGeoRec objects to the end of the list using
// RecAppend() or you can request the list be automatically regenerated for
// you for a given part using Regenerate().  As the CCodeGeoXref was conceived
// to satisfy certain customer expectations, this latter method is restricted
// to using a CMorbidelliFormatter to format the data.
//
// ============================================================================

#include "CodeGeoRec.h"
#include "ModelClfile.h"


// =======================================================================

class dllExport CCodeGeoXref
{
public:

	CCodeGeoXref();

	void Init( const CModelClfile* clfile );

	// Get the number of records in the list.
	int Count();

	const CCodeGeoRec* GetAt( int recNo );

	// Get the Text/Geo record at a given list-index
	bool Seek( int recNo );

	// Get the record number of the first text
	// item associated with the given entity id.
	int RecNo( ID dbEntityId );

	ID Id() const;

	int RecType() const;

	const CString& Text() const;

	const CDbEntity* Entity() const;

	// Append a new CCodeGeoRec record to the end of the list.
	int Append( const char* text );
	
	// Delete all records.
	void Flush();

	virtual ~CCodeGeoXref( void );

private:

	CGeoElem* GeoElemCreate( const CString& params );

private:

	// Disabled.
	CCodeGeoXref( const CCodeGeoXref& );
	CCodeGeoXref& operator = ( const CCodeGeoXref& );
	int operator == ( const CCodeGeoXref& );
	int operator != ( const CCodeGeoXref& );

private:

	// Pointer to clfile that is being used.
	const CModelClfile*	m_clfile;

	// The list of text/geometry records.
	CCodeGeoRecList	m_list;

	// The current record in m_list.
	CCodeGeoRec*	m_rec;
};

#endif