
#include "StdAfx.h"
#include "DbTool.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbHole.h"
#include "DbIterator.h"
#include "DxfWriter.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CDxfWriter::CDxfWriter()
{
	m_path = "";
	m_file = NULL;
	m_db = NULL;
	m_fmt = "";
	m_ACDB = false;
}

CDxfWriter::~CDxfWriter()
{
	if (m_file != NULL)
		fclose( m_file );
}

CReturn
CDxfWriter::Write(
	const CString	path,
	const CModel&	model,
	int				mantissa,
	bool			layers )
{
	CReturn	status;

	status = Init( path, model, mantissa );
	if ( status.IsOk() )
	{
		if ( layers )
			LayersDump();
		
		ModelDump();
		
		Terminate();
	}
	
	return status;
}

CReturn
CDxfWriter::Init( const CString path, const CModel& model, int mantissa )
{
	CReturn	status( STATUS_ERROR );  // assume failure

	m_path = path;

	m_file = fopen( m_path, "w" );
	if (m_file != NULL)
	{
		if ((mantissa > 0) && (mantissa < 7))
		{
			m_fmt.Format( "%s.%df", "%-12", mantissa );
			m_db = &model.Db();

			status = STATUS_OKAY;
		}
	}

	return status;
}

void
CDxfWriter::Terminate()
{
  	StringDump( "  0" );
	StringDump( "EOF" );

	fclose( m_file );
	m_file = NULL;
}

void
CDxfWriter::LayersDump()
{
	CDbIterator	iter;
		
	StringDump( "  0" );
	StringDump( "SECTION" );
  	
	StringDump( "  2" );
	StringDump( "TABLES" );
	
	StringDump( "  0" );
	StringDump( "TABLE" );
  	
	StringDump( "  2" );
	StringDump( "LAYER" );
	
	if ( m_ACDB )
	{
		StringDump( "100" );
		StringDump( "AcDbSymbolTable" );
	}
	
	iter.Init( *m_db, DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if (dbTool->RefCnt() > 0)
		{
			if ( m_ACDB )
			{
				StringDump( "100" );
				StringDump( "AcDbLayerTableRecord" );
			}

			StringDump( "  0" );
			StringDump( "LAYER" );
  
			StringDump( "  2" );
			StringDump( dbTool->Name() );

			StringDump( " 70" );
			StringDump( "  0" );

			StringDump( " 62" );
			StringDump( "  7" );

			StringDump( "  6" );
			StringDump( "CONTINUOUS" );
		}

		iter.Next();
	}
  	
	StringDump( "  0" );
	StringDump( "ENDTAB" );
	
	StringDump( "  0" );
	StringDump( "ENDSEC" );
}

void
CDxfWriter::ModelDump()
{
	StringDump( "  0" );
	StringDump( "SECTION" );
	
	StringDump( "  2" );
	StringDump( "ENTITIES" );
	
	LinesDump();
	
	ArcsDump();
	
	HolesDump();

	StringDump( "  0" );
	StringDump( "ENDSEC" );
}

void
CDxfWriter::LinesDump()
{
	CDbIterator	iter;
	
	iter.Init( *m_db, DBLINE );
	while (1)
	{
		CDbLine* dbLine = dynamic_cast<CDbLine*>( iter() );
		if (dbLine == NULL)
			break;

		CDbTool* dbTool = dbLine->Tool();

		LineDump( dbTool->Name(), dbLine->StartPt(), dbLine->EndPt() );

		iter.Next();
	}
}

void
CDxfWriter::ArcsDump()
{
	CDbIterator	iter;
	CGeoArc		geoArc;
	C3dCoord	ps;
	C3dCoord	pe;
	C3dCoord	pc;
	double		as, ae, incang;
	int			dir;
	boolean		circle;
	
	iter.Init( *m_db, DBARC );
	while (1)
	{
		CDbArc* dbArc = dynamic_cast<CDbArc*>( iter() );
		if (dbArc == NULL)
			break;

		CDbTool* dbTool = dbArc->Tool();

		ps = dbArc->StartPt();
		pe = dbArc->EndPt();
		pc = dbArc->CenterPt();
		
		geoArc.Init( ps, pe, pc, dbArc->Dir() );
		
		incang = geoArc.IncludedAngle() * RAD2DEG;
		circle = (fabs( incang - 360. ) < 1.e-2);
		
		StringDump( "  0" );
		StringDump( (circle ? "CIRCLE" : "ARC") );
		
		if ( m_ACDB )
		{
			StringDump( "100" );
			StringDump( (circle ? " AcDbCircle" : "AcDbArc") );
		}
		
		StringDump( "  8" );
		StringDump( dbTool->Name() );
		
		StringDump( " 10" );
		DoubleDump( pc.X() );
  		
		StringDump( " 20" );
		DoubleDump( pc.Y() );
  		
		StringDump( " 30" );
		DoubleDump( pc.Z() );
  		
		StringDump( " 40" );
		DoubleDump( dbArc->Radius() );
  		
  		if (circle == false)
  		{
			geoArc.Angles( &as, &ae );

			as *= RAD2DEG;
			ae *= RAD2DEG;
			
			dir = geoArc.Dir();
			
			StringDump( " 50" );
			DoubleDump( ((dir > 0) ? as : ae) );
	  		
			StringDump( " 51" );
			DoubleDump( ((dir > 0) ? ae : as) );
		}

		iter.Next();
	}
}

void
CDxfWriter::HolesDump()
{
	CDbIterator	iter;
	C3dCoord	ps;
	C3dCoord	pe;
	C3dCoord	pc;
	double	radius;
	double	SIZE = 0.125;
	int		toolType;
	boolean	isRound;
		
	iter.Init( *m_db, DBHOLE );
	while (1)
	{
		CDbHole* dbHole = dynamic_cast<CDbHole*>( iter() );
		if (dbHole == NULL)
			break;

		CDbTool* dbTool = dbHole->Tool();
		
		pc = dbHole->Center();

		toolType = dbTool->IntGet( "Type_ID", TTYPE_NONE );
		
		isRound = (toolType == TTYPE_ROUND);
		radius = (isRound ? (0.5 * dbTool->DoubleGet( "Diameter", 0. )) : SIZE );			
		
		StringDump( "  0" );
		StringDump( "CIRCLE" );
		
		if ( m_ACDB )
		{
			StringDump( "100" );
			StringDump( " AcDbCircle" );
		}
		
		StringDump( "  8" );
		StringDump( dbTool->Name() );
		
		StringDump( " 10" );
		DoubleDump( pc.X() );
  		
		StringDump( " 20" );
		DoubleDump( pc.Y() );
  		
		StringDump( " 30" );
		DoubleDump( pc.Z() );
  		
		StringDump( " 40" );
		DoubleDump( radius );
		
		if (isRound == false)
		{
			ps.XYZ( pc.X(), (pc.Y() - SIZE), pc.Z() );
			pe.XYZ( pc.X(), (pc.Y() + SIZE), pc.Z() );
			LineDump( "BOGUS", ps, pe );

			ps.XYZ( (pc.X() - SIZE), pc.Y(), pc.Z() );
			pe.XYZ( (pc.X() + SIZE), pc.Y(), pc.Z() );
			LineDump( "BOGUS", ps, pe );
		}

		iter.Next();
	}
}

void
CDxfWriter::LineDump( CString layerName, const C3dCoord& ps, const C3dCoord& pe )
{
	StringDump( "  0" );
	StringDump( "LINE" );
	
	if ( m_ACDB )
	{
		StringDump( "100" );
		StringDump( "AcDbLine" );
	}
	
	StringDump( "  8" );
	StringDump( layerName );
	
	StringDump( " 10" );
	DoubleDump( ps.X() );
	
	StringDump( " 20" );
	DoubleDump( ps.Y() );
	
	StringDump( " 30" );
	DoubleDump( ps.Z() );
	
	StringDump( " 11" );
	DoubleDump( pe.X() );
	
	StringDump( " 21" );
	DoubleDump( pe.Y() );
	
	StringDump( " 31" );
	DoubleDump( pe.Z() );
}

void
CDxfWriter::StringDump( CString sval )
{
	fprintf( m_file, "%s\n", sval );
}

void
CDxfWriter::DoubleDump( double dval )
{
	CString	sval;

	sval.Format( m_fmt, dval );
	StringDump( sval );
}
