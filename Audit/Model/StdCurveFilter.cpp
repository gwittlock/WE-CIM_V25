
#include "stdafx.h"
#include "DbTool.h"
#include "StdCurveFilter.h"
#include "DbIterator.h"


////////////////////////////////////////////////////////////////////////

CStdCurveFilter::CStdCurveFilter()
	: m_list(),
	  m_seed( NULL )
{
	m_testflag = ( SCF_TSTHIDDEN
				 | SCF_TSTLAYER
				 | SCF_TSTOWNER
				 | SCF_TSTLOOSE );
}

CStdCurveFilter::~CStdCurveFilter()
{
}

void
CStdCurveFilter::Init( const CModel& model, CDbCurve* dbSeedCurve )
{
	CDbIterator iter;

	m_list.BenignFlush();
	m_seed = dbSeedCurve;

	iter.Init( model.Db(), DBLINE );
	while (1)
	{
		CDbEntity* dbEntity = iter();

		CDbCurve* candidate = dynamic_cast<CDbCurve*>( dbEntity );

		if (candidate == NULL)
			break;

		Add( candidate );

		iter.Next();
	}
}

void
CStdCurveFilter::Init( CDbCurveList* inputCurves, CDbCurve* dbSeedCurve )
{
	m_list.BenignFlush();
	m_seed = dbSeedCurve;

	int indx = 0;
	while (indx < inputCurves->Count())
	{
		CDbEntity* dbEntity = (*inputCurves)[ indx ];

		CDbCurve* candidate = dynamic_cast<CDbCurve*>( dbEntity );

		if (candidate == NULL)
			break;

		if ( Add( candidate ) )
			inputCurves->Remove( indx );
		else
			++indx;
	}
}

bool
CStdCurveFilter::Add( CDbCurve* candidate )
{
	if ( candidate->IsDeleted() )
		return FALSE;

	if ( TestHidden() )
	{
		if ( candidate->IsHidden() )
			return FALSE;

		// if (candidate->Tool()->IsLayer() && candidate->Tool()->IsHidden())
		if (candidate->Tool() == NULL)
			return FALSE;

		if (candidate->Tool()->IsHidden())
			return FALSE;
	}

	if ( TestLoose() )
	{
		if ( TestOwner()
			&& (candidate->Owner() != m_seed->Owner()))
			return FALSE;
	}
	else
	{
		if ( TestOwner()
			&& (candidate->Owner() != NULL)
			&& (candidate->Owner() != m_seed->Owner()) )
			return FALSE;  // The candidate already belongs to another container.
	}

	if ( TestLayer()
		&& (candidate->Tool() != m_seed->Tool()) )
		return FALSE;

	m_list.Append( candidate );

	return TRUE;
}

CDbCurveList&
CStdCurveFilter::Curves()
{
	return m_list;
}


CGeoCurveList*
CStdCurveFilter::GeoCurves()
{
	CGeoCurveList* geoList = new CGeoCurveList();

	int num = m_list.Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbCurve* dbCurve = m_list[idx];
		CGeoCurve* geoCurve = dbCurve->Curve();
		geoCurve->UserData(dbCurve);

		geoList->Append(geoCurve);
	}
	return geoList;
}
