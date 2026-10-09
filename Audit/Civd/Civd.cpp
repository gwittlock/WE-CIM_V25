// CadProcess.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Civd.h"
#include <initguid.h>

#include "vdPrimary.h"
#include "vdDocument.h"
#include "vdCommand.h"

#include "vdFigure.h"
#include "vdLayers.h"
#include "vdSelections.h"

#include "vdXProperties.h"
#include "vdXProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//CRouteList CCivd::m_cadRouter;

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

CvdDocument* CCivd::m_doc[3];
CvdCommand* CCivd::m_cmd[3];

// For type conversion.
CVarList CCivd::m_types;


/////////////////////////////////////////////////////////////////////////////
// CCivd

BEGIN_MESSAGE_MAP(CCivd, CWinApp)
	//{{AFX_MSG_MAP(CCivd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCivd construction

CCivd::CCivd()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_doc[CI_MAIN] = NULL;
	m_cmd[CI_MAIN] = NULL;

	m_doc[CI_AUX] = NULL;
	m_cmd[CI_AUX] = NULL;

	m_doc[CI_UNDO] = NULL;
	m_cmd[CI_UNDO] = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CCivd object

CCivd theApp;

	
CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}
/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer(TRUE); //TRUE indicates that typelib is unreg'd
	return S_OK;
}

BOOL CCivd::InitInstance()
{
	if (!InitATL())
		return FALSE;

	return CWinApp::InitInstance();

}

int CCivd::ExitInstance()
{
	_Module.Term();

	return CWinApp::ExitInstance();

}

BOOL CCivd::InitATL()
{
	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	return TRUE;

}

CvdDocument&
CCivd::DocumentGet( eCiHookup which_obj )
{
	return (*m_doc[which_obj]);
}

CvdCommand&
CCivd::CommandGet( eCiHookup which_obj )
{
	return (*m_cmd[which_obj]);
}

CvdEntities
CCivd::GetEntities( eCiHookup which_obj )
{
	return m_doc[which_obj]->GetEntities();
}

CvdSelections
CCivd::GetSelections( eCiHookup which_obj )
{
	return m_doc[which_obj]->GetSelections();
}

CvdLayers
CCivd::GetLayers( eCiHookup which_obj )
{
	return m_doc[which_obj]->GetLayers();
}

void
CCivd::SetActiveLayer( eCiHookup which_obj, const CString& name )
{
	DocumentGet( which_obj ).SetActiveLayer( name );
}

CvdFigure
CCivd::GetFromHandle( eCiHookup which_obj, const CString& handle )
{
	return (CvdFigure) DocumentGet( which_obj ).GetFromHandle( handle );
}

// Encapsulates dirty work of getting a valid entity from VDraw.
CvdFigure*
CCivd::EntityGet( eCiHookup which_obj, const CString& handle )
{
	CvdFigure*	fig = NULL;

	if ( !handle.IsEmpty() )
	{
		// NOTE: This alternate implementation fails ...
		//		CvdFigure*	fig;
		//		fig = &( m_doc->GetFromHandle( handle ) );

		CvdFigure	tmp = DocumentGet( which_obj ).GetFromHandle( handle );
		if ( IsVdObject(tmp) )
		{
			fig = new CvdFigure();
			(*fig) = tmp;
		}
	}

	return fig;
}

// ============================================================================

BOOL
CCivd::Init( LPDISPATCH docObject, LPDISPATCH cmdObject, eCiHookup which_obj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_doc[which_obj] = new CvdDocument();
	m_cmd[which_obj] = new CvdCommand();

	CCivd::DocumentGet(which_obj).AttachDispatch( docObject, FALSE );
	CCivd::CommandGet(which_obj).AttachDispatch( cmdObject, FALSE );

	return TRUE;
}

BOOL
CCivd::Terminate()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	eCiHookup hndx;

	for (int indx = CI_MAIN; indx <= CI_UNDO; ++indx)
	{
		hndx = (eCiHookup) indx;

		if (m_doc[hndx] != NULL)
		{
			m_doc[hndx]->DetachDispatch();
			delete m_doc[hndx];
			m_doc[hndx] = NULL;
		}

		if (m_cmd[hndx] != NULL)
		{
			m_cmd[hndx]->DetachDispatch();
			delete m_cmd[hndx];
			m_cmd[hndx] = NULL;
		}
	}

	return TRUE;
}

eCiType
CCivd::CiType( const CString& vdrawType )
{
	if (m_types.countVar() < 1)
	{
		m_types.setInt( "VD3DFACE",		CI3DFACE );
		m_types.setInt( "VDARC",		CIARC );
		m_types.setInt( "VDATTRIB",		CIATTRIB );
		m_types.setInt( "VDBLOCK",		CIBLOCK );
		m_types.setInt( "VDCIRCLE",		CICIRCLE );
		m_types.setInt( "VDDIMSTYLE",	CIDIMSTYLE );
		m_types.setInt( "VDDIMENSION",	CIDIMENSION );
		m_types.setInt( "VDELLIPSE",	CIELLIPSE );
		m_types.setInt( "VDFIGURE",		CIFIGURE );
		m_types.setInt( "VDIMAGE",		CIIMAGE );
		m_types.setInt( "VDINSERT",		CIINSERT );
		m_types.setInt( "VDLAYER",		CILAYER );
		m_types.setInt( "VDLAYOUT",		CILAYOUT );
		m_types.setInt( "VDLINE",		CILINE );
		m_types.setInt( "VDPOINT",		CIPOINT );
		m_types.setInt( "VDPOLYFACE",	CIPOLYFACE );
		m_types.setInt( "VDPOLYHATCH",	CIPOLYHATCH );
		m_types.setInt( "VDPOLYLINE",	CIPOLYLINE );
		m_types.setInt( "VDRECTANGLE",	CIRECTANGLE );
		m_types.setInt( "VDTEXT",		CITEXT );
		m_types.setInt( "VDTEXTSTYLE",	CITEXTSTYLE );
		m_types.setInt( "VDVIEWPORT",	CIVIEWPORT );
	}
	
	return ( (eCiType) m_types.getInt( vdrawType, CIUNDEFINED_TYPE ) );
}

// From VectorCAD\Samples\Util.cpp
VARIANT
CCivd::XYZToVariant( double x, double y, double z )
{
    COleSafeArray	array;
	VARIANT			vaResult;
	double			res[3];

	VariantInit(&vaResult);

	res[0] = x;
	res[1] = y;
	res[2] = z;
    
    array.CreateOneDim( VT_R8, 3, res );
    vaResult = array.Detach();

	return vaResult;
}

// From VectorCAD\Samples\Util.cpp
BOOL
CCivd::VariantToXYZ( const VARIANT FAR& variant, double* x, double* y, double* z )
{
	if (variant.vt == (VT_ARRAY | VT_R8))
	{ 
		if(!variant.parray->pvData)
			return FALSE;

		double xy[3];
		memcpy( xy, variant.parray->pvData, sizeof(double)*3 );

		(*x) = xy[0];
		(*y) = xy[1];
		(*z) = xy[2];
	}

	return FALSE;
}

void
CCivd::XpropRemove( CvdPrimary& prim, const CString& name )
{
	CvdXProperties	xproplist;
	CvdXProperty	xprop;

	xproplist = prim.GetXProperties();
	xprop = xproplist.FindName(name);
	if ( IsVdObject(xprop) )
	{
		xproplist.RemoveItem( xprop );
	}
}