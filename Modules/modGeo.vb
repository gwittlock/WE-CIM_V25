Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities


Module modGeo

    Public Sub DrawArcCorrectly(
    ByVal arc As DxfArc,
    ByVal g As Graphics,
    ByVal bounds As Rectangle,
    ByVal scaleFactor As Double,
    ByVal offsetX As Double,
    ByVal offsetY As Double)

        '----------------------------------------
        ' Calculate bounding rectangle (world → screen)
        '----------------------------------------
        Dim diameter As Single = CSng(arc.Radius * 2 * scaleFactor)

        Dim left As Single =
        CSng((arc.Center.X - arc.Radius) * scaleFactor + offsetX)

        Dim top As Single =
        CSng(bounds.Height - ((arc.Center.Y + arc.Radius) * scaleFactor + offsetY))

        Dim arcRect As New RectangleF(left, top, diameter, diameter)

        '----------------------------------------
        ' DXF → GDI+ angle conversion
        '----------------------------------------
        Dim startAngle As Single = CSng(-arc.StartAngle)
        Dim sweepAngle As Single = CSng(-(arc.EndAngle - arc.StartAngle))

        '----------------------------------------
        ' Normalize sweep angle
        '----------------------------------------
        If Math.Abs(sweepAngle) > 360.0F Then
            sweepAngle = Math.Sign(sweepAngle) * 360.0F
        End If

        '----------------------------------------
        ' Draw arc
        '----------------------------------------
        g.DrawArc(Pens.Black, arcRect, startAngle, sweepAngle)

    End Sub


End Module
