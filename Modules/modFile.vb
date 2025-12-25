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
Imports FabV25_WIN8.CamBuilder



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

    Public Function FileOpen() As List(Of CamProfile)

        Using dlg As New DXFFileSelectionDialog()
            If dlg.ShowDialog() <> DialogResult.OK Then Return Nothing

            Dim selectedPath As String = dlg.SelectedDxfPath

            ' Show Import Dialog to gather context
            Using frm As New frmOpenProject(selectedPath)
                If frm.ShowDialog() <> DialogResult.OK Then Return Nothing

                ' Import DXF using the LayerSetup selected in frmOpenProject
                Dim profiles As List(Of CamProfile) =
                ImportDxfWithLayerSetupV25(selectedPath, SelectedLayerSetup, frmMain.pnlPicmodeler.BackColor)

                ' Calculate bounding box & total length for each profile
                For Each profile In profiles
                    profile.ComputeBoundingBox()
                    profile.ComputeTotalLength()
                Next

                Return profiles
            End Using
        End Using

    End Function


    Public Function DxfColorToDrawingColor(
    dxfColor As DxfColor,
    bgColor As Color
) As Color

        Dim result As Color = Color.Black

        If dxfColor IsNot Nothing Then
            Dim aci As Short = dxfColor.RawValue

            Select Case aci
                Case 1 : result = Color.Red
                Case 2 : result = Color.Yellow
                Case 3 : result = Color.Green
                Case 4 : result = Color.Cyan
                Case 5 : result = Color.Blue
                Case 6 : result = Color.Magenta
                Case 7 : result = Color.White
                Case Else
                    result = Color.Black
            End Select
        End If

        ' Background safety
        If result.ToArgb() = bgColor.ToArgb() Then
            result = If(bgColor.GetBrightness() > 0.5, Color.Black, Color.White)
        End If

        Return result
    End Function


    '-----------------------------------
    ' Map AutoCAD ColorIndex to Drawing.Color
    '-----------------------------------
    Private Function IxMiliaColorIndexToDrawingColor(index As Integer) As Color
        ' Minimal palette mapping
        Select Case index
            Case 1 : Return Color.Red
            Case 2 : Return Color.Yellow
            Case 3 : Return Color.Green
            Case 4 : Return Color.Cyan
            Case 5 : Return Color.Blue
            Case 6 : Return Color.Magenta
            Case 7 : Return Color.White
            Case Else : Return Color.Black
        End Select
    End Function

    '-----------------------------------
    ' Import DXF with optional LayerSetup
    '-----------------------------------
    'Public Function ImportDxfWithLayerSetup(selectedPath As String, layerSetup As AppData.LayerSetup, bgColor As Color) As List(Of ImportedEntity)
    '    Dim results As New List(Of ImportedEntity)

    '    ' Load DXF
    '    Dim dxf As DxfFile = DxfFile.Load(selectedPath)

    '    ' Prepare directives lookup (case-insensitive)
    '    Dim directivesLookup = layerSetup.Layers.ToDictionary(Function(d) d.CADLayer.ToLower(), Function(d) d)

    '    ' Find the "*default*" directive (if any)
    '    Dim defaultDirective As AppData.Directive = Nothing
    '    If directivesLookup.ContainsKey("*default*") Then
    '        defaultDirective = directivesLookup("*default*")
    '    End If

    '    ' Iterate DXF entities
    '    For Each ent As DxfEntity In dxf.Entities
    '        Dim imported As New ImportedEntity()

    '        ' Determine which directive applies
    '        Dim cadLayerName = If(ent.Layer, String.Empty).ToLower()
    '        Dim directive As AppData.Directive = Nothing

    '        If directivesLookup.ContainsKey(cadLayerName) Then
    '            directive = directivesLookup(cadLayerName)
    '        ElseIf defaultDirective IsNot Nothing Then
    '            directive = defaultDirective
    '        End If

    '        ' Set CAMLayer and color from directive (if available)
    '        If directive IsNot Nothing Then
    '            imported.CAMLayer = directive.CAMLayer
    '            imported.DisplayColor = AdjustColorForBackground(directive.DisplayColor, bgColor)
    '            imported.DirectiveID = directive.ID
    '        Else
    '            imported.CAMLayer = ent.Layer ' fallback to original layer name
    '            imported.DisplayColor = AdjustColorForBackground(Color.Black, bgColor)
    '            imported.DirectiveID = -1
    '        End If

    '        ' Classify geometry (Arc must be detected before Circle)
    '        If TypeOf ent Is DxfArc Then
    '            imported.GeometryType = ImportedEntity.GeometryTypeEnum.Arc
    '            imported.Arc = DirectCast(ent, DxfArc)
    '        ElseIf TypeOf ent Is DxfCircle Then
    '            imported.GeometryType = ImportedEntity.GeometryTypeEnum.Circle
    '            imported.Circle = DirectCast(ent, DxfCircle)
    '        ElseIf TypeOf ent Is DxfLine Then
    '            imported.GeometryType = ImportedEntity.GeometryTypeEnum.Line
    '            imported.Line = DirectCast(ent, DxfLine)
    '        Else
    '            imported.GeometryType = ImportedEntity.GeometryTypeEnum.Unknown
    '        End If

    '        results.Add(imported)
    '    Next

    '    Return results
    'End Function
    Public Function ImportDxfWithLayerSetupV25(selectedPath As String, layerSetup As AppData.LayerSetup, bgColor As Color) As List(Of CamProfile)

        ' 1. Load DXF
        Dim dxf As DxfFile = DxfFile.Load(selectedPath)

        ' 2. Convert DXF entities to CadEntity
        Dim cadEntities As New List(Of CadEntity)

        For Each ent As DxfEntity In dxf.Entities
            Dim cad = CadEntity.FromDxf(ent)
            If cad Is Nothing Then Continue For

            ' Assign DisplayColor from LayerSetup or fallback to *default*
            Dim layerDirective As AppData.Directive =
        layerSetup.Layers.FirstOrDefault(Function(l) l.CADLayer.Equals(cad.LayerName, StringComparison.OrdinalIgnoreCase))

            If layerDirective Is Nothing Then
                layerDirective = layerSetup.Layers.FirstOrDefault(Function(l) l.CADLayer = "*default*")
            End If

            cad.DisplayColor = If(layerDirective?.DisplayColor, Color.Black)

            cadEntities.Add(cad)
        Next


        ' 3. Build profiles using BuildProfilesV25
        Dim profiles As List(Of CamProfile) = CamBuilder.BuildProfilesV25(cadEntities, layerSetup)

        Return profiles
    End Function




    '-------------------------------------------------
    ' Helper functions for DXF entities
    '-------------------------------------------------
    Public Function GetEntityStartPoint(e As ImportedEntity) As DxfPoint
        Select Case e.GeometryType
            Case ImportedEntity.GeometryTypeEnum.Line
                Return e.Line.P1
            Case ImportedEntity.GeometryTypeEnum.Arc
                Return GetArcStartPoint(e.Arc)
            Case ImportedEntity.GeometryTypeEnum.Circle
                ' For circles, arbitrarily pick a point on circumference (0 degrees)
                Return New DxfPoint(e.Circle.Center.X + e.Circle.Radius, e.Circle.Center.Y, 0)
            Case Else
                Throw New ArgumentException("Unsupported geometry type")
        End Select
    End Function
    '''' <summary>
    '''' Returns the start and end points of an ImportedEntity in DXF coordinates.
    '''' </summary>
    '''' <param name="entity">The ImportedEntity to query.</param>
    '''' <returns>A tuple (StartPoint, EndPoint) as DxfPoint.</returns>
    'Public Function GetEntityEndpoints(entity As ImportedEntity) As (StartPoint As DxfPoint, EndPoint As DxfPoint)
    '    Select Case entity.GeometryType
    '        Case ImportedEntity.GeometryTypeEnum.Line
    '            Return (entity.Line.P1, entity.Line.P2)

    '        Case ImportedEntity.GeometryTypeEnum.Arc
    '            Return (GetArcStartPoint(entity.Arc), GetArcEndPoint(entity.Arc))

    '        Case ImportedEntity.GeometryTypeEnum.Circle
    '            ' For circles, pick an arbitrary point on circumference (0°)
    '            Dim startPt As New DxfPoint(entity.Circle.Center.X + entity.Circle.Radius, entity.Circle.Center.Y, 0)
    '            Dim endPt As DxfPoint = startPt  ' same for circle
    '            Return (startPt, endPt)

    '        Case Else
    '            Throw New ArgumentException("Unsupported geometry type")
    '    End Select
    'End Function



    ''' <summary>
    ''' Computes the start point of a DXF Arc based on its center, radius, and start angle.
    ''' </summary>

    Public Function MinDistanceBetweenEntities(e1 As ImportedEntity, e2 As ImportedEntity) As Double
        Dim p1 = GetEntityStartPoint(e1)
        Dim p2 = GetEntityStartPoint(e2)

        Dim dx = p1.X - p2.X
        Dim dy = p1.Y - p2.Y
        Dim dz = p1.Z - p2.Z

        Return Math.Sqrt(dx * dx + dy * dy + dz * dz)
    End Function


    ''' <summary>
    ''' Computes the start point of a DXF Arc based on its center, radius, and start angle.
    ''' </summary>
    Public Function GetArcStartPoint(arc As DxfArc) As DxfPoint
        Dim rad As Double = arc.StartAngle * Math.PI / 180.0
        Return New DxfPoint(arc.Center.X + arc.Radius * Math.Cos(rad),
                        arc.Center.Y + arc.Radius * Math.Sin(rad),
                        0)
    End Function

    ''' <summary>
    ''' Computes the end point of a DXF Arc based on its center, radius, and end angle.
    ''' </summary>
    Public Function GetArcEndPoint(arc As DxfArc) As DxfPoint
        Dim rad As Double = arc.EndAngle * Math.PI / 180.0
        Return New DxfPoint(arc.Center.X + arc.Radius * Math.Cos(rad),
                        arc.Center.Y + arc.Radius * Math.Sin(rad),
                        0)
    End Function
    ' Helper function to avoid drawing on the same color as the background
    Private Function AdjustColorForBackground(c As Color, bgColor As Color) As Color
        If c.ToArgb() = bgColor.ToArgb() Then
            ' Simple adjustment: invert color
            Return Color.FromArgb(c.A, 255 - c.R, 255 - c.G, 255 - c.B)
        End If
        Return c
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



    '-----------------------------------
    ' Import DXF with optional LayerSetup
    '-----------------------------------





    ' Adjust function to accept DxfColor
    '-----------------------------------
    ' Converts IxMilia DXF color to Drawing.Color
    ' Avoids background color for contrast
    '-----------------------------------


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

    '-------------------------------------------------
    ' Compute gap pairs from a list of ImportedEntity
    ' Returns: List(Of Tuple(Of ImportedEntity, ImportedEntity))
    '-------------------------------------------------
    '-------------------------------------------------
    ' Compute all pairs of entities closer than gapTolerance
    '-------------------------------------------------
    Public Function ComputeGapPairs(entities As List(Of ImportedEntity),
                                gapTolerance As Double) _
                                As List(Of Tuple(Of ImportedEntity, ImportedEntity))

        Dim gapPairs As New List(Of Tuple(Of ImportedEntity, ImportedEntity))()

        For i As Integer = 0 To entities.Count - 2
            For j As Integer = i + 1 To entities.Count - 1
                Dim e1 = entities(i)
                Dim e2 = entities(j)

                Dim dist = MinDistanceBetweenEntities(e1, e2)
                If dist <= gapTolerance Then
                    gapPairs.Add(Tuple.Create(Of ImportedEntity, ImportedEntity)(e1, e2))
                End If
            Next
        Next

        Return gapPairs
    End Function


    Public Function ComputeSignedArea(profile As List(Of CadEntity)) As Double
        ' Treat profile as polygon using start points of entities
        Dim points As New List(Of (Double, Double))()

        For Each e As CadEntity In profile
            Dim endpoints = GetEntityEndpoints(e)   ' Returns ((X,Y),(X,Y))
            points.Add(endpoints.Item1)             ' take the start point
        Next

        Dim area As Double = 0
        Dim n As Integer = points.Count

        For i As Integer = 0 To n - 1
            Dim j As Integer = (i + 1) Mod n
            area += (points(i).Item1 * points(j).Item2) - (points(j).Item1 * points(i).Item2)
        Next

        Return area / 2.0
    End Function




    '-----------------------------------------
    ' Compute 2D distance between DxfPoints
    '-----------------------------------------
    Private Function Distance(p1 As DxfPoint, p2 As DxfPoint) As Double
        Dim dx As Double = p2.X - p1.X
        Dim dy As Double = p2.Y - p1.Y
        Return Math.Sqrt(dx * dx + dy * dy)
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
