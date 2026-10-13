<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmConvertCMDB
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmConvertCMDB))
        Me.lblCMDBFile = New System.Windows.Forms.Label()
        Me.txtCMDBFile = New System.Windows.Forms.TextBox()
        Me.picCMDBFile = New System.Windows.Forms.PictureBox()
        Me.lblDataFolder = New System.Windows.Forms.Label()
        Me.txtDataFolder = New System.Windows.Forms.TextBox()
        Me.picJsonFolder = New System.Windows.Forms.PictureBox()
        Me.btnConvertCMDB = New System.Windows.Forms.Button()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.fbdJsonDataFolder = New System.Windows.Forms.FolderBrowserDialog()
        Me.ofdCMDBFile = New System.Windows.Forms.OpenFileDialog()
        Me.Button1 = New System.Windows.Forms.Button()
        CType(Me.picCMDBFile, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.picJsonFolder, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'lblCMDBFile
        '
        Me.lblCMDBFile.AutoSize = True
        Me.lblCMDBFile.Location = New System.Drawing.Point(57, 31)
        Me.lblCMDBFile.Name = "lblCMDBFile"
        Me.lblCMDBFile.Size = New System.Drawing.Size(60, 13)
        Me.lblCMDBFile.TabIndex = 0
        Me.lblCMDBFile.Text = "CMDB File:"
        '
        'txtCMDBFile
        '
        Me.txtCMDBFile.Location = New System.Drawing.Point(123, 29)
        Me.txtCMDBFile.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.txtCMDBFile.Name = "txtCMDBFile"
        Me.txtCMDBFile.Size = New System.Drawing.Size(418, 20)
        Me.txtCMDBFile.TabIndex = 1
        '
        'picCMDBFile
        '
        Me.picCMDBFile.Image = CType(resources.GetObject("picCMDBFile.Image"), System.Drawing.Image)
        Me.picCMDBFile.Location = New System.Drawing.Point(545, 36)
        Me.picCMDBFile.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.picCMDBFile.Name = "picCMDBFile"
        Me.picCMDBFile.Size = New System.Drawing.Size(16, 16)
        Me.picCMDBFile.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize
        Me.picCMDBFile.TabIndex = 2
        Me.picCMDBFile.TabStop = False
        '
        'lblDataFolder
        '
        Me.lblDataFolder.AutoSize = True
        Me.lblDataFolder.Location = New System.Drawing.Point(34, 57)
        Me.lblDataFolder.Name = "lblDataFolder"
        Me.lblDataFolder.Size = New System.Drawing.Size(83, 13)
        Me.lblDataFolder.TabIndex = 3
        Me.lblDataFolder.Text = "Save To Folder:"
        '
        'txtDataFolder
        '
        Me.txtDataFolder.Location = New System.Drawing.Point(123, 54)
        Me.txtDataFolder.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.txtDataFolder.Name = "txtDataFolder"
        Me.txtDataFolder.Size = New System.Drawing.Size(418, 20)
        Me.txtDataFolder.TabIndex = 4
        '
        'picJsonFolder
        '
        Me.picJsonFolder.Image = CType(resources.GetObject("picJsonFolder.Image"), System.Drawing.Image)
        Me.picJsonFolder.Location = New System.Drawing.Point(545, 57)
        Me.picJsonFolder.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.picJsonFolder.Name = "picJsonFolder"
        Me.picJsonFolder.Size = New System.Drawing.Size(16, 16)
        Me.picJsonFolder.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize
        Me.picJsonFolder.TabIndex = 5
        Me.picJsonFolder.TabStop = False
        '
        'btnConvertCMDB
        '
        Me.btnConvertCMDB.AutoSize = True
        Me.btnConvertCMDB.Location = New System.Drawing.Point(417, 93)
        Me.btnConvertCMDB.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnConvertCMDB.Name = "btnConvertCMDB"
        Me.btnConvertCMDB.Size = New System.Drawing.Size(64, 23)
        Me.btnConvertCMDB.TabIndex = 6
        Me.btnConvertCMDB.Text = "Convert"
        Me.btnConvertCMDB.UseVisualStyleBackColor = True
        '
        'btnCancel
        '
        Me.btnCancel.AutoSize = True
        Me.btnCancel.Location = New System.Drawing.Point(495, 93)
        Me.btnCancel.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(64, 23)
        Me.btnCancel.TabIndex = 7
        Me.btnCancel.Text = "Cancel"
        Me.btnCancel.UseVisualStyleBackColor = True
        '
        'ofdCMDBFile
        '
        Me.ofdCMDBFile.DefaultExt = "cmdb"
        Me.ofdCMDBFile.FileName = "OpenFileDialog1"
        Me.ofdCMDBFile.Filter = """CMDB Files|*.mdb"""
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(28, 93)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(75, 23)
        Me.Button1.TabIndex = 8
        Me.Button1.Text = "Button1"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'frmConvertCMDB
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(572, 119)
        Me.Controls.Add(Me.Button1)
        Me.Controls.Add(Me.btnCancel)
        Me.Controls.Add(Me.btnConvertCMDB)
        Me.Controls.Add(Me.picJsonFolder)
        Me.Controls.Add(Me.txtDataFolder)
        Me.Controls.Add(Me.lblDataFolder)
        Me.Controls.Add(Me.picCMDBFile)
        Me.Controls.Add(Me.txtCMDBFile)
        Me.Controls.Add(Me.lblCMDBFile)
        Me.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.Name = "frmConvertCMDB"
        Me.Text = "frmConvertCMDB"
        CType(Me.picCMDBFile, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.picJsonFolder, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents lblCMDBFile As Label
    Friend WithEvents txtCMDBFile As TextBox
    Friend WithEvents picCMDBFile As PictureBox
    Friend WithEvents lblDataFolder As Label
    Friend WithEvents txtDataFolder As TextBox
    Friend WithEvents picJsonFolder As PictureBox
    Friend WithEvents btnConvertCMDB As Button
    Friend WithEvents btnCancel As Button
    Friend WithEvents fbdJsonDataFolder As FolderBrowserDialog
    Friend WithEvents ofdCMDBFile As OpenFileDialog
    Friend WithEvents Button1 As Button
End Class
