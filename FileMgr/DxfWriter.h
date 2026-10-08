#ifndef _DXFWRITER_H
#define _DXFWRITER_H

#include "Return.h"
#include "Model.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CDxfWriter
{
public:

	CDxfWriter();

	~CDxfWriter();

	CReturn Write(
		const CString	path,
		const CModel&	model,
		int				mantissa,
		bool			layers );

private:

	CReturn Init( const CString path, const CModel& model, int mantissa );

	void Terminate();

	void LayersDump();

	void ModelDump();

	void LinesDump();

	void ArcsDump();

	void HolesDump();

	void LineDump( CString layerName, const C3dCoord& ps, const C3dCoord& pe );

	void StringDump( CString sval );

	void DoubleDump( double dval );

private:

	CString	m_path;

	FILE*	m_file;

	const CEntityDb*	m_db;

	CString	m_fmt;

	bool	m_ACDB;
};

#endif
