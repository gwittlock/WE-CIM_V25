Namespace WindowsApplication234
    Partial Public Class ChkBoxFrame
        ''' <summary> 
        ''' Required designer variable.
        ''' </summary>
        Private components As System.ComponentModel.IContainer = Nothing

        ''' <summary> 
        ''' Clean up any resources being used.
        ''' </summary>
        ''' <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing AndAlso (components IsNot Nothing) Then
                components.Dispose()
            End If
            MyBase.Dispose(disposing)
        End Sub

#Region "Component Designer generated code"

        ''' <summary> 
        ''' Required method for Designer support - do not modify 
        ''' the contents of this method with the code editor.
        ''' </summary>
        Private Sub InitializeComponent()
            Me.panelEx1 = New DevExpress.XtraEditors.GroupControl
            Me.CheckEdit1 = New DevExpress.XtraEditors.CheckEdit
            CType(Me.panelEx1, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.panelEx1.SuspendLayout()
            CType(Me.CheckEdit1.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.SuspendLayout()
            '
            'panelEx1
            '
            Me.panelEx1.Controls.Add(Me.CheckEdit1)
            Me.panelEx1.Location = New System.Drawing.Point(0, 0)
            Me.panelEx1.Name = "panelEx1"
            Me.panelEx1.Size = New System.Drawing.Size(200, 100)
            Me.panelEx1.TabIndex = 0
            '
            'CheckEdit1
            '
            Me.CheckEdit1.Location = New System.Drawing.Point(14, 0)
            Me.CheckEdit1.Name = "CheckEdit1"
            Me.CheckEdit1.Properties.Appearance.ForeColor = System.Drawing.Color.Silver
            Me.CheckEdit1.Properties.Appearance.Options.UseForeColor = True
            Me.CheckEdit1.Properties.Caption = "CheckEdit1"
            Me.CheckEdit1.Size = New System.Drawing.Size(75, 19)
            Me.CheckEdit1.TabIndex = 0
            '
            'ChkBoxFrame
            '
            Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.BackColor = System.Drawing.Color.Transparent
            Me.Controls.Add(Me.panelEx1)
            Me.Name = "ChkBoxFrame"
            Me.Padding = New System.Windows.Forms.Padding(0, 11, 0, 0)
            Me.Size = New System.Drawing.Size(201, 103)
            CType(Me.panelEx1, System.ComponentModel.ISupportInitialize).EndInit()
            Me.panelEx1.ResumeLayout(False)
            CType(Me.CheckEdit1.Properties, System.ComponentModel.ISupportInitialize).EndInit()
            Me.ResumeLayout(False)

        End Sub
        Friend WithEvents panelEx1 As DevExpress.XtraEditors.GroupControl
        Friend WithEvents CheckEdit1 As DevExpress.XtraEditors.CheckEdit

#End Region

    End Class
End Namespace
