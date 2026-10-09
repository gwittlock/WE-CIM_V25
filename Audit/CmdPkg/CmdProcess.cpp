// ==================================================================
//		MyClass
//
// ==================================================================

#include "stdafx.h"
#include "Return.h"
#include "Command.h"
#include "CmdProcess.h"

// ==================================================================

CCmdProcess::CCmdProcess()
{
}

CCmdProcess::~CCmdProcess()
{
}


// ==================================================================


CReturn	
CCmdProcess::Execute( 
	CCommand& in_cmd )
{
	return CReturn( STATUS_ERROR );
}
