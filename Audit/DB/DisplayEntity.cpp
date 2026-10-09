// ==================================================================
//		Display Entity
//
//	A single display "thing" -- which links back to a database
//	entity (by ID), and contains the drawing instructions for 
//	that entity.
//
// ==================================================================

#include "stdafx.h"

#include "MathConst.h"
#include "DbCurve.h"
#include "GeoLine.h"
#include "DisplayEntity.h"
#include "Solution.h"

// ==================================================================

CDisplayEntity::CDisplayEntity()
{
	m_entity = NULL;
}

CDisplayEntity::CDisplayEntity( 
	const CDbEntity*	in_entity )
{
	m_entity = in_entity;
}

CDisplayEntity::~CDisplayEntity()
{
	m_entity = NULL;
}

void
CDisplayEntity::Init( const CDbEntity* entity )
{
	m_entity = entity;
}

// ==================================================================
ID
CDisplayEntity::ID() const
{ 
	// This should be inline, but it causes compiler problems...
	// ... too many loops in the headers
	return m_entity->Id();
}

// ==================================================================
//		PntDistance
//
//	Get the true distance of a point from the display representation
//	of this entity.  Check each line segment OR dot position, 
//	returning the minimum perpindicular distance.
//
//	NOTE: There is a complicated little dance regarding hot-dots and
//	the end points of entities.  Prior to V16, hot-dots were primarily
//	triggered by 'system' point entities.  However, these entities are
//	never drawn because they are considered to be 'hidden'.  This is
//	especially true now that CModel::is_hidden() is used both for
//	entity display and entity selection.  As such, the following list
//	of display commands has been added to the prefered display commands
//	that trigger hot-dots:
//
//		DCMD_MOVETO
//		DCMD_LINETO
//		DCMD_ARCCCTO
//		DCMD_ARCCWTO
//
double
CDisplayEntity::PntDistance( 
	const C2dCoord&	in_pnt,
	double			in_max_dist,
	bool*			io_dot,			// if TRUE, check dots... FALSE, don't
	C3dCoord*		io_snap,
	bool*			io_end ) const
{
	C3dCoord	en_pt;
	C3dCoord	st_pt;
	C3dCoord	ct_pt;
	C3dCoord	near_pt;
	int			cmd_num, cmd_idx;
	bool		end;

	double min_dist = in_max_dist;

	(*io_end) = end = FALSE;

	// Instead of doing the display entity distance work in 2D, do it in
	// glorious 3D so we *do* have a Z... this way, when it transforms to the ref,
	// the Z doesn't get fucked upl  eww IT#351

	cmd_num = countCommand();
	if (*io_dot)
	{
		for (cmd_idx=0; cmd_idx<cmd_num; cmd_idx++)
		{
			// Draw it...
			// TODO:  Add commands to switch between actual geometry and display (eg: untested) geometry
			switch (Command( cmd_idx ))
			{
			case DCMD_DOT:
			case DCMD_SYSDOT:
			case DCMD_MOVETO:
			case DCMD_LINETO:
			case DCMD_ARCCCTO:
			case DCMD_ARCCWTO:
			case DCMD_HOTTEXT:
			case DCMD_TEXT:
				en_pt = WorldCoord( Param( cmd_idx ) );

				// Now, check against the dot
				C2dVec delta = (en_pt - in_pnt);
				double dist = delta.Length();
				if (dist < min_dist)
				{
					min_dist = dist;
					*io_snap = WorldCoord( Param( cmd_idx ) );
				}
				break;
			}
			st_pt = en_pt;
		}
	}

	if (min_dist == in_max_dist)
	{
		*io_dot = FALSE;

		for (cmd_idx=0; cmd_idx<cmd_num; cmd_idx++)
		{
			switch (Command( cmd_idx ))
			{
			case DCMD_TEXT:
				// TERMINATE
//					cmd_idx = cmd_num;
				{
				CString text;
				cmd_idx += decode_text( cmd_idx, &text );
				}
				break;

			case DCMD_START:
				end = FALSE;
				break;

			case DCMD_END:
				end = TRUE;
				break;

			case DCMD_MOVETO:
				en_pt = WorldCoord( Param( cmd_idx ) );
				break;

			case DCMD_ARCCTR:
				ct_pt = WorldCoord( Param( cmd_idx ) );
				break;

			case DCMD_LINETO:
				{
					en_pt = WorldCoord( Param( cmd_idx ) );
					
					CGeoLine line( st_pt, en_pt );
					if ( !EQUAL( line.Length2d(), 0.0 ) )
					{
						double par_u;
						double dist = line.PointClosest( in_pnt, &near_pt, &par_u );
						if ( (dist < min_dist)
							&& (par_u >= 0.0)
							&& (par_u <= 1.0) )
						{
							min_dist = dist;

							(*io_snap) = near_pt;
							(*io_end) = end;
						}
					}
				}
				break;

			case DCMD_ARCCCTO:
				{
					en_pt = WorldCoord( Param( cmd_idx ) );
						
					CGeoArc arc ( st_pt, en_pt, ct_pt, CCW );
					if ( !EQUAL( arc.Length2d(), 0.0 ) )
					{
						double par_u;
						double dist = arc.PointClosest( in_pnt, &near_pt, &par_u );
						if ( (dist < min_dist)
							&& (par_u >= 0.0)
							&& (par_u <= 1.0) )
						{
							min_dist = dist;

							(*io_snap) = near_pt;
							(*io_end) = end;
						}
					}
				}
				break;

			case DCMD_ARCCWTO:
				{
					// Yeah yeah whatever
					en_pt = WorldCoord( Param( cmd_idx ) );
					
					CGeoArc arc ( st_pt, en_pt, ct_pt, CW );
					if ( !EQUAL( arc.Length2d(), 0.0 ) )
					{
						double par_u;
						double dist = arc.PointClosest( in_pnt, &near_pt, &par_u );
						if ( (dist < min_dist)
							&& (par_u >= 0.0)
							&& (par_u <= 1.0) )
						{
							min_dist = dist;

							(*io_snap) = near_pt;
							(*io_end) = end;
						}
					}
				}
				break;
			}
			st_pt = en_pt;
		}
	}
	else
		(*io_dot) = TRUE;

	return min_dist;
}

