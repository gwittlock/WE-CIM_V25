
#include "stdafx.h"
#include "DbEntity.h"
#include "DbTool.h"
#include "DbIterator.h"
#include "ToolSequencer.h"



////////////////////////////////////////////////////////////////////////

CToolSequencer::CToolSequencer()
{
}

CToolSequencer::~CToolSequencer()
{
}


void
CToolSequencer::ModelOrder(
						const CDbEntityArray&	dbEntityList,
						CDbEntityArray*			dbToolOrder )
{
	CDbEntity*	dbEntity;
	CDbTool*	dbTool;
	int			count, indx;

	dbToolOrder->BenignFlush();

	count = dbEntityList.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = dbEntityList[indx];
		dbTool = dbEntity->Tool();

		if ( !dbTool->IsLayer() )
			dbToolOrder->ConditionalAppend( dbTool );
	}
}

// Create a list of tools that are ordered by their order in
// the machining database.  By default, the resulting tool
// list will contain only those tools that are 'in use'.
// When 'drill drop optimization' becomes an issue, the
// tool list will also contain those tools that have the
// potential to be used.  Restricting the tool list to
// tools that are 'in use' improves downstream efficiency.
//
void
CToolSequencer::ToolSetupOrder(
						const CModel&			model,
						bool					drillsOptimize,
						CDbEntityArray*			dbToolOrder )
{
	CDbIterator iter;

	dbToolOrder->BenignFlush();

	iter.Init( model.Db(), DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if (!dbTool->IsLayer() && (dbTool->RefCnt() > 0 || drillsOptimize))
		{
			int order = dbTool->IntGet( "order", -1 );

			if (order >= 0)
			{
				// The tool order has been changed via CDbToolGen::DbToolsUpdate()

				int indx, count = dbToolOrder->Count();
				for (indx = 0; indx < count; ++indx)
				{
					CDbTool* tmpTool = dynamic_cast<CDbTool*>( (*dbToolOrder)[indx] );

					int tmpOrder = tmpTool->IntGet( "order", -1 );
					if (tmpOrder > order)
						break;
				}

				dbToolOrder->InsertBefore( indx, dbTool );
			}
			else
			{
				dbToolOrder->Append( dbTool );
			}
		}

		iter.Next();
	}
}
