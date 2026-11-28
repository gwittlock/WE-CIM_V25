Imports System.Drawing

Public NotInheritable Class PatternGeometry

    Private Sub New()
    End Sub

    Public Shared Function TransformPatternPoint(
        x As Double,
        y As Double,
        instance As PatternInstance) As PointF

        Dim radians As Double =
            instance.Angle * Math.PI / 180.0

        Dim cosA As Double = Math.Cos(radians)
        Dim sinA As Double = Math.Sin(radians)

        Dim transformedX As Double =
            x * cosA - y * sinA + instance.X

        Dim transformedY As Double =
            x * sinA + y * cosA + instance.Y

        Return New PointF(
            CSng(transformedX),
            CSng(transformedY))

    End Function

End Class