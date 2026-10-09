
#include "stdafx.h"
#include "DbEntity.h"
#include "DbCurve.h"
#include "DbHole.h"
#include "WorkPkg.h"



////////////////////////////////////////////////////////////////////////

CWorkPkg::	CWorkPkg(
					CDbFeature* topLevelFeature,
					bool		isSubdef,
					double		xmin,
					double		ymin,
					double		xmax,
					double		ymax )

	: m_feature( topLevelFeature ),
	  m_isSubdef( isSubdef ),
	  m_box( xmin, ymin, xmax, ymax ),
	  m_entities()
{
}

CWorkPkg::~CWorkPkg()
{
	m_entities.BenignFlush();
}

C3dCoord
CWorkPkg::StartPt()
{
	C3dCoord	ps;
	CDbEntity*	dbEntity;
	CDbCurve*	dbCurve;
	CDbHole*	dbHole;

	if (m_entities.Count() > 0)
	{
		dbEntity = m_entities[0];

		dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
		if (dbCurve != NULL)
			ps = dbCurve->StartPt();
		else
		{
			dbHole = dynamic_cast<CDbHole*>( dbEntity );
			if (dbHole != NULL)
				ps = dbHole->Center();
		}
	}

	return ps;
}

// Eeeeew gross !
// This method was introduced as a work-around to a bug where entities
// under clamps were not being processed when grid optimization was active.
// Attempting to avoid a long explanation, the X values of the bounding box
// were originally retrieved from the 'xmin' & 'xmax' attributes attached to
// any feature having the '_repo' attribute.  The assumption being that there
// was a one-to-one relationship between a workzone and a repo-feature.  Well,
// this is not the case with clamp-features, and it is easier to address the
// problem in this manner than it is to propogate other fixes through nesting, etc.
//
// NOTE: The Y bounds are left intact because they are determined by the material.
void
CWorkPkg::BoxUpdate()
{
	CDbEntity*	dbEntity;
	C3dBox		box;
	int			count, indx;

	count = m_entities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = m_entities[indx];
		box = dbEntity->Box();
		m_box.X( box.Xmin() );
		m_box.X( box.Xmax() );
	}
}
