<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmAssignLayerToolSetups
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
        Me.dgvLayerToolSetups = New System.Windows.Forms.DataGridView()
        Me.colMachine = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.colLayerSetup = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.colToolSetup = New System.Windows.Forms.DataGridViewComboBoxColumn()
        Me.btnSaveAssignments = New System.Windows.Forms.Button()
        CType(Me.dgvLayerToolSetups, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'dgvLayerToolSetups
        '
        Me.dgvLayerToolSetups.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvLayerToolSetups.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.colMachine, Me.colLayerSetup, Me.colToolSetup})
        Me.dgvLayerToolSetups.Location = New System.Drawing.Point(0, 0)
        Me.dgvLayerToolSetups.Name = "dgvLayerToolSetups"
        Me.dgvLayerToolSetups.Size = New System.Drawing.Size(788, 395)
        Me.dgvLayerToolSetups.TabIndex = 0
        '
        'colMachine
        '
        Me.colMachine.HeaderText = "MACHINE"
        Me.colMachine.Name = "colMachine"
        Me.colMachine.Resizable = System.Windows.Forms.DataGridViewTriState.[True]
        '
        'colLayerSetup
        '
        Me.colLayerSetup.HeaderText = "LAYER SETUP"
        Me.colLayerSetup.Name = "colLayerSetup"
        Me.colLayerSetup.Resizable = System.Windows.Forms.DataGridViewTriState.[True]
        '
        'colToolSetup
        '
        Me.colToolSetup.HeaderText = "TOOL SETUP"
        Me.colToolSetup.Name = "colToolSetup"
        '
        'btnSaveAssignments
        '
        Me.btnSaveAssignments.Location = New System.Drawing.Point(630, 415)
        Me.btnSaveAssignments.Name = "btnSaveAssignments"
        Me.btnSaveAssignments.Size = New System.Drawing.Size(75, 23)
        Me.btnSaveAssignments.TabIndex = 1
        Me.btnSaveAssignments.Text = "Save"
        Me.btnSaveAssignments.UseVisualStyleBackColor = True
        '
        'frmAssignLayerToolSetups
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(800, 450)
        Me.Controls.Add(Me.btnSaveAssignments)
        Me.Controls.Add(Me.dgvLayerToolSetups)
        Me.Name = "frmAssignLayerToolSetups"
        Me.Text = "frmAssignLayerToolSetups"
        CType(Me.dgvLayerToolSetups, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents dgvLayerToolSetups As DataGridView
    Friend WithEvents colMachine As DataGridViewTextBoxColumn
    Friend WithEvents colLayerSetup As DataGridViewTextBoxColumn
    Friend WithEvents colToolSetup As DataGridViewComboBoxColumn
    Friend WithEvents btnSaveAssignments As Button
End Class
