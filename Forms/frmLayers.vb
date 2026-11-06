Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraGrid.Views.Grid
Public Class frmLayers

    Public Property ProductionJob As Job
    Dim sLayerName As String
    Dim lLayerColor As System.Drawing.Color
    Private edit As RepositoryItemCheckEdit

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
    Private Sub frmLayers_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        'frmMain.CloseFirstPanel()
    End Sub

    Private Sub sbAdd_Click(sender As Object, e As EventArgs) Handles sbAdd.Click
        'Dim ni As Long
        'Dim lSuccess As Long

        'Try

        '    ' make sure layer name is unique

        '    For ni = 0 To gViewToolLayer.RowCount - 1
        '        If (UCase$(gViewToolLayer.GetRowCellValue(ni, gViewToolLayer.Columns(2))) = UCase$(teLayerName.Text)) Then
        '            MsgBox("Layer already exists. Select a new name.", vbInformation, App_Title)
        '            teLayerName.Focus()

        '            Exit Sub
        '        End If
        '    Next

        '    ' let the system verify the name for validity
        '    ' lReturn = 'PortalExecute("model:namecheck:name=" & QStr(teLayerName.Text))
        '    'lSuccess = PortalGetInt("bool", 0)

        '    If lSuccess < 1 Then
        '        MsgBox("Invalid layer name. Select a new name.", vbInformation, App_Title)
        '        teLayerName.Focus()
        '        Exit Sub
        '    End If

        '    ' create new layer
        '    Call SetActiveLayer(teLayerName.Text, ColorTranslator.ToOle(Me.ColorPickEdit1.Color))
        '    ' refresh the layer list
        '    LoadLayersListview()

        '    cbfAddLayer.Checked = False

        'Catch ex As Exception
        '    MessageBox.Show(ex.Message)
        'End Try

    End Sub

    Private Sub CreateRandomLayer(sLayerName As String, lLayerColor As System.Drawing.Color)
        Dim sTemp As String
        Dim colLayers As New Collection
        Dim nI As Integer
        Dim bFound As Boolean
        Dim Index As Integer

        Try

            For nI = 0 To gViewToolLayer.RowCount - 1
                sTemp = UCase$(gViewToolLayer.GetRowCellValue(nI, gViewToolLayer.Columns(2)))
                'If (sTemp.Substring(0, 6) = "LAYER_") Then
                colLayers.Add(sTemp)
                'End If

            Next

            nI = 0

            Do
                bFound = False
                nI = nI + 1

                For Index = 1 To colLayers.Count
                    sTemp = colLayers.Item(Index)

                    If UCase$(gViewToolLayer.GetRowCellValue(nI, gViewToolLayer.Columns(1))) = CStr(nI) Then bFound = True

                Next Index
            Loop Until (bFound = False)

            sLayerName = "Layer_" & nI

        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Sub

    '    Private Sub frmLayers_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
    '        Me.Dispose()
    '    End Sub

    Private Sub frmLayers_Load(sender As Object, e As EventArgs) Handles Me.Load

        LoadLayersListview()

    End Sub
    Private Class TheTools
        Private privateToolHidden As Boolean
        Public Property ToolHidden() As Boolean
            Get
                Return privateToolHidden
            End Get
            Set(ByVal value As Boolean)
                privateToolHidden = value
            End Set
        End Property
        Private privateToolNumber As Integer
        Public Property ToolNumber() As Integer
            Get
                Return privateToolNumber
            End Get
            Set(ByVal value As Integer)
                privateToolNumber = value
            End Set
        End Property
        Private privateToolName As String
        Public Property ToolName() As String
            Get
                Return privateToolName
            End Get
            Set(ByVal value As String)
                privateToolName = value
            End Set
        End Property
        Private privateToolColor As System.Drawing.Color
        Public Property ToolColor() As System.Drawing.Color
            Get
                Return privateToolColor
            End Get
            Set(ByVal value As System.Drawing.Color)
                privateToolColor = value
            End Set
        End Property
    End Class
    Public Sub LoadLayersListview()

        grdToolLayer.DataSource = Nothing

        If ProductionJob Is Nothing Then Return

        Dim layers As New List(Of Layer)

        For Each sheet As JobSheet In ProductionJob.JobSheets

            For Each part As Part In sheet.Parts

                If part Is Nothing Then Continue For

                layers.AddRange(part.GetLayers())

            Next

        Next

        Dim layerList = layers.
        GroupBy(Function(layer) layer.Name,
                StringComparer.OrdinalIgnoreCase).
        Select(Function(group) group.First()).
        OrderBy(Function(layer) layer.Name).
        ToList()

        grdToolLayer.DataSource = layerList

        gViewToolLayer.PopulateColumns()

        gViewToolLayer.Columns("ID").Visible = False

        gViewToolLayer.Columns("Name").Caption = "Layer Name"
        gViewToolLayer.Columns("Visible").VisibleIndex = 0
        gViewToolLayer.Columns("Name").VisibleIndex = 1
        gViewToolLayer.Columns("Visible").Caption = "Visible"
        gViewToolLayer.OptionsBehavior.Editable = True

        gViewToolLayer.Columns("Name").OptionsColumn.AllowEdit = False
        gViewToolLayer.Columns("Visible").OptionsColumn.AllowEdit = True
        gViewToolLayer.BestFitColumns()

        gViewToolLayer.OptionsBehavior.Editable = True

        gViewToolLayer.Columns("ID").OptionsColumn.AllowEdit = False
        gViewToolLayer.Columns("Name").OptionsColumn.AllowEdit = False
        gViewToolLayer.Columns("Visible").OptionsColumn.AllowEdit = True
    End Sub

    Private Sub gViewToolLayer_RowCellClick(
    sender As Object,
    e As RowCellClickEventArgs) Handles gViewToolLayer.RowCellClick

        If e.Column.FieldName <> "Visible" Then Return

        Dim selectedLayer As Layer =
        TryCast(gViewToolLayer.GetRow(e.RowHandle), Layer)

        If selectedLayer Is Nothing Then Return

        Dim newVisible As Boolean = Not selectedLayer.Visible

        For Each sheet As JobSheet In ProductionJob.JobSheets

            For Each part As Part In sheet.Parts

                If part Is Nothing Then Continue For

                For Each layer As Layer In part.GetLayers()

                    If String.Equals(
                    layer.Name,
                    selectedLayer.Name,
                    StringComparison.OrdinalIgnoreCase) Then

                        layer.Visible = newVisible

                    End If

                Next

            Next

        Next

        gViewToolLayer.RefreshData()

    End Sub
    'Try

    '    If (e.Column.Name = "colToolHidden") Then
    '        nLayerId = gViewToolLayer.GetRowCellValue(e.RowHandle, gViewToolLayer.Columns("ToolNumber"))


    '        gViewToolLayer.SetRowCellValue(e.RowHandle, e.Column, IIf(e.CellValue = True, False, True))

    '        If (e.CellValue = False) Then

    '            'Return = PortalExecute("entity:hide:id=" & nLayerId)
    '        Else
    '            'lReturn = PortalExecute("entity:show:id=" & nLayerId)
    '        End If

    '        modView.ViewRegen()

    '    End If

    'Catch ex As Exception
    '    MessageBox.Show(ex.Message)
    'End Try


    Private Sub gViewToolLayer_RowStyle(sender As Object, e As RowStyleEventArgs) Handles gViewToolLayer.RowStyle
        'Dim lst As List(Of TheTools)
        'Dim item As TheTools

        'If (e.RowHandle < 0) Then

        '    Return

        'End If

        'lst = CType(gViewToolLayer.DataSource, List(Of TheTools))
        'item = lst(e.RowHandle)
        'e.Appearance.ForeColor = item.ToolColor
        'e.Appearance.BackColor = Color.Black

    End Sub

    Private Sub sbHideAll_Click(sender As Object, e As EventArgs) Handles sbHideAll.Click
        'Dim ni As Long
        'Dim nLayerid As Long

        'Try
        '    For ni = 0 To gViewToolLayer.RowCount - 1
        '        nLayerid = gViewToolLayer.GetRowCellValue(ni, gViewToolLayer.Columns("ToolNumber"))

        '        lReturn = PortalExecute("entity:hide:id=" & nLayerid)
        '    Next

        '    modView.ViewRegen()
        '    LoadLayersListview()


        'Catch ex As Exception

        '    MessageBox.Show("sbHideAll: " & ex.Message)

        'End Try
    End Sub

    Private Sub sbShowAll_Click(sender As Object, e As EventArgs) Handles sbShowAll.Click
        'Dim ni As Long
        'Dim nLayerid As Long

        'Try
        '    For ni = 0 To gViewToolLayer.RowCount - 1
        '        nLayerid = gViewToolLayer.GetRowCellValue(ni, gViewToolLayer.Columns("ToolNumber"))

        '        lReturn = PortalExecute("entity:show:id=" & nLayerid)
        '    Next

        '    modView.ViewRegen()
        '    LoadLayersListview()

        'Catch ex As Exception

        '    MessageBox.Show("sbHideAll: " & ex.Message)

        'End Try
    End Sub

    Private Sub sbSetCurrent_Click(sender As Object, e As EventArgs) Handles sbSetCurrent.Click

        'Try

        '    If (gViewToolLayer.FocusedRowHandle < 0) Then

        '        MessageBox.Show("Please select a 'Layer'.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
        '        Exit Sub
        '    End If

        '    If (gViewToolLayer.GetRowCellValue(gViewToolLayer.FocusedRowHandle, gViewToolLayer.Columns("ToolHidden")) = False) Then

        '        MessageBox.Show("The Selected Layer is Hidden", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)

        '    End If

        '    If (UCase(gViewToolLayer.GetRowCellValue(gViewToolLayer.FocusedRowHandle, gViewToolLayer.Columns("ToolName"))) <> "STOCK") Then

        '        Call SetActiveLayer(gViewToolLayer.GetRowCellValue(gViewToolLayer.FocusedRowHandle, gViewToolLayer.Columns("ToolName")), _
        '                             System.Drawing.ColorTranslator.ToOle(gViewToolLayer.GetRowCellValue(gViewToolLayer.FocusedRowHandle, gViewToolLayer.Columns("ToolColor"))))

        '    End If

        '    ' clear the default drawing flag
        '    lReturn = PortalExecute("attrib:defdel:name=" & QStr("partprof"))

        '    frmMain.CloseFirstPanel()
        '    Me.Dispose()


        'Catch ex As Exception
        '    MessageBox.Show(ex.Message)
        'End Try
    End Sub

    Private Sub btnAddLayer_Click(sender As Object, e As EventArgs) Handles btnAddLayer.Click

    End Sub
End Class