
#include "stdafx.h"
#include "Return.h"
#include "Portal.h"
#include "JniPortal.h"


// ie. CiPortal.Execute( String command )
JNIEXPORT jboolean JNICALL Java_Weng_System_CiPortal_Execute
	(JNIEnv* env, jclass, jstring jCommand )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* command = env->GetStringUTFChars( jCommand, &isCopy );

	CReturn status = CPortal::Execute( command );

	if ( isCopy )
		env->ReleaseStringUTFChars( jCommand, 0 );

	return ( status.IsOk() );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// IntGet methods
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// ie. CiPortal.IntGet( String paramName )
JNIEXPORT jint JNICALL Java_Weng_System_CiPortal_IntGet__Ljava_lang_String_2
	(JNIEnv* env, jclass, jstring jParamName )
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

// ie. CiPortal.IntGet( String paramName, int defaultValue )
JNIEXPORT jint JNICALL Java_Weng_System_CiPortal_IntGet__Ljava_lang_String_2I
	(JNIEnv* env, jclass, jstring jParamName, jint defaultValue )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );

	int value;
	CReturn status = CPortal::GetInt( paramName, &value );
	if ( !status.IsOk() )
		value = defaultValue;

	if ( isCopy )
		env->ReleaseStringUTFChars( jParamName, 0 );

	return value;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// DoubleGet methods
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// ie. CiPortal.DoubleGet( String paramName )
JNIEXPORT jdouble JNICALL Java_Weng_System_CiPortal_DoubleGet__Ljava_lang_String_2
	(JNIEnv* env, jclass, jstring jParamName )
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

// ie. CiPortal.DoubleGet( String paramName, double defaultValue )
JNIEXPORT jdouble JNICALL Java_Weng_System_CiPortal_DoubleGet__Ljava_lang_String_2D
	(JNIEnv* env, jclass, jstring jParamName, jdouble defaultValue )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );

	double value;
	CReturn status = CPortal::GetReal( paramName, &value );
	if ( !status.IsOk() )
		value = defaultValue;

	if ( isCopy )
		env->ReleaseStringUTFChars( jParamName, 0 );

	return value;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// StringGet methods
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// ie. CiPortal.StringGet( String paramName )
JNIEXPORT jstring JNICALL Java_Weng_System_CiPortal_StringGet__Ljava_lang_String_2
	(JNIEnv* env, jclass, jstring jParamName )
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

// ie. CiPortal.StringGet( String paramName, String defaultValue )
JNIEXPORT jstring JNICALL Java_Weng_System_CiPortal_StringGet__Ljava_lang_String_2Ljava_lang_String_2
	(JNIEnv* env, jclass, jstring jParamName, jstring jDefaultValue )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const int MAXBUF = 1024;
	char buf[ MAXBUF ];

	jboolean isCopy;
	const char* paramName = env->GetStringUTFChars( jParamName, &isCopy );
	const char* defaultValue = env->GetStringUTFChars( jDefaultValue, &isCopy );

	CReturn status = CPortal::GetString( paramName, buf, MAXBUF );
	if ( !status.IsOk() )
		strncpy( buf, defaultValue, MAXBUF );

	if ( isCopy )
	{
		env->ReleaseStringUTFChars( jParamName, 0 );
		env->ReleaseStringUTFChars( jDefaultValue, 0 );
	}

	return env->NewStringUTF( buf );
}


