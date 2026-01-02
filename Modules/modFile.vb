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

Imports FabV25_WIN8.DrawableEntity

Imports FabV25_WIN8.DxfDrawable
Imports FabV25_WIN8.CamEntity
Imports FabV25_WIN8.CadEntity
Imports System.Runtime.CompilerServices
Imports FabV25_WIN8.AutoPunchingEngine
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models

Public Module modFile

    ' Holds the currently selected LayerSetup object
    Public _selectedLayerSetup As LayerSetup
    Public _selectedToolSetup As ToolSetup
    Private m_FileName As String
    Public _importedEntities As List(Of ImportedEntity)
    Public _drawables As List(Of DxfDrawable)



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

    'Public Function FileOpen() As List(Of CamProfile)

    'Using dlg As New DXFFileSelectionDialog()
    'If dlg.ShowDialog() <> DialogResult.OK Then Return Nothing

    '        Dim selectedPath As String = dlg.SelectedDxfPath

    '' Show Import Dialog to gather context
    'Using frm As New frmOpenProject(selectedPath)
    'If frm.ShowDialog() <> DialogResult.OK Then Return Nothing

    '            CurrentDXFEntities = ParseDXFToCadEntities(selectedPath)

    '            Dim layerColorDict As New Dictionary(Of String, Color)(StringComparer.OrdinalIgnoreCase)

    'For Each layer In _selectedLayerSetup.Layers
    '                layerColorDict(layer.CADLayer) = layer.DisplayColor
    '            Next

    'Dim resolver As New LayerColorResolver(layerColorDict)

    'Dim drawables As New List(Of DxfDrawable)

    '            _cadEntities = CurrentDXFEntities

    '            PopulateDrawables(_cadEntities, _selectedLayerSetup)

    '        End Using
    'End Using

    'End Function

    Public Function FileOpen() As List(Of Polyline2D)
        Using dlg As New DXFFileSelectionDialog()
            If dlg.ShowDialog() <> DialogResult.OK Then Exit Function
            Dim selectedPath As String = dlg.SelectedDxfPath

            Using frm As New frmOpenProject(selectedPath)
                If frm.ShowDialog() <> DialogResult.OK Then Exit Function

                RunTest()

                '        ' Parse DXF into CadEntities
                '        CurrentDXFEntities = ParseDXFToCadEntities(selectedPath)
                '        _cadEntities = CurrentDXFEntities

                '        ' Optional: setup layer colors
                '        Dim layerColorDict As New Dictionary(Of String, Color)(StringComparer.OrdinalIgnoreCase)
                '        For Each layer In _selectedLayerSetup.Layers
                '            layerColorDict(layer.CADLayer) = layer.DisplayColor
                '        Next
                '        Dim resolver As New LayerColorResolver(layerColorDict)

                '        ' Populate drawables (if needed)
                '        PopulateDrawables(_cadEntities, _selectedLayerSetup)

                '        '-----------------------------
                '        ' 2) Convert circles to Polyline2D
                '        '-----------------------------
                '        Dim circlePolylines As New List(Of (Profile As Polyline2D, LayerName As String))

                '        For Each cad In _cadEntities
                '            If cad.CadEntityType = CadEntity.EntityType.Circle Then
                '                Dim cx = CSng(cad.CircleCenter.X)
                '                Dim cy = CSng(cad.CircleCenter.Y)
                '                Dim r = CSng(cad.CircleRadius)

                '                Dim points As New List(Of PointF)
                '                Dim steps = 64 ' higher accuracy
                '                For i = 0 To steps - 1
                '                    Dim theta = 2 * Math.PI * i / steps
                '                    Dim x = cx + r * Math.Cos(theta)
                '                    Dim y = cy + r * Math.Sin(theta)
                '                    points.Add(New PointF(x, y))
                '                Next

                '                Dim pl As New Polyline2D(points)
                '                circlePolylines.Add((Profile:=pl, LayerName:=cad.LayerName))
                '            End If
                '        Next

                '        '-----------------------------
                '        ' 3) Prepare ToolSetup instance
                '        '-----------------------------
                '        Dim toolSetup As ToolSetup = _selectedToolSetup ' ensure this is assigned

                '        '-----------------------------
                '        ' 4) Run AutoPunch on all circles
                '        '-----------------------------
                '        For Each cp In circlePolylines
                '            ' Find directive for this layer
                '            Dim dir As AppData.Directive = _selectedLayerSetup.FindRuleByLayer(cp.LayerName)
                '            If dir Is Nothing Then Continue For
                '            If dir.Station = StationMode.None Then Continue For

                '            ' Convert directive → LayerPunchRule
                '            'Dim rule As LayerPunchRule = ConvertDirectiveToLayerPunchRule(dir)

                '            ' Create working profile
                '            Dim workingProfile = CreateWorkingProfile(cp.Profile, rule, "Demo") ' role = "Demo"

                '            ' Run AutoPunch
                '            Dim punchResult = AutoPunch(workingProfile, toolSetup, rule)

                '            ' Inspect result
                '            Console.WriteLine($"Circle Layer: {cp.LayerName}, PunchResult: {punchResult.ResultType}")
                '            If punchResult.ToolIDs.Any() Then
                '                Console.WriteLine($"Tools Applied: {String.Join(",", punchResult.ToolIDs)}")
                '            End If
                '        Next

                '        Console.WriteLine("Circle AutoPunch test completed.")
            End Using
        End Using

    End Function

    '' ==========================
    '' 1) Helper: Convert Directive -> LayerPunchRule
    '' ==========================
    'Private Function DirectiveToLayerPunchRule(dir As AppData.Directive) As LayerPunchRule
    '    Return New LayerPunchRule With {
    '    .CADLayer = dir.CADLayer,
    '    .Station = dir.Station,
    '    .CutDirection = dir.CutDirection,
    '    .Distance = dir.Distance,
    '    .AutoIndex = dir.AutoIndex
    '}
    'End Function

    ' ==========================
    ' 2) Public CreateWorkingProfile (accept Polyline2D)
    '' ==========================
    'Public Function CreateWorkingProfile(baseProfile As Polyline2D, directive As Directive, role As ProfileRole) As Polyline2D

    '    ' If Distance > 0, offset the profile
    '    Dim workingProfile As Polyline2D = baseProfile.Clone() ' make a copy

    '    If LayerPunchRule.Distance <> 0 Then
    '        workingProfile = workingProfile.Offset(Rule.Distance, Rule.CutSide)
    '    End If

    '    ' Resolve winding direction
    '    workingProfile.Winding = ResolveWinding(role, Rule.CutDirection)

    '    Return workingProfile
    'End Function

    '' ==========================
    '' 3) Run AutoPunch on a list of circles
    '' ==========================
    'Public Sub TestAutoPunchCircles(cadProfiles As List(Of CadProfile))
    '    ' 1) Loop through each profile
    '    For Each cp In cadProfiles

    '        ' 1a) Find directive for layer
    '        Dim dir As AppData.Directive = _selectedLayerSetup.FindRuleByLayer(cp.LayerName)
    '        If dir Is Nothing OrElse dir.Station = StationMode.None Then Continue For

    '        ' 1b) Convert to LayerPunchRule
    '        ' Dim rule As LayerPunchRule = DirectiveToLayerPunchRule(dir)

    '        ' 2) Create working profile from Polyline2D
    '        Dim workingProfile As Polyline2D = CreateWorkingProfile(cp.Profile, rule, cp.Role)

    '        ' 3) Run AutoPunch
    '        'Dim punchResult As PunchResult = AutoPunch(workingProfile, _selectedToolSetup, rule)

    '        ' 4) Inspect result
    '        Console.WriteLine($"Profile: {cp.LayerName}, PunchResult: {punchResult.ResultType}")
    '        If punchResult.ToolIDs.Any() Then
    '            Console.WriteLine($"Tools Applied: {String.Join(",", punchResult.ToolIDs)}")
    '        End If

    '    Next

    '    Console.WriteLine("Circle AutoPunch test completed.")
    'End Sub

    ' ==========================
    ' 4) ResolveWinding (make Public)
    ' ==========================
    Public Function ResolveWinding(role As AutoPunchingEngine.ProfileRole, cutDir As AutoPunchingEngine.CutDirection) As AutoPunchingEngine.WindingDirection
        If cutDir = AutoPunchingEngine.CutDirection.Auto Then
            Return If(role = AutoPunchingEngine.ProfileRole.Inside, AutoPunchingEngine.WindingDirection.CCW, AutoPunchingEngine.WindingDirection.CW)
        End If
        ' For explicit CutDirection
        Return If(cutDir = AutoPunchingEngine.CutDirection.CW, AutoPunchingEngine.WindingDirection.CW, AutoPunchingEngine.WindingDirection.CCW)
    End Function

    Public Function GetPolylinesFromCadEntities(entities As List(Of CadEntity)) As List(Of Polyline2D)
        Dim polylines As New List(Of Polyline2D)

        For Each cad In entities
            Select Case cad.CadEntityType
                Case CadEntity.EntityType.Circle
                    Dim cx = CSng(cad.CircleCenter.X)
                    Dim cy = CSng(cad.CircleCenter.Y)
                    Dim r = CSng(cad.CircleRadius)

                    ' Approximate circle as N-point polygon
                    Dim points As New List(Of PointF)
                    Dim steps = 32  ' can increase for smoother circle
                    For i = 0 To steps - 1
                        Dim theta = 2 * Math.PI * i / steps
                        Dim x = cx + r * Math.Cos(theta)
                        Dim y = cy + r * Math.Sin(theta)
                        points.Add(New PointF(x, y))
                    Next

                    Dim pl As New Polyline2D(points)
                    pl.LayerName = cad.LayerName
                    polylines.Add(pl)

                Case Else
                    ' For now ignore lines/arcs
            End Select
        Next

        Return polylines
    End Function

    'Private Function PolylineFromCadEntity(entity As CadEntity) As Polyline2D
    '    ' Only convert closed polylines
    '    If Not entity.CadEntityType = EntityType.Polyline Then
    '        Return Nothing
    '    End If

    '    ' Check if entity is closed
    '    'If Not entity.IsPolylineClosed() Then Return Nothing

    '    ' Convert DXF points to PointF
    '    Dim pts As New List(Of PointF)
    '    For Each p In entity.GetPolylinePoints()
    '        pts.Add(New PointF(p.X, p.Y))
    '    Next

    '    Return New Polyline2D(pts)
    'End Function



    ''' <summary>
    ''' Converts a list of CadEntity into DxfDrawable for drawing on panel.
    ''' </summary>
    Public Sub PopulateDrawables(cadEntities As List(Of CadEntity), layerSetup As AppData.LayerSetup)
        _drawables = New List(Of DxfDrawable)

        For Each cad In cadEntities
            Dim d As New DxfDrawable()

            ' Resolve color from LayerSetup / directive
            Dim layerDirective As AppData.Directive =
            layerSetup.Layers.FirstOrDefault(Function(l) l.CADLayer.Equals(cad.LayerName, StringComparison.OrdinalIgnoreCase))
            If layerDirective Is Nothing Then
                layerDirective = layerSetup.Layers.FirstOrDefault(Function(l) l.CADLayer.Equals("*default*", StringComparison.OrdinalIgnoreCase))
            End If
            d.Color = If(layerDirective IsNot Nothing, ResolveDirectiveColor(layerDirective), System.Drawing.Color.Black)

            ' Populate geometry
            Select Case cad.CadEntityType
                Case CadEntity.EntityType.Line
                    d.DrawableType = DxfDrawable.DrawableTypeEnum.Line
                    d.StartPoint = New PointF(CSng(cad.StartPoint.X), CSng(cad.StartPoint.Y))
                    d.EndPoint = New PointF(CSng(cad.EndPoint.X), CSng(cad.EndPoint.Y))

                Case CadEntity.EntityType.Circle
                    d.DrawableType = DxfDrawable.DrawableTypeEnum.Circle
                    d.Center = New PointF(CSng(cad.CircleCenter.X), CSng(cad.CircleCenter.Y))
                    d.Radius = CSng(cad.CircleRadius)


                Case CadEntity.EntityType.Arc

                    Dim arc = TryCast(cad.DxfEntity, IxMilia.Dxf.Entities.DxfArc)
                    If arc Is Nothing Then Exit Select

                    d.DrawableType = DxfDrawable.DrawableTypeEnum.Arc

                    ' Center MUST come from DxfArc, not CadEntity
                    d.Center = New PointF(CSng(arc.Center.X), CSng(arc.Center.Y))

                    d.Radius = CSng(arc.Radius)

                    ' Angles ONLY exist here
                    d.StartAngle = CSng(arc.StartAngle)
                    d.EndAngle = CSng(arc.EndAngle)

                    ' Optional but useful
                    d.IsClockwise = cad.ArcIsClockwise

                Case Else
                    Continue For
            End Select

            _drawables.Add(d)
        Next
    End Sub





    Public Function BuildLayerColorMap(
    layerSetup As AppData.LayerSetup
) As Dictionary(Of String, Color)

        Dim dict As New Dictionary(Of String, Color)(StringComparer.OrdinalIgnoreCase)

        For Each layer In layerSetup.Layers
            dict(layer.CADLayer) = layer.DisplayColor
        Next

        Return dict
    End Function
    'Function MapEntityToDrawable(cadEntity As Object, layers As List(Of LayerSetup)) As DrawableEntity
    '    Dim layerName As String = cadEntity.Layer
    '    Dim layerSetup = layers.FirstOrDefault(Function(l) l.Description = layerName)

    '    Dim drawable As New DrawableEntity With {
    '    .Type = DetermineType(cadEntity),
    '    .Geometry = cadEntity,
    '    .Layer = layerName,
    '    .Color = If(layerSetup IsNot Nothing, layerSetup.Color, Color.Black),
    '    .Visible = If(layerSetup IsNot Nothing, layerSetup.Visible, True)
    '}

    '    Return drawable
    'End Function



    'Private Sub DrawEntities(g As Graphics, entities As List(Of DrawableEntity))
    '    g.SmoothingMode = Drawing2D.SmoothingMode.AntiAlias

    '    For Each ent In entities
    '        If Not ent.Visible Then Continue For

    '        Dim pen As New Pen(ent.Color, 1)
    '        Select Case ent.Type
    '            Case CamEntityType.Line
    '                Dim line = CType(ent.Geometry, DxfLine)
    '                g.DrawLine(pen, line.Start.X, line.Start.Y, line.End.X, line.End.Y)

    '            Case CamEntityType.Circle
    '                Dim circle = CType(ent.Geometry, DxfCircle)
    '                g.DrawEllipse(pen, circle.Center.X - circle.Radius, circle.Center.Y - circle.Radius, circle.Radius * 2, circle.Radius * 2)

    '            Case CamEntityType.Arc
    '                Dim arc = CType(ent.Geometry, DxfArc)
    '                ' Simplified; you may need angle conversion
    '                g.DrawArc(pen, arc.BoundingRect, arc.StartAngle, arc.SweepAngle)

    '            Case CamEntityType.Point
    '                Dim pt = CType(ent.Geometry, DxfPoint)
    '                g.FillEllipse(New SolidBrush(ent.Color), pt.X - 2, pt.Y - 2, 4, 4)
    '        End Select
    '    Next
    'End Sub


    Function DetermineType(entity As Object) As CamEntityType
        ' This is simplified; adapt based on your DXF entity type
        If TypeOf entity Is DxfLine Then Return CamEntityType.Line
        If TypeOf entity Is DxfCircle Then Return CamEntityType.Circle
        If TypeOf entity Is DxfArc Then Return CamEntityType.Arc
        If TypeOf entity Is DxfPoint Then Return CamEntityType.Point
        Return CamEntityType.Line ' default fallback
    End Function




    Public Function DxfColorToDrawingColor(
    dxfColor As DxfColor,
    bgColor As Color
) As Color

        Dim result As Color = System.Drawing.Color.Black

        If dxfColor IsNot Nothing Then
            Dim aci As Short = dxfColor.RawValue

            Select Case aci
                Case 1 : result = System.Drawing.Color.Red
                Case 2 : result = System.Drawing.Color.Yellow
                Case 3 : result = System.Drawing.Color.Green
                Case 4 : result = System.Drawing.Color.Cyan
                Case 5 : result = System.Drawing.Color.Blue
                Case 6 : result = System.Drawing.Color.Magenta
                Case 7 : result = System.Drawing.Color.White
                Case Else
                    result = System.Drawing.Color.Black
            End Select
        End If

        ' Background safety
        If result.ToArgb() = bgColor.ToArgb() Then
            result = If(bgColor.GetBrightness() > 0.5, System.Drawing.Color.Black, System.Drawing.Color.White)
        End If

        Return result
    End Function


    '-----------------------------------
    ' Map AutoCAD ColorIndex to Drawing.Color
    '-----------------------------------
    Private Function IxMiliaColorIndexToDrawingColor(index As Integer) As System.Drawing.Color
        ' Minimal palette mapping
        Select Case index
            Case 1 : Return System.Drawing.Color.Red
            Case 2 : Return System.Drawing.Color.Yellow
            Case 3 : Return System.Drawing.Color.Green
            Case 4 : Return System.Drawing.Color.Cyan
            Case 5 : Return System.Drawing.Color.Blue
            Case 6 : Return System.Drawing.Color.Magenta
            Case 7 : Return System.Drawing.Color.White
            Case Else : Return System.Drawing.Color.Black
        End Select
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
            Return System.Drawing.Color.FromArgb(c.A, 255 - c.R, 255 - c.G, 255 - c.B)
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
