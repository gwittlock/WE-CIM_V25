#pragma once

// ==================================================================
// NOTE: Map between our terminology and Microsoft's
//
//     path    key
//     field   subkey
//     value   data
//
// In the Microsoft world, every key is created with a default
// subkey.  The default subkey is seen via regedit as '(Default)'.
// Internally, calls to Microsoft's registry functions use the
// empty string to denote the default subkey.  Likewise, we use
// the empty string fo the same purpose.  Therefore, if you want
// to get/set the default field use eg. reg.getString( "" );
//
enum EHkey
{
	CURRENT_USER  = 0,
	LOCAL_MACHINE = 1
};

class dllExport CRegister
{
public:

	static void HKEYSet( EHkey which );
	static HKEY HKEYGet();

	// eg. where reg_root = "Software\WE-CIM\19.0"
	static void SetRootPath( const CString& reg_root );

	static void BoolSetV( const CString& key, const CString& subkey, bool defval );
	static bool BoolGetV( const CString& key, const CString& subkey, bool defval );

	static void IntSetV( const CString& key, const CString& subkey, int defval );
	static int IntGetV( const CString& key, const CString& subkey, int defval );

	static void DoubleSetV( const CString& key, const CString& subkey, double defval );
	static double DoubleGetV( const CString& key, const CString& subkey, double defval );

	static void StringSetV( const CString& key, const CString& subkey, const CString& defval );
	static CString StringGetV( const CString& key, const CString& subkey, const CString& defval );

	static bool		BoolGet( const CString& key, const CString& subkey, bool defval );
	static int		IntGet( const CString& key, const CString& subkey, int defval );
	static double	DoubleGet( const CString& key, const CString& subkey, double defval );
	static CString	StringGet( const CString& key, const CString& subkey, const CString& defval );

	static void		BoolSet( const CString& key, const CString& subkey, bool val );
	static void		IntSet( const CString& key, const CString& subkey, int val );
	static void		DoubleSet( const CString& key, const CString& subkey, double val );
	static void		StringSet( const CString& key, const CString& subkey, const CString& val );

	static CString RootPath();
	static CString RootPathV();

	static CString RootPath( const CString& sub )	{ return RootPath() + sub; }
	static CString RootPathV( const CString& sub )	{ return RootPathV() + sub; }

	static int Debug( const CString& subkey );

private:

	static HKEY m_hkey;
	static CString	m_rootv;
	static CString	m_root;
};


// IsPhenolicProject() was originally implemented in Portal.h but doing
// so introduced a new (though not necessary a bad) dependency.
bool dllExport IsPhenolicProject();
bool dllExport IsPhenolicProject2();
