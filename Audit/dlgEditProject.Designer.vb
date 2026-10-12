<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlgEditProject
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
        Me.layoutControl1 = New DevExpress.XtraLayout.LayoutControl()
        Me.sbCancel = New DevExpress.XtraEditors.SimpleButton()
        Me.sbAccept = New DevExpress.XtraEditors.SimpleButton()
        Me.vgSettingsProplist = New DevExpress.XtraVerticalGrid.VGridControl()
        Me.layoutControlGroup1 = New DevExpress.XtraLayout.LayoutControlGroup()
        Me.LayoutControlItem1 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.LayoutControlItem2 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.LayoutControlItem3 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.lbOptions = New DevExpress.XtraEditors.ListBoxControl()
        Me.popupContainerControl1 = New DevExpress.XtraEditors.PopupContainerControl()
        Me.tl = New DevExpress.XtraTreeList.TreeList()
        Me.MainRoot = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.popupContainerEdit1 = New DevExpress.XtraEditors.PopupContainerEdit()
        CType(Me.layoutControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.layoutControl1.SuspendLayout()
        CType(Me.vgSettingsProplist, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.layoutControlGroup1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.lbOptions, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.popupContainerControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.popupContainerControl1.SuspendLayout()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'layoutControl1
        '
        Me.layoutControl1.Controls.Add(Me.sbCancel)
        Me.layoutControl1.Controls.Add(Me.sbAccept)
        Me.layoutControl1.Controls.Add(Me.vgSettingsProplist)
        Me.layoutControl1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.layoutControl1.Location = New System.Drawing.Point(0, 0)
        Me.layoutControl1.Name = "layoutControl1"
        Me.layoutControl1.OptionsCustomizationForm.DesignTimeCustomizationFormPositionAndSize = New System.Drawing.Rectangle(486, 427, 250, 350)
        Me.layoutControl1.Root = Me.layoutControlGroup1
        Me.layoutControl1.Size = New System.Drawing.Size(253, 498)
        Me.layoutControl1.TabIndex = 1
        Me.layoutControl1.Text = "layoutControl1"
        '
        'sbCancel
        '
        Me.sbCancel.Location = New System.Drawing.Point(12, 464)
        Me.sbCancel.Name = "sbCancel"
        Me.sbCancel.Size = New System.Drawing.Size(107, 22)
        Me.sbCancel.StyleController = Me.layoutControl1
        Me.sbCancel.TabIndex = 11
        Me.sbCancel.Text = "Cancel"
        '
        'sbAccept
        '
        Me.sbAccept.Location = New System.Drawing.Point(123, 464)
        Me.sbAccept.Margin = New System.Windows.Forms.Padding(4)
        Me.sbAccept.Name = "sbAccept"
        Me.sbAccept.Size = New System.Drawing.Size(118, 22)
        Me.sbAccept.StyleController = Me.layoutControl1
        Me.sbAccept.TabIndex = 10
        Me.sbAccept.Text = "Accept"
        '
        'vgSettingsProplist
        '
        Me.vgSettingsProplist.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgSettingsProplist.Location = New System.Drawing.Point(12, 12)
        Me.vgSettingsProplist.Name = "vgSettingsProplist"
        Me.vgSettingsProplist.OptionsBehavior.UseEnterAsTab = True
        Me.vgSettingsProplist.OptionsView.AutoScaleBands = True
        Me.vgSettingsProplist.OptionsView.FixRowHeaderPanelWidth = True
        Me.vgSettingsProplist.RecordWidth = 77
        Me.vgSettingsProplist.RowHeaderWidth = 123
        Me.vgSettingsProplist.Size = New System.Drawing.Size(229, 448)
        Me.vgSettingsProplist.TabIndex = 9
        Me.vgSettingsProplist.TreeButtonStyle = DevExpress.XtraVerticalGrid.TreeButtonStyle.ExplorerBar
        '
        'layoutControlGroup1
        '
        Me.layoutControlGroup1.CustomizationFormText = "layoutControlGroup1"
        Me.layoutControlGroup1.EnableIndentsWithoutBorders = DevExpress.Utils.DefaultBoolean.[True]
        Me.layoutControlGroup1.GroupBordersVisible = False
        Me.layoutControlGroup1.Items.AddRange(New DevExpress.XtraLayout.BaseLayoutItem() {Me.LayoutControlItem1, Me.LayoutControlItem2, Me.LayoutControlItem3})
        Me.layoutControlGroup1.Name = "layoutControlGroup1"
        Me.layoutControlGroup1.Size = New System.Drawing.Size(253, 498)
        Me.layoutControlGroup1.TextVisible = False
        '
        'LayoutControlItem1
        '
        Me.LayoutControlItem1.Control = Me.vgSettingsProplist
        Me.LayoutControlItem1.Location = New System.Drawing.Point(0, 0)
        Me.LayoutControlItem1.Name = "LayoutControlItem1"
        Me.LayoutControlItem1.ShowInCustomizationForm = False
        Me.LayoutControlItem1.Size = New System.Drawing.Size(233, 452)
        Me.LayoutControlItem1.TextSize = New System.Drawing.Size(0, 0)
        Me.LayoutControlItem1.TextVisible = False
        '
        'LayoutControlItem2
        '
        Me.LayoutControlItem2.Control = Me.sbAccept
        Me.LayoutControlItem2.Location = New System.Drawing.Point(111, 452)
        Me.LayoutControlItem2.Name = "LayoutControlItem2"
        Me.LayoutControlItem2.Size = New System.Drawing.Size(122, 26)
        Me.LayoutControlItem2.TextSize = New System.Drawing.Size(0, 0)
        Me.LayoutControlItem2.TextVisible = False
        '
        'LayoutControlItem3
        '
        Me.LayoutControlItem3.Control = Me.sbCancel
        Me.LayoutControlItem3.Location = New System.Drawing.Point(0, 452)
        Me.LayoutControlItem3.Name = "LayoutControlItem3"
        Me.LayoutControlItem3.Size = New System.Drawing.Size(111, 26)
        Me.LayoutControlItem3.TextSize = New System.Drawing.Size(0, 0)
        Me.LayoutControlItem3.TextVisible = False
        '
        'lbOptions
        '
        Me.lbOptions.Appearance.BackColor = System.Drawing.Color.White
        Me.lbOptions.Appearance.Options.UseBackColor = True
        Me.lbOptions.Cursor = System.Windows.Forms.Cursors.Default
        Me.lbOptions.Location = New System.Drawing.Point(480, 257)
        Me.lbOptions.Name = "lbOptions"
        Me.lbOptions.Size = New System.Drawing.Size(166, 32)
        Me.lbOptions.TabIndex = 72
        Me.lbOptions.Visible = False
        '
        'popupContainerControl1
        '
        Me.popupContainerControl1.Controls.Add(Me.tl)
        Me.popupContainerControl1.Location = New System.Drawing.Point(386, 10)
        Me.popupContainerControl1.Name = "popupContainerControl1"
        Me.popupContainerControl1.Size = New System.Drawing.Size(299, 241)
        Me.popupContainerControl1.TabIndex = 71
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
        Me.tl.OptionsView.ShowIndicator = False
        Me.tl.Size = New System.Drawing.Size(299, 241)
        Me.tl.TabIndex = 7
        '
        'MainRoot
        '
        Me.MainRoot.AllNodesSummary = True
        Me.MainRoot.Caption = "Materials"
        Me.MainRoot.FieldName = "Key"
        Me.MainRoot.MinWidth = 33
        Me.MainRoot.Name = "MainRoot"
        Me.MainRoot.SortMode = DevExpress.XtraGrid.ColumnSortMode.DisplayText
        Me.MainRoot.SortOrder = System.Windows.Forms.SortOrder.Ascending
        Me.MainRoot.SummaryFooter = DevExpress.XtraTreeList.SummaryItemType.Count
        Me.MainRoot.SummaryFooterStrFormat = "Count keys = {0}"
        Me.MainRoot.UnboundType = DevExpress.XtraTreeList.Data.UnboundColumnType.[String]
        Me.MainRoot.Visible = True
        Me.MainRoot.VisibleIndex = 0
        Me.MainRoot.Width = 235
        '
        'popupContainerEdit1
        '
        Me.popupContainerEdit1.Location = New System.Drawing.Point(418, 316)
        Me.popupContainerEdit1.Name = "popupContainerEdit1"
        Me.popupContainerEdit1.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.popupContainerEdit1.Properties.ShowPopupCloseButton = False
        Me.popupContainerEdit1.Properties.ShowPopupShadow = False
        Me.popupContainerEdit1.Properties.SuppressMouseEventOnOuterMouseClick = True
        Me.popupContainerEdit1.Size = New System.Drawing.Size(228, 20)
        Me.popupContainerEdit1.TabIndex = 73
        Me.popupContainerEdit1.Visible = False
        '
        'dlgEditProject
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(253, 498)
        Me.Controls.Add(Me.popupContainerEdit1)
        Me.Controls.Add(Me.lbOptions)
        Me.Controls.Add(Me.popupContainerControl1)
        Me.Controls.Add(Me.layoutControl1)
        Me.DoubleBuffered = True
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow
        Me.Name = "dlgEditProject"
        Me.Text = "Edit Project"
        CType(Me.layoutControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.layoutControl1.ResumeLayout(False)
        CType(Me.vgSettingsProplist, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.layoutControlGroup1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.lbOptions, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.popupContainerControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.popupContainerControl1.ResumeLayout(False)
        CType(Me.tl, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents layoutControl1 As DevExpress.XtraLayout.LayoutControl
    Private WithEvents layoutControlGroup1 As DevExpress.XtraLayout.LayoutControlGroup
    Private WithEvents vgSettingsProplist As DevExpress.XtraVerticalGrid.VGridControl
    Friend WithEvents LayoutControlItem1 As DevExpress.XtraLayout.LayoutControlItem
    Private WithEvents sbAccept As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents LayoutControlItem2 As DevExpress.XtraLayout.LayoutControlItem
    Friend WithEvents lbOptions As ListBoxControl
    Public WithEvents popupContainerControl1 As PopupContainerControl
    Private WithEvents tl As DevExpress.XtraTreeList.TreeList
    Public WithEvents MainRoot As DevExpress.XtraTreeList.Columns.TreeListColumn
    Private WithEvents popupContainerEdit1 As PopupContainerEdit
    Friend WithEvents sbCancel As SimpleButton
    Friend WithEvents LayoutControlItem3 As DevExpress.XtraLayout.LayoutControlItem
End Class
