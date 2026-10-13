Imports DevExpress.XtraEditors
Imports DevExpress.XtraTreeList.Nodes

Public Class MatListTree

    ' Public property to get selected material sheet
    Public Property SelectedMaterial As MaterialSheet

    ' Internal references
    Private _materialTypes As List(Of MaterialType)
    Private _materialSheets As List(Of MaterialSheet)

    ' Call this to load data into the tree
    Public Sub LoadMaterials(materialTypes As List(Of MaterialType), materialSheets As List(Of MaterialSheet))
        _materialTypes = materialTypes
        _materialSheets = materialSheets

        ' Clear existing nodes
        tl.ClearNodes()

        ' Add root
        'Dim rootNode As TreeListNode = tl.AppendNode(New Object() {"Materials"}, Nothing)
        Dim rootNode As TreeListNode = tl.AppendNode(New Object() {"Materials"}, CType(Nothing, TreeListNode))


        ' Populate material types
        For Each matType In _materialTypes
            Dim typeNode As TreeListNode = tl.AppendNode(New Object() {matType.Description}, rootNode)

            ' Populate sheets under this type
            For Each sheet In _materialSheets.Where(Function(s) s.TypeID = matType.ID)
                Dim sheetNode As TreeListNode = tl.AppendNode(New Object() {sheet.Description}, typeNode)
                sheetNode.Tag = sheet
            Next

            typeNode.Expand()
        Next

        rootNode.ExpandAll()
    End Sub

    ' Handle node selection
    Private Sub tl_FocusedNodeChanged(sender As Object, e As DevExpress.XtraTreeList.FocusedNodeChangedEventArgs) Handles tl.FocusedNodeChanged
        If e.Node IsNot Nothing AndAlso TypeOf e.Node.Tag Is MaterialSheet Then
            SelectedMaterial = DirectCast(e.Node.Tag, MaterialSheet)
            popupContainerEdit1.EditValue = SelectedMaterial.Description
            popupContainerEdit1.ClosePopup()
        End If
    End Sub

    ' Ensure popup points to TreeList
    Private Sub MatListTree_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        popupContainerEdit1.Properties.PopupControl = popupContainerControl1
        popupContainerEdit1.Properties.ShowPopupCloseButton = False
    End Sub

    Public ReadOnly Property SelectedMaterialID As Integer
        Get
            If SelectedMaterial IsNot Nothing Then
                Return SelectedMaterial.ID
            End If
            Return -1
        End Get
    End Property

End Class
