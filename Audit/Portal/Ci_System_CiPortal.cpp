
#include "stdafx.h"
#include "Return.h"
#include "Portal.h"
#include "Ci_System_CiPortal.h"

#include "portable.h"

/*
 * Class:     Ci_System_CiPortal
 * Method:    Execute
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_Ci_System_CiPortal_Execute
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
 * Class:     Ci_System_CiPortal
 * Method:    IntGet
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_Ci_System_CiPortal_IntGet__Ljava_lang_String_2
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
 * Class:     Ci_System_CiPortal
 * Method:    IntGet
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL
Java_Ci_System_CiPortal_IntGet__Ljava_lang_String_2I
  (JNIEnv* env, jclass, jstring jParamName, jint jDefaultValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );

	int value;
	CReturn status = CPortal::GetInt( paramName, &value );
	if ( !status.IsOk() )
		value = jDefaultValue;

	if ( isCopy )
		env->ReleaseStringUTFChars( jParamName, 0 );

	return value;
}

/*
 * Class:     Ci_System_CiPortal
 * Method:    DoubleGet
 * Signature: (Ljava/lang/String;)D
 */
JNIEXPORT jdouble JNICALL
Java_Ci_System_CiPortal_DoubleGet__Ljava_lang_String_2
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
 * Class:     Ci_System_CiPortal
 * Method:    DoubleGet
 * Signature: (Ljava/lang/String;D)D
 */
JNIEXPORT jdouble JNICALL
Java_Ci_System_CiPortal_DoubleGet__Ljava_lang_String_2D
  (JNIEnv* env, jclass, jstring jParamName, jdouble jDefaultValue)
{
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );

	double value;
	CReturn status = CPortal::GetReal( paramName, &value );
	if ( !status.IsOk() )
		value = jDefaultValue;

	if ( isCopy )
		env->ReleaseStringUTFChars( jParamName, 0 );

	return value;
}
}

/*
 * Class:     Ci_System_CiPortal
 * Method:    StringGet
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_Ci_System_CiPortal_StringGet__Ljava_lang_String_2
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

/*
 * Class:     Ci_System_CiPortal
 * Method:    StringGet
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_Ci_System_CiPortal_StringGet__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv* env, jclass, jstring jParamName, jstring jDefaultValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const int MAXBUF = 1024;
	char buf[ MAXBUF ];

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );
	const char* defaultValue = env->GetStringUTFChars( jDefaultValue, &isCopy );

	CReturn status = CPortal::GetString( paramName, buf, MAXBUF );
	if ( !status.IsOk() )
		strncpy_s( buf, MAXBUF, defaultValue, MAXBUF );

	if ( isCopy )
	{
		env->ReleaseStringUTFChars( jParamName, 0 );
		env->ReleaseStringUTFChars( jDefaultValue, 0 );
	}

	return env->NewStringUTF( buf );
}
