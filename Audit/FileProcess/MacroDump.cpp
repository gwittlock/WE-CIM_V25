
#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"

#include "Path.h"

#include "DbAllEntities.h"
#include "DbIterator.h"
#include "Model.h"

#include "FileProcess.h"

static CDbTool*			gActiveTool = NULL;
static CDbWorkplane*	gActiveWork = NULL;

static void MacroBody( CModel& model, FILE* file );
static void MacroTool( CModel& model, FILE* file );
static void MacroWorkplane( CModel& model, FILE* file );
static void MacroPoint( CModel& model, FILE* file );
static void MacroLine( CModel& model, FILE* file );
static void MacroArc( CModel& model, FILE* file );
static void MacroHole( CModel& model, FILE* file );
static void MacroProfile( CModel& model, FILE* file );
static void MacroFeature( CModel& model, FILE* file );
static void MacroActive( CModel& model, FILE* file, CDbTool* dbTool, CDbWorkplane* dbWork );

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn
CFileProcessApp::MacroDump( CModel& model, CString filename )
{
	CReturn status;

	FILE* file = fopen( filename, "w" );

	if (file == NULL)
	{
		status.User( IDS_INTERNAL_ERROR, "CFileProcessApp::MacroDump()" );
	}
	else
	{
		CPath path( filename );

		gActiveTool = NULL;
		gActiveWork = NULL;

		fprintf( file, "\n" );
		fprintf( file, "import Bbi.System.*;\n" );
		fprintf( file, "import Bbi.Modeler.*;\n" );
		fprintf( file, "\n" );
		fprintf( file, "public class %s implements BbiMacro\n", path.FileName() );
		fprintf( file, "{\n" );
		MacroBody( model, file );
		fprintf( file, "}\n" );
		fclose( file );
	}

	return status;
}

static void MacroBody( CModel& model, FILE* file )
{
	fprintf( file, "\tpublic void main()\n" );
	fprintf( file, "\t{\n" );
	MacroTool( model, file );
	MacroWorkplane( model, file );
	MacroActive( model, file, NULL, NULL );
	MacroPoint( model, file );
	MacroLine( model, file );
	MacroArc( model, file );
	MacroHole( model, file );
	MacroProfile( model, file );
	MacroFeature( model, file );
	fprintf( file, "\t}\n" );
}

static void MacroTool( CModel& model, FILE* file )
{
	CDbIterator iter;

	iter.Init( model.Db(), DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if ( !dbTool->IsSystem() )
		{
			CString name = dbTool->Name();

			fprintf( file, "\t\tDbTool %s = new DbTool( \"%s\" );\n", name, name );
		}

		iter.Next();
	}
}

static void MacroWorkplane( CModel& model, FILE* file )
{
	CDbIterator iter;
	bool newLineReqd = TRUE;

	iter.Init( model.Db(), DBWORKPLANE );
	while (1)
	{
		CDbWorkplane* dbWorkplane = dynamic_cast<CDbWorkplane*>( iter() );
		if (dbWorkplane == NULL)
			break;

		if ( !dbWorkplane->IsSystem() )
		{
			CString name = dbWorkplane->Name();
			C3x4Matrix xform = dbWorkplane->Transform();
			int toolUp = dbWorkplane->ToolUp();

			const C3dVec& ivec = xform.getI();
			const C3dVec& jvec = xform.getJ();
			const C3dVec& kvec = xform.getK();
			const C3dCoord& orig = xform.getT();

			if ( newLineReqd )
			{
				fprintf( file, "\n" );
				newLineReqd = FALSE;
			}

			fprintf( file, "\t\tDbWorkplane %s = new DbWorkplane( \"%s\",\n" \
						   "\t\t\t%f, %f, %f,\n" \
						   "\t\t\t%f, %f, %f,\n" \
						   "\t\t\t%f, %f, %f,\n" \
						   "\t\t\t%f, %f, %f, %s );\n",
						   name, name,
						   ivec.X(), ivec.Y(), ivec.Z(),
						   jvec.X(), jvec.Y(), jvec.Z(),
						   kvec.X(), kvec.Y(), kvec.Z(),
						   orig.X(), orig.Y(), orig.Z(), (toolUp ? "true" : "false") );
		}

		iter.Next();
	}
}

static void MacroPoint( CModel& model, FILE* file )
{
	CDbIterator iter;
	bool newLineReqd = TRUE;

	iter.Init( model.Db(), DBPOINT );
	while (1)
	{
		CDbPoint* dbPoint = dynamic_cast<CDbPoint*>( iter() );
		if (dbPoint == NULL)
			break;

		if ( !dbPoint->IsSystem() )
		{
			CString name = dbPoint->Name();
			CDbTool* dbTool = dbPoint->Tool();
			CDbWorkplane* dbWork = dbPoint->Workplane();
			C3dCoord pt = dbPoint->Coord();

			if ( newLineReqd )
			{
				fprintf( file, "\n" );
				newLineReqd = FALSE;
			}

			MacroActive( model, file, dbTool, dbWork );

			fprintf( file, "\t\tDbPoint %s = new DbPoint( %f, %f, %f );\n",
						name, pt.X(), pt.Y(), pt.Z() );
		}

		iter.Next();
	}
}

static void MacroLine( CModel& model, FILE* file )
{
	CDbIterator iter;
	bool newLineReqd = TRUE;

	iter.Init( model.Db(), DBLINE );
	while (1)
	{
		CDbLine* dbLine = dynamic_cast<CDbLine*>( iter() );
		if (dbLine == NULL)
			break;

		if ( !dbLine->IsSystem() )
		{
			CString name = dbLine->Name();
			CDbTool* dbTool = dbLine->Tool();
			CDbWorkplane* dbWork = dbLine->Workplane();
			C3dCoord ps = dbLine->StartPt();
			C3dCoord pe = dbLine->EndPt();

			if ( newLineReqd )
			{
				fprintf( file, "\n" );
				newLineReqd = FALSE;
			}

			MacroActive( model, file, dbTool, dbWork );

			fprintf( file, "\t\tDbLine %s = new DbLine( %f, %f, %f, %f, %f, %f );\n",
						name, ps.X(), ps.Y(), ps.Z(), pe.X(), pe.Y(), pe.Z() );
		}

		iter.Next();
	}
}

static void MacroArc( CModel& model, FILE* file )
{
	CDbIterator iter;
	bool newLineReqd = TRUE;

	iter.Init( model.Db(), DBARC );
	while (1)
	{
		CDbArc* dbArc = dynamic_cast<CDbArc*>( iter() );
		if (dbArc == NULL)
			break;

		if ( !dbArc->IsSystem() )
		{
			CString name = dbArc->Name();
			CDbTool* dbTool = dbArc->Tool();
			CDbWorkplane* dbWork = dbArc->Workplane();
			C3dCoord ps = dbArc->StartPt();
			C3dCoord pe = dbArc->EndPt();
			C3dCoord pc = dbArc->CenterPt();

			if ( newLineReqd )
			{
				fprintf( file, "\n" );
				newLineReqd = FALSE;
			}

			MacroActive( model, file, dbTool, dbWork );

			fprintf( file, "\t\tDbArc %s = new DbArc( %f, %f, %f, %f, %f, %f, %f, %f, %f, %d );\n",
						name,
						ps.X(), ps.Y(), ps.Z(), pe.X(), pe.Y(), pe.Z(),
						pc.X(), pc.Y(), pc.Z(), dbArc->Dir() );
		}

		iter.Next();
	}
}

static void MacroHole( CModel& model, FILE* file )
{
	CDbIterator iter;
	bool newLineReqd = TRUE;

	iter.Init( model.Db(), DBHOLE );
	while (1)
	{
		CDbHole* dbHole = dynamic_cast<CDbHole*>( iter() );
		if (dbHole == NULL)
			break;

		if ( !dbHole->IsSystem() )
		{
			CString name = dbHole->Name();
			CDbTool* dbTool = dbHole->Tool();
			CDbWorkplane* dbWork = dbHole->Workplane();
			C3dCoord pc = dbHole->Center();

			if ( newLineReqd )
			{
				fprintf( file, "\n" );
				newLineReqd = FALSE;
			}

			MacroActive( model, file, dbTool, dbWork );

			fprintf( file, "\t\tDbHole %s = new DbHole( %f, %f, %f, %f, %f );\n",
						name, pc.X(), pc.Y(), pc.Z(), dbHole->Depth(), dbHole->Diam() );
		}

		iter.Next();
	}
}

static void MacroProfile( CModel& model, FILE* file )
{
	CDbIterator iter;

	iter.Init( model.Db(), DBPROFILE );
	while (1)
	{
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		if ( !dbProfile->IsSystem() )
		{
			CString name = dbProfile->Name();
			CDbTool* dbTool = dbProfile->Tool();
			CDbWorkplane* dbWork = dbProfile->Workplane();

			MacroActive( model, file, dbTool, dbWork );

			fprintf( file, "\n" );
			fprintf( file, "\t\tDbProfile %s = new DbProfile();\n", name );

			int count = dbProfile->Count();
			for (int indx = 0; indx < count; ++indx)
			{
				CDbEntity* dbEntity = (*dbProfile)[indx];

				fprintf( file, "\t\t%s.Append( %s );\n", name, dbEntity->Name() );
			}
		}

		iter.Next();
	}
}

static void MacroFeature( CModel& model, FILE* file )
{
	CDbIterator iter;

	iter.Init( model.Db(), DBFEATURE );
	while (1)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( !dbFeature->IsSystem() )
		{
			CString name = dbFeature->Name();
			CDbWorkplane* dbWork = dbFeature->Workplane();
			CDbTool* dbTool = dbFeature->Tool();

			MacroActive( model, file, dbTool, dbWork );

			fprintf( file, "\n" );
			fprintf( file, "\t\tDbFeature %s = new DbFeature( %s );\n",
						name, ((dbTool == NULL) ? "null" : dbTool->Name()) );

			int count = dbFeature->Count();
			for (int indx = 0; indx < count; ++indx)
			{
				CDbEntity* dbEntity = (*dbFeature)[indx];

				fprintf( file, "\t\t%s.Append( %s );\n", name, dbEntity->Name() );
			}
		}

		iter.Next();
	}
}

void MacroActive( CModel& model, FILE* file, CDbTool* dbTool, CDbWorkplane* dbWork )
{
	if (dbTool != gActiveTool)
	{
		gActiveTool = dbTool;

		if (gActiveTool != NULL)
			fprintf( file, "\t\tModel.ActiveToolSet( %s );\n", gActiveTool->Name() );
	}

	if (dbWork != gActiveWork)
	{
		gActiveWork = dbWork;

		if (gActiveWork != NULL)
			fprintf( file, "\t\tModel.ActiveWorkplaneSet( %s );\n", gActiveWork->Name() );
	}
}
