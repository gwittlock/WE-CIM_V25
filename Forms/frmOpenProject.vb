
Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraTreeList
Imports DevExpress.XtraTreeList.Nodes
Imports FabV25_WIN8.WE_ENG_V25_0.Core.State
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.AppData
Imports System.IO
Imports IxMilia.Dxf

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
    Private _toolSetups As List(Of ToolSetUp)
    Private _layerSetups As List(Of LayerSetup)
    Private _materialTypes As List(Of MaterialType)
    Private _materialSheets As List(Of MaterialSheet)

    ' Selected items
    Public SelectedMachineID As Integer
    Public SelectedToolSetupID As Integer
    Public SelectedLayerSetupID As Integer
    Public SelectedMaterialID As Integer
    Public CustomerName As String
    Public OrderPO As String
    Public DueDate As DateTime

    ' Material picker control
    Private matPicker As MaterialPickerTree

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

        ' Load all JSON data
        _machines = AppData.Machines
        _toolSetups = AppData.ToolSetups
        _layerSetups = AppData.LayerSetups
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
    End Sub


    ' ----------------------------
    ' Initialize Machine ComboBox
    ' ----------------------------
    Private Sub InitMachineCombo()
        cboMachine.Properties.Items.Clear()

        ' Add None
        cboMachine.Properties.Items.Add(New myItemData("None", 0))

        For Each m In _machines
            cboMachine.Properties.Items.Add(New myItemData(m.Description, m.ID))
        Next

        cboMachine.SelectedIndex = 0
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

        cboLayerSetup.SelectedIndex = 0
    End Sub

    Private Sub cboMachine_EditValueChanged(sender As Object, e As EventArgs) Handles cboMachine.EditValueChanged
        Dim selectedItem = TryCast(cboMachine.SelectedItem, myItemData)
        If selectedItem Is Nothing Then Return

        SelectedMachineID = selectedItem.ItemData

        ' Filter ToolSetup ComboBox for this machine
        cboToolSetup.Properties.Items.Clear()
        cboToolSetup.Properties.Items.Add(New myItemData("None", 0))

        Dim setups = _toolSetups.Where(Function(ts) ts.MachineID = SelectedMachineID)
        For Each ts In setups
            cboToolSetup.Properties.Items.Add(New myItemData(ts.Description, ts.ID))
        Next

        cboToolSetup.SelectedIndex = 0


        ' Reload Layer Setups filtered by machine
        LoadLayerSetupsForMachine(SelectedMachineID)

    End Sub

    Private Sub cboToolSetup_EditValueChanged(sender As Object, e As EventArgs) Handles cboToolSetup.EditValueChanged
        Dim selectedItem = TryCast(cboToolSetup.SelectedItem, myItemData)
        If selectedItem Is Nothing Then Return
        SelectedToolSetupID = selectedItem.ItemData
    End Sub

    Private Sub cboLayerSetup_EditValueChanged(sender As Object, e As EventArgs) Handles cboLayerSetup.EditValueChanged
        Dim selectedItem = TryCast(cboLayerSetup.SelectedItem, myItemData)
        If selectedItem Is Nothing Then Return
        SelectedLayerSetupID = selectedItem.ItemData
    End Sub


    Private Sub cmdOK_Click(sender As Object, e As EventArgs) Handles cmdOK.Click
        ' Validate selections
        If SelectedMachineID = 0 Then
            MessageBox.Show("Please select a machine.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        If SelectedToolSetupID = 0 Then
            MessageBox.Show("Please select a tool setup.", "Validation", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        If MatListTree1.SelectedMaterialID = 0 Then

            MessageBox.Show("Please select a material.", "Validation",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Save optional fields
        CustomerName = txtCustomer.Text
        OrderPO = txtOrderPO.Text
        DueDate = dtDueDate.EditValue
        Dim selectedMaterialID As Integer = MatListTree1.SelectedMaterialID

        ' Close dialog
        Me.DialogResult = DialogResult.OK
        Me.Close()
    End Sub

    Private Sub cmdCancel_Click(sender As Object, e As EventArgs) Handles cmdCancel.Click
        Me.DialogResult = DialogResult.Cancel
        Me.Close()
    End Sub


    Private Sub popupContainerEdit1_EditValueChanged(sender As Object, e As EventArgs)
        Dim sPieces() As String
        Try


            If (popupContainerEdit1.Tag IsNot Nothing) Then
                sPieces = Split(popupContainerEdit1.Tag, ",")

                Dim materialID As Long

                'MsgBox("The material ID is < " & e.ToString)

                materialID = (sPieces(1))

                tebMaterialLength.Text = modMaterial.GetMaterialLength(materialID)
                tebMaterialWidth.Text = modMaterial.GetMaterialWidth(materialID)
                tebMaterialThickness.Text = modMaterial.GetMaterialThickness(materialID)
            End If

        Catch ex As Exception
            MessageBox.Show("popupContainerEdit1_EditValueChanged: " & ex.Message)
        End Try
        'EnableOkButton()
    End Sub


    Private Sub popupContainerEdit1_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs)
        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")

    End Sub



    Private Sub LoadLayerSetupsForMachine(machineID As Integer)
        cboLayerSetup.Properties.Items.Clear()

        ' Assume _layerSetups is a List(Of LayerSetup) loaded from JSON
        Dim filteredSetups = _layerSetups.Where(Function(l) l.MachineID = machineID).ToList()

        ' Add default "None" item
        cboLayerSetup.Properties.Items.Add(New myItemData("None", 0))

        For Each ls In filteredSetups
            cboLayerSetup.Properties.Items.Add(New myItemData(ls.Description, ls.ID))
        Next

        ' Optionally select first item
        If cboLayerSetup.Properties.Items.Count > 0 Then
            cboLayerSetup.SelectedIndex = 0
        End If
    End Sub


End Class



