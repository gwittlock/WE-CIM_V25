Imports FabV25_WIN8.AppData
Imports FabV25_WIN8.CadEntity
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.Drawing

Public Module DxfImport

    ' Store imported CadEntities
    Public CurrentDXFEntities As New List(Of CadEntity)

    ''' <summary>
    ''' Parse a DXF file and return a list of CadEntity objects.
    ''' Only lines, arcs, and circles are currently supported.
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

    Public Class LayerColorResolver

        Private ReadOnly _layerColors As Dictionary(Of String, Color)

        Public Sub New(layerColors As Dictionary(Of String, Color))
            _layerColors = layerColors
        End Sub

        Public Function Resolve(layerName As String) As Color

            If _layerColors Is Nothing OrElse _layerColors.Count = 0 Then
                Return Color.Gray
            End If

            ' 1. Exact layer match
            If _layerColors.ContainsKey(layerName) Then
                Return _layerColors(layerName)
            End If

            ' 2. Fallback to *default*
            If _layerColors.ContainsKey("*default*") Then
                Return _layerColors("*default*")
            End If

            ' 3. Final safety fallback
            Return Color.Gray

        End Function


    End Class


    ''' <summary>
    ''' Computes scale and center offsets to fit DXF entities in the panel.
    ''' Returns scale and sets centerX / centerY.
    ''' </summary>
    ''' <summary>
    ''' Computes the scale and center offsets to fit DXF geometry inside the panel with padding.
    ''' Handles vertical/horizontal-only DXFs.
    ''' </summary>
    Public Sub ComputePanelTransform(entities As List(Of CadEntity),
                                 panelWidth As Integer,
                                 panelHeight As Integer,
                                 padding As Integer,
                                 ByRef scale As Double,
                                 ByRef offsetX As Double,
                                 ByRef offsetY As Double)

        ' Get bounding box
        Dim minX As Double, minY As Double, maxX As Double, maxY As Double
        CadEntity.GetBoundingBox(entities, minX, minY, maxX, maxY)

        Dim dxfWidth As Double = maxX - minX
        Dim dxfHeight As Double = maxY - minY

        Dim scaleX As Double = (panelWidth - 2 * padding) / dxfWidth
        Dim scaleY As Double = (panelHeight - 2 * padding) / dxfHeight
        scale = Math.Min(scaleX, scaleY)

        ' Center the DXF
        offsetX = (panelWidth - (dxfWidth * scale)) / 2 - minX * scale
        offsetY = (panelHeight + (dxfHeight * scale)) / 2 + minY * scale
    End Sub



    ''' <summary>
    ''' Transforms DXF coordinates to panel coordinates, centered and flipped Y.
    ''' </summary>
    ''' <summary>
    ''' Converts a DXF point to panel coordinates, centered and flipped Y.
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

        ' Get bounding box for all entities
        Dim minX As Double, minY As Double, maxX As Double, maxY As Double
        CadEntity.GetBoundingBox(entities, minX, minY, maxX, maxY)

        Dim dxfWidth As Double = maxX - minX
        Dim dxfHeight As Double = maxY - minY

        ' --- Guard against degenerate geometry ---
        If dxfWidth = 0 Then dxfWidth = 1
        If dxfHeight = 0 Then dxfHeight = 1

        ' Compute scale to fit panel
        Dim scaleX As Double = (panelWidth - 2 * padding) / dxfWidth
        Dim scaleY As Double = (panelHeight - 2 * padding) / dxfHeight
        Dim scale As Double = Math.Min(scaleX, scaleY)

        ' Flip Y-axis for Windows drawing coordinates
        scaleYFlip = -1

        ' Compute centered offsets
        offsetX = (panelWidth - (dxfWidth * scale)) / 2 - minX * scale
        offsetY = (panelHeight + (dxfHeight * scale)) / 2 + minY * scale   ' Y flipped

        Return scale
    End Function

    ''' <summary>
    ''' Transform a CadEntity to panel coordinates with Y-axis flip.
    ''' Returns a tuple of transformed start and end points.
    ''' </summary>
    Public Function TransformEntityToPanel(entity As CadEntity,
                                           scale As Double,
                                           offsetX As Double,
                                           offsetY As Double,
                                           Optional scaleYFlip As Double = -1) As ((X As Double, Y As Double), (X As Double, Y As Double))
        Dim sp = entity.StartPoint
        Dim ep = entity.EndPoint

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

    ' Converts a single CadEntity to an ImportedEntity
    Public Function ConvertCadEntityToImportedEntity(cad As CadEntity, layerSetup As AppData.LayerSetup) As ImportedEntity
        Dim imp As New ImportedEntity()

        ' Assign CAM layer name
        imp.CAMLayer = cad.LayerName
        imp.DirectiveID = -1  ' Adjust if you track directive IDs

        ' Determine the color from LayerSetup
        Dim layerDirective As AppData.Directive =
    layerSetup.Layers.FirstOrDefault(Function(l) l.CADLayer.Equals(cad.LayerName, StringComparison.OrdinalIgnoreCase))

        ' Fallback to *default* if no match
        If layerDirective Is Nothing Then

            layerDirective = layerSetup.Layers.FirstOrDefault(Function(l) l.CADLayer.Equals("*default*", StringComparison.OrdinalIgnoreCase))
        End If

        If layerDirective IsNot Nothing Then
            imp.DisplayColor = ResolveDirectiveColor(layerDirective)
        Else
            imp.DisplayColor = Color.Black
        End If

        ' Map geometry type and DXF reference
        Select Case cad.CadEntityType
            Case CadEntity.EntityType.Line
                imp.GeometryType = ImportedEntity.GeometryTypeEnum.Line
                imp.Line = TryCast(cad.DxfEntity, DxfLine)

            Case CadEntity.EntityType.Arc
                imp.GeometryType = ImportedEntity.GeometryTypeEnum.Arc
                imp.Arc = TryCast(cad.DxfEntity, DxfArc)

            Case CadEntity.EntityType.Circle
                imp.GeometryType = ImportedEntity.GeometryTypeEnum.Circle
                imp.Circle = TryCast(cad.DxfEntity, DxfCircle)

            Case Else
                imp.GeometryType = ImportedEntity.GeometryTypeEnum.Unknown
        End Select
        Return imp
    End Function



    ''' <summary>
    ''' Returns a System.Drawing.Color for a directive, resolving *default* and raw integer values if needed.
    ''' </summary>
    Public Function ResolveDirectiveColor(directive As AppData.Directive) As Color
        If directive Is Nothing Then Return Color.Black

        ' If DisplayColor is already set, use it
        If directive.DisplayColor <> Color.Empty Then
            Return directive.DisplayColor
        End If

        ' Try to use the integer Color property
        Try
            Return Color.FromArgb(directive.Color)
        Catch
            ' fallback
            Return Color.Black
        End Try
    End Function

    Public Function ConvertCadToDrawable(
      cad As CadEntity,
      resolver As LayerColorResolver
  ) As DxfDrawable

        Dim d As New DxfDrawable With {
            .GeometryType = cad.CadEntityType,
            .LayerName = cad.LayerName,
            .Color = resolver.Resolve(cad.LayerName)
        }

        Select Case cad.EntityType.Line
            Case CadEntity.EntityType.Line
                d.GeometryType = DxfDrawable.DrawableTypeEnum.Line
                d.StartPoint = New PointF(CSng(cad.StartPoint.X), CSng(cad.StartPoint.Y))
                d.EndPoint = New PointF(CSng(cad.EndPoint.X), CSng(cad.EndPoint.Y))
                d.Color = cad.DisplayColor

            Case CadEntity.EntityType.Circle
                d.GeometryType = DxfDrawable.DrawableTypeEnum.Circle
                d.Center = New PointF(CSng(cad.Center.X), CSng(cad.Center.Y))
                d.Radius = CSng(cad.Radius)
                d.Color = cad.DisplayColor

            Case CadEntity.EntityType.Arc
                d.GeometryType = DxfDrawable.DrawableTypeEnum.Arc
                d.Center = New PointF(CSng(cad.Center.X), CSng(cad.Center.Y))
                d.Radius = CSng(cad.Radius)
                d.StartAngle = CSng(cad.StartAngle)
                d.EndAngle = CSng(cad.EndAngle)
                d.Color = cad.DisplayColor
        End Select


        Return d
    End Function


End Module
