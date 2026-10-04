Imports DevExpress.XtraVerticalGrid
Imports DevExpress.XtraVerticalGrid.Rows

Public Class dlgEditClamps
    'Implements IDialogMouseUp
    Private WithEvents rbPropSettings As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit
    Private m_is_active As Boolean
    Dim nclamps As Long
    Delegate Sub FocusTextHandler(ByRef VerticalGrid)
    Private Sub FocusText(ByRef TheVerticalGrid)
        Try
            If TypeOf TheVerticalGrid.ActiveEditor Is TextEdit Then
                Dim edit As TextEdit = TryCast(TheVerticalGrid.ActiveEditor, TextEdit)
                edit.SelectionStart = 0
                edit.SelectionLength = (CStr(edit.EditValue)).Length

            End If
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub
    Public Shadows Function IsActive() As Boolean
        IsActive = m_is_active
    End Function

    'Private Sub frmEditClamps_Activated(sender As Object, e As EventArgs) Handles Me.Activated
    '    m_curr_dlg = Me
    'End Sub


    'Private Sub frmEditClamps_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
    '    m_is_active = False
    '    frmMain.CloseFirstPanel()
    'End Sub

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

    'Public Function ClientMouseUp() As Long Implements IDialogMouseUp.ClientMouseUp
    '    Dim pick_info As clsPickInfo
    '    Dim my_DatVarName As String
    '    Dim my_DialogDisplay As String
    '    Dim my_VBValue As String
    '    Dim my_CValue As String
    '    Dim my_CVarName As String
    '    Dim my_DataType As Integer
    '    Dim my_RecodID As Integer
    '    Dim my_sOptions As String

    '    Try
    '        If (vgProplist.FocusedRow IsNot Nothing) Then
    '            my_DatVarName = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "DatVarName", "")
    '            my_DialogDisplay = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "DialogDisplay", "")
    '            my_VBValue = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "VBValue", "")
    '            my_CValue = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "CValue", "")
    '            my_CVarName = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "CVarName", "")
    '            my_DataType = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "VarDataType", "")
    '            my_RecodID = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "nRecordID", "")
    '            my_sOptions = GetValueFromDelimitedData(vgProplist.FocusedRow.Tag, "Options", "")

    '            modEdit.RevertProfile()

    '            pick_info = modPick.GetPickInfo()

    '            vgProplist.BeginDataUpdate()

    '            UpdateAccumulatorPickInfo(vgProplist, pick_info)

    '            vgProplist.EndDataUpdate()

    '            ClientMouseUp = 1

    '            Dim nChildRowCount As Integer

    '            'determine row
    '            Dim row As BaseRow = vgProplist.FocusedRow

    '            If TypeOf row Is CategoryRow Then
    '                Dim operation As New MoveToNextCategoryOperation(vgProplist)
    '                vgProplist.RowsIterator.DoOperation(operation)
    '                vgProplist.Focus()

    '                If (operation.NextCategory IsNot Nothing) Then
    '                    vgProplist.FocusedRow = operation.NextCategory
    '                    vgProplist.LayoutChanged()
    '                Else
    '                    vgProplist.FocusedRow = vgProplist.Rows(0)
    '                    vgProplist.LayoutChanged()
    '                End If

    '            End If

    '            If TypeOf row Is EditorRow Then

    '                nChildRowCount = row.ParentRow.ChildRows.Count - 1

    '                If (row.Index = nChildRowCount) Then
    '                    Dim operation As New MoveToNextCategoryOperation(vgProplist)
    '                    vgProplist.RowsIterator.DoOperation(operation)
    '                    vgProplist.Focus()

    '                    If (operation.NextCategory IsNot Nothing) Then
    '                        vgProplist.FocusedRow = operation.NextCategory
    '                        vgProplist.LayoutChanged()
    '                    Else
    '                        vgProplist.FocusedRow = vgProplist.Rows(0)
    '                        vgProplist.LayoutChanged()
    '                    End If
    '                Else
    '                    Dim operation As New MoveToNextEditorRowOperation(vgProplist)
    '                    vgProplist.RowsIterator.DoOperation(operation)
    '                    vgProplist.Focus()
    '                    vgProplist.FocusedRow = operation.NextEditorRow
    '                    vgProplist.LayoutChanged()
    '                End If
    '            End If



    '        End If
    '    Catch ex As Exception

    '    End Try
    'End Function
    Private Class MoveToNextEditorRowOperation
        Inherits RowOperation
        Private nextCategory_Renamed As EditorRow
        Private currentRow As BaseRow

        Public Sub New(ByVal grid As VGridControl)
            MyBase.New()
            currentRow = grid.FocusedRow
        End Sub
        Public Overrides Sub Execute(ByVal row As BaseRow)
            Try
                If row.VisibleIndex > currentRow.VisibleIndex Then
                    If nextCategory_Renamed Is Nothing AndAlso TypeOf row Is EditorRow Then
                        nextCategory_Renamed = TryCast(row, EditorRow)
                    End If
                End If
            Catch ex As Exception
            End Try
        End Sub

        Public ReadOnly Property NextEditorRow() As EditorRow
            Get
                Return nextCategory_Renamed
            End Get
        End Property
    End Class

    Public Class MoveToNextCategoryOperation
        Inherits RowOperation
        Private nextCategory_Renamed As CategoryRow
        Private currentRow As BaseRow

        Public Sub New(ByVal grid As VGridControl)
            MyBase.New()
            currentRow = grid.FocusedRow
        End Sub
        Public Overrides Sub Execute(ByVal row As BaseRow)

            Try
                If (currentRow IsNot Nothing) Then
                    If row.VisibleIndex > currentRow.VisibleIndex Then
                        If nextCategory_Renamed Is Nothing AndAlso TypeOf row Is CategoryRow Then
                            nextCategory_Renamed = TryCast(row, CategoryRow)
                        End If
                    End If

                End If
            Catch ex As Exception
            End Try
        End Sub

        Public ReadOnly Property NextCategory() As CategoryRow
            Get
                Return nextCategory_Renamed
            End Get
        End Property
    End Class


    Private Sub dlgEditClamps_Load(sender As Object, e As EventArgs) Handles Me.Load
        Dim org_clamp_pos(0 To 3) As Double
        Dim indx As Long
        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer
        Dim my_sOptions As String

        Dim currentCategoryRow As CategoryRow = Nothing
        Try

            '    currentCategoryRow = CreateCategoryRow(vgProplist, "Clamp Information", "Clamp Information", "Clamp Information", "Clamp Information", "Clamp Information", 0, DB_HEADER, sEmpty, 0)

            '    cboDefn.Properties.Items.Add("Clamp Locations")
            '    cboDefn.SelectedIndex = 0
            '    nclamps = HeaderIntGet("Number_Of_Clamps", 0)

            '    org_clamp_pos(0) = HeaderDoubleGet("Clamp1Pos", 0)
            '    org_clamp_pos(1) = HeaderDoubleGet("Clamp2Pos", 0)
            '    org_clamp_pos(2) = HeaderDoubleGet("Clamp3Pos", 0)
            '    org_clamp_pos(3) = HeaderDoubleGet("Clamp4Pos", 0)

            '    For indx = 0 To nclamps - 1
            '        my_DatVarName = "Clamp " & indx & ":"
            '        my_DialogDisplay = "Clamp " & indx
            '        my_VBValue = org_clamp_pos(indx)
            '        my_CValue = org_clamp_pos(indx)
            '        my_CVarName = "Part Quantity"
            '        my_DataType = DB_DBL
            '        my_RecodID = 0
            '        my_sOptions = sEmpty

            '        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgProplist, _
            '                                                               my_DatVarName, _
            '                                                               my_DialogDisplay, _
            '                                                               my_VBValue, _
            '                                                               my_CValue, _
            '                                                               my_CVarName, _
            '                                                               my_DataType, _
            '                                                               my_RecodID, _
            '                                                               my_sOptions, 0))


            'Next
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub

    Private Sub btnAccept_Click(sender As Object, e As EventArgs) Handles btnAccept.Click
        'Dim mach As MACHINE_ATTRIBUTES
        'Dim cmd As String
        'Dim dYMin As Double
        'Dim dYMax As Double
        'Dim dYpos As Double
        'Dim machine_type As Long
        'Dim workplane_type As Long
        'Dim nclamps As Long
        'Dim dClamp1 As Double
        'Dim dClamp2 As Double
        'Dim dClamp3 As Double
        'Dim dClamp4 As Double


        'Try
        '    machine_type = HeaderIntGet("MachineType", 0)
        '    workplane_type = HeaderIntGet("WorkplaneType", 0)

        '    mach = modMachine.GetMachineAttributes()

        '    If (workplane_type = 2) Then
        '        dYpos = HeaderDoubleGet("Width", 0)
        '        If (machine_type = 8) Then
        '            dYMin = 0
        '            dYMax = Max(mach.Punch_Clamp_Deadzone_Width, mach.Torch_Clamp_Deadzone_Width)
        '        Else
        '            dYMin = Max(mach.Punch_Clamp_Deadzone_Width, mach.Torch_Clamp_Deadzone_Width) * -1
        '            dYMax = 0
        '        End If
        '    Else
        '        dYpos = 0
        '        dYMin = 0
        '        dYMax = Max(mach.Punch_Clamp_Deadzone_Width, mach.Torch_Clamp_Deadzone_Width)
        '    End If

        '    nclamps = HeaderIntGet("Number_Of_Clamps", 0)


        '    For Each row As CategoryRow In vgProplist.Rows

        '        For Each childRow As EditorRow In row.ChildRows

        '            Select Case childRow.Index

        '                Case 0

        '                    dClamp1 = CDbl(childRow.Properties.Value)

        '                Case 1

        '                    dClamp2 = CDbl(childRow.Properties.Value)

        '                Case 2

        '                    dClamp3 = CDbl(childRow.Properties.Value)

        '                Case 3

        '                    dClamp4 = CDbl(childRow.Properties.Value)

        '            End Select

        '        Next
        '    Next

        '    Select Case nclamps
        '        Case 1
        '            cmd = "Create:Clamp: num=1" & _
        '                ",x1=" & CDbl(dClamp1) & ",y1=" & dYpos

        '        Case 2
        '            cmd = "Create:Clamp: num=2" & _
        '                ",x1=" & CDbl(dClamp1) & ",y1=" & dYpos & _
        '                ",x2=" & CDbl(dClamp2) & ",y2=" & dYpos

        '        Case 3
        '            cmd = "Create:Clamp: num=3" & _
        '                ",x1=" & CDbl(dClamp1) & ",y1=" & dYpos & _
        '                ",x2=" & CDbl(dClamp2) & ",y2=" & dYpos & _
        '                ",x3=" & CDbl(dClamp3) & ",y3=" & dYpos

        '        Case 4
        '            cmd = "Create:Clamp: num=4" & _
        '                ",x1=" & CDbl(dClamp1) & ",y1=" & dYpos & _
        '                ",x2=" & CDbl(dClamp2) & ",y2=" & dYpos & _
        '                ",x3=" & CDbl(dClamp3) & ",y3=" & dYpos & _
        '                ",x4=" & CDbl(dClamp4) & ",y4=" & dYpos

        '        Case Else
        '            cmd = "Create:Clamp:"

        '    End Select

        '    UndoPrepare()

        '    'Temporarily set current workplane to world.
        '    lReturn = PortalExecute("create:plane:id=" & modPlanes.GetWorldID())

        '    'Update the clamps.
        '    lReturn = PortalExecute(cmd)

        '    'Reset the previously active workplane.
        '    lReturn = PortalExecute("create:plane:id=" & modPlanes.GetTopID())

        '    UndoCommit()

        '    modView.ViewRefresh()

        '    If (bIsEntityLoaded = True) Then frmEntityList.FillTreeView()
        'Catch ex As Exception
        '    MessageBox.Show(ex.Message)
        'End Try

    End Sub

    Private Sub vgProplist_ShowingEditor(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles vgProplist.ShowingEditor
        BeginInvoke(New FocusTextHandler(AddressOf FocusText), sender)
    End Sub
End Class