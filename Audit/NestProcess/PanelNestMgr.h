#if !defined(_PANELNESTMGR_H)
#define _PANELNESTMGR_H

// ==================================================================
//		PanelNestMgr
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common.h"
#include "Command.h"
#include "NestConfig.h"
#include "Sheet.h"
#include "NestingPart.h"

#include "SheetNode.h"

// ==================================================================

class CPanelNestMgr
{
public:
	CPanelNestMgr();
	virtual ~CPanelNestMgr();

	CReturn		Execute(
					CCommand*			io_cmd, 
					CNestConfig&		config, 
					CSheet*				sheet, 
					CPartBin*			partbin,
					CViewMgr&			view );

	void		Dump( const CString& filename );

protected:

private:
	// Disabled.
	CPanelNestMgr( const CPanelNestMgr& );
	const CPanelNestMgr& operator = ( const CPanelNestMgr& );
	int operator == ( const CPanelNestMgr& ) const;
	int operator != ( const CPanelNestMgr& ) const;

	void	fill_panel_list( 
							CNestConfig&		config, 
							const CPartBin&		partbin, 
							CPanelList*			panel_list );

	void	update_panel_list( 
							const CNestConfig&		config, 
							const CPartBin&			partbin, 
							CPanelList*				panel_list );

	int		duplicate_sheet( 
							CPartBin*	partbin,
							CSheet*		sheet );

	CReturn	clear_results(
						const CString&	database );

	void	save_results(
							const CString&			database,
							const CSheet&			sheet,
							const CPartBin&			partbin );

	void	remnant_sheet(
							CNestConfig&		config, 
							CSheet*				sheet,
							const C2dBox&		scrap,
							CViewMgr&			view );

private:
	// Solution Root Node
	CSheetNode	m_sheet;
};

#endif

