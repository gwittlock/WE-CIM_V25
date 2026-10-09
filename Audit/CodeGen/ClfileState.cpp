
#include "stdafx.h"
#include "cmn_resource.h"
#include "Return.h"
#include "ClfileState.h"



////////////////////////////////////////////////////////////////////////

CClfileState::CClfileState( int recDataCount, int intDataCount, int dblDataCount )
	: m_recData(),
	  m_intData(),
	  m_dblData(),
	  m_attribs()
{
	m_recData.SetSize( recDataCount );
	m_intData.SetSize( intDataCount );
	m_dblData.SetSize( dblDataCount );
}

CClfileState::~CClfileState()
{
	m_attribs.Reset();
}


CReturn
CClfileState::Set(
			 const long* recData,
			 const long* intData,
			 const double* dblData,
			 const CVarList& attribs )
{
	CReturn status;

	int indx, count;

	count = m_recData.GetSize();
	for (indx = 0; indx < count; ++indx)
		m_recData[indx] = recData[indx];

	count = m_intData.GetSize();
	for (indx = 0; indx < count; ++indx)
		m_intData[indx] = intData[indx];

	count = m_dblData.GetSize();
	for (indx = 0; indx < count; ++indx)
		m_dblData[indx] = dblData[indx];

	m_attribs = attribs;

	return status;
}

CReturn
CClfileState::Get(
			 long* recData,
			 long* intData,
			 double* dblData,
			 CVarList* attribs )
{
	CReturn status;

	int indx, count;

	count = m_recData.GetSize();
	for (indx = 0; indx < count; ++indx)
		recData[indx] = m_recData[indx];

	count = m_intData.GetSize();
	for (indx = 0; indx < count; ++indx)
		intData[indx] = m_intData[indx];

	count = m_dblData.GetSize();
	for (indx = 0; indx < count; ++indx)
		dblData[indx] = m_dblData[indx];

	(*attribs) = m_attribs;

	return status;
}
