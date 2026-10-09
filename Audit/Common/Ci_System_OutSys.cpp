
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "OutSys.h"
#include "Ci_System_OutSys.h"

JNIEXPORT jstring JNICALL Java_Ci_System_OutSys_Path( JNIEnv* env, jobject obj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COutputHandler* oh = (COutputHandler*) OutSysHandler();
	CString path = ((oh == NULL) ? "" : oh->Path());

	return env->NewStringUTF( (LPCSTR) path );
}


JNIEXPORT jint JNICALL Java_Ci_System_OutSys_Open( JNIEnv* env, jobject obj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COutputHandler* oh = (COutputHandler*) OutSysHandler();
	BOOL status = ((oh == NULL) ? -1 : oh->Open( false, false ));

	return status;
}

JNIEXPORT jint JNICALL Java_Ci_System_OutSys_OpenAsBinary( JNIEnv* env, jobject obj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COutputHandler* oh = (COutputHandler*) OutSysHandler();
	BOOL status = ((oh == NULL) ? -1 : oh->Open( true, false ));

	return status;
}

JNIEXPORT void JNICALL Java_Ci_System_OutSys_Close( JNIEnv* env, jobject obj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COutputHandler* oh = (COutputHandler*) OutSysHandler();
	if (oh != NULL)
		oh->Close();
}

JNIEXPORT jint JNICALL Java_Ci_System_OutSys_Type( JNIEnv* env, jobject obj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COutputHandler* oh = (COutputHandler*) OutSysHandler();
	return ((oh == NULL) ? 0 : oh->Type());
}

JNIEXPORT jboolean JNICALL
Java_Ci_System_OutSys_IsValid( JNIEnv* env, jobject obj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COutputHandler* oh = (COutputHandler*) OutSysHandler();
	BOOL status = (oh != NULL);
	return status;
}

JNIEXPORT jint JNICALL
Java_Ci_System_OutSys_Export( JNIEnv* env, jobject obj, jstring buffer )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COutputHandler* oh = (COutputHandler*) OutSysHandler();
	if (oh == NULL)
		return -1;

	const char* theString = env->GetStringUTFChars(buffer, 0);

	int status = oh->Put( theString );

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars(buffer, theString);

	return status;
}



