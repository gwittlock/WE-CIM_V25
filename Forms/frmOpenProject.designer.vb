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
        Me.cbeMachine = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblMachine = New DevExpress.XtraEditors.LabelControl()
        Me.lblToolSetup = New DevExpress.XtraEditors.LabelControl()
        Me.cbeToolSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.tebCustomer = New DevExpress.XtraEditors.TextEdit()
        Me.lblCutomer = New DevExpress.XtraEditors.LabelControl()
        Me.lblMaterial = New DevExpress.XtraEditors.LabelControl()
        Me.deDueDate = New DevExpress.XtraEditors.DateEdit()
        Me.lblDueDate = New DevExpress.XtraEditors.LabelControl()
        Me.tebOderPO = New DevExpress.XtraEditors.TextEdit()
        Me.lblOrderPO = New DevExpress.XtraEditors.LabelControl()
        Me.cbeLayerSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblLayerSetup = New DevExpress.XtraEditors.LabelControl()
        Me.tl = New DevExpress.XtraTreeList.TreeList()
        Me.MainRoot = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.tebMaterialThickness = New DevExpress.XtraEditors.TextEdit()
        Me.tebMaterialWidth = New DevExpress.XtraEditors.TextEdit()
        Me.tebMaterialLength = New DevExpress.XtraEditors.TextEdit()
        Me.cbeMaterial = New DevExpress.XtraEditors.ComboBoxEdit()
        CType(Me.cbeMachine.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeToolSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebCustomer.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.deDueDate.Properties.CalendarTimeProperties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.deDueDate.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebOderPO.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeLayerSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialThickness.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialWidth.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebMaterialLength.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeMaterial.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
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
        'cbeMachine
        '
        Me.cbeMachine.Location = New System.Drawing.Point(135, 12)
        Me.cbeMachine.Name = "cbeMachine"
        Me.cbeMachine.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeMachine.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeMachine.Size = New System.Drawing.Size(228, 20)
        Me.cbeMachine.TabIndex = 0
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
        'cbeToolSetup
        '
        Me.cbeToolSetup.Location = New System.Drawing.Point(135, 41)
        Me.cbeToolSetup.Name = "cbeToolSetup"
        Me.cbeToolSetup.Properties.AutoHeight = False
        Me.cbeToolSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeToolSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeToolSetup.Size = New System.Drawing.Size(228, 22)
        Me.cbeToolSetup.TabIndex = 1
        '
        'tebCustomer
        '
        Me.tebCustomer.Location = New System.Drawing.Point(135, 69)
        Me.tebCustomer.Name = "tebCustomer"
        Me.tebCustomer.Properties.AutoHeight = False
        Me.tebCustomer.Size = New System.Drawing.Size(228, 22)
        Me.tebCustomer.TabIndex = 2
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
        'deDueDate
        '
        Me.deDueDate.EditValue = Nothing
        Me.deDueDate.Location = New System.Drawing.Point(135, 129)
        Me.deDueDate.Name = "deDueDate"
        Me.deDueDate.Properties.AutoHeight = False
        Me.deDueDate.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.deDueDate.Properties.CalendarTimeProperties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.deDueDate.Size = New System.Drawing.Size(228, 22)
        Me.deDueDate.TabIndex = 4
        '
        'lblDueDate
        '
        Me.lblDueDate.Location = New System.Drawing.Point(80, 133)
        Me.lblDueDate.Name = "lblDueDate"
        Me.lblDueDate.Size = New System.Drawing.Size(49, 13)
        Me.lblDueDate.TabIndex = 49
        Me.lblDueDate.Text = "Due Date:"
        '
        'tebOderPO
        '
        Me.tebOderPO.Location = New System.Drawing.Point(135, 97)
        Me.tebOderPO.Name = "tebOderPO"
        Me.tebOderPO.Properties.AutoHeight = False
        Me.tebOderPO.Size = New System.Drawing.Size(228, 22)
        Me.tebOderPO.TabIndex = 3
        '
        'lblOrderPO
        '
        Me.lblOrderPO.Location = New System.Drawing.Point(52, 100)
        Me.lblOrderPO.Name = "lblOrderPO"
        Me.lblOrderPO.Size = New System.Drawing.Size(77, 13)
        Me.lblOrderPO.TabIndex = 47
        Me.lblOrderPO.Text = "Order or P/O #:"
        '
        'cbeLayerSetup
        '
        Me.cbeLayerSetup.Location = New System.Drawing.Point(135, 188)
        Me.cbeLayerSetup.Name = "cbeLayerSetup"
        Me.cbeLayerSetup.Properties.AutoHeight = False
        Me.cbeLayerSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeLayerSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeLayerSetup.Size = New System.Drawing.Size(228, 22)
        Me.cbeLayerSetup.TabIndex = 6
        '
        'lblLayerSetup
        '
        Me.lblLayerSetup.Location = New System.Drawing.Point(58, 192)
        Me.lblLayerSetup.Name = "lblLayerSetup"
        Me.lblLayerSetup.Size = New System.Drawing.Size(71, 13)
        Me.lblLayerSetup.TabIndex = 53
        Me.lblLayerSetup.Text = "* Layer Setup:"
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
        'cbeMaterial
        '
        Me.cbeMaterial.Location = New System.Drawing.Point(135, 162)
        Me.cbeMaterial.Name = "cbeMaterial"
        Me.cbeMaterial.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeMaterial.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeMaterial.Size = New System.Drawing.Size(228, 20)
        Me.cbeMaterial.TabIndex = 61
        '
        'frmOpenProject
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(379, 276)
        Me.ControlBox = False
        Me.Controls.Add(Me.cbeMaterial)
        Me.Controls.Add(Me.tebMaterialThickness)
        Me.Controls.Add(Me.tebMaterialWidth)
        Me.Controls.Add(Me.tebMaterialLength)
        Me.Controls.Add(Me.cmdOK)
        Me.Controls.Add(Me.cmdCancel)
        Me.Controls.Add(Me.cbeLayerSetup)
        Me.Controls.Add(Me.lblLayerSetup)
        Me.Controls.Add(Me.lblMaterial)
        Me.Controls.Add(Me.deDueDate)
        Me.Controls.Add(Me.lblDueDate)
        Me.Controls.Add(Me.tebOderPO)
        Me.Controls.Add(Me.lblOrderPO)
        Me.Controls.Add(Me.tebCustomer)
        Me.Controls.Add(Me.lblCutomer)
        Me.Controls.Add(Me.cbeToolSetup)
        Me.Controls.Add(Me.lblToolSetup)
        Me.Controls.Add(Me.lblMachine)
        Me.Controls.Add(Me.cbeMachine)
        Me.Controls.Add(Me.lblRequired)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmOpenProject"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Import CAD Drawing"
        CType(Me.cbeMachine.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeToolSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebCustomer.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.deDueDate.Properties.CalendarTimeProperties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.deDueDate.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebOderPO.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeLayerSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tl, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialThickness.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialWidth.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebMaterialLength.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeMaterial.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents lblRequired As DevExpress.XtraEditors.LabelControl
    Friend WithEvents cmdOK As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents cmdCancel As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents cbeMachine As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents lblMachine As DevExpress.XtraEditors.LabelControl
    Friend WithEvents lblToolSetup As DevExpress.XtraEditors.LabelControl
    Friend WithEvents cbeToolSetup As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents tebCustomer As DevExpress.XtraEditors.TextEdit
    Friend WithEvents lblCutomer As DevExpress.XtraEditors.LabelControl
    Friend WithEvents lblMaterial As DevExpress.XtraEditors.LabelControl
    Friend WithEvents deDueDate As DevExpress.XtraEditors.DateEdit
    Friend WithEvents lblDueDate As DevExpress.XtraEditors.LabelControl
    Friend WithEvents tebOderPO As DevExpress.XtraEditors.TextEdit
    Friend WithEvents lblOrderPO As DevExpress.XtraEditors.LabelControl
    Friend WithEvents cbeLayerSetup As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents lblLayerSetup As DevExpress.XtraEditors.LabelControl
    Private WithEvents tl As DevExpress.XtraTreeList.TreeList
    Private WithEvents MainRoot As DevExpress.XtraTreeList.Columns.TreeListColumn
    Friend WithEvents tebMaterialThickness As TextEdit
    Friend WithEvents tebMaterialWidth As TextEdit
    Friend WithEvents tebMaterialLength As TextEdit
    Friend WithEvents cbeMaterial As ComboBoxEdit
End Class
