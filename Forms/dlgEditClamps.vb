Imports DevExpress.XtraVerticalGrid
Imports DevExpress.XtraVerticalGrid.Rows

Public Class dlgEditClamps
    'Implements IDialogMouseUp
    Public Property Job As JobContext
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

        If CurrentJob Is Nothing Then Exit Sub
        If CurrentJob.MachineDefinition Is Nothing Then Exit Sub

        ' Ensure the job has its clamp records.
        InitializeClamps(CurrentJob)

        vgProplist.Rows.Clear()

        Dim category As New CategoryRow()
        category.Properties.Caption = "Clamp Locations"

        vgProplist.Rows.Add(category)

        For i As Integer = 0 To CurrentJob.Clamps.Count - 1

            Dim clamp As Clamp = CurrentJob.Clamps(i)

            Dim clampRow As New EditorRow()
            clampRow.Properties.Caption = "Clamp " & (i + 1).ToString()
            clampRow.Properties.Value = clamp.X

            category.ChildRows.Add(clampRow)

        Next

    End Sub

    Private Sub btnAccept_Click(sender As Object, e As EventArgs) Handles btnAccept.Click

        If CurrentJob Is Nothing Then Exit Sub

        ' Commit any cell currently being edited.
        vgProplist.CloseEditor()

        Dim category As CategoryRow =
        TryCast(vgProplist.Rows(0), CategoryRow)

        If category Is Nothing Then Exit Sub

        ' Read all positions before changing the job.
        Dim positions As New List(Of Double)

        For Each row As EditorRow In category.ChildRows

            Dim x As Double

            If Not Double.TryParse(
            Convert.ToString(row.Properties.Value),
            x) Then

                MessageBox.Show(
                "Please enter a valid clamp position.",
                "Clamp Locations",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning)

                Return
            End If

            positions.Add(x)

        Next

        If positions.Count <> CurrentJob.Clamps.Count Then
            MessageBox.Show(
            "Clamp count does not match the current job.",
            "Clamp Locations",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)
            Return
        End If

        ' Save positions to the job.
        For i As Integer = 0 To positions.Count - 1
            CurrentJob.Clamps(i).X = positions(i)
        Next

        Me.DialogResult = DialogResult.OK

        Me.Close()

    End Sub

    Private Sub vgProplist_ShowingEditor(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles vgProplist.ShowingEditor
        BeginInvoke(New FocusTextHandler(AddressOf FocusText), sender)
    End Sub
End Class