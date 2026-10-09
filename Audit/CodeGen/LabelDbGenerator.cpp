
#include "stdafx.h"
#include "Path.h"
#include "DaoDB.h"
#include "DaoQuery.h"
#include "DbPattern.h"
#include "LabelDbGenerator.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CString LABEL_RESULT("Label Result");

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CLabelDbGenerator::CLabelDbGenerator()
{
}

CLabelDbGenerator::~CLabelDbGenerator()
{
}

void
CLabelDbGenerator::Init( const CString& labelDbPath )
{
	m_labelDbPath = labelDbPath;
}

CReturn
CLabelDbGenerator::LabelTableUpdate(
	const CString&		pdbPath,
	int					sheet_num,
	const CModelClfile&	clfile )
{
	CReturn	status;
	CString msg;

	if ( EWMDiagnosticAllow() )
	{
		msg.Format( "LabelTableUpdate: m_labelDbPath <%s>", m_labelDbPath );
		EWMDiagnostic( (LPCSTR) msg );
	}

	if ( !m_labelDbPath.IsEmpty() )
	{
		// I suppose we should check that the file exists first
		// because VB is responsible for the lifetime of the file.
		// Otherwise, the fopen statement will create the file.
		FILE* db = fopen( m_labelDbPath, "a+" );
		if (db == NULL)
		{
			msg.Format( "LabelTableUpdate: open <%s> FAILED", m_labelDbPath );
			status.Internal( IDS_INTERNAL_ERROR, msg );
		}
		else
		{
			msg.Format( "LabelTableUpdate: open <%s> OK", m_labelDbPath );
			status.Diagnostic( msg );

			int count = clfile.PartCount();
			if (count == 0)
			{
				// ASSUMPTION: The model was coded with Optimize disabled.
				// And though I don't agree with updating the label table
				// under these conditions, I guess we need to do something.
				CDaoDB pdb;

				pdb.addTable( "Sheet Result" );
				status = pdb.Open( pdbPath );
				if ( status.IsOk() )
				{
					CDaoQuery* query;
					CString sql;

					sql.Format( "SELECT * FROM [Part Result] WHERE ([Sheet ID]=%d)", sheet_num );
					query = pdb.QueryExecute( sql );

					int rcnt = ((query == NULL) ? 0 : query->RecordCount());

					msg.Format( "LabelTableUpdate (Optimize disabled?): record count <%d>", rcnt );
					status.Diagnostic( msg );

					for (int rndx = 0; rndx < rcnt; ++rndx)
					{
						query->Move( ((rndx == 0) ? 0 : 1) );

						int part_id = query->IntGet( "Part ID" );

						msg.Format( "%s|%d|%d", pdbPath, sheet_num, part_id );
						fprintf( db, "%s\n", msg );
					}
				}
			}
			else
			{
				msg.Format( "LabelTableUpdate: part count <%d>", count );
				status.Diagnostic( msg );

				for (int indx = 0; indx < count; ++indx)
				{
					CDbPattern* dbPattern = clfile.PartGet(indx);

					int part_id = dbPattern->IntGet( "part_id", -1 );

					msg.Format( "%s|%d|%d", pdbPath, sheet_num, part_id );
					fprintf( db, "%s\n", msg );
				}
			}

			fclose( db );
			EWMDiagnostic( "LabelTableUpdate: close()" );
		}
	}

	return status;
}
