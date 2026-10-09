#if !defined(_DBTOOLGEN_H)
#define _DBTOOLGEN_H

// ==================================================================
//		DbToolGen
//
// ==================================================================

#include "Return.h"
#include "Shape.h"
#include "ToolSetup.h"
#include "Model.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

class dllExport CDbToolGen
{
public:

	CDbToolGen();

	CReturn	DbToolsCreate(
					const TArrayOfAttribLists&	tools,
					CModel*						model );

	CReturn	DbToolsCreate(
					const TSortedShapes&	tooledStationShapes,
					CModel*					model );

	///////////////////////////////////////////////////////////
	// Updates the attributes of existing tools in the model
	// by first matching a tool by is NC_Code_Number and then
	// substituting the model tool attibutes with those from
	// the machining database.  Additionally, new tools in
	// the database setup are added to the model.
	CReturn	DbToolsUpdate(
					const TArrayOfAttribLists&	tools,
					CModel*						model );

	virtual ~CDbToolGen();

protected:

private:
	// Disabled.
	CDbToolGen( const CDbToolGen& );
	const CDbToolGen& operator = ( const CDbToolGen& );
	int operator == ( const CDbToolGen& ) const;
	int operator != ( const CDbToolGen& ) const;

private:

};

#endif

