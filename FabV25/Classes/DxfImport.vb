Imports FabV25_WIN8.AppData
Imports FabV25_WIN8.CadEntity
Imports FabV25_WIN8.ImportedEntity
Imports FabV25_WIN8.LayerDefinition
Imports FabV25_WIN8.LayerDefinition.ImportedEntity
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.Drawing

Public Module DxfImport

    ' Store imported CadEntities
    Public CurrentDXFEntities As New List(Of CadEntity)

    Public Enum DxfPlacementMode
        PreserveCoordinates
        AlignLowerLeftToSheetOrigin
        AlignLowerLeftToActiveWorkZone
        CenterInSheet
    End Enum

    Public Sub ApplyDxfPlacement(
    entities As List(Of CadEntity),
    mode As DxfPlacementMode,
    sheetWidth As Double,
    sheetHeight As Double,
    Optional activeWorkZone As WorkZone = Nothing
)
        If entities Is Nothing OrElse entities.Count = 0 Then Exit Sub

        Dim bounds = HelperFunctions.GetCadBounds(entities)
        If bounds.IsEmpty Then Exit Sub

        Dim dx As Single = 0
        Dim dy As Single = 0

        Select Case mode

            Case DxfPlacementMode.PreserveCoordinates
                Return

            Case DxfPlacementMode.AlignLowerLeftToSheetOrigin
                dx = -bounds.Left
                dy = -bounds.Bottom

            Case DxfPlacementMode.CenterInSheet
                dx = CSng(sheetWidth / 2 - (bounds.Left + bounds.Width / 2))
                dy = CSng(sheetHeight / 2 - (bounds.Bottom + bounds.Height / 2))

            Case DxfPlacementMode.AlignLowerLeftToActiveWorkZone
                If activeWorkZone Is Nothing Then Exit Sub
                dx = CSng(activeWorkZone.XMin - bounds.Left)
                dy = CSng(activeWorkZone.YMin - bounds.Bottom)

        End Select

        For Each e In entities
            e.Translate(dx, dy)
        Next
    End Sub


    'Converts polylines With bulges into lines/arcs
    Private Function ExplodePolyline(pl As IxMilia.Dxf.Entities.DxfPolyline) As List(Of CadEntity)
        Dim result As New List(Of CadEntity)()

        Dim verts = pl.Vertices
        Dim count = verts.Count
        For i = 0 To count - 1
            Dim v1 = verts(i)
            Dim v2 = verts((i + 1) Mod count) ' wrap if closed

            ' Check for bulge (arc segment)
            If v1.Bulge = 0 Then
                ' straight line
                result.Add(New CadLine With {
                .StartPoint = New PointF(CSng(v1.Location.X), CSng(v1.Location.Y)),
                .EndPoint = New PointF(CSng(v2.Location.X), CSng(v2.Location.Y)),
                .Color = System.Drawing.Color.Black
            })
            Else
                ' arc from bulge
                result.Add(ArcFromBulge(v1.Location, v2.Location, v1.Bulge))
            End If
        Next

        Return result
    End Function


    ' Converts two vertices + bulge into a CadArc
    Private Function ArcFromBulge(p1 As IxMilia.Dxf.DxfPoint, p2 As IxMilia.Dxf.DxfPoint, bulge As Double) As CadArc
        ' Calculate chord length
        Dim dx = p2.X - p1.X
        Dim dy = p2.Y - p1.Y
        Dim chord = Math.Sqrt(dx * dx + dy * dy)

        ' Included angle
        Dim angle = 4 * Math.Atan(bulge)

        ' Radius
        Dim r = chord / (2 * Math.Sin(angle / 2))

        ' Midpoint
        Dim mx = (p1.X + p2.X) / 2
        Dim my = (p1.Y + p2.Y) / 2

        ' Determine center of arc
        Dim sagitta = r * (1 - Math.Cos(angle / 2))
        Dim perpX = -dy / chord * sagitta
        Dim perpY = dx / chord * sagitta
        Dim cx = mx + perpX
        Dim cy = my + perpY

        ' Start/End angles
        Dim startAngle = Math.Atan2(p1.Y - cy, p1.X - cx) * 180 / Math.PI
        Dim endAngle = Math.Atan2(p2.Y - cy, p2.X - cx) * 180 / Math.PI

        Return New CadArc With {
        .Center = New PointF(CSng(cx), CSng(cy)),
        .Radius = CSng(r),
        .StartAngle = CSng(startAngle),
        .EndAngle = CSng(endAngle),
        .Color = System.Drawing.Color.Black
    }
    End Function

    Public Function ImportDxf(path As String) As List(Of CadEntity)



        Dim entities As New List(Of CadEntity)
        Dim dxf = DxfFile.Load(path)

        ' -----------------------------
        ' 1) Import raw entities
        ' -----------------------------
        For Each e In dxf.Entities

            Select Case e.EntityType

                Case DxfEntityType.Line
                    Dim l = CType(e, DxfLine)
                    entities.Add(New CadLine With {
                    .StartPoint = New PointF(CSng(l.P1.X), CSng(l.P1.Y)),
                    .EndPoint = New PointF(CSng(l.P2.X), CSng(l.P2.Y)),
                    .LayerName = l.Layer,
                   .Color = GetLayerColor(dxf, l.Layer)
                })

                Case DxfEntityType.Circle
                    Dim c = CType(e, DxfCircle)
                    entities.Add(New CadCircle With {
                    .Center = New PointF(CSng(c.Center.X), CSng(c.Center.Y)),
                    .Radius = c.Radius,
                    .LayerName = c.Layer,
                   .Color = GetLayerColor(dxf, c.Layer)
                })

                Case DxfEntityType.Arc
                    Dim a = CType(e, DxfArc)
                    entities.Add(New CadArc With {
                    .Center = New PointF(CSng(a.Center.X), CSng(a.Center.Y)),
                    .Radius = a.Radius,
                    .StartAngle = a.StartAngle,
                    .EndAngle = a.EndAngle,
                    .LayerName = a.Layer,
                    .Color = GetLayerColor(dxf, a.Layer)
                })

                Case DxfEntityType.Point
                    Debug.Print("DXF POINT ENTITY CLASS = " & e.GetType().FullName)

                    Dim p = CType(e, IxMilia.Dxf.Entities.DxfModelPoint)
                    Debug.Print("Count:" & p.XData.Count)

                    entities.Add(New CadPoint(
                        New PointF(
                            CSng(p.Location.X),
                            CSng(p.Location.Y))) With {
                        .LayerName = p.Layer,
                       .Color = GetLayerColor(dxf, p.Layer)
                    })


            End Select

        Next

        ' -----------------------------
        ' 2) Normalize geometry to (0,0)
        ' -----------------------------
        Dim bounds As RectangleF = HelperFunctions.GetCadBounds(entities)

        If Not bounds.IsEmpty Then
            Dim dx As Double = -bounds.X
            Dim dy As Double = -bounds.Y

            For Each ce In entities
                ce.Translate(dx, dy)
            Next
        End If

        Return entities

    End Function
    Public Sub NormalizeCadToSheet(entities As List(Of CadEntity), sheetWidth As Double, sheetHeight As Double)
        If entities Is Nothing OrElse entities.Count = 0 Then Exit Sub

        ' Get CAD model bounds
        Dim bounds = HelperFunctions.GetCadBounds(entities)

        ' Translate part to sheet bottom-left
        Dim dx As Double = -bounds.X               ' move part left edge to X=0
        Dim dy As Double = -bounds.Y              ' move part bottom edge to Y=0

        For Each e In entities
            e.Translate(dx, dy)

            ' Flip part vertically to match sheet bottom-left origin
            'e.FlipY(bounds.Height)
        Next
    End Sub

    Private Sub DrawCadLine(
    l As CadLine,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        Dim p1 = WorldToScreen(l.StartPoint.X, l.StartPoint.Y, vt, panelHeight)
        Dim p2 = WorldToScreen(l.EndPoint.X, l.EndPoint.Y, vt, panelHeight)

        Dim drawColor As System.Drawing.Color = l.Color
        Dim drawWidth As Single = 1.0F

        If l.IsSelected Then
            drawColor = System.Drawing.Color.Yellow
            drawWidth = 3.0F
        End If

        Using pen As New Pen(drawColor, drawWidth)
            g.DrawLine(pen, p1, p2)
        End Using

    End Sub

    Private Sub DrawCadPoint(
    pt As CadPoint,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        Dim screenPt As PointF =
        WorldToScreen(
            pt.Position.X,
            pt.Position.Y,
            vt,
            panelHeight)


        Const pointSize As Single = 6.0F

        g.FillRectangle(
        Brushes.Black,
        screenPt.X - pointSize / 2.0F,
        screenPt.Y - pointSize / 2.0F,
        pointSize,
        pointSize)

    End Sub
    Private Sub DrawCadArc(
    a As CadArc,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        Dim centerScreen = WorldToScreen(a.Center.X, a.Center.Y, vt, panelHeight)
        Dim r As Single = CSng(a.Radius * vt.Scale)

        Dim p1 As New PointF(centerScreen.X - r, centerScreen.Y - r)
        Dim p2 As New PointF(centerScreen.X + r, centerScreen.Y + r)

        Dim rect As RectangleF = HelperFunctions.RectFromPoints(p1, p2)

        Dim sweep As Single

        If a.IsClockwise Then

            sweep = CSng(a.StartAngle - a.EndAngle)

            If sweep < 0 Then
                sweep += 360
            End If

        Else

            sweep = CSng(a.EndAngle - a.StartAngle)

            If sweep < 0 Then
                sweep += 360
            End If

        End If

        Dim startAngle As Single = CSng(-a.StartAngle)

        Dim sweepAngle As Single

        If a.IsClockwise Then
            sweepAngle = sweep
        Else
            sweepAngle = -sweep
        End If

        Dim drawColor As System.Drawing.Color = a.Color
        Dim drawWidth As Single = 1.0F

        If a.IsSelected Then
            drawColor = System.Drawing.Color.Yellow
            drawWidth = 3.0F
        End If

        Using pen As New Pen(drawColor, drawWidth)
            g.DrawArc(pen, rect, startAngle, sweepAngle)
        End Using

    End Sub
    Private Sub DrawCadCircle(
    c As CadCircle,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        Dim center = WorldToScreen(c.Center.X, c.Center.Y, vt, panelHeight)
        Dim r As Single = CSng(c.Radius * vt.Scale)

        Dim p1 As New PointF(center.X - r, center.Y - r)
        Dim p2 As New PointF(center.X + r, center.Y + r)

        Dim rect = HelperFunctions.RectFromPoints(p1, p2)

        Dim drawColor As System.Drawing.Color = c.Color
        Dim drawWidth As Single = 1.0F

        If c.IsSelected Then
            drawColor = System.Drawing.Color.Yellow
            drawWidth = 3.0F
        End If

        Using pen As New Pen(drawColor, drawWidth)
            g.DrawEllipse(pen, rect)
        End Using

    End Sub

    Public Function ToScreen(
    pt As PointF,
    scale As Double,
    ox As Double,
    oy As Double,
    worldMaxY As Double
) As PointF

        Dim sx As Single = CSng((pt.X * scale) + ox)
        Dim sy As Single = CSng((worldMaxY - pt.Y) * scale + oy)

        Return New PointF(sx, sy)
    End Function

    Public Sub DrawCadEntities(
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If _cadEntities Is Nothing OrElse _cadEntities.Count = 0 Then Exit Sub

        For Each ce In _cadEntities
            Select Case True
                Case TypeOf ce Is CadLine
                    DrawCadLine(CType(ce, CadLine), g, vt, panelHeight)

                Case TypeOf ce Is CadArc
                    DrawCadArc(CType(ce, CadArc), g, vt, panelHeight)

                Case TypeOf ce Is CadCircle
                    DrawCadCircle(CType(ce, CadCircle), g, vt, panelHeight)

                Case TypeOf ce Is CadPoint
                    DrawCadPoint(CType(ce, CadPoint), g, vt, panelHeight)
            End Select
        Next


    End Sub


    Private Function GetLayerColor(
    dxfFile As DxfFile,
    layerName As String) As System.Drawing.Color

        If dxfFile Is Nothing Then
            Return System.Drawing.Color.Black
        End If

        If String.IsNullOrWhiteSpace(layerName) Then
            Return System.Drawing.Color.Black
        End If

        Dim layer As DxfLayer =
            dxfFile.Layers.FirstOrDefault(
                Function(x) String.Equals(
                    x.Name,
                    layerName,
                    StringComparison.OrdinalIgnoreCase))

        If layer Is Nothing Then
            Return System.Drawing.Color.Black
        End If

        Dim dxfColor As DxfColor = layer.Color

        Dim rgb As Integer = dxfColor.ToRGB()

        Return System.Drawing.Color.FromArgb(rgb)

        ' We will convert the ACI color here.
        Return System.Drawing.Color.Black


    End Function


End Module
