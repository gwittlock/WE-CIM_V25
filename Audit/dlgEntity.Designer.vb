<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class dlgEntity
    Inherits DevExpress.XtraEditors.XtraForm

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.btnAccept = New DevExpress.XtraEditors.SimpleButton()
        Me.dlgEntityConvertedLayout = New DevExpress.XtraLayout.LayoutControl()
        Me.cboDefn = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.vgProplist = New DevExpress.XtraVerticalGrid.VGridControl()
        Me.LayoutControlGroup1 = New DevExpress.XtraLayout.LayoutControlGroup()
        Me.LayoutControlGroup2 = New DevExpress.XtraLayout.LayoutControlGroup()
        Me.LayoutControlItem1 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.LayoutControlItem2 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.LayoutControlItem3 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.LayoutConverter1 = New DevExpress.XtraLayout.Converter.LayoutConverter(Me.components)
        Me.popupContainerEdit1 = New DevExpress.XtraEditors.PopupContainerEdit()
        Me.popupContainerControl1 = New DevExpress.XtraEditors.PopupContainerControl()
        Me.tl = New DevExpress.XtraTreeList.TreeList()
        Me.MainRoot = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.ListBox1 = New System.Windows.Forms.ListBox()
        CType(Me.dlgEntityConvertedLayout, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.dlgEntityConvertedLayout.SuspendLayout()
        CType(Me.cboDefn.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.vgProplist, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlGroup1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlGroup2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.popupContainerControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.popupContainerControl1.SuspendLayout()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnAccept
        '
        Me.btnAccept.Location = New System.Drawing.Point(12, 311)
        Me.btnAccept.Name = "btnAccept"
        Me.btnAccept.Size = New System.Drawing.Size(216, 22)
        Me.btnAccept.StyleController = Me.dlgEntityConvertedLayout
        Me.btnAccept.TabIndex = 3
        Me.btnAccept.Text = "Accept"
        Me.btnAccept.ToolTip = "Applies the values for the current operation."
        '
        'dlgEntityConvertedLayout
        '
        Me.dlgEntityConvertedLayout.Controls.Add(Me.cboDefn)
        Me.dlgEntityConvertedLayout.Controls.Add(Me.btnAccept)
        Me.dlgEntityConvertedLayout.Controls.Add(Me.vgProplist)
        Me.dlgEntityConvertedLayout.Location = New System.Drawing.Point(0, 0)
        Me.dlgEntityConvertedLayout.Name = "dlgEntityConvertedLayout"
        Me.dlgEntityConvertedLayout.Root = Me.LayoutControlGroup1
        Me.dlgEntityConvertedLayout.Size = New System.Drawing.Size(240, 345)
        Me.dlgEntityConvertedLayout.TabIndex = 12
        '
        'cboDefn
        '
        Me.cboDefn.EnterMoveNextControl = True
        Me.cboDefn.Location = New System.Drawing.Point(60, 12)
        Me.cboDefn.Name = "cboDefn"
        Me.cboDefn.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboDefn.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboDefn.Size = New System.Drawing.Size(168, 20)
        Me.cboDefn.StyleController = Me.dlgEntityConvertedLayout
        Me.cboDefn.TabIndex = 8
        '
        'vgProplist
        '
        Me.vgProplist.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.Simple
        Me.vgProplist.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgProplist.Location = New System.Drawing.Point(12, 36)
        Me.vgProplist.Name = "vgProplist"
        Me.vgProplist.OptionsView.ShowButtons = False
        Me.vgProplist.RecordWidth = 92
        Me.vgProplist.RowHeaderWidth = 108
        Me.vgProplist.Size = New System.Drawing.Size(216, 271)
        Me.vgProplist.TabIndex = 2
        Me.vgProplist.TreeButtonStyle = DevExpress.XtraVerticalGrid.TreeButtonStyle.ExplorerBar
        '
        'LayoutControlGroup1
        '
        Me.LayoutControlGroup1.CustomizationFormText = "LayoutControlGroup1"
        Me.LayoutControlGroup1.EnableIndentsWithoutBorders = DevExpress.Utils.DefaultBoolean.[True]
        Me.LayoutControlGroup1.GroupBordersVisible = False
        Me.LayoutControlGroup1.Items.AddRange(New DevExpress.XtraLayout.BaseLayoutItem() {Me.LayoutControlGroup2})
        Me.LayoutControlGroup1.Name = "LayoutControlGroup1"
        Me.LayoutControlGroup1.Size = New System.Drawing.Size(240, 345)
        Me.LayoutControlGroup1.TextVisible = False
        '
        'LayoutControlGroup2
        '
        Me.LayoutControlGroup2.CustomizationFormText = "LayoutControlGroup1"
        Me.LayoutControlGroup2.GroupBordersVisible = False
        Me.LayoutControlGroup2.Items.AddRange(New DevExpress.XtraLayout.BaseLayoutItem() {Me.LayoutControlItem1, Me.LayoutControlItem2, Me.LayoutControlItem3})
        Me.LayoutControlGroup2.Location = New System.Drawing.Point(0, 0)
        Me.LayoutControlGroup2.Name = "GroupControl1item"
        Me.LayoutControlGroup2.Size = New System.Drawing.Size(220, 325)
        Me.LayoutControlGroup2.Text = "LayoutControlGroup1"
        '
        'LayoutControlItem1
        '
        Me.LayoutControlItem1.Control = Me.cboDefn
        Me.LayoutControlItem1.CustomizationFormText = "Definition"
        Me.LayoutControlItem1.Location = New System.Drawing.Point(0, 0)
        Me.LayoutControlItem1.Name = "cboDefnitem"
        Me.LayoutControlItem1.Size = New System.Drawing.Size(220, 24)
        Me.LayoutControlItem1.Text = "Definition"
        Me.LayoutControlItem1.TextSize = New System.Drawing.Size(45, 13)
        '
        'LayoutControlItem2
        '
        Me.LayoutControlItem2.Control = Me.btnAccept
        Me.LayoutControlItem2.CustomizationFormText = "btnAcceptitem"
        Me.LayoutControlItem2.Location = New System.Drawing.Point(0, 299)
        Me.LayoutControlItem2.Name = "btnAcceptitem"
        Me.LayoutControlItem2.Size = New System.Drawing.Size(220, 26)
        Me.LayoutControlItem2.TextSize = New System.Drawing.Size(0, 0)
        Me.LayoutControlItem2.TextVisible = False
        '
        'LayoutControlItem3
        '
        Me.LayoutControlItem3.Control = Me.vgProplist
        Me.LayoutControlItem3.CustomizationFormText = "vgProplistitem"
        Me.LayoutControlItem3.Location = New System.Drawing.Point(0, 24)
        Me.LayoutControlItem3.Name = "vgProplistitem"
        Me.LayoutControlItem3.Size = New System.Drawing.Size(220, 275)
        Me.LayoutControlItem3.TextSize = New System.Drawing.Size(0, 0)
        Me.LayoutControlItem3.TextVisible = False
        '
        'popupContainerEdit1
        '
        Me.popupContainerEdit1.Location = New System.Drawing.Point(303, 308)
        Me.popupContainerEdit1.Name = "popupContainerEdit1"
        Me.popupContainerEdit1.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.popupContainerEdit1.Properties.ShowPopupCloseButton = False
        Me.popupContainerEdit1.Properties.ShowPopupShadow = False
        Me.popupContainerEdit1.Properties.SuppressMouseEventOnOuterMouseClick = True
        Me.popupContainerEdit1.Size = New System.Drawing.Size(228, 20)
        Me.popupContainerEdit1.TabIndex = 55
        Me.popupContainerEdit1.Visible = False
        '
        'popupContainerControl1
        '
        Me.popupContainerControl1.Controls.Add(Me.tl)
        Me.popupContainerControl1.Location = New System.Drawing.Point(343, 15)
        Me.popupContainerControl1.Name = "popupContainerControl1"
        Me.popupContainerControl1.Size = New System.Drawing.Size(234, 302)
        Me.popupContainerControl1.TabIndex = 56
        '
        'tl
        '
        Me.tl.Columns.AddRange(New DevExpress.XtraTreeList.Columns.TreeListColumn() {Me.MainRoot})
        Me.tl.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tl.Location = New System.Drawing.Point(0, 0)
        Me.tl.Name = "tl"
        Me.tl.OptionsBehavior.AutoChangeParent = False
        Me.tl.OptionsBehavior.AutoNodeHeight = False
        Me.tl.OptionsBehavior.AutoSelectAllInEditor = False
        Me.tl.OptionsBehavior.CloseEditorOnLostFocus = False
        Me.tl.OptionsBehavior.Editable = False
        Me.tl.OptionsBehavior.ResizeNodes = False
        Me.tl.OptionsBehavior.SmartMouseHover = False
        Me.tl.OptionsMenu.EnableFooterMenu = False
        Me.tl.OptionsPrint.PrintHorzLines = False
        Me.tl.OptionsPrint.PrintVertLines = False
        Me.tl.OptionsSelection.EnableAppearanceFocusedCell = False
        Me.tl.OptionsSelection.KeepSelectedOnClick = False
        Me.tl.OptionsView.FocusRectStyle = DevExpress.XtraTreeList.DrawFocusRectStyle.None
        Me.tl.OptionsView.ShowHorzLines = False
        Me.tl.OptionsView.ShowIndicator = False
        Me.tl.OptionsView.ShowVertLines = False
        Me.tl.Size = New System.Drawing.Size(234, 302)
        Me.tl.TabIndex = 7
        Me.tl.Visible = False
        '
        'MainRoot
        '
        Me.MainRoot.AllNodesSummary = True
        Me.MainRoot.Caption = "Materials"
        Me.MainRoot.FieldName = "Key"
        Me.MainRoot.MinWidth = 33
        Me.MainRoot.Name = "MainRoot"
        Me.MainRoot.SummaryFooter = DevExpress.XtraTreeList.SummaryItemType.Count
        Me.MainRoot.SummaryFooterStrFormat = "Count keys = {0}"
        Me.MainRoot.UnboundType = DevExpress.XtraTreeList.Data.UnboundColumnType.[String]
        Me.MainRoot.Visible = True
        Me.MainRoot.VisibleIndex = 0
        Me.MainRoot.Width = 235
        '
        'ListBox1
        '
        Me.ListBox1.FormattingEnabled = True
        Me.ListBox1.Location = New System.Drawing.Point(10, 341)
        Me.ListBox1.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.ListBox1.Name = "ListBox1"
        Me.ListBox1.Size = New System.Drawing.Size(103, 173)
        Me.ListBox1.TabIndex = 59
        Me.ListBox1.Visible = False
        '
        'dlgEntity
        '
        Me.AcceptButton = Me.btnAccept
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoValidate = System.Windows.Forms.AutoValidate.Disable
        Me.ClientSize = New System.Drawing.Size(558, 339)
        Me.Controls.Add(Me.ListBox1)
        Me.Controls.Add(Me.popupContainerControl1)
        Me.Controls.Add(Me.popupContainerEdit1)
        Me.Controls.Add(Me.dlgEntityConvertedLayout)
        Me.DoubleBuffered = True
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "dlgEntity"
        CType(Me.dlgEntityConvertedLayout, System.ComponentModel.ISupportInitialize).EndInit()
        Me.dlgEntityConvertedLayout.ResumeLayout(False)
        CType(Me.cboDefn.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.vgProplist, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlGroup1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlGroup2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.popupContainerControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.popupContainerControl1.ResumeLayout(False)
        CType(Me.tl, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents btnAccept As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents dlgEntityConvertedLayout As DevExpress.XtraLayout.LayoutControl
    Friend WithEvents cboDefn As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents vgProplist As DevExpress.XtraVerticalGrid.VGridControl
    Friend WithEvents LayoutControlGroup1 As DevExpress.XtraLayout.LayoutControlGroup
    Friend WithEvents LayoutControlGroup2 As DevExpress.XtraLayout.LayoutControlGroup
    Friend WithEvents LayoutControlItem1 As DevExpress.XtraLayout.LayoutControlItem
    Friend WithEvents LayoutControlItem2 As DevExpress.XtraLayout.LayoutControlItem
    Friend WithEvents LayoutControlItem3 As DevExpress.XtraLayout.LayoutControlItem
    Friend WithEvents LayoutConverter1 As DevExpress.XtraLayout.Converter.LayoutConverter
    Private WithEvents popupContainerEdit1 As PopupContainerEdit
    Private WithEvents popupContainerControl1 As PopupContainerControl
    Private WithEvents tl As DevExpress.XtraTreeList.TreeList
    Private WithEvents MainRoot As DevExpress.XtraTreeList.Columns.TreeListColumn
    Friend WithEvents ListBox1 As ListBox
End Class
