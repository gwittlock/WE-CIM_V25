<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmTooling
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
        Dim GridLevelNode1 As DevExpress.XtraGrid.GridLevelNode = New DevExpress.XtraGrid.GridLevelNode()
        Me.sbShowAll = New DevExpress.XtraEditors.SimpleButton()
        Me.sbHideAll = New DevExpress.XtraEditors.SimpleButton()
        Me.sbSetCurrent = New DevExpress.XtraEditors.SimpleButton()
        Me.vgProcessParameters = New DevExpress.XtraVerticalGrid.VGridControl()
        Me.grdToolLayer = New DevExpress.XtraGrid.GridControl()
        Me.gViewToolLayer = New DevExpress.XtraGrid.Views.Grid.GridView()
        Me.lstOptions = New DevExpress.XtraEditors.ListBoxControl()
        CType(Me.vgProcessParameters, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.lstOptions, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'sbShowAll
        '
        Me.sbShowAll.Location = New System.Drawing.Point(12, 12)
        Me.sbShowAll.Name = "sbShowAll"
        Me.sbShowAll.Size = New System.Drawing.Size(75, 23)
        Me.sbShowAll.TabIndex = 0
        Me.sbShowAll.Text = "Show All"
        '
        'sbHideAll
        '
        Me.sbHideAll.Location = New System.Drawing.Point(93, 12)
        Me.sbHideAll.Name = "sbHideAll"
        Me.sbHideAll.Size = New System.Drawing.Size(75, 23)
        Me.sbHideAll.TabIndex = 1
        Me.sbHideAll.Text = "Hide All"
        '
        'sbSetCurrent
        '
        Me.sbSetCurrent.Location = New System.Drawing.Point(209, 12)
        Me.sbSetCurrent.Name = "sbSetCurrent"
        Me.sbSetCurrent.Size = New System.Drawing.Size(75, 23)
        Me.sbSetCurrent.TabIndex = 2
        Me.sbSetCurrent.Text = "Set Current"
        '
        'vgProcessParameters
        '
        Me.vgProcessParameters.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgProcessParameters.Location = New System.Drawing.Point(12, 293)
        Me.vgProcessParameters.Name = "vgProcessParameters"
        Me.vgProcessParameters.OptionsView.ShowButtons = False
        Me.vgProcessParameters.RecordWidth = 96
        Me.vgProcessParameters.RowHeaderWidth = 104
        Me.vgProcessParameters.Size = New System.Drawing.Size(272, 103)
        Me.vgProcessParameters.TabIndex = 105
        '
        'grdToolLayer
        '
        Me.grdToolLayer.Cursor = System.Windows.Forms.Cursors.Default
        GridLevelNode1.RelationName = "Level1"
        Me.grdToolLayer.LevelTree.Nodes.AddRange(New DevExpress.XtraGrid.GridLevelNode() {GridLevelNode1})
        Me.grdToolLayer.Location = New System.Drawing.Point(12, 41)
        Me.grdToolLayer.MainView = Me.gViewToolLayer
        Me.grdToolLayer.Name = "grdToolLayer"
        Me.grdToolLayer.Size = New System.Drawing.Size(272, 248)
        Me.grdToolLayer.TabIndex = 107
        Me.grdToolLayer.ViewCollection.AddRange(New DevExpress.XtraGrid.Views.Base.BaseView() {Me.gViewToolLayer})
        '
        'gViewToolLayer
        '
        Me.gViewToolLayer.FocusRectStyle = DevExpress.XtraGrid.Views.Grid.DrawFocusRectStyle.RowFocus
        Me.gViewToolLayer.GridControl = Me.grdToolLayer
        Me.gViewToolLayer.Name = "gViewToolLayer"
        Me.gViewToolLayer.OptionsBehavior.AllowAddRows = DevExpress.Utils.DefaultBoolean.[False]
        Me.gViewToolLayer.OptionsBehavior.AllowDeleteRows = DevExpress.Utils.DefaultBoolean.[False]
        Me.gViewToolLayer.OptionsBehavior.Editable = False
        Me.gViewToolLayer.OptionsView.ColumnAutoWidth = False
        Me.gViewToolLayer.OptionsView.ShowDetailButtons = False
        Me.gViewToolLayer.OptionsView.ShowGroupExpandCollapseButtons = False
        Me.gViewToolLayer.OptionsView.ShowGroupPanel = False
        Me.gViewToolLayer.OptionsView.ShowIndicator = False
        '
        'lstOptions
        '
        Me.lstOptions.Location = New System.Drawing.Point(113, 402)
        Me.lstOptions.Name = "lstOptions"
        Me.lstOptions.Size = New System.Drawing.Size(120, 25)
        Me.lstOptions.TabIndex = 108
        Me.lstOptions.Visible = False
        '
        'frmTooling
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(292, 454)
        Me.Controls.Add(Me.lstOptions)
        Me.Controls.Add(Me.vgProcessParameters)
        Me.Controls.Add(Me.sbSetCurrent)
        Me.Controls.Add(Me.sbHideAll)
        Me.Controls.Add(Me.sbShowAll)
        Me.Controls.Add(Me.grdToolLayer)
        Me.Name = "frmTooling"
        Me.Text = "Select Tool"
        CType(Me.vgProcessParameters, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.lstOptions, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents sbShowAll As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbHideAll As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbSetCurrent As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents vgProcessParameters As DevExpress.XtraVerticalGrid.VGridControl
    Private WithEvents grdToolLayer As DevExpress.XtraGrid.GridControl
    Private WithEvents gViewToolLayer As DevExpress.XtraGrid.Views.Grid.GridView
    Friend WithEvents lstOptions As DevExpress.XtraEditors.ListBoxControl
End Class
