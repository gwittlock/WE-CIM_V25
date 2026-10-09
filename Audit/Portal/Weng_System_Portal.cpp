
#include "stdafx.h"
#include "Return.h"
#include "Portal.h"
#include "Weng_System_Portal.h"
/*
 * Class:     Weng_System_Portal
 * Method:    Execute
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Weng_System_Portal_Execute
  (JNIEnv* env, jclass, jstring jCommand)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* command = env->GetStringUTFChars( jCommand, &isCopy );

	CReturn status = CPortal::Execute( command );

	if ( isCopy )
		env->ReleaseStringUTFChars( jCommand, 0 );

	return ( status.IsOk() );
}

/*
 * Class:     Weng_System_Portal
 * Method:    IntGet
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_Weng_System_Portal_IntGet
  (JNIEnv* env, jclass, jstring jParamName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );

	int value;
	CReturn status = CPortal::GetInt( paramName, &value );

	if ( isCopy )
		env->ReleaseStringUTFChars( jParamName, 0 );

	return value;
}

/*
 * Class:     Weng_System_Portal
 * Method:    DoubleGet
 * Signature: (Ljava/lang/String;)D
 */
JNIEXPORT jdouble JNICALL Java_Weng_System_Portal_DoubleGet
  (JNIEnv* env, jclass, jstring jParamName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );

	double value;
	CReturn status = CPortal::GetReal( paramName, &value );

	if ( isCopy )
		env->ReleaseStringUTFChars( jParamName, 0 );

	return value;
}

/*
 * Class:     Weng_System_Portal
 * Method:    StringGet
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_Weng_System_Portal_StringGet
  (JNIEnv* env, jclass, jstring jParamName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const int MAXBUF = 1024;
	char buf[ MAXBUF ];

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );

	CReturn status = CPortal::GetString( paramName, buf, MAXBUF );

	if ( isCopy )
		env->ReleaseStringUTFChars( jParamName, 0 );

	return env->NewStringUTF( buf );
}
