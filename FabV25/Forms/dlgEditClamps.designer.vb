<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class dlgEditClamps
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
        Me.vgProplist = New DevExpress.XtraVerticalGrid.VGridControl()
        Me.btnAccept = New DevExpress.XtraEditors.SimpleButton()
        Me.frmEditClampsConvertedLayout = New DevExpress.XtraLayout.LayoutControl()
        Me.cboDefn = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.LayoutControlGroup1 = New DevExpress.XtraLayout.LayoutControlGroup()
        Me.LayoutControlItem1 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.LayoutControlItem2 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.LayoutControlItem3 = New DevExpress.XtraLayout.LayoutControlItem()
        Me.LayoutConverter1 = New DevExpress.XtraLayout.Converter.LayoutConverter(Me.components)
        CType(Me.vgProplist, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.frmEditClampsConvertedLayout, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.frmEditClampsConvertedLayout.SuspendLayout()
        CType(Me.cboDefn.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlGroup1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.LayoutControlItem3, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'vgProplist
        '
        Me.vgProplist.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.Simple
        Me.vgProplist.LayoutStyle = DevExpress.XtraVerticalGrid.LayoutViewStyle.SingleRecordView
        Me.vgProplist.Location = New System.Drawing.Point(12, 36)
        Me.vgProplist.Name = "vgProplist"
        Me.vgProplist.OptionsBehavior.UseEnterAsTab = True
        Me.vgProplist.OptionsView.ShowButtons = False
        Me.vgProplist.Size = New System.Drawing.Size(270, 272)
        Me.vgProplist.TabIndex = 3
        Me.vgProplist.TreeButtonStyle = DevExpress.XtraVerticalGrid.TreeButtonStyle.ExplorerBar
        '
        'btnAccept
        '
        Me.btnAccept.Location = New System.Drawing.Point(12, 312)
        Me.btnAccept.Name = "btnAccept"
        Me.btnAccept.Size = New System.Drawing.Size(270, 22)
        Me.btnAccept.StyleController = Me.frmEditClampsConvertedLayout
        Me.btnAccept.TabIndex = 4
        Me.btnAccept.Text = "Accept"
        Me.btnAccept.ToolTip = "Applies the new values for each clamp to the model."
        '
        'frmEditClampsConvertedLayout
        '
        Me.frmEditClampsConvertedLayout.Controls.Add(Me.cboDefn)
        Me.frmEditClampsConvertedLayout.Controls.Add(Me.btnAccept)
        Me.frmEditClampsConvertedLayout.Controls.Add(Me.vgProplist)
        Me.frmEditClampsConvertedLayout.Dock = System.Windows.Forms.DockStyle.Fill
        Me.frmEditClampsConvertedLayout.Location = New System.Drawing.Point(0, 0)
        Me.frmEditClampsConvertedLayout.Name = "frmEditClampsConvertedLayout"
        Me.frmEditClampsConvertedLayout.Root = Me.LayoutControlGroup1
        Me.frmEditClampsConvertedLayout.Size = New System.Drawing.Size(294, 346)
        Me.frmEditClampsConvertedLayout.TabIndex = 11
        '
        'cboDefn
        '
        Me.cboDefn.EnterMoveNextControl = True
        Me.cboDefn.Location = New System.Drawing.Point(60, 12)
        Me.cboDefn.Name = "cboDefn"
        Me.cboDefn.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboDefn.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboDefn.Size = New System.Drawing.Size(222, 20)
        Me.cboDefn.TabIndex = 9
        '
        'LayoutControlGroup1
        '
        Me.LayoutControlGroup1.CustomizationFormText = "LayoutControlGroup1"
        Me.LayoutControlGroup1.EnableIndentsWithoutBorders = DevExpress.Utils.DefaultBoolean.[True]
        Me.LayoutControlGroup1.GroupBordersVisible = False
        Me.LayoutControlGroup1.Items.AddRange(New DevExpress.XtraLayout.BaseLayoutItem() {Me.LayoutControlItem1, Me.LayoutControlItem2, Me.LayoutControlItem3})
        Me.LayoutControlGroup1.Name = "Root"
        Me.LayoutControlGroup1.Size = New System.Drawing.Size(294, 346)
        Me.LayoutControlGroup1.TextVisible = False
        '
        'LayoutControlItem1
        '
        Me.LayoutControlItem1.Control = Me.cboDefn
        Me.LayoutControlItem1.CustomizationFormText = "Definition"
        Me.LayoutControlItem1.Location = New System.Drawing.Point(0, 0)
        Me.LayoutControlItem1.Name = "cboDefnitem"
        Me.LayoutControlItem1.Size = New System.Drawing.Size(274, 24)
        Me.LayoutControlItem1.Text = "Definition"
        Me.LayoutControlItem1.TextSize = New System.Drawing.Size(45, 13)
        '
        'LayoutControlItem2
        '
        Me.LayoutControlItem2.Control = Me.btnAccept
        Me.LayoutControlItem2.CustomizationFormText = "btnAcceptitem"
        Me.LayoutControlItem2.Location = New System.Drawing.Point(0, 300)
        Me.LayoutControlItem2.Name = "btnAcceptitem"
        Me.LayoutControlItem2.Size = New System.Drawing.Size(274, 26)
        Me.LayoutControlItem2.TextSize = New System.Drawing.Size(0, 0)
        Me.LayoutControlItem2.TextVisible = False
        '
        'LayoutControlItem3
        '
        Me.LayoutControlItem3.Control = Me.vgProplist
        Me.LayoutControlItem3.CustomizationFormText = "vgProplistitem"
        Me.LayoutControlItem3.Location = New System.Drawing.Point(0, 24)
        Me.LayoutControlItem3.Name = "vgProplistitem"
        Me.LayoutControlItem3.Size = New System.Drawing.Size(274, 276)
        Me.LayoutControlItem3.TextSize = New System.Drawing.Size(0, 0)
        Me.LayoutControlItem3.TextVisible = False
        '
        'dlgEditClamps
        '
        Me.AcceptButton = Me.btnAccept
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(294, 346)
        Me.Controls.Add(Me.frmEditClampsConvertedLayout)
        Me.DoubleBuffered = True
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow
        Me.Name = "dlgEditClamps"
        Me.Text = "Edit Clamp Locations"
        CType(Me.vgProplist, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.frmEditClampsConvertedLayout, System.ComponentModel.ISupportInitialize).EndInit()
        Me.frmEditClampsConvertedLayout.ResumeLayout(False)
        CType(Me.cboDefn.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlGroup1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.LayoutControlItem3, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents vgProplist As DevExpress.XtraVerticalGrid.VGridControl
    Friend WithEvents btnAccept As DevExpress.XtraEditors.SimpleButton
    Friend WithEvents cboDefn As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents frmEditClampsConvertedLayout As DevExpress.XtraLayout.LayoutControl
    Friend WithEvents LayoutControlGroup1 As DevExpress.XtraLayout.LayoutControlGroup
    Friend WithEvents LayoutControlItem1 As DevExpress.XtraLayout.LayoutControlItem
    Friend WithEvents LayoutControlItem2 As DevExpress.XtraLayout.LayoutControlItem
    Friend WithEvents LayoutControlItem3 As DevExpress.XtraLayout.LayoutControlItem
    Friend WithEvents LayoutConverter1 As DevExpress.XtraLayout.Converter.LayoutConverter
End Class
