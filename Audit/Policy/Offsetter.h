
#ifndef _OFFSETTER_H
#define _OFFSETTER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _MODEL_H
#include "Model.h"
#endif

#ifndef _PROFILE_H
#include "Profile.h"
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport COffsetter
{
public:


	static CReturn UniformOffset(
						CModel* model,
						CDbWorkplane* dbWork,
						CDbTool* dbTool,
						CDbEntity* dbEntity,
						int dir,
						double delta,
						double sharpAngle,
						double zlevel,
						CDbFeature* dbFeature );

	static CReturn ConvexHullOffset(
						CModel* model,
						ID* featid,
						ID profid,
						ID toolid,
						int dir );

	static CReturn AutoIndexOffset(
						CModel* model,
						ID* featid,
						ID profid,
						ID toolid,
						int dir,
						bool use_long_side );

	static CReturn ProfOffset(
						CProfile*		src_profile,
						int				off_dir,
						CDbTool*		dst_tool,
						CProfileList*	dst_proflist );

protected:

private:  // Methods

private:  // Disabled

	COffsetter();
	COffsetter( const COffsetter& );
	virtual ~COffsetter();
	const COffsetter& operator = ( const COffsetter& );
	int operator == ( const COffsetter& ) const;
	int operator != ( const COffsetter& ) const;

private:  // Data

};

#endif

