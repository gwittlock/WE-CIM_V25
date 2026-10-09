#ifndef _LABELDBGENERATOR_H
#define _LABELDBGENERATOR_H

#include "Return.h"
#include "ModelClfile.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CLabelDbGenerator
{
public:

	CLabelDbGenerator();

	~CLabelDbGenerator();

	void Init( const CString& labelDbPath );

	CReturn LabelTableUpdate(
		const CString&		pdbPath,
		int					sheet_num,
		const CModelClfile&	clfile );

private:  // disabled

	CLabelDbGenerator( const CDaoDB& );
	CLabelDbGenerator& operator = ( const CLabelDbGenerator& );
	int operator == ( const CLabelDbGenerator& ) const;
	int operator != ( const CLabelDbGenerator& ) const;

private:

	CString	m_labelDbPath;
	CString	m_pdbPath;
	int		m_sheet_num;
};

#endif
