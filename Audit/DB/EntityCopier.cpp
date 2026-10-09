
#include "stdafx.h"
#include "CommonFlags.h"
#include "3x4Matrix.h"
#include "EntityCopier.h"



////////////////////////////////////////////////////////////////////////

CEntityCopier::CEntityCopier()
{
	m_dst_db = NULL;
	m_fwd_ptr = NULL;
	m_rev_ptr = NULL;
}

CEntityCopier::~CEntityCopier()
{
}

void
CEntityCopier::Init( CEntityDb* dst_db, BYTE flags )
{
	m_dst_db = dst_db;
	m_flags = flags;

	m_fwd_ptr = &m_forward_reference_map;
	m_rev_ptr = NULL;

	m_fwd_ptr->RemoveAll();
}

void
CEntityCopier::Init(
				CEntityDb*		dst_db,
				tDbEntityMap*	forward_reference_map,
				tDbEntityMap*	reverse_reference_map,
				BYTE			flags )
{
	m_dst_db = dst_db;
	m_flags = flags;

	m_fwd_ptr = forward_reference_map;
	if (m_fwd_ptr != NULL)
		m_fwd_ptr->RemoveAll();
	else
		m_fwd_ptr = &m_forward_reference_map;

	m_rev_ptr = reverse_reference_map;
	if (m_rev_ptr != NULL)
		m_rev_ptr->RemoveAll();
}

void
CEntityCopier::Visit( CDbEntity* dbEntity )
{
	EDbEntityType	type;

	if (dbEntity == NULL)
		return;	// assert?

	type = dbEntity->Type();

	switch (type)
	{
	case DBWORKPLANE:
		WorkCopy( dynamic_cast<CDbWorkplane*>(dbEntity) );
		break;

	case DBTOOL:
		ToolCopy( dynamic_cast<CDbTool*>(dbEntity) );
		break;

	case DBPOINT:
		PointCopy( dynamic_cast<CDbPoint*>(dbEntity) );
		break;

	case DBLINE:
		LineCopy( dynamic_cast<CDbLine*>(dbEntity) );
		break;

	case DBARC:
		ArcCopy( dynamic_cast<CDbArc*>(dbEntity) );
		break;

	case DBHOLE:
		HoleCopy( dynamic_cast<CDbHole*>(dbEntity) );
		break;

	case DBPROFILE:
		ProfileCopy( dynamic_cast<CDbProfile*>(dbEntity) );
		break;

	case DBCOMMAND:
		CommandCopy( dynamic_cast<CDbCommand*>(dbEntity) );
		break;

	case DBFEATURE:
		FeatureCopy( dynamic_cast<CDbFeature*>(dbEntity) );
		break;

	case DBSEQUENCE:
		SequenceCopy( dynamic_cast<CDbSequence*>(dbEntity) );
		break;

	case DBPATTERN:
		PatternCopy( dynamic_cast<CDbPattern*>(dbEntity) );
		break;

	default:
		ASSERT( FALSE );  // Invalid case.
	}
}

CDbEntity*
CEntityCopier::ForwardLookup( CDbEntity* theSource )
{
	CDbEntity*	theCopy;

	if (m_fwd_ptr == NULL || !m_fwd_ptr->Lookup( theSource, theCopy ))
		theCopy = NULL;

	return theCopy;
}

CDbWorkplane*
CEntityCopier::WorkCopy( CDbWorkplane* dbWork )
{
	C3x4Matrix		xform;
	CDbEntity*		tmp;
	CDbWorkplane*	theCopy;

	if ( m_fwd_ptr->Lookup( dbWork, tmp ) )
	{
		theCopy = dynamic_cast<CDbWorkplane*>(tmp);
	}
	else if (dbWork->Db() == m_dst_db)
	{
		theCopy = dbWork;
		EntityMapUpdate( dbWork, theCopy );
	}
	else
	{
		m_dst_db->Create( DBWORKPLANE, (CDbEntity**) &theCopy );
		theCopy->CommonSubsetCopy( (*dbWork) );

		xform = dbWork->Transform();

		theCopy->Init(
					xform.getI(),
					xform.getJ(),
					xform.getK(),
					xform.getT(),
					dbWork->ToolUp() );

		EntityMapUpdate( dbWork, theCopy );
	}

	return theCopy;
}

