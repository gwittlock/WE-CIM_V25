
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
Imports FabV25_WIN8.ToolShapes


Public Module modFile

    Public loadTimer As Stopwatch
    Public _hotSpotManager As HotSpotManager

    Public _sceneReady As Boolean = False
    Public _visibleEntities As List(Of CadEntity) = Nothing

    ' Holds the currently selected LayerSetup object
    Public _selectedLayerSetup As layersetup
    Public _selectedToolSetup As ToolSetup
    Public _selectedMachine As Machine
    Public _selectedMaterial As MaterialSheet
    Private m_FileName As String

    Public CurrentJob As JobContext
    Public CurrentProductionJob As Job
    Public _importedEntities As List(Of ImportedEntity)

    ' Store all loaded CAD entities
    Public _cadEntities As List(Of CadEntity)

    ' Optional: layer color resolver
    Public _layerResolver As LayerColorResolver

    Public _DoPrepareUI As Boolean = False
    Public _DoDrawUI As Boolean = False

    Public PendingCustomerName As String
    Public PendingOrderPO As String
    Public PendingDueDate As Object


    Public Sub PrePareUI(sFilename As String)


        '============================================================
        ' Material Dimensions
        '============================================================
        matWidth = GetMaterialWidth(
        MaterialSheets,
        _SelectedMaterialID)

        matLength = GetMaterialLength(
        MaterialSheets,
        _SelectedMaterialID)

        '============================================================
        ' Calculate Initial View
        '============================================================
        Dim margin As Integer = 20

        Dim scaleX As Double =
        (panelWidth - 2 * margin) / matLength

        Dim scaleY As Double =
        (panelHeight - 2 * margin) / matWidth

        Dim scale As Double =
        Math.Min(scaleX, scaleY)

        Dim offsetX As Double =
        (panelWidth - matLength * scale) / 2

        Dim offsetY As Double =
        panelHeight - (matWidth * scale) - margin

        myTransform = New ViewTransform With {
        .Scale = scale,
        .OffsetX = offsetX,
        .OffsetY = offsetY
    }

        '============================================================
        ' Create Production Job
        '============================================================
        CurrentProductionJob = New Job With {.MachineID = SelectedMachineID, .ToolSetupID = SelectedToolSetupID, .LayerSetupID = SelectedLayerSetupID, .MaterialTypeID = _selectedMaterial.TypeID}

        '============================================================
        ' Transfer pending Job information
        '============================================================
        CurrentProductionJob.UDPs("Customer") = PendingCustomerName
        CurrentProductionJob.UDPs("PONumber") = PendingOrderPO
        If PendingDueDate Is Nothing OrElse
    PendingDueDate Is DBNull.Value Then

            CurrentProductionJob.UDPs("DueDate") = Nothing

        Else
            CurrentProductionJob.UDPs("DueDate") =
        Convert.ToDateTime(PendingDueDate)
        End If
        '============================================================
        ' Create Production Sheet
        '============================================================
        Dim productionSheet As New JobSheet With {.MaterialSheetID = _SelectedMaterialID, .Length = matLength, .Width = matWidth}

        CurrentProductionJob.JobSheets.Add(productionSheet)

        '============================================================
        ' Load Machine Definition
        '===========================================================
        Dim machineDef As MachineDefinition = MachineLoader.LoadMachineDefinition(CurrentProductionJob.MachineID)

        If machineDef Is Nothing Then

            MessageBox.Show("Unable to load the selected machine definition.", "Machine Definition", MessageBoxButtons.OK, MessageBoxIcon.Warning)

            Return

        End If

        '============================================================
        ' Create Current Job Context
        '============================================================
        CurrentJob = New JobContext()

        CurrentJob.MachineDefinition = machineDef

        CurrentJob.ToolSetupID = SelectedToolSetupID
        CurrentJob.MaterialID = SelectedMaterialID
        CurrentJob.LayerSetupID = SelectedLayerSetupID

        RestoreAndDrawJob()

        productionSheet.WorkZones = CurrentJob.WorkZones

        productionSheet.ManufacturingModel.WorkZones = productionSheet.WorkZones

        '============================================================
        ' Set Selected Machine Configuration
        '============================================================
        machineDef.SelectedToolSetupID = SelectedToolSetupID

        machineDef.SelectedLayerSetupID = SelectedLayerSetupID

        machineDef.SelectedMaterialID = SelectedMaterialID
        'IDxfImporter
        '============================================================
        ' Import DXF
        '============================================================
        _cadEntities = DxfImport.ImportDxf(_selectedDxfPath)

        '============================================================
        ' Initialize Hot Spot Manager
        '============================================================

        _visibleEntities = _cadEntities.ToList()

        _sceneReady = True

        ''============================================================
        '' Get CAD Bounds
        ''============================================================
        'Dim bounds = HelperFunctions.GetCadBounds(_cadEntities)

        '============================================================
        ' Normalize CAD Geometry to Sheet
        '============================================================
        NormalizeCadToSheet(_cadEntities, matLength, matWidth)


        '============================================================
        ' TEST - Native Part Geometry Conversion
        '============================================================
        Dim importedPart As Part = PartGeometryBuilder.CreatePartGeometry(_cadEntities, _selectedLayerSetup)


        ProcessRoundPunchAutoTool(importedPart, machineDef)

        '============================================================
        ' Preferred Horizontal / Vertical AutoTool
        '============================================================

        Dim preferredContext As PreferredAutoToolContext = InitializePreferredAutoTools()

        If _selectedLayerSetup Is Nothing Then
            Exit Sub
        End If
        '============================================================
        ' Preferred AutoTool - Horizontal Line Eligibility
        '============================================================

        ProcessHorizontalAutoTool(importedPart, preferredContext)

        ProcessVerticalAutoTool(importedPart, preferredContext)
        '============================================================
        ' Create Part
        '============================================================
        importedPart.ID = 0

        importedPart.Description = Path.GetFileNameWithoutExtension(_selectedDxfPath)

        productionSheet.Parts.Add(importedPart)

        '============================================================
        ' Register Part and Manufacturing Geometry in MM3
        '============================================================
        ' productionSheet.ManufacturingModel.PartGeometry.AddRange(importedPart.Geometry)

        productionSheet.ManufacturingModel.ManufacturingGeometry.AddRange(importedPart.Features)

        Debug.WriteLine("FEATURE COUNT BEFORE REGISTRATION = " &
                importedPart.Features.Count)

        For Each feature As MachiningFeature In importedPart.Features

            frmMain.RegisterManufacturingFeature(feature)

        Next


        frmMain.InitializeEntityList()
        frmMain.RefreshEntityList()

        ' MyStopWatch.Sw.Stop()
        ' MessageBox.Show("Elapsed time: " & MyStopWatch.Sw.Elapsed.ToString())
        'frmMain.dockPanelEntityList.Show()
    End Sub


    Private Sub ProcessRoundPunchAutoTool(
    importedPart As Part,
    machineDef As MachineDefinition)



        '============================================================
        ' TEST - Automatic Exact Round Punch Tooling
        '============================================================
        For Each circle As GeoCircle In importedPart.Geometry.OfType(Of GeoCircle)()


            Dim camLayer As Layer = TryCast(circle.Association, Layer)

            If camLayer Is Nothing Then Continue For

            Dim directive = PartGeometryBuilder.FindDirectiveByCAMLayer(camLayer.Name, _selectedLayerSetup)

            If directive Is Nothing Then Continue For

            Dim selectedStation As String = directive.Station.Trim()

            Dim memberID As Integer = 0

            Dim toolDiameter As Double = 0


            If String.Equals(selectedStation, "None", StringComparison.OrdinalIgnoreCase) Then

                Continue For

            End If

            If Not String.Equals(selectedStation, "**Auto**", StringComparison.OrdinalIgnoreCase) Then

                Dim ncNumber As Integer

                If Not Integer.TryParse(selectedStation, ncNumber) Then
                    Continue For
                End If

                Dim stationRecord = AppData.StationsTable.Rows.
                    FirstOrDefault(
                        Function(s) CInt(s("MachineID")) = machineDef.ID AndAlso
                                    CInt(s("NCCodeNumber")) = ncNumber)

                If stationRecord Is Nothing Then
                    Continue For
                End If

                Dim stationID As Integer = CInt(stationRecord("ID"))

                Dim assignedMember = AppData.ToolSetupMembers.
                    FirstOrDefault(
                        Function(m) m.ToolSetupID = CurrentJob.ToolSetupID AndAlso
                                    m.StationID = stationID)

                If assignedMember Is Nothing Then
                    Continue For
                End If

                memberID = assignedMember.ID

                Dim assignedTool As Tool =
                    AppData.ToolCrib.FirstOrDefault(
                        Function(t) t.ID = assignedMember.ToolID)

                If assignedTool Is Nothing OrElse
                   assignedTool.Attributes Is Nothing Then
                    Continue For
                End If

                Dim diameterValue As Object = Nothing

                If Not assignedTool.Attributes.TryGetValue("DIAMETER", diameterValue) Then
                    Continue For
                End If

                toolDiameter = Convert.ToDouble(diameterValue, Globalization.CultureInfo.InvariantCulture)

            End If

            If String.Equals(
    selectedStation,
    "**Auto**",
    StringComparison.OrdinalIgnoreCase) Then

                memberID = AutoPunchingEngine.FindExactRoundPunch(
        circle,
        CurrentJob.ToolSetupID,
        machineDef)

            End If


            ' Temporary safeguard for the specific-station test.
            ' Station 104 has a 1.000-inch round punch.
            ' Only create single-hit holes for matching circles.

            If Not String.Equals(
    selectedStation,
    "**Auto**",
    StringComparison.OrdinalIgnoreCase) Then

                If Math.Abs((circle.Radius * 2.0) - toolDiameter) > 0.0001 Then

                    Continue For

                End If

            End If


            If memberID = 0 Then Continue For
            If circle.Center Is Nothing Then Continue For

            Dim hole As New HoleFeature With {
        .ID = importedPart.GetNextFeatureID(),
        .FeatureType = MachiningFeatureType.Hole,
        .Description = "Punch Hole",
        .X = circle.Center.X,
        .Y = circle.Center.Y,
        .Angle = 0,
        .SourceGeometryID = circle.ID,
        .ToolSetupMemberID = memberID}

            importedPart.Features.Add(hole)

        Next
    End Sub




    Private Sub ProcessHorizontalAutoTool(importedPart As Part, preferredContext As PreferredAutoToolContext)

        Dim horizontalMember As ToolSetupMember = preferredContext.HorizontalMember

        Dim horizontalTool As Tool = preferredContext.HorizontalTool

        Dim horizontalAngleValid As Boolean = preferredContext.HorizontalAngleValid

        Dim closedProfileCount As Integer = 0
        Dim horizontalLineCount As Integer = 0
        Dim eligibleHorizontalCount As Integer = 0

        For Each profile As GeoProfile In importedPart.GeoProfiles

            If Not profile.IsClosed Then Continue For

            closedProfileCount += 1

            Dim isInsideProfile As Boolean = False

            Dim innerPath As ToolShapes.IPath2D =
    PartGeometryBuilder.ConvertGeoProfileToPath(profile)

            If innerPath IsNot Nothing AndAlso innerPath.IsClosed Then

                For Each outerProfile As GeoProfile In importedPart.GeoProfiles

                    If outerProfile Is profile OrElse Not outerProfile.IsClosed Then
                        Continue For
                    End If

                    Dim outerPath As ToolShapes.IPath2D =
            PartGeometryBuilder.ConvertGeoProfileToPath(outerProfile)

                    If outerPath Is Nothing OrElse Not outerPath.IsClosed Then
                        Continue For
                    End If

                    If ToolShapes.Geometry2D.ClassifyProfileContainment(
            innerPath, outerPath) =
            ToolShapes.ProfileContainmentResult.Inside Then

                        isInsideProfile = True
                        Exit For

                    End If

                Next

            End If
            Dim reverseProfileDirection As Boolean =
    PartGeometryBuilder.ShouldReverseProfileDirection(
        profile,
        isInsideProfile)

            For Each segment As GeoProfileSegment In profile.Segments

                Dim line As GeoLine = TryCast(segment.Entity, GeoLine)

                If line Is Nothing Then Continue For

                'Only process geometry configured for AutoTool
                Dim camLayer As Layer =
                    TryCast(line.Association, Layer)

                If camLayer Is Nothing Then Continue For

                Dim directive =
                    PartGeometryBuilder.FindDirectiveByCAMLayer(
                        camLayer.Name,
                        _selectedLayerSetup)

                If directive Is Nothing Then Continue For

                If Not String.Equals(
                    directive.Station.Trim(),
                    "**Auto**",
                    StringComparison.OrdinalIgnoreCase) Then

                    Continue For

                End If



                If line.StartPoint Is Nothing OrElse
           line.EndPoint Is Nothing Then Continue For

                Dim dx As Double =
            line.EndPoint.X - line.StartPoint.X

                Dim dy As Double =
            line.EndPoint.Y - line.StartPoint.Y

                If Math.Abs(dy) > 0.000001 Then Continue For

                horizontalLineCount += 1

                Dim lineLength As Double = Math.Abs(dx)

                If horizontalAngleValid AndAlso
     horizontalTool IsNot Nothing AndAlso
     lineLength >=
         1.5 * Convert.ToDouble(
             horizontalTool.Attributes("LENGTH"),
             Globalization.CultureInfo.InvariantCulture) Then
                    eligibleHorizontalCount += 1

                    Debug.WriteLine(
                        "AutoTool Directive: " &
                        "CAMLayer=" & camLayer.Name &
                        " CutSide=" & directive.CutSide.ToString() &
                        " CutDirection=" & directive.CutDirection.ToString() &
                        " SegmentReversed=" & segment.Reversed.ToString())

                    Debug.WriteLine(
                "Eligible Horizontal Line: " &
                "Length=" & lineLength.ToString("F3") &
                " Start=(" &
                line.StartPoint.X & "," &
                line.StartPoint.Y & ")" &
                " End=(" &
                line.EndPoint.X & "," &
                line.EndPoint.Y & ")")

                    Dim halfToolLength As Double =
                        Convert.ToDouble(
                            horizontalTool.Attributes("LENGTH"),
                            Globalization.CultureInfo.InvariantCulture) / 2.0

                    Dim directionX As Double = Math.Sign(dx)

                    If segment.Reversed Xor reverseProfileDirection Then
                        directionX = -directionX
                    End If

                    Dim effectiveStartX As Double = line.StartPoint.X
                    Dim effectiveEndX As Double = line.EndPoint.X

                    If segment.Reversed Xor reverseProfileDirection Then
                        effectiveStartX = line.EndPoint.X
                        effectiveEndX = line.StartPoint.X
                    End If

                    Dim toolStartX As Double = effectiveStartX + directionX * halfToolLength

                    Dim toolEndX As Double = effectiveEndX - directionX * halfToolLength
                    Debug.WriteLine(
                                "Horizontal Tool Center: StartX=" &
                                toolStartX.ToString("F3") &
                                " EndX=" &
                                toolEndX.ToString("F3") &
                                " Y=" &
                                line.StartPoint.Y.ToString("F3"))

                    Dim halfToolWidth As Double =
                            Convert.ToDouble(
                                horizontalTool.Attributes("WIDTH"),
                                Globalization.CultureInfo.InvariantCulture) / 2.0

                    Dim toolCenterY As Double = line.StartPoint.Y

                    Select Case directive.CutSide.ToString()

                        Case "Left"
                            toolCenterY += directionX * halfToolWidth

                        Case "Right"
                            toolCenterY -= directionX * halfToolWidth

                        Case "None"
        ' No perpendicular offset.

                        Case "Auto"
                            ' Material-side calculation comes later.
                            Debug.WriteLine("Auto CutSide: Not yet implemented")
                            Continue For

                    End Select

                    Debug.WriteLine(
                        "Offset Tool Center: Start=(" &
                        toolStartX.ToString("F3") & "," &
                        toolCenterY.ToString("F3") &
                        ") End=(" &
                        toolEndX.ToString("F3") & "," &
                        toolCenterY.ToString("F3") & ")")


                    Dim toolPath As New ToolShapes.Path2D()

                    toolPath.Add(
                            New ToolShapes.LineSegment2D(
                                New PointF(
                                    CSng(toolStartX),
                                    CSng(toolCenterY)),
                                New PointF(
                                    CSng(toolEndX),
                                    CSng(toolCenterY))))

                    Dim contour As New ContourFeature With {
                        .ID = importedPart.GetNextFeatureID(),
                        .Description = "Preferred Horizontal AutoTool",
                        .ToolSetupMemberID = horizontalMember.ID,
                        .ToolIndexAngle = 0.0,
                        .Path = toolPath,
                        .IsLogicallyClosed = False
                    }

                    importedPart.Features.Add(contour)

                    Debug.WriteLine(
                            "Created AutoTool Contour ID=" & contour.ID &
                            " ToolMemberID=" & contour.ToolSetupMemberID)




                    Debug.WriteLine(
                        "AutoTool Profile ID: " & profile.ID &
                        " SegmentReversed: " & segment.Reversed)
                End If

            Next

        Next

        Debug.WriteLine("Closed Profiles: " & closedProfileCount)
        Debug.WriteLine("Horizontal Lines: " & horizontalLineCount)
        Debug.WriteLine("Eligible Horizontal Lines: " & eligibleHorizontalCount)

    End Sub

    Private Sub ProcessVerticalAutoTool(importedPart As Part, preferredContext As PreferredAutoToolContext)

        Dim verticalMember As ToolSetupMember =
    preferredContext.VerticalMember

        Dim verticalTool As Tool =
            preferredContext.VerticalTool

        Dim verticalAngleValid As Boolean =
            preferredContext.VerticalAngleValid


        Dim verticalLineCount As Integer = 0
        Dim eligibleVerticalCount As Integer = 0

        For Each profile As GeoProfile In importedPart.GeoProfiles

            If Not profile.IsClosed Then Continue For

            For Each segment As GeoProfileSegment In profile.Segments

                Dim line As GeoLine = TryCast(segment.Entity, GeoLine)
                If line Is Nothing Then Continue For

                Dim camLayer As Layer =
    TryCast(line.Association, Layer)

                If camLayer Is Nothing Then Continue For

                Dim directive =
    PartGeometryBuilder.FindDirectiveByCAMLayer(
        camLayer.Name,
        _selectedLayerSetup)

                If directive Is Nothing Then Continue For

                If Not String.Equals(
    directive.Station.Trim(),
    "**Auto**",
    StringComparison.OrdinalIgnoreCase) Then

                    Continue For

                End If


                Dim dx As Double = line.EndPoint.X - line.StartPoint.X
                Dim dy As Double = line.EndPoint.Y - line.StartPoint.Y

                If Math.Abs(dx) > 0.000001 Then Continue For

                verticalLineCount += 1

                Dim lineLength As Double = Math.Abs(dy)

                If verticalAngleValid AndAlso
           verticalTool IsNot Nothing AndAlso
           lineLength >= 1.5 * Convert.ToDouble(
               verticalTool.Attributes("LENGTH"),
               Globalization.CultureInfo.InvariantCulture) Then

                    eligibleVerticalCount += 1

                    Dim halfToolLength As Double =
    Convert.ToDouble(
        verticalTool.Attributes("LENGTH"),
        Globalization.CultureInfo.InvariantCulture) / 2.0

                    Dim directionY As Double = Math.Sign(dy)

                    Dim effectiveStartY As Double = line.StartPoint.Y
                    Dim effectiveEndY As Double = line.EndPoint.Y

                    Dim isInsideProfile As Boolean = False

                    Dim innerPath As ToolShapes.IPath2D =
    PartGeometryBuilder.ConvertGeoProfileToPath(profile)

                    If innerPath IsNot Nothing AndAlso innerPath.IsClosed Then

                        For Each outerProfile As GeoProfile In importedPart.GeoProfiles

                            If outerProfile Is profile OrElse Not outerProfile.IsClosed Then
                                Continue For
                            End If

                            Dim outerPath As ToolShapes.IPath2D =
            PartGeometryBuilder.ConvertGeoProfileToPath(outerProfile)

                            If outerPath Is Nothing OrElse Not outerPath.IsClosed Then
                                Continue For
                            End If

                            If ToolShapes.Geometry2D.ClassifyProfileContainment(
            innerPath, outerPath) =
            ToolShapes.ProfileContainmentResult.Inside Then

                                isInsideProfile = True
                                Exit For

                            End If

                        Next

                    End If

                    Dim reverseProfileDirection As Boolean =
    PartGeometryBuilder.ShouldReverseProfileDirection(
        profile,
        isInsideProfile)

                    If segment.Reversed Xor reverseProfileDirection Then
                        directionY = -directionY
                        effectiveStartY = line.EndPoint.Y
                        effectiveEndY = line.StartPoint.Y
                    End If

                    Dim toolStartY As Double =
    effectiveStartY + directionY * halfToolLength

                    Dim toolEndY As Double =
    effectiveEndY - directionY * halfToolLength

                    Debug.WriteLine(
                        "Vertical Tool Center: StartY=" &
                        toolStartY.ToString("F3") &
                        " EndY=" &
                        toolEndY.ToString("F3") &
                        " X=" &
                        line.StartPoint.X.ToString("F3"))

                    Dim halfToolWidth As Double =
    Convert.ToDouble(
        verticalTool.Attributes("WIDTH"),
        Globalization.CultureInfo.InvariantCulture) / 2.0

                    Dim toolCenterX As Double = line.StartPoint.X


                    Select Case directive.CutSide.ToString()

                        Case "Left"
                            toolCenterX -= directionY * halfToolWidth

                        Case "Right"
                            toolCenterX += directionY * halfToolWidth

                        Case "None"
        ' No perpendicular offset.

                        Case "Auto"
                            Debug.WriteLine("Vertical Auto CutSide: Not yet implemented")
                            Continue For

                    End Select

                    Debug.WriteLine(
                        "Vertical Offset Tool Center: Start=(" &
                        toolCenterX.ToString("F3") & "," &
                        toolStartY.ToString("F3") &
                        ") End=(" &
                        toolCenterX.ToString("F3") & "," &
                        toolEndY.ToString("F3") & ")")


                    Dim toolPath As New ToolShapes.Path2D()

                    toolPath.Add(
    New ToolShapes.LineSegment2D(
        New PointF(
            CSng(toolCenterX),
            CSng(toolStartY)),
        New PointF(
            CSng(toolCenterX),
            CSng(toolEndY))))

                    Dim contour As New ContourFeature With {
                    .ID = importedPart.GetNextFeatureID(),
                    .Description = "Preferred Vertical AutoTool",
                    .ToolSetupMemberID = verticalMember.ID,
                    .ToolIndexAngle = 90.0,
                    .Path = toolPath,
                    .IsLogicallyClosed = False
                }

                    importedPart.Features.Add(contour)

                    Debug.WriteLine(
    "Created Vertical AutoTool Contour ID=" & contour.ID &
    " ToolMemberID=" & contour.ToolSetupMemberID)



                    Debug.WriteLine(
                "Eligible Vertical Line: Length=" &
                lineLength.ToString("F3") &
                " Start=(" & line.StartPoint.X & "," & line.StartPoint.Y & ")" &
                " End=(" & line.EndPoint.X & "," & line.EndPoint.Y & ")")

                End If

            Next

        Next

        Debug.WriteLine("Vertical Lines: " & verticalLineCount)
        Debug.WriteLine("Eligible Vertical Lines: " & eligibleVerticalCount)

    End Sub


    Private Class PreferredAutoToolContext

        Public Property HorizontalMember As ToolSetupMember
        Public Property VerticalMember As ToolSetupMember

        Public Property HorizontalTool As Tool
        Public Property VerticalTool As Tool

        Public Property HorizontalShape As ToolShape
        Public Property VerticalShape As ToolShape

        Public Property HorizontalAngleValid As Boolean
        Public Property VerticalAngleValid As Boolean

    End Class

    Private Function InitializePreferredAutoTools() As PreferredAutoToolContext

        Dim context As New PreferredAutoToolContext()

        Dim activeToolSetup As ToolSetup =
    AppData.ToolSetups.FirstOrDefault(
        Function(ts) ts.ID = CurrentJob.ToolSetupID)

        If activeToolSetup Is Nothing Then
            Return context
        End If


        context.HorizontalMember =
    AppData.ToolSetupMembers.FirstOrDefault(
        Function(m) m.ID =
            activeToolSetup.PreferredHorizontalCuttingToolID)

        Dim horizontalStation = Nothing

        If context.HorizontalMember IsNot Nothing Then

            horizontalStation =
        AppData.StationsTable.Rows.FirstOrDefault(
            Function(s) CInt(s("ID")) =
                context.HorizontalMember.StationID)

        End If


        context.VerticalMember =
    AppData.ToolSetupMembers.FirstOrDefault(
        Function(m) m.ID =
            activeToolSetup.PreferredVerticalCuttingToolID)

        Dim verticalStation = Nothing

        If context.VerticalMember IsNot Nothing Then

            verticalStation =
        AppData.StationsTable.Rows.FirstOrDefault(
            Function(s) CInt(s("ID")) =
                context.VerticalMember.StationID)

        End If

        Dim horizontalAutoIndex As Boolean = False

        If horizontalStation IsNot Nothing Then
            horizontalAutoIndex =
        Not IsDBNull(horizontalStation("AutoIndex")) AndAlso
        Convert.ToBoolean(horizontalStation("AutoIndex"))
        End If

        context.HorizontalAngleValid =
    context.HorizontalMember IsNot Nothing AndAlso
    (horizontalAutoIndex OrElse
     Math.Abs(context.HorizontalMember.IndexAngle - 0.0) < 0.001 OrElse
     Math.Abs(context.HorizontalMember.IndexAngle - 180.0) < 0.001)


        Dim verticalAutoIndex As Boolean = False

        If verticalStation IsNot Nothing Then
            verticalAutoIndex =
        Not IsDBNull(verticalStation("AutoIndex")) AndAlso
        Convert.ToBoolean(verticalStation("AutoIndex"))
        End If

        context.VerticalAngleValid =
    context.VerticalMember IsNot Nothing AndAlso
    (verticalAutoIndex OrElse
     Math.Abs(context.VerticalMember.IndexAngle - 90.0) < 0.001 OrElse
     Math.Abs(context.VerticalMember.IndexAngle - 270.0) < 0.001)


        If context.HorizontalAngleValid Then

            ToolShapeResolver.TryResolve(
        context.HorizontalMember.ID,
        context.HorizontalTool,
        context.HorizontalShape)

        End If

        If context.VerticalAngleValid Then

            ToolShapeResolver.TryResolve(
        context.VerticalMember.ID,
        context.VerticalTool,
        context.VerticalShape)

        End If


        Debug.WriteLine(
    "Preferred AutoTool Context | " &
    "HorizontalMember=" &
    If(context.HorizontalMember Is Nothing,
       "None",
       context.HorizontalMember?.ID.ToString()) &
    " | VerticalMember=" &
    If(context.VerticalMember Is Nothing,
       "None",
       context.VerticalMember?.ID.ToString()) &
    " | HorizontalResolved=" &
    (context.HorizontalTool IsNot Nothing).ToString() &
    " | VerticalResolved=" &
    (context.VerticalTool IsNot Nothing).ToString())

        Return context

    End Function
    Public Function FileOpen()

        frmMain.Cursor = Cursors.WaitCursor

        Try

            Debug.WriteLine("Right before  New DXFFileSelectionDialog()")

            Using dlg As New DXFFileSelectionDialog()

                If dlg.ShowDialog() <> DialogResult.OK Then
                    Return Nothing
                End If


                Select Case dlg.SelectedFileType

                    Case "MM3"

                        Try
                            Dim nativeFile As MM3NativeFile =
            MM3NativeFileService.ReadNativeFile(dlg.SelectedFilePath)

                            Dim restoredJob As Job =
            MM3NativeFileService.RestoreJobInformation(nativeFile)

                            Dim restoredContext As JobContext =
            MM3NativeFileService.RestoreManufacturingEnvironment(nativeFile)

                            Dim restoredSheet As JobSheet =
            MM3NativeFileService.RestoreJobSheet(nativeFile)

                            restoredJob.JobSheets.Add(restoredSheet)

                            restoredContext.WorkZones = restoredSheet.WorkZones

                            Dim mm = restoredSheet.ManufacturingModel

                            Debug.WriteLine("===== MM3 RESTORED COLLECTIONS =====")
                            Debug.WriteLine("Parts: " & restoredSheet.Parts.Count)
                            Dim partGeometryCount As Integer = 0

                            For Each part As Part In restoredSheet.Parts

                                If part Is Nothing OrElse part.Geometry Is Nothing Then Continue For

                                partGeometryCount += part.Geometry.Count

                            Next

                            Debug.WriteLine("PartGeometry: " & partGeometryCount.ToString())

                            Debug.WriteLine("ManufacturingGeometry: " &
    If(mm.ManufacturingGeometry Is Nothing,
       "Nothing",
       mm.ManufacturingGeometry.Count.ToString()))

                            Debug.WriteLine("WorkZones: " & restoredSheet.WorkZones.Count)

                            Debug.WriteLine("WorkZoneAssignments: " &
    restoredSheet.ManufacturingModel.WorkZoneAssignments.Count)

                            ' Preserve the original MM3 filename.
                            restoredSheet.MM3FilePath = dlg.SelectedFilePath

                            ' Restore the saved sheet dimensions.
                            matWidth = CDbl(restoredSheet.Width)
                            matLength = CDbl(restoredSheet.Length)

                            '---------------------------------------------
                            ' Initialize the MM3 drawing view.
                            '---------------------------------------------

                            Dim panelWidth As Integer = frmMain.pnlPicmodeler.ClientSize.Width
                            Dim panelHeight As Integer = frmMain.pnlPicmodeler.ClientSize.Height

                            If matLength <= 0 OrElse matWidth <= 0 OrElse
           panelWidth <= 40 OrElse panelHeight <= 40 Then

                                Throw New InvalidDataException(
                "Invalid MM3 sheet dimensions or drawing panel size.")
                            End If

                            Dim margin As Integer = 20

                            Dim scaleX As Double =
            (panelWidth - 2 * margin) / matLength

                            Dim scaleY As Double =
            (panelHeight - 2 * margin) / matWidth

                            Dim scale As Double =
            Math.Min(scaleX, scaleY)

                            Dim offsetX As Double =
            (panelWidth - matLength * scale) / 2

                            Dim offsetY As Double =
            panelHeight - (matWidth * scale) - margin

                            myTransform = New ViewTransform With {
            .Scale = scale,
            .OffsetX = offsetX,
            .OffsetY = offsetY
        }

                            '---------------------------------------------
                            ' Activate the restored MM3 job.
                            '---------------------------------------------

                            CurrentProductionJob = restoredJob
                            CurrentJob = restoredContext

                            ' MM3 is already restored. Do not run DXF import.
                            _DoPrepareUI = False
                            _DoDrawUI = True

                            ' Register restored MM3 geometry with SelectionManager.
                            frmMain.BuildDrawables()

                            frmMain.pnlPicmodeler.Invalidate()

                            frmMain.RefreshEntityList()


                            MessageBox.Show(
            "MM3 restoration completed successfully.",
            "Open Model",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)

                        Catch ex As Exception

                            MessageBox.Show(
            "MM3 restoration failed:" &
            Environment.NewLine &
            ex.ToString(),
            "Open Model",
            MessageBoxButtons.OK,
            MessageBoxIcon.Error)

                        End Try

                        Return Nothing

                    Case "NST", "CTG"

                        MessageBox.Show(
            "Opening " & dlg.SelectedFileType &
            " files is not connected yet." &
            Environment.NewLine &
            dlg.SelectedFilePath,
            "Open Model",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)

                        Return Nothing

                    Case "DXF"
                        ' Continue through the existing DXF workflow.

                    Case Else
                        MessageBox.Show("Unsupported file type.")
                        Return Nothing

                End Select



                '-------------------------------------------------
                ' Preserve Job-level UDP values.
                ' PrePareUI() runs later during the Paint cycle.
                '-------------------------------------------------

                _selectedDxfPath = dlg.SelectedDxfPath

                Using frm As New frmOpenProject(_selectedDxfPath)

                    If frm.ShowDialog() <> DialogResult.OK Then
                        Return Nothing
                    End If

                    PendingCustomerName = frm.CustomerName
                    PendingOrderPO = frm.OrderPO
                    PendingDueDate = frm.DueDate

                    '-------------------------------------------------
                    ' Store the Job-level UDP values.
                    '
                    ' PrePareUI will create CurrentProductionJob.
                    ' These values will be transferred once
                    ' CurrentProductionJob has been created.
                    '-------------------------------------------------

                    ' For now, preserve the existing selected values.
                    ' Customer/PO/Due Date will be connected once
                    ' CurrentProductionJob has been created.

                    '-------------------------------------------------
                    ' Tell the UI that a new file needs preparation.
                    ' The Paint event performs PrePareUI().
                    '-------------------------------------------------
                    Debug.WriteLine("Tell the UI that a new file needs preparation.")

                    _DoPrepareUI = True
                    _DoDrawUI = False

                    frmMain.InitializeWorkZonesPanel()

                    frmMain.pnlPicmodeler.Invalidate()
                    frmMain.RefreshEntityList()


                End Using

            End Using

        Finally

            frmMain.Cursor = Cursors.Default


            'Debug.WriteLine("DXF Dialog Load Time: " &
            '                loadTimer.ElapsedMilliseconds & " ms")


        End Try

    End Function


    Public Function ResolveWinding(
        role As AutoPunchingEngine.ProfileRole,
        cutDir As AutoPunchingEngine.CutDirection) _
        As AutoPunchingEngine.WindingDirection

        If cutDir = AutoPunchingEngine.CutDirection.Auto Then

            Return If(
                role = AutoPunchingEngine.ProfileRole.Inside,
                AutoPunchingEngine.WindingDirection.CCW,
                AutoPunchingEngine.WindingDirection.CW)

        End If

        ' For explicit CutDirection
        Return If(
            cutDir = AutoPunchingEngine.CutDirection.CW,
            AutoPunchingEngine.WindingDirection.CW,
            AutoPunchingEngine.WindingDirection.CCW)

    End Function


    Public Function BuildLayerColorMap(
        layerSetup As AppData.layersetup
    ) As Dictionary(Of String, Color)

        Dim dict As New Dictionary(Of String, Color)(
            StringComparer.OrdinalIgnoreCase)

        For Each layer In layerSetup.Layers
            dict(layer.CADLayer) = layer.DisplayColor
        Next

        Return dict

    End Function


    Function DetermineType(entity As Object) As CamEntityType

        ' This is simplified; adapt based on your DXF entity type

        If TypeOf entity Is DxfLine Then
            Return CamEntityType.Line
        End If

        If TypeOf entity Is DxfCircle Then
            Return CamEntityType.Circle
        End If

        If TypeOf entity Is DxfArc Then
            Return CamEntityType.Arc
        End If

        If TypeOf entity Is DxfPoint Then
            Return CamEntityType.Point
        End If

        Return CamEntityType.Line ' default fallback

    End Function


    Public Function DxfColorToDrawingColor(
        dxfColor As DxfColor,
        bgColor As Color) As Color

        Dim result As Color =
            System.Drawing.Color.Black

        If dxfColor IsNot Nothing Then

            Dim aci As Short =
                dxfColor.RawValue

            Select Case aci

                Case 1
                    result = System.Drawing.Color.Red

                Case 2
                    result = System.Drawing.Color.Yellow

                Case 3
                    result = System.Drawing.Color.Green

                Case 4
                    result = System.Drawing.Color.Cyan

                Case 5
                    result = System.Drawing.Color.Blue

                Case 6
                    result = System.Drawing.Color.Magenta

                Case 7
                    result = System.Drawing.Color.White

                Case Else
                    result = System.Drawing.Color.Black

            End Select

        End If

        ' Background safety
        If result.ToArgb() = bgColor.ToArgb() Then

            result =
                If(
                    bgColor.GetBrightness() > 0.5,
                    System.Drawing.Color.Black,
                    System.Drawing.Color.White)

        End If

        Return result

    End Function


    '-----------------------------------
    ' Map AutoCAD ColorIndex to Drawing.Color
    '-----------------------------------

    Private Function IxMiliaColorIndexToDrawingColor(
        index As Integer) As System.Drawing.Color

        ' Minimal palette mapping

        Select Case index

            Case 1
                Return System.Drawing.Color.Red

            Case 2
                Return System.Drawing.Color.Yellow

            Case 3
                Return System.Drawing.Color.Green

            Case 4
                Return System.Drawing.Color.Cyan

            Case 5
                Return System.Drawing.Color.Blue

            Case 6
                Return System.Drawing.Color.Magenta

            Case 7
                Return System.Drawing.Color.White

            Case Else
                Return System.Drawing.Color.Black

        End Select

    End Function


    '-------------------------------------------------
    ' Helper functions for DXF entities
    '-------------------------------------------------

    Public Function GetEntityStartPoint(
        e As ImportedEntity) As DxfPoint

        Select Case e.GeometryType

            Case ImportedEntity.GeometryTypeEnum.Line

                Return e.Line.P1

            Case ImportedEntity.GeometryTypeEnum.Arc

                Return GetArcStartPoint(e.Arc)

            Case ImportedEntity.GeometryTypeEnum.Circle

                ' For circles, arbitrarily pick a point on circumference (0 degrees)

                Return New DxfPoint(
                    e.Circle.Center.X + e.Circle.Radius,
                    e.Circle.Center.Y,
                    0)

            Case Else

                Throw New ArgumentException(
                    "Unsupported geometry type")

        End Select

    End Function


    ''' <summary>
    ''' Computes the start point of a DXF Arc based on its center, radius, and start angle.
    ''' </summary>

    Public Function MinDistanceBetweenEntities(
        e1 As ImportedEntity,
        e2 As ImportedEntity) As Double

        ' Profiles must not cross CAM layers.
        If Not String.Equals(
            e1.CAMLayer,
            e2.CAMLayer,
            StringComparison.OrdinalIgnoreCase) Then

            Return Double.MaxValue

        End If

        Dim points1 As New List(Of DxfPoint)
        Dim points2 As New List(Of DxfPoint)


        ' ---------------------------------------
        ' Entity 1 connection points
        ' ---------------------------------------

        Select Case e1.GeometryType

            Case ImportedEntity.GeometryTypeEnum.Line

                points1.Add(e1.Line.P1)
                points1.Add(e1.Line.P2)

            Case ImportedEntity.GeometryTypeEnum.Arc

                points1.Add(GetArcStartPoint(e1.Arc))
                points1.Add(GetArcEndPoint(e1.Arc))

            Case ImportedEntity.GeometryTypeEnum.Circle

                ' A circle is already closed.
                ' It does not have an open connection endpoint.

                Return Double.MaxValue

        End Select


        ' ---------------------------------------
        ' Entity 2 connection points
        ' ---------------------------------------

        Select Case e2.GeometryType

            Case ImportedEntity.GeometryTypeEnum.Line

                points2.Add(e2.Line.P1)
                points2.Add(e2.Line.P2)

            Case ImportedEntity.GeometryTypeEnum.Arc

                points2.Add(GetArcStartPoint(e2.Arc))
                points2.Add(GetArcEndPoint(e2.Arc))

            Case ImportedEntity.GeometryTypeEnum.Circle

                Return Double.MaxValue

        End Select


        ' ---------------------------------------
        ' Find minimum endpoint-to-endpoint distance
        ' ---------------------------------------

        Dim minDistance As Double =
            Double.MaxValue

        For Each p1 As DxfPoint In points1

            For Each p2 As DxfPoint In points2

                Dim dx As Double =
                    p1.X - p2.X

                Dim dy As Double =
                    p1.Y - p2.Y

                Dim dz As Double =
                    p1.Z - p2.Z

                Dim distance As Double =
                    Math.Sqrt(
                        dx * dx +
                        dy * dy +
                        dz * dz)

                If distance < minDistance Then
                    minDistance = distance
                End If

            Next

        Next

        Return minDistance

    End Function


    ''' <summary>
    ''' Computes the start point of a DXF Arc based on its center, radius, and start angle.
    ''' </summary>

    Public Function GetArcStartPoint(
        arc As DxfArc) As DxfPoint

        Dim rad As Double =
            arc.StartAngle * Math.PI / 180.0

        Return New DxfPoint(
            arc.Center.X +
                arc.Radius * Math.Cos(rad),
            arc.Center.Y +
                arc.Radius * Math.Sin(rad),
            0)

    End Function


    ''' <summary>
    ''' Computes the end point of a DXF Arc based on its center, radius, and end angle.
    ''' </summary>

    Public Function GetArcEndPoint(
        arc As DxfArc) As DxfPoint

        Dim rad As Double =
            arc.EndAngle * Math.PI / 180.0

        Return New DxfPoint(
            arc.Center.X +
                arc.Radius * Math.Cos(rad),
            arc.Center.Y +
                arc.Radius * Math.Sin(rad),
            0)

    End Function


    ' Helper function to avoid drawing on the same color as the background

    Private Function AdjustColorForBackground(
        c As Color,
        bgColor As Color) As Color

        If c.ToArgb() = bgColor.ToArgb() Then

            ' Simple adjustment: invert color

            Return System.Drawing.Color.FromArgb(
                c.A,
                255 - c.R,
                255 - c.G,
                255 - c.B)

        End If

        Return c

    End Function


    Private Function MatchDirective(
        entity As DxfEntity,
        layerSetup As AppData.layersetup) As Directive

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


    Private Function DirectiveMatchesEntity(
        directive As AppData.Directive,
        entity As DxfEntity) As Boolean

        If directive Is Nothing OrElse
           entity Is Nothing Then

            Return False

        End If

        If String.IsNullOrWhiteSpace(
            directive.CADLayer) Then

            Return False

        End If

        If String.IsNullOrWhiteSpace(
            entity.Layer) Then

            Return False

        End If

        Return String.Equals(
            entity.Layer.Trim(),
            directive.CADLayer.Trim(),
            StringComparison.OrdinalIgnoreCase)

    End Function


    ' Helper to compute key points for arc bounding box

    Private Function GetArcBoundingPoints(
        a As DxfArc) As List(Of DxfPoint)

        Dim points As New List(Of DxfPoint)

        ' Start and end points

        Dim startRad =
            a.StartAngle * Math.PI / 180

        Dim endRad =
            a.EndAngle * Math.PI / 180

        points.Add(
            New DxfPoint(
                a.Center.X +
                    a.Radius * Math.Cos(startRad),
                a.Center.Y +
                    a.Radius * Math.Sin(startRad),
                0))

        points.Add(
            New DxfPoint(
                a.Center.X +
                    a.Radius * Math.Cos(endRad),
                a.Center.Y +
                    a.Radius * Math.Sin(endRad),
                0))


        ' Check cardinal points (0, 90, 180, 270 degrees)

        Dim angles = {0, 90, 180, 270}

        For Each deg In angles

            Dim rad =
                deg * Math.PI / 180

            If deg >= a.StartAngle AndAlso
               deg <= a.EndAngle Then

                points.Add(
                    New DxfPoint(
                        a.Center.X +
                            a.Radius * Math.Cos(rad),
                        a.Center.Y +
                            a.Radius * Math.Sin(rad),
                        0))

            End If

        Next

        Return points

    End Function


    ' Compute all pairs of entities closer than gapTolerance
    '-------------------------------------------------

    Public Function ComputeGapPairs(
        entities As List(Of ImportedEntity),
        gapTolerance As Double) _
        As List(Of Tuple(Of ImportedEntity, ImportedEntity))

        Dim gapPairs As New List(Of Tuple(Of ImportedEntity, ImportedEntity))()

        For i As Integer = 0 To entities.Count - 2

            For j As Integer = i + 1 To entities.Count - 1

                Dim e1 = entities(i)
                Dim e2 = entities(j)


                ' Profiles cannot cross CAD layers.

                If e1.CadEntity Is Nothing OrElse
                   e2.CadEntity Is Nothing Then

                    Continue For

                End If


                If Not String.Equals(
                    e1.CadEntity.LayerName,
                    e2.CadEntity.LayerName,
                    StringComparison.OrdinalIgnoreCase) Then

                    Continue For

                End If


                Dim dist =
                    MinDistanceBetweenEntities(
                        e1,
                        e2)


                If dist <= gapTolerance Then

                    gapPairs.Add(
                        Tuple.Create(Of ImportedEntity, ImportedEntity)(
                            e1,
                            e2))

                End If

            Next

        Next

        Return gapPairs

    End Function


    'Public Function ComputeSignedArea(profile As List(Of CadEntity)) As Double
    '    ' Treat profile as polygon using start points of entities
    '    Dim points As New List(Of (Double, Double))()
    '
    '    For Each e As CadEntity In profile
    '        Dim endpoints = GetEntityEndpoints(e)
    '        points.Add(endpoints.Item1)
    '    Next
    '
    '    Dim area As Double = 0
    '    Dim n As Integer = points.Count
    '
    '    For i As Integer = 0 To n - 1
    '        Dim j As Integer = (i + 1) Mod n
    '        area += (points(i).Item1 * points(j).Item2) -
    '                (points(j).Item1 * points(i).Item2)
    '    Next
    '
    '    Return area / 2.0
    'End Function


    '-----------------------------------------
    ' Compute 2D distance between DxfPoints
    '-----------------------------------------

    Public Function Distance(
        p1 As DxfPoint,
        p2 As DxfPoint) As Double

        Dim dx As Double =
            p2.X - p1.X

        Dim dy As Double =
            p2.Y - p1.Y

        Return Math.Sqrt(
            dx * dx +
            dy * dy)

    End Function


    Public ReadOnly Property SelectedLayerSetup _
        As AppData.layersetup

        Get
            Return _selectedLayerSetup
        End Get

    End Property


    Public Sub SetFilename(
        ByVal Filename As String)

        m_FileName = Filename

    End Sub


    Public Function GetFilename() As String

        GetFilename = m_FileName

    End Function


    Private Function FindLayer(
        dxf As DxfFile,
        layerName As String) As DxfLayer

        Return dxf.Layers.FirstOrDefault(
            Function(l) l.Name = layerName)

    End Function


    Public Function ConvertCadEntitiesToImportedEntities(
        cadEntities As List(Of CadEntity),
        layerSetup As AppData.layersetup) _
        As List(Of ImportedEntity)

        Dim results As New List(Of ImportedEntity)()

        If cadEntities Is Nothing OrElse
           cadEntities.Count = 0 Then

            Return results

        End If

        If layerSetup Is Nothing OrElse
           layerSetup.Layers Is Nothing OrElse
           layerSetup.Layers.Count = 0 Then

            Return results

        End If


        ' Find the *DEFAULT* directive once.

        Dim defaultDirective As AppData.Directive =
            layerSetup.Layers.FirstOrDefault(
                Function(d) String.Equals(
                    d.CADLayer,
                    "*DEFAULT*",
                    StringComparison.OrdinalIgnoreCase))


        For Each cad As CadEntity In cadEntities

            If cad Is Nothing Then Continue For


            ' ----------------------------------------
            ' Find directive for this CAD layer.
            ' ----------------------------------------

            Dim directive As AppData.Directive =
                layerSetup.Layers.FirstOrDefault(
                    Function(d) String.Equals(
                        d.CADLayer,
                        cad.LayerName,
                        StringComparison.OrdinalIgnoreCase))


            ' No specific layer match -> use *DEFAULT*

            If directive Is Nothing Then
                directive = defaultDirective
            End If


            ' There should always be a default according
            ' to layersetup.Validate(), but don't fail the
            ' entire import if one is missing.

            If directive Is Nothing Then
                Continue For
            End If


            Dim imported As New ImportedEntity()

            cad.LayerName = directive.CAMLayer

            imported.CadEntity = cad

            imported.CAMLayer = directive.CAMLayer
            imported.DirectiveID = directive.ID
            imported.DisplayColor = directive.DisplayColor

            ' ----------------------------------------
            ' Geometry.
            ' ----------------------------------------

            If TypeOf cad Is CadEntity.CadLine Then

                Dim line As CadEntity.CadLine =
                    DirectCast(
                        cad,
                        CadEntity.CadLine)

                imported.GeometryType =
                    DxfGeometryHelpers.GeometryTypeEnum.Line

                imported.Line =
                    New DxfLine(
                        New DxfPoint(
                            line.StartPoint.X,
                            line.StartPoint.Y,
                            0),
                        New DxfPoint(
                            line.EndPoint.X,
                            line.EndPoint.Y,
                            0))


            ElseIf TypeOf cad Is CadEntity.CadCircle Then

                Dim circle As CadEntity.CadCircle =
                    DirectCast(
                        cad,
                        CadEntity.CadCircle)

                imported.GeometryType =
                    DxfGeometryHelpers.GeometryTypeEnum.Circle

                imported.Circle =
                    New DxfCircle(
                        New DxfPoint(
                            circle.Center.X,
                            circle.Center.Y,
                            0),
                        circle.Radius)

                imported.Radius =
                    circle.Radius


            ElseIf TypeOf cad Is CadEntity.CadArc Then

                Dim arc As CadEntity.CadArc =
                    DirectCast(
                        cad,
                        CadEntity.CadArc)

                imported.GeometryType =
                    DxfGeometryHelpers.GeometryTypeEnum.Arc

                imported.Arc =
                    New DxfArc(
                        New DxfPoint(
                            arc.Center.X,
                            arc.Center.Y,
                            0),
                        arc.Radius,
                        arc.StartAngle,
                        arc.EndAngle)

                imported.Radius =
                    arc.Radius

                imported.StartAngle =
                    arc.StartAngle

                imported.EndAngle =
                    arc.EndAngle


            ElseIf TypeOf cad Is CadPoint Then

                Dim point As CadPoint =
                    DirectCast(
                        cad,
                        CadPoint)

                imported.GeometryType =
                    DxfGeometryHelpers.GeometryTypeEnum.Point

                imported.Point =
                    New DxfPoint(
                        point.Position.X,
                        point.Position.Y,
                        0)


            Else

                ' Unsupported CAD entity type for the
                ' current profile pipeline.

                Continue For

            End If


            results.Add(imported)

        Next


        Return results

    End Function


    '-----------------------------------------
    ' Get the start point of an imported entity
    '-----------------------------------------

    Private Function GetImportedEntityStartPoint(
        entity As ImportedEntity) As PointF

        If entity Is Nothing Then
            Return PointF.Empty
        End If


        Select Case entity.GeometryType

            Case DxfGeometryHelpers.GeometryTypeEnum.Line

                If entity.Line Is Nothing Then
                    Return PointF.Empty
                End If

                Return New PointF(
                    CSng(entity.Line.P1.X),
                    CSng(entity.Line.P1.Y))


            Case DxfGeometryHelpers.GeometryTypeEnum.Arc

                If entity.Arc Is Nothing Then
                    Return PointF.Empty
                End If

                Return Geometry2D.PointOnCircle(
                    New PointF(
                        CSng(entity.Arc.Center.X),
                        CSng(entity.Arc.Center.Y)),
                    entity.Arc.Radius,
                    DegreesToRadians(
                        entity.Arc.StartAngle))


            Case Else

                Return PointF.Empty

        End Select

    End Function


    '-----------------------------------------
    ' Get the end point of an imported entity
    '-----------------------------------------

    Private Function GetImportedEntityEndPoint(
        entity As ImportedEntity) As PointF

        If entity Is Nothing Then
            Return PointF.Empty
        End If


        Select Case entity.GeometryType

            Case DxfGeometryHelpers.GeometryTypeEnum.Line

                If entity.Line Is Nothing Then
                    Return PointF.Empty
                End If

                Return New PointF(
                    CSng(entity.Line.P2.X),
                    CSng(entity.Line.P2.Y))


            Case DxfGeometryHelpers.GeometryTypeEnum.Arc

                If entity.Arc Is Nothing Then
                    Return PointF.Empty
                End If

                Return Geometry2D.PointOnCircle(
                    New PointF(
                        CSng(entity.Arc.Center.X),
                        CSng(entity.Arc.Center.Y)),
                    entity.Arc.Radius,
                    DegreesToRadians(
                        entity.Arc.EndAngle))


            Case Else

                Return PointF.Empty

        End Select

    End Function


    '-----------------------------------------
    ' Convert CAD degrees to path radians
    '-----------------------------------------

    Private Function DegreesToRadians(
        degrees As Double) As Double

        Return degrees * Math.PI / 180.0

    End Function


    ''-----------------------------------------
    '' Order the entities in a connected profile
    ''-----------------------------------------

    'Public Function OrderProfileEntities(
    '    profileEntities As List(Of ImportedEntity),
    '    tolerance As Double) _
    '    As List(Of ImportedEntity)

    '    Dim ordered As New List(Of ImportedEntity)()

    '    If profileEntities Is Nothing OrElse
    '       profileEntities.Count = 0 Then

    '        Return ordered

    '    End If


    '    Dim remaining As New List(Of ImportedEntity)(
    '        profileEntities)


    '    ' Start with the first entity.

    '    Dim current As ImportedEntity =
    '        remaining(0)

    '    remaining.RemoveAt(0)

    '    ordered.Add(current)


    '    Dim currentEnd As PointF =
    '        GetImportedEntityEndPoint(current)


    '    While remaining.Count > 0

    '        Dim foundIndex As Integer = -1
    '        Dim reverse As Boolean = False


    '        For i As Integer = 0 To remaining.Count - 1

    '            Dim candidate As ImportedEntity =
    '                remaining(i)

    '            Dim candidateStart As PointF =
    '                GetImportedEntityStartPoint(candidate)

    '            Dim candidateEnd As PointF =
    '                GetImportedEntityEndPoint(candidate)


    '            If DistanceBetweenPoints(
    '                currentEnd,
    '                candidateStart) <= tolerance Then

    '                foundIndex = i
    '                reverse = False

    '                Exit For

    '            End If


    '            If DistanceBetweenPoints(
    '                currentEnd,
    '                candidateEnd) <= tolerance Then

    '                foundIndex = i
    '                reverse = True

    '                Exit For

    '            End If

    '        Next


    '        If foundIndex < 0 Then
    '            Exit While
    '        End If


    '        Dim nextEntity As ImportedEntity =
    '            remaining(foundIndex)

    '        remaining.RemoveAt(foundIndex)


    '        If reverse Then

    '            nextEntity =
    '                ReverseImportedEntity(
    '                    nextEntity)

    '        End If


    '        ordered.Add(nextEntity)


    '        currentEnd =
    '            GetImportedEntityEndPoint(
    '                nextEntity)

    '    End While


    '    Return ordered

    'End Function


    '-----------------------------------------
    ' Distance between two world points
    '-----------------------------------------

    Private Function DistanceBetweenPoints(
        p1 As PointF,
        p2 As PointF) As Double

        Dim dx As Double =
            p2.X - p1.X

        Dim dy As Double =
            p2.Y - p1.Y

        Return Math.Sqrt(
            dx * dx +
            dy * dy)

    End Function


    '-----------------------------------------
    ' Create a reversed copy of an imported
    ' entity for profile path ordering.
    '-----------------------------------------

    Private Function ReverseImportedEntity(
        source As ImportedEntity) As ImportedEntity

        If source Is Nothing Then
            Return Nothing
        End If


        Dim reversed As New ImportedEntity()


        ' Copy common properties.

        reversed.CAMLayer =
            source.CAMLayer

        reversed.ColorMode =
            source.ColorMode

        reversed.ExplicitColor =
            source.ExplicitColor

        reversed.DisplayColor =
            source.DisplayColor

        reversed.DirectiveID =
            source.DirectiveID

        reversed.GeometryType =
            source.GeometryType

        reversed.Radius =
            source.Radius

        reversed.StartAngle =
            source.StartAngle

        reversed.EndAngle =
            source.EndAngle

        reversed.CadEntity =
            source.CadEntity

        reversed.WindingDirection =
            source.WindingDirection


        Select Case source.GeometryType

            Case DxfGeometryHelpers.GeometryTypeEnum.Line

                If source.Line Is Nothing Then
                    Return reversed
                End If

                reversed.Line =
                    New DxfLine(
                        source.Line.P2,
                        source.Line.P1)


            Case DxfGeometryHelpers.GeometryTypeEnum.Arc

                If source.CadEntity Is Nothing Then
                    Return reversed
                End If


                Dim sourceArc As CadEntity.CadArc =
                    TryCast(
                        source.CadEntity,
                        CadEntity.CadArc)

                If sourceArc Is Nothing Then
                    Return reversed
                End If


                ' Reverse the arc direction.

                reversed.StartAngle =
                    sourceArc.EndAngle

                reversed.EndAngle =
                    sourceArc.StartAngle

                reversed.WindingDirection =
                    If(
                        sourceArc.IsClockwise,
                        "CCW",
                        "CW")


                reversed.Arc =
                    New DxfArc(
                        New DxfPoint(
                            sourceArc.Center.X,
                            sourceArc.Center.Y,
                            0),
                        sourceArc.Radius,
                        reversed.StartAngle,
                        reversed.EndAngle)

        End Select


        Return reversed

    End Function


    '-----------------------------------------
    ' Convert an ordered imported profile into
    ' the nominal Path2D representation.
    ''-----------------------------------------

    'Public Function BuildProfilePath(
    '    orderedEntities As List(Of ImportedEntity)) _
    '    As IPath2D

    '    Dim path As New Path2D()


    '    If orderedEntities Is Nothing OrElse
    '       orderedEntities.Count = 0 Then

    '        Return path

    '    End If


    '    For Each entity As ImportedEntity _
    '        In orderedEntities

    '        If entity Is Nothing Then
    '            Continue For
    '        End If


    '        Select Case entity.GeometryType

    '            Case DxfGeometryHelpers.GeometryTypeEnum.Line

    '                If entity.Line Is Nothing Then
    '                    Continue For
    '                End If


    '                Dim startPoint As New PointF(
    '                    CSng(entity.Line.P1.X),
    '                    CSng(entity.Line.P1.Y))

    '                Dim endPoint As New PointF(
    '                    CSng(entity.Line.P2.X),
    '                    CSng(entity.Line.P2.Y))


    '                path.Add(
    '                    New LineSegment2D(
    '                        startPoint,
    '                        endPoint))


    '            Case DxfGeometryHelpers.GeometryTypeEnum.Arc

    '                If entity.Arc Is Nothing Then
    '                    Continue For
    '                End If


    '                Dim center As New PointF(
    '                    CSng(entity.Arc.Center.X),
    '                    CSng(entity.Arc.Center.Y))


    '                Dim startAngle As Double =
    '                    DegreesToRadians(
    '                        entity.Arc.StartAngle)

    '                Dim endAngle As Double =
    '                    DegreesToRadians(
    '                        entity.Arc.EndAngle)


    '                path.Add(
    '                    New ArcSegment2D(
    '                        center,
    '                        entity.Arc.Radius,
    '                        startAngle,
    '                        endAngle,
    '                        GetArcClockwise(entity)))

    '        End Select

    '    Next


    '    Return path

    'End Function


    Private Function GetImportedArcClockwise(
        entity As ImportedEntity) As Boolean

        If entity Is Nothing Then
            Return False
        End If


        If String.Equals(
            entity.WindingDirection,
            "CW",
            StringComparison.OrdinalIgnoreCase) Then

            Return True

        End If


        If String.Equals(
            entity.WindingDirection,
            "CCW",
            StringComparison.OrdinalIgnoreCase) Then

            Return False

        End If


        If entity.CadEntity Is Nothing Then
            Return False
        End If


        Dim arc As CadEntity.CadArc =
            TryCast(
                entity.CadEntity,
                CadEntity.CadArc)

        If arc Is Nothing Then
            Return False
        End If


        Return arc.IsClockwise

    End Function


    Private Function GetArcClockwise(
        entity As ImportedEntity) As Boolean

        If entity Is Nothing OrElse
           entity.CadEntity Is Nothing Then

            Return False

        End If


        Dim arc As CadEntity.CadArc =
            TryCast(
                entity.CadEntity,
                CadEntity.CadArc)

        If arc Is Nothing Then
            Return False
        End If


        Return arc.IsClockwise

    End Function


    ''-----------------------------------------
    '' Create a nominal Profile from an ordered
    '' imported entity group.
    ''-----------------------------------------

    'Public Function CreateProfileFromEntities(
    '    profileEntities As List(Of ImportedEntity),
    '    tolerance As Double,
    '    side As ProfileSide) As Profile

    '    If profileEntities Is Nothing OrElse
    '       profileEntities.Count = 0 Then

    '        Return Nothing

    '    End If


    '    Dim orderedEntities As List(Of ImportedEntity) =
    '        OrderProfileEntities(
    '            profileEntities,
    '            tolerance)

    '    If orderedEntities.Count = 0 Then
    '        Return Nothing
    '    End If


    '    Dim path As IPath2D =
    '        BuildProfilePath(
    '            orderedEntities)

    '    If path Is Nothing OrElse
    '       path.Segments Is Nothing Then

    '        Return Nothing

    '    End If


    '    Return New Profile(path, side, Nothing)

    'End Function


    'Public Function BuildProfilesFromEntities(
    '    entities As List(Of ImportedEntity),
    '    gapTolerance As Double) _
    '    As List(Of ProfileBuildResult)

    '    Dim profiles As New List(Of ProfileBuildResult)()


    '    If entities Is Nothing OrElse
    '       entities.Count = 0 Then

    '        Return profiles

    '    End If


    '    Dim profileGroups As List(Of List(Of ImportedEntity)) =
    '        BuildProfiles(
    '            entities,
    '            gapTolerance)


    '    ' Item1 = CAD layer
    '    ' Item2 = Profile path
    '    ' Item3 = DirectiveID

    '    Dim profileData As New List(
    'Of Tuple(Of String, IPath2D, Integer, List(Of ImportedEntity)))()


    '    ' Build all profile paths and retain the CAD layer
    '    ' and DirectiveID for later feature creation.

    '    For Each profileEntities As List(Of ImportedEntity) _
    '        In profileGroups

    '        If profileEntities Is Nothing OrElse
    '           profileEntities.Count = 0 Then

    '            Continue For

    '        End If


    '        Dim orderedEntities As List(Of ImportedEntity) =
    '            OrderProfileEntities(
    '                profileEntities,
    '                gapTolerance)

    '        If orderedEntities.Count = 0 Then
    '            Continue For
    '        End If


    '        Dim path As IPath2D =
    '            BuildProfilePath(
    '                orderedEntities)

    '        If path Is Nothing OrElse
    '           path.Segments Is Nothing Then

    '            Continue For

    '        End If


    '        Dim cadLayer As String = Nothing
    '        Dim directiveID As Integer = 0


    '        For Each entity As ImportedEntity _
    '            In orderedEntities

    '            If entity IsNot Nothing Then

    '                If entity.CadEntity IsNot Nothing AndAlso
    '                   cadLayer Is Nothing Then

    '                    cadLayer =
    '                        entity.CadEntity.LayerName

    '                End If


    '                directiveID =
    '                    entity.DirectiveID

    '                Exit For

    '            End If

    '        Next


    '        profileData.Add(
    'Tuple.Create(
    '    cadLayer,
    '    path,
    '    directiveID,
    '    orderedEntities))

    '    Next


    '    ' Create the final profiles. Containment is evaluated
    '    ' only against profiles on the same CAD layer.

    '    For Each item In profileData

    '        Dim currentLayer As String =
    '            item.Item1

    '        Dim currentPath As IPath2D =
    '            item.Item2

    '        Dim directiveID As Integer =
    '            item.Item3
    '        Dim sourceEntities As List(Of ImportedEntity) =
    '            item.Item4

    '        Debug.Print(
    '"PROFILE SOURCE COUNT = " &
    'If(sourceEntities Is Nothing, 0, sourceEntities.Count))

    '        If sourceEntities IsNot Nothing Then

    '            For Each sourceEntity As ImportedEntity In sourceEntities

    '                If sourceEntity Is Nothing Then Continue For

    '                Debug.Print(
    '        "  Geometry=" &
    '        sourceEntity.GeometryType.ToString() &
    '        ", CadEntity=" &
    '        If(sourceEntity.CadEntity Is Nothing,
    '           "Nothing",
    '           sourceEntity.CadEntity.GetType().Name))

    '            Next

    '        End If

    '        Dim sameLayerPaths As New List(Of IPath2D)()


    '        For Each candidate In profileData

    '            If String.Equals(
    '                currentLayer,
    '                candidate.Item1,
    '                StringComparison.OrdinalIgnoreCase) Then

    '                sameLayerPaths.Add(
    '                    candidate.Item2)

    '            End If

    '        Next


    '        Dim side As ProfileSide =
    '            DetermineProfileSide(
    '                currentPath,
    '                sameLayerPaths)


    '        profiles.Add(
    '            New ProfileBuildResult With {
    '                .Profile =
    '                   New Profile(
    '                    currentPath,
    '                    side,
    '                    sourceEntities),
    '                .DirectiveID =
    '                    directiveID
    '            })
    '    Next


    '    Return profiles

    'End Function


    'Private Function DetermineProfileSide(
    '    profilePath As IPath2D,
    '    otherPaths As List(Of IPath2D)) _
    '    As ProfileSide

    '    If profilePath Is Nothing OrElse
    '       otherPaths Is Nothing Then

    '        Return ProfileSide.Outside

    '    End If


    '    If Not profilePath.IsClosed Then
    '        Return ProfileSide.Outside
    '    End If


    '    Dim containmentDepth As Integer = 0


    '    For Each otherPath As IPath2D _
    '        In otherPaths

    '        If otherPath Is Nothing Then
    '            Continue For
    '        End If


    '        If Object.ReferenceEquals(
    '            profilePath,
    '            otherPath) Then

    '            Continue For

    '        End If


    '        If Not otherPath.IsClosed Then
    '            Continue For
    '        End If


    '        If Geometry2D.IsPathInsidePath(
    '            profilePath,
    '            otherPath) Then

    '            containmentDepth += 1

    '        End If

    '    Next


    '    If (containmentDepth Mod 2) = 0 Then

    '        Return ProfileSide.Outside

    '    Else

    '        Return ProfileSide.Inside

    '    End If

    'End Function


    'Public Function BuildProfiles(
    '    entities As List(Of ImportedEntity),
    '    gapTol As Double) _
    '    As List(Of List(Of ImportedEntity))

    '    Dim profiles As New List(Of List(Of ImportedEntity))()

    '    Dim visited As New HashSet(Of ImportedEntity)()


    '    Dim gapPairs As List(Of Tuple(Of ImportedEntity, ImportedEntity)) =
    '        ComputeGapPairs(
    '            entities,
    '            gapTol)


    '    For Each entity As ImportedEntity _
    '        In entities

    '        If visited.Contains(entity) Then
    '            Continue For
    '        End If


    '        Dim profile As New List(Of ImportedEntity)()

    '        Dim stack As New Stack(Of ImportedEntity)()

    '        stack.Push(entity)


    '        While stack.Count > 0

    '            Dim current As ImportedEntity =
    '                stack.Pop()


    '            If visited.Contains(current) Then
    '                Continue While
    '            End If


    '            visited.Add(current)

    '            profile.Add(current)


    '            For Each pair As Tuple(Of ImportedEntity, ImportedEntity) _
    '                In gapPairs

    '                If pair.Item1 Is current AndAlso
    '                   Not visited.Contains(pair.Item2) Then

    '                    stack.Push(pair.Item2)


    '                ElseIf pair.Item2 Is current AndAlso
    '                       Not visited.Contains(pair.Item1) Then

    '                    stack.Push(pair.Item1)

    '                End If

    '            Next

    '        End While


    '        profiles.Add(profile)

    '    Next


    '    Return profiles

    'End Function


End Module

