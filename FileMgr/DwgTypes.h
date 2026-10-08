
#ifndef _DWGTYPES_H
#define _DWGTYPES_H

// #define BITSHORT unsigned short : 2
#define UINT8 unsigned char
#define INT16 WORD
#define INT32 int
#define DOUBLE double
#define MODULAR_SHORT int

const short DWG_UNUSED				= 0x00;
const short DWG_TEXT				= 0x01;
const short DWG_ATTRIB				= 0x02;
const short DWG_ATTDEF				= 0x03;
const short DWG_BLOCK				= 0x04;
const short DWG_ENDBLK				= 0x05;
const short DWG_SEQEND				= 0x06;
const short DWG_INSERT				= 0x07;
const short DWG_MINSERT				= 0x08;
const short DWG_0x09				= 0x09;
const short DWG_VERTEX_2D			= 0x0A;
const short DWG_VERTEX_3D			= 0x0B;
const short DWG_VERTEX_MESH			= 0x0C;
const short DWG_VERTEX_PFACE		= 0x0D;
const short DWG_VERTEX_FACE			= 0x0E;
const short DWG_POLYLINE_2D			= 0x0F;
const short DWG_POLYLINE_3D			= 0x10;
const short DWG_ARC					= 0x11;
const short DWG_CIRCLE				= 0x12;
const short DWG_LINE				= 0x13;
const short DWG_DIM_ORDINATE		= 0x14;
const short DWG_DIM_LINEAR			= 0x15;
const short DWG_DIM_ALIGNED			= 0x16;
const short DWG_DIM_ANG_3PT			= 0x17;
const short DWG_DIM_ANG_2LN			= 0x18;
const short DWG_DIM_RADIUS			= 0x19;
const short DWG_DIM_DIAMETER		= 0x1A;
const short DWG_POINT				= 0x1B;
const short DWG_3DFACE				= 0x1C;
const short DWG_POLYLINE_PFACE		= 0x1D;
const short DWG_POLYLINE_MESH		= 0x1E;
const short DWG_SOLID				= 0x1F;
const short DWG_TRACE				= 0x20;
const short DWG_SHAPE				= 0x21;
const short DWG_VIEWPORT 			= 0x22;
const short DWG_ELLIPSE				= 0x23;
const short DWG_SPLINE 				= 0x24;
const short DWG_REGION				= 0x25;
const short DWG_3DSOLID				= 0x26;
const short DWG_BODY				= 0x27;
const short DWG_RAY					= 0x28;
const short DWG_XLINE				= 0x29;
const short DWG_DICTIONARY			= 0x2A;
const short DWG_0x2B				= 0x2B;
const short DWG_MTEXT				= 0x2C;
const short DWG_LEADER				= 0x2D;
const short DWG_TOLERANCE			= 0x2E;
const short DWG_MLINE				= 0x2F;
const short DWG_BLOCK_CTRL_OBJ		= 0x30;
const short DWG_BLOCK_HEADER		= 0x31;
const short DWG_LAYER_CTRL_OBJ		= 0x32;
const short DWG_LAYER				= 0x33;
const short DWG_STYLE_CTRL_OBJ		= 0x34;
const short DWG_STYLE				= 0x35;
const short DWG_0x36				= 0x36;
const short DWG_0x37				= 0x37;
const short DWG_LTYPE_CTRL_OBJ		= 0x38;
const short DWG_LTYPE				= 0x39;
const short DWG_0x3A				= 0x3A;
const short DWG_0x3B				= 0x3B;
const short DWG_VIEW_CTRL_OBJ		= 0x3C;
const short DWG_VIEW				= 0x3D;
const short DWG_UCS_CTRL_OBJ		= 0x3E;
const short DWG_UCS					= 0x3F;
const short DWG_VPORT_CTRL_OBJ		= 0x40;
const short DWG_VPORT				= 0x41;
const short DWG_APPID_CTRL_OBJ		= 0x42;
const short DWG_APPID				= 0x43;
const short DWG_DIMSTYLE_CTRL_OBJ	= 0x44;
const short DWG_DIMSTYLE			= 0x45;
const short DWG_VP_ENT_HDR_CTRL_OBJ	= 0x46;
const short DWG_VP_ENT_HDR			= 0x47;
const short DWG_GROUP				= 0x48;
const short DWG_MLINESTYLE			= 0x49;
const short DWG_POLYGON				= 0x4D;	// may be a guess

#endif
