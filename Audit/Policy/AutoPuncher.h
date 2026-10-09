#if !defined(_AUTOPUNCHER_H)
#define _AUTOPUNCHER_H

// ==================================================================
//		AutoPuncher
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AutoTool.h"


// ==================================================================

class dllExport CAutoPuncher
{
public:

	CAutoPuncher();

	// Auto-punch multiple entities.
	CReturn	AutoPunch(
				CDbEntityList*	entities,
				CModel*			model,
				CToolSetup*		toolSetup,
				bool			buildSetup,
				double			ptol,
				double			mtol,
				EMatchingRigour	rigour );

	// Auto-punch a single entity.
	// This method can be used to both create and regenerate punched features.
	CReturn	AutoPunch(
					CDbEntity*		dbEntity,
					CModel*			model,
					CToolSetup*		toolSetup,
					bool			buildSetup,
					double			ptol,
					double			mtol,
					EMatchingRigour	rigour,
					CDbFeature*		dbFeature );

	virtual ~CAutoPuncher();

protected:

private:
	// Disabled.
	CAutoPuncher( const CAutoPuncher& );
	const CAutoPuncher& operator = ( const CAutoPuncher& );
	int operator == ( const CAutoPuncher& ) const;
	int operator != ( const CAutoPuncher& ) const;

private:

};

#endif

