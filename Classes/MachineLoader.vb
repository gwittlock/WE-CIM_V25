Imports System.IO
Imports System.Text.Json
Imports FabV25_WIN8.AppData
Imports FabV25_WIN8.WorkZoneFactory

Module MachineLoader
    Public _machineDef As MachineDefinition
    Public ReadOnly Property WorkZones As New List(Of WorkZone)
    Public ReadOnly WorkzoneColors As Color() = {
    Color.LightBlue,
    Color.LightGreen,
    Color.LightCoral,
    Color.LightGoldenrodYellow,
    Color.LightPink,
    Color.LightSalmon
}
    'Public Machines As List(Of Machine)
    'Public MachineAttributeTypes As List(Of MachineAttributeType)
    'Public MachineAttributes As List(Of MachineAttribute)

    '' Call this once to load all machine data from JSON files
    'Public Sub LoadAllMachineData(basePath As String)
    '    Machines = LoadTableWrapped(Of Machine)(Path.Combine(basePath, "Machines.json"))
    '    MachineAttributeTypes = LoadTableWrapped(Of MachineAttributeType)(Path.Combine(basePath, "MachineAttributeTypes.json"))
    '    MachineAttributes = LoadTableWrapped(Of MachineAttribute)(Path.Combine(basePath, "MachineAttributes.json"))
    'End Sub

    ' Generic JSON loader
    Private Function LoadTableWrapped(Of T)(filePath As String) As List(Of T)
        Dim json = File.ReadAllText(filePath)
        Dim wrapper = JsonSerializer.Deserialize(Of TableWrapper(Of T))(json)
        Return wrapper.Rows
    End Function

    ' Load a MachineDefinition given a MachineID
    Public Function LoadMachineDefinition(machineID As Integer) As MachineDefinition
        ' Find the machine
        Dim machineRow = Machines.FirstOrDefault(Function(m) m.ID = machineID)
        If machineRow Is Nothing Then Return Nothing

        ' Build a map of AttributeTypeID -> Description
        Dim attributeMap As New Dictionary(Of Integer, String)
        For Each t In MachineAttributeTypes
            attributeMap(t.ID) = t.Description
        Next

        ' Find all attributes for this machine
        Dim machineAttrs = MachineAttributes.Where(Function(a) a.MachineID = machineID).ToList()

        ' Fill MachineDefinition
        Dim def As New MachineDefinition With {
        .ID = machineRow.ID,
        .Name = machineRow.Description,
        .TypeID = machineRow.TypeID,
        .CNC_Folder = machineRow.CNC_Folder,
        .Quadrant = MachineEnvelope.MachineQuadrant.LowerLeft, ' default, can update
        .Units = machineRow.Units,
        .WorkplaneTypeID = machineRow.WorkplaneTypeID,
        .Description = machineRow.Description
    }

        ' Use reflection to map attributes dynamically
        Dim props = GetType(MachineDefinition).GetProperties()
        For Each attr In machineAttrs
            Dim propName As String = Nothing
            If attributeMap.ContainsKey(attr.AttributeTypeID) Then
                propName = attributeMap(attr.AttributeTypeID)
            End If

            If Not String.IsNullOrEmpty(propName) Then
                Dim prop = props.FirstOrDefault(Function(p) p.Name = propName)
                If prop IsNot Nothing AndAlso prop.CanWrite Then
                    ' Convert value string to the property type
                    Dim val As Object = Convert.ChangeType(attr.Value, prop.PropertyType)
                    prop.SetValue(def, val)
                End If
            End If
        Next

        Return def
    End Function

    Public Sub DrawSheetAndWorkzones(g As Graphics, sheetWidth As Double, sheetHeight As Double)
        g.SmoothingMode = Drawing2D.SmoothingMode.AntiAlias
        Dim margin As Integer = 20
        ' --- Compute scale to fit sheet into panel ---
        Dim scaleX As Double = panelWidth / sheetWidth
        Dim scaleY As Double = panelHeight / sheetHeight
        Dim scale As Double = Math.Min(scaleX, scaleY) ' maintain aspect ratio

        '' Compute offsets to center sheet in panel
        'Dim offsetX As Double = (panelWidth - sheetWidth * scale) / 2
        'Dim offsetY As Double = (panelHeight - sheetHeight * scale) / 2


        'Dim offsetX As Double = (panelWidth - sheetWidth * scale) / 2
        'Dim offsetY As Double = panelHeight - (sheetHeight * scale) - margin

        '' Create transform
        'Dim myTransform As New ViewTransform With {
        '    .Scale = scale,
        '    .OffsetX = offsetX,
        '    .OffsetY = offsetY
        '}

        ' --- Draw sheet background ---
        Dim bl As PointF = WorldToScreen(0, 0, myTransform, panelHeight)
        Dim tr As PointF = WorldToScreen(sheetWidth, sheetHeight, myTransform, panelHeight)
        Dim sheetRect = HelperFunctions.RectFromPoints(bl, tr)
        g.FillRectangle(Brushes.LightGray, sheetRect)

        ' --- Draw workzones ---
        For i = 0 To CurrentJob.WorkZones.Count - 1
            Dim wz = CurrentJob.WorkZones(i)
            Dim wzColor As Color = WorkzoneColors(i Mod WorkzoneColors.Length)
            Dim wzBrush As New SolidBrush(Color.FromArgb(60, wzColor))
            Dim wzPen As New Pen(wzColor, 2)

            ' World-space corners
            Dim wzBL As PointF = WorldToScreen(wz.XMin, wz.YMin, myTransform, panelHeight)
            Dim wzTR As PointF = WorldToScreen(wz.XMax, wz.YMax, myTransform, panelHeight)
            Dim wzRect = HelperFunctions.RectFromPoints(wzBL, wzTR)

            g.FillRectangle(wzBrush, wzRect)
            g.DrawRectangle(wzPen, wzRect.X, wzRect.Y, wzRect.Width, wzRect.Height)

            ' --- Draw clamps ---
            For Each clamp In wz.Clamps
                ' Punch rectangle
                Dim punchBL As PointF = WorldToScreen(clamp.X - clamp.Width / 2, clamp.Y, myTransform, panelHeight)
                Dim punchTR As PointF = WorldToScreen(clamp.X + clamp.Width / 2, clamp.Y + clamp.Height, myTransform, panelHeight)
                Dim punchRect = HelperFunctions.RectFromPoints(punchBL, punchTR)
                g.FillRectangle(wzBrush, punchRect)
                g.DrawRectangle(wzPen, punchRect.X, punchRect.Y, punchRect.Width, punchRect.Height)

                ' Torch rectangle
                Dim torchBL As PointF = WorldToScreen(clamp.X - clamp.TorchSize.Width / 2, clamp.Y, myTransform, panelHeight)
                Dim torchTR As PointF = WorldToScreen(clamp.X + clamp.TorchSize.Width / 2, clamp.Y + clamp.TorchSize.Height, myTransform, panelHeight)
                Dim torchRect = HelperFunctions.RectFromPoints(torchBL, torchTR)
                g.FillRectangle(wzBrush, torchRect)
                g.DrawRectangle(wzPen, torchRect.X, torchRect.Y, torchRect.Width, torchRect.Height)
            Next
        Next
    End Sub




    Public Sub RestoreAndDrawJob()
        ' SAFETY
        If CurrentJob Is Nothing Then Exit Sub
        If CurrentJob.MachineDefinition Is Nothing Then Exit Sub


        Dim sheetHeight = GetMaterialWidth(MaterialSheets, _SelectedMaterialID)
        Dim sheetWidth = GetMaterialLength(MaterialSheets, _SelectedMaterialID)
        ' 1. Ensure workzones exist
        If CurrentJob.WorkZones.Count = 0 Then
            CurrentJob.WorkZones = WorkZoneFactory.CreateInitialZones(
            CurrentJob.MachineDefinition,
            sheetWidth,
            sheetHeight
        )
        End If

        ' 2. Initialize clamps
        InitializeClamps(CurrentJob, 2)

        ' 3. Redraw
        ' frmMain.pnlPicmodeler.Invalidate()
    End Sub


    Public Sub InitializeClamps(
   job As JobContext,
   userClampCount As Integer
)
        Dim md = job.MachineDefinition

        userClampCount = Math.Min(userClampCount, md.Number_of_Clamps)

        Dim punchWidth = md.Punch_Clamp_Deadzone_Width
        Dim punchLength = md.Punch_Clamp_Deadzone_Length
        Dim punchCenterX = md.Punch_Clamp_Deadzone_Center

        Dim torchWidth = md.Torch_Clamp_Deadzone_Width
        Dim torchLength = md.Torch_Clamp_Deadzone_Length
        Dim torchCenterX = md.Torch_Clamp_Deadzone_Center

        For Each wz In job.WorkZones
            wz.Clamps.Clear()

            For i = 1 To userClampCount
                wz.Clamps.Add(New Clamp With {
                    .X = punchCenterX,
                    .Y = 0,
                    .Width = punchLength,
                    .Height = punchWidth,
                    .TorchSize = New SizeF(
                        CSng(torchLength),
                        CSng(torchWidth)
                    )
                })
            Next
        Next
    End Sub


End Module





' Generic JSON wrapper
Public Class TableWrapper(Of T)
        Public Property TableName As String
        Public Property Columns As Object()
        Public Property Rows As List(Of T)
    End Class
