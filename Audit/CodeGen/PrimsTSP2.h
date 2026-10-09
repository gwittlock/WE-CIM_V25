#ifndef _PRIMSTSP2_H
#define _PRIMSTSP2_H
/*
 * Simulated annealing and the Symmetric Euclidian Traveling Salesman Problem (TSP).
 *
 * Solution based on local search heuristics for
 * non-crossing paths and nearest neighbors 
 *
 * Storage Requirements: n^2+4n ints
 *
 * Problem: given the coordinates of n cities in the plane, find a
 * permutation pi_1, pi_2, ..., pi_n of 1, 2, ..., n that minimizes
 * sum for 1<=i<n D(pi_i,pi_i+1), where D(i,j) is the euclidian
 * distance between cities i and j
 *
 * Note: with n cities, there is (n-1)!/2 possible tours.
 * factorial(10)=3628800  factorial(50)=3E+64  factorial(150)=5.7E+262
 * If we could check one tour per clock cycle on a 100 MHZ computer, we
 * would still need to wait approximately 10^236 times the age of the
 * universe to explore all tours for 150 cities. 
 * 
 * Original source by Maugis Lionel (1995) (Sofreavia)
 *    maugis@cenaath.cena.dgac.fr
 *
 * Original source code located via 
 *    http://www.cs.sunysb.edu/~algorith/implement/tsp/implement.shtml
 *
 * Note, elsewhere on the web, there are numerous examples of solutions
 * based on Genetic Algorithms (GA) and Ant Colony behavior.
 */

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stdafx.h"
#include "Type.h"
#include "Return.h"
#include "City.h"


typedef int tTSP_path[3];

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CPrimsTSP2 : public CObject
{
public:

	CPrimsTSP2();

	CReturn Solve( tCity* cities, int count );

	// Using file-based input.
	CReturn	Debug( const char* path );

	virtual ~CPrimsTSP2();

private:

	CReturn	Allocate( int nCities );
	void	Deallocate();
	CReturn	DataInit( tCity* cities );
	double	PathLength( int* order );

	int		MOD( int a, int b );
	CReturn	EulerPath();
	double	getThreeWayCost( tTSP_path p );
	void	doThreeWay( tTSP_path p );
	double	getReverseCost( tTSP_path p );
	void	doReverse( tTSP_path p );
	void	Anneal();

	CReturn	Reorder( tCity* resulting_tour );

	// For debugging.
	void	DumpFinalSolution(
					const char*	path,
					tCity*		cities,
					int			nCities );

private:
	// Disabled.
	CPrimsTSP2( const CPrimsTSP2& );
	const CPrimsTSP2& operator = ( const CPrimsTSP2& );
	int operator == ( const CPrimsTSP2& ) const;
	int operator != ( const CPrimsTSP2& ) const;

private:

	// The count of cities.
	int		m_nCities;

	// An N array of all city locations.
	tCity*	m_cities;

	// An N-squared array representing the distances between
	// all pairs of cities (ie. the minumum spanning tree).
	double*	m_dist;

	// An N array of visitation indices.
	int*    m_iorder;
	int*	m_jorder;

	double	m_extents[4];

	// For debugging.
	bool	m_debug;
	FILE*	m_output_file;
};

#endif

