
#include <sstream>

#include "stdafx.h"
#include "cmn_resource.h"
#include "Path.h"
#include "StringConst.h"
#include "3x4Matrix.h"
#include "GeoPoint.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "DbAllEntities.h"
#include "DbIterator.h"
#include "Model.h"
#include "ModelText.h"
#include "mm2.h"
#include "xml.h"

#define SORE(x) ((x == nullptr) ? "" : (x))


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CXML::CXML()
{
	m_model = nullptr;
	m_dbProfile = nullptr;
}

CXML::~CXML()
{
}

CReturn CXML::Write( const CString& fname, const CModel& model )
{
	CReturn status;

	TiXmlDocument doc;

	m_model = (CModel*) &model;

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	ModelWrite( model, &doc );

	doc.SaveFile( (LPCSTR) fname );

	return status;
}

void CXML::ModelWrite( const CModel& model, TiXmlDocument* doc )
{
	CDbIterator iter;
	CReturn status;

	m_model = (CModel*) &model;

#if 0
	int len = m_currentVersion.GetLength() + 1;
	file.Write( (BYTE*) &len, sizeof(len) );
	file.Write( (BYTE*)(LPCSTR) m_currentVersion, len );
#endif

	TiXmlElement* modelElement = new TiXmlElement( "model" );

	// Model header and defaults...
	AttributesWrite( modelElement, "model_header", m_model->Header() );
	AttributesWrite( modelElement, "model_defaults", m_model->Default() );

	// Database...
#if 0
	int entityCount = m_model->EntityCount();
	file.Write( (BYTE*) &entityCount, sizeof(entityCount) );
#endif

	iter.Init( m_model->Db(), DBWORKPLANE );

	while (1)
	{
		const CDbEntity* dbEntity = iter();
		if (dbEntity == nullptr)
			break;

		EntityWrite( modelElement, *dbEntity );

		iter.Next();
	}

	doc->LinkEndChild( modelElement );
}

void CXML::EntityWrite( TiXmlElement* parent, const CDbEntity& dbEntity )
{
	EDbEntityType type = dbEntity.Type();

	switch ( dbEntity.Type() )
	{
	case DBWORKPLANE:
		WorkplaneWrite( parent, dynamic_cast<const CDbWorkplane&>( dbEntity ) );
		break;

	case DBTOOL:
		ToolWrite( parent, dynamic_cast<const CDbTool&>( dbEntity ) );
		break;

	case DBPOINT:
		// status = WritePoint( file, dbEntity );
		break;

	case DBLINE:
		LineWrite( parent, dynamic_cast<const CDbLine&>( dbEntity ) );
		break;

	case DBARC:
		ArcWrite( parent, dynamic_cast<const CDbArc&>( dbEntity ) );
		break;

	case DBHOLE:
		HoleWrite( parent, dynamic_cast<const CDbHole&>( dbEntity ) );
		break;

	case DBPROFILE:
		ProfileWrite( parent, dynamic_cast<const CDbProfile&>( dbEntity ) );
		break;

	case DBCOMMAND:
		break;

	case DBFEATURE:
		FeatureWrite( parent, dynamic_cast<const CDbFeature&>( dbEntity ) );
		break;

	case DBSEQUENCE:
		// status = WriteSequence( file, dbEntity );
		break;

	case DBPATTERN:
		// status = WritePattern( file, dbEntity );
		break;
	}
}

void CXML::WorkplaneWrite( TiXmlElement* parent, const CDbWorkplane& dbWorkplane )
{
	TiXmlElement* workplaneElement = new TiXmlElement( "workplane" );

	const C3x4Matrix& xform = dbWorkplane.Transform();

	const C3dVec& ivec = xform.getI();
	const C3dVec& jvec = xform.getJ();
	const C3dVec& kvec = xform.getK();
	const C3dCoord& origin = xform.getT();

	workplaneElement->SetAttribute( "name", (LPCSTR) dbWorkplane.Name() );

	VectorWrite( workplaneElement, "ivec", ivec );
	VectorWrite( workplaneElement, "jvec", jvec );
	VectorWrite( workplaneElement, "kvec", kvec );
	PointWrite( workplaneElement, "t", origin );
#if 0
	int	up = dbWorkplane.ToolUp();
	file.Write( (BYTE*) &up, sizeof(up) );
#endif

	parent->LinkEndChild( workplaneElement );
}

void CXML::ToolWrite( TiXmlElement* parent, const CDbTool& dbTool )
{
	TiXmlElement* toolElement = new TiXmlElement( "tool" );

	toolElement->SetAttribute( "name", (LPCSTR) dbTool.Name() );
	AttributesWrite( toolElement, "common", dbTool.Attrib() );

	parent->LinkEndChild( toolElement );
}

void CXML::LineWrite( TiXmlElement* parent, const CDbLine& dbLine )
{
	TiXmlElement* lineElement = new TiXmlElement( "line" );

	CommonWrite( lineElement, dbLine );

	PointWrite( lineElement, "ps", dbLine.StartPt() );
	PointWrite( lineElement, "pe", dbLine.EndPt() );

	parent->LinkEndChild( lineElement );
}

void CXML::ArcWrite( TiXmlElement* parent, const CDbArc& dbArc )
{
	TiXmlElement* arcElement = new TiXmlElement( "arc" );

	CommonWrite( arcElement, dbArc );

	PointWrite( arcElement, "ps", dbArc.StartPt() );
	PointWrite( arcElement, "pe", dbArc.EndPt() );
	PointWrite( arcElement, "pc", dbArc.CenterPt() );

	TiXmlElement* dir = new TiXmlElement( "dir" );
	dir->SetAttribute( "ival", dbArc.Dir() );
	arcElement->LinkEndChild( dir );

	parent->LinkEndChild( arcElement );
}

void CXML::HoleWrite( TiXmlElement* parent, const CDbHole& dbHole )
{
	TiXmlElement* holeElement = new TiXmlElement( "hole" );

	CommonWrite( holeElement, dbHole );

	PointWrite( holeElement, "pc", dbHole.Center() );

	// dbHole.Diam();

	TiXmlElement* depth = new TiXmlElement( "depth" );
	depth->SetAttribute( "dval", ToString( dbHole.Depth() ) );
	holeElement->LinkEndChild( depth );

	parent->LinkEndChild( holeElement );
}

void CXML::ProfileWrite( TiXmlElement* parent, const CDbProfile& dbProfile )
{
	TiXmlElement* profileElement = new TiXmlElement( "profile" );

	CommonWrite( profileElement, dbProfile );

	int count = dbProfile.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = dbProfile.GetAt( indx );
		this->EntityWrite( profileElement, *dbEntity );
	}

	parent->LinkEndChild( profileElement );
}

void CXML::FeatureWrite( TiXmlElement* parent, const CDbFeature& dbFeature )
{
	TiXmlElement* featureElement = new TiXmlElement( "feature" );

	CommonWrite( featureElement, dbFeature );

	int count = dbFeature.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = dbFeature.GetAt( indx );
		this->EntityWrite( featureElement, *dbEntity );
	}

	parent->LinkEndChild( featureElement );
}

void CXML::PointWrite( TiXmlElement* parent, const char* elementName, const C3dCoord& pt )
{
	TiXmlElement* pointElement = new TiXmlElement( elementName );

	TripleWrite( pointElement, pt.X(), pt.Y(), pt.Z() );

	parent->LinkEndChild( pointElement );
}

void CXML::VectorWrite( TiXmlElement* parent, const char* elementName, const C3dVec& vec )
{
	TiXmlElement* vectorElement = new TiXmlElement( elementName );

	TripleWrite( vectorElement, vec.X(), vec.Y(), vec.Z() );

	parent->LinkEndChild( vectorElement );
}

void CXML::CommonWrite( TiXmlElement* parent, const CDbEntity& dbEntity )
{
	AttributesWrite( parent, "common", dbEntity.Attrib() );
}

