#ifndef _CITYPE_H
#define _CITYPE_H

#include "Type.h"
#include "MathConst.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// The constants defined here must match their Java counterparts that are
// defined in CiJava\Source\Ci\Modeler\CiMConst.java
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/* From VDraw documentation
"VD3DFACE"
"VDARC"
"VDATTRIB"
"VDBLOCK"
"VDCIRCLE"
"VDDIMSTYLE"
"VDDIMENSION"
"VDELLIPSE"
"VDFIGURE"
"VDIMAGE"
"VDINSERT"
"VDLAYER"
"VDLAYOUT"
"VDLINE"
"VDPOINT"
"VDPOLYFACE"
"VDPOLYHATCH"
"VDPOLYLINE"
"VDRECTANGLE"
"VDTEXT"
"VDTEXTSTYLE"
"VDVIEWPORT"
*/

#define NCITYPES 23

typedef enum
{
	CI3DFACE	= 0x000001,
	CIARC		= 0x000002,
	CIATTRIB	= 0x000004,
	CIBLOCK		= 0x000008,
	CICIRCLE	= 0x000010,
	CIDIMSTYLE	= 0x000020,
	CIDIMENSION	= 0x000040,
	CIELLIPSE	= 0x000080,
	CIFIGURE	= 0x000100,
	CIIMAGE		= 0x000200,
	CIINSERT	= 0x000400,
	CILAYER		= 0x000800,
	CILAYOUT	= 0x001000,
	CILINE		= 0x002000,
	CIPOINT		= 0x004000,
	CIPOLYFACE	= 0x008000,
	CIPOLYHATCH	= 0x010000,
	CIPOLYLINE	= 0x020000,
	CIRECTANGLE	= 0x040000,
	CITEXT		= 0x080000,
	CITEXTSTYLE	= 0x100000,
	CIVIEWPORT	= 0x200000,
	CIPROFILE	= 0x400000,
	CIUNDEFINED_TYPE = 0,
	CIALL		= 0xFFFFFF
} eCiType;

typedef enum
{
	CICREATE	= 0,
	CIUPDATE	= 1,
	CIQUERY		= 2,
	CIDELETE	= 3,
	CIFLUSH		= 4,
	CISTARTPT	= 5,
	CIENDPT		= 6,
	CIAPPEND	= 7,
	CIPREPEND	= 8,
	CIGETAT		= 9,
	CISETAT		= 10,
	CIREVERSE	= 11,
	CIEXTRACT	= 12,
	CIPUSH		= 13,
	CIPOP		= 14,
	CIREPLACE	= 15,
	CIINSERTAT	= 16,
	CIREMOVE	= 17
} eCiAction;

typedef enum
{
	CI_UNDO_ENABLE	= 0,
	CI_UNDO_LIMIT	= 1,
	CI_UNDO_BEGIN	= 2,
	CI_UNDO_END		= 3,
	CI_UNDO_UNDO	= 4,
	CI_UNDO_REDO	= 5
} eCiUndoAction;

typedef enum
{
	CISTR		= 1000,
	CIPNT		= 1010,
	CIDYNPNT	= 1011,
	CIDBL		= 1040,
	CIDYNDBL	= 1041,
	CIINT		= 1071
} eCiXPropType;

typedef enum
{
	CIBYNAME	= 0,
	CIBYID		= 1,
	CIBYINDEX	= 2
} eCiMethod;

/* From VDraw documentation
Description
Saves changes to a drawing given the full filename, including the path.
The active document takes on the new name. 

You can save only in these formats : DWG, DXF, VDF, VDI, BMP, WMF, EMF

Saving in DWG - DXF formats 
Value Constant Description 
0 VdCadVer25 AutoCAD Release 2.5 
1 VdCadVer26 AutoCAD Release 2.6 
2 VdCadVer9  AutoCAD Release 9 
3 VdCadVer10 AutoCAD Release 10 
4 VdCadVer11 AutoCAD Release 11 
5 VdCadVer13 AutoCAD Release 13 
6 VdCadVer14 AutoCAD Release 14 
7 VdCadVer2000 AutoCAD Release 2000 
100 (default) - AutoCAD Release 14 

Saving in VDF - VDI formats 
Value Description 
0 Save in the same version as the Open File always >= version 3.1 
100 (default) Save in latest version 
301 Save in version 3.1 = (3+1/100)*100 
302 Save in version 3.2 = (3+3/100)*100 
303 Save in version 3.3 = (3+3/100)*100 
in general for version Major.Minor = (major+minor/100)*100 

From 3,3,4,0 and up we compare the 1st and 3rd version number  
304 Save in version 3.3.4.x = (3+4/100)*100 
in general for version Major.Minor1.Minor2.Monor3 = (major+minor2/100)*100  
etc... 
Range: >= 301 or 0 or 100 

Remarks: the Save method always save in latest version
*/

typedef enum
{
	// DXF/DWG Formats
	CIR2_5	= 0,
	CIR2_6	= 1,
	CIR9_0	= 2,
	CIR10	= 3,
	CIR11	= 4,
	CIR13	= 5,
	CIR14	= 6,	// (default)
	CIR2000	= 7,

	// VDF/VDI Formats
	CIVDF_CURR	= 0,	// Save in same version as was opened
	CIVDF_301	= 310,	// 3.1
	CIVDF_302	= 320,	// 3.2
	CIVDF_303	= 330,	// 3.3

	// DXF/DWG/VDF/VDI
	CILATEST	= 100,	// (default) Save in latest version 
	CI2000		= 7	/// DWG/DXF R2000/2002
} eCiFileType;

typedef enum
{
	VDHIGHLIGHTSHOWNORMAL	= 0,
	VDHIGHLIGHTDOT			= 1,
	VDHIGHLIGHTINVISIBLE	= 2
} eCiVdHightLight;

typedef enum
{
	VDPENSOLID		= 0,
	VDPENDASH		= 1,
	VDPENDOT		= 2,
	VDPENDASHDOT	= 3
} eCiPenStyle;

#endif

