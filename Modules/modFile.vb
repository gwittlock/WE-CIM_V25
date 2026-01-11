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
Imports FabV25_WIN8.dxfImporter
Imports FabV25_WIN8.DrawableEntity
Imports FabV25_WIN8.DxfImporter_IxMilia
Imports FabV25_WIN8.DxfDrawable
Imports FabV25_WIN8.CamEntity
Imports System.Runtime.CompilerServices
Imports FabV25_WIN8.AutoPunchingEngine
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.LayerDefinition
Imports FabV25_WIN8.MachineDefinition
Imports FabV25_WIN8.MaterialWrapper.Material
Imports FabV25_WIN8.WorkZone
Imports FabV25_WIN8.MachineEnvelope
Imports FabV25_WIN8.WorkZoneFactory
Imports FabV25_WIN8.modView


Public Module modFile

    ' Holds the currently selected LayerSetup object
    Public _selectedLayerSetup As layersetup
    Public _selectedToolSetup As ToolSetup
    Public _selectedMachine As Machine
    Public _selectedMaterial As MaterialSheet
    Private m_FileName As String
    Public CurrentJob As JobContext
    Public _importedEntities As List(Of ImportedEntity)
    Public _drawables As List(Of DxfDrawable)
    ' Store all loaded CAD entities
    Public _cadEntities As List(Of CadEntity)
    ' Optional: layer color resolver
    Public _layerResolver As LayerColorResolver



    Public Sub PrePareUI(sFilename As String, ByVal picModelerHandle As IntPtr)
        Dim numericWidth As Double
        Dim numericLength As Double

        ' NOTE:
        ' All geometry values are expressed in machine units.
        ' The system is unit-agnostic: inches vs metric is metadata only.
        ' No implicit unit conversions are performed anywhere in geometry or rendering.

        'When preparing the UI several things must happen before importing a file.
        'this sub does eactly wha it says.
        ''1) It prepares th UI to the corrodinate system
        '2) sets up transforms to display geometry correctly
        '3) get Machine Data in order to draw workzones and clamps
        '4) Get material data in order to draw the material.
        '5) Determines the Filetype in order to fire off the correct open state
        '   a)  a MM3 (Our file format, maybe jsut a renamed DXF)
        '   b) a NST file a dxf file renamed. (again or format for a nested sheet of parts
        '   c) a PDB file (Parts Database for nesting
        '   d) a CTG File a dxf file renamed. (Again our format for a "Custom Tool Graphic"
        '   e) a CLAMP.CTG  a dxf file renamed. The is a Custom Drawn file used to Display Clamps on the screen
        '   f) a TABLE.CTG a dxf file renamed. (A Custom Drawn file which represent things such as slats or stops on the table)
        '   g) a DXF file. The original dxf used to create CAM data files


        'Each CAM file must include:
        '    Machinetype
        '   Machine
        '   Tool Setup
        '   Material
        '   optional Layer Setup

        'The first thing we prepare is the material. Ideally this comes from the MaterialInvemtory.json file
        'Give the selected materiaid we can get the width, length, thicknes, and type of material.
        'This is enough information to draw the material. Material is just a representaion of the
        'material. It is not selectable. to get these values we do the following

        Dim matWidth = GetMaterialWidth(MaterialSheets, _SelectedMaterialID)
        Dim matLength = GetMaterialLength(MaterialSheets, _SelectedMaterialID)

        If matWidth > 0 Then
            numericWidth = Convert.ToDouble(matWidth)
            ' Perform logic with numericWidth
        End If

        If matLength > 0 Then
            numericLength = Convert.ToDouble(matLength)
            ' Perform logic with numericWidth
        End If

        Dim margin As Integer = 20

        Dim scaleX As Double = (panelWidth - 2 * margin) / matLength
        Dim scaleY As Double = (panelHeight - 2 * margin) / matWidth
        Dim scale As Double = Math.Min(scaleX, scaleY)


        Dim offsetX As Double = (panelWidth - matLength * scale) / 2
        Dim offsetY As Double = panelHeight - (matWidth * scale) - margin

        myTransform = New ViewTransform With {
        .Scale = scale,
        .OffsetX = offsetX,
        .OffsetY = offsetY
    }

        '' ---- Resolve machine from configuration ----
        Dim machineDef As MachineDefinition = MachineLoader.LoadMachineDefinition(SelectedMachineID)

        _machineDef = machineDef

        If machineDef Is Nothing Then
            Throw New Exception($"Machine with ID {SelectedMachineID} not found.")
        End If

        Dim g As Graphics = Graphics.FromHwnd(picModelerHandle)

        Dim originScreen = WorldToScreen(0, 0, myTransform, panelHeight)


        '' ---- Assign to current job ----
        CurrentJob = New JobContext()
        CurrentJob.MachineDefinition = machineDef

        RestoreAndDrawJob()

        '' Create / update job context
        CurrentJob.ToolSetupID = SelectedToolSetupID
        CurrentJob.MaterialID = SelectedMaterialID
        CurrentJob.LayerSetupID = SelectedLayerSetupID

        If CurrentJob Is Nothing Then Return
        If CurrentJob.WorkZones Is Nothing OrElse CurrentJob.WorkZones.Count = 0 Then Return

        DrawSheetAndWorkzones(g, matLength, matWidth)
        g.FillEllipse(Brushes.Red, originScreen.X - 3, originScreen.Y - 3, 6, 6)
        machineDef.SelectedToolSetupID = SelectedToolSetupID
        machineDef.SelectedLayerSetupID = SelectedLayerSetupID
        machineDef.SelectedMaterialID = SelectedMaterialID

    End Sub

    Public Function FileOpen()
        Dim importer As New DxfImporter_IxMilia()

        Using dlg As New DXFFileSelectionDialog()
            If dlg.ShowDialog() <> DialogResult.OK Then Return Nothing
            Dim selectedPath As String = dlg.SelectedDxfPath

            Using frm As New frmOpenProject(selectedPath)
                If frm.ShowDialog() <> DialogResult.OK Then Return Nothing

                PrePareUI(selectedPath, _pnlPicModelerHandle)

                Dim g As Graphics = Graphics.FromHwnd(_pnlPicModelerHandle)

                _cadEntities = ImportDxf(selectedPath)

                Dim bounds = HelperFunctions.GetCadBounds(_cadEntities)

                NormalizeCadToSheet(_cadEntities)


                Dim matWidth = GetMaterialWidth(MaterialSheets, _SelectedMaterialID)
                Dim matLength = GetMaterialLength(MaterialSheets, _SelectedMaterialID)


                RedrawAll()

            End Using
        End Using
    End Function

    Public Function ResolveWinding(role As AutoPunchingEngine.ProfileRole, cutDir As AutoPunchingEngine.CutDirection) As AutoPunchingEngine.WindingDirection
        If cutDir = AutoPunchingEngine.CutDirection.Auto Then
            Return If(role = AutoPunchingEngine.ProfileRole.Inside, AutoPunchingEngine.WindingDirection.CCW, AutoPunchingEngine.WindingDirection.CW)
        End If
        ' For explicit CutDirection
        Return If(cutDir = AutoPunchingEngine.CutDirection.CW, AutoPunchingEngine.WindingDirection.CW, AutoPunchingEngine.WindingDirection.CCW)
    End Function


    Public Function BuildLayerColorMap(
    layerSetup As AppData.layersetup
) As Dictionary(Of String, Color)

        Dim dict As New Dictionary(Of String, Color)(StringComparer.OrdinalIgnoreCase)

        For Each layer In layerSetup.Layers
            dict(layer.CADLayer) = layer.DisplayColor
        Next

        Return dict
    End Function

    Function DetermineType(entity As Object) As CamEntityType
        ' This is simplified; adapt based on your DXF entity type
        If TypeOf entity Is DxfLine Then Return CamEntityType.Line
        If TypeOf entity Is DxfCircle Then Return CamEntityType.Circle
        If TypeOf entity Is DxfArc Then Return CamEntityType.Arc
        If TypeOf entity Is DxfPoint Then Return CamEntityType.Point
        Return CamEntityType.Line ' default fallback
    End Function

    Public Function DxfColorToDrawingColor(dxfColor As DxfColor, bgColor As Color) As Color

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

    Private Function MatchDirective(entity As DxfEntity, layerSetup As AppData.layersetup) As Directive

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

    'Public Function ComputeSignedArea(profile As List(Of CadEntity)) As Double
    '    ' Treat profile as polygon using start points of entities
    '    Dim points As New List(Of (Double, Double))()

    '    For Each e As CadEntity In profile
    '        Dim endpoints = GetEntityEndpoints(e)   ' Returns ((X,Y),(X,Y))
    '        points.Add(endpoints.Item1)             ' take the start point
    '    Next

    '    Dim area As Double = 0
    '    Dim n As Integer = points.Count

    '    For i As Integer = 0 To n - 1
    '        Dim j As Integer = (i + 1) Mod n
    '        area += (points(i).Item1 * points(j).Item2) - (points(j).Item1 * points(i).Item2)
    '    Next

    '    Return area / 2.0
    'End Function

    '-----------------------------------------
    ' Compute 2D distance between DxfPoints
    '-----------------------------------------
    Private Function Distance(p1 As DxfPoint, p2 As DxfPoint) As Double
        Dim dx As Double = p2.X - p1.X
        Dim dy As Double = p2.Y - p1.Y
        Return Math.Sqrt(dx * dx + dy * dy)
    End Function

    Public ReadOnly Property SelectedLayerSetup As AppData.layersetup
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