void CXML::AttributesWrite( TiXmlElement* parent, const char* elementName, const CVarList& varlist )
{
	int count = varlist.countVar();
	if (count > 0)
	{
		TiXmlElement* attribs = new TiXmlElement( elementName );

		// file.Write( (BYTE*) &count, sizeof(count) );

		for (int indx = 0; indx < count; ++indx)
		{
			CVar* var = varlist.getVar( indx );

			eVarType type = var->getType();
			TiXmlElement* attrib = new TiXmlElement( (LPCSTR) var->getName() );
#if 0
			// The type is implicit in the tags (ival, dval, sval).
			attrib->SetAttribute( "type", type );
#endif
			switch (type)
			{
			case VAR_INT:
				attrib->SetAttribute( "ival", var->getInt() );
				break;

			case VAR_REAL:
				{
					const char* temp = ToString( var->getReal() ) ;
					int breakpoint = 0;
				}
				attrib->SetAttribute( "dval", ToString( var->getReal() ) );
				break;

			case VAR_STRING:
				attrib->SetAttribute( "sval", (LPCSTR) var->getString() );
				break;
			}

			attribs->LinkEndChild( attrib );
		}

		parent->LinkEndChild( attribs );
	}
}

void CXML::TripleWrite( TiXmlElement* parent, double x, double y, double z )
{
	parent->SetAttribute( "x", ToString(x) );
	parent->SetAttribute( "y", ToString(y) );
	parent->SetAttribute( "z", ToString(z) );
}

static std::string result;
const char* CXML::ToString( double dval )
{
	std::stringstream ss;
	ss << dval << "\0";
	result = ss.str();
	return ( result.c_str() );
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn CXML::Read( const CString& fname, CModel* model )
{
	CReturn status;

	m_model = model;
	WorkplanesCreate();

	TiXmlDocument doc;
	if ( doc.LoadFile( (LPCSTR) fname ) )
	{
		TiXmlHandle hDoc( &doc );
		TiXmlHandle hRoot( 0 );

		TiXmlElement* pElem = hDoc.FirstChildElement().Element();

		std::string name = SORE( pElem->Value() );
		if (name == "model")
		{
			hRoot = TiXmlHandle( pElem );

			m_model->EntityCreate( DBFEATURE, (CDbEntity**) &m_workzone );
			if (m_workzone != nullptr)
			{
				TiXmlElement* element = pElem->FirstChildElement();
			
				while (1)
				{
					if (element == nullptr)
						break;

					ElementRead( element );

					element = element->NextSiblingElement();
				}

				m_workzone->Name( "_repo_zone_1" );
				m_workzone->StringSet( STR_TYPE, "_zone" );
				m_workzone->IntSet( "_zone_num", 1 );
			}
		}


		CPath path( fname );
		path.ForceExt( "txt" );
		{
			CModelText text;
			text.Enable( true );
			status = text.Dump( path.FullPath(), *m_model );
		}

		path.ForceExt( "mm2" );
		{
			CMM2 mm2;
			status = mm2.Write( path.FullPath(), *m_model );
		}
	}
	else
	{
		status.User( IDS_FILE_READ_ERR, fname );
	}

	return status;
}

void CXML::ElementRead( const TiXmlElement* element )
{
	std::string type = SORE( element->Value() );

	if (type == "model_header")
		AttributesRead( element, m_model->pHeader() );
	else if (type == "model_defaults")
		AttributesRead( element, m_model->pDefault() );
	else if (type == "workplane")
		WorkplaneRead( element );
	else if (type == "tool")
		ToolRead( element );
	else if (type == "line")
		LineRead( element );
	else if (type == "arc")
		ArcRead( element );
	else if (type == "hole")
		HoleRead( element );
	else if (type == "profile")
		ProfileRead( element );
	else if (type == "feature")
		FeatureRead( element );
}

void CXML::AttributesRead( const TiXmlElement* attributesElement, CVarList* attribs )
{
	int ival;
	double dval;
	std::string sval;

	const TiXmlElement* child = attributesElement->FirstChildElement();

	while (1)
	{
		if (child == nullptr)
			break;

		CString name = SORE( child->Value() );

		if (child->QueryIntAttribute( "ival", &ival ) == TIXML_SUCCESS)
			attribs->setInt( name, ival );
		else if (child->QueryDoubleAttribute( "dval", &dval ) == TIXML_SUCCESS)
			attribs->setReal( name, dval );
		else if (child->QueryStringAttribute( "sval", &sval ) == TIXML_SUCCESS)
			attribs->setString( name, CString( sval.c_str() ) );

		child = child->NextSiblingElement();
	}
}

void CXML::WorkplaneRead( const TiXmlElement* workplaneElement )
{
	const char* name = SORE( workplaneElement->Value() );

	C3dVec ivec, jvec, kvec;
	C3dCoord origin;

	const TiXmlElement* element = nullptr;

	element = workplaneElement->FirstChildElement( "ivec" );
	VectorRead( element, &ivec );

	element = workplaneElement->FirstChildElement( "jvec" );
	VectorRead( element, &jvec );

	element = workplaneElement->FirstChildElement( "kvec" );
	VectorRead( element, &kvec );

	element = workplaneElement->FirstChildElement( "t" );
	PointRead( element, &origin );

	CDbWorkplane* dbWorkplane;
	m_model->EntityCreate( DBWORKPLANE, (CDbEntity**) &dbWorkplane );

	dbWorkplane->Name( CString( name ) );
	dbWorkplane->Init( ivec, jvec, kvec, origin, true );
}

void CXML::ToolRead( const TiXmlElement* toolElement )
{
	const char* name = SORE( toolElement->Value() );

	CVarList common;

	const TiXmlElement* element = toolElement->FirstChildElement( "common" );
	if (element == nullptr)
	{
		// ASSUMPTION: We've encountered something like "<tool name="Stock" />"
	}
	else
	{
		AttributesRead( element, &common );
	}

	CDbTool* dbTool;
	m_model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );

	// dbTool->Name( CString( name ) );

	CVarList* attribs = dbTool->pAttrib();
	(*attribs) = common;

	int type_id = attribs->getInt( "tltype", -1 );
	if (type_id != -1)
		attribs->setInt( STR_TYPE_ID, type_id );

	int tnum = attribs->getInt( "NC_Code_Number", -1 );
	m_toolMap[tnum] = dbTool;
}

