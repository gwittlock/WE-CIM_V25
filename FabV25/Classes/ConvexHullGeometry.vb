Imports System.Drawing

Namespace ToolShapes


    Friend Enum ArcOverlapClassification

        None = 0

        PartStartInToolRange = 8

        PartEndInToolRange = 4

        ToolStartInPartRange = 2

        ToolEndInPartRange = 1

    End Enum
    Friend Module ConvexHullGeometry
        Private Const VectorTolerance As Double = 0.0000001
        Private Const VectorSmall As Double = 0.000000000001
        Private Const SmallTolerance As Double = 0.000001

        Public Function DirectionAngle(
            fromPoint As PointF,
            toPoint As PointF) As Double

            Return Math.Atan2(
                toPoint.Y - fromPoint.Y,
                toPoint.X - fromPoint.X)

        End Function

        Public Function NormalizeAngle(
            angle As Double) As Double

            Dim result As Double =
                angle Mod (2.0 * Math.PI)

            If result < 0.0 Then
                result += 2.0 * Math.PI
            End If

            Return result

        End Function

        Public Function CreateMoveFromLine(
    line As LineSegment2D) As ConvexHullArc

            If line Is Nothing Then
                Throw New ArgumentNullException(NameOf(line))
            End If

            Dim tangent As Double =
                DirectionAngle(
                    line.StartPoint,
                    line.EndPoint)

            Return New ConvexHullArc(
                line.EndPoint,
                tangent,
                tangent,
                0.0,
                False)

        End Function

        Public Function CreateTurnFromArc(
    arc As ArcSegment2D) As ConvexHullArc

            If arc Is Nothing Then
                Throw New ArgumentNullException(NameOf(arc))
            End If

            Dim startTangent As Double
            Dim endTangent As Double

            If arc.IsClockwise Then

                startTangent =
                    NormalizeAngle(
                        arc.StartAngle - Math.PI / 2.0)

                endTangent =
                    NormalizeAngle(
                        arc.EndAngle - Math.PI / 2.0)

            Else

                startTangent =
                    NormalizeAngle(
                        arc.StartAngle + Math.PI / 2.0)

                endTangent =
                    NormalizeAngle(
                        arc.EndAngle + Math.PI / 2.0)

            End If

            Return New ConvexHullArc(
                arc.Center,
                startTangent,
                endTangent,
                arc.Radius,
                arc.IsClockwise)

        End Function


        Public Function ConvertSegment(
    segment As IPathSegment) As ConvexHullArc

            If segment Is Nothing Then
                Throw New ArgumentNullException(NameOf(segment))
            End If

            Select Case segment.SegmentType

                Case PathSegmentType.Line

                    Dim line As LineSegment2D =
                        TryCast(segment, LineSegment2D)

                    If line Is Nothing Then
                        Throw New InvalidCastException(
                            "Line segment could not be converted to LineSegment2D.")
                    End If

                    Return CreateMoveFromLine(line)

                Case PathSegmentType.Arc

                    Dim arc As ArcSegment2D =
                        TryCast(segment, ArcSegment2D)

                    If arc Is Nothing Then
                        Throw New InvalidCastException(
                            "Arc segment could not be converted to ArcSegment2D.")
                    End If

                    Return CreateTurnFromArc(arc)

                Case Else

                    Throw New NotSupportedException(
                        "Unsupported path segment type: " &
                        segment.SegmentType.ToString())

            End Select

        End Function

        Public Function TangentsMatch(
    firstTangent As Double,
    secondTangent As Double,
    Optional tolerance As Double = 0.000001) As Boolean

            Dim difference As Double =
                NormalizeAngle(
                    firstTangent - secondTangent)

            If difference > Math.PI Then
                difference =
                    (2.0 * Math.PI) - difference
            End If

            Return Math.Abs(difference) <= tolerance

        End Function

        Public Function CreateTransition(
    point As PointF,
    incomingTangent As Double,
    outgoingTangent As Double,
    isClockwise As Boolean) As ConvexHullArc

            Return New ConvexHullArc(
                point,
                incomingTangent,
                outgoingTangent,
                0.0,
                isClockwise)

        End Function

        Public Function CenterVector(
    partArc As ConvexHullArc,
    toolArc As ConvexHullArc) As PointF

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If toolArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolArc))
            End If

            Return New PointF(
                partArc.Center.X - toolArc.Center.X,
                partArc.Center.Y - toolArc.Center.Y)

        End Function

        Public Function RadialVectorFromTangent(
    tangent As Double,
    offsetDirection As ProfileSide) As PointF

            Dim angle As Double

            If offsetDirection = ProfileSide.Outside Then
                angle = tangent - (Math.PI / 2.0)
            Else
                angle = tangent + (Math.PI / 2.0)
            End If

            Return New PointF(
                CSng(Math.Cos(angle)),
                CSng(Math.Sin(angle)))

        End Function

        Public Function ClassifyArcOverlap(
    partArc As ConvexHullArc,
    toolArc As ConvexHullArc,
    offsetDirection As Integer) As ArcOverlapClassification

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If toolArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolArc))
            End If

            Dim ts As PointF
            Dim te As PointF

            ' Get the radial vectors for the tool arc.
            If toolArc.ArcType = ConvexHullArcType.Turn AndAlso
   Not toolArc.IsClockwise Then

                ' CCW tool arc
                ts = New PointF(
        CSng(Math.Sin(toolArc.StartTangent)),
        CSng(-Math.Cos(toolArc.StartTangent)))

                te = New PointF(
        CSng(Math.Sin(toolArc.EndTangent)),
        CSng(-Math.Cos(toolArc.EndTangent)))

            Else

                ' MOVE / legacy non-CCW case
                ts = New PointF(
        CSng(-Math.Sin(toolArc.StartTangent)),
        CSng(Math.Cos(toolArc.StartTangent)))

                te = New PointF(
        CSng(-Math.Sin(toolArc.EndTangent)),
        CSng(Math.Cos(toolArc.EndTangent)))

            End If

            Dim partArcDir As Integer

            If partArc.ArcType = ConvexHullArcType.Move Then
                partArcDir = 0
            ElseIf partArc.IsClockwise Then
                partArcDir = -1
            Else
                partArcDir = 1
            End If

            Dim offsetDir As Integer = offsetDirection

            If ((offsetDir < 0) AndAlso (partArcDir < 0)) OrElse
       ((offsetDir > 0) AndAlso (partArcDir > 0)) Then

                ts = New PointF(-ts.X, -ts.Y)
                te = New PointF(-te.X, -te.Y)

            End If

            ' Get the radial vectors for the part arc.
            Dim psAngle As Double =
    partArc.StartTangent -
    (offsetDir * (Math.PI / 2.0))

            Dim peAngle As Double =
    partArc.EndTangent -
    (offsetDir * (Math.PI / 2.0))

            Dim ps As New PointF(
    CSng(Math.Cos(psAngle)),
    CSng(Math.Sin(psAngle)))

            Dim pe As New PointF(
    CSng(Math.Cos(peAngle)),
    CSng(Math.Sin(peAngle)))

            Dim psDts As Double =
        DotProduct(ps, ts)

            Dim psDte As Double =
        DotProduct(ps, te)

            Dim peDts As Double =
        DotProduct(pe, ts)

            Dim peDte As Double =
        DotProduct(pe, te)

            If psDts < -VectorTolerance Then
                Return ArcOverlapClassification.None
            End If

            If psDte < -VectorTolerance Then
                Return ArcOverlapClassification.None
            End If

            If peDts < -VectorTolerance Then
                Return ArcOverlapClassification.None
            End If

            If peDte < -VectorTolerance Then
                Return ArcOverlapClassification.None
            End If

            Dim classification As ArcOverlapClassification =
        ArcOverlapClassification.None

            Dim psXts As Double =
        CrossProduct(ps, ts)

            Dim psXte As Double =
        CrossProduct(ps, te)

            Dim peXts As Double =
        CrossProduct(pe, ts)

            Dim peXte As Double =
        CrossProduct(pe, te)

            Dim test As Integer

            test =
        SignWithTolerance(psXts) +
        SignWithTolerance(psXte)

            If test > -2 AndAlso test < 2 Then

                classification =
            classification Or
            ArcOverlapClassification.ToolStartInPartRange

            Else

                Dim tsXps As Double =
            CrossProduct(ts, ps)

                Dim tsXpe As Double =
            CrossProduct(ts, pe)

                test =
            SignWithTolerance(tsXps) +
            SignWithTolerance(tsXpe)

                If test > -2 AndAlso test < 2 Then

                    classification =
                classification Or
                ArcOverlapClassification.PartStartInToolRange

                End If

            End If

            test =
        SignWithTolerance(peXts) +
        SignWithTolerance(peXte)

            If test > -2 AndAlso test < 2 Then

                classification =
            classification Or
            ArcOverlapClassification.ToolEndInPartRange

            Else

                Dim teXps As Double =
            CrossProduct(te, ps)

                Dim teXpe As Double =
            CrossProduct(te, pe)

                test =
            SignWithTolerance(teXps) +
            SignWithTolerance(teXpe)

                If test > -2 AndAlso test < 2 Then

                    classification =
                classification Or
                ArcOverlapClassification.PartEndInToolRange

                End If

            End If

            Return classification

        End Function


        Public Function DotProduct(
                                 first As PointF,
                                second As PointF) As Double

            Return (first.X * second.X) + (first.Y * second.Y)

        End Function

        Public Function CrossProduct(
                                first As PointF,
                                second As PointF) As Double

            Return (first.X * second.Y) - (first.Y * second.X)

        End Function


        Public Function SignWithTolerance(
    value As Double) As Integer

            If Math.Abs(value) < VectorTolerance Then
                Return 0
            End If

            If value < 0.0 Then
                Return -1
            End If

            Return 1

        End Function


        Public Function SameDirection(
    partArc As ConvexHullArc,
    toolArc As ConvexHullArc) As Boolean

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If toolArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolArc))
            End If

            ' Legacy CChArc::SameDirection():
            ' A tool MOVE has no direction and always matches.
            If toolArc.ArcType = ConvexHullArcType.Move Then
                Return True
            End If

            If partArc.ArcType = ConvexHullArcType.Move Then
                Return False
            End If

            Dim toolDirection As Integer =
                If(toolArc.IsClockwise, -1, 1)

            Dim partDirection As Integer =
                If(partArc.IsClockwise, -1, 1)

            Return toolDirection = partDirection

        End Function

        Public Function SignDirection(
    value As Integer) As Integer

            If value < 0 Then
                Return -1
            End If

            If value > 0 Then
                Return 1
            End If

            Return 0

        End Function


        Public Function ConvolutionRadius(
    partArc As ConvexHullArc,
    toolArc As ConvexHullArc,
    offsetDirection As Integer) As Double

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If toolArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolArc))
            End If

            Const stockAmount As Double = 0.0

            Dim radius As Double

            If partArc.ArcType = ConvexHullArcType.Move Then

                radius =
                    toolArc.Radius + stockAmount

            Else

                Dim subtractToolRadius As Boolean =
                    (partArc.IsClockwise AndAlso offsetDirection > 0) OrElse
                    ((Not partArc.IsClockwise) AndAlso offsetDirection < 0)

                If subtractToolRadius Then

                    radius =
                        partArc.Radius -
                        toolArc.Radius -
                        stockAmount

                Else

                    radius =
                        partArc.Radius +
                        toolArc.Radius +
                        stockAmount

                End If

            End If

            If Math.Abs(radius) < 0.0000001 Then
                radius = 0.0
            End If

            Return radius

        End Function

        Public Sub GetConvolutionToolTangents(
            partArc As ConvexHullArc,
            toolArc As ConvexHullArc,
            ByRef startTangent As Double,
            ByRef endTangent As Double)

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If toolArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolArc))
            End If

            If SameDirection(partArc, toolArc) Then

                startTangent = toolArc.StartTangent
                endTangent = toolArc.EndTangent

            Else

                startTangent = toolArc.EndTangent
                endTangent = toolArc.StartTangent

            End If

        End Sub

        Public Sub ApplyConvolutionClassification(
    classification As ArcOverlapClassification,
    partArc As ConvexHullArc,
    offsetDirection As Integer,
    toolStartTangent As Double,
    toolEndTangent As Double,
    ByRef startTangent As Double,
    ByRef endTangent As Double)

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            startTangent = toolStartTangent
            endTangent = toolEndTangent

            If (classification And
                ArcOverlapClassification.PartStartInToolRange) <> 0 Then

                startTangent = partArc.StartTangent

            ElseIf (classification And
                    ArcOverlapClassification.ToolStartInPartRange) <> 0 Then

                If offsetDirection < 0 Then
                    startTangent =
                        toolStartTangent + Math.PI
                End If

            End If

            If (classification And
                ArcOverlapClassification.PartEndInToolRange) <> 0 Then

                endTangent = partArc.EndTangent

            ElseIf (classification And
                    ArcOverlapClassification.ToolEndInPartRange) <> 0 Then

                If offsetDirection < 0 Then
                    endTangent =
                        toolEndTangent + Math.PI
                End If

            End If

        End Sub


        Public Function CreateConvolutionArc(
    center As PointF,
    startTangent As Double,
    endTangent As Double,
    radius As Double,
    partArc As ConvexHullArc) As ConvexHullArc

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If partArc.ArcType = ConvexHullArcType.Move Then
                Throw New ArgumentException(
                    "A convolution arc cannot be created directly from a MOVE part arc.",
                    NameOf(partArc))
            End If

            Return New ConvexHullArc(
                center,
                startTangent,
                endTangent,
                radius,
                partArc.IsClockwise)

        End Function

        Public Function CreateConvolutionMove(
    center As PointF,
    tangent As Double) As ConvexHullArc

            Return New ConvexHullArc(center, tangent)

        End Function


        Public Function TangentsNearlySame(
    startTangent As Double,
    endTangent As Double) As Boolean

            Dim startVector As New PointF(
                CSng(Math.Cos(startTangent)),
                CSng(Math.Sin(startTangent)))

            Dim endVector As New PointF(
                CSng(Math.Cos(endTangent)),
                CSng(Math.Sin(endTangent)))

            Dim dot As Double =
                DotProduct(startVector, endVector)

            Return dot > (1.0 - VectorSmall)

        End Function

        Public Function PointsWithinTolerance(
            firstPoint As PointF,
            secondPoint As PointF,
            tolerance As Double) As Boolean

            Dim dx As Double =
                firstPoint.X - secondPoint.X

            Dim dy As Double =
                firstPoint.Y - secondPoint.Y

            Return (dx * dx) + (dy * dy) <=
                   (tolerance * tolerance)

        End Function

        Public Function ConvolutionOfOverlap(
    classification As ArcOverlapClassification,
    offsetDirection As Integer,
    partArc As ConvexHullArc,
    toolArc As ConvexHullArc) As ConvexHullArc

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If toolArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolArc))
            End If

            Dim center As PointF =
                CenterVector(partArc, toolArc)

            Dim radius As Double =
                ConvolutionRadius(
                    partArc,
                    toolArc,
                    offsetDirection)

            Dim toolStartTangent As Double
            Dim toolEndTangent As Double

            GetConvolutionToolTangents(
                partArc,
                toolArc,
                toolStartTangent,
                toolEndTangent)

            Dim startTangent As Double
            Dim endTangent As Double

            ApplyConvolutionClassification(
                classification,
                partArc,
                offsetDirection,
                toolStartTangent,
                toolEndTangent,
                startTangent,
                endTangent)

            If partArc.ArcType = ConvexHullArcType.Move Then

                Dim moveAngle As Double =
        startTangent +
        (SignDirection(offsetDirection) *
         (Math.PI / 2.0))

                Dim moveVector As New PointF(
        CSng(Math.Cos(moveAngle)),
        CSng(Math.Sin(moveAngle)))

                Dim moveCenter As New PointF(
        CSng(center.X + (moveVector.X * Math.Abs(radius))),
        CSng(center.Y + (moveVector.Y * Math.Abs(radius))))

                Return CreateConvolutionMove(
        moveCenter,
        startTangent)

            End If

            Dim result As ConvexHullArc =
        CreateConvolutionArc(
            center,
            startTangent,
            endTangent,
            radius,
            partArc)

            If TangentsNearlySame(startTangent, endTangent) Then

                If PointsWithinTolerance(result.StartPoint, result.EndPoint, SmallTolerance) Then

                    Dim moveCenter As PointF = CalculateDegenerateMoveCenter(center, startTangent, radius, partArc)

                    Dim moveTangent As Double = startTangent

                    If offsetDirection < 0 Then
                        moveTangent += Math.PI
                    End If

                    Return CreateConvolutionMove(moveCenter, moveTangent)

                End If

            End If

            Return result

        End Function

        Public Function CalculateDegenerateMoveCenter(center As PointF, tangent As Double, radius As Double, partArc As ConvexHullArc) As PointF

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If partArc.ArcType = ConvexHullArcType.Move Then
                Throw New ArgumentException(
                    "Part arc must be a Turn.",
                    NameOf(partArc))
            End If

            Dim partDirection As Integer =
                If(partArc.IsClockwise, -1, 1)

            Dim moveAngle As Double =
                tangent -
                (partDirection * (Math.PI / 2.0))

            Dim moveVector As New PointF(CSng(Math.Cos(moveAngle)), CSng(Math.Sin(moveAngle)))

            Return New PointF(CSng(center.X + (moveVector.X * Math.Abs(radius))), CSng(center.Y + (moveVector.Y * Math.Abs(radius))))

        End Function

        Public Function IsOppositeDirection(
    fromPoint As PointF,
    toPoint As PointF,
    referenceTangent As Double) As Boolean

            Dim dx As Double =
                fromPoint.X - toPoint.X

            Dim dy As Double =
                fromPoint.Y - toPoint.Y

            Dim length As Double =
                Math.Sqrt((dx * dx) + (dy * dy))

            If length < SmallTolerance Then
                Return False
            End If

            Dim ux As Double = dx / length
            Dim uy As Double = dy / length

            Dim tangentX As Double =
                Math.Cos(referenceTangent)

            Dim tangentY As Double =
                Math.Sin(referenceTangent)

            Dim dot As Double =
                (ux * tangentX) +
                (uy * tangentY)

            Return dot < (-1.0 + VectorSmall)

        End Function

        Public Sub ToolPositionAdd(
    classification As ArcOverlapClassification,
    offsetDirection As Integer,
    partArc As ConvexHullArc,
    toolArc As ConvexHullArc,
    result As IList(Of ConvexHullArc))

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            If toolArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolArc))
            End If

            If result Is Nothing Then
                Throw New ArgumentNullException(NameOf(result))
            End If

            Dim convol As ConvexHullArc =
                ConvolutionOfOverlap(
                    classification,
                    offsetDirection,
                    partArc,
                    toolArc)

            If convol Is Nothing Then
                Return
            End If

            If convol.ArcType = ConvexHullArcType.Move Then

                If result.Count > 0 Then

                    Dim previous As ConvexHullArc =
                        result(result.Count - 1)

                    If PointsWithinTolerance(
                        convol.StartPoint,
                        previous.EndPoint,
                        SmallTolerance) Then

                        Return
                    End If

                    If IsOppositeDirection(
                        convol.StartPoint,
                        previous.EndPoint,
                        previous.EndTangent) Then

                        Return
                    End If

                End If
            End If

            result.Add(convol)

        End Sub

        Public Function OffsetConvexHull(
            partArcs As IReadOnlyList(Of ConvexHullArc),
            toolArcs As IReadOnlyList(Of ConvexHullArc),
            offsetDirection As Integer) As IReadOnlyList(Of ConvexHullArc)

            If partArcs Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArcs))
            End If

            If toolArcs Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolArcs))
            End If

            If partArcs.Count = 0 Then
                Throw New ArgumentException(
                    "Part arc list cannot be empty.",
                    NameOf(partArcs))
            End If

            If toolArcs.Count = 0 Then
                Throw New ArgumentException(
                    "Tool arc list cannot be empty.",
                    NameOf(toolArcs))
            End If

            Dim partIterator As New ConvexHullArcIterator(partArcs)
            Dim toolIterator As New ConvexHullArcIterator(toolArcs)

            Dim result As New List(Of ConvexHullArc)

            Dim moveOnTool As Boolean = False
            Dim moveOnPart As Boolean = False
            Dim classification As ArcOverlapClassification

            While True

                If Not moveOnTool AndAlso Not moveOnPart Then

                    classification =
                        toolIterator.StartPosition(
                            partIterator,
                            offsetDirection)

                    If classification = ArcOverlapClassification.None Then
                        Exit While
                    End If

                    ToolPositionAdd(
                        classification,
                        offsetDirection,
                        partIterator.Current,
                        toolIterator.Current,
                        result)

                    moveOnTool = True

                End If

                If moveOnTool Then

                    If partIterator.AtEnd Then
                        Exit While
                    End If

                    classification =
                        toolIterator.Advance(
                            partIterator.Current,
                            offsetDirection)

                    If classification = ArcOverlapClassification.None Then

                        If partIterator.AtEnd Then
                            Exit While
                        End If

                        moveOnTool = False
                        moveOnPart = True

                    Else

                        ToolPositionAdd(
                            classification,
                            offsetDirection,
                            partIterator.Current,
                            toolIterator.Current,
                            result)

                    End If

                End If

                If moveOnPart Then

                    partIterator.MoveNext()

                    moveOnTool = False
                    moveOnPart = False

                End If

            End While

            Return result

        End Function

        Public Function ConvertRawOffsetChain(
    rawOffset As RawOffsetChain) As Path2D

            If rawOffset Is Nothing Then
                Throw New ArgumentNullException(NameOf(rawOffset))
            End If

            Dim result As New Path2D()

            If rawOffset.Count = 0 Then
                Return result
            End If

            For i As Integer = 0 To rawOffset.Count - 1

                Dim current As ConvexHullArc =
                    rawOffset.Arcs(i)

                If current.ArcType = ConvexHullArcType.Turn Then

                    Dim arc As New ArcSegment2D(
                        current.Center,
                        current.Radius,
                        current.StartTangent,
                        current.EndTangent,
                        current.IsClockwise)

                    result.Add(arc)

                Else
                    ' MOVE geometry is handled as a deferred
                    ' linear transition to the next arc.
                    If i < rawOffset.Count - 1 Then

                        Dim nextArc As ConvexHullArc =
                            rawOffset.Arcs(i + 1)

                        If Not PointsWithinTolerance(
                            current.EndPoint,
                            nextArc.StartPoint,
                            SmallTolerance) Then

                            result.Add(
                                New LineSegment2D(
                                    current.EndPoint,
                                    nextArc.StartPoint))

                        End If
                    End If
                End If

            Next

            Return result

        End Function

        Public Sub CleanupOffsetPath(path As Path2D)

            If path Is Nothing Then
                Throw New ArgumentNullException(NameOf(path))
            End If

            Dim segments As List(Of IPathSegment) =
                path.Segments.ToList()

            If segments.Count <= 2 Then
                Return
            End If

            ' Get the first and last elements.
            Dim lineA As LineSegment2D =
                TryCast(segments(0), LineSegment2D)

            Dim lineB As LineSegment2D =
                TryCast(segments(segments.Count - 1), LineSegment2D)

            ' The first and last elements must be lines.
            If lineA Is Nothing OrElse lineB Is Nothing Then
                Return
            End If

            ' ------------------------------------------------------------
            ' The lines must be colinear.
            '
            ' Equivalent to:
            '     C2dUnitVec tanA = lineA->StartTan();
            '     C2dUnitVec tanB = lineB->StartTan();
            '     dot = tanA * tanB;
            ' ------------------------------------------------------------

            Dim ax As Double =
                CDbl(lineA.EndPoint.X) -
                CDbl(lineA.StartPoint.X)

            Dim ay As Double =
                CDbl(lineA.EndPoint.Y) -
                CDbl(lineA.StartPoint.Y)

            Dim bx As Double =
                CDbl(lineB.EndPoint.X) -
                CDbl(lineB.StartPoint.X)

            Dim by As Double =
                CDbl(lineB.EndPoint.Y) -
                CDbl(lineB.StartPoint.Y)

            Dim lengthA As Double =
                Math.Sqrt(ax * ax + ay * ay)

            Dim lengthB As Double =
                Math.Sqrt(bx * bx + by * by)

            If lengthA <= VectorSmall OrElse
               lengthB <= VectorSmall Then

                Return
            End If

            ax /= lengthA
            ay /= lengthA

            bx /= lengthB
            by /= lengthB

            Dim dot As Double =
                ax * bx + ay * by

            If dot < (1.0 - VectorSmall) Then
                Return
            End If

            ' ------------------------------------------------------------
            ' The lines must be coincident.
            '
            ' Project both endpoints of lineB onto lineA.
            ' This reproduces PointClosest(..., &uA/uB).
            ' ------------------------------------------------------------

            Dim lineDx As Double =
                CDbl(lineA.EndPoint.X) -
                CDbl(lineA.StartPoint.X)

            Dim lineDy As Double =
                CDbl(lineA.EndPoint.Y) -
                CDbl(lineA.StartPoint.Y)

            Dim lineLengthSquared As Double =
                lineDx * lineDx +
                lineDy * lineDy

            If lineLengthSquared <= VectorSmall Then
                Return
            End If

            Dim startBx As Double =
                CDbl(lineB.StartPoint.X) -
                CDbl(lineA.StartPoint.X)

            Dim startBy As Double =
                CDbl(lineB.StartPoint.Y) -
                CDbl(lineA.StartPoint.Y)

            Dim endBx As Double =
                CDbl(lineB.EndPoint.X) -
                CDbl(lineA.StartPoint.X)

            Dim endBy As Double =
                CDbl(lineB.EndPoint.Y) -
                CDbl(lineA.StartPoint.Y)

            Dim uA As Double =
                (startBx * lineDx +
                 startBy * lineDy) /
                lineLengthSquared

            Dim uB As Double =
                (endBx * lineDx +
                 endBy * lineDy) /
                lineLengthSquared

            If Not (
                (uA > -VectorSmall AndAlso
                 uA < (1.0 + VectorSmall)) OrElse
                (uB > -VectorSmall AndAlso
                 uB < (1.0 + VectorSmall))) Then

                Return
            End If

            ' ------------------------------------------------------------
            ' Determine whether the end of lineB lies forward along lineA.
            '
            ' Equivalent to:
            '     vec = lineB.EndPt() - lineA.StartPt();
            '     dot = vec * tanA;
            ' ------------------------------------------------------------

            Dim vecX As Double =
                CDbl(lineB.EndPoint.X) -
                CDbl(lineA.StartPoint.X)

            Dim vecY As Double =
                CDbl(lineB.EndPoint.Y) -
                CDbl(lineA.StartPoint.Y)

            dot =
                vecX * ax +
                vecY * ay

            If dot > (1.0 - VectorSmall) Then

                ' Remove the last element in the offset.
                path.RemoveLast()

            End If

        End Sub

        Public Sub ReverseOffsetPath(path As Path2D)

            If path Is Nothing Then
                Throw New ArgumentNullException(NameOf(path))
            End If

            Dim original As List(Of IPathSegment) =
                path.Segments.ToList()

            If original.Count <= 1 Then
                Return
            End If

            Dim reversed As New List(Of IPathSegment)

            For index As Integer = original.Count - 1 To 0 Step -1

                Dim segment As IPathSegment =
                    original(index)

                Dim line As LineSegment2D =
                    TryCast(segment, LineSegment2D)

                If line IsNot Nothing Then

                    reversed.Add(
                        New LineSegment2D(
                            line.EndPoint,
                            line.StartPoint))

                    Continue For
                End If

                Dim arc As ArcSegment2D =
                    TryCast(segment, ArcSegment2D)

                If arc IsNot Nothing Then

                    reversed.Add(
                        New ArcSegment2D(
                            arc.Center,
                            arc.Radius,
                            arc.EndAngle,
                            arc.StartAngle,
                            Not arc.IsClockwise))

                    Continue For
                End If

                Throw New InvalidOperationException(
                    "Unsupported path segment type during offset reversal.")

            Next

            path.Clear()

            For Each segment As IPathSegment In reversed
                path.Add(segment)
            Next

        End Sub

        Private Sub ArcAngles(
    arc As ArcSegment2D,
    ByRef startAngle As Double,
    ByRef endAngle As Double)

            Const AngleTolerance As Double = 0.0002

            Dim asAngle As Double = arc.StartAngle
            Dim aeAngle As Double = arc.EndAngle

            Dim twoPi As Double = 2.0 * Math.PI

            ' CGeoArc::Angles()
            '
            ' Legacy convention:
            '   dir < 0 = clockwise
            '   dir > 0 = counter-clockwise
            '
            ' ArcSegment2D.IsClockwise is the equivalent here.

            If arc.IsClockwise Then

                If aeAngle > asAngle Then
                    asAngle += twoPi
                End If

            Else

                If aeAngle < asAngle Then
                    aeAngle += twoPi
                End If

            End If

            ' Legacy CGeoArc special handling for nearly equal
            ' angular endpoints.
            If Math.Abs(aeAngle - asAngle) < AngleTolerance Then

                Dim distance As Double =
                    2.0 * arc.Radius *
                    Math.Sin(0.5 * Math.Abs(aeAngle - asAngle))

                If distance < SmallTolerance Then

                    If arc.IsClockwise Then
                        asAngle += twoPi
                    Else
                        aeAngle += twoPi
                    End If

                End If

            End If

            startAngle = asAngle
            endAngle = aeAngle

        End Sub

        Private Function ArcAdjust(
    startAngle As Double,
    endAngle As Double,
    pointAngle As Double) As Double

            Dim twoPi As Double = 2.0 * Math.PI
            Dim adjustedAngle As Double = pointAngle

            ' CGeoArc::Adjust()
            '
            ' This deliberately mirrors the legacy angle handling.
            ' Do not normalize the angles globally.

            If startAngle >= (twoPi - SmallTolerance) AndAlso
               adjustedAngle < startAngle AndAlso
               adjustedAngle < endAngle Then

                adjustedAngle += twoPi

            ElseIf endAngle >= (twoPi - SmallTolerance) AndAlso
                   adjustedAngle < endAngle AndAlso
                   adjustedAngle < startAngle Then

                adjustedAngle += twoPi

            End If

            Dim delta As Double

            If endAngle < startAngle Then
                ' Clockwise
                delta = startAngle - adjustedAngle
            Else
                ' Counter-clockwise
                delta = adjustedAngle - startAngle
            End If

            Dim total As Double

            If endAngle < startAngle Then
                ' Clockwise
                total = startAngle - endAngle
            Else
                ' Counter-clockwise
                total = endAngle - startAngle
            End If

            If total < SmallTolerance Then
                Return Double.NaN
            End If

            Return delta / total

        End Function

        Private Function ArcUParam(
    arc As ArcSegment2D,
    point As PointF) As Double

            Dim startAngle As Double
            Dim endAngle As Double

            ArcAngles(
                arc,
                startAngle,
                endAngle)

            ' Calculate the polar angle of the point relative
            ' to the arc center, matching CGeoArc::Angle().
            Dim dx As Double =
                CDbl(point.X) - arc.Center.X

            Dim dy As Double =
                CDbl(point.Y) - arc.Center.Y

            Dim pointAngle As Double =
                Math.Atan2(dy, dx)

            If pointAngle < 0.0 Then
                pointAngle += 2.0 * Math.PI
            End If

            Dim u As Double =
                ArcAdjust(
                    startAngle,
                    endAngle,
                    pointAngle)

            ' CGeoArc::PointClosest() converts U ~= 1 back
            ' to U = 0 for a full circle.
            Dim totalAngle As Double =
                Math.Abs(endAngle - startAngle)

            If Math.Abs(
                (2.0 * Math.PI) - totalAngle) < SmallTolerance AndAlso
               Math.Abs(1.0 - u) < SmallTolerance Then

                u = 0.0

            End If

            Return u

        End Function

        Private Function LineUParam(
    line As LineSegment2D,
    point As PointF) As Double

            Dim dx As Double =
                CDbl(line.EndPoint.X) - line.StartPoint.X

            Dim dy As Double =
                CDbl(line.EndPoint.Y) - line.StartPoint.Y

            Dim magnitudeSquared As Double =
                (dx * dx) + (dy * dy)

            ' Legacy CGeoLine::PointClosest()
            If magnitudeSquared <=
               (SmallTolerance * SmallTolerance) Then

                Return Double.NaN

            End If

            Dim px As Double =
                CDbl(point.X) - line.StartPoint.X

            Dim py As Double =
                CDbl(point.Y) - line.StartPoint.Y

            Dim dotProduct As Double =
                (dx * px) + (dy * py)

            Return dotProduct / magnitudeSquared

        End Function

        Private Function LinePointClosest(
    line As LineSegment2D,
    point As PointF,
    ByRef closestPoint As PointF,
    ByRef uParam As Double) As Double

            Dim dx As Double =
        CDbl(line.EndPoint.X) - line.StartPoint.X

            Dim dy As Double =
        CDbl(line.EndPoint.Y) - line.StartPoint.Y

            Dim magnitudeSquared As Double =
        (dx * dx) + (dy * dy)

            If magnitudeSquared <=
       (SmallTolerance * SmallTolerance) Then

                closestPoint = line.StartPoint
                uParam = Double.NaN

                Return Double.PositiveInfinity

            End If

            Dim px As Double =
        CDbl(point.X) - line.StartPoint.X

            Dim py As Double =
        CDbl(point.Y) - line.StartPoint.Y

            Dim dotProduct As Double =
        (dx * px) + (dy * py)

            uParam = dotProduct / magnitudeSquared

            Dim closestX As Double =
        CDbl(line.StartPoint.X) + (dx * uParam)

            Dim closestY As Double =
        CDbl(line.StartPoint.Y) + (dy * uParam)

            closestPoint = New PointF(
        CSng(closestX),
        CSng(closestY))

            Dim distanceX As Double =
        closestX - point.X

            Dim distanceY As Double =
        closestY - point.Y

            Return Math.Sqrt(
        (distanceX * distanceX) +
        (distanceY * distanceY))

        End Function

        Private Function IntersectArcLine(
    arc As ArcSegment2D,
    line As LineSegment2D,
    tolerance As Double,
    ByRef points As List(Of PointF),
    ByRef uArc As List(Of Double),
    ByRef uLine As List(Of Double)) As Boolean

            points = New List(Of PointF)()
            uArc = New List(Of Double)()
            uLine = New List(Of Double)()

            If arc.Radius < SmallTolerance Then
                Return False
            End If

            Dim closestPoint As PointF
            Dim lineU As Double

            Dim distance As Double =
        LinePointClosest(
            line,
            arc.Center,
            closestPoint,
            lineU)

            If Double.IsInfinity(distance) Then
                Return False
            End If

            If distance > (arc.Radius + tolerance) Then
                Return False
            End If

            Dim tangent As Boolean = False

            If Math.Abs(arc.Radius - distance) < SmallTolerance Then
                distance = arc.Radius
                tangent = True
            End If

            Dim lengthSquared As Double =
        (arc.Radius * arc.Radius) -
        (distance * distance)

            Dim length As Double

            If lengthSquared < (tolerance * tolerance) Then
                length = 0.0
            Else
                length = Math.Sqrt(lengthSquared)
            End If

            ' Unit tangent along the line.
            Dim dx As Double =
        CDbl(line.EndPoint.X) - line.StartPoint.X

            Dim dy As Double =
        CDbl(line.EndPoint.Y) - line.StartPoint.Y

            Dim magnitude As Double =
        Math.Sqrt((dx * dx) + (dy * dy))

            If magnitude <= SmallTolerance Then
                Return False
            End If

            Dim unitX As Double = dx / magnitude
            Dim unitY As Double = dy / magnitude

            Dim candidatePoints As New List(Of PointF)

            candidatePoints.Add(
    New PointF(
        CSng(closestPoint.X + (unitX * length)),
        CSng(closestPoint.Y + (unitY * length))))

            candidatePoints.Add(
    New PointF(
        CSng(closestPoint.X - (unitX * length)),
        CSng(closestPoint.Y - (unitY * length))))



            For index As Integer = 0 To 1

                Dim candidate As PointF =
            candidatePoints(index)

                Dim arcParam As Double =
            ArcUParam(
                arc,
                candidate)

                Dim lineParam As Double =
            LineUParam(
                line,
                candidate)

                If Double.IsNaN(arcParam) OrElse
           Double.IsNaN(lineParam) Then

                    Continue For

                End If

                Dim okayArc As Boolean =
            ResultCheck(
                arc.StartPoint,
                arc.EndPoint,
                candidate,
                arcParam)

                Dim okayLine As Boolean =
            ResultCheck(
                line.StartPoint,
                line.EndPoint,
                candidate,
                lineParam)

                If okayArc AndAlso okayLine Then

                    Dim duplicate As Boolean = False

                    For Each existing As PointF In points

                        If PointsWithinTolerance(
                    candidate,
                    existing,
                    tolerance) Then

                            duplicate = True
                            Exit For

                        End If

                    Next

                    If Not duplicate Then

                        points.Add(candidate)
                        uArc.Add(arcParam)
                        uLine.Add(lineParam)

                    End If

                End If

            Next

            Return points.Count > 0

        End Function

        Friend Function TestIntersectArcLine(
    arc As ArcSegment2D,
    line As LineSegment2D,
    tolerance As Double,
    ByRef points As List(Of PointF),
    ByRef uArc As List(Of Double),
    ByRef uLine As List(Of Double)) As Boolean

            Return IntersectArcLine(
        arc,
        line,
        tolerance,
        points,
        uArc,
        uLine)

        End Function
        Private Function ProcessLineArcIntersection(
    line As LineSegment2D,
    segmentIndexLine As Integer,
    arc As ArcSegment2D,
    segmentIndexArc As Integer,
    tolerance As Double,
    intersections As List(Of WmIntersectionRecord),
    curveRecords As List(Of WmCurveRecord),
    segments As IReadOnlyList(Of IPathSegment)) As Boolean

            Dim points As List(Of PointF)
            Dim uArc As List(Of Double)
            Dim uLine As List(Of Double)

            If Not IntersectArcLine(
        arc,
        line,
        tolerance,
        points,
        uArc,
        uLine) Then

                Return False

            End If

            For index As Integer = 0 To points.Count - 1

                Dim intersection As WmIntersectionRecord =
            FindOrCreateIntersection(
                intersections,
                points(index),
                tolerance)

                Dim curveRecordLine As WmCurveRecord =
            FindOrCreateCurveRecord(
                curveRecords,
                segmentIndexLine,
                segments)

                Dim curveRecordArc As WmCurveRecord =
            FindOrCreateCurveRecord(
                curveRecords,
                segmentIndexArc,
                segments)

                RegisterIntersection(
            intersection,
            curveRecordLine,
            curveRecordArc,
            uLine(index),
            uArc(index))

            Next

            Return True

        End Function


        Private Function ProcessArcArcIntersection(
    arcA As ArcSegment2D,
    segmentIndexA As Integer,
    arcB As ArcSegment2D,
    segmentIndexB As Integer,
    tolerance As Double,
    intersections As List(Of WmIntersectionRecord),
    curveRecords As List(Of WmCurveRecord),
    segments As IReadOnlyList(Of IPathSegment)) As Boolean

            Dim points As List(Of PointF)
            Dim uA As List(Of Double)
            Dim uB As List(Of Double)

            If Not IntersectArcArc(
        arcA,
        arcB,
        tolerance,
        points,
        uA,
        uB) Then

                Return False

            End If

            For index As Integer = 0 To points.Count - 1

                Dim intersection As WmIntersectionRecord =
            FindOrCreateIntersection(
                intersections,
                points(index),
                tolerance)

                Dim curveRecordA As WmCurveRecord =
            FindOrCreateCurveRecord(
                curveRecords,
                segmentIndexA,
                segments)

                Dim curveRecordB As WmCurveRecord =
            FindOrCreateCurveRecord(
                curveRecords,
                segmentIndexB,
                segments)

                RegisterIntersection(
            intersection,
            curveRecordA,
            curveRecordB,
            uA(index),
            uB(index))

            Next

            Return True

        End Function

        Private Function ResultCheck(
    startPoint As PointF,
    endPoint As PointF,
    point As PointF,
    ByRef uParam As Double) As Boolean

            ' Legacy VECTOR_SMALL check.
            '
            ' A result slightly outside [0,1] is normally rejected.
            ' However, if the calculated intersection is actually
            ' within SMALL of an endpoint, it is accepted and the
            ' U parameter is clamped to that endpoint.

            If uParam > -VectorSmall AndAlso
               uParam < (1.0 + VectorSmall) Then

                Return True

            End If

            If uParam <= -VectorSmall Then

                If PointsWithinTolerance(
                    point,
                    startPoint,
                    SmallTolerance) Then

                    uParam = 0.0
                    Return True

                End If

            ElseIf uParam >= (1.0 + VectorSmall) Then

                If PointsWithinTolerance(
                    point,
                    endPoint,
                    SmallTolerance) Then

                    uParam = 1.0
                    Return True

                End If

            End If

            Return False

        End Function

        Private Function IntersectLineLine(
    lineA As LineSegment2D,
    lineB As LineSegment2D,
    ByRef intersection As PointF,
    ByRef uA As Double,
    ByRef uB As Double) As Boolean

            Dim psA As PointF = lineA.StartPoint
            Dim peA As PointF = lineA.EndPoint

            Dim psB As PointF = lineB.StartPoint
            Dim peB As PointF = lineB.EndPoint

            ' Equivalent to:
            '
            ' C2dVec vecA = peA - psA;
            ' C2dVec vecB = peB - psB;

            Dim vecAX As Double =
                CDbl(peA.X) - psA.X

            Dim vecAY As Double =
                CDbl(peA.Y) - psA.Y

            Dim vecBX As Double =
                CDbl(peB.X) - psB.X

            Dim vecBY As Double =
                CDbl(peB.Y) - psB.Y

            ' Legacy:
            ' det = vecA.X() * vecB.Y() -
            '       vecA.Y() * vecB.X()
            '
            ' Parallel threshold is VECTOR_SMALL.
            Dim det As Double =
                (vecAX * vecBY) -
                (vecAY * vecBX)

            If Math.Abs(det) < VectorSmall Then
                Return False
            End If

            Dim numerator As Double =
                vecBX * (CDbl(psA.Y) - psB.Y) -
                vecBY * (CDbl(psA.X) - psB.X)

            Dim numeratorB As Double =
                vecAX * (CDbl(psB.Y) - psA.Y) -
                vecAY * (CDbl(psB.X) - psA.X)

            Dim paramA As Double =
                numerator / det

            Dim paramB As Double =
                -numeratorB / det

            ' Legacy onSegment behavior.
            If Not (
                paramA > -VectorSmall AndAlso
                paramA < (1.0 + VectorSmall) AndAlso
                paramB > -VectorSmall AndAlso
                paramB < (1.0 + VectorSmall)
            ) Then

                Return False

            End If

            Dim ix As Double =
                CDbl(psA.X) + (vecAX * paramA)

            Dim iy As Double =
                CDbl(psA.Y) + (vecAY * paramA)

            intersection = New PointF(
                CSng(ix),
                CSng(iy))

            uA = paramA
            uB = paramB

            Return True

        End Function

        Private Function FindOrCreateIntersection(
    intersections As List(Of WmIntersectionRecord),
    point As PointF,
    tolerance As Double) As WmIntersectionRecord

            ' Legacy CWmChainIntsctor::FindCreate()
            '
            ' Important:
            ' The legacy code searches from the beginning and uses
            ' the FIRST record within tolerance. It does not search
            ' for the nearest record.

            For Each record As WmIntersectionRecord In intersections

                If PointsWithinTolerance(
                    point,
                    record.Point,
                    tolerance) Then

                    Return record

                End If

            Next

            Dim newRecord As New WmIntersectionRecord(point)

            intersections.Add(newRecord)

            Return newRecord

        End Function

        Private Function FindOrCreateCurveRecord(
    curveRecords As List(Of WmCurveRecord),
    segmentIndex As Integer,
    segments As IReadOnlyList(Of IPathSegment)) As WmCurveRecord

            ' One curve record per segment.
            For Each record As WmCurveRecord In curveRecords

                If record.SegmentIndex = segmentIndex Then
                    Return record
                End If

            Next

            Dim newRecord As New WmCurveRecord(
        segmentIndex,
        segments(segmentIndex))

            curveRecords.Add(newRecord)

            Return newRecord

        End Function

        Private Sub RegisterIntersection(
    intersection As WmIntersectionRecord,
    curveRecordA As WmCurveRecord,
    curveRecordB As WmCurveRecord,
    uA As Double,
    uB As Double)

            Dim extA As WmCurveRecordExtData =
                curveRecordA.Update(
                    intersection,
                    uA)

            Dim extB As WmCurveRecordExtData =
                curveRecordB.Update(
                    intersection,
                    uB)

            intersection.Update(extA)
            intersection.Update(extB)

        End Sub

        Private Function ProcessLineLineIntersection(
    lineA As LineSegment2D,
    segmentIndexA As Integer,
    lineB As LineSegment2D,
    segmentIndexB As Integer,
    tolerance As Double,
    intersections As List(Of WmIntersectionRecord),
    curveRecords As List(Of WmCurveRecord),
    segments As IReadOnlyList(Of IPathSegment)) As Boolean

            Dim intersectionPoint As PointF
            Dim uA As Double
            Dim uB As Double

            ' Perform the geometric intersection.
            If Not IntersectLineLine(
                lineA,
                lineB,
                intersectionPoint,
                uA,
                uB) Then

                Return False

            End If

            ' Legacy FindCreate(point, tol).
            Dim intersection As WmIntersectionRecord =
                FindOrCreateIntersection(
                    intersections,
                    intersectionPoint,
                    tolerance)

            Dim curveRecordA As WmCurveRecord =
    FindOrCreateCurveRecord(
        curveRecords,
        segmentIndexA,
        segments)

            Dim curveRecordB As WmCurveRecord =
    FindOrCreateCurveRecord(
        curveRecords,
        segmentIndexB,
        segments)

            ' Register the intersection on both curves and
            ' on the intersection record.
            RegisterIntersection(
                intersection,
                curveRecordA,
                curveRecordB,
                uA,
                uB)

            Return True

        End Function

        Private Function OkayToIntersect(
    segmentIndexA As Integer,
    segmentIndexB As Integer,
    segments As IReadOnlyList(Of IPathSegment)) As Boolean

            ' A segment never intersects itself.
            If segmentIndexA = segmentIndexB Then
                Return False
            End If

            Dim count As Integer = segments.Count

            ' The legacy chain is ordered, so adjacent elements are
            ' identified by Next() / Prev().
            '
            ' We reproduce that behavior using segment indexes.

            If segmentIndexB = segmentIndexA + 1 Then

                Dim pointA As PointF =
                    segments(segmentIndexA).EndPoint

                Dim pointB As PointF =
                    segments(segmentIndexB).StartPoint

                ' Normal shared endpoint:
                ' don't treat it as an intersection.
                '
                ' If the segments are actually disconnected by more
                ' than SMALL, however, they are allowed to intersect.
                Return Not PointsWithinTolerance(
                    pointA,
                    pointB,
                    SmallTolerance)

            End If

            If segmentIndexB = segmentIndexA - 1 Then

                Dim pointA As PointF =
                    segments(segmentIndexA).StartPoint

                Dim pointB As PointF =
                    segments(segmentIndexB).EndPoint

                Return Not PointsWithinTolerance(
                    pointA,
                    pointB,
                    SmallTolerance)

            End If

            ' Non-adjacent segments are always candidates.
            Return True

        End Function

        Private Function IntersectLineSegments(
    segments As IReadOnlyList(Of IPathSegment),
    tolerance As Double,
    intersections As List(Of WmIntersectionRecord),
    curveRecords As List(Of WmCurveRecord)) As Boolean

            Dim doesIntersect As Boolean = False

            For i As Integer = 0 To segments.Count - 1

                Dim lineA As LineSegment2D =
                    TryCast(segments(i), LineSegment2D)

                If lineA Is Nothing Then
                    Continue For
                End If

                For j As Integer = i + 1 To segments.Count - 1

                    ' Reproduce CWmChainIntsctor::OkayToIntersect().
                    If Not OkayToIntersect(
                        i,
                        j,
                        segments) Then

                        Continue For

                    End If

                    Dim lineB As LineSegment2D =
                        TryCast(segments(j), LineSegment2D)

                    If lineB Is Nothing Then
                        Continue For
                    End If

                    If ProcessLineLineIntersection(
                            lineA,
                            i,
                            lineB,
                            j,
                            tolerance,
                            intersections,
                            curveRecords,
                            segments) Then


                        doesIntersect = True

                    End If

                Next

            Next

            Return doesIntersect

        End Function

        Private Function IntersectLineArcSegments(
    segments As IReadOnlyList(Of IPathSegment),
    tolerance As Double,
    intersections As List(Of WmIntersectionRecord),
    curveRecords As List(Of WmCurveRecord)) As Boolean

            Dim doesIntersect As Boolean = False

            For i As Integer = 0 To segments.Count - 1

                For j As Integer = i + 1 To segments.Count - 1

                    If Not OkayToIntersect(
                i,
                j,
                segments) Then

                        Continue For

                    End If

                    Dim lineA As LineSegment2D =
                TryCast(segments(i), LineSegment2D)

                    Dim arcA As ArcSegment2D =
                TryCast(segments(i), ArcSegment2D)

                    Dim lineB As LineSegment2D =
                TryCast(segments(j), LineSegment2D)

                    Dim arcB As ArcSegment2D =
                TryCast(segments(j), ArcSegment2D)

                    ' Line / Arc
                    If lineA IsNot Nothing AndAlso
               arcB IsNot Nothing Then

                        If ProcessLineArcIntersection(
                    lineA,
                    i,
                    arcB,
                    j,
                    tolerance,
                    intersections,
                    curveRecords,
                    segments) Then

                            doesIntersect = True

                        End If

                        ' Arc / Line
                    ElseIf arcA IsNot Nothing AndAlso
                   lineB IsNot Nothing Then

                        If ProcessLineArcIntersection(
                    lineB,
                    j,
                    arcA,
                    i,
                    tolerance,
                    intersections,
                    curveRecords,
                    segments) Then

                            doesIntersect = True

                        End If

                    End If

                Next

            Next

            Return doesIntersect

        End Function

        Private Function IntersectArcArc(
    arcA As ArcSegment2D,
    arcB As ArcSegment2D,
    tolerance As Double,
    ByRef points As List(Of PointF),
    ByRef uA As List(Of Double),
    ByRef uB As List(Of Double)) As Boolean

            points = New List(Of PointF)()
            uA = New List(Of Double)()
            uB = New List(Of Double)()

            Dim radiusA As Double = arcA.Radius
            Dim radiusB As Double = arcB.Radius

            If radiusA < tolerance Then
                radiusA = 0.0
            End If

            If radiusB < tolerance Then
                radiusB = 0.0
            End If

            Dim dx As Double =
        CDbl(arcB.Center.X) - arcA.Center.X

            Dim dy As Double =
        CDbl(arcB.Center.Y) - arcA.Center.Y

            Dim centerDistance As Double =
        Math.Sqrt(
            (dx * dx) +
            (dy * dy))

            ' Same center.
            If centerDistance < tolerance Then
                Return False
            End If

            ' ------------------------------------------------------------
            ' External separation / external tangent
            ' ------------------------------------------------------------

            Dim delta As Double =
        centerDistance - (radiusA + radiusB)

            If delta > tolerance Then
                Return False
            End If

            If Math.Abs(delta) < tolerance Then

                Dim ux As Double = dx / centerDistance
                Dim uy As Double = dy / centerDistance

                Dim tangentPoint As New PointF(
            CSng(arcA.Center.X + (ux * radiusA)),
            CSng(arcA.Center.Y + (uy * radiusA)))

                Dim paramA As Double =
            ArcUParam(
                arcA,
                tangentPoint)

                Dim paramB As Double =
            ArcUParam(
                arcB,
                tangentPoint)

                If Double.IsNaN(paramA) OrElse
           Double.IsNaN(paramB) Then

                    Return False

                End If

                If Not ResultCheck(
            arcA.StartPoint,
            arcA.EndPoint,
            tangentPoint,
            paramA) Then

                    Return False

                End If

                If Not ResultCheck(
            arcB.StartPoint,
            arcB.EndPoint,
            tangentPoint,
            paramB) Then

                    Return False

                End If

                points.Add(tangentPoint)
                uA.Add(paramA)
                uB.Add(paramB)

                Return True

            End If

            ' ------------------------------------------------------------
            ' Internal separation / internal tangent
            ' ------------------------------------------------------------

            Dim radiusDifference As Double =
        Math.Abs(radiusA - radiusB)

            delta =
        radiusDifference - centerDistance

            If delta > tolerance Then
                Return False
            End If

            ' Legacy V13 internal tangent handling.
            If Math.Abs(delta) < (0.1 * tolerance) Then

                Dim ux As Double = dx / centerDistance
                Dim uy As Double = dy / centerDistance

                Dim tangentPoint As PointF

                If radiusB < radiusA Then

                    tangentPoint =
                New PointF(
                    CSng(arcA.Center.X + (ux * radiusA)),
                    CSng(arcA.Center.Y + (uy * radiusA)))

                Else

                    tangentPoint =
                New PointF(
                    CSng(arcA.Center.X - (ux * radiusA)),
                    CSng(arcA.Center.Y - (uy * radiusA)))

                End If

                Dim paramA As Double =
            ArcUParam(
                arcA,
                tangentPoint)

                Dim paramB As Double =
            ArcUParam(
                arcB,
                tangentPoint)

                If Double.IsNaN(paramA) OrElse
           Double.IsNaN(paramB) Then

                    Return False

                End If

                If Not ResultCheck(
            arcA.StartPoint,
            arcA.EndPoint,
            tangentPoint,
            paramA) Then

                    Return False

                End If

                If Not ResultCheck(
            arcB.StartPoint,
            arcB.EndPoint,
            tangentPoint,
            paramB) Then

                    Return False

                End If

                points.Add(tangentPoint)
                uA.Add(paramA)
                uB.Add(paramB)

                Return True

            End If

            ' ------------------------------------------------------------
            ' Two-circle intersection
            ' ------------------------------------------------------------

            Dim a As Double =
        ((radiusA * radiusA) -
         (radiusB * radiusB) +
         (centerDistance * centerDistance)) /
        (2.0 * centerDistance)

            Dim hSquared As Double =
        (radiusA * radiusA) -
        (a * a)

            If hSquared < 0.0 Then
                Return False
            End If

            Dim h As Double =
        Math.Sqrt(hSquared)

            Dim baseX As Double =
        CDbl(arcA.Center.X) +
        (a * dx / centerDistance)

            Dim baseY As Double =
        CDbl(arcA.Center.Y) +
        (a * dy / centerDistance)

            Dim offsetX As Double =
        -dy * h / centerDistance

            Dim offsetY As Double =
        dx * h / centerDistance

            Dim candidatePoints As New List(Of PointF)

            candidatePoints.Add(
        New PointF(
            CSng(baseX + offsetX),
            CSng(baseY + offsetY)))

            candidatePoints.Add(
        New PointF(
            CSng(baseX - offsetX),
            CSng(baseY - offsetY)))

            For Each candidate As PointF In candidatePoints

                Dim paramA As Double =
            ArcUParam(
                arcA,
                candidate)

                Dim paramB As Double =
            ArcUParam(
                arcB,
                candidate)

                If Double.IsNaN(paramA) OrElse
           Double.IsNaN(paramB) Then

                    Continue For

                End If

                If Not ResultCheck(
            arcA.StartPoint,
            arcA.EndPoint,
            candidate,
            paramA) Then

                    Continue For

                End If

                If Not ResultCheck(
            arcB.StartPoint,
            arcB.EndPoint,
            candidate,
            paramB) Then

                    Continue For

                End If

                Dim duplicate As Boolean = False

                For Each existing As PointF In points

                    If PointsWithinTolerance(
                candidate,
                existing,
                tolerance) Then

                        duplicate = True
                        Exit For

                    End If

                Next

                If Not duplicate Then

                    points.Add(candidate)
                    uA.Add(paramA)
                    uB.Add(paramB)

                End If

            Next

            Return points.Count > 0

        End Function


        Friend Function TestIntersectArcArc(
    arcA As ArcSegment2D,
    arcB As ArcSegment2D,
    tolerance As Double,
    ByRef points As List(Of PointF),
    ByRef uA As List(Of Double),
    ByRef uB As List(Of Double)) As Boolean

            Return IntersectArcArc(
            arcA,
            arcB,
            tolerance,
            points,
            uA,
            uB)

        End Function

        Friend Function BuildLineIntersectionRecords(
    segments As IReadOnlyList(Of IPathSegment),
    tolerance As Double,
    ByRef intersections As List(Of WmIntersectionRecord),
    ByRef curveRecords As List(Of WmCurveRecord)) As Boolean

            intersections =
        New List(Of WmIntersectionRecord)()

            curveRecords =
        New List(Of WmCurveRecord)()

            Dim doesIntersect As Boolean =
        IntersectLineSegments(
            segments,
            tolerance,
            intersections,
            curveRecords)

            FinalizeCurveRecords(
        curveRecords)

            Return doesIntersect

        End Function

        Friend Function BuildLineArcIntersectionRecords(
    segments As IReadOnlyList(Of IPathSegment),
    tolerance As Double,
    ByRef intersections As List(Of WmIntersectionRecord),
    ByRef curveRecords As List(Of WmCurveRecord)) As Boolean

            intersections =
        New List(Of WmIntersectionRecord)()

            curveRecords =
        New List(Of WmCurveRecord)()

            Dim doesIntersect As Boolean =
        IntersectLineArcSegments(
            segments,
            tolerance,
            intersections,
            curveRecords)

            FinalizeCurveRecords(
        curveRecords)

            Return doesIntersect

        End Function

        Friend Function BuildArcArcIntersectionRecords(
    segments As IReadOnlyList(Of IPathSegment),
    tolerance As Double,
    ByRef intersections As List(Of WmIntersectionRecord),
    ByRef curveRecords As List(Of WmCurveRecord)) As Boolean

            intersections =
        New List(Of WmIntersectionRecord)()

            curveRecords =
        New List(Of WmCurveRecord)()

            Dim doesIntersect As Boolean =
        False

            For i As Integer = 0 To segments.Count - 1

                Dim arcA As ArcSegment2D =
            TryCast(segments(i), ArcSegment2D)

                If arcA Is Nothing Then
                    Continue For
                End If

                For j As Integer = i + 1 To segments.Count - 1

                    If Not OkayToIntersect(
                i,
                j,
                segments) Then

                        Continue For

                    End If

                    Dim arcB As ArcSegment2D =
                TryCast(segments(j), ArcSegment2D)

                    If arcB Is Nothing Then
                        Continue For

                    End If

                    If ProcessArcArcIntersection(
                arcA,
                i,
                arcB,
                j,
                tolerance,
                intersections,
                curveRecords,
                segments) Then

                        doesIntersect = True

                    End If

                Next

            Next

            FinalizeCurveRecords(
        curveRecords)

            Return doesIntersect

        End Function

        Friend Function BuildIntersectionRecords(
    segments As IReadOnlyList(Of IPathSegment),
    tolerance As Double,
    ByRef intersections As List(Of WmIntersectionRecord),
    ByRef curveRecords As List(Of WmCurveRecord)) As Boolean

            intersections =
        New List(Of WmIntersectionRecord)()

            curveRecords =
        New List(Of WmCurveRecord)()

            Dim doesIntersect As Boolean = False

            For i As Integer = 0 To segments.Count - 1

                For j As Integer = i + 1 To segments.Count - 1

                    If Not OkayToIntersect(
                i,
                j,
                segments) Then

                        Continue For

                    End If

                    Dim lineA As LineSegment2D =
                TryCast(segments(i), LineSegment2D)

                    Dim arcA As ArcSegment2D =
                TryCast(segments(i), ArcSegment2D)

                    Dim lineB As LineSegment2D =
                TryCast(segments(j), LineSegment2D)

                    Dim arcB As ArcSegment2D =
                TryCast(segments(j), ArcSegment2D)

                    ' ----------------------------------------------------
                    ' Line / Line
                    ' ----------------------------------------------------

                    If lineA IsNot Nothing AndAlso
               lineB IsNot Nothing Then

                        If ProcessLineLineIntersection(
                    lineA,
                    i,
                    lineB,
                    j,
                    tolerance,
                    intersections,
                    curveRecords,
                    segments) Then

                            doesIntersect = True

                        End If

                        ' ----------------------------------------------------
                        ' Line / Arc
                        ' ----------------------------------------------------

                    ElseIf lineA IsNot Nothing AndAlso
                   arcB IsNot Nothing Then

                        If ProcessLineArcIntersection(
                    lineA,
                    i,
                    arcB,
                    j,
                    tolerance,
                    intersections,
                    curveRecords,
                    segments) Then

                            doesIntersect = True

                        End If

                        ' ----------------------------------------------------
                        ' Arc / Line
                        ' ----------------------------------------------------

                    ElseIf arcA IsNot Nothing AndAlso
                   lineB IsNot Nothing Then

                        If ProcessLineArcIntersection(
                    lineB,
                    j,
                    arcA,
                    i,
                    tolerance,
                    intersections,
                    curveRecords,
                    segments) Then

                            doesIntersect = True

                        End If

                        ' ----------------------------------------------------
                        ' Arc / Arc
                        ' ----------------------------------------------------

                    ElseIf arcA IsNot Nothing AndAlso
                   arcB IsNot Nothing Then

                        If ProcessArcArcIntersection(
                    arcA,
                    i,
                    arcB,
                    j,
                    tolerance,
                    intersections,
                    curveRecords,
                    segments) Then

                            doesIntersect = True

                        End If

                    End If

                Next

            Next

            FinalizeCurveRecords(
        curveRecords)

            Return doesIntersect

        End Function


        '==========================================================
        ' Build intersection records between two separate chains.
        '
        ' Unlike the single-chain version, segments from chain A
        ' and chain B are always considered eligible for testing.
        '
        ' The two curve-record collections remain separate because
        ' SegmentIndex is only meaningful within its own chain.
        '
        ' WmIntersectionRecord objects are shared between both
        ' curve-record collections.
        '==========================================================
        Friend Function BuildIntersectionRecords(
    segmentsA As IReadOnlyList(Of IPathSegment),
    segmentsB As IReadOnlyList(Of IPathSegment),
    tolerance As Double,
    ByRef intersections As List(Of WmIntersectionRecord),
    ByRef curveRecordsA As List(Of WmCurveRecord),
    ByRef curveRecordsB As List(Of WmCurveRecord)) As Boolean

            intersections =
        New List(Of WmIntersectionRecord)()

            curveRecordsA =
        New List(Of WmCurveRecord)()

            curveRecordsB =
        New List(Of WmCurveRecord)()

            If segmentsA Is Nothing OrElse
       segmentsB Is Nothing Then

                Return False

            End If

            Dim doesIntersect As Boolean = False

            '==========================================================
            ' Every segment in A is tested against every segment in B.
            '
            ' There is intentionally NO OkayToIntersect() call here.
            ' Adjacency suppression applies only to segments belonging
            ' to the same chain.
            '==========================================================

            For segmentIndexA As Integer = 0 To segmentsA.Count - 1

                Dim segmentA As IPathSegment =
            segmentsA(segmentIndexA)

                If segmentA Is Nothing Then
                    Continue For
                End If

                For segmentIndexB As Integer = 0 To segmentsB.Count - 1

                    Dim segmentB As IPathSegment =
                segmentsB(segmentIndexB)

                    If segmentB Is Nothing Then
                        Continue For
                    End If

                    Dim found As Boolean = False

                    '==================================================
                    ' Line / Line
                    '==================================================

                    Dim lineA As LineSegment2D =
                TryCast(segmentA, LineSegment2D)

                    Dim lineB As LineSegment2D =
                TryCast(segmentB, LineSegment2D)

                    If lineA IsNot Nothing AndAlso
               lineB IsNot Nothing Then

                        Dim intersectionPoint As PointF
                        Dim uA As Double
                        Dim uB As Double

                        If IntersectLineLine(
                    lineA,
                    lineB,
                    intersectionPoint,
                    uA,
                    uB) Then

                            Dim intersection As WmIntersectionRecord =
                        FindOrCreateIntersection(
                            intersections,
                            intersectionPoint,
                            tolerance)

                            Dim curveRecordA As WmCurveRecord =
                        FindOrCreateCurveRecord(
                            curveRecordsA,
                            segmentIndexA,
                            segmentsA)

                            Dim curveRecordB As WmCurveRecord =
                        FindOrCreateCurveRecord(
                            curveRecordsB,
                            segmentIndexB,
                            segmentsB)

                            RegisterIntersection(
                        intersection,
                        curveRecordA,
                        curveRecordB,
                        uA,
                        uB)

                            found = True

                        End If

                    Else

                        '==================================================
                        ' Line / Arc
                        '==================================================

                        Dim arcA As ArcSegment2D =
                    TryCast(segmentA, ArcSegment2D)

                        Dim arcB As ArcSegment2D =
                    TryCast(segmentB, ArcSegment2D)

                        If lineA IsNot Nothing AndAlso
                   arcB IsNot Nothing Then

                            Dim points As List(Of PointF)
                            Dim uArc As List(Of Double)
                            Dim uLine As List(Of Double)

                            If IntersectArcLine(
                        arcB,
                        lineA,
                        tolerance,
                        points,
                        uArc,
                        uLine) Then

                                For index As Integer = 0 To points.Count - 1

                                    Dim intersection As WmIntersectionRecord =
                                FindOrCreateIntersection(
                                    intersections,
                                    points(index),
                                    tolerance)

                                    Dim curveRecordA As WmCurveRecord =
                                FindOrCreateCurveRecord(
                                    curveRecordsA,
                                    segmentIndexA,
                                    segmentsA)

                                    Dim curveRecordB As WmCurveRecord =
                                FindOrCreateCurveRecord(
                                    curveRecordsB,
                                    segmentIndexB,
                                    segmentsB)

                                    RegisterIntersection(
                                intersection,
                                curveRecordA,
                                curveRecordB,
                                uLine(index),
                                uArc(index))

                                    found = True

                                Next

                            End If

                        ElseIf arcA IsNot Nothing AndAlso
                       lineB IsNot Nothing Then

                            '==================================================
                            ' Arc / Line
                            '
                            ' IntersectArcLine returns the arc parameter first
                            ' and the line parameter second.
                            '==================================================

                            Dim points As List(Of PointF)
                            Dim uArc As List(Of Double)
                            Dim uLine As List(Of Double)

                            If IntersectArcLine(
                        arcA,
                        lineB,
                        tolerance,
                        points,
                        uArc,
                        uLine) Then

                                For index As Integer = 0 To points.Count - 1

                                    Dim intersection As WmIntersectionRecord =
                                FindOrCreateIntersection(
                                    intersections,
                                    points(index),
                                    tolerance)

                                    Dim curveRecordA As WmCurveRecord =
                                FindOrCreateCurveRecord(
                                    curveRecordsA,
                                    segmentIndexA,
                                    segmentsA)

                                    Dim curveRecordB As WmCurveRecord =
                                FindOrCreateCurveRecord(
                                    curveRecordsB,
                                    segmentIndexB,
                                    segmentsB)

                                    RegisterIntersection(
                                intersection,
                                curveRecordA,
                                curveRecordB,
                                uArc(index),
                                uLine(index))

                                    found = True

                                Next

                            End If

                        ElseIf arcA IsNot Nothing AndAlso
                       arcB IsNot Nothing Then

                            '==================================================
                            ' Arc / Arc
                            '==================================================

                            Dim points As List(Of PointF)
                            Dim uA As List(Of Double)
                            Dim uB As List(Of Double)

                            If IntersectArcArc(
                        arcA,
                        arcB,
                        tolerance,
                        points,
                        uA,
                        uB) Then

                                For index As Integer = 0 To points.Count - 1

                                    Dim intersection As WmIntersectionRecord =
                                FindOrCreateIntersection(
                                    intersections,
                                    points(index),
                                    tolerance)

                                    Dim curveRecordA As WmCurveRecord =
                                FindOrCreateCurveRecord(
                                    curveRecordsA,
                                    segmentIndexA,
                                    segmentsA)

                                    Dim curveRecordB As WmCurveRecord =
                                FindOrCreateCurveRecord(
                                    curveRecordsB,
                                    segmentIndexB,
                                    segmentsB)

                                    RegisterIntersection(
                                intersection,
                                curveRecordA,
                                curveRecordB,
                                uA(index),
                                uB(index))

                                    found = True

                                Next

                            End If

                        End If

                    End If

                    If found Then
                        doesIntersect = True
                    End If

                Next

            Next

            '==========================================================
            ' Add the implicit beginning/end records for every segment
            ' that received an intersection.
            '==========================================================

            FinalizeCurveRecords(curveRecordsA)
            FinalizeCurveRecords(curveRecordsB)

            Return doesIntersect

        End Function

        Private Sub FinalizeCurveRecords(
    curveRecords As List(Of WmCurveRecord))

            For Each curveRecord As WmCurveRecord In curveRecords

                If curveRecord.Count = 0 Then
                    curveRecord.Update(
                        Nothing,
                        0.0)

                    curveRecord.Update(
                        Nothing,
                        1.0)

                    Continue For
                End If

                Dim firstExt As WmCurveRecordExtData =
                    curveRecord.ExtData(0)

                If Math.Abs(firstExt.UParam) > SmallTolerance Then
                    curveRecord.Update(
                        Nothing,
                        0.0)
                End If

                Dim lastExt As WmCurveRecordExtData =
                    curveRecord.ExtData(
                        curveRecord.Count - 1)

                If (1.0 - Math.Abs(lastExt.UParam)) > SmallTolerance Then
                    curveRecord.Update(
                        Nothing,
                        1.0)
                End If

            Next

        End Sub

    End Module

    '==========================================================
    ' Intersection record
    ' Corresponds to legacy CWmIntRec
    '==========================================================
    Friend Class WmIntersectionRecord

        Public ReadOnly Property Point As PointF

        Private ReadOnly _curveRecords As New List(Of WmCurveRecordExtData)

        Public ReadOnly Property Count As Integer
            Get
                Return _curveRecords.Count
            End Get
        End Property

        Public Sub New(point As PointF)
            Me.Point = point
        End Sub

        Public Function ExtData(index As Integer) As WmCurveRecordExtData
            Return _curveRecords(index)
        End Function

        Public Sub Update(record As WmCurveRecordExtData)
            If record Is Nothing Then Return

            ' Prevent duplicate references to the same
            ' curve-record extension.
            If _curveRecords.Contains(record) Then Return

            _curveRecords.Add(record)
        End Sub

        Public Sub Reduce()
            If _curveRecords.Count <= 2 Then Return

            ' The legacy implementation removes tagged
            ' extension records when more than two references
            ' exist at an intersection.
            For i As Integer = _curveRecords.Count - 1 To 0 Step -1
                If _curveRecords(i).IsTagged Then
                    _curveRecords.RemoveAt(i)
                End If
            Next
        End Sub

        Public Function IsValid() As Boolean

            If _curveRecords.Count = 0 Then
                Return True
            End If

            Return _curveRecords(0) IsNot Nothing AndAlso
                   _curveRecords(_curveRecords.Count - 1) IsNot Nothing

        End Function



    End Class


    '==========================================================
    ' Curve record
    ' Corresponds to legacy CWmCrvRec
    '==========================================================
    Friend Class WmCurveRecord

        Public ReadOnly Property Segment As IPathSegment

        Public ReadOnly Property SegmentIndex As Integer

        Private ReadOnly _extData As New List(Of WmCurveRecordExtData)

        Public ReadOnly Property Count As Integer
            Get
                Return _extData.Count
            End Get
        End Property

        Public Sub New(
    segmentIndex As Integer,
    segment As IPathSegment)

            Me.SegmentIndex = segmentIndex
            Me.Segment = segment

        End Sub

        Public Function ExtData(index As Integer) As WmCurveRecordExtData
            Return _extData(index)
        End Function

        Public Function Update(
    intersection As WmIntersectionRecord,
    uParam As Double) As WmCurveRecordExtData

            For Each existing As WmCurveRecordExtData In _extData

                If Object.ReferenceEquals(
            existing.Intersection,
            intersection) AndAlso
           intersection IsNot Nothing Then

                    Return existing

                End If

                If Math.Abs(
            existing.UParam - uParam) <= 0.000000000001 Then

                    Return existing

                End If

            Next

            Dim newData As New WmCurveRecordExtData(
        Me,
        intersection,
        uParam)

            For i As Integer = 0 To _extData.Count - 1

                If uParam < _extData(i).UParam Then

                    _extData.Insert(
                i,
                newData)

                    Return newData

                End If

            Next

            _extData.Add(newData)

            Return newData

        End Function

        Friend Function AddIntersection(
    intersection As WmIntersectionRecord,
    uParam As Double) As WmCurveRecordExtData

            Return Update(intersection, uParam)

        End Function

    End Class


    '==========================================================
    ' Curve record extension data
    ' Corresponds to legacy CWmCrvRecExtData
    '==========================================================
    Friend Class WmCurveRecordExtData

        Public ReadOnly Property Owner As WmCurveRecord

        Public ReadOnly Property Intersection As WmIntersectionRecord

        Public ReadOnly Property UParam As Double

        Private _subChain As Object
        Private _tagged As Boolean

        Public Sub New(
            owner As WmCurveRecord,
            intersection As WmIntersectionRecord,
            uParam As Double)

            Me.Owner = owner
            Me.Intersection = intersection
            Me.UParam = uParam

        End Sub

        Public ReadOnly Property Point As PointF
            Get

                If Intersection IsNot Nothing Then
                    Return Intersection.Point
                End If

                If Owner Is Nothing OrElse
           Owner.Segment Is Nothing Then

                    Return PointF.Empty

                End If

                If Math.Abs(UParam) < 0.000001 Then
                    Return Owner.Segment.StartPoint
                End If

                Return Owner.Segment.EndPoint

            End Get
        End Property

        Public Property SubChain As Object
            Get
                Return _subChain
            End Get
            Set(value As Object)
                _subChain = value
            End Set
        End Property

        Public Sub Tag()
            _tagged = True
        End Sub

        Public ReadOnly Property IsTagged As Boolean
            Get
                Return _tagged
            End Get
        End Property

    End Class
End Namespace