Imports DevExpress.LookAndFeel
Imports DevExpress.Skins
Imports DevExpress.Spreadsheet
Imports DevExpress.XtraTreeList
Imports DevExpress.XtraTreeList.Nodes
Imports DevExpress.XtraVerticalGrid.Rows
Imports System.Data.OleDb
Imports System.IO

Public Class frmMRP
    Private USER_FIELDS_ROOT_NODE As TreeListNode
    Private USER_FIELDS_NODE As TreeListNode
    Private SYSTEM_ROOT_NODE As TreeListNode
    Private SYSTEM_PART_ROOT_NODE As TreeListNode
    Private SYSTEM_PART_NODE As TreeListNode
    Private SYSTEM_NESTING_ROOT_NODE As TreeListNode
    Private SYSTEM_NESTING_NODE As TreeListNode
    Private SYSTEM_MATERIAL_ROOT_NODE As TreeListNode
    Private SYSTEM_MATERIAL_NAME_NODE As TreeListNode
    Private SYSTEM_MATERIAL_LENGTH_NODE As TreeListNode
    Private SYSTEM_MATERIAL_WIDTH_NODE As TreeListNode
    Private SYSTEM_MATERIAL_THICKNESS_NODE As TreeListNode

    Private Const TAG_ROOT_NODE = -100
    Private Const TAG_PART_ROOT_NODE = -200
    Private Const TAG_PART_NODE = -300
    Private Const TAG_NESTING_ROOT_NODE = -400
    Private Const TAG_NESTING_NODE = -500
    Private Const TAG_MATERIAL_ROOT_NODE = -600
    Private Const TAG_MATERIAL_NAME_NODE = -700
    Private Const TAG_MATERIAL_LENGTH_NODE = -800
    Private Const TAG_MATERIAL_WIDTH_NODE = -900
    Private Const TAG_MATERIAL_THICKNESS_NODE = -1000
    Private Const TAG_USER_ROOT_NODE = -1100
    Private Const TAG_USER_FIELDS_NODE = -1200

    Private Const XLS_TYPE = 0
    Private Const MDB_TYPE = 1
    Private Const TAB_TYPE = 2
    Private Const CSV_TYPE = 3
    Private Const SEMICOLON_TYPE = 4
    Private Const XLSX_TYPE = 5

    Private m_usage As Long

    Private sPieces() As String
    Private m_pdb_filename As String = Nothing
    Private m_folderDialog As New FolderBrowserDialog()
    Private m_NestResults_Folder As String
    Private m_templateName As String
    Private intImportType As Long

    Private s_Machine As String = String.Empty
    Private s_ToolSetup As String = String.Empty
    Private s_LayerSetup As String = String.Empty
    Private s_LeadSetup As String = String.Empty
    Private cFirstCell As Cell = Nothing
    Private cSecondCell As Cell = Nothing
    Private cThridCell As Cell = Nothing
    Private cFourthCell As Cell = Nothing
    Private m_XLS_Name As String
    Private m_job_name As String
    Private m_is_nesting As Boolean
    Private Const m_col_FileName = 0
    Private Const m_col_Material_type = 1
    Private Const m_col_Material_width = 3
    Private Const m_col_Material_length = 4
    Private Const m_col_Material_Thickness = 5
    Private Const m_col_MirrorPart = 7
    Private Const m_col_PreNest = 8
    Private Const m_col_Part_Quantity = 9
    Private Const m_col_Rotation_NUM = 10
    Private Const m_col_Rotation_start = 11
    Private Const m_col_Part_Priority = 12
    Private Const m_col_Cost_Per_Part = 13
    Private Const m_col_Representation = 14
    Private m_Search_Col As Integer
    Private sMRP_DB As String
    Private sMergePartsFilename As String
    Dim sTempMDB As String
    Dim sTempUserFolder As String




    Private Sub sbCancel_Click(sender As Object, e As EventArgs) Handles sbCancel.Click

        bDo_MRP_Nesting = False

        Me.Close()
    End Sub
    Private Sub SearchForFiles()

        Dim ni As Integer

        For ni = 0 To ListBoxControl1.Items.Count - 1
            FindAllFiles(fcbSourceFolder.Text, lbcFilesFromMrp.Items(ni).ToString & "*.*", lbcFoundFiles, lbcFilesNotFound)
        Next

        Dim sw As New StreamWriter(Application.StartupPath & "\DeBug\ApiCopyFIles.bat", True)
        For ni = 0 To lbcFoundFiles.Items.Count - 1
            sw.WriteLine("copy " & QStr(lbcFoundFiles.Items(ni).ToString) & " " & QStr(Application.StartupPath & "\Debug"))
        Next
        sw.Close()

    End Sub
    Private Sub sbNext_Click(sender As Object, e As EventArgs) Handles sbNext.Click
        Dim sLinksFromReg As String

        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        Try

            sbBack.Enabled = False
            sbNext.Enabled = True

            xtcMRP.SelectedTabPageIndex = xtcMRP.SelectedTabPageIndex + 1
            m_templateName = RegGetString("MRP", "MRP_TEMPLATE", "")


            Select Case UCase(xtcMRP.SelectedTabPage.Name)
                Case "TAbMRPSorting"

                Case "TABMRPPREVIEW"
                    Select Case m_usage
                        Case XLS_TYPE
                            RegPutString("MRP", "MRP_TEMPLATE", "XLS")

                            GetSpreadSheet()

                        Case XLSX_TYPE
                            RegPutString("MRP", "MRP_TEMPLATE", "XLSX")

                            GetSpreadSheet()

                        Case CSV_TYPE
                            RegPutString("MRP", "MRP_TEMPLATE", "COMMA")

                        Case TAB_TYPE
                            RegPutString("MRP", "MRP_TEMPLATE", "TAB")

                        Case SEMICOLON_TYPE
                            RegPutString("MRP", "MRP_TEMPLATE", "SEMICOLON")

                        Case MDB_TYPE
                            RegPutString("MRP", "MRP_TEMPLATE", "MDB")

                            'ExportOneTable(fcbImportFile.Text, cboSheetTable.EditValue)

                            'SpreadsheetControl1.LoadDocument(PathTempFolder() & PathFile(fcbImportFile.Text) & ".xls", DocumentFormat.Xls)


                    End Select

                    SPreadSheetRowsCols()

                    sbBack.Enabled = True

                Case "TABMRPMAPPING"
                    GetUserFields()
                    GetSystemfields()

                    Dim sUserFieldName As String

                    sUserFieldName = RegGetString("MRP", "USER_FIELD", "")

                    If (sUserFieldName <> "") Then
                        For ni = 0 To cboFieldName.Properties.Items.Count - 1
                            If (sUserFieldName = cboFieldName.Properties.Items.Item(ni).ToString) Then
                                cboFieldName.SelectedIndex = ni
                            End If
                        Next
                    End If


                    TV_MappedFields.Nodes.Clear()

                    sLinksFromReg = GetLinksFromReg(m_templateName)
                    sPieces = Split(sLinksFromReg, ",")

                    Me.TV_MappedFields.BeginUnboundLoad()
                    Dim parentForRootNodes As TreeListNode = Nothing
                    Dim resultData As String = String.Empty

                    For ni = 0 To UBound(sPieces)
                        resultData = sPieces(ni)
                        Dim rootNode As TreeListNode = Me.TV_MappedFields.AppendNode(New Object() {resultData}, parentForRootNodes)


                    Next

                    Me.TV_MappedFields.EndUnboundLoad()
                    sbBack.Enabled = True

                Case "TABMRPFILESPROCESSED"
                    lbcFilesNotFound.Items.Clear()
                    lbcFoundFiles.Items.Clear()
                    sbBack.Enabled = True
                    BuildFileList()
                    gcFileNotFOund.Text = "(" & lbcFilesNotFound.Items.Count & ") " & "Files Not Found Or Wrong Revision"
                    gcFilesFOund.Text = "( " & lbcFoundFiles.Items.Count & " ) " & "Files Found"


                    'SearchForFiles()

                Case "TABMRPSORTING"
                    sbBack.Enabled = True


                Case "TABMRPPARTDEFAULTS"
                    sbBack.Enabled = True

                    FillPartDefaultGrid()

                Case "TABMRPNESTDEFAULTS"
                    sbBack.Enabled = True

                    PopulateDefaultSettings()

                    sbFinish.Enabled = True
                    sbNext.Enabled = False
            End Select


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
        Windows.Forms.Cursor.Current = Cursors.Default

    End Sub

    Private Sub BuildFileList()
        Dim workbook As IWorkbook = SpreadsheetControl1.Document
        Dim worksheet As Worksheet = workbook.Worksheets(0)
        Dim cellC2 As Cell
        Dim ni As Integer
        Dim sPartFilename As String
        Dim sRevNumber As String
        Dim nPartColIndex As Integer = -1
        Dim nRevColIndex As Integer = -1
        Dim nRMPartColIndex As Integer = -1
        Dim sTempStr As String
        Dim sActualName As String
        Dim sPartMappedField As String
        Dim sRevMappedField As String
        Dim sPieces() As String


        For ni = 0 To TV_MappedFields.Nodes.Count - 1
            sTempStr = TV_MappedFields.Nodes.Item(ni).GetDisplayText(0)
            sPieces = Split(sTempStr, "=")

            If (InStr(UCase(sTempStr), "PART NAME") > 0) Then
                sPartMappedField = Trim(sPieces(0))
                nPartColIndex = GetColIndexFromText(sPartMappedField, worksheet)
                If (nPartColIndex = -1) Then
                    nPartColIndex = ni
                End If
            End If
            If (InStr(UCase(sTempStr), "COMP REV") > 0) Then
                sRevMappedField = Trim(sPieces(0))
                nRevColIndex = GetColIndexFromText(sRevMappedField, worksheet)
            End If

            If (InStr(UCase(sTempStr), "RM PART") > 0) Then

                nRMPartColIndex = GetColIndexFromText("RM PART", worksheet)

            End If
            If (nPartColIndex <> -1 And nRevColIndex <> -1) Then
                Exit For
            End If
        Next

        lbcFilesFromMrp.Items.Clear()

        For ni = 1 To SpreadsheetControl1.ActiveWorksheet.Rows.LastUsedIndex


            sTempStr = worksheet.Cells(ni, nPartColIndex).DisplayText

            sPartFilename = Replace(sTempStr, "/", "-")

            sRevNumber = worksheet.Cells(ni, nRevColIndex).DisplayText
            If (sRevNumber > 0) Then
                sActualName = sPartFilename & "_R0" & sRevNumber
            Else
                sActualName = sPartFilename
            End If

            lbcFilesFromMrp.Items.Add(New myItemData(sActualName, worksheet.Cells(ni, nRMPartColIndex).DisplayText))
            'lbcFilesFromMrp.Items.Add(sActualName)

            FindAllFiles("c:\laser", sActualName, lbcFoundFiles, lbcFilesNotFound)
            'DoRecursiveSearch("t:\Laser", sActualName & ".dxf", "*", True)
        Next

    End Sub
    Private Sub FillPartDefaultGrid()
        Dim sData As String
        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer
        Dim my_sOptions As String
        Dim m_nesting_Template As String
        Dim sChoices As New Collection
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim sql As String
        Dim ni As Integer

        Try

            catPartDefaultInfo.ChildRows.Clear()

            my_DatVarName = "Part Quantity"
            my_DialogDisplay = "Part Quantity"
            my_VBValue = 1
            my_CValue = 1
            my_CVarName = "Part Quantity"
            my_DataType = DB_INT
            my_RecodID = 0
            my_sOptions = sEmpty

            catPartDefaultInfo.ChildRows.Add(CreateIntegerEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

            my_DatVarName = "Fill Part"
            my_DialogDisplay = "Fill Part"
            my_VBValue = 0
            my_CValue = 0
            my_CVarName = "Fill Part"
            my_DataType = DB_BOOL
            my_RecodID = 0
            my_sOptions = sEmpty

            catPartDefaultInfo.ChildRows.Add(CreateBooleanEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

            my_DatVarName = "Rotation Num"
            my_DialogDisplay = "Rotations/360"
            my_VBValue = 1
            my_CValue = 1
            my_CVarName = "Rotation Num"
            my_DataType = DB_INT
            my_RecodID = 0
            my_sOptions = sEmpty

            catPartDefaultInfo.ChildRows.Add(CreateIntegerEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

            my_DatVarName = "Rotation Start"
            my_DialogDisplay = "Pre-Rotate"
            my_VBValue = 0
            my_CValue = 0
            my_CVarName = "Rotation Start"
            my_DataType = DB_DBL
            my_RecodID = 0
            my_sOptions = sEmpty

            catPartDefaultInfo.ChildRows.Add(CreateDoubleEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

            my_DatVarName = "Part Pass"
            my_DialogDisplay = "Priority"
            my_VBValue = 1
            my_CValue = 1
            my_CVarName = "Part Pass"
            my_DataType = DB_INT
            my_RecodID = 0
            my_sOptions = sEmpty

            catPartDefaultInfo.ChildRows.Add(CreateIntegerEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

            my_DatVarName = "Cost Per Part"
            my_DialogDisplay = "Cost Per Part"
            my_VBValue = 0
            my_CValue = 0
            my_CVarName = "Cost Per Part"
            my_DataType = DB_DBL
            my_RecodID = 0
            my_sOptions = sEmpty

            catPartDefaultInfo.ChildRows.Add(CreateDoubleEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

            sChoices.Clear()
            sChoices.Add("Actual Part")
            sChoices.Add("Bounding Box")
            sChoices.Add("Rubberband Box")

            my_DatVarName = "Representation"
            my_DialogDisplay = "Representation"
            my_VBValue = "Actual Part"
            my_CValue = 0
            my_CVarName = "Representation"
            my_DataType = DB_DCOMBO
            my_RecodID = 0
            my_sOptions = sEmpty

            catPartDefaultInfo.ChildRows.Add(CreateComboBoxEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_RecodID, my_VBValue, sChoices))

            my_DatVarName = "MirrorPart"
            my_DialogDisplay = "Mirroring"
            my_VBValue = 0
            my_CValue = 0
            my_CVarName = "MirrorPart"
            my_DataType = DB_INT
            my_RecodID = 0
            my_sOptions = sEmpty

            catPartDefaultInfo.ChildRows.Add(CreateIntegerEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

            vgPartDefaults.RowHeaderWidth = 100
            vgPartDefaults.Appearance.RowHeaderPanel.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Far


            m_nesting_Template = RegGetString("Nesting", "Template", "")

            If (FileExists(m_nesting_Template)) Then

                Using sr As StreamReader = New StreamReader(m_nesting_Template)

                    While (sr.EndOfStream = False)

                        sData = sr.ReadLine()

                        sPieces = Split(sData, "|")

                        my_DatVarName = sPieces(2)
                        my_DialogDisplay = sPieces(2)
                        my_VBValue = sPieces(3)
                        my_CValue = sPieces(3)
                        my_CVarName = sPieces(2)

                        Select Case ConvertDatTypeToDBType(sPieces(1))

                            Case DB_STR
                                catPartDefaultInfo.ChildRows.Add(CreateStringEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))


                            Case DB_BOOL
                                catPartDefaultInfo.ChildRows.Add(CreateBooleanEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                            Case DB_INT
                                catPartDefaultInfo.ChildRows.Add(CreateIntegerEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                            Case DB_DBL
                                catPartDefaultInfo.ChildRows.Add(CreateDoubleEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                            Case DB_CRIB
                                MessageBox.Show("We should have loaded the tools")
                            Case DB_TOOL
                                Dim nToolSetupID As Long = RegGetString("Global", "ToolSetupID", 0)


                                my_CValue = 0
                                my_VBValue = "None"
                                my_RecodID = 0

                                sql = "SELECT [Station Configuration].[NC Code Number], [Tool Crib].Description, [Tool Setup Members].[ID]" &
                                    " FROM [Tool Crib] INNER JOIN ([Station Configuration] INNER JOIN [Tool Setup Members] " &
                                    "ON [Station Configuration].ID = [Tool Setup Members].[Station ID]) ON " &
                                    "[Tool Crib].ID = [Tool Setup Members].[Tool ID]" &
                                    " WHERE ((([Tool Setup Members].[Tool Setup ID]) = " & nToolSetupID & "))" &
                                    " ORDER BY [Station Configuration].[NC Code Number]"

                                arrOptions = PopulateComboList(sql, False, True)

                                catPartDefaultInfo.ChildRows.Add(CreateComboBoxListEditorRow(vgPartDefaults,
                                                                           my_DatVarName,
                                                                        my_DialogDisplay,
                                                                        my_VBValue,
                                                                        my_CValue,
                                                                        my_CVarName,
                                                                        my_DataType,
                                                                        my_RecodID,
                                                                        arrOptions, 0))


                            Case DB_DCOMBO

                                sChoices.Clear()

                                For ni = 4 To UBound(sPieces)
                                    sChoices.Add(sPieces(ni))
                                Next

                                my_DatVarName = sPieces(2)
                                my_DialogDisplay = sPieces(2)
                                my_VBValue = sPieces(3)
                                my_CValue = sPieces(3)
                                my_CVarName = sPieces(2)
                                my_DataType = DB_DCOMBO
                                my_RecodID = 0
                                my_sOptions = sEmpty

                                catPartDefaultInfo.ChildRows.Add(CreateComboBoxEditorRow(vgPartDefaults, my_DatVarName, my_DialogDisplay, my_RecodID, my_VBValue, sChoices))


                        End Select

                    End While

                    sr.Dispose()
                End Using

            End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    Private Sub PopulateDefaultSettings()
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim myMachineList As MachineListCombo = New MachineListCombo()
        Dim sDefaultName As String = Nothing

        Dim datfilename As String = Nothing

        Dim sTemplate As String = RegGetString("Nesting", "Template", "")

        Try

            vgNestDefaults.SuspendLayout()
            vgNestDefaults.Rows.Clear()
            currentCategoryRow = CreateCategoryRow(vgNestDefaults, "glb.Template",
                                                                    "Template File",
                                                                    sTemplate,
                                                                    sTemplate,
                                                                    "Template",
                                                                    DB_HEADER,
                                                                    0,
                                                                    sEmpty, 0)

            currentCategoryRow.ChildRows.Add(CreateButtonEditEditorRow(vgNestDefaults,
                                                                    "glb.Template",
                                                                    "Template",
                                                                    sTemplate,
                                                                    sTemplate,
                                                                    "Template",
                                                                    DB_FILE,
                                                                    0,
                                                                    "Template Files (*.template);*.template", 0))



            sDefaultName = RegGetString("MRP", "MRP_RESULT_FOLDER", App_Path & "\nests")

            currentCategoryRow.ChildRows.Add(CreateFileEditEditorRow(vgNestDefaults,
                                                                     "glb.ResultPath",
                                                                     "Result Path",
                                                                     sDefaultName,
                                                                     sDefaultName,
                                                                     "Result Path",
                                                                     DB_FOLDER,
                                                                     0,
                                                                     sEmpty, 0))

            arrMachine = PopulateComboList("SELECT * FROM [Machines]", True, False)

            currentCategoryRow = CreateCategoryRow(vgNestDefaults, "glb.Machine",
                                                                         "Machines",
                                                                        s_Machine,
                                                                        m_machine_id,
                                                                        "Machine ID",
                                                                        DB_HEADER,
                                                                        0,
                                                                        sEmpty, 0)

            sDefaultName = RegGetString("Global", "Machine", "")

            s_Machine = cbeMachine.Text
            m_machine_id = modMachine.GetMachineIDFromDescrip(s_Machine)

            If (m_machine_id < 1) Then
                s_Machine = "None"

            End If
            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgNestDefaults,
                                                                         "glb.Machine",
                                                                         "Machine",
                                                                        s_Machine,
                                                                        m_machine_id,
                                                                        "Machine ID",
                                                                        DB_MACHINE,
                                                                        0,
                                                                        arrMachine, 0))


            arrToolSetup = PopulateComboList("SELECT * FROM [Tool Setups] " &
                                             "WHERE ([Machine ID]=" & m_machine_id & ") ORDER BY [Description]", False, False)
            s_ToolSetup = cbeToolSetup.Text

            currentCategoryRow = CreateCategoryRow(vgNestDefaults, "glb.ToolSetupID",
                                                                         "Tool_Setup",
                                                                         s_ToolSetup,
                                                                         0,
                                                                         "Tool Setup ID", DB_HEADER,
                                                                         0, sEmpty, 0)

            Dim n_ToolSetupID As Integer
            sDefaultName = cbeToolSetup.Text ' RegGetString("Global", "ToolSetup", "")
            s_ToolSetup = cbeToolSetup.Text
            n_ToolSetupID = modTranslate.GetToolSetupIDFromDescription(s_ToolSetup)

            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgNestDefaults,
                                                                         "glb.ToolSetupID",
                                                                         "Tool Setup",
                                                                         s_ToolSetup,
                                                                         n_ToolSetupID,
                                                                         "Tool Setup ID",
                                                                         DB_TOOLSETUP,
                                                                         0,
                                                                         arrToolSetup, 0))

            arrLayerSetup = PopulateComboList("SELECT * FROM [Layer Setups] " &
                            "WHERE ([Machine ID]=" & m_machine_id & ") ORDER BY [Description]", False, True)

            currentCategoryRow = CreateCategoryRow(vgNestDefaults, "glb.LayerSetupID",
                                                                         "Layer_Setup",
                                                                         s_LayerSetup,
                                                                         0,
                                                                         "Layer Setup ID",
                                                                         DB_HEADER,
                                                                         0,
                                                                         sEmpty, 0)
            'sDefaultName = RegGetString("Global", "LayerSetup", "None")

            sDefaultName = cbeLayerSetup.Text

            Dim n_LayerSetupID As Integer = modTranslate.GetLayerSetupID(sDefaultName, m_machine_id)

            If (sDefaultName = String.Empty) Then
                sDefaultName = "None"
            End If

            s_LayerSetup = sDefaultName

            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgNestDefaults,
                                                                         "glb.LayerSetupID",
                                                                         "Layer Setup",
                                                                         s_LayerSetup,
                                                                         n_LayerSetupID,
                                                                         "Layer Setup ID",
                                                                         DB_LAYERSETUP,
                                                                         0,
                                                                         arrLayerSetup, 0))

            arrLeadSetup = PopulateComboList("SELECT * FROM [Lead Setup] ORDER BY [Description]", False, True)

            'sDefaultName = RegGetString("Global", "LeadSetup", "None")
            sDefaultName = cbeLeadSetup.Text

            Dim n_LeadSetupID As Integer

            If (sDefaultName = String.Empty) Then
                sDefaultName = "None"
            End If

            s_LeadSetup = sDefaultName
            n_LeadSetupID = modTranslate.GetLeadSetupID(s_LeadSetup)

            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgNestDefaults, "glb.LeadSetupID",
                                                                         "Lead Setup",
                                                                         s_LeadSetup,
                                                                         n_LeadSetupID,
                                                                         "Lead Setup ID",
                                                                         DB_LEADSETUP,
                                                                         0,
                                                                         arrLeadSetup, 0))


            If (FileExists(NormalizePath(App_Path) & s_Machine & ".dat") = True) Then
                datfilename = NormalizePath(App_Path) & s_Machine & ".dat"
            Else
                datfilename = RegGetDatFilePath()
            End If

            populateFromDatFile(vgNestDefaults, datfilename, currentCategoryRow, lbOptions)

            vgNestDefaults.BestFit()
            vgNestDefaults.ResumeLayout()

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub


    Private Function GetLinksFromReg(sTemplateName As String) As String


        GetLinksFromReg = RegGetString("MRP\MRP_TEMPLATE\" & sTemplateName, "Links", "")
        ceMaterialToToolSetup.Checked = RegGetString("MRP\MRP_TEMPLATE\" & sTemplateName, "MaterialToTooletup", 0)
        ceMaterialToLayerSetup.Checked = RegGetString("MRP\MRP_TEMPLATE\" & sTemplateName, "MaterialToLayeretup", 0)

    End Function

    Sub GetSystemfields()
        Dim m_nesting_Template As String
        Dim sDatFileName As String
        Dim sData As String


        SYSTEM_ROOT_NODE = Nothing

        Try

            TV_NestConfig.ClearNodes()

            SYSTEM_MATERIAL_ROOT_NODE = TV_NestConfig.AppendNode(New Object() {"Material Vars"}, SYSTEM_ROOT_NODE, 0)
            SYSTEM_MATERIAL_ROOT_NODE.Tag = 0
            SYSTEM_MATERIAL_NAME_NODE = TV_NestConfig.AppendNode(New Object() {"Material_Name"}, SYSTEM_MATERIAL_ROOT_NODE, TAG_MATERIAL_NAME_NODE)
            SYSTEM_MATERIAL_LENGTH_NODE = TV_NestConfig.AppendNode(New Object() {"Material_Length"}, SYSTEM_MATERIAL_ROOT_NODE, TAG_MATERIAL_LENGTH_NODE)
            SYSTEM_MATERIAL_WIDTH_NODE = TV_NestConfig.AppendNode(New Object() {"Material_Width"}, SYSTEM_MATERIAL_ROOT_NODE, TAG_MATERIAL_WIDTH_NODE)
            SYSTEM_MATERIAL_THICKNESS_NODE = TV_NestConfig.AppendNode(New Object() {"Material_Thickness"}, SYSTEM_MATERIAL_ROOT_NODE, TAG_MATERIAL_THICKNESS_NODE)

            SYSTEM_NESTING_ROOT_NODE = TV_NestConfig.AppendNode(New Object() {"Nesting Vars"}, SYSTEM_ROOT_NODE, 0)

            If (FileExists(CStr(cbeMachine.Text) & ".dat") = True) Then

                sDatFileName = App_Path & "\" & cbeMachine.Text & ".dat"

            Else

                sDatFileName = RegGetDatFilePath()

            End If

            Using sr As StreamReader = New StreamReader(sDatFileName)

                While (sr.EndOfStream = False)

                    sData = sr.ReadLine()

                    sPieces = Split(sData, "|")

                    If (UBound(sPieces) > 1) Then

                        SYSTEM_NESTING_NODE = TV_NestConfig.AppendNode(New Object() {sPieces(2)}, SYSTEM_NESTING_ROOT_NODE, TAG_NESTING_NODE)

                    End If

                End While

                sr.Dispose()

            End Using


            SYSTEM_PART_ROOT_NODE = TV_NestConfig.AppendNode(New Object() {"Part Vars"}, SYSTEM_ROOT_NODE, 0)
            SYSTEM_PART_ROOT_NODE.Tag = 0
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Part Name"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Part Quantity"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Fill Part"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Rotations"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Pre-Rotate"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Priority"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Cost Per Part"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Representation"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Mirroring"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)
            SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {"Pre-Nest"}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)

            m_nesting_Template = RegGetString("Nesting", "Template", "")

            If (FileExists(m_nesting_Template)) Then

                Add_User_Vars_Destination(m_nesting_Template)

            End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub Add_User_Vars_Destination(s_template As String)
        Dim sData As String


        Try

            If (FileExists(s_template) = True) Then

                Using sr As StreamReader = New StreamReader(s_template)

                    While (sr.EndOfStream = False)

                        sData = sr.ReadLine()

                        sPieces = Split(sData, "|")

                        SYSTEM_PART_NODE = TV_NestConfig.AppendNode(New Object() {sPieces(2)}, SYSTEM_PART_ROOT_NODE, TAG_PART_ROOT_NODE)

                    End While

                    sr.Dispose()
                End Using

            End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Sub GetUserFields()
        Dim workbook As IWorkbook = SpreadsheetControl1.Document
        Dim my_WorkSheet As Worksheet = workbook.Worksheets(cboSheetTable.Text)
        Dim ni As Integer
        Dim sUserFieldName As String
        Dim sIndextext As String = sEmpty

        USER_FIELDS_NODE = Nothing

        cbeSortBy.Properties.Items.Clear()
        cbeThenBy.Properties.Items.Clear()
        cbeAndThenBy.Properties.Items.Clear()
        cbeAndThenBy2.Properties.Items.Clear()
        TV_UserFields.ClearNodes()

        chkThenBy.Enabled = False
        chkAndThenBy.Enabled = False
        chkAndThenBy2.Enabled = False

        cbeSortBy.SelectedIndex = -1
        cbeThenBy.SelectedIndex = -1
        cbeAndThenBy.SelectedIndex = -1
        cbeAndThenBy2.SelectedIndex = -1

        Try

            For ni = 0 To my_WorkSheet.GetUsedRange().ColumnCount - 1

                sUserFieldName = my_WorkSheet.Cells(ni).DisplayText
                If (sUserFieldName <> "") Then


                    USER_FIELDS_ROOT_NODE = TV_UserFields.AppendNode(New Object() {sUserFieldName}, USER_FIELDS_NODE, 0)

                    cbeSortBy.Properties.Items.Add(sUserFieldName)
                    cbeThenBy.Properties.Items.Add(sUserFieldName)
                    cbeAndThenBy.Properties.Items.Add(sUserFieldName)
                    cbeAndThenBy2.Properties.Items.Add(sUserFieldName)
                    cboFieldName.Properties.Items.Add(sUserFieldName)
                End If


            Next


            If (cbeSortBy.Properties.Items.Count > -1) Then
                sIndextext = RegGetString("MRP\Sorting", "SortBy", sEmpty)

                For ni = 0 To cbeSortBy.Properties.Items.Count - 1
                    If (UCase(sIndextext) = UCase(cbeSortBy.Properties.Items(ni).ToString)) Then
                        cbeSortBy.SelectedIndex = ni
                    End If

                Next
            End If

            If (cbeThenBy.Properties.Items.Count > -1) Then
                chkThenBy.Enabled = True
                sIndextext = RegGetString("MRP\Sorting", "ThenBy", sEmpty)

                For ni = 0 To cbeThenBy.Properties.Items.Count - 1
                    If (UCase(sIndextext) = UCase(cbeThenBy.Properties.Items(ni).ToString)) Then
                        cbeThenBy.SelectedIndex = ni
                    End If

                Next
            End If

            If (cbeAndThenBy.Properties.Items.Count > -1) Then
                chkAndThenBy.Enabled = True
                sIndextext = RegGetString("MRP\Sorting", "AndThenBy", sEmpty)

                For ni = 0 To cbeAndThenBy.Properties.Items.Count - 1
                    If (UCase(sIndextext) = UCase(cbeAndThenBy.Properties.Items(ni).ToString)) Then
                        cbeAndThenBy.SelectedIndex = ni
                    End If

                Next
            End If

            If (cbeAndThenBy2.Properties.Items.Count > -1) Then
                chkAndThenBy2.Enabled = True
                sIndextext = RegGetString("MRP\Sorting", "AndThenBy2", sEmpty)

                For ni = 0 To cbeAndThenBy2.Properties.Items.Count - 1
                    If (UCase(sIndextext) = UCase(cbeAndThenBy2.Properties.Items(ni).ToString)) Then
                        cbeAndThenBy2.SelectedIndex = ni
                    End If

                Next

            End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub sbBack_Click(sender As Object, e As EventArgs) Handles sbBack.Click

        sbNext.Enabled = True
        sbFinish.Enabled = False

        If (xtcMRP.SelectedTabPageIndex > 0) Then

            xtcMRP.SelectedTabPageIndex = xtcMRP.SelectedTabPageIndex - 1
            If (xtcMRP.SelectedTabPageIndex = 0) Then
                sbBack.Enabled = False
            Else
                sbBack.Enabled = True
            End If
        End If
        If (xtcMRP.SelectedTabPageIndex > 1) Then

        End If


    End Sub

    Private Sub fcbImportFile_ButtonClick(sender As Object, e As Controls.ButtonPressedEventArgs) Handles fcbImportFile.ButtonClick
        Dim sMRP_File As String
        Dim sMRP_FileOpen As New OpenFileDialog
        Dim constring As String
        Dim dt As DataTable
        Dim con As OleDbConnection
        Dim da As OleDbDataAdapter

        cboSheetTable.Properties.Items.Clear()

        Try

            SpreadsheetControl1.CreateNewDocument()

            Select Case m_usage
                Case XLS_TYPE

                    sMRP_FileOpen.Filter = "XLS files (*.xls)|*.xls"
                    sMRP_FileOpen.FilterIndex = 1
                    sMRP_FileOpen.RestoreDirectory = True

                Case CSV_TYPE

                    sMRP_FileOpen.Filter = "CSV files (*.cs)|*.csv"
                    sMRP_FileOpen.FilterIndex = 1
                    sMRP_FileOpen.RestoreDirectory = True

                Case TAB_TYPE

                    sMRP_FileOpen.Filter = "TAB Delimited files (*.txt)|*.txt"
                    sMRP_FileOpen.FilterIndex = 1
                    sMRP_FileOpen.RestoreDirectory = True

                Case SEMICOLON_TYPE

                    sMRP_FileOpen.Filter = "Semi Colon files (*.txt)|*.txt"
                    sMRP_FileOpen.FilterIndex = 1
                    sMRP_FileOpen.RestoreDirectory = True
                Case MDB_TYPE

                    sMRP_FileOpen.Filter = "MDB files (*.mdb)|*.mdb"
                    sMRP_FileOpen.FilterIndex = 1
                    sMRP_FileOpen.RestoreDirectory = True

                Case XLSX_TYPE

                    sMRP_FileOpen.Filter = "XLSX files (*.xlsx)|*.xlsx"
                    sMRP_FileOpen.FilterIndex = 1
                    sMRP_FileOpen.RestoreDirectory = True

            End Select


            If sMRP_FileOpen.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

                sMRP_File = sMRP_FileOpen.FileName
                fcbImportFile.Text = sMRP_File

                fcbImportFile.ToolTip = sMRP_File

                Select Case m_usage

                    Case XLS_TYPE

                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Xls)
                    Case CSV_TYPE, SEMICOLON_TYPE

                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Csv)


                    Case TAB_TYPE
                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Text)
                    Case MDB_TYPE
                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Xls)

                    Case SEMICOLON_TYPE
                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Csv)

                    Case XLSX_TYPE

                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Xlsx)


                End Select
                'If m_usage = CSV_TYPE Then

                '    SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Csv)
                '    PopulateSheetCombo(cboSheetTable)
                '    cboSheetTable.SelectedIndex = 0
                'End If


            End If
            PopulateSheetCombo(cboSheetTable)
            cboSheetTable.SelectedIndex = 0

            'SpreadsheetControl1.ActiveWorksheet.Cells.AutoFitColumns

            'SpreadsheetControl1.Refresh()

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    Private Sub frmMRP_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        Me.Dispose()

    End Sub

    Private Sub frmMRP_Load(sender As Object, e As EventArgs) Handles Me.Load
        Dim machineID As Long
        Dim ToolSetupID As Long
        Dim materialID As Long
        Dim layersetupID As Long
        Dim ni As Integer

        sTempUserFolder = My.Computer.FileSystem.SpecialDirectories.Temp

        fcbImportFile.Text = ""

        cboSheetTable.Properties.Items.Clear()
        Try

#If DIAMOND Then
            lcbFileType.Properties.Items.Add("DiamondLife")
#End If

            sMergePartsFilename = NormalizePath(sTempUserFolder) & "_merpart.txt"

            rgMrpFileType.SelectedIndex = RegGetString("MRP", "MRP_TYPE", "0")

            machineID = modProject.GetMachineID()

            If (DoesMachineExists(machineID) = True) Then

                ToolSetupID = modProject.GetToolSetupID()
                layersetupID = modProject.GetLayerSetupID

            Else

                machineID = 0
                ToolSetupID = 0
                layersetupID = 0

            End If

            materialID = modProject.GetMaterialID()

            If (DoesMaterialExist(materialID) = False) Then

                materialID = 0

            End If

            modProject.InitMachineList(cbeMachine, machineID)
            modProject.InitToolSetupList(cbeToolSetup, machineID, ToolSetupID)
            modProject.InitMaterialList(cbeMaterial, materialID)
            modProject.InitLayerSetupList(cbeLayerSetup, machineID, layersetupID)

            arrLeadSetup = PopulateComboList("SELECT * FROM [Lead Setup] ORDER BY [Description]", False, True)

            For ni = 1 To arrLeadSetup.Count

                cbeLeadSetup.Properties.Items.Add(arrLeadSetup.Item(ni).description)

            Next

            cbeLeadSetup.SelectedIndex = 0


            optPDBName.SelectedIndex = 0

            DateEdit1.EditValue = Now
            GetDefaultsFromRegistry()

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub GetDefaultsFromRegistry()
        Dim ni As Integer
        Dim sSheetTableName As String

        Try

            chkHeader.CheckState = IIf(RegGetString("MRP", "MRP_HAS_HEADER", "0") = True, 1, 0)
            chkHasPath.CheckState = IIf(RegGetString("MRP", "MRP_HAS_PATH", "0") = True, 1, 0)
            chkAppendMat.CheckState = IIf(RegGetString("MRP", "MRP_APPEND_MATERIAL", "0") = True, 1, 0)
            chkCopySource.CheckState = IIf(RegGetString("MRP", "MRP_COPY_SOURCE", "0") = True, 1, 0)


            fcbSourceFolder.Text = RegGetString("MRP", "MRP_SOURCE_FOLDER", "")
            fcbResultsDirectory.Text = RegGetString("MRP", "MRP_RESULT_FOLDER", "")
            fcbMM2Source.Text = RegGetString("MRP", "MM2_SOURCE_FOLDER", "")
            sSheetTableName = RegGetString("MRP", "MRP_WORKSHEET", "")


            intImportType = RegGetString("MRP", "MRP_TYPE", "0")

            rgMrpFileType.SelectedIndex = intImportType


            fcbImportFile.Text = RegGetString("MRP", "MRP_FILE", "")

            lcbFileType.SelectedIndex = RegGetString("MRP", "MRP_SOURCE_TYPE", "0")

            optPDBName.SelectedIndex = RegGetString("MRP", "optPDBName", "0")

            'fcbImportFile.Enabled = False

            teProvidedName.Enabled = False

            cboFieldName.Enabled = False



            Select Case optPDBName.SelectedIndex
                Case 0
                    fcbImportFile.Enabled = True
                    teImportFilename.Text = PathFile(fcbImportFile.Text)

                Case 1
                    teProvidedName.Enabled = True
                    teProvidedName.Text = "<Enter Job Name>"
                Case 2
                    cboFieldName.Enabled = True



            End Select
            If (fcbImportFile.Text <> "") Then

                For ni = 0 To cboSheetTable.Properties.Items.Count - 1
                    If (cboSheetTable.Properties.Items(ni).ToString = sSheetTableName) Then
                        cboSheetTable.SelectedIndex = ni
                    End If
                Next

            End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub

    Private Sub fcbImportFile_EditValueChanged(sender As Object, e As EventArgs) Handles fcbImportFile.EditValueChanged

        Try

            Select Case m_usage
                Case XLS_TYPE
                    If (fcbImportFile.Text <> "") Then

                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Xls)
                        PopulateSheetCombo(cboSheetTable)

                    End If
                    cboSheetTable.SelectedIndex = -1

                Case CSV_TYPE
                    If (fcbImportFile.Text <> "") Then

                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Csv)
                        PopulateSheetCombo(cboSheetTable)
                        'Dim myTable As Table = SpreadsheetControl1.ActiveWorksheet.Tables(0)

                        'myTable.ShowHeaders = True
                        'myTable.ShowTotals = True
                        '' Apply banded column formatting to the table.
                        'myTable.ShowTableStyleRowStripes = False
                        'myTable.ShowTableStyleColumnStripes = True

                        ''ApplyInitialBuiltInTableStyle()

                        'PopulateSheetCombo(cboSheetTable)

                    End If
                    cboSheetTable.SelectedIndex = 0

                Case TAB_TYPE

                    If (fcbImportFile.Text <> "") Then

                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Text)
                        PopulateSheetCombo(cboSheetTable)

                    End If
                    cboSheetTable.SelectedIndex = 0

                Case SEMICOLON_TYPE

                    If (fcbImportFile.Text <> "") Then

                        SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Text)
                        PopulateSheetCombo(cboSheetTable)

                    End If
                    cboSheetTable.SelectedIndex = 0

                Case MDB_TYPE
                    If (fcbImportFile.Text <> "") Then

                        PopulateMDB_Tables_To_Combo(fcbImportFile.Text)

                    End If

                    cboSheetTable.SelectedIndex = 0

                Case XLSX_TYPE

                    SpreadsheetControl1.LoadDocument(fcbImportFile.Text, DocumentFormat.Xlsx)
                    PopulateSheetCombo(cboSheetTable)

                    cboSheetTable.SelectedIndex = -1

            End Select

            teImportFilename.Text = PathFile(fcbImportFile.Text)

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub ApplyInitialBuiltInTableStyle()
        '            #Region "#ApplyInitialBuiltInTableStyle"
        SpreadsheetControl1.BeginUpdate()
        ' Access the workbook's collection of table styles.
        Dim tableStyles As TableStyleCollection = SpreadsheetControl1.Document.TableStyles
        ' Access the built-in table style from the collection by its Id.
        Dim tableStyle As TableStyle = tableStyles(BuiltInTableStyleId.TableStyleDark9)

        ' Apply the table style to the existing table.
        Dim myTable As Table = SpreadsheetControl1.ActiveWorksheet.Tables(0)
        myTable.Style = tableStyle

        ' Show header and total rows.
        myTable.ShowHeaders = True
        myTable.ShowTotals = True
        ' Apply banded column formatting to the table.
        myTable.ShowTableStyleRowStripes = False
        myTable.ShowTableStyleColumnStripes = True

        SpreadsheetControl1.EndUpdate()
        '            #End Region ' #ApplyInitialBuiltInTableStyle
    End Sub
    Private Sub SetDefaultsInRegistry()
        Dim nToolSetupId As Long
        Dim nLayerSetupId As Long
        Dim nLeadSetupId As Long
        Dim nMachineSetupId As Long
        Dim nMaterialSetupId As Long

        Try

            nMachineSetupId = GetMachineIDFromDescrip(cbeMachine.Text)

            nToolSetupId = GetToolSetupIDFromDescription(cbeToolSetup.Text)

            nLayerSetupId = modTranslate.GetLayerSetupID(cbeLayerSetup.Text, nMachineSetupId)

            nLeadSetupId = GetLeadSetupID(cbeLeadSetup.Text)

            nMaterialSetupId = GetMaterialIDFromDescription(cbeMaterial.Text)

            SetToolSetup(cbeToolSetup.Text, nToolSetupId)

            SetLayerSetup(cbeLayerSetup.Text, nLayerSetupId)

            SetLeadSetup(cbeLeadSetup.Text, nLeadSetupId)

            SetMachine(cbeMachine.Text, nMachineSetupId)
            SetMaterial(cbeMaterial.Text, nMaterialSetupId)

            RegPutString("MRP", "MRP_FILE", fcbImportFile.Text)
            RegPutString("MRP", "MRP_TYPE", intImportType)
            RegPutString("MRP", "MRP_WORKSHEET", cboSheetTable.Text)
            RegPutString("MRP", "MRP_HAS_HEADER", chkHeader.Checked)
            RegPutString("MRP", "MRP_HAS_PATH", chkHasPath.Checked)
            RegPutString("MRP", "MRP_APPEND_MATERIAL", chkAppendMat.Checked)
            RegPutString("MRP", "MRP_COPY_SOURCE", chkCopySource.Checked)
            RegPutString("MRP", "MRP_SOURCE_TYPE", lcbFileType.SelectedIndex)
            RegPutString("MRP", "MRP_SOURCE_FOLDER", fcbSourceFolder.Text)
            RegPutString("MRP", "MRP_RESULT_FOLDER", fcbResultsDirectory.Text)
            RegPutString("MRP", "MM2_SOURCE_FOLDER", fcbMM2Source.Text)

            RegPutString("MRP", "optPDBName", optPDBName.SelectedIndex)

            If (cboFieldName.SelectedIndex > -1) Then

                RegPutString("MRP", "USER_FIELD", cboFieldName.Properties.Items.Item(cboFieldName.SelectedIndex).ToString)

            Else

                RegPutString("MRP", "USER_FIELD", -1)

            End If

            If (cbeSortBy.SelectedIndex > -1) Then
                RegPutString("MRP\Sorting", "SortBy", cbeSortBy.Text)
            End If

            If (cbeThenBy.SelectedIndex > -1) Then
                RegPutString("MRP\Sorting", "ThenBy", cbeThenBy.Text)
            End If

            If (cbeAndThenBy.SelectedIndex > -1) Then
                RegPutString("MRP\Sorting", "AndThenBy", cbeAndThenBy.Text)
            End If

            If (cbeAndThenBy2.SelectedIndex > -1) Then
                RegPutString("MRP\Sorting", "AndThenBy2", cbeAndThenBy2.Text)
            End If


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub
    Private Sub cboSheetTable_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboSheetTable.SelectedIndexChanged

        Try

            Select Case m_usage
                Case XLS_TYPE

                Case TAB_TYPE

                Case SEMICOLON_TYPE

                Case MDB_TYPE
                    If (fcbImportFile.Text <> "") Then
                        Dim connectionString As String = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & fcbImportFile.Text

                        ' Create the dataset and add the Categories table to it:
                        Dim myAccessConn As OleDbConnection = Nothing
                        Try
                            myAccessConn = New OleDbConnection(connectionString)
                        Catch ex As Exception
                            MessageBox.Show("Error: Failed to create a database connection. " & Constants.vbCrLf & ex.Message, "Error")
                            Return
                        End Try
                        Try
                            myAccessConn.Open()

                            SpreadsheetControl1.BeginUpdate()
                            SpreadsheetControl1.Document.CreateNewDocument()
                            ' We only want user tables, not system tables
                            Dim restrictions(3) As String
                            restrictions(3) = "Table"
                            ' Get list of user tables
                            Dim userTables As DataTable = myAccessConn.GetSchema("Tables", restrictions)
                            ' Getting the data for every user tables

                            Dim currentTableName As String = cboSheetTable.Text  'userTablesRow("TABLE_NAME").ToString()

                            Using cmd As New OleDbCommand(String.Format("SELECT * FROM [{0}]", currentTableName), myAccessConn)

                                Using adapter As New OleDbDataAdapter(cmd)
                                    Dim currentDataTable As New DataTable(currentTableName)
                                    adapter.Fill(currentDataTable)
                                    LoadDataTableContentIntoSpreadsheetControl(currentDataTable)
                                End Using


                            End Using

                            SpreadsheetControl1.EndUpdate()

                        Catch ex As Exception
                            MessageBox.Show("Error: Failed to retrieve the required data from the DataBase." & Constants.vbCrLf & ex.Message, "Error")
                            Return

                        Finally
                            myAccessConn.Close()

                        End Try

                    End If

            End Select

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub
    Private Sub LoadDataTableContentIntoSpreadsheetControl(ByVal currentTable As DataTable)
        Dim workSheet As Worksheet = SpreadsheetControl1.Document.Worksheets(0)

        Try

            SpreadsheetControl1.Document.Worksheets(0).Name = currentTable.TableName

            For i As Integer = 0 To currentTable.Columns.Count - 1
                workSheet.Cells(0, i).Value = currentTable.Columns(i).ColumnName
            Next i

            For i As Integer = 0 To currentTable.Columns.Count - 1
                For j As Integer = 0 To currentTable.Rows.Count - 1
                    workSheet.Cells(j + 2, i).Value = currentTable.Rows(j)(i).ToString()
                Next j
            Next i
            Dim table As Table = workSheet.Tables.Add(workSheet.GetDataRange(), True)
            table.Style = SpreadsheetControl1.Document.TableStyles(BuiltInTableStyleId.TableStyleLight1)
            workSheet.Columns.AutoFit(0, workSheet.Columns.LastUsedIndex)

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub
    Private Sub fcbSourceFolder_ButtonClick(sender As Object, e As Controls.ButtonPressedEventArgs) Handles fcbSourceFolder.ButtonClick
        Dim m_folderDialog As New FolderBrowserDialog()

        If m_folderDialog.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            fcbSourceFolder.Text = m_folderDialog.SelectedPath

        End If

    End Sub

    Private Sub fcbResultsDirectory_ButtonClick(sender As Object, e As Controls.ButtonPressedEventArgs)
        Dim m_folderDialog As New FolderBrowserDialog()

        If m_folderDialog.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            fcbResultsDirectory.Text = m_folderDialog.SelectedPath

        End If
    End Sub

    Private Sub fcbMM2Source_ButtonClick(sender As Object, e As Controls.ButtonPressedEventArgs)
        Dim m_folderDialog As New FolderBrowserDialog()

        If m_folderDialog.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            fcbMM2Source.Text = m_folderDialog.SelectedPath

        End If
    End Sub

    Private Sub cbeMachine_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeMachine.SelectedIndexChanged

        If (cbeMachine.SelectedIndex > -1) Then

            Dim machineid As Long = (TryCast(cbeMachine.SelectedItem, myItemData).ItemData.ToString())

            modProject.InitToolSetupList(cbeToolSetup, machineid, 0)
            modProject.InitLayerSetupList(cbeLayerSetup, machineid, 0)
            SetMachine(cbeMachine.Text, machineid)


        End If
    End Sub

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        bDo_MRP_Nesting = False

        Dim skin As Skin = CommonSkins.GetSkin(UserLookAndFeel.Default)

        DnDHighlightHelper1.AppearanceHighlight.BackColor = skin.Colors.GetColor(CommonColors.Highlight)

        ' Add any initialization after the InitializeComponent() call.
        AddHandler UserLookAndFeel.Default.StyleChanged, AddressOf Default_StyleChanged

        xtcMRP.ShowTabHeader = DevExpress.Utils.DefaultBoolean.False

    End Sub
    Private Sub Default_StyleChanged(sender As Object, e As EventArgs)
        SetFrozenRowColor()
    End Sub

    Private Sub SetFrozenRowColor()

        Dim formatting As Formatting = SpreadsheetControl1.ActiveWorksheet.Rows(0).BeginUpdateFormatting()
        Dim skin As Skin = CommonSkins.GetSkin(UserLookAndFeel.Default)

        formatting.Fill.BackgroundColor = skin.Colors.GetColor(CommonColors.ReadOnly)

        SpreadsheetControl1.ActiveWorksheet.Rows(0).EndUpdateFormatting(formatting)

    End Sub
    Private Sub SaveLinkData()

        m_templateName = RegGetString("MRP", "MRP_TEMPLATE", "")

        RegPutString("MRP\MRP_TEMPLATE\" & m_templateName, "Links", CreateStringFromLinks())
        RegPutString("MRP\MRP_TEMPLATE\" & m_templateName, "MaterialToTooletup", ceMaterialToToolSetup.Checked)
        RegPutString("MRP\MRP_TEMPLATE\" & m_templateName, "MaterialToLayeretup", ceMaterialToLayerSetup.Checked)


    End Sub

    Private Function CreateStringFromLinks() As String

        CreateStringFromLinks = ""

        For ni = 0 To TV_MappedFields.Nodes.Count - 1

            CreateStringFromLinks = CreateStringFromLinks & TV_MappedFields.Nodes.Item(ni).GetDisplayText(0) & ","

        Next

        If Mid$(CreateStringFromLinks, Len(CreateStringFromLinks), 1) = "," Then
            CreateStringFromLinks = Mid$(CreateStringFromLinks, 1, Len(CreateStringFromLinks) - 1)
        End If

    End Function


    Private Sub sbFinish_Click(sender As Object, e As EventArgs) Handles sbFinish.Click
        Dim ni As Integer
        Dim sWorkSheetTable As String = sEmpty
        Dim workbook As IWorkbook = SpreadsheetControl1.Document

        Try
            ' SplashScreenManager.ShowForm(GetType(WaitForm1))
            Windows.Forms.Cursor.Current = Cursors.WaitCursor


            sWorkSheetTable = cboSheetTable.Text

            Dim worksheet As Worksheet = workbook.Worksheets(sWorkSheetTable)
            workbook.BeginUpdate()

            Dim dataRange As Range = worksheet.GetDataRange()
            Dim sortRange As Range = worksheet.Range.FromLTRB(dataRange.LeftColumnIndex, dataRange.TopRowIndex + 1, dataRange.RightColumnIndex, dataRange.BottomRowIndex)
            Dim sortCriteria As New List(Of SortField)()

            If (cbeSortBy.SelectedIndex > -1) Then
                cFirstCell = worksheet.Search(cbeSortBy.Text).FirstOrDefault()
                sortCriteria.Add(New SortField() With {.ColumnOffset = cFirstCell.LeftColumnIndex, .Comparer = worksheet.Comparers.Ascending})
            End If

            If (cbeThenBy.SelectedIndex > -1) Then
                cSecondCell = worksheet.Search(cbeThenBy.Text).FirstOrDefault()
                sortCriteria.Add(New SortField() With {.ColumnOffset = cSecondCell.LeftColumnIndex, .Comparer = worksheet.Comparers.Ascending})
            End If

            If (cbeAndThenBy.SelectedIndex > -1) Then
                cThridCell = worksheet.Search(cbeAndThenBy.Text).FirstOrDefault()
                sortCriteria.Add(New SortField() With {.ColumnOffset = cThridCell.LeftColumnIndex, .Comparer = worksheet.Comparers.Ascending})
            End If

            If (cbeAndThenBy2.SelectedIndex > -1) Then
                cFourthCell = worksheet.Search(cbeAndThenBy2.Text).FirstOrDefault()
                sortCriteria.Add(New SortField() With {.ColumnOffset = cFourthCell.LeftColumnIndex, .Comparer = worksheet.Comparers.Ascending})
            End If

            worksheet.Sort(sortRange, sortCriteria)
            workbook.EndUpdate()

            For ni = 1 To worksheet.Rows.LastUsedIndex

            Next

            SetDefaultsInRegistry()

#If APIHeatTransfer Then
            sTempMDB = sTempUserFolder & "\tempmdb.mdb"
            ConvertXLSToMDB(fcbImportFile.Text, sTempMDB, cboSheetTable.Text)

            CreateMasterPDBaNdSubPDBs()

#Else
            GetPDBNames()

            FillPartListInPDBs()

#End If

            PathPDB_Set(sMRP_DB)

            bDo_MRP_Nesting = True

            Windows.Forms.Cursor.Current = Cursors.Default

            'SplashScreenManager.CloseForm()

            MessageBox.Show("Conversion completed!", "Conversion Completed", MessageBoxButtons.OK, MessageBoxIcon.Information)

            Me.Close()

        Catch ex As Exception
            Windows.Forms.Cursor.Current = Cursors.Default

            MessageBox.Show("Conversion Failed!", "Conversion Failed! " & ex.Message, MessageBoxButtons.OK, MessageBoxIcon.Stop)
        End Try
    End Sub

    Private Sub CreateMasterPDBaNdSubPDBs()
        Dim pdb_store_name As String = Nothing
        Dim pdb_name As String
        Dim pdb_path As String
        Dim pdb_Path_Filename As String = Nothing
        Dim LabelDBName As String
        Dim cat As ADOX.Catalog = New ADOX.Catalog()
        Dim ws As Worksheet
        Dim workbook As IWorkbook
        Dim cMyFirstCell As Cell
        Dim cMySecondCell As Cell
        Dim cMyThirdCell As Cell
        Dim cMyFourthCell As Cell
        Dim cMyFifthCell As Cell
        Dim sMyFirstCellText As String
        Dim sMySecondCellText As String = Nothing
        Dim range As Range
        Dim pdb_info As NESTDBINFO
        Dim sResultPath As String
        Dim l_MachineID As Long
        Dim l_LayerSetupID As Long
        Dim l_LeadSetupID As Long
        Dim l_ToolSetupid As Long
        Dim l_Mat_ID As Long = -1
        Dim l_Mat_Type_ID As Long = -1
        Dim s_MatTypeDescrip As String
        Dim sConstructedPDBName As String = Nothing
        Dim nSpreadSheetRowCnt As Long
        Dim b_UseMatForToolSetup As Boolean
        Dim b_UseMatForLayerSetup As Boolean
        Dim sMappedString As String
        Dim sSearchText As String = Nothing
        Dim npos As Integer
        Dim sMappedName As String
        Dim niPart As Integer
        Dim nStartRange As Integer
        Dim nEndRange As Integer
        Dim nCounter As Integer = 1
        Dim sMySQL As String
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim sMatCode As String
        Dim sFullPartName As String


        Try
            m_XLS_Name = fcbImportFile.Text

            m_job_name = "Job_" & PathFile(m_XLS_Name)

            pdb_store_name = Nothing

            s_Machine = cbeMachine.Text
            l_MachineID = GetMachineIDFromDescrip(s_Machine)


            s_ToolSetup = cbeToolSetup.Text

            l_ToolSetupid = GetToolSetupIDFromDescription(s_ToolSetup)
            s_LeadSetup = cbeLeadSetup.Text
            l_LeadSetupID = GetLeadSetupID(s_LeadSetup)
            s_LayerSetup = cbeLayerSetup.Text
            l_LayerSetupID = modTranslate.GetLayerSetupID(s_LayerSetup, l_MachineID)

            sMRP_DB = NormalizePath(PathDriveFolder(m_XLS_Name)) & m_job_name & ".mrp"

            If (FileExists(sMRP_DB) = True) Then

                ConditionalFileDelete(sMRP_DB)

            End If

            cat.Create("Provider=Microsoft.Jet.OLEDB.4.0;" &
                               "Data Source=" & sMRP_DB & ";" &
                               "Jet OLEDB:Engine Type=5")

            cat.ActiveConnection.close()

            modNesting.MRPDBNAME = sMRP_DB

            CreateMRPTable(sMRP_DB)

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sTempMDB & "';"

            sMySQL = "Select* From [" & cboSheetTable.Text & "] Order By [RM Part]"

            connection = New OleDbConnection(connetionString)

            oledbAdapter = New OleDbDataAdapter(sMySQL, connection)

            oledbAdapter.Fill(ds)

            oledbAdapter.Dispose()

            connection.Dispose()
            connection.Close()

            For i = 0 To ds.Tables(0).Rows.Count - 1

                sFullPartName = ds.Tables(0).Rows(i).Item("RM Name").ToString

                sMatCode = ds.Tables(0).Rows(i).Item("RM Part").ToString
                pdb_name = sMatCode



                l_Mat_Type_ID = GetMaterialTypeIDFromDisplay(sMatCode)

                l_Mat_ID = Get1stMaterialID(l_Mat_Type_ID)

                s_MatTypeDescrip = ds.Tables(0).Rows(i).Item("RM Name").ToString

                'pdb_name= 
                If (InStr(UCase(s_MatTypeDescrip), "ALUM") > 0) Then
                    pdb_Path_Filename = "c:\Laser\" & sAlumFolder & "\" & sMRPNestFolder & "\" & m_job_name & "_" & sMatCode & ".pdb"
                    sResultPath = "c:\Laser\" & sAlumFolder & "\" & sMRPNestFolder & "\"
                Else
                    pdb_Path_Filename = "c:\Laser\" & sSteelFolder & "\" & sMRPNestFolder & "\" & m_job_name & "_" & sMatCode & ".pdb"

                    sResultPath = "c:\Laser\" & sSteelFolder & "\" & sMRPNestFolder & "\"
                End If

                l_Mat_ID = modTranslate.GetMaterialIDFromDescription(s_MatTypeDescrip)

                l_Mat_Type_ID = GetMaterialTypeIDFromDescription(s_MatTypeDescrip)

                d_MatLength = modMaterial.GetMaterialLength(l_Mat_ID)

                d_MatWidth = modMaterial.GetMaterialWidth(l_Mat_ID)

                d_MatThickness = modMaterial.GetMaterialThickness(l_Mat_ID)

                d_MatQty = modMaterial.GetMaterialQty(l_Mat_ID)

                d_MatWeight = modMaterial.CheckMaterialWeight(l_Mat_ID)

                d_MatCost = modMaterial.GetMaterialCost(l_Mat_ID)

                If (UCase(Trim(pdb_name)) = UCase(Trim(pdb_store_name)) = False) Then
                    PathPDB_Set(pdb_Path_Filename)

                    pdb_store_name = pdb_name

                    AddPdbToTable(sMRP_DB, pdb_Path_Filename)

                    If (FileExists(pdb_Path_Filename) = True) Then

                        ConditionalFileDelete(pdb_Path_Filename)

                    End If

                    'SplashScreenManager.Default.SetWaitFormDescription("Creating Nesting Database" & pdb_store_name)

                    pdb_info = InitPdbInfo(
                                pdb_Path_Filename, sResultPath, s_Machine, l_MachineID,
                                s_ToolSetup, l_ToolSetupid, s_MatTypeDescrip, l_Mat_ID,
                                s_LeadSetup, l_LeadSetupID,
                                s_LayerSetup, l_LayerSetupID)


                    If (EstablishPDB(pdb_info) = String.Empty) Then
                        MsgBox("Unable to create nesting database. " & vbCrLf & pdb_name)
                    Else
                        modMRP.AddMaterialToMaterialInventorys(pdb_Path_Filename, s_MatTypeDescrip, l_Mat_Type_ID, d_MatLength, d_MatWidth, d_MatThickness, d_MatWeight, d_MatQty, d_MatCost)

                        UpdateSetupTable(pdb_Path_Filename, vgNestDefaults, s_MatTypeDescrip, b_UseMatForToolSetup, b_UseMatForLayerSetup)
                    End If

                End If


                If (l_Mat_ID > 0) Then
#If APIHeatTransfer Then

                    AddAPIHeatPart(pdb_Path_Filename, s_MatTypeDescrip, ds, i)
#Else

                    AddNormalPart(pdb_Path_Filename, niPart, SpreadsheetControl1)
#End If

                End If

            Next

        Catch ex As Exception

            MessageBox.Show("CreateMasterPDBaNdSubPDBs: " & ex.Message)

        End Try

    End Sub
    Private Sub FillPartListInPDBs()
        Dim sPartName As String
        Dim sFields As String
        Dim sValues As String
        Dim sdata As String
        Dim sPieces() As String

        Using sr As StreamReader = New StreamReader(sMergePartsFilename)

            While (sr.EndOfStream = False)
                sdata = sr.ReadLine

                If (sdata <> " ") Then
                    Dim m_cnPart As New OleDb.OleDbConnection
                    Dim m_daPart As New OleDb.OleDbDataAdapter
                    Dim m_dtPart As New DataTable
                    Dim connetionString As String
                    Dim sql As String


                    sPieces = Split(sdata, "|")
                    sPartName = sPieces(1)
                    sFields = "[Filename],[Part Quantity],[Fill Part],[Rotation Num], [MirrorPart],[Rotation Start],[Part Pass],[Part Priority],[Cost Per Part], [Representation],[Special Priority],[Hold ID], [PreNest], [Part_Extents]"
                    sValues = sPieces(2)

                    connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPieces(0) & " ';"

            m_cnPart = New OleDbConnection(connetionString)

                    m_cnPart.Open()

                    sql = "INSERT INTO [Part] (" & sFields & " ) VALUES(" & sValues & ")"


                    m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

                    lReturn = m_daPart.InsertCommand.ExecuteNonQuery()

                    m_daPart.Dispose()
                    m_daPart = Nothing
                    m_cnPart.Dispose()
                    m_cnPart = Nothing
                End If


            End While
            sr.Dispose()
        End Using

    End Sub
    Private Sub PopulateSheetCombo(cboComboBox As DevExpress.XtraEditors.ComboBoxEdit)
        Dim sWorkSheetName As String
        Dim workbook As IWorkbook = SpreadsheetControl1.Document

        For ni = 0 To workbook.Worksheets.Count - 1

            sWorkSheetName = workbook.Worksheets(ni).Name

            cboComboBox.Properties.Items.Add(sWorkSheetName)

        Next

    End Sub

    Private Overloads Sub OnDragDrop(ByVal sender As Object, ByVal e As DragEventArgs) Handles TV_NestConfig.DragDrop, TV_UserFields.DragDrop
        Dim node As TreeListNode = GetDragNode(e.Data)
        If node Is Nothing Then
            Return
        End If
        Dim list As TreeList = DirectCast(sender, TreeList)
        If list Is node.TreeList Then
            Return
        End If

        Dim info As TreeListHitInfo = list.CalcHitInfo(list.PointToClient(New Point(e.X, e.Y)))

        If info.Node IsNot Nothing Then

            Me.TV_MappedFields.BeginUnboundLoad()
            Dim parentForRootNodes As TreeListNode = Nothing

            Dim resultData As String = String.Empty
            Dim firstData As Object = node.GetValue(Me.TV_UserFields.Columns(0))
            Dim secondData As Object = info.Node.GetValue(Me.TV_NestConfig.Columns(0))
            If firstData IsNot Nothing Then
                resultData &= firstData.ToString() & " = "
            End If

            If secondData IsNot Nothing Then
                resultData &= secondData.ToString()
            End If
            Dim rootNode As TreeListNode = Me.TV_MappedFields.AppendNode(New Object() {resultData}, parentForRootNodes)
            Me.TV_MappedFields.EndUnboundLoad()
            DnDHighlightHelper1.prevNode = Nothing
            DnDHighlightHelper1.hotTrackNode_Renamed = Nothing
        End If

    End Sub

    Private Function GetDragNode(ByVal data As IDataObject) As TreeListNode
        Return DirectCast(data.GetData(GetType(TreeListNode)), TreeListNode)
    End Function

    Private Overloads Sub OnDragEnter(ByVal sender As Object, ByVal e As DragEventArgs) Handles TV_UserFields.DragEnter, TV_NestConfig.DragEnter
        Dim list As TreeList = DirectCast(sender, TreeList)
        Dim node As TreeListNode = GetDragNode(e.Data)
        If node IsNot Nothing AndAlso node.TreeList IsNot list Then
            e.Effect = DragDropEffects.Link

        End If
    End Sub

    Private Sub cbeSortBy_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeSortBy.SelectedIndexChanged
        If (cbeSortBy.Text <> "None") Then
            chkSortBy.Enabled = True
        End If
    End Sub

    Private Sub cbeThenBy_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeThenBy.SelectedIndexChanged
        If (cbeThenBy.Text <> "None") Then
            cbeThenBy.Enabled = True
        End If
    End Sub

    Private Sub cbeAndThenBy_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeAndThenBy.SelectedIndexChanged
        If (cbeAndThenBy.Text <> "None") Then
            cbeAndThenBy.Enabled = True
        End If
    End Sub

    Private Sub ExportOneTable(sMDB_Name As String, sMDB_Table As String)
        Dim connetionString As String
        Dim m_cnSettings As New OleDb.OleDbConnection
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim strExcelFile As String

        Dim sql As String

        strExcelFile = PathTempFolder() & PathFile(sMDB_Name) & ".xls"

        Try
            ConditionalFileDelete(strExcelFile)


            sql = "SELECT * INTO [" & sMDB_Table & "] IN '" & strExcelFile & "' 'Excel 8.0;' FROM " & sMDB_Table

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sMDB_Name & "';"

            m_cnSettings = New OleDbConnection(connetionString)

            m_cnSettings.Open()

            m_daSettings.SelectCommand = New OleDbCommand(sql, m_cnSettings)

            m_daSettings.SelectCommand.ExecuteNonQuery()

            m_cnSettings.Close()

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub PopulateMDB_Tables_To_Combo(sMDB_Name As String)
        Dim userTables As DataTable = Nothing

        Dim i As Integer

        Dim restrictions() As String = New String(3) {}

        Try

            restrictions(3) = "Table"

            Dim connection As New OleDbConnection

            connection.ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & sMDB_Name

            'Populate a DataTable with a list of all tables in the connected database.
            connection.Open()

            userTables = connection.GetSchema("Tables", restrictions)

            connection.Close()

            For i = 0 To userTables.Rows.Count - 1

                cboSheetTable.Properties.Items.Add(userTables.Rows(i)(2).ToString())

            Next

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub

    Private Sub rgMrpFileType_SelectedIndexChanged(sender As Object, e As EventArgs) Handles rgMrpFileType.SelectedIndexChanged
        m_usage = rgMrpFileType.SelectedIndex
        RegPutString("MRP", "MRP_TYPE", m_usage)
    End Sub

    Private Sub GetSpreadSheet()
        Dim workbook As IWorkbook = SpreadsheetControl1.Document
        Dim ni As Integer

        Try

            For ni = 0 To workbook.Worksheets.Count - 1

                If (workbook.Worksheets(ni).Name <> cboSheetTable.EditValue) Then

                    workbook.Worksheets(ni).VisibilityType = WorksheetVisibilityType.VeryHidden

                End If
            Next

            workbook.Worksheets.ActiveWorksheet = workbook.Worksheets(CStr(cboSheetTable.EditValue))
            workbook.Worksheets.ActiveWorksheet.Columns.AutoFit(0, workbook.Worksheets.ActiveWorksheet.Columns.LastUsedIndex)
            SpreadsheetControl1.Document.Worksheets(CStr(cboSheetTable.EditValue)).FreezeRows(0)

            SetFrozenRowColor()
            'SpreadsheetControl1.Refresh()

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    Private Function GetColIndexFromText(sSearchText As String, wsActiveWorkSheet As DevExpress.Spreadsheet.Worksheet) As Integer
        Dim ni As Integer
        Dim lastCol = wsActiveWorkSheet.Columns.LastUsedIndex
        Dim m_DisplayText As String
        Dim gary As String
        ' Dim range As Range = wsActiveWorkSheet.Range.FromLTRB(0, 0, lastCol, 0)

        sSearchText = UCase(sSearchText)
        GetColIndexFromText = -1

        For ni = 0 To lastCol
            gary = wsActiveWorkSheet.GetCellValue(ni, 0).ToString
            ' m_DisplayText = UCase(range(ni).DisplayText)
            m_DisplayText = UCase(wsActiveWorkSheet.Cells(0, ni).DisplayText)
            sSearchText = UCase(sSearchText)

            If m_DisplayText = sSearchText Then
                GetColIndexFromText = ni
                Exit For

            End If
        Next

    End Function

    Private Sub fcbResultsDirectory_ButtonClick1(sender As Object, e As Controls.ButtonPressedEventArgs) Handles fcbResultsDirectory.ButtonClick
        Dim m_folderDialog As New FolderBrowserDialog()

        If m_folderDialog.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            fcbResultsDirectory.Text = m_folderDialog.SelectedPath

        End If

    End Sub

    Private Sub fcbMM2Source_ButtonClick1(sender As Object, e As Controls.ButtonPressedEventArgs) Handles fcbMM2Source.ButtonClick
        Dim m_folderDialog As New FolderBrowserDialog()

        If m_folderDialog.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            fcbMM2Source.Text = m_folderDialog.SelectedPath

        End If

    End Sub

    Private Sub TV_MappedFields_MouseClick(sender As Object, e As MouseEventArgs) Handles TV_MappedFields.MouseClick

        If (e.Button = Windows.Forms.MouseButtons.Right) Then

            PopupMenu1.ShowPopup(Control.MousePosition)

        End If
    End Sub

    Private Sub cbeToolSetup_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeToolSetup.SelectedIndexChanged

        If (cbeToolSetup.SelectedIndex > -1) Then

            Dim ntoolsetupid As Long = (TryCast(cbeToolSetup.SelectedItem, myItemData).ItemData.ToString())

            SetToolSetup(cbeToolSetup.Text, ntoolsetupid)

        End If

    End Sub

    Private Sub cbeMaterial_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeMaterial.SelectedIndexChanged

        If (cbeMaterial.SelectedIndex > -1) Then

            Dim nMaterialID As Long = (TryCast(cbeMaterial.SelectedItem, myItemData).ItemData.ToString())
            SetMaterial(cbeMaterial.Text, nMaterialID)

        End If

    End Sub

    Private Sub fcbResultsDirectory_TextChanged(sender As Object, e As EventArgs) Handles fcbResultsDirectory.TextChanged
        RegPutString("MRP", "MRP_RESULT_FOLDER", fcbResultsDirectory.Text)

    End Sub

    Private Sub optPDBName_SelectedIndexChanged(sender As Object, e As EventArgs) Handles optPDBName.SelectedIndexChanged

        '        fcbImportFile.Visible = False
        teProvidedName.Visible = False
        cboFieldName.Visible = False

        Select Case optPDBName.SelectedIndex
            Case 0
                fcbImportFile.Visible = True
                fcbImportFile.Enabled = True
            Case 1
                teProvidedName.Visible = True
                teProvidedName.Enabled = True
            Case 2
                cboFieldName.Visible = True
                cboFieldName.Enabled = True
        End Select

    End Sub


    Private Sub CreateMRPTable(sMRP_DBName As String)
        Dim connection As New OleDb.OleDbConnection("PROVIDER=Microsoft.Jet.OLEDB.4.0;Data Source =" & sMRP_DBName)
        Dim sTablename As String = "PDB_Files"
        Dim ds As New DataSet
        Dim dt1 As DataTable = ds.Tables("PDB_Files")
        Dim sql As String = "SELECT * FROM PDB_Files"

        Try
            connection.Open()

            'Get database schema 
            Dim dbSchema As DataTable = connection.GetOleDbSchemaTable(OleDb.OleDbSchemaGuid.Tables, New Object() {Nothing, Nothing, sTablename, "TABLE"})

            connection.Close()

            ' If the table exists, the count = 1 
            If dbSchema.Rows.Count > 0 Then
                ' do whatever you want to do if the table exists 
            Else
                'do whatever you want to do if the table does not exist 
                ' e.g. create a table 
                Dim cmd As New OleDb.OleDbCommand("CREATE TABLE [" & sTablename &
                                                  "] ([ID] AUTOINCREMENT , " &
                                                  "[PDB_Name] TEXT(255) )", connection)
                connection.Open()
                cmd.ExecuteNonQuery()

                connection.Close()

            End If

        Catch ex As Exception

            MessageBox.Show("CreateMRPTable: " & ex.Message)

        End Try

    End Sub

    Private Sub GetPDBNames()
        'Dim workbook As IWorkbook = SpreadsheetControl1.Document
        'Dim worksheet As Worksheet = workbook.Worksheets(0)
        Dim pdb_store_name As String = Nothing
        Dim pdb_name As String
        Dim pdb_path As String
        Dim pdb_Path_Filename As String = Nothing
        Dim LabelDBName As String
        Dim cat As ADOX.Catalog = New ADOX.Catalog()
        Dim ws As Worksheet
        Dim workbook As IWorkbook
        Dim cMyFirstCell As Cell
        Dim cMySecondCell As Cell
        Dim cMyThirdCell As Cell
        Dim cMyFourthCell As Cell
        Dim cMyFifthCell As Cell
        Dim sMyFirstCellText As String
        Dim sMySecondCellText As String = Nothing
        Dim range As Range
        Dim pdb_info As NESTDBINFO
        Dim sResultPath As String
        Dim l_MachineID As Long
        Dim l_LayerSetupID As Long
        Dim l_LeadSetupID As Long
        Dim l_ToolSetupid As Long
        Dim l_Mat_ID As Long = -1
        Dim l_Mat_Type_ID As Long = -1
        Dim s_MatTypeDescrip As String
        Dim sConstructedPDBName As String = Nothing
        Dim nSpreadSheetRowCnt As Long
        Dim b_UseMatForToolSetup As Boolean
        Dim b_UseMatForLayerSetup As Boolean
        Dim sMappedString As String
        Dim sSearchText As String = Nothing
        Dim npos As Integer
        Dim sMappedName As String
        Dim niPart As Integer
        Dim nStartRange As Integer
        Dim nEndRange As Integer
        Dim nCounter As Integer = 1

        Try
            SaveLinkData()

            sMappedString = GetLinksFromReg(m_templateName)

            sMappedName = "Material_Name"

            sPieces = Split(sMappedString, ",")

            For ni = 0 To UBound(sPieces)

                npos = InStr(sPieces(ni), sMappedName)

                If (npos > 0) Then

                    sSearchText = Mid(sPieces(ni), 1, npos - 4)

                    Exit For

                End If

            Next

            ConditionalFileDelete(sMergePartsFilename)

            m_XLS_Name = fcbImportFile.Text
            pdb_path = PathDriveFolder(m_XLS_Name) & "\"


            'If (Dir(NormalizePath(PathDriveFolder(m_XLS_Name)) & "*.pdb") <> "") Then

            '    answer = MsgBox("Do you want to keep the existing part toolpaths and nesting results?", vbYesNo)

            'End If

            LabelDBName = NormalizePath(PathDriveFolder(m_XLS_Name)) & "labeldb.mdb"

            'Clean up labeldb file if needed
            If (FileExists(LabelDBName) = True) Then

                ConditionalFileDelete(LabelDBName)

            End If

            sReturn = PathCNC_Set(NormalizePath(PathDriveFolder(m_XLS_Name)))

            If (IS_CCS_FAB = True) Then

                m_job_name = "Job " & PathFile(m_XLS_Name)

            Else

                Select Case optPDBName.SelectedIndex

                    Case 0
                        m_job_name = "Job " & PathFile(m_XLS_Name)
                        sConstructedPDBName = PathFile(PathRemoveExt(m_XLS_Name))
                    Case 1
                        m_job_name = "Job " & teProvidedName.Text
                        sConstructedPDBName = teProvidedName.Text
                        'MessageBox.Show("We should use the entered jobname")

                    Case 2
                        m_job_name = "Job " & PathFile(m_XLS_Name)

                        'MessageBox.Show("We should be using the field name selected Which is < " & cboFieldName.Text & " >")
                End Select

            End If

            'If (optPDBName.SelectedIndex = 0) Then

            'm_job_name = "Job " & PathFile(m_XLS_Name)

            'End If

            'sMRP_DB = NormalizePath(PathDriveFolder(m_XLS_Name)) & PathFile(m_XLS_Name) & ".mrp"
            sMRP_DB = NormalizePath(PathDriveFolder(m_XLS_Name)) & m_job_name & ".mrp"

            If (FileExists(sMRP_DB) = True) Then

                ConditionalFileDelete(sMRP_DB)

            End If

            cat.Create("Provider=Microsoft.Jet.OLEDB.4.0;" &
                               "Data Source=" & sMRP_DB & ";" &
                               "Jet OLEDB:Engine Type=5")

            cat.ActiveConnection.close()

            modNesting.MRPDBNAME = sMRP_DB

            CreateMRPTable(sMRP_DB)

            sResultPath = fcbResultsDirectory.Text


            s_Machine = cbeMachine.Text
            l_MachineID = GetMachineIDFromDescrip(s_Machine)


            s_ToolSetup = cbeToolSetup.Text

            l_ToolSetupid = GetToolSetupIDFromDescription(s_ToolSetup)
            s_LeadSetup = cbeLeadSetup.Text
            l_LeadSetupID = GetLeadSetupID(s_LeadSetup)
            s_LayerSetup = cbeLayerSetup.Text
            l_LayerSetupID = modTranslate.GetLayerSetupID(s_LayerSetup, l_MachineID)

            ws = SpreadsheetControl1.ActiveWorksheet

            workbook = SpreadsheetControl1.Document
            'Dim gary2 As Range = ws.GetUsedRange()

            cMyFirstCell = ws.Search(sSearchText).FirstOrDefault()
            cMySecondCell = ws.Search("Gauge").FirstOrDefault()
            cMyThirdCell = ws.Search("Length").FirstOrDefault()
            cMyFourthCell = ws.Search("Width").FirstOrDefault()
            cMyFifthCell = ws.Search("Thickness").FirstOrDefault()

            range = ws.GetDataRange()

            Dim nRMPartColIndex As Integer = GetColIndexFromText("RM PART", ws)
            Dim nPartNoColIndex As Integer = GetColIndexFromText("Part No", ws)
            ws.Sort(range, nRMPartColIndex)

            nSpreadSheetRowCnt = range.RowCount

            If (chkHeader.CheckState = CheckState.Checked) Then
                nStartRange = 1
                nEndRange = nSpreadSheetRowCnt - 1
            Else
                nStartRange = 0
                nEndRange = nSpreadSheetRowCnt - 1

            End If
            For niPart = nStartRange To nEndRange
                MsgBox("Adding Part number < " & nCounter & " > ")
                nCounter = nCounter + 1


#If APIHeatTransfer Then
                IS_APIHEATTRANSFER = True




                m_job_name = ws.Cells(niPart, nPartNoColIndex).DisplayText


                Dim sMatCode As String = ws.Cells(niPart, nRMPartColIndex).DisplayText

                ' worksheet.Cells(ni, nRMPartColIndex).DisplayText
                ' MessageBox.Show("the row value is " & ws.Rows(niPart).Item(0).Value.TextValue & "_|" & ws.Rows(niPart).Item(1).Value.TextValue & "|_" & ws.Rows(niPart).Item(2).Value.TextValue)
                sConstructedPDBName = m_job_name & "_" & sMatCode
                'If (ws.Rows(niPart).Item(1).Value.IsNumeric = True) Then
                '    MessageBox.Show("The qty is " & ws.Rows(niPart).Item(1).Value.NumericValue & " for part number " & nCounter)
                '    nCounter = nCounter + 1


                'End If
                'MessageBox.Show("The qty is " & ws.Rows(niPart).Item(1).Value.TextValue)
                sMyFirstCellText = ws.Rows(niPart).Item(cMyFirstCell.ColumnIndex).Value.TextValue

                'If (ceMaterialToToolSetup.CheckState = CheckState.Checked) Then
                '    b_UseMatForToolSetup = True
                'Else
                '    b_UseMatForToolSetup = False
                'End If
#End If


                If (ceMaterialToLayerSetup.CheckState = CheckState.Checked) Then

                    b_UseMatForLayerSetup = True
                Else
                    b_UseMatForLayerSetup = False

                End If

                If (cMySecondCell IsNot Nothing) Then
                    sMySecondCellText = ws.Rows(niPart).Item(cMySecondCell.ColumnIndex).Value.TextValue
                End If

                s_MatTypeDescrip = ws.Rows(niPart).Item(cMyFirstCell.ColumnIndex).Value.TextValue

                If (IS_CCS_FAB = False) Then

                    l_Mat_ID = modTranslate.GetMaterialIDFromDescription(s_MatTypeDescrip)

                    l_Mat_Type_ID = GetMaterialTypeIDFromDescription(s_MatTypeDescrip)


                    d_MatLength = modMaterial.GetMaterialLength(l_Mat_ID)

                    d_MatWidth = modMaterial.GetMaterialWidth(l_Mat_ID)

                    d_MatThickness = modMaterial.GetMaterialThickness(l_Mat_ID)

                    d_MatQty = modMaterial.GetMaterialQty(l_Mat_ID)

                    d_MatWeight = modMaterial.CheckMaterialWeight(l_Mat_ID)

                    d_MatCost = modMaterial.GetMaterialCost(l_Mat_ID)

                End If

                If (cMyThirdCell IsNot Nothing) Then
                    d_MatLength = CDbl(ws.Rows(niPart).Item(cMyThirdCell.ColumnIndex).Value.TextValue)
                End If

                If (cMyFourthCell IsNot Nothing) Then
                    d_MatWidth = CDbl(ws.Rows(niPart).Item(cMyFourthCell.ColumnIndex).Value.TextValue)
                End If


                If (cMyFourthCell IsNot Nothing) Then
                    d_MatThickness = CDbl(ws.Rows(niPart).Item(cMyFifthCell.ColumnIndex).Value.TextValue)
                End If



                'If (niPart = 10) Then
                '    If (IS_CCS_FAB = True) Then

                '        pdb_store_name = sMyFirstCellText & " " & sMySecondCellText & " GA."

                '    ElseIf (IS_APIHEATTRANSFER) Then
                '        pdb_store_name = sConstructedPDBName
                '    Else

                '        If (chkSortBy.CheckState = CheckState.Checked) Then
                '            pdb_store_name = sMyFirstCellText & "."
                '        End If

                '    End If

                '    pdb_name = pdb_store_name

                '    If (IS_CCS_FAB = True) Then
                '        pdb_Path_Filename = pdb_path & Replace(pdb_name, ".", ".pdb")
                '    Else
                '        pdb_Path_Filename = pdb_path & sConstructedPDBName & ".pdb"
                '    End If


                '    If (FileExists(pdb_Path_Filename) = True) Then

                '        ConditionalFileDelete(pdb_Path_Filename)

                '    End If

                '    'SplashScreenManager.Default.SetWaitFormDescription("Creating Nesting Database" & pdb_store_name)

                '    pdb_info = InitPdbInfo(
                '                pdb_Path_Filename, sResultPath, s_Machine, l_MachineID,
                '                s_ToolSetup, l_ToolSetupid, "", 0,
                '                s_LeadSetup, l_LeadSetupID,
                '                s_LayerSetup, l_LayerSetupID)

                '    If (EstablishPDB(pdb_info) = String.Empty) Then
                '        MsgBox("Unable to create nesting database. " & vbCrLf & pdb_name)
                '    Else
                '        l_Mat_ID = GetTheMaterialIDFromXlS(pdb_Path_Filename, s_MatTypeDescrip, d_MatLength, d_MatWidth, d_MatThickness)

                '        If (l_Mat_ID > 0) Then


                '            modMRP.AddMaterialToMaterialInventorys(pdb_Path_Filename, s_MatTypeDescrip, l_Mat_Type_ID, d_MatLength, d_MatWidth, d_MatThickness, d_MatWeight, d_MatQty, d_MatCost)
                '        End If

                '        UpdateSetupTable(pdb_Path_Filename, vgNestDefaults, s_MatTypeDescrip, b_UseMatForToolSetup, b_UseMatForLayerSetup)

                '        'AddPdbToTable(sMRP_DB, pdb_Path_Filename)

                '    End If


                'Else

#If APIHeatTransfer Then
                pdb_name = sConstructedPDBName
#Else
                    pdb_name = sMyFirstCellText ' & " " & sMySecondCellText

#End If
                'pdb_name = sMyFirstCellText ' & " " & sMySecondCellText

                If (ceMaterialToToolSetup.CheckState = CheckState.Checked) Then

                    s_ToolSetup = sMyFirstCellText
                    l_ToolSetupid = GetToolSetupIDFromDescription(s_ToolSetup)

                End If


                If (ceMaterialToLayerSetup.CheckState = CheckState.Checked) Then

                    s_LayerSetup = sMyFirstCellText
                    l_LayerSetupID = modTranslate.GetLayerSetupID(s_LayerSetup, l_MachineID)

                End If

                If (String.Equals(Trim(pdb_name), Trim(pdb_store_name))) Then
                    'MessageBox.Show(pdb_name & " is equal to " & pdb_store_name)
                Else
                    'MessageBox.Show(pdb_name & " is not equal to " & pdb_store_name)
                End If

                If (UCase(Trim(pdb_name)) = UCase(Trim(pdb_store_name)) = False) Then

                    If (IS_CCS_FAB = True) Then

                        pdb_Path_Filename = pdb_path & pdb_name & ".pdb"

                    Else

                        If (InStr(pdb_name, ".pdb") > 0) Then
                            pdb_Path_Filename = pdb_path & sConstructedPDBName & "_" & Replace(pdb_name, ".", ".pdb")
                        Else
                            pdb_Path_Filename = pdb_path & pdb_name & ".pdb"
                        End If
                        'MessageBox.Show("The nesting DB Is < " & pdb_Path_Filename & " >")

                    End If



                    PathPDB_Set(pdb_Path_Filename)

                    pdb_store_name = pdb_name

                    AddPdbToTable(sMRP_DB, pdb_Path_Filename)

                    If (FileExists(pdb_Path_Filename) = True) Then

                        ConditionalFileDelete(pdb_Path_Filename)

                    End If

                    'SplashScreenManager.Default.SetWaitFormDescription("Creating Nesting Database" & pdb_store_name)

                    pdb_info = InitPdbInfo(
                                pdb_Path_Filename, sResultPath, s_Machine, l_MachineID,
                                s_ToolSetup, l_ToolSetupid, "", 0,
                                s_LeadSetup, l_LeadSetupID,
                                s_LayerSetup, l_LayerSetupID)


                    If (EstablishPDB(pdb_info) = String.Empty) Then
                        MsgBox("Unable to create nesting database. " & vbCrLf & pdb_name)
                    Else

                        If (IS_CCS_FAB = False) Then

                            'l_Mat_ID = GetMaterialTypeIDFromDisplay(sMatCode)



                            'l_Mat_Type_ID = Get1stMaterialID(l_Mat_ID)


                            'd_MatLength = modMaterial.Get1stMaterialLength(l_Mat_ID)

                            'd_MatWidth = modMaterial.Get1stMaterialWidth(l_Mat_ID)

                            'd_MatThickness = modMaterial.Get1stMaterialThickness(l_Mat_ID)

                            'd_MatQty = modMaterial.Get1stMaterialQty(l_Mat_ID)

                            'd_MatWeight = modMaterial.CheckMaterialWeight(l_Mat_ID)

                            'd_MatCost = modMaterial.Get1stMaterialCost(l_Mat_ID)

                        End If

                        'l_Mat_ID = GetMaterialTypeID(s_MatTypeDescrip)

                        modMRP.AddMaterialToMaterialInventorys(pdb_Path_Filename, s_MatTypeDescrip, l_Mat_Type_ID, d_MatLength, d_MatWidth, d_MatThickness, d_MatWeight, d_MatQty, d_MatCost)

                        UpdateSetupTable(pdb_Path_Filename, vgNestDefaults, s_MatTypeDescrip, b_UseMatForToolSetup, b_UseMatForLayerSetup)
                    End If

                End If

                'End If

                If (l_Mat_ID > 0) Then

#If APIHeatTransfer Then
                    'AddAPIHeatPart(pdb_Path_Filename, niPart, SpreadsheetControl1)
#Else
                    Dim gary As String = ws.Rows(niPart).Item(0).Value.TextValue
                    AddNormalPart(pdb_Path_Filename, niPart, SpreadsheetControl1)
#End If
                End If


            Next

        Catch ex As Exception

            MsgBox(ex.ToString)

        End Try

    End Sub
    Private Sub AddAPIHeatPart(sPdbName As String, s_MatTypeDescrip As String, dtDS As DataSet, dtRow As Integer)
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim m_dtPart As New DataTable
        Dim cmdGetID As New OleDbCommand
        Dim sFieldNames As String
        Dim sFieldValues As String
        Dim sFileToConvert As String = Nothing
        Dim sPartName As String = Nothing
        Dim sPartPath As String
        Dim Sqlstatement As String
        Dim s_My_Part_Quantity As String
        Dim s_My_Fill_Part As String
        Dim s_My_Rotation_Num As String
        Dim s_My_MirrorPart As String
        Dim s_My_Rotation_Start As String
        Dim s_My_Part_Pass As String
        Dim s_My_Part_Priority As String
        Dim s_My_Cost_Per_Part As String
        Dim s_My_Representation As String
        Dim s_My_Special_Priority As String
        Dim s_My_Hold_ID As String
        Dim s_My_PreNest As String
        Dim s_My_Part_Extents As String
        Dim sFileExist As String = Nothing
        Dim connetionString As String
        Dim pdb_Path_Filename As String
        Dim sFullPartName As String
        Dim sMatCode As String
        Dim sResultPath As String
        Dim sColName As String
        Dim sTempPartNAme As String

        Try

            sTempPartNAme = dtDS.Tables(0).Rows(dtRow).Item("Comp Part").ToString


            sMatCode = dtDS.Tables(0).Rows(dtRow).Item("RM Part").ToString

            If (InStr(UCase(s_MatTypeDescrip), "ALUM") > 0) Then
                sPartPath = "c:\laser\" & sAlumFolder & "\" & sMRPPartsFolder & "\"
                pdb_Path_Filename = "c:\Laser\" & sAlumFolder & "\" & sMRPNestFolder & "\" & m_job_name & "_" & sMatCode & ".pdb"
                sResultPath = "c:\Laser\" & sAlumFolder & "\" & sMRPNestFolder & "\"
            Else

                sPartPath = "c:\laser\" & sSteelFolder & "\" & sMRPPartsFolder & "\"
                pdb_Path_Filename = "c:\Laser\" & sSteelFolder & "\" & sMRPNestFolder & "\" & m_job_name & "_" & sMatCode & ".pdb"
                sResultPath = "c:\Laser\" & sSteelFolder & "\" & sMRPNestFolder & "\"
            End If

            sFullPartName = sPartPath & sTempPartNAme & ".mm2"

            'sPartPath = NormalizePath(fcbSourceFolder.Text)

            Select Case UCase(PathExt(sFullPartName))
                Case "DXF"
                    sFileToConvert = sFullPartName

                    If (FileExists(sFileToConvert) = False) Then
                        sFileExist = "The FIle < " & sFileToConvert & " > Does Not exist! The part will Not be added to the nesting database!"

                        MessageBox.Show(sFileExist, "Part Does Not Exist!", MessageBoxButtons.OK, MessageBoxIcon.Warning)

                        GoTo SkipPart

                    End If

                    'SplashScreenManager.Default.SetWaitFormDescription("Importing < " & sFileToConvert & " >")

                    sPartName = PathReplaceExt(sFileToConvert, "MM2")

                    modFileIO.ImportDXF(sFileToConvert, sPartName)


                Case "DWG"
                    sFileToConvert = sFullPartName

                    'SplashScreenManager.Default.SetWaitFormDescription("Importing < " & sFileToConvert & " >")

                    sPartName = PathReplaceExt(sFileToConvert, "MM2")

                    modFileIO.ImportDWG(sFileToConvert, sPartName)


                Case "MM2"
                    ' sFileToConvert = sPartPath & GetUserValues("Part Name", nRow)
                    sFileToConvert = sFullPartName

                    'SplashScreenManager.Default.SetWaitFormDescription("Importing < " & sFileToConvert & " >")
                    If (InStr(sFileToConvert, "/")) Then
                        sPartName = Replace(sFileToConvert, "/", "-")
                        sPartName = PathReplaceExt(sPartName, "MM2")
                    Else
                        sPartName = PathReplaceExt(sFileToConvert, "MM2")
                    End If


                    modFileIO.ImportMM2(sPartName)


                Case "PM4"
                    sFileToConvert = sFileToConvert = sFullPartName

                    'SplashScreenManager.Default.SetWaitFormDescription("Importing < " & sFileToConvert & " >")

                    sPartName = PathReplaceExt(sFileToConvert, "MM2")

                    modFileIO.ImportPM4(sFileToConvert, sPartName)

                Case "DIAMONDLIFE"
                    MessageBox.Show("We Should run the macro based on the panel type")

            End Select

            sColName = GetStandardMappedField("Quantity")

            If (sColName <> "USE DEFAULT") Then
                s_My_Part_Quantity = 1 ' dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_Part_Quantity = 1
            End If

            sColName = GetStandardMappedField("Fill Part")

            If (sColName <> "USE DEFAULT") Then
                s_My_Fill_Part = dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_Fill_Part = "0"
            End If

            sColName = GetStandardMappedField("Rotations")

            If (sColName <> "USE DEFAULT") Then
                s_My_Rotation_Num = dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_Rotation_Num = "1"
            End If

            sColName = GetStandardMappedField("Pre-Rotate")

            If (sColName <> "USE DEFAULT") Then
                s_My_Rotation_Start = dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_Rotation_Start = "0"
            End If

            sColName = GetStandardMappedField("Priority")

            If (sColName <> "USE DEFAULT") Then
                s_My_Part_Pass = dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_Part_Pass = "0"
            End If

            sColName = GetStandardMappedField("Cost Per Part")

            If (sColName <> "USE DEFAULT") Then
                s_My_Cost_Per_Part = dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_Cost_Per_Part = "0"
            End If

            sColName = GetStandardMappedField("Representation")

            If (sColName <> "USE DEFAULT") Then
                s_My_Representation = dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_Representation = "0"
            End If

            s_My_Special_Priority = 0
            s_My_Hold_ID = 0

            sColName = GetStandardMappedField("Mirroring")

            If (sColName <> "USE DEFAULT") Then
                s_My_MirrorPart = dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_MirrorPart = "0"
            End If

            sColName = GetStandardMappedField("Pre-Nest")

            If (sColName <> "USE DEFAULT") Then
                s_My_PreNest = dtDS.Tables(0).Rows(dtRow).Item(sColName).ToString
            Else
                s_My_PreNest = "0"
            End If



            sFieldNames = sEmpty
            sFieldValues = sEmpty

            sPartName = PathReplaceExt(sPartName, "MM2")

            sFieldNames = "[Filename]," &
                                "[Part Quantity]," &
                                "[Fill Part]," &
                                "[Rotation Num]," &
                                "[MirrorPart]," &
                                "[Rotation Start]," &
                                "[Part Pass]," &
                                "[Part Priority]," &
                                "[Cost Per Part]," &
                                "[Representation]," &
                                "[Special Priority]," &
                                "[Hold ID]," &
                                "[PreNest]," &
                                "[Part_Extents]"

            sFieldValues = "'" & sPartName & "'," &
                                1 & "," &
                                "false," &
                                1 & "," &
                                0 & "," &
                                0 & "," &
                                1 & "," &
                                0 & "," &
                                0 & "," &
                                0 & "," &
                                "0," &
                                "0," &
                                "false," &
                                "'" & 0 & "'"

            Sqlstatement = "INSERT INTO [Part] (" & sFieldNames & " ) VALUES(" & sFieldValues & ")"

            'MessageBox.Show(Sqlstatement)


            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPdbName & "';"

            m_cnPart = New OleDbConnection(connetionString)

            m_cnPart.Open()

            m_daPart.InsertCommand = New OleDbCommand(Sqlstatement, m_cnPart)

            lReturn = m_daPart.InsertCommand.ExecuteNonQuery()

SkipPart:

            dtDS.Dispose()
            dtDS = Nothing

        Catch ex As Exception
            MessageBox.Show("AddAPIHeatPart: " & ex.Message)
        End Try

    End Sub
    Private Sub AddNormalPart(sPdbName As String, nRow As Integer, sSpreadsheetControl As DevExpress.XtraSpreadsheet.SpreadsheetControl)
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim m_dtPart As New DataTable
        Dim cmdGetID As New OleDbCommand
        Dim sFieldNames As String
        Dim sFieldValues As String
        Dim ws As Worksheet
        Dim workbook As IWorkbook
        Dim sFileToConvert As String = Nothing
        Dim sPartName As String = Nothing
        Dim sPartPath As String
        Dim Sqlstatement As String
        Dim s_My_Part_Quantity As String
        Dim s_My_Fill_Part As String
        Dim s_My_Rotation_Num As String
        Dim s_My_MirrorPart As String
        Dim s_My_Rotation_Start As String
        Dim s_My_Part_Pass As String
        Dim s_My_Part_Priority As String
        Dim s_My_Cost_Per_Part As String
        Dim s_My_Representation As String
        Dim s_My_Special_Priority As String
        Dim s_My_Hold_ID As String
        Dim s_My_PreNest As String
        Dim s_My_Part_Extents As String
        Dim stempfilename As System.IO.StreamWriter
        Dim sFileExist As String = Nothing
        Dim connetionString As String

        Try

            stempfilename = My.Computer.FileSystem.OpenTextFileWriter(sMergePartsFilename, True)

            ws = sSpreadsheetControl.ActiveWorksheet
            workbook = sSpreadsheetControl.Document

            sPartPath = NormalizePath(fcbSourceFolder.Text)

            Select Case UCase(lcbFileType.Text)
                Case "DXF"
                    sFileToConvert = sPartPath & ws.Rows(nRow).Item(m_col_FileName).Value.TextValue & ".dxf"

                    If (FileExists(sFileToConvert) = False) Then
                        sFileExist = "The FIle < " & sFileToConvert & " > Does Not exist! The part will Not be added to the nesting database!"

                        MessageBox.Show(sFileExist, "Part Does Not Exist!", MessageBoxButtons.OK, MessageBoxIcon.Warning)

                        GoTo SkipPart

                    End If

                    'SplashScreenManager.Default.SetWaitFormDescription("Importing < " & sFileToConvert & " >")

                    sPartName = PathReplaceExt(sFileToConvert, "MM2")

                    modFileIO.ImportDXF(sFileToConvert, sPartName)


                Case "DWG"
                    sFileToConvert = sPartPath & ws.Rows(nRow).Item(m_col_FileName).Value.TextValue & ".dwg"

                    'SplashScreenManager.Default.SetWaitFormDescription("Importing < " & sFileToConvert & " >")

                    sPartName = PathReplaceExt(sFileToConvert, "MM2")

                    modFileIO.ImportDWG(sFileToConvert, sPartName)


                Case "MM2"
                    sFileToConvert = sPartPath & GetUserValues("Part Name", nRow)

                    'SplashScreenManager.Default.SetWaitFormDescription("Importing < " & sFileToConvert & " >")
                    If (InStr(sFileToConvert, "/")) Then
                        sPartName = Replace(sFileToConvert, "/", "-")
                        sPartName = PathReplaceExt(sPartName, "MM2")
                    Else
                        sPartName = PathReplaceExt(sFileToConvert, "MM2")
                    End If


                    modFileIO.ImportMM2(sPartName)


                Case "PM4"
                    sFileToConvert = sPartPath & ws.Rows(nRow).Item(m_col_FileName).Value.TextValue & ".pm4"

                    'SplashScreenManager.Default.SetWaitFormDescription("Importing < " & sFileToConvert & " >")

                    sPartName = PathReplaceExt(sFileToConvert, "MM2")

                    modFileIO.ImportPM4(sFileToConvert, sPartName)

                Case "DIAMONDLIFE"
                    MessageBox.Show("We Should run the macro based on the panel type")

            End Select


            'SplashScreenManager.Default.SetWaitFormDescription("Setting Part Values < " & sFileToConvert & " >")
            s_My_Part_Quantity = GetUserValues("Part Quantity", nRow)
            s_My_Fill_Part = GetUserValues("Fill Part", nRow)
            s_My_Rotation_Num = GetUserValues("Rotations/360", nRow)
            s_My_MirrorPart = GetUserValues("Mirroring", nRow)
            s_My_Rotation_Start = GetUserValues("Pre-Rotate", nRow)
            s_My_Part_Pass = 1
            s_My_Part_Priority = GetUserValues("Priority", nRow)
            s_My_Cost_Per_Part = GetUserValues("Cost Per Part", nRow)
            s_My_Representation = 0 '"'" & GetUserValues("Representation", nRow) & "'"
            s_My_Special_Priority = 0
            s_My_Hold_ID = 0
            s_My_PreNest = 0
            s_My_Part_Extents = 0


            sFieldNames = sEmpty
            sFieldValues = sEmpty

            sPartName = PathReplaceExt(sPartName, "MM2")

            sFieldNames = "[Filename]," &
                                "[Part Quantity]," &
                                "[Fill Part]," &
                                "[Rotation Num]," &
                                "[MirrorPart]," &
                                "[Rotation Start]," &
                                "[Part Pass]," &
                                "[Part Priority]," &
                                "[Cost Per Part]," &
                                "[Representation]," &
                                "[Special Priority]," &
                                "[Hold ID]," &
                                "[PreNest]," &
                                "[Part_Extents]"


            sFieldValues = "'" & sPartName & "'," &
                                s_My_Part_Quantity & "," &
                                "0," &
                                s_My_Rotation_Num & "," &
                                s_My_MirrorPart & "," &
                                s_My_Rotation_Start & "," &
                                s_My_Part_Pass & "," &
                                s_My_Part_Priority & "," &
                                s_My_Cost_Per_Part & "," &
                                s_My_Representation & "," &
                                "0," &
                                "0," &
                                "0," &
                                "'" & 0 & "'"


            stempfilename.WriteLine(sPdbName & "|" & sPartName & "|" & sFieldValues)
            'stempfilename.WriteLine(sPartName)
            'stempfilename.WriteLine(sFieldNames)
            'stempfilename.WriteLine(sFieldValues)
            'stempfilename.WriteLine(" ")

            Sqlstatement = "INSERT INTO [Part] (" & sFieldNames & " ) VALUES(" & sFieldValues & ")"

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPdbName & "';"

            m_cnPart = New OleDbConnection(connetionString)

            m_cnPart.Open()

            m_daPart.InsertCommand = New OleDbCommand(Sqlstatement, m_cnPart)

            lReturn = m_daPart.InsertCommand.ExecuteNonQuery()
SkipPart:

            stempfilename.Close()


        Catch ex As Exception

            MessageBox.Show("AddBMPart: " & ex.Message)

        End Try

        m_daPart.Dispose()
        m_daPart = Nothing

        m_cnPart.Dispose()

        m_cnPart.Close()
        m_cnPart = Nothing

        'MessageBox.Show("Check PDB")

    End Sub


    Private Sub cbeLayerSetup_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeLayerSetup.SelectedIndexChanged
        If (cbeLayerSetup.SelectedIndex > -1) Then
            Dim nLayerSetupId As Long = modTranslate.GetLayerSetupID(cbeLayerSetup.Text, RegGetString("Global", "MachineID", 0))

            SetLayerSetup(cbeLayerSetup.Text, nLayerSetupId)

        End If
    End Sub

    Private Sub cbeLeadSetup_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cbeLeadSetup.SelectedIndexChanged
        If (cbeLeadSetup.SelectedIndex > -1) Then
            Dim l_leadSetupID As Long = GetLeadSetupID(cbeLeadSetup.Text)
            SetLeadSetup(cbeLeadSetup.Text, l_leadSetupID)

        End If
    End Sub

    Private Sub bbiUnlink_ItemClick(sender As Object, e As DevExpress.XtraBars.ItemClickEventArgs) Handles bbiUnlink.ItemClick
        Dim nTheNode As TreeListNode

        nTheNode = TV_MappedFields.FocusedNode

        If (nTheNode IsNot Nothing) Then
            TV_MappedFields.Nodes.Remove(nTheNode)
        End If
    End Sub
    Private Function GetStandardMappedField(sMappedName As String) As String
        Dim sMappedString As String
        Dim ws As Worksheet
        Dim sSearchText As String
        Dim npos As Integer
        Dim sFields() As String

        GetStandardMappedField = "USE DEFAULT"

        Try
            m_templateName = RegGetString("MRP", "MRP_TEMPLATE", "")

            sMappedString = GetLinksFromReg(m_templateName)
            sPieces = Split(sMappedString, ",")

            For ni = 0 To UBound(sPieces)

                npos = InStr(sPieces(ni), sMappedName)

                If (npos > 0) Then

                    sSearchText = Mid(sPieces(ni), 1)
                    sFields = Split(sSearchText, "=")
                    GetStandardMappedField = Trim(sFields(0))
                    Exit Function

                End If

            Next
        Catch ex As Exception
            MessageBox.Show("GetStandardMappedField: " & ex.Message)
        End Try
    End Function



    Private Function IsMapped(sMappedName As String) As Integer
        Dim sMappedString As String
        Dim ws As Worksheet
        Dim sSearchText As String
        Dim npos As Integer

        IsMapped = -1

        Try
            ws = SpreadsheetControl1.ActiveWorksheet

            m_templateName = RegGetString("MRP", "MRP_TEMPLATE", "")

            sMappedString = GetLinksFromReg(m_templateName)
            sPieces = Split(sMappedString, ",")

            For ni = 0 To UBound(sPieces)

                npos = InStr(sPieces(ni), sMappedName)

                If (npos > 0) Then

                    sSearchText = Mid(sPieces(ni), 1, npos - 4)
                    m_Search_Col = GetColIndexFromText(sSearchText, ws)
                    IsMapped = m_Search_Col

                    Exit Function

                End If

            Next
        Catch ex As Exception
            MessageBox.Show("IsMapped: " & ex.Message)
        End Try
    End Function

    Private Function GetRowByCaption(ByVal s As String, vg_TheGrid As DevExpress.XtraVerticalGrid.VGridControl) As DevExpress.XtraVerticalGrid.Rows.BaseRow
        Dim r As DevExpress.XtraVerticalGrid.Rows.BaseRow
        For Each row As DevExpress.XtraVerticalGrid.Rows.BaseRow In vg_TheGrid.Rows
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

    Private Function GetMappedOrDefaultValues(sRowCaption As String, nRow As Integer) As String
        Dim ws As Worksheet
        Dim workbook As IWorkbook
        Dim childrow As BaseRow

        Try
            GetMappedOrDefaultValues = Nothing


            ws = SpreadsheetControl1.ActiveWorksheet
            workbook = SpreadsheetControl1.Document

            If (IsMapped(sRowCaption)) Then
                GetMappedOrDefaultValues = ws.Rows(nRow).Item(m_Search_Col).Value.ToString
            Else

                childrow = GetRowByCaption(sRowCaption, vgPartDefaults)
                GetMappedOrDefaultValues = vgPartDefaults.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value()


            End If
        Catch ex As Exception

        End Try
    End Function

    Private Function GetUserValues(sRowCaption As String, nRow As Integer) As String
        Dim ws As Worksheet
        Dim workbook As IWorkbook
        Dim childrow As BaseRow


        GetUserValues = Nothing

        ws = SpreadsheetControl1.ActiveWorksheet
        workbook = SpreadsheetControl1.Document

        If (IsMapped(sRowCaption) > -1) Then

            GetUserValues = ws.Rows(nRow).Item(m_Search_Col).Value.ToString

        Else

            childrow = GetRowByCaption(sRowCaption, vgPartDefaults)
            GetUserValues = vgPartDefaults.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value()


        End If

    End Function
    Private Sub SPreadSheetRowsCols()
        Dim sValues As String
        Dim sdata As String
        Dim sPieces() As String
        Dim sDataTypefilename As System.IO.StreamWriter
        Dim sTempFile As String
        Dim colCount As Integer
        Dim workbook As IWorkbook
        Dim columns As ColumnCollection

        sTempFile = Application.StartupPath & "\debug\SDataTypes.text"

        If (FileExists(sTempFile)) Then

            Kill(sTempFile)

        End If

        sDataTypefilename = My.Computer.FileSystem.OpenTextFileWriter(sTempFile, True)

        workbook = SpreadsheetControl1.Document

        ' Access a collection of columns.
        columns = workbook.Worksheets(0).Columns

        'SpreadsheetControl1.SaveDocument(sTempXLS, DocumentFormat.Xlsx)

        Dim worksheet As Worksheet = SpreadsheetControl1.ActiveWorksheet
        Dim usedRange As Range = worksheet.GetUsedRange()
        Dim topRowIndex As Integer = usedRange.TopRowIndex
        Dim headerRange As Range = worksheet.Range.FromLTRB(usedRange.LeftColumnIndex, topRowIndex, usedRange.RightColumnIndex, topRowIndex)
        Dim rowIndexWithData As Integer = headerRange.BottomRowIndex + 1
        Dim dataRange As Range = worksheet.Range.FromLTRB(usedRange.LeftColumnIndex, rowIndexWithData, usedRange.RightColumnIndex, usedRange.BottomRowIndex)

        For rowindex As Integer = 0 To dataRange.RowCount - 1

        Next
        For columnIndex As Integer = 0 To dataRange.ColumnCount - 1
            Dim definedColumnName = headerRange(columnIndex).Value
            Dim Gary = definedColumnName.GetType

            If (definedColumnName.TextValue = "Code (Part,Qty,RM Desc)") Then
                sDataTypefilename.WriteLine("[Actual Part Name] ")
            Else
                sDataTypefilename.WriteLine("[" & definedColumnName.TextValue & "] ")
            End If

        Next

        sDataTypefilename.Close()

    End Sub

    Private Sub gcMrpSettings_Resize(sender As Object, e As EventArgs) Handles gcMrpSettings.Resize
        xtcMRP.Width = gcMrpSettings.Width - 10

    End Sub

End Class