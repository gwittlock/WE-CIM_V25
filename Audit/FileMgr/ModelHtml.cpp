
#include "stdafx.h"
#include "MathConst.h"
#include "cmn_resource.h"
#include "StrgList.h"
#include "DbEntity.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbPoint.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbHole.h"
#include "DbContainer.h"
#include "DbProfile.h"
#include "DbIterator.h"
#include "DbCommand.h"
#include "Model.h"
#include "ModelHtml.h"



////////////////////////////////////////////////////////////////////////

CModelHtml::CModelHtml()
{
}

CModelHtml::~CModelHtml()
{
}


CReturn
CModelHtml::Dump( const CString& filepath, const CModel& model )
{
	CReturn status;
	CDbIterator iter;

	FILE* f = NULL;
	if (filepath.GetLength() > 0)
		f = fopen( filepath, "w" );

	if (f == NULL)
	{
		status.User( IDS_INTERNAL_ERROR, "CModelHtml::Dump()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Now that 'all is a go' ....

	fprintf( f, "<html>\n" );
	fprintf( f, "<head>\n" );
	fprintf( f, "</head>\n" );
	fprintf( f, "<body>\n" );

	VarListDump( f, model.Header(), "HEADER" );

	VarListDump( f, model.Default(), "DEFAULTS" );

	fprintf( f, "<br>&nbsp;" );
	fprintf( f, "<br>-------------------------------------" );
	fprintf( f, "<br>&nbsp;" );

	iter.Init( model.Db(), DBWORKPLANE );
	while (1)
	{
		const CDbEntity* dbEntity = iter();
		if (dbEntity == NULL)
			break;

		HighLevelDump( (*dbEntity), f );

		iter.Next();
	}

	iter.Init( model.Db(), DBWORKPLANE );
	while (1)
	{
		const CDbEntity* dbEntity = iter();
		if (dbEntity == NULL)
			break;

		AttributesDump( (*dbEntity), f );

		iter.Next();
	}


	iter.Init( model.Db(), DBWORKPLANE );
	while (1)
	{
		const CDbEntity* dbEntity = iter();
		if (dbEntity == NULL)
			break;

		ReferencesDump( (*dbEntity), f );

		iter.Next();
	}


	iter.Init( model.Db(), DBWORKPLANE );
	while (1)
	{
		const CDbEntity* dbEntity = iter();
		if (dbEntity == NULL)
			break;

		OwnsDump( (*dbEntity), f );

		iter.Next();
	}


	iter.Init( model.Db(), DBWORKPLANE );
	while (1)
	{
		const CDbEntity* dbEntity = iter();
		if (dbEntity == NULL)
			break;

		CanonicalDump( (*dbEntity), f );

		iter.Next();
	}

	fprintf( f, "</body>\n" );
	fprintf( f, "</html>\n" );

	fclose( f );

	return status;
}

void
CModelHtml::VarListDump( 
	FILE*	f,
	const CVarList& varlist,
	const CString& title )
{
	fprintf( f, "<br>------------------- %s ------------------", title );
	fprintf( f, "<br>&nbsp;" );

	int num=varlist.countVar();
	for (int idx=0; idx<num; idx++)
	{
		CVar*	var = varlist.getVar(idx);

		fprintf( f, "<br>    %s = %s", var->getName(), var->getString() );
	}

	fprintf( f, "<br>&nbsp;" );
}

void
CModelHtml::HighLevelDump( const CDbEntity& dbEntity, FILE* f )
{
	CDbLayer* dbLayer = dbEntity.Layer();
	CDbWorkplane* dbWork = dbEntity.Workplane();
	CDbTool* dbTool = dbEntity.Tool();
	CString desc = EntityDescription( dbEntity );

	// Put the target name for this entity.
	if (dbEntity.Name().GetLength() > 0)
	{
		fprintf( f, "\n<br><a NAME=\"e%d\"></a> %s, name=\"%s\", refcnt=%d",
				dbEntity.Id(), desc, dbEntity.Name(), dbEntity.RefCnt() );
	}
	else
	{
		fprintf( f, "\n<br><a NAME=\"e%d\"></a> %s, refcnt=%d",
				dbEntity.Id(), desc, dbEntity.RefCnt() );
	}

	// Put the links to the details.

	if (dbLayer != NULL)
		fprintf( f, ", <a href=\"#e%d\">layer</a>", dbLayer->Id() );

	if (dbTool != NULL)
		fprintf( f, ", <a href=\"#e%d\">tool</a>", dbTool->Id() );

	if (dbWork != NULL)
		fprintf( f, ", <a href=\"#e%d\">work</a>", dbWork->Id() );

	fprintf( f, ", <a href=\"#a%d\">attrib</a>", dbEntity.Id() );

	if ( CanReference( dbEntity ) )
		fprintf( f, ", <a href=\"#r%d\">refs</a>", dbEntity.Id() );

	if ( IsContainer( dbEntity ) )
		fprintf( f, ", <a href=\"#o%d\">owns</a>", dbEntity.Id() );
	
	if ( HasCanonicalData( dbEntity ) )
		fprintf( f, ", <a href=\"#c%d\">canon</a>", dbEntity.Id() );
}

void
CModelHtml::CanonicalDump( const CDbEntity& dbEntity, FILE* f )
{
	if ( HasCanonicalData( dbEntity ) )
	{
		CString desc = EntityDescription( dbEntity );

		// Put the target for the details.
		fprintf( f, "\n<br><a NAME=\"c%d\"></a>", dbEntity.Id() );

		// Put a link back to the entity.
		fprintf( f, "<a href=\"#e%d\">%s</a>, canon=[", dbEntity.Id(), desc );

		switch (dbEntity.Type())
		{
		case DBWORKPLANE:
			break;

		case DBTOOL:
			break;

		case DBPOINT:
			{
			C3dCoord pt = ((const CDbPoint&) dbEntity).Coord();
			fprintf( f, "x=%-11.6f, y=%-11.6f, z=%-11.6f", pt.X(), pt.Y(), pt.Z() );
			}
			break;

		case DBLINE:
			{
			const CDbLine& dbLine = (const CDbLine&) dbEntity;
			double len = 0.0;
			if ( !dbLine.IsDeleted() )
			{
				C3dVec vec = dbLine.EndPt() - dbLine.StartPt();
				len = vec.Length();
			}
			fprintf( f, "3dLen=%-11.6f", len );
			}
			break;

		case DBARC:
			{
			const CDbArc& dbArc = (const CDbArc&) dbEntity;
			double radius = (dbArc.IsDeleted() ? 0.0 : dbArc.Radius());
			fprintf( f, "rad=%-11.6f, dir=%d", radius, dbArc.Dir() );
			}
			break;

		case DBHOLE:
			{
			const CDbHole& dbHole = (const CDbHole&) dbEntity;
			C3dCoord pt = dbHole.Coord();
			fprintf( f, "x=%-11.6f, y=%-11.6f, z=%-11.6f, diam=%-11.6f, depth=%-11.6f",
					 pt.X(), pt.Y(), pt.Z(), dbHole.Diam(), dbHole.Depth() );
			}
			break;

		case DBPROFILE:
			{
			const CDbProfile& dbProfile = (const CDbProfile&) dbEntity;
			fprintf( f, "assoc=%d", (dbProfile.IsAssociated() ? 1 : 0) );
			}
			break;

		case DBCOMMAND:
			{
			const CDbCommand& dbCmd = (const CDbCommand&) dbEntity;
			C3dCoord pt = dbCmd.Coord();
			fprintf( f, "x=%-11.6f, y=%-11.6f, z=%-11.6f, '%s'", pt.X(), pt.Y(), pt.Z(), dbCmd.Text()  );
			}
			break;

		case DBFEATURE:
		case DBPATTERN:
			break;

		case DBSEQUENCE:
			break;

		default:
			ASSERT( FALSE );  // Invalid case.
		}

		fprintf( f, "]" );
	}
}

void
CModelHtml::AttributesDump( const CDbEntity& dbEntity, FILE* f )
{
	CString desc = EntityDescription( dbEntity );

	// Put the target for the details.
	fprintf( f, "\n<br><a NAME=\"a%d\"></a>", dbEntity.Id() );

	// Put a link back to the entity.
	fprintf( f, "<a href=\"#e%d\">%s</a>, attrib=[", dbEntity.Id(), desc );

	const CVarList& attribs = dbEntity.Attrib();
	int count = attribs.countVar();

	for (int indx = 0; indx < count; ++indx)
	{
		const CVar* var = attribs.getVar( indx );
		const CString& name = var->getName();
		CString value = var->getString();
		char type;

		switch (var->getType())
		{
		case VAR_INT: type = 'i'; break;
		case VAR_REAL: type = 'r'; break;
		case VAR_STRING: type = 's'; break;
		default: type = 'u'; break;
		}

		fprintf( f, "(%c)%s=\"%s\",", type, name, value );
	}

	fprintf( f, "]" );
}

void
CModelHtml::ReferencesDump( const CDbEntity& dbEntity, FILE* f )
{
	if ( CanReference( dbEntity ) )
	{
		CString desc = EntityDescription( dbEntity );
		
		// Put the target for the references.
		fprintf( f, "\n<br><a NAME=\"r%d\"></a>", dbEntity.Id() );

		// Put a link back to the entity.
		fprintf( f, "<a href=\"#e%d\">%s</a>, refs=[", dbEntity.Id(), desc );

		CDbEntityList refs;
		dbEntity.RefsTo( &refs );

		int count = refs.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* tmp = refs[indx];
			fprintf( f, "<a href=\"#e%d\">%d</a>,", tmp->Id(), tmp->Id() );
		}

		fprintf( f, "]" );
	}
}

void
CModelHtml::OwnsDump( const CDbEntity& dbEntity, FILE* f )
{
	const CDbContainer* dbContainer = dynamic_cast<const CDbContainer*>( &dbEntity );
	if (dbContainer != NULL)
	{
		CString desc = EntityDescription( dbEntity );
		
		// Put the target for the ownership list.
		fprintf( f, "\n<br><a NAME=\"o%d\"></a>", dbEntity.Id() );

		// Put a link back to the entity.
		fprintf( f, "<a href=\"#e%d\">%s</a>, owns=[", dbEntity.Id(), desc );

		int count = dbContainer->Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* tmp = (*dbContainer)[indx];
			fprintf( f, ",<a href=\"#e%d\">%d</a>", tmp->Id(), tmp->Id() );
		}

		fprintf( f, "]" );
	}
}

CString
CModelHtml::EntityDescription( const CDbEntity& dbEntity )
{
	CString desc;
	CString name;

	switch (dbEntity.Type())
	{
	case DBWORKPLANE:	name.Format(  "Workplane: id=%d", dbEntity.Id() );	break;
	case DBTOOL:		name.Format(  "Tool: id=%d", dbEntity.Id() );		break;
	case DBPOINT:		name.Format(  "Point: id=%d", dbEntity.Id() );		break;
	case DBLINE:		name.Format(  "Line: id=%d", dbEntity.Id() );		break;
	case DBARC:			name.Format(  "Arc: id=%d", dbEntity.Id() );		break;
	case DBHOLE:		name.Format(  "Hole: id=%d", dbEntity.Id() );		break;
	case DBPROFILE:		name.Format(  "Profile: id=%d", dbEntity.Id() );	break;
	case DBCOMMAND:		name.Format(  "Command: id=%d", dbEntity.Id() );	break;
	case DBFEATURE:		name.Format(  "Feature: id=%d", dbEntity.Id() );	break;
	case DBSEQUENCE:	name.Format(  "Sequence: id=%d", dbEntity.Id() );	break;
	case DBPATTERN:		name.Format(  "Pattern: id=%d", dbEntity.Id() );	break;
	default:			name.Format(  "UNKNOWN: id=%d", dbEntity.Id() );	break;
	}

	if ( dbEntity.IsDeleted() )
		desc.Format( "<font color=\"#FF0000\">%s</font>", name );
	else
		desc = name;

	return desc;
}

BOOL
CModelHtml::CanReference( const CDbEntity& dbEntity )
{
	switch (dbEntity.Type())
	{
	case DBWORKPLANE:	return FALSE;	break;
	case DBTOOL:		return FALSE;	break;
	case DBPOINT:		return FALSE;	break;
	case DBLINE:		return TRUE;	break;
	case DBARC:			return TRUE;	break;
	case DBHOLE:		return FALSE;	break;
	case DBPROFILE:		return TRUE;	break;
	case DBCOMMAND:		return FALSE;	break;
	case DBFEATURE:		return TRUE;	break;
	case DBSEQUENCE:	return TRUE;	break;
	case DBPATTERN:		return TRUE;	break;
	default:			return FALSE;	break;
	}
}

BOOL
CModelHtml::IsContainer( const CDbEntity& dbEntity )
{
	const CDbContainer* dbContainer = dynamic_cast<const CDbContainer*>( &dbEntity );
	return (dbContainer != NULL);
}

BOOL
CModelHtml::HasCanonicalData( const CDbEntity& dbEntity )
{
	switch (dbEntity.Type())
	{
	case DBPOINT:
	case DBLINE:
	case DBARC:
	case DBHOLE:
	case DBPROFILE:
	case DBCOMMAND:
		return TRUE;
	default:
		return FALSE;
	}
}


