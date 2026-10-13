<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmMRP
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMRP))
        Me.bbCMDB = New DevExpress.XtraBars.BarEditItem()
        Me.sbCancel = New DevExpress.XtraEditors.SimpleButton()
        Me.ceMaterialToLayerSetup = New DevExpress.XtraEditors.CheckEdit()
        Me.BarManager1 = New DevExpress.XtraBars.BarManager(Me.components)
        Me.barDockControlTop = New DevExpress.XtraBars.BarDockControl()
        Me.barDockControlBottom = New DevExpress.XtraBars.BarDockControl()
        Me.barDockControlLeft = New DevExpress.XtraBars.BarDockControl()
        Me.barDockControlRight = New DevExpress.XtraBars.BarDockControl()
        Me.bbiUnlink = New DevExpress.XtraBars.BarButtonItem()
        Me.ceMaterialToToolSetup = New DevExpress.XtraEditors.CheckEdit()
        Me.lstPDBNames = New DevExpress.XtraEditors.ListBoxControl()
        Me.lbOptions = New DevExpress.XtraEditors.ListBoxControl()
        Me.tbFieldValues = New System.Windows.Forms.TextBox()
        Me.tbFieldNames = New System.Windows.Forms.TextBox()
        Me.SpreadsheetControl1 = New DevExpress.XtraSpreadsheet.SpreadsheetControl()
        Me.cboFieldName = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.teProvidedName = New DevExpress.XtraEditors.TextEdit()
        Me.teImportFilename = New DevExpress.XtraEditors.TextEdit()
        Me.vgNestDefaults = New DevExpress.XtraVerticalGrid.VGridControl()
        Me.catNestDefaults = New DevExpress.XtraVerticalGrid.Rows.CategoryRow()
        Me.vgPartDefaults = New DevExpress.XtraVerticalGrid.VGridControl()
        Me.catPartDefaultInfo = New DevExpress.XtraVerticalGrid.Rows.CategoryRow()
        Me.lcbFileType = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.DateEdit1 = New DevExpress.XtraEditors.DateEdit()
        Me.tebOderPO = New DevExpress.XtraEditors.TextEdit()
        Me.tebCustomer = New DevExpress.XtraEditors.TextEdit()
        Me.cbeLeadSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.cbeLayerSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.cbeMaterial = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.cbeToolSetup = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.cbeMachine = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.cboSheetTable = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.fcbSourceFolder = New DevExpress.XtraEditors.ButtonEdit()
        Me.fcbImportFile = New DevExpress.XtraEditors.ButtonEdit()
        Me.chkAppendMat = New DevExpress.XtraEditors.CheckEdit()
        Me.chkCopySource = New DevExpress.XtraEditors.CheckEdit()
        Me.chkHasPath = New DevExpress.XtraEditors.CheckEdit()
        Me.chkHeader = New DevExpress.XtraEditors.CheckEdit()
        Me.rgMrpFileType = New DevExpress.XtraEditors.RadioGroup()
        Me.TV_MappedFields = New DevExpress.XtraTreeList.TreeList()
        Me.TreeListColumn3 = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.TV_NestConfig = New DevExpress.XtraTreeList.TreeList()
        Me.TreeListColumn2 = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.TV_UserFields = New DevExpress.XtraTreeList.TreeList()
        Me.TreeListColumn1 = New DevExpress.XtraTreeList.Columns.TreeListColumn()
        Me.optPDBName = New DevExpress.XtraEditors.RadioGroup()
        Me.fcbMM2Source = New DevExpress.XtraEditors.ButtonEdit()
        Me.fcbResultsDirectory = New DevExpress.XtraEditors.ButtonEdit()
        Me.cbeAndThenBy = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.cbeThenBy = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.cbeSortBy = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.chkAndThenBy = New DevExpress.XtraEditors.CheckEdit()
        Me.chkThenBy = New DevExpress.XtraEditors.CheckEdit()
        Me.chkSortBy = New DevExpress.XtraEditors.CheckEdit()
        Me.sbFinish = New DevExpress.XtraEditors.SimpleButton()
        Me.sbNext = New DevExpress.XtraEditors.SimpleButton()
        Me.sbBack = New DevExpress.XtraEditors.SimpleButton()
        Me.cbeAndThenBy2 = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.chkAndThenBy2 = New DevExpress.XtraEditors.CheckEdit()
        Me.LayoutConverter1 = New DevExpress.XtraLayout.Converter.LayoutConverter(Me.components)
        Me.PopupMenu1 = New DevExpress.XtraBars.PopupMenu(Me.components)
        Me.DnDHighlightHelper1 = New FabV22_WIN8.WindowsApplication1.DnDHighlightHelper()
        Me.SplashScreenManager1 = New DevExpress.XtraSplashScreen.SplashScreenManager(Me, GetType(Global.FabV22_WIN8.WaitForm1), True, True)
        Me.gcMrpSettings = New DevExpress.XtraEditors.GroupControl()
        Me.xtcMRP = New DevExpress.XtraTab.XtraTabControl()
        Me.TabMrpSettings = New DevExpress.XtraTab.XtraTabPage()
        Me.GroupControl2 = New DevExpress.XtraEditors.GroupControl()
        Me.LabelControl1 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl2 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl3 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl4 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl5 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl6 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl7 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl8 = New DevExpress.XtraEditors.LabelControl()
        Me.GroupControl3 = New DevExpress.XtraEditors.GroupControl()
        Me.LabelControl9 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl10 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl11 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl12 = New DevExpress.XtraEditors.LabelControl()
        Me.GroupControl4 = New DevExpress.XtraEditors.GroupControl()
        Me.TabMrpPreview = New DevExpress.XtraTab.XtraTabPage()
        Me.TabMrpMapping = New DevExpress.XtraTab.XtraTabPage()
        Me.TabMrpFilesProcessed = New DevExpress.XtraTab.XtraTabPage()
        Me.SplitContainerControl2 = New DevExpress.XtraEditors.SplitContainerControl()
        Me.gcFileFromMRP = New DevExpress.XtraEditors.GroupControl()
        Me.lbcFilesFromMrp = New DevExpress.XtraEditors.ListBoxControl()
        Me.gcFileNotFOund = New DevExpress.XtraEditors.GroupControl()
        Me.ListBoxControl2 = New DevExpress.XtraEditors.ListBoxControl()
        Me.lbcFilesNotFound = New DevExpress.XtraEditors.ListBoxControl()
        Me.gcFilesFOund = New DevExpress.XtraEditors.GroupControl()
        Me.lbcFoundFiles = New DevExpress.XtraEditors.ListBoxControl()
        Me.ListBoxControl1 = New DevExpress.XtraEditors.ListBoxControl()
        Me.TabMRPSorting = New DevExpress.XtraTab.XtraTabPage()
        Me.GroupControl5 = New DevExpress.XtraEditors.GroupControl()
        Me.GroupControl6 = New DevExpress.XtraEditors.GroupControl()
        Me.LabelControl13 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl14 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl15 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl16 = New DevExpress.XtraEditors.LabelControl()
        Me.GroupControl7 = New DevExpress.XtraEditors.GroupControl()
        Me.LabelControl17 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl18 = New DevExpress.XtraEditors.LabelControl()
        Me.TAbMrpPartDefaults = New DevExpress.XtraTab.XtraTabPage()
        Me.TabMrpNestDefaults = New DevExpress.XtraTab.XtraTabPage()
        Me.LabelControl19 = New DevExpress.XtraEditors.LabelControl()
        Me.LabelControl20 = New DevExpress.XtraEditors.LabelControl()
        CType(Me.ceMaterialToLayerSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.BarManager1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ceMaterialToToolSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.lstPDBNames, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.lbOptions, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboFieldName.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.teProvidedName.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.teImportFilename.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.vgNestDefaults, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.vgPartDefaults, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.lcbFileType.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.DateEdit1.Properties.CalendarTimeProperties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.DateEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebOderPO.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tebCustomer.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeLeadSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeLayerSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeMaterial.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeToolSetup.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeMachine.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboSheetTable.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.fcbSourceFolder.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.fcbImportFile.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chkAppendMat.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chkCopySource.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chkHasPath.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chkHeader.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.rgMrpFileType.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.TV_MappedFields, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.TV_NestConfig, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.TV_UserFields, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.optPDBName.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.fcbMM2Source.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.fcbResultsDirectory.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeAndThenBy.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeThenBy.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeSortBy.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chkAndThenBy.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chkThenBy.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chkSortBy.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cbeAndThenBy2.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.chkAndThenBy2.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.PopupMenu1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.gcMrpSettings, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gcMrpSettings.SuspendLayout()
        CType(Me.xtcMRP, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.xtcMRP.SuspendLayout()
        Me.TabMrpSettings.SuspendLayout()
        CType(Me.GroupControl2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupControl2.SuspendLayout()
        CType(Me.GroupControl3, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupControl3.SuspendLayout()
        CType(Me.GroupControl4, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupControl4.SuspendLayout()
        Me.TabMrpPreview.SuspendLayout()
        Me.TabMrpMapping.SuspendLayout()
        Me.TabMrpFilesProcessed.SuspendLayout()
        CType(Me.SplitContainerControl2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainerControl2.SuspendLayout()
        CType(Me.gcFileFromMRP, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gcFileFromMRP.SuspendLayout()
        CType(Me.lbcFilesFromMrp, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.gcFileNotFOund, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gcFileNotFOund.SuspendLayout()
        CType(Me.ListBoxControl2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.lbcFilesNotFound, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.gcFilesFOund, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gcFilesFOund.SuspendLayout()
        CType(Me.lbcFoundFiles, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ListBoxControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TabMRPSorting.SuspendLayout()
        CType(Me.GroupControl5, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupControl5.SuspendLayout()
        CType(Me.GroupControl6, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupControl6.SuspendLayout()
        CType(Me.GroupControl7, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupControl7.SuspendLayout()
        Me.TAbMrpPartDefaults.SuspendLayout()
        Me.TabMrpNestDefaults.SuspendLayout()
        Me.SuspendLayout()
        '
        'bbCMDB
        '
        Me.bbCMDB.Caption = "  CMDB File:"
        Me.bbCMDB.Edit = Nothing
        Me.bbCMDB.EditWidth = 400
        Me.bbCMDB.Id = 257
        Me.bbCMDB.Name = "bbCMDB"
        '
        'sbCancel
        '
        Me.sbCancel.Location = New System.Drawing.Point(15, 472)
        Me.sbCancel.Name = "sbCancel"
        Me.sbCancel.Size = New System.Drawing.Size(131, 22)
        Me.sbCancel.TabIndex = 7
        Me.sbCancel.Text = "Cancel"
        '
        'ceMaterialToLayerSetup
        '
        Me.ceMaterialToLayerSetup.Location = New System.Drawing.Point(265, 2)
        Me.ceMaterialToLayerSetup.MenuManager = Me.BarManager1
        Me.ceMaterialToLayerSetup.Name = "ceMaterialToLayerSetup"
        Me.ceMaterialToLayerSetup.Properties.Caption = "Use Material Name For Layer Setup"
        Me.ceMaterialToLayerSetup.Size = New System.Drawing.Size(191, 19)
        Me.ceMaterialToLayerSetup.TabIndex = 17
        '
        'BarManager1
        '
        Me.BarManager1.DockControls.Add(Me.barDockControlTop)
        Me.BarManager1.DockControls.Add(Me.barDockControlBottom)
        Me.BarManager1.DockControls.Add(Me.barDockControlLeft)
        Me.BarManager1.DockControls.Add(Me.barDockControlRight)
        Me.BarManager1.Form = Me
        Me.BarManager1.Items.AddRange(New DevExpress.XtraBars.BarItem() {Me.bbiUnlink})
        Me.BarManager1.MaxItemId = 1
        '
        'barDockControlTop
        '
        Me.barDockControlTop.CausesValidation = False
        Me.barDockControlTop.Dock = System.Windows.Forms.DockStyle.Top
        Me.barDockControlTop.Location = New System.Drawing.Point(0, 0)
        Me.barDockControlTop.Manager = Me.BarManager1
        Me.barDockControlTop.Size = New System.Drawing.Size(833, 0)
        '
        'barDockControlBottom
        '
        Me.barDockControlBottom.CausesValidation = False
        Me.barDockControlBottom.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.barDockControlBottom.Location = New System.Drawing.Point(0, 501)
        Me.barDockControlBottom.Manager = Me.BarManager1
        Me.barDockControlBottom.Size = New System.Drawing.Size(833, 0)
        '
        'barDockControlLeft
        '
        Me.barDockControlLeft.CausesValidation = False
        Me.barDockControlLeft.Dock = System.Windows.Forms.DockStyle.Left
        Me.barDockControlLeft.Location = New System.Drawing.Point(0, 0)
        Me.barDockControlLeft.Manager = Me.BarManager1
        Me.barDockControlLeft.Size = New System.Drawing.Size(0, 501)
        '
        'barDockControlRight
        '
        Me.barDockControlRight.CausesValidation = False
        Me.barDockControlRight.Dock = System.Windows.Forms.DockStyle.Right
        Me.barDockControlRight.Location = New System.Drawing.Point(833, 0)
        Me.barDockControlRight.Manager = Me.BarManager1
        Me.barDockControlRight.Size = New System.Drawing.Size(0, 501)
        '
        'bbiUnlink
        '
        Me.bbiUnlink.Caption = "Un-Map Link"
        Me.bbiUnlink.Id = 0
        Me.bbiUnlink.Name = "bbiUnlink"
        '
        'ceMaterialToToolSetup
        '
        Me.ceMaterialToToolSetup.Location = New System.Drawing.Point(77, 2)
        Me.ceMaterialToToolSetup.MenuManager = Me.BarManager1
        Me.ceMaterialToToolSetup.Name = "ceMaterialToToolSetup"
        Me.ceMaterialToToolSetup.Properties.Caption = "Use Material Name For Tool Setup"
        Me.ceMaterialToToolSetup.Size = New System.Drawing.Size(184, 19)
        Me.ceMaterialToToolSetup.TabIndex = 17
        '
        'lstPDBNames
        '
        Me.lstPDBNames.Location = New System.Drawing.Point(325, 582)
        Me.lstPDBNames.Name = "lstPDBNames"
        Me.lstPDBNames.Size = New System.Drawing.Size(319, 20)
        Me.lstPDBNames.TabIndex = 82
        Me.lstPDBNames.Visible = False
        '
        'lbOptions
        '
        Me.lbOptions.Appearance.BackColor = System.Drawing.Color.White
        Me.lbOptions.Appearance.Options.UseBackColor = True
        Me.lbOptions.Location = New System.Drawing.Point(2, 554)
        Me.lbOptions.MaximumSize = New System.Drawing.Size(0, 50)
        Me.lbOptions.Name = "lbOptions"
        Me.lbOptions.Size = New System.Drawing.Size(0, 20)
        Me.lbOptions.TabIndex = 75
        Me.lbOptions.Visible = False
        '
        'tbFieldValues
        '
        Me.tbFieldValues.Location = New System.Drawing.Point(129, 534)
        Me.tbFieldValues.Name = "tbFieldValues"
        Me.tbFieldValues.Size = New System.Drawing.Size(515, 21)
        Me.tbFieldValues.TabIndex = 79
        Me.tbFieldValues.Visible = False
        '
        'tbFieldNames
        '
        Me.tbFieldNames.Location = New System.Drawing.Point(129, 558)
        Me.tbFieldNames.Name = "tbFieldNames"
        Me.tbFieldNames.Size = New System.Drawing.Size(515, 21)
        Me.tbFieldNames.TabIndex = 78
        Me.tbFieldNames.Visible = False
        '
        'SpreadsheetControl1
        '
        Me.SpreadsheetControl1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SpreadsheetControl1.Location = New System.Drawing.Point(0, 0)
        Me.SpreadsheetControl1.MenuManager = Me.BarManager1
        Me.SpreadsheetControl1.Name = "SpreadsheetControl1"
        Me.SpreadsheetControl1.Options.Behavior.Column.AutoFit = DevExpress.XtraSpreadsheet.DocumentCapability.Enabled
        Me.SpreadsheetControl1.Options.Behavior.Column.Resize = DevExpress.XtraSpreadsheet.DocumentCapability.Enabled
        Me.SpreadsheetControl1.Options.Behavior.Row.AutoFit = DevExpress.XtraSpreadsheet.DocumentCapability.Enabled
        Me.SpreadsheetControl1.Options.Cells.AutoFitMergedCellRowHeight = True
        Me.SpreadsheetControl1.Options.Cells.KeepCellAlignmentOnShowFormulas = True
        Me.SpreadsheetControl1.Options.Import.Csv.Encoding = CType(resources.GetObject("SpreadsheetControl1.Options.Import.Csv.Encoding"), System.Text.Encoding)
        Me.SpreadsheetControl1.Options.Import.Txt.Encoding = CType(resources.GetObject("SpreadsheetControl1.Options.Import.Txt.Encoding"), System.Text.Encoding)
        Me.SpreadsheetControl1.Options.View.ShowColumnHeaders = False
        Me.SpreadsheetControl1.Options.View.ShowPrintArea = False
        Me.SpreadsheetControl1.Options.View.ShowRowHeaders = False
        Me.SpreadsheetControl1.Size = New System.Drawing.Size(804, 429)
        Me.SpreadsheetControl1.TabIndex = 3
        Me.SpreadsheetControl1.Text = "SpreadsheetControl1"
        '
        'cboFieldName
        '
        Me.cboFieldName.Location = New System.Drawing.Point(389, 114)
        Me.cboFieldName.Name = "cboFieldName"
        Me.cboFieldName.Properties.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.NoBorder
        Me.cboFieldName.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboFieldName.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboFieldName.Size = New System.Drawing.Size(278, 18)
        Me.cboFieldName.TabIndex = 77
        Me.cboFieldName.Visible = False
        '
        'teProvidedName
        '
        Me.teProvidedName.Location = New System.Drawing.Point(389, 75)
        Me.teProvidedName.Name = "teProvidedName"
        Me.teProvidedName.Properties.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.NoBorder
        Me.teProvidedName.Size = New System.Drawing.Size(278, 18)
        Me.teProvidedName.TabIndex = 76
        Me.teProvidedName.Visible = False
        '
        'teImportFilename
        '
        Me.teImportFilename.Enabled = False
        Me.teImportFilename.Location = New System.Drawing.Point(389, 39)
        Me.teImportFilename.Name = "teImportFilename"
        Me.teImportFilename.Properties.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.NoBorder
        Me.teImportFilename.Properties.ReadOnly = True
        Me.teImportFilename.Size = New System.Drawing.Size(278, 18)
        Me.teImportFilename.TabIndex = 75
        Me.teImportFilename.Visible = False
        '
        'vgNestDefaults
        '
        Me.vgNestDefaults.Cursor = System.Windows.Forms.Cursors.SizeNS
        Me.vgNestDefaults.Dock = System.Windows.Forms.DockStyle.Fill
        Me.vgNestDefaults.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgNestDefaults.Location = New System.Drawing.Point(0, 0)
        Me.vgNestDefaults.Name = "vgNestDefaults"
        Me.vgNestDefaults.OptionsBehavior.AutoFocusNewRecord = True
        Me.vgNestDefaults.OptionsBehavior.UseEnterAsTab = True
        Me.vgNestDefaults.OptionsView.AutoScaleBands = True
        Me.vgNestDefaults.OptionsView.ShowButtons = False
        Me.vgNestDefaults.Rows.AddRange(New DevExpress.XtraVerticalGrid.Rows.BaseRow() {Me.catNestDefaults})
        Me.vgNestDefaults.Size = New System.Drawing.Size(804, 429)
        Me.vgNestDefaults.TabIndex = 70
        '
        'catNestDefaults
        '
        Me.catNestDefaults.Height = 19
        Me.catNestDefaults.Name = "catNestDefaults"
        Me.catNestDefaults.Properties.Caption = "Enter any Nesting Defaults that are not included in the import file."
        '
        'vgPartDefaults
        '
        Me.vgPartDefaults.Cursor = System.Windows.Forms.Cursors.SizeNS
        Me.vgPartDefaults.Dock = System.Windows.Forms.DockStyle.Fill
        Me.vgPartDefaults.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgPartDefaults.Location = New System.Drawing.Point(0, 0)
        Me.vgPartDefaults.Name = "vgPartDefaults"
        Me.vgPartDefaults.OptionsBehavior.AutoFocusNewRecord = True
        Me.vgPartDefaults.OptionsBehavior.UseEnterAsTab = True
        Me.vgPartDefaults.OptionsView.AutoScaleBands = True
        Me.vgPartDefaults.OptionsView.ShowButtons = False
        Me.vgPartDefaults.Rows.AddRange(New DevExpress.XtraVerticalGrid.Rows.BaseRow() {Me.catPartDefaultInfo})
        Me.vgPartDefaults.Size = New System.Drawing.Size(804, 429)
        Me.vgPartDefaults.TabIndex = 66
        '
        'catPartDefaultInfo
        '
        Me.catPartDefaultInfo.Height = 19
        Me.catPartDefaultInfo.Name = "catPartDefaultInfo"
        Me.catPartDefaultInfo.Properties.Caption = "Enter any Part Defaults that are not included in the imprt file."
        '
        'lcbFileType
        '
        Me.lcbFileType.EditValue = "MM2"
        Me.lcbFileType.Location = New System.Drawing.Point(163, 140)
        Me.lcbFileType.Name = "lcbFileType"
        Me.lcbFileType.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.lcbFileType.Properties.Items.AddRange(New Object() {"MM2", "DWG", "DXF", "PM4"})
        Me.lcbFileType.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.lcbFileType.Size = New System.Drawing.Size(534, 20)
        Me.lcbFileType.TabIndex = 64
        '
        'DateEdit1
        '
        Me.DateEdit1.EditValue = Nothing
        Me.DateEdit1.Location = New System.Drawing.Point(163, 94)
        Me.DateEdit1.Name = "DateEdit1"
        Me.DateEdit1.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.DateEdit1.Properties.CalendarTimeProperties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.DateEdit1.Size = New System.Drawing.Size(205, 20)
        Me.DateEdit1.TabIndex = 63
        '
        'tebOderPO
        '
        Me.tebOderPO.EditValue = "<Enter PO or Work Order>"
        Me.tebOderPO.Location = New System.Drawing.Point(478, 94)
        Me.tebOderPO.Name = "tebOderPO"
        Me.tebOderPO.Properties.AutoHeight = False
        Me.tebOderPO.Size = New System.Drawing.Size(219, 20)
        Me.tebOderPO.TabIndex = 60
        '
        'tebCustomer
        '
        Me.tebCustomer.EditValue = "<Enter Custome Name>"
        Me.tebCustomer.Location = New System.Drawing.Point(477, 70)
        Me.tebCustomer.Name = "tebCustomer"
        Me.tebCustomer.Properties.AutoHeight = False
        Me.tebCustomer.Size = New System.Drawing.Size(220, 20)
        Me.tebCustomer.TabIndex = 58
        '
        'cbeLeadSetup
        '
        Me.cbeLeadSetup.Location = New System.Drawing.Point(163, 70)
        Me.cbeLeadSetup.Name = "cbeLeadSetup"
        Me.cbeLeadSetup.Properties.AutoHeight = False
        Me.cbeLeadSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeLeadSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeLeadSetup.Size = New System.Drawing.Size(205, 20)
        Me.cbeLeadSetup.TabIndex = 56
        '
        'cbeLayerSetup
        '
        Me.cbeLayerSetup.Location = New System.Drawing.Point(475, 46)
        Me.cbeLayerSetup.Name = "cbeLayerSetup"
        Me.cbeLayerSetup.Properties.AutoHeight = False
        Me.cbeLayerSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeLayerSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeLayerSetup.Size = New System.Drawing.Size(222, 20)
        Me.cbeLayerSetup.TabIndex = 54
        '
        'cbeMaterial
        '
        Me.cbeMaterial.Location = New System.Drawing.Point(163, 46)
        Me.cbeMaterial.Name = "cbeMaterial"
        Me.cbeMaterial.Properties.AutoHeight = False
        Me.cbeMaterial.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeMaterial.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeMaterial.Size = New System.Drawing.Size(205, 20)
        Me.cbeMaterial.TabIndex = 44
        '
        'cbeToolSetup
        '
        Me.cbeToolSetup.Location = New System.Drawing.Point(474, 22)
        Me.cbeToolSetup.Name = "cbeToolSetup"
        Me.cbeToolSetup.Properties.AutoHeight = False
        Me.cbeToolSetup.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeToolSetup.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeToolSetup.Size = New System.Drawing.Size(223, 20)
        Me.cbeToolSetup.TabIndex = 37
        '
        'cbeMachine
        '
        Me.cbeMachine.Location = New System.Drawing.Point(163, 22)
        Me.cbeMachine.Name = "cbeMachine"
        Me.cbeMachine.Properties.AutoHeight = False
        Me.cbeMachine.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeMachine.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cbeMachine.Size = New System.Drawing.Size(205, 20)
        Me.cbeMachine.TabIndex = 36
        '
        'cboSheetTable
        '
        Me.cboSheetTable.Location = New System.Drawing.Point(163, 92)
        Me.cboSheetTable.Name = "cboSheetTable"
        Me.cboSheetTable.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboSheetTable.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboSheetTable.Size = New System.Drawing.Size(534, 20)
        Me.cboSheetTable.TabIndex = 9
        '
        'fcbSourceFolder
        '
        Me.fcbSourceFolder.Location = New System.Drawing.Point(163, 116)
        Me.fcbSourceFolder.Name = "fcbSourceFolder"
        Me.fcbSourceFolder.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton()})
        Me.fcbSourceFolder.Size = New System.Drawing.Size(534, 20)
        Me.fcbSourceFolder.TabIndex = 2
        '
        'fcbImportFile
        '
        Me.fcbImportFile.Location = New System.Drawing.Point(163, 68)
        Me.fcbImportFile.Name = "fcbImportFile"
        Me.fcbImportFile.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton()})
        Me.fcbImportFile.Size = New System.Drawing.Size(534, 20)
        Me.fcbImportFile.TabIndex = 0
        '
        'chkAppendMat
        '
        Me.chkAppendMat.Location = New System.Drawing.Point(469, 45)
        Me.chkAppendMat.Name = "chkAppendMat"
        Me.chkAppendMat.Properties.Caption = "Append Material Name to MM2 Source"
        Me.chkAppendMat.Size = New System.Drawing.Size(228, 19)
        Me.chkAppendMat.TabIndex = 15
        '
        'chkCopySource
        '
        Me.chkCopySource.Location = New System.Drawing.Point(103, 22)
        Me.chkCopySource.Name = "chkCopySource"
        Me.chkCopySource.Properties.Caption = "Copy Source File To MM2 Folder"
        Me.chkCopySource.Size = New System.Drawing.Size(183, 19)
        Me.chkCopySource.TabIndex = 14
        '
        'chkHasPath
        '
        Me.chkHasPath.Location = New System.Drawing.Point(469, 22)
        Me.chkHasPath.Name = "chkHasPath"
        Me.chkHasPath.Properties.Caption = "Does File Contain Full Path To Source File"
        Me.chkHasPath.Size = New System.Drawing.Size(228, 19)
        Me.chkHasPath.TabIndex = 13
        '
        'chkHeader
        '
        Me.chkHeader.Location = New System.Drawing.Point(290, 22)
        Me.chkHeader.Name = "chkHeader"
        Me.chkHeader.Properties.Caption = "Does File Contain Header Row"
        Me.chkHeader.Size = New System.Drawing.Size(175, 19)
        Me.chkHeader.TabIndex = 12
        '
        'rgMrpFileType
        '
        Me.rgMrpFileType.Location = New System.Drawing.Point(78, 27)
        Me.rgMrpFileType.Name = "rgMrpFileType"
        Me.rgMrpFileType.Properties.Columns = 3
        Me.rgMrpFileType.Properties.GlyphAlignment = DevExpress.Utils.HorzAlignment.[Default]
        Me.rgMrpFileType.Properties.Items.AddRange(New DevExpress.XtraEditors.Controls.RadioGroupItem() {New DevExpress.XtraEditors.Controls.RadioGroupItem(True, "MS Excel XLS", True, "MSExcel"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "MS Acess", True, "MSAcess"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "Tab Delimited", True, "TabDelimited"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "Comma Delimited", True, "CommaDelimited"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "Semi-Colon Delimited", True, "SemiColonDelimited"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "MS Excel XLSX ", True, "MSExcelXLSX")})
        Me.rgMrpFileType.Size = New System.Drawing.Size(638, 53)
        Me.rgMrpFileType.TabIndex = 2
        '
        'TV_MappedFields
        '
        Me.TV_MappedFields.ActiveFilterEnabled = False
        Me.TV_MappedFields.Columns.AddRange(New DevExpress.XtraTreeList.Columns.TreeListColumn() {Me.TreeListColumn3})
        Me.TV_MappedFields.Cursor = System.Windows.Forms.Cursors.Default
        Me.TV_MappedFields.Location = New System.Drawing.Point(554, 27)
        Me.TV_MappedFields.MenuManager = Me.BarManager1
        Me.TV_MappedFields.Name = "TV_MappedFields"
        Me.TV_MappedFields.OptionsBehavior.AllowExpandOnDblClick = False
        Me.TV_MappedFields.OptionsBehavior.AutoChangeParent = False
        Me.TV_MappedFields.OptionsBehavior.AutoPopulateColumns = False
        Me.TV_MappedFields.OptionsBehavior.AutoSelectAllInEditor = False
        Me.TV_MappedFields.OptionsBehavior.Editable = False
        Me.TV_MappedFields.OptionsMenu.EnableColumnMenu = False
        Me.TV_MappedFields.OptionsMenu.EnableFooterMenu = False
        Me.TV_MappedFields.OptionsMenu.ShowAutoFilterRowItem = False
        Me.TV_MappedFields.OptionsSelection.InvertSelection = True
        Me.TV_MappedFields.OptionsView.ShowButtons = False
        Me.TV_MappedFields.OptionsView.ShowIndicator = False
        Me.TV_MappedFields.ShowButtonMode = DevExpress.XtraTreeList.ShowButtonModeEnum.ShowOnlyInEditor
        Me.TV_MappedFields.Size = New System.Drawing.Size(247, 381)
        Me.TV_MappedFields.TabIndex = 4
        '
        'TreeListColumn3
        '
        Me.TreeListColumn3.Caption = "Mapped Fields"
        Me.TreeListColumn3.FieldName = "Mapped Fields"
        Me.TreeListColumn3.Name = "TreeListColumn3"
        Me.TreeListColumn3.Visible = True
        Me.TreeListColumn3.VisibleIndex = 0
        '
        'TV_NestConfig
        '
        Me.TV_NestConfig.Columns.AddRange(New DevExpress.XtraTreeList.Columns.TreeListColumn() {Me.TreeListColumn2})
        Me.TV_NestConfig.Cursor = System.Windows.Forms.Cursors.Default
        Me.TV_NestConfig.Location = New System.Drawing.Point(279, 27)
        Me.TV_NestConfig.Name = "TV_NestConfig"
        Me.TV_NestConfig.OptionsBehavior.Editable = False
        Me.TV_NestConfig.OptionsDragAndDrop.DragNodesMode = DevExpress.XtraTreeList.DragNodesMode.[Single]
        Me.TV_NestConfig.OptionsView.ShowIndicator = False
        Me.TV_NestConfig.Size = New System.Drawing.Size(270, 381)
        Me.TV_NestConfig.TabIndex = 2
        '
        'TreeListColumn2
        '
        Me.TreeListColumn2.Caption = "System Fields"
        Me.TreeListColumn2.FieldName = "System Fields"
        Me.TreeListColumn2.Name = "TreeListColumn2"
        Me.TreeListColumn2.Visible = True
        Me.TreeListColumn2.VisibleIndex = 0
        '
        'TV_UserFields
        '
        Me.TV_UserFields.Columns.AddRange(New DevExpress.XtraTreeList.Columns.TreeListColumn() {Me.TreeListColumn1})
        Me.TV_UserFields.Cursor = System.Windows.Forms.Cursors.Default
        Me.TV_UserFields.Location = New System.Drawing.Point(3, 25)
        Me.TV_UserFields.Name = "TV_UserFields"
        Me.TV_UserFields.OptionsBehavior.Editable = False
        Me.TV_UserFields.OptionsDragAndDrop.DragNodesMode = DevExpress.XtraTreeList.DragNodesMode.[Single]
        Me.TV_UserFields.OptionsView.ShowButtons = False
        Me.TV_UserFields.OptionsView.ShowIndicator = False
        Me.TV_UserFields.Size = New System.Drawing.Size(270, 381)
        Me.TV_UserFields.TabIndex = 3
        '
        'TreeListColumn1
        '
        Me.TreeListColumn1.Caption = "User Fields"
        Me.TreeListColumn1.FieldName = "User Fields"
        Me.TreeListColumn1.MinWidth = 88
        Me.TreeListColumn1.Name = "TreeListColumn1"
        Me.TreeListColumn1.Visible = True
        Me.TreeListColumn1.VisibleIndex = 0
        Me.TreeListColumn1.Width = 147
        '
        'optPDBName
        '
        Me.optPDBName.Location = New System.Drawing.Point(225, 25)
        Me.optPDBName.Name = "optPDBName"
        Me.optPDBName.Properties.Items.AddRange(New DevExpress.XtraEditors.Controls.RadioGroupItem() {New DevExpress.XtraEditors.Controls.RadioGroupItem(True, "Use Import Filename"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "Set Job Name"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "Use Field Name")})
        Me.optPDBName.Size = New System.Drawing.Size(158, 123)
        Me.optPDBName.TabIndex = 0
        '
        'fcbMM2Source
        '
        Me.fcbMM2Source.Location = New System.Drawing.Point(230, 58)
        Me.fcbMM2Source.Name = "fcbMM2Source"
        Me.fcbMM2Source.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton()})
        Me.fcbMM2Source.Size = New System.Drawing.Size(467, 20)
        Me.fcbMM2Source.TabIndex = 10
        '
        'fcbResultsDirectory
        '
        Me.fcbResultsDirectory.Location = New System.Drawing.Point(230, 34)
        Me.fcbResultsDirectory.Name = "fcbResultsDirectory"
        Me.fcbResultsDirectory.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton()})
        Me.fcbResultsDirectory.Size = New System.Drawing.Size(467, 20)
        Me.fcbResultsDirectory.TabIndex = 8
        '
        'cbeAndThenBy
        '
        Me.cbeAndThenBy.Location = New System.Drawing.Point(227, 83)
        Me.cbeAndThenBy.Name = "cbeAndThenBy"
        Me.cbeAndThenBy.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeAndThenBy.Size = New System.Drawing.Size(158, 20)
        Me.cbeAndThenBy.TabIndex = 5
        '
        'cbeThenBy
        '
        Me.cbeThenBy.Location = New System.Drawing.Point(227, 59)
        Me.cbeThenBy.Name = "cbeThenBy"
        Me.cbeThenBy.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeThenBy.Size = New System.Drawing.Size(158, 20)
        Me.cbeThenBy.TabIndex = 3
        '
        'cbeSortBy
        '
        Me.cbeSortBy.Location = New System.Drawing.Point(227, 35)
        Me.cbeSortBy.Name = "cbeSortBy"
        Me.cbeSortBy.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeSortBy.Size = New System.Drawing.Size(158, 20)
        Me.cbeSortBy.TabIndex = 1
        '
        'chkAndThenBy
        '
        Me.chkAndThenBy.Enabled = False
        Me.chkAndThenBy.Location = New System.Drawing.Point(389, 81)
        Me.chkAndThenBy.Name = "chkAndThenBy"
        Me.chkAndThenBy.Properties.Caption = "Append This Field Value To PDB Name"
        Me.chkAndThenBy.Size = New System.Drawing.Size(305, 19)
        Me.chkAndThenBy.TabIndex = 6
        '
        'chkThenBy
        '
        Me.chkThenBy.Enabled = False
        Me.chkThenBy.Location = New System.Drawing.Point(389, 58)
        Me.chkThenBy.Name = "chkThenBy"
        Me.chkThenBy.Properties.Caption = "Append This Field Value To PDB Name"
        Me.chkThenBy.Size = New System.Drawing.Size(305, 19)
        Me.chkThenBy.TabIndex = 4
        '
        'chkSortBy
        '
        Me.chkSortBy.EditValue = True
        Me.chkSortBy.Enabled = False
        Me.chkSortBy.Location = New System.Drawing.Point(389, 35)
        Me.chkSortBy.Name = "chkSortBy"
        Me.chkSortBy.Properties.Caption = "Append This Field Value To PDB Name"
        Me.chkSortBy.Size = New System.Drawing.Size(305, 19)
        Me.chkSortBy.TabIndex = 2
        '
        'sbFinish
        '
        Me.sbFinish.Enabled = False
        Me.sbFinish.Location = New System.Drawing.Point(534, 472)
        Me.sbFinish.Name = "sbFinish"
        Me.sbFinish.Size = New System.Drawing.Size(131, 22)
        Me.sbFinish.TabIndex = 10
        Me.sbFinish.Text = "Finish"
        '
        'sbNext
        '
        Me.sbNext.Location = New System.Drawing.Point(361, 472)
        Me.sbNext.Name = "sbNext"
        Me.sbNext.Size = New System.Drawing.Size(131, 22)
        Me.sbNext.TabIndex = 9
        Me.sbNext.Text = "Next >"
        '
        'sbBack
        '
        Me.sbBack.Enabled = False
        Me.sbBack.Location = New System.Drawing.Point(188, 472)
        Me.sbBack.Name = "sbBack"
        Me.sbBack.Size = New System.Drawing.Size(131, 22)
        Me.sbBack.TabIndex = 8
        Me.sbBack.Text = "< Back"
        '
        'cbeAndThenBy2
        '
        Me.cbeAndThenBy2.Location = New System.Drawing.Point(227, 107)
        Me.cbeAndThenBy2.Name = "cbeAndThenBy2"
        Me.cbeAndThenBy2.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cbeAndThenBy2.Size = New System.Drawing.Size(158, 20)
        Me.cbeAndThenBy2.TabIndex = 7
        '
        'chkAndThenBy2
        '
        Me.chkAndThenBy2.Enabled = False
        Me.chkAndThenBy2.Location = New System.Drawing.Point(391, 104)
        Me.chkAndThenBy2.Name = "chkAndThenBy2"
        Me.chkAndThenBy2.Properties.Caption = "Append This Field Value To PDB Name"
        Me.chkAndThenBy2.Size = New System.Drawing.Size(305, 19)
        Me.chkAndThenBy2.TabIndex = 8
        '
        'PopupMenu1
        '
        Me.PopupMenu1.LinksPersistInfo.AddRange(New DevExpress.XtraBars.LinkPersistInfo() {New DevExpress.XtraBars.LinkPersistInfo(Me.bbiUnlink)})
        Me.PopupMenu1.Manager = Me.BarManager1
        Me.PopupMenu1.Name = "PopupMenu1"
        '
        'DnDHighlightHelper1
        '
        Me.DnDHighlightHelper1.SelectedTreeList = Nothing
        '
        'SplashScreenManager1
        '
        Me.SplashScreenManager1.ClosingDelay = 500
        '
        'gcMrpSettings
        '
        Me.gcMrpSettings.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.gcMrpSettings.Controls.Add(Me.xtcMRP)
        Me.gcMrpSettings.Controls.Add(Me.sbFinish)
        Me.gcMrpSettings.Controls.Add(Me.sbNext)
        Me.gcMrpSettings.Controls.Add(Me.sbBack)
        Me.gcMrpSettings.Controls.Add(Me.sbCancel)
        Me.gcMrpSettings.Controls.Add(Me.LabelControl19)
        Me.gcMrpSettings.Controls.Add(Me.tbFieldValues)
        Me.gcMrpSettings.Controls.Add(Me.LabelControl20)
        Me.gcMrpSettings.Controls.Add(Me.tbFieldNames)
        Me.gcMrpSettings.Controls.Add(Me.lbOptions)
        Me.gcMrpSettings.Controls.Add(Me.lstPDBNames)
        Me.gcMrpSettings.Dock = System.Windows.Forms.DockStyle.Fill
        Me.gcMrpSettings.Location = New System.Drawing.Point(0, 0)
        Me.gcMrpSettings.Name = "gcMrpSettings"
        Me.gcMrpSettings.ShowCaption = False
        Me.gcMrpSettings.Size = New System.Drawing.Size(833, 501)
        Me.gcMrpSettings.TabIndex = 17
        Me.gcMrpSettings.Text = "Root"
        '
        'xtcMRP
        '
        Me.xtcMRP.Location = New System.Drawing.Point(12, 12)
        Me.xtcMRP.Name = "xtcMRP"
        Me.xtcMRP.SelectedTabPage = Me.TabMrpSettings
        Me.xtcMRP.Size = New System.Drawing.Size(809, 454)
        Me.xtcMRP.TabIndex = 0
        Me.xtcMRP.TabPages.AddRange(New DevExpress.XtraTab.XtraTabPage() {Me.TabMrpSettings, Me.TabMrpPreview, Me.TabMrpMapping, Me.TabMrpFilesProcessed, Me.TabMRPSorting, Me.TAbMrpPartDefaults, Me.TabMrpNestDefaults})
        '
        'TabMrpSettings
        '
        Me.TabMrpSettings.Controls.Add(Me.GroupControl2)
        Me.TabMrpSettings.Controls.Add(Me.GroupControl3)
        Me.TabMrpSettings.Controls.Add(Me.GroupControl4)
        Me.TabMrpSettings.Name = "TabMrpSettings"
        Me.TabMrpSettings.Size = New System.Drawing.Size(804, 429)
        Me.TabMrpSettings.Text = "MRP Settings"
        '
        'GroupControl2
        '
        Me.GroupControl2.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.GroupControl2.Controls.Add(Me.LabelControl1)
        Me.GroupControl2.Controls.Add(Me.DateEdit1)
        Me.GroupControl2.Controls.Add(Me.LabelControl2)
        Me.GroupControl2.Controls.Add(Me.tebOderPO)
        Me.GroupControl2.Controls.Add(Me.LabelControl3)
        Me.GroupControl2.Controls.Add(Me.tebCustomer)
        Me.GroupControl2.Controls.Add(Me.LabelControl4)
        Me.GroupControl2.Controls.Add(Me.cbeLeadSetup)
        Me.GroupControl2.Controls.Add(Me.LabelControl5)
        Me.GroupControl2.Controls.Add(Me.cbeLayerSetup)
        Me.GroupControl2.Controls.Add(Me.LabelControl6)
        Me.GroupControl2.Controls.Add(Me.cbeMaterial)
        Me.GroupControl2.Controls.Add(Me.LabelControl7)
        Me.GroupControl2.Controls.Add(Me.cbeToolSetup)
        Me.GroupControl2.Controls.Add(Me.LabelControl8)
        Me.GroupControl2.Controls.Add(Me.cbeMachine)
        Me.GroupControl2.Location = New System.Drawing.Point(0, 270)
        Me.GroupControl2.Name = "GroupControl2"
        Me.GroupControl2.Size = New System.Drawing.Size(801, 156)
        Me.GroupControl2.TabIndex = 0
        Me.GroupControl2.Text = "Model Settings"
        '
        'LabelControl1
        '
        Me.LabelControl1.Location = New System.Drawing.Point(103, 94)
        Me.LabelControl1.Name = "LabelControl1"
        Me.LabelControl1.Size = New System.Drawing.Size(49, 13)
        Me.LabelControl1.TabIndex = 0
        Me.LabelControl1.Text = "Due Date:"
        '
        'LabelControl2
        '
        Me.LabelControl2.Location = New System.Drawing.Point(410, 94)
        Me.LabelControl2.Name = "LabelControl2"
        Me.LabelControl2.Size = New System.Drawing.Size(64, 13)
        Me.LabelControl2.TabIndex = 64
        Me.LabelControl2.Text = "Order P/O #:"
        '
        'LabelControl3
        '
        Me.LabelControl3.Location = New System.Drawing.Point(422, 70)
        Me.LabelControl3.Name = "LabelControl3"
        Me.LabelControl3.Size = New System.Drawing.Size(50, 13)
        Me.LabelControl3.TabIndex = 65
        Me.LabelControl3.Text = "Customer:"
        '
        'LabelControl4
        '
        Me.LabelControl4.Location = New System.Drawing.Point(103, 70)
        Me.LabelControl4.Name = "LabelControl4"
        Me.LabelControl4.Size = New System.Drawing.Size(58, 13)
        Me.LabelControl4.TabIndex = 66
        Me.LabelControl4.Text = "Lead Setup:"
        '
        'LabelControl5
        '
        Me.LabelControl5.Location = New System.Drawing.Point(400, 46)
        Me.LabelControl5.Name = "LabelControl5"
        Me.LabelControl5.Size = New System.Drawing.Size(71, 13)
        Me.LabelControl5.TabIndex = 67
        Me.LabelControl5.Text = "* Layer Setup:"
        '
        'LabelControl6
        '
        Me.LabelControl6.Location = New System.Drawing.Point(103, 46)
        Me.LabelControl6.Name = "LabelControl6"
        Me.LabelControl6.Size = New System.Drawing.Size(51, 13)
        Me.LabelControl6.TabIndex = 68
        Me.LabelControl6.Text = "* Material:"
        '
        'LabelControl7
        '
        Me.LabelControl7.Location = New System.Drawing.Point(405, 22)
        Me.LabelControl7.Name = "LabelControl7"
        Me.LabelControl7.Size = New System.Drawing.Size(64, 13)
        Me.LabelControl7.TabIndex = 69
        Me.LabelControl7.Text = "* Tool Setup:"
        '
        'LabelControl8
        '
        Me.LabelControl8.Location = New System.Drawing.Point(103, 22)
        Me.LabelControl8.Name = "LabelControl8"
        Me.LabelControl8.Size = New System.Drawing.Size(52, 13)
        Me.LabelControl8.TabIndex = 70
        Me.LabelControl8.Text = "* Machine:"
        '
        'GroupControl3
        '
        Me.GroupControl3.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.GroupControl3.Controls.Add(Me.LabelControl9)
        Me.GroupControl3.Controls.Add(Me.cboSheetTable)
        Me.GroupControl3.Controls.Add(Me.LabelControl10)
        Me.GroupControl3.Controls.Add(Me.fcbSourceFolder)
        Me.GroupControl3.Controls.Add(Me.LabelControl11)
        Me.GroupControl3.Controls.Add(Me.fcbImportFile)
        Me.GroupControl3.Controls.Add(Me.chkAppendMat)
        Me.GroupControl3.Controls.Add(Me.chkCopySource)
        Me.GroupControl3.Controls.Add(Me.chkHasPath)
        Me.GroupControl3.Controls.Add(Me.chkHeader)
        Me.GroupControl3.Controls.Add(Me.LabelControl12)
        Me.GroupControl3.Controls.Add(Me.lcbFileType)
        Me.GroupControl3.Location = New System.Drawing.Point(0, 86)
        Me.GroupControl3.Name = "GroupControl3"
        Me.GroupControl3.Size = New System.Drawing.Size(801, 184)
        Me.GroupControl3.TabIndex = 1
        Me.GroupControl3.Text = "General Import Settings"
        '
        'LabelControl9
        '
        Me.LabelControl9.Location = New System.Drawing.Point(89, 95)
        Me.LabelControl9.Name = "LabelControl9"
        Me.LabelControl9.Size = New System.Drawing.Size(62, 13)
        Me.LabelControl9.TabIndex = 0
        Me.LabelControl9.Text = "Sheet/Table:"
        '
        'LabelControl10
        '
        Me.LabelControl10.Location = New System.Drawing.Point(15, 119)
        Me.LabelControl10.Name = "LabelControl10"
        Me.LabelControl10.Size = New System.Drawing.Size(137, 13)
        Me.LabelControl10.TabIndex = 10
        Me.LabelControl10.Text = "Import File(s) Source Folder:"
        '
        'LabelControl11
        '
        Me.LabelControl11.Location = New System.Drawing.Point(71, 71)
        Me.LabelControl11.Name = "LabelControl11"
        Me.LabelControl11.Size = New System.Drawing.Size(79, 13)
        Me.LabelControl11.TabIndex = 11
        Me.LabelControl11.Text = "MRP Import File:"
        '
        'LabelControl12
        '
        Me.LabelControl12.Location = New System.Drawing.Point(68, 143)
        Me.LabelControl12.Name = "LabelControl12"
        Me.LabelControl12.Size = New System.Drawing.Size(82, 13)
        Me.LabelControl12.TabIndex = 16
        Me.LabelControl12.Text = "Import File Type:"
        '
        'GroupControl4
        '
        Me.GroupControl4.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.GroupControl4.Controls.Add(Me.rgMrpFileType)
        Me.GroupControl4.Location = New System.Drawing.Point(0, 0)
        Me.GroupControl4.Name = "GroupControl4"
        Me.GroupControl4.Size = New System.Drawing.Size(801, 80)
        Me.GroupControl4.TabIndex = 2
        Me.GroupControl4.Text = "MRP File Type"
        '
        'TabMrpPreview
        '
        Me.TabMrpPreview.Controls.Add(Me.SpreadsheetControl1)
        Me.TabMrpPreview.Name = "TabMrpPreview"
        Me.TabMrpPreview.Size = New System.Drawing.Size(804, 429)
        Me.TabMrpPreview.Text = "Preview"
        '
        'TabMrpMapping
        '
        Me.TabMrpMapping.Controls.Add(Me.TV_MappedFields)
        Me.TabMrpMapping.Controls.Add(Me.TV_NestConfig)
        Me.TabMrpMapping.Controls.Add(Me.TV_UserFields)
        Me.TabMrpMapping.Controls.Add(Me.ceMaterialToToolSetup)
        Me.TabMrpMapping.Controls.Add(Me.ceMaterialToLayerSetup)
        Me.TabMrpMapping.Name = "TabMrpMapping"
        Me.TabMrpMapping.Size = New System.Drawing.Size(804, 429)
        Me.TabMrpMapping.Text = "Mapping"
        '
        'TabMrpFilesProcessed
        '
        Me.TabMrpFilesProcessed.Controls.Add(Me.SplitContainerControl2)
        Me.TabMrpFilesProcessed.Controls.Add(Me.ListBoxControl1)
        Me.TabMrpFilesProcessed.Name = "TabMrpFilesProcessed"
        Me.TabMrpFilesProcessed.Size = New System.Drawing.Size(804, 429)
        Me.TabMrpFilesProcessed.Text = "Files Processed"
        '
        'SplitContainerControl2
        '
        Me.SplitContainerControl2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainerControl2.Location = New System.Drawing.Point(0, 0)
        Me.SplitContainerControl2.Name = "SplitContainerControl2"
        Me.SplitContainerControl2.Panel1.Controls.Add(Me.gcFileFromMRP)
        Me.SplitContainerControl2.Panel1.Controls.Add(Me.gcFileNotFOund)
        Me.SplitContainerControl2.Panel1.Text = "Panel1"
        Me.SplitContainerControl2.Panel2.Controls.Add(Me.gcFilesFOund)
        Me.SplitContainerControl2.Panel2.Text = "Panel2"
        Me.SplitContainerControl2.Size = New System.Drawing.Size(804, 429)
        Me.SplitContainerControl2.SplitterPosition = 460
        Me.SplitContainerControl2.TabIndex = 5
        Me.SplitContainerControl2.Text = "SplitContainerControl1"
        '
        'gcFileFromMRP
        '
        Me.gcFileFromMRP.Controls.Add(Me.lbcFilesFromMrp)
        Me.gcFileFromMRP.Dock = System.Windows.Forms.DockStyle.Left
        Me.gcFileFromMRP.Location = New System.Drawing.Point(0, 0)
        Me.gcFileFromMRP.Name = "gcFileFromMRP"
        Me.gcFileFromMRP.Size = New System.Drawing.Size(176, 429)
        Me.gcFileFromMRP.TabIndex = 5
        Me.gcFileFromMRP.Text = "Files From MRP"
        '
        'lbcFilesFromMrp
        '
        Me.lbcFilesFromMrp.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbcFilesFromMrp.Location = New System.Drawing.Point(2, 22)
        Me.lbcFilesFromMrp.Name = "lbcFilesFromMrp"
        Me.lbcFilesFromMrp.Size = New System.Drawing.Size(172, 405)
        Me.lbcFilesFromMrp.TabIndex = 0
        '
        'gcFileNotFOund
        '
        Me.gcFileNotFOund.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.gcFileNotFOund.Controls.Add(Me.ListBoxControl2)
        Me.gcFileNotFOund.Controls.Add(Me.lbcFilesNotFound)
        Me.gcFileNotFOund.Dock = System.Windows.Forms.DockStyle.Right
        Me.gcFileNotFOund.Location = New System.Drawing.Point(181, 0)
        Me.gcFileNotFOund.Name = "gcFileNotFOund"
        Me.gcFileNotFOund.Size = New System.Drawing.Size(279, 429)
        Me.gcFileNotFOund.TabIndex = 4
        Me.gcFileNotFOund.Text = "Files Not Found Or Wrong Revision"
        '
        'ListBoxControl2
        '
        Me.ListBoxControl2.Location = New System.Drawing.Point(178, 77)
        Me.ListBoxControl2.Name = "ListBoxControl2"
        Me.ListBoxControl2.Size = New System.Drawing.Size(120, 95)
        Me.ListBoxControl2.TabIndex = 4
        Me.ListBoxControl2.Visible = False
        '
        'lbcFilesNotFound
        '
        Me.lbcFilesNotFound.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbcFilesNotFound.HorizontalScrollbar = True
        Me.lbcFilesNotFound.Location = New System.Drawing.Point(2, 22)
        Me.lbcFilesNotFound.Name = "lbcFilesNotFound"
        Me.lbcFilesNotFound.Size = New System.Drawing.Size(275, 405)
        Me.lbcFilesNotFound.TabIndex = 3
        '
        'gcFilesFOund
        '
        Me.gcFilesFOund.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.gcFilesFOund.Controls.Add(Me.lbcFoundFiles)
        Me.gcFilesFOund.Dock = System.Windows.Forms.DockStyle.Fill
        Me.gcFilesFOund.Location = New System.Drawing.Point(0, 0)
        Me.gcFilesFOund.Name = "gcFilesFOund"
        Me.gcFilesFOund.Size = New System.Drawing.Size(338, 429)
        Me.gcFilesFOund.TabIndex = 3
        Me.gcFilesFOund.Text = "Files Found"
        '
        'lbcFoundFiles
        '
        Me.lbcFoundFiles.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbcFoundFiles.HorizontalScrollbar = True
        Me.lbcFoundFiles.Location = New System.Drawing.Point(2, 22)
        Me.lbcFoundFiles.Name = "lbcFoundFiles"
        Me.lbcFoundFiles.Size = New System.Drawing.Size(334, 405)
        Me.lbcFoundFiles.TabIndex = 1
        '
        'ListBoxControl1
        '
        Me.ListBoxControl1.Location = New System.Drawing.Point(44, 260)
        Me.ListBoxControl1.Name = "ListBoxControl1"
        Me.ListBoxControl1.Size = New System.Drawing.Size(8, 8)
        Me.ListBoxControl1.TabIndex = 1
        '
        'TabMRPSorting
        '
        Me.TabMRPSorting.Controls.Add(Me.GroupControl5)
        Me.TabMRPSorting.Controls.Add(Me.GroupControl6)
        Me.TabMRPSorting.Controls.Add(Me.GroupControl7)
        Me.TabMRPSorting.Name = "TabMRPSorting"
        Me.TabMRPSorting.Size = New System.Drawing.Size(804, 429)
        Me.TabMRPSorting.Text = "Sorting"
        '
        'GroupControl5
        '
        Me.GroupControl5.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.GroupControl5.Controls.Add(Me.optPDBName)
        Me.GroupControl5.Controls.Add(Me.teImportFilename)
        Me.GroupControl5.Controls.Add(Me.teProvidedName)
        Me.GroupControl5.Controls.Add(Me.cboFieldName)
        Me.GroupControl5.Location = New System.Drawing.Point(5, 246)
        Me.GroupControl5.Name = "GroupControl5"
        Me.GroupControl5.Size = New System.Drawing.Size(796, 180)
        Me.GroupControl5.TabIndex = 0
        Me.GroupControl5.Text = "DefineJob Name"
        '
        'GroupControl6
        '
        Me.GroupControl6.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.GroupControl6.Controls.Add(Me.LabelControl13)
        Me.GroupControl6.Controls.Add(Me.cbeAndThenBy)
        Me.GroupControl6.Controls.Add(Me.LabelControl14)
        Me.GroupControl6.Controls.Add(Me.cbeThenBy)
        Me.GroupControl6.Controls.Add(Me.LabelControl15)
        Me.GroupControl6.Controls.Add(Me.cbeSortBy)
        Me.GroupControl6.Controls.Add(Me.chkAndThenBy)
        Me.GroupControl6.Controls.Add(Me.chkThenBy)
        Me.GroupControl6.Controls.Add(Me.chkSortBy)
        Me.GroupControl6.Controls.Add(Me.LabelControl16)
        Me.GroupControl6.Controls.Add(Me.cbeAndThenBy2)
        Me.GroupControl6.Controls.Add(Me.chkAndThenBy2)
        Me.GroupControl6.Location = New System.Drawing.Point(3, 99)
        Me.GroupControl6.Name = "GroupControl6"
        Me.GroupControl6.Size = New System.Drawing.Size(798, 135)
        Me.GroupControl6.TabIndex = 1
        Me.GroupControl6.Text = "Sorting Method"
        '
        'LabelControl13
        '
        Me.LabelControl13.Location = New System.Drawing.Point(156, 83)
        Me.LabelControl13.Name = "LabelControl13"
        Me.LabelControl13.Size = New System.Drawing.Size(65, 13)
        Me.LabelControl13.TabIndex = 0
        Me.LabelControl13.Text = "And Then By:"
        '
        'LabelControl14
        '
        Me.LabelControl14.Location = New System.Drawing.Point(178, 59)
        Me.LabelControl14.Name = "LabelControl14"
        Me.LabelControl14.Size = New System.Drawing.Size(43, 13)
        Me.LabelControl14.TabIndex = 6
        Me.LabelControl14.Text = "Then By:"
        '
        'LabelControl15
        '
        Me.LabelControl15.Location = New System.Drawing.Point(182, 35)
        Me.LabelControl15.Name = "LabelControl15"
        Me.LabelControl15.Size = New System.Drawing.Size(39, 13)
        Me.LabelControl15.TabIndex = 7
        Me.LabelControl15.Text = "Sort By:"
        '
        'LabelControl16
        '
        Me.LabelControl16.Location = New System.Drawing.Point(156, 110)
        Me.LabelControl16.Name = "LabelControl16"
        Me.LabelControl16.Size = New System.Drawing.Size(65, 13)
        Me.LabelControl16.TabIndex = 8
        Me.LabelControl16.Text = "And Then By:"
        '
        'GroupControl7
        '
        Me.GroupControl7.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.GroupControl7.Controls.Add(Me.LabelControl17)
        Me.GroupControl7.Controls.Add(Me.fcbMM2Source)
        Me.GroupControl7.Controls.Add(Me.LabelControl18)
        Me.GroupControl7.Controls.Add(Me.fcbResultsDirectory)
        Me.GroupControl7.Location = New System.Drawing.Point(0, 3)
        Me.GroupControl7.Name = "GroupControl7"
        Me.GroupControl7.Size = New System.Drawing.Size(801, 90)
        Me.GroupControl7.TabIndex = 2
        Me.GroupControl7.Text = "Define Results Folders"
        '
        'LabelControl17
        '
        Me.LabelControl17.Location = New System.Drawing.Point(103, 58)
        Me.LabelControl17.Name = "LabelControl17"
        Me.LabelControl17.Size = New System.Drawing.Size(124, 13)
        Me.LabelControl17.TabIndex = 0
        Me.LabelControl17.Text = "Result MM2 File(s) Folder:"
        '
        'LabelControl18
        '
        Me.LabelControl18.Location = New System.Drawing.Point(103, 34)
        Me.LabelControl18.Name = "LabelControl18"
        Me.LabelControl18.Size = New System.Drawing.Size(111, 13)
        Me.LabelControl18.TabIndex = 11
        Me.LabelControl18.Text = "Nesting Results Folder:"
        '
        'TAbMrpPartDefaults
        '
        Me.TAbMrpPartDefaults.Controls.Add(Me.vgPartDefaults)
        Me.TAbMrpPartDefaults.Name = "TAbMrpPartDefaults"
        Me.TAbMrpPartDefaults.Size = New System.Drawing.Size(804, 429)
        Me.TAbMrpPartDefaults.Text = "Part Defaults"
        '
        'TabMrpNestDefaults
        '
        Me.TabMrpNestDefaults.Controls.Add(Me.vgNestDefaults)
        Me.TabMrpNestDefaults.Name = "TabMrpNestDefaults"
        Me.TabMrpNestDefaults.Size = New System.Drawing.Size(804, 429)
        Me.TabMrpNestDefaults.Text = "Nest Setup Defaults"
        '
        'LabelControl19
        '
        Me.LabelControl19.Location = New System.Drawing.Point(2, 534)
        Me.LabelControl19.Name = "LabelControl19"
        Me.LabelControl19.Size = New System.Drawing.Size(102, 13)
        Me.LabelControl19.TabIndex = 11
        Me.LabelControl19.Text = "LayoutControlItem39"
        '
        'LabelControl20
        '
        Me.LabelControl20.Location = New System.Drawing.Point(2, 558)
        Me.LabelControl20.Name = "LabelControl20"
        Me.LabelControl20.Size = New System.Drawing.Size(102, 13)
        Me.LabelControl20.TabIndex = 80
        Me.LabelControl20.Text = "LayoutControlItem40"
        '
        'frmMRP
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(833, 501)
        Me.Controls.Add(Me.gcMrpSettings)
        Me.Controls.Add(Me.barDockControlLeft)
        Me.Controls.Add(Me.barDockControlRight)
        Me.Controls.Add(Me.barDockControlBottom)
        Me.Controls.Add(Me.barDockControlTop)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow
        Me.Name = "frmMRP"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "MRP Interface"
        CType(Me.ceMaterialToLayerSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.BarManager1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ceMaterialToToolSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.lstPDBNames, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.lbOptions, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboFieldName.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.teProvidedName.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.teImportFilename.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.vgNestDefaults, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.vgPartDefaults, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.lcbFileType.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.DateEdit1.Properties.CalendarTimeProperties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.DateEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebOderPO.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tebCustomer.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeLeadSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeLayerSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeMaterial.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeToolSetup.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeMachine.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboSheetTable.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.fcbSourceFolder.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.fcbImportFile.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chkAppendMat.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chkCopySource.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chkHasPath.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chkHeader.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.rgMrpFileType.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.TV_MappedFields, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.TV_NestConfig, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.TV_UserFields, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.optPDBName.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.fcbMM2Source.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.fcbResultsDirectory.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeAndThenBy.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeThenBy.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeSortBy.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chkAndThenBy.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chkThenBy.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chkSortBy.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cbeAndThenBy2.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.chkAndThenBy2.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.PopupMenu1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.gcMrpSettings, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gcMrpSettings.ResumeLayout(False)
        Me.gcMrpSettings.PerformLayout()
        CType(Me.xtcMRP, System.ComponentModel.ISupportInitialize).EndInit()
        Me.xtcMRP.ResumeLayout(False)
        Me.TabMrpSettings.ResumeLayout(False)
        CType(Me.GroupControl2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupControl2.ResumeLayout(False)
        Me.GroupControl2.PerformLayout()
        CType(Me.GroupControl3, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupControl3.ResumeLayout(False)
        Me.GroupControl3.PerformLayout()
        CType(Me.GroupControl4, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupControl4.ResumeLayout(False)
        Me.TabMrpPreview.ResumeLayout(False)
        Me.TabMrpMapping.ResumeLayout(False)
        Me.TabMrpFilesProcessed.ResumeLayout(False)
        CType(Me.SplitContainerControl2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainerControl2.ResumeLayout(False)
        CType(Me.gcFileFromMRP, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gcFileFromMRP.ResumeLayout(False)
        CType(Me.lbcFilesFromMrp, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.gcFileNotFOund, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gcFileNotFOund.ResumeLayout(False)
        CType(Me.ListBoxControl2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.lbcFilesNotFound, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.gcFilesFOund, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gcFilesFOund.ResumeLayout(False)
        CType(Me.lbcFoundFiles, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ListBoxControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TabMRPSorting.ResumeLayout(False)
        CType(Me.GroupControl5, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupControl5.ResumeLayout(False)
        CType(Me.GroupControl6, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupControl6.ResumeLayout(False)
        Me.GroupControl6.PerformLayout()
        CType(Me.GroupControl7, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupControl7.ResumeLayout(False)
        Me.GroupControl7.PerformLayout()
        Me.TAbMrpPartDefaults.ResumeLayout(False)
        Me.TabMrpNestDefaults.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents bbCMDB As DevExpress.XtraBars.BarEditItem
    Friend WithEvents sbCancel As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbBack As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbNext As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbFinish As DevExpress.XtraEditors.SimpleButton
    Private WithEvents TV_NestConfig As DevExpress.XtraTreeList.TreeList
    Friend WithEvents TreeListColumn2 As DevExpress.XtraTreeList.Columns.TreeListColumn
    Friend WithEvents optPDBName As DevExpress.XtraEditors.RadioGroup
    Friend WithEvents fcbMM2Source As DevExpress.XtraEditors.ButtonEdit
    Friend WithEvents fcbResultsDirectory As DevExpress.XtraEditors.ButtonEdit
    Friend WithEvents chkAndThenBy As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents chkThenBy As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents chkSortBy As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents cbeAndThenBy As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents cbeThenBy As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents cbeSortBy As DevExpress.XtraEditors.ComboBoxEdit
    Private WithEvents TV_MappedFields As DevExpress.XtraTreeList.TreeList
    Friend WithEvents TreeListColumn3 As DevExpress.XtraTreeList.Columns.TreeListColumn
    Private WithEvents TV_UserFields As DevExpress.XtraTreeList.TreeList
    Friend WithEvents TreeListColumn1 As DevExpress.XtraTreeList.Columns.TreeListColumn
    Friend WithEvents SpreadsheetControl1 As DevExpress.XtraSpreadsheet.SpreadsheetControl
    Friend WithEvents DateEdit1 As DevExpress.XtraEditors.DateEdit
    Friend WithEvents tebOderPO As DevExpress.XtraEditors.TextEdit
    Friend WithEvents tebCustomer As DevExpress.XtraEditors.TextEdit
    Friend WithEvents cbeLeadSetup As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents cbeLayerSetup As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents cbeMaterial As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents cbeToolSetup As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents cbeMachine As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents chkAppendMat As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents chkCopySource As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents chkHasPath As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents chkHeader As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents cboSheetTable As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents fcbSourceFolder As DevExpress.XtraEditors.ButtonEdit
    Friend WithEvents fcbImportFile As DevExpress.XtraEditors.ButtonEdit
    Friend WithEvents rgMrpFileType As DevExpress.XtraEditors.RadioGroup
    Friend WithEvents LayoutConverter1 As DevExpress.XtraLayout.Converter.LayoutConverter
    Friend WithEvents lcbFileType As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents BarManager1 As DevExpress.XtraBars.BarManager
    Friend WithEvents barDockControlTop As DevExpress.XtraBars.BarDockControl
    Friend WithEvents barDockControlBottom As DevExpress.XtraBars.BarDockControl
    Friend WithEvents barDockControlLeft As DevExpress.XtraBars.BarDockControl
    Friend WithEvents barDockControlRight As DevExpress.XtraBars.BarDockControl
    Friend WithEvents bbiUnlink As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents PopupMenu1 As DevExpress.XtraBars.PopupMenu
    Friend WithEvents vgPartDefaults As DevExpress.XtraVerticalGrid.VGridControl
    Friend WithEvents catPartDefaultInfo As DevExpress.XtraVerticalGrid.Rows.CategoryRow
    Friend WithEvents vgNestDefaults As DevExpress.XtraVerticalGrid.VGridControl
    Friend WithEvents catNestDefaults As DevExpress.XtraVerticalGrid.Rows.CategoryRow
    Friend WithEvents DnDHighlightHelper1 As FabV22_WIN8.WindowsApplication1.DnDHighlightHelper
    Friend WithEvents cbeAndThenBy2 As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents chkAndThenBy2 As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents cboFieldName As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents teProvidedName As DevExpress.XtraEditors.TextEdit
    Friend WithEvents teImportFilename As DevExpress.XtraEditors.TextEdit
    Friend WithEvents tbFieldValues As System.Windows.Forms.TextBox
    Friend WithEvents tbFieldNames As System.Windows.Forms.TextBox
    Friend WithEvents lbOptions As DevExpress.XtraEditors.ListBoxControl
    Friend WithEvents lstPDBNames As DevExpress.XtraEditors.ListBoxControl
    Friend WithEvents ceMaterialToLayerSetup As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents ceMaterialToToolSetup As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents SplashScreenManager1 As DevExpress.XtraSplashScreen.SplashScreenManager
    Friend WithEvents gcMrpSettings As GroupControl
    Friend WithEvents xtcMRP As DevExpress.XtraTab.XtraTabControl
    Friend WithEvents TabMrpSettings As DevExpress.XtraTab.XtraTabPage
    Friend WithEvents GroupControl2 As GroupControl
    Friend WithEvents LabelControl1 As LabelControl
    Friend WithEvents LabelControl2 As LabelControl
    Friend WithEvents LabelControl3 As LabelControl
    Friend WithEvents LabelControl4 As LabelControl
    Friend WithEvents LabelControl5 As LabelControl
    Friend WithEvents LabelControl6 As LabelControl
    Friend WithEvents LabelControl7 As LabelControl
    Friend WithEvents LabelControl8 As LabelControl
    Friend WithEvents GroupControl3 As GroupControl
    Friend WithEvents LabelControl9 As LabelControl
    Friend WithEvents LabelControl10 As LabelControl
    Friend WithEvents LabelControl11 As LabelControl
    Friend WithEvents LabelControl12 As LabelControl
    Friend WithEvents GroupControl4 As GroupControl
    Friend WithEvents TabMrpFilesProcessed As DevExpress.XtraTab.XtraTabPage
    Friend WithEvents ListBoxControl1 As ListBoxControl
    Friend WithEvents TabMrpPreview As DevExpress.XtraTab.XtraTabPage
    Friend WithEvents TabMrpMapping As DevExpress.XtraTab.XtraTabPage
    Friend WithEvents TabMRPSorting As DevExpress.XtraTab.XtraTabPage
    Friend WithEvents GroupControl5 As GroupControl
    Friend WithEvents GroupControl6 As GroupControl
    Friend WithEvents LabelControl13 As LabelControl
    Friend WithEvents LabelControl14 As LabelControl
    Friend WithEvents LabelControl15 As LabelControl
    Friend WithEvents LabelControl16 As LabelControl
    Friend WithEvents GroupControl7 As GroupControl
    Friend WithEvents LabelControl17 As LabelControl
    Friend WithEvents LabelControl18 As LabelControl
    Friend WithEvents TAbMrpPartDefaults As DevExpress.XtraTab.XtraTabPage
    Friend WithEvents TabMrpNestDefaults As DevExpress.XtraTab.XtraTabPage
    Friend WithEvents LabelControl19 As LabelControl
    Friend WithEvents LabelControl20 As LabelControl
    Friend WithEvents SplitContainerControl2 As SplitContainerControl
    Friend WithEvents gcFileFromMRP As GroupControl
    Friend WithEvents lbcFilesFromMrp As ListBoxControl
    Friend WithEvents gcFileNotFOund As GroupControl
    Friend WithEvents ListBoxControl2 As ListBoxControl
    Friend WithEvents lbcFilesNotFound As ListBoxControl
    Friend WithEvents gcFilesFOund As GroupControl
    Friend WithEvents lbcFoundFiles As ListBoxControl
End Class
