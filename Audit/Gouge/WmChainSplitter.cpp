
#include "stdafx.h"
#include <math.h>

#include "Return.h"
#include "WmSubchn.h"
#include "WmChainSplitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmChainSplitter::CWmChainSplitter()
{

}

CWmChainSplitter::~CWmChainSplitter()
{
}

// NOTE: With V16 it became apparent that unnecessary subchain nodes
// were being inserted a the start/end of some curves that were not
// touching other curves (at the end where the insertion occurred).
// In turn, this caused interference index assignment to fail.
// This situtation is now controlled by the variable 'okaySplit'.
//
// This particular bug was manifested by V16 during resolution of
// the offset of a nesting part (now RtlSource\Baseline\V16_009seed.mm2)
//
void
CWmChainSplitter::Split( CWmChainIntsctor* intsctor )
{
	C3dCoord	ptA;
	C3dCoord	ptB;
	CWmCrvRecExtData* extA;
	const CWmIntRec* intrec;
	CWmSubchn*	currSubchn;
	CWmChain*	currChain;
	CWmSubchn*	nextSubchn;
	CWmSubchn*	subchn;
	CWmCrvRec*	crvrec;
	CWmElem*	currElem;
	CWmElem*	otherElem;
	C2dCoord	ipt;
	CGeoElem*	elem;
	CGeoElem*	copy;
	int			icnt, indx;
	int			jcnt, jndx;
	bool		okaySplit;

	// By finalizing the intersector, we ensure that all intersected
	// curves will be tabulated in the range u =[0..1]
	intsctor->Finalize();

	CWmCrvRecList& crvs = intsctor->CrvRecs();
	CWmIntRecList& ints = intsctor->IntRecs();

	icnt = crvs.Count();

	for (indx = 0; indx < icnt; ++indx)
	{
		crvrec = crvs[indx];

		currElem = crvrec->WmElem();

		currSubchn = currElem->Owner();
		currChain = currSubchn->Owner();

		jcnt = crvrec->Count();
		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			extA = crvrec->ExtData( jndx );

			intrec = extA->IntRec();

			if (intrec != NULL)
			{
				// There is an intersection at this point.

				nextSubchn = currSubchn->SubchnNext();
				subchn = NULL;

				okaySplit = TRUE;

				if (jndx == 0)
				{
					// At the start of the entity.
					otherElem = dynamic_cast<CWmElem*>( currElem->Prev() );
					if (otherElem != NULL)
					{
						ptA = currElem->Elem()->StartPt();
						ptB = otherElem->Elem()->EndPt();
						okaySplit = ptA.WithinTol( ptB, SMALL );
					}
					else
					{
						subchn = dynamic_cast<CWmSubchn*>( currElem->Prev() );
					}
				}
				else if (jndx == (jcnt - 1))
				{
					// At the end of the entity.
					otherElem = dynamic_cast<CWmElem*>( currElem->Next() );
					if (otherElem != NULL)
					{
						ptA = currElem->Elem()->EndPt();
						ptB = otherElem->Elem()->StartPt();
						okaySplit = ptA.WithinTol( ptB, SMALL );
					}
					else
					{
						subchn = dynamic_cast<CWmSubchn*>( currElem->Next() );
					}
				}
				else
				{
					// Mid-entity.
					subchn = NULL;

					// 2005.01.12 (PE) -- Encountered a very short arc
					// that degenerated into a full circle when it was
					// split at the interior intersection point.
					ipt = intrec->Point();
					elem = currElem->Elem();

					ptA = currElem->Elem()->StartPt();
					okaySplit = ( !ptA.WithinTolXY( ipt, SMALL ) );
					if ( okaySplit )
					{
						ptB = currElem->Elem()->EndPt();
						okaySplit = ( !ptB.WithinTolXY( ipt, SMALL ) );
					}
				}

				if (subchn == NULL && okaySplit)
				{
					// Split the chain at this intersection point.

					subchn = new CWmSubchn( currChain );

					subchn->SubchnPrev( currSubchn );
					subchn->SubchnNext( nextSubchn );

					currSubchn->SubchnNext( subchn );
					nextSubchn->SubchnPrev( subchn );

					if (jndx == 0)
					{
						currElem->Prev( subchn );
					}
					else if (jndx == (jcnt - 1))
					{
						currElem->Next( subchn );
					}
					else
					{
						ipt = intrec->Point();

						elem = currElem->Elem();
						copy = elem->Clone( true );

						copy->EndPt( ipt );
						elem->StartPt( ipt );

						CWmElem* newElem = new CWmElem( currSubchn, copy );
						currElem->Prev( newElem );

						newElem->Next( subchn );
					}

					subchn->OwnerShipUpdate();
				}
				else
				{
					// Mark this as a redundant intersection for removal.
					// See also comment in CWmChainIntsctor::Reduce()

					extA->Tag();
				}

				if ( okaySplit )
				{
					extA->Subchn( subchn );

					currSubchn = subchn;
				}
			}
		}
	}

	// Conditionally clean up the intersection table.
	intsctor->Reduce();
}
