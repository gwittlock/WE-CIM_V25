<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class lblTextBox
    Inherits System.Windows.Forms.UserControl
#Region "Windows Form Designer generated code "
    'UserControl overrides dispose to clean up the component list.
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
        Me.txtInput = New System.Windows.Forms.TextBox()
        Me.lblCaption = New DevExpress.XtraEditors.LabelControl()
        Me.SuspendLayout()
        '
        'txtInput
        '
        Me.txtInput.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.txtInput.Location = New System.Drawing.Point(39, 3)
        Me.txtInput.Name = "txtInput"
        Me.txtInput.Size = New System.Drawing.Size(92, 20)
        Me.txtInput.TabIndex = 1
        '
        'lblCaption
        '
        Me.lblCaption.Location = New System.Drawing.Point(0, 6)
        Me.lblCaption.Name = "lblCaption"
        Me.lblCaption.Size = New System.Drawing.Size(37, 13)
        Me.lblCaption.TabIndex = 2
        Me.lblCaption.Text = "Caption"
        '
        'lblTextBox
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.Transparent
        Me.Controls.Add(Me.lblCaption)
        Me.Controls.Add(Me.txtInput)
        Me.Name = "lblTextBox"
        Me.Size = New System.Drawing.Size(138, 27)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents txtInput As System.Windows.Forms.TextBox
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
