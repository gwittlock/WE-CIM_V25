Imports System.Drawing

Namespace ToolShapes

    Friend Enum ConvexHullArcType
        Turn
        Move
    End Enum

    Friend Class ConvexHullArc

        Public ReadOnly Property ArcType As ConvexHullArcType

        Public ReadOnly Property Center As PointF

        Public ReadOnly Property StartTangent As Double

        Public ReadOnly Property EndTangent As Double

        Public ReadOnly Property Radius As Double

        Public ReadOnly Property IsClockwise As Boolean

        Public ReadOnly Property StartPoint As PointF

        Public ReadOnly Property EndPoint As PointF

        Public Sub New(
            center As PointF,
            startTangent As Double,
            endTangent As Double,
            radius As Double,
            isClockwise As Boolean)

            ArcType = ConvexHullArcType.Turn

            Me.Center = center
            Me.StartTangent = startTangent
            Me.EndTangent = endTangent
            Me.Radius = radius
            Me.IsClockwise = isClockwise

            StartPoint =
                CalculatePoint(
                    center,
                    radius,
                    startTangent)

            EndPoint =
                CalculatePoint(
                    center,
                    radius,
                    endTangent)

        End Sub

        Public Sub New(
    point As PointF,
    tangent As Double)

            ArcType = ConvexHullArcType.Move

            Me.Center = point
            Me.StartPoint = point
            Me.EndPoint = point

            Me.StartTangent = tangent
            Me.EndTangent = tangent
            Me.Radius = 0.0
            Me.IsClockwise = False

        End Sub

        Private Shared Function CalculatePoint(
            center As PointF,
            radius As Double,
            angle As Double) As PointF

            Return New PointF(
                CSng(center.X + radius * Math.Cos(angle)),
                CSng(center.Y + radius * Math.Sin(angle)))

        End Function

    End Class

End Namespace