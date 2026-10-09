
#include "CmdRecorder.h"

CCmdRecorder::CCmdRecorder()
	: m_log_file_path(),
	  m_incremental_update( false ),
	  m_file( NULL )
{
}

CCmdRecorder::~CCmdRecorder()
{
	Terminate();
}

void
CCmdRecorder::Init( const CString& log_file_path, bool incremental_update )
{
	m_log_file_path = log_file_path;
	m_incremental_update = incremental_update;

	remove( log_file_path );
}

void
CCmdRecorder::Terminate()
{
	if (m_file != NULL)
	{
		fflush( m_file );
		fclose( m_file );
		m_file = NULL;
	}
}

void
CCmdRecorder::Record( const CString& log_msg )
{
	if ( m_incremental_update )
	{
		m_file = fopen( m_log_file_path, "a+" );
		if (m_file != NULL)
		{
			fprintf( m_file, "%s", log_msg );
			fflush( m_file );
			fclose( m_file );
			m_file = NULL;
		}
	}
	else
	{
		if (m_file == NULL)
			m_file = fopen( m_log_file_path, "w" );

		if (m_file != NULL)
			fprintf( m_file, "%s", log_msg );
	}
}
