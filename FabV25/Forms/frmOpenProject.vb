
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

'========================================================
' frmOpenProject.vb
' Fully rewritten version using frmMain global JSON collections
' Handles DXF project opening with machine, tool, material, layer selection
'========================================================

Public Class frmOpenProject
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


    Private _selectedDxfPath As String

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

    ' Holds the imported DXF entities, classified by directive
    Private _importedEntities As List(Of ImportedEntity)

    Private _loadingDefaults As Boolean = False



    ' Stores the currently loaded DXF file path
    Private _currentDxfPath As String

    ' Material picker control
    Private matPicker As MaterialPickerTree
    Dim importer As IDxfImporter = New DxfImporter_IxMilia()



    ' ----------------------------
    ' Constructor
    ' ----------------------------
    Public Sub New(selectedDxfPath As String)
        InitializeComponent()
        _selectedDxfPath = selectedDxfPath
    End Sub

    ' ----------------------------
    ' Form Load
    ' ----------------------------
    Private Sub frmOpenProject_Load(sender As Object, e As EventArgs) Handles MyBase.Load

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
        'InitLayerSetupCombo()

        ' Initialize Material Picker
        'matPicker = New MaterialPickerTree With {
        '    .Location = cboMaterial.Location,
        '    .Size = cboMaterial.Size
        '}
        ' Me.Controls.Add(matPicker)
        cboMaterial.Visible = False ' hide old combo
        ' matPicker.LoadMaterials(_materialTypes, _materialSheets)

        ' Set optional fields defaults
        dtDueDate.EditValue = DateTime.Now


        MatListTree1.LoadMaterials(
            AppData.MaterialTypes,
            AppData.MaterialSheets
        )



        ' Restore selections safely by ID
        'RestoreComboSelection(cboMachine, nMyMachineID)
        'RestoreComboSelection(cboToolSetup, nMyToolSetupID)
        ' RestoreComboSelection(cboMaterial, nMyMaterialID)
        ' RestoreComboSelection(cboLayerSetup, nMyLayerSetupID)

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
    ' ----------------------------
    ' Initialize LayerSetup ComboBox
    ' ----------------------------
    Private Sub InitLayerSetupCombo()
        cboLayerSetup.Properties.Items.Clear()

        ' Add None
        cboLayerSetup.Properties.Items.Add(New myItemData("None", 0))

        For Each ls In _layerSetups
            cboLayerSetup.Properties.Items.Add(New myItemData(ls.Description, ls.ID))
        Next

        'cboLayerSetup.SelectedIndex = 0
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


        ' Reload Layer Setups filtered by machine
        LoadLayerSetupsForMachine(SelectedMachineID)

    End Sub

    Private Sub cboToolSetup_EditValueChanged(sender As Object, e As EventArgs) Handles cboToolSetup.EditValueChanged

        'If _loadingDefaults Then Return

        Dim selectedItem = TryCast(cboToolSetup.SelectedItem, myItemData)
        If selectedItem Is Nothing Then Return
        SelectedToolSetupID = selectedItem.ItemData
        _selectedToolSetup = ToolSetups.FirstOrDefault(Function(ts) ts.ID = SelectedToolSetupID)
    End Sub

    Private Sub cboLayerSetup_EditValueChanged(sender As Object, e As EventArgs) Handles cboLayerSetup.EditValueChanged

        If _loadingDefaults Then Return

        Dim selectedItem = TryCast(cboLayerSetup.SelectedItem, myItemData)
        If selectedItem Is Nothing Then Return
        If UCase(selectedItem.Name) = "NONE" Then Return

        ' Update the selected ID
        SelectedLayerSetupID = selectedItem.ItemData

        ' Lookup the selected LayerSetup object
        _selectedLayerSetup = LayerSetups.FirstOrDefault(Function(ls) ls.ID = SelectedLayerSetupID)

        If _selectedLayerSetup Is Nothing Then
            MessageBox.Show("Selected Layer Setup not found.")
            Return
        End If

    End Sub


    Private Sub cmdOK_Click(sender As Object, e As EventArgs) Handles cmdOK.Click

        Try

            ' Validate selections
            If SelectedMachineID = 0 Then
                MessageBox.Show("Please select a machine.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Exit Sub
            End If

        If SelectedToolSetupID = 0 Then
            MessageBox.Show("Please select a tool setup.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Exit Sub
            End If

            If MatListTree1.SelectedMaterialID = -1 Then
                MessageBox.Show("Please select a material.", "Validation",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Exit Sub

            End If
            If MatListTree1.SelectedMaterialID = 0 Then

                MessageBox.Show("Please select a material.", "Validation",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Exit Sub
            End If

            ' Lookup the selected LayerSetup object
            _selectedLayerSetup = LayerSetups.FirstOrDefault(Function(ls) ls.ID = SelectedLayerSetupID)

            If _selectedLayerSetup Is Nothing Then
                MessageBox.Show("Selected Layer Setup not found.")
                Return
            End If


            ' Save optional fields
            CustomerName = txtCustomer.Text
        OrderPO = txtOrderPO.Text
        DueDate = dtDueDate.EditValue
        Dim selectedMaterialID As Integer = MatListTree1.SelectedMaterialID
        _SelectedMaterialID = selectedMaterialID
        _selectedMaterial = MatListTree1.SelectedMaterial

        RegPutString("Global", "Machine", cboMachine.Text)
        RegPutString("Global", "MachineID", CStr(SelectedMachineID))
        RegPutString("Global", "ToolSetup", cboToolSetup.Text)
        RegPutString("Global", "ToolSetupID", CStr(SelectedToolSetupID))
        RegPutString("Global", "Material", MatListTree1.SelectedMaterial.Description)
        RegPutString("Global", "MaterialID", CStr(selectedMaterialID))
        RegPutString("Global", "LayerSetup", cboLayerSetup.Text)
        RegPutString("Global", "LayerSetupID", CStr(SelectedLayerSetupID))

        _DoPrepareUI = True

        ' Close dialog
        Me.DialogResult = DialogResult.OK
            Me.Close()


        Catch ex As Exception
            MessageBox.Show("frmOpenProject cmdOK_Click: " & ex.Message)
        End Try

    End Sub

    Private Sub cmdCancel_Click(sender As Object, e As EventArgs) Handles cmdCancel.Click
        Me.DialogResult = DialogResult.Cancel
        Me.Close()
    End Sub


    Private Sub LoadLayerSetupsForMachine(machineID As Integer)

        cboLayerSetup.Properties.Items.Clear()

        Dim filteredSetups = _layerSetups.
        Where(Function(l) l.MachineID = machineID).
        ToList()

        ' Add default "None" item
        cboLayerSetup.Properties.Items.Add(
        New myItemData("None", 0))

        Dim selectedLayerSetup As myItemData = Nothing

        For Each ls In filteredSetups

            Dim item As New myItemData(
            ls.Description,
            ls.ID)

            cboLayerSetup.Properties.Items.Add(item)

            If ls.ID = nMyLayerSetupID Then
                selectedLayerSetup = item
            End If

        Next

        ' Restore the saved Layer Setup if it belongs to this machine.
        ' Otherwise leave the selection at None.
        If selectedLayerSetup IsNot Nothing Then
            cboLayerSetup.EditValue = selectedLayerSetup
        Else
            cboLayerSetup.SelectedIndex = 0
        End If

    End Sub

    Private Sub cmdOK_DragDrop(sender As Object, e As DragEventArgs) Handles cmdOK.DragDrop

    End Sub

    Private Sub frmOpenProject_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        InitMaterialCombo()
    End Sub

    Private Sub popupContainerEdit1_EditValueChanged(sender As Object, e As EventArgs) Handles popupContainerEdit1.EditValueChanged
        Dim sPieces() As String
        Try


            If (popupContainerEdit1.Tag IsNot Nothing) Then
                sPieces = Split(popupContainerEdit1.Tag, ",")

                Dim materialID As Long

                'MsgBox("The material ID is < " & e.ToString)

                materialID = (sPieces(1))

                tebMaterialLength.Text = GetMaterialLength(MaterialSheets, _SelectedMaterialID)
                tebMaterialWidth.Text = GetMaterialWidth(MaterialSheets, _SelectedMaterialID)
                tebMaterialThickness.Text = modMaterial.GetMaterialThickness(MaterialSheets, _SelectedMaterialID)
            End If

        Catch ex As Exception
            MessageBox.Show("popupContainerEdit1_EditValueChanged: " & ex.Message)
        End Try
        'EnableOkButton()
    End Sub

    Private Sub popupContainerEdit1_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs) Handles popupContainerEdit1.QueryDisplayText
        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")
    End Sub
End Class