void CXML::LineRead( const TiXmlElement* lineElement )
{
	const char* name = SORE( lineElement->Value() );

	CVarList common;
	C3dCoord ps, pe;

	const TiXmlElement* commonElement = lineElement->FirstChildElement( "common" );
	AttributesRead( commonElement, &common );

	const TiXmlElement* psElement = lineElement->FirstChildElement( "ps" );
	PointRead( psElement, &ps );

	const TiXmlElement* peElement = lineElement->FirstChildElement( "pe" );
	PointRead( peElement, &pe );

	CDbTool* dbTool;
	CDbWorkplane* dbWork;
	if ( ReferenceEntitiesGet( common, &dbTool, &dbWork ) )
	{
		CDbLine* dbLine;
		m_model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );

		// dbLine->Name( CString( name ) );
		dbLine->Init( dbTool, dbWork, ps, pe );

		CVarList* attribs = dbLine->pAttrib();
		(*attribs) = common;

		if (m_dbProfile != nullptr)
			m_dbProfile->Append( dbLine );
	}
}

void CXML::ArcRead( const TiXmlElement* arcElement )
{
	const char* name = SORE( arcElement->Value() );

	CVarList common;
	C3dCoord ps, pe, pc;
	int dir = 0;

	const TiXmlElement* commonElement = arcElement->FirstChildElement( "common" );
	AttributesRead( commonElement, &common );

	const TiXmlElement* psElement = arcElement->FirstChildElement( "ps" );
	PointRead( psElement, &ps );

	const TiXmlElement* peElement = arcElement->FirstChildElement( "pe" );
	PointRead( peElement, &pe );

	const TiXmlElement* pcElement = arcElement->FirstChildElement( "pc" );
	PointRead( pcElement, &pc );

	const TiXmlElement* dirElement = arcElement->FirstChildElement( "dir" );
	dirElement->QueryIntAttribute( "ival", &dir );

	CDbTool* dbTool;
	CDbWorkplane* dbWork;
	if ( ReferenceEntitiesGet( common, &dbTool, &dbWork ) )
	{
		CDbArc* dbArc;
		m_model->EntityCreate( DBARC, (CDbEntity**) &dbArc );

		// dbArc->Name( CString( name ) );
		dbArc->Init( dbTool, dbWork, ps, pe, pc, dir );

		CVarList* attribs = dbArc->pAttrib();
		(*attribs) = common;

		if (m_dbProfile != nullptr)
			m_dbProfile->Append( dbArc );
	}
}

void CXML::HoleRead( const TiXmlElement* holeElement )
{
	CVarList common;
	C3dCoord pc;
	double diam = 0.;
	double depth = 0.;

	const TiXmlElement* commonElement = holeElement->FirstChildElement( "common" );
	AttributesRead( commonElement, &common );

	const TiXmlElement* pcElement = holeElement->FirstChildElement( "pc" );
	PointRead( pcElement, &pc );

	const TiXmlElement* depthElement = holeElement->FirstChildElement( "depth" );
	depthElement->QueryDoubleAttribute( "dval", &depth );

	CDbTool* dbTool;
	CDbWorkplane* dbWork;
	if ( ReferenceEntitiesGet( common, &dbTool, &dbWork ) )
	{
		CDbHole* dbHole;
		m_model->EntityCreate( DBHOLE, (CDbEntity**) &dbHole );

		// dbHole->Name( CString( name ) );
		dbHole->Init( dbTool, dbWork, pc, diam, depth );

		CVarList* attribs = dbHole->pAttrib();
		(*attribs) = common;

		m_workzone->Append( dbHole );
	}
}

void CXML::ProfileRead( const TiXmlElement* profileElement )
{
	const TiXmlElement* child = profileElement->FirstChildElement();
	if (child != nullptr)
	{
		CReturn status = m_model->EntityCreate( DBPROFILE, (CDbEntity**) &m_dbProfile );
		if ( status.IsOk() )
		{
			while (child != nullptr)
			{
				ElementRead( child );
				child = child->NextSiblingElement();
			}

			m_workzone->Append( m_dbProfile, false );
		}

		m_dbProfile = nullptr;
	}
}

void CXML::FeatureRead( const TiXmlElement* featureElement )
{
}

void CXML::PointRead( const TiXmlElement* pointElement, C3dCoord* pt )
{
	double triple[3];
	TripleRead( pointElement, triple );

	pt->XYZ( triple[0], triple[1], triple[2] );
}

void CXML::VectorRead( const TiXmlElement* vectorElement, C3dVec* vec )
{
	double triple[3];
	TripleRead( vectorElement, triple );

	vec->Init( triple[0], triple[1], triple[2] );
}

void CXML::TripleRead( const TiXmlElement* tripleElement, double* vals )
{
	tripleElement->QueryDoubleAttribute( "x", &vals[0] );
	tripleElement->QueryDoubleAttribute( "y", &vals[1] );
	tripleElement->QueryDoubleAttribute( "z", &vals[2] );
}

void CXML::WorkplanesCreate()
{
	CDbWorkplane* dbWork = PlaneFindCreate( STR_WORLD );
	dbWork->Hide();
	dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 1, 0 ),		// j
					C3dVec( 0, 0, 1 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up

	dbWork = PlaneFindCreate( STR_TOP );
	dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 1, 0 ),		// j
					C3dVec( 0, 0, 1 ),		// k
					C3dCoord( 0., 96., 12. ),	// t
					1 );					// up

	// Otherwise, m_model->IsRightHanded() may return the wrong answer.
	// pilfered from CImportUtil::StandardPlanesCreate()
	m_model->pHeader()->setInt( "WorkplaneType", 1 );
	m_model->pHeader()->setInt( "MachineType", 5 );
}

CDbWorkplane* CXML::PlaneFindCreate( const CString& name )
{
	CDbWorkplane* dbWork;

	m_model->EntityFind( name, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork == NULL)
	{
		m_model->EntityCreate( DBWORKPLANE, (CDbEntity**) &dbWork );
		if (dbWork != NULL)
			dbWork->Name( name );
	}

	return dbWork;
}

bool CXML::ReferenceEntitiesGet( const CVarList& common, CDbTool** dbTool, CDbWorkplane** dbWork )
{
	(*dbTool) = nullptr;
	(*dbWork) = nullptr;

	int tnum = common.getInt( "tnum", -1 );
	if (m_toolMap.count( tnum ) > 0)
		(*dbTool) = m_toolMap[tnum];

	if ((*dbTool) != nullptr)
		(*dbWork) = PlaneFindCreate( STR_WORLD );  // STR_TOP perhaps?

	return (((*dbTool) != nullptr) && ((*dbWork) != nullptr));
}
