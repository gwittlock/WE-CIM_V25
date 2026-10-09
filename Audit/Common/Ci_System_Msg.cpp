
#include "stdafx.h"
#include "Return.h"
#include "Register.h"
#include "Msg.h"
#include "Ci_System_Msg.h"


JNIEXPORT jboolean JNICALL Java_Ci_System_Msg_IsMessageActive
  (JNIEnv *, jclass)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return MsgIsActive();
}

/*
 * Class:     Ci_System_Msg
 * Method:    MessageMsg
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_Ci_System_Msg_MessageMsg
  (JNIEnv* env, jclass, jstring msg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( MsgIsActive() )
	{
		const char* theString = env->GetStringUTFChars( msg, 0 );

		MsgDisplay( theString );

		// Failure to call ReleaseStringUTFChars() will
		// eventually lead to total loss of memory.
		//
		env->ReleaseStringUTFChars( msg, theString );
	}
}


/*
 * Class:     Ci_System_Msg
 * Method:    IsDebugActive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_Ci_System_Msg_IsDebugActive
  (JNIEnv* env, jclass)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString value = CRegister::StringGetV( "Java", "Debug", "" );

	return ( !value.IsEmpty() );
}


/*
 * Class:     Ci_System_Msg
 * Method:    DebugMsg
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_Ci_System_Msg_DebugMsg
  (JNIEnv* env, jclass, jstring msg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const char* theMsg = env->GetStringUTFChars( msg, 0 );

	MessageBox( NULL, theMsg, "Debug", (MB_OK|MB_ICONEXCLAMATION) );

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars( msg, theMsg );
}


/*
 * Class:     Ci_System_Msg
 * Method:    DiagnosticMsg
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_Ci_System_Msg_DiagnosticMsg
  (JNIEnv* env, jclass, jstring msg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const char* theMsg = env->GetStringUTFChars( msg, 0 );

	CReturn status;
	status.Diagnostic( theMsg );

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars( msg, theMsg );
}

