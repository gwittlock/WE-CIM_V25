<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> Partial Class LabelComboBox
#Region "Windows Form Designer generated code "
	<System.Diagnostics.DebuggerNonUserCode()> Public Sub New()
		MyBase.New()
		'This call is required by the Windows Form Designer.
		InitializeComponent()
		UserControl_Initialize()
	End Sub
	'Form overrides dispose to clean up the component list.
	<System.Diagnostics.DebuggerNonUserCode()> Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
		If Disposing Then
			If Not components Is Nothing Then
				components.Dispose()
			End If
		End If
		MyBase.Dispose(Disposing)
	End Sub
	'Required by the Windows Form Designer
	Private components As System.ComponentModel.IContainer
	Public ToolTip1 As System.Windows.Forms.ToolTip
	Friend WithEvents cboChoices As System.Windows.Forms.ComboBox
    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.
    'Do not modify it using the code editor.
	<System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.ToolTip1 = New System.Windows.Forms.ToolTip(Me.components)
        Me.cboChoices = New System.Windows.Forms.ComboBox
        Me.lblCaption = New DevExpress.XtraEditors.LabelControl
        Me.SuspendLayout()
        '
        'cboChoices
        '
        Me.cboChoices.BackColor = System.Drawing.SystemColors.Window
        Me.cboChoices.Cursor = System.Windows.Forms.Cursors.Default
        Me.cboChoices.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboChoices.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cboChoices.ForeColor = System.Drawing.SystemColors.WindowText
        Me.cboChoices.Location = New System.Drawing.Point(44, 0)
        Me.cboChoices.Name = "cboChoices"
        Me.cboChoices.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.cboChoices.Size = New System.Drawing.Size(89, 22)
        Me.cboChoices.TabIndex = 0
        '
        'lblCaption
        '
        Me.lblCaption.Location = New System.Drawing.Point(0, 4)
        Me.lblCaption.Name = "lblCaption"
        Me.lblCaption.Size = New System.Drawing.Size(37, 13)
        Me.lblCaption.TabIndex = 1
        Me.lblCaption.Text = "Caption"
        '
        'LabelComboBox
        '
        Me.BackColor = System.Drawing.Color.Transparent
        Me.Controls.Add(Me.lblCaption)
        Me.Controls.Add(Me.cboChoices)
        Me.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Name = "LabelComboBox"
        Me.Size = New System.Drawing.Size(133, 25)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents lblCaption As DevExpress.XtraEditors.LabelControl
#End Region 
#Region "Upgrade Support"
	<System.Runtime.InteropServices.ProgId("KeyPressEventArgs_NET.KeyPressEventArgs")> Public NotInheritable Class KeyPressEventArgs
		Inherits System.EventArgs
        Public KeyAscii As Integer
        Public Sub New(ByRef KeyAscii As Integer)
            MyBase.New()
            Me.KeyAscii = KeyAscii
        End Sub
	End Class
#End Region 
End Class