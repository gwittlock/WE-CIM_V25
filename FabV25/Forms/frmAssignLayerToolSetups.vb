Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports System.Linq
Public Class frmAssignLayerToolSetups
    Private _conversionFolder As String
    Private Sub frmAssignLayerToolSetups_Load(sender As Object, e As EventArgs) Handles MyBase.Load

    End Sub
    Public Sub InitializeConversion(conversionFolder As String)

        _conversionFolder = conversionFolder

        LoadLayerToolSetupGrid(conversionFolder)

    End Sub
    Private Sub LoadLayerToolSetupGrid(conversionFolder As String)

        Dim layerTable =
    Newtonsoft.Json.JsonConvert.DeserializeObject(
        Of AppData.JsonTable(Of AppData.layersetup))(
            IO.File.ReadAllText(
                IO.Path.Combine(conversionFolder, "LayerSetups.json")))

        Dim toolTable =
    Newtonsoft.Json.JsonConvert.DeserializeObject(
        Of AppData.JsonTable(Of ToolSetup))(
            IO.File.ReadAllText(
                IO.Path.Combine(conversionFolder, "ToolSetups.json")))

        Dim machineTable =
    Newtonsoft.Json.JsonConvert.DeserializeObject(
        Of AppData.JsonTable(Of Machine))(
            IO.File.ReadAllText(
                IO.Path.Combine(conversionFolder, "Machines.json")))

        dgvLayerToolSetups.Rows.Clear()
        dgvLayerToolSetups.AllowUserToAddRows = False

        For Each ls In layerTable.Rows

            Dim machine = machineTable.Rows.
            FirstOrDefault(Function(m) m.ID = ls.MachineID)

            Dim rowIndex As Integer =
            dgvLayerToolSetups.Rows.Add()

            Dim row = dgvLayerToolSetups.Rows(rowIndex)

            row.Tag = ls.ID

            row.Cells("colMachine").Value =
            If(machine Is Nothing,
               "Unknown Machine",
               machine.Description)

            row.Cells("colLayerSetup").Value =
            ls.Description

            Dim combo =
            DirectCast(row.Cells("colToolSetup"),
                       DataGridViewComboBoxCell)

            combo.DisplayMember = "Description"
            combo.ValueMember = "ID"

            combo.DataSource =
            toolTable.Rows.
                Where(Function(t) t.MachineID = ls.MachineID).
                ToList()

            ' No Tool Setup has been assigned yet.
            combo.Value = Nothing
        Next

    End Sub


    Private Function ValidateToolSetupMachines() As Boolean

        Dim toolTable =
        Newtonsoft.Json.JsonConvert.DeserializeObject(
            Of AppData.JsonTable(Of ToolSetup))(
                IO.File.ReadAllText(
                    IO.Path.Combine(_conversionFolder, "ToolSetups.json")))

        Dim layerTable =
        Newtonsoft.Json.JsonConvert.DeserializeObject(
            Of AppData.JsonTable(Of AppData.layersetup))(
                IO.File.ReadAllText(
                    IO.Path.Combine(_conversionFolder, "LayerSetups.json")))

        For Each row As DataGridViewRow In dgvLayerToolSetups.Rows

            If row.IsNewRow Then Continue For

            Dim layerSetupID As Integer = CInt(row.Tag)
            Dim toolSetupID As Integer = CInt(row.Cells("colToolSetup").Value)

            Dim ls = layerTable.Rows.FirstOrDefault(
            Function(x) x.ID = layerSetupID)

            Dim ts = toolTable.Rows.FirstOrDefault(
            Function(x) x.ID = toolSetupID)

            If ls Is Nothing OrElse
           ts Is Nothing OrElse
           ls.MachineID <> ts.MachineID Then

                MessageBox.Show(
                $"Invalid Tool Setup assignment for Layer Setup ID {layerSetupID}.",
                "Machine Association Error",
                MessageBoxButtons.OK,
                MessageBoxIcon.Error)

                Return False

            End If

        Next

        Return True

    End Function



    Private Sub btnSaveAssignments_Click(
        sender As Object,
        e As EventArgs) Handles btnSaveAssignments.Click

        ' Commit any selection currently being edited.
        dgvLayerToolSetups.EndEdit()

        For Each row As DataGridViewRow In dgvLayerToolSetups.Rows

            If row.IsNewRow Then Continue For

            Dim selectedToolSetup = row.Cells("colToolSetup").Value

            If selectedToolSetup Is Nothing OrElse
               selectedToolSetup Is DBNull.Value Then

                MessageBox.Show(
                    "Every Layer Setup must have a Tool Setup assigned.",
                    "Incomplete Conversion",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Warning)

                dgvLayerToolSetups.CurrentCell =
                    row.Cells("colToolSetup")

                Return

            End If

        Next


        ' Verify each Tool Setup belongs to the correct Machine.
        If Not ValidateToolSetupMachines() Then
            Return
        End If


        Try

            SaveLayerToolSetupAssignments()

            MessageBox.Show(
        "Layer Setup assignments saved successfully.",
        "Conversion",
        MessageBoxButtons.OK,
        MessageBoxIcon.Information)

        Catch ex As Exception

            MessageBox.Show(
        "Unable to save Layer Setup assignments: " & ex.Message,
        "Conversion Error",
        MessageBoxButtons.OK,
        MessageBoxIcon.Error)

        End Try

    End Sub

    Private Sub SaveLayerToolSetupAssignments()

        Dim filePath As String =
        IO.Path.Combine(_conversionFolder, "LayerSetups.json")

        Dim root As Newtonsoft.Json.Linq.JObject =
        Newtonsoft.Json.Linq.JObject.Parse(
            IO.File.ReadAllText(filePath))

        Dim rows As Newtonsoft.Json.Linq.JArray =
        DirectCast(root("Rows"), Newtonsoft.Json.Linq.JArray)

        For Each gridRow As DataGridViewRow In dgvLayerToolSetups.Rows

            If gridRow.IsNewRow Then Continue For

            Dim layerSetupID As Integer = CInt(gridRow.Tag)
            Dim toolSetupID As Integer =
            CInt(gridRow.Cells("colToolSetup").Value)

            Dim layerRow = rows.
            OfType(Of Newtonsoft.Json.Linq.JObject)().
            FirstOrDefault(
                Function(r) r.Value(Of Integer)("ID") = layerSetupID)

            If layerRow Is Nothing Then
                Throw New InvalidOperationException(
                $"Layer Setup ID {layerSetupID} was not found.")
            End If

            layerRow("ToolSetupID") = toolSetupID

        Next

        IO.File.WriteAllText(
        filePath,
        root.ToString(Newtonsoft.Json.Formatting.Indented))

    End Sub

End Class