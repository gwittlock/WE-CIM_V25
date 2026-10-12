Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraTreeList.Nodes
Imports DevExpress.XtraVerticalGrid
Imports DevExpress.XtraVerticalGrid.Rows
Imports System.Data.OleDb

Public Class dlgEntity
    Implements IDialogMouseUp

    Private nRowIndx As Integer
    Private m_datlines() As String
    Private m_sub_item_sncs As Long
    Private editValue As String
    Private m_entity_id As Long
    Private m_In_Edit_Mode As Boolean
    Private m_prev_selection_filter As Long
    Private m_hot_dot_state As Boolean
    Private WithEvents rbPropSettings As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit
    Private WithEvents rbMaterialSettings As DevExpress.XtraEditors.Repository.RepositoryItemPopupContainerEdit
    Private m_is_active As Boolean
    Private nToolSetupRowId As BaseRow
    Private SelectedMachineName As String
    Private SelectedToolSetupName As String

    Private CFG_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_CATEGORY_NODE As TreeListNode
    Private CFG_MATERIAL_NODE As TreeListNode
    Private Const TAG_MATERIAL_ROOT_NODE = -200
    Private Const TAG_MATERIAL_CATEGORY_NODE = -201
    Private Const TAG_MATERIAL_NODE = -202


    Delegate Sub FocusTextHandler(ByRef VerticalGrid)
    Dim sUsage As String

    Private Structure POINTAPI
        Dim X As Integer
        Dim Y As Integer
    End Structure

    Dim L1SX As Double
    Dim L1SY As Double
    Dim LS1Z As Double
    Dim L2SX As Double
    Dim L2SY As Double
    Dim L2SZ As Double
    Dim L2EY As Double
    Dim L2EX As Double
    Dim L3EY As Double
    Dim L3EX As Double
    Dim L3EZ As Double
    Dim cornerNum As Integer
    'Dim s_flename As String = Application.StartupPath & "\CrossDataOutput.txt"
    'Dim 'objWriter As New System.IO.StreamWriter(s_flename)

    Dim m_Radius As Double

    Dim dSX As Double
    Dim dSY As Double
    Dim dEX As Double
    Dim dEY As Double
    Dim dCX As Double
    Dim dCY As Double
    Dim lDir As Long
    Dim entity_1 As Long
    Dim entity_2 As Long
    Dim Entity_1_Side As Long
    Dim Entity_2_Side As Long
    Dim cnt As Long
    Dim indx As Long
    Dim indx2 As Long
    Dim lSelectionID As Long
    Dim lType As Long
    Dim dir As Long
    Dim containerID As Integer
    Dim Count As Integer
    Dim entityID As Long
    Dim ProfEntTYpe As Long
    Dim pt1SX, pt1SY, pt1SZ As Double
    Dim pt2SX, pt2SY, pt2SZ As Double
    Dim pt3SX, pt3SY, pt3SZ As Double
    Dim pt1EX, pt1EY, pt1EZ As Double
    Dim pt2EX, pt2EY, pt2EZ As Double
    Dim pt3EX, pt3EY, pt3EZ As Double
    Dim BlendEntId1, BlendEntId2, BlendEntId3 As Long
    Dim firstloop As Boolean = True
    Dim pntIndx As Long
    Dim sTheAngle As Double
    Dim m_EntityID_1 As Long
    Dim m_EntityID_2 As Long
    Dim m_EntityID_3 As Long
    Dim lblAngle As Double
    Dim AX As Single
    Dim AY As Single
    Dim BX As Single
    Dim BY As Single
    Dim CX As Single
    Dim CY As Single
    Dim vector_ang As Single
    Dim CycleThru As Long
    Dim nArcDir As Long
    Dim myList As New List(Of String)()
    Dim Pnt1E As New Point3D
    Dim pt1S As New Point3D
    Dim m_myPT As PortalPoint
    Dim strtemp As String


    Private crossProduct As Double
    Private crossProduct2 As Double
    Private m_Points(3) As POINTAPI
    Private m_NextPoint As Short

    Private Sub FocusText(ByRef TheVerticalGrid)
        If TypeOf TheVerticalGrid.ActiveEditor Is TextEdit Then
            Dim edit As TextEdit = TryCast(TheVerticalGrid.ActiveEditor, TextEdit)
            edit.SelectionStart = 0
            edit.SelectionLength = (CStr(edit.EditValue)).Length

        End If
    End Sub
    Private Function ClientMouseUp() As Long Implements IDialogMouseUp.ClientMouseUp
        Dim pick_info As clsPickInfo
        On Error Resume Next

        'Try


        If (vgProplist.FocusedRow IsNot Nothing) Then

                modEdit.RevertProfile()

                pick_info = modPick.GetPickInfo()

                If (pick_info IsNot Nothing) Then
                    UpdateAccumulatorPickInfo(vgProplist, pick_info)
                End If

                ClientMouseUp = 1

                Dim nChildRowCount As Integer

            'determine row
            If (vgProplist.FocusedRow IsNot Nothing) Then


                Dim row As BaseRow = vgProplist.FocusedRow

                If TypeOf row Is CategoryRow Then

                    Dim operation As New MoveToNextCategoryOperation(vgProplist)
                    vgProplist.RowsIterator.DoOperation(operation)
                    vgProplist.Focus()

                    If (operation.NextCategory IsNot Nothing) Then
                        vgProplist.FocusedRow = operation.NextCategory
                        vgProplist.LayoutChanged()
                    Else
                        vgProplist.FocusedRow = vgProplist.Rows(0)
                        vgProplist.LayoutChanged()
                    End If

                End If

                If TypeOf row Is EditorRow Then


                    nChildRowCount = row.ParentRow.ChildRows.Count - 1

                    If (row.Index = nChildRowCount) Then
                        Dim operation As New MoveToNextCategoryOperation(vgProplist)
                        vgProplist.RowsIterator.DoOperation(operation)
                        vgProplist.Focus()

                        If (operation.NextCategory IsNot Nothing) Then
                            vgProplist.FocusedRow = operation.NextCategory
                            vgProplist.LayoutChanged()
                        Else
                            vgProplist.FocusedRow = vgProplist.Rows(0)

                            vgProplist.LayoutChanged()
                        End If
                    Else
                        Dim operation As New MoveToNextEditorRowOperation(vgProplist)
                        vgProplist.RowsIterator.DoOperation(operation)
                        vgProplist.Focus()
                        vgProplist.FocusedRow = operation.NextEditorRow

                        vgProplist.LayoutChanged()
                    End If
                End If
            End If

        End If




        ' Catch ex As Exception

        'End Try
    End Function

    Private Sub FillDefnCombo(sncs As Long)
        Dim ds As DataTable
        Dim connection As New OleDbConnection
        Dim sDisplayStr As String
        Dim sID As Integer
        Dim sql As String

        Try



            cboDefn.Properties.Items.Clear()

            cboDefn.Properties.ShowDropDown = DevExpress.XtraEditors.Controls.ShowDropDown.SingleClick
            cboDefn.Properties.Buttons(0).Enabled = True
            cboDefn.Properties.Buttons(0).Visible = True

            cboDefn.Visible = True

            If (sncs <= 2000) Then  'ie. geometry defn.


                'ws = my_DBEngine.Workspaces(0)

                'db_CIDB = ws.OpenDatabase(PathAppDB())

                ' connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathAppDB() & "';Connect Timeout=30;"

                sql = "SELECT * FROM Defn WHERE (DefnCatalogID = " & sncs & ")"

                ds = ConnectToAppDB(sql)

                'connection = New OleDbConnection(connetionString)


                'connection.Open()

                'oledbAdapter = New OleDbDataAdapter(sql, connection)

                'oledbAdapter.Fill(ds)

                'oledbAdapter.Dispose()

                'connection.Dispose()
                'connection.Close()

                For i = 0 To ds.Rows.Count - 1
                    sDisplayStr = ds.Rows(i).Item("Display")
                    'sDisplayStr = ds.Tables(0).Rows(i).Item("Display")
                    'sID = ds.Tables(0).Rows(i).Item("ID")
                    sID = ds.Rows(i).Item("ID")

                    With cboDefn.Properties

                        .Items.Add(New myItemData(sDisplayStr, sID))

                    End With


                Next

                ds.Dispose()
            End If



            CloseAppdb()


            'DatabaseClose()


            If connection IsNot Nothing Then connection = Nothing


        Catch ex As Exception

            MessageBox.Show("FillDefnCombo: " & ex.Message)

        Finally

            If ds IsNot Nothing Then ds = Nothing

            If connection IsNot Nothing Then connection = Nothing

        End Try



    End Sub
    Public Shadows Function IsActive() As Boolean
        IsActive = m_is_active
    End Function
    Private Sub dlgEntity_Activated(sender As Object, e As EventArgs) Handles Me.Activated

        m_curr_dlg = Me

    End Sub

    Private Sub dlgEntity_Enter(sender As Object, e As EventArgs) Handles Me.Enter
        modView.ViewRefresh()
    End Sub

    Private Sub dlgEntity_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed

        m_is_active = False
        frmMain.CloseFirstPanel()
        modView.ViewRegen()
        'Me.Dispose()
    End Sub


    Private Sub InitStuff()
        Dim sql As String
        Dim lToolID As Long
        Dim childrow As BaseRow



        cboDefn.Visible = False

        m_is_active = True

        Try

            If (m_sncs <= 2000) Then

                'ie. the 'root' sncs of geometry defn
                'NOTE: Has side effect of populating the property list.
                FillDefnCombo(m_sncs)
                cboDefn.Visible = True
                cboDefn.Properties.Buttons(0).Visible = True

                cboDefn.SelectedIndex = 0

                If (g_clsActiveToolData IsNot Nothing) Then
                    Name = g_clsActiveToolData.Description

                    childrow = GetRowByCaption("Tool")

                    If (childrow IsNot Nothing) Then
                        vgProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = "[" & g_clsActiveToolData.NC_Code_Number & "] " & Name

                        lToolID = GetActiveLayer().Id

                        lReturn = UpdateAccumulatorDirect(vgProplist, "tool", lToolID)

                    End If

                End If

            ElseIf ((m_sncs > 2000) And (m_sncs < 3000)) Then

                lReturn = SetLabel(m_sncs)

                sql = "SELECT * FROM ModifyVar WHERE (ModifyID=" & m_sncs & ") Order By ModifyVar.ID"
                lReturn = modPropList.InitFromDB(vgProplist, PathAppDB(), sql)

                If (m_sncs = 2012) Then

                End If

            ElseIf ((m_sncs > 3000)) Then

                sql = "SELECT * FROM DefnVar WHERE (DefnID=" & m_sncs & ")"
                lReturn = modPropList.InitFromDB(vgProplist, PathAppDB(), sql)

            End If

            If (m_In_Edit_Mode = True) Then

                FillEntityProps(m_entity_id)

            End If

        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

        vgProplist.BestFit()


    End Sub

    Public Function SetEntityID(entity_id As Long) As Long
        m_entity_id = entity_id
    End Function


    Public Function SetSNCS(sncs As Long) As Long

        Dim must_init As Boolean

        modEdit.RevertProfile()

        'By doing this, we prevent users from inadvertantly resetting
        'the proplist's values (eg. if they are in the line dialog
        'and select the line icon again when the dialog is open).
        '  must_init = (m_sncs <> sncs)
        must_init = True

        'ie. TrimExtend
        If ((sncs = 2017) And modModeMgmt.IsHotDotEnabled()) Then
            lReturn = modModeMgmt.HotDotEnable(False)
        End If

        m_sncs = sncs

        If (Me.Visible = True) Then
            If (must_init = True) Then
                InitStuff()
                PopulateValues()

            End If
        End If


    End Function
    Protected Overloads Overrides Sub WndProc(ByRef m As Message)

        '//Prevent the user from moving the form
        Try
            If (m.Msg = WM_SYSCOMMAND) AndAlso (m.WParam.ToInt32() = SC_MOVE) Then
                Exit Sub
            End If

            If (m.Msg = WM_NCLBUTTONDOWN) AndAlso (m.WParam.ToInt32() = HTCAPTION) Then
                Exit Sub
            End If

            MyBase.WndProc(m)

        Catch

        End Try

    End Sub

    Private Function SetLabel(sncs As Long) As Long
        Dim sql As String
        Dim ds As DataTable
        Dim connection As New OleDbConnection
        Dim oledbAdapter As New OleDbDataAdapter

        cboDefn.Visible = False
        cboDefn.Properties.Items.Clear()

        Try

            If ((sncs > 1000) And (sncs < 3000)) Then  'ie. an editing defn.

                sql = "SELECT * FROM Modify WHERE (ID=" & sncs & ")"

                ds = ConnectToAppDB(sql)

                cboDefn.EditValue = (ds.Rows(0).Item("Display").ToString)


                cboDefn.EditValue = (ds.Rows(0).Item("Display").ToString)
                cboDefn.Properties.Buttons(0).Enabled = False
                cboDefn.Properties.Buttons(0).Visible = False

                If (cboDefn.Properties.Items.Count = 1) Then

                    cboDefn.Properties.ShowDropDown = DevExpress.XtraEditors.Controls.ShowDropDown.Never
                    cboDefn.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor

                End If

                cboDefn.Visible = True

                'If connection IsNot Nothing Then connection = Nothing
                CloseAppdb()

                ds.Dispose()

            End If


            'If connection IsNot Nothing Then connection = Nothing
            'If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing



        Catch ex As Exception

            MessageBox.Show("SetLabel: SNCS, " & sncs & ex.Message)
            If connection IsNot Nothing Then connection = Nothing
            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing
        Finally

            If ds IsNot Nothing Then ds = Nothing
            If connection IsNot Nothing Then connection = Nothing
            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        End Try



    End Function

    Private Sub rbPropSettings_ButtonClick(sender As Object, e As DevExpress.XtraEditors.Controls.ButtonPressedEventArgs) Handles rbPropSettings.ButtonClick
        Dim FocusedRow As BaseRow = vgProplist.FocusedRow
        Dim Tag As Object = FocusedRow.Tag
        Dim filters As Long = 0
        Dim iname As String = Nothing
        Dim filter_mode As EXPLORER_OPTION
        Dim nDataType As Long
        Dim m_exploer_Filename As String

        Try
            frmMain.tbSelect_Enable.Checked = False
            frmMain.tbSelect_ByWindow.Checked = False

            'FilterSet(0)


            nDataType = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "VarDataType", 0)
            Select Case nDataType
                Case DB_FILE
                    filter_mode = 0

                    frmExplorer.Usage("Merge Model", filter_mode, False, FILE_MM2)

                    frmExplorer.ShowDialog(frmMain)
                    m_exploer_Filename = frmExplorer.GetFilename
                    frmExplorer.Dispose()

                    modView.ViewActivate(VIEW_WIRE)

                    modView.ViewRegen()

                    vgProplist.BeginDataUpdate()
                    vgProplist.SetCellValue(FocusedRow, vgProplist.FocusedRecord, m_exploer_Filename)
                    vgProplist.EndDataUpdate()

                    iname = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "DatVarName", "")

                    lReturn = UpdateAccumulatorDirect(vgProplist, iname, m_exploer_Filename)

                Case DB_MACHINE

                Case DB_TOOLSETUP
                Case DB_MATERIAL


            End Select

            modView.ViewRegen()

        Catch ex As Exception
            MessageBox.Show("rbPropSettings_ButtonClick: " & ex.Message)
        End Try

    End Sub

    Private Sub vgProplist_CellValueChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgProplist.CellValueChanged

        Dim iname As String
        Dim sOptions As String
        Dim nVarType As Integer
        Dim sValue As String
        Dim childrow As BaseRow
        Dim sParseTool As String
        Dim nInstring As Integer

        Try
            nVarType = CInt(GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "VarDataType", ""))
            iname = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "DatVarName", "")

            sOptions = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "Options", "")

            Dim sDatVarName As String

            sDatVarName = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "DatVarName", "")

            childrow = GetRowByCaptionChild(vgProplist.FocusedRow, vgProplist.FocusedRow.Properties.Caption)

            Select Case nVarType
                Case DB_DIR
                    If (e.Value = "CW") Then
                        sValue = -1
                    Else
                        sValue = 1
                    End If
                    '                    sValue = GetOptionIndex(sOptions, e.Value)

                Case DB_MACHINE
                    sValue = GetMachineIDFromDescrip(e.Value)

                Case DB_TOOLSETUP
                    sValue = GetToolSetupIDFromDescription(e.Value)

                Case DB_COMBO, DB_USERVAR, DB_SOLN
                    sValue = GetOptionIndex(sOptions, e.Value)


                Case DB_CRIB
                    nInstring = InStrRev(e.Value, "]")

                    sParseTool = Mid(e.Value, nInstring + 2)

                    sValue = FindModelIDFromToolDescrip(sParseTool)
                    g_clsActiveToolData = FindToolByModelID(CLng(sValue))
                    Name = GetEntityName(CLng(sValue))

                    SetActiveLayer(Name, g_clsActiveToolData.TlColor)

                Case DB_LEADSETUP
                    sValue = modTranslate.GetLeadSetupID(e.Value)

                Case DB_MATERIAL
                    sValue = modTranslate.GetMaterialIDFromDescription(e.Value)


                Case DB_DBL
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetDouble("val", 0)

                        childrow.Properties.Value = Round(sValue, Places.TenThousandths)
                    Else
                        sValue = e.Value
                    End If

                Case DB_INT, DB_ZONE

                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetInt("val", 0)

                        childrow.Properties.Value = sValue
                    Else
                        sValue = e.Value
                    End If
                Case DB_STR, DB_FILE
                    sValue = e.Value

                Case DB_TAN
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetInt("val", 0)

                        childrow.Properties.Value = sValue
                    End If



                Case Else
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetDouble("val", 0)

                        childrow.Properties.Value = FormatNumber(sValue, 4)
                    Else
                        sValue = e.Value
                    End If

            End Select

            lReturn = UpdateAccumulatorDirect(vgProplist, iname, sValue)
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Sub

    Private Sub vgProplist_CellValueChanging(sender As Object, e As Events.CellValueChangedEventArgs) Handles vgProplist.CellValueChanging
        Dim sDatVarName As String
        Dim nMatID As Long

        If (e.Row.Properties.Caption = "Material Type") Then

            nMatID = modTranslate.GetMaterialIDFromDescription(e.Value)

            Dim childRows As VGridRows = vgProplist.FocusedRow.ParentRow.ChildRows


            For Each childRow As EditorRow In vgProplist.FocusedRow.ParentRow.ChildRows

                sDatVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")
                Select Case UCase(sDatVarName)
                    Case "MAT_LEN"
                        childRow.Properties.Value = modMaterial.GetMaterialLength(nMatID)

                    Case "MAT_WID"
                        childRow.Properties.Value = modMaterial.GetMaterialWidth(nMatID)

                    Case "MAT_THICK"
                        childRow.Properties.Value = modMaterial.GetMaterialThickness(nMatID)

                End Select

            Next

        End If

    End Sub


    Private Sub vgProplist_CustomDrawRowHeaderCell(sender As Object, e As Events.CustomDrawRowHeaderCellEventArgs) Handles vgProplist.CustomDrawRowHeaderCell
        If vgProplist.FocusedRow Is e.Row Then
            e.Appearance.BackColor = Color.Gold
        End If
    End Sub

    Private Sub vgPropList_CustomRecordCellEdit(sender As Object, e As DevExpress.XtraVerticalGrid.Events.GetCustomRowCellEditEventArgs) Handles vgProplist.CustomRecordCellEdit

        Dim nTemp As Integer
        Try
            If (e.Row.Tag IsNot Nothing) Then
                nTemp = CInt(GetValueFromDelimitedData(e.Row.Tag, "VarDataType", 0))

                Select Case nTemp
                    Case DB_FILE
                        e.RepositoryItem = rbPropSettings

                    Case DB_FOLDER
                        e.RepositoryItem = rbPropSettings

                    Case DB_USERVAR

                    Case DB_MATLIST
                        e.RepositoryItem = rbMaterialSettings

                End Select
                If (nTemp = DB_FILE Or nTemp = DB_FOLDER) Then

                End If

            End If
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Sub
    Public Function Set_In_Edit_Mode(Mode As Boolean) As Long
        m_In_Edit_Mode = Mode
    End Function
    Private Sub btnAccept_Click(sender As Object, e As EventArgs) Handles btnAccept.Click

        'send the result back tp the form
        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        Me.vgProplist.FocusFirst()

        If (m_sncs = 2021 Or m_sncs = 2034 Or m_sncs = 9500 Or m_sncs = 500) Then
            SetTheAccumulator()
        End If

        lReturn = Execute()
        Set_In_Edit_Mode(False)
        modView.ViewRegen()

        Windows.Forms.Cursor.Current = Cursors.Default

    End Sub
    Private Sub SetTheAccumulator()
        Dim iname As String
        Dim sOptions As String
        Dim nVarType As Integer
        Dim sValue As String = Nothing

        Try

            For Each row As CategoryRow In vgProplist.Rows
                Dim childRows As VGridRows = row.ChildRows

                For Each childRow As EditorRow In childRows

                    nVarType = CInt(GetValueFromDelimitedData(childRow.Tag, "VarDataType", ""))
                    iname = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")

                    sOptions = GetValueFromDelimitedData(childRow.Tag, "Options", "")

                    Dim sDatVarName As String

                    sDatVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")

                    'childRow = GetRowByCaptionChild(vgProplist.FocusedRow, vgProplist.FocusedRow.Properties.Caption)

                    Select Case nVarType
                        Case DB_DIR
                            sValue = GetOptionIndex(sOptions, childRow.Properties.Value)

                        Case DB_MACHINE
                            sValue = GetMachineIDFromDescrip(childRow.Properties.Value)

                        Case DB_TOOLSETUP
                            sValue = GetToolSetupIDFromDescription(childRow.Properties.Value)

                        Case DB_COMBO, DB_USERVAR
                            sValue = GetOptionIndex(sOptions, childRow.Properties.Value)

                        Case DB_CRIB

                            sValue = FindModelIDFromToolDescrip(childRow.Properties.Value)
                            g_clsActiveToolData = FindToolByModelID(CLng(sValue))
                            Name = GetEntityName(CLng(sValue))

                            If (sValue > 0) Then
                                SetActiveLayer(Name, g_clsActiveToolData.TlColor)
                            End If
                        Case DB_LEADSETUP
                            sValue = modTranslate.GetLeadSetupID(childRow.Properties.Value)

                        Case DB_MATERIAL
                            sValue = modTranslate.GetMaterialIDFromDescription(childRow.Properties.Value)


                        Case DB_DBL
                            If (PortalExecute("*solve:eval:exp=" & QStr(childRow.Properties.Value)) > 0) Then

                                sValue = PortalGetDouble("val", 0)

                                childRow.Properties.Value = Round(sValue, Places.Millionths)

                            End If

                        Case DB_INT
                            If (PortalExecute("*solve:eval:exp=" & QStr(childRow.Properties.Value)) > 0) Then

                                sValue = PortalGetInt("val", 0)

                                childRow.Properties.Value = sValue

                            End If

                        Case Else
                            sValue = childRow.Properties.Value

                    End Select

                    lReturn = UpdateAccumulatorDirect(vgProplist, iname, sValue)
                Next
            Next

        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try


    End Sub

    Public Function Execute() As Long

        If (m_sncs < 2000) Then

            If (m_sncs = 1600) Then
                lReturn = modEdit.Execute(m_sub_item_sncs, GetAccumulator())
            Else
                lReturn = modGeometry.Execute(
                   m_sncs, m_entity_id, m_sub_item_sncs, GetAccumulator())
            End If

            lReturn = UpdateLastValues(m_sub_item_sncs)

        ElseIf ((m_sncs > 2000) And (m_sncs < 3000)) Then
            If (m_sncs = 2062) Then
                lReturn = modProcess.ExecPatternTextToCircle(GetAccumulator)
                lReturn = UpdateLastValues(m_sncs)
            Else
                lReturn = modEdit.Execute(m_sncs, GetAccumulator)
                lReturn = UpdateLastValues(m_sncs)
            End If
        End If

        modView.ViewRegen()


    End Function
    Private Function UpdateLastValues(sncs As Long) As Long
        Dim oledbAdapter As New OleDbDataAdapter
        Dim connection As OleDbConnection
        Dim connetionString As String
        Dim dsProps As New DataSet
        Dim sql As String
        Dim ChildRowTag As String
        Dim recordID As Long
        Dim sInternalName As String = Nothing
        Dim indx As Integer

        If (RegGetString("Preferences\ViewOptions", "SaveLastUsed", 1) <> 0) Then

            'This function updates the AppDb to store the last values used
            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathAppDB() & "';"

            connection = New OleDbConnection(connetionString)

            For Each row As CategoryRow In vgProplist.Rows

                For Each childRow As EditorRow In row.ChildRows

                    If (UCase(childRow.Properties.Caption) <> "ID") Then

                        ChildRowTag = childRow.Tag

                        'Using the record id which is stored in the tag value makes finding the right record real easy
                        recordID = GetValueFromDelimitedData(ChildRowTag, "nRecordID", 0)

                        'Check the sncs so that we look in the right table.
                        'If the value is between 2000 and 3000 we use the modify table
                        If ((sncs > 2000) And (sncs < 3000)) Then

                            sql = "SELECT * FROM ModifyVar WHERE (ID =" & recordID & ") And (ModifyVar.Type <> 0 )"

                            oledbAdapter = New OleDbDataAdapter(sql, connection)

                            oledbAdapter.Fill(dsProps)

                            oledbAdapter.Dispose()

                            If (dsProps.Tables(0).Rows.Count > 0) Then

                                For indx = 0 To dsProps.Tables(0).Rows.Count - 1
                                    sInternalName = dsProps.Tables(0).Rows(indx).Item("InternalName")
                                Next

                            End If

                            If ((sInternalName <> "STAN") And (sInternalName <> "NOP") And (sInternalName <> "ETAN")) Then

                                sql = "UPDATE [ModifyVar] set [ModifyVar].[Value] = '" & childRow.Properties.Value & "' Where ([ModifyVar].[ID] =" & recordID & " )"

                            End If

                        Else

                            sql = "SELECT * FROM DefnVar WHERE (ID = " & recordID & ") And (Type <> 0 )"

                            oledbAdapter = New OleDbDataAdapter(sql, connection)

                            oledbAdapter.Fill(dsProps)

                            oledbAdapter.Dispose()

                            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                            If (dsProps.Tables(0).Rows.Count > 0) Then

                                sInternalName = UCase(dsProps.Tables(0).Rows(indx).Item("InternalName"))

                            End If


                            If ((sInternalName <> "STAN") And (sInternalName <> "NOP") And (sInternalName <> "ETAN")) Then

                                sql = "UPDATE [DefnVar] set [DefnVar].[Value] = '" & childRow.Properties.Value & "' Where ([DefnVar].[ID] =" & recordID & " )"

                            End If

                        End If

                        connection.Open()

                        Dim cmd As New OleDbCommand(sql, connection)

                        cmd.ExecuteNonQuery()

                        connection.Close()


                    End If

                Next

            Next

            dsProps.Dispose()

            If connection IsNot Nothing Then connection = Nothing
            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing


        End If

        If dsProps IsNot Nothing Then dsProps = Nothing

    End Function

    Public Sub PopulateValues()

        Select Case m_sncs

            Case 2021 : lReturn = GetProjectInfo()


        End Select


    End Sub

    'Private Sub dlgEntity_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
    '    Me.Dispose()
    'End Sub

    Private Sub dlgEntity_KeyUp(sender As Object, e As KeyEventArgs) Handles Me.KeyUp
        ProcessKeyPress(e.KeyCode, e.Modifiers)
    End Sub


    Private Sub dlgEntity_Load(sender As Object, e As EventArgs) Handles Me.Load

        Dim materialID As Long
        Dim sMaterialTYpeAndSize As String


        m_is_active = True

        frmMain.SetCurrentDialog(Me)
        ' Uncheck all buttons
        'Dim button As ToolStripButton = Nothing
        'For Each obj As Object In frmMain.tbSelect.Items
        '    If TypeOf obj Is ToolStripButton Then
        '        button = DirectCast(obj, ToolStripButton)
        '        'button.Checked = False
        '        ' Patch to get rid of the faulty activated ToolTip background color
        '        'button.Visible = False
        '        'button.Visible = True
        '    End If
        'Next

        'CreateMaterialTypeNodes(tl)


        'popupContainerControl1.Controls.Add(tl)
        'popupContainerEdit1.Properties.PopupControl = popupContainerControl1
        'popupContainerEdit1.Properties.ShowPopupCloseButton = False
        'AddHandler popupContainerEdit1.QueryDisplayText, AddressOf popupContainerEdit1_QueryDisplayText


        'sMaterialTYpeAndSize = GetMaterialTypeName(GetMaterialType(materialID))
        'sMaterialTYpeAndSize = sMaterialTYpeAndSize & "," & GetMaterialDescription(materialID)

        'popupContainerEdit1.Text = sMaterialTYpeAndSize
        'popupContainerEdit1.Tag = TAG_MATERIAL_NODE & "," & materialID


        Me.ResumeLayout()
        'newForm.Show()
        'FilterSet(0)
        bIsEntityLoaded = True

    End Sub

    Private Function GetProjectInfo() As Long
        Dim toolSetupName As String
        Dim materialName As String
        Dim machineID As Long
        Dim smachineName As String
        Dim ToolSetupID As Long
        Dim materialID As Long
        Dim my_Value

        Try
            toolSetupName = HeaderStringGet("MachCfg", "")

            materialName = HeaderStringGet("MatCfg", "")

            machineID = modTranslate.GetMachineID(toolSetupName)
            smachineName = GetMachineDescription(machineID)
            ToolSetupID = modTranslate.GetToolSetupIDFromDescription(toolSetupName)
            materialID = modTranslate.GetMaterialIDFromDescription(materialName)

            my_Value = Nothing


            For Each row As CategoryRow In vgProplist.Rows

                If (row.Properties.Caption = "Tool Setup") Then

                    row.Properties.Caption = "Tool_Setup"

                End If

                For Each childRow As EditorRow In row.ChildRows

                    Select Case UCase(childRow.Properties.Caption)

                        Case "MACHINE"
                            my_Value = smachineName

                        Case "TOOL SETUP"
                            my_Value = toolSetupName

                        Case "UPDATE TOOLING"
                            my_Value = True

                        Case "CUSTOMER"
                            my_Value = HeaderStringGet("customer", "")

                        Case "ORDER OR P/O #"
                            my_Value = HeaderStringGet("PoNum", "")

                        Case "DUE DATE"
                            my_Value = HeaderStringGet("DueDate", "")

                        Case "MATERIAL TYPE"
                            my_Value = materialName

                        Case "MATERIAL LENGTH"
                            my_Value = Round(HeaderDoubleGet("Length", 0), Places.Thousandths, True)

                        Case "MATERIAL WIDTH"
                            my_Value = Round(HeaderDoubleGet("Width", 0), Places.Thousandths, True)

                        Case "MATERIAL THICKNESS"
                            my_Value = Round(HeaderDoubleGet("thickness", 0), Places.Thousandths, True)


                    End Select

                    vgProplist.BeginDataUpdate()
                    childRow.Properties.Value = my_Value
                    vgProplist.EndDataUpdate()

                Next childRow

            Next
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Function

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.
        rbPropSettings = New RepositoryItemButtonEdit()
        rbMaterialSettings = New RepositoryItemPopupContainerEdit()
    End Sub
    Private Class MoveToNextEditorRowOperation
        Inherits RowOperation
        Private nextCategory_Renamed As EditorRow
        Private currentRow As BaseRow
        Public Sub New(ByVal grid As VGridControl)
            MyBase.New()
            currentRow = grid.FocusedRow
        End Sub
        Public Overrides Sub Execute(ByVal row As BaseRow)
            If (IsNothing(currentRow) = False) Then
                If row.VisibleIndex > currentRow.VisibleIndex Then
                    If nextCategory_Renamed Is Nothing AndAlso TypeOf row Is EditorRow Then
                        nextCategory_Renamed = TryCast(row, EditorRow)
                    End If
                End If
            End If
        End Sub

        Public ReadOnly Property NextEditorRow() As EditorRow
            Get
                Return nextCategory_Renamed
            End Get
        End Property
    End Class

    Private Class MoveToNextCategoryOperation
        Inherits RowOperation
        Private nextCategory_Renamed As CategoryRow
        Private currentRow As BaseRow

        Public Sub New(ByVal grid As VGridControl)
            MyBase.New()
            currentRow = grid.FocusedRow
        End Sub
        Public Overrides Sub Execute(ByVal row As BaseRow)

            If (currentRow IsNot Nothing) Then
                If row.VisibleIndex > currentRow.VisibleIndex Then
                    If nextCategory_Renamed Is Nothing AndAlso TypeOf row Is CategoryRow Then
                        nextCategory_Renamed = TryCast(row, CategoryRow)
                    End If
                End If

            End If
        End Sub

        Public ReadOnly Property NextCategory() As CategoryRow
            Get
                Return nextCategory_Renamed
            End Get
        End Property
    End Class

    Private Sub cboDefn_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboDefn.SelectedIndexChanged
        Dim sql As String

        m_sub_item_sncs = (TryCast(cboDefn.SelectedItem, myItemData).ItemData.ToString())

        sql = "SELECT * FROM DefnVar WHERE (DefnID=" & m_sub_item_sncs & ")"
        lReturn = modPropList.InitFromDB(vgProplist, PathAppDB(), sql)

    End Sub
    Private Sub vgProplist_KeyUp(sender As Object, e As KeyEventArgs) Handles vgProplist.KeyUp


        If (e.KeyCode = Keys.Enter) Then
            If (vgProplist.FocusedRow Is vgProplist.GetLastVisible()) Then

                lReturn = Execute()
                Set_In_Edit_Mode(False)
                modView.ViewRegen()

                vgProplist.FocusedRow = vgProplist.Rows.FirstVisible

            Else
                System.Windows.Forms.SendKeys.Send("{TAB}") ' treat as a tab to calculate
            End If


        End If
    End Sub

    Private Sub vgProplist_MouseClick(sender As Object, e As MouseEventArgs) Handles vgProplist.MouseClick
        frmMain.tbSelect_Enable.Checked = False
        frmMain.tbSelect_ByWindow.Checked = False

        m_selection_mode = False
        'FilterSet(0)
        lReturn = ModeEnd(MODE_SELECT)
    End Sub

    Private Sub vgProplist_MouseDown(sender As Object, e As MouseEventArgs) Handles vgProplist.MouseDown

        frmMain.tbSelect_Enable.Checked = False
        'm_selection_mode = frmMain.tbSelect_Enable.Checked
        lReturn = ModeEnd(MODE_SELECT)
    End Sub

    Private Sub vgProplist_ShowingEditor(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles vgProplist.ShowingEditor
        BeginInvoke(New FocusTextHandler(AddressOf FocusText), sender)
    End Sub

    Private Sub vgProplist_ShownEditor(sender As Object, e As EventArgs) Handles vgProplist.ShownEditor
        Dim edit As ComboBoxEdit = TryCast(vgProplist.ActiveEditor, ComboBoxEdit)

        Dim indx As Integer
        Dim DB_TOOLSETUP As Integer = 8
        Dim DB_LAYERSETUP As Integer = 42
        Dim s_ToolSetup As String
        Dim n_ToolSetupID As Long



        Dim sOptions As String = sEmpty

        If (vgProplist.FocusedRow.Properties.Caption = "Tool Setup") Then ' IsNot Nothing Then

            'If vgProplist.FocusedRow Is nToolSetupRowId Then


            edit.Properties.Items.Clear()

            arrToolSetup = PopulateComboList("SELECT * FROM [Tool Setups] " &
                                               "WHERE ([Machine ID]=" & m_machine_id & ") ORDER BY [Description]", False, False)

            For indx = 1 To arrToolSetup.Count

                edit.Properties.Items.Add(New ComboBoxItem(arrToolSetup(indx).Description))

                If (indx = 1) Then
                    sOptions = arrToolSetup(indx).Description
                Else
                    sOptions = sOptions & ";" & arrToolSetup(indx).Description
                End If

            Next


            If (arrToolSetup.Count > 0) Then
                s_ToolSetup = arrToolSetup(1).description

                n_ToolSetupID = GetToolSetupIDFromDescription(s_ToolSetup)

                edit.EditValue = s_ToolSetup

                vgProplist.FocusedRow.Tag = EncodeTag("toolsetup",
                                                                 "Tool Setup",
                                                                 s_ToolSetup,
                                                                 n_ToolSetupID,
                                                                 "Tool Setup ID",
                                                                 DB_TOOLSETUP,
                                                                 0,
                                                                 sOptions, 0)
            End If
        End If


        If edit IsNot Nothing Then
            AddHandler edit.EditValueChanged, AddressOf edit_EditValueChanged
        End If
    End Sub

    Private Sub edit_EditValueChanged(sender As Object, e As EventArgs)
        Dim edit As ComboBoxEdit = TryCast(sender, ComboBoxEdit)
        Dim my_DefnID As Integer
        Dim childrow As BaseRow
        Dim nMaterialID As Long
        Dim my_Cvalue As String
        Dim nVarType As Integer


        Try
            nToolSetupRowId = Nothing

            editValue = edit.EditValue.ToString()

            nVarType = CInt(GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "VarDataType", ""))


            If (vgProplist.FocusedRow.Properties.Caption = "Machine") Then

                SelectedMachineName = edit.EditValue.ToString()
                m_machine_id = GetMachineIDFromDescrip(SelectedMachineName)


                nToolSetupRowId = GetRowByCaption("Tool Setup")

                If (IsNothing(nToolSetupRowId) = False) Then
                    nRowIndx = nToolSetupRowId.Index


                    vgProplist.FocusedRow = vgProplist.Rows(nToolSetupRowId.ParentRow.Index).ChildRows(nToolSetupRowId.Index)
                    vgProplist.FocusedRow.Properties.Value = ""
                    vgProplist.ShowEditor()
                End If
            End If

            If (vgProplist.FocusedRow.Properties.Caption = "Material Type") Then
                my_Cvalue = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "CValue", "")

                nMaterialID = CLng(GetMaterialIDFromDescription(edit.EditValue))

                childrow = GetRowByCaption("Material Length")

                vgProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialLength(CLng(nMaterialID))
                m_accumulator.mat_len = CDbl(modMaterial.GetMaterialLength(CLng(nMaterialID)))

                childrow = GetRowByCaption("Material Width")
                vgProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialWidth(CLng(nMaterialID))
                m_accumulator.mat_wid = CDbl(modMaterial.GetMaterialWidth(CLng(nMaterialID)))

                childrow = GetRowByCaption("Material thickness")
                vgProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialThickness(CLng(nMaterialID))
                m_accumulator.mat_thick = CDbl(modMaterial.GetMaterialThickness(CLng(nMaterialID)))

            End If

            my_DefnID = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "DefnID", 0)

            If (my_DefnID > 0 And nVarType = DB_USERVAR) Then


                lReturn = GetPropsForUserVar(vgProplist, vgProplist.FocusedRow, edit.SelectedIndex, my_DefnID)
                vgProplist.FocusedRow.Properties.Value = editValue
                m_accumulator.soln_Set(5, edit.SelectedIndex)

            End If


        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try


    End Sub

    Private Function GetRowByCaption(ByVal s As String) As DevExpress.XtraVerticalGrid.Rows.BaseRow
        Dim r As DevExpress.XtraVerticalGrid.Rows.BaseRow
        For Each row As DevExpress.XtraVerticalGrid.Rows.BaseRow In vgProplist.Rows
            r = GetRowByCaptionChild(row, s)
            If Not r Is Nothing Then
                Return r
            End If
        Next row
        Return Nothing
    End Function

    Private Function GetRowByCaptionChild(ByVal row As DevExpress.XtraVerticalGrid.Rows.BaseRow, ByVal s As String) As DevExpress.XtraVerticalGrid.Rows.BaseRow
        If row.Properties.Caption = s Then
            Return row
        End If
        Dim r As DevExpress.XtraVerticalGrid.Rows.BaseRow
        For Each cRow As DevExpress.XtraVerticalGrid.Rows.BaseRow In row.ChildRows
            r = GetRowByCaptionChild(cRow, s)
            If Not r Is Nothing Then
                Return r
            End If
        Next cRow
        Return Nothing
    End Function

    Public Function MoveToFirstRow() As Long
        Dim operation As New MoveToNextCategoryOperation(vgProplist)
        vgProplist.RowsIterator.DoOperation(operation)
        vgProplist.Focus()
        vgProplist.FocusedRow = vgProplist.Rows(0)
        vgProplist.LayoutChanged()
        MoveToFirstRow = 1

    End Function

    Private Sub FillEntityProps(lEntityID As Long)
        Dim lType As Long
        Dim DX As Double
        Dim DY As Double
        Dim dZ As Double
        Dim dx2 As Double
        Dim dy2 As Double
        Dim dz2 As Double
        Dim lTemp As Long
        Dim lToolID As Long
        Dim sTemp As String
        Dim ownerID As Long
        Dim sFeatureParams As String
        Dim sFeatureName As String
        Dim Angle As Double
        Dim position As Long
        Dim sAccumualtorName As String

        Try

            lReturn = PortalExecute("*create:extract:id=" & lEntityID) ' Getting other info about entity
            lType = PortalGetInt("type", c_INVALID) ' Type: Line, Arc, Hole, Point

            '' get any tooling or layer information
            If ((lType >= c_DBPOINT And lType <= c_DBHOLE) Or lType = c_DBCOMMAND) Then

                lReturn = PortalExecute("*Entity:Tool:id=" & lEntityID)
                lToolID = PortalGetInt("ID", 0)

                lReturn = UpdateAccumulatorDirect(vgProplist, "tool", lToolID)

            End If

            lReturn = PortalExecute("*create:extract:id=" & lEntityID) ' Getting other info about entity
            lType = PortalGetInt("type", c_INVALID) ' Type: Line, Arc, Hole, Point

            Select Case lType

                Case c_DBPOINT ' point
                    DX = PortalGetDouble("x", 0)
                    DY = PortalGetDouble("y", 0)
                    dZ = PortalGetDouble("z", 0)


                Case c_DBLINE ' line
                    DX = PortalGetDouble("sx", 0)
                    DY = PortalGetDouble("sy", 0)
                    dZ = PortalGetDouble("sz", 0)

                    dx2 = PortalGetDouble("ex", 0)
                    dy2 = PortalGetDouble("ey", 0)
                    dz2 = PortalGetDouble("ez", 0)

                    SetValue(vgProplist, "ps_x", Round(DX, Places.Millionths))
                    SetValue(vgProplist, "ps_y", Round(DY, Places.Millionths))
                    SetValue(vgProplist, "ps_z", Round(dZ, Places.Millionths))

                    SetValue(vgProplist, "pe_x", Round(dx2, Places.Millionths))
                    SetValue(vgProplist, "pe_y", Round(dy2, Places.Millionths))
                    SetValue(vgProplist, "pe_z", Round(dz2, Places.Millionths))


                Case c_DBARC ' arc
                    ' cboDefn.ListIndex = 4

                    DX = PortalGetDouble("sx", 0)
                    DY = PortalGetDouble("sy", 0)
                    dZ = PortalGetDouble("sz", 0)

                    SetValue(vgProplist, "ps_x", Round(DX, Places.Millionths))
                    SetValue(vgProplist, "ps_y", Round(DY, Places.Millionths))
                    SetValue(vgProplist, "ps_z", Round(dZ, Places.Millionths))

                    DX = PortalGetDouble("ex", 0)
                    DY = PortalGetDouble("ey", 0)
                    dZ = PortalGetDouble("ez", 0)

                    SetValue(vgProplist, "pe_x", Round(DX, Places.Millionths))
                    SetValue(vgProplist, "pe_y", Round(DY, Places.Millionths))
                    SetValue(vgProplist, "pe_z", Round(dZ, Places.Millionths))

                    dx2 = PortalGetDouble("cx", 0)
                    dy2 = PortalGetDouble("cy", 0)
                    dz2 = PortalGetDouble("cz", 0)

                    SetValue(vgProplist, "pc_x", Round(dx2, Places.Millionths))
                    SetValue(vgProplist, "pc_y", Round(dy2, Places.Millionths))
                    SetValue(vgProplist, "pc_z", Round(dz2, Places.Millionths))

                    lTemp = PortalGetInt("dir", 0)

                    SetValue(vgProplist, "dir", IIf((lTemp = -1), 0, 1))


                Case c_DBHOLE ' hole
                    ownerID = GetOwnerID(lEntityID)
                    'TODO: Make the true and false values in the function string 0 and 1

                    sFeatureParams = EntityStringGet(ownerID, "function", "")
                    lReturn = vbParse(sFeatureParams)

                    sFeatureName = PortalGetString("function", "")
                    lTemp = PortalGetInt("sncs", 0)

                Case c_DBCOMMAND ' command

                    DX = PortalGetDouble("x", 0.0#)
                    DY = PortalGetDouble("y", 0.0#)
                    dZ = PortalGetDouble("z", 0.0#)

                    SetValue(vgProplist, "ps_x", Round(DX, Places.Millionths))
                    SetValue(vgProplist, "ps_y", Round(DY, Places.Millionths))
                    SetValue(vgProplist, "ps_z", Round(dZ, Places.Millionths))

                    sTemp = PortalGetString("text", "")
                    If InStr(1, sTemp, "INSTANCE") > 0 Then

                        '???
                        'Not sure what we should do if it is an instance

                    Else

                        Angle = PortalGetDouble("angle", 0.0#)
                        position = IIf((PortalGetInt("pos", 2) = 1), 1, 0)
                        'sTemp = Left(sTemp, InStr(1, sTemp, Chr$(0)) - 1)
                        SetValue(vgProplist, "text", sTemp)
                        SetValue(vgProplist, "soln(0)", position)
                        SetValue(vgProplist, "angle", Angle)

                    End If

            End Select

            For Each row As CategoryRow In vgProplist.Rows
                Dim childRows As VGridRows = row.ChildRows

                For Each childRow As EditorRow In childRows

                    sAccumualtorName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")

                    Select Case sAccumualtorName
                        Case "ps_x" : childRow.Properties.Value = m_accumulator.ps.x
                        Case "ps_y" : childRow.Properties.Value = m_accumulator.ps.y
                        Case "ps_z" : childRow.Properties.Value = m_accumulator.ps.Z
                        Case "ps"
                        Case "ps_o"
                        Case "ps_osnap"
                        Case "ps_entityID"

                        Case "pe"
                        Case "pe_x" : childRow.Properties.Value = m_accumulator.pe.x
                        Case "pe_y" : childRow.Properties.Value = m_accumulator.pe.y
                        Case "pe_z" : childRow.Properties.Value = m_accumulator.pe.Z
                        Case "pe_o"
                        Case "pe_osnap"
                        Case "pe_entityID"

                        Case "pc"
                        Case "pc_x" : childRow.Properties.Value = m_accumulator.pc.x
                        Case "pc_y" : childRow.Properties.Value = m_accumulator.pc.y
                        Case "pc_z" : childRow.Properties.Value = m_accumulator.pc.Z
                        Case "pc_o"
                        Case "pc_osnap"
                        Case "pc_entityID"

                        Case "pi"
                        Case "pi_x" : childRow.Properties.Value = m_accumulator.PI.x
                        Case "pi_y" : childRow.Properties.Value = m_accumulator.PI.y
                        Case "pi_z" : childRow.Properties.Value = m_accumulator.PI.Z
                        Case "pi_o"
                        Case "pi_osnap"
                        Case "pi_entityID"

                        Case "length" : childRow.Properties.Value = m_accumulator.Length
                        Case "radius" : childRow.Properties.Value = m_accumulator.Rad
                        Case "angle" : childRow.Properties.Value = m_accumulator.Ang
                        Case "angincr" : childRow.Properties.Value = m_accumulator.angincr
                        Case "toolang" : childRow.Properties.Value = m_accumulator.toolang
                        Case "major" : childRow.Properties.Value = m_accumulator.major
                        Case "minor" : childRow.Properties.Value = m_accumulator.minor

                            'This set of parameters was introduced for modEdit.
                        Case "dx" : childRow.Properties.Value = m_accumulator.DX
                        Case "dy" : childRow.Properties.Value = m_accumulator.DY
                        Case "dz" : childRow.Properties.Value = m_accumulator.dZ
                            'Case "dist" : childRow.Properties.Value = lReturn : childRow.Properties.Value = m_accumulator.dist_Set(subIndex, CDbl(Value))
                        Case "factor" : childRow.Properties.Value = m_accumulator.factor
                        Case "nx" : childRow.Properties.Value = m_accumulator.nx
                        Case "ny" : childRow.Properties.Value = m_accumulator.ny
                        Case "cnt" : childRow.Properties.Value = m_accumulator.cnt

                        Case "etan" : childRow.Properties.Value = m_accumulator.etan
                        Case "stan" : childRow.Properties.Value = m_accumulator.stan

                        Case "dir" : childRow.Properties.Value = m_accumulator.dir
                        Case "soln"
                            childRow.Properties.Value = m_accumulator.soln_Set(childRow.Properties.Value, 0)
                            '                lReturn : childRow.Properties.Value = m_accumulator.soln_choice : childRow.Properties.Value = subindex
                            '
                            ' Case "soln" : childRow.Properties.Value = lReturn : childRow.Properties.Value = m_accumulator.soln_Set(subIndex, CLng(Value))
                            'lReturn : childRow.Properties.Value = m_accumulator.soln_choice : childRow.Properties.Value = subindex


                        Case "qty" : childRow.Properties.Value = m_accumulator.qty
                        Case "qty_x" : childRow.Properties.Value = m_accumulator.qty_x
                        Case "qty_y" : childRow.Properties.Value = m_accumulator.qty_y

                        Case "rptdir" : childRow.Properties.Value = m_accumulator.rptdir
                        Case "rptcnt" : childRow.Properties.Value = m_accumulator.rptcnt
                        Case "rptdst" : childRow.Properties.Value = m_accumulator.rptdst

                        Case "spacing" : childRow.Properties.Value = m_accumulator.spacing
                        Case "spacing_x" : childRow.Properties.Value = m_accumulator.spacing_x
                        Case "spacing_y" : childRow.Properties.Value = m_accumulator.spacing_y

                        Case "tool" : childRow.Properties.Value = m_accumulator.tool

                        Case "font" : childRow.Properties.Value = m_accumulator.Font
                        Case "text" : childRow.Properties.Value = m_accumulator.Text
                        Case "height" : childRow.Properties.Value = m_accumulator.Height
                        Case "justification" : childRow.Properties.Value = m_accumulator.justification
                        Case "filename" : childRow.Properties.Value = m_accumulator.Filename
                        Case "file" : childRow.Properties.Value = m_accumulator.Filename
                        Case "machine" : childRow.Properties.Value = m_accumulator.machine
                        Case "toolsetup" : childRow.Properties.Value = m_accumulator.ToolSetupID
                        Case "materialtype"

                            childRow.Properties.Value = SetValue(vgProplist, "mat_len", modMaterial.GetMaterialLength(CLng(m_accumulator.materialType)))
                            childRow.Properties.Value = SetValue(vgProplist, "mat_wid", modMaterial.GetMaterialWidth(CLng(m_accumulator.materialType)))
                            childRow.Properties.Value = SetValue(vgProplist, "mat_thick", modMaterial.GetMaterialThickness(CLng(m_accumulator.materialType)))

                        Case "mat_len" : childRow.Properties.Value = m_accumulator.mat_len
                        Case "mat_wid" : childRow.Properties.Value = m_accumulator.mat_wid
                        Case "mat_thick" : childRow.Properties.Value = m_accumulator.mat_thick
                        Case "datepick" : childRow.Properties.Value = m_accumulator.datepick
                        Case "customer" : childRow.Properties.Value = m_accumulator.customer
                        Case "num_clamps" : childRow.Properties.Value = m_accumulator.num_clamps
                        Case "clamp1" : childRow.Properties.Value = m_accumulator.clamp1
                        Case "clamp2" : childRow.Properties.Value = m_accumulator.clamp2
                        Case "clamp3" : childRow.Properties.Value = m_accumulator.clamp3
                        Case "clamp4" : childRow.Properties.Value = m_accumulator.clamp4
                        Case "folder" : childRow.Properties.Value = m_accumulator.folder
                        Case "leadsetup" : childRow.Properties.Value = m_accumulator.leadSetupID
                        Case "layersetup" : childRow.Properties.Value = m_accumulator.layersetupID
                        Case "duedate" : childRow.Properties.Value = m_accumulator.duedate


                        Case "sql_table_name" : childRow.Properties.Value = m_accumulator.sql_table_name

                        Case "pdb_DB_name" : childRow.Properties.Value = m_accumulator.pdb_DB_name

                            'Added the following for Shape Library

                        Case "CornerRad" : childRow.Properties.Value = m_accumulator.CornerRad
                        Case "Orient" : childRow.Properties.Value = m_accumulator.Orient
                        Case "ProfLen" : childRow.Properties.Value = m_accumulator.ProfLen
                        Case "ProfWid" : childRow.Properties.Value = m_accumulator.ProfWid
                        Case "CenRad" : childRow.Properties.Value = m_accumulator.CenRad
                        Case "CenPt_x" : childRow.Properties.Value = m_accumulator.CenPt.x
                        Case "CenPt_y" : childRow.Properties.Value = m_accumulator.CenPt.y
                        Case "RefPt_x" : childRow.Properties.Value = m_accumulator.RefPt.x
                        Case "RefPt_y" : childRow.Properties.Value = m_accumulator.RefPt.y
                        Case "HoleDia" : childRow.Properties.Value = m_accumulator.HoleDia
                        Case "MajorDia" : childRow.Properties.Value = m_accumulator.MajorDia

                        Case "CornerRad2" : childRow.Properties.Value = m_accumulator.CornerRad2
                        Case "Orient2" : childRow.Properties.Value = m_accumulator.Orient2
                        Case "ProfLen2" : childRow.Properties.Value = m_accumulator.ProfLen2
                        Case "ProfWid2" : childRow.Properties.Value = m_accumulator.ProfWid2
                        Case "CenRad2" : childRow.Properties.Value = m_accumulator.CenRad2
                        Case "CenPt2_x" : childRow.Properties.Value = m_accumulator.CenPt2.x
                        Case "CenPt2_y" : childRow.Properties.Value = m_accumulator.CenPt2.y
                        Case "RefPt2_x" : childRow.Properties.Value = m_accumulator.RefPt2.x
                        Case "RefPt2_y" : childRow.Properties.Value = m_accumulator.RefPt2.y
                        Case "ps1_x" : childRow.Properties.Value = m_accumulator.ps1.x
                        Case "ps1_y" : childRow.Properties.Value = m_accumulator.ps1.y
                        Case "ps2_x" : childRow.Properties.Value = m_accumulator.ps2.x
                        Case "ps2_y" : childRow.Properties.Value = m_accumulator.ps2.y
                        Case "ps3_x" : childRow.Properties.Value = m_accumulator.ps3.x
                        Case "ps3_y" : childRow.Properties.Value = m_accumulator.ps3.y
                        Case "ps4_x" : childRow.Properties.Value = m_accumulator.ps4.x
                        Case "ps4_y" : childRow.Properties.Value = m_accumulator.ps4.y
                        Case "ps5_x" : childRow.Properties.Value = m_accumulator.ps5.x
                        Case "ps5_y" : childRow.Properties.Value = m_accumulator.ps5.y
                        Case "ps6_x" : childRow.Properties.Value = m_accumulator.ps6.x
                        Case "ps6_y" : childRow.Properties.Value = m_accumulator.ps6.y
                        Case "ps7_x" : childRow.Properties.Value = m_accumulator.ps7.x
                        Case "ps7_y" : childRow.Properties.Value = m_accumulator.ps7.y
                        Case "ps8_x" : childRow.Properties.Value = m_accumulator.ps8.x
                        Case "ps8_y" : childRow.Properties.Value = m_accumulator.ps8.y


                    End Select


                Next
            Next

        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Sub

    Private Sub dlgEntity_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        bIsEntityLoaded = False
    End Sub

    Private Sub rbMaterialSettings_ButtonClick(sender As Object, e As ButtonPressedEventArgs) Handles rbMaterialSettings.ButtonClick
        'MessageBox.Show("We got here")
    End Sub

    Private Sub CreateMaterialTypeNodes(ByVal tl As DevExpress.XtraTreeList.TreeList)
        Dim sql As String
        Dim ds As New System.Data.DataSet

        Dim s_MaterialCategory As String
        Dim n_MaterialID As Integer
        Dim i As Integer
        Dim dtMaterialType As System.Data.DataTable


        tl.BeginUnboundLoad()

        tl.ClearNodes()

        CFG_ROOT_NODE = Nothing


        CFG_MATERIAL_ROOT_NODE = tl.AppendNode(New Object() {"Materials"}, CFG_ROOT_NODE, TAG_MATERIAL_ROOT_NODE)

        s_MaterialCategory = ""

        sql = "Select [Material Types].* From [Material Types]"

        Try

            dtMaterialType = ConnectToCMDB(sql)


            For i = 0 To dtMaterialType.Rows.Count - 1
                s_MaterialCategory = dtMaterialType.Rows(i).Item("Description")

                If InStr(UCase(s_MaterialCategory), UCase(sUsage)) Then
                    n_MaterialID = dtMaterialType.Rows(i).Item("ID")

                    CFG_MATERIAL_CATEGORY_NODE = tl.AppendNode(New Object() {s_MaterialCategory}, CFG_MATERIAL_ROOT_NODE, TAG_MATERIAL_CATEGORY_NODE)

                    FillMaterialNodes(n_MaterialID)

                    dtMaterialType.Dispose()
                End If

            Next

            tl.EndUnboundLoad()



        Catch ex As Exception

            MessageBox.Show("CreateMaterialTypeNodes: " & ex.Message)

        End Try

    End Sub

    Private Sub popupContainerEdit1_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs) Handles popupContainerEdit1.QueryDisplayText
        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")

    End Sub

    Private Sub FillMaterialNodes(n_MaterialID As Long)
        Dim sql As String
        Dim ds As New System.Data.DataSet
        Dim s_MaterialDescription As String
        Dim i As Integer
        Dim dtMaterialNode As System.Data.DataTable

        s_MaterialDescription = ""

        sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & n_MaterialID & " )"

        Try

            dtMaterialNode = ConnectToCMDB(sql)

            For i = 0 To dtMaterialNode.Rows.Count - 1
                s_MaterialDescription = dtMaterialNode.Rows(i).Item("Description")

                n_MaterialID = dtMaterialNode.Rows(i).Item("ID")

                CFG_MATERIAL_NODE = tl.AppendNode(New Object() {s_MaterialDescription}, CFG_MATERIAL_CATEGORY_NODE, TAG_MATERIAL_NODE & "," & n_MaterialID)

            Next
            tl.ExpandAll()

            dtMaterialNode.Dispose()

        Catch ex As Exception

            MessageBox.Show("FillMaterialNodes: " & ex.Message)

        End Try

    End Sub

    Private Sub dlgEntity_Resize(sender As Object, e As EventArgs) Handles Me.Resize
        dlgEntityConvertedLayout.Width = Me.Width - 10
        'If Me.Width > 295 Then
        '    Me.Width = 295
        'End If
    End Sub

    Private Function MacToPortal() As Boolean
        Dim cmd As String


        Try
            Windows.Forms.Cursor.Current = Cursors.WaitCursor

            UndoPrepare()

            PrepareVars()

            sReturn = PathMacroSet("9500")
            cmd = "admin:macrorun: javafile=" & QStr(App_Path & "\macros\SL_Global.java")

            'Lets be sure thera are no empty features lying around
            lReturn = PortalExecute("create:empty:") ' get rid of empty feature


            frmMain.PublicPatternComboShow()
            frmMain.SynchronizeUI(modSelection.SelectedCount())

            Windows.Forms.Cursor.Current = Cursors.Default
            UndoCommit()

        Catch ex As Exception
            MessageBox.Show("MacToPortal_Click: " & ex.Message)
        End Try

    End Function

    Private Function dbProfile() As Object
        Throw New NotImplementedException()
    End Function

    Private Function MasterCheck() As Boolean

        Try

            Dim b_Loop As Boolean = True
            Dim b_Loop2 As Boolean = True
            Dim b_Loop3 As Boolean = True
            Dim value As Double
            MasterCheck = False

            m_NextPoint = 0

            'm_Radius = accumulator.Rad

            'Get the  count from the Selected Profile.
            'This    routine will only work on profilew
            cnt = modSelection.SelectedCount

            'Loop through each entity in the profile
            For indx = 1 To cnt

                'Get the entity ID for the entity at the "indx" position in the selection
                lSelectionID = modSelection.GetAt(indx - 1)

                'Get the iiproperties of the entity
                lReturn = PortalExecute("*create:extract:id=" & lSelectionID)

                'Get the geometry type to make sure we are working with a profile
                lType = PortalGetInt("type", c_INVALID)

                dir = PortalGetInt("dir", 1)

                Select Case lType

                    'For now we only work with selected profiles
                    Case c_DBPROFILE

                        'We have a Profile so we can continue.
                        containerID = lSelectionID

                        'get the number of entities in th profile
                        lReturn = PortalExecute("*Entity:Container:Count:id=" & containerID)

                        'Loop through each entity within the profile.
                        Count = PortalGetInt("count", 0)

                        firstloop = True

                        pntIndx = Count - 1

                        'Loop through each entity within the selected profile.
                        For indx2 = Count To 0 Step -1

                            Dim m_StartPts(3) As Point3D
                            Dim m_EndPts(3) As Point3D

                            'let's get some dat from the nth entity in the profile specified by "indx2"
                            lReturn = PortalExecute("*Entity:Container:Get:" & " id=" & containerID & ",index=" & indx2)

                            'Set the entity ID 
                            entityID = PortalGetInt("id", 0)


                            '.WriteLine("The Real ID" & entityID)
                            'Now w have the entity, so let's get some infr from it.
                            lReturn = PortalExecute("*create:extract:id=" & entityID)

                            'see what type of entity is within the profile at "indx2"
                            ProfEntTYpe = PortalGetInt("type", c_INVALID)

                            'right now we are only dealing with lines, so we check the type
                            If (ProfEntTYpe = c_DBLINE) Then

                                ListBox1.Items.Add(entityID)

                                If b_Loop = True Then

                                    CreateFirstPointOnCorner(entityID)
                                    b_Loop = False
                                    GoTo Jumps

                                End If

                                If b_Loop2 = True Then
                                    CreateSecondPointOnCorner(entityID, 0.3)

                                    b_Loop2 = False

                                    GoTo Jumps
                                End If


                                If b_Loop3 = True Then
                                    CreateThirdPointOnCorner(entityID)

                                    b_Loop3 = False

                                    GoTo Jumps
                                End If


                                ''objWriter.WriteLine("*Point: " & (indx2 + 1))


                                'Select Case m_NextPoint
                                '    Case 1

                                '        pt1S.X = PortalGetDouble("sx", 0)
                                '        pt1S.Y = PortalGetDouble("sy", 0)
                                '        pt1S.Z = PortalGetDouble("sz", 0)
                                '        m_myPT.dx = pt1S.X
                                '        m_myPT.dy = pt1S.Y

                                '        Pnt1E.X = PortalGetDouble("ex", 0)
                                '        Pnt1E.Y = PortalGetDouble("ey", 0)
                                '        Pnt1E.Z = PortalGetDouble("ez", 0)

                                '        m_EntityID_1 = entityID

                                '        'objWriter.WriteLine("Case 1 Entity 1 ID " & m_EntityID_1 & "No Entity 2 id ")

                                '        'objWriter.WriteLine("  ")
                                '        'objWriter.WriteLine("*Point: " & (indx2 + 1))
                                '        'objWriter.WriteLine("Case 1")
                                '        'objWriter.WriteLine("  ")
                                '        ' 'objWriter.WriteLine(pt1S.X & "," & pt1S.Y & "," & pt1S.Z)
                                '        ' 'objWriter.WriteLine(Pnt1E.X & "," & Pnt1E.Y & "," & Pnt1E.Z)
                                '        'objWriter.WriteLine("  ")

                                '    Case 2

                                '        Dim Pnt2E As New Point3D
                                '        Dim pt2S As New Point3D

                                '        m_EntityID_2 = entityID

                                '        'objWriter.WriteLine("Case 2 Entity 1 ID " & m_EntityID_1 & " Entity 2 id " & m_EntityID_2)

                                '        pt2S.X = PortalGetDouble("sx", 0)
                                '        pt2S.Y = PortalGetDouble("sy", 0)
                                '        pt2S.Z = PortalGetDouble("sz", 0)
                                '        Pnt2E.X = PortalGetDouble("ex", 0)
                                '        Pnt2E.Y = PortalGetDouble("ey", 0)
                                '        Pnt2E.Z = PortalGetDouble("ez", 0)

                                '        lReturn = PortalExecute(
                                '                            "Solve:Blend:" &
                                '                            " idA=" & m_EntityID_1 &
                                '                            ",idB =" & m_EntityID_2 &
                                '                            ",rad = " & m_Radius)

                                '        dSX = PortalGetDouble("sx", UNDEFINED)
                                '        dSY = PortalGetDouble("sy", UNDEFINED)
                                '        dCX = PortalGetDouble("cx", UNDEFINED)
                                '        dCY = PortalGetDouble("cy", UNDEFINED)
                                '        dEX = PortalGetDouble("ex", UNDEFINED)
                                '        dEY = PortalGetDouble("ey", UNDEFINED)

                                '        lDir = PortalGetInt("dir", 0)

                                'Select Case m_NextPoint

                                '    Case 3
                                'GoTo Jumps
                                Dim Pnt3E As New Point3D
                                Dim pt3S As New Point3D

                                m_EntityID_2 = m_EntityID_3 + 3

                                ''objWriter.WriteLine("Case 3 Entity 1 ID " & m_EntityID_2 & " Entity 2 id " & m_EntityID_3)
                                lReturn = PortalExecute(
                                            "Solve:Blend:" &
                                            " idA=" & m_EntityID_2 &
                                            ",idB =" & m_EntityID_3 &
                                            ",rad = " & m_Radius)

                                dSX = PortalGetDouble("sx", UNDEFINED)
                                dSY = PortalGetDouble("sy", UNDEFINED)
                                dCX = PortalGetDouble("cx", UNDEFINED)
                                dCY = PortalGetDouble("cy", UNDEFINED)
                                dEX = PortalGetDouble("ex", UNDEFINED)
                                dEY = PortalGetDouble("ey", UNDEFINED)

                                lDir = PortalGetInt("dir", 0)

                                'objWriter.WriteLine("@Point: " & (indx2 + 1))

                                pt3S.X = PortalGetDouble("sx", 0)
                                pt3S.Y = PortalGetDouble("sy", 0)

                                pt3S.Z = PortalGetDouble("sz", 0)
                                Pnt3E.X = PortalGetDouble("ex", 0)
                                Pnt3E.Y = PortalGetDouble("ey", 0)
                                Pnt3E.Z = PortalGetDouble("ez", 0)
                                value = Round(PortalGetDouble("ang", 0) * RAD2DEG)

                                'Get ready for the next set of points.
                                AX = pt1S.X
                                AY = pt1S.Y
                                BX = Pnt1E.X
                                BY = Pnt1E.Y
                                CX = Pnt3E.X
                                CY = Pnt3E.Y

                                vector_ang = VecGetAngle(AX, AY, BX, BY, CX, CY)

                                Dim dTemp As Double = Radians2Degrees(ATan2(BY - AY, BX - AX))

                                If (vector_ang > 0) Then

                                    'objWriter.WriteLine("The ARC Dir is < CCW > end1 is <0> and end 2 is <1>")
                                    'objWriter.WriteLine("The vector_ang is < " & (vector_ang * RAD2DEG) & " > (Actual ANG<" & dTemp & " >)")
                                    nArcDir = 1

                                    PortalExecute(" profile:blend: id1=" & m_EntityID_2 & ",end1=0,x1=" & dSX &
                                                  ",y1=" & dSY & ",id2=" & m_EntityID_3 & ",end2=1,x2=" & dEX &
                                                  ",y2=" & dEY & ",cx=" & dCX & " ,cy=" & dCY & ",dir=" & nArcDir)

                                ElseIf (vector_ang < 0) Then

                                    'objWriter.WriteLine("The ARC Dir is < CW > end1 is <1> and end 2 is <0>")
                                    'objWriter.WriteLine("The vector_ang is < " & (vector_ang * RAD2DEG) & " > (Actual ANG<" & dTemp & " >)")
                                    nArcDir = 1

                                    PortalExecute(" profile:blend: id1=" & m_EntityID_2 & ",end1=0,x1=" & dSX &
                                                  ",y1=" & dSY & ",id2=" & m_EntityID_3 & ",end2=1,x2=" & dEX &
                                                  ",y2=" & dEY & ",cx=" & dCX & " ,cy=" & dCY & ",dir=" & nArcDir)

                                ElseIf vector_ang = 0 Or vector_ang > 1 Then
                                    nArcDir = -1
                                    'objWriter.WriteLine("The vector_ang is < " & (vector_ang * RAD2DEG) & " > (Actual ANG<" & dTemp & " >)")

                                    PortalExecute(" profile:blend: id1=" & m_EntityID_2 & ",end1=0,x1=" & dSX &
                                                  ",y1=" & dSY & ",id2=" & m_EntityID_3 & ",end2=1,x2=" & dEX &
                                                  ",y2=" & dEY & ",cx=" & dCX & " ,cy=" & dCY & ",dir=" & nArcDir)


                                Else
                                    ' MessageBox.Show("HOLD DA BUS")
                                End If

                                ' (b_Loop = True) Then

                                'PortalExecute(" profile:blend: id1=" & m_EntityID_2 & ",end1=0,x1=" & dSX & ",y1=" & dSY & ",id2=" & m_EntityID_3 & ",end2=1,x2=" & dEX & ",y2=" & dEY & ",cx=" & dCX & " ,cy=" & dCY & ",dir=" & nArcDir)
                                'objWriter.WriteLine(" profile:blend: id1=" & m_EntityID_2 & ",end1=0,x1=" & dSX & ",y1=" & dSY & ",id2=" & m_EntityID_3 & ",end2=1,x2=" & dEX & ",y2=" & dEY & ",cx=" & dCX & " ,cy=" & dCY & ",dir=" & nArcDir)

                                b_Loop = False


                                'objWriter.WriteLine("  ")

                                m_EntityID_3 = entityID

                            End If

Jumps:

                            pntIndx = pntIndx - 1

                            m_NextPoint = m_NextPoint + 1

                        Next

                End Select
            Next

            MasterCheck = True

            'objWriter.Close()

        Catch ex As Exception

            MessageBox.Show("MasterCheck < " & ex.Message & " >")

        End Try

    End Function


    'Public Sub ExecProfileBlend(m_rad)

    '    UndoPrepare()
    '    MacToPortal()
    '    MasterCheck()

    '    UndoCommit()

    '    frmMain.SynchronizeUI(modSelection.SelectedCount())

    '    If (bIsEntityLoaded = True) Then frmEntityList.FillTreeView()

    'End Sub

    Sub CreateFirstPointOnCorner(entityID As Long)

        pt1S.X = PortalGetDouble("sx", 0)
        pt1S.Y = PortalGetDouble("sy", 0)
        pt1S.Z = PortalGetDouble("sz", 0)
        m_myPT.dx = pt1S.X
        m_myPT.dy = pt1S.Y

        Pnt1E.X = PortalGetDouble("ex", 0)
        Pnt1E.Y = PortalGetDouble("ey", 0)
        Pnt1E.Z = PortalGetDouble("ez", 0)

        m_EntityID_1 = entityID

        'objWriter.WriteLine("Case 1 Entity 1 ID " & m_EntityID_1 & "No Entity 2 id ")

        'objWriter.WriteLine("  ")
        'objWriter.WriteLine("*Point: " & (indx2 + 1))
        'objWriter.WriteLine("Case 1")
        'objWriter.WriteLine("  ")
        ' 'objWriter.WriteLine(pt1S.X & "," & pt1S.Y & "," & pt1S.Z)
        ' 'objWriter.WriteLine(Pnt1E.X & "," & Pnt1E.Y & "," & Pnt1E.Z)
        'objWriter.WriteLine("  ")

    End Sub

    Function CreateSecondPointOnCorner(entityID As Long, m_Rad As Double)
        Dim Pnt2E As New Point3D
        Dim pt2S As New Point3D

        m_Radius = m_Rad

        m_EntityID_2 = entityID

        'objWriter.WriteLine("Case 2 Entity 1 ID " & m_EntityID_1 & " Entity 2 id " & m_EntityID_2)

        pt2S.X = PortalGetDouble("sx", 0)
        pt2S.Y = PortalGetDouble("sy", 0)
        pt2S.Z = PortalGetDouble("sz", 0)
        Pnt2E.X = PortalGetDouble("ex", 0)
        Pnt2E.Y = PortalGetDouble("ey", 0)
        Pnt2E.Z = PortalGetDouble("ez", 0)

        lReturn = PortalExecute(
                            "Solve:Blend:" &
                            " idA=" & m_EntityID_1 &
                            ",idB =" & m_EntityID_2 &
                            ",rad = " & m_Radius)

        dSX = PortalGetDouble("sx", UNDEFINED)
        dSY = PortalGetDouble("sy", UNDEFINED)
        dCX = PortalGetDouble("cx", UNDEFINED)
        dCY = PortalGetDouble("cy", UNDEFINED)
        dEX = PortalGetDouble("ex", UNDEFINED)
        dEY = PortalGetDouble("ey", UNDEFINED)

        lDir = PortalGetInt("dir", 0)
    End Function

    Function CreateThirdPointOnCorner(entityID As Long)

        Dim Pnt3E As New Point3D
        Dim pt3S As New Point3D

        'objWriter.WriteLine("Case 3 Entity 1 ID " & m_EntityID_1 & " Entity 2 id " & m_EntityID_2)
        lReturn = PortalExecute(
                                                            "Solve:Blend:" &
                                                            " idA=" & m_EntityID_1 &
                                                            ",idB =" & m_EntityID_2 &
                                                            ",rad = " & m_Radius)

        dSX = PortalGetDouble("sx", UNDEFINED)
        dSY = PortalGetDouble("sy", UNDEFINED)
        dCX = PortalGetDouble("cx", UNDEFINED)
        dCY = PortalGetDouble("cy", UNDEFINED)
        dEX = PortalGetDouble("ex", UNDEFINED)
        dEY = PortalGetDouble("ey", UNDEFINED)

        lDir = PortalGetInt("dir", 0)

        'objWriter.WriteLine("@Point: " & (indx2 + 1))

        pt3S.X = PortalGetDouble("sx", 0)
        pt3S.Y = PortalGetDouble("sy", 0)

        pt3S.Z = PortalGetDouble("sz", 0)
        Pnt3E.X = PortalGetDouble("ex", 0)
        Pnt3E.Y = PortalGetDouble("ey", 0)
        Pnt3E.Z = PortalGetDouble("ez", 0)

        'Get ready for the next set of points.
        AX = pt1S.X
        AY = pt1S.Y
        BX = Pnt1E.X
        BY = Pnt1E.Y
        CX = Pnt3E.X
        CY = Pnt3E.Y

        vector_ang = VecGetAngle(AX, AY, BX, BY, CX, CY)
        Dim dTemp As Double = Radians2Degrees(ATan2(BY - AY, BX - AX))

        If (vector_ang > 0) Then

            'objWriter.WriteLine("The ARC Dir is < CCW > end1 is <0> and end 2 is 1")
            'objWriter.WriteLine("The vector_ang is < " & (vector_ang * RAD2DEG) & " > (Actual ANG<" & dTemp & " >)")
            nArcDir = 1
        Else

            'objWriter.WriteLine("The ARC Dir is < CW > end1 is <0> and end 2 is 1")
            'objWriter.WriteLine("The vector_ang is < " & (vector_ang * RAD2DEG) & " > (Actual ANG<" & dTemp & " >)")
            nArcDir = -1

        End If

        ' (b_Loop = True) Then

        PortalExecute(" profile:blend: id1=" & m_EntityID_1 & ",end1=0,x1=" & dSX & ",y1=" & dCY & ",id2=" & m_EntityID_2 & ",end2=1,x2=" & dCX & ",y2=" & dEY & ",cx=" & dCX & " ,cy=" & dCY & ",dir=" & nArcDir)
        'objWriter.WriteLine(" profile:blend: id1=" & m_EntityID_1 & ",end1=0,x1=" & dSX & ",y1=" & dCY & ",id2=" & m_EntityID_2 & ",end2=1,x2=" & dCX & ",y2=" & dEY & ",cx=" & dCX & " ,cy=" & dCY & ",dir=" & nArcDir)

        'b_Loop = False

        ' End If

        'objWriter.WriteLine("  ")

        m_EntityID_3 = entityID

        lReturn = PortalExecute("Solve:Blend:" &
                                " idA=" & m_EntityID_1 &
                                ",idB =" & m_EntityID_2 &
                                ",rad = " & m_Radius)


    End Function

    Private Function PrepareVars() As Long
        Dim sval As String
        Dim cmd As String
        Dim iname As String
        Dim my_DataType As Integer
        Dim sOptions As String

        Try
            For Each row As CategoryRow In vgProplist.Rows

                For Each childRow As EditorRow In row.ChildRows

                    my_DataType = GetValueFromDelimitedData(childRow.Tag, "VarDataType", "")

                    If (my_DataType = DB_COMBO) Then

                        sOptions = GetValueFromDelimitedData(childRow.Tag, "Options", "")

                        sval = GetOptionIndex(sOptions, childRow.Properties.Value)

                        ' sval = GetValueFromDelimitedData(childRow.Properties.Value, "CValue", "")
                    Else
                        sval = childRow.Properties.Value
                    End If


                    iname = Trim(GetValueFromDelimitedData(childRow.Tag, "DatVarName", ""))
                    ' iname = childRow.Tag
                    'Record the variable in the model header.  Doing this allows
                    'us to display the last-used value in the dialog (if ever we
                    'execute the same macro on the same (or similar) model).
                    lReturn = HeaderStringSet(iname, sval)

                    'Redundant to HeaderStringSet().  Introduced with V17.
                    'Just another way of passing data to the macro.  In this
                    'case, the macro can use VarSys.StrGet().
                    cmd = "Admin:VarSys:StrPut:" &
                          " name=" & QStr(iname) &
                          ",val=" & QStr(sval)

                    lReturn = PortalExecute(cmd)
                Next

            Next

        Catch ex As Exception
            MessageBox.Show("PrePareVars: " & ex.Message)
        End Try

    End Function

End Class