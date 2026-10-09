// ==================================================================
//		PanelNode
//
// ==================================================================

#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"

#include "DbTool.h"
#include "PanelNode.h"
#include "SheetNode.h"
#include "DbIterator.h"
#include "DbCommand.h"

// ==================================================================
// score = score / (BASE + depth*FACTOR)
#define DEPTH_BASE		1	
#define DEPTH_FACTOR	.1

// ==================================================================


CPanelNode::CPanelNode(
	const CPanelList&	list, 
	int					idx, 
	int					qty )
	: m_list( list ),
	  m_idx( idx ),
	  m_quantity( qty )
{
	// m_list holds the original values... cut out this panel
	m_data = m_list.pPanelData( m_idx );

//TRACE("NODE %d: %g, %g\n", idx, m_data->Extent().Dx(), m_data->Extent().Dy() );

	m_data->Quantity( m_data->Quantity() - m_quantity );
}

CPanelNode::~CPanelNode()
{
	m_sheetlist.DestructiveFlush();
}


// ==================================================================
//		Place
//
//	Given original sheet dimensions, and cutting instructions,
//	create two sub-sheets and then nest on *those*.
//
double
CPanelNode::Place( 
	const CNestConfig& config,
	const C2dBox&		sheet_ext,	// Current sheet extents
	bool				ycut,		// Cut along y-axis or x?
	bool				rot90,		// Cut part rotated 90' or base position?
	int					depth )
{
	double	score = 0.0;

	C2dBox	panel_ext = m_data->Extent();

	//
	// Determine the used extend of this panel; depends on whether
	// used as given, or rotated 90 degrees.  This information is kept
	// redundently; both the flag, and the rotated extent, for ease of use.
	//
	if (rot90)
	{
		m_extent = C2dBox(	panel_ext.Ymin(),
							panel_ext.Xmin(),
							panel_ext.Ymax(),
							panel_ext.Xmax() );
	}
	else
		m_extent = panel_ext;

	m_ycut = ycut;
	m_rot90 = rot90;

	//
	// The score of this cut is the area of the panel being cut,
	// times the number of placements of that panel.
	//
	m_score = m_quantity * m_extent.Dx() * m_extent.Dy();

	//
	// Create up two two sub-sheets, based on ycut flag and current extents
	// These are the scraps left over after making this cut.
	//
	CSheetNode*	sub_sheet;

	// Minor remnant (at end of panels)
	sub_sheet = new CSheetNode;
	if (ycut)
		sub_sheet->Extent( C2dBox(	sheet_ext.Xmin(),
									sheet_ext.Ymin() + (m_quantity * m_extent.Dy()), 
//									sheet_ext.Xmin() + m_extent.Xmax(), 
									sheet_ext.Xmin() + m_extent.Dx(), 
									sheet_ext.Ymax() ) );
	else // xcut
		sub_sheet->Extent( C2dBox(	sheet_ext.Xmin() + (m_quantity * m_extent.Dx()),
									sheet_ext.Ymin(), 
									sheet_ext.Xmax(),
									sheet_ext.Ymin() + m_extent.Dy() ) );

	// Only keep this sheet if it is a valid dimension
//	if ( (sub_sheet->Extent().Dx() > 0)
//		&& (sub_sheet->Extent().Dy() > 0) )
	if ( (sub_sheet->Extent().Dx() >= config.Narrow(1))
		&& (sub_sheet->Extent().Dy() >= config.Narrow(1)) )
	{
		m_sheetlist.Append( sub_sheet );

		//
		// Recursion... this sub-sheet is immediately re-cut.
		//
		score += sub_sheet->Place( config, &m_list, depth+1 );
	}

	// ---------

	// Major remnant (alongside panels)
	sub_sheet = new CSheetNode;
	if (ycut)
		sub_sheet->Extent( C2dBox(	sheet_ext.Xmin() + m_extent.Dx(),
									sheet_ext.Ymin(), 
									sheet_ext.Xmax(),
									sheet_ext.Ymax() ) );
	else // xcut
		sub_sheet->Extent( C2dBox(	sheet_ext.Xmin(),
									sheet_ext.Ymin() + m_extent.Dy(), 
									sheet_ext.Xmax(),
									sheet_ext.Ymax() ) );
	// Only keep this sheet if it is a valid dimension
//	if ( (sub_sheet->Extent().Dx() > 0)
//		&& (sub_sheet->Extent().Dy() > 0) )
	if ( (sub_sheet->Extent().Dx() >= config.Narrow(1))
		&& (sub_sheet->Extent().Dy() >= config.Narrow(1)) )
	{
		m_sheetlist.Append( sub_sheet );

		//
		// Recursion... this sub-sheet is immediately re-cut.
		//
		score += sub_sheet->Place( config, &m_list, depth+1 );
	}

	//
	// The total score (lower branch, plus this panel) is now adjusted based
	// on the current recursion depth.
	//
	m_score /= (DEPTH_BASE + depth*DEPTH_FACTOR);

	//
	// The panel score, m_score, was described above.  The *other* score is
	// the sum of any sheet scores above (each of which, of course, is the value
	// of the highest node trial on that sheet).  Sure it's funky, but damn it works.
	//
	m_score += score;

	//
	// NOTE:  We *may* want to reverse the two operations, though I think I have
	//	it right.
	//
	//		m_score /= depth
	//		m_score += score
	//
	// to be:
	//
	//		m_score += score
	//		m_score /= depth
	//

	return m_score;
}

// ==================================================================
//		Cut
//
//	Cut this node into the "real" sheet, reduce
//	the partbin count, and return the number of panels cut.
//
//	Recursive, like the tree itself.
//
int			
CPanelNode::Cut(
	const CNestConfig&	config,
	CPartBin*			partbin,
	CSheet*				sheet,
	C2dBox*				scrap,
	const C2dCoord&		pos,
	CViewMgr&			view )
{
	CNestingPart* part = partbin->GetAt( m_idx );

	//
	// "use up" the part in question
	//
	part->Used( part->Used() + m_quantity );
	part->Quantity( part->Quantity() - m_quantity );

	int idx, parts_cut = m_quantity;

	//
	// Place the model into the sheet
	//
	C2dCoord	place = pos;
	for (idx=0; idx<m_quantity; idx++)
	{
		do_cut( sheet, part, m_idx, place, view );

		if (m_ycut)
			place += C2dVec( 0.0, m_extent.Dy() );
		else
			place += C2dVec( m_extent.Dx(), 0.0 );
	}

	//
	// Recurse to sub-sheets.
	//
	for (idx=0; idx<m_sheetlist.Count(); idx++)
		parts_cut += m_sheetlist[idx]->Cut( config, partbin, sheet, scrap, view );

	// How many parts cut in this branch?
	return parts_cut;
}

// ==================================================================
void
CPanelNode::do_cut( 
	CSheet*			sheet, 
	CNestingPart*		part, 
	int				partnum,
	const C2dCoord&	pos,
	CViewMgr&		view )
{
	CModel*			dst_model = sheet->pModel();
	CModel*			src_model = part->pModel(0);

	//
	// Position on sheet, handle of panel... starting position
	//
	C3dCoord	place( pos.X(), pos.Y(), 0.0 );

	C3dCoord	handle;
	C3x4Matrix	rotate;
	C3x4Matrix	shift;

	rotate.setUnit();

	//
	// Possible 90' rotation; affects shift matrix to incorporate rotation
	//
	if (m_rot90)
	{
		//
		// Looks simple, doesn't it.  WELL BITE ME!  This sucker was a pain
		// in the ass to figure out...  brain mush.
		//
		handle = C3dCoord( -m_extent.Xmax(), m_extent.Ymin(), 0.0 );

		shift.setI( 0, 1, 0 );
		shift.setJ( -1, 0, 0 );
		shift.setK( 0, 0, 1 );
		shift.setT( place.X()-handle.X(), place.Y()-handle.Y(),0.0 );
	}
	else
	{
		handle = C3dCoord( m_extent.Xmin(), m_extent.Ymin(), 0.0 );

		shift.setUnit();
		shift.Shift( C3dVec( place.X() - handle.X(), place.Y() - handle.Y(), 0.0 ) );
	}

	//
	// Clear the transformation tags (and set up generic model and iterator)
	//
	CDbIterator		iter;
	CDbEntity*		db_ent = NULL;

	CDbEntity::NewAction();

	//
	// Perform actual copy and transformation of geometry into a
	//	holding buffer
	//
	src_model->EntityPrepareCopy( dst_model );

	CDbEntityList	buffer;

	iter.Init( src_model->Db(), DBLINE );
	while (TRUE)
	{
		db_ent = iter();
		if (!db_ent)
			break;
		iter.Next();

		if ( src_model->Db().WasCopied( *db_ent ) )
			continue;

		if (db_ent->Tool() == NULL)
			continue;

		if (db_ent->Tool()->Name().CompareNoCase(STR_STOCK) == 0)
			continue;

		// Traverse up ownership to TOP DOG...
		while (db_ent->Owner())
			db_ent = db_ent->Owner();

		CDbEntity* new_ent = NULL;
		src_model->EntityCopy( *db_ent, &new_ent );
		new_ent->Transform( shift );

		buffer.Append( new_ent );
	}

	//
	// Create a new feature in the sheet to receive this buffer
	//
	CDbFeature*	feature = NULL;
	dst_model->EntityCreate( DBFEATURE, (CDbEntity**)&feature );
	if (!feature)
		return;

	CDbTool* dbTool = dst_model->ActiveTool();
	CDbWorkplane* dbWork = dst_model->ActiveWorkplane();

	CDbWorkplane* dbTop = NULL;
	dst_model->EntityFind( STR_TOP, (CDbEntity**)&dbTop, DBWORKPLANE, DBWORKPLANE );
	if (!dbTop)
		dbTop = dbWork;

	feature->Name( part->Name() );

	//
	// Create a part label
	//
	CString label_text;
	label_text.Format( "%d", partnum+1 );

	CDbCommand*	label = NULL;
	dst_model->EntityCreate( DBCOMMAND, (CDbEntity**)&label );
	if (!label)
		return;

	//
	// Copy the buffered entities over to the new feature
	//
	feature->Append( label );
	for (int idx=0; idx<buffer.Count(); idx++)
	{
		db_ent = buffer[idx];

		db_ent->Tool( dbTool );
		feature->Append( db_ent );

		render_part( *dst_model, view, db_ent );
	}

	//
	// Finally, init the label
	//

	C2dBox mer = feature->Box();

	//
	// TODO: 1) Use MarkIntExt() to id all profiles in the part, for increased robustness
	// TODO: 2) Get outer profile, and scan around in it to locate a valid in-profile position for text
	//
	label->Init( dbTool, dbTop, mer.Xc(), (mer.Ymin() + mer.Yc())/2, 0.0, label_text );
}

void
CPanelNode::render_part( 
	CModel&	model,
	CViewMgr&		view,
	CDbEntity*		db_ent )
{
	CDbContainer* container = dynamic_cast<CDbContainer*>( db_ent );
	if (container)
	{
		int num = container->Count();
		for (int idx=0; idx<num; idx++)
		{
			db_ent= (*container)[idx];
			if (!db_ent)
				break;

			render_part( model, view, db_ent );
		}
	}
	else
	{
		view.ModelSet( model );
		view.Refresh( db_ent->Id(), FALSE );
	}
}

// ==================================================================
void			
CPanelNode::Dump( 
	HANDLE	file, 
	int		in_depth )
{
	DWORD	dope;
	CString	prefix;
	CString	note;

	int depth = in_depth;
	while (depth--)
		prefix += "| ";

	note.Format( "%sP%d[%d] %c %dx%d%c\t\t%d L%d\n", 
					prefix, 
					m_idx,
					m_quantity,
					m_ycut?'Y':'X',
					(int)m_extent.Dx(), 
					(int)m_extent.Dy(),
					m_rot90?'R':' ',
					(int)(m_score),
					(in_depth+1)/2 );
	::WriteFile( file, note, note.GetLength(), &dope, NULL );

	for (int idx=0; idx<m_sheetlist.Count(); idx++)
		m_sheetlist[idx]->Dump( file, in_depth+1 );
}

