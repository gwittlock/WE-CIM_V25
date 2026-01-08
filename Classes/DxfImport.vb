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
    ''' <summary>
    ''' Parse a DXF file and return a list of CadEntity objects.
    ''' Only lines, arcs, circles, and polylines are currently supported.
    ''' </summary>
    Public Function ParseDXFToCadEntities(filePath As String) As List(Of CadEntity)
        Dim entities As New List(Of CadEntity)

        Dim dxfDoc As DxfFile = DxfFile.Load(filePath)

        For Each ent In dxfDoc.Entities
            Dim cadEnt = CadEntity.FromDxf(ent)
            If cadEnt IsNot Nothing Then
                entities.Add(cadEnt)
            End If
        Next

        CurrentDXFEntities = entities
        Return entities
    End Function


    ''' <summary>
    ''' Computes scale and offsets to fit CadEntities in a panel.
    ''' </summary>
    Public Sub ComputePanelTransform(entities As List(Of CadEntity),
                                     panelWidth As Integer,
                                     panelHeight As Integer,
                                     padding As Integer,
                                     ByRef scale As Double,
                                     ByRef offsetX As Double,
                                     ByRef offsetY As Double)

        Dim minX, minY, maxX, maxY As Double
        CadEntity.GetBoundingBox(entities, minX, minY, maxX, maxY)

        Dim dxfWidth = maxX - minX
        Dim dxfHeight = maxY - minY

        Dim scaleX = (panelWidth - 2 * padding) / dxfWidth
        Dim scaleY = (panelHeight - 2 * padding) / dxfHeight
        scale = Math.Min(scaleX, scaleY)

        offsetX = (panelWidth - (dxfWidth * scale)) / 2 - minX * scale
        offsetY = (panelHeight + (dxfHeight * scale)) / 2 + minY * scale
    End Sub

    ''' <summary>
    ''' Transforms DXF coordinates to panel coordinates, centered and flipped Y.
    ''' </summary>
    Public Function TransformToPanel(x As Double, y As Double,
                                     scale As Double,
                                     offsetX As Double,
                                     offsetY As Double,
                                     Optional scaleYFlip As Double = -1) As PointF
        Return New PointF(
            CSng(x * scale + offsetX),
            CSng(y * scale * scaleYFlip + offsetY)
        )
    End Function

    ''' <summary>
    ''' Compute scale and offsets to fit CadEntities in a panel.
    ''' </summary>
    Public Function ComputeScaleAndOffset(entities As List(Of CadEntity),
                                          panelWidth As Integer,
                                          panelHeight As Integer,
                                          padding As Integer,
                                          ByRef offsetX As Double,
                                          ByRef offsetY As Double,
                                          Optional ByRef scaleYFlip As Double = 1) As Double

        Dim minX, minY, maxX, maxY As Double
        CadEntity.GetBoundingBox(entities, minX, minY, maxX, maxY)

        Dim dxfWidth = Math.Max(maxX - minX, 1)
        Dim dxfHeight = Math.Max(maxY - minY, 1)

        Dim scaleX = (panelWidth - 2 * padding) / dxfWidth
        Dim scaleY = (panelHeight - 2 * padding) / dxfHeight
        Dim scale = Math.Min(scaleX, scaleY)

        scaleYFlip = -1

        offsetX = (panelWidth - (dxfWidth * scale)) / 2 - minX * scale
        offsetY = (panelHeight + (dxfHeight * scale)) / 2 + minY * scale

        Return scale
    End Function

    ''' <summary>
    ''' Transform a CadEntity to panel coordinates with Y-axis flip.
    ''' Returns a tuple of transformed start and end points (works for lines/arcs).
    ''' </summary>
    Public Function TransformEntityToPanel(entity As CadEntity,
                                           scale As Double,
                                           offsetX As Double,
                                           offsetY As Double,
                                           Optional scaleYFlip As Double = -1) As ((X As Double, Y As Double), (X As Double, Y As Double))

        Dim sp = entity.StartPt
        Dim ep = entity.EndPt

        Dim transformedStart = (
            sp.X * scale + offsetX,
            sp.Y * scale * scaleYFlip + offsetY
        )
        Dim transformedEnd = (
            ep.X * scale + offsetX,
            ep.Y * scale * scaleYFlip + offsetY
        )

        Return (transformedStart, transformedEnd)
    End Function

    ''' <summary>
    ''' Convert a single CadEntity to an ImportedEntity
    ''' </summary>
    Public Function ConvertCadToImportedEntity(cad As CadEntity) As ImportedEntity
        Dim imp As New ImportedEntity()
        imp.CadEntity = cad
        imp.CAMLayer = cad.LayerName

        ' IxMilia: ByLayer often comes through as Black
        If cad.DisplayColor.IsEmpty OrElse cad.DisplayColor = Color.Black Then
            imp.ColorMode = ColorSourceMode.ByLayer
        Else
            imp.ColorMode = ColorSourceMode.ByEntity
            imp.ExplicitColor = cad.DisplayColor
        End If

        Return imp
    End Function




    Public Function ResolveCadColorIndex(index As Integer) As Color
        ' Example simple mapping, replace with DXF palette if needed
        Select Case index
            Case 1 : Return Color.Red
            Case 2 : Return Color.Yellow
            Case 3 : Return Color.Green
        ' ... etc.
            Case 256 : Return Color.Empty  ' ByLayer
            Case Else : Return Color.Black
        End Select
    End Function


    ''' <summary>
    ''' Returns a System.Drawing.Color for a directive.
    ''' </summary>
    Public Function ResolveDirectiveColor(directive As AppData.Directive) As Color
        If directive Is Nothing Then Return Color.Black
        If directive.DisplayColor <> Color.Empty Then Return directive.DisplayColor
        Try
            Return Color.FromArgb(directive.Color)
        Catch
            Return Color.Black
        End Try
    End Function

    ''' <summary>
    ''' Convert CadEntity to DxfDrawable for panel rendering.
    ''' </summary>
    'Public Function ConvertCadToDrawable(cad As CadEntity,
    '                                 resolver As LayerColorResolver) As DxfDrawable

    '    Dim d As New DxfDrawable With {
    '    .LayerName = cad.LayerName,
    '    .Color = resolver.Resolve(cad)
    '}

    '    Select Case cad.Type

    '        Case CadEntity.EntityType.Line
    '            d.DrawableType = DxfDrawable.DrawableTypeEnum.Line
    '            d.StartPoint = New PointF(
    '            CSng(cad.StartPt.X),
    '            CSng(cad.StartPt.Y))
    '            d.EndPoint = New PointF(
    '            CSng(cad.EndPt.X),
    '            CSng(cad.EndPt.Y))

    '        Case CadEntity.EntityType.Circle
    '            d.DrawableType = DxfDrawable.DrawableTypeEnum.Circle
    '            d.Center = New PointF(
    '            CSng(cad.CircleCenter.Value.X),
    '            CSng(cad.CircleCenter.Value.Y))
    '            d.Radius = CSng(cad.CircleRadius)

    '        Case CadEntity.EntityType.Arc
    '            d.DrawableType = DxfDrawable.DrawableTypeEnum.Arc
    '            d.Center = New PointF(
    '            CSng(cad.CircleCenter.Value.X),
    '            CSng(cad.CircleCenter.Value.Y))
    '            d.Radius = CSng(cad.ArcRadius.Value)
    '            d.StartAngle = CSng(cad.ArcStartAngle)
    '            d.EndAngle = CSng(cad.ArcEndAngle)

    '        Case Else
    '            Return Nothing

    '    End Select

    '    Return d
    'End Function
    Public Function ConvertImportedToDrawable(
    imp As ImportedEntity,
    resolver As LayerColorResolver
) As DxfDrawable

        Dim cad = imp.CadEntity
        Dim d As New DxfDrawable With {
        .LayerName = imp.CAMLayer,
        .Color = resolver.Resolve(imp)
    }

        Select Case cad.Type
            Case CadEntity.EntityType.Line
                d.DrawableType = DxfDrawable.DrawableTypeEnum.Line
                d.StartPoint = New PointF(CSng(cad.StartPt.X), CSng(cad.StartPt.Y))
                d.EndPoint = New PointF(CSng(cad.EndPt.X), CSng(cad.EndPt.Y))

            Case CadEntity.EntityType.Circle
                d.DrawableType = DxfDrawable.DrawableTypeEnum.Circle
                d.Center = New PointF(CSng(cad.CircleCenter.Value.X), CSng(cad.CircleCenter.Value.Y))
                d.Radius = CSng(cad.CircleRadius)

            Case CadEntity.EntityType.Arc
                d.DrawableType = DxfDrawable.DrawableTypeEnum.Arc
                d.Center = New PointF(CSng(cad.CircleCenter.Value.X), CSng(cad.CircleCenter.Value.Y))
                d.Radius = CSng(cad.ArcRadius.Value)
                d.StartAngle = CSng(cad.ArcStartAngle)
                d.EndAngle = CSng(cad.ArcEndAngle)

            Case Else
                Return Nothing
        End Select

        Return d
    End Function


End Module
