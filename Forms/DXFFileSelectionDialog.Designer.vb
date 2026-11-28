<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class DXFFileSelectionDialog
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
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
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(DXFFileSelectionDialog))
        Me.PanelControl1 = New DevExpress.XtraEditors.PanelControl()
        Me.lblPartExtents = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cboFileTypes = New System.Windows.Forms.ComboBox()
        Me.pnlDxfOptions = New System.Windows.Forms.Panel()
        Me.txtPreRotate = New System.Windows.Forms.TextBox()
        Me.chkRaw = New System.Windows.Forms.CheckBox()
        Me.chkStripLayers = New System.Windows.Forms.CheckBox()
        Me.chkAutoSave = New System.Windows.Forms.CheckBox()
        Me.LabelControl1 = New DevExpress.XtraEditors.LabelControl()
        Me.txtFilename = New System.Windows.Forms.TextBox()
        Me.lblFilename = New System.Windows.Forms.Label()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.btnOK = New System.Windows.Forms.Button()
        Me.PanelControl2 = New DevExpress.XtraEditors.PanelControl()
        Me.SplitContainer2 = New System.Windows.Forms.SplitContainer()
        Me.tvFolders = New System.Windows.Forms.TreeView()
        Me.SplitContainer3 = New System.Windows.Forms.SplitContainer()
        Me.lvFiles = New System.Windows.Forms.ListView()
        Me.colName = CType(New System.Windows.Forms.ColumnHeader(), System.Windows.Forms.ColumnHeader)
        Me.colSize = CType(New System.Windows.Forms.ColumnHeader(), System.Windows.Forms.ColumnHeader)
        Me.colType = CType(New System.Windows.Forms.ColumnHeader(), System.Windows.Forms.ColumnHeader)
        Me.colDateModified = CType(New System.Windows.Forms.ColumnHeader(), System.Windows.Forms.ColumnHeader)
        Me.pnlPreview = New System.Windows.Forms.Panel()
        Me.pnlAddress = New System.Windows.Forms.Panel()
        Me.txtAddress = New System.Windows.Forms.TextBox()
        Me.lblAddress = New System.Windows.Forms.Label()
        Me.Panel2 = New System.Windows.Forms.Panel()
        CType(Me.PanelControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.PanelControl1.SuspendLayout()
        Me.pnlDxfOptions.SuspendLayout()
        CType(Me.PanelControl2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.PanelControl2.SuspendLayout()
        CType(Me.SplitContainer2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainer2.Panel1.SuspendLayout()
        Me.SplitContainer2.Panel2.SuspendLayout()
        Me.SplitContainer2.SuspendLayout()
        CType(Me.SplitContainer3, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainer3.Panel1.SuspendLayout()
        Me.SplitContainer3.Panel2.SuspendLayout()
        Me.SplitContainer3.SuspendLayout()
        Me.pnlAddress.SuspendLayout()
        Me.SuspendLayout()
        '
        'PanelControl1
        '
        Me.PanelControl1.Controls.Add(Me.lblPartExtents)
        Me.PanelControl1.Controls.Add(Me.Label1)
        Me.PanelControl1.Controls.Add(Me.cboFileTypes)
        Me.PanelControl1.Controls.Add(Me.pnlDxfOptions)
        Me.PanelControl1.Controls.Add(Me.txtFilename)
        Me.PanelControl1.Controls.Add(Me.lblFilename)
        Me.PanelControl1.Controls.Add(Me.btnCancel)
        Me.PanelControl1.Controls.Add(Me.btnOK)
        Me.PanelControl1.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.PanelControl1.Location = New System.Drawing.Point(0, 401)
        Me.PanelControl1.Name = "PanelControl1"
        Me.PanelControl1.Size = New System.Drawing.Size(1027, 91)
        Me.PanelControl1.TabIndex = 0
        '
        'lblPartExtents
        '
        Me.lblPartExtents.AutoSize = True
        Me.lblPartExtents.Location = New System.Drawing.Point(685, 12)
        Me.lblPartExtents.Name = "lblPartExtents"
        Me.lblPartExtents.Size = New System.Drawing.Size(82, 13)
        Me.lblPartExtents.TabIndex = 8
        Me.lblPartExtents.Text = "Model Extents: "
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(6, 46)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(59, 13)
        Me.Label1.TabIndex = 7
        Me.Label1.Text = "File Types:"
        '
        'cboFileTypes
        '
        Me.cboFileTypes.FormattingEnabled = True
        Me.cboFileTypes.Location = New System.Drawing.Point(71, 43)
        Me.cboFileTypes.Name = "cboFileTypes"
        Me.cboFileTypes.Size = New System.Drawing.Size(252, 21)
        Me.cboFileTypes.TabIndex = 6
        '
        'pnlDxfOptions
        '
        Me.pnlDxfOptions.Controls.Add(Me.txtPreRotate)
        Me.pnlDxfOptions.Controls.Add(Me.chkRaw)
        Me.pnlDxfOptions.Controls.Add(Me.chkStripLayers)
        Me.pnlDxfOptions.Controls.Add(Me.chkAutoSave)
        Me.pnlDxfOptions.Controls.Add(Me.LabelControl1)
        Me.pnlDxfOptions.Location = New System.Drawing.Point(329, 15)
        Me.pnlDxfOptions.Name = "pnlDxfOptions"
        Me.pnlDxfOptions.Size = New System.Drawing.Size(335, 66)
        Me.pnlDxfOptions.TabIndex = 5
        '
        'txtPreRotate
        '
        Me.txtPreRotate.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.txtPreRotate.Location = New System.Drawing.Point(95, 3)
        Me.txtPreRotate.Name = "txtPreRotate"
        Me.txtPreRotate.Size = New System.Drawing.Size(52, 21)
        Me.txtPreRotate.TabIndex = 20
        Me.txtPreRotate.WordWrap = False
        '
        'chkRaw
        '
        Me.chkRaw.AutoSize = True
        Me.chkRaw.Location = New System.Drawing.Point(283, 31)
        Me.chkRaw.Name = "chkRaw"
        Me.chkRaw.Size = New System.Drawing.Size(47, 17)
        Me.chkRaw.TabIndex = 18
        Me.chkRaw.Text = "Raw"
        Me.chkRaw.UseVisualStyleBackColor = True
        '
        'chkStripLayers
        '
        Me.chkStripLayers.AutoSize = True
        Me.chkStripLayers.Location = New System.Drawing.Point(166, 31)
        Me.chkStripLayers.Name = "chkStripLayers"
        Me.chkStripLayers.Size = New System.Drawing.Size(83, 17)
        Me.chkStripLayers.TabIndex = 17
        Me.chkStripLayers.Text = "Strip Layers"
        Me.chkStripLayers.UseVisualStyleBackColor = True
        '
        'chkAutoSave
        '
        Me.chkAutoSave.AutoSize = True
        Me.chkAutoSave.Location = New System.Drawing.Point(166, 6)
        Me.chkAutoSave.Name = "chkAutoSave"
        Me.chkAutoSave.Size = New System.Drawing.Size(164, 17)
        Me.chkAutoSave.TabIndex = 16
        Me.chkAutoSave.Text = "Auto Save ( in source folder)"
        Me.chkAutoSave.UseVisualStyleBackColor = True
        '
        'LabelControl1
        '
        Me.LabelControl1.Location = New System.Drawing.Point(3, 5)
        Me.LabelControl1.Name = "LabelControl1"
        Me.LabelControl1.Size = New System.Drawing.Size(86, 13)
        Me.LabelControl1.TabIndex = 15
        Me.LabelControl1.Text = "Pre Rotate Angle:"
        '
        'txtFilename
        '
        Me.txtFilename.Location = New System.Drawing.Point(71, 12)
        Me.txtFilename.Name = "txtFilename"
        Me.txtFilename.Size = New System.Drawing.Size(252, 21)
        Me.txtFilename.TabIndex = 4
        '
        'lblFilename
        '
        Me.lblFilename.AutoSize = True
        Me.lblFilename.Location = New System.Drawing.Point(12, 15)
        Me.lblFilename.Name = "lblFilename"
        Me.lblFilename.Size = New System.Drawing.Size(53, 13)
        Me.lblFilename.TabIndex = 3
        Me.lblFilename.Text = "Filename:"
        '
        'btnCancel
        '
        Me.btnCancel.Location = New System.Drawing.Point(921, 36)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(75, 23)
        Me.btnCancel.TabIndex = 1
        Me.btnCancel.Text = "Cancel"
        Me.btnCancel.UseVisualStyleBackColor = True
        '
        'btnOK
        '
        Me.btnOK.Location = New System.Drawing.Point(840, 35)
        Me.btnOK.Name = "btnOK"
        Me.btnOK.Size = New System.Drawing.Size(75, 23)
        Me.btnOK.TabIndex = 0
        Me.btnOK.Text = "Ok"
        Me.btnOK.UseVisualStyleBackColor = True
        '
        'PanelControl2
        '
        Me.PanelControl2.Controls.Add(Me.SplitContainer2)
        Me.PanelControl2.Controls.Add(Me.pnlAddress)
        Me.PanelControl2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.PanelControl2.Location = New System.Drawing.Point(0, 0)
        Me.PanelControl2.Name = "PanelControl2"
        Me.PanelControl2.Size = New System.Drawing.Size(1027, 401)
        Me.PanelControl2.TabIndex = 1
        '
        'SplitContainer2
        '
        Me.SplitContainer2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainer2.Location = New System.Drawing.Point(2, 38)
        Me.SplitContainer2.Name = "SplitContainer2"
        '
        'SplitContainer2.Panel1
        '
        Me.SplitContainer2.Panel1.Controls.Add(Me.tvFolders)
        '
        'SplitContainer2.Panel2
        '
        Me.SplitContainer2.Panel2.Controls.Add(Me.SplitContainer3)
        Me.SplitContainer2.Size = New System.Drawing.Size(1023, 361)
        Me.SplitContainer2.SplitterDistance = 257
        Me.SplitContainer2.TabIndex = 0
        '
        'tvFolders
        '
        Me.tvFolders.Location = New System.Drawing.Point(0, 0)
        Me.tvFolders.Name = "tvFolders"
        Me.tvFolders.Size = New System.Drawing.Size(250, 361)
        Me.tvFolders.TabIndex = 0
        '
        'SplitContainer3
        '
        Me.SplitContainer3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainer3.Location = New System.Drawing.Point(0, 0)
        Me.SplitContainer3.Name = "SplitContainer3"
        '
        'SplitContainer3.Panel1
        '
        Me.SplitContainer3.Panel1.Controls.Add(Me.lvFiles)
        '
        'SplitContainer3.Panel2
        '
        Me.SplitContainer3.Panel2.Controls.Add(Me.pnlPreview)
        Me.SplitContainer3.Size = New System.Drawing.Size(762, 361)
        Me.SplitContainer3.SplitterDistance = 405
        Me.SplitContainer3.TabIndex = 0
        '
        'lvFiles
        '
        Me.lvFiles.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.colName, Me.colSize, Me.colType, Me.colDateModified})
        Me.lvFiles.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lvFiles.FullRowSelect = True
        Me.lvFiles.HideSelection = False
        Me.lvFiles.Location = New System.Drawing.Point(0, 0)
        Me.lvFiles.MultiSelect = False
        Me.lvFiles.Name = "lvFiles"
        Me.lvFiles.Size = New System.Drawing.Size(405, 361)
        Me.lvFiles.TabIndex = 1
        Me.lvFiles.UseCompatibleStateImageBehavior = False
        Me.lvFiles.View = System.Windows.Forms.View.Details
        '
        'colName
        '
        Me.colName.Text = "Name"
        Me.colName.Width = 145
        '
        'colSize
        '
        Me.colSize.Text = "Size"
        Me.colSize.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.colSize.Width = 65
        '
        'colType
        '
        Me.colType.Text = "Type"
        Me.colType.Width = 85
        '
        'colDateModified
        '
        Me.colDateModified.Text = "Date Modified"
        Me.colDateModified.Width = 135
        '
        'pnlPreview
        '
        Me.pnlPreview.BackColor = System.Drawing.Color.Black
        Me.pnlPreview.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlPreview.Location = New System.Drawing.Point(0, 0)
        Me.pnlPreview.Name = "pnlPreview"
        Me.pnlPreview.Size = New System.Drawing.Size(353, 361)
        Me.pnlPreview.TabIndex = 0
        '
        'pnlAddress
        '
        Me.pnlAddress.Controls.Add(Me.txtAddress)
        Me.pnlAddress.Controls.Add(Me.lblAddress)
        Me.pnlAddress.Controls.Add(Me.Panel2)
        Me.pnlAddress.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnlAddress.Location = New System.Drawing.Point(2, 2)
        Me.pnlAddress.Name = "pnlAddress"
        Me.pnlAddress.Size = New System.Drawing.Size(1023, 36)
        Me.pnlAddress.TabIndex = 1
        '
        'txtAddress
        '
        Me.txtAddress.Location = New System.Drawing.Point(60, 3)
        Me.txtAddress.Name = "txtAddress"
        Me.txtAddress.Size = New System.Drawing.Size(953, 21)
        Me.txtAddress.TabIndex = 2
        '
        'lblAddress
        '
        Me.lblAddress.AutoSize = True
        Me.lblAddress.Location = New System.Drawing.Point(4, 7)
        Me.lblAddress.Name = "lblAddress"
        Me.lblAddress.Size = New System.Drawing.Size(50, 13)
        Me.lblAddress.TabIndex = 1
        Me.lblAddress.Text = "Address:"
        '
        'Panel2
        '
        Me.Panel2.Location = New System.Drawing.Point(67, 41)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(8, 8)
        Me.Panel2.TabIndex = 0
        '
        'DXFFileSelectionDialog
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1027, 492)
        Me.Controls.Add(Me.PanelControl2)
        Me.Controls.Add(Me.PanelControl1)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Name = "DXFFileSelectionDialog"
        Me.Text = "Open Model"
        CType(Me.PanelControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.PanelControl1.ResumeLayout(False)
        Me.PanelControl1.PerformLayout()
        Me.pnlDxfOptions.ResumeLayout(False)
        Me.pnlDxfOptions.PerformLayout()
        CType(Me.PanelControl2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.PanelControl2.ResumeLayout(False)
        Me.SplitContainer2.Panel1.ResumeLayout(False)
        Me.SplitContainer2.Panel2.ResumeLayout(False)
        CType(Me.SplitContainer2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer2.ResumeLayout(False)
        Me.SplitContainer3.Panel1.ResumeLayout(False)
        Me.SplitContainer3.Panel2.ResumeLayout(False)
        CType(Me.SplitContainer3, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer3.ResumeLayout(False)
        Me.pnlAddress.ResumeLayout(False)
        Me.pnlAddress.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents PanelControl1 As PanelControl
    Friend WithEvents PanelControl2 As PanelControl
    Friend WithEvents SplitContainer2 As SplitContainer
    Friend WithEvents tvFolders As TreeView
    Friend WithEvents SplitContainer3 As SplitContainer
    Friend WithEvents btnCancel As Button
    Friend WithEvents btnOK As Button
    Friend WithEvents pnlPreview As Panel
    Friend WithEvents txtFilename As TextBox
    Friend WithEvents lblFilename As Label
    Friend WithEvents Label1 As Label
    Friend WithEvents cboFileTypes As Windows.Forms.ComboBox
    Friend WithEvents pnlDxfOptions As Panel
    Friend WithEvents txtPreRotate As TextBox
    Friend WithEvents chkRaw As CheckBox
    Friend WithEvents chkStripLayers As CheckBox
    Friend WithEvents chkAutoSave As CheckBox
    Friend WithEvents LabelControl1 As LabelControl
    Friend WithEvents pnlAddress As Panel
    Friend WithEvents Panel2 As Panel
    Friend WithEvents lblPartExtents As Label
    Friend WithEvents txtAddress As TextBox
    Friend WithEvents lblAddress As Label
    Friend WithEvents lvFiles As ListView
    Friend WithEvents colName As ColumnHeader
    Friend WithEvents colSize As ColumnHeader
    Friend WithEvents colType As ColumnHeader
    Friend WithEvents colDateModified As ColumnHeader
End Class
