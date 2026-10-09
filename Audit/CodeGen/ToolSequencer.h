#if !defined(_TOOLSEQUENCER_H)
#define _TOOLSEQUENCER_H


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbEntity.h"
#include "Model.h"


// ==================================================================

class dllExport CToolSequencer
{
public:

	CToolSequencer();

	void	ModelOrder(
						const CDbEntityArray&	dbEntityList,
						CDbEntityArray*			dbToolOrder );

	void	ToolSetupOrder(
						const CModel&			model,
						bool					drillsOptimize,
						CDbEntityArray*			dbToolOrder );

	virtual ~CToolSequencer();

protected:

private:
	// Disabled.
	CToolSequencer( const CToolSequencer& );
	const CToolSequencer& operator = ( const CToolSequencer& );
	int operator == ( const CToolSequencer& ) const;
	int operator != ( const CToolSequencer& ) const;

private:

};

#endif

