
#ifndef _MODELCLFILE_H
#define _MODELCLFILE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "stdafx.h"

#ifndef _CLFILE_H
#include "Clfile.h"
#endif

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _CLFILEREC_H
#include "ClfileRec.h"
#endif

#ifndef _COMMAND_H
#include "Command.h"
#endif

#ifndef _AUTOMODDB_H
#include "AutoModDb.h"
#endif

#include "SeqRules.h"
#include "WorkPkg.h"

class CReturn;
class CDbTool;
class CDbFeature;
class CDbProfile;
class CDbPoint;
class CDbLine;
class CDbArc;
class CDbHole;
class CModel;
class CVarList;
class CProfile;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CCiClfile : public CClfile  
{
public:

	CCiClfile();

	CReturn Init( CModel* model, bool code_top );

	/////////////////////////////////////////////
	// Obtain the total number of records.
	//
	virtual int Count() const;

	virtual int CurrRecNo() const;

	virtual const ClfileRec* Fetch( int recNo ) const;

	/////////////////////////////////////////////
	// Read a record and transfer its 
	// values to the given buffers.
	// 
	// The default calling convention Read( recNo < 0 )
	// simply reads the next record.
	//
	// The other calling convention Read( recNo >= 0 )
	// reads the specified record.
	//
	// NOTE: Some arguments are 'long' instead of 'int'
	// because Java's 'jint' is a 'long'.
	//
	virtual int Read( long				recNo,
					  long*				recInfo,
					  long*				intArray,
					  long				intArraySize,
					  double*			dblArray,
					  long				dblArraySize ) const;

	void Dump( const CString& path );

	virtual ~CCiClfile();

private:  // methods

	CReturn CutOrderGenerate( CDbEntityArray* mainOrder, bool code_bottom );

	CReturn MainBegin();
	CReturn MainEnd();
	CReturn SafeZoneInfo();
	CReturn ProgramStart();
	CReturn ProgramEnd();
	CReturn ProgramBody( const CDbEntityArray& mainOrder );

	CReturn CutOrderProcess( const CDbEntityArray& cutOrder );
	CReturn	Point( const CDbPoint* dbPoint );
	CReturn Line( const CDbLine* dbLine );
	CReturn Arc( const CDbArc* dbArc );
	CReturn Hole( const CDbHole* dbHole );
	CReturn	Command( const CDbCommand* dbCommand );
	CReturn UserCommandsProcess( CDbEntity* dbEntity );
	CReturn ToolChange( const CDbEntity* dbEntity );

	CReturn Transition( const CDbEntity* next );

	void CurrPosUpdate( const C3dCoord& globalPos, const C3dCoord& localPos );

	CReturn RecAppend( int recType, ClfileRec** rec );
	CReturn RecInsertBefore( int indx, int recType, ClfileRec** rec );
	CReturn RecInsertAfter( int indx, int recType, ClfileRec** rec );

	//=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Clfile access helper methods.
	//=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	void DataTransfer( const ClfileRec& rec,
					   long*    intArrray,
					   long     intArraySize,
					   double*  dblArray,
					   long     dblArraySize ) const;

	int TagOutOfRange( const char* func, int tag, int upperBound ) const;
	int RecordOutOfRange( const char* func, int tag ) const;

	void	CutOrderAppend(
						CDbFeature*				topLevelFeature,
						CDbEntityArray*			dbEntities,
						CDbEntityArray*			cutOrder );

	void AttribsCopyAppend( const CVarList* from, CVarList* to ) const;

	bool	IsExplicitlySequenced( const CDbEntityArray& workPkgEntities );
	void	CopyAndSort( const CDbEntityArray& workPkgEntities, CDbEntityArray* optimizedEntities );

	static int		QSortExpseqCompare( const void* ptrA, const void* ptrB );

private:  // disabled

	CCiClfile& operator = ( const CCiClfile& );
	int operator == ( const CCiClfile& );
	int operator != ( const CCiClfile& );

private:  // data

	CAutoModDb m_autoModDb;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// State information for generating the clfile data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CModel* m_model;

	C3dCoord* m_currGlobalPos;
	C3dCoord* m_currLocalPos;
	CDbTool*  m_currTool;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// The clfile data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// The clfile records.
	ClfileRecList m_clfile;
};

#endif

