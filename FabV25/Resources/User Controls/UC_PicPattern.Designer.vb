<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class UC_PicPattern
    Inherits System.Windows.Forms.UserControl

    'UserControl overrides dispose to clean up the component list.
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
        Me.pnlPicPattern = New System.Windows.Forms.Panel()
        Me.beSheets = New DevExpress.XtraEditors.ButtonEdit()
        Me.lcSheets = New DevExpress.XtraEditors.LabelControl()
        Me.sbDelete = New DevExpress.XtraEditors.SimpleButton()
        Me.sbCancel = New DevExpress.XtraEditors.SimpleButton()
        Me.sbDrop = New DevExpress.XtraEditors.SimpleButton()
        Me.OptMoveCopy = New DevExpress.XtraEditors.RadioGroup()
        Me.teOrientation = New DevExpress.XtraEditors.TextEdit()
        Me.lblOreintation = New DevExpress.XtraEditors.LabelControl()
        Me.txtHandleY = New DevExpress.XtraEditors.TextEdit()
        Me.lblHandleY = New DevExpress.XtraEditors.LabelControl()
        Me.tbHandleX = New DevExpress.XtraEditors.TextEdit()
        Me.lblHandleX = New DevExpress.XtraEditors.LabelControl()
        CType(Me.beSheets.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.OptMoveCopy.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.teOrientation.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.txtHandleY.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.tbHandleX.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'pnlPicPattern
        '
        Me.pnlPicPattern.BackColor = System.Drawing.Color.Black
        Me.pnlPicPattern.Dock = System.Windows.Forms.DockStyle.Left
        Me.pnlPicPattern.Location = New System.Drawing.Point(0, 0)
        Me.pnlPicPattern.Name = "pnlPicPattern"
        Me.pnlPicPattern.Size = New System.Drawing.Size(60, 60)
        Me.pnlPicPattern.TabIndex = 0
        '
        'beSheets
        '
        Me.beSheets.Location = New System.Drawing.Point(128, 3)
        Me.beSheets.Name = "beSheets"
        Me.beSheets.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.beSheets.Size = New System.Drawing.Size(392, 20)
        Me.beSheets.TabIndex = 51
        '
        'lcSheets
        '
        Me.lcSheets.Location = New System.Drawing.Point(89, 6)
        Me.lcSheets.Name = "lcSheets"
        Me.lcSheets.Size = New System.Drawing.Size(33, 13)
        Me.lcSheets.TabIndex = 50
        Me.lcSheets.Text = "Sheets"
        '
        'sbDelete
        '
        Me.sbDelete.Location = New System.Drawing.Point(697, 36)
        Me.sbDelete.Name = "sbDelete"
        Me.sbDelete.Size = New System.Drawing.Size(75, 20)
        Me.sbDelete.TabIndex = 73
        Me.sbDelete.Text = "Delete"
        '
        'sbCancel
        '
        Me.sbCancel.Location = New System.Drawing.Point(616, 36)
        Me.sbCancel.Name = "sbCancel"
        Me.sbCancel.Size = New System.Drawing.Size(75, 20)
        Me.sbCancel.TabIndex = 72
        Me.sbCancel.Text = "Cancel"
        '
        'sbDrop
        '
        Me.sbDrop.Location = New System.Drawing.Point(531, 36)
        Me.sbDrop.Name = "sbDrop"
        Me.sbDrop.Size = New System.Drawing.Size(75, 20)
        Me.sbDrop.TabIndex = 71
        Me.sbDrop.Text = "Drop"
        '
        'OptMoveCopy
        '
        Me.OptMoveCopy.Location = New System.Drawing.Point(531, 3)
        Me.OptMoveCopy.Name = "OptMoveCopy"
        Me.OptMoveCopy.Properties.Columns = 2
        Me.OptMoveCopy.Properties.Items.AddRange(New DevExpress.XtraEditors.Controls.RadioGroupItem() {New DevExpress.XtraEditors.Controls.RadioGroupItem(True, "Move"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "Copy")})
        Me.OptMoveCopy.Size = New System.Drawing.Size(165, 25)
        Me.OptMoveCopy.TabIndex = 70
        '
        'teOrientation
        '
        Me.teOrientation.Location = New System.Drawing.Point(424, 36)
        Me.teOrientation.Name = "teOrientation"
        Me.teOrientation.Size = New System.Drawing.Size(100, 20)
        Me.teOrientation.TabIndex = 69
        '
        'lblOreintation
        '
        Me.lblOreintation.Location = New System.Drawing.Point(364, 39)
        Me.lblOreintation.Name = "lblOreintation"
        Me.lblOreintation.Size = New System.Drawing.Size(54, 13)
        Me.lblOreintation.TabIndex = 68
        Me.lblOreintation.Text = "Orientation"
        '
        'txtHandleY
        '
        Me.txtHandleY.Location = New System.Drawing.Point(254, 36)
        Me.txtHandleY.Name = "txtHandleY"
        Me.txtHandleY.Size = New System.Drawing.Size(100, 20)
        Me.txtHandleY.TabIndex = 67
        '
        'lblHandleY
        '
        Me.lblHandleY.Location = New System.Drawing.Point(238, 39)
        Me.lblHandleY.Name = "lblHandleY"
        Me.lblHandleY.Size = New System.Drawing.Size(10, 13)
        Me.lblHandleY.TabIndex = 66
        Me.lblHandleY.Text = "Y:"
        '
        'tbHandleX
        '
        Me.tbHandleX.Location = New System.Drawing.Point(132, 37)
        Me.tbHandleX.Name = "tbHandleX"
        Me.tbHandleX.Size = New System.Drawing.Size(100, 20)
        Me.tbHandleX.TabIndex = 65
        '
        'lblHandleX
        '
        Me.lblHandleX.Location = New System.Drawing.Point(80, 39)
        Me.lblHandleX.Name = "lblHandleX"
        Me.lblHandleX.Size = New System.Drawing.Size(46, 13)
        Me.lblHandleX.TabIndex = 64
        Me.lblHandleX.Text = "Handle X:"
        '
        'UC_PicPattern
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.sbDelete)
        Me.Controls.Add(Me.sbCancel)
        Me.Controls.Add(Me.sbDrop)
        Me.Controls.Add(Me.OptMoveCopy)
        Me.Controls.Add(Me.teOrientation)
        Me.Controls.Add(Me.lblOreintation)
        Me.Controls.Add(Me.txtHandleY)
        Me.Controls.Add(Me.lblHandleY)
        Me.Controls.Add(Me.tbHandleX)
        Me.Controls.Add(Me.lblHandleX)
        Me.Controls.Add(Me.beSheets)
        Me.Controls.Add(Me.lcSheets)
        Me.Controls.Add(Me.pnlPicPattern)
        Me.Name = "UC_PicPattern"
        Me.Size = New System.Drawing.Size(777, 60)
        CType(Me.beSheets.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.OptMoveCopy.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.teOrientation.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.txtHandleY.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.tbHandleX.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents pnlPicPattern As System.Windows.Forms.Panel
    Friend WithEvents beSheets As DevExpress.XtraEditors.ButtonEdit
    Friend WithEvents lcSheets As DevExpress.XtraEditors.LabelControl
    Friend WithEvents sbDelete As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbCancel As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents sbDrop As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents OptMoveCopy As DevExpress.XtraEditors.RadioGroup
    Friend WithEvents teOrientation As DevExpress.XtraEditors.TextEdit
    Friend WithEvents lblOreintation As DevExpress.XtraEditors.LabelControl
    Friend WithEvents txtHandleY As DevExpress.XtraEditors.TextEdit
    Friend WithEvents lblHandleY As DevExpress.XtraEditors.LabelControl
    Friend WithEvents tbHandleX As DevExpress.XtraEditors.TextEdit
    Friend WithEvents lblHandleX As DevExpress.XtraEditors.LabelControl

End Class
