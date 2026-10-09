
#include "stdafx.h"
#include "ErrSys.h"
#include "Ci_System_ErrSys.h"

/*
 * Class:     Ci_System_ErrSys
 * Method:    IsValid
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_Ci_System_ErrSys_IsValid
  (JNIEnv* env, jobject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return ErrSysIsActive();
}

/*
 * Class:     Ci_System_ErrSys
 * Method:    Export
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_Ci_System_ErrSys_Export
  (JNIEnv* env, jobject, jstring buffer)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( !ErrSysIsActive() )
		return -1;

	const char* theString = env->GetStringUTFChars(buffer, 0);

	int status = ErrSysAdd( theString );

	// Failure to call ReleaseStringUTFChars() will
	// eventually lead to total loss of memory.
	//
	env->ReleaseStringUTFChars(buffer, theString);

	// TODO: Return a real ErrSys code so the jvm
	// will know if it is allowed to continue.
	//
	return status;
}