CDbTool*
CEntityCopier::ToolCopy( CDbTool* dbTool )
{
	CDbEntity*	tmp;
	CDbTool*	theCopy;

	theCopy = NULL;

	if (dbTool != NULL && !dbTool->IsHidden())
	{
		if ( m_fwd_ptr->Lookup( dbTool, tmp ) )
		{
			theCopy = dynamic_cast<CDbTool*>(tmp);
		}
		else if (dbTool->Db() == m_dst_db)
		{
			theCopy = dbTool;
			EntityMapUpdate( dbTool, theCopy );
		}
		else
		{
			if (m_flags & FLAG_COPY_TOOL_BY_CONTENT)
			{
				// The "Nesting" case.

				theCopy = m_dst_db->ToolFindCreate( (*dbTool), TRUE );
			}
			else
			{
				// The "CodeGen" case.

				m_dst_db->Create( DBTOOL, (CDbEntity**) &theCopy );
				theCopy->CommonSubsetCopy( (*dbTool) );
			}

			EntityMapUpdate( dbTool, theCopy );
		}
	}

	return theCopy;
}

CDbPoint*
CEntityCopier::PointCopy( CDbPoint* dbPoint )
{
	CDbEntity*	tmp;
	CDbPoint*	theCopy;

	theCopy = NULL;

	if ( m_fwd_ptr->Lookup( dbPoint, tmp ) )
	{
		theCopy = dynamic_cast<CDbPoint*>(tmp);
	}
	else
	{
		CDbTool*	dbTool;

		dbTool = dbPoint->Tool();
		if (dbTool != NULL && !dbTool->IsHidden())
		{
			CDbWorkplane*	dbWork;

			m_dst_db->Create( DBPOINT, (CDbEntity**) &theCopy );
			theCopy->CommonSubsetCopy( (*dbPoint) );

			dbTool = ToolCopy( dbTool );
			dbWork = WorkCopy( dbPoint->Workplane() );

			theCopy->Init( dbTool, dbWork, dbPoint->Coord() );

			EntityMapUpdate( dbPoint, theCopy );
		}
	}

	return theCopy;
}

CDbLine*
CEntityCopier::LineCopy( CDbLine* dbLine )
{
	CDbEntity*	tmp;
	CDbLine*	theCopy;

	theCopy = NULL;

	if ( m_fwd_ptr->Lookup( dbLine, tmp ) )
	{
		theCopy = dynamic_cast<CDbLine*>(tmp);
	}
	else
	{
		CDbTool*	dbTool;

		dbTool = dbLine->Tool();
		if (dbTool != NULL && !dbTool->IsHidden())
		{
			CDbWorkplane*	dbWork;
			CDbPoint*		dbPs;
			CDbPoint*		dbPe;

			m_dst_db->Create( DBLINE, (CDbEntity**) &theCopy );
			theCopy->CommonSubsetCopy( (*dbLine) );

			dbTool = ToolCopy( dbTool );
			dbWork = WorkCopy( dbLine->Workplane() );

			dbPs = PointCopy( dbLine->DbStartPt() );
			dbPe = PointCopy( dbLine->DbEndPt() );

			theCopy->Init( dbTool, dbWork, dbPs, dbPe );

			EntityMapUpdate( dbLine, theCopy );

			ConditionalAddToContainer( dbLine, theCopy );
		}
	}

	return theCopy;
}

CDbArc*
CEntityCopier::ArcCopy( CDbArc* dbArc )
{
	CDbEntity*	tmp;
	CDbArc*	theCopy;

	theCopy = NULL;

	if ( m_fwd_ptr->Lookup( dbArc, tmp ) )
	{
		theCopy = dynamic_cast<CDbArc*>(tmp);
	}
	else
	{
		CDbTool*	dbTool;

		dbTool = dbArc->Tool();
		if (dbTool != NULL && !dbTool->IsHidden())
		{
			CDbWorkplane*	dbWork;
			CDbPoint*		dbPs;
			CDbPoint*		dbPe;
			CDbPoint*		dbPc;

			m_dst_db->Create( DBARC, (CDbEntity**) &theCopy );
			theCopy->CommonSubsetCopy( (*dbArc) );

			dbTool = ToolCopy( dbTool );
			dbWork = WorkCopy( dbArc->Workplane() );

			dbPs = PointCopy( dbArc->DbStartPt() );
			dbPe = PointCopy( dbArc->DbEndPt() );
			dbPc = PointCopy( dbArc->DbCenterPt() );

			theCopy->Init( dbTool, dbWork, dbPs, dbPe, dbPc, dbArc->Dir() );

			EntityMapUpdate( dbArc, theCopy );
		
			ConditionalAddToContainer( dbArc, theCopy );
		}
	}

	return theCopy;
}

CDbHole*
CEntityCopier::HoleCopy( CDbHole* dbHole )
{
	CDbEntity*	tmp;
	CDbHole*	theCopy;

	theCopy = NULL;

	if ( m_fwd_ptr->Lookup( dbHole, tmp ) )
	{
		theCopy = dynamic_cast<CDbHole*>(tmp);
	}
	else
	{
		CDbTool*		dbTool;

		dbTool = dbHole->Tool();
		if (dbTool != NULL && !dbTool->IsHidden())
		{
			CDbWorkplane*	dbWork;

			m_dst_db->Create( DBHOLE, (CDbEntity**) &theCopy );
			theCopy->CommonSubsetCopy( (*dbHole) );

			dbTool = ToolCopy( dbTool );
			dbWork = WorkCopy( dbHole->Workplane() );

			theCopy->Init( dbTool, dbWork, dbHole->Center(), dbHole->Diam(), dbHole->Depth() );

			EntityMapUpdate( dbHole, theCopy );

			ConditionalAddToContainer( dbHole, theCopy );
		}
	}

	return theCopy;
}

CDbCommand*
CEntityCopier::CommandCopy( CDbCommand* dbCommand )
{
	CDbEntity*	tmp;
	CDbCommand*	theCopy;

	theCopy = NULL;

	if ( m_fwd_ptr->Lookup( dbCommand, tmp ) )
	{
		theCopy = dynamic_cast<CDbCommand*>(tmp);
	}
	else
	{
		CDbTool*		dbTool;

		dbTool = dbCommand->Tool();
		if (dbTool != NULL && !dbTool->IsHidden())
		{
			CDbWorkplane*	dbWork;

			m_dst_db->Create( DBCOMMAND, (CDbEntity**) &theCopy );
			theCopy->CommonSubsetCopy( (*dbCommand) );

			dbTool = ToolCopy( dbTool );
			dbWork = WorkCopy( dbCommand->Workplane() );

			theCopy->Init( dbTool, dbWork, dbCommand->Coord(), dbCommand->Text() );

			EntityMapUpdate( dbCommand, theCopy );

			ConditionalAddToContainer( dbCommand, theCopy );
		}
	}

	return theCopy;
}

CDbProfile*
CEntityCopier::ProfileCopy( CDbProfile* dbProfile )
{
	CDbEntity*	tmp;
	CDbProfile*	theCopy;

	if ( m_fwd_ptr->Lookup( dbProfile, tmp ) )
	{
		theCopy = dynamic_cast<CDbProfile*>(tmp);
	}
	else
	{
		m_dst_db->Create( DBPROFILE, (CDbEntity**) &theCopy );
		theCopy->CommonSubsetCopy( (*dbProfile) );

		EntityMapUpdate( dbProfile, theCopy );
	
		ConditionalAddToContainer( dbProfile, theCopy );
	}

	return theCopy;
}

CDbFeature*
CEntityCopier::FeatureCopy( CDbFeature* dbFeature )
{
	CDbEntity*	tmp;
	CDbFeature*	theCopy;

	if ( m_fwd_ptr->Lookup( dbFeature, tmp ) )
	{
		theCopy = dynamic_cast<CDbFeature*>(tmp);
	}
	else
	{
		m_dst_db->Create( DBFEATURE, (CDbEntity**) &theCopy );
		theCopy->CommonSubsetCopy( (*dbFeature) );

		EntityMapUpdate( dbFeature, theCopy );

		ConditionalAddToContainer( dbFeature, theCopy );
	}

	return theCopy;
}

// Arrrgh.  Don't see anyway around this right now.  This method
// assumes the referenced entities have already been copied.
// TODO: EntityCopier should not do traversal.  That should be
// left to the sequence entity itself.
CDbSequence*
CEntityCopier::SequenceCopy( CDbSequence* dbSequence )
{
	CDbEntity*		tmp;
	CDbEntity*		tmp2;
	CDbSequence*	theCopy;
	int				count, indx;

	if ( m_fwd_ptr->Lookup( dbSequence, tmp ) )
	{
		theCopy = dynamic_cast<CDbSequence*>(tmp);
	}
	else
	{
		m_dst_db->Create( DBSEQUENCE, (CDbEntity**) &theCopy );
		theCopy->CommonSubsetCopy( (*dbSequence) );

		EntityMapUpdate( dbSequence, theCopy );

		count = dbSequence->Count();
		for (indx = 0; indx < count; ++indx)
		{
			tmp = (*dbSequence)[indx];

			if (tmp->Type() == DBSEQUENCE)
			{
				tmp2 = SequenceCopy( dynamic_cast<CDbSequence*>(tmp) );
				theCopy->Append( tmp2 );
			}
			else
			{
				if ( m_fwd_ptr->Lookup( tmp, tmp2 ) )
					theCopy->Append( tmp2 );
			}
		}
	}

	return theCopy;
}

CDbPattern*
CEntityCopier::PatternCopy( CDbPattern* dbPattern )
{
	CDbEntity*	tmp;
	CDbPattern*	theCopy;

	if ( m_fwd_ptr->Lookup( dbPattern, tmp ) )
	{
		theCopy = dynamic_cast<CDbPattern*>(tmp);
	}
	else
	{
		m_dst_db->Create( DBPATTERN, (CDbEntity**) &theCopy );
		theCopy->CommonSubsetCopy( (*dbPattern) );

		EntityMapUpdate( dbPattern, theCopy );

		ConditionalAddToContainer( dbPattern, theCopy );
	}

	return theCopy;
}

// NOTE: The visitor class knows the traversal is top-down recursive.
//       As such, an owner is instantiated before its children.
void
CEntityCopier::ConditionalAddToContainer( CDbEntity* theOriginal, CDbEntity* theCopy )
{
	CDbEntity*		tmp;
	CDbEntity*		owner;
	CDbContainer*	dbContainer;

	owner = theOriginal->Owner();
	if (owner != NULL)
	{
		if ( m_fwd_ptr->Lookup( owner, tmp ) )
		{
			dbContainer = dynamic_cast<CDbContainer*>(tmp);
			dbContainer->Append( theCopy, FALSE );
		}
	}
}

void
CEntityCopier::EntityMapUpdate( CDbEntity* theOriginal, CDbEntity* theCopy )
{
	m_fwd_ptr->SetAt( theOriginal, theCopy );

	if (m_rev_ptr != NULL)
		m_rev_ptr->SetAt( theCopy, theOriginal );
}
