<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class XtraForm4
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(XtraForm4))
        Me.SpreadsheetControl1 = New DevExpress.XtraSpreadsheet.SpreadsheetControl()
        Me.lbcMatTypes = New DevExpress.XtraEditors.ListBoxControl()
        Me.SimpleButton1 = New DevExpress.XtraEditors.SimpleButton()
        Me.lbToolList = New DevExpress.XtraEditors.ListBoxControl()
        Me.SimpleButton2 = New DevExpress.XtraEditors.SimpleButton()
        Me.sbToolSetups = New DevExpress.XtraEditors.SimpleButton()
        Me.SimpleButton3 = New DevExpress.XtraEditors.SimpleButton()
        Me.PictureEdit1 = New DevExpress.XtraEditors.PictureEdit()
        CType(Me.lbcMatTypes, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.lbToolList, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.PictureEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'SpreadsheetControl1
        '
        Me.SpreadsheetControl1.Location = New System.Drawing.Point(13, 8)
        Me.SpreadsheetControl1.Name = "SpreadsheetControl1"
        Me.SpreadsheetControl1.Options.Import.Csv.Encoding = CType(resources.GetObject("SpreadsheetControl1.Options.Import.Csv.Encoding"), System.Text.Encoding)
        Me.SpreadsheetControl1.Options.Import.Txt.Encoding = CType(resources.GetObject("SpreadsheetControl1.Options.Import.Txt.Encoding"), System.Text.Encoding)
        Me.SpreadsheetControl1.Size = New System.Drawing.Size(539, 324)
        Me.SpreadsheetControl1.TabIndex = 0
        Me.SpreadsheetControl1.Text = "SpreadsheetControl1"
        '
        'lbcMatTypes
        '
        Me.lbcMatTypes.Location = New System.Drawing.Point(558, 12)
        Me.lbcMatTypes.Name = "lbcMatTypes"
        Me.lbcMatTypes.Size = New System.Drawing.Size(120, 320)
        Me.lbcMatTypes.TabIndex = 1
        '
        'SimpleButton1
        '
        Me.SimpleButton1.AutoSize = True
        Me.SimpleButton1.Location = New System.Drawing.Point(706, 294)
        Me.SimpleButton1.Name = "SimpleButton1"
        Me.SimpleButton1.Size = New System.Drawing.Size(139, 22)
        Me.SimpleButton1.TabIndex = 2
        Me.SimpleButton1.Text = "Create material toolsetups "
        '
        'lbToolList
        '
        Me.lbToolList.Appearance.BackColor = System.Drawing.Color.Black
        Me.lbToolList.Appearance.Options.UseBackColor = True
        Me.lbToolList.Location = New System.Drawing.Point(696, 12)
        Me.lbToolList.Name = "lbToolList"
        Me.lbToolList.Size = New System.Drawing.Size(110, 276)
        Me.lbToolList.TabIndex = 3
        '
        'SimpleButton2
        '
        Me.SimpleButton2.AutoSize = True
        Me.SimpleButton2.Location = New System.Drawing.Point(706, 323)
        Me.SimpleButton2.Name = "SimpleButton2"
        Me.SimpleButton2.Size = New System.Drawing.Size(108, 22)
        Me.SimpleButton2.TabIndex = 4
        Me.SimpleButton2.Text = "Create Layer Setups"
        '
        'sbToolSetups
        '
        Me.sbToolSetups.AutoSize = True
        Me.sbToolSetups.Location = New System.Drawing.Point(848, 12)
        Me.sbToolSetups.Name = "sbToolSetups"
        Me.sbToolSetups.Size = New System.Drawing.Size(139, 22)
        Me.sbToolSetups.TabIndex = 5
        Me.sbToolSetups.Text = "Create material toolsetups "
        '
        'SimpleButton3
        '
        Me.SimpleButton3.Location = New System.Drawing.Point(874, 118)
        Me.SimpleButton3.Name = "SimpleButton3"
        Me.SimpleButton3.Size = New System.Drawing.Size(75, 23)
        Me.SimpleButton3.TabIndex = 6
        Me.SimpleButton3.Text = "XLS to MDB"
        '
        'PictureEdit1
        '
        Me.PictureEdit1.Location = New System.Drawing.Point(847, 194)
        Me.PictureEdit1.Name = "PictureEdit1"
        Me.PictureEdit1.Properties.ShowCameraMenuItem = DevExpress.XtraEditors.Controls.CameraMenuItemVisibility.[Auto]
        Me.PictureEdit1.Size = New System.Drawing.Size(65, 69)
        Me.PictureEdit1.TabIndex = 7
        '
        'XtraForm4
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1066, 381)
        Me.Controls.Add(Me.PictureEdit1)
        Me.Controls.Add(Me.SimpleButton3)
        Me.Controls.Add(Me.sbToolSetups)
        Me.Controls.Add(Me.SimpleButton2)
        Me.Controls.Add(Me.lbToolList)
        Me.Controls.Add(Me.SimpleButton1)
        Me.Controls.Add(Me.lbcMatTypes)
        Me.Controls.Add(Me.SpreadsheetControl1)
        Me.Name = "XtraForm4"
        Me.Text = "XtraForm4"
        CType(Me.lbcMatTypes, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.lbToolList, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.PictureEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents SpreadsheetControl1 As DevExpress.XtraSpreadsheet.SpreadsheetControl
    Friend WithEvents lbcMatTypes As ListBoxControl
    Friend WithEvents SimpleButton1 As SimpleButton
    Friend WithEvents lbToolList As ListBoxControl
    Friend WithEvents SimpleButton2 As SimpleButton
    Friend WithEvents sbToolSetups As SimpleButton
    Friend WithEvents SimpleButton3 As SimpleButton
    Friend WithEvents PictureEdit1 As PictureEdit
End Class
