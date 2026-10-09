
#include "stdafx.h"
#include "StringConst.h"
#include "Return.h"
#include "VarList.h"
#include "Shape.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "Model.h"
#include "ModelUtil.h"
#include "DbToolGen.h"



////////////////////////////////////////////////////////////////////////

CDbToolGen::CDbToolGen()
{
}

CDbToolGen::~CDbToolGen()
{
}

// Create CDbTool objects in the model based on the tooling
// information that was extracted from the machining database.
CReturn
CDbToolGen::DbToolsCreate(
					const TArrayOfAttribLists&	tools,
					CModel*						model )
{
	CReturn			status;
	CString			face;
	CDbWorkplane*	dbWork;
	CDbTool*		dbTool;
	CVarList*		toolAttribs;
	int				count, indx;

	count = tools.Count();
	for (indx = 0; indx < count; ++indx)
	{
		status = model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
		if ( !status.IsOk() )
			break;

		toolAttribs = dbTool->pAttrib();
		(*toolAttribs) = (*(tools[indx]));

		// Associate the tool with a workplane.

		face = toolAttribs->getString( STR_WORKPLANE, STR_TOP );

		status = model->EntityFind( face, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

		dbTool->Workplane( dbWork );
	}

	return status;
}

CReturn
CDbToolGen::DbToolsCreate(
					const TSortedShapes&	tooledStationShapes,
					CModel*					model )
{
	CReturn			status;
	CString			face;
	CDbWorkplane*	dbWork;
	CDbTool*		dbTool;
	CShape*			tooledStation;
	CVarList*		toolAttribs;
	int				stationID;
	int				count, indx, jndx;

	for (indx = 0; indx < SHAPE_TERMINAL; ++indx)
	{
		EShape type = (EShape) indx;

		const TShapeArray& shapes = tooledStationShapes[ type ];

		count = shapes.Count();
		for (jndx = 0; jndx < count; ++jndx)
		{
			tooledStation = shapes[ jndx ];

			stationID = tooledStation->Attribs().getInt( STR_STATION_ID, 0 );

			dbTool = CModelUtil::DbToolFind( model->Db(), STR_STATION_ID, stationID );
			if (dbTool == NULL)
			{
				status = model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
				if ( !status.IsOk() )
					break;
			}

			toolAttribs = dbTool->pAttrib();
			(*toolAttribs) = (*(tooledStation->pAttribs()));

			// Remove any attributes.
			toolAttribs->deleteVar( STR_OX );
			toolAttribs->deleteVar( STR_OY );
			toolAttribs->deleteVar( STR_CODE );
			toolAttribs->deleteVar( STR_CRADIUS );
			toolAttribs->deleteVar( STR_SHAPE );

			// Associate the tool with a workplane.

			face = toolAttribs->getString( STR_WORKPLANE, STR_TOP );

			status = model->EntityFind( face, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

			dbTool->Workplane( dbWork );
		}
	}

	return status;
}

CReturn
CDbToolGen::DbToolsUpdate(
					const TArrayOfAttribLists&	tools,
					CModel*						model )
{
	CReturn			status;
	CString			face;
	CDbWorkplane*	dbWork;
	CDbTool*		dbTool;
	CVarList*		mdbToolAttribs;
	CVar*			attrib;
	int				count, indx;
	int				attribCount, jndx;
	int				toolNo;

	count = tools.Count();

	for (indx = 0; indx < count; ++indx)
	{
		mdbToolAttribs = tools[indx];

		toolNo = mdbToolAttribs->getInt( STR_NC_CODE_NUMBER, -1 );

		if (toolNo > 0)
		{
			dbTool = CModelUtil::DbToolFind( model->Db(), STR_NC_CODE_NUMBER, toolNo );

			if (dbTool == NULL)
			{
				// Create a new tool.

				status = model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );

				if ( status.IsOk() )
				{
					// Associate the tool with a workplane.

					face = mdbToolAttribs->getString( STR_WORKPLANE, STR_TOP );

					status = model->EntityFind( face, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

					dbTool->Workplane( dbWork );
				}
			}
		}

		if ( status.IsOk() && dbTool != NULL )
		{
			// Copy/Add the attribs from the machine database to the tool.
			attribCount = mdbToolAttribs->countVar();
			for (jndx = 0; jndx < attribCount; ++jndx)
			{
				attrib = mdbToolAttribs->getVar( jndx );
				dbTool->pAttrib()->setVar( (*attrib) );
			}
			dbTool->ModifyFlag( true );

			// See also COptimizer::PrepToolOrder()
			dbTool->IntSet( "order", indx );

			// Cause the image/geometry to be recalculated.
			dbTool->PointRepClear();
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CDbToolGen::DbToolsUpdate()" );
			break;
		}
	}

	return status;
}

