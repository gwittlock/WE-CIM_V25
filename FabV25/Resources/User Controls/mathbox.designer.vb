<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated(), ToolboxBitmap(GetType(MathTextBox), "MathTextBox.ToolboxBitmap")> Partial Class MathTextBox
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
	Friend WithEvents txtMath As System.Windows.Forms.TextBox
    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.
    'Do not modify it using the code editor.
	<System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.ToolTip1 = New System.Windows.Forms.ToolTip(Me.components)
        Me.txtMath = New System.Windows.Forms.TextBox
        Me.lblCaption = New DevExpress.XtraEditors.LabelControl
        Me.SuspendLayout()
        '
        'txtMath
        '
        Me.txtMath.AcceptsReturn = True
        Me.txtMath.BackColor = System.Drawing.SystemColors.Window
        Me.txtMath.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.txtMath.Cursor = System.Windows.Forms.Cursors.IBeam
        Me.txtMath.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtMath.ForeColor = System.Drawing.SystemColors.WindowText
        Me.txtMath.Location = New System.Drawing.Point(44, 0)
        Me.txtMath.MaxLength = 0
        Me.txtMath.Name = "txtMath"
        Me.txtMath.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.txtMath.Size = New System.Drawing.Size(61, 20)
        Me.txtMath.TabIndex = 0
        '
        'lblCaption
        '
        Me.lblCaption.Location = New System.Drawing.Point(2, 4)
        Me.lblCaption.Name = "lblCaption"
        Me.lblCaption.Size = New System.Drawing.Size(37, 13)
        Me.lblCaption.TabIndex = 1
        Me.lblCaption.Text = "Caption"
        '
        'MathTextBox
        '
        Me.BackColor = System.Drawing.Color.Transparent
        Me.Controls.Add(Me.lblCaption)
        Me.Controls.Add(Me.txtMath)
        Me.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Name = "MathTextBox"
        Me.Size = New System.Drawing.Size(105, 21)
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