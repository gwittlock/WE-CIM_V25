Public Class frmEntityProperties

    Public Sub ShowSelectedObject(selectedObject As Object)

        pgEntityProperties.SelectedObject = selectedObject
        pgEntityProperties.Enabled = False

    End Sub



    Private Sub frmEntityProperties_Load(sender As Object, e As EventArgs) Handles MyBase.Load

    End Sub
End Class