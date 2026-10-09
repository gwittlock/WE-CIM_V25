
#include "stdafx.h"
#include "Return.h"
#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "PunchDecomposer.h"



////////////////////////////////////////////////////////////////////////

CPunchDecomposer::CPunchDecomposer()
	: CShapeDecomposer()
{
}

CPunchDecomposer::~CPunchDecomposer()
{
}

CReturn
CPunchDecomposer::Decompose(
							const CShape&	theShape,
							int				iteration,
							TShapeArray*	shapes ) const
{
	CReturn status;

	EShape type = theShape.Type();

	shapes->DestructiveFlush();

	if (type == SHAPE_OBROUND)
	{
		status = ObroundDecompose( theShape, iteration, shapes );
	}
	else if (type == SHAPE_KEYHOLE)
	{
		status = KeyholeDecompose( theShape, iteration, shapes );
	}
	else if (type < SHAPE_ROUND || type >= SHAPE_TERMINAL)
	{
		status.Internal( IDS_INTERNAL_ERROR,
			"Warning from CPunchDecomposer::Decompose() -- could not decompose shape" );
	}

	return status;
}

// NOTE: The is very strong coupling with CShape::Normalize()
CReturn
CPunchDecomposer::ObroundDecompose(
								const CShape&	theShape,
								int				iteration,
								TShapeArray*	shapes ) const
{
	CReturn status;

	if (iteration == 0)
	{
		// Reduce to two circles and a rectangle.

		CGeoCurveArray curves;
		CShape* primitive;

		// The first arc.
		const CGeoArc* arcA = dynamic_cast<const CGeoArc*>( theShape[1] );
		curves.Append( new CGeoArc( arcA->CenterPt(), arcA->Radius(), arcA->Dir() ) );

		primitive = new CShape();
		primitive->Init( curves, FALSE );
		shapes->Append( primitive );
		curves.DestructiveFlush();

		// The second arc.
		const CGeoArc* arcC = dynamic_cast<const CGeoArc*>( theShape[3] );
		curves.Append( new CGeoArc( arcC->CenterPt(), arcC->Radius(), arcC->Dir() ) );

		primitive = new CShape();
		primitive->Init( curves, FALSE );
		shapes->Append( primitive );
		curves.DestructiveFlush();

		// The rectangle.
		const CGeoLine* lineB = dynamic_cast<const CGeoLine*>( theShape[0] );
		const CGeoLine* lineD = dynamic_cast<const CGeoLine*>( theShape[2] );
		Rectangle( lineB, lineD, &curves );

		primitive = new CShape();
		primitive->Init( curves, FALSE );
		shapes->Append( primitive );

		// NOTE: Because we are going to match a rectangular/square punch to
		// this rectangular portion of this decomposed obround, we can consider
		// punches that have a corner radius.
		primitive->pAttribs()->setInt( "~original_shape", SHAPE_OBROUND );

		curves.DestructiveFlush();
	}

	return status;
}

// NOTE: There is very strong coupling with CShape::Normalize()
CReturn
CPunchDecomposer::KeyholeDecompose(
								const CShape&	theShape,
								int				iteration,
								TShapeArray*	shapes ) const
{
	CReturn status;

	if (iteration == 0 || iteration == 1)
	{
		// Reduce to a big circle.

		CGeoCurveArray curves;
		CShape* primitive;

		const CGeoArc* arcC = dynamic_cast<const CGeoArc*>( theShape[3] );
		const CGeoArc* arcA = dynamic_cast<const CGeoArc*>( theShape[1] );
		const CGeoLine* lineB = dynamic_cast<const CGeoLine*>( theShape[0] );
		const CGeoLine* lineD = dynamic_cast<const CGeoLine*>( theShape[2] );

		curves.Append( new CGeoArc( arcA->CenterPt(), arcA->Radius(), arcA->Dir() ) );

		primitive = new CShape();
		primitive->Init( curves, FALSE );
		shapes->Append( primitive );
		curves.DestructiveFlush();

		if (iteration == 0)
		{
			// And an obround.

			Obround( lineB, lineD, arcC, &curves );

			primitive = new CShape();
			primitive->Init( curves, FALSE );
			shapes->Append( primitive );
			curves.DestructiveFlush();
		}
		else if (iteration == 1)
		{
			// A small circle and a rectangle.

			curves.Append( new CGeoArc( arcC->CenterPt(), arcC->Radius(), arcC->Dir() ) );

			primitive = new CShape();
			primitive->Init( curves, FALSE );
			shapes->Append( primitive );
			curves.DestructiveFlush();

			Rectangle( lineB, lineD, &curves );

			primitive = new CShape();
			primitive->Init( curves, FALSE );
			shapes->Append( primitive );
			curves.DestructiveFlush();
		}
	}

	return status;
}

// Where lineA & lineB represent opposing sides of the rectangle.
void
CPunchDecomposer::Rectangle(
						const CGeoLine*	lineA,
						const CGeoLine*	lineB,
						CGeoCurveArray*	curves ) const
{
	curves->Append( new CGeoLine( lineA->StartPt(), lineA->EndPt() ) );
	curves->Append( new CGeoLine( lineA->EndPt(), lineB->StartPt() ) );
	curves->Append( new CGeoLine( lineB->StartPt(), lineB->EndPt() ) );
	curves->Append( new CGeoLine( lineB->EndPt(), lineA->StartPt() ) );
}

// Where lineA & lineB represent opposing sides of the obround.
// Where arcC establishes the radius and direction of the arcs.
void
CPunchDecomposer::Obround(
						const CGeoLine*	lineA,
						const CGeoLine*	lineB,
						const CGeoArc*	arcC,
						CGeoCurveArray*	curves ) const
{
	C3dCoord ps, pe, pc;

	curves->Append( new CGeoLine( lineA->StartPt(), lineA->EndPt() ) );

	ps = lineA->EndPt();
	pe = lineB->StartPt();
	pc = ps + ((pe - ps) * 0.5);
	curves->Append( new CGeoArc( ps, pe, pc, arcC->Dir() ) );

	curves->Append( new CGeoLine( lineB->StartPt(), lineB->EndPt() ) );

	ps = lineB->EndPt();
	pe = lineA->StartPt();
	pc = ps + ((pe - ps) * 0.5);
	curves->Append( new CGeoArc( ps, pe, pc, arcC->Dir() ) );
}
