
#ifndef _TREEVIEWSUPPORT_H
#define _TREEVIEWSUPPORT_H

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

class CModel;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CTreeViewSupport
{
public:

	CTreeViewSupport();

	void Init( CModel& model );

	int Count() const;

	CDbEntity* operator [] ( int indx ) const;

	void Rebuild();

	virtual ~CTreeViewSupport();

private:  // Methods

	CReturn Select( CDbEntity* dbEntity, bool checkAllowable );

	bool IsAllowed( const CDbEntity* dbEntity );

	CReturn Add( CDbEntity* dbEntity );

private:  // Data.

	CModel* m_model;

	CDbEntityList m_list;

	bool m_isAllowed[ DBTERMINAL ];
};

#endif
