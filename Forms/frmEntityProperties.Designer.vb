<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEntityProperties
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
        Me.pgEntityProperties = New System.Windows.Forms.PropertyGrid()
        Me.SuspendLayout()
        '
        'pgEntityProperties
        '
        Me.pgEntityProperties.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pgEntityProperties.Location = New System.Drawing.Point(0, 0)
        Me.pgEntityProperties.Name = "pgEntityProperties"
        Me.pgEntityProperties.Size = New System.Drawing.Size(252, 450)
        Me.pgEntityProperties.TabIndex = 0
        '
        'frmEntityProperties
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(252, 450)
        Me.Controls.Add(Me.pgEntityProperties)
        Me.Name = "frmEntityProperties"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "frmEntityProperties"
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents pgEntityProperties As PropertyGrid
End Class
