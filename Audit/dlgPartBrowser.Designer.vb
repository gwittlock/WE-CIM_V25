<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlgPartBrowser
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
        Me.components = New System.ComponentModel.Container()
        Me.LayoutConverter1 = New DevExpress.XtraLayout.Converter.LayoutConverter(Me.components)
        Me.sccPartBrowser = New DevExpress.XtraEditors.SplitContainerControl()
        Me.GroupControl1 = New DevExpress.XtraEditors.GroupControl()
        Me.cePreviewPart = New DevExpress.XtraEditors.CheckEdit()
        Me.ubgParts = New FabV24_WIN8.CustomGrid.CustomGrid.UnboundGridControl()
        Me.ubgPartsView = New FabV24_WIN8.CustomGrid.CustomGrid.UnboundGridView()
        Me.sbAddSelectedFiles = New DevExpress.XtraEditors.SimpleButton()
        Me.sbAddSelectedFolder = New DevExpress.XtraEditors.SimpleButton()
        Me.chkStripLayers = New System.Windows.Forms.CheckBox()
        Me.SplitContainerControl2 = New DevExpress.XtraEditors.SplitContainerControl()
        Me.fldrVw = New LogicNP.FolderViewControl.FolderView()
        Me.fgList = New LogicNP.FileViewControl.FileView(Me.components)
        Me.sbRemovePart = New DevExpress.XtraEditors.SimpleButton()
        Me.cmdOk = New DevExpress.XtraEditors.SimpleButton()
        Me.cmdCancel = New DevExpress.XtraEditors.SimpleButton()
        Me.ltbDefaultQTY = New FabV24_WIN8.lblTextBox()
        Me.lcbRepresentation = New FabV24_WIN8.LabelComboBox()
        Me.ltbDefaultRotation = New FabV24_WIN8.lblTextBox()
        Me.lcbFileType = New FabV24_WIN8.LabelComboBox()
        Me.picPreview1 = New System.Windows.Forms.PictureBox()
        CType(Me.sccPartBrowser, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.sccPartBrowser.SuspendLayout()
        CType(Me.GroupControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupControl1.SuspendLayout()
        CType(Me.cePreviewPart.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ubgParts, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ubgPartsView, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.SplitContainerControl2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainerControl2.SuspendLayout()
        CType(Me.picPreview1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'sccPartBrowser
        '
        Me.sccPartBrowser.Dock = System.Windows.Forms.DockStyle.Fill
        Me.sccPartBrowser.IsSplitterFixed = True
        Me.sccPartBrowser.Location = New System.Drawing.Point(0, 0)
        Me.sccPartBrowser.Name = "sccPartBrowser"
        Me.sccPartBrowser.Panel1.Controls.Add(Me.GroupControl1)
        Me.sccPartBrowser.Panel1.Text = "Panel1"
        Me.sccPartBrowser.Panel2.Controls.Add(Me.picPreview1)
        Me.sccPartBrowser.Panel2.Text = "Panel2"
        Me.sccPartBrowser.Size = New System.Drawing.Size(1137, 477)
        Me.sccPartBrowser.SplitterPosition = 777
        Me.sccPartBrowser.TabIndex = 0
        Me.sccPartBrowser.Text = "SplitContainerControl1"
        '
        'GroupControl1
        '
        Me.GroupControl1.CaptionLocation = DevExpress.Utils.Locations.Top
        Me.GroupControl1.Controls.Add(Me.cePreviewPart)
        Me.GroupControl1.Controls.Add(Me.ubgParts)
        Me.GroupControl1.Controls.Add(Me.sbAddSelectedFiles)
        Me.GroupControl1.Controls.Add(Me.sbAddSelectedFolder)
        Me.GroupControl1.Controls.Add(Me.chkStripLayers)
        Me.GroupControl1.Controls.Add(Me.SplitContainerControl2)
        Me.GroupControl1.Controls.Add(Me.sbRemovePart)
        Me.GroupControl1.Controls.Add(Me.cmdOk)
        Me.GroupControl1.Controls.Add(Me.cmdCancel)
        Me.GroupControl1.Controls.Add(Me.ltbDefaultQTY)
        Me.GroupControl1.Controls.Add(Me.lcbRepresentation)
        Me.GroupControl1.Controls.Add(Me.ltbDefaultRotation)
        Me.GroupControl1.Controls.Add(Me.lcbFileType)
        Me.GroupControl1.Location = New System.Drawing.Point(12, 12)
        Me.GroupControl1.Name = "GroupControl1"
        Me.GroupControl1.ShowCaption = False
        Me.GroupControl1.Size = New System.Drawing.Size(764, 444)
        Me.GroupControl1.TabIndex = 74
        Me.GroupControl1.Text = "Root"
        '
        'cePreviewPart
        '
        Me.cePreviewPart.AutoSizeInLayoutControl = True
        Me.cePreviewPart.Location = New System.Drawing.Point(307, 10)
        Me.cePreviewPart.Name = "cePreviewPart"
        Me.cePreviewPart.Properties.Caption = "Preview FIle"
        Me.cePreviewPart.Size = New System.Drawing.Size(98, 19)
        Me.cePreviewPart.TabIndex = 74
        '
        'ubgParts
        '
        Me.ubgParts.Cursor = System.Windows.Forms.Cursors.Default
        Me.ubgParts.Location = New System.Drawing.Point(2, 253)
        Me.ubgParts.MainView = Me.ubgPartsView
        Me.ubgParts.Name = "ubgParts"
        Me.ubgParts.Size = New System.Drawing.Size(740, 143)
        Me.ubgParts.TabIndex = 67
        Me.ubgParts.ViewCollection.AddRange(New DevExpress.XtraGrid.Views.Base.BaseView() {Me.ubgPartsView})
        '
        'ubgPartsView
        '
        Me.ubgPartsView.ActiveFilterEnabled = False
        Me.ubgPartsView.GridControl = Me.ubgParts
        Me.ubgPartsView.HorzScrollVisibility = DevExpress.XtraGrid.Views.Base.ScrollVisibility.Always
        Me.ubgPartsView.Name = "ubgPartsView"
        Me.ubgPartsView.OptionsCustomization.AllowColumnMoving = False
        Me.ubgPartsView.OptionsCustomization.AllowFilter = False
        Me.ubgPartsView.OptionsCustomization.AllowGroup = False
        Me.ubgPartsView.OptionsCustomization.AllowQuickHideColumns = False
        Me.ubgPartsView.OptionsCustomization.AllowSort = False
        Me.ubgPartsView.OptionsDetail.ShowDetailTabs = False
        Me.ubgPartsView.OptionsNavigation.EnterMoveNextColumn = True
        Me.ubgPartsView.OptionsView.ColumnAutoWidth = False
        Me.ubgPartsView.OptionsView.ShowDetailButtons = False
        Me.ubgPartsView.OptionsView.ShowFilterPanelMode = DevExpress.XtraGrid.Views.Base.ShowFilterPanelMode.Never
        Me.ubgPartsView.OptionsView.ShowGroupPanel = False
        Me.ubgPartsView.OptionsView.ShowIndicator = False
        Me.ubgPartsView.VertScrollVisibility = DevExpress.XtraGrid.Views.Base.ScrollVisibility.Always
        '
        'sbAddSelectedFiles
        '
        Me.sbAddSelectedFiles.Location = New System.Drawing.Point(459, 227)
        Me.sbAddSelectedFiles.Name = "sbAddSelectedFiles"
        Me.sbAddSelectedFiles.Size = New System.Drawing.Size(283, 22)
        Me.sbAddSelectedFiles.TabIndex = 66
        Me.sbAddSelectedFiles.Text = "Add Selected Files"
        '
        'sbAddSelectedFolder
        '
        Me.sbAddSelectedFolder.Location = New System.Drawing.Point(2, 227)
        Me.sbAddSelectedFolder.Name = "sbAddSelectedFolder"
        Me.sbAddSelectedFolder.Size = New System.Drawing.Size(301, 22)
        Me.sbAddSelectedFolder.TabIndex = 64
        Me.sbAddSelectedFolder.Text = "Add Selected Folder"
        '
        'chkStripLayers
        '
        Me.chkStripLayers.Location = New System.Drawing.Point(307, 227)
        Me.chkStripLayers.Name = "chkStripLayers"
        Me.chkStripLayers.Padding = New System.Windows.Forms.Padding(20, 0, 0, 0)
        Me.chkStripLayers.Size = New System.Drawing.Size(148, 22)
        Me.chkStripLayers.TabIndex = 65
        Me.chkStripLayers.Text = "Strip Layers"
        Me.chkStripLayers.UseVisualStyleBackColor = True
        '
        'SplitContainerControl2
        '
        Me.SplitContainerControl2.Location = New System.Drawing.Point(2, 94)
        Me.SplitContainerControl2.Name = "SplitContainerControl2"
        Me.SplitContainerControl2.Panel1.Controls.Add(Me.fldrVw)
        Me.SplitContainerControl2.Panel1.Text = "Panel1"
        Me.SplitContainerControl2.Panel2.Controls.Add(Me.fgList)
        Me.SplitContainerControl2.Panel2.Text = "Panel2"
        Me.SplitContainerControl2.Size = New System.Drawing.Size(740, 129)
        Me.SplitContainerControl2.SplitterPosition = 299
        Me.SplitContainerControl2.TabIndex = 63
        Me.SplitContainerControl2.Text = "SplitContainerControl2"
        '
        'fldrVw
        '
        Me.fldrVw.Dock = System.Windows.Forms.DockStyle.Fill
        Me.fldrVw.FileView = Me.fgList
        Me.fldrVw.Location = New System.Drawing.Point(0, 0)
        Me.fldrVw.Name = "fldrVw"
        Me.fldrVw.Padding = New System.Windows.Forms.Padding(3)
        Me.fldrVw.Size = New System.Drawing.Size(299, 129)
        Me.fldrVw.TabIndex = 1
        Me.fldrVw.Text = "FolderView1"
        '
        'fgList
        '
        Me.fgList.AllowFileExecution = False
        Me.fgList.Dock = System.Windows.Forms.DockStyle.Fill
        Me.fgList.Location = New System.Drawing.Point(0, 0)
        Me.fgList.Name = "fgList"
        Me.fgList.Padding = New System.Windows.Forms.Padding(3)
        Me.fgList.ShowBackgroundMenu = False
        Me.fgList.ShowFolders = False
        Me.fgList.ShowHiddenItems = False
        Me.fgList.Size = New System.Drawing.Size(435, 129)
        Me.fgList.TabIndex = 1
        Me.fgList.Text = "FileView1"
        Me.fgList.ViewStyle = LogicNP.FileViewControl.ViewStyles.Report
        '
        'sbRemovePart
        '
        Me.sbRemovePart.Location = New System.Drawing.Point(2, 400)
        Me.sbRemovePart.Name = "sbRemovePart"
        Me.sbRemovePart.Size = New System.Drawing.Size(127, 22)
        Me.sbRemovePart.TabIndex = 72
        Me.sbRemovePart.Text = "Remove Part"
        '
        'cmdOk
        '
        Me.cmdOk.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.cmdOk.Enabled = False
        Me.cmdOk.Location = New System.Drawing.Point(517, 400)
        Me.cmdOk.Name = "cmdOk"
        Me.cmdOk.Size = New System.Drawing.Size(82, 22)
        Me.cmdOk.TabIndex = 72
        Me.cmdOk.Text = "Ok"
        '
        'cmdCancel
        '
        Me.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.cmdCancel.Location = New System.Drawing.Point(635, 400)
        Me.cmdCancel.Name = "cmdCancel"
        Me.cmdCancel.Size = New System.Drawing.Size(97, 22)
        Me.cmdCancel.TabIndex = 72
        Me.cmdCancel.Text = "Cancel"
        '
        'ltbDefaultQTY
        '
        Me.ltbDefaultQTY.BackColor = System.Drawing.Color.Transparent
        Me.ltbDefaultQTY.Caption = "Default QTY:"
        Me.ltbDefaultQTY.CaptionLocation = FabV24_WIN8.lblTextBox.CaptionLocations.clLeft
        Me.ltbDefaultQTY.FieldStyle = FabV24_WIN8.lblTextBox.FieldStyles.fsAuto
        Me.ltbDefaultQTY.FieldWidth = 200
        Me.ltbDefaultQTY.Location = New System.Drawing.Point(65, 2)
        Me.ltbDefaultQTY.MaxLength = 32767
        Me.ltbDefaultQTY.Name = "ltbDefaultQTY"
        Me.ltbDefaultQTY.Size = New System.Drawing.Size(170, 25)
        Me.ltbDefaultQTY.TabIndex = 73
        '
        'lcbRepresentation
        '
        Me.lcbRepresentation.BackColor = System.Drawing.Color.Transparent
        Me.lcbRepresentation.Caption = "Default Representation"
        Me.lcbRepresentation.CaptionLocation = FabV24_WIN8.LabelComboBox.CaptionLocations.clLeft
        Me.lcbRepresentation.FieldStyle = FabV24_WIN8.LabelComboBox.FieldStyles.fsAuto
        Me.lcbRepresentation.FieldWidth = 495
        Me.lcbRepresentation.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lcbRepresentation.ListIndex = -1
        Me.lcbRepresentation.Location = New System.Drawing.Point(15, 70)
        Me.lcbRepresentation.Name = "lcbRepresentation"
        Me.lcbRepresentation.Size = New System.Drawing.Size(220, 25)
        Me.lcbRepresentation.TabIndex = 73
        Me.lcbRepresentation.UseMnemonic = True
        '
        'ltbDefaultRotation
        '
        Me.ltbDefaultRotation.BackColor = System.Drawing.Color.Transparent
        Me.ltbDefaultRotation.Caption = "Default Rotation:"
        Me.ltbDefaultRotation.CaptionLocation = FabV24_WIN8.lblTextBox.CaptionLocations.clLeft
        Me.ltbDefaultRotation.FieldStyle = FabV24_WIN8.lblTextBox.FieldStyles.fsAuto
        Me.ltbDefaultRotation.FieldWidth = 50
        Me.ltbDefaultRotation.Location = New System.Drawing.Point(44, 36)
        Me.ltbDefaultRotation.MaxLength = 32767
        Me.ltbDefaultRotation.Name = "ltbDefaultRotation"
        Me.ltbDefaultRotation.Size = New System.Drawing.Size(191, 25)
        Me.ltbDefaultRotation.TabIndex = 73
        '
        'lcbFileType
        '
        Me.lcbFileType.BackColor = System.Drawing.Color.Transparent
        Me.lcbFileType.Caption = "File Type:"
        Me.lcbFileType.CaptionLocation = FabV24_WIN8.LabelComboBox.CaptionLocations.clLeft
        Me.lcbFileType.FieldStyle = FabV24_WIN8.LabelComboBox.FieldStyles.fsAuto
        Me.lcbFileType.FieldWidth = 500
        Me.lcbFileType.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lcbFileType.ListIndex = -1
        Me.lcbFileType.Location = New System.Drawing.Point(341, 70)
        Me.lcbFileType.Name = "lcbFileType"
        Me.lcbFileType.Padding = New System.Windows.Forms.Padding(20, 0, 0, 0)
        Me.lcbFileType.Size = New System.Drawing.Size(207, 25)
        Me.lcbFileType.TabIndex = 73
        Me.lcbFileType.UseMnemonic = True
        '
        'picPreview1
        '
        Me.picPreview1.BackColor = System.Drawing.Color.Black
        Me.picPreview1.Location = New System.Drawing.Point(4, 25)
        Me.picPreview1.Name = "picPreview1"
        Me.picPreview1.Size = New System.Drawing.Size(339, 295)
        Me.picPreview1.TabIndex = 2
        Me.picPreview1.TabStop = False
        '
        'dlgPartBrowser
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.ClientSize = New System.Drawing.Size(1137, 477)
        Me.Controls.Add(Me.sccPartBrowser)
        Me.DoubleBuffered = True
        Me.Name = "dlgPartBrowser"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "dlgPartBrowser"
        CType(Me.sccPartBrowser, System.ComponentModel.ISupportInitialize).EndInit()
        Me.sccPartBrowser.ResumeLayout(False)
        CType(Me.GroupControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupControl1.ResumeLayout(False)
        CType(Me.cePreviewPart.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ubgParts, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ubgPartsView, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.SplitContainerControl2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainerControl2.ResumeLayout(False)
        CType(Me.picPreview1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents LayoutConverter1 As DevExpress.XtraLayout.Converter.LayoutConverter
    Friend WithEvents sccPartBrowser As DevExpress.XtraEditors.SplitContainerControl
    Friend WithEvents GroupControl1 As DevExpress.XtraEditors.GroupControl
    Friend WithEvents cePreviewPart As DevExpress.XtraEditors.CheckEdit
    Friend WithEvents ubgParts As FabV24_WIN8.CustomGrid.CustomGrid.UnboundGridControl
    Friend WithEvents ubgPartsView As FabV24_WIN8.CustomGrid.CustomGrid.UnboundGridView
    Friend WithEvents sbAddSelectedFiles As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbAddSelectedFolder As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents chkStripLayers As System.Windows.Forms.CheckBox
    Friend WithEvents SplitContainerControl2 As DevExpress.XtraEditors.SplitContainerControl
    Friend WithEvents fldrVw As LogicNP.FolderViewControl.FolderView
    Friend WithEvents fgList As LogicNP.FileViewControl.FileView
    Friend WithEvents sbRemovePart As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents cmdOk As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents cmdCancel As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents ltbDefaultQTY As FabV24_WIN8.lblTextBox
    Friend WithEvents lcbRepresentation As FabV24_WIN8.LabelComboBox
    Friend WithEvents ltbDefaultRotation As FabV24_WIN8.lblTextBox
    Friend WithEvents lcbFileType As FabV24_WIN8.LabelComboBox
    Friend WithEvents picPreview1 As System.Windows.Forms.PictureBox
End Class
