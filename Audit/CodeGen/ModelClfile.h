
#ifndef _MODELCLFILE_H
#define _MODELCLFILE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "stdafx.h"

#include "Clfile.h"
#include "IndxList.h"
#include "ClfileRec.h"
#include "Command.h"
#include "AutoModDb.h"
#include "SeqRules.h"
#include "WorkPkg.h"
#include "DbPattern.h"

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
class CSpeedCalc;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CModelClfile : public CClfile  
{
public:

	CModelClfile();

	virtual ~CModelClfile();

	CReturn Init( CSeqRules* seqRules );

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

	// Made public (V16) to support the Portal command CodeGen:Optimize:
	CReturn OptiCutOrder(
						const CWorkPkgArray&	workPkgs,
						CSeqRules*				seqRules,
						CDbEntityArray*			cutOrder );


	// Additions for CCS Fab.

	void RecordPartOrder( bool record )
		{ m_record_order = record; }

	int PartCount() const
		{ return m_order.Count(); }

	CDbPattern* PartGet( int indx ) const
		{ return m_order.GetAt(indx); }

	// For debugging.
	void Dump( const CString& path );

public:

	static CReturn	SequenceInit( CModel* model, bool addInsertMarkers );

	static void		InsertMarkersUpdate( CModel* model );

private:  // disabled

	CModelClfile& operator = ( const CModelClfile& );
	int operator == ( const CModelClfile& );
	int operator != ( const CModelClfile& );

private:  // methods

	CReturn MainBegin();
	CReturn MainEnd();
	CReturn SafeZoneInfo();
	CReturn ProgramStart();
	CReturn ProgramEnd();
	CReturn ProgramBody( const CDbEntityArray& mainOrder );
	CReturn Subdefs( const CDbEntityArray& subdefOrder );

	bool HasEvenContainmentParity( const CDbEntity* dbEntity );

	CReturn CutOrderProcess( const CDbEntityArray& cutOrder, bool subdefs );
	void	ConditionalSubdefBegin( const CDbEntity* dbEntity );
	void	ConditionalSubdefEnd( const CDbEntity* dbEntity );
	CReturn	Point( const CDbPoint* dbPoint );
	CReturn Line( const CDbLine* dbLine );
	CReturn Arc( const CDbArc* dbArc );
	CReturn Hole( const CDbHole* dbHole );
	CReturn	Command( const CDbCommand* dbCommand );
	CReturn UserCommandsProcess( CDbEntity* dbEntity, bool allow_repo );
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

	CReturn CutOrderGenerate(
						CSeqRules*				seqRules,
						CDbEntityArray*			mainOrder,
						CDbEntityArray*			subdefOrder );

	CReturn	OptiCutOrder(
						CSeqRules*				seqRules,
						CDbEntityArray*			mainOrder,
						CDbEntityArray*			subdefOrder );

	CReturn ExplicitCutOrder(
						CSeqRules*				seqRules,
						CDbEntityArray*			cutOrder );

	void	SubdefUpdate(
						CWorkPkg*				workPkg,
						CDbEntityArray*			optimizedEntities );

	void	SubcallsResolve(
						const CDbEntityArray&	mainOrder );

	CDbPattern* SubdefFind( const CDbEntity& dbEntity );

	void	CutOrderAppend(
						CDbFeature*				topLevelFeature,
						CDbEntityArray*			dbEntities,
						CDbEntityArray*			cutOrder );

	void AttribsCopyAppend( const CVarList* from, CVarList* to ) const;

	bool	IsExplicitlySequenced( const CDbEntityArray& workPkgEntities );
	void	CopyAndSort( const CDbEntityArray& workPkgEntities, CDbEntityArray* optimizedEntities );

	static int		QSortExpseqCompare( const void* ptrA, const void* ptrB );

	CReturn AdditionalFormatting();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// 'speed ramping' methods
	CReturn SpeedRampingApply();
	void SpeedPreproc( double sharp, double minRadius, ClfileRec* recA, ClfileRec* recB );
	int SpeedPostproc( int indx, const CSpeedCalc& calc );
	CReturn Accel( int ndivs, const CSpeedCalc& calc, int* indx );
	CReturn Decel( int ndivs, const CSpeedCalc& calc, int* indx );

	// For debugging speed ramping
	void CondSpeedRampShow( const C3dCoord& pt, const CSpeedCalc& calc, int indx );

	CReturn dogleg(const CDbEntity* db_ent );

	CDbPattern* PatternGet( const CDbEntity* dbEntity );
	CDbFeature* PartGet( const CDbEntity* dbEntity );

private:  // data

	CAutoModDb m_autoModDb;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// State information for generating the clfile data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CModel*		m_model;

	C3dCoord*	m_currGlobalPos;
	C3dCoord*	m_currLocalPos;
	CDbTool*	m_currTool;
	CDbEntity*	m_currEntity;

	bool		m_speedramp;
	bool		m_nibble;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// The clfile data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// The clfile records.
	ClfileRecList m_clfile;

	// Added for CCS Fab so that we can track the completion-order
	// of parts (so that physical part labels can be printed in order).
	CDbPatternArray	m_order;
	bool	m_record_order;
};

#endif

