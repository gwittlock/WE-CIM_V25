Public Class frmCreatePattern
    Public ReadOnly Property PatternName As String
        Get
            Return ltPatternName.Text.Trim()
        End Get
    End Property

    Public ReadOnly Property HandleX As Double
        Get
            Return Double.Parse(mtbHandleX.Text)
        End Get
    End Property

    Public ReadOnly Property HandleY As Double
        Get
            Return Double.Parse(mtbHandleY.Text)
        End Get
    End Property

    Private Sub lblPatternName_Click(sender As Object, e As EventArgs)

    End Sub
    Private Sub btnOK_Click(sender As Object, e As EventArgs) Handles btnOk.Click

        If String.IsNullOrWhiteSpace(ltPatternName.Text) Then

            MessageBox.Show(
            "Please enter a Pattern Name.",
            "Create Pattern",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)

            ltPatternName.Focus()
            Return

        End If

        Dim x As Double
        Dim y As Double

        If Not Double.TryParse(mtbHandleX.Text, x) Then

            MessageBox.Show(
            "Please enter a valid Handle X coordinate.",
            "Create Pattern",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)

            mtbHandleX.Focus()
            Return

        End If

        If Not Double.TryParse(mtbHandleY.Text, y) Then

            MessageBox.Show(
            "Please enter a valid Handle Y coordinate.",
            "Create Pattern",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)

            mtbHandleY.Focus()
            Return

        End If

        Me.DialogResult = DialogResult.OK
        Me.Close()

    End Sub

    Private Sub frmCreatePattern_Load(sender As Object, e As EventArgs) Handles Me.Load
        mtbHandleX.Text = 0
        mtbHandleY.Text = 0
    End Sub
End Class