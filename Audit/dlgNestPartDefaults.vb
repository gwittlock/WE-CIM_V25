Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraVerticalGrid.Rows
Public Class dlgNestPartDefaults
    Private WithEvents repositoryItemButtonEdit As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit

    Delegate Sub FocusTextHandler(ByRef VerticalGrid)

    Private Sub dlgNestPartDefaults_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        'Dim crPartVarHeader As CategoryRow = Nothing
        'vgPartVars.Rows.Clear()

        'crPartVarHeader = CreateCategoryRow(vgPartVars, "Part Varables", "Part Variables", "Part Variables", "Part Variables", "Part Variables", DB_HEADER, 0, sEmpty, 0)

        'LoadDefaultValues(crPartVarHeader, 0, 0, True)

        Dim crPartVarHeader As CategoryRow = Nothing
        vgPartVars.Rows.Clear()

        crPartVarHeader = CreateCategoryRow(vgPartVars, "Part Varables", "Part Variables", "Part Variables", "Part Variables", "Part Variables", DB_HEADER, 0, sEmpty, 0)

        CreatePartDefaultValues(crPartVarHeader, 0, 0, True)

        SetDefaultsFromReg()

    End Sub
    Private Sub SetDefaultsFromReg()
        Dim indx As Integer
        Dim sValue As String
        Dim sFieldName As String

        For indx = 0 To vgPartVars.Rows(0).ChildRows.Count - 1

            Dim rowType As Type = vgPartVars.Rows(0).ChildRows(indx).Properties.RowType

            sFieldName = GetValueFromDelimitedData(vgPartVars.Rows(0).ChildRows(indx).Tag, "DialogDisplay", 0)
            sValue = RegGetString("Nesting\Part_Defaults", sFieldName, 0)


            If (rowType.Name = "Boolean") Then
                If (sValue = True) Then


                    vgPartVars.Rows(0).ChildRows(indx).Properties.Value = True
                Else

                    vgPartVars.Rows(0).ChildRows(indx).Properties.Value = False

                End If
            Else

                vgPartVars.Rows(0).ChildRows(indx).Properties.Value = sValue

            End If

        Next

    End Sub
    Private Sub LoadDefaultValues(currentCategoryRow As CategoryRow, sPartExtents As String, nid As Integer, AddVarRecord As Boolean)
        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer
        Dim my_sOptions As String



        Dim crPartVarHeader As CategoryRow = Nothing




        my_DatVarName = "Part Quantity"
        my_DialogDisplay = "Quantity"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Part Quantity"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgPartVars, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        my_DatVarName = "Fill Part"
        my_DialogDisplay = "Filler"
        my_VBValue = False
        my_CValue = 0
        my_CVarName = "Fill Part"
        my_DataType = DB_BOOL
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(vgPartVars, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        my_DatVarName = "Rotation Num"
        my_DialogDisplay = "Rotation"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Rotation Num"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgPartVars, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        my_DatVarName = "Rotation Start"
        my_DialogDisplay = "Pre Rotate"
        my_VBValue = 0
        my_CValue = 0
        my_CVarName = "Rotation Start"
        my_DataType = DB_DBL
        my_RecodID = 0
        my_sOptions = sEmpty


        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgPartVars, _
                                                               my_DatVarName, _
                                                               my_DialogDisplay, _
                                                               my_VBValue, _
                                                               my_CValue, _
                                                               my_CVarName, _
                                                               my_DataType, _
                                                               my_RecodID, _
                                                               my_sOptions, 0))

        my_DatVarName = "PreNest"
        my_DialogDisplay = "Pre Nest"
        my_VBValue = False
        my_CValue = 0
        my_CVarName = "PreNest"
        my_DataType = DB_BOOL
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(vgPartVars, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        lbOptions.Items.Clear()

        lbOptions.Items.Add("None")
        lbOptions.Items.Add("Allow")
        lbOptions.Items.Add("Pre Mirror")

        arrOptions = PopulateComboListFromDat(lbOptions)

        my_DatVarName = "MirrorPart"
        my_DialogDisplay = "Mirroring"
        my_VBValue = "None"
        my_CValue = 0
        my_CVarName = "MirrorPart"
        my_DataType = DB_DCOMBO
        my_RecodID = 0


        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgPartVars, _
                                                                        my_DatVarName, _
                                                                        my_DialogDisplay, _
                                                                        my_VBValue, _
                                                                        my_CValue, _
                                                                        my_CVarName, _
                                                                        my_DataType, _
                                                                        my_RecodID, _
                                                                        arrOptions, 0))

        my_DatVarName = "Part Priority"
        my_DialogDisplay = "Priority"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Part Priority"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgPartVars, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        my_DatVarName = "Cost Per Part"
        my_DialogDisplay = "Cost Per Part"
        my_VBValue = 0
        my_CValue = 0
        my_CVarName = "Cost Per Part"
        my_DataType = DB_DBL
        my_RecodID = 0
        my_sOptions = sEmpty


        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgPartVars, _
                                                               my_DatVarName, _
                                                               my_DialogDisplay, _
                                                               my_VBValue, _
                                                               my_CValue, _
                                                               my_CVarName, _
                                                               my_DataType, _
                                                               my_RecodID, _
                                                               my_sOptions, 0))


        lbOptions.Items.Clear()

        lbOptions.Items.Add("Actual Part")
        lbOptions.Items.Add("Bounding Box")
        lbOptions.Items.Add("Rubberband Box")

        arrOptions = PopulateComboListFromDat(lbOptions)
        my_DatVarName = "Representation"
        my_DialogDisplay = "Representation"
        my_VBValue = "Actual Part"
        my_CValue = 0
        my_CVarName = "Representation"
        my_DataType = DB_DCOMBO
        my_RecodID = 0


        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgPartVars, _
                                                                        my_DatVarName, _
                                                                        my_DialogDisplay, _
                                                                        my_VBValue, _
                                                                        my_CValue, _
                                                                        my_CVarName, _
                                                                        my_DataType, _
                                                                        my_RecodID, _
                                                                        arrOptions, 0))


        my_DatVarName = "Part_Extents"
        my_DialogDisplay = "Part Extents"
        my_VBValue = sPartExtents
        my_CValue = sPartExtents
        my_CVarName = "Part_Extents"
        my_DataType = DB_STR
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateStringEditorRow(vgPartVars, _
                                                               my_DatVarName, _
                                                               my_DialogDisplay, _
                                                               my_VBValue, _
                                                               my_CValue, _
                                                               my_CVarName, _
                                                               my_DataType, _
                                                               my_RecodID, _
                                                               my_sOptions, 0))
        currentCategoryRow.Properties.ReadOnly = True

    End Sub

    Private Sub sbFinish_Click(sender As Object, e As EventArgs) Handles sbFinish.Click
        Me.Dispose()
    End Sub

    Private Sub sbSetDefaults_Click(sender As Object, e As EventArgs) Handles sbSetDefaults.Click
        Dim indx As Integer
        Dim sValue As String
        Dim sFieldName As String
        Dim nFieldType As Integer

        For indx = 0 To vgPartVars.Rows(0).ChildRows.Count - 1

            nFieldType = GetValueFromDelimitedData(vgPartVars.Rows(0).ChildRows(indx).Tag, "VarDataType", 0)

            sFieldName = GetValueFromDelimitedData(vgPartVars.Rows(0).ChildRows(indx).Tag, "DialogDisplay", 0)
            sValue = vgPartVars.Rows(0).ChildRows(indx).Properties.Value

            RegPutString("Nesting\Part_Defaults", sFieldName, sValue)

            'sMyString = "Type < " & nFieldType & " > Name < " & sFieldName & " > Value < " & vgPartVars.Rows(0).ChildRows(indx).Tag & " > "

        Next

        Me.Dispose()

    End Sub

    Private Sub vgPartVars_ShowingEditor(sender As Object, e As System.ComponentModel.CancelEventArgs) Handles vgPartVars.ShowingEditor
        BeginInvoke(New FocusTextHandler(AddressOf FocusText), sender)

    End Sub

    Private Sub vgPartVars_ShownEditor(sender As Object, e As EventArgs) Handles vgPartVars.ShownEditor
        If (vgPartVars.FocusedRow.Properties.Caption = "Part Extents") Then

            vgPartVars.FocusedRow.Properties.ReadOnly = True

        Else

            BeginInvoke(New FocusTextHandler(AddressOf FocusText), sender)

        End If
    End Sub
    Private Sub FocusText(ByRef TheVerticalGrid)
        If TypeOf TheVerticalGrid.ActiveEditor Is TextEdit Then
            Dim edit As TextEdit = TryCast(TheVerticalGrid.ActiveEditor, TextEdit)
            edit.SelectionStart = 0
            edit.SelectionLength = (CStr(edit.EditValue)).Length

        End If
    End Sub

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.
        repositoryItemButtonEdit = New RepositoryItemButtonEdit()

    End Sub

    Private Sub repositoryItemButtonEdit_ButtonClick(sender As Object, e As ButtonPressedEventArgs) Handles repositoryItemButtonEdit.ButtonClick

    End Sub
    Private Sub CreatePartDefaultValues(currentCategoryRow As CategoryRow, sPartExtents As String, nid As Integer, AddVarRecord As Boolean)
        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer
        Dim my_sOptions As String

        my_DatVarName = "Part Quantity"
        my_DialogDisplay = "Quantity"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Part Quantity"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgParts, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        my_DatVarName = "Fill Part"
        my_DialogDisplay = "Filler"
        my_VBValue = False
        my_CValue = 0
        my_CVarName = "Fill Part"
        my_DataType = DB_BOOL
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(vgParts, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        my_DatVarName = "Rotation Num"
        my_DialogDisplay = "Rotation"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Rotation Num"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgParts, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        my_DatVarName = "Rotation Start"
        my_DialogDisplay = "Pre Rotate"
        my_VBValue = 0
        my_CValue = 0
        my_CVarName = "Rotation Start"
        my_DataType = DB_DBL
        my_RecodID = 0
        my_sOptions = sEmpty


        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgParts, _
                                                               my_DatVarName, _
                                                               my_DialogDisplay, _
                                                               my_VBValue, _
                                                               my_CValue, _
                                                               my_CVarName, _
                                                               my_DataType, _
                                                               my_RecodID, _
                                                               my_sOptions, 0))

        my_DatVarName = "PreNest"
        my_DialogDisplay = "Pre Nest"
        my_VBValue = False
        my_CValue = 0
        my_CVarName = "PreNest"
        my_DataType = DB_BOOL
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(vgParts, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        lbOptions.Items.Clear()

        lbOptions.Items.Add("None")
        lbOptions.Items.Add("Allow")
        lbOptions.Items.Add("Pre Mirror")

        arrOptions = PopulateComboListFromDat(lbOptions)

        my_DatVarName = "MirrorPart"
        my_DialogDisplay = "Mirroring"
        my_VBValue = "None"
        my_CValue = 0
        my_CVarName = "MirrorPart"
        my_DataType = DB_DCOMBO
        my_RecodID = 0


        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgParts, _
                                                                        my_DatVarName, _
                                                                        my_DialogDisplay, _
                                                                        my_VBValue, _
                                                                        my_CValue, _
                                                                        my_CVarName, _
                                                                        my_DataType, _
                                                                        my_RecodID, _
                                                                        arrOptions, 0))

        my_DatVarName = "Part Priority"
        my_DialogDisplay = "Priority"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Part Priority"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgParts, _
                                                                my_DatVarName, _
                                                                my_DialogDisplay, _
                                                                my_VBValue, _
                                                                my_CValue, _
                                                                my_CVarName, _
                                                                my_DataType, _
                                                                my_RecodID, _
                                                                my_sOptions, 0))

        my_DatVarName = "Cost Per Part"
        my_DialogDisplay = "Cost Per Part"
        my_VBValue = 0
        my_CValue = 0
        my_CVarName = "Cost Per Part"
        my_DataType = DB_DBL
        my_RecodID = 0
        my_sOptions = sEmpty


        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgParts, _
                                                               my_DatVarName, _
                                                               my_DialogDisplay, _
                                                               my_VBValue, _
                                                               my_CValue, _
                                                               my_CVarName, _
                                                               my_DataType, _
                                                               my_RecodID, _
                                                               my_sOptions, 0))


        lbOptions.Items.Clear()

        lbOptions.Items.Add("Actual Part")
        lbOptions.Items.Add("Bounding Box")
        lbOptions.Items.Add("Rubberband Box")

        arrOptions = PopulateComboListFromDat(lbOptions)
        my_DatVarName = "Representation"
        my_DialogDisplay = "Representation"
        my_VBValue = "Actual Part"
        my_CValue = 0
        my_CVarName = "Representation"
        my_DataType = DB_DCOMBO
        my_RecodID = 0


        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgParts, _
                                                                        my_DatVarName, _
                                                                        my_DialogDisplay, _
                                                                        my_VBValue, _
                                                                        my_CValue, _
                                                                        my_CVarName, _
                                                                        my_DataType, _
                                                                        my_RecodID, _
                                                                        arrOptions, 0))


        my_DatVarName = "Part_Extents"
        my_DialogDisplay = "Part Extents"
        my_VBValue = sPartExtents
        my_CValue = sPartExtents
        my_CVarName = "Part_Extents"
        my_DataType = DB_STR
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateStringEditorRow(vgParts, _
                                                               my_DatVarName, _
                                                               my_DialogDisplay, _
                                                               my_VBValue, _
                                                               my_CValue, _
                                                               my_CVarName, _
                                                               my_DataType, _
                                                               my_RecodID, _
                                                               my_sOptions, 0))
        currentCategoryRow.Properties.ReadOnly = True

        If (AddVarRecord = True) Then
            'CreateUserVarsForParts(currentCategoryRow, nid)
        End If

    End Sub
End Class