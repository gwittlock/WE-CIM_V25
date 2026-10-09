#ifndef _REGISTER_H
#define _REGISTER_H

// ==================================================================
//		Register
//
//		Registry manager, simple function wrappers
//
// ==================================================================

#ifndef _WINREG_H
#include <winreg.h>
#define _WINREG_H
#endif

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
class dllExport CRegister
{
public:

	static int		IntGet( const CString& key, const CString& subkey = "" );
	static double	DoubleGet( const CString& key, const CString& subkey = "" );
	static CString	StringGet( const CString& key, const CString& subkey = "" );

	static void		IntSet( const CString& key, const CString& subkey, int val );
	static void		DoubleSet( const CString& key, const CString& subkey, double val );
	static void		StringSet( const CString& key, const CString& subkey, const CString& val );

protected:

};

#endif