
#include "stdafx.h"

#include "MathConst.h"
#include "Register.h"
#include "GeoLine.h"
#include "WmElem.h"
#include "WmSubchn.h"
#include "WmChain.h"
#include "WmChainIntsctor.h"
#include "WmChainSplitter.h"
#include "WmChainAssigner.h"
#include "WmChainResolver.h"
#include "WmChainIterator.h"
#include "Raster.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CRaster::CRaster()

	: m_results()
{
}

CRaster::~CRaster()
{
	Flush();
}

// 2007.06.06 (PE) -- Introduced set-back for General Thermodynamics
// because the ends of the slitting moves actually nicked the parts.
void
CRaster::Trim( CWmChainList* boundaries, CWmChainList* paths, double tol, double set_back )
{
	CWmChainIntsctor	intsctor;
	CWmChainSplitter	splitter;
	CWmChainAssigner	assigner;
	CWmChainResolver	resolver;
	CWmChainIterator	iter;
	CWmChainList		chainList;
	CWmChain*			bchain;
	CWmChain*			pchain;
	CWmChain*			chain;
	CWmSubchn*			wmsubchn;
	CWmElem*			wmelem;
	CGeoLine*			geo_line;
	C3dCoord			ps;
	C3dCoord			pe;
	C2dUnitVec			vec;
	int					bcnt, bndx;
	int					pcnt, pndx;
	int					count, indx;
	int					scnt;

	int debug = CRegister::Debug("WmChain");

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Intersect the boundary chains with the paths that cross them.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bcnt = boundaries->Count();
	pcnt = paths->Count();

	for (bndx = 0; bndx < bcnt; ++bndx)
	{
		bchain = (*boundaries)[bndx];
		for (pndx = 0; pndx < pcnt; ++pndx)
		{
			pchain = paths->GetAt( pndx );
			intsctor.Intersect( (*bchain), (*pchain), tol );
		}
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Split the chains at their points of intersection.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	splitter.Split( &intsctor );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Assign interference indices the the chain segments.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	for (bndx = 0; bndx < bcnt; ++bndx)
	{
		bchain = boundaries->GetAt( bndx );
		chainList.Append( bchain );
	}

	for (pndx = 0; pndx < pcnt; ++pndx)
	{
		pchain = paths->GetAt( pndx );
		chainList.Append( pchain );
	}

	assigner.InterferenceIndicesAssign( &chainList );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Extract the chain segments of interest from the crossing paths.
	// NOTE: In its initial usage, this method assumes the path chains
	// extend slightly beyond the edges of the outer boundary chain.
	// As such, the odd intervals are the ones we want to keep.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// The material boundary is used to limit trimming.
	bchain = boundaries->GetAt(0);

	chain = new CWmChain();
	m_results.Append( chain );

	count = bcnt + pcnt;
	for (indx = bcnt; indx < count; ++indx)
	{
		pchain = chainList[indx];

		iter.Init( (*pchain) );
		scnt = 0;

		while ( !iter.AtEnd() )
		{
			wmsubchn = iter.Subchn();

			if ((scnt & 0x1) && wmsubchn->Iindex() > -IUNDEFINED)
//			if (((scnt & 0x1) == 0) && (wmsubchn->Iindex() > -IUNDEFINED))
			{
				wmelem = iter.Elem();

				geo_line = ((wmelem == NULL) ? NULL : dynamic_cast<CGeoLine*>( wmelem->Elem() ));
				if (geo_line != NULL)
				{
					if (set_back > SMALL)
					{
						// Trim the pass as necessary. Note, it is okay
						// to modify the entity directly (ie. we do not
						// have to modify a copy of it).
						if (geo_line->Length2d() > (2. * set_back))
						{
							// Continually calculating the direction vector may
							// seem inefficient, but it is necessary because each
							// pass may alternate direction. Additionally, 'paths'
							// can (potentially) represent arbitrary curves!
							vec = geo_line->StartTan();

							ps = geo_line->StartPt();
							pe = geo_line->EndPt();

							// Be careful not to apply setback at material edge.
							if ( CanTrim( wmsubchn, bchain ) )
							{
								ps.X( ps.X() + (set_back * vec.X()) );
								ps.Y( ps.Y() + (set_back * vec.Y()) );
							}

							// Be careful not to apply setback at material edge.
							if ( CanTrim( wmsubchn->SubchnNext(), bchain ) )
							{
								pe.X( pe.X() - (set_back * vec.X()) );
								pe.Y( pe.Y() - (set_back * vec.Y()) );
							}

							geo_line->Init( ps, pe );
						}
					}

					chain->CopyAppend( (*geo_line) );
				}
			}

			iter.NextSubchn();
			++scnt;
		}
	}
}

int
CRaster::Count() const
{
	return m_results.Count();
}

const CWmChainList&
CRaster::Results() const
{
	return m_results;
}

void
CRaster::Flush()
{
	m_results.DestructiveFlush();
}

bool
CRaster::CanTrim( const CWmSubchn* raster, const CWmChain* material )
{
	bool can_trim = true;

	if (raster != NULL)
	{
		CWmSubchn* sister = raster->Sister();
		can_trim = ((sister != NULL) && (sister->Owner() != material));
	}

	return can_trim;
}