Imports System
Imports System.Linq
Imports System.Collections.Generic
Imports System.Windows.Forms

Public Class MaterialPicker
    Inherits UserControl

    ' ComboBoxes for material type and sheet
    Private WithEvents cboMaterialType As New ComboBox()
    Private WithEvents cboMaterialSheet As New ComboBox()

    ' Selected material info
    Public Property SelectedMaterialSheet As SheetItem

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
        ' Layout
        Me.Controls.Add(cboMaterialType)
        Me.Controls.Add(cboMaterialSheet)
        cboMaterialType.Top = 0
        cboMaterialType.Left = 0
        cboMaterialType.Width = 200
        cboMaterialSheet.Top = 30
        cboMaterialSheet.Left = 0
        cboMaterialSheet.Width = 200

        ' Populate material types
        LoadMaterialTypes()
    End Sub

    Private Sub LoadMaterialTypes()
        cboMaterialType.Items.Clear()
        For Each mt In AppData.MaterialTypes
            cboMaterialType.Items.Add(New KeyValuePair(Of String, Integer)(mt.Description, mt.ID))
        Next
        If cboMaterialType.Items.Count > 0 Then cboMaterialType.SelectedIndex = 0
    End Sub

    Private Sub cboMaterialType_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboMaterialType.SelectedIndexChanged
        LoadMaterialSheets()
    End Sub

    Private Sub LoadMaterialSheets()
        cboMaterialSheet.Items.Clear()
        If cboMaterialType.SelectedItem Is Nothing Then Return

        Dim typeID = DirectCast(cboMaterialType.SelectedItem, KeyValuePair(Of String, Integer)).Value

        Dim sheets = AppData.MaterialSheets.Where(Function(s) s.TypeID = typeID).ToList()
        For Each s In sheets
            cboMaterialSheet.Items.Add(New SheetItem() With {
                .ID = s.ID,
                .Name = s.Description,
                .TypeID = s.TypeID,
                .Length = s.Length,
                .Width = s.Width,
                .Thickness = s.Thickness
            })
        Next

        If cboMaterialSheet.Items.Count > 0 Then cboMaterialSheet.SelectedIndex = 0
    End Sub

    Private Sub cboMaterialSheet_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboMaterialSheet.SelectedIndexChanged
        SelectedMaterialSheet = TryCast(cboMaterialSheet.SelectedItem, SheetItem)
    End Sub

    ' Optional helper: get selected sheet as string
    Public Function GetSelectedMaterialString() As String
        If SelectedMaterialSheet Is Nothing Then Return ""
        Return $"{SelectedMaterialSheet.Name} ({SelectedMaterialSheet.Length} x {SelectedMaterialSheet.Width} x {SelectedMaterialSheet.Thickness})"
    End Function

End Class
