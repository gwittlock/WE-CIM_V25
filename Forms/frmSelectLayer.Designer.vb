<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSelectLayer
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
        Dim GridLevelNode2 As DevExpress.XtraGrid.GridLevelNode = New DevExpress.XtraGrid.GridLevelNode()
        Me.grdToolLayer = New DevExpress.XtraGrid.GridControl()
        Me.gViewToolLayer = New DevExpress.XtraGrid.Views.Grid.GridView()
        Me.sbOk = New DevExpress.XtraEditors.SimpleButton()
        Me.sbCancel = New DevExpress.XtraEditors.SimpleButton()
        Me.cbfAddLayer = New FabV25_WIN8.WindowsApplication234.ChkBoxFrame()
        Me.btnAddLayer = New System.Windows.Forms.Button()
        Me.ColorPickEdit1 = New DevExpress.XtraEditors.ColorPickEdit()
        Me.TextBox1 = New System.Windows.Forms.TextBox()
        Me.lblLayerColor = New System.Windows.Forms.Label()
        Me.lblLayerName = New System.Windows.Forms.Label()
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.cbfAddLayer.SuspendLayout()
        CType(Me.ColorPickEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grdToolLayer
        '
        Me.grdToolLayer.Cursor = System.Windows.Forms.Cursors.Default
        GridLevelNode2.RelationName = "Level1"
        Me.grdToolLayer.LevelTree.Nodes.AddRange(New DevExpress.XtraGrid.GridLevelNode() {GridLevelNode2})
        Me.grdToolLayer.Location = New System.Drawing.Point(6, 6)
        Me.grdToolLayer.MainView = Me.gViewToolLayer
        Me.grdToolLayer.Name = "grdToolLayer"
        Me.grdToolLayer.Size = New System.Drawing.Size(272, 248)
        Me.grdToolLayer.TabIndex = 101
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
        'sbOk
        '
        Me.sbOk.Location = New System.Drawing.Point(197, 385)
        Me.sbOk.Name = "sbOk"
        Me.sbOk.Size = New System.Drawing.Size(75, 23)
        Me.sbOk.TabIndex = 102
        Me.sbOk.Text = "Ok"
        '
        'sbCancel
        '
        Me.sbCancel.Location = New System.Drawing.Point(13, 385)
        Me.sbCancel.Name = "sbCancel"
        Me.sbCancel.Size = New System.Drawing.Size(75, 23)
        Me.sbCancel.TabIndex = 103
        Me.sbCancel.Text = "Cancel"
        '
        'cbfAddLayer
        '
        Me.cbfAddLayer.BackColor = System.Drawing.Color.FromArgb(CType(CType(133, Byte), Integer), CType(CType(133, Byte), Integer), CType(CType(133, Byte), Integer))
        Me.cbfAddLayer.Caption = "Add Layer"
        Me.cbfAddLayer.CaptionColor = System.Drawing.Color.Silver
        Me.cbfAddLayer.Checked = False
        Me.cbfAddLayer.Controls.Add(Me.btnAddLayer)
        Me.cbfAddLayer.Controls.Add(Me.ColorPickEdit1)
        Me.cbfAddLayer.Controls.Add(Me.TextBox1)
        Me.cbfAddLayer.Controls.Add(Me.lblLayerColor)
        Me.cbfAddLayer.Controls.Add(Me.lblLayerName)
        Me.cbfAddLayer.Location = New System.Drawing.Point(13, 271)
        Me.cbfAddLayer.Name = "cbfAddLayer"
        Me.cbfAddLayer.Padding = New System.Windows.Forms.Padding(0, 11, 0, 0)
        Me.cbfAddLayer.Size = New System.Drawing.Size(265, 108)
        Me.cbfAddLayer.TabIndex = 104
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
        'ColorPickEdit1
        '
        Me.ColorPickEdit1.EditValue = System.Drawing.Color.Empty
        Me.ColorPickEdit1.Location = New System.Drawing.Point(53, 58)
        Me.ColorPickEdit1.Name = "ColorPickEdit1"
        Me.ColorPickEdit1.Properties.AutomaticColor = System.Drawing.Color.Black
        Me.ColorPickEdit1.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.ColorPickEdit1.Size = New System.Drawing.Size(94, 20)
        Me.ColorPickEdit1.TabIndex = 4
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
        'frmSelectLayer
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(284, 461)
        Me.Controls.Add(Me.cbfAddLayer)
        Me.Controls.Add(Me.sbCancel)
        Me.Controls.Add(Me.sbOk)
        Me.Controls.Add(Me.grdToolLayer)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow
        Me.Name = "frmSelectLayer"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Manage Layers"
        CType(Me.grdToolLayer, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.gViewToolLayer, System.ComponentModel.ISupportInitialize).EndInit()
        Me.cbfAddLayer.ResumeLayout(False)
        Me.cbfAddLayer.PerformLayout()
        CType(Me.ColorPickEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents grdToolLayer As DevExpress.XtraGrid.GridControl
    Private WithEvents gViewToolLayer As DevExpress.XtraGrid.Views.Grid.GridView
    Friend WithEvents sbOk As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbCancel As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents cbfAddLayer As WindowsApplication234.ChkBoxFrame
    Friend WithEvents TextBox1 As TextBox
    Friend WithEvents lblLayerColor As Label
    Friend WithEvents lblLayerName As Label
    Friend WithEvents ColorPickEdit1 As ColorPickEdit
    Friend WithEvents btnAddLayer As Button
End Class
