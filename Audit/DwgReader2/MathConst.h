
#ifndef _MATHCONST_H
#define _MATHCONST_H

#include <math.h>  // for fabs()


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: When changing values in this header, be sure to
// make the corresponding change in Bbi/System/Const.java
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const double UNDEFINED    = 1.e9;
const int	 IUNDEFINED	  = 1000000000;

const double SMALL        = 1.e-6;
const double VECTOR_SMALL = 1.e-12;

const double PI           = 3.14159265358979323846;
const double QUARTERPI    = (0.25 * PI);
const double HALFPI       = (0.5 * PI);
const double THREEHALFPI  = (1.5 * PI);
const double TWOPI        = (2.0 * PI);
const double FOURPI       = (4.0 * PI);

const double RAD2DEG      = (180.0 / PI);
const double DEG2RAD      = (PI / 180.0);

const int CW    = -1;
const int CCW   =  1;

const int RIGHT = -1;
const int NONE  =  0;
const int LEFT  =  1;

#define EQUAL( in_a, in_b )		(fabs(in_a - in_b) < SMALL)
#define CLOSE( in_a, in_b, tol )	(fabs(in_a - in_b) < tol)
#define ZERO( in_a )					(fabs(in_a) < SMALL)
#define ZEROVEC( in_a )					(fabs(in_a) < VECTOR_SMALL)
#define ONE( in_a )					(fabs(in_a) >= SMALL)

#define SGN( in_a )					( EQUAL(in_a, 0) ? 0 : ((in_a < 0) ? -1 : 1) )

#endif
