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
        Me.PanelControl1 = New DevExpress.XtraEditors.PanelControl()
        Me.txtFilename = New System.Windows.Forms.TextBox()
        Me.lblFilename = New System.Windows.Forms.Label()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.btnOK = New System.Windows.Forms.Button()
        Me.PanelControl2 = New DevExpress.XtraEditors.PanelControl()
        Me.SplitContainer2 = New System.Windows.Forms.SplitContainer()
        Me.tvFolders = New System.Windows.Forms.TreeView()
        Me.SplitContainer3 = New System.Windows.Forms.SplitContainer()
        Me.lstFiles = New System.Windows.Forms.ListBox()
        Me.pnlPreview = New System.Windows.Forms.Panel()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.LabelControl1 = New DevExpress.XtraEditors.LabelControl()
        Me.chkRaw = New System.Windows.Forms.CheckBox()
        Me.chkStripLayers = New System.Windows.Forms.CheckBox()
        Me.chkAutoSave = New System.Windows.Forms.CheckBox()
        Me.ltbCurveRes = New DevExpress.XtraEditors.LabelControl()
        Me.txtPreRotate = New System.Windows.Forms.TextBox()
        Me.tbCurveResolution = New System.Windows.Forms.TextBox()
        Me.ComboBox1 = New System.Windows.Forms.ComboBox()
        Me.Label1 = New System.Windows.Forms.Label()
        CType(Me.PanelControl1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.PanelControl1.SuspendLayout()
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
        Me.Panel1.SuspendLayout()
        Me.SuspendLayout()
        '
        'PanelControl1
        '
        Me.PanelControl1.Controls.Add(Me.Label1)
        Me.PanelControl1.Controls.Add(Me.ComboBox1)
        Me.PanelControl1.Controls.Add(Me.Panel1)
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
        Me.btnCancel.Location = New System.Drawing.Point(924, 6)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(75, 23)
        Me.btnCancel.TabIndex = 1
        Me.btnCancel.Text = "Cancel"
        Me.btnCancel.UseVisualStyleBackColor = True
        '
        'btnOK
        '
        Me.btnOK.Location = New System.Drawing.Point(843, 5)
        Me.btnOK.Name = "btnOK"
        Me.btnOK.Size = New System.Drawing.Size(75, 23)
        Me.btnOK.TabIndex = 0
        Me.btnOK.Text = "Ok"
        Me.btnOK.UseVisualStyleBackColor = True
        '
        'PanelControl2
        '
        Me.PanelControl2.Controls.Add(Me.SplitContainer2)
        Me.PanelControl2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.PanelControl2.Location = New System.Drawing.Point(0, 0)
        Me.PanelControl2.Name = "PanelControl2"
        Me.PanelControl2.Size = New System.Drawing.Size(1027, 401)
        Me.PanelControl2.TabIndex = 1
        '
        'SplitContainer2
        '
        Me.SplitContainer2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainer2.Location = New System.Drawing.Point(2, 2)
        Me.SplitContainer2.Name = "SplitContainer2"
        '
        'SplitContainer2.Panel1
        '
        Me.SplitContainer2.Panel1.Controls.Add(Me.tvFolders)
        '
        'SplitContainer2.Panel2
        '
        Me.SplitContainer2.Panel2.Controls.Add(Me.SplitContainer3)
        Me.SplitContainer2.Size = New System.Drawing.Size(1023, 397)
        Me.SplitContainer2.SplitterDistance = 341
        Me.SplitContainer2.TabIndex = 0
        '
        'tvFolders
        '
        Me.tvFolders.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tvFolders.Location = New System.Drawing.Point(0, 0)
        Me.tvFolders.Name = "tvFolders"
        Me.tvFolders.Size = New System.Drawing.Size(341, 397)
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
        Me.SplitContainer3.Panel1.Controls.Add(Me.lstFiles)
        '
        'SplitContainer3.Panel2
        '
        Me.SplitContainer3.Panel2.Controls.Add(Me.pnlPreview)
        Me.SplitContainer3.Size = New System.Drawing.Size(678, 397)
        Me.SplitContainer3.SplitterDistance = 317
        Me.SplitContainer3.TabIndex = 0
        '
        'lstFiles
        '
        Me.lstFiles.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lstFiles.FormattingEnabled = True
        Me.lstFiles.Location = New System.Drawing.Point(0, 0)
        Me.lstFiles.Name = "lstFiles"
        Me.lstFiles.Size = New System.Drawing.Size(317, 397)
        Me.lstFiles.TabIndex = 0
        '
        'pnlPreview
        '
        Me.pnlPreview.BackColor = System.Drawing.Color.Black
        Me.pnlPreview.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlPreview.Location = New System.Drawing.Point(0, 0)
        Me.pnlPreview.Name = "pnlPreview"
        Me.pnlPreview.Size = New System.Drawing.Size(357, 397)
        Me.pnlPreview.TabIndex = 0
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.tbCurveResolution)
        Me.Panel1.Controls.Add(Me.txtPreRotate)
        Me.Panel1.Controls.Add(Me.ltbCurveRes)
        Me.Panel1.Controls.Add(Me.chkRaw)
        Me.Panel1.Controls.Add(Me.chkStripLayers)
        Me.Panel1.Controls.Add(Me.chkAutoSave)
        Me.Panel1.Controls.Add(Me.LabelControl1)
        Me.Panel1.Location = New System.Drawing.Point(329, 15)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(335, 66)
        Me.Panel1.TabIndex = 5
        '
        'LabelControl1
        '
        Me.LabelControl1.Location = New System.Drawing.Point(3, 5)
        Me.LabelControl1.Name = "LabelControl1"
        Me.LabelControl1.Size = New System.Drawing.Size(86, 13)
        Me.LabelControl1.TabIndex = 15
        Me.LabelControl1.Text = "Pre Rotate Angle:"
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
        'ltbCurveRes
        '
        Me.ltbCurveRes.Location = New System.Drawing.Point(3, 34)
        Me.ltbCurveRes.Name = "ltbCurveRes"
        Me.ltbCurveRes.Size = New System.Drawing.Size(86, 13)
        Me.ltbCurveRes.TabIndex = 19
        Me.ltbCurveRes.Text = "Curve Resolution:"
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
        'tbCurveResolution
        '
        Me.tbCurveResolution.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.tbCurveResolution.Location = New System.Drawing.Point(95, 29)
        Me.tbCurveResolution.Name = "tbCurveResolution"
        Me.tbCurveResolution.Size = New System.Drawing.Size(52, 21)
        Me.tbCurveResolution.TabIndex = 21
        Me.tbCurveResolution.WordWrap = False
        '
        'ComboBox1
        '
        Me.ComboBox1.FormattingEnabled = True
        Me.ComboBox1.Location = New System.Drawing.Point(71, 43)
        Me.ComboBox1.Name = "ComboBox1"
        Me.ComboBox1.Size = New System.Drawing.Size(252, 21)
        Me.ComboBox1.TabIndex = 6
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
        'DXFFileSelectionDialog
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1027, 492)
        Me.Controls.Add(Me.PanelControl2)
        Me.Controls.Add(Me.PanelControl1)
        Me.Name = "DXFFileSelectionDialog"
        Me.Text = "DXFFileSelectionDialog"
        CType(Me.PanelControl1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.PanelControl1.ResumeLayout(False)
        Me.PanelControl1.PerformLayout()
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
        Me.Panel1.ResumeLayout(False)
        Me.Panel1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents PanelControl1 As PanelControl
    Friend WithEvents PanelControl2 As PanelControl
    Friend WithEvents SplitContainer2 As SplitContainer
    Friend WithEvents tvFolders As TreeView
    Friend WithEvents SplitContainer3 As SplitContainer
    Friend WithEvents lstFiles As ListBox
    Friend WithEvents btnCancel As Button
    Friend WithEvents btnOK As Button
    Friend WithEvents pnlPreview As Panel
    Friend WithEvents txtFilename As TextBox
    Friend WithEvents lblFilename As Label
    Friend WithEvents Label1 As Label
    Friend WithEvents ComboBox1 As Windows.Forms.ComboBox
    Friend WithEvents Panel1 As Panel
    Friend WithEvents tbCurveResolution As TextBox
    Friend WithEvents txtPreRotate As TextBox
    Friend WithEvents ltbCurveRes As LabelControl
    Friend WithEvents chkRaw As CheckBox
    Friend WithEvents chkStripLayers As CheckBox
    Friend WithEvents chkAutoSave As CheckBox
    Friend WithEvents LabelControl1 As LabelControl
End Class
