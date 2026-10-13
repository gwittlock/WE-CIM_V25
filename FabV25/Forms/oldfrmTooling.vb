Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraGrid
Imports DevExpress.XtraGrid.Views.Grid
Imports DevExpress.XtraVerticalGrid.Rows
Imports System.Data.OleDb

Public Class frmTooling
    Protected Overloads Overrides Sub WndProc(ByRef m As Message)

        '//Prevent the user from moving the form

        If (m.Msg = WM_SYSCOMMAND) AndAlso (m.WParam.ToInt32() = SC_MOVE) Then
            Exit Sub
        End If

        If (m.Msg = WM_NCLBUTTONDOWN) AndAlso (m.WParam.ToInt32() = HTCAPTION) Then
            Exit Sub
        End If

        MyBase.WndProc(m)
    End Sub
    Private edit As RepositoryItemCheckEdit
    Private sPieces() As String
    Private WithEvents rbProcessParameters As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit
    Private bToolMode As Boolean

    Private Sub frmTooling_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        frmMain.CloseFirstPanel()
    End Sub

    Private Sub frmTooling_Load(sender As Object, e As EventArgs) Handles Me.Load
        LoadToolSetupListview()
    End Sub

    Private Sub LoadToolSetupListview()
        Dim lst As New List(Of TheTools)()
        Dim clsToolData As ToolData
        Dim bestwidth As Integer

        bToolMode = True

        grdToolLayer.DataSource = Nothing

        gViewToolLayer.Columns.Clear()

        edit = New RepositoryItemCheckEdit()
        edit.CheckStyle = DevExpress.XtraEditors.Controls.CheckStyles.Radio

        g_DescripChar_len = 0

        If Not g_colToolSetup Is Nothing Then
            For Each clsToolData In g_colToolSetup
                With clsToolData
                    If UCase$(.Workplane) = UCase$(GetCurrentWorkplane()) Then

                        If (Len(.Description) > g_DescripChar_len) Then

                            g_DescripChar_len = Len(.Description)

                        End If
                        lst.Add(New TheTools() With {.ToolHidden = IIf((clsToolData.show), True, False), _
                                                     .ToolNumber = clsToolData.NC_Code_Number, _
                                                     .ToolName = clsToolData.Description, _
                                                     .ToolColor = System.Drawing.ColorTranslator.FromOle(clsToolData.TlColor)})
                    End If
                End With
            Next

            grdToolLayer.DataSource = lst
            gViewToolLayer.Columns("ToolHidden").Caption = "Visible "
            gViewToolLayer.Columns("ToolHidden").ColumnEdit = edit
            gViewToolLayer.Columns("ToolColor").Visible = False
            gViewToolLayer.Columns("ToolNumber").Visible = True


            grdToolLayer.SuspendLayout()

            For Each col In gViewToolLayer.Columns

                bestwidth = col.GetBestWidth()
                col.MinWidth = bestwidth
            Next


            grdToolLayer.ResumeLayout()
            AddHandler gViewToolLayer.RowStyle, AddressOf gViewToolLayer_RowStyle

        End If

    End Sub
    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.
        rbProcessParameters = New RepositoryItemButtonEdit()
    End Sub
    Private Sub LoadToolData(clsToolData As ToolData)

        Dim clsOperation As Operation
        Dim sData As String
        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer
        Dim my_sOptions As String
        Dim arrOptions As Collection
        Dim currentCategoryRow As CategoryRow = Nothing

        vgProcessParameters.Rows.Clear()

        currentCategoryRow = CreateCategoryRow(vgProcessParameters, "Processs Parameters", "Processs Parameters", "Processs Parameters", "Processs Parameters", "Processs Parameters", 0, DB_HEADER, sEmpty, 0)

        ''degParameters.InitializeGrid()

        For Each clsOperation In clsToolData.ToolOps
            ' parse out the data
            sData = clsOperation.Description & "|" & clsOperation.Parameters

            sPieces = Split(sData, "|")

            my_DatVarName = sPieces(0)
            my_CVarName = sPieces(0)
            my_DataType = ConvertDatTypeToDBType(sPieces(1))
            my_DialogDisplay = sPieces(2)
            my_VBValue = sPieces(3)
            my_CValue = sPieces(3) 'Also see the case statement for combobox
            my_RecodID = 0
            my_sOptions = sEmpty

            Select Case (my_DataType)

                Case DB_HEADER
                    currentCategoryRow = CreateCategoryRow(vgProcessParameters, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0)

                Case DB_STR
                    currentCategoryRow.ChildRows.Add(CreateStringEditorRow(vgProcessParameters, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))


                Case DB_BOOL
                    currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(vgProcessParameters, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                Case DB_INT
                    currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgProcessParameters, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                Case DB_DBL
                    currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgProcessParameters, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                Case DB_TOOL

                Case DB_DCOMBO, DB_COMBO
                    Dim i As Integer

                    lstOptions.Items.Clear()

                    For i = 4 To sPieces.Count - 1
                        lstOptions.Items.Add(sPieces(i).ToString)

                        If (sPieces(i).ToString = my_VBValue) Then
                            my_CValue = i - 4
                        End If
                    Next
                    arrOptions = PopulateComboListFromDat(lstOptions)

                    currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgProcessParameters, _
                                                            my_DatVarName, _
                                                             my_DialogDisplay, _
                                                             my_VBValue, _
                                                             my_CValue, my_CVarName, my_DataType, my_RecodID, arrOptions, 0))
            End Select

        Next

        clsOperation = Nothing
        ' now we add our custom parameters (if not punching)

        If (clsToolData.type_id < 15) Or (clsToolData.type_id > 27) Then
            '    'Note: We set the level to the empty string because we
            '    'do *not* want to generate a Create:Hole: portal command
            '    'unless the user provides a level value.

            sData = "Level|dbl|Level|"

            sPieces = Split(sData, "|")

            my_DatVarName = sPieces(0)
            my_CVarName = sPieces(0)
            my_DataType = ConvertDatTypeToDBType(sPieces(1))
            my_DialogDisplay = sPieces(2)
            my_VBValue = 0
            my_CValue = 0
            my_RecodID = 0
            my_sOptions = sEmpty
            currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgProcessParameters, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

        End If

    End Sub



    Private Sub gViewToolLayer_Click(sender As Object, e As EventArgs) Handles gViewToolLayer.Click
        Dim clsToolData As ToolData
        Dim View As Views.Grid.GridView
        Dim MyConnection As OleDbConnection = OpenTheCmdb()

        View = CType(sender, Views.Grid.GridView)

        Dim col As Object = View.GetRowCellValue(View.FocusedRowHandle, View.Columns("ToolNumber"))
        clsToolData = g_colToolSetup("T" & col.ToString())

        GetToolOperations(clsToolData, MyConnection)
        GetMaterialAttributes(clsToolData, MyConnection)

        MyConnection.Close()
        MyConnection.Dispose()
        LoadToolData(clsToolData)
        clsToolData = Nothing
    End Sub

    Private Sub gViewToolLayer_RowCellClick(sender As Object, e As RowCellClickEventArgs) Handles gViewToolLayer.RowCellClick
        Dim View As Views.Grid.GridView

        View = CType(sender, Views.Grid.GridView)

        If (e.Column.Name = "colToolHidden") Then

            Dim clsToolData As ToolData

            Dim col As Object = gViewToolLayer.GetRowCellValue(e.RowHandle, gViewToolLayer.Columns(1))

            clsToolData = g_colToolSetup("T" & col.ToString())

            gViewToolLayer.SetRowCellValue(e.RowHandle, e.Column, IIf(e.CellValue = True, False, True))

            clsToolData.show = e.CellValue

            modView.ViewRegen()

        End If
    End Sub

    Private Sub gViewToolLayer_RowStyle(sender As Object, e As RowStyleEventArgs) Handles gViewToolLayer.RowStyle
        Dim lst As List(Of TheTools)
        Dim item As TheTools

        If (e.RowHandle < 0) Then

            Return

        End If

        lst = CType(gViewToolLayer.DataSource, List(Of TheTools))
        item = lst(e.RowHandle)
        e.Appearance.ForeColor = item.ToolColor
        e.Appearance.BackColor = Color.Black
    End Sub

    Private Sub gViewToolLayer_ShownEditor(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles gViewToolLayer.ShownEditor
        Dim View As Views.Grid.GridView

        View = CType(sender, Views.Grid.GridView)

        If (View.FocusedColumn.Name = "colToolName" Or View.FocusedColumn.Name = "colToolNumber") Then

            e.Cancel = True

        End If
    End Sub

    Private Sub grdToolLayer_GotFocus(sender As Object, e As EventArgs) Handles grdToolLayer.GotFocus
        Dim bestwidth As Integer
        grdToolLayer.SuspendLayout()

        For Each col In gViewToolLayer.Columns

            bestwidth = col.GetBestWidth()
            col.MinWidth = bestwidth
        Next

        grdToolLayer.ResumeLayout()
    End Sub

    Private Sub sbShowAll_Click(sender As Object, e As EventArgs) Handles sbShowAll.Click
        ShowAllTools()
    End Sub

    Private Sub ShowAllTools()
        Dim clsToolData As ToolData
        Dim nI As Integer


        For nI = 0 To gViewToolLayer.RowCount - 1
            Dim col As Object = gViewToolLayer.GetRowCellValue(nI, gViewToolLayer.Columns(1))

            clsToolData = g_colToolSetup("T" & col.ToString())

            gViewToolLayer.SetRowCellValue(gViewToolLayer.GetVisibleRowHandle(nI), gViewToolLayer.Columns(0), True)
            clsToolData.show = True

        Next

        modView.ViewRegen()
    End Sub

    Private Sub HideAllTools()
        Dim clsToolData As ToolData
        Dim nI As Integer


        For nI = 0 To gViewToolLayer.RowCount - 1
            Dim col As Object = gViewToolLayer.GetRowCellValue(nI, gViewToolLayer.Columns(1))

            clsToolData = g_colToolSetup("T" & col.ToString())

            gViewToolLayer.SetRowCellValue(gViewToolLayer.GetVisibleRowHandle(nI), gViewToolLayer.Columns(0), False)
            clsToolData.show = False

        Next

        modView.ViewRegen()
    End Sub

    Private Sub sbHideAll_Click(sender As Object, e As EventArgs) Handles sbHideAll.Click
        HideAllTools()

    End Sub

    Private Sub sbSetCurrent_Click(sender As Object, e As EventArgs) Handles sbSetCurrent.Click
        Dim name As String
        Dim status_bar_text As String
        If (gViewToolLayer.FocusedRowHandle < 0) Then

            MessageBox.Show("Please select a 'Tool'.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)

        Else
            g_clsActiveToolData = g_colToolSetup("T" & gViewToolLayer.GetRowCellValue(gViewToolLayer.FocusedRowHandle, gViewToolLayer.Columns("ToolNumber")))

            If (g_clsActiveToolData.show = False) Then

                MessageBox.Show("Selected 'Tool' is currently hidden.", "Error", MessageBoxButtons.AbortRetryIgnore, MessageBoxIcon.Exclamation)

            Else

                name = GetEntityName(g_clsActiveToolData.Model_ID)

                SetActiveLayer(name, g_clsActiveToolData.TlColor)

                status_bar_text = "Tool - " & g_clsActiveToolData.NC_Code_Number


                DefaultZ = g_clsActiveToolData.level

                ' set the default drawing flag
                lReturn = DefaultIntSet("partprof", HeaderIntGet("PartProf", 0))
                frmMain.statusActiveLayer.Caption = "Active: " & status_bar_text


            End If

        End If
        frmMain.CloseFirstPanel()

        Me.Dispose()

    End Sub

    Private Sub vgProcessParameters_CellValueChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgProcessParameters.CellValueChanged
        Dim clsToolData As ToolData
        Dim sName As String

        clsToolData = g_colToolSetup("T" & gViewToolLayer.GetRowCellValue(gViewToolLayer.FocusedRowHandle, gViewToolLayer.Columns("ToolNumber")))

        sName = e.Row.Properties.Caption

        Select Case sName

            Case "Amperage"
                clsToolData.Amperage = e.Value

            Case "Angle"
                clsToolData.Angle = e.Value

            Case "CutQuality"
                Select Case e.Value

                    Case "Scribing"
                        clsToolData.CutQuality = 0

                    Case "Coarse"
                        clsToolData.CutQuality = 1

                    Case "Medium"
                        clsToolData.CutQuality = 2

                    Case "Medium Fine"
                        clsToolData.CutQuality = 3

                    Case "Fine"
                        clsToolData.CutQuality = 4

                    Case "Extra Fine"
                        clsToolData.CutQuality = 5

                End Select

            Case "CutSide"
                Select Case e.Value

                    Case "Left"
                        clsToolData.CutSide = 1

                    Case "Right"
                        clsToolData.CutSide = -1

                    Case "None"
                        clsToolData.CutSide = 0

                End Select

            Case "CutType"
                Select Case e.Value

                    Case "Inside"
                        clsToolData.CutType = 0

                    Case "Outside"
                        clsToolData.CutType = 1

                End Select

            Case "Doff"
                clsToolData.Doff = e.Value

            Case "Feed"
                clsToolData.Feed = e.Value

            Case "FeedMode"
                Select Case e.Value

                    Case "Canned Cycle"
                        clsToolData.FeedMode = 0

                    Case "Standard Punching"
                        clsToolData.FeedMode = 1

                    Case "Equalized Punching"
                        clsToolData.FeedMode = 2

                    Case "Bridge Punching"
                        clsToolData.FeedMode = 3

                    Case "UPR"
                        clsToolData.FeedMode = 4

                    Case "UPT"
                        clsToolData.FeedMode = 5

                End Select

            Case "Loff"
                clsToolData.Loff = e.Value

            Case "MaxPressure"
                clsToolData.MaxPressure = e.Value

            Case "Plunge"
                clsToolData.Plunge = e.Value

            Case "Speed"
                clsToolData.Speed = e.Value

                ' our custom parameters

            Case "Level"
                clsToolData.level = e.Value

                '        Case "IndexAngle":
                '            clsToolData.Index_Angle = e.value

        End Select

        clsToolData = Nothing

    End Sub

End Class