
#ifndef _DBENTITYVISITOR_H
#define _DBENTITYVISITOR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbEntity.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CDbEntityVisitor
{
public:

	virtual void Visit( CDbEntity* dbEntity ) = 0;

	virtual ~CDbEntityVisitor();

protected:

	CDbEntityVisitor();

private:  // Methods

private:  // Disabled

	CDbEntityVisitor( const CDbEntityVisitor& );
	const CDbEntityVisitor& operator = ( const CDbEntityVisitor& );
	int operator == ( const CDbEntityVisitor& ) const;
	int operator != ( const CDbEntityVisitor& ) const;

private:  // Data

};

#endif

