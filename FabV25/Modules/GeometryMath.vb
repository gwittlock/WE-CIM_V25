Public Module GeometryMath

    Public Function DistancePointToSegment(p As PointF,
                                           a As PointF,
                                           b As PointF) As Double

        Dim dx = b.X - a.X
        Dim dy = b.Y - a.Y

        If dx = 0 AndAlso dy = 0 Then
            Return Distance(p, a)
        End If

        Dim t = ((p.X - a.X) * dx + (p.Y - a.Y) * dy) / (dx * dx + dy * dy)

        t = Math.Max(0, Math.Min(1, t))

        Dim projX = a.X + t * dx
        Dim projY = a.Y + t * dy

        Return Math.Sqrt((p.X - projX) ^ 2 + (p.Y - projY) ^ 2)

    End Function

    Public Function Distance(p1 As PointF, p2 As PointF) As Double
        Return Math.Sqrt((p1.X - p2.X) ^ 2 + (p1.Y - p2.Y) ^ 2)
    End Function

    Public Function ScreenToWorld(pt As PointF,
                              vt As ViewTransform,
                              panelHeight As Integer) As PointF

        Dim worldX As Single = CSng((pt.X - vt.OffsetX) / vt.Scale)
        Dim worldY As Single = CSng((panelHeight - pt.Y - vt.OffsetY) / vt.Scale)

        Return New PointF(worldX, worldY)

    End Function


End Module
