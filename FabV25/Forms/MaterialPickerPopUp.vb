Imports System.Windows.Forms
Imports System.Linq

Public Class MaterialPickerPopup
    Inherits Form

    ' Selected sheet (UI-friendly)
    Public Property SelectedSheet As SheetItem = Nothing

    ' TreeView for material selection
    Private tv As New TreeView With {
        .Dock = DockStyle.Fill,
        .HideSelection = False
    }

    ' Constructor
    Public Sub New()
        Me.FormBorderStyle = FormBorderStyle.FixedToolWindow
        Me.StartPosition = FormStartPosition.Manual
        Me.Width = 350
        Me.Height = 400

        Me.Controls.Add(tv)

        ' Handle double-click on nodes
        AddHandler tv.NodeMouseDoubleClick, AddressOf OnNodeDoubleClick
    End Sub

    ' Load tree from AppData
    Public Sub LoadMaterialTree()
        tv.Nodes.Clear()

        ' Group all sheets by their MaterialTypeID
        Dim grouped = AppData.MaterialSheets.
        GroupBy(Function(s) s.TypeID).
        ToList()

        For Each g In grouped
            ' Get the material type for this group
            Dim typeRow = AppData.MaterialTypes.FirstOrDefault(Function(t) t.ID = g.Key)
            If typeRow Is Nothing Then Continue For

            ' Create parent node for material type
            Dim typeNode As New TreeNode(typeRow.Description) With {
            .Tag = Nothing
        }

            ' Add child nodes for each sheet
            For Each s In g
                ' Use the MaterialSheet Description if set; otherwise, fall back to Values list
                Dim descParam = s.Values.FirstOrDefault(Function(p) p.Name.Equals("Description", StringComparison.OrdinalIgnoreCase))
                Dim desc As String = If(Not String.IsNullOrEmpty(s.Description), s.Description,
                                     If(descParam IsNot Nothing, descParam.Value?.ToString(), "Unnamed Sheet"))

                ' Create SheetItem object
                Dim sheet As New SheetItem With {
                .ID = s.ID,
                .MaterialTypeID = s.TypeID,
                .Thickness = s.Thickness,
                .Length = s.Length,
                .Width = s.Width,
                .Description = $"{desc} ({s.Length} x {s.Width} x {s.Thickness})"
            }

                ' Add node for this sheet
                Dim child As New TreeNode(sheet.Description) With {
                .Tag = sheet
            }
                typeNode.Nodes.Add(child)
            Next

            tv.Nodes.Add(typeNode)
        Next

        tv.ExpandAll()
    End Sub


    ' Handle double-click to select sheet
    Private Sub OnNodeDoubleClick(sender As Object, e As TreeNodeMouseClickEventArgs)
        If e.Node.Tag IsNot Nothing AndAlso TypeOf e.Node.Tag Is SheetItem Then
            SelectedSheet = DirectCast(e.Node.Tag, SheetItem)
            Me.DialogResult = DialogResult.OK
            Me.Close()
        End If
    End Sub

    ' UI-friendly sheet class
    Public Class SheetItem
        Public Property ID As Integer
        Public Property MaterialTypeID As Integer
        Public Property Description As String
        Public Property Length As Decimal
        Public Property Width As Decimal
        Public Property Thickness As Decimal
        Public Overrides Function ToString() As String
            Return Description
        End Function
    End Class
End Class
