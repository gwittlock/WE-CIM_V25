
Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraTreeList
Imports DevExpress.XtraTreeList.Nodes
Imports FabV25_WIN8.WE_ENG_V25_0.Core.State


Public Class frmOpenProject


    ' Public properties to retrieve selected values after dialog closes
    Public Property SelectedMachineID As Integer
    Public Property SelectedToolSetupID As Integer
    Public Property SelectedCustomer As String
    Public Property SelectedOrderPO As String
    Public Property SelectedDueDate As DateTime?
    Public Property SelectedMaterialID As Integer
    Public Property SelectedLayerSetup As String
    Public Property MaterialLength As Double?
    Public Property MaterialWidth As Double?
    Public Property MaterialThickness As Double?
    Public Class ToolSetupItem
        Public Property ID As Integer
        Public Property Description As String
        Public Overrides Function ToString() As String
            Return Description
        End Function
    End Class

    Private Sub frmOpenProject_Load(sender As Object, e As EventArgs) Handles Me.Load
        PopulateMachines()
        'PopulateToolSetups()
        PopulateMaterials()
        PopulateLayerSetups()
    End Sub

    Private Sub cbeMachine_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeMachine.SelectedIndexChanged
        Dim selectedMachine = cbeMachine.SelectedItem
        If selectedMachine IsNot Nothing Then
            currentMachineID = selectedMachine.Value
            UpdateToolSetupCombo()
        End If
    End Sub

    Private Sub PopulateMachines()
        'cbeMachine.Properties.Items.Clear()
        'For Each m In AppData.Machines
        '    cbeMachine.Properties.Items.Add(New ComboBoxItem(m.Description, m.ID))
        'Next
        cbeMachine.Properties.Items.Clear()

        For Each m In AppData.Machines
            ' Show description but store ID via KeyValuePair
            cbeMachine.Properties.Items.Add(New KeyValuePair(Of String, Integer)(m.Description, m.ID))
        Next

        If cbeMachine.Properties.Items.Count > 0 Then
            cbeMachine.SelectedIndex = 0
        End If

    End Sub

    Private Sub PopulateToolSetups(machineID As Integer)
        'cbeToolSetup.Properties.Items.Clear()

        'For Each ts In AppData.ToolSetups
        '    ' Extract MachineID and Name from the JObject
        '    Dim tsMachineID As Integer = ts.Value(Of Integer)("MachineID")
        '    Dim tsID As Integer = ts.Value(Of Integer)("ID")
        '    Dim tsName As String = ts.Value(Of String)("Name")

        '    If tsMachineID = machineID Then
        '        cbeToolSetup.Properties.Items.Add(New ComboBoxItem(tsName, tsID))
        '    End If
        'Next
    End Sub

    Private Sub PopulateMaterials()
        cbeMaterial.Properties.Items.Clear()
        'For Each mat In AppState.Materials
        '    cbeMaterial.Properties.Items.Add(New ComboBoxItem(mat.Name, mat.ID))
        'Next
    End Sub

    Private Sub PopulateLayerSetups()
        ' Example: add layer setups manually, or pull from AppState if defined
        cbeLayerSetup.Properties.Items.Clear()
        cbeLayerSetup.Properties.Items.Add("Standard")
        cbeLayerSetup.Properties.Items.Add("Custom")
    End Sub

    Private Sub cmdOK_Click(sender As Object, e As EventArgs) Handles cmdOK.Click
        ' Validate required fields
        If cbeMachine.SelectedItem Is Nothing Then
            XtraMessageBox.Show("Please select a Machine.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        If cbeToolSetup.SelectedItem Is Nothing Then
            XtraMessageBox.Show("Please select a Tool Setup.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        If cbeMaterial.SelectedItem Is Nothing Then
            XtraMessageBox.Show("Please select a Material.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        'If cbeLayerSetup.SelectedItem Is Nothing Then
        '    XtraMessageBox.Show("Please select a Layer Setup.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
        '    Return
        'End If

        ' Save selections
        SelectedMachineID = DirectCast(cbeMachine.SelectedItem, ComboBoxItem).Value
        SelectedToolSetupID = DirectCast(cbeToolSetup.SelectedItem, ComboBoxItem).Value
        SelectedCustomer = tebCustomer.Text
        SelectedOrderPO = tebOderPO.Text
        SelectedDueDate = If(deDueDate.EditValue IsNot Nothing, CType(deDueDate.EditValue, DateTime?), Nothing)
        SelectedMaterialID = DirectCast(cbeMaterial.SelectedItem, ComboBoxItem).Value
        SelectedLayerSetup = cbeLayerSetup.Text

        '' Optional material dimensions
        'If tebMaterialLength.Visible Then MaterialLength = Val(tebMaterialLength.Text)
        'If tebMaterialWidth.Visible Then MaterialWidth = Val(tebMaterialWidth.Text)
        'If tebMaterialThickness.Visible Then MaterialThickness = Val(tebMaterialThickness.Text)

        Me.DialogResult = DialogResult.OK
        Me.Close()
    End Sub

    Private Sub cmdCancel_Click(sender As Object, e As EventArgs) Handles cmdCancel.Click
        Me.DialogResult = DialogResult.Cancel
        Me.Close()
    End Sub

    Private Sub cbeMaterial_Click(sender As Object, e As EventArgs) Handles cbeMaterial.Click
        ' Create the popup
        Dim picker As New MaterialPickerPopup()

        ' Position the popup near the combo box
        Dim pt As Point = cbeMaterial.PointToScreen(Point.Empty)
        picker.StartPosition = FormStartPosition.Manual
        picker.Location = New Point(pt.X, pt.Y + cbeMaterial.Height)

        ' Load the material tree
        picker.LoadMaterialTree()

        ' Show the popup as dialog
        If picker.ShowDialog() = DialogResult.OK Then
            ' Get the selected sheet
            Dim selectedSheet = picker.SelectedSheet
            If selectedSheet IsNot Nothing Then
                ' Display in the combo box
                cbeMaterial.Text = selectedSheet.Description

                ' Save any additional info as needed
                ' Example: store the selected sheet ID
                Dim sheetID As Integer = selectedSheet.ID
                Dim typeID As Integer = selectedSheet.MaterialTypeID
                ' You can also populate any hidden fields, variables, or update PropertyGrid

                MessageBox.Show($"Selected Material: {selectedSheet.Description}")
            End If
        End If
    End Sub

    Private Sub UpdateToolSetupCombo()
        cbeToolSetup.Properties.Items.Clear()
        cbeToolSetup.Text = Nothing

        If cbeMachine.SelectedItem Is Nothing Then Return


        Dim selectedMachineID As Integer = DirectCast(cbeMachine.SelectedItem, KeyValuePair(Of String, Integer)).Value

        '' Filter ToolSetups for this machine
        'Dim filteredToolSetups = AppData.ToolSetups.Where(Function(ts) ts("MachineID")?.ToObject(Of Integer)() = selectedMachineID).ToList()

        'For Each ts In filteredToolSetups
        '    cbeToolSetup.Properties.Items.Add(New ToolSetupItem With {
        '    .ID = ts("ID").ToObject(Of Integer)(),
        '    .Description = ts("Description").ToString()
        '})
        'Next

        ' Optional: select first tool setup automatically
        If cbeToolSetup.Properties.Items.Count > 0 Then cbeToolSetup.SelectedIndex = 0
    End Sub

    ' --- Helper: get currently selected ToolSetup ID ---
    Private Function GetSelectedToolSetupID() As Integer
        Dim selected As ToolSetupItem = TryCast(cbeToolSetup.SelectedItem, ToolSetupItem)
        If selected IsNot Nothing Then
            Return selected.ID
        End If
        Return -1
    End Function

    ' Simple helper class
    Private Class ComboBoxItem
        Public Property Text As String
        Public Property Value As Integer
        Public Sub New(t As String, v As Integer)
            Text = t
            Value = v
        End Sub
        Public Overrides Function ToString() As String
            Return Text
        End Function
    End Class

    Private Sub cbeMaterial_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeMaterial.SelectedIndexChanged

    End Sub
End Class