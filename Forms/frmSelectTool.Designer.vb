<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSelectTool
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmSelectTool))
        Me.grdToolLayer = New DevExpress.XtraGrid.GridControl()
        Me.gViewToolLayer = New DevExpress.XtraGrid.Views.Grid.GridView()
        Me.sbSetCurrent = New DevExpress.XtraEditors.SimpleButton()
        Me.sbHideAll = New DevExpress.XtraEditors.SimpleButton()
        Me.sbShowAll = New DevExpress.XtraEditors.SimpleButton()
        Me.sbAssign = New DevExpress.XtraEditors.SimpleButton()
        Me.vgProcessParameters = New DevExpress.XtraVerticalGrid.VGridControl()
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.vgProcessParameters, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grdToolLayer
        '
        Me.grdToolLayer.Cursor = System.Windows.Forms.Cursors.Default
        GridLevelNode1.RelationName = "Level1"
        Me.grdToolLayer.LevelTree.Nodes.AddRange(New DevExpress.XtraGrid.GridLevelNode() {GridLevelNode1})
        Me.grdToolLayer.Location = New System.Drawing.Point(8, 41)
        Me.grdToolLayer.MainView = Me.gViewToolLayer
        Me.grdToolLayer.Name = "grdToolLayer"
        Me.grdToolLayer.Size = New System.Drawing.Size(465, 461)
        Me.grdToolLayer.TabIndex = 108
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
        'sbSetCurrent
        '
        Me.sbSetCurrent.Location = New System.Drawing.Point(204, 12)
        Me.sbSetCurrent.Name = "sbSetCurrent"
        Me.sbSetCurrent.Size = New System.Drawing.Size(75, 23)
        Me.sbSetCurrent.TabIndex = 111
        Me.sbSetCurrent.Text = "Set Current"
        '
        'sbHideAll
        '
        Me.sbHideAll.Location = New System.Drawing.Point(88, 12)
        Me.sbHideAll.Name = "sbHideAll"
        Me.sbHideAll.Size = New System.Drawing.Size(75, 23)
        Me.sbHideAll.TabIndex = 110
        Me.sbHideAll.Text = "Hide All"
        '
        'sbShowAll
        '
        Me.sbShowAll.Location = New System.Drawing.Point(7, 12)
        Me.sbShowAll.Name = "sbShowAll"
        Me.sbShowAll.Size = New System.Drawing.Size(75, 23)
        Me.sbShowAll.TabIndex = 109
        Me.sbShowAll.Text = "Show All"
        '
        'sbAssign
        '
        Me.sbAssign.Location = New System.Drawing.Point(96, 617)
        Me.sbAssign.Name = "sbAssign"
        Me.sbAssign.Size = New System.Drawing.Size(75, 23)
        Me.sbAssign.TabIndex = 112
        Me.sbAssign.Text = "Assign"
        '
        'vgProcessParameters
        '
        Me.vgProcessParameters.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgProcessParameters.Location = New System.Drawing.Point(7, 508)
        Me.vgProcessParameters.Name = "vgProcessParameters"
        Me.vgProcessParameters.OptionsView.ShowButtons = False
        Me.vgProcessParameters.Size = New System.Drawing.Size(466, 103)
        Me.vgProcessParameters.TabIndex = 113
        Me.vgProcessParameters.Visible = False
        '
        'frmSelectTool
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(485, 650)
        Me.Controls.Add(Me.vgProcessParameters)
        Me.Controls.Add(Me.sbAssign)
        Me.Controls.Add(Me.sbSetCurrent)
        Me.Controls.Add(Me.sbHideAll)
        Me.Controls.Add(Me.sbShowAll)
        Me.Controls.Add(Me.grdToolLayer)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Name = "frmSelectTool"
        Me.Text = "Select Tool"
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.vgProcessParameters, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Private WithEvents grdToolLayer As DevExpress.XtraGrid.GridControl
    Private WithEvents gViewToolLayer As DevExpress.XtraGrid.Views.Grid.GridView
    Friend WithEvents sbSetCurrent As SimpleButton
    Friend WithEvents sbHideAll As SimpleButton
    Friend WithEvents sbShowAll As SimpleButton
    Friend WithEvents sbAssign As SimpleButton
    Friend WithEvents vgProcessParameters As DevExpress.XtraVerticalGrid.VGridControl
End Class
