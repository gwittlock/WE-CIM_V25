Imports FabV25_WIN8.ToolShapes
Imports System.Drawing

Public NotInheritable Class ToolProfileRenderer

    Private Sub New()
    End Sub

    Private Shared Function WorldToScreen(
        x As Double,
        y As Double,
        vt As ViewTransform,
        panelHeight As Integer
    ) As PointF

        Dim sx As Single =
            CSng(x * vt.Scale + vt.OffsetX)

        Dim sy As Single =
            CSng(panelHeight - (y * vt.Scale + vt.OffsetY))

        Return New PointF(sx, sy)

    End Function

    '-------------------------------------------------
    ' Draw a tool cutting profile at a given position
    ' and rotation.
    '-------------------------------------------------
    Public Shared Sub DrawToolCuttingProfile(
        g As Graphics,
        shape As ToolShape,
        centerX As Double,
        centerY As Double,
        angle As Double,
        vt As ViewTransform,
        panelHeight As Integer,
        pen As Pen)

        If shape Is Nothing OrElse
           shape.CuttingProfile Is Nothing Then
            Return
        End If

        Dim angleRadians As Double =
            angle * Math.PI / 180.0

        Dim cosA As Double = Math.Cos(angleRadians)
        Dim sinA As Double = Math.Sin(angleRadians)

        For Each segment As IPathSegment In
            shape.CuttingProfile.Segments

            If segment Is Nothing Then Continue For

            Select Case segment.SegmentType

                Case PathSegmentType.Line

                    'Rotate the start point.
                    Dim x1 As Double =
                        segment.StartPoint.X * cosA -
                        segment.StartPoint.Y * sinA

                    Dim y1 As Double =
                        segment.StartPoint.X * sinA +
                        segment.StartPoint.Y * cosA

                    'Rotate the end point.
                    Dim x2 As Double =
                        segment.EndPoint.X * cosA -
                        segment.EndPoint.Y * sinA

                    Dim y2 As Double =
                        segment.EndPoint.X * sinA +
                        segment.EndPoint.Y * cosA

                    'Convert to screen coordinates.
                    Dim p1 As PointF =
                        WorldToScreen(
                            centerX + x1,
                            centerY + y1,
                            vt,
                            panelHeight)

                    Dim p2 As PointF =
                        WorldToScreen(
                            centerX + x2,
                            centerY + y2,
                            vt,
                            panelHeight)

                    g.DrawLine(pen, p1, p2)

                Case PathSegmentType.Arc

                    Dim arc As ArcSegment2D =
                        DirectCast(segment, ArcSegment2D)

                    'Rotate the arc center.
                    Dim rotatedCenterX As Double =
                        arc.Center.X * cosA -
                        arc.Center.Y * sinA

                    Dim rotatedCenterY As Double =
                        arc.Center.X * sinA +
                        arc.Center.Y * cosA

                    Dim center As PointF =
                        WorldToScreen(
                            centerX + rotatedCenterX,
                            centerY + rotatedCenterY,
                            vt,
                            panelHeight)

                    Dim radius As Single =
                        CSng(arc.Radius * vt.Scale)

                    Dim rect As New RectangleF(
                        center.X - radius,
                        center.Y - radius,
                        radius * 2.0F,
                        radius * 2.0F)

                    Dim startAngle As Single =
                        CSng(-(arc.StartAngle + angleRadians) *
                             180.0 / Math.PI)

                    Dim sweepRadians As Double =
                        arc.EndAngle - arc.StartAngle

                    If arc.IsClockwise Then

                        If sweepRadians > 0 Then
                            sweepRadians -= 2.0 * Math.PI
                        End If

                    Else

                        If sweepRadians < 0 Then
                            sweepRadians += 2.0 * Math.PI
                        End If

                    End If

                    Dim sweepAngle As Single =
                        CSng(-sweepRadians * 180.0 / Math.PI)

                    If Math.Abs(sweepRadians) >=
                       (2.0 * Math.PI - 0.000001) Then

                        g.DrawEllipse(pen, rect)

                    Else

                        g.DrawArc(
                            pen,
                            rect,
                            startAngle,
                            sweepAngle)

                    End If

            End Select

        Next

    End Sub

    '-------------------------------------------------
    ' Draw a tool cutting profile from a HoleFeature.
    '-------------------------------------------------
    Public Shared Sub DrawToolCuttingProfile(
        g As Graphics,
        shape As ToolShape,
        hole As HoleFeature,
        vt As ViewTransform,
        panelHeight As Integer,
        pen As Pen)

        If hole Is Nothing Then Return

        DrawToolCuttingProfile(
            g,
            shape,
            hole.X,
            hole.Y,
            hole.Angle,
            vt,
            panelHeight,
            pen)

    End Sub

End Class