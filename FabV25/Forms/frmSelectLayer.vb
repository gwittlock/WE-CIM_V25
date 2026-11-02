Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraGrid
Imports DevExpress.XtraGrid.Views.Grid
Imports DevExpress.XtraGrid.Views.Grid.ViewInfo

Public Class frmSelectLayer
    Private edit As RepositoryItemCheckEdit
    Public Property GeoEntities As IEnumerable(Of GeoEntity)
    Public Property MaterialDisplayColor As System.Drawing.Color


    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.

    End Sub

    Public Property SelectedLayerName As String

    Private Sub frmSelectLayer_Load(sender As Object, e As EventArgs) Handles Me.Load

        LoadLayersList()

    End Sub

    Private Sub LoadLayersList()

        AddHandler gViewToolLayer.RowStyle, AddressOf gViewToolLayer_RowStyle

        Dim lst As New List(Of TheTools)

        If GeoEntities Is Nothing Then
            Return
        End If

        For Each ent As GeoEntity In GeoEntities

            If ent Is Nothing Then
                Continue For
            End If

            Dim layer As Layer = TryCast(ent.Association, Layer)

            If layer Is Nothing Then
                Continue For
            End If

            If String.IsNullOrWhiteSpace(layer.Name) Then
                Continue For
            End If

            If lst.Any(Function(x) String.Equals(
               x.ToolName,
               layer.Name,
               StringComparison.OrdinalIgnoreCase)) Then
                Continue For
            End If

            lst.Add(New TheTools With {
        .ToolName = layer.Name,
        .ToolColor = layer.Color
    })

        Next

        lst.Sort(Function(a, b) String.Compare(
                 a.ToolName,
                 b.ToolName,
                 StringComparison.OrdinalIgnoreCase))

        edit = New RepositoryItemCheckEdit()
        edit.CheckStyle = DevExpress.XtraEditors.Controls.CheckStyles.Radio
        grdToolLayer.RepositoryItems.Add(edit)

        grdToolLayer.DataSource = lst

        gViewToolLayer.Appearance.Row.BackColor = MaterialDisplayColor
        gViewToolLayer.Appearance.Row.Options.UseBackColor = True

        gViewToolLayer.Appearance.Empty.BackColor = MaterialDisplayColor
        gViewToolLayer.Appearance.Empty.Options.UseBackColor = True

        gViewToolLayer.Columns.Clear()

        gViewToolLayer.Columns.AddVisible("ToolHidden", "Visible")
        gViewToolLayer.Columns.AddVisible("ToolName", "Layer Name")
        gViewToolLayer.Columns.AddVisible("ToolColor", "Color")

        gViewToolLayer.Columns("ToolColor").Visible = False
        gViewToolLayer.Columns("ToolHidden").ColumnEdit = edit
    End Sub

    Private Sub gViewToolLayer_RowStyle(sender As Object, e As RowStyleEventArgs) Handles gViewToolLayer.RowStyle

        If e.RowHandle < 0 Then Return

        Dim lst As List(Of TheTools) =
            CType(gViewToolLayer.DataSource, List(Of TheTools))

        Dim item As TheTools = lst(e.RowHandle)

        e.Appearance.ForeColor = item.ToolColor
        e.Appearance.BackColor = MaterialDisplayColor


    End Sub

    Private Class TheTools

        Public Property ToolHidden As Boolean

        Public Property ToolName As String

        Public Property ToolColor As System.Drawing.Color

    End Class
    Private Sub sbOk_Click(sender As Object, e As EventArgs) Handles sbOk.Click

        Dim rowHandle As Integer = gViewToolLayer.FocusedRowHandle

        If rowHandle < 0 Then
            Return
        End If

        SelectedLayerName =
        CStr(gViewToolLayer.GetRowCellValue(rowHandle, "ToolName"))

        Me.DialogResult = DialogResult.OK
        Me.Close()

    End Sub

    Private Sub sbCancel_Click(sender As Object, e As EventArgs) Handles sbCancel.Click

        Me.DialogResult = DialogResult.Cancel
        Me.Close()

    End Sub
End Class