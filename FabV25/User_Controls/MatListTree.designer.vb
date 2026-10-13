<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class MatListTree
    Inherits System.Windows.Forms.UserControl

    'UserControl overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.MainRoot = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.popupContainerControl1 = New DevExpress.XtraEditors.PopupContainerControl()
        Me.tl = New DevExpress.XtraTreeList.TreeList()
        Me.TreeListColumn1 = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.popupContainerEdit1 = New DevExpress.XtraEditors.PopupContainerEdit()
        CType(Me.popupContainerControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.popupContainerControl1.SuspendLayout()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
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
        'popupContainerControl1
        '
        Me.popupContainerControl1.AutoSize = True
        Me.popupContainerControl1.Controls.Add(Me.tl)
        Me.popupContainerControl1.Location = New System.Drawing.Point(3, 29)
        Me.popupContainerControl1.Name = "popupContainerControl1"
        Me.popupContainerControl1.Size = New System.Drawing.Size(231, 241)
        Me.popupContainerControl1.TabIndex = 57
        '
        'tl
        '
        Me.tl.Columns.AddRange(New DevExpress.XtraTreeList.Columns.TreeListColumn() {Me.TreeListColumn1})
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
        Me.tl.Size = New System.Drawing.Size(231, 241)
        Me.tl.TabIndex = 7
        '
        'TreeListColumn1
        '
        Me.TreeListColumn1.AllNodesSummary = True
        Me.TreeListColumn1.Caption = "Materials"
        Me.TreeListColumn1.FieldName = "Key"
        Me.TreeListColumn1.MinWidth = 33
        Me.TreeListColumn1.Name = "TreeListColumn1"
        Me.TreeListColumn1.SortMode = DevExpress.XtraGrid.ColumnSortMode.DisplayText
        Me.TreeListColumn1.SortOrder = System.Windows.Forms.SortOrder.Ascending
        Me.TreeListColumn1.SummaryFooter = DevExpress.XtraTreeList.SummaryItemType.Count
        Me.TreeListColumn1.SummaryFooterStrFormat = "Count keys = {0}"
        Me.TreeListColumn1.UnboundType = DevExpress.XtraTreeList.Data.UnboundColumnType.[String]
        Me.TreeListColumn1.Visible = True
        Me.TreeListColumn1.VisibleIndex = 0
        Me.TreeListColumn1.Width = 235
        '
        'popupContainerEdit1
        '
        Me.popupContainerEdit1.Dock = System.Windows.Forms.DockStyle.Top
        Me.popupContainerEdit1.Location = New System.Drawing.Point(0, 0)
        Me.popupContainerEdit1.Margin = New System.Windows.Forms.Padding(0)
        Me.popupContainerEdit1.Name = "popupContainerEdit1"
        Me.popupContainerEdit1.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.popupContainerEdit1.Properties.ShowPopupCloseButton = False
        Me.popupContainerEdit1.Properties.ShowPopupShadow = False
        Me.popupContainerEdit1.Properties.SuppressMouseEventOnOuterMouseClick = True
        Me.popupContainerEdit1.Size = New System.Drawing.Size(237, 20)
        Me.popupContainerEdit1.TabIndex = 58
        '
        'MatListTree
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoSize = True
        Me.Controls.Add(Me.popupContainerEdit1)
        Me.Controls.Add(Me.popupContainerControl1)
        Me.Name = "MatListTree"
        Me.Size = New System.Drawing.Size(237, 273)
        CType(Me.popupContainerControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.popupContainerControl1.ResumeLayout(False)
        CType(Me.tl, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Private WithEvents MainRoot As DevExpress.XtraTreeList.Columns.TreeListColumn
    Private WithEvents popupContainerControl1 As PopupContainerControl
    Private WithEvents tl As DevExpress.XtraTreeList.TreeList
    Private WithEvents TreeListColumn1 As DevExpress.XtraTreeList.Columns.TreeListColumn
    Public WithEvents popupContainerEdit1 As PopupContainerEdit
End Class
