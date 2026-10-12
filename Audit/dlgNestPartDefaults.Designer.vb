<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlgNestPartDefaults
    Inherits DevExpress.XtraEditors.XtraForm

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.vgPartVars = New DevExpress.XtraVerticalGrid.VGridControl()
        Me.lbOptions = New DevExpress.XtraEditors.ListBoxControl()
        Me.sbFinish = New DevExpress.XtraEditors.SimpleButton()
        Me.sbSetDefaults = New DevExpress.XtraEditors.SimpleButton()
        Me.vgParts = New DevExpress.XtraVerticalGrid.VGridControl()
        CType(Me.vgPartVars, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.lbOptions, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.vgParts, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'vgPartVars
        '
        Me.vgPartVars.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgPartVars.Location = New System.Drawing.Point(23, 12)
        Me.vgPartVars.Name = "vgPartVars"
        Me.vgPartVars.OptionsBehavior.UseEnterAsTab = True
        Me.vgPartVars.Size = New System.Drawing.Size(331, 246)
        Me.vgPartVars.TabIndex = 85
        '
        'lbOptions
        '
        Me.lbOptions.Appearance.BackColor = System.Drawing.Color.White
        Me.lbOptions.Appearance.Options.UseBackColor = True
        Me.lbOptions.Cursor = System.Windows.Forms.Cursors.Default
        Me.lbOptions.Location = New System.Drawing.Point(12, 264)
        Me.lbOptions.Name = "lbOptions"
        Me.lbOptions.Size = New System.Drawing.Size(120, 21)
        Me.lbOptions.TabIndex = 86
        Me.lbOptions.Visible = False
        '
        'sbFinish
        '
        Me.sbFinish.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.sbFinish.Location = New System.Drawing.Point(279, 264)
        Me.sbFinish.Name = "sbFinish"
        Me.sbFinish.Size = New System.Drawing.Size(75, 23)
        Me.sbFinish.TabIndex = 87
        Me.sbFinish.Text = "Finish"
        '
        'sbSetDefaults
        '
        Me.sbSetDefaults.Location = New System.Drawing.Point(198, 264)
        Me.sbSetDefaults.Name = "sbSetDefaults"
        Me.sbSetDefaults.Size = New System.Drawing.Size(75, 23)
        Me.sbSetDefaults.TabIndex = 88
        Me.sbSetDefaults.Text = "Set Defaults"
        '
        'vgParts
        '
        Me.vgParts.Appearance.FocusedRow.BackColor = System.Drawing.SystemColors.Highlight
        Me.vgParts.Appearance.FocusedRow.Options.UseBackColor = True
        Me.vgParts.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgParts.Location = New System.Drawing.Point(379, 41)
        Me.vgParts.Name = "vgParts"
        Me.vgParts.OptionsBehavior.DragRowHeaders = True
        Me.vgParts.OptionsBehavior.UseEnterAsTab = True
        Me.vgParts.OptionsSelectionAndFocus.MultiSelect = True
        Me.vgParts.OptionsSelectionAndFocus.MultiSelectMode = DevExpress.XtraVerticalGrid.MultiSelectMode.RowSelect
        Me.vgParts.Size = New System.Drawing.Size(315, 164)
        Me.vgParts.TabIndex = 89
        '
        'dlgNestPartDefaults
        '
        Me.AcceptButton = Me.sbSetDefaults
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.sbFinish
        Me.ClientSize = New System.Drawing.Size(959, 417)
        Me.Controls.Add(Me.vgParts)
        Me.Controls.Add(Me.sbSetDefaults)
        Me.Controls.Add(Me.sbFinish)
        Me.Controls.Add(Me.lbOptions)
        Me.Controls.Add(Me.vgPartVars)
        Me.DoubleBuffered = True
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow
        Me.Name = "dlgNestPartDefaults"
        Me.Text = "Set Defaults for the Part values used in ""Advanced Nesting"""
        CType(Me.vgPartVars, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.lbOptions, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.vgParts, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents vgPartVars As DevExpress.XtraVerticalGrid.VGridControl
    Friend WithEvents lbOptions As DevExpress.XtraEditors.ListBoxControl
    Friend WithEvents sbFinish As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbSetDefaults As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents vgParts As DevExpress.XtraVerticalGrid.VGridControl
End Class
