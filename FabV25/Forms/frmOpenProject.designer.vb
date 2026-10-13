<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmOpenProject
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
        Me.lblRequired = New DevExpress.XtraEditors.LabelControl()
        Me.cmdOK = New DevExpress.XtraEditors.SimpleButton()
        Me.cmdCancel = New DevExpress.XtraEditors.SimpleButton()
        Me.cboMachine = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblMachine = New DevExpress.XtraEditors.LabelControl()
        Me.lblToolSetup = New DevExpress.XtraEditors.LabelControl()
        Me.cboToolSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.txtCustomer = New DevExpress.XtraEditors.TextEdit()
        Me.lblCutomer = New DevExpress.XtraEditors.LabelControl()
        Me.lblMaterial = New DevExpress.XtraEditors.LabelControl()
        Me.dtDueDate = New DevExpress.XtraEditors.DateEdit()
        Me.lblDueDate = New DevExpress.XtraEditors.LabelControl()
        Me.txtOrderPO = New DevExpress.XtraEditors.TextEdit()
        Me.lblOrderPO = New DevExpress.XtraEditors.LabelControl()
        Me.cboLayerSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblLayerSetup = New DevExpress.XtraEditors.LabelControl()
        Me.tvMaterial = New DevExpress.XtraTreeList.TreeList()
        Me.MainRoot = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.tebMaterialThickness = New DevExpress.XtraEditors.TextEdit()
        Me.tebMaterialWidth = New DevExpress.XtraEditors.TextEdit()
        Me.tebMaterialLength = New DevExpress.XtraEditors.TextEdit()
        Me.MatListTree1 = New FabV25_WIN8.MatListTree()
        Me.cboMaterial = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.popupContainerEdit1 = New DevExpress.XtraEditors.PopupContainerEdit()
        Me.tl = New DevExpress.XtraTreeList.TreeList()
        Me.TreeListColumn1 = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        CType(Me.cboMachine.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboToolSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.txtCustomer.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dtDueDate.Properties.CalendarTimeProperties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dtDueDate.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.txtOrderPO.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboLayerSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tvMaterial, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialThickness.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialWidth.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialLength.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboMaterial.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'lblRequired
        '
        Me.lblRequired.Appearance.BackColor = System.Drawing.Color.Yellow
        Me.lblRequired.Appearance.Font = New System.Drawing.Font("Tahoma", 10.0!)
        Me.lblRequired.Appearance.ForeColor = System.Drawing.Color.Red
        Me.lblRequired.Appearance.Options.UseBackColor = True
        Me.lblRequired.Appearance.Options.UseFont = True
        Me.lblRequired.Appearance.Options.UseForeColor = True
        Me.lblRequired.Location = New System.Drawing.Point(60, 216)
        Me.lblRequired.Name = "lblRequired"
        Me.lblRequired.Size = New System.Drawing.Size(268, 16)
        Me.lblRequired.TabIndex = 36
        Me.lblRequired.Text = "* Fields preceeded by an Asterisk are required"
        '
        'cmdOK
        '
        Me.cmdOK.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.cmdOK.Location = New System.Drawing.Point(223, 240)
        Me.cmdOK.Name = "cmdOK"
        Me.cmdOK.Size = New System.Drawing.Size(67, 23)
        Me.cmdOK.TabIndex = 7
        Me.cmdOK.Text = "Ok"
        '
        'cmdCancel
        '
        Me.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.cmdCancel.Location = New System.Drawing.Point(296, 240)
        Me.cmdCancel.Name = "cmdCancel"
        Me.cmdCancel.Size = New System.Drawing.Size(67, 23)
        Me.cmdCancel.TabIndex = 8
        Me.cmdCancel.Text = "Cancel"
        '
        'cboMachine
        '
        Me.cboMachine.Location = New System.Drawing.Point(135, 12)
        Me.cboMachine.Name = "cboMachine"
        Me.cboMachine.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboMachine.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboMachine.Size = New System.Drawing.Size(228, 20)
        Me.cboMachine.TabIndex = 0
        '
        'lblMachine
        '
        Me.lblMachine.Location = New System.Drawing.Point(77, 15)
        Me.lblMachine.Name = "lblMachine"
        Me.lblMachine.Size = New System.Drawing.Size(52, 13)
        Me.lblMachine.TabIndex = 42
        Me.lblMachine.Text = "* Machine:"
        '
        'lblToolSetup
        '
        Me.lblToolSetup.Location = New System.Drawing.Point(65, 45)
        Me.lblToolSetup.Name = "lblToolSetup"
        Me.lblToolSetup.Size = New System.Drawing.Size(64, 13)
        Me.lblToolSetup.TabIndex = 43
        Me.lblToolSetup.Text = "* Tool Setup:"
        '
        'cboToolSetup
        '
        Me.cboToolSetup.Location = New System.Drawing.Point(135, 41)
        Me.cboToolSetup.Name = "cboToolSetup"
        Me.cboToolSetup.Properties.AutoHeight = False
        Me.cboToolSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboToolSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboToolSetup.Size = New System.Drawing.Size(228, 22)
        Me.cboToolSetup.TabIndex = 1
        '
        'txtCustomer
        '
        Me.txtCustomer.Location = New System.Drawing.Point(135, 69)
        Me.txtCustomer.Name = "txtCustomer"
        Me.txtCustomer.Properties.AutoHeight = False
        Me.txtCustomer.Size = New System.Drawing.Size(228, 22)
        Me.txtCustomer.TabIndex = 2
        '
        'lblCutomer
        '
        Me.lblCutomer.Location = New System.Drawing.Point(79, 73)
        Me.lblCutomer.Name = "lblCutomer"
        Me.lblCutomer.Size = New System.Drawing.Size(50, 13)
        Me.lblCutomer.TabIndex = 45
        Me.lblCutomer.Text = "Customer:"
        '
        'lblMaterial
        '
        Me.lblMaterial.Location = New System.Drawing.Point(27, 164)
        Me.lblMaterial.Name = "lblMaterial"
        Me.lblMaterial.Size = New System.Drawing.Size(102, 13)
        Me.lblMaterial.TabIndex = 51
        Me.lblMaterial.Text = "* Material Inventory:"
        '
        'dtDueDate
        '
        Me.dtDueDate.EditValue = Nothing
        Me.dtDueDate.Location = New System.Drawing.Point(135, 129)
        Me.dtDueDate.Name = "dtDueDate"
        Me.dtDueDate.Properties.AutoHeight = False
        Me.dtDueDate.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.dtDueDate.Properties.CalendarTimeProperties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.dtDueDate.Size = New System.Drawing.Size(228, 22)
        Me.dtDueDate.TabIndex = 4
        '
        'lblDueDate
        '
        Me.lblDueDate.Location = New System.Drawing.Point(80, 133)
        Me.lblDueDate.Name = "lblDueDate"
        Me.lblDueDate.Size = New System.Drawing.Size(49, 13)
        Me.lblDueDate.TabIndex = 49
        Me.lblDueDate.Text = "Due Date:"
        '
        'txtOrderPO
        '
        Me.txtOrderPO.Location = New System.Drawing.Point(135, 97)
        Me.txtOrderPO.Name = "txtOrderPO"
        Me.txtOrderPO.Properties.AutoHeight = False
        Me.txtOrderPO.Size = New System.Drawing.Size(228, 22)
        Me.txtOrderPO.TabIndex = 3
        '
        'lblOrderPO
        '
        Me.lblOrderPO.Location = New System.Drawing.Point(52, 100)
        Me.lblOrderPO.Name = "lblOrderPO"
        Me.lblOrderPO.Size = New System.Drawing.Size(77, 13)
        Me.lblOrderPO.TabIndex = 47
        Me.lblOrderPO.Text = "Order or P/O #:"
        '
        'cboLayerSetup
        '
        Me.cboLayerSetup.Location = New System.Drawing.Point(135, 188)
        Me.cboLayerSetup.Name = "cboLayerSetup"
        Me.cboLayerSetup.Properties.AutoHeight = False
        Me.cboLayerSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboLayerSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboLayerSetup.Size = New System.Drawing.Size(228, 22)
        Me.cboLayerSetup.TabIndex = 6
        '
        'lblLayerSetup
        '
        Me.lblLayerSetup.Location = New System.Drawing.Point(58, 192)
        Me.lblLayerSetup.Name = "lblLayerSetup"
        Me.lblLayerSetup.Size = New System.Drawing.Size(71, 13)
        Me.lblLayerSetup.TabIndex = 53
        Me.lblLayerSetup.Text = "* Layer Setup:"
        '
        'tvMaterial
        '
        Me.tvMaterial.Columns.AddRange(New DevExpress.XtraTreeList.Columns.TreeListColumn() {Me.MainRoot})
        Me.tvMaterial.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tvMaterial.Location = New System.Drawing.Point(0, 0)
        Me.tvMaterial.Name = "tvMaterial"
        Me.tvMaterial.OptionsBehavior.AutoChangeParent = False
        Me.tvMaterial.OptionsBehavior.AutoNodeHeight = False
        Me.tvMaterial.OptionsBehavior.AutoSelectAllInEditor = False
        Me.tvMaterial.OptionsBehavior.CloseEditorOnLostFocus = False
        Me.tvMaterial.OptionsBehavior.Editable = False
        Me.tvMaterial.OptionsBehavior.ResizeNodes = False
        Me.tvMaterial.OptionsBehavior.SmartMouseHover = False
        Me.tvMaterial.OptionsMenu.EnableFooterMenu = False
        Me.tvMaterial.OptionsPrint.PrintHorzLines = False
        Me.tvMaterial.OptionsPrint.PrintVertLines = False
        Me.tvMaterial.OptionsSelection.EnableAppearanceFocusedCell = False
        Me.tvMaterial.OptionsSelection.KeepSelectedOnClick = False
        Me.tvMaterial.OptionsView.FocusRectStyle = DevExpress.XtraTreeList.DrawFocusRectStyle.None
        Me.tvMaterial.OptionsView.ShowHorzLines = False
        Me.tvMaterial.OptionsView.ShowIndicator = False
        Me.tvMaterial.OptionsView.ShowVertLines = False
        Me.tvMaterial.Size = New System.Drawing.Size(299, 241)
        Me.tvMaterial.TabIndex = 7
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
        'tebMaterialThickness
        '
        Me.tebMaterialThickness.EnterMoveNextControl = True
        Me.tebMaterialThickness.Location = New System.Drawing.Point(276, 355)
        Me.tebMaterialThickness.Name = "tebMaterialThickness"
        Me.tebMaterialThickness.Properties.AutoHeight = False
        Me.tebMaterialThickness.Size = New System.Drawing.Size(228, 22)
        Me.tebMaterialThickness.TabIndex = 60
        Me.tebMaterialThickness.Visible = False
        '
        'tebMaterialWidth
        '
        Me.tebMaterialWidth.EnterMoveNextControl = True
        Me.tebMaterialWidth.Location = New System.Drawing.Point(276, 326)
        Me.tebMaterialWidth.Name = "tebMaterialWidth"
        Me.tebMaterialWidth.Properties.AutoHeight = False
        Me.tebMaterialWidth.Size = New System.Drawing.Size(228, 22)
        Me.tebMaterialWidth.TabIndex = 59
        Me.tebMaterialWidth.Visible = False
        '
        'tebMaterialLength
        '
        Me.tebMaterialLength.EnterMoveNextControl = True
        Me.tebMaterialLength.Location = New System.Drawing.Point(276, 297)
        Me.tebMaterialLength.Name = "tebMaterialLength"
        Me.tebMaterialLength.Properties.AutoHeight = False
        Me.tebMaterialLength.Size = New System.Drawing.Size(228, 22)
        Me.tebMaterialLength.TabIndex = 58
        Me.tebMaterialLength.Visible = False
        '
        'MatListTree1
        '
        Me.MatListTree1.AutoSize = True
        Me.MatListTree1.Location = New System.Drawing.Point(135, 157)
        Me.MatListTree1.Name = "MatListTree1"
        Me.MatListTree1.Size = New System.Drawing.Size(228, 22)
        Me.MatListTree1.TabIndex = 61
        '
        'cboMaterial
        '
        Me.cboMaterial.Location = New System.Drawing.Point(2, 323)
        Me.cboMaterial.Name = "cboMaterial"
        Me.cboMaterial.Properties.AutoHeight = False
        Me.cboMaterial.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboMaterial.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboMaterial.Size = New System.Drawing.Size(215, 22)
        Me.cboMaterial.TabIndex = 62
        Me.cboMaterial.Visible = False
        '
        'popupContainerEdit1
        '
        Me.popupContainerEdit1.Location = New System.Drawing.Point(396, 8)
        Me.popupContainerEdit1.Name = "popupContainerEdit1"
        Me.popupContainerEdit1.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.popupContainerEdit1.Properties.ShowPopupCloseButton = False
        Me.popupContainerEdit1.Properties.ShowPopupShadow = False
        Me.popupContainerEdit1.Properties.SuppressMouseEventOnOuterMouseClick = True
        Me.popupContainerEdit1.Size = New System.Drawing.Size(228, 20)
        Me.popupContainerEdit1.TabIndex = 63
        '
        'tl
        '
        Me.tl.Columns.AddRange(New DevExpress.XtraTreeList.Columns.TreeListColumn() {Me.TreeListColumn1})
        Me.tl.Location = New System.Drawing.Point(400, 0)
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
        Me.tl.Size = New System.Drawing.Size(299, 241)
        Me.tl.TabIndex = 64
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
        'frmOpenProject
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(379, 276)
        Me.ControlBox = False
        Me.Controls.Add(Me.tl)
        Me.Controls.Add(Me.popupContainerEdit1)
        Me.Controls.Add(Me.cboMaterial)
        Me.Controls.Add(Me.MatListTree1)
        Me.Controls.Add(Me.tebMaterialThickness)
        Me.Controls.Add(Me.tebMaterialWidth)
        Me.Controls.Add(Me.tebMaterialLength)
        Me.Controls.Add(Me.cmdOK)
        Me.Controls.Add(Me.cmdCancel)
        Me.Controls.Add(Me.cboLayerSetup)
        Me.Controls.Add(Me.lblLayerSetup)
        Me.Controls.Add(Me.lblMaterial)
        Me.Controls.Add(Me.dtDueDate)
        Me.Controls.Add(Me.lblDueDate)
        Me.Controls.Add(Me.txtOrderPO)
        Me.Controls.Add(Me.lblOrderPO)
        Me.Controls.Add(Me.txtCustomer)
        Me.Controls.Add(Me.lblCutomer)
        Me.Controls.Add(Me.cboToolSetup)
        Me.Controls.Add(Me.lblToolSetup)
        Me.Controls.Add(Me.lblMachine)
        Me.Controls.Add(Me.cboMachine)
        Me.Controls.Add(Me.lblRequired)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmOpenProject"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Import CAD Drawing"
        CType(Me.cboMachine.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboToolSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.txtCustomer.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dtDueDate.Properties.CalendarTimeProperties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dtDueDate.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.txtOrderPO.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboLayerSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tvMaterial, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialThickness.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialWidth.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialLength.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboMaterial.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents lblRequired As DevExpress.XtraEditors.LabelControl
    Friend WithEvents cmdOK As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents cmdCancel As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents cboMachine As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents lblMachine As DevExpress.XtraEditors.LabelControl
    Friend WithEvents lblToolSetup As DevExpress.XtraEditors.LabelControl
    Friend WithEvents cboToolSetup As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents txtCustomer As DevExpress.XtraEditors.TextEdit
    Friend WithEvents lblCutomer As DevExpress.XtraEditors.LabelControl
    Friend WithEvents lblMaterial As DevExpress.XtraEditors.LabelControl
    Friend WithEvents dtDueDate As DevExpress.XtraEditors.DateEdit
    Friend WithEvents lblDueDate As DevExpress.XtraEditors.LabelControl
    Friend WithEvents txtOrderPO As DevExpress.XtraEditors.TextEdit
    Friend WithEvents lblOrderPO As DevExpress.XtraEditors.LabelControl
    Friend WithEvents cboLayerSetup As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents lblLayerSetup As DevExpress.XtraEditors.LabelControl
    Private WithEvents tvMaterial As DevExpress.XtraTreeList.TreeList
    Private WithEvents MainRoot As DevExpress.XtraTreeList.Columns.TreeListColumn
    Friend WithEvents tebMaterialThickness As TextEdit
    Friend WithEvents tebMaterialWidth As TextEdit
    Friend WithEvents tebMaterialLength As TextEdit
    Friend WithEvents MatListTree1 As MatListTree
    Friend WithEvents cboMaterial As ComboBoxEdit
    Private WithEvents popupContainerEdit1 As PopupContainerEdit
    Private WithEvents tl As DevExpress.XtraTreeList.TreeList
    Private WithEvents TreeListColumn1 As DevExpress.XtraTreeList.Columns.TreeListColumn
End Class
