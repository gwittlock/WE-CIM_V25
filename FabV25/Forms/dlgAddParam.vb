Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.IO
Imports FabV25_WIN8.ConfigurationManagerForm
Imports FabV25_WIN8.frmMain

Public Class dlgAddParam
    Public Shared MyWhichNode As Integer
    Public Shared IsAdding As Boolean

    Private Sub btnParamOk_Click(sender As Object, e As EventArgs) Handles btnParamOk.Click

        ' Validate required fields
        If String.IsNullOrWhiteSpace(txtParamName.Text) Then
            MessageBox.Show("Parameter Name cannot be empty.",
                            "Validation",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Warning)
            Return
        End If

        If String.IsNullOrWhiteSpace(txtParamDisplay.Text) Then
            MessageBox.Show("Display Name cannot be empty.",
                            "Validation",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Warning)
            Return
        End If

        If cboParamDataType.SelectedItem Is Nothing Then
            MessageBox.Show("Please select a data type.",
                            "Validation",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Warning)
            Return
        End If

        ' Return the entered values to the calling form.
        Me.DialogResult = DialogResult.OK
        Me.Close()

    End Sub

    Private Sub cboParamDataType_Click(sender As Object, e As EventArgs) Handles cboParamDataType.Click

    End Sub
End Class