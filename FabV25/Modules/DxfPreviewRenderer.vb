Imports System.Drawing
Imports System.Drawing.Drawing2D
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities

Public Module DxfPreviewRenderer

    '==============================
    ' Main Entry Point
    '==============================
    Public Sub DrawDxfPreview(dxf As DxfFile, g As Graphics, bounds As Rectangle)

        If dxf Is Nothing Then
            g.Clear(Color.White)
            Return
        End If

        g.Clear(Color.White)
        g.SmoothingMode = SmoothingMode.AntiAlias

        ' Compute world bounds
        Dim worldBounds As RectangleF = ComputeDxfBounds(dxf)
        If worldBounds.IsEmpty Then Return

        ' Compute scale & offsets
        Dim scaleX As Double = bounds.Width / worldBounds.Width
        Dim scaleY As Double = bounds.Height / worldBounds.Height
        Dim scaleFactor As Double = Math.Min(scaleX, scaleY) * 0.95

        Dim offsetX As Double = (bounds.Width - worldBounds.Width * scaleFactor) / 2 - worldBounds.Left * scaleFactor
        Dim offsetY As Double = (bounds.Height - worldBounds.Height * scaleFactor) / 2 - worldBounds.Top * scaleFactor

        ' Draw all entities
        For Each ent As DxfEntity In dxf.Entities
            Select Case True
                Case TypeOf ent Is DxfLine
                    Dim l = CType(ent, DxfLine)
                    DrawLineWorld(l.P1, l.P2, g, bounds, scaleFactor, offsetX, offsetY)

                Case TypeOf ent Is DxfCircle
                    DrawCircleWorld(CType(ent, DxfCircle), g, bounds, scaleFactor, offsetX, offsetY)

                Case TypeOf ent Is DxfArc
                    DrawArcCorrectly(CType(ent, DxfArc), g, bounds, scaleFactor, offsetX, offsetY)

                Case TypeOf ent Is DxfPolyline
                    DrawPolylineWorld(CType(ent, DxfPolyline), g, bounds, scaleFactor, offsetX, offsetY)
            End Select
        Next

    End Sub

    '==============================
    ' Compute DXF Bounds
    '==============================
    Public Function ComputeDxfBounds(dxf As DxfFile) As RectangleF
        Dim minX As Double = Double.MaxValue
        Dim minY As Double = Double.MaxValue
        Dim maxX As Double = Double.MinValue
        Dim maxY As Double = Double.MinValue

        For Each ent As DxfEntity In dxf.Entities
            Select Case True
                Case TypeOf ent Is DxfLine
                    Dim l = CType(ent, DxfLine)
                    UpdateBounds(l.P1.X, l.P1.Y, minX, minY, maxX, maxY)
                    UpdateBounds(l.P2.X, l.P2.Y, minX, minY, maxX, maxY)

                Case TypeOf ent Is DxfCircle
                    Dim c = CType(ent, DxfCircle)
                    UpdateBounds(c.Center.X - c.Radius, c.Center.Y - c.Radius, minX, minY, maxX, maxY)
                    UpdateBounds(c.Center.X + c.Radius, c.Center.Y + c.Radius, minX, minY, maxX, maxY)

                Case TypeOf ent Is DxfArc
                    Dim a = CType(ent, DxfArc)
                    UpdateBounds(a.Center.X - a.Radius, a.Center.Y - a.Radius, minX, minY, maxX, maxY)
                    UpdateBounds(a.Center.X + a.Radius, a.Center.Y + a.Radius, minX, minY, maxX, maxY)

                Case TypeOf ent Is DxfPolyline
                    Dim pl = CType(ent, DxfPolyline)
                    For Each v In pl.Vertices
                        UpdateBounds(v.Location.X, v.Location.Y, minX, minY, maxX, maxY)
                    Next
            End Select
        Next

        If minX = Double.MaxValue Then Return RectangleF.Empty
        Return RectangleF.FromLTRB(CSng(minX), CSng(minY), CSng(maxX), CSng(maxY))
    End Function

    Private Sub UpdateBounds(x As Double, y As Double, ByRef minX As Double, ByRef minY As Double, ByRef maxX As Double, ByRef maxY As Double)
        minX = Math.Min(minX, x)
        minY = Math.Min(minY, y)
        maxX = Math.Max(maxX, x)
        maxY = Math.Max(maxY, y)
    End Sub

    '==============================
    ' Drawing Helpers
    '==============================
    Private Sub DrawLineWorld(p1 As DxfPoint, p2 As DxfPoint, g As Graphics, bounds As Rectangle, scaleFactor As Double, offsetX As Double, offsetY As Double)
        g.DrawLine(Pens.Black,
                   CSng(p1.X * scaleFactor + offsetX), CSng(bounds.Height - (p1.Y * scaleFactor + offsetY)),
                   CSng(p2.X * scaleFactor + offsetX), CSng(bounds.Height - (p2.Y * scaleFactor + offsetY)))
    End Sub

    Private Sub DrawCircleWorld(c As DxfCircle, g As Graphics, bounds As Rectangle, scaleFactor As Double, offsetX As Double, offsetY As Double)
        Dim size = CSng(c.Radius * 2 * scaleFactor)
        Dim x = CSng((c.Center.X - c.Radius) * scaleFactor + offsetX)
        Dim y = CSng(bounds.Height - ((c.Center.Y + c.Radius) * scaleFactor + offsetY))
        g.DrawEllipse(Pens.Blue, x, y, size, size)
    End Sub

    Private Sub DrawArcCorrectly(a As DxfArc, g As Graphics, bounds As Rectangle, scaleFactor As Double, offsetX As Double, offsetY As Double)
        Dim size = CSng(a.Radius * 2 * scaleFactor)
        Dim x = CSng((a.Center.X - a.Radius) * scaleFactor + offsetX)
        Dim y = CSng(bounds.Height - ((a.Center.Y + a.Radius) * scaleFactor + offsetY))
        g.DrawArc(Pens.Green, x, y, size, size, CSng(-a.StartAngle), CSng(-(a.EndAngle - a.StartAngle)))
    End Sub

    Private Sub DrawPolylineWorld(pl As DxfPolyline, g As Graphics, bounds As Rectangle, scaleFactor As Double, offsetX As Double, offsetY As Double)
        If pl.Vertices.Count < 2 Then Return
        For i = 0 To pl.Vertices.Count - 2
            DrawLineWorld(pl.Vertices(i).Location, pl.Vertices(i + 1).Location, g, bounds, scaleFactor, offsetX, offsetY)
        Next
        If pl.IsClosed Then
            DrawLineWorld(pl.Vertices.Last().Location, pl.Vertices.First().Location, g, bounds, scaleFactor, offsetX, offsetY)
        End If
    End Sub

End Module
