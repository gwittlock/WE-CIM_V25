Public Class frmEntityProperties

    Public Sub ShowSelectedObject(selectedObject As Object)

        pgEntityProperties.SelectedObject = selectedObject
        pgEntityProperties.Enabled = True

    End Sub



    Private Sub frmEntityProperties_Load(sender As Object, e As EventArgs) Handles MyBase.Load

        pgEntityProperties.ExpandAllGridItems()

    End Sub

End Class

