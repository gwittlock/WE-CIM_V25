Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraGrid

Public Class frmSelectTools
    Private edit As RepositoryItemCheckEdit
    Private sPieces() As String
    Private WithEvents rbProcessParameters As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit
    Private bToolMode As Boolean
    'Protected Overloads Overrides Sub WndProc(ByRef m As Message)

    '    '//Prevent the user from moving the form

    '    If (m.Msg = WM_SYSCOMMAND) AndAlso (m.WParam.ToInt32() = SC_MOVE) Then
    '        Exit Sub
    '    End If

    '    If (m.Msg = WM_NCLBUTTONDOWN) AndAlso (m.WParam.ToInt32() = HTCAPTION) Then
    '        Exit Sub
    '    End If

    '    MyBase.WndProc(m)
    'End Sub
    'Private Sub LoadToolSetupListview()
    '    Dim lst As New List(Of TheTools)()
    '    Dim clsToolData As ToolData
    '    Dim bestwidth As Integer

    '    bToolMode = True

    '    grdToolLayer.DataSource = Nothing

    '    gViewToolLayer.Columns.Clear()

    '    edit = New RepositoryItemCheckEdit()
    '    edit.CheckStyle = DevExpress.XtraEditors.Controls.CheckStyles.Radio

    '    g_DescripChar_len = 0

    '    If Not g_colToolSetup Is Nothing Then
    '        For Each clsToolData In g_colToolSetup
    '            With clsToolData
    '                If UCase$(.Workplane) = UCase$(GetCurrentWorkplane()) Then

    '                    If (Len(.Description) > g_DescripChar_len) Then

    '                        g_DescripChar_len = Len(.Description)

    '                    End If
    '                    lst.Add(New TheTools() With {.ToolHidden = IIf((clsToolData.show), True, False), _
    '                                                 .ToolNumber = clsToolData.NC_Code_Number, _
    '                                                 .ToolName = clsToolData.Description, _
    '                                                 .ToolColor = System.Drawing.ColorTranslator.FromOle(clsToolData.TlColor)})
    '                End If
    '            End With
    '        Next

    '        grdToolLayer.DataSource = lst
    '        gViewToolLayer.Columns("ToolHidden").Caption = "Hidden "
    '        gViewToolLayer.Columns("ToolHidden").ColumnEdit = edit
    '        gViewToolLayer.Columns("ToolColor").Visible = False
    '        gViewToolLayer.Columns("ToolNumber").Visible = True


    '        grdToolLayer.SuspendLayout()

    '        For Each col In gViewToolLayer.Columns

    '            bestwidth = col.GetBestWidth()
    '            col.MinWidth = bestwidth
    '        Next


    '        grdToolLayer.ResumeLayout()
    '        AddHandler gViewToolLayer.RowStyle, AddressOf gViewToolLayer_RowStyle

    '    End If

    'End Sub

    'Private Sub gViewToolLayer_Click(sender As Object, e As EventArgs) Handles gViewToolLayer.Click
    '    Dim clsToolData As ToolData
    '    Dim View As Views.Grid.GridView

    '    View = CType(sender, Views.Grid.GridView)

    '    Dim col As Object = View.GetRowCellValue(View.FocusedRowHandle, View.Columns("ToolNumber"))
    '    clsToolData = g_colToolSetup("T" & col.ToString())

    '    clsToolData = Nothing
    'End Sub

    'Private Sub gViewToolLayer_RowStyle(sender As Object, e As DevExpress.XtraGrid.Views.Grid.RowStyleEventArgs) Handles gViewToolLayer.RowStyle
    '    Dim lst As List(Of TheTools)
    '    Dim item As TheTools

    '    If (e.RowHandle < 0) Then

    '        Return

    '    End If

    '    lst = CType(gViewToolLayer.DataSource, List(Of TheTools))
    '    item = lst(e.RowHandle)
    '    e.Appearance.ForeColor = item.ToolColor
    '    e.Appearance.BackColor = Color.Black
    'End Sub



    'Private Sub gViewToolLayer_ShowingEditor(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles gViewToolLayer.ShowingEditor
    '    Dim View As Views.Grid.GridView

    '    View = CType(sender, Views.Grid.GridView)

    '    If (View.FocusedColumn.Name = "colToolName" Or View.FocusedColumn.Name = "colToolNumber") Then

    '        e.Cancel = True

    '    End If
    'End Sub

    'Private Sub frmSelectTools_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
    '    'frmMain.CloseFirstPanel()
    'End Sub

    'Private Sub frmSelectTools_Load(sender As Object, e As EventArgs) Handles Me.Load
    '    LoadToolSetupListview()
    'End Sub



    'Private Sub gViewToolLayer_ShownEditor(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles gViewToolLayer.ShownEditor
    '    LoadToolSetupListview()
    'End Sub

    'Private Sub grdToolLayer_GotFocus(sender As Object, e As EventArgs) Handles grdToolLayer.GotFocus
    '    Dim bestwidth As Integer
    '    grdToolLayer.SuspendLayout()

    '    For Each col In gViewToolLayer.Columns

    '        bestwidth = col.GetBestWidth()
    '        col.MinWidth = bestwidth
    '    Next

    '    grdToolLayer.ResumeLayout()
    'End Sub

    'Private Sub sbOk_Click(sender As Object, e As EventArgs) Handles sbOk.Click
    '    Dim toolID As Long
    '    Dim lCount As Long
    '    Dim m_FocusedHandle As Long

    '    m_FocusedHandle = gViewToolLayer.FocusedRowHandle


    '    If (m_FocusedHandle < 0) Then

    '        Call MsgBox("Please select a 'Tool'.", vbExclamation, App_Path)

    '    Else

    '        g_clsActiveToolData = g_colToolSetup("T" & gViewToolLayer.GetRowCellValue(gViewToolLayer.FocusedRowHandle, gViewToolLayer.Columns("ToolNumber")))

    '        toolID = g_clsActiveToolData.Model_ID


    '        lReturn = SetSelectionFilters( _
    '            (SELECTION_FILTER.SEL_LINE Or SELECTION_FILTER.SEL_ARC Or SELECTION_FILTER.SEL_HOLE Or SELECTION_FILTER.SEL_POINT Or _
    '                SELECTION_FILTER.SEL_COMMAND Or SELECTION_FILTER.SEL_PROFILE Or SELECTION_FILTER.SEL_FEATURE))

    '        lCount = SelectAllRefsTo(toolID)

    '        frmMain.SynchronizeUI(lCount)

    '        Me.Dispose()

    '    End If
    'End Sub

    'Private Sub sbCancel_Click(sender As Object, e As EventArgs) Handles sbCancel.Click
    '    Me.Dispose()
    'End Sub

    'Private Sub frmSelectTools_Resize(sender As Object, e As EventArgs) Handles Me.Resize
    '    LayoutControlGroup1.Width = Me.Width - 24
    '    grdToolLayer.Width = LayoutControlGroup1.Width - 24


    'End Sub

    'Private Sub grdToolLayer_Click(sender As Object, e As EventArgs) Handles grdToolLayer.Click

    'End Sub
End Class