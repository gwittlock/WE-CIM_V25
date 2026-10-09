
#include "stdafx.h"
#include <stdio.h>
#include "cmn_resource.h"
#include "MathConst.h"
#include "Msg.h"
#include "Var.h"
#include "VarList.h"
#include "DbEntity.h"
#include "DbFeature.h"
#include "Clfile.h"
#include "ClfileState.h"
#include "Weng_CodeGen_Clfile.h"

// A pointer (conceptually a reference) to the applications' clfile object.
static CClfile* m_weng_clfile = NULL;

// A stack of clfile states supporting look-ahead functionality.
static CClfileStateList m_weng_clfileStates;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void DLL_DECL
WENG_ClfileInit( CClfile* clfile )
{
	m_weng_clfile = clfile;
}

void DLL_DECL
WENG_ClfileTerm()
{
	m_weng_clfile = NULL;
	m_weng_clfileStates.DestructiveFlush();
}


JNIEXPORT jint JNICALL
Java_Weng_CodeGen_Clfile_Count( JNIEnv* env, jobject )
{
	return (jint) ((m_weng_clfile == NULL) ? 0 : m_weng_clfile->Count());
}

/*
 * Class:     Clfile
 * Method:    Read
 * Signature: (I[I[D)I
 * Notes:
 *		Unfortunate goofiness of JNI, can't pass integers
 *      only arrays of integers, hence 'jintArray recType'.
 */
JNIEXPORT jint JNICALL
Java_Weng_CodeGen_Clfile_Read(
				  JNIEnv* env, jobject,
				  jint         recNo,
				  jintArray    recInfo,
				  jintArray    intArray,
				  jdoubleArray dblArray )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString msg;


	// Initially, class CPartClfile was designed to
	// buffer the clfile data values.  The buffering
	// has since been delegated to the client, thereby
	// greatly reducing the number of calls, from Java
	// to C++, to obtain a clfile data.  Otherwise, a
	// call would have to made for each data item.

	jint* iRecInfo = env->GetIntArrayElements( recInfo, 0 );

	if (recNo == m_weng_clfile->Count())
	{
		// Assume the clfile is be read sequentially
		// from its beginning.  Attempting to read
		// this record will therefore indicate that
		// we are at the end of the clfile.  Attempts
		// to read beyond this point will cause
		// Clfile::Read() to issue an error indicating
		// that the requested record is 'out of range'.

		iRecInfo[0] = -1;  // record event type
		iRecInfo[1] =  0;  // id of entity associated with record
		recNo = -1;
	}
	else
	{
		jint* iArray = env->GetIntArrayElements( intArray, 0 );
		jsize iArraySize = env->GetArrayLength( intArray );

		jdouble* dArray = env->GetDoubleArrayElements( dblArray, 0 );
		jsize dArraySize = env->GetArrayLength( dblArray );

		recNo = m_weng_clfile->Read(
			recNo, iRecInfo, iArray, iArraySize, dArray, dArraySize );

		env->ReleaseIntArrayElements( intArray, iArray, 0 );
		env->ReleaseDoubleArrayElements( dblArray, dArray, 0 );
	}

	env->ReleaseIntArrayElements( recInfo, iRecInfo, 0 );

	return recNo;
}

JNIEXPORT jint JNICALL
Java_Weng_CodeGen_Clfile_Int(
				  JNIEnv* env, jobject,
				  jstring jAttrName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* attrName = env->GetStringUTFChars( jAttrName, &isCopy );

	CVar* var = m_weng_clfile->Attrib().getVar( attrName );

	jint iValue = ((var == NULL) ? IUNDEFINED : var->getInt());

	if ( isCopy )
		env->ReleaseStringUTFChars( jAttrName, 0 );
	
	return iValue;
}

JNIEXPORT jdouble JNICALL
Java_Weng_CodeGen_Clfile_Dbl(
				  JNIEnv* env, jobject,
				  jstring jAttrName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* attrName = env->GetStringUTFChars( jAttrName, &isCopy );

	CVar* var = m_weng_clfile->Attrib().getVar( attrName );

	jdouble dValue = ((var == NULL) ? UNDEFINED : var->getReal());

	if ( isCopy )
		env->ReleaseStringUTFChars( jAttrName, 0 );
	
	return dValue;
}

JNIEXPORT jstring JNICALL
Java_Weng_CodeGen_Clfile_Str(
				  JNIEnv* env, jobject,
				  jstring jAttrName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	jboolean isCopy;
	const char* attrName = env->GetStringUTFChars( jAttrName, &isCopy );

	CVar* var = m_weng_clfile->Attrib().getVar( attrName );

	CString sValue = ((var == NULL) ? "" : var->getString());

	if ( isCopy )
		env->ReleaseStringUTFChars( jAttrName, 0 );

	return env->NewStringUTF( sValue );
}

JNIEXPORT void JNICALL
Java_Weng_CodeGen_Clfile_StatePop(
					JNIEnv* env, jobject,
					jintArray    recDataArray,
					jintArray    intDataArray,
					jdoubleArray dblDataArray )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
		
	jint* recData = env->GetIntArrayElements( recDataArray, 0 );
	jsize recDataSize = env->GetArrayLength( recDataArray );

	jint* intData = env->GetIntArrayElements( intDataArray, 0 );
	jsize intDataSize = env->GetArrayLength( intDataArray );

	jdouble* dblData = env->GetDoubleArrayElements( dblDataArray, 0 );
	jsize dblDataSize = env->GetArrayLength( dblDataArray );

	//=-=-=

	if (m_weng_clfileStates.Count() > 0)
	{
		CClfileState* state = m_weng_clfileStates.Remove( 0 );

		state->Get( recData, intData, dblData, m_weng_clfile->pAttrib() );

		delete state;
	
		// Synchronize the clfile object state with the rest
		// of the world.  Otherwise, the application's View/Editor
		// will display the wrong entity when a line of NC code
		// is selected.
		m_weng_clfile->Fetch( (int) recData[0] );
	}
	else
	{
		CReturn status;
		status.User( IDS_INTERNAL_ERROR, "Clfile state stack is empty" );
	}

	//=-=-=

	env->ReleaseIntArrayElements( recDataArray, recData, 0 );
	env->ReleaseIntArrayElements( intDataArray, intData, 0 );
	env->ReleaseDoubleArrayElements( dblDataArray, dblData, 0 );
}

/*
 * Class:     Bbi_CodeGen_Clfile
 * Method:    StatePush
 * Signature: ([I[I[D)V
 */
JNIEXPORT void JNICALL
Java_Weng_CodeGen_Clfile_StatePush(
					JNIEnv* env, jobject,
					jintArray    recDataArray,
					jintArray    intDataArray,
					jdoubleArray dblDataArray )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
		
	jint* recData = env->GetIntArrayElements( recDataArray, 0 );
	jsize recDataSize = env->GetArrayLength( recDataArray );

	jint* intData = env->GetIntArrayElements( intDataArray, 0 );
	jsize intDataSize = env->GetArrayLength( intDataArray );

	jdouble* dblData = env->GetDoubleArrayElements( dblDataArray, 0 );
	jsize dblDataSize = env->GetArrayLength( dblDataArray );

	//=-=-=

	CClfileState* state = new CClfileState( recDataSize, intDataSize, dblDataSize );

	state->Set( recData, intData, dblData, m_weng_clfile->Attrib() );

	m_weng_clfileStates.Prepend( state );

	//=-=-=

	env->ReleaseIntArrayElements( recDataArray, recData, 0 );
	env->ReleaseIntArrayElements( intDataArray, intData, 0 );
	env->ReleaseDoubleArrayElements( dblDataArray, dblData, 0 );
}

