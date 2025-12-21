Imports System.Data.OleDb
Imports System.Drawing
Imports System.Drawing.Drawing2D
Imports System.Windows.Forms
Imports System.IO
Imports System.Linq

' IxMilia DXF imports
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports FabV25_WIN8.AppData
Imports FabV25_WIN8.ImportedEntity

Public Module modFile

    ' Holds the currently selected LayerSetup object
    Public _selectedLayerSetup As LayerSetup
    Private m_FileName As String
    Private _importedEntities As List(Of ImportedEntity)

    'Public Function FileOpen() As List(Of ImportedEntity)

    '    Using dlg As New DXFFileSelectionDialog()
    '        If dlg.ShowDialog() = DialogResult.OK Then

    '            ' Get the selected DXF file path and DxfFile object
    '            Dim selectedPath As String = dlg.SelectedDxfPath
    '            Dim loadedDxf As DxfFile = Nothing

    '            Try
    '                loadedDxf = DxfFile.Load(selectedPath)
    '            Catch ex As Exception
    '                MessageBox.Show("Failed to load DXF: " & ex.Message, "DXF Import", MessageBoxButtons.OK, MessageBoxIcon.Error)
    '                Return False
    '            End Try

    '            ' Show Import Dialog to gather context (Machine, ToolSetup, Material, etc.)
    '            Using frmOpenProject As New frmOpenProject(selectedPath)
    '                If frmOpenProject.ShowDialog() = DialogResult.OK Then

    '                    ' Process DXF file and draw in pnlPicmodeler
    '                    ' Use the loaded DxfFile from the dialog instead of re-loading
    '                    Dim importedEntities = ImportDxfWithLayerSetup(selectedPath, SelectedLayerSetup)
    '                    Return importedEntities
    '                End If
    '            End Using
    '        End If
    '    End Using

    'End Function

    Public Function FileOpen() As List(Of ImportedEntity)

        Using dlg As New DXFFileSelectionDialog()
            If dlg.ShowDialog() <> DialogResult.OK Then Return Nothing

            Dim selectedPath As String = dlg.SelectedDxfPath

            ' Show Import Dialog to gather context
            Using frm As New frmOpenProject(selectedPath)
                If frm.ShowDialog() <> DialogResult.OK Then Return Nothing

                ' Import DXF using the LayerSetup selected in frmOpenProject
                Dim dxf As DxfFile = DxfFile.Load(selectedPath)
                Dim importedEntities As List(Of ImportedEntity) =
                ImportDxfWithLayerSetup(selectedPath, SelectedLayerSetup)

                Return importedEntities
            End Using
        End Using

    End Function




    Private Function MatchDirective(entity As DxfEntity, layerSetup As AppData.LayerSetup) As Directive

        ' IMPORTANT:
        ' Directives are evaluated in the order they appear in LayerSetups.json
        ' First match wins. Only one Directive may apply.
        For Each d In layerSetup.Layers
            If DirectiveMatchesEntity(d, entity) Then
                Return d
            End If
        Next

        Return Nothing
    End Function
    Private Function DirectiveMatchesEntity(directive As AppData.Directive, entity As DxfEntity) As Boolean

        If directive Is Nothing OrElse entity Is Nothing Then Return False
        If String.IsNullOrWhiteSpace(directive.CADLayer) Then Return False
        If String.IsNullOrWhiteSpace(entity.Layer) Then Return False

        Return String.Equals(
        entity.Layer.Trim(),
        directive.CADLayer.Trim(),
        StringComparison.OrdinalIgnoreCase
    )

    End Function

    'Public Function ImportDxfWithLayerSetup(dxfPath As String, Optional layerSetup As LayerSetup = Nothing) As List(Of ImportedEntity)
    '    Dim importedEntities As New List(Of ImportedEntity)

    '    Dim dxf As DxfFile = DxfFile.Load(dxfPath)
    '    Dim defaultDirectiveID As Integer = 0 ' fallback if no directive mapping
    '    Dim color As Color

    '    For Each entity In dxf.Entities

    '        ' Determine display color
    '        color = Color.Yellow  'DxfColorToColor(entity.Color)

    '        Select Case True
    '            Case TypeOf entity Is DxfLine
    '                Dim line As DxfLine = DirectCast(entity, DxfLine)
    '                importedEntities.Add(New ImportedEntity(line, defaultDirectiveID, color, False))

    '            Case TypeOf entity Is DxfCircle
    '                Dim circle As DxfCircle = DirectCast(entity, DxfCircle)
    '                importedEntities.Add(New ImportedEntity(circle, defaultDirectiveID, color, False))

    '            Case TypeOf entity Is DxfArc
    '                Dim arc As DxfArc = DirectCast(entity, DxfArc)
    '                importedEntities.Add(New ImportedEntity(arc, defaultDirectiveID, color, False))

    '            Case TypeOf entity Is DxfPolyline
    '                Dim pline As DxfPolyline = DirectCast(entity, DxfPolyline)
    '                importedEntities.Add(New ImportedEntity(pline, defaultDirectiveID, color, False))

    '                ' Add other DXF entity types as needed
    '        End Select

    '    Next

    '    Return importedEntities
    'End Function

    Public Function ImportDxfWithLayerSetup(selectedPath As String, layerSetup As AppData.LayerSetup) As List(Of ImportedEntity)
        Dim results As New List(Of ImportedEntity)

        ' Load DXF
        Dim dxf As DxfFile = DxfFile.Load(selectedPath)

        For Each ent As DxfEntity In dxf.Entities
            Dim displayColor As Color = Color.Black

            ' Determine color
            If ent.Color IsNot Nothing Then
                displayColor = DxfColorToDrawingColor(ent.Color)
            Else
                Dim layer As DxfLayer = dxf.Layers.FirstOrDefault(Function(l) l.Name = ent.Layer)
                If layer IsNot Nothing AndAlso layer.Color IsNot Nothing Then
                    displayColor = DxfColorToDrawingColor(layer.Color)
                End If
            End If

            Dim imported As New ImportedEntity With {.DisplayColor = displayColor}

            ' Classify entity
            If TypeOf ent Is DxfArc Then
                imported.GeometryType = ImportedEntity.GeometryTypeEnum.Arc
                imported.Arc = DirectCast(ent, DxfArc)
            ElseIf TypeOf ent Is DxfCircle Then
                imported.GeometryType = ImportedEntity.GeometryTypeEnum.Circle
                imported.Circle = DirectCast(ent, DxfCircle)
            ElseIf TypeOf ent Is DxfLine Then
                imported.GeometryType = ImportedEntity.GeometryTypeEnum.Line
                imported.Line = DirectCast(ent, DxfLine)
            Else
                imported.GeometryType = ImportedEntity.GeometryTypeEnum.Unknown
            End If

            results.Add(imported)
        Next

        Return results
    End Function




    ' Adjust function to accept DxfColor
    Public Function DxfColorToDrawingColor(dxfColor As DxfColor) As Color
        If dxfColor Is Nothing OrElse dxfColor.IsByLayer OrElse dxfColor.IsByBlock Then
            Return Color.Black
        End If

        Dim rgb = dxfColor.ToRGB()
        ' Return Color.FromArgb(rgb.R, rgb.G, rgb.B)
    End Function

    ' Helper to compute key points for arc bounding box
    Private Function GetArcBoundingPoints(a As DxfArc) As List(Of DxfPoint)
        Dim points As New List(Of DxfPoint)

        ' Start and end points
        Dim startRad = a.StartAngle * Math.PI / 180
        Dim endRad = a.EndAngle * Math.PI / 180
        points.Add(New DxfPoint(a.Center.X + a.Radius * Math.Cos(startRad), a.Center.Y + a.Radius * Math.Sin(startRad), 0))
        points.Add(New DxfPoint(a.Center.X + a.Radius * Math.Cos(endRad), a.Center.Y + a.Radius * Math.Sin(endRad), 0))

        ' Check cardinal points (0, 90, 180, 270 degrees)
        Dim angles = {0, 90, 180, 270}
        For Each deg In angles
            Dim rad = deg * Math.PI / 180
            If deg >= a.StartAngle AndAlso deg <= a.EndAngle Then
                points.Add(New DxfPoint(a.Center.X + a.Radius * Math.Cos(rad), a.Center.Y + a.Radius * Math.Sin(rad), 0))
            End If
        Next

        Return points
    End Function



    Public ReadOnly Property SelectedLayerSetup As AppData.LayerSetup
        Get
            Return _selectedLayerSetup
        End Get
    End Property

    Public Sub SetFilename(ByVal Filename As String)
        m_FileName = Filename
    End Sub

    Public Function GetFilename() As String
        GetFilename = m_FileName
    End Function

    Private Function FindLayer(dxf As DxfFile, layerName As String) As DxfLayer
        Return dxf.Layers.FirstOrDefault(Function(l) l.Name = layerName)
    End Function



End Module
