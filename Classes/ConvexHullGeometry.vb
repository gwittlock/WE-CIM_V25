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
        Private Const VectorSmall As Double = 0.000001

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

        Public Function NormalizeVector(
    vector As PointF) As PointF

            Dim length As Double =
                Math.Sqrt(
                    vector.X * vector.X +
                    vector.Y * vector.Y)

            If length < 0.0000001 Then
                Return New PointF(0.0F, 0.0F)
            End If

            Return New PointF(
                CSng(vector.X / length),
                CSng(vector.Y / length))

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

                If PointsWithinTolerance(result.StartPoint, result.EndPoint, 0.000001) Then

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


    End Module

End Namespace