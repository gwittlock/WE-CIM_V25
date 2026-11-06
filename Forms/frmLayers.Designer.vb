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
        Me.cbfAddLayer = New FabV25_WIN8.WindowsApplication234.ChkBoxFrame()
        Me.btnAddLayer = New System.Windows.Forms.Button()
        Me.ColorPickEdit2 = New DevExpress.XtraEditors.ColorPickEdit()
        Me.TextBox1 = New System.Windows.Forms.TextBox()
        Me.lblLayerColor = New System.Windows.Forms.Label()
        Me.lblLayerName = New System.Windows.Forms.Label()
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ColorPickEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.cbfAddLayer.SuspendLayout()
        CType(Me.ColorPickEdit2.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
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
        'cbfAddLayer
        '
        Me.cbfAddLayer.BackColor = System.Drawing.Color.FromArgb(CType(CType(133, Byte), Integer), CType(CType(133, Byte), Integer), CType(CType(133, Byte), Integer))
        Me.cbfAddLayer.Caption = "Add Layer"
        Me.cbfAddLayer.CaptionColor = System.Drawing.Color.Silver
        Me.cbfAddLayer.Checked = False
        Me.cbfAddLayer.Controls.Add(Me.btnAddLayer)
        Me.cbfAddLayer.Controls.Add(Me.ColorPickEdit2)
        Me.cbfAddLayer.Controls.Add(Me.TextBox1)
        Me.cbfAddLayer.Controls.Add(Me.lblLayerColor)
        Me.cbfAddLayer.Controls.Add(Me.lblLayerName)
        Me.cbfAddLayer.Location = New System.Drawing.Point(19, 292)
        Me.cbfAddLayer.Name = "cbfAddLayer"
        Me.cbfAddLayer.Padding = New System.Windows.Forms.Padding(0, 11, 0, 0)
        Me.cbfAddLayer.Size = New System.Drawing.Size(265, 108)
        Me.cbfAddLayer.TabIndex = 112
        '
        'btnAddLayer
        '
        Me.btnAddLayer.Location = New System.Drawing.Point(174, 50)
        Me.btnAddLayer.Name = "btnAddLayer"
        Me.btnAddLayer.Size = New System.Drawing.Size(70, 23)
        Me.btnAddLayer.TabIndex = 5
        Me.btnAddLayer.Text = "Add"
        Me.btnAddLayer.UseVisualStyleBackColor = True
        '
        'ColorPickEdit2
        '
        Me.ColorPickEdit2.EditValue = System.Drawing.Color.Empty
        Me.ColorPickEdit2.Location = New System.Drawing.Point(53, 58)
        Me.ColorPickEdit2.Name = "ColorPickEdit2"
        Me.ColorPickEdit2.Properties.AutomaticColor = System.Drawing.Color.Black
        Me.ColorPickEdit2.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.ColorPickEdit2.Size = New System.Drawing.Size(94, 20)
        Me.ColorPickEdit2.TabIndex = 4
        '
        'TextBox1
        '
        Me.TextBox1.Location = New System.Drawing.Point(53, 27)
        Me.TextBox1.Name = "TextBox1"
        Me.TextBox1.Size = New System.Drawing.Size(95, 21)
        Me.TextBox1.TabIndex = 3
        '
        'lblLayerColor
        '
        Me.lblLayerColor.AutoSize = True
        Me.lblLayerColor.Location = New System.Drawing.Point(12, 61)
        Me.lblLayerColor.Name = "lblLayerColor"
        Me.lblLayerColor.Size = New System.Drawing.Size(36, 13)
        Me.lblLayerColor.TabIndex = 2
        Me.lblLayerColor.Text = "Color:"
        '
        'lblLayerName
        '
        Me.lblLayerName.AutoSize = True
        Me.lblLayerName.Location = New System.Drawing.Point(10, 30)
        Me.lblLayerName.Name = "lblLayerName"
        Me.lblLayerName.Size = New System.Drawing.Size(38, 13)
        Me.lblLayerName.TabIndex = 1
        Me.lblLayerName.Text = "Name:"
        '
        'frmLayers
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(295, 418)
        Me.Controls.Add(Me.cbfAddLayer)
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
        Me.cbfAddLayer.ResumeLayout(False)
        Me.cbfAddLayer.PerformLayout()
        CType(Me.ColorPickEdit2.Properties, System.ComponentModel.ISupportInitialize).EndInit()
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
    Friend WithEvents cbfAddLayer As WindowsApplication234.ChkBoxFrame
    Friend WithEvents btnAddLayer As Button
    Friend WithEvents ColorPickEdit2 As ColorPickEdit
    Friend WithEvents TextBox1 As TextBox
    Friend WithEvents lblLayerColor As Label
    Friend WithEvents lblLayerName As Label
End Class
