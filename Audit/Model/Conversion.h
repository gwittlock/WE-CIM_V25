
#ifndef _CONVERSION_H
#define _CONVERSION_H

#include "GeoElem.h"
#include "GeoPoly.h"
#include "Profile.h"

class CDbEntity;
class CDbWorkplane;
class CDbTool;
class CDbContainer;
class CDbProfile;
class CDbFeature;
class CModel;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CConversion
{
public:

	static CReturn Convert(
						const CDbWorkplane*	dbWork,
						const CDbEntity*	dbEntity,
						CGeoPoly*			poly );

	static CReturn Convert(
						const CDbWorkplane*	dbWork,
						const CDbEntity*	dbEntity,
						CProfile*			profile );

	static CReturn Convert(
						const CProfile&		profile,
						double				zlevel,
						CDbTool*			dbTool,
						CDbWorkplane*		dbWork,
						CDbContainer*		dbContainer );

	static CReturn Convert(
						const CProfileList&	profList,
						double				zlevel,
						CDbTool*			dbTool,
						CDbWorkplane*		dbWork,
						CDbFeature*			dbFeature );

	static CReturn Convert(
						const CGeoElemList&	elems,
						CDbTool*			dbTool,
						CDbWorkplane*		dbWork,
						CDbFeature*			dbFeature );

private:

};

#endif
