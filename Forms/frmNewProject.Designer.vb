<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmNewProject
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmNewProject))
        Me.dtDueDate = New DevExpress.XtraEditors.DateEdit()
        Me.MainRoot = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.tl = New DevExpress.XtraTreeList.TreeList()
        Me.popupContainerControl1 = New DevExpress.XtraEditors.PopupContainerControl()
        Me.popupContainerEdit1 = New DevExpress.XtraEditors.PopupContainerEdit()
        Me.tebMaterialThickness = New DevExpress.XtraEditors.TextEdit()
        Me.lblMaterialThickness = New DevExpress.XtraEditors.LabelControl()
        Me.tebMaterialWidth = New DevExpress.XtraEditors.TextEdit()
        Me.lblMaterialWidth = New DevExpress.XtraEditors.LabelControl()
        Me.tebMaterialLength = New DevExpress.XtraEditors.TextEdit()
        Me.lblMateriallength = New DevExpress.XtraEditors.LabelControl()
        Me.lblMaterial = New DevExpress.XtraEditors.LabelControl()
        Me.lblDueDate = New DevExpress.XtraEditors.LabelControl()
        Me.txtOrderPO = New DevExpress.XtraEditors.TextEdit()
        Me.lblOrderPO = New DevExpress.XtraEditors.LabelControl()
        Me.txtCustomer = New DevExpress.XtraEditors.TextEdit()
        Me.lblCutomer = New DevExpress.XtraEditors.LabelControl()
        Me.cboToolSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblToolSetup = New DevExpress.XtraEditors.LabelControl()
        Me.lblMachine = New DevExpress.XtraEditors.LabelControl()
        Me.cboMachine = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblRequired = New DevExpress.XtraEditors.LabelControl()
        Me.cboMaterial = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.cmdOK = New DevExpress.XtraEditors.SimpleButton()
        Me.cmdCancel = New DevExpress.XtraEditors.SimpleButton()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.cbeMaterialType = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.MatListTree1 = New FabV25_WIN8.MatListTree()
        Me.cboLayerSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblLayerSetup = New DevExpress.XtraEditors.LabelControl()
        CType(Me.dtDueDate.Properties.CalendarTimeProperties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dtDueDate.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.popupContainerControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.popupContainerControl1.SuspendLayout()
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialThickness.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialWidth.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialLength.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.txtOrderPO.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.txtCustomer.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboToolSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboMachine.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboMaterial.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TableLayoutPanel1.SuspendLayout()
        CType(Me.cbeMaterialType.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboLayerSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'dtDueDate
        '
        Me.dtDueDate.EditValue = Nothing
        Me.dtDueDate.EnterMoveNextControl = True
        Me.dtDueDate.Location = New System.Drawing.Point(106, 151)
        Me.dtDueDate.Name = "dtDueDate"
        Me.dtDueDate.Properties.AutoHeight = False
        Me.dtDueDate.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.dtDueDate.Properties.CalendarTimeProperties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.dtDueDate.Size = New System.Drawing.Size(228, 22)
        Me.dtDueDate.TabIndex = 59
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
        Me.tl.Size = New System.Drawing.Size(299, 241)
        Me.tl.TabIndex = 7
        '
        'popupContainerControl1
        '
        Me.popupContainerControl1.Controls.Add(Me.tl)
        Me.popupContainerControl1.Location = New System.Drawing.Point(424, 24)
        Me.popupContainerControl1.Name = "popupContainerControl1"
        Me.popupContainerControl1.Size = New System.Drawing.Size(299, 241)
        Me.popupContainerControl1.TabIndex = 76
        '
        'popupContainerEdit1
        '
        Me.popupContainerEdit1.Location = New System.Drawing.Point(106, 183)
        Me.popupContainerEdit1.Name = "popupContainerEdit1"
        Me.popupContainerEdit1.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.popupContainerEdit1.Properties.ShowPopupCloseButton = False
        Me.popupContainerEdit1.Properties.ShowPopupShadow = False
        Me.popupContainerEdit1.Properties.SuppressMouseEventOnOuterMouseClick = True
        Me.popupContainerEdit1.Size = New System.Drawing.Size(228, 20)
        Me.popupContainerEdit1.TabIndex = 75
        '
        'tebMaterialThickness
        '
        Me.tebMaterialThickness.EnterMoveNextControl = True
        Me.tebMaterialThickness.Location = New System.Drawing.Point(106, 267)
        Me.tebMaterialThickness.Name = "tebMaterialThickness"
        Me.tebMaterialThickness.Properties.AutoHeight = False
        Me.tebMaterialThickness.Size = New System.Drawing.Size(228, 22)
        Me.tebMaterialThickness.TabIndex = 63
        '
        'lblMaterialThickness
        '
        Me.lblMaterialThickness.Location = New System.Drawing.Point(0, 271)
        Me.lblMaterialThickness.Name = "lblMaterialThickness"
        Me.lblMaterialThickness.Size = New System.Drawing.Size(100, 13)
        Me.lblMaterialThickness.TabIndex = 74
        Me.lblMaterialThickness.Text = "* Material Thickness:"
        '
        'tebMaterialWidth
        '
        Me.tebMaterialWidth.EnterMoveNextControl = True
        Me.tebMaterialWidth.Location = New System.Drawing.Point(106, 238)
        Me.tebMaterialWidth.Name = "tebMaterialWidth"
        Me.tebMaterialWidth.Properties.AutoHeight = False
        Me.tebMaterialWidth.Size = New System.Drawing.Size(228, 22)
        Me.tebMaterialWidth.TabIndex = 62
        '
        'lblMaterialWidth
        '
        Me.lblMaterialWidth.Location = New System.Drawing.Point(18, 242)
        Me.lblMaterialWidth.Name = "lblMaterialWidth"
        Me.lblMaterialWidth.Size = New System.Drawing.Size(82, 13)
        Me.lblMaterialWidth.TabIndex = 73
        Me.lblMaterialWidth.Text = "* Material Width:"
        '
        'tebMaterialLength
        '
        Me.tebMaterialLength.EnterMoveNextControl = True
        Me.tebMaterialLength.Location = New System.Drawing.Point(106, 209)
        Me.tebMaterialLength.Name = "tebMaterialLength"
        Me.tebMaterialLength.Properties.AutoHeight = False
        Me.tebMaterialLength.Size = New System.Drawing.Size(228, 22)
        Me.tebMaterialLength.TabIndex = 61
        '
        'lblMateriallength
        '
        Me.lblMateriallength.Location = New System.Drawing.Point(13, 213)
        Me.lblMateriallength.Name = "lblMateriallength"
        Me.lblMateriallength.Size = New System.Drawing.Size(87, 13)
        Me.lblMateriallength.TabIndex = 72
        Me.lblMateriallength.Text = "* Material Length:"
        '
        'lblMaterial
        '
        Me.lblMaterial.Location = New System.Drawing.Point(53, 184)
        Me.lblMaterial.Name = "lblMaterial"
        Me.lblMaterial.Size = New System.Drawing.Size(47, 13)
        Me.lblMaterial.TabIndex = 71
        Me.lblMaterial.Text = "* Material"
        '
        'lblDueDate
        '
        Me.lblDueDate.Location = New System.Drawing.Point(51, 155)
        Me.lblDueDate.Name = "lblDueDate"
        Me.lblDueDate.Size = New System.Drawing.Size(49, 13)
        Me.lblDueDate.TabIndex = 70
        Me.lblDueDate.Text = "Due Date:"
        '
        'txtOrderPO
        '
        Me.txtOrderPO.EnterMoveNextControl = True
        Me.txtOrderPO.Location = New System.Drawing.Point(106, 122)
        Me.txtOrderPO.Name = "txtOrderPO"
        Me.txtOrderPO.Properties.AutoHeight = False
        Me.txtOrderPO.Size = New System.Drawing.Size(228, 22)
        Me.txtOrderPO.TabIndex = 58
        '
        'lblOrderPO
        '
        Me.lblOrderPO.Location = New System.Drawing.Point(23, 126)
        Me.lblOrderPO.Name = "lblOrderPO"
        Me.lblOrderPO.Size = New System.Drawing.Size(77, 13)
        Me.lblOrderPO.TabIndex = 69
        Me.lblOrderPO.Text = "Order or P/O #:"
        '
        'txtCustomer
        '
        Me.txtCustomer.EnterMoveNextControl = True
        Me.txtCustomer.Location = New System.Drawing.Point(106, 93)
        Me.txtCustomer.Name = "txtCustomer"
        Me.txtCustomer.Properties.AutoHeight = False
        Me.txtCustomer.Size = New System.Drawing.Size(228, 22)
        Me.txtCustomer.TabIndex = 57
        '
        'lblCutomer
        '
        Me.lblCutomer.Location = New System.Drawing.Point(50, 97)
        Me.lblCutomer.Name = "lblCutomer"
        Me.lblCutomer.Size = New System.Drawing.Size(50, 13)
        Me.lblCutomer.TabIndex = 68
        Me.lblCutomer.Text = "Customer:"
        '
        'cboToolSetup
        '
        Me.cboToolSetup.EnterMoveNextControl = True
        Me.cboToolSetup.Location = New System.Drawing.Point(106, 40)
        Me.cboToolSetup.Name = "cboToolSetup"
        Me.cboToolSetup.Properties.AutoHeight = False
        Me.cboToolSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboToolSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboToolSetup.Size = New System.Drawing.Size(228, 22)
        Me.cboToolSetup.TabIndex = 56
        '
        'lblToolSetup
        '
        Me.lblToolSetup.Location = New System.Drawing.Point(36, 44)
        Me.lblToolSetup.Name = "lblToolSetup"
        Me.lblToolSetup.Size = New System.Drawing.Size(64, 13)
        Me.lblToolSetup.TabIndex = 67
        Me.lblToolSetup.Text = "* Tool Setup:"
        '
        'lblMachine
        '
        Me.lblMachine.Location = New System.Drawing.Point(48, 15)
        Me.lblMachine.Name = "lblMachine"
        Me.lblMachine.Size = New System.Drawing.Size(52, 13)
        Me.lblMachine.TabIndex = 66
        Me.lblMachine.Text = "* Machine:"
        '
        'cboMachine
        '
        Me.cboMachine.EnterMoveNextControl = True
        Me.cboMachine.Location = New System.Drawing.Point(106, 11)
        Me.cboMachine.Name = "cboMachine"
        Me.cboMachine.Properties.AutoHeight = False
        Me.cboMachine.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboMachine.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboMachine.Size = New System.Drawing.Size(228, 22)
        Me.cboMachine.TabIndex = 55
        '
        'lblRequired
        '
        Me.lblRequired.Appearance.BackColor = System.Drawing.Color.Yellow
        Me.lblRequired.Appearance.Font = New System.Drawing.Font("Tahoma", 10.0!)
        Me.lblRequired.Appearance.ForeColor = System.Drawing.Color.Red
        Me.lblRequired.Appearance.Options.UseBackColor = True
        Me.lblRequired.Appearance.Options.UseFont = True
        Me.lblRequired.Appearance.Options.UseForeColor = True
        Me.lblRequired.Location = New System.Drawing.Point(36, 295)
        Me.lblRequired.Name = "lblRequired"
        Me.lblRequired.Size = New System.Drawing.Size(268, 16)
        Me.lblRequired.TabIndex = 65
        Me.lblRequired.Text = "* Fields preceeded by an Asterisk are required"
        '
        'cboMaterial
        '
        Me.cboMaterial.EnterMoveNextControl = True
        Me.cboMaterial.Location = New System.Drawing.Point(39, 383)
        Me.cboMaterial.Name = "cboMaterial"
        Me.cboMaterial.Properties.AutoHeight = False
        Me.cboMaterial.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboMaterial.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboMaterial.Size = New System.Drawing.Size(228, 22)
        Me.cboMaterial.TabIndex = 60
        Me.cboMaterial.Visible = False
        '
        'cmdOK
        '
        Me.cmdOK.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.cmdOK.Location = New System.Drawing.Point(3, 3)
        Me.cmdOK.Name = "cmdOK"
        Me.cmdOK.Size = New System.Drawing.Size(67, 23)
        Me.cmdOK.TabIndex = 9
        Me.cmdOK.Text = "Ok"
        '
        'cmdCancel
        '
        Me.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.cmdCancel.Location = New System.Drawing.Point(76, 3)
        Me.cmdCancel.Name = "cmdCancel"
        Me.cmdCancel.Size = New System.Drawing.Size(67, 23)
        Me.cmdCancel.TabIndex = 10
        Me.cmdCancel.Text = "Cancel"
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel1.BackColor = System.Drawing.Color.Transparent
        Me.TableLayoutPanel1.ColumnCount = 2
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.cmdOK, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.cmdCancel, 1, 0)
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(188, 328)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 1
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(146, 29)
        Me.TableLayoutPanel1.TabIndex = 64
        '
        'cbeMaterialType
        '
        Me.cbeMaterialType.EnterMoveNextControl = True
        Me.cbeMaterialType.Location = New System.Drawing.Point(26, 414)
        Me.cbeMaterialType.Name = "cbeMaterialType"
        Me.cbeMaterialType.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeMaterialType.Size = New System.Drawing.Size(245, 20)
        Me.cbeMaterialType.TabIndex = 77
        Me.cbeMaterialType.Visible = False
        '
        'MatListTree1
        '
        Me.MatListTree1.AutoSize = True
        Me.MatListTree1.Location = New System.Drawing.Point(106, 184)
        Me.MatListTree1.Name = "MatListTree1"
        Me.MatListTree1.SelectedMaterial = Nothing
        Me.MatListTree1.Size = New System.Drawing.Size(228, 22)
        Me.MatListTree1.TabIndex = 78
        '
        'cboLayerSetup
        '
        Me.cboLayerSetup.Location = New System.Drawing.Point(106, 65)
        Me.cboLayerSetup.Name = "cboLayerSetup"
        Me.cboLayerSetup.Properties.AutoHeight = False
        Me.cboLayerSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboLayerSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboLayerSetup.Size = New System.Drawing.Size(228, 22)
        Me.cboLayerSetup.TabIndex = 79
        '
        'lblLayerSetup
        '
        Me.lblLayerSetup.Location = New System.Drawing.Point(29, 69)
        Me.lblLayerSetup.Name = "lblLayerSetup"
        Me.lblLayerSetup.Size = New System.Drawing.Size(71, 13)
        Me.lblLayerSetup.TabIndex = 80
        Me.lblLayerSetup.Text = "* Layer Setup:"
        '
        'frmNewProject
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(359, 369)
        Me.Controls.Add(Me.cboLayerSetup)
        Me.Controls.Add(Me.lblLayerSetup)
        Me.Controls.Add(Me.MatListTree1)
        Me.Controls.Add(Me.dtDueDate)
        Me.Controls.Add(Me.popupContainerControl1)
        Me.Controls.Add(Me.popupContainerEdit1)
        Me.Controls.Add(Me.tebMaterialThickness)
        Me.Controls.Add(Me.lblMaterialThickness)
        Me.Controls.Add(Me.tebMaterialWidth)
        Me.Controls.Add(Me.lblMaterialWidth)
        Me.Controls.Add(Me.tebMaterialLength)
        Me.Controls.Add(Me.lblMateriallength)
        Me.Controls.Add(Me.lblMaterial)
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
        Me.Controls.Add(Me.cboMaterial)
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.Controls.Add(Me.cbeMaterialType)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Name = "frmNewProject"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "New Project"
        CType(Me.dtDueDate.Properties.CalendarTimeProperties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dtDueDate.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.popupContainerControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.popupContainerControl1.ResumeLayout(False)
        CType(Me.popupContainerEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialThickness.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialWidth.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialLength.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.txtOrderPO.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.txtCustomer.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboToolSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboMachine.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboMaterial.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TableLayoutPanel1.ResumeLayout(False)
        CType(Me.cbeMaterialType.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboLayerSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents dtDueDate As DateEdit
    Private WithEvents MainRoot As DevExpress.XtraTreeList.Columns.TreeListColumn
    Private WithEvents tl As DevExpress.XtraTreeList.TreeList
    Private WithEvents popupContainerControl1 As PopupContainerControl
    Private WithEvents popupContainerEdit1 As PopupContainerEdit
    Friend WithEvents tebMaterialThickness As TextEdit
    Friend WithEvents lblMaterialThickness As LabelControl
    Friend WithEvents tebMaterialWidth As TextEdit
    Friend WithEvents lblMaterialWidth As LabelControl
    Friend WithEvents tebMaterialLength As TextEdit
    Friend WithEvents lblMateriallength As LabelControl
    Friend WithEvents lblMaterial As LabelControl
    Friend WithEvents lblDueDate As LabelControl
    Friend WithEvents txtOrderPO As TextEdit
    Friend WithEvents lblOrderPO As LabelControl
    Friend WithEvents txtCustomer As TextEdit
    Friend WithEvents lblCutomer As LabelControl
    Friend WithEvents cboToolSetup As ComboBoxEdit
    Friend WithEvents lblToolSetup As LabelControl
    Friend WithEvents lblMachine As LabelControl
    Friend WithEvents cboMachine As ComboBoxEdit
    Friend WithEvents lblRequired As LabelControl
    Friend WithEvents cboMaterial As ComboBoxEdit
    Friend WithEvents cmdOK As SimpleButton
    Friend WithEvents cmdCancel As SimpleButton
    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents cbeMaterialType As ComboBoxEdit
    Friend WithEvents MatListTree1 As MatListTree
    Friend WithEvents cboLayerSetup As ComboBoxEdit
    Friend WithEvents lblLayerSetup As LabelControl
End Class
