Imports System
Imports System.Linq
Imports System.Collections.Generic
Imports System.Windows.Forms

Public Class MaterialPicker
    Inherits UserControl
    Public Property SelectedMaterial As MaterialSheet

    Private Const COLLAPSED_HEIGHT As Integer = 24
    Private Const EXPANDED_HEIGHT As Integer = 220

    Public WithEvents txtSelected As New TextBox()
    Private WithEvents tvMaterials As New TreeView()

    Private _materialTypes As List(Of MaterialType)
    Private _materialSheets As List(Of MaterialSheet)

    ' Helper class to hold material info
    Public Class SheetItem
        Public Property ID As Integer
        Public Property Name As String
        Public Property TypeID As Integer
        Public Property Length As Decimal
        Public Property Width As Decimal
        Public Property Thickness As Decimal
        Public Overrides Function ToString() As String
            Return Name
        End Function
    End Class

    Public Sub New()
        Me.Width = 250
        Me.Height = COLLAPSED_HEIGHT

        txtSelected.ReadOnly = True
        txtSelected.Dock = DockStyle.Top
        txtSelected.Height = COLLAPSED_HEIGHT
        Me.Controls.Add(txtSelected)

        tvMaterials.Dock = DockStyle.Fill
        tvMaterials.Visible = False
        Me.Controls.Add(tvMaterials)

        AddHandler txtSelected.Click, AddressOf ToggleTree
        AddHandler tvMaterials.Leave, AddressOf CollapseTree
    End Sub

    Private Sub ToggleTree(sender As Object, e As EventArgs)
        If tvMaterials.Visible Then
            CollapseTree(Nothing, EventArgs.Empty)
        Else
            ExpandTree()
        End If
    End Sub

    Private Sub ExpandTree()
        tvMaterials.Visible = True
        Me.Height = EXPANDED_HEIGHT
        Me.BringToFront()
        tvMaterials.Focus()
    End Sub

    Private Sub CollapseTree(sender As Object, e As EventArgs)
        tvMaterials.Visible = False
        Me.Height = COLLAPSED_HEIGHT
    End Sub


    'Private Sub LoadMaterialTypes()
    '    cboMaterialType.Items.Clear()
    '    For Each mt In AppData.MaterialTypes
    '        cboMaterialType.Items.Add(New KeyValuePair(Of String, Integer)(mt.Description, mt.ID))
    '    Next
    '    If cboMaterialType.Items.Count > 0 Then cboMaterialType.SelectedIndex = 0
    'End Sub

    'Private Sub cboMaterialType_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboMaterialType.SelectedIndexChanged
    '    LoadMaterialSheets()
    'End Sub

    'Private Sub LoadMaterialSheets()
    '    cboMaterialSheet.Items.Clear()
    '    If cboMaterialType.SelectedItem Is Nothing Then Return

    '    Dim typeID = DirectCast(cboMaterialType.SelectedItem, KeyValuePair(Of String, Integer)).Value

    '    Dim sheets = AppData.MaterialSheets.Where(Function(s) s.TypeID = typeID).ToList()
    '    For Each s In sheets
    '        cboMaterialSheet.Items.Add(New SheetItem() With {
    '            .ID = s.ID,
    '            .Name = s.Description,
    '            .TypeID = s.TypeID,
    '            .Length = s.Length,
    '            .Width = s.Width,
    '            .Thickness = s.Thickness
    '        })
    '    Next

    '    If cboMaterialSheet.Items.Count > 0 Then cboMaterialSheet.SelectedIndex = 0
    'End Sub

    'Private Sub cboMaterialSheet_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboMaterialSheet.SelectedIndexChanged
    '    SelectedMaterialSheet = TryCast(cboMaterialSheet.SelectedItem, SheetItem)
    'End Sub

    ' Optional helper: get selected sheet as string
    'Public Function GetSelectedMaterialString() As String
    '    If SelectedMaterialSheet Is Nothing Then Return ""
    '    Return $"{SelectedMaterialSheet.Name} ({SelectedMaterialSheet.Length} x {SelectedMaterialSheet.Width} x {SelectedMaterialSheet.Thickness})"
    'End Function

    Private Sub tvMaterials_AfterSelect(sender As Object, e As TreeViewEventArgs) Handles tvMaterials.AfterSelect
        If TypeOf e.Node.Tag Is MaterialSheet Then
            SelectedMaterial = DirectCast(e.Node.Tag, MaterialSheet)
            txtSelected.Text = SelectedMaterial.Description
            CollapseTree(Nothing, EventArgs.Empty)
        End If
    End Sub
End Class
