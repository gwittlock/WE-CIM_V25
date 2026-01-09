Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities


Module modGeo


    Private Function DxfToPointF(p As IxMilia.Dxf.DxfPoint) As PointF
        Return New PointF(CSng(p.X), CSng(p.Y))
    End Function


    Public Sub DrawArcCorrectly(
    ByVal arc As DxfArc,
    ByVal g As Graphics,
    ByVal bounds As Rectangle,
    ByVal scaleFactor As Double,
    ByVal offsetX As Double,
    ByVal offsetY As Double,
    worldMaxY As Double)

        '----------------------------------------
        ' Calculate bounding rectangle (world → screen)
        '----------------------------------------
        Dim diameter As Single = CSng(arc.Radius * 2 * scaleFactor)

        Dim left As Single =
        CSng((arc.Center.X - arc.Radius) * scaleFactor + offsetX)

        Dim top As Single =
        CSng(bounds.Height - ((arc.Center.Y + arc.Radius) * scaleFactor + offsetY))

        ' Convert DXF center to screen space
        Dim centerWorld As PointF = DxfToPointF(arc.Center)
        Dim centerScreen As PointF = ToScreen(centerWorld, scaleFactor, offsetX, offsetY, worldMaxY)

        ' Define bounding box points
        Dim p1 As New PointF(centerScreen.X - arc.Radius, centerScreen.Y - arc.Radius) ' bottom-left / top-left
        Dim p2 As New PointF(centerScreen.X + arc.Radius, centerScreen.Y + arc.Radius) ' top-right / bottom-right


        Dim arcRect As RectangleF = HelperFunctions.RectFromPoints(p1, p2)

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
