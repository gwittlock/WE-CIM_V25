#ifndef _CODEUTIL_H
#define _CODEUTIL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "3dCoord.h"


// ==================================================================

class dllExport CCodeUtil
{
public:

	static C3dCoord	StartPoint( const CDbEntity* dbEntity );
	static C3dCoord	EndPoint( const CDbEntity* dbEntity );

protected:

private:

	// Disabled.
	CCodeUtil();
	CCodeUtil( const CCodeUtil& );
	const CCodeUtil& operator = ( const CCodeUtil& );
	int operator == ( const CCodeUtil& ) const;
	int operator != ( const CCodeUtil& ) const;
	virtual ~CCodeUtil();

private:

};

#endif

