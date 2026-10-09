// ==================================================================
//		Register
//
//		Registry manager, simple function wrappers
//
// ==================================================================

#include "stdafx.h"
#include "MathConst.h"
#include "register.h"

#include "portable.h"

static const int MAXBUF = 1024;
static char g_buf[MAXBUF];

HKEY CRegister::m_hkey = HKEY_CURRENT_USER;
CString CRegister::m_rootv = "";
CString CRegister::m_root = "";

// ============================================================================

void CRegister::HKEYSet( EHkey which )
{
	// The options were introduce for ITI. A standard installation uses HKEY_CURRENT_USER
	// and a server installation (aka "silent mode") uses HKEY_LOCAL_MACHINE.
	m_hkey = ((which == LOCAL_MACHINE) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER);
}

HKEY CRegister::HKEYGet()
{
	return m_hkey;
}

void CRegister::SetRootPath( const CString& reg_root )
{
	//MessageBox(NULL, "The regroot is <" + reg_root + " >", "Error", MB_OK);

	//MessageBox(NULL, "FATAL ERROR: Unable to find jvm.dll", "Error", MB_OK);
	// eg. where reg_root = "Software\WE-CIM\19.0"
	m_rootv = reg_root;
	int indx = m_rootv.ReverseFind( '\\' );
	if (indx > 0)
	{
		// eg. where m_root = "Software\WE-CIM"
		m_root = m_rootv.Left( indx );
	}
}

void CRegister::BoolSetV( const CString& key, const CString& subkey, bool defval )
{
	CString vkey = RootPathV( CString( "\\" ) + key );
	BoolSet( vkey, subkey, defval );
}

bool CRegister::BoolGetV( const CString& key, const CString& subkey, bool defval )
{
	CString vkey = RootPathV( CString( "\\" ) + key );
	return BoolGet( vkey, subkey, defval );
}

void CRegister::IntSetV( const CString& key, const CString& subkey, int defval )
{
	CString vkey = RootPathV( CString( "\\" ) + key );
	IntSet( vkey, subkey, defval );
}

int CRegister::IntGetV( const CString& key, const CString& subkey, int defval )
{
	CString vkey = RootPathV( CString( "\\" ) + key );
	return IntGet( vkey, subkey, defval );
}

void CRegister::DoubleSetV( const CString& key, const CString& subkey, double defval )
{
	CString vkey = RootPathV( CString( "\\" ) + key );
	DoubleSet( vkey, subkey, defval );
}

double CRegister::DoubleGetV( const CString& key, const CString& subkey, double defval )
{
	CString vkey = RootPathV( CString( "\\" ) + key );
	return DoubleGet( vkey, subkey, defval );
}

void CRegister::StringSetV( const CString& key, const CString& subkey, const CString& defval )
{
	CString vkey = RootPathV( CString( "\\" ) + key );
	StringGet( vkey, subkey, defval );
}

CString CRegister::StringGetV( const CString& key, const CString& subkey, const CString& defval )
{
	CString vkey = RootPathV( CString( "\\" ) + key );
	return StringGet( vkey, subkey, defval );
}



bool CRegister::BoolGet( const CString& key, const CString& subkey, bool defval )
{
	int ival = IntGet( key, subkey, (defval ? 1 : 0) );
	return (ival != 0);
}

int CRegister::IntGet( const CString& key, const CString& subkey, int defval )
{
	CString buf;
	
	buf.Format( "%d", defval );
	buf = StringGet( key, subkey, buf );

	return ( atoi( buf ) );
}

double CRegister::DoubleGet( const CString& key, const CString& subkey, double defval )
{
	CString buf;
	
	buf.Format( "%f", defval );
	buf= StringGet( key, subkey, buf );

	return ( atof( buf ) );
}

CString CRegister::StringGet( const CString& key, const CString& subkey, const CString& defval )
{
	HKEY reg_key;

	long status = RegOpenKey( m_hkey, key, &reg_key );
	if (status == ERROR_SUCCESS)
	{
		static DWORD type;
		unsigned long buf_len = sizeof( g_buf );

		(*g_buf) = '\0';
		status = RegQueryValueEx( reg_key, subkey, NULL, &type, (BYTE*)g_buf, &buf_len );

		RegCloseKey( reg_key );

		// TODO: Put the value in the reg if failed (ie. status != 0)?
	}

	return ((status == ERROR_SUCCESS) ? CString( g_buf ) : defval);
}

void CRegister::BoolSet( const CString& key, const CString& subkey, bool val )
{
	IntSet( key, subkey, (val ? 1 : 0) );
}

void CRegister::IntSet( const CString& key, const CString& subkey, int val )
{
	wsprintf( g_buf, "%d", val );
	StringSet( key, subkey, g_buf );
}

void CRegister::DoubleSet( const CString& key, const CString& subkey, double val )
{
	// wsprintf( g_buf, "%f", val );  -- failed on vals like 0.001 :-(
	sprintf_s( g_buf, MAXBUF, "%f", val );
	StringSet( key, subkey, g_buf );
}

void CRegister::StringSet( const CString& key, const CString& subkey, const CString& val )
{
	HKEY reg_key;

	long reg_err = RegCreateKey( m_hkey, key, &reg_key );
	if (reg_err == ERROR_SUCCESS )
	{
		wsprintf( g_buf, "%d", val );
		reg_err = RegSetValueEx( reg_key, subkey, NULL, REG_SZ, (BYTE*)(LPCSTR)val, val.GetLength() );

		RegCloseKey( reg_key );
	}

	return;
}

int CRegister::Debug( const CString& subkey )
{
	CString key = RootPathV() + "\\Debug";

	return ( IntGet( key, subkey, 0 ) );
}


CString CRegister::RootPathV()
	{ return m_rootv; }

CString CRegister::RootPath()
	{ return m_root; }


bool IsPhenolicProject()
{
	// 2011.11.18 (PE) -- At this date, Gary had encountered failures
	// in code generation. Literally, he could not generate code.
	// Originally, the cause was thought to be the Java subsystem but
	// we eventually determined the cause to be this phenolic setting.
	//
	// Also, with recent discussions with GoMech(?) issues arose wrt
	// toolpath sequencing during code generation. Said behavior is
	// related to the phenolic setting. Since that behavior is now
	// deemed undesirable (harded-coded to use tool order) we disable
	// it here by forcing IsPhenolicProject() to always return false.
#if 0
	bool phenolic = CRegister::BoolGetV( "Customizations", "phenolic", false );
	return phenolic;
#else
	return false;
#endif
}

bool IsPhenolicProject2()
{
	// IsPhenolicProject2() partially undoes the
	// damage caused by disabling IsPhenolicProject().
	bool phenolic = CRegister::BoolGetV( "Customizations", "phenolic", false );
	return phenolic;
}
