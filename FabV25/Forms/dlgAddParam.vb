Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.IO
Imports FabV25_WIN8.ConfigurationManagerForm
Imports FabV25_WIN8.frmMain

Public Class dlgAddParam
    Public Shared MyWhichNode As Integer
    Public Shared IsAdding As Boolean

    Private Sub btnParamOk_Click(sender As Object, e As EventArgs) Handles btnParamOk.Click
        'Private Sub AddToolSetupParameter(paramName As String, paramDisplay As String, dataType As String, defaultValue As String, visible As Boolean)
        Dim paramName As String = txtParamName.Text.Trim()
        Dim paramDataType As String = cboParamDataType.SelectedItem.ToString()
        Dim paramDisplay As String = txtParamDisplay.Text.Trim()
        Dim defaultValue As String = txtParamDefault.Text.Trim()
        Dim visible As Boolean = chkIsVisible.Checked

        '' --- Validate input ---
        'If String.IsNullOrWhiteSpace(txtParamName.Text) Then
        '    MessageBox.Show("Parameter Name cannot be empty.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
        '    Return
        'End If

        'If String.IsNullOrWhiteSpace(txtParamDisplay.Text) Then
        '    MessageBox.Show("Display Name cannot be empty.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
        '    Return
        'End If

        '' --- Access main form data ---
        'Dim mainForm As ConfigurationManagerForm = CType(Me.Owner, ConfigurationManagerForm)
        'Dim machineID As Integer = mainForm.currentMachineID ' <-- you'll need to set this before showing the dialog

        '' --- Step D: Add new AttributeType ---
        'Dim newAttrTypeID As Integer = 1
        'If mainForm.machineAttributeTypesTable.Rows.Any() Then
        '    newAttrTypeID = mainForm.machineAttributeTypesTable.Rows.Max(Function(t) t.ID) + 1
        'End If

        'Dim newAttrType As New ConfigurationManagerForm.MachineAttributeType With {
        '    .ID = newAttrTypeID,
        '    .Display = txtParamDisplay.Text
        '}
        'mainForm.machineAttributeTypesTable.Rows.Add(newAttrType)

        '' --- Save updated MachineAttributeTypes.json ---
        'Dim attrTypesFile = Path.Combine(mainForm.dataFolder, "MachineAttributeTypes.json")
        'File.WriteAllText(attrTypesFile, JsonConvert.SerializeObject(mainForm.machineAttributeTypesTable, Formatting.Indented))

        '' --- Step F: Add new MachineAttribute for current machine ---
        'Dim newAttr As New ConfigurationManagerForm.MachineAttribute With {
        '    .MachineID = machineID,
        '    .AttributeTypeID = newAttrTypeID,
        '    .Value = txtParamDefault.Text
        '}
        'mainForm.machineAttributesTable.Rows.Add(newAttr)

        '' --- Save updated MachineAttributes.json ---
        'Dim attrFile = Path.Combine(mainForm.dataFolder, "MachineAttributes.json")
        'File.WriteAllText(attrFile, JsonConvert.SerializeObject(mainForm.machineAttributesTable, Formatting.Indented))


        ' Check if the column already exists
        If stationsTable.Columns.Any(Function(c) c.Field = paramName) Then
            MessageBox.Show($"Parameter '{paramName}' already exists.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Add new column to the columns collection
        stationsTable.Columns.Add(New JsonColumn With {
            .Field = paramName,
            .Header = paramDisplay,
            .Visible = visible
        })

        ' Add default value to every station row
        For Each station As JObject In stationsTable.Rows
            station(paramName) = defaultValue
        Next

        ' Save back to JSON
        JsonSaver.SaveObjectToJsonFile(stationsTable, Path.Combine(dataFolder, "stations.json"))

        ' --- Close the dialog ---
        Me.DialogResult = DialogResult.OK
        Me.Close()

    End Sub

End Class