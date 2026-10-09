// ==================================================================
// Font.cpp :
//
// ==================================================================

#include "stdafx.h"

#include "MathConst.h"
#include "CommonFlags.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "CommonFlags.h"
#include "Path.h"

#include "3dBox.h"
#include "DbWorkplane.h"
#include "DbFeature.h"
#include "DbTool.h"
#include "DbIterator.h"
#include "MM2.h"
#include "ViewMgr.h"
#include "Selector.h"
#include "ModelUtil.h"
#include "DbEntityVisitor.h"

#include "CreateProcess.h"

static char BASED_CODE MM2_FILTER[] = "MM2 Files (*.mm2)|*.mm2||";
static DWORD FFLAGS = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CFontAttribChanger : public CDbEntityVisitor
	{
	public:

		CFontAttribChanger()
		{
		}

		void Init( const CVarList& attribs, CDbTool* dbTool )
		{
			m_tool = dbTool;
			m_attribs = attribs;
		}

		virtual void Visit( CDbEntity* dbEntity )
		{
			CVar*	attrib;
			CString	name;
			int		count, indx;

			if (dbEntity != m_tool)
			{
				dbEntity->Tool( m_tool );
			}

			count = m_attribs.countVar();
			for (indx = 0; indx < count; ++indx)
			{
				attrib = m_attribs.getVar( indx );
				name = attrib->getName();

				if (name.CompareNoCase("vis") == 0)
				{
					if (attrib->getInt() == 0)
						dbEntity->Hide();
					else
						dbEntity->Seek();
				}
				else
				{
					dbEntity->pAttrib()->setVar( (*attrib) );
				}
			}
		}

		virtual ~CFontAttribChanger()
		{
		}

	private:

		CVarList	m_attribs;
		CDbTool*	m_tool;
	};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// Font:Load:file=%s
CReturn 
CCreateProcessApp::FontLoad( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbFeature*	dbFeature;
	CString		file;
	CPath		path;
	CMM2		mm2;

	CModel&		model = io_cmd->getModel();

	file = io_cmd->VarList().getString( "file", "" );
	if ( file.IsEmpty() )
	{
		// TODO: Why doesn't the dialog open?
		CFileDialog dlg( TRUE, "*.mm2", "c:\\", FFLAGS, MM2_FILTER );
		if (dlg.DoModal() == IDOK)
		{
			file = dlg.GetPathName();
		}
	}

	if ( !file.IsEmpty() )
	{
		status =  mm2.Merge( file, C3dCoord( 0., 0., 0. ), &model, NULL );
		if ( status.IsOk() )
		{
			path.Set( file );

			dbFeature = FontFind( model, path.FileName() );
			if (dbFeature != NULL)
			{
				FontLayer( &model, dbFeature );
			}
		}
	}

	return status;
}

// Font:Unload:font=%s
CReturn 
CCreateProcessApp::FontUnload( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;

	CDbEntity*	dbFont;
	CDbEntity*	dbTool;

	CModel&		model = io_cmd->getModel();

	CString		font	= io_cmd->VarList().getString( "font", "" );

	if ( !font.IsEmpty() )
	{
		dbFont = FontFind( model, font );

		if (dbFont != NULL)
		{
			dbTool = dbFont->Tool();

			model.Db().Delete( &dbFont );
			model.Db().Delete( &dbTool );
		}
	}

	return status;
}

// Font:Text: font=%s, text=%s, size=%f, orient=%f, x=%f, y=%f
CReturn 
CCreateProcessApp::FontText( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;

	CModel&		model = io_cmd->getModel();

	CString		font	= io_cmd->VarList().getString( "font", "" );
	CString		text	= io_cmd->VarList().getString( "text", "" );
	double		size	= io_cmd->VarList().getReal( "size", 0. );
	double		orient	= io_cmd->VarList().getReal( "orient", 0. );
	double		x		= io_cmd->VarList().getReal( "x", UNDEFINED );
	double		y		= io_cmd->VarList().getReal( "y", UNDEFINED );

	if ( !font.IsEmpty() && !text.IsEmpty() )
	{
		CDbFeature* dbFont = FontFind( model, font );

		if (dbFont != NULL)
		{
			TextCreate( (*dbFont), text, size, orient, C3dCoord( x, y, 0. ), &model );
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn
CCreateProcessApp::TextCreate(
						const CDbFeature&	dbFont,
						const CString&		text,
						double				size,
						double				orient,
						const C3dCoord&		pt,
						CModel*				model )
{
	CReturn			status;

	CDbFeature*		dbChar;
	CDbFeature*		dbText;
	CDbTool*		activeTool;
	CDbWorkplane*	activeWork;
	C3dBox			box;
	C3dVec			delta;
	C3x4Matrix		matrix;
	CVarList		attribs;
	CFontAttribChanger	attribChanger;
	double			xhnd, yhnd;
	double			xsiz, ysiz;
	double			tally;
	int				count, indx;
	int				countA, countB;
	int				color;
	char			chr;

	CSelectorStack& selectorStack = model->SelectorStack();

	selectorStack.Push();

	CSelector& selector = selectorStack();
	selector.Restrictions( TRUE );

	dbText = NULL;

	activeTool = model->ActiveTool();
	activeWork = model->ActiveWorkplane();

	color = activeTool->ColorGet( DCOLOR_WHITE );
	model->pDefault()->setColor( color );

	ysiz = dbFont.DoubleGet( "ysiz", 0. );
	tally = 0.;

	count = text.GetLength();
	for (indx = 0; indx < count; ++indx)
	{
		chr = text[indx];

		dbChar = CharFind( dbFont, chr );

		if (dbChar != NULL)
		{
			xsiz = dbChar->DoubleGet( "xsiz", 0. );

			if (chr != ' ')
			{
				xhnd = dbChar->DoubleGet( "xhnd", 0. );
				yhnd = dbChar->DoubleGet( "yhnd", 0. );

				delta.X( (pt.X() - xhnd) + tally );
				delta.Y( (pt.Y() - yhnd) );
				delta.Z( 0. );

				matrix.setUnit();
				matrix.Shift( delta );

				countA = model->Db().Count( DBFEATURE );

				selector.Add( dbChar, FALSE );

				status = CModelUtil::Transform( model, matrix, 1, (XFORM_NO_SYSTEM | XFORM_NO_ASSOC) );

				if ( status.IsOk() )
				{
					countB = model->Db().Count( DBFEATURE );

					if (dbText == NULL)
					{
						model->EntityCreate( DBFEATURE, (CDbEntity**) &dbText );

						dbText->StringSet( "text", text );
						dbText->DoubleSet( "orient", orient );
					}

					dbChar = dynamic_cast<CDbFeature*>( model->Db().Get( DBFEATURE, (countB-1) ) );
					dbText->Append( dbChar );
				}

				selector.Clear();
			}

			tally += xsiz;
		}
	}

	if (dbText != NULL)
	{
		C3x4Matrix	to_origin;
		C3x4Matrix	rotate;
		C3x4Matrix	from_origin;

		to_origin.setUnit();

		to_origin.Shift( C3dVec( -pt.X(), -pt.Y(), 0.0 ) );
		to_origin.InvertTo( &from_origin );

		rotate.setXYAngle( (orient * DEG2RAD) );
		rotate.Scale( size / ysiz );
		to_origin.Transform( &rotate );
		rotate.Transform( &from_origin );

		selector.Add( dbText, FALSE );

		status = CModelUtil::Transform( model, from_origin, 0, (XFORM_NO_SYSTEM | XFORM_NO_ASSOC) );

		selector.Clear();
	}

	attribs.setInt( "vis", TRUE );
	attribs.setColor( DCOLOR_WHITE );

	attribChanger.Init( attribs, activeTool );
	dbText->Accept( &attribChanger );

	selectorStack.Pop();

	return status;
}

CDbFeature*
CCreateProcessApp::CharFind(
						const CDbFeature&	dbFont,
						const char&			chr )
{
	CDbFeature*	dbFeature;
	CDbFeature*	dbChar;
	CString		schr( chr );
	CString		id;
	int			count, indx;

	dbChar = NULL;

	count = dbFont.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbFeature = dynamic_cast<CDbFeature*>( dbFont[indx] );
		if (dbFeature != NULL)
		{
			id = dbFeature->StringGet( "char", "" );
			if (id.Compare( schr ) == 0)
			{
				dbChar = dbFeature;
				break;
			}
		}
	}

	return dbChar;
}

CDbFeature*
CCreateProcessApp::FontFind( const CModel& model, const CString& fontName )
{
	CDbIterator		iter;
	CDbFeature*		dbFont;
	CDbFeature*		dbFeature;
	CString			id;

	dbFont = NULL;

	iter.Init( model.Db(), DBFEATURE );
	while(1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		id = dbFeature->StringGet( "font", "" );
		if (id.CompareNoCase( fontName ) == 0)
		{
			dbFont = dbFeature;
			break;
		}

		iter.Next();
	}

	return dbFont;
}

// Move the font to a hidden layer.
void
CCreateProcessApp::FontLayer( CModel* model, CDbFeature* dbFont )
{
	CFontAttribChanger	attribChanger;
	CVarList			attribs;
	CString				layerName;
	CDbTool*			dbTool;

	layerName = "font_" + dbFont->StringGet( "font", "font" );

	model->EntityFind( layerName, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
	if (dbTool == NULL)
	{
		model->EntityCreate( DBLAYER, (CDbEntity**) &dbTool );
		dbTool->Name( layerName );
	}

	attribs.setInt( "vis", FALSE );
	attribs.setColor( DCOLOR_WHITE );

	attribChanger.Init( attribs, dbTool );
	dbTool->Accept( &attribChanger );
	dbFont->Accept( &attribChanger );
}
