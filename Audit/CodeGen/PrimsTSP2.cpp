
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "PrimsTSP2.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: Potential problems
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// 1. Failure will occur if you are using the home-brew random
//    number generator and you forget to call initRand().
//
// 2. Results are sensitve to XY scaling.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define D(x,y) m_dist[ (x) * m_nCities + y ]

#if ORIGINAL_CODE

// X-preferential scaling
#define SCALEX_XPREF(x) (50+500.0/(m_extents[1]-m_extents[0])*((x) - m_extents[0]))
#define SCALEY_XPREF(y) (50+700.0/(m_extents[3]-m_extents[2])*((y) - m_extents[2]))

// Y-preferential scaling
#define SCALEX_YPREF(x) (50+700.0/(m_extents[1]-m_extents[0])*((x) - m_extents[0]))
#define SCALEY_YPREF(y) (50+500.0/(m_extents[3]-m_extents[2])*((y) - m_extents[2]))

#else

// X-preferential scaling
#define SCALEX_XPREF(x) ( (x) )
#define SCALEY_XPREF(y) ( (y) )

// Y-preferential scaling
#define SCALEX_YPREF(x) ( (x) )
#define SCALEY_YPREF(y) ( (y) )

#endif

#define T_INIT                        100
#define FINAL_T                       0.1
#define COOLING                       0.9 /* to lower down T (< 1) */
#define TRIES_PER_T                   (500 * m_nCities)
#define IMPROVED_PATH_PER_T           (60 * m_nCities)

#define sqr(x)   ((x)*(x))
//#define min(a,b) (a)<(b)?(a):(b)
//#define max(a,b) (a)>(b)?(a):(b)

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Portable Uniform Integer Random Number in [0-2^31] range
// Performs better than ansi-C rand() 
// D.E Knuth, 1994 - The Stanford GraphBase
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define two_to_the_31   ((unsigned long)0x80000000) 
#define RREAL           ((double)RANDOM()/(double)two_to_the_31)

// Use the standard rand() in case the homebrew gives trouble.
#define HOME_BREWED_RANDOM 1
#if HOME_BREWED_RANDOM

	#define RANDOM()        (*rand_fptr >= 0 ? *rand_fptr-- : flipCycle ()) 

	static long A[56]= {-1};
	long *rand_fptr = A;

	#define mod_diff(x,y)   (((x)-(y))&0x7fffffff) 
	long flipCycle()
	{
		register long *ii,*jj;
		for (ii = &A[1], jj = &A[32]; jj <= &A[55]; ii++, jj++)
		*ii= mod_diff (*ii, *jj);

		for (jj = &A[1]; ii <= &A[55]; ii++, jj++)
		*ii= mod_diff (*ii, *jj);
		rand_fptr = &A[54];
		return A[55];
	}

	void initRand (long seed)
	{
		register long i;
		register long prev = seed, next = 1;
		seed = prev = mod_diff (prev,0);
		A[55] = prev;
		for (i = 21; i; i = (i+21)%55)
		{
			A[i] = next;
			next = mod_diff (prev, next);
			if (seed&1) seed = 0x40000000 + (seed >> 1);
			else seed >>= 1;
			next = mod_diff (next,seed);
			prev = A[i];
		}
		
		for (i = 0; i < 7; i++) flipCycle(); 
	}

#else

	#define RANDOM()	(rand())

#endif

long unifRand (long m)
{
	register unsigned long t = two_to_the_31 - (two_to_the_31%m);
	register long r;
	do {
		r = RANDOM();
	} while (t <= (unsigned long)r);
	return r%m;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CPrimsTSP2::CPrimsTSP2()
	: m_cities(NULL),
	  m_dist(NULL),
	  m_iorder(NULL),
	  m_jorder(NULL),
	  m_nCities(0),
	  m_debug(FALSE),
	  m_output_file(NULL)
{
}

CPrimsTSP2::~CPrimsTSP2()
{
	Deallocate();
}

CReturn
CPrimsTSP2::Solve( tCity* cities, int count )
{
	CReturn status;
	CString	msg;
	int		indx;
	long  seed = -314159L;

	Deallocate();

	status = Allocate( count );

	if ( status.IsOk() )
	{
		m_output_file = (m_debug ? fopen("c:\\_tmp\\_tsp_debug2.txt","w") : NULL);

#if HOME_BREWED_RANDOM
		initRand (seed);
#endif

		DataInit( cities );

		if (m_debug)
		{
			msg.Format( "count: %d", count );
			status.Diagnostic( msg );

			for (indx = 0; indx < count; ++indx)
			{
				msg.Format( "[%d] X:%8.4f Y:%8.4f P:0x%x",
					indx, cities[indx].x, cities[indx].y, cities[indx].ptr );
				status.Diagnostic( msg );
			}
		}

		// Set up the Eulerian path (m_iorder).
		EulerPath(); 

		// Improve upon the Eulerian path.
		Anneal();

		// Write the resulting tour back to the input, ensuring the
		// start point of the tour is the same as the given start point.
		Reorder( cities );

		if (m_debug)
		{
			for (indx = 0; indx < count; ++indx)
			{
				msg.Format( "[%d] iorder:%d", indx, m_iorder[indx] );
				status.Diagnostic( msg );
			}

			for (indx = 0; indx < count; ++indx)
			{
				msg.Format( "[%d] X:%8.4f Y:%8.4f P:0x%x",
					indx, cities[indx].x, cities[indx].y, cities[indx].ptr );
				status.Diagnostic( msg );
			}
		}

		// If you want to create a java macro that shows the results in WE-CIM.
		if ( m_debug )
			DumpFinalSolution("c:\\_tmp\\_tsp_output.java", cities, count );

		if (m_output_file != NULL)
			fclose(m_output_file);
	}

	return status;
}

CReturn
CPrimsTSP2::Allocate( int nCities )
{
	CReturn	status;

	if (nCities < 3)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CPrimsTSP2::Allocate(#1)" );
	}
	else
	{
		m_nCities = nCities;

		m_cities	= new tCity[ m_nCities ];
		m_dist		= new double[ m_nCities*m_nCities ];
		m_iorder	= new int[ m_nCities ];
		m_jorder	= new int[ m_nCities ];

		if ((m_cities == NULL) ||
			(m_dist == NULL)   ||
			(m_iorder == NULL) ||
			(m_jorder == NULL))
		{
			status.Internal( IDS_INTERNAL_ERROR, "CPrimsTSP2::Allocate(#2)" );
		}
	}

	return status;
}

void
CPrimsTSP2::Deallocate()
{
	delete [] m_cities;
	m_cities = NULL;

	delete [] m_dist;
	m_dist = NULL;

	delete [] m_iorder;
	m_iorder = NULL;

	delete [] m_jorder;
	m_jorder = NULL;
}

CReturn
CPrimsTSP2::DataInit( tCity* cities )
{
	CReturn	status;
	CString	msg;
	double	dx_cities;
	double	dy_cities;
	double	dx, dy;
	int		indx, jndx;

	for (indx = 0; indx < m_nCities; ++indx)
	{
		m_cities[indx] = cities[indx];

		// Track the xy extents of the sample.
		if (indx==0)
		{
			m_extents[0] = m_cities[indx].x;
			m_extents[1] = m_extents[0]; 
			m_extents[2] = m_cities[indx].y;
			m_extents[3] = m_extents[2];
		}
		else
		{
			m_extents[0] = min(m_extents[0],m_cities[indx].x);
			m_extents[1] = max(m_extents[1],m_cities[indx].x);
			m_extents[2] = min(m_extents[2],m_cities[indx].y);
			m_extents[3] = max(m_extents[3],m_cities[indx].y);
		}
	}

	dx_cities = fabs(m_extents[1] - m_extents[0]);
	dy_cities = fabs(m_extents[3] - m_extents[2]);


	// Compute inter city distance matrix.
	for (indx = 0; indx < m_nCities; indx++)
	{
		// identity permutation
		m_iorder[indx] = indx;

		for (jndx = 0; jndx < m_nCities; jndx++)
		{
			if (dy_cities > dx_cities)
			{
				dx = SCALEX_XPREF(m_cities[indx].x) - SCALEX_XPREF(m_cities[jndx].x);
				dy = SCALEY_XPREF(m_cities[indx].y) - SCALEY_XPREF(m_cities[jndx].y);
			}
			else
			{
				dx = SCALEX_YPREF(m_cities[indx].x) - SCALEX_YPREF(m_cities[jndx].x);
				dy = SCALEY_YPREF(m_cities[indx].y) - SCALEY_YPREF(m_cities[jndx].y);
			}

			// D(indx,jndx) satisfies triangle inequality
			// D(indx,jndx) = sqrt ((int)(dx*dx + dy*dy));
			D(indx,jndx) = sqrt(sqr(dx) + sqr(dy));

			if (m_output_file != NULL)
				fprintf( m_output_file, "(%d,%d) %d\n", indx, jndx, D(indx,jndx) );
		}
	}

	if (m_debug)
	{
		msg.Format( "Initial Path Length: %f\n", PathLength(m_iorder) );
		status.Diagnostic( msg );
	}

	if (m_output_file != NULL)
		fprintf( m_output_file, "%s", msg );

	return status;
}

double
CPrimsTSP2::PathLength( int* order )
{
	double len = 0;
	for (int indx = 0; indx < m_nCities-1; indx++)
	{
		len += D(order[indx], order[indx+1]);
	}
	len += D(order[m_nCities-1], order[0]); /* close path */
	return (len);
}

int
CPrimsTSP2::MOD( int a, int b )
{
	int c = a % b;
	return ((c >= 0) ? c : c+b);
}

/*
 * Prim's approximated TSP tour
 * See also [Cristophides'92]
 *
 * From http://www.ics.uci.edu/~eppstein/161/960206.html
 * Prim's algorithm
	Rather than build a subgraph one edge at a time, Prim's algorithm builds a 
	tree one vertex at a time.

		Prim's algorithm:
		let T be a single vertex x
		while (T has fewer than n vertices)
		{
			find the smallest edge connecting T to G-T
			add it to T
		}

	Since each edge added is the smallest connecting T to G-T, the lemma we proved
	shows that we only add edges that should be part of the MST.

	Again, it looks like the loop has a slow step in it. But again, some data
	structures can be used to speed this up. The idea is to use a heap to remember,
	for each vertex, the smallest edge connecting T with that vertex.

		Prim with heaps:
		make a heap of values (vertex,edge,weight(edge))
			initially (v,-,infinity) for each vertex
			let tree T be empty
		while (T has fewer than n vertices)
		{
			let (v,e,weight(e)) have the smallest weight in the heap
			remove (v,e,weight(e)) from the heap
			add v and e to T
			for each edge f=(u,v)
			if u is not already in T
				find value (u,g,weight(g)) in heap
				if weight(f) < weight(g)
				replace (u,g,weight(g)) with (u,f,weight(f))
		}

	Analysis: We perform n steps in which we remove the smallest element in the heap,
	and at most 2m steps in which we examine an edge f=(u,v). For each of those steps,
	we might replace a value on the heap, reducing it's weight. (You also have to find
	the right value on the heap, but that can be done easily enough by keeping a pointer
	from the vertices to the corresponding values.) I haven't described how to reduce
	the weight of an element of a binary heap, but it's easy to do in O(log n) time.
	Alternately by using a more complicated data structure known as a Fibonacci heap,
	you can reduce the weight of an element in constant time. The result is a total
	time bound of O(m + n log n).
 */
CReturn
CPrimsTSP2::EulerPath()
{
	CReturn	status;
	CString	msg;
	int*	mst;
	int*	arc;
	double*	dis;
	double	d;
	int		i, j, k, l, a;
	double	maxd;

	mst = new int[m_nCities];
	arc = new int[m_nCities];
	dis = new double[m_nCities];

	if (mst == NULL || arc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CPrimsTSP2::EulerPath(#1)" );
	}
	else
	{
		j = 0;  // Prevents crash with regular grid of points!

		maxd = sqr(m_extents[1]-m_extents[0]) + sqr(m_extents[3]-m_extents[2]);
		d    = maxd;
		dis[0] = -1;
		arc[0] = 0;
		for (i = 1; i < m_nCities; i++)
		{
			dis[i] = D(i,0);
			arc[i] = 0;
			if (d > dis[i])
			{
				d = dis[i];
				j = i;
			}
		}

		/*
		 * O(n^2) Minimum Spanning Trees by Prim and Jarnick 
		 * for graphs with adjacency matrix. 
		 */
		k = 0;  // Prevents crash with regular grid of points!
		for (a = 0; a < m_nCities - 1; a++)
		{
			mst[a] = j * m_nCities + arc[j]; /* join fragment j with Minimum Spanning Tree */
			dis[j] = -1; 
			d = maxd;

			//	k = 0;  // Prevents crash with regular grid of points!
			for (i = 0; i < m_nCities; i++)
			{
				if (dis[i] >= 0) /* not connected yet */
				{
					if (dis[i] > D(i,j))
					{
						dis[i] = D(i,j);
						arc[i] = j;
					}

					if (d > dis[i])
					{
						d = dis[i];
						k = i;
					}
				}
			}
			j = k;
		}

		/*
		 * Preorder Tour of Minimum Spanning Tree
		 */
		#define VISITED(x) m_jorder[x]
		#define NQ(x) arc[l++] = x
		#define DQ()  arc[--l]
		#define EMPTY (l==0)
			
		for (i = 0; i < m_nCities; i++)
			VISITED(i) = 0;

		k = 0;
		l = 0;
		d = 0; NQ(0);
		while (!EMPTY)
		{
			i = DQ();
			if (!VISITED(i))
			{
				m_iorder[k++] = i;
				VISITED(i)  = 1;			
				for (j = 0; j < m_nCities - 1; j++) /* push all kids of i */
				{
					if (i == mst[j]%m_nCities)
						NQ(mst[j]/m_nCities); 
				}	
			}
		}

		if (m_debug)
		{
			msg.Format( "Approximated Path Length: %f\n", PathLength(m_iorder) );
			status.Diagnostic( msg );
		}
		
		if (m_output_file != NULL)
			fprintf( m_output_file, "%s", msg );
	}

	delete [] mst;
	delete [] arc;
	delete [] dis;

	return status;
}

/*
 * Local Search Heuristics
 *  b-------a        b       a
 *  .       .   =>   .\     /.
 *  . d...e .        . e...d .  
 *  ./     \.        .       .
 *  c       f        c-------f
 */
double
CPrimsTSP2::getThreeWayCost( tTSP_path p )
{
	int a, b, c, d, e, f;
	a = m_iorder[MOD(p[0]-1,m_nCities)];
	b = m_iorder[p[0]];
	c = m_iorder[p[1]];
	d = m_iorder[MOD(p[1]+1,m_nCities)];
	e = m_iorder[p[2]];
	f = m_iorder[MOD(p[2]+1,m_nCities)];
	return (D(a,d) + D(e,b) + D(c,f) - D(a,b) - D(c,d) - D(e,f)); 
        /* add cost between d and e if non symmetric TSP */ 
}

void
CPrimsTSP2::doThreeWay( tTSP_path p )
{
	int i, count, m1, m2, m3, a, b, c, d, e, f;

	a = MOD(p[0]-1,m_nCities);
	b = p[0];
	c = p[1];
	d = MOD(p[1]+1,m_nCities);
	e = p[2];
	f = MOD(p[2]+1,m_nCities);	
	
	m1 = MOD(m_nCities+c-b,m_nCities)+1;  /* num m_cities from b to c */
	m2 = MOD(m_nCities+a-f,m_nCities)+1;  /* num m_cities from f to a */
	m3 = MOD(m_nCities+e-d,m_nCities)+1;  /* num m_cities from d to e */

	count = 0;

	/* [b..c] */
	for (i = 0; i < m1; i++)
		m_jorder[count++] = m_iorder[MOD(i+b,m_nCities)];

	/* [f..a] */
	for (i = 0; i < m2; i++)
		m_jorder[count++] = m_iorder[MOD(i+f,m_nCities)];

	/* [d..e] */
	for (i = 0; i < m3; i++)
		m_jorder[count++] = m_iorder[MOD(i+d,m_nCities)];

	/* copy segment back into m_iorder */
	for (i = 0; i < m_nCities; i++)
		m_iorder[i] = m_jorder[i];
}

/*
 *   c..b       c..b
 *    \/    =>  |  |
 *    /\        |  |
 *   a  d       a  d
 */
double
CPrimsTSP2::getReverseCost( tTSP_path p )
{
	int a, b, c, d;
	a = m_iorder[MOD(p[0]-1,m_nCities)];
	b = m_iorder[p[0]];
	c = m_iorder[p[1]];
	d = m_iorder[MOD(p[1]+1,m_nCities)];
	
	return (D(d,b) + D(c,a) - D(a,b) - D(c,d));
        /* add cost between c and b if non symmetric TSP */ 
}

void
CPrimsTSP2::doReverse( tTSP_path p )
{
	int i, nswaps, first, last, tmp;

	/* reverse path b...c */
	nswaps = (MOD(p[1]-p[0],m_nCities)+1)/2;
	for (i = 0; i < nswaps; i++)
    {
		first = MOD(p[0]+i, m_nCities);
		last  = MOD(p[1]-i, m_nCities);

		tmp   = m_iorder[first];
		m_iorder[first] = m_iorder[last];
		m_iorder[last]  = tmp;
    }
}

// 2005.02.12 (PE) -- Added special case "logic" for 3 cities.  Said case,
// found by Ned, caused infinite loops.  Initial attempts to avoid infinite
// loops resulted in crash, during destruction of m_jorder.  I was unable
// to determine why the crash occurred, but it seemed to be associated
// with calling getThreeWayCost().
void
CPrimsTSP2::Anneal()
{
	tTSP_path	p;
	CReturn		status;
	CString		msg;
	long		random;
	bool		early_exit;

	int    i=1, j, pathchg;
	int    numOnPath, numNotOnPath;
	double pathlen, bestlen;
	double energyChange, T;

	early_exit = FALSE;

	pathlen = PathLength (m_iorder); 
	bestlen = pathlen;

	for (T = T_INIT; T > FINAL_T; T *= COOLING)  /* annealing schedule */
    {
		pathchg = 0;
		for (j = 0; j < TRIES_PER_T; j++)
		{
			do
			{
				p[0] = unifRand (m_nCities);
				p[1] = unifRand (m_nCities);

				if (p[0] == p[1])
					p[1] = MOD(p[0]+1,m_nCities); /* non-empty path */

				numOnPath = MOD(p[1]-p[0],m_nCities) + 1;
				numNotOnPath = m_nCities - numOnPath;

				// 2005.02.12 (PE) -- See also note above.
				if ((m_nCities == 3) && (numOnPath == 3))
					break;

			} while (numOnPath < 2 || numNotOnPath < 2); /* non-empty path */
			
			random = RANDOM();
			//if (m_output_file != NULL)
			//	fprintf( m_output_file, "RANDOM() %d\n", random );

			// 2005.02.12 (PE) -- See also note above.
			//   if (random % 2)
			if ((m_nCities > 3) && (random % 2)) /*  threeWay */
			{
				do
				{
					p[2] = MOD(unifRand (numNotOnPath)+p[1]+1,m_nCities);
				} while (p[0] == MOD(p[2]+1,m_nCities)); /* avoids a non-change */

				energyChange = getThreeWayCost (p);
				if (energyChange < 0 || RREAL < exp(-energyChange/T) )
				{
					pathchg++;
					pathlen += energyChange;
					doThreeWay (p);
				}
			}
			else            /* path Reverse */
			{
				energyChange = getReverseCost (p);
				if (energyChange < 0 || RREAL < exp(-energyChange/T))
				{
					pathchg++;
					pathlen += energyChange;
					doReverse(p); 
				}
			}

			if (pathlen < bestlen)
			{
				bestlen = pathlen;

				// 2005.02.12 (PE) -- See also note above.
				early_exit = ((m_nCities == 3) && (bestlen < 0));
			}

			if (early_exit)
				break;

			if (pathchg > IMPROVED_PATH_PER_T)
				break; /* finish early */
		}

		if ( m_debug )
		{
			msg.Format("T:%f L:%d B:%d C:%d\n", T, pathlen, bestlen, pathchg);
			status.Diagnostic( msg );
		
			if (m_output_file != NULL)
				fprintf( m_output_file, "%s", msg );
		}

		if (early_exit)
			break;

		if (pathchg == 0)
			break;   /* if no change then quit */
    }

	if (m_debug)
	{
		msg.Format( "Best Path Length: %f\n", PathLength(m_iorder) );
		status.Diagnostic( msg );
	}

	if (m_output_file != NULL)
	{
		for (int indx = 0; indx < m_nCities; ++indx)
		{
			fprintf (m_output_file, "m_iorder[%d] = %d\n", indx, m_iorder[indx]);
		}
		fprintf( m_output_file, "%s", msg );
	}
}

CReturn
CPrimsTSP2::Reorder( tCity* resulting_tour )
{
	CReturn	status;
	int		count, indx;

	// Find the original starting city.
	for (indx = 0; indx < m_nCities; ++indx)
	{
		if (m_iorder[indx] == 0)
			break;
	}

	//--m_nCities;  // Leave as 'open' tour.
	count = 0;
	while (count < m_nCities)
	{
		if (indx >= m_nCities)
			indx = 0;

		resulting_tour[count] = m_cities[ m_iorder[indx] ]; 
		resulting_tour[count].tag = count;

		++indx;
		++count;
	}

	return status;
}

// For debugging.
void
CPrimsTSP2::DumpFinalSolution(
					const char*	path,
					tCity*		cities,
					int			nCities )
{
	FILE*	f;
	int		indx;

	f = fopen( path, "w");
	if (f != NULL)
	{
		fprintf( f,"\n" );
		fprintf( f,"import Weng.System.*;\n" );
		fprintf( f, "import Weng.Modeler.*;\n" );
		fprintf( f, "\n" );
		fprintf( f, "public class _tsp_output implements WengMacro\n" );
		fprintf( f, "{\n" );
		fprintf( f, "\tpublic void main()\n" );
		fprintf( f, "\t{\n" );

		for (indx = 1; indx < nCities; ++indx)
		{
			fprintf( f, "\t\tnew DbLine(%f,%f,0.,%f,%f,0.);\n",
				cities[indx-1].x, cities[indx-1].y,
				cities[indx].x, cities[indx].y );
		}

		fprintf( f, "\t}\n" );
		fprintf( f, "}\n" );
		fclose(f);
	}
}

// Using file-based input.
CReturn
CPrimsTSP2::Debug( const char* path )
{
	CReturn	status;
	CString	msg;
	tCity*	sample;
	FILE*	f;
	float	xp, yp;
	int		count, indx;

	sample = NULL;

	f = fopen( path, "r" );
	if (f == NULL)
	{
		msg.Format( "CPrimsTSP2::Debug() -- can not open input %s", path );
		status.Internal( IDS_INTERNAL_ERROR, msg );
	}
	else
	{
		if (fscanf(f,"%d", &count) == 1)
		{
			if (count > 0)
			{
				sample = new tCity[count];

				for (indx = 0; indx < count; indx++)
				{
					if (fscanf(f,"%f %f", &xp, &yp) != 2)
					{
						status.Internal( IDS_INTERNAL_ERROR, "CPrimsTSP2::Debug() -- bad input" );
						break;  // early termination at erroneous input
					}

					sample[indx].x = xp;
					sample[indx].y = yp;
					sample[indx].ptr = NULL;
				}

				m_debug = TRUE;

				m_output_file = (m_debug ? fopen("c:\\_tmp\\_tsp_debug2.txt","w") : NULL);

				status = Solve( sample, count );

				if (m_output_file != NULL)
					fclose(m_output_file);

				delete [] sample;
			}
		}

		fclose( f );
	}

	return status;
}

