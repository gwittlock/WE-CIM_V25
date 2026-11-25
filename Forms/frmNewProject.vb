Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraTreeList
Imports DevExpress.XtraTreeList.Nodes
Imports FabV25_WIN8.WE_ENG_V25_0.Core.State
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.Material
Imports FabV25_WIN8.AppData
Imports System.IO
Imports IxMilia.Dxf
Imports FabV25_WIN8.MateriaWrapper
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Helpers
Imports FabV25_WIN8.MaterialWrapper
Imports FabV25_WIN8.MachineDefinition

Public Class frmNewProject
    ' ----------------------------
    ' Private Fields
    ' ----------------------------

    Private CFG_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_CATEGORY_NODE As TreeListNode
    Private CFG_MATERIAL_NODE As TreeListNode
    Private Const TAG_MATERIAL_ROOT_NODE = -200
    Private Const TAG_MATERIAL_CATEGORY_NODE = -201
    Private Const TAG_MATERIAL_NODE = -202
    Dim sUsage As String

    Private _machines As List(Of Machine)
    Private _toolSetups As List(Of ToolSetup)
    Private _layerSetups As List(Of layersetup)
    Private _materialTypes As List(Of MaterialType)
    Private _materialSheets As List(Of MaterialSheet)


    Public CustomerName As String
    Public OrderPO As String
    Public DueDate As DateTime
    Public _material As Material

    Private sMyToolSetup As String
    Private sMyLayerSetup As String
    Private sMyMaterial As String
    Private sMyMachine As String
    Private nMyToolSetupID As Integer
    Private nMyLayerSetupID As Integer
    Private nMyMaterialID As Integer
    Private nMyMachineID As Integer

    Private _loadingDefaults As Boolean = False


    ' Material picker control
    Private matPicker As MaterialPickerTree

    ' ----------------------------
    ' Constructor
    ' ----------------------------
    Public Sub New()
        InitializeComponent()

    End Sub

    Private Sub frmNewProject_Load(sender As Object, e As EventArgs) Handles Me.Load

        _loadingDefaults = True

        sMyToolSetup = RegGetString("Global", "ToolSetup", "PunchDemo6")
        sMyLayerSetup = RegGetString("Global", "LayerSetup", "DXFDemo")
        sMyMaterial = RegGetString("Global", "Material", "ALUM .250 X 96 X 48")
        sMyMachine = RegGetString("Global", "Machine", "Amada Pega")

        nMyToolSetupID = RegGetInt("Global", "ToolSetupID", 55)
        nMyLayerSetupID = RegGetInt("Global", "LayerSetupID", 0)
        nMyMaterialID = RegGetInt("Global", "MaterialID", 5)
        nMyMachineID = RegGetInt("Global", "MachineID", 2)



        ' Load all JSON data
        _machines = AppData.Machines
        _toolSetups = AppData.ToolSetups
        _layerSetups = AppData.LayerSetups
        SelectedLayerSetupID = nMyLayerSetupID

        _selectedLayerSetup =
    _layerSetups.FirstOrDefault(
        Function(ls) ls.ID = SelectedLayerSetupID)


        _materialTypes = AppData.MaterialTypes
        _materialSheets = AppData.MaterialSheets

        ' Initialize ComboBoxes
        InitMachineCombo()

        cboMaterial.Visible = False ' hide old combo

        ' Set optional fields defaults
        dtDueDate.EditValue = DateTime.Now


        MatListTree1.LoadMaterials(
            AppData.MaterialTypes,
            AppData.MaterialSheets
        )

        _loadingDefaults = False
    End Sub


    Private Sub RestoreComboSelection(combo As DevExpress.XtraEditors.ComboBoxEdit, id As Integer)

        If id <= 0 Then Return

        For Each item In combo.Properties.Items
            Dim data = TryCast(item, myItemData)
            If data IsNot Nothing AndAlso data.ItemData = id Then
                'Combo.EditValue = data.ItemData
                combo.SelectedItem = item
                Return
            End If
        Next

    End Sub
    ' ----------------------------
    ' Initialize Machine ComboBox
    ' ----------------------------
    Private Sub InitMachineCombo()
        Dim ni As Integer
        Dim selectedMachine As myItemData = Nothing

        cboMachine.Properties.Items.Clear()

        ' Add None
        cboMachine.Properties.Items.Add(New myItemData("None", 0))

        For Each m In _machines
            Dim item As New myItemData(m.Description, m.ID)
            cboMachine.Properties.Items.Add(item)

            If m.ID = nMyMachineID Then
                selectedMachine = item   ' ← remember the exact instance we just added
            End If
        Next

        cboMachine.EditValue = selectedMachine

    End Sub

    ' ----------------------------
    ' Initialize Machine ComboBox
    ' ----------------------------
    Private Sub InitMaterialCombo()
        Dim ni As Integer
        Dim selectedMaterial As myItemData = Nothing

        'cboMachine.Properties.Items.Clear()

        ' Add None
        'cboMachine.Properties.Items.Add(New myItemData("None", 0))

        For Each m In _materialSheets
            Dim item As New myItemData(m.Description, m.ID)
            ' cbmaterial.Properties.Items.Add(item)

            If m.ID = nMyMaterialID Then
                selectedMaterial = item   ' ← remember the exact instance we just added
            End If
        Next

        cboMaterial.EditValue = selectedMaterial

    End Sub


    Private Sub cboMachine_EditValueChanged(sender As Object, e As EventArgs) Handles cboMachine.EditValueChanged
        Dim selectedToolSetup As myItemData = Nothing
        'If _loadingDefaults Then Return

        Dim selectedItem = TryCast(cboMachine.SelectedItem, myItemData)
        If selectedItem Is Nothing Then Return

        SelectedMachineID = selectedItem.ItemData


        ' Filter ToolSetup ComboBox for this machine
        cboToolSetup.Properties.Items.Clear()
        cboToolSetup.Properties.Items.Add(New myItemData("None", 0))

        Dim setups = _toolSetups.Where(Function(ts) ts.MachineID = SelectedMachineID)

        For Each ts In setups
            Dim item As New myItemData(ts.Description, ts.ID)
            cboToolSetup.Properties.Items.Add(item)

            If ts.ID = nMyToolSetupID Then
                selectedToolSetup = item   ' ← remember the exact instance we just added
            End If
        Next

        cboToolSetup.EditValue = selectedToolSetup


    End Sub
    Private Sub cboToolSetup_EditValueChanged(sender As Object, e As EventArgs) Handles cboToolSetup.EditValueChanged

        Dim selectedItem = TryCast(cboToolSetup.SelectedItem, myItemData)

        SelectedToolSetupID = 0
        _selectedToolSetup = Nothing

        If selectedItem IsNot Nothing AndAlso selectedItem.ItemData > 0 Then

            SelectedToolSetupID = selectedItem.ItemData

            _selectedToolSetup = ToolSetups.FirstOrDefault(
            Function(ts) ts.ID = SelectedToolSetupID)

        End If

        ' Populate compatible Layer Setups.
        LoadLayerSetupsForToolSetup(
        SelectedMachineID,
        SelectedToolSetupID)

    End Sub




    Private Sub cmdCancel_Click(sender As Object, e As EventArgs) Handles cmdCancel.Click
        Me.DialogResult = DialogResult.Cancel
        Me.Close()
    End Sub


    Private Sub cmdOK_DragDrop(sender As Object, e As DragEventArgs) Handles cmdOK.DragDrop

    End Sub

    Private Sub frmOpenProject_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        MatListTree1.SelectMaterialByID(nMyMaterialID)
    End Sub

    Private Sub popupContainerEdit1_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs) Handles popupContainerEdit1.QueryDisplayText
        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")
    End Sub


    Private Sub MatListTree1_MaterialSelected(
    sender As Object,
    e As EventArgs) Handles MatListTree1.MaterialSelected

        Dim sheet As MaterialSheet = MatListTree1.SelectedMaterial

        If sheet Is Nothing Then Return

        tebMaterialLength.Text =
            GetMaterialLength(MaterialSheets, sheet.ID).ToString()

        tebMaterialWidth.Text =
            GetMaterialWidth(MaterialSheets, sheet.ID).ToString()

        tebMaterialThickness.Text =
            modMaterial.GetMaterialThickness(MaterialSheets, sheet.ID).ToString()

    End Sub
    Private Sub cmdOK_Click(sender As Object, e As EventArgs) Handles cmdOK.Click

        Try

            Dim machineItem = TryCast(cboMachine.SelectedItem, myItemData)
            Dim toolSetupItem = TryCast(cboToolSetup.SelectedItem, myItemData)

            If machineItem Is Nothing OrElse machineItem.ItemData <= 0 Then
                MessageBox.Show("Please select a machine.", "Validation",
                            MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Return
            End If

            If toolSetupItem Is Nothing OrElse toolSetupItem.ItemData <= 0 Then
                MessageBox.Show("Please select a tool setup.", "Validation",
                            MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Return
            End If

            If MatListTree1.SelectedMaterial Is Nothing OrElse
           MatListTree1.SelectedMaterialID <= 0 Then

                MessageBox.Show("Please select a material.", "Validation",
                            MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Return

            End If

            ' Store selected configuration.
            SelectedMachineID = machineItem.ItemData
            SelectedToolSetupID = toolSetupItem.ItemData

            _selectedToolSetup = ToolSetups.FirstOrDefault(
            Function(ts) ts.ID = SelectedToolSetupID)

            _SelectedMaterialID = MatListTree1.SelectedMaterialID
            _selectedMaterial = MatListTree1.SelectedMaterial

            ' Validate the selected Layer Setup.
            Dim layerSetupItem = TryCast(cboLayerSetup.SelectedItem, myItemData)

            If layerSetupItem Is Nothing OrElse layerSetupItem.ItemData <= 0 Then

                MessageBox.Show("Please select a layer setup.", "Validation",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Return

            End If

            SelectedLayerSetupID = layerSetupItem.ItemData

            _selectedLayerSetup = _layerSetups.FirstOrDefault(
    Function(ls) ls.ID = SelectedLayerSetupID AndAlso
                 ls.MachineID = SelectedMachineID AndAlso
                 ls.ToolSetupID = SelectedToolSetupID)

            If _selectedLayerSetup Is Nothing Then

                MessageBox.Show("The selected Layer Setup is not compatible with the selected Machine and Tool Setup.",
                    "Validation",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Warning)
                Return

            End If

            ' Store optional job information.
            CustomerName = txtCustomer.Text
            OrderPO = txtOrderPO.Text
            DueDate = dtDueDate.EditValue

            ' Preserve last-used configuration.
            RegPutString("Global", "Machine", cboMachine.Text)
            RegPutString("Global", "MachineID", CStr(SelectedMachineID))

            RegPutString("Global", "ToolSetup", cboToolSetup.Text)
            RegPutString("Global", "ToolSetupID", CStr(SelectedToolSetupID))

            RegPutString("Global", "Material", _selectedMaterial.Description)
            RegPutString("Global", "MaterialID", CStr(_SelectedMaterialID))

            Me.DialogResult = DialogResult.OK
            Me.Close()

        Catch ex As Exception

            MessageBox.Show("frmNewProject cmdOK_Click: " & ex.Message)

        End Try

    End Sub
    Private Sub cboLayerSetup_EditValueChanged(sender As Object, e As EventArgs) Handles cboLayerSetup.EditValueChanged

        Dim selectedItem = TryCast(cboLayerSetup.SelectedItem, myItemData)

        SelectedLayerSetupID = 0
        _selectedLayerSetup = Nothing

        If selectedItem Is Nothing OrElse selectedItem.ItemData <= 0 Then
            Return
        End If

        SelectedLayerSetupID = selectedItem.ItemData

        _selectedLayerSetup = _layerSetups.FirstOrDefault(
        Function(ls) ls.ID = SelectedLayerSetupID)

    End Sub

    Private Sub LoadLayerSetupsForToolSetup(
    machineID As Integer,
    toolSetupID As Integer)

        cboLayerSetup.Properties.Items.Clear()

        cboLayerSetup.Properties.Items.Add(
            New myItemData("None", 0))

        Dim selectedLayerSetup As myItemData = Nothing

        If machineID > 0 AndAlso toolSetupID > 0 Then

            Dim filteredSetups = _layerSetups.
                Where(Function(ls) ls.MachineID = machineID AndAlso
                                   ls.ToolSetupID = toolSetupID).
                ToList()

            For Each ls In filteredSetups

                Dim item As New myItemData(
                    ls.Description,
                    ls.ID)

                cboLayerSetup.Properties.Items.Add(item)

                If ls.ID = nMyLayerSetupID Then
                    selectedLayerSetup = item
                End If

            Next

        End If

        If selectedLayerSetup IsNot Nothing Then
            cboLayerSetup.EditValue = selectedLayerSetup
        Else
            cboLayerSetup.SelectedIndex = 0
        End If

    End Sub
End Class