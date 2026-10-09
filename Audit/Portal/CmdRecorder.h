
#ifndef _CMDRECORDER_H
#define _CMDRECORDER_H

#include "stdafx.h"
#include "Return.h"

class dllExport CCmdRecorder
{
public:

	CCmdRecorder();

	~CCmdRecorder();

	void Init( const CString& log_file_path, bool incremental_update );

	void Terminate();

	void Record( const CString& log_msg );

private:

	CString	m_log_file_path;
	bool	m_incremental_update;
	FILE*	m_file;
};

#endif
