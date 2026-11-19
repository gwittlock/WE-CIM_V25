<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmCreatePattern
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.btnOk = New System.Windows.Forms.Button()
        Me.ltPatternName = New FabV25_WIN8.lblTextBox()
        Me.mtbHandleX = New System.Windows.Forms.TextBox()
        Me.mtbHandleY = New System.Windows.Forms.TextBox()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label1.Location = New System.Drawing.Point(82, 0)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(114, 17)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "Create Pattern"
        '
        'btnCancel
        '
        Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnCancel.Location = New System.Drawing.Point(15, 174)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(75, 23)
        Me.btnCancel.TabIndex = 6
        Me.btnCancel.Text = "Cancel"
        Me.btnCancel.UseVisualStyleBackColor = True
        '
        'btnOk
        '
        Me.btnOk.Location = New System.Drawing.Point(191, 174)
        Me.btnOk.Name = "btnOk"
        Me.btnOk.Size = New System.Drawing.Size(75, 23)
        Me.btnOk.TabIndex = 7
        Me.btnOk.Text = "Ok"
        Me.btnOk.UseVisualStyleBackColor = True
        '
        'ltPatternName
        '
        Me.ltPatternName.BackColor = System.Drawing.Color.Transparent
        Me.ltPatternName.Caption = "Pattern Name:"
        Me.ltPatternName.CaptionLocation = FabV25_WIN8.lblTextBox.CaptionLocations.clLeft
        Me.ltPatternName.FieldStyle = FabV25_WIN8.lblTextBox.FieldStyles.fsAuto
        Me.ltPatternName.FieldWidth = 50
        Me.ltPatternName.Location = New System.Drawing.Point(12, 48)
        Me.ltPatternName.MaxLength = 32767
        Me.ltPatternName.Name = "ltPatternName"
        Me.ltPatternName.Size = New System.Drawing.Size(224, 25)
        Me.ltPatternName.TabIndex = 8
        '
        'mtbHandleX
        '
        Me.mtbHandleX.Location = New System.Drawing.Point(85, 79)
        Me.mtbHandleX.Name = "mtbHandleX"
        Me.mtbHandleX.Size = New System.Drawing.Size(151, 20)
        Me.mtbHandleX.TabIndex = 9
        '
        'mtbHandleY
        '
        Me.mtbHandleY.Location = New System.Drawing.Point(85, 105)
        Me.mtbHandleY.Name = "mtbHandleY"
        Me.mtbHandleY.Size = New System.Drawing.Size(151, 20)
        Me.mtbHandleY.TabIndex = 10
        '
        'frmCreatePattern
        '
        Me.AcceptButton = Me.btnOk
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.btnCancel
        Me.ClientSize = New System.Drawing.Size(278, 450)
        Me.Controls.Add(Me.mtbHandleY)
        Me.Controls.Add(Me.mtbHandleX)
        Me.Controls.Add(Me.ltPatternName)
        Me.Controls.Add(Me.btnOk)
        Me.Controls.Add(Me.btnCancel)
        Me.Controls.Add(Me.Label1)
        Me.Name = "frmCreatePattern"
        Me.Text = "frmCreatePattern"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents Label1 As Label
    Friend WithEvents btnCancel As Button
    Friend WithEvents btnOk As Button
    Friend WithEvents ltPatternName As lblTextBox
    Friend WithEvents mtbHandleX As TextBox
    Friend WithEvents mtbHandleY As TextBox
End Class
