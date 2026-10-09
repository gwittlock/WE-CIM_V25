#if !defined(_PANELNODE_H)
#define _PANELNODE_H

// ==================================================================
//		PanelNode
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSheetNode;

#include "Common.h"

#include "IndxList.h"
#include "PanelList.h"
#include "NestConfig.h"
#include "PartBin.h"
#include "Sheet.h"
#include "ViewMgr.h"
#include "2dBox.h"

// ==================================================================

class dllExport CPanelNode
{
public:
	CPanelNode( const CPanelList& list, int idx, int qty );
	virtual ~CPanelNode();

	double	Place( 
					const CNestConfig& config,
					const C2dBox&		sheet_ext,
					bool				ycut,
					bool				rot90,
					int					depth );

	int		Cut(
				const CNestConfig&	config,
				CPartBin*			partbin,
				CSheet*				sheet,
				C2dBox*				scrap,
				const C2dCoord&		pos,
				CViewMgr&			view );

	double			Score( void ) const					{ return m_score; }
	int				Index( void ) const					{ return m_idx; }
	int				Quantity( void ) const				{ return m_quantity; }
	bool			YCut( void ) const					{ return m_ycut; }

	double			Dx( void ) const					{ return m_extent.Dx(); }
	double			Dy( void ) const					{ return m_extent.Dy(); }

	const CPanelList& PanelList( void ) const			{ return m_list; }

	void			Dump( HANDLE file, int depth );

protected:

private:
	// Disabled.
	CPanelNode();
	CPanelNode( const CPanelNode& );
	const CPanelNode& operator = ( const CPanelNode& );
	int operator == ( const CPanelNode& ) const;
	int operator != ( const CPanelNode& ) const;

	void	do_cut( 
					CSheet*				sheet, 
					CNestingPart*		part, 
					int					partnum,
					const C2dCoord&		pos,
					CViewMgr&			view );

	void	render_part( 
						CModel&		model,
						CViewMgr&	view,
						CDbEntity*	db_ent );

private:
	CPanelList				m_list;				// Current list of panels, less this one
	int						m_idx;				// Index of this panel in the list
	int						m_quantity;			// How many cut
	bool					m_ycut;				// Remember...
	bool					m_rot90;			// reMemBer...

	double					m_score;			// quantity * area

	CPanelData*				m_data;				// Handy reference to this node in the list
	C2dBox					m_extent;			// Another handy reference

	CIndxList<CSheetNode*>	m_sheetlist;		// List of sheets fragmented by this node
};

#endif

