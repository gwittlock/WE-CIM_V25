<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlgAddParam
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
        Me.cboParamDataType = New System.Windows.Forms.ComboBox()
        Me.lblParamDataType = New System.Windows.Forms.Label()
        Me.lblParamName = New System.Windows.Forms.Label()
        Me.txtParamName = New System.Windows.Forms.TextBox()
        Me.lblParamDisplay = New System.Windows.Forms.Label()
        Me.txtParamDisplay = New System.Windows.Forms.TextBox()
        Me.lblParamDefault = New System.Windows.Forms.Label()
        Me.txtParamDefault = New System.Windows.Forms.TextBox()
        Me.btnParamOk = New System.Windows.Forms.Button()
        Me.btnParamCancel = New System.Windows.Forms.Button()
        Me.chkIsVisible = New System.Windows.Forms.CheckBox()
        Me.SuspendLayout()
        '
        'cboParamDataType
        '
        Me.cboParamDataType.FormattingEnabled = True
        Me.cboParamDataType.Items.AddRange(New Object() {"Double", "Integer", "String", "DropDown", "File", "Folder", "Color"})
        Me.cboParamDataType.Location = New System.Drawing.Point(135, 21)
        Me.cboParamDataType.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.cboParamDataType.Name = "cboParamDataType"
        Me.cboParamDataType.Size = New System.Drawing.Size(135, 21)
        Me.cboParamDataType.TabIndex = 0
        '
        'lblParamDataType
        '
        Me.lblParamDataType.AutoSize = True
        Me.lblParamDataType.Location = New System.Drawing.Point(71, 24)
        Me.lblParamDataType.Name = "lblParamDataType"
        Me.lblParamDataType.Size = New System.Drawing.Size(60, 13)
        Me.lblParamDataType.TabIndex = 1
        Me.lblParamDataType.Text = "Data Type:"
        '
        'lblParamName
        '
        Me.lblParamName.AutoSize = True
        Me.lblParamName.Location = New System.Drawing.Point(36, 47)
        Me.lblParamName.Name = "lblParamName"
        Me.lblParamName.Size = New System.Drawing.Size(89, 13)
        Me.lblParamName.TabIndex = 2
        Me.lblParamName.Text = "Parameter Name:"
        '
        'txtParamName
        '
        Me.txtParamName.Location = New System.Drawing.Point(135, 45)
        Me.txtParamName.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.txtParamName.Name = "txtParamName"
        Me.txtParamName.Size = New System.Drawing.Size(135, 20)
        Me.txtParamName.TabIndex = 3
        '
        'lblParamDisplay
        '
        Me.lblParamDisplay.AutoSize = True
        Me.lblParamDisplay.Location = New System.Drawing.Point(30, 71)
        Me.lblParamDisplay.Name = "lblParamDisplay"
        Me.lblParamDisplay.Size = New System.Drawing.Size(95, 13)
        Me.lblParamDisplay.TabIndex = 4
        Me.lblParamDisplay.Text = "Parameter Display:"
        '
        'txtParamDisplay
        '
        Me.txtParamDisplay.Location = New System.Drawing.Point(135, 69)
        Me.txtParamDisplay.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.txtParamDisplay.Name = "txtParamDisplay"
        Me.txtParamDisplay.Size = New System.Drawing.Size(135, 20)
        Me.txtParamDisplay.TabIndex = 5
        '
        'lblParamDefault
        '
        Me.lblParamDefault.AutoSize = True
        Me.lblParamDefault.Location = New System.Drawing.Point(56, 95)
        Me.lblParamDefault.Name = "lblParamDefault"
        Me.lblParamDefault.Size = New System.Drawing.Size(74, 13)
        Me.lblParamDefault.TabIndex = 6
        Me.lblParamDefault.Text = "Default Value:"
        '
        'txtParamDefault
        '
        Me.txtParamDefault.Location = New System.Drawing.Point(135, 93)
        Me.txtParamDefault.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.txtParamDefault.Name = "txtParamDefault"
        Me.txtParamDefault.Size = New System.Drawing.Size(135, 20)
        Me.txtParamDefault.TabIndex = 7
        '
        'btnParamOk
        '
        Me.btnParamOk.Location = New System.Drawing.Point(136, 151)
        Me.btnParamOk.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnParamOk.Name = "btnParamOk"
        Me.btnParamOk.Size = New System.Drawing.Size(64, 18)
        Me.btnParamOk.TabIndex = 8
        Me.btnParamOk.Text = "Ok"
        Me.btnParamOk.UseVisualStyleBackColor = True
        '
        'btnParamCancel
        '
        Me.btnParamCancel.Location = New System.Drawing.Point(206, 151)
        Me.btnParamCancel.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnParamCancel.Name = "btnParamCancel"
        Me.btnParamCancel.Size = New System.Drawing.Size(64, 18)
        Me.btnParamCancel.TabIndex = 9
        Me.btnParamCancel.Text = "Cancel"
        Me.btnParamCancel.UseVisualStyleBackColor = True
        '
        'chkIsVisible
        '
        Me.chkIsVisible.AutoSize = True
        Me.chkIsVisible.CheckAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.chkIsVisible.Location = New System.Drawing.Point(63, 116)
        Me.chkIsVisible.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.chkIsVisible.Name = "chkIsVisible"
        Me.chkIsVisible.Size = New System.Drawing.Size(82, 17)
        Me.chkIsVisible.TabIndex = 10
        Me.chkIsVisible.Text = "chkIsVisible"
        Me.chkIsVisible.UseVisualStyleBackColor = True
        '
        'dlgAddParam
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(292, 178)
        Me.Controls.Add(Me.chkIsVisible)
        Me.Controls.Add(Me.btnParamCancel)
        Me.Controls.Add(Me.btnParamOk)
        Me.Controls.Add(Me.txtParamDefault)
        Me.Controls.Add(Me.lblParamDefault)
        Me.Controls.Add(Me.txtParamDisplay)
        Me.Controls.Add(Me.lblParamDisplay)
        Me.Controls.Add(Me.txtParamName)
        Me.Controls.Add(Me.lblParamName)
        Me.Controls.Add(Me.lblParamDataType)
        Me.Controls.Add(Me.cboParamDataType)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow
        Me.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.Name = "dlgAddParam"
        Me.Text = "Add Machine Parameter"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents cboParamDataType As System.Windows.Forms.ComboBox
    Friend WithEvents lblParamDataType As Label
    Friend WithEvents lblParamName As Label
    Friend WithEvents txtParamName As TextBox
    Friend WithEvents lblParamDisplay As Label
    Friend WithEvents txtParamDisplay As TextBox
    Friend WithEvents lblParamDefault As Label
    Friend WithEvents txtParamDefault As TextBox
    Friend WithEvents btnParamOk As Button
    Friend WithEvents btnParamCancel As Button
    Friend WithEvents chkIsVisible As CheckBox
End Class