bool
CDisplayEntity::Intersection( CDisplayEntity* ent_2,
	C3dCoord*			int_pnt )
{
	const CDbEntity*	db_ent = ent_2->Entity();

	const CDbCurve*	db_curve1 = dynamic_cast<const CDbCurve*>(m_entity);
	const CDbCurve*	db_curve2 = dynamic_cast<const CDbCurve*>(db_ent);

	if ( !db_curve1 || !db_curve2 )
		return FALSE;

	if (db_curve1->WorkplaneId() != db_curve2->WorkplaneId())
		return FALSE;

	CGeoCurve* curve1 = db_curve1->Curve();
	CGeoCurve* curve2 = db_curve2->Curve();

	C3dCoord	hit[2];
	bool		got = FALSE;

	if (CSolution::Intersect( *curve1, *curve2, TRUE, hit ))
	{
		got = TRUE;

		int soln = 0;
		if ((curve1->Type() == GEOARC) || (curve2->Type() == GEOARC))
		{
			double dxA = int_pnt->X() - hit[0].X();
			double dyA = int_pnt->Y() - hit[0].Y();
			double dsqrdA = dxA * dxA + dyA + dyA;

			double dxB = int_pnt->X() - hit[1].X();
			double dyB = int_pnt->Y() - hit[1].Y();
			double dsqrdB = dxB * dxB + dyB + dyB;

			soln = ((dsqrdA < dsqrdB) ? 0 : 1);
		}
		
		(*int_pnt) = hit[soln];
	}

	delete curve1;
	delete curve2;

	return got;
}



int
CDisplayEntity::decode_text( int cmd_idx, CString* text ) const
{
	__int64	cmd;

	cmd = m_command_list.GetAt( cmd_idx );
	int len = (WORD)(cmd >> 48);
	int	chunk = (len + 2 + 7) >> 3;

	int byte;
	for (int idx=0; idx<chunk; idx++)
	{
		if (!idx)
			byte = 2;
		else
		{
			byte = 0;
			cmd = m_command_list.GetAt( cmd_idx + idx );
		}

		while (byte < 8)
		{
			char chr = (BYTE)(cmd >> (56 - (byte*8)));
			(*text) += chr;

			byte++;
			len--;
			if (!len) break;
		}
	}

	return chunk;
}
