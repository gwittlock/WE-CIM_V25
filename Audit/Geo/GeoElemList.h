
#ifndef _GEOELEMLIST_H
#define _GEOELEMLIST_H

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif


// class CGeoElem;
// typedef CIndxList<CGeoElem*> CGeoElemList;

class CGeoPoint;
typedef CIndxList<CGeoPoint*> CGeoPointList;

class CGeoLine;
typedef CIndxList<CGeoLine*> CGeoLineList;

#endif
