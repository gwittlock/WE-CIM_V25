
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <PROCESS.H>
#include "JFileMgr.h"


JNIEXPORT jboolean JNICALL Java_Weng_System_FileMgr_Copy
	(JNIEnv* env, jclass obj, jstring sourceName, jstring targetName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int status = 1;

	const char* theSourceName = env->GetStringUTFChars(sourceName, 0);
	const char* theTargetName = env->GetStringUTFChars(targetName, 0);

	TRY
	{
		CFile fSrc;
		CFile fDst;
		BOOL bSrc = fSrc.Open( theSourceName, (CFile::modeRead | CFile::shareExclusive) );
		BOOL bDst = fDst.Open( theTargetName, (CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive) );

		if (bSrc && bDst)
		{
			const int MAXBUF = 255;
			char buf[MAXBUF+1];

			while (1)
			{
				int count = fSrc.Read( buf, MAXBUF );
				if (count <= 0)
					break;  // end of file

				fDst.Write( buf, count );
			}
		}

		fDst.Close();
		fSrc.Close();

	}
	CATCH( CFileException, e )
	{
		status = 0;
	}
	END_CATCH

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars(sourceName, theSourceName);
	env->ReleaseStringUTFChars(targetName, theTargetName);

	return status;
}

JNIEXPORT jboolean JNICALL Java_Weng_System_FileMgr_Delete
	(JNIEnv* env, jclass obj, jstring fileName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int status = 1;

	const char* theFileName = env->GetStringUTFChars(fileName, 0);

	TRY
	{
		CFile::Remove( theFileName );
	}
	CATCH( CFileException, e )
	{
		status = 0;
	}
	END_CATCH

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars(fileName, theFileName);

	return status;
}

JNIEXPORT jboolean JNICALL Java_Weng_System_FileMgr_Exists
	(JNIEnv* env, jclass obj, jstring fileName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CFileStatus fileStatus;
	const char* theFileName = env->GetStringUTFChars(fileName, 0);

	int status = CFile::GetStatus( theFileName, fileStatus );

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars(fileName, theFileName);

	return status;
}

JNIEXPORT jint JNICALL Java_Weng_System_FileMgr_Length
	(JNIEnv* env, jclass obj, jstring fileName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CFileStatus fileStatus;
	const char* theFileName = env->GetStringUTFChars(fileName, 0);

	CFile file( theFileName, CFile::modeRead );
	int len = (int)file.GetLength();

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars(fileName, theFileName);

	return len;
}

JNIEXPORT jboolean JNICALL Java_Weng_System_FileMgr_Rename
	(JNIEnv* env, jclass obj, jstring sourceName, jstring targetName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int status = 1;

	const char* theSourceName = env->GetStringUTFChars(sourceName, 0);
	const char* theTargetName = env->GetStringUTFChars(targetName, 0);

	TRY
	{
		CFile::Rename( theSourceName, theTargetName );
	}
	CATCH( CFileException, e )
	{
		status = 0;
	}
	END_CATCH

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars(sourceName, theSourceName);
	env->ReleaseStringUTFChars(targetName, theTargetName);

	return status;
}



