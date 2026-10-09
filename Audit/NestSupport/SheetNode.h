#if !defined(_SHEETNODE_H)
#define _SHEETNODE_H

// ==================================================================
//		SheetNode
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPanelNode;

#include "Common.h"

#include "NestConfig.h"
#include "PanelList.h"
#include "PartBin.h"
#include "Sheet.h"
#include "ViewMgr.h"
#include "IndxList.h"
#include "2dBox.h"

// ==================================================================

class dllExport CSheetNode
{
public:
	CSheetNode();
	virtual ~CSheetNode();

	void		Reset( void );

	double		Place( 
					const CNestConfig& config,
					CPanelList*			panel_list,
					int					depth );


	int			Cut(
					const CNestConfig&	config,
					CPartBin*			partbin,
					CSheet*				sheet,
					C2dBox*				scrap,
					CViewMgr&			view );

	void			Extent( const C2dBox& extent )			{ m_extent = extent; }
	const C2dBox&	Extent( void ) const					{ return m_extent; }

	void			Dump( HANDLE file, int depth );

protected:

private:
	// Disabled.
	CSheetNode( const CSheetNode& );
	const CSheetNode& operator = ( const CSheetNode& );
	int operator == ( const CSheetNode& ) const;
	int operator != ( const CSheetNode& ) const;

	double	do_place( 
					const CNestConfig& config,
					const CPanelList&	panel_list, 
					int					idx,
					int					qty,
					bool				ycut,
					bool				rot90,
					int					depth );


	void	saw_line(
					CModel*			model,
					const C3dCoord&	st,
					const C3dCoord& en,
					const CViewMgr&	view );

private:
	CIndxList<CPanelNode*>	m_nodelist;		// List of parts that may be nested

	C2dBox					m_extent;
};

#endif

