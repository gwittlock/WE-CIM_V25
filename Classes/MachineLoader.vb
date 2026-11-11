Imports System.IO
Imports System.Text.Json
Imports FabV25_WIN8.AppData
Imports FabV25_WIN8.WorkZoneFactory

Module MachineLoader
    Public _machineDef As MachineDefinition
    Public ReadOnly Property WorkZones As New List(Of WorkZone)
    Public ReadOnly WorkzoneColors As Color() = {
    Color.DarkBlue,
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
        .Clamp_CTG = machineRow.Clamp_CTG,
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

    Public Sub RestoreAndDrawJob()
        ' SAFETY
        If CurrentJob Is Nothing Then Exit Sub
        If CurrentJob.MachineDefinition Is Nothing Then Exit Sub


        ' 1. Ensure workzones exist
        If CurrentJob.WorkZones.Count = 0 Then
            CurrentJob.WorkZones = WorkZoneFactory.CreateInitialZones(
            CurrentJob.MachineDefinition,
            matLength,
            matWidth
        )
        End If


        ' 2. Initialize clamps
        'InitializeClamps(CurrentJob, 2)

        ' 3. Redraw
        'frmMain.pnlPicmodeler.Invalidate()
    End Sub

    Public Sub InitializeClamps(job As JobContext)

        If job Is Nothing Then Exit Sub
        If job.MachineDefinition Is Nothing Then Exit Sub

        Dim clampCount As Integer =
            Math.Max(0, job.MachineDefinition.Number_of_Clamps)

        ' Preserve existing clamp positions.
        ' Add only clamps that do not already exist.
        While job.Clamps.Count < clampCount

            job.Clamps.Add(New Clamp With {
                .Name = "Clamp " & (job.Clamps.Count + 1).ToString(),
                .X = 0,
                .Y = 0
            })

        End While

    End Sub

End Module





' Generic JSON wrapper
Public Class TableWrapper(Of T)
        Public Property TableName As String
        Public Property Columns As Object()
        Public Property Rows As List(Of T)
    End Class
