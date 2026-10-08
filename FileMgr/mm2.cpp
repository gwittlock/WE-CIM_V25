
#include "stdafx.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "DbTool.h"
#include "DbFeature.h"
#include "DbIterator.h"

#include "binary_file.h"

#include "Mm2.h"

////////////////////////////////////////////////////////////////////////

CMM2::CMM2()
	: m_model( NULL ),
	  m_currentVersion( "v16.5" ),
	  m_version(),
	  m_isValidVersion( FALSE ),
	  m_isPreVersion16( FALSE ),
	  m_flags( MM2_STD_MODE ),
	  m_featureId()
{
}

CMM2::~CMM2()
{
}

CReturn CMM2::Read( const CString& fname, CModel* model, int flags )
{
	CReturn status;
	CBinaryFile file;
	
	ASSERT( (model != NULL) );
	m_model = model;
	m_flags = flags;

	if (file.Open( fname, FILEMODE_READ ).IsOk())
	{
		m_model->UndoBufferSuppress();

		// The active layer and workplane will be set to
		// the first of each encountered will reading the
		// m_model file.  These are the first historically
		// recorded entites of the their type, usually the
		// default layer and workplane.
		m_model->ActiveTool( NULL );
		m_model->ActiveWorkplane( NULL );

		Read( file );

		m_model->UndoBufferActivate();

		file.Close();

		m_model->PostReadInit();
	}
	else
		status.setStatus( STATUS_ERROR );
	return status;
}

CReturn CMM2::Read( CBinaryFile& file )
{
	CReturn status = VersionRead( file, &m_version );

	if ( !status.IsOk() )
		return status;

	VersionFlagsSet( m_version );

	if ( !IsValidVersion() )
	{
		CString msg;

		// We may encounter an 'unknown version' during development
		// because file formats and/or data organization may change.
		// Moreover, development versions of files should be indicated
		// as such.  By issuing this error, we will be forced to
		// introduce a proper file version prior to release.  We
		// clearly failed to do this with the relase of V13.
		//
		// This may well be an annoyance, but what is the correct
		// thing to do?

		msg.Format( "Unknown file version <%s>", m_version );
		status.Diagnostic( msg );
	}
	
	status = ReadFile0_001( file );

	return status;
}

// MergeMM2:file="e:\work\bbi5\advmach\debug\drawing\metric.mm2",ox=0, oy=0, oz=0
CReturn CMM2::Merge( 
	const CString& fileName, 
	const C3dCoord& origin, 
	CModel* model,
	CDbContainer * owner )
{
	CReturn				ret;
	CSelectorStack		dummy_stack;
	CTreeViewSupport	dummy_tree;
	CModel				dummy_model;  // the model representing the part being merged

	CEntityDb&	db = (CEntityDb&)model->Db();

	dummy_model.UndoBufferSuppress();
	dummy_model.Init( dummy_stack, dummy_tree );

	ret += Read( fileName, &dummy_model, (MM2_SKIP_SEQ_OBJS | MM2_SKIP_WORK_ZONES) );
	if (ret.isOkay())
	{
		// Have the merge file in the dummy -- transfer it over to our
		// "real" model...
		//
		// VERY SIMILAR to the nest() function in NestProcess, Sheet.cpp
		//
		// Prepare for transformation (shift to origin)
		//
		C3x4Matrix	shift;
		shift.setUnit();
		shift.Shift( origin );

		// Housekeeping -- set up for clean copies
		dummy_model.UndoBufferSuppress();
//		model->UndoBufferSuppress();

		dummy_model.EntityPrepareCopy( model );

		// Clear the Transform tags...
		CDbEntity::NewAction();

		// 2018.03.26 -- ConditionalToolingCopy() was introduced to address
		// a crash occurring because of a missing tool in the recipient model.
		ConditionalToolingCopy( dummy_model, model );

		// Copy all relevant pieces of the incoming model...
		CDbIterator iter;
		iter.Init( dummy_model.Db(), DBLINE );
		while (TRUE)
		{
			CDbEntity* db_ent = iter();
			if (!db_ent)
				break;
			iter.Next();

			if ( dummy_model.Db().WasCopied( *db_ent ) )
				continue;

			// Travel up to ultimate parent...
			while (db_ent->Owner())
				db_ent = db_ent->Owner();

			// Skip the damn stock
			if ( db_ent->Tool()
				&& db_ent->Tool()->Name().CompareNoCase( STR_STOCK ) == 0)
				continue;

			//
			// Do the actual copy and shift...
			//
			// TODO:  Manage tool mismatch; for now CopyTo() does it's best to find a tool
			//
			CDbEntity* new_ent = NULL;
			dummy_model.EntityCopy( *db_ent, &new_ent );

			// Move to it's new home ... in space and order
			new_ent->Transform( shift );

			// put it in the new owner, if that is specified...
			if (owner)
				owner->Append( new_ent, false );
		}
	}

//	model->UndoBufferActivate();

	return ret;
}

CReturn CMM2::Write( const CString& fname, const CModel& model )
{
	CReturn status;

	m_model = (CModel*) &model;

	try
	{
		CBinaryFile file;
		
		status += file.Open( fname, FILEMODE_WRITE );

		m_model->pHeader()->setString( "ProjNum", fname );

		if (status.IsOk())
		{ 
			Write( file );
			file.Close();
		}
		return status;
	}
	catch( CFileException* e )
	{
		CString	msg;
		
		// Note: the exception code  e->m_cause is an integer, and not
		// suitable for our error message.
		status.User( IDS_FILE_WRITE_ERR, fname );
		delete e;

		return status;
	}

	return status;
}

CReturn CMM2::Write( CBinaryFile& file )
{
	CDbIterator iter;
	CReturn status;

	int len = m_currentVersion.GetLength() + 1;
	file.Write( (BYTE*) &len, sizeof(len) );
	file.Write( (BYTE*)(LPCSTR) m_currentVersion, len );

	//
	// Model header and defaults...
	//
	Write( file, m_model->Header() );
	Write( file, m_model->Default() );

	//
	// Database...
	//
	int entityCount = m_model->EntityCount();
	file.Write( (BYTE*) &entityCount, sizeof(entityCount) );

	iter.Init( m_model->Db(), DBWORKPLANE );

	while (1)
	{
		const CDbEntity* dbEntity = iter();

		if (dbEntity == NULL)
			break;

		EDbEntityType type = dbEntity->Type();

		file.Write( (BYTE*) &type, sizeof(type) );

		status = Write( file, (*dbEntity) );

		if ( !status.IsOk() )
			break;

		iter.Next();
	}

	return status;
}


// TODO: Add error handling?
CReturn CMM2::VersionRead( CBinaryFile& file, CString* version )
{
	CReturn status;
	char buf[256];
	int len;

	status += file.Read( (BYTE*) &len, sizeof(len) );

	status += file.Read( (BYTE*) buf, len );

	(*version) = buf;

	return status;
}

void CMM2::VersionFlagsSet( const CString& version )
{
#if BEFORE_V16
	// In order of likelyhood of occurrence...
	bool isValid =
		( (version.CompareNoCase( m_currentVersion ) == 0)	||
		  (version.CompareNoCase( "v15.11" ) == 0)		||
		  (version.CompareNoCase( "v15.02" ) == 0)		||
		  (version.CompareNoCase( "v15.01" ) == 0)		||
		  (version.CompareNoCase( "v15.alpha" ) == 0)		||
		  (version.CompareNoCase( "v14.601" ) == 0)		||
		  (version.CompareNoCase( "v14.501" ) == 0)		||
		  (version.CompareNoCase( "v14.001" ) == 0)		||
		  (version.CompareNoCase( "v14.alpha" ) == 0)		||
		  (version.CompareNoCase( "v12.001" ) == 0)
	    );
#else
	// In order of likelyhood of occurrence...
	m_isValidVersion =
		( (version.CompareNoCase( m_currentVersion ) == 0)	||
		  (version.CompareNoCase( "v16.01" ) == 0)			||
		  (version.CompareNoCase( "v16.0" ) == 0)			||
		  (version.CompareNoCase( "v15.11" ) == 0)			||
		  (version.CompareNoCase( "v15.02" ) == 0)			||
		  (version.CompareNoCase( "v15.01" ) == 0)			||
		  (version.CompareNoCase( "v15.alpha" ) == 0)		||
		  (version.CompareNoCase( "v14.601" ) == 0)			||
		  (version.CompareNoCase( "v14.501" ) == 0)	);

	if (version.Find("v16") < 0)
		m_isPreVersion16 = TRUE;
	else
		m_isPreVersion16 = (version.CompareNoCase("v16.0") == 0);
#endif
}

// 2018.03.26 -- ConditionalToolingCopy() was introduced to address
// a crash occurring because of a missing tool in the recipient model.
void CMM2::ConditionalToolingCopy( const CModel& incoming, CModel* model )
{
	CDbIterator iter;
	iter.Init( incoming.Db(), DBTOOL );
	while (1)
	{
		CDbTool* incomingTool = dynamic_cast<CDbTool*>(iter());
		if (incomingTool == nullptr)
			break;

		iter.Next();

		CDbTool* residentTool = nullptr;
		if ( incomingTool->IsLayer() )
		{
			residentTool = LayerFind( *model, incomingTool->Name() );
			if (residentTool == nullptr)
			{
				model->EntityCreate( DBTOOL, (CDbEntity**) &residentTool );
				residentTool->Name( incomingTool->Name() );
			}
		}
		else
		{
			// We search by "Tool_ID" instead of by "NC_Code_Number" because the
			// tool id is constant regardless of the station it is placed in.
			residentTool = ToolFind( *model, incomingTool->IntGet( STR_TOOL_ID, 0 ) );
			if (residentTool == nullptr)
			{
				model->EntityCreate( DBTOOL, (CDbEntity**) &residentTool );
				CVarList* attribs = residentTool->pAttrib();
				(*attribs) = incomingTool->Attrib();
			}
		}
	}
}

CDbTool* CMM2::LayerFind( const CModel& model, const CString& layerName )
{
	CDbTool* theLayer = nullptr;

	CDbIterator iter;
	iter.Init( model.Db(), DBTOOL );
	while (1)
	{
		CDbTool* tool = dynamic_cast<CDbTool*>(iter());
		if (tool == nullptr)
			break;

		iter.Next();

		if (tool->IsLayer() && (tool->Name() == layerName))
		{
			theLayer = tool;
			break;
		}
	}

	return theLayer;
}

CDbTool* CMM2::ToolFind( const CModel& model, int toolId )
{
	CDbTool* theTool = nullptr;

	CDbIterator iter;
	iter.Init( model.Db(), DBTOOL );
	while (1)
	{
		CDbTool* tool = dynamic_cast<CDbTool*>(iter());
		if (tool == nullptr)
			break;

		iter.Next();

		if (!tool->IsLayer() && (tool->IntGet( STR_TOOL_ID, -1 ) == toolId))
		{
			theTool = tool;
			break;
		}
	}

	return theTool;
}
