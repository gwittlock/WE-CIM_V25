<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmLayers
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
        Me.sbSetCurrent = New DevExpress.XtraEditors.SimpleButton()
        Me.sbHideAll = New DevExpress.XtraEditors.SimpleButton()
        Me.sbShowAll = New DevExpress.XtraEditors.SimpleButton()
        Me.grdToolLayer = New DevExpress.XtraGrid.GridControl()
        Me.gViewToolLayer = New DevExpress.XtraGrid.Views.Grid.GridView()
        Me.teLayerName = New System.Windows.Forms.TextBox()
        Me.sbAdd = New DevExpress.XtraEditors.SimpleButton()
        Me.lcLayerColor = New DevExpress.XtraEditors.LabelControl()
        Me.lcLayerName = New DevExpress.XtraEditors.LabelControl()
        Me.ColorPickEdit1 = New DevExpress.XtraEditors.ColorPickEdit()
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ColorPickEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'sbSetCurrent
        '
        Me.sbSetCurrent.Location = New System.Drawing.Point(209, 9)
        Me.sbSetCurrent.Name = "sbSetCurrent"
        Me.sbSetCurrent.Size = New System.Drawing.Size(75, 23)
        Me.sbSetCurrent.TabIndex = 110
        Me.sbSetCurrent.Text = "Set Current"
        '
        'sbHideAll
        '
        Me.sbHideAll.Location = New System.Drawing.Point(93, 9)
        Me.sbHideAll.Name = "sbHideAll"
        Me.sbHideAll.Size = New System.Drawing.Size(75, 23)
        Me.sbHideAll.TabIndex = 109
        Me.sbHideAll.Text = "Hide All"
        '
        'sbShowAll
        '
        Me.sbShowAll.Location = New System.Drawing.Point(12, 9)
        Me.sbShowAll.Name = "sbShowAll"
        Me.sbShowAll.Size = New System.Drawing.Size(75, 23)
        Me.sbShowAll.TabIndex = 108
        Me.sbShowAll.Text = "Show All"
        '
        'grdToolLayer
        '
        Me.grdToolLayer.Cursor = System.Windows.Forms.Cursors.Default
        GridLevelNode1.RelationName = "Level1"
        Me.grdToolLayer.LevelTree.Nodes.AddRange(New DevExpress.XtraGrid.GridLevelNode() {GridLevelNode1})
        Me.grdToolLayer.Location = New System.Drawing.Point(12, 38)
        Me.grdToolLayer.MainView = Me.gViewToolLayer
        Me.grdToolLayer.Name = "grdToolLayer"
        Me.grdToolLayer.Size = New System.Drawing.Size(272, 248)
        Me.grdToolLayer.TabIndex = 111
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
        'teLayerName
        '
        Me.teLayerName.Location = New System.Drawing.Point(65, 24)
        Me.teLayerName.Name = "teLayerName"
        Me.teLayerName.Size = New System.Drawing.Size(100, 20)
        Me.teLayerName.TabIndex = 6
        '
        'sbAdd
        '
        Me.sbAdd.Location = New System.Drawing.Point(180, 82)
        Me.sbAdd.Name = "sbAdd"
        Me.sbAdd.Size = New System.Drawing.Size(75, 23)
        Me.sbAdd.TabIndex = 5
        Me.sbAdd.Text = "Add"
        '
        'lcLayerColor
        '
        Me.lcLayerColor.Location = New System.Drawing.Point(28, 54)
        Me.lcLayerColor.Name = "lcLayerColor"
        Me.lcLayerColor.Size = New System.Drawing.Size(29, 13)
        Me.lcLayerColor.TabIndex = 4
        Me.lcLayerColor.Text = "Color:"
        '
        'lcLayerName
        '
        Me.lcLayerName.Location = New System.Drawing.Point(28, 28)
        Me.lcLayerName.Name = "lcLayerName"
        Me.lcLayerName.Size = New System.Drawing.Size(31, 13)
        Me.lcLayerName.TabIndex = 3
        Me.lcLayerName.Text = "Name:"
        '
        'ColorPickEdit1
        '
        Me.ColorPickEdit1.EditValue = System.Drawing.Color.Empty
        Me.ColorPickEdit1.Location = New System.Drawing.Point(65, 51)
        Me.ColorPickEdit1.Name = "ColorPickEdit1"
        Me.ColorPickEdit1.Properties.AutomaticColor = System.Drawing.Color.Black
        Me.ColorPickEdit1.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.ColorPickEdit1.Size = New System.Drawing.Size(100, 20)
        Me.ColorPickEdit1.TabIndex = 2
        '
        'frmLayers
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(295, 418)
        Me.Controls.Add(Me.sbSetCurrent)
        Me.Controls.Add(Me.sbHideAll)
        Me.Controls.Add(Me.sbShowAll)
        Me.Controls.Add(Me.grdToolLayer)
        Me.MinimizeBox = False
        Me.Name = "frmLayers"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "SHow/Hide/Set Active Layers"
        Me.TopMost = True
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ColorPickEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents sbSetCurrent As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbHideAll As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbShowAll As DevExpress.XtraEditors.SimpleButton
    Private WithEvents grdToolLayer As DevExpress.XtraGrid.GridControl
    Private WithEvents gViewToolLayer As DevExpress.XtraGrid.Views.Grid.GridView
    ' Friend WithEvents cbfAddLayer As FabV24_WIN8.WindowsApplication234.ChkBoxFrame
    Friend WithEvents lcLayerColor As DevExpress.XtraEditors.LabelControl
    Friend WithEvents lcLayerName As DevExpress.XtraEditors.LabelControl
    Friend WithEvents ColorPickEdit1 As DevExpress.XtraEditors.ColorPickEdit
    Friend WithEvents sbAdd As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents teLayerName As System.Windows.Forms.TextBox
End Class
