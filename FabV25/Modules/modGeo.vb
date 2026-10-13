Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports FabV25_WIN8.CadEntity

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

    Private Sub DrawCadArc(
    a As CadArc,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        ' --------------------------------
        ' Center → screen space
        ' --------------------------------
        Dim center = WorldToScreen(a.Center.X, a.Center.Y, vt, panelHeight)

        ' --------------------------------
        ' Radius (scaled)
        ' --------------------------------
        Dim r As Single = CSng(a.Radius * vt.Scale)

        ' --------------------------------
        ' Bounding rectangle
        ' --------------------------------
        Dim p1 As New PointF(center.X - r, center.Y - r)
        Dim p2 As New PointF(center.X + r, center.Y + r)

        Dim rect As RectangleF = HelperFunctions.RectFromPoints(p1, p2)

        ' --------------------------------
        ' DXF → GDI+ angle conversion
        '
        ' DXF:
        '   - CCW
        '   - 0° at +X
        '
        ' GDI+:
        '   - CW
        '   - 0° at +X
        ' --------------------------------
        Dim startAngle As Single = CSng(-a.StartAngle)
        Dim sweepAngle As Single = CSng(-(a.EndAngle - a.StartAngle))

        ' Normalize sweep
        If sweepAngle <= 0 Then
            sweepAngle += 360.0F
        End If

        Using pen As New Pen(a.Color, 1.0F)
            g.DrawArc(pen, rect, startAngle, sweepAngle)
        End Using

    End Sub

End Module
