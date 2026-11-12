<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmWorkZones
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
        Me.grdWorkZones = New DevExpress.XtraGrid.GridControl()
        Me.grdWorkZonesView = New DevExpress.XtraGrid.Views.Grid.GridView()
        Me.colZone = New DevExpress.XtraGrid.Columns.GridColumn()
        Me.colStartX = New DevExpress.XtraGrid.Columns.GridColumn()
        Me.colEndX = New DevExpress.XtraGrid.Columns.GridColumn()
        Me.colHoldX = New DevExpress.XtraGrid.Columns.GridColumn()
        Me.colHoldY = New DevExpress.XtraGrid.Columns.GridColumn()
        Me.colRepo = New DevExpress.XtraGrid.Columns.GridColumn()
        Me.cmdCancel = New DevExpress.XtraEditors.SimpleButton()
        Me.cmdHold = New DevExpress.XtraEditors.SimpleButton()
        Me.cmdRemove = New DevExpress.XtraEditors.SimpleButton()
        Me.cmdAssign = New DevExpress.XtraEditors.SimpleButton()
        Me.cmdAdd = New DevExpress.XtraEditors.SimpleButton()
        CType(Me.grdWorkZones, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.grdWorkZonesView, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grdWorkZones
        '
        Me.grdWorkZones.Location = New System.Drawing.Point(12, 12)
        Me.grdWorkZones.MainView = Me.grdWorkZonesView
        Me.grdWorkZones.Name = "grdWorkZones"
        Me.grdWorkZones.Size = New System.Drawing.Size(431, 200)
        Me.grdWorkZones.TabIndex = 0
        Me.grdWorkZones.ViewCollection.AddRange(New DevExpress.XtraGrid.Views.Base.BaseView() {Me.grdWorkZonesView})
        '
        'grdWorkZonesView
        '
        Me.grdWorkZonesView.Columns.AddRange(New DevExpress.XtraGrid.Columns.GridColumn() {Me.colZone, Me.colStartX, Me.colEndX, Me.colHoldX, Me.colHoldY, Me.colRepo})
        Me.grdWorkZonesView.GridControl = Me.grdWorkZones
        Me.grdWorkZonesView.Name = "grdWorkZonesView"
        Me.grdWorkZonesView.OptionsBehavior.AutoPopulateColumns = False
        Me.grdWorkZonesView.OptionsView.ColumnAutoWidth = False
        Me.grdWorkZonesView.OptionsView.ShowGroupPanel = False
        Me.grdWorkZonesView.OptionsView.ShowIndicator = False
        '
        'colZone
        '
        Me.colZone.Caption = "Zone"
        Me.colZone.FieldName = "Index"
        Me.colZone.Name = "colZone"
        Me.colZone.OptionsColumn.AllowEdit = False
        Me.colZone.Visible = True
        Me.colZone.VisibleIndex = 0
        Me.colZone.Width = 45
        '
        'colStartX
        '
        Me.colStartX.Caption = "Start X"
        Me.colStartX.FieldName = "XMin"
        Me.colStartX.Name = "colStartX"
        Me.colStartX.Visible = True
        Me.colStartX.VisibleIndex = 1
        '
        'colEndX
        '
        Me.colEndX.Caption = "End X"
        Me.colEndX.FieldName = "XMax"
        Me.colEndX.Name = "colEndX"
        Me.colEndX.Visible = True
        Me.colEndX.VisibleIndex = 2
        '
        'colHoldX
        '
        Me.colHoldX.Caption = "" & Global.Microsoft.VisualBasic.ChrW(9) & "Hold X"
        Me.colHoldX.FieldName = "HoldX"
        Me.colHoldX.Name = "colHoldX"
        Me.colHoldX.Visible = True
        Me.colHoldX.VisibleIndex = 3
        '
        'colHoldY
        '
        Me.colHoldY.Caption = "Hold Y"
        Me.colHoldY.FieldName = "HoldY"
        Me.colHoldY.Name = "colHoldY"
        Me.colHoldY.Visible = True
        Me.colHoldY.VisibleIndex = 4
        '
        'colRepo
        '
        Me.colRepo.Caption = "Repo"
        Me.colRepo.FieldName = "RepositionDistanceX"
        Me.colRepo.Name = "colRepo"
        Me.colRepo.Visible = True
        Me.colRepo.VisibleIndex = 5
        '
        'cmdCancel
        '
        Me.cmdCancel.Location = New System.Drawing.Point(311, 218)
        Me.cmdCancel.Name = "cmdCancel"
        Me.cmdCancel.Size = New System.Drawing.Size(101, 22)
        Me.cmdCancel.TabIndex = 22
        Me.cmdCancel.Text = "Cancel"
        '
        'cmdHold
        '
        Me.cmdHold.Location = New System.Drawing.Point(116, 244)
        Me.cmdHold.Name = "cmdHold"
        Me.cmdHold.Size = New System.Drawing.Size(101, 22)
        Me.cmdHold.TabIndex = 21
        Me.cmdHold.Text = "Drag"
        '
        'cmdRemove
        '
        Me.cmdRemove.Location = New System.Drawing.Point(11, 244)
        Me.cmdRemove.Name = "cmdRemove"
        Me.cmdRemove.Size = New System.Drawing.Size(101, 22)
        Me.cmdRemove.TabIndex = 20
        Me.cmdRemove.Text = "Remove"
        '
        'cmdAssign
        '
        Me.cmdAssign.Location = New System.Drawing.Point(116, 218)
        Me.cmdAssign.Name = "cmdAssign"
        Me.cmdAssign.Size = New System.Drawing.Size(101, 22)
        Me.cmdAssign.TabIndex = 19
        Me.cmdAssign.Text = "Assign"
        '
        'cmdAdd
        '
        Me.cmdAdd.Location = New System.Drawing.Point(11, 218)
        Me.cmdAdd.Name = "cmdAdd"
        Me.cmdAdd.Size = New System.Drawing.Size(101, 22)
        Me.cmdAdd.TabIndex = 18
        Me.cmdAdd.Text = "Add"
        '
        'frmWorkZones
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(455, 344)
        Me.Controls.Add(Me.cmdCancel)
        Me.Controls.Add(Me.cmdHold)
        Me.Controls.Add(Me.cmdRemove)
        Me.Controls.Add(Me.cmdAssign)
        Me.Controls.Add(Me.cmdAdd)
        Me.Controls.Add(Me.grdWorkZones)
        Me.Name = "frmWorkZones"
        Me.Text = "frmWorkZones"
        CType(Me.grdWorkZones, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.grdWorkZonesView, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents grdWorkZones As DevExpress.XtraGrid.GridControl
    Friend WithEvents grdWorkZonesView As DevExpress.XtraGrid.Views.Grid.GridView
    Friend WithEvents colStartX As DevExpress.XtraGrid.Columns.GridColumn
    Friend WithEvents colEndX As DevExpress.XtraGrid.Columns.GridColumn
    Friend WithEvents colHoldX As DevExpress.XtraGrid.Columns.GridColumn
    Friend WithEvents colHoldY As DevExpress.XtraGrid.Columns.GridColumn
    Friend WithEvents colRepo As DevExpress.XtraGrid.Columns.GridColumn
    Friend WithEvents cmdCancel As SimpleButton
    Friend WithEvents cmdHold As SimpleButton
    Friend WithEvents cmdRemove As SimpleButton
    Friend WithEvents cmdAssign As SimpleButton
    Friend WithEvents cmdAdd As SimpleButton
    Friend WithEvents colZone As DevExpress.XtraGrid.Columns.GridColumn
End Class
