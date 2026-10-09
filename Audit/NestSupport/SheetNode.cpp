// ==================================================================
//		SheetNode
//
// ==================================================================

#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "StringConst.h"
#include "SheetNode.h"
#include "DbTool.h"
#include "DbLine.h"
#include "PanelNode.h"

// ==================================================================

#define LAYER_PANELSAW "PANELSAW"
#define COLOR_PANELSAW 0x0000ff

// ==================================================================

CSheetNode::CSheetNode()
{
}

CSheetNode::~CSheetNode()
{
	Reset();
}


void
CSheetNode::Reset( void )
{
	m_nodelist.DestructiveFlush();
}

// ==================================================================
//		Place
//
//	Slice up this sheet with the most relevant panel in the list.
//	Makes a modified copy of the list to be passed on down the tree.
//
double
CSheetNode::Place( 
	const CNestConfig& config,
	CPanelList*			panel_list,
	int					depth )
{
	double	score = 0.0;

#ifdef CUT_THIS_OUT
CString tab = " ";
int add=depth-1;
while (add--)
	tab += " ";
TRACE( "S%s%02d: %g, %g\n", tab, depth, m_extent.Dx(), m_extent.Dy() );
#endif

	//
	// Find a part with (a) quantity>0, and (b) the lowest pass possible
	//
	bool did_cut = FALSE;
	for (int pass=1; pass<=config.PassNum(); pass++)
	{
		for (int idx=0; idx<panel_list->Count(); idx++)
		{
			const CPanelData& data = (*panel_list)[idx];

			//
			// Note that we ONLY attempt to cut on type of panel each pass:
			//	The panel type with the lowest pass number (there is no other priority
			//		encoding) that (a) has panels left to cut, and (b) fits.
			//
			// A POSSIBLE ENHANCEMENT to the algorithm would cut (and hence score) more
			// than one panel type at a time... but this would also explode the tree size.
			// Of course, with the dynamic pruning we do... we need to feel our way through
			// the issues.
			//
			if ( (data.Quantity() > 0)
				&& (data.Pass() == pass) )
			{
				//	... and get it's fit count for Horizontal and Vertical, regular and 90 rotate
				int xfit0 = (int)floor( m_extent.Dx() / data.Extent().Dx() );
				int yfit0 = (int)floor( m_extent.Dy() / data.Extent().Dy() );

				int xfit90 = 0;
				int yfit90 = 0;
				if (data.Rotation())
				{
					xfit90 = (int)floor( m_extent.Dx() / data.Extent().Dy() );
					yfit90 = (int)floor( m_extent.Dy() / data.Extent().Dx() );
				}

				// Pre-emptive pruning; assume we want the longest runs only
				int maxfit = 0;
				if ( (xfit0 > 0)
					&& (yfit0 > 0) )
					maxfit = max( xfit0, yfit0 );
				if ( (xfit90 > 0)
					&& (yfit90 > 0) )
					maxfit = max( maxfit, max(xfit90, yfit90) );

				//
				//	... and stuff any fits into the tree.
				// Note the scoring... each "cut" is given a score which reflects the
				// material used in that cut, divided by the pass depth.
				//
				if ( (xfit0 > 0)
					&& (yfit0 > 0) )
				{
					double subscore;

					if (yfit0 == maxfit)
					{
						subscore = do_place( config, *panel_list, idx, yfit0, TRUE, FALSE, depth );
						if (subscore > score)
							score = subscore;
						did_cut = TRUE;
					}

					if (xfit0 == maxfit)
					{
						subscore = do_place( config, *panel_list, idx, xfit0, FALSE, FALSE, depth );
						if (subscore > score)
							score = subscore;
						did_cut = TRUE;
					}
				}

				if ( (xfit90 > 0)
					&& (yfit90 > 0) )
				{
					double subscore;

					if (yfit90 == maxfit)
					{
						subscore = do_place( config, *panel_list, idx, yfit90, TRUE, TRUE, depth );
						if (subscore > score)
							score = subscore;
						did_cut = TRUE;
					}

					if (xfit90 == maxfit)
					{
						subscore = do_place( config, *panel_list, idx, xfit90, FALSE, TRUE, depth );
						if (subscore > score)
							score = subscore;
						did_cut = TRUE;
					}
				}

			} // end if pass && quantity

		} // end for idx

		// Only shuffle through the passes if we can't cut
		if (did_cut)
			break;

	} // end for pass

	//
	// Remove any nodes that are less than the high score.
	// This is where the tree is pruned... the score we see is
	// accumulated up through the tree, and is balanced for both
	// area used in that branch and the depth of the branch.
	//
	int idx, num = m_nodelist.Count()-1;
	for (idx=num; idx>=0; idx--)
	{
		CPanelNode* node = m_nodelist[idx];

		// TODO:  Floating-point fudge here?
		if ( node->Score() < score )
		{
			m_nodelist.Remove(idx);
			delete node;
		}
	}

	//
	// Final pruning.. we MUST have only one node branch, so delete
	// any but the first... but show a preference for a y-cut part!
	//	TODO: A more graceful way; sort by y then x, High score to Low. Then delete all but [0]
	//
	num = m_nodelist.Count()-1;
	int ynum = 0;
	// Count the forms in y
	for (idx=num; idx>=0; idx--)
	{
		CPanelNode* node = m_nodelist[idx];
		if (node->YCut())
			ynum++;
	}
	if (ynum > 0)
	{
		// we *have* in y, delete in x
		for (idx=num; idx>=0; idx--)
		{
			CPanelNode* node = m_nodelist[idx];
			if (!node->YCut())
			{
				m_nodelist.Remove(idx);
				delete node;
			}
		}
	}

	// Kill all but first
	num = m_nodelist.Count()-1;
	for (idx=num; idx>=1; idx--)
	{
		CPanelNode* node = m_nodelist.Remove(idx);
		delete node;
	}

	//
	// Finally, one remains... bubble the winning list of remaining
	//	panels up.
	//
	num = m_nodelist.Count();
	if (num)
	{
		CPanelNode* node = m_nodelist[0];

		*panel_list = node->PanelList();
	}

//TRACE( "*%s%02d===> %g\n", tab, depth, score );
	//
	// The score at this level of processing is the maximum score of
	// each way this sheet might have been cut... and since we pruned
	// the others out, this is the *only* way this sheet can be cut
	//

	return score;
}


// ==================================================================
double
CSheetNode::do_place( 
	const CNestConfig& config,
	const CPanelList&	panel_list,	// panel list, prior to cut
	int					idx,		// index in list we are cutting
	int					qty,		// quantity to cut
	bool				ycut,		// else xcut
	bool				rot90,		// else rot0
	int					depth )
{
	int	adj_qty = min( qty, panel_list[idx].Quantity() );

	CPanelNode*		node = new CPanelNode( panel_list, idx, adj_qty );

	double score = node->Place( config, m_extent, ycut, rot90, depth );

	m_nodelist.Append( node );

	return score;
}


// ==================================================================
//		Cut
//
//	Given a successful placement, cut it into the sheet, reduce
//	the partbin count, and return the number of panels cut.
//
//	Recursive, like the tree itself; the part nodes do all the work.
//
int			
CSheetNode::Cut(
	const CNestConfig&	config,
	CPartBin*			partbin,
	CSheet*				sheet,
	C2dBox*				scrap,
	CViewMgr&			view )
{
	int		parts_cut = 0;

	// At this point, reduced to a single branch...at most
	if (m_nodelist.Count())
	{
		CPanelNode*	node = m_nodelist[0];

		//
		// Major panel-saw cuts...
		//
		CModel*	dst_model = sheet->pModel();
		if (node->YCut())
		{
			saw_line( dst_model,
						C3dCoord( m_extent.Xmin() + node->Dx(), m_extent.Ymin(), 0.0 ),
						C3dCoord( m_extent.Xmin() + node->Dx(), m_extent.Ymax(), 0.0 ),
						view
					);

			//
			// Minor panel-saw cuts
			//
			C3dCoord cut( m_extent.Xmin(), m_extent.Ymin(), 0.0 );
			for (int idx=0; idx<node->Quantity();idx++)
			{
				cut += C3dVec( 0.0, node->Dy(), 0.0 );

				saw_line( dst_model,
							cut,
							cut + C3dVec( node->Dx(), 0.0, 0.0 ),
							view
						);
			}
		}
		else // xcut
		{
			saw_line( dst_model,
						C3dCoord( m_extent.Xmin(), m_extent.Ymin() + node->Dy(), 0.0 ), 
						C3dCoord( m_extent.Xmax(), m_extent.Ymin() + node->Dy(), 0.0 ),
						view
					);

			//
			// Minor panel-saw cuts
			//
			C3dCoord cut( m_extent.Xmin(), m_extent.Ymin(), 0.0 );
			for (int idx=0; idx<node->Quantity();idx++)
			{
				cut += C3dVec( node->Dx(), 0.0, 0.0 );

				saw_line( dst_model,
							cut,
							cut + C3dVec( 0.0, node->Dy(), 0.0 ),
							view
						);
			}
		}

		//
		// Place the panels themselves...
		//
		parts_cut += node->Cut(
								config, 
								partbin, 
								sheet, 
								scrap,
								C2dCoord( m_extent.Xmin(), m_extent.Ymin() ), 
								view );
	}
	else // No sub-node... this is scrap sheet!
	{
		double	this_area = m_extent.Dx()*m_extent.Dy();

		bool valid_scrap = (m_extent.Dx() > config.RemnantLength())
						&& (m_extent.Dy() > config.RemnantWidth())
						&& (this_area > config.RemnantArea());

		if (valid_scrap)
		{
			double	scrap_area = scrap->Dx()*scrap->Dy();

			if ( !scrap->IsDefined()
				&& (this_area > scrap_area)
				)
			{
				// Valid, and bigger than the current scrap (if any)
				// TODO:  Some notion of "squareness" to adjust the validity?
				*scrap = m_extent;
			}
		}
	}

	return parts_cut;
}


// ==================================================================
void
CSheetNode::saw_line(
	CModel*			model,
	const C3dCoord&	st,
	const C3dCoord& en,
	const CViewMgr&	view )
{
	CDbTool* dbTool = NULL;
	model->EntityFind( LAYER_PANELSAW, (CDbEntity**)&dbTool, DBTOOL, DBTOOL );
	if (!dbTool)
	{
		model->EntityCreate( DBTOOL, (CDbEntity**)&dbTool );
		dbTool->Name( LAYER_PANELSAW );
		dbTool->ColorSet( COLOR_PANELSAW );
	}
	if (!dbTool)
		return;

	CDbWorkplane* dbWork = NULL;
	model->EntityFind( STR_TOP, (CDbEntity**)&dbWork, DBWORKPLANE, DBWORKPLANE );
	if (!dbWork)
		dbWork = model->ActiveWorkplane();
	if (!dbWork)
		return;

	CDbLine* line = NULL;
	model->EntityCreate( DBLINE, (CDbEntity**)&line);
	if (!line)
		return;

	line->Init( dbTool, dbWork, st, en );
	line->ColorSet( COLOR_PANELSAW );

	view.ModelSet( *model );
	view.Refresh( line->Id(), FALSE );
}

// ==================================================================
void			
CSheetNode::Dump( 
	HANDLE	file, 
	int		in_depth )
{
	DWORD	dope;
	CString	prefix;
	CString	note;

	int depth = in_depth;
	while (depth--)
		prefix += "| ";

	note.Format( "%sSheet %d,%d .. %d,%d\n", 
					prefix, 
					(int)m_extent.Xmin(), 
					(int)m_extent.Ymin(),
					(int)m_extent.Xmax(), 
					(int)m_extent.Ymax()
					);
	::WriteFile( file, note, note.GetLength(), &dope, NULL );

	for (int idx=0; idx<m_nodelist.Count(); idx++)
		m_nodelist[idx]->Dump( file, in_depth+1 );
}




