#ifndef _CUTBACK_H
#define _CUTBACK_H

#include "Return.h"
#include "DbAllEntities.h"
#include "model.h"

class dllExport CCutBack
{
public:

	CCutBack();

	~CCutBack();

	void ModelSet( CModel* model );

	CDbLine* CutBackLineGet() const;

	CReturn Update( int station_id, double xp );

	CReturn Delete();

private:

	CDbFeature* MaxWorkzoneGet() const;

	CReturn CutBackCreate( int station_id, double xp );
	CReturn CutBackModify( int station_id, double xp, CDbLine* cutback );

	bool CanDelete( CDbFeature* dbFeature ) const;

	void WorkzoneUpdate( const C2dBox& box, CDbFeature* dbZone );
	CDbFeature* CutbackWorkzoneCreate( const CDbFeature* max_workzone, CDbLine* cutback_line );

private:  // disabled

	CCutBack( const CCutBack& rhs );
	CCutBack& operator = ( const CCutBack& rhs );
	bool operator == ( const CCutBack& rhs );
	bool operator != ( const CCutBack& rhs );

private:

	CModel*	m_model;
};

#endif
