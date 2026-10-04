Imports System.Windows.Forms

Public Class MaterialPickerTree
    Inherits UserControl

    ' Public property to get the selected sheet
    Public Property SelectedMaterial As MaterialSheet

    ' Internal controls
    Public WithEvents txtSelected As New TextBox()
    Private WithEvents tvMaterials As New TreeView()

    ' Material data
    Private _materialTypes As List(Of MaterialType)
    Private _materialSheets As List(Of MaterialSheet)

    Public Sub New()
        Me.Width = 250
        Me.Height = 200

        ' TextBox at the top
        txtSelected.ReadOnly = True
        txtSelected.Dock = DockStyle.Top
        txtSelected.Height = 24
        Me.Controls.Add(txtSelected)

        ' TreeView below
        tvMaterials.Dock = DockStyle.Fill
        Me.Controls.Add(tvMaterials)

        ' Initially hidden
        tvMaterials.Visible = False

        ' Toggle tree visibility on click
        AddHandler txtSelected.Click, Sub(sender, e)
                                          tvMaterials.Visible = Not tvMaterials.Visible
                                      End Sub
    End Sub

    ' Load data into the picker
    Public Sub LoadMaterials(materialTypes As List(Of MaterialType), materialSheets As List(Of MaterialSheet))
        _materialTypes = materialTypes
        _materialSheets = materialSheets

        tvMaterials.Nodes.Clear()

        For Each matType In _materialTypes
            Dim typeNode As New TreeNode(matType.Description)
            typeNode.Tag = matType.ID
            tvMaterials.Nodes.Add(typeNode)

            ' Add child sheets
            For Each sheet In _materialSheets.Where(Function(s) s.TypeID = matType.ID)
                Dim sheetNode As New TreeNode(sheet.Description)
                sheetNode.Tag = sheet
                typeNode.Nodes.Add(sheetNode)
            Next

            typeNode.Expand()
        Next
    End Sub

    ' Handle selection
    Private Sub tvMaterials_AfterSelect(sender As Object, e As TreeViewEventArgs) Handles tvMaterials.AfterSelect
        If TypeOf e.Node.Tag Is MaterialSheet Then
            SelectedMaterial = DirectCast(e.Node.Tag, MaterialSheet)
            txtSelected.Text = SelectedMaterial.Description
            tvMaterials.Visible = False
        End If
    End Sub

    ' Optional: get selected material ID easily
    Public ReadOnly Property SelectedMaterialID As Integer
        Get
            If SelectedMaterial IsNot Nothing Then
                Return SelectedMaterial.ID
            End If
            Return -1
        End Get
    End Property
End Class
