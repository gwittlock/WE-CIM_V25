<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class UC_picModeler
    Inherits System.Windows.Forms.UserControl

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
        Me.PicBump = New DevExpress.XtraEditors.PanelControl()
        CType(Me.PicBump, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'PicBump
        '
        Me.PicBump.Dock = System.Windows.Forms.DockStyle.Fill
        Me.PicBump.Location = New System.Drawing.Point(0, 0)
        Me.PicBump.Name = "PicBump"
        Me.PicBump.Size = New System.Drawing.Size(150, 70)
        Me.PicBump.TabIndex = 0
        '
        'UC_picModeler
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.PicBump)
        Me.Name = "UC_picModeler"
        Me.Size = New System.Drawing.Size(150, 70)
        CType(Me.PicBump, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents PicBump As DevExpress.XtraEditors.PanelControl

End Class
