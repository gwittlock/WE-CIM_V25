
#include "stdafx.h"
#include "cmn_resource.h"
#include "StringConst.h"
#include "3dVec.h"
#include "DbAllEntities.h"
#include "DbIterator.h"
#include "Model.h"
#include "Mm2.h"
#include "AppConst.h"

////////////////////////////////////////////////////////////////////////

CReturn
CMM2::ReadFile0_001( CBinaryFile& file )
{
	CReturn	status;
	bool	skipSeqObjs;
	bool	toolsSorted;

	skipSeqObjs = (m_flags & MM2_PREVIEW_MODE) ||
				  ((m_flags & MM2_SKIP_SEQ_OBJS) && !(m_flags & MM2_KEEP_SEQ_ORDER));

	toolsSorted = FALSE;

	m_featureId.SetSize( 0, 500 );
	m_patternId.SetSize( 0, 500 );
	m_sequenceId.SetSize( 0, 500 );

	//
	// Model header and defaults...
	//
	ReadAttrib0_001( file, m_model->pHeader(), FALSE );
	ReadAttrib0_001( file, m_model->pDefault(), FALSE );

	//
	// Database...
	//
	int entityCount;
	file.Read( (BYTE*) &entityCount, sizeof(entityCount) );
	for ( int indx = 0; indx < entityCount; ++indx)
	{
		EDbEntityType type;
		CDbEntity* dbEntity;

		file.Read( (BYTE*) &type, sizeof(type) );

		if (IsPreVersion16() && type == DBPATTERN)
		{
			// In V15 files, the enumerated value for what is now
			// DBPATTERN used to be the value for DBSEQUENCE.
			// See also Db\DbConsts.h
			type = DBSEQUENCE;
		}

		// We can do an early exit during
		// file preview and nesting operations.
		if ( skipSeqObjs && (type == DBSEQUENCE) )
			break;

		status = m_model->EntityCreate( type, (CDbEntity**) &dbEntity );

		if ( !status.IsOk() )
			break;

		if ( IsPreVersion16() && type > DBTOOL && !toolsSorted )
		{
			// With V16, layers are implemented using CDbTool.
			// As such, the entity IDs as read from the MM2 file
			// are not likely to be arranged in increasing order.
			ToolsRepair();
			toolsSorted = TRUE;
		}

		status = ReadEntity0_001( file, dbEntity );

		if ( !status.IsOk() )
		{
			dbEntity->Delete();
			break;
		}
	}

	if ( status.IsOk() && !(m_flags == MM2_PREVIEW_MODE) )
		status = ToolsResolve();

	if ( status.IsOk() )
		status = FeaturesResolve();

	if ( status.IsOk() )
		status = PatternsResolve();

	if ( status.IsOk() && !skipSeqObjs )
		status = SequencesResolve();

	if ( status.IsOk() && IsPreVersion16() )
	{
		NestingDecorationsMigrate();
	}

	return status;
}

// ==================================================================

CReturn
CMM2::ReadAttrib0_001( CBinaryFile& file, CVarList* varlist, bool replaceWhiteSpace )
{
	int			num;
	int			idx;
	eVarType	type;
	int			len;
	CString		str;
	CString		name;
	char*		buf;

	varlist->Reset();

	file.Read( (BYTE*) &num, sizeof(num) );

	for (idx=0; idx<num; idx++)
	{
		file.Read( (BYTE*) &type, sizeof(type) );

		file.Read( (BYTE*) &len, sizeof(len) );
		buf = new char[len];
		file.Read( (BYTE*)buf, len );
		name = buf;
		delete buf;

		if ( replaceWhiteSpace )
			name.Replace( ' ', '_' );

		switch (type)
		{
			case VAR_INT:
				{
					int	ival;

					file.Read( (BYTE*)&ival, sizeof(ival) );
					varlist->newInt( name, ival );
				}
				break;
			case VAR_REAL:
				{
					double	rval;
					file.Read( (BYTE*)&rval, sizeof(rval) );
					varlist->newReal( name, rval );
				}
				break;
			case VAR_STRING:
				{
					file.Read( (BYTE*) &len, sizeof(len) );
					buf = new char[len];
					file.Read( (BYTE*)buf, len );
					str = buf;
					varlist->newString( name, str );

					delete buf;
				}
				break;
		}
	}

	// TODO:  real error checking
	return CReturn( STATUS_OKAY );
}


CReturn
CMM2::ReadEntity0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
// TODO: spead up...??
	CReturn status = ReadCommon0_001( file, dbEntity );

	if ( !status.IsOk() )
		return status;


	switch ( dbEntity->Type() )
	{
	case DBWORKPLANE:
		status = ReadWorkplane0_001( file, dbEntity );
		if (status.IsOk() && m_model->ActiveWorkplane() == NULL)
			m_model->ActiveWorkplane( (CDbWorkplane*) dbEntity );
		break;

	case DBTOOL:
		status = ReadTool0_001( file, dbEntity );
		break;

	case DBPOINT:
		status = ReadPoint0_001( file, dbEntity );
		break;

	case DBLINE:
		status = ReadLine0_001( file, dbEntity );
		break;

	case DBARC:
		status = ReadArc0_001( file, dbEntity );
		break;

	case DBHOLE:
		status = ReadHole0_001( file, dbEntity );
		break;

	case DBPROFILE:
		status = ReadProfile0_001( file, dbEntity );
		break;

	case DBCOMMAND:
		status = ReadCommand0_001( file, dbEntity );
		break;

	case DBFEATURE:
		status = ReadFeature0_001( file, dbEntity );
		break;

	case DBPATTERN:
		status = ReadPattern0_001( file, dbEntity );
		break;

	case DBSEQUENCE:
		status = ReadSequence0_001( file, dbEntity );
		break;
	}

	return status;
}

CReturn
CMM2::ReadCommon0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;
	ID entityID, layerID, workID, ToolID;
	FLAGS flags;

	CDbWorkplane* dbWork = NULL;
	CDbTool* dbTool = NULL;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=
	// Get the data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=

	file.Read( (BYTE*) &entityID, sizeof(entityID) );
	if ( IsValidVersion() )
	{
		int len;
		file.Read( (BYTE*) &len, sizeof(len) );
		if (len > 0)
		{
			// Read the user-defined name.
			char* buf = new char[len];
			file.Read( (BYTE*)buf, len );
			dbEntity->SystemName( buf );
			delete buf;
		}
	}

	file.Read( (BYTE*) &flags, sizeof(flags) );

	if ( IsPreVersion16() )
	{
		file.Read( (BYTE*) &layerID, sizeof(layerID) );
		file.Read( (BYTE*) &workID, sizeof(workID) );
		file.Read( (BYTE*) &ToolID, sizeof(ToolID) );

		if (ToolID == 0)
			ToolID = layerID;  // tools supercede layers
	}
	else
	{
		file.Read( (BYTE*) &workID, sizeof(workID) );
		file.Read( (BYTE*) &ToolID, sizeof(ToolID) );
	}

	if (workID > 0)
		m_model->EntityFind( workID, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	if (ToolID > 0)
		m_model->EntityFind( ToolID, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	dbEntity->Tool( dbTool );
	dbEntity->Workplane( dbWork );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=
	// Get the attributes attached to this entity.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=
	CVarList* varlist = dbEntity->pAttrib();

	ReadAttrib0_001( file, varlist, (dbEntity->Type() == DBTOOL) );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=
	// Initialize the entity.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=

	dbEntity->Id( entityID );
	dbEntity->Flags( flags );

	if (dbEntity->Type() == DBTOOL)
	{
		// By default, all entities support snap & hotdot view behavior.
		dbEntity->SnappableFlag( true );
		dbEntity->HotDotFlag( true );
	}

	return status;
}

CReturn
CMM2::ReadWorkplane0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn	status;
	double	x, y, z;
	int		up;

	CDbWorkplane* dbWorkplane = dynamic_cast<CDbWorkplane*>( dbEntity );

	if (&dbWorkplane == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadWorkplane" );
	}


	ReadTriple( file, &x, &y, &z );
	C3dVec ivec( x, y ,z );

	ReadTriple( file, &x, &y, &z );
	C3dVec jvec( x, y ,z );

	ReadTriple( file, &x, &y, &z );
	C3dVec kvec( x, y ,z );

	ReadTriple( file, &x, &y, &z );
	C3dCoord origin( x, y ,z );

	file.Read( (BYTE*) &up, sizeof(up) );

	dbWorkplane->Init( ivec, jvec, kvec, origin, up );

	return status;
}

CReturn
CMM2::ReadTool0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;

	if ( IsPreVersion16() )
	{
		// Nothing to do.  At first implementation, all tooling
		// information is stored as attributes on the tool.
	}
	else
	{
		// Still nothing to do.
	}

	// BugID: 711 -- Hidden layers/tools are not appearing in layer/tool dialogs.
	dbEntity->Seek();

	// 2012.02.05 (PE) -- Model migration. Prior to this date,
	// snap/hotdot behavior was determined by the model name.
	if (dbEntity->Name().CompareNoCase( STR_STOCK ) == 0)
		dbEntity->SnappableFlag( false );

	return status;
}

CReturn
CMM2::ReadPoint0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;

	CDbPoint* dbPoint = dynamic_cast<CDbPoint*>( dbEntity );

	if (dbPoint == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadPoint0_001" );
	}

	if ( status.IsOk() )
	{
		double x, y, z;

		status = ReadTriple( file, &x, &y, &z );

		dbPoint->Init( x, y, z );
	}

	return status;
}

CReturn
CMM2::ReadLine0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;

	CDbLine* dbLine = dynamic_cast<CDbLine*>( dbEntity );

	if (dbLine == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadLine0_001" );
	}

	if ( status.IsOk() )
	{
		ID psid, peid;
		int count;

		file.Read( (BYTE*) &count, sizeof(count) );

		file.Read( (BYTE*) &psid, sizeof(psid) );
		file.Read( (BYTE*) &peid, sizeof(peid) );

		status = dbLine->Init( psid, peid );
		//
		// We REQUIRE a tool on geometry, so force it on our points.
		//
		dbLine->DbStartPt()->Tool( dbLine->Tool() );
		dbLine->DbEndPt()->Tool( dbLine->Tool() );
	}

	return status;
}

CReturn
CMM2::ReadArc0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;

	CDbArc* dbArc = dynamic_cast<CDbArc*>( dbEntity );

	if (dbArc == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadArc0_001" );
	}

	if ( status.IsOk() )
	{
		ID psid, peid, pcid;
		int dir;
		int count;

		file.Read( (BYTE*) &count, sizeof(count) );

		file.Read( (BYTE*) &psid, sizeof(psid) );
		file.Read( (BYTE*) &peid, sizeof(peid) );
		file.Read( (BYTE*) &pcid, sizeof(pcid) );
		file.Read( (BYTE*) &dir, sizeof(dir) );

		status = dbArc->Init( psid, peid, pcid, dir );
		//
		// We REQUIRE a tool on geometry, so force it on our points.
		//
		dbArc->DbStartPt()->Tool( dbArc->Tool() );
		dbArc->DbEndPt()->Tool( dbArc->Tool() );
		dbArc->DbCenterPt()->Tool( dbArc->Tool() );
	}

	return status;
}

CReturn
CMM2::ReadHole0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;

	CDbHole* dbHole = dynamic_cast<CDbHole*>( dbEntity );

	if (dbHole == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadHole0_001" );
	}

	double diam, depth, x, y, z;

	file.Read( (BYTE*) &diam, sizeof(diam) );
	file.Read( (BYTE*) &depth, sizeof(depth) );

	status = ReadTriple( file, &x, &y, &z );

	if ( status.IsOk() )
	{
		CDbTool* dbTool = dbHole->Tool();
		CDbWorkplane* dbWorkplane = dbHole->Workplane();

		C3dCoord center( x, y, z );
		dbHole->Init( dbTool, dbWorkplane, center, diam, depth );
	}

	return status;
}

CReturn
CMM2::ReadProfile0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;

	CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

	if (dbProfile == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadProfile0_001" );
	}

	if ( status.IsOk() )
	{
		int count, isAssociated;

		file.Read( (BYTE*) &isAssociated, sizeof(isAssociated) );

		dbProfile->m_associated = ((isAssociated) ? TRUE : FALSE);

		file.Read( (BYTE*) &count, sizeof(count) );

		for (int indx = 0; indx < count; ++indx)
		{
			ID id;

			file.Read( (BYTE*) &id, sizeof(id) );

			CDbEntity* dbEntity;
			m_model->EntityFind( id, &dbEntity, DBLINE, DBARC );

			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
			if (dbCurve == NULL)
			{
				status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadProfile0_001" );
			}
			else
			{
				status = dbProfile->Append( dbCurve );
			}

			if ( !status.IsOk() )
				break;
		}
	}

	return status;
}

// See also CMM2:FeaturesResolve();
CReturn
CMM2::ReadFeature0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn		status;
	CString		name;
	CDbEntity*	refdEntity;
	ID			id;
	int			count, indx;
	boolean		okay;

	CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( dbEntity );

	if (dbFeature == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadFeature0_001" );
	}

	okay = (((m_flags & MM2_SKIP_WORK_ZONES) && dbFeature->IsWorkZone()) ? false : true);

	if ( IsPreVersion16() )
	{
		name = dbFeature->Name();
		
		name.MakeLower();

		if (name.Left(8).CompareNoCase("_lead_in") == 0)
		{
			dbFeature->Name("");
			dbFeature->StringSet( STR_TYPE, "_lead_in" );
		}
		else if (name.Left(9).CompareNoCase("_lead_out") == 0)
		{
			dbFeature->Name("");
			dbFeature->StringSet( STR_TYPE, "_lead_out" );
		}
	}

	if ( status.IsOk() )
	{
		// Link to all of the reference entities.

		file.Read( (BYTE*) &count, sizeof(count) );

		for (indx = 0; indx < count; ++indx)
		{
			file.Read( (BYTE*) &id, sizeof(id) );

			if ( okay )
			{
				m_model->EntityFind( id, &refdEntity );

				if (refdEntity == NULL)
				{
					status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadFeature0_001" );
				}
				else
				{
					dbFeature->AddRef( refdEntity );
				}
			}
		}
	}

	if ( status.IsOk() )
	{
		// Build a temporary list of contained entity ids to
		// accomodate nested features.  Resolution between
		// features and their contained entities is deferred to
		// FeaturesResolve() until after the file has been read.

		if ( okay )
		{
			m_featureId.Add( 0 );
			m_featureId.Add( dbEntity->Id() );
		}

		file.Read( (BYTE*) &count, sizeof(count) );

		for (indx = 0; indx < count; ++indx)
		{
			file.Read( (BYTE*) &id, sizeof(id) );

			if ( okay )
			{
				m_featureId.Add( id );
			}
		}
	}

	if ( !okay )
	{
		dbFeature->Delete();
	}

	return status;
}

CReturn
CMM2::ReadPattern0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;

	CDbPattern* dbPattern= dynamic_cast<CDbPattern*>( dbEntity );

	if (dbPattern == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadPattern0_001" );
	}

	if ( status.IsOk() )
	{
		// Build a temporary list of contained entity ids to
		// accomodate patterns.  Resolution between
		// patterns and their contained entities is deferred to
		// PatternsResolve() until after the file has been read.

		m_patternId.Add( 0 );
		m_patternId.Add( dbEntity->Id() );

		int count;
		file.Read( (BYTE*) &count, sizeof(count) );

		for (int indx = 0; indx < count; ++indx)
		{
			ID id;

			file.Read( (BYTE*) &id, sizeof(id) );
			m_patternId.Add( id );
		}
	}

	return status;
}


CReturn
CMM2::ReadSequence0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn			status;
	CDbSequence*	dbSequence;
	int				count, indx;
	ID				id;
	
	dbSequence = dynamic_cast<CDbSequence*>( dbEntity );

	if (dbSequence == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadSequence0_001" );
	}

	if ( status.IsOk() )
	{
		// Build a temporary list of contained entity ids to
		// accomodate nested sequences.  Resolution between
		// sequences and their contained entities is deferred to
		// SequencesResolve() until after the file has been read.

		m_sequenceId.Add( 0 );
		m_sequenceId.Add( dbEntity->Id() );

		file.Read( (BYTE*) &count, sizeof(count) );

		for (indx = 0; indx < count; ++indx)
		{
			file.Read( (BYTE*) &id, sizeof(id) );
			m_sequenceId.Add( id );
		}
	}

	return status;
}

CReturn
CMM2::ReadCommand0_001( CBinaryFile& file, CDbEntity* dbEntity )
{
	CReturn status;

	CDbCommand* dbCommand = dynamic_cast<CDbCommand*>( dbEntity );

	if (dbCommand == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, "CMM2::ReadCommand0_001" );
	}

	if ( status.IsOk() )
	{
		double x, y, z;

		status = ReadTriple( file, &x, &y, &z );

		CString text;
		int len;
		file.Read( (BYTE*) &len, sizeof(len) );
		if (len > 0)
		{
			// Read the user-defined name.
			char* buf = new char[len];
			file.Read( (BYTE*)buf, len );
			text = buf;
			delete buf;
		}


		CDbTool* dbTool = dbCommand->Tool();
		CDbWorkplane* dbWorkplane = dbCommand->Workplane();

		dbCommand->Init( dbTool, dbWorkplane, x, y, z, text );
	}

	return status;
}

CReturn
CMM2::ReadTriple( CBinaryFile& file, double* x, double* y, double* z )
{
	CReturn status;

	file.Read( (BYTE*) x, sizeof(double) );
	file.Read( (BYTE*) y, sizeof(double) );
	file.Read( (BYTE*) z, sizeof(double) );
	//
	// Slam unknown Z.  Could also be used to slam other unknowns.
	//
	if (!DEFINED(fabs(*z)))
	{ *z = 0.0; }

	return status;
}


// See also CMM2::ReadFeature...
CReturn
CMM2::FeaturesResolve()
{
	CReturn status;

	CDbEntity* dbEntity;
	CDbFeature* dbFeature;

	int indx = 0;
	int count = m_featureId.GetSize();

	while (indx < count)
	{
		ID id = m_featureId[indx];
		if (id == 0)
		{
			// An ID of zero indicates the next entity is a feature.

			++indx;
			id = m_featureId[indx];

			m_model->EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
			if (dbFeature == NULL)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CMM2::FeaturesResolve()" );
			}
		}
		else
		{
			// We have a contained entity.
			// ASSUMPTION: A feature does not contain layers, workplanes or points.

			m_model->EntityFind( id, &dbEntity, DBLINE, DBFEATURE );

			if (dbEntity == NULL)
				status.Internal( IDS_INTERNAL_ERROR, "CMM2::FeaturesResolve()" );
			else
				status = dbFeature->Append( dbEntity );
		}

		if ( !status.IsOk() )
			break;

		++indx;
	}

	return status;
}

CReturn
CMM2::PatternsResolve()
{
	CReturn status;

	CDbEntity* dbEntity;
	CDbPattern* dbPattern;

	int indx = 0;
	int count = m_patternId.GetSize();

	while (indx < count)
	{
		ID id = m_patternId[indx];
		if (id == 0)
		{
			// An ID of zero indicates the next entity is a pattern.

			++indx;
			id = m_patternId[indx];

			m_model->EntityFind( id, (CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );
			if (dbPattern == NULL)
				status.Internal( IDS_INTERNAL_ERROR, "CMM2::PatternsResolve()" );
		}
		else
		{
			// We have a contained entity.
			// ASSUMPTION: A pattern does not contain layers, workplanes or points.

			m_model->EntityFind( id, &dbEntity, DBLINE, DBFEATURE );
			if (dbEntity == NULL)
				status.Internal( IDS_INTERNAL_ERROR, "CMM2::PatternsResolve()" );
			else
				status = dbPattern->Append( dbEntity );
		}

		if ( !status.IsOk() )
			break;

		++indx;
	}

	return status;
}


// See also CMM2::ReadSequence...
CReturn
CMM2::SequencesResolve()
{
	CReturn			status;
	CDbEntity*		dbEntity;
	CDbSequence*	dbSequence;
	int				indx, count;
	ID				id;

	bool			preserveCutOrder;
	int				expseq;

	// In the case of Modular Services, they sequence the cut order
	// of each part such that the torch does not cross over an area
	// that has already been cut.  We need to preserve this cutting
	// order with the part (aka pattern) after it is nested.  This
	// cut order can then be output in a subroutine during code
	// generation.
	//
	preserveCutOrder = ((m_flags & MM2_SKIP_SEQ_OBJS) && (m_flags & MM2_KEEP_SEQ_ORDER));
	expseq = 0;

	indx = 0;
	count = m_sequenceId.GetSize();

	while (indx < count)
	{
		id = m_sequenceId[indx];
		if (id == 0)
		{
			// An ID of zero indicates the next entity is a sequence.

			++indx;
			id = m_sequenceId[indx];

			m_model->EntityFind( id, (CDbEntity**) &dbSequence, DBSEQUENCE, DBSEQUENCE );
			if (dbSequence == NULL)
				status.Internal( IDS_INTERNAL_ERROR, "CMM2::SequencesResolve()" );
		}
		else
		{
			// We have a contained entity.

			m_model->EntityFind( id, &dbEntity, DBLINE, DBARC );
			if (dbEntity == NULL)
				m_model->EntityFind( id, &dbEntity, DBHOLE, DBHOLE );
			if (dbEntity == NULL)
				m_model->EntityFind( id, &dbEntity, DBCOMMAND, DBCOMMAND );
			if (dbEntity == NULL)
				m_model->EntityFind( id, &dbEntity, DBFEATURE, DBFEATURE );
			if (dbEntity == NULL)
				m_model->EntityFind( id, &dbEntity, DBSEQUENCE, DBSEQUENCE );

			if (dbEntity == NULL)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CMM2::SequencesResolve()" );
			}
			else
			{
				if ( preserveCutOrder )
				{
					++expseq;
					dbEntity->IntSet( "_expseq", expseq );
				}
				else
				{
					dbSequence->Append( dbEntity );
				}
			}
		}

		if ( !status.IsOk() )
			break;

		++indx;
	}

	if ( preserveCutOrder )
	{
		CDbIterator	iter;

		iter.Init( m_model->Db(), DBSEQUENCE );
		while (1)
		{
			dbSequence = dynamic_cast<CDbSequence*>( iter() );
			if (dbSequence == NULL)
				break;

			dbSequence->Delete();

			iter.Next();
		}
	}

	return status;
}

// Introduced during resolution of BugID: 156
// It was discovered that files created by nesting had tools whose
// workplanes were undefined.  ToolsResolve() addresses that issue.
//
CReturn
CMM2::ToolsResolve()
{
	CReturn status;

	CDbIterator iter;
	CDbTool* dbTool;
	CDbWorkplane* dbWork;
	CString face;

	iter.Init( m_model->Db(), DBTOOL );
	while (1)
	{
		dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		dbWork = dbTool->Workplane();
		if (dbWork == NULL)
		{
			face = dbTool->StringGet( STR_WORKPLANE, STR_TOP );
			status += m_model->EntityFind( face, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
			dbTool->Workplane( dbWork );
		}

		iter.Next();
	}

	return status;
}

// NOTE: This method is only used for pre-version 16 files.
void
CMM2::ToolsRepair()
{
	CDbIterator	iter;
	CString		name;
	CDbTool*	dbTool;

	m_model->Db().SortByID( DBTOOL );

	iter.Init( m_model->Db(), DBTOOL );
	while (1)
	{
		dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if ( dbTool->IsLayer() )
		{
			// The tool either has a user-defined name (eg."Layer_1")
			// or it is an obsolete "tool layer" (eg."tool101").
			name = dbTool->Name().Left(4);
			if (name.CompareNoCase("tool") == 0)
				dbTool->Delete();
		}
		else
		{
			// Otherwise we must name the properly name the tool (eg."tool203").
	//		toolNo = dbTool->IntGet( STR_NC_CODE_NUMBER, 0 );
	//		name.Format( "tool%d", toolNo );
	//		dbTool->Name( name );
		}

		iter.Next();
	}
}

void
CMM2::NestingDecorationsMigrate()
{
	CDbIterator	iter;
	CString		strg;
	CDbFeature*	zone;
	CDbFeature*	dbFeature;
	CDbProfile*	dbProfile;
	CDbCommand*	dbCommand;

	zone = NULL;

	// Migrate the drop/stop attributes to the geometry.
	iter.Init( m_model->Db(), DBPROFILE );
	while (1)
	{
		dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		iter.Next();

		strg = dbProfile->StringGet( "@DROPSTOP", "" );
		if ( !strg.IsEmpty() )
			DropStopMigrate( dbProfile );
	}

	iter.Init( m_model->Db(), DBFEATURE );
	while (1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		iter.Next();

		strg = dbFeature->StringGet( "@DROPSTOP", "" );
		if ( !strg.IsEmpty() )
			DropStopMigrate( dbFeature );
	}

	// Migrate the clamps from dbCommands to attributes on a feature.
	iter.Init( m_model->Db(), DBCOMMAND );
	while (1)
	{
		dbCommand = dynamic_cast<CDbCommand*>( iter() );
		if (dbCommand == NULL)
			break;

		iter.Next();

		if ( dbCommand->IsA("CLAMP") )
		{
			if (zone == NULL)
				zone = FeaturesMigrate();

			ClampMigrate( dbCommand );
		}
		else if ( dbCommand->IsA("HOLD") )
		{
			if (zone == NULL)
				zone = FeaturesMigrate();

			HoldMigrate( dbCommand );
		}
		else if ( dbCommand->IsA("ZONE") )
		{
			zone = ZoneMigrate( dbCommand );
		}
	}
}

// Wrap everything in a repo zone (if there is none).
CDbFeature*
CMM2::FeaturesMigrate()
{
	CDbIterator	iter;
	CDbFeature*	zone;
	CDbFeature* dbFeature;

	m_model->EntityFind( "_repo_zone_1", (CDbEntity**) &zone, DBFEATURE, DBFEATURE );
	if (zone == NULL)
	{
		m_model->EntityCreate( DBFEATURE, (CDbEntity**) &zone );
		zone->SystemName("_repo_zone_1");
		zone->StringSet( STR_TYPE, "_zone" );
		zone->IntSet( "_zone_num", 1 );
		zone->DoubleSet( "_zone_top", 0 );
		zone->DoubleSet( "_zone_left", 0 );
		zone->DoubleSet( "_zone_bottom", m_model->Header().getReal( STR_WIDTH, 0. ) );
		zone->DoubleSet( "_zone_right", m_model->Header().getReal( STR_LENGTH, 0. ) );

#if REQUIRED
		// Unfortunately, setting the system flag prevents the
		// work-zone features from appearing in the entity list.

		// By setting the system flag, we prevent work-zones features
		// from being inadvertantly selected and deleted.
		zone->SystemFlag( true );
#endif

		iter.Init( m_model->Db(), DBFEATURE );
		while (1)
		{
			dbFeature = dynamic_cast<CDbFeature*>( iter() );
			if (dbFeature == NULL)
				break;

			iter.Next();

			if (dbFeature == zone)
				continue;
			
			if (dbFeature->Owner() != NULL)
				continue;

			zone->Append( dbFeature );

		}
	}

	return zone;
}

void
CMM2::ClampMigrate( CDbCommand* dbCommand )
{
	CString		strg;
	C3dCoord	pt;
	CDbFeature*	dbFeature;
	double		xmin, ymin;
	double		xmax, ymax;
	int			repo, num;
	int			count;

	pt = dbCommand->Coord();

	xmin = dbCommand->DoubleGet( STR_XMIN, 0. );
	ymin = dbCommand->DoubleGet( STR_YMIN, 0. );
	xmax = dbCommand->DoubleGet( STR_XMAX, 0. );
	ymax = dbCommand->DoubleGet( STR_YMAX, 0. );

	repo = dbCommand->IntGet( "repo", 0 );
	num  = dbCommand->IntGet( "num", 0 );

	strg.Format( "_repo_zone_%d", repo );
	m_model->EntityFind( strg, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
	if (dbFeature != NULL)
	{
		strg.Format( "_clamp%d_x", num );
		dbFeature->DoubleSet( strg, pt.X() );

		strg.Format( "_clamp%d_y", num );
		dbFeature->DoubleSet( strg, pt.Y() );

		count = dbFeature->IntGet( "_clamp_num", 0 ) + 1;
		dbFeature->IntSet( "_clamp_num", count );

		if (count == 1)
		{
			dbFeature->DoubleSet( "_clamp_lldx", (xmin - pt.X()) );
			dbFeature->DoubleSet( "_clamp_lldy", (ymin - pt.Y()) );
			dbFeature->DoubleSet( "_clamp_trdx", (xmax - pt.X()) );
			dbFeature->DoubleSet( "_clamp_trdy", (ymax - pt.Y()) );
		}
	}

	dbCommand->Delete();
}

void
CMM2::DropStopMigrate( CDbContainer* dbContainer )
{
	CString		strg;
	CDbEntity*	dbEntity;
	int			count;

	strg = dbContainer->StringGet( "@DROPSTOP", "" ).Mid(1,4);

	count = dbContainer->Count();
	if (count > 0)
	{
		dbEntity = (*dbContainer)[count-1];

		if (strg.CompareNoCase("STOP") == 0)
		{
			dbEntity->IntSet( "_dropstop", DROP_CONST );
		}
		else if (strg.CompareNoCase("DROP") == 0)
		{
			dbEntity->IntSet( "_dropstop", STOP_CONST );
		}
	}

	dbContainer->AttribDelete("@DROPSTOP");
}

void
CMM2::HoldMigrate( CDbCommand* dbCommand )
{
	CDbFeature*	dbFeature;
	C3dCoord	pt;
	CString		strg;
	double		c1x, c1y;
	double		c2x, c2y;
	double		dia, x, y;
	int			type, repo;

	pt = dbCommand->Coord();

	type = dbCommand->IntGet( STR_TYPE, 0 );
	repo = dbCommand->IntGet( "repo", 2 );

	c1x = dbCommand->DoubleGet( "c1x", 0. );
	c1y = dbCommand->DoubleGet( "c1y", 0. );
	c2x = dbCommand->DoubleGet( "c2x", 0. );
	c2y = dbCommand->DoubleGet( "c2y", 0. );
	dia = dbCommand->DoubleGet( "dia", 0. );
	x   = dbCommand->DoubleGet( "x", 0. );
	y   = dbCommand->DoubleGet( "y", 0. );

	strg.Format( "_repo_zone_%d", (repo-1) );
	m_model->EntityFind( strg, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
	if (dbFeature != NULL)
	{
		dbFeature->IntSet( "_hold", type );
		dbFeature->DoubleSet( "_hold_c1dx", (c1x - pt.X()) );
		dbFeature->DoubleSet( "_hold_c1dy", (c1y - pt.Y()) );
		dbFeature->DoubleSet( "_hold_c2dx", (c2x - pt.X()) );
		dbFeature->DoubleSet( "_hold_c2dy", (c2y - pt.Y()) );
		dbFeature->DoubleSet( "_hold_dia", dia );
		dbFeature->DoubleSet( "_hold_x", x );
		dbFeature->DoubleSet( "_hold_y", y );
	}

	dbCommand->Delete();
}

CDbFeature*
CMM2::ZoneMigrate( CDbCommand* dbCommand )
{
	CDbFeature*	dbFeature;
	CString		strg;

	dbFeature = NULL;

	// TODO: Migrate the attributes from the command to the appropriate feature.
	int repo = dbCommand->IntGet( "repo", 0 ) + 1;
	double xmin = dbCommand->DoubleGet( "xmin", 0.0 );
	double ymin = dbCommand->DoubleGet( "ymin", 0.0 );
	double xmax = dbCommand->DoubleGet( "xmax", 0.0 );
	double ymax = dbCommand->DoubleGet( "ymax", 0.0 );

	strg.Format( "_repo_zone_%d", repo );
	m_model->EntityFind( strg, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
	if (dbFeature != NULL)
	{
		// Note that "_zone_repo" is actually set from
		// the @REPO command during nesting.
		dbFeature->IntSet( "_zone_num", repo );
		dbFeature->DoubleSet( "_zone_left", xmin );
		dbFeature->DoubleSet( "_zone_top", ymin );
		dbFeature->DoubleSet( "_zone_right", xmax );
		dbFeature->DoubleSet( "_zone_bottom", ymax );
	}

	dbCommand->Delete();

	return dbFeature;
}



