Imports DevExpress.XtraBars
Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraTreeList
Imports DevExpress.XtraTreeList.Columns
Imports DevExpress.XtraTreeList.Nodes
Imports DevExpress.XtraTreeList.Nodes.Operations
Imports DevExpress.XtraTreeList.ViewInfo
Imports DevExpress.XtraVerticalGrid.Rows
Imports System.Data.OleDb
Imports System.IO
Imports System.Text

Public Class frmNesting
    Private _hitInfo As TreeListHitInfo
    Private WithEvents repositoryItemButtonEdit As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit
    Private bDraggingStarted As Boolean = False
    Private m_pdb_filename As String = Nothing
    Private m_last_Mode As Long
    Public m_code_gen As String
    Private m_is_active As Boolean
    Private CFG_ROOT_NODE As TreeListNode
    Private CFG_SETTINGS_NODE As TreeListNode
    Private CFG_PARAMETERS_NODE As TreeListNode
    Private CFG_PARTS_NODE As TreeListNode
    Private CFG_MATERIAL_NODE As TreeListNode
    Public CFG_RESULTS_NODE As TreeListNode
    Private CFG_RESULT_MAT_NODE As TreeListNode
    Private CFG_RESULTS_SHEET As TreeListNode
    Private CFG_RESULTS_PART As TreeListNode
    Private CFG_MATERIAL_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_CATEGORY_NODE As TreeListNode


    Private Const TAG_ROOT_NODE = 0
    Private Const TAG_SETTINGS_NODE = -100
    Private Const TAG_PARAMETERS_NODE = -101
    Private Const TAG_PARTS_NODE = -102
    Private Const TAG_MATERIAL_NODE = -103
    Private Const TAG_RESULTS_NODE = -104
    Private Const TAG_RESULTS_MAT_NODE = -105
    Private Const TAG_RESULTS_SHEET = -106
    Private Const TAG_RESULTS_PART = -107
    Private Const TAG_MATERIAL_ROOT_NODE = -200
    Private Const TAG_MATERIAL_CATEGORY_NODE = -201

    Public arrMachine As Collection
    Public arrToolSetup As Collection
    Public arrLeadSetup As Collection
    Public arrLayerSetup As Collection
    Public arrOptions As Collection
    Public rowRemnantInentory As CategoryRow

    Private s_Machine As String = String.Empty
    Private s_ToolSetup As String = String.Empty
    Private s_LayerSetup As String = String.Empty
    Private s_LeadSetup As String = String.Empty

    Private m_isVirginDB As Boolean
    Private bFromCreateNodes As Boolean
    Private m_current_part As String

    Private m_is_view_initialized As Boolean
    Private bFromAddButton As Boolean
    Private bEmptyPdb As Boolean

    Private editValue As String
    Private nRowIndx As Integer
    Private nRowId As BaseRow
    Private nToolSetupRowId As BaseRow
    Private nLayerSetupRowID As BaseRow
    Private SelectedMachineName As String
    Private SelectedToolSetupName As String
    Private SelectedLayerSetupName As String
    Private ArePartsLoaded As Boolean
    Private IsMaterialLoaded As Boolean

    Delegate Sub FocusTextHandler(ByRef VerticalGrid)
    Private m_currRecordID As Long
    Private m_searchString As String
    Private m_searchIndex As Integer

    Private table As DataTable

    Private bFromMrp As Boolean
    Private m_ncfile As String

    Private sPortalCommand As String

    Dim er_SelectedMaterialRow As EditorRow
    Dim pdb_info As NESTDBINFO
    Dim bDoAutoCode As Boolean
    Dim bDoAutoPrint As Boolean

    Public Sub SetPortalCommand(sCmd As String)
        sPortalCommand = sCmd

    End Sub
    Public Sub CodeGenPurge()
        m_code_gen = NEED_CODEGEN
    End Sub

    Private Sub frmTemp_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        frmMain.CloseFirstPanel()
        m_is_nesting = False
        Me.Dispose()
        Me.Close()
    End Sub

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()
        AddHandler Load, AddressOf frmNesting_Load
        AddHandler vgParts.CustomDrawRowHeaderCell, AddressOf vgParts_CustomDrawRowHeaderCell
        AddHandler vgParts.EndDragRow, AddressOf vgParts_EndDragRow
        AddHandler vgParts.ProcessDragRow, AddressOf vgParts_ProcessDragRow

        ' Add any initialization after the InitializeComponent() call.
        repositoryItemButtonEdit = New RepositoryItemButtonEdit()
        ' Add any initialization after the InitializeComponent() call.

        xtcResults.SelectedTabPage = xtpDummy

        m_is_view_initialized = False
        bEmptyPdb = False
        m_is_nesting = True
        Me.SuspendLayout()


        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        If (IS_CCS_FAB = False) Then

            pnlXLSFilename.Visible = False

            'frmCCS_FAB_Setup.ShowDialog(frmMain)

        End If

        m_pdb_filename = PathPDB()

        If (FileExists(m_pdb_filename) = True) Then

            m_isVirginDB = False

            LoadPDB(m_pdb_filename)
        Else

            'Dim result = MessageBox.Show("Nesting Database: " & m_pdb_filename & _
            '                            " Does Not Exist!" & Environment.NewLine & Environment.NewLine & _
            '                           "Would you like to create a new database?", _
            '                          "Nest Databse Does Not Exist", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation)
            '
            'If (result = DialogResult.No) Then

            'bEmptyPdb = True
            'Else
            'CreateNewPDB()
            'End If


        End If

        m_is_active = True

        Me.ResumeLayout()

        Windows.Forms.Cursor.Current = Cursors.Default

        Me.Focus()
    End Sub

    Private Sub bbNew_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbNew.ItemClick
        Dim Pdb_FileSave As New SaveFileDialog

        Pdb_FileSave.Filter = "PDB files (*.pdb)|*.pdb"
        Pdb_FileSave.FilterIndex = 1

        If (FolderExists(PathDriveFolder(PathNestFolder())) = True) Then
            Pdb_FileSave.InitialDirectory = PathDriveFolder(PathNestFolder())
        Else
            Pdb_FileSave.InitialDirectory = App_Path & "\nests\"
        End If

        If Pdb_FileSave.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            m_pdb_filename = Pdb_FileSave.FileName

            Pdb_FileSave.Dispose()

            ConditionalFileDelete(m_pdb_filename)

            PathPDB_Set(m_pdb_filename)


            CreateNewPDB()
            frmMain.doNesting()


        Else
            Me.Dispose()
        End If

    End Sub

    Private Sub CreateRootNodes(ByVal tl As DevExpress.XtraTreeList.TreeList, sPDB_Name As String)
        Dim pdb_displayname As String
        Dim sPdb_Name_Tag As String



        pdb_displayname = PathFileExt(sPDB_Name)

        bFromCreateNodes = True
        tl.ClearNodes()

        CFG_ROOT_NODE = tl.AppendNode(New Object() {"Job(s)"}, 0)
        CFG_ROOT_NODE.Tag = 0

        'use the tag field to store boithe the node type tag and the pdb filename.
        ' we will then parse out the pdbname from the tag when needed for opening the database

        sPdb_Name_Tag = TAG_SETTINGS_NODE & "," & sPDB_Name

        CFG_SETTINGS_NODE = tl.AppendNode(New Object() {pdb_displayname}, CFG_ROOT_NODE, sPdb_Name_Tag)

        CFG_PARTS_NODE = tl.AppendNode(New Object() {"Parts"}, CFG_SETTINGS_NODE, TAG_PARTS_NODE)
        CFG_MATERIAL_NODE = tl.AppendNode(New Object() {"Material"}, CFG_SETTINGS_NODE, TAG_MATERIAL_NODE)
        CFG_RESULTS_NODE = tl.AppendNode(New Object() {"Results (Pending)"}, CFG_SETTINGS_NODE, TAG_RESULTS_NODE)

        For Each column As DevExpress.XtraTreeList.Columns.TreeListColumn In TV_NestConfig.Columns
            column.OptionsColumn.AllowMove = False
        Next

        CFG_ROOT_NODE.Expanded = True
        CFG_SETTINGS_NODE.Expanded = True
        tl.FocusedNode = CFG_SETTINGS_NODE


        Dim crPartVarHeader As CategoryRow = Nothing
        vgPartVars.Rows.Clear()

        crPartVarHeader = CreateCategoryRow(vgPartVars, "Part Varables", "Part Variables", "Part Variables", "Part Variables", "Part Variables", DB_HEADER, 0, sEmpty, 0)

        CreatePartDefaultValues(crPartVarHeader, 0, 0, True)

        Dim crPartResultVarHeader As CategoryRow = Nothing
        vgPartResults.Rows.Clear()

        crPartResultVarHeader = CreateCategoryRow(vgPartResults, "Part Varables", "Part Variables", "Part Variables", "Part Variables", "Part Variables", DB_HEADER, 0, sEmpty, 0)

        CreatePartResultValues(crPartResultVarHeader, 0, 0, True)

    End Sub


    Private Function TreeItemType(nNode As TreeListNode) As Long

        Dim Parent As TreeListNode
        Dim itemTag As Long
        Dim sPieces() As String

        Try

            Parent = nNode.ParentNode

            'If the parent node is the pdb name node then we need to parse the node type and full pdb name
            sPieces = Split(Parent.Tag, ",")

            If (sPieces.Length > 1) Then
                itemTag = sPieces(0)
            Else

                itemTag = Parent.Tag

            End If

            itemTag = itemTag '- 1

            TreeItemType = itemTag

        Catch ex As ArgumentException

            MessageBox.Show(ex.Message)

        End Try

    End Function


    Private Sub HideControls(nTheMode As Long)

        Dim sNestedSheetName As String
        Dim spieces() As String
        '        Dim er_SelectedMaterialRow As EditorRow

        Windows.Forms.Cursor.Current = Cursors.WaitCursor


        sbExportCNC.Enabled = False
        sbCodeViewer.Enabled = False

        vgParts.FocusedRow = Nothing


        Select Case nTheMode

            Case TAG_ROOT_NODE

                cmdBuild.Text = "Build All"
                sbExportCNC.Enabled = True

            Case TAG_SETTINGS_NODE
                cmdBuild.Text = "Build Current"

                If (InStr(1, UCase$(m_command), "~XLS") > 0) Then
                    sbExportCNC.Enabled = False
                Else
                    sbExportCNC.Enabled = True
                End If

                CFG_SETTINGS_NODE = TV_NestConfig.FocusedNode

                spieces = Split(TV_NestConfig.FocusedNode.Tag, ",")

                If (spieces.Length > 1) Then
                    m_pdb_filename = spieces(1)

                    PathPDB_Set(m_pdb_filename)

                    modNesting.SetupInfoGet(PathPDB())

                    PopulateDefaultSettings()
                    GetSetupValuesFromPDB()


                End If

                xtcResults.SelectedTabPage = xtpSettings

            Case TAG_PARTS_NODE
                sbExportCNC.Enabled = False

                Dim crPartVarHeader As CategoryRow = Nothing
                vgPartVars.Rows.Clear()

                crPartVarHeader = CreateCategoryRow(vgPartVars, "Part Varables", "Part Variables", "Part Variables", "Part Variables", "Part Variables", DB_HEADER, 0, sEmpty, 0)

                CreatePartDefaultValues(crPartVarHeader, 0, 0, True)

                xtcResults.SelectedTabPage = xtpParts

                If (ArePartsLoaded = False Or bFromMrp = True) Then

                    spieces = Split(TV_NestConfig.FocusedNode.ParentNode.Tag, ",")

                    If (spieces.Length > 1) Then

                        m_pdb_filename = spieces(1)
                        PathPDB_Set(m_pdb_filename)

                    End If

                    LoadPartsFromDB()

                    ArePartsLoaded = True
                End If

                If (vgParts.Rows.Count > 1) Then
                    vgParts.FocusedRow = vgParts.Rows(1)
                    vgParts.SelectRow(vgParts.FocusedRow)

                End If
            Case TAG_MATERIAL_NODE
                sbExportCNC.Enabled = False
                spieces = Split(TV_NestConfig.FocusedNode.ParentNode.Tag, ",")

                If (spieces.Length > 1) Then

                    m_pdb_filename = spieces(1)
                    PathPDB_Set(m_pdb_filename)

                End If


                LoadMaterials()

                LoadMaterialFromDB()



                xtcResults.SelectedTabPage = xtpMaterial

                vgMaterial.FocusedRow = vgMaterial.Rows(0).ChildRows(0)

                er_SelectedMaterialRow = vgMaterial.FocusedRow

                PopulateMaterialParameters(er_SelectedMaterialRow)



                cmdBuild.Text = "Build Current"

            Case TAG_RESULTS_NODE


            Case TAG_RESULTS_SHEET

                xtcResults.SelectedTabPage = xtpSheetResults

                spieces = Split(TV_NestConfig.FocusedNode.Tag, ",")

                If (spieces.Length > 1) Then
                    sNestedSheetName = spieces(1)
                Else

                    sNestedSheetName = Nothing

                End If

                ShowSheetResults(TV_NestConfig.FocusedNode.Tag)


                sNestedSheetName = ImportFile(sNestedSheetName, sNestedSheetName)

                If (InStr(1, UCase$(m_command), "~XLS") > 0) Then
                    sbExportCNC.Enabled = False
                Else
                    sbExportCNC.Enabled = True
                End If

                sbCodeViewer.Enabled = True

            Case TAG_RESULTS_PART

                xtcResults.SelectedTabPage = xtpPartResults

                FillPartResultsGrid(TV_NestConfig.FocusedNode.Tag)


        End Select

        m_last_Mode = nTheMode

        Windows.Forms.Cursor.Current = Cursors.Default

        frmMain.Focus()

    End Sub


    Private Sub PopulateMaterialParameters(sActualMaterial As EditorRow)
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim dsMaterial As New DataSet
        Dim sql As String
        Dim dLength As Double
        Dim dWidth As Double
        Dim dThickness As Double
        Dim iQuantity As Integer
        Dim dWeight As Double
        Dim dCostPerUnit As Double
        Dim nTypeID As Long
        Dim sDescription As String = ""
        Dim nMatID As Long

        Try
            vgMatVars.Rows.Clear()
            Dim rowParameters As New CategoryRow("Parameters")

            vgMatVars.Rows.Add(rowParameters)

            If (sActualMaterial IsNot Nothing) Then


                nMatID = CLng(GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "PMatID", 0))

                If (nMatID > 0) Then

                    sql = "Select * From [Material Inventory] Where (ID = " & nMatID & " )"

                Else
                    nMatID = CLng(GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "MatID", 0))
                    sql = "Select * From [Material Inventory] Where (ID = " & nMatID & " )"
                End If

                connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"



                connection = New OleDbConnection(connetionString)

                connection.Open()

                oledbAdapter = New OleDbDataAdapter(sql, connection)

                oledbAdapter.Fill(dsMaterial)

                oledbAdapter.Dispose()

                For i = 0 To dsMaterial.Tables(0).Rows.Count - 1

                    sDescription = vgMaterial.FocusedRow.Properties.Caption
                    dLength = dsMaterial.Tables(0).Rows(i).Item("Length")
                    dWidth = dsMaterial.Tables(0).Rows(i).Item("Width")
                    dThickness = dsMaterial.Tables(0).Rows(i).Item("Thickness")
                    iQuantity = dsMaterial.Tables(0).Rows(i).Item("Quantity")
                    dWeight = dsMaterial.Tables(0).Rows(i).Item("Weight")
                    dCostPerUnit = dsMaterial.Tables(0).Rows(i).Item("Cost Per Unit")
                    nTypeID = GetMaterialTypeIDFromDescription(vgMaterial.FocusedRow.Properties.Caption)

                    lReturn = SetParameterValues(rowParameters, dLength, dWidth, dThickness, iQuantity, dWeight, dCostPerUnit, sDescription, nMatID)
                Next

                connection.Close()

                dsMaterial.Dispose()
                oledbAdapter.Dispose()

                If dsMaterial IsNot Nothing Then dsMaterial = Nothing
                If connection IsNot Nothing Then connection = Nothing
                If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing


            Else
                sDescription = ""
                dLength = 0
                dWidth = 0
                dThickness = 0
                iQuantity = 0
                dWeight = 0
                dCostPerUnit = 0
                nMatID = -1
                nTypeID = -1

                lReturn = SetParameterValues(rowParameters, dLength, dWidth, dThickness, iQuantity, dWeight, dCostPerUnit, sDescription, nMatID)

            End If
        Catch

        Finally

            If dsMaterial IsNot Nothing Then dsMaterial = Nothing

        End Try

    End Sub
    Private Sub PopulateDefaultSettings()
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim myMachineList As MachineListCombo = New MachineListCombo()
        Dim sDefaultName As String = Nothing

        Dim datfilename As String = Nothing
        Dim sTemplate As String = RegGetString("Nesting", "Template", "")


        vgSettingsProplist.SuspendLayout()
        vgSettingsProplist.Rows.Clear()
        currentCategoryRow = CreateCategoryRow(vgSettingsProplist, "glb.Template",
                                                                "Template File",
                                                                sTemplate,
                                                                sTemplate,
                                                                "Template",
                                                                DB_HEADER,
                                                                0,
                                                                sEmpty, 0)

        currentCategoryRow.ChildRows.Add(CreateButtonEditEditorRow(vgSettingsProplist,
                                                                "glb.Template",
                                                                "Template",
                                                                sTemplate,
                                                                sTemplate,
                                                                "Template",
                                                                DB_FILE,
                                                                0,
                                                                "Template Files (*.template);*.template", 0))



        sDefaultName = RegGetString("Global", "NestResultPath", App_Path & "\nests")
        currentCategoryRow.ChildRows.Add(CreateFileEditEditorRow(vgSettingsProplist,
                                                                 "glb.ResultPath",
                                                                 "Result Path",
                                                                 sDefaultName,
                                                                 sDefaultName,
                                                                 "Result Path",
                                                                 DB_FOLDER,
                                                                 0,
                                                                 sEmpty, 0))

        arrMachine = PopulateComboList("SELECT * FROM [Machines]", True, False)

        currentCategoryRow = CreateCategoryRow(vgSettingsProplist, "glb.Machine",
                                                                     "Machines",
                                                                    s_Machine,
                                                                    m_machine_id,
                                                                    "Machine ID",
                                                                    DB_HEADER,
                                                                    0,
                                                                    sEmpty, 0)

        sDefaultName = RegGetString("Global", "Machine", "")

        s_Machine = sDefaultName

        m_machine_id = modMachine.GetMachineIDFromDescrip(s_Machine)

        If (m_machine_id < 1) Then
            s_Machine = "None"

        End If
        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgSettingsProplist,
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

        currentCategoryRow = CreateCategoryRow(vgSettingsProplist, "glb.ToolSetupID",
                                                                     "Tool_Setup",
                                                                     s_ToolSetup,
                                                                     0,
                                                                     "Tool Setup ID", DB_HEADER,
                                                                     0, sEmpty, 0)

        Dim n_ToolSetupID As Integer
        sDefaultName = RegGetString("Global", "ToolSetup", "")
        s_ToolSetup = sDefaultName
        n_ToolSetupID = modTranslate.GetToolSetupIDFromDescription(s_ToolSetup)

        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgSettingsProplist,
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

        currentCategoryRow = CreateCategoryRow(vgSettingsProplist, "glb.LayerSetupID",
                                                                     "Layer_Setup",
                                                                     s_LayerSetup,
                                                                     0,
                                                                     "Layer Setup ID",
                                                                     DB_HEADER,
                                                                     0,
                                                                     sEmpty, 0)
        sDefaultName = RegGetString("Global", "LayerSetup", "None")

        Dim n_LayerSetupID As Integer = modTranslate.GetLayerSetupID(sDefaultName, m_machine_id)

        If (sDefaultName = String.Empty) Then
            sDefaultName = "None"
        End If

        s_LayerSetup = sDefaultName

        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgSettingsProplist,
                                                                     "glb.LayerSetupID",
                                                                     "Layer Setup",
                                                                     s_LayerSetup,
                                                                     n_LayerSetupID,
                                                                     "Layer Setup ID",
                                                                     DB_LAYERSETUP,
                                                                     0,
                                                                     arrLayerSetup, 0))

        arrLeadSetup = PopulateComboList("SELECT * FROM [Lead Setup] ORDER BY [Description]", False, True)

        sDefaultName = RegGetString("Global", "LeadSetup", "None")

        Dim n_LeadSetupID As Integer

        If (sDefaultName = String.Empty) Then
            sDefaultName = "None"
        End If

        s_LeadSetup = sDefaultName
        n_LeadSetupID = modTranslate.GetLeadSetupID(s_LeadSetup)

        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgSettingsProplist, "glb.LeadSetupID",
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

        populateFromDatFile(vgSettingsProplist, datfilename, currentCategoryRow, lbOptions)

        GetRemnantControlValues()


        vgSettingsProplist.BestFit()
        vgSettingsProplist.ResumeLayout()


    End Sub

    Private Sub StoreGlobalVars()

        Dim folder As String

        PathPDB_Set(m_pdb_filename)

        folder = PathDriveFolder(PathPDB())

        If Len(folder) <> 0 Then
            RegPutString("Nesting", "Database", folder)

        Else
            PathPDB_Set(NormalizePath(RegGetString("Nesting", "Database", App_Path & "\Database\")) & PathPDB())
        End If

    End Sub

    Private Sub CreatePDB()
        Dim pdb_name As String
        Dim sResultPath As String
        Dim l_MachineID As Long
        Dim l_LayerSetupID As Long
        Dim l_LeadSetupID As Long
        Dim l_ToolSetupid As Long


        StoreGlobalVars()

        sResultPath = RegGetString("Global", "NestResultPath", App_Path & "\nests")
        l_MachineID = RegGetString("Global", "MachineID", 0)
        l_LayerSetupID = RegGetString("Global", "LayerSetupID", 0)
        l_LeadSetupID = RegGetString("Global", "LeadSetupID", 0)
        l_ToolSetupid = RegGetString("Global", "ToolSetupID", 0)

        pdb_name = m_pdb_filename

        pdb_info = InitPdbInfo(
            pdb_name, sResultPath, s_Machine, l_MachineID,
            s_ToolSetup, l_ToolSetupid, "", 0,
            s_LeadSetup, l_LeadSetupID,
            s_LayerSetup, l_LayerSetupID)

        If (EstablishPDB(pdb_info) = String.Empty) Then
            MsgBox("Unable to create nesting database. " & vbCrLf & pdb_name)
        Else
            PathPDB_Set(pdb_name)

            m_isVirginDB = False
        End If

    End Sub
    Private Sub UpdateSetupTable()
        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim m_cnSettings As New OleDb.OleDbConnection
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim m_dtSettings As New DataTable
        Dim sql As String
        Dim ds As New DataSet
        Dim drNewRow As DataRow = m_dtSettings.NewRow()
        Dim connetionString As String
        Dim cmdGetID As New OleDbCommand
        Dim sFieldName As String = ""
        Dim sFieldValue As String = ""
        Dim sMyTag As String

        Try

            my_DatVarName = Nothing
            my_DialogDisplay = Nothing
            my_VBValue = Nothing
            my_CValue = Nothing
            my_CVarName = Nothing
            tbFieldNames.Text = Nothing
            tbFieldValues.Text = Nothing

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            m_cnSettings = New OleDbConnection(connetionString)

            m_cnSettings.Open()

            For Each row As CategoryRow In vgSettingsProplist.Rows

                For Each childRow As EditorRow In row.ChildRows

                    If (UCase(childRow.Properties.Caption) <> "ID") Then
                        sMyTag = childRow.Tag

                        If (UCase(childRow.Properties.Caption) = "COLOR") Then

                            sFieldValue = ColorTranslator.ToOle(childRow.Properties.Value)

                            my_CValue = sFieldValue
                            my_VBValue = sFieldValue

                        Else

                            my_VBValue = GetValueFromDelimitedData(sMyTag, "VBValue", 0)
                            my_CValue = GetValueFromDelimitedData(sMyTag, "CValue", 0)

                        End If

                        my_DatVarName = GetValueFromDelimitedData(sMyTag, "DatVarName", 0)
                        my_DialogDisplay = GetValueFromDelimitedData(sMyTag, "DialogDisplay", 0)
                        my_CVarName = GetValueFromDelimitedData(sMyTag, "CVarName", 0)
                        my_DataType = GetValueFromDelimitedData(sMyTag, "VarDataType", 0)

                    End If

                    If (my_DataType = DB_BOOL) Then

                        If (my_VBValue = 0) Then
                            my_VBValue = "FALSE"

                        Else

                            my_VBValue = "TRUE"
                        End If
                    End If

                    tbFieldNames.Text = "DatVarName, DialogDisplay, VBValue, CValue, CVarName"
                    tbFieldValues.Text = "'" & my_DatVarName & "','" & my_DialogDisplay & "','" & my_VBValue & "','" & my_CValue & "','" & my_CVarName & "'"

                    sql = "INSERT INTO [Setup] (" & tbFieldNames.Text & " ) VALUES(" & tbFieldValues.Text & ")"

                    m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

                    m_daSettings.InsertCommand.ExecuteNonQuery()



                Next childRow

            Next row

            m_cnSettings.Close()

            If m_cnSettings IsNot Nothing Then m_cnSettings = Nothing
            If m_daSettings IsNot Nothing Then m_daSettings = Nothing


        Catch ex As Exception

            MessageBox.Show("UpdateSetupTable " & ex.Message)

        End Try
    End Sub
    Private Sub GetSetupValuesFromPDB()
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim sql As String
        Dim my_VBValue As String
        Dim my_CVarValue As String
        Dim my_CVarName As String
        Dim sDialogDisplay As String

        Try

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            sql = "Select * From [Setup]"

            'connection = New OleDbConnection(connetionString)

            'connection.Open()

            'oledbAdapter = New OleDbDataAdapter(sql, connection)

            'oledbAdapter.Fill(dsSetup)

            'oledbAdapter.Dispose()

            For Each row As CategoryRow In vgSettingsProplist.Rows

                For Each childRow As EditorRow In row.ChildRows

                    Dim dsSetup As New DataSet

                    sql = "Select Setup.* From Setup Where (Setup.DialogDisplay = '" & childRow.Properties.Caption & "' )"

                    connection = New OleDbConnection(connetionString)

                    connection.Open()

                    oledbAdapter = New OleDbDataAdapter(sql, connection)

                    oledbAdapter.Fill(dsSetup)

                    oledbAdapter.Dispose()

                    If (dsSetup.Tables(0).Rows.Count > 0) Then

                        sDialogDisplay = dsSetup.Tables(0).Rows(0).Item("DialogDisplay").ToString
                        my_VBValue = dsSetup.Tables(0).Rows(0).Item("VBValue")
                        my_CVarName = dsSetup.Tables(0).Rows(0).Item("CVarName").ToString()
                        my_CVarValue = dsSetup.Tables(0).Rows(0).Item("CValue").ToString

                        If (UCase(my_CVarName) = "PONUM") Then

                            m_PONum = my_VBValue

                        End If

                        If (UCase(my_CVarName) = "CUSTOMER") Then
                            m_Customer = my_VBValue
                        End If


                        Select Case UCase(sDialogDisplay)

                            Case "MACHINE"
                                modProject.SetMachine(my_VBValue, my_CVarValue)

                            Case "TOOL SETUP"
                                modProject.SetToolSetup(my_VBValue, my_CVarValue)

                            Case "LAYER SETUP"
                                modProject.SetLayerSetup(my_VBValue, my_CVarValue)

                            Case "LEAD SETUP"
                                modProject.SetLeadSetup(my_VBValue, my_CVarValue)

                            Case "AUTOCODE"
                                sReturn = RegPutString("Global", "AutoCode", my_CVarValue)

                        End Select
                        If (my_CVarName = "Template") Then

                            s_NestingTemplate = my_VBValue
                            sReturn = RegPutString("Nesting", "Template", s_NestingTemplate)

                        End If

                        Dim rowType As Type = childRow.Properties.RowType

                        If (rowType.Name = "Boolean") Then
                            If (my_VBValue = True) Then


                                childRow.Properties.Value = True
                            Else

                                childRow.Properties.Value = False

                            End If
                        Else

                            childRow.Properties.Value = my_VBValue

                        End If

                    End If

                    connection.Dispose()

                    dsSetup.Tables.Clear()

                    oledbAdapter.Dispose()

                    If connection IsNot Nothing Then connection = Nothing

                    If dsSetup IsNot Nothing Then dsSetup = Nothing

                    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing
                Next

            Next

            vgSettingsProplist.BestFit()

        Finally

        End Try
    End Sub

    Private Sub TV_NestConfig_FocusedNodeChanged(sender As Object, e As FocusedNodeChangedEventArgs) Handles TV_NestConfig.FocusedNodeChanged
        Dim itemtype As Long
        Dim nCurr_Node_Tag As Long
        Dim sCurr_Node_Tag As String
        Dim sCurr_Display As String
        Dim nTestNode
        Dim sPieces() As String
        Dim datfilename As String = Nothing

        Try
            sbExportCNC.Enabled = False

            ' node will be nothing if the user had open a pdb and then went to either create a new or open another pdb
            ' so in that case we exit the sub so that the tree can be populated with the new data
            If (IsNothing(e.Node)) Then

                Exit Sub

            End If

            sPieces = Split(e.Node.Tag, ",")


            If (sPieces.Length > 1) Then

                itemtype = sPieces(0)
                sCurr_Node_Tag = sPieces(1)

            Else

                itemtype = e.Node.Tag
                nCurr_Node_Tag = e.Node.Tag

            End If

            sCurr_Display = e.Node.GetDisplayText(TV_NestConfig.Columns(0))

            If (e.Node.ParentNode IsNot Nothing) Then
                nTestNode = e.Node.ParentNode.GetDisplayText(TV_NestConfig.Columns(0))
            End If


            HideControls(itemtype)

            Dim currentCategoryRow As CategoryRow = Nothing

            If (FileExists(NormalizePath(App_Path) & s_Machine & ".dat") = True) Then
                datfilename = NormalizePath(App_Path) & s_Machine & ".dat"
            Else
                datfilename = RegGetDatFilePath()
            End If

            'populateFromDatFile(vgSettingsProplist, datfilename, currentCategoryRow, lbOptions)

        Catch ex As Exception

            MessageBox.Show("TV_NestConfig_FocusedNodeChanged: " & ex.Message)
        End Try

    End Sub

    Private Sub repositoryItemButtonEdit_ButtonClick(sender As Object, e As DevExpress.XtraEditors.Controls.ButtonPressedEventArgs) Handles repositoryItemButtonEdit.ButtonClick
        Dim edit As DevExpress.XtraEditors.ButtonEdit = TryCast(sender, DevExpress.XtraEditors.ButtonEdit)
        Dim sFilename As String
        Dim dialog As New OpenFileDialog()
        Dim m_folderDialog As New FolderBrowserDialog()
        Dim FocusedRow As BaseRow = vgSettingsProplist.FocusedRow
        Dim iname As String
        Dim m_fieldType As Integer
        Dim sFilter As String

        Try
            m_fieldType = CInt(GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "VarDataType", 0))

            Select Case m_fieldType

                Case DB_FILE

                    sFilter = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "Options", sEmpty)
                    sFilter = Replace(sFilter, ";", "|")

                    dialog.Multiselect = False
                    dialog.Title = "Select " & vgSettingsProplist.FocusedRow.Properties.Caption

                    dialog.Filter = sFilter

                    If dialog.ShowDialog(Me) = DialogResult.OK Then
                        edit.EditValue = TrimToFit(dialog.FileName, 35)

                        sFilename = (dialog.FileName)

                        vgSettingsProplist.BeginDataUpdate()
                        vgSettingsProplist.SetCellValue(FocusedRow, vgSettingsProplist.FocusedRecord, sFilename)
                        vgSettingsProplist.EndDataUpdate()

                        iname = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "DatVarName", "")

                        lReturn = UpdateAccumulatorDirect(vgSettingsProplist, iname, sFilename)

                    End If

                Case DB_FOLDER

                    m_folderDialog.SelectedPath = NormalizePath(vgSettingsProplist.FocusedRow.Properties.Value)


                    If (m_folderDialog.ShowDialog() = DialogResult.OK) Then

                        'vgSettingsProplist.BeginDataUpdate()
                        vgSettingsProplist.SetCellValue(FocusedRow, vgSettingsProplist.FocusedRecord, m_folderDialog.SelectedPath)

                        'vgSettingsProplist.EndDataUpdate()

                        iname = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "DatVarName", "")

                        lReturn = UpdateAccumulatorDirect(vgSettingsProplist, iname, m_folderDialog.SelectedPath)


                    End If
            End Select

        Catch ex As Exception

            MessageBox.Show("rbPropSettings_ButtonClick: " & ex.Message)

        End Try

    End Sub

    Private Sub vgSettingsProplist_CellValueChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgSettingsProplist.CellValueChanged
        Dim sPdbName As String
        Dim spieces() As String
        Dim sFieldName As String
        Dim VbVarValue As Object
        Dim CVarValue As Object
        Dim m_cnSettings As New OleDb.OleDbConnection
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim m_dtSettings As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim nDataType As Integer
        Dim sDialogDisplay As String


        Try

            sFieldName = GetValueFromDelimitedData(e.Row.Tag, "DatVarName", sEmpty)

            nDataType = CInt(GetValueFromDelimitedData(e.Row.Tag, "VarDataType", sEmpty))

            If (UCase(sFieldName) = "GLB.PONUM") Then

                m_PONum = e.Value

            End If

            If (UCase(sFieldName) = "GLB.CUSTOMER") Then
                m_Customer = e.Value
            End If

            If (Trim(e.Row.Properties.Caption) = "Result Path") Then

                RegPutString("Global", "NestResultPath", e.Value)

            End If

            VbVarValue = e.Row.Properties.Value

            spieces = Split(TV_NestConfig.FocusedNode.Tag, ",")
            sPdbName = spieces(1)

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPdbName & "';"

            m_cnSettings = New OleDbConnection(connetionString)

            m_cnSettings.Open()

            sql = "UPDATE [Setup] set [Setup].[VbValue] = '" & VbVarValue & "' Where ([Setup].[DatVarName] ='" & sFieldName & "' )"

            m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

            m_daSettings.InsertCommand.ExecuteNonQuery()

            CVarValue = GetCValueBasedOnVarType(nDataType, VbVarValue, e.Row.Tag)

            sql = "UPDATE [Setup] set [Setup].[CValue] = '" & CVarValue & "' Where ([Setup].[DatVarName] ='" & sFieldName & "' )"

            m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

            m_daSettings.InsertCommand.ExecuteNonQuery()

            m_daSettings.Dispose()
            m_cnSettings.Dispose()
            m_cnSettings.Close()

            Select Case nDataType

                Case DB_MACHINE

                    modProject.SetMachine(VbVarValue, CVarValue)

                    CheckIfBuildEnabled()

                Case DB_TOOLSETUP

                    modProject.SetToolSetup(VbVarValue, CVarValue)
                    CheckIfBuildEnabled()

                Case DB_LAYERSETUP

                    modProject.SetLayerSetup(VbVarValue, CVarValue)

                Case DB_LEADSETUP

                    modProject.SetLeadSetup(VbVarValue, CVarValue)

            End Select

            If m_daSettings IsNot Nothing Then m_daSettings = Nothing

            If m_cnSettings IsNot Nothing Then m_cnSettings = Nothing

        Catch ex As Exception

            MessageBox.Show("vgSettingsProplist_CellValueChanged: " & ex.Message)

        End Try

    End Sub

    Private Sub vgSettingsProplist_CustomRecordCellEdit(sender As Object, e As DevExpress.XtraVerticalGrid.Events.GetCustomRowCellEditEventArgs) Handles vgSettingsProplist.CustomRecordCellEdit
        Dim nTemp As Integer

        If (e.Row.Tag IsNot Nothing) Then
            nTemp = CInt(GetValueFromDelimitedData(e.Row.Tag, "VarDataType", 0))


            If (nTemp = DB_FILE Or nTemp = DB_FOLDER) Then
                e.RepositoryItem = repositoryItemButtonEdit
            End If
        End If
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

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgParts,
                                                                my_DatVarName,
                                                                my_DialogDisplay,
                                                                my_VBValue,
                                                                my_CValue,
                                                                my_CVarName,
                                                                my_DataType,
                                                                my_RecodID,
                                                                my_sOptions, 0))

        my_DatVarName = "Fill Part"
        my_DialogDisplay = "Filler"
        my_VBValue = False
        my_CValue = 0
        my_CVarName = "Fill Part"
        my_DataType = DB_BOOL
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(vgParts,
                                                                my_DatVarName,
                                                                my_DialogDisplay,
                                                                my_VBValue,
                                                                my_CValue,
                                                                my_CVarName,
                                                                my_DataType,
                                                                my_RecodID,
                                                                my_sOptions, 0))

        my_DatVarName = "Rotation Num"
        my_DialogDisplay = "Rotation"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Rotation Num"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgParts,
                                                                my_DatVarName,
                                                                my_DialogDisplay,
                                                                my_VBValue,
                                                                my_CValue,
                                                                my_CVarName,
                                                                my_DataType,
                                                                my_RecodID,
                                                                my_sOptions, 0))

        my_DatVarName = "Rotation Start"
        my_DialogDisplay = "Pre Rotate"
        my_VBValue = 0
        my_CValue = 0
        my_CVarName = "Rotation Start"
        my_DataType = DB_DBL
        my_RecodID = 0
        my_sOptions = sEmpty


        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgParts,
                                                               my_DatVarName,
                                                               my_DialogDisplay,
                                                               my_VBValue,
                                                               my_CValue,
                                                               my_CVarName,
                                                               my_DataType,
                                                               my_RecodID,
                                                               my_sOptions, 0))

        my_DatVarName = "PreNest"
        my_DialogDisplay = "Pre Nest"
        my_VBValue = False
        my_CValue = 0
        my_CVarName = "PreNest"
        my_DataType = DB_BOOL
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(vgParts,
                                                                my_DatVarName,
                                                                my_DialogDisplay,
                                                                my_VBValue,
                                                                my_CValue,
                                                                my_CVarName,
                                                                my_DataType,
                                                                my_RecodID,
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


        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgParts,
                                                                        my_DatVarName,
                                                                        my_DialogDisplay,
                                                                        my_VBValue,
                                                                        my_CValue,
                                                                        my_CVarName,
                                                                        my_DataType,
                                                                        my_RecodID,
                                                                        arrOptions, 0))

        my_DatVarName = "Part Priority"
        my_DialogDisplay = "Priority"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Part Priority"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgParts,
                                                                my_DatVarName,
                                                                my_DialogDisplay,
                                                                my_VBValue,
                                                                my_CValue,
                                                                my_CVarName,
                                                                my_DataType,
                                                                my_RecodID,
                                                                my_sOptions, 0))

        my_DatVarName = "Cost Per Part"
        my_DialogDisplay = "Cost Per Part"
        my_VBValue = 0
        my_CValue = 0
        my_CVarName = "Cost Per Part"
        my_DataType = DB_DBL
        my_RecodID = 0
        my_sOptions = sEmpty


        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgParts,
                                                               my_DatVarName,
                                                               my_DialogDisplay,
                                                               my_VBValue,
                                                               my_CValue,
                                                               my_CVarName,
                                                               my_DataType,
                                                               my_RecodID,
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


        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgParts,
                                                                        my_DatVarName,
                                                                        my_DialogDisplay,
                                                                        my_VBValue,
                                                                        my_CValue,
                                                                        my_CVarName,
                                                                        my_DataType,
                                                                        my_RecodID,
                                                                        arrOptions, 0))


        my_DatVarName = "Part_Extents"
        my_DialogDisplay = "Part Extents"
        my_VBValue = sPartExtents
        my_CValue = sPartExtents
        my_CVarName = "Part_Extents"
        my_DataType = DB_STR
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateStringEditorRow(vgParts,
                                                               my_DatVarName,
                                                               my_DialogDisplay,
                                                               my_VBValue,
                                                               my_CValue,
                                                               my_CVarName,
                                                               my_DataType,
                                                               my_RecodID,
                                                               my_sOptions, 0))
        currentCategoryRow.Properties.ReadOnly = True

        If (AddVarRecord = True) Then
            CreateUserVarsForParts(currentCategoryRow, nid)
        End If

    End Sub
    Private Sub CreatePartResultValues(currentCategoryRow As CategoryRow, sPartExtents As String, nid As Integer, AddVarRecord As Boolean)
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

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgParts,
                                                                my_DatVarName,
                                                                my_DialogDisplay,
                                                                my_VBValue,
                                                                my_CValue,
                                                                my_CVarName,
                                                                my_DataType,
                                                                my_RecodID,
                                                                my_sOptions, 0))


        my_DatVarName = "Part ID"
        my_DialogDisplay = "Part ID"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Part Id"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgParts,
                                                                my_DatVarName,
                                                                my_DialogDisplay,
                                                                my_VBValue,
                                                                my_CValue,
                                                                my_CVarName,
                                                                my_DataType,
                                                                my_RecodID,
                                                                my_sOptions, 0))

        my_DatVarName = "Area"
        my_DialogDisplay = "Area"
        my_VBValue = 0
        my_CValue = 0
        my_CVarName = "Area"
        my_DataType = DB_DBL
        my_RecodID = 0
        my_sOptions = sEmpty


        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgParts,
                                                               my_DatVarName,
                                                               my_DialogDisplay,
                                                               my_VBValue,
                                                               my_CValue,
                                                               my_CVarName,
                                                               my_DataType,
                                                               my_RecodID,
                                                               my_sOptions, 0))


    End Sub

    Private Sub CreateUserVarsForParts(currentCategoryRow As CategoryRow, nid As Integer)
        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer
        Dim my_sOptions As String
        Dim iPartLastIndex As Integer
        Dim iUserLastIndex As Integer
        Dim ni As Integer

        my_DatVarName = "Part Quantity"
        my_DialogDisplay = "Quantity"
        my_VBValue = 1
        my_CValue = 1
        my_CVarName = "Part Quantity"
        my_DataType = DB_INT
        my_RecodID = 0
        my_sOptions = sEmpty

        iPartLastIndex = currentCategoryRow.Properties.Row.ChildRows.LastVisible.Index

        populateFromDatFile(vgParts, RegGetString("Nesting", "Template", ""), currentCategoryRow, lbOptions)

        iUserLastIndex = currentCategoryRow.Properties.Row.ChildRows.LastVisible.Index

        For ni = iPartLastIndex + 1 To iUserLastIndex

            my_VBValue = GetValueFromDelimitedData(currentCategoryRow.ChildRows(ni).Tag, "VBValue", "None")
            my_DialogDisplay = GetValueFromDelimitedData(currentCategoryRow.ChildRows(ni).Tag, "DialogDisplay", 0)

        Next

    End Sub

    Private Sub addRecordToUserVarTable(nid As Integer, my_VBValue As String, my_DialogDisplay As String)
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim m_dtPart As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim cmdGetID As New OleDbCommand
        Dim sPdbName As String

        Try

            sPdbName = m_pdb_filename

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPdbName & "';"

            sql = "INSERT INTO [Part Variables] ([Part ID], [Value], [Description] ) VALUES(" & nid & ", '" & my_VBValue & "', '" & my_DialogDisplay & "')"

            m_cnPart = New OleDbConnection(connetionString)

            m_cnPart.Open()

            m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

            m_daPart.InsertCommand.ExecuteNonQuery()

            m_daPart.Dispose()
            m_cnPart.Dispose()
            m_cnPart.Close()

            If m_cnPart IsNot Nothing Then m_cnPart = Nothing
            If m_daPart IsNot Nothing Then m_daPart = Nothing

        Catch ex As Exception

            MessageBox.Show("addRecordToUserVarTable: " & ex.Message)

        End Try
    End Sub
    Private Sub FocusText(ByRef TheVerticalGrid)
        If TypeOf TheVerticalGrid.ActiveEditor Is TextEdit Then
            Dim edit As TextEdit = TryCast(TheVerticalGrid.ActiveEditor, TextEdit)
            edit.SelectionStart = 0
            edit.SelectionLength = (CStr(edit.EditValue)).Length

        End If
    End Sub
    Private Sub LoadMaterialParams()
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim sql As String
        Dim ds As New DataSet
        Dim i As Integer
        Dim dtMaterialType As DataTable = Nothing
        Dim dLength As Double
        Dim dWidth As Double
        Dim dThickness As Double
        Dim iQuantity As Integer
        Dim dWeight As Double
        Dim dCostPerUnit As Double
        Dim nTypeID As Long
        Dim sDescription As String = ""
        Dim nMatID As Long

        vgMatVars.Rows.Clear()

        nMatID = CLng(GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "MatID", 0))

        sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].ID = " & nMatID & " )"

        dtMaterialType = ConnectToCMDB(sql)

        Dim rowParameters As New CategoryRow("Parameters")

        vgMatVars.Rows.Add(rowParameters)

        For i = 0 To dtMaterialType.Rows.Count - 1
            sDescription = vgMaterial.FocusedRow.Properties.Caption
            dLength = dtMaterialType.Rows(i).Item("Length")
            dWidth = dtMaterialType.Rows(i).Item("Width")
            iQuantity = dtMaterialType.Rows(i).Item("Quantity")
            dWeight = dtMaterialType.Rows(i).Item("Weight")
            dCostPerUnit = dtMaterialType.Rows(i).Item("Cost Per Unit")
            dThickness = dtMaterialType.Rows(i).Item("Thickness")
            nTypeID = GetMaterialTypeIDFromDescription(vgMaterial.FocusedRow.Properties.Caption)

            lReturn = SetParameterValues(rowParameters, dLength, dWidth, dThickness, iQuantity, dWeight, dCostPerUnit, sDescription, nMatID)

        Next
        DatabaseClose()

    End Sub
    Public Sub LoadMaterials()
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim sql As String
        Dim ds As New DataSet
        Dim s_MaterialCategory As String
        Dim n_MaterialID As Integer
        Dim i As Integer
        Dim dtMaterialType As DataTable = Nothing
        'If (IsMaterialLoaded And bFromMrp = False) Then
        'Exit Sub

        'End If
        vgMaterial.Rows.Clear()

        currentCategoryRow = CreateCategoryRow(vgMaterial, "Process Order", "ProcessOrder", "ProcessOrder", "ProcessOrder", "ProcessOrder", DB_HEADER, 0, sEmpty, 0)

        Dim rowMaterialInentory As New CategoryRow("Material Inventory")

        vgMaterial.Rows.Add(rowMaterialInentory)

        DatabaseClose()

        s_MaterialCategory = ""

        sql = "SELECT * FROM [Material Types] ORDER BY Display"

        Try

            dtMaterialType = ConnectToCMDB(sql)

            For i = 0 To dtMaterialType.Rows.Count - 1
                s_MaterialCategory = dtMaterialType.Rows(i).Item("Description")


                n_MaterialID = dtMaterialType.Rows(i).Item("ID")

                Dim rowMaterialType As New EditorRow(s_MaterialCategory)

                rowMaterialType.Properties.Caption = s_MaterialCategory
                rowMaterialInentory.ChildRows.Add(rowMaterialType)

                FillMaterialNodes(n_MaterialID, currentCategoryRow, rowMaterialType)

                dtMaterialType.Dispose()

            Next


            Dim rowRemnantInentory As New CategoryRow("Remnant Inventory")
            vgMaterial.Rows.Add(rowRemnantInentory)

            DatabaseClose()

            FillRemnantInventoryTree(rowRemnantInentory)

            'vgMaterial.CollapseAllRows()  '.ExpandAllRows()


            vgMaterial.BestFit()


        Catch ex As Exception

            MessageBox.Show("LoadMaterials: " & ex.Message)

        End Try

    End Sub
    Public Sub FillRemnantInventoryTree(nRemRow As CategoryRow)
        Dim Sql2 As String
        Dim sql As String
        Dim dtRemnants As DataTable = Nothing
        Dim dtRemnantInv As DataTable = Nothing
        Dim my_DatVarName As String
        Dim my_VBValue As String
        Dim Rem_ID As Long
        Dim Rem_Name As String = Nothing
        Dim rem_length As Double
        Dim rem_width As Double
        Dim rem_thickness As Double
        Dim rem_area As Double
        Dim Rem_Date As String
        Dim rem_material As String
        Dim material_name As String = Nothing
        Dim materialID As Long
        Dim rem_Name_Holder As String = Nothing
        Dim rowRemnantInentory As EditorRow = Nothing

        Try


            sql = "SELECT * FROM Remnant WHERE (Remnant.MachineID > 0 )"
            dtRemnants = ConnectToCMDB(sql)

            For i = 0 To dtRemnants.Rows.Count - 1

                Rem_ID = dtRemnants.Rows(i).Item("ID")

                Sql2 = "SELECT * FROM RemnantVar WHERE (RemnantID = " & Rem_ID & " )"

                dtRemnantInv = ConnectToCMDB(Sql2)

                For ni = 0 To dtRemnantInv.Rows.Count - 1

                    Select Case UCase(dtRemnantInv.Rows(ni).Item("InternalName"))
                        Case "NAME"
                            Rem_Name = dtRemnantInv.Rows(ni).Item("Value")

                        Case "LENGTH"
                            rem_length = dtRemnantInv.Rows(ni).Item("Value")

                        Case "WIDTH"
                            rem_width = dtRemnantInv.Rows(ni).Item("Value")

                        Case "THICKNESS"
                            rem_thickness = dtRemnantInv.Rows(ni).Item("Value")

                        Case "AREA"
                            rem_area = dtRemnantInv.Rows(ni).Item("Value")

                        Case "CREATION_DATE"
                            Rem_Date = dtRemnantInv.Rows(ni).Item("Value")

                        Case "MATERIAL"
                            rem_material = dtRemnantInv.Rows(ni).Item("Value")
                            materialID = GetMaterialTypeIDFromDescription(rem_material)
                            material_name = GetMaterialTypeName(materialID)

                    End Select


                Next

                DatabaseClose()

                If (materialID > 0) Then
                    If (material_name <> rem_Name_Holder) Then
                        rowRemnantInentory = New EditorRow(material_name)
                        rowRemnantInentory.Properties.Caption = material_name
                        rowRemnantInentory.Tag = CHR1 & "IsRem" & CHR2 & True &
                                                CHR1 & "RemID" & CHR2 & Rem_ID &
                                                CHR1 & "MatID" & CHR2 & materialID

                        nRemRow.ChildRows.Add(rowRemnantInentory)

                        rem_Name_Holder = material_name

                    End If

                    my_VBValue = Rem_Name
                    my_DatVarName = "Remnant"

                    Dim sActualMaterial As New EditorRow(my_DatVarName)
                    sActualMaterial.Properties.Caption = my_VBValue
                    sActualMaterial.Tag = CHR1 & "IsRem" & CHR2 & True &
                                                CHR1 & "RemID" & CHR2 & Rem_ID &
                                                CHR1 & "MatID" & CHR2 & materialID

                    rowRemnantInentory.ChildRows.Add(sActualMaterial)
                End If

            Next

        Catch ex As Exception

            MessageBox.Show("FillRemnantInventoryTree: " & ex.Message)

        End Try


    End Sub
    Private Sub LoadRemnantParams()
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim sql As String
        Dim ds As New DataSet
        Dim i As Integer
        Dim dtMaterialType As DataTable = Nothing
        Dim dLength As Double
        Dim dWidth As Double
        Dim dThickness As Double
        Dim iQuantity As Integer
        Dim dArea As Double
        Dim dWeight As Double
        Dim dCostPerUnit As Double
        Dim nTypeID As Long
        Dim sDescription As String = Nothing
        Dim sCreationDate As String = Nothing
        Dim nMatID As Long
        Dim sMaterial As String
        Dim sRem_fileName As String = Nothing


        vgMatVars.Rows.Clear()

        sql = "Select [RemnantVar].* From [RemnantVar] Where ([RemnantVar].[RemnantID] = " & GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "RemID", 0) & " )"

        nMatID = GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "MatID", 0)

        dtMaterialType = ConnectToCMDB(sql)

        Dim rowParameters As New CategoryRow("Parameters")

        vgMatVars.Rows.Add(rowParameters)

        For i = 0 To dtMaterialType.Rows.Count - 1
            Select Case UCase(dtMaterialType.Rows(i).Item("InternalName"))
                Case "NAME"
                    sDescription = dtMaterialType.Rows(i).Item("Value")

                    sRem_fileName = PathRemnants() & "\" & sDescription & ".rem"

                Case "LENGTH"
                    dLength = dtMaterialType.Rows(i).Item("Value")

                Case "WIDTH"
                    dWidth = dtMaterialType.Rows(i).Item("Value")

                Case "THICKNESS"
                    dThickness = dtMaterialType.Rows(i).Item("Value")

                Case "AREA"
                    dArea = dtMaterialType.Rows(i).Item("Value")

                Case "CREATION_DATE"
                    sCreationDate = dtMaterialType.Rows(i).Item("Value")

                Case "MATERIAL"
                    sMaterial = dtMaterialType.Rows(i).Item("Value")
                    nTypeID = GetMaterialTypeIDFromDescription(sMaterial)

                Case "QUANTITY"
                    iQuantity = CInt(dtMaterialType.Rows(i).Item("Value"))

            End Select


        Next

        lReturn = SetRemParameterValues(rowParameters, dLength, dWidth, dThickness, iQuantity, sRem_fileName, sCreationDate, dWeight, dCostPerUnit, nTypeID, sDescription, nMatID)

        DatabaseClose()

    End Sub

    Private Sub FillMaterialNodes(n_MaterialID As Long, currentCategoryRow As CategoryRow, rowMaterialType As EditorRow)
        Dim sql As String
        Dim s_MaterialCategory As String
        Dim dtMaterials As DataTable = Nothing
        Dim i As Integer
        Dim sID As String
        Dim sTheTypeID As String
        Dim my_DatVarName As String
        Dim my_VBValue As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer

        Try
            sql = "SELECT * FROM [Material Inventory] WHERE " &
                "([Type ID] = " & n_MaterialID & " ) " 'ORDER BY [Description]"

            dtMaterials = ConnectToCMDB(sql)

            For i = 0 To dtMaterials.Rows.Count - 1

                s_MaterialCategory = dtMaterials.Rows(i).Item("Description")
                sID = dtMaterials.Rows(i).Item("ID")

                sTheTypeID = dtMaterials.Rows(i).Item("Type ID")

                my_DatVarName = "Description"
                my_VBValue = s_MaterialCategory
                my_DataType = DB_STR
                my_RecodID = sID

                Dim sActualMaterial As New EditorRow(my_DatVarName)
                sActualMaterial.Properties.Caption = my_VBValue
                sActualMaterial.Tag = CHR1 & "IsRem" & CHR2 & False &
                                            CHR1 & "MatID" & CHR2 & sID

                ' adding the Material row to the Material Type row's child collection 
                rowMaterialType.ChildRows.Add(sActualMaterial)


            Next

            'vgMaterial.ExpandAllRows()



            vgMaterial.BestFit()

            DatabaseClose()

        Catch ex As Exception

            MessageBox.Show("FillMaterialNodes: " & ex.Message)

        End Try

    End Sub

    Private Function AddMaterialToOldMaterialInventorys(sDescription As String,
                                                    nTypeID As Integer,
                                                    dLength As Double,
                                                    dWidth As Double,
                                                    dThickness As Double,
                                                    dWeight As Double,
                                                    iQuantity As Integer,
                                                    dCostPerUnit As Double) As Long

        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim m_dtPart As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim cmdGetID As New OleDbCommand
        Dim sPieces() As String
        Dim sPdbName As String

        Try
            AddMaterialToOldMaterialInventorys = -1

            sPieces = Split(TV_NestConfig.FocusedNode.ParentNode.Tag, ",")
            sPdbName = sPieces(1)


            tbFieldNames.Clear()
            tbFieldValues.Clear()

            tbFieldNames.Text = "[Description]," &
                                "[TYpe ID]," &
                                "[Length]," &
                                "[Width]," &
                                "[Thickness]," &
                                "[Weight]," &
                                "[Quantity]," &
                                "[Cost Per Unit]"

            tbFieldValues.Text = "'" & sDescription & "'," &
                                "'" & nTypeID & "'," &
                                "'" & dLength & "'," &
                                "'" & dWidth & "'," &
                                "'" & dThickness & "'," &
                                "'" & dWeight & "'," &
                                "'" & iQuantity & "'," &
                                "'" & dCostPerUnit & "'"


            sql = "INSERT INTO [OLDMaterial Inventory] (" & tbFieldNames.Text & " ) VALUES(" & tbFieldValues.Text & ")"

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPdbName & "';"

            m_cnPart = New OleDbConnection(connetionString)

            m_cnPart.Open()

            m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

            m_daPart.InsertCommand.ExecuteNonQuery()

            cmdGetID.Connection = m_cnPart

            cmdGetID.CommandText = "SELECT MAX(ID) FROM [Material Inventory]"

            AddMaterialToOldMaterialInventorys = cmdGetID.ExecuteScalar

            m_daPart.Dispose()
            m_cnPart.Dispose()
            m_cnPart.Close()

            If m_cnPart IsNot Nothing Then m_cnPart = Nothing
            If m_daPart IsNot Nothing Then m_daPart = Nothing

        Catch ex As Exception

            MessageBox.Show("AddMaterialToMaterialInventorys: " & ex.Message)

        End Try

    End Function
    Private Function AddMaterialToMaterialInventorys(sDescription As String,
                                                    nTypeID As Integer,
                                                    dLength As Double,
                                                    dWidth As Double,
                                                    dThickness As Double,
                                                    dWeight As Double,
                                                    iQuantity As Integer,
                                                    dCostPerUnit As Double) As Long

        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim m_dtPart As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim cmdGetID As New OleDbCommand
        Dim sPieces() As String
        Dim sPdbName As String

        Try
            AddMaterialToMaterialInventorys = -1

            sPieces = Split(TV_NestConfig.FocusedNode.ParentNode.Tag, ",")
            sPdbName = sPieces(1)


            tbFieldNames.Clear()
            tbFieldValues.Clear()

            tbFieldNames.Text = "[Description]," &
                                "[TYpe ID]," &
                                "[Length]," &
                                "[Width]," &
                                "[Thickness]," &
                                "[Weight]," &
                                "[Quantity]," &
                                "[Cost Per Unit]"

            tbFieldValues.Text = "'" & sDescription & "'," &
                                "'" & nTypeID & "'," &
                                "'" & dLength & "'," &
                                "'" & dWidth & "'," &
                                "'" & dThickness & "'," &
                                "'" & dWeight & "'," &
                                "'" & iQuantity & "'," &
                                "'" & dCostPerUnit & "'"


            sql = "INSERT INTO [Material Inventory] (" & tbFieldNames.Text & " ) VALUES(" & tbFieldValues.Text & ")"

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPdbName & "';"

            m_cnPart = New OleDbConnection(connetionString)

            m_cnPart.Open()

            m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

            m_daPart.InsertCommand.ExecuteNonQuery()

            cmdGetID.Connection = m_cnPart

            cmdGetID.CommandText = "SELECT MAX(ID) FROM [Material Inventory]"

            AddMaterialToMaterialInventorys = cmdGetID.ExecuteScalar

            m_daPart.Dispose()
            m_cnPart.Dispose()
            m_cnPart.Close()

            If m_cnPart IsNot Nothing Then m_cnPart = Nothing
            If m_daPart IsNot Nothing Then m_daPart = Nothing

        Catch ex As Exception

            MessageBox.Show("AddMaterialToMaterialInventorys: " & ex.Message)

        End Try

    End Function

    Private Function AddRemnantToMaterialInventorys(sDescription As String,
                                                nTypeID As Integer,
                                                dLength As Double,
                                                dWidth As Double,
                                                dThickness As Double,
                                                dArea As Double,
                                                dWeight As Double,
                                                iQuantity As Integer,
                                                dCostPerUnit As Double,
                                                sCreationDate As String) As Long
        'AddRemnantToMaterialInventorys(Rem_Name, sCMDB_MatID, rem_length, rem_width, rem_thickness, rem_area, rem_weight, rem_qty, rem_cost, Rem_Date)
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim m_dtPart As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim cmdGetID As New OleDbCommand
        Dim sPieces() As String
        Dim sPdbName As String

        Try
            AddRemnantToMaterialInventorys = -1

            sPieces = Split(TV_NestConfig.FocusedNode.ParentNode.Tag, ",")
            sPdbName = sPieces(1)


            tbFieldNames.Clear()
            tbFieldValues.Clear()

            tbFieldNames.Text = "[ID]," &
                                "[Filename]," &
                                "[Type ID]," &
                                "[Length]," &
                                "[Width]," &
                                "[Thickness]," &
                                "[Area]," &
                                "[Weight]," &
                                "[Quantity]," &
                                "[Cost Per Unit]," &
                                "[Creation Date]"

            tbFieldValues.Text = "'" & nTypeID & "'," &
                                "'" & NormalizePath(PathRemnants()) & sDescription & ".rem'," &
                                "'" & nTypeID & "'," &
                                "'" & dLength & "'," &
                                "'" & dWidth & "'," &
                                "'" & dThickness & "'," &
                                "'" & dArea & "'," &
                                "'" & dWeight & "'," &
                                "'" & iQuantity & "'," &
                                "'" & dCostPerUnit & "'," &
                                "'" & sCreationDate & "'"


            sql = "INSERT INTO [Remnant Inventory] (" & tbFieldNames.Text & " ) VALUES(" & tbFieldValues.Text & ")"

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPdbName & "';"

            m_cnPart = New OleDbConnection(connetionString)

            m_cnPart.Open()

            m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

            m_daPart.InsertCommand.ExecuteNonQuery()

            cmdGetID.Connection = m_cnPart

            'cmdGetID.CommandText = "SELECT MAX(ID) FROM [Remnant Inventory]"

            AddRemnantToMaterialInventorys = nTypeID

            sql = "UPDATE [Remnant Inventory] set [Remnant Inventory].[ID ] = '" & nTypeID & "' Where ([Remnant Inventory].[ID] = " & AddRemnantToMaterialInventorys & " ) "

            m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

            m_daPart.InsertCommand.ExecuteNonQuery()


            m_daPart.Dispose()
            m_cnPart.Dispose()
            m_cnPart.Close()

            If m_cnPart IsNot Nothing Then m_cnPart = Nothing
            If m_daPart IsNot Nothing Then m_daPart = Nothing

        Catch ex As Exception

            AddRemnantToMaterialInventorys = -1

            MessageBox.Show("AddRemnantToMaterialInventorys: " & ex.Message)

        End Try

    End Function

    Private Sub bbOpen_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbOpen.ItemClick

        bCameFromStartUp = False
        OpenExistingPDB()

    End Sub

    Public Function OpenExistingPDB() As Long

        Dim Pdb_FileOpen As New OpenFileDialog
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim lSleepMilliSeconds As Integer = 0

        Try


            TV_NestConfig.ClearNodes()
            OpenExistingPDB = 0

            If (bCameFromStartUp = False) Then
                Pdb_FileOpen.Filter = "PDB files (*.pdb)|*.pdb|MRP files (*.MRP)|*.MRP"
                Pdb_FileOpen.FilterIndex = 1



                If (FolderExists(PathDriveFolder(PathPDB())) = True) Then
                    Pdb_FileOpen.InitialDirectory = PathDriveFolder(PathPDB()) & "\"
                Else
                    Pdb_FileOpen.InitialDirectory = App_Path & "\nests\"
                End If

                If (FileExists(PathPDB()) = True) Then
                    Pdb_FileOpen.FileName = PathFileExt(PathPDB())

                End If

                If Pdb_FileOpen.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                    OpenExistingPDB = 1
                    Windows.Forms.Cursor.Current = Cursors.WaitCursor
                    m_pdb_filename = Pdb_FileOpen.FileName

                    PathPDB_Set(m_pdb_filename)
                    LoadPDB(m_pdb_filename)

                End If
            Else

                If (FileExists(PathPDB()) = True) Then
                    Pdb_FileOpen.FileName = PathPDB()

                End If
                OpenExistingPDB = 1
                Windows.Forms.Cursor.Current = Cursors.WaitCursor
                m_pdb_filename = Pdb_FileOpen.FileName

                PathPDB_Set(m_pdb_filename)
                LoadPDB(m_pdb_filename)
                bCameFromStartUp = False
            End If

            Windows.Forms.Cursor.Current = Cursors.Default
        Catch ex As Exception

            MessageBox.Show("bbOpen_ItemClick: " & ex.Message)

        End Try
    End Function



    Private Sub RunAllPDBs()
        Dim cmd As String
        Dim sTemp As String
        Dim m_Length As Double
        Dim m_Width As Double
        Dim m_Thickness As Double
        Dim materialID As Long
        Dim bDebugGraphics As Boolean
        Dim machineID As Long
        Dim ToolSetupID As Long
        Dim lCount As Integer
        Dim sql As String

        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim stock_vis As Boolean
        Dim nParentNode As TreeListNode
        Dim spieces() As String

        nParentNode = TV_NestConfig.Nodes(0)

        For Each childnode In nParentNode.Nodes

            If childnode.level = 1 Then

                TV_NestConfig.FocusedNode = childnode
                CFG_SETTINGS_NODE = childnode


                spieces = Split(TV_NestConfig.FocusedNode.Tag, ",")

                If (spieces.Length > 1) Then
                    m_pdb_filename = spieces(1)

                    PathPDB_Set(m_pdb_filename)

                End If

                If (DoesTableExist("Sheet Result", PathPDB()) = True) Then

                    RemoveExistingResults()

                End If
                Dim ds As New DataSet
                connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

                sql = "SELECT * FROM [Material Inventory]"

                connection = New OleDbConnection(connetionString)

                connection.Open()

                oledbAdapter = New OleDbDataAdapter(sql, connection)

                oledbAdapter.Fill(ds)

                oledbAdapter.Dispose()

                connection.Close()

                If (ds.Tables(0).Rows.Count > 0) Then

                    m_Length = ds.Tables(0).Rows(0).Item("Length")
                    m_Width = ds.Tables(0).Rows(0).Item("Width")
                    m_Thickness = ds.Tables(0).Rows(0).Item("Thickness")
                    materialID = GetMaterialIDFromDescription(Trim(ds.Tables(0).Rows(0).Item("Description")))

                    ds.Dispose()

                Else

                    sql = "SELECT * FROM [Remnant Inventory]"""

                    connection.Open()

                    oledbAdapter = New OleDbDataAdapter(sql, connection)

                    oledbAdapter.Fill(ds)

                    oledbAdapter.Dispose()

                    connection.Close()

                    If (ds.Tables(0).Rows.Count > 0) Then

                        m_Length = ds.Tables(0).Rows(0).Item("Length")
                        m_Width = ds.Tables(0).Rows(0).Item("Width")
                        m_Thickness = ds.Tables(0).Rows(0).Item("Thickness")
                        materialID = ds.Tables(0).Rows(0).Item("[Type ID]")

                    End If

                    ds.Dispose()

                End If

                If ds IsNot Nothing Then ds = Nothing

                If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                If connection IsNot Nothing Then connection = Nothing


                machineID = modProject.GetMachineID()

                ToolSetupID = modProject.GetToolSetupID()

                bDebugGraphics = True

                m_is_nesting = True

                stock_vis = modView.StockLayerVisible(True)
                modView.SetViewModes(VIEW_WIRE)

                sTemp = NewModel("<nesting>", machineID, ToolSetupID, materialID, m_Length, m_Width, m_Thickness)


                cmd = "nest:true:" &
                      " configdb=" & QStr(PathCMDB()) &
                      ",partdb=" & QStr(PathPDB()) &
                      ",display=" & IIf(bDebugGraphics, 1, 0)

                lReturn = PortalExecute(cmd)

                lCount = PortalGetInt("sheet count", 0)

                'Restore the previous stock layer visibility state.
                stock_vis = modView.StockLayerVisible(stock_vis)

                m_is_nesting = False

                '=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                'If (lCount > 0) Then

                NestResults()

                UpdateWeightPerPart()

                TV_NestConfig.FocusedNode.Expanded = True

                'End If

            End If

        Next
    End Sub
    Private Sub DeleteResultsForselectedNode(m_PDB_Node As TreeListNode)

        If m_PDB_Node.HasChildren = True Then

            For Each child As TreeListNode In m_PDB_Node.Nodes

                If child.HasChildren = True Then

                    child.Nodes.Clear()
                    child(0) = "Results (Pending)"

                End If

            Next
        End If

    End Sub

    Private Function GetResultNode(m_pdb_Node As TreeListNode) As TreeListNode
        GetResultNode = Nothing

        If m_pdb_Node.HasChildren = True Then

            For Each child As TreeListNode In m_pdb_Node.Nodes

                If (child(0) = "Results (Pending)") Then

                    GetResultNode = child
                    Exit Function

                End If
                If child.HasChildren = True Then
                    GetResultNode = child
                    child.Nodes.Clear()
                    child(0) = "Results (Pending)"
                    Exit Function

                End If

            Next
        End If

    End Function
    Private Sub NestResults()

        'See also comments at NestPostProc()
        lReturn = modNesting.NestPostProc()


        frmNestResults.ShowDialog(frmMain)

        modView.ViewActivate(VIEW_WIRE)

        If (frmNestResults.CompletionStatus() = vbCancel) Then

            DeleteNestResults()

            RestoreOriginalMats()

            modView.ViewActivate(VIEW_WIRE)

        Else
            TV_NestConfig.SuspendLayout()

            LoadResultsIntoTree(m_pdb_filename)

            'TV_NestConfig.ExpandToLevel(2)

            TV_NestConfig.ResumeLayout()

            TV_NestConfig.FocusedNode = CFG_RESULTS_NODE.FirstNode


        End If

    End Sub
    Private Sub PreviewRefresh()

        lReturn = PortalExecute("!view:materialextents: regen=0")

    End Sub

    Private Sub cmdAddMulti_Click(sender As Object, e As EventArgs) Handles cmdAddMulti.Click
        Dim cnt As Integer
        Dim indx As Integer
        Dim sFileName As String
        Dim sExt As String
        Dim sCandidate As String
        Dim part_data() As clsNestingPartData
        Dim part_user_var_data() As clsNestTemplateVars
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim indx2 As Integer
        Dim sTemp As String = Nothing

        Try
            If (m_debug_tempdir = True) Then
                MessageBox.Show("Into Add Multi")
            End If

            vgParts.Visible = False

            dlgAddMulti.ShowDialog(Me)

            part_data = dlgAddMulti.GetPartData
            part_user_var_data = dlgAddMulti.GetUserVarData

            cnt = dlgAddMulti.FileCount()

            If (m_debug_tempdir = True) Then
                MessageBox.Show("The Add Multi Part Count Is " & cnt)
            End If

            If (cnt > 0) Then

                For indx = 0 To cnt - 1

                    sCandidate = part_data(indx).Path

                    Select Case dlgAddMulti.cboFileType.SelectedIndex
                        Case 0

                            sTemp = PathForceExt(sCandidate, ".mm2")

                        Case 1

                            sTemp = PathForceExt(sCandidate, ".dxf")

                        Case 2

                            sTemp = PathForceExt(sCandidate, ".dwg")

                        Case 3

                            sTemp = PathForceExt(sCandidate, ".psm")

                    End Select



                    sCandidate = sTemp


                    If (m_debug_tempdir = True) Then
                        MessageBox.Show("The Canidate is " & sCandidate)
                    End If

                    If (sCandidate <> "") Then

                        If (m_debug_tempdir = True) Then
                            MessageBox.Show("Check the file extension of the Canidate to determine how the file should be converted")
                        End If

                        sExt = UCase$(PathExt(sCandidate))
                        If (sExt = "MM2") Then

                            'The corresponding MM2 file name.
                            m_current_part = sCandidate

                        Else

                            'The selected file is a CAD file
                            m_current_part = modFileIO.GetTargetName(sCandidate)

                            If (m_debug_tempdir = True) Then
                                MessageBox.Show("The file is a CAD file so we have toConvert it")

                                MessageBox.Show("The coverted filename is " & m_current_part)
                            End If

                        End If

                        If (m_current_part <> "") Then

                            'Because only frmExplorer provides the Pre-Rotate option.
                            lReturn = modConversion.ConversionParametersSet(
                                0.0#, False, dlgAddMulti.GetStripLayers())

                            'At this point, the user has committed to importing the file.
                            modFileIO.AllowMessages(False)

                            If (m_debug_tempdir = True) Then
                                MessageBox.Show("We should be converting " & sCandidate & " to " & m_current_part)
                            End If

                            modView.ViewActivate(VIEW_WIRE)
                            sFileName = modFileIO.ImportFile(sCandidate, m_current_part)

                            modFileIO.AllowMessages(True)

                        End If

                        If (m_current_part <> "") Then

                            Dim sPartExtent As String = CalculateDimensionsJustXY()
                            Dim sPdbName As String
                            Dim spieces() As String
                            Dim nid As Long

                            spieces = Split(TV_NestConfig.FocusedNode.ParentNode.Tag, ",")
                            sPdbName = spieces(1)

                            part_data(indx).part_extents = sPartExtent

                            nid = AddPartToTable(sPdbName,
                                                 m_current_part,
                                                part_data(indx).qty,
                                                part_data(indx).io,
                                                part_data(indx).po,
                                                part_data(indx).repr,
                                                part_data(indx).part_extents)

                            currentCategoryRow = CreateCategoryRow(vgParts, "Filename", PathFile(m_current_part), m_current_part, m_current_part, "Filename", DB_HEADER, nid, sEmpty, 0)


                            If (part_user_var_data IsNot Nothing) Then
                                indx2 = part_data(indx).Var_Data_Count()
                            End If
                            lReturn = GetTotalPartTravel()
                            Dim indx3 As Long = 0

                            DeletePartVars(nid)

                            For indx3 = 0 To indx2 - 1

                                Select Case UCase(part_data(indx).Get_Var_Data_Descrip(indx2))

                                    Case "PIERCES"
                                        lReturn = modNesting.AddPartUserVarsToNestDb(nid,
                                                                                        part_data(indx).Get_Var_Data_Descrip(indx2),
                                                                                        CStr(g_num_Pierces))

                                    Case "BURN TRAVEL"
                                        lReturn = modNesting.AddPartUserVarsToNestDb(nid,
                                                                                        part_data(indx).Get_Var_Data_Descrip(indx2),
                                                                                        CStr(Round(g_CutTravel, Places.TenThousandths)))

                                    Case "LINEAR TRAVEL"
                                        lReturn = modNesting.AddPartUserVarsToNestDb(nid,
                                                                                        part_data(indx).Get_Var_Data_Descrip(indx2),
                                                                                        CStr(Round(g_CutTravel, Places.TenThousandths)))

                                    Case Else
                                        lReturn = modNesting.AddPartUserVarsToNestDb(nid,
                                                                                        part_data(indx).Get_Var_Data_Descrip(indx2),
                                                                                        part_data(indx).Get_Var_Data_Value(indx2))

                                End Select

                            Next


                        End If

                    End If

                Next

            End If

            modView.ViewActivate(VIEW_WIRE)
            modView.ViewRefresh()

            modFileIO.AllowMessages(True)

            vgParts.CollapseAllRows()

            vgParts.Visible = True


            dlgAddMulti.Dispose()

        Catch ex As Exception

            MessageBox.Show("cmdAddMulti_Click: " & ex.Message)

        End Try

    End Sub

    Private Sub cmdAddParts_Click(sender As Object, e As EventArgs) Handles cmdAddParts.Click
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim sCandidate As String
        Dim sFileName As String
        Dim sExt As String
        Dim pre_rotate As Double
        Dim strip_layers As Boolean
        Dim sPdbName As String
        Dim spieces() As String
        Dim nid As Long
        Dim indx As Integer
        Dim sValue As String
        Dim sFieldName As String
        Dim nFieldType As Integer


        m_current_part = ""

        Try

            sCandidate = modFileIO.ImportFileBrowse(Me)

            If (sCandidate <> "") Then

                sExt = UCase$(PathExt(sCandidate))
                If (sExt = "MM2") Then

                    'The corresponding MM2 file name.
                    m_current_part = sCandidate

                Else

                    'The selected file is a CAD file
                    m_current_part = modFileIO.GetTargetName(sCandidate)

                End If

                If (m_current_part <> "") Then

                    pre_rotate = frmExplorer.GetPreRotate()
                    strip_layers = frmExplorer.GetStripLayers()

                    lReturn = modConversion.ConversionParametersSet(
                        pre_rotate, False, strip_layers)

                    'At this point, the user has committed to importing the file.
                    modFileIO.AllowMessages(False)

                    sFileName = modFileIO.ImportFile(sCandidate, m_current_part)

                    modFileIO.AllowMessages(True)

                    'set the dirty flag to false to avoid the
                    '"Do You wnat to save" message
                    'from appearing on each add
                    ModelDirtyClear()

                End If

            End If


            If (m_current_part <> "") Then

                bFromAddButton = True

                Dim sPartExtent As String = CalculateDimensionsJustXY()

                sReturn = RegPutString("Nesting\Part_Defaults", "Part Extents", sPartExtent)


                spieces = Split(TV_NestConfig.FocusedNode.ParentNode.Tag, ",")
                sPdbName = spieces(1)



                'nid = AddPartToTable(sPdbName, _
                '                                 m_current_part, _
                '                               1, _
                '                                0, _
                '                                4, _
                '                                0, _
                '                                sPartExtent)

                'Public Function AddPartToTable(pdb_name As String, _
                '                  sPartFile As String, _
                '                   qty As Long, _
                '                   rot_start As Long, _
                '                   rot_num As Long, _
                '                   repr As Long, _
                '                   part_extents As String) As Long

                nid = AddPartToTable(sPdbName,
                                      m_current_part,
                                      RegGetString("Nesting\Part_Defaults", "Quantity", 1),
                                      RegGetString("Nesting\Part_Defaults", "Pre Rotate", 0),
                                      RegGetString("Nesting\Part_Defaults", "Rotation", 1),
                                      RegGetString("Nesting\Part_Defaults", "Representation", 0),
                                      RegGetString("Nesting\Part_Defaults", "Part Extents", sPartExtent))

                If (vgPartVars.Rows(0).ChildRows.Count > 9) Then

                    For indx = 10 To vgPartVars.Rows(0).ChildRows.Count - 1
                        nFieldType = GetValueFromDelimitedData(vgPartVars.Rows(0).ChildRows(indx).Tag, "VarDataType", 0)
                        sFieldName = GetValueFromDelimitedData(vgPartVars.Rows(0).ChildRows(indx).Tag, "DialogDisplay", 0)
                        sValue = vgPartVars.Rows(0).ChildRows(indx).Properties.Value

                        If (sValue = "") Then

                            sValue = 0

                        End If

                        Select Case nFieldType

                            Case DB_STR, DB_COMBO, DB_DCOMBO, DB_FILE, DB_FOLDER
                                sValue = sValue

                        End Select

                        If (sFieldName = "Linear Travel") Then
                            lReturn = GetTotalPartTravel()
                            sValue = CStr(Round(g_CutTravel, Places.TenThousandths))

                        End If

                        If (sFieldName = "") Then

                        End If

                        AddPartUserVarsToNestDb(nid, sFieldName, sValue)

                    Next

                End If

                currentCategoryRow = CreateCategoryRow(vgParts, "Filename", PathFile(m_current_part), m_current_part, m_current_part, "Filename", DB_HEADER, nid, sEmpty, 0)


                ShowPartAndFillGrid()

                vgParts.FocusedRow = currentCategoryRow
                vgParts.SelectRow(vgParts.FocusedRow)

            End If
            bFromAddButton = False

        Catch ex As Exception
            MessageBox.Show("CmdAddParts: " & ex.Message)
        End Try

    End Sub

    Private Sub vgParts_CellValueChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgParts.CellValueChanged
        Dim sFieldName As String
        Dim nPartID As Integer
        Dim CVarValue As Object
        Dim m_cnSettings As New OleDb.OleDbConnection
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim m_dtSettings As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim nDataType As Integer

        Try
            'If (bFromAddButton = False) Then
            nPartID = GetValueFromDelimitedData(vgParts.FocusedRow.Tag, "ID", 0)
            sFieldName = GetValueFromDelimitedData(e.Row.Tag, "DatVarName", sEmpty)
            nDataType = CInt(GetValueFromDelimitedData(e.Row.Tag, "VarDataType", sEmpty))

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            m_cnSettings = New OleDbConnection(connetionString)

            m_cnSettings.Open()

            CVarValue = GetCValueBasedOnVarType(nDataType, e.Row.Properties.Value, e.Row.Tag)

            sql = "UPDATE [Part] set [Part].[" & sFieldName & "] = " & CVarValue & " Where ([Part].[ID] =" & nPartID & " )"

            m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

            m_daSettings.InsertCommand.ExecuteNonQuery()

            m_daSettings.Dispose()
            m_cnSettings.Dispose()
            m_cnSettings.Close()

            If m_cnSettings IsNot Nothing Then m_cnSettings = Nothing
            If m_daSettings IsNot Nothing Then m_daSettings = Nothing

            ' End If

        Catch ex As Exception

            MessageBox.Show("vgPartVars_CellValueChanged: " & ex.Message)

        End Try

    End Sub


    Private Sub vgParts_CustomDrawRowHeaderCell(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CustomDrawRowHeaderCellEventArgs) Handles vgParts.CustomDrawRowHeaderCell
        Dim sMyFileName As String

        If e.Row.GetType() Is GetType(CategoryRow) AndAlso vgParts.FocusedRow Is e.Row Then
            If (vgParts.FocusedRow.Index <> 0) Then

                sMyFileName = GetValueFromDelimitedData(vgParts.FocusedRow.Tag, "VBValue", "")
                If (FileExists(sMyFileName) = False) Then
                    e.Appearance.BackColor = Color.Red
                Else
                    e.Appearance.BackColor = Color.Gold
                End If
            End If


        End If
    End Sub
    Private Sub vgParts_EndDragRow(sender As Object, e As DevExpress.XtraVerticalGrid.Events.EndDragRowEventArgs) Handles vgParts.EndDragRow


        If (e.Effect = DevExpress.XtraVerticalGrid.RowDragEffect.MoveChild) Then
            e.Effect = DevExpress.XtraVerticalGrid.RowDragEffect.InsertAfter

            bDraggingStarted = False

            RenameTable("Part", "Oldpart")

            If (DoesTableExist("Part Variables", PathPDB()) = False) Then
                ReBuildPartVariableTable()
            Else
                RenameTable("[Part Variables]", "[OldPart Variables]")
            End If

            RemoveExistingTable("Part", PathPDB())
            'RemoveExistingTable("Part Variables", PathPDB())


            BeginInvoke(New MethodInvoker(Sub() UpdatePartTableAndPartVarTableAfterMove()))

        End If

    End Sub
    Private Function ReBuildPartVariableTable() As Boolean
        Dim connection As New OleDb.OleDbConnection("PROVIDER=Microsoft.Jet.OLEDB.4.0;Data Source =" & PathPDB())
        Dim sTablename As String = "Part Variables"
        Dim ds As New DataSet
        Dim dt1 As DataTable = ds.Tables("Part Variables")


        ReBuildPartVariableTable = False

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
                                                    "] ([ID] AUTOINCREMENT UNIQUE  , " &
                                                  "[Part ID] INTEGER, " &
                                                  "[Value] TEXT(255)," &
                                                  "[Description] TEXT(255))", connection)

                connection.Open()
                cmd.ExecuteNonQuery()
                cmd.Dispose()

                If cmd IsNot Nothing Then cmd = Nothing

                connection.Close()

                ReBuildPartVariableTable = True

            End If

            connection.Dispose()


            If connection IsNot Nothing Then connection = Nothing



        Catch ex As Exception

            MessageBox.Show("ReBuildPartVariableTable: " & ex.Message)

        End Try

    End Function
    Private Sub UpdatePartTableAndPartVarTableAfterMove()
        Dim connection As New OleDb.OleDbConnection("PROVIDER=Microsoft.Jet.OLEDB.4.0;Data Source =" & PathPDB())
        Dim dbSchema As New DataTable
        Dim sTablename As String = "Part"
        Dim lst As New List(Of DataColumn)()
        Dim recordID As Long


        Try

            For Each col As DataColumn In table.Columns
                lst.Add(col)
            Next col

            table.Columns.Clear()

            connection.Open()

            'Get database schema 
            dbSchema = connection.GetOleDbSchemaTable(OleDb.OleDbSchemaGuid.Tables, New Object() {Nothing, Nothing, sTablename, "TABLE"})

            connection.Close()

            ' If the table exists, the count = 1 
            If dbSchema.Rows.Count = 0 Then

                'do whatever you want to do if the table does not exist 
                ' e.g. create a table 
                Dim cmd As New OleDb.OleDbCommand("CREATE TABLE [" & sTablename &
                                                    "] ([ID] AUTOINCREMENT UNIQUE  , " &
                                                  "[Filename] TEXT(255), " &
                                                  "[Part Quantity] INTEGER, " &
                                                  "[Fill Part] LOGICAL," &
                                                  "[Rotation Num] INTEGER," &
                                                  "[MirrorPart] INTEGER," &
                                                  "[Rotation Start] FLOAT," &
                                                  "[Part Pass] INTEGER," &
                                                  "[Part Priority] FLOAT," &
                                                  "[Cost Per Part] FLOAT," &
                                                  "[Representation] INTEGER," &
                                                  "[Special Priority] INTEGER," &
                                                  "[Hold ID] INTEGER," &
                                                  "[PreNest] LOGICAL," &
                                                  "[Part_Extents] TEXT(255))", connection)

                connection.Open()
                cmd.ExecuteNonQuery()

                dbSchema.Dispose()
                dbSchema = Nothing

                connection.Dispose()
                connection.Close()

                If cmd IsNot Nothing Then cmd = Nothing
                If dbSchema IsNot Nothing Then dbSchema = Nothing

            End If

            For Each rowItem As BaseRow In vgParts.Rows


                Dim TagVal As String = rowItem.Tag
                For Each col As DataColumn In lst
                    If col.Caption = TagVal Then
                        table.Columns.Add(col)

                        recordID = GetValueFromDelimitedData(TagVal, "nRecordID", 0)

                        lReturn = CopyPartData(recordID)

                        'MessageBox.Show("The tag is < " & recordID & " > ")

                    End If
                Next col
            Next

            If connection IsNot Nothing Then connection = Nothing
            If lst IsNot Nothing Then lst = Nothing


        Catch ex As Exception

            MessageBox.Show("UpdatePartTableAndPartVarTableAfterMove: " & ex.Message)


        End Try


    End Sub
    Private Function CopyPartData(nRecordID As Long) As Long
        Dim m_Filename As String = Nothing
        Dim m_Part_Quantity As Long
        Dim m_Fill_Part As Boolean
        Dim m_Rotation_Num As Long
        Dim m_MirrorPart As Long
        Dim m_Rotation_Start As Double
        Dim m_Part_Pass As Long
        Dim m_Part_Priority As Double
        Dim m_Cost_Per_Part As Double
        Dim m_Representation As Long
        Dim m_Special_Priority As Long
        Dim m_Hold_ID As Long
        Dim m_PreNest As Boolean
        Dim m_Part_Extents As String = Nothing

        Dim dsParts As DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim cnt As Integer
        Dim sFieldNames As String
        Dim sFieldValues As String
        Dim sql As String
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim m_dtPart As New DataTable
        Dim cmdGetID As New OleDbCommand


        Try
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


            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"
            sql = "Select * From OldPart Where (OldPart.ID = " & nRecordID & " )"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)


            dsParts = New DataSet

            oledbAdapter.Fill(dsParts)

            oledbAdapter.Dispose()

            cnt = dsParts.Tables(0).Rows.Count

            If (cnt > 0) Then
                m_Filename = dsParts.Tables(0).Rows(0).Item("Filename")
                m_Part_Quantity = dsParts.Tables(0).Rows(0).Item("Part Quantity")
                m_Fill_Part = dsParts.Tables(0).Rows(0).Item("Fill Part")
                m_Rotation_Num = dsParts.Tables(0).Rows(0).Item("Rotation Num")
                m_MirrorPart = dsParts.Tables(0).Rows(0).Item("MirrorPart")
                m_Rotation_Start = dsParts.Tables(0).Rows(0).Item("Rotation Start")
                m_Part_Pass = dsParts.Tables(0).Rows(0).Item("Part Pass")
                m_Part_Priority = dsParts.Tables(0).Rows(0).Item("Part Priority")
                m_Cost_Per_Part = dsParts.Tables(0).Rows(0).Item("Cost Per Part")
                m_Representation = dsParts.Tables(0).Rows(0).Item("Representation")
                m_Special_Priority = dsParts.Tables(0).Rows(0).Item("Special Priority")
                m_Hold_ID = dsParts.Tables(0).Rows(0).Item("Hold ID")
                m_PreNest = dsParts.Tables(0).Rows(0).Item("PreNest")
                m_Part_Extents = dsParts.Tables(0).Rows(0).Item("Part_Extents")


            End If

            dsParts.Dispose()



            sFieldValues = "'" & m_Filename & "'," &
                                 m_Part_Quantity & "," &
                                m_Fill_Part & "," &
                                m_Rotation_Num & "," &
                                m_MirrorPart & "," &
                                m_Rotation_Start & "," &
                                m_Part_Pass & "," &
                                m_Part_Priority & "," &
                                m_Cost_Per_Part & "," &
                                m_Representation & "," &
                                m_Special_Priority & "," &
                                m_Hold_ID & "," &
                                m_PreNest & "," &
                                "'" & m_Part_Extents & "'"

            sql = "INSERT INTO [Part] (" & sFieldNames & " ) VALUES(" & sFieldValues & ")"

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

            m_cnPart = New OleDbConnection(connetionString)

            m_cnPart.Open()

            m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

            m_daPart.InsertCommand.ExecuteNonQuery()

            cmdGetID.Connection = m_cnPart

            cmdGetID.CommandText = "SELECT MAX(ID) FROM [Part]"

            CopyPartData = cmdGetID.ExecuteScalar

            m_dtPart.Dispose()
            m_cnPart.Dispose()
            m_cnPart.Close()

            If m_dtPart IsNot Nothing Then m_dtPart = Nothing

            If m_daPart IsNot Nothing Then m_daPart = Nothing

            If m_cnPart IsNot Nothing Then m_cnPart = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If dsParts IsNot Nothing Then dsParts = Nothing

            If connection IsNot Nothing Then connection = Nothing


        Catch ex As Exception

            MessageBox.Show("CopyPartData: " & ex.Message)

        End Try

    End Function
    Private Sub vgParts_FocusedRowChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.FocusedRowChangedEventArgs) Handles vgParts.FocusedRowChanged
        ShowPartAndFillGrid()

    End Sub

    Private Sub vgParts_KeyUp(sender As Object, e As KeyEventArgs) Handles vgParts.KeyUp
        If (e.KeyCode = Windows.Forms.Keys.Delete) Then

            DeletePart()

        End If
    End Sub


    Private Function DeleteChildNodesByText(ByRef m_TreeList As TreeList, sTextToFind As String) As Long
        Dim op As New TreeListOperationFindNodeByText(sTextToFind)

        DeleteChildNodesByText = -1

        m_TreeList.NodesIterator.DoOperation(op)

        If op.Node IsNot Nothing Then

            op.Node.Selected = True

            If op.Node.HasChildren Then

                Dim i As Integer = 0

                Do While i < op.Node.Nodes.Count

                    op.Node.Nodes.RemoveAt(i)

                Loop

                DeleteChildNodesByText = 1

                m_TreeList.Refresh()

                m_TreeList.Focus()

            End If

        End If

    End Function
    Friend Class TreeListOperationFindNodeByText
        Inherits TreeListOperation
        Private resultNode As TreeListNode

        Private text As String
        Public Sub New(ByVal text As String)
            MyBase.New()
            Me.text = text
        End Sub
        Public Overrides Sub Execute(ByVal node As TreeListNode)

            For Each col As TreeListColumn In node.TreeList.Columns

                If node(col).ToString() = text Then

                    If resultNode Is Nothing Then

                        resultNode = node

                    End If

                End If

            Next col

        End Sub
        Public ReadOnly Property Node() As TreeListNode
            Get
                Return resultNode

            End Get

        End Property

    End Class
    Public Sub LoadResultsIntoTree(sPdbName As String)
        Dim dsSheets As DataSet
        Dim dsParts As DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim SheetFileName As String
        Dim sSheetTag As String
        Dim cnt As Integer
        Dim cnt2 As Integer
        Dim i As Integer
        Dim i2 As Integer
        Dim sql As String
        Dim tl As TreeList
        Dim nPartId As Integer
        Dim sPartName As String
        Dim sFullSheetName As String = Nothing

        Dim d_Area As Double

        Try
            tl = TV_NestConfig

            Dim currentCategoryRow As CategoryRow = Nothing

            If (DoesTableExist("Sheet Result", sPdbName) = True) Then

                connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & sPdbName & "';"

                sql = "SELECT * FROM [Sheet Result]"

                connection = New OleDbConnection(connetionString)

                connection.Open()

                oledbAdapter = New OleDbDataAdapter(sql, connection)

                dsSheets = New DataSet

                oledbAdapter.Fill(dsSheets)

                oledbAdapter.Dispose()

                connection.Close()

                cnt = dsSheets.Tables(0).Rows.Count

                If (cnt > 0) Then

                    CFG_RESULTS_NODE = GetResultNode(CFG_SETTINGS_NODE)
                    CFG_RESULTS_NODE(0) = "Results Completed"



                    For i = 0 To cnt - 1
                        frmMain.NestingImageCollection.Images.Clear()


                        SheetFileName = dsSheets.Tables(0).Rows(i).Item("Sheet Filename")

                        If (i = 0) Then
                            sFullSheetName = SheetFileName
                        End If

                        sql = "Select [Part Result].* From [Part Result] Where ([Part Result].[Sheet Filename] = '" & SheetFileName & "' )"

                        sSheetTag = TAG_RESULTS_SHEET & "," & SheetFileName

                        SheetFileName = PathFile(SheetFileName)

                        CFG_RESULTS_SHEET = tl.AppendNode(New Object() {SheetFileName}, CFG_RESULTS_NODE, sSheetTag)

                        If (DoesTableExist("Part Result", sPdbName) = True) Then
                            connection = New OleDbConnection(connetionString)

                            connection.Open()

                            oledbAdapter = New OleDbDataAdapter(sql, connection)

                            dsParts = New DataSet

                            oledbAdapter.Fill(dsParts)

                            oledbAdapter.Dispose()

                            connection.Close()
                            'tl.FocusedNode = CFG_RESULTS_SHEET

                            cnt2 = dsParts.Tables(0).Rows.Count

                            If (cnt2 > 0) Then

                                For i2 = 0 To cnt2 - 1

                                    sPartName = dsParts.Tables(0).Rows(i2).Item("Part Name")

                                    nPartId = dsParts.Tables(0).Rows(i2).Item("Part ID")

                                    SheetFileName = dsParts.Tables(0).Rows(i2).Item("Sheet Filename")

                                    d_Area = dsParts.Tables(0).Rows(i2).Item("Area")

                                    sSheetTag = TAG_RESULTS_PART & "," & sPartName & "," & sPdbName & "," & SheetFileName & "," & d_Area

                                    CFG_RESULTS_PART = tl.AppendNode(New Object() {sPartName}, CFG_RESULTS_SHEET, sSheetTag)

                                    'MessageBox.Show("The Part area is < " + Str(sGary) + " > ")

                                Next

                                dsParts.Dispose()

                                If dsParts IsNot Nothing Then dsParts = Nothing

                                If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                                If connection IsNot Nothing Then connection = Nothing

                            End If

                        End If

                    Next

                    dsSheets.Dispose()

                    If dsSheets IsNot Nothing Then dsSheets = Nothing

                    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                    If connection IsNot Nothing Then connection = Nothing

                End If

            End If


            frmMain.ShowCombos(sFullSheetName)

        Catch ex As Exception

            MessageBox.Show("LoadResultsIntoTree: " & ex.Message)

        End Try
    End Sub

    Private Sub ShowSheetResults(sSheetTag As String)
        Dim sPieces() As String
        Dim sSheetFilename As String
        Dim dsSheets As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim sql As String
        Dim the_PDB_Node_Name As String

        Try

            vgResults.Rows.Clear()
            vgResults.SuspendLayout()

            sPieces = Split(TV_NestConfig.FocusedNode.ParentNode.ParentNode.Tag, ",")

            the_PDB_Node_Name = sPieces(1)

            sPieces = Split(TV_NestConfig.FocusedNode.Tag, ",")

            sSheetFilename = sPieces(1)

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & the_PDB_Node_Name & "';"

            sql = "Select * From [Sheet Result] Where ([Sheet Result].[Sheet Filename] = '" & sSheetFilename & "' )"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(dsSheets)

            oledbAdapter.Dispose()

            Dim SheetCategoryRow As New CategoryRow

            SheetCategoryRow = CreateCategoryRow(vgResults, "Sheet Fileename", "Sheet Infromation", sSheetFilename, sSheetFilename, "Sheet Filename", DB_HEADER, 0, sEmpty, 0)

            For Each Row As DataRow In dsSheets.Tables(0).Rows

                For Each Coll As DataColumn In dsSheets.Tables(0).Columns

                    Dim sValue As String = Row(Coll.ColumnName).ToString()

                    'We do not want to show the interal fields such as ID  so skip over tem
                    If (Coll.Ordinal > 3) Then

                        If (IsNumeric(sValue)) Then

                            sValue = String.Format("{0:N3}", Double.Parse(sValue))

                        End If

                        If (Coll.ColumnName <> "Image") Then
                            SheetCategoryRow.ChildRows.Add(CreateStringEditorRow(vgResults,
                                                                       Coll.ColumnName,
                                                                      Coll.ColumnName,
                                                                      sValue,
                                                                      sValue,
                                                                       Coll.ColumnName,
                                                                      DB_STR,
                                                                      0,
                                                                      sEmpty, 0))
                        End If
                    End If

                Next

            Next

            vgResults.BestFit()

            vgResults.ResumeLayout()

            connection.Dispose()

            connection.Close()

            dsSheets.Dispose()

            If dsSheets IsNot Nothing Then dsSheets = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

        Catch ex As Exception

            MessageBox.Show("ShowSheetResults: " & ex.Message)

        End Try

    End Sub
    Private Function GetColumnNamesInTable(ByVal connectionString As String, ByVal tableName As String) As List(Of String)
        Dim connection As OleDbConnection = New OleDbConnection(connectionString)
        Dim restrictions As String() = New String() {Nothing, Nothing, tableName, Nothing}

        connection.Open()

        Dim dataTable As DataTable = connection.GetSchema("Columns", restrictions)

        connection.Close()

        Dim returnList As List(Of String) = New List(Of String)

        For Each dataRow As DataRow In dataTable.Rows

            returnList.Add(dataRow("Column_Name"))

        Next

        Return returnList

    End Function

    Private Sub LoadPartsFromDB()
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim dsParts As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim cnt As Integer
        Dim i As Integer
        Dim sql As String
        Dim sPartName As String
        Dim nID As Integer
        table = New DataTable("PartGrid")

        Try

            vgParts.Rows.Clear()

            currentCategoryRow = CreateCategoryRow(vgParts, "Part List", "Part List", "Part List", "Part List", "Part List", DB_HEADER, 0, sEmpty, 0)

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            sql = "Select * From Part"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(dsParts)

            oledbAdapter.Dispose()

            connection.Close()

            cnt = dsParts.Tables(0).Rows.Count

            If (cnt > 0) Then
                frmMain.NestingImageCollection.Clear()

                For i = 0 To cnt - 1

                    sPartName = dsParts.Tables(0).Rows(i).Item("Filename")

                    nID = dsParts.Tables(0).Rows(i).Item("ID")

                    currentCategoryRow = CreateCategoryRow(vgParts, "Filename", PathFile(sPartName), sPartName, sPartName, "Filename", DB_HEADER, nID, sEmpty, 0)
                    Dim col As DataColumn = table.Columns.Add(currentCategoryRow.Tag)
                    currentCategoryRow.Properties.ToolTip = sPartName
                Next

                vgParts.FocusedRow = vgParts.Rows(1)
                vgParts.SelectRow(vgParts.FocusedRow)
                vgParts.FocusedRow.Appearance.BackColor = Color.Red
                'vgParts.FocusedRow.Tag = sPartName
                vgPartVars.Enabled = True

            End If

            dsParts.Dispose()

            connection.Close()

            If dsParts IsNot Nothing Then dsParts = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

        Catch ex As Exception

            MessageBox.Show("LoadPartsFromDB: " & ex.Message)

        End Try

    End Sub

    Private Sub FillPartGridWithValues(nPartID As Integer)
        Dim dsPartVars As DataSet
        Dim dsParts As DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim cnt As Integer
        Dim sFieldName As String
        Dim nFieldType As Integer
        Dim sql As String
        Dim m_Temp_Value
        Dim m_Options As String
        Dim sPartExtents As String
        Dim d_PartArea As Double



        Try
            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            sql = "Select * From Part Where (Part.ID = " & nPartID & " )"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)


            dsParts = New DataSet

            oledbAdapter.Fill(dsParts)

            oledbAdapter.Dispose()

            cnt = dsParts.Tables(0).Rows.Count

            If (cnt > 0) Then

                For Each row As CategoryRow In vgPartVars.Rows

                    For Each childRow As EditorRow In row.ChildRows

                        'We stop at 10 because that defines satndard fields.
                        'anything over 10 will be a user var and must be filled by the partvars table


                        nFieldType = GetValueFromDelimitedData(childRow.Tag, "VarDataType", 0)
                        m_Options = GetValueFromDelimitedData(childRow.Tag, "Options", "")

                        If (childRow.Index < 10) Then

                            sFieldName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", 0)


                            If (sFieldName = "Part_Extents") Then

                                sPartExtents = CalculateDimensionsJustXY()
                                m_Temp_Value = sPartExtents

                                d_PartArea = GetPartArea()

                                'MessageBox.Show("The Part area is < " + Str(d_PartArea) + " > ")

                            Else

                                m_Temp_Value = dsParts.Tables(0).Rows(0).Item(sFieldName)

                            End If



                        Else

                            sFieldName = GetValueFromDelimitedData(childRow.Tag, "DialogDisplay", 0)

                            'sql = "Select * From [Part Variables] Where ([Part Variables].[Part ID] = " & nPartID & " )"
                            sql = "Select * From [Part Variables] Where ([Part Variables].[Part ID] = " & nPartID & " )" &
                                   "And ([Part Variables].Description = '" & sFieldName & " )"
                            sql = "Select [Part Variables].* From [Part Variables] Where ([Part Variables].[Part ID] = " & nPartID & " ) And ([Part Variables].Description = '" & sFieldName & "' )"

                            oledbAdapter = New OleDbDataAdapter(sql, connection)

                            dsPartVars = New DataSet

                            oledbAdapter.Fill(dsPartVars)

                            oledbAdapter.Dispose()

                            If (dsPartVars.Tables(0).Rows.Count > 0) Then

                                m_Temp_Value = dsPartVars.Tables(0).Rows(0).Item("Value")
                                dsPartVars.Clear()
                                dsPartVars.Dispose()

                                If dsPartVars IsNot Nothing Then dsPartVars = Nothing

                            Else
                                dsPartVars.Clear()
                                dsPartVars.Dispose()

                                If dsPartVars IsNot Nothing Then dsPartVars = Nothing

                                GoTo EarlyExit

                            End If


                        End If

                        Select Case nFieldType

                            Case DB_DCOMBO
                                If (childRow.Index < 10) Then
                                    m_Temp_Value = GetOptionText(";", m_Options, m_Temp_Value)
                                End If


                            Case DB_BOOL

                                If (m_Temp_Value = 0) Then

                                    m_Temp_Value = False

                                Else

                                    m_Temp_Value = True

                                End If

                        End Select

                        childRow.Properties.Value = m_Temp_Value



                        m_Temp_Value = sEmpty

                    Next

                    connection.Close()

                    If dsParts IsNot Nothing Then dsParts = Nothing
                    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                    If connection IsNot Nothing Then connection = Nothing
                Next

            End If

EarlyExit:

            If (connection IsNot Nothing) Then
                connection.Close()
            End If


            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

        Catch ex As Exception

            MessageBox.Show("FillPartGridWithValues: " & ex.Message)

        End Try

    End Sub

    Private Sub vgPartVars_CellValueChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgPartVars.CellValueChanged
        Dim sql As String
        Dim ds As New DataSet

        Dim connetionString As String
        Dim m_cnSettings As New OleDb.OleDbConnection
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim m_dtSettings As New DataTable

        Dim s_DatVarName As String
        Dim n_Part_ID As Long
        Dim sRow_Value As String
        Dim sEdit_Value As String
        Dim nDataType As String

        sEdit_Value = Nothing

        Try
            sRow_Value = e.Row.Properties.Value

            nDataType = CInt(GetValueFromDelimitedData(e.Row.Tag, "VarDataType", sEmpty))

            n_Part_ID = GetValueFromDelimitedData(vgParts.FocusedRow.Tag, "nRecordID", 0)

            s_DatVarName = GetValueFromDelimitedData(e.Row.Tag, "DatVarName", 0)

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            m_cnSettings = New OleDbConnection(connetionString)

            m_cnSettings.Open()

            If (e.Row.VisibleIndex < 10) Then ' we edit the standard part variables

                Select Case UCase(s_DatVarName)

                    Case "PART QUANTITY", "ROTATION NUM", "ROTATION START", "PART PRIORITY", "COST PER PART", "PRENEST", "FILL PART"

                        sEdit_Value = sRow_Value

                    Case "REPRESENTATION"

                        Select Case UCase(sRow_Value)

                            Case "ACTUAL PART"
                                sEdit_Value = 0

                            Case "BOUNDING BOX"
                                sEdit_Value = 1

                            Case "RUBBERBAND BOX"
                                sEdit_Value = 2
                        End Select

                    Case "MIRRORPART"

                        Select Case UCase(sRow_Value)

                            Case "NONE"
                                sEdit_Value = 0

                            Case "ALLOW"
                                sEdit_Value = 1

                            Case "PRE MIRROR"
                                sEdit_Value = 2

                        End Select



                End Select

                sql = "UPDATE [Part] set [Part].[" & s_DatVarName & "] = " & sEdit_Value & " Where ([Part].[ID] =" & n_Part_ID & " )"

                m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

                m_daSettings.InsertCommand.ExecuteNonQuery()
            Else

                sql = "UPDATE [Part Variables] set [Part Variables].[Value] = '" & sRow_Value &
                    "'  Where ([Part Variables].[Part ID] = " & n_Part_ID & " ) And ([Part Variables].[Description] = '" & e.Row.Properties.Caption & "' )"

                m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

                m_daSettings.InsertCommand.ExecuteNonQuery()


            End If

            m_cnSettings.Dispose()
            m_cnSettings.Close()
            If m_cnSettings IsNot Nothing Then m_cnSettings = Nothing

        Catch ex As Exception

            MessageBox.Show("vgPartVars_CellValueChanged: " & ex.Message)

        End Try

        'm_cnSettings.Close()

    End Sub

    'Private Sub vgPartVars_ShowingEditor(sender As Object, e As ComponentModel.CancelEventArgs) Handles vgPartVars.ShowingEditor
    '    BeginInvoke(New FocusTextHandler(AddressOf FocusText), sender)
    'End Sub

    Private Sub vgPartVars_ShownEditor(sender As Object, e As EventArgs) Handles vgPartVars.ShownEditor
        If (vgPartVars.FocusedRow.Properties.Caption = "Part Extents") Then

            vgPartVars.FocusedRow.Properties.ReadOnly = True

        Else

            BeginInvoke(New FocusTextHandler(AddressOf FocusText), sender)

        End If

    End Sub
    Private Function CheckIfBuildEnabled() As Boolean
        Dim dsRecords As New DataSet
        Dim dsSetup As New DataSet
        Dim dsToolSetup As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim cnt As Integer
        Dim sql As String
        Dim bHasMaterial As Boolean
        Dim bHasMachine As Boolean
        Dim bHasToolSetup As Boolean
        Dim bHasParts As Boolean
        Dim n_MachineID As Integer
        Dim n_Toolsetup As Integer

        bHasMachine = False
        bHasMaterial = False
        bHasToolSetup = False
        bHasParts = False

        CheckIfBuildEnabled = False

        Try

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            sql = "Select Setup.* From Setup Where (Setup.DialogDisplay = 'Machine' )"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(dsSetup)

            oledbAdapter.Dispose()

            cnt = dsSetup.Tables(0).Rows.Count


            If (cnt > 0) Then
                n_MachineID = dsSetup.Tables(0).Rows(0).Item("CValue")

                If (n_MachineID > 0) Then

                    bHasMachine = True

                End If
            End If

            dsSetup.Dispose()

            connection.Close()

            If dsSetup IsNot Nothing Then dsSetup = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing


            sql = "Select * From [Material Inventory]"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(dsRecords)

            oledbAdapter.Dispose()

            cnt = dsRecords.Tables(0).Rows.Count

            If (cnt > 0) Then

                bHasMaterial = True

            End If

            dsRecords.Dispose()

            connection.Close()


            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

            sql = "Select * From [Part]"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(dsRecords)

            oledbAdapter.Dispose()

            cnt = dsRecords.Tables(0).Rows.Count

            If (cnt > 0) Then

                bHasParts = True

            End If

            dsRecords.Dispose()

            connection.Close()

            If dsRecords IsNot Nothing Then dsRecords = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

            sql = "Select Setup.* From Setup Where (Setup.DialogDisplay = 'Tool Setup' )"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(dsToolSetup)

            oledbAdapter.Dispose()

            cnt = dsToolSetup.Tables(0).Rows.Count


            If (cnt > 0) Then

                n_Toolsetup = dsToolSetup.Tables(0).Rows(0).Item("CValue")

                If (n_Toolsetup > 0) Then

                    bHasToolSetup = True

                End If
            End If

            dsToolSetup.Dispose()

            connection.Close()

            If dsToolSetup IsNot Nothing Then dsToolSetup = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

            If (bHasMachine = True And bHasToolSetup = True And bHasMaterial = True And bHasParts = True) Then

                CheckIfBuildEnabled = True

            End If

        Catch ex As Exception

            MessageBox.Show("CheckIfBuildEnabled: " & ex.Message)

        End Try
    End Function

    Private Sub ShowPartAndFillGrid()
        Dim sMm2Filename As String
        Dim nPartID As Integer

        Try

            If (TypeOf vgParts.FocusedRow Is CategoryRow) Then

                sMm2Filename = GetValueFromDelimitedData(vgParts.FocusedRow.Tag, "VBValue", "")

                If (sMm2Filename = "" Or vgParts.FocusedRow.Index = 0) Then
                    GoTo Skip
                End If




                If (File.Exists(sMm2Filename) = False) Then
                    vgParts.FocusedRow.Appearance.BackColor = Color.Red

                    'MessageBox.Show("Part < " & sMm2Filename & " > Does NOt Exist", "Part Does Not Exist!", MessageBoxButtons.OK, MessageBoxIcon.Error)

                End If


                If (Not bFromAddButton) Then

                    sMm2Filename = ImportFile(sMm2Filename, sMm2Filename)

                    bFromAddButton = False

                End If


                nPartID = GetValueFromDelimitedData(vgParts.FocusedRow.Tag, "nRecordID", 0)

                If (nPartID > 0) Then

                    FillPartGridWithValues(nPartID)

                    Me.cmdDelete.Enabled = True

                Else

                    Me.cmdDelete.Enabled = False

                End If



                vgPartVars.Enabled = cmdDelete.Enabled

            End If



Skip:

        Catch ex As Exception

            MessageBox.Show("ShowPartAndFillGrid: " & ex.Message)

        End Try
    End Sub

    Private Sub DeletePart()
        Dim nPartID As Integer
        Dim connetionString As String
        Dim sql As String
        Dim m_dtPart As New DataTable
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim nCurrentRowIndex As Integer

        On Error Resume Next

        'Try

        For Each row In vgParts.GetSelectedRows
            ' vgParts.Rows.RemoveAt(row.Index)


            nPartID = GetValueFromDelimitedData(vgParts.FocusedRow.ToString, "nRecordID", 0)

            nPartID = GetValueFromDelimitedData(vgParts.FocusedRow.Tag, "nRecordID", 0)

            If (nPartID > 0) Then

                connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

                m_cnPart = New OleDbConnection(connetionString)

                m_cnPart.Open()

                sql = "DELETE Part.* From Part Where (Part.ID = " & nPartID & " )"

                m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

                m_daPart.InsertCommand.ExecuteNonQuery()

                m_cnPart.Dispose()
                m_cnPart.Close()

                nCurrentRowIndex = vgParts.FocusedRow.Index

                'nCurrentRowIndex = nCurrentRow.Index

                'vgParts.Rows.Remove(row.Index)
                vgParts.Rows.RemoveAt(row.Index)
                vgParts.Refresh()
            End If

        Next
        m_daPart.Dispose()

        If m_daPart IsNot Nothing Then m_daPart = Nothing
        If m_cnPart IsNot Nothing Then m_cnPart = Nothing

        If ((nCurrentRowIndex - 1) <= vgParts.Rows.Count - 1) Then

            'vgParts.FocusedRow = vgParts.Rows(vgParts.Rows.Count) '(nCurrentRowIndex - 1)
            vgParts.FocusedRow = vgParts.Rows(nCurrentRowIndex - 1)
            vgParts.SelectRow(vgParts.Rows.Count - 1)

            ShowPartAndFillGrid()
            vgParts.SelectRow(vgParts.Rows.Count - 1)

        End If

        If (vgParts.Rows.Count = 1) Then

            ModelReset()
            PathCurrent_Set("")
            frmMain.Text = App_Title & " - "

        End If



        'Catch ex As Exception
        '    MessageBox.Show("DeletePart: " & ex.Message)
        'End Try

    End Sub


    Private Sub cmdDelete_Click(sender As Object, e As EventArgs) Handles cmdDelete.Click
        DeletePart()
        vgPartVars.Enabled = False

        If (vgParts.Rows.Count > 0) Then
            vgParts.FocusedRow = vgParts.Rows.LastVisible
            vgParts.SelectRow(vgParts.FocusedRow)

        End If
    End Sub

    Private Sub LoadMaterialFromDB()
        Dim cnt As Integer
        Dim connetionString As String
        Dim sql As String
        Dim m_dtMaterial As New DataTable
        Dim m_dsMaterial As New DataSet
        Dim m_cnMaterial As New OleDb.OleDbConnection
        Dim m_daPMaterial As New OleDb.OleDbDataAdapter
        Dim dLength As Double
        Dim dWidth As Double
        Dim dThickness As Double
        Dim iQuantity As Integer
        Dim dWeight As Double
        Dim dCostPerUnit As Double
        Dim nID As Long
        Dim sDescription As String
        Dim Rem_ID As Long

        Try
            'If (IsMaterialLoaded And bFromMrp = False) Then
            'Exit Sub
            'End If
            'vgMaterial.BeginInit()

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            m_cnMaterial = New OleDbConnection(connetionString)

            m_cnMaterial.Open()

            sql = "Select * From [Material Inventory]"

            m_daPMaterial = New OleDbDataAdapter(sql, m_cnMaterial)

            m_daPMaterial.Fill(m_dsMaterial)

            m_daPMaterial.Dispose()

            cnt = m_dsMaterial.Tables(0).Rows.Count

            If (cnt > 0) Then

                For i = 0 To cnt - 1


                    nID = m_dsMaterial.Tables(0).Rows(i).Item("ID")

                    sDescription = m_dsMaterial.Tables(0).Rows(i).Item("Description")

                    Dim sActualMaterial As New EditorRow()

                    sActualMaterial.Properties.Caption = sDescription
                    sActualMaterial.Tag = CHR1 & "IsRem" & CHR2 & False &
                                            CHR1 & "MatID" & CHR2 & 0 &
                                            CHR1 & "PMatID" & CHR2 & nID

                    ' adding the Material row to the Material Type row's child collection 
                    vgMaterial.Rows(0).ChildRows.Add(sActualMaterial)

                    er_SelectedMaterialRow = vgMaterial.Rows(0).ChildRows(0)
                    dLength = m_dsMaterial.Tables(0).Rows(i).Item("Length")
                    dWidth = m_dsMaterial.Tables(0).Rows(i).Item("Width")
                    dThickness = m_dsMaterial.Tables(0).Rows(i).Item("Thickness")
                    iQuantity = m_dsMaterial.Tables(0).Rows(i).Item("Quantity")
                    dWeight = m_dsMaterial.Tables(0).Rows(i).Item("Weight")
                    dCostPerUnit = m_dsMaterial.Tables(0).Rows(i).Item("Cost Per Unit")

                Next

            End If

            m_dsMaterial.Dispose()

            sql = "Select * From [Remnant Inventory]"

            m_daPMaterial = New OleDbDataAdapter(sql, m_cnMaterial)

            m_daPMaterial.Fill(m_dsMaterial)

            m_daPMaterial.Dispose()

            cnt = m_dsMaterial.Tables(0).Rows.Count

            If (cnt > 0) Then

                For i = 0 To cnt - 1
                    If (IsDBNull(m_dsMaterial.Tables(0).Rows(i).Item("FileName")) = False) Then

                        sDescription = PathFile(m_dsMaterial.Tables(0).Rows(i).Item("FileName"))

                        Rem_ID = m_dsMaterial.Tables(0).Rows(i).Item("Type Id")

                        nID = GetRemnantMaterialType(Rem_ID)

                        Dim sActualMaterial As New EditorRow()

                        sActualMaterial.Properties.Caption = sDescription
                        sActualMaterial.Tag = CHR1 & "IsRem" & CHR2 & True &
                                            CHR1 & "RemID" & CHR2 & Rem_ID &
                                            CHR1 & "MatID" & CHR2 & 0 &
                                            CHR1 & "PMatID" & CHR2 & nID

                        ' adding the Material row to the Material Type row's child collection 
                        vgMaterial.Rows(0).ChildRows.Add(sActualMaterial)
                    End If

                Next

            End If


            m_cnMaterial.Close()

            If m_dsMaterial IsNot Nothing Then m_dsMaterial = Nothing

            If m_daPMaterial IsNot Nothing Then m_daPMaterial = Nothing

            If m_cnMaterial IsNot Nothing Then m_cnMaterial = Nothing

            ' vgMaterial.EndInit()
            vgMaterial.BestFit()

            vgMaterial.SelectRow(0)

            IsMaterialLoaded = True
        Catch ex As Exception

            MessageBox.Show("LoadMaterialFromDB: " & ex.Message)

        End Try
    End Sub

    Private Function GetRemnantMaterialType(nRemID As Long) As Long
        Dim Sql2 As String
        Dim dtRemnantInv As DataTable = Nothing
        Dim rem_material As String

        Try
            GetRemnantMaterialType = -1

            Sql2 = "SELECT * FROM RemnantVar WHERE (RemnantID = " & nRemID & " )"

            dtRemnantInv = ConnectToCMDB(Sql2)

            For ni = 0 To dtRemnantInv.Rows.Count - 1

                Select Case UCase(dtRemnantInv.Rows(ni).Item("InternalName"))

                    Case "MATERIAL"
                        rem_material = dtRemnantInv.Rows(ni).Item("Value")
                        GetRemnantMaterialType = GetMaterialTypeIDFromDescription(rem_material)

                        Exit For

                End Select


            Next

        Catch ex As Exception

            MessageBox.Show("LoadMaterialFromDB: " & ex.Message)

        End Try

        DatabaseClose()

    End Function
    Public Sub LoadPDB(m_pdb_filename As String)
        Dim bReturn As Boolean
        Try

            If (FileExists(m_pdb_filename) = True) Then


                ArePartsLoaded = False

                modAdmin.ModelReset()

                vgResults.Rows.Clear()
                vgMaterial.Rows.Clear()
                vgParts.Rows.Clear()

                m_isVirginDB = False

                Select Case UCase(PathExt(m_pdb_filename))
                    Case "PDB"
                        If (DoesFieldExist(m_pdb_filename, "Part", "Part_Extents") = False) Then

                            bReturn = AddFieldToTable(m_pdb_filename, "Part", "Part_Extents")

                        End If

                        modNesting.SetupInfoGet(m_pdb_filename)

                        TV_NestConfig.SuspendLayout()

                        CreateRootNodes(TV_NestConfig, m_pdb_filename)

                        PopulateDefaultSettings()
                        GetSetupValuesFromPDB()


                        LoadResultsIntoTree(m_pdb_filename)

                        TV_NestConfig.ResumeLayout()

                        frmMain.ShowCombos("")

                        bFromAddButton = False

                        DeleteRemantInventory()

                        bFromMrp = False

                    Case "MRP"

                        CreateMRPRootNodes(TV_NestConfig, m_pdb_filename)
                        bCameFromStartUp = False


                        TV_NestConfig.Visible = True
                        TV_NestConfig.ExpandToLevel(2)
                        bFromMrp = True

                        If (IS_CCS_FAB = True) Then

                            Me.fcbXlsName.Text = PathXLSGet()

                        End If

                End Select

                modView.ViewActivate(VIEW_WIRE)

            End If
        Catch ex As Exception

            MessageBox.Show("LoadPDB: " & ex.Message)
            'LoadPDB()
        End Try

    End Sub
    Sub DeleteRemantInventory()
        Dim connetionString As String
        Dim sql As String
        Dim m_cnMaterial As New OleDb.OleDbConnection
        Dim cmd As OleDbCommand
        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

        m_cnMaterial = New OleDbConnection(connetionString)

        m_cnMaterial.Open()

        sql = "DELETE * FROM [Remnant Inventory]"

        cmd = New OleDbCommand(sql, m_cnMaterial)

        cmd.ExecuteNonQuery()

        cmd.Dispose()

        m_cnMaterial.Dispose()
        m_cnMaterial.Close()

        If m_cnMaterial IsNot Nothing Then m_cnMaterial = Nothing
        If cmd IsNot Nothing Then cmd = Nothing

    End Sub
    Protected Overrides Sub Finalize()
        MyBase.Finalize()
    End Sub



    Public Shadows Function IsActive() As Boolean
        IsActive = m_is_active
    End Function

    Public Sub VirginStatusSet(is_virgin As Boolean)
        m_isVirginDB = is_virgin
    End Sub

    Private Sub frmNesting_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        m_is_active = False
        bIsNestLoaded = False
        Me.Dispose()
    End Sub

    Private Sub frmNestin_KeyUp(sender As Object, e As KeyEventArgs) Handles Me.KeyUp
        If (e.KeyCode = System.Windows.Forms.Keys.Escape) Then
            'DefaultToolTipController1.DefaultController.HideHint()
            ModeCancelCurrent()
        End If
    End Sub

    Private Sub vgMaterial_DoubleClick(sender As Object, e As EventArgs) Handles vgMaterial.DoubleClick
        Dim sMatInvenotryCap
        Dim sMatRemnantCap
        Dim dtMaterialType As DataTable = Nothing
        Dim sql As String
        Dim dLength As Double
        Dim dWidth As Double
        Dim dThickness As Double
        Dim iQuantity As Integer
        Dim dWeight As Double
        Dim dCostPerUnit As Double
        Dim nTypeID As Long
        Dim sDescription As String
        Dim nMatID As Long
        Dim sCMDB_MatID As Long
        Dim nFocusIndex As Integer
        Dim dtRemnantInv As DataTable = Nothing
        Dim Rem_Name As String = Nothing
        Dim rem_length As Double
        Dim rem_width As Double
        Dim rem_thickness As Double
        Dim rem_qty As Long = 1
        Dim rem_area As Double
        Dim Rem_Date As String = Nothing
        Dim rem_material As String
        Dim rem_weight As Double = 0
        Dim rem_cost As Double = 0

        Try


            If (vgMaterial.FocusedRow.Level = 2) Then

                sMatInvenotryCap = vgMaterial.FocusedRow.ParentRow.ParentRow.Properties.Caption

                sMatRemnantCap = vgMaterial.FocusedRow.ParentRow.ParentRow.Properties.Caption

                If (sMatInvenotryCap = "Material Inventory" Or sMatRemnantCap = "Remnant Inventory") Then
                    nTypeID = GetMaterialTypeIDFromDescription(vgMaterial.FocusedRow.Properties.Caption)

                    sDescription = vgMaterial.FocusedRow.Properties.Caption

                    Dim sActualMaterial As New EditorRow

                    sActualMaterial.Properties.Caption = vgMaterial.FocusedRow.Properties.Caption

                    sCMDB_MatID = CLng(GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "MatID", 0))

                    sActualMaterial.Tag = vgMaterial.FocusedRow.Tag

                    ' adding the Material row to the Material Type row's child collection 
                    nFocusIndex = vgMaterial.Rows(0).ChildRows.Add(sActualMaterial)

                    vgMatVars.Rows.Clear()

                    Dim rowParameters As New CategoryRow("Parameters")

                    vgMatVars.Rows.Add(rowParameters)

                    If (sMatInvenotryCap = "Material Inventory") Then

                        sql = "Select * From [Material Inventory] Where (ID = " & sCMDB_MatID & " )"

                        dtMaterialType = ConnectToCMDB(sql)

                        For i = 0 To dtMaterialType.Rows.Count - 1

                            dLength = dtMaterialType.Rows(i).Item("Length")
                            dWidth = dtMaterialType.Rows(i).Item("Width")
                            iQuantity = dtMaterialType.Rows(i).Item("Quantity")
                            dWeight = dtMaterialType.Rows(i).Item("Weight")
                            dCostPerUnit = dtMaterialType.Rows(i).Item("Cost Per Unit")
                            dThickness = dtMaterialType.Rows(i).Item("Thickness")


                            nMatID = AddMaterialToMaterialInventorys(sDescription, nTypeID, dLength, dWidth, dThickness, dWeight, iQuantity, dCostPerUnit)

                            lReturn = SetParameterValues(rowParameters, dLength, dWidth, dThickness, iQuantity, dWeight, dCostPerUnit, sDescription, nMatID)

                            Dim nLastMaterialInProcess As EditorRow = vgMaterial.Rows(0).ChildRows.LastVisible

                            vgMaterial.FocusedRow = nLastMaterialInProcess

                            vgMaterial.FocusedRow.Tag = sActualMaterial.Tag & CHR1 & "PMatID" & CHR2 & nMatID

                        Next

                    ElseIf (sMatRemnantCap = "Remnant Inventory") Then

                        sCMDB_MatID = CLng(GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "RemID", 0))

                        sql = "SELECT * FROM RemnantVar WHERE (RemnantID = " & sCMDB_MatID & " )"

                        dtRemnantInv = ConnectToCMDB(sql)

                        For ni = 0 To dtRemnantInv.Rows.Count - 1

                            Select Case UCase(dtRemnantInv.Rows(ni).Item("InternalName"))
                                Case "NAME"
                                    Rem_Name = dtRemnantInv.Rows(ni).Item("Value")

                                Case "LENGTH"
                                    rem_length = dtRemnantInv.Rows(ni).Item("Value")

                                Case "WIDTH"
                                    rem_width = dtRemnantInv.Rows(ni).Item("Value")

                                Case "THICKNESS"
                                    rem_thickness = dtRemnantInv.Rows(ni).Item("Value")

                                Case "AREA"
                                    rem_area = dtRemnantInv.Rows(ni).Item("Value")

                                Case "CREATION_DATE"
                                    Rem_Date = dtRemnantInv.Rows(ni).Item("Value")

                                Case "MATERIAL"
                                    rem_material = dtRemnantInv.Rows(ni).Item("Value")


                            End Select


                        Next

                        nMatID = AddRemnantToMaterialInventorys(Rem_Name, sCMDB_MatID, rem_length, rem_width, rem_thickness, rem_area, rem_weight, rem_qty, rem_cost, Rem_Date)

                        lReturn = SetParameterValues(rowParameters, dLength, dWidth, dThickness, iQuantity, dWeight, dCostPerUnit, sDescription, nMatID)
                        'sActualMaterial.Tag = nMatID

                        Dim nmaybe As EditorRow = vgMaterial.Rows(0).ChildRows.LastVisible

                        vgMaterial.FocusedRow = nmaybe

                        DatabaseClose()

                    End If

                    vgMaterial.BestFit()

                    cmdBuild.Text = "Build Current"
                    cmdBuild.Enabled = CheckIfBuildEnabled()

                    DatabaseClose()

                End If

            End If

        Catch ex As Exception

            MessageBox.Show("vgMaterial_DoubleClick: " & ex.Message)

        End Try

    End Sub


    Private Sub vgMaterial_KeyUp(sender As Object, e As KeyEventArgs) Handles vgMaterial.KeyUp
        Dim nID As Long
        Dim rTheCurrentRow As Long
        Dim sql As String
        Dim connection As OleDbConnection
        Dim connetionString As String
        Dim cmd As OleDbCommand

        Dim row As DevExpress.XtraVerticalGrid.Rows.BaseRow = vgMaterial.FocusedRow


        If (e.KeyCode = Windows.Forms.Keys.Delete) Then

            'RemoveSingleMaterialFromPDB()

            rTheCurrentRow = vgMaterial.FocusedRow.Index

            If (vgMaterial.FocusedRow.ParentRow.Index = 0) Then 'We are under the Process order node

                If (vgMaterial.Rows(0).HasChildren = True) Then

                    nID = GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "nRecordID", 0)

                    '****************************************************************************************************
                    nID = GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "PMatID", 0)

                    connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

                    connection = New OleDbConnection(connetionString)

                    connection.Open()

                    sql = "DELETE * From [Material Inventory] Where ([Material Inventory].ID =" & nID & " )"

                    cmd = New OleDbCommand(sql, connection)

                    cmd.ExecuteNonQuery()

                    connection.Dispose()
                    connection.Close()

                    '****************************************************************************************************

                    row.ParentRow.ChildRows.Remove(row)

                    cmd.Dispose()

                    If cmd IsNot Nothing Then cmd = Nothing
                    If connection IsNot Nothing Then connection = Nothing

                End If

            End If

        End If

    End Sub


    Private Sub vgMaterial_MouseClick(sender As Object, e As MouseEventArgs) Handles vgMaterial.MouseClick

        If (IsNothing(vgMaterial.FocusedRow.Tag)) Then
            GoTo DoNext
        End If

        If (e.Button = Windows.Forms.MouseButtons.Right And vgMaterial.FocusedRow.ParentRow.Index = 0) Then 'We are under the Process order node) Then

            If (IsNothing(vgMaterial.FocusedRow.ParentRow) = False) Then


                PopUp_TV_Config.ItemLinks.Clear()

                For Each item As BarItem In CFG_Man_BarMan.Items

                    If item.Category.Name = "Material" Then

                        PopUp_TV_Config.AddItem(item)

                    End If
                Next

                PopUp_TV_Config.ShowPopup(Control.MousePosition)

            End If
        End If

        ''If vgMaterial.FocusedRow.ParentRow.Index = 0 Then
        'vgMatVars.Enabled = True
        'End If
DoNext:
    End Sub

    Private Sub bbRemoveMat_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbRemoveMat.ItemClick

        RemoveSingleMaterialFromPDB()

    End Sub
    Private Sub RemoveSingleMaterialFromPDB()

        Dim nID As Long
        Dim rTheCurrentRow As Long
        Dim sql As String
        Dim connection As OleDbConnection
        Dim connetionString As String
        Dim cmd As OleDbCommand

        Dim row As DevExpress.XtraVerticalGrid.Rows.BaseRow = vgMaterial.FocusedRow

        rTheCurrentRow = vgMaterial.FocusedRow.Index

        If (vgMaterial.FocusedRow.ParentRow.Index = 0) Then 'We are under the Process order node

            nID = GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "PMatID", 0)

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            sql = "DELETE * From [Material Inventory] Where ([Material Inventory].ID =" & nID & " )"

            cmd = New OleDbCommand(sql, connection)

            cmd.ExecuteNonQuery()

            connection.Close()
            connection.Dispose()



            If connection IsNot Nothing Then connection = Nothing
            If cmd IsNot Nothing Then cmd = Nothing

            row.ParentRow.ChildRows.Remove(row)

        End If

    End Sub

    Private Function SetParameterValues(rowParameters As CategoryRow,
                                        dLength As Double,
                                        dWidth As Double,
                                        dThickness As Double,
                                        iQuantity As Integer,
                                        dWeight As Double,
                                        dCostPerUnit As Double,
                                        sDescription As String,
                                        nMatID As Integer) As Long

        Dim editorRow As New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Length"
        editorRow.Properties.Value = dLength
        editorRow.Tag = EncodeTag("Length", "Length", dLength, dLength, "Length", DB_INT, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        Dim reCalcEdit As New RepositoryItemCalcEdit()

        vgMatVars.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

        editorRow = New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Width"
        editorRow.Properties.Value = dWidth
        editorRow.Tag = EncodeTag("Width", "Width", dWidth, dWidth, "Width", DB_INT, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        reCalcEdit = New RepositoryItemCalcEdit()

        vgMatVars.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

        editorRow = New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Thickness"
        editorRow.Properties.Value = dThickness
        editorRow.Tag = EncodeTag("Thickness", "Thickness", dLength, dLength, "Thickness", DB_INT, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        reCalcEdit = New RepositoryItemCalcEdit()

        vgMatVars.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

        editorRow = New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Quantity"
        editorRow.Properties.Value = iQuantity
        editorRow.Tag = EncodeTag("Quantity", "Quantity", iQuantity, iQuantity, "Quantity", DB_INT, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        reCalcEdit = New RepositoryItemCalcEdit()

        vgMatVars.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

    End Function
    Private Function SetRemParameterValues(rowParameters As CategoryRow,
                                     dLength As Double,
                                     dWidth As Double,
                                     dThickness As Double,
                                     iQuantity As Integer,
                                     sFilename As String,
                                     sCreationDate As String,
                                     dWeight As Double,
                                     dCostPerUnit As Double,
                                     nTypeId As Integer,
                                     sDescription As String,
                                     nMatID As Integer) As Long

        Dim editorRow As New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Length"
        editorRow.Properties.Value = dLength
        editorRow.Tag = EncodeTag("Length", "Length", dLength, dLength, "Length", DB_INT, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        Dim reCalcEdit As New RepositoryItemCalcEdit()

        vgMatVars.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

        editorRow = New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Width"
        editorRow.Properties.Value = dWidth
        editorRow.Tag = EncodeTag("Width", "Width", dWidth, dWidth, "Width", DB_INT, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        reCalcEdit = New RepositoryItemCalcEdit()

        vgMatVars.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

        editorRow = New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Thickness"
        editorRow.Properties.Value = dThickness
        editorRow.Tag = EncodeTag("Thickness", "Thickness", dLength, dLength, "Thickness", DB_INT, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        reCalcEdit = New RepositoryItemCalcEdit()

        vgMatVars.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

        editorRow = New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Quantity"
        editorRow.Properties.Value = 1
        editorRow.Tag = EncodeTag("Quantity", "Quantity", 1, 1, "Quantity", DB_INT, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        reCalcEdit = New RepositoryItemCalcEdit()

        vgMatVars.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

        editorRow = New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Filename"
        editorRow.Properties.Value = sFilename
        editorRow.Tag = EncodeTag("Filename", "Filename", sFilename, sFilename, "Filename", DB_STR, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

        editorRow = New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = "Creation Date"
        editorRow.Properties.Value = sCreationDate
        editorRow.Tag = EncodeTag("Creation Date", "Creation Date", sCreationDate, sCreationDate, "Creation Date", DB_STR, nMatID, sEmpty, 0)

        rowParameters.ChildRows.Add(editorRow)

    End Function

    Private Sub vgMaterial_MouseUp(sender As Object, e As MouseEventArgs) Handles vgMaterial.MouseUp
        Dim dtMaterialType As DataTable = Nothing
        Dim indx As Integer
        Dim sActualMaterial As New EditorRow()
        Dim bIsRem As Boolean
        Dim childRows As VGridRows

        If (e.Button = MouseButtons.Left) Then

            sActualMaterial.Properties.Caption = vgMaterial.FocusedRow.Properties.Caption

            sActualMaterial.Tag = vgMaterial.FocusedRow.Tag

            If (vgMaterial.FocusedRow.Level = 1) Then

                indx = vgMaterial.FocusedRow.Index


                If (indx >= 0 And vgMaterial.Rows(0).HasChildren = True) Then


                    bIsRem = CBool(GetValueFromDelimitedData(vgMaterial.FocusedRow.Tag, "IsRem", False))

                    If (bIsRem = True) Then

                        LoadRemnantParams()
                        childRows = vgMatVars.Rows(0).ChildRows

                        For Each childRow As EditorRow In childRows

                            childRow.Properties.ReadOnly = True

                        Next

                    Else

                        PopulateMaterialParameters(sActualMaterial)

                    End If

                End If

            ElseIf (vgMaterial.FocusedRow.Level = 2) Then

                If (vgMaterial.FocusedRow.Properties.FieldName = "Remnant") Then

                    LoadRemnantParams()

                Else

                    LoadMaterialParams()

                End If
            End If

Skip:
            childRows = vgMatVars.Rows(0).ChildRows

            For Each childRow As EditorRow In childRows

                childRow.Properties.ReadOnly = True

            Next
        End If

        If (e.Button = MouseButtons.Left And vgMaterial.FocusedRow.ParentRow.Name = "categoryProcessOrder") Then
            For Each childRow As EditorRow In childRows

                childRow.Properties.ReadOnly = False

            Next

        End If
    End Sub

    Private Sub cmdAddKit_Click(sender As Object, e As EventArgs) Handles cmdAddKit.Click
        Dim Pdb_FileOpen As New OpenFileDialog
        Dim currentCategoryRow As CategoryRow = Nothing

        Try

            Pdb_FileOpen.Filter = "PDB files (*.pdb)|*.pdb"
            Pdb_FileOpen.FilterIndex = 1
            'Pdb_FileOpen.RestoreDirectory = True
            Pdb_FileOpen.InitialDirectory = PathNestFolder()

            If Pdb_FileOpen.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                m_pdb_filename = Pdb_FileOpen.FileName

                LoadPDB(m_pdb_filename)

            End If

            Windows.Forms.Cursor.Current = Cursors.Default

        Catch ex As Exception

            MessageBox.Show("cmdAddKit_Click: " & ex.Message)

        End Try
    End Sub


    Public Function ThumbnailCallback() As Boolean
        Return False
    End Function

    Private Sub RemoveExistingResults()
        Dim sql As String = Nothing
        Dim connection As OleDbConnection
        Dim connetionString As String
        Dim oledbAdapter As OleDbDataAdapter
        Dim ds As New DataSet
        Dim cmd As OleDbCommand = New OleDbCommand


        Try
            If (DoesTableExist("SheetTooling", PathPDB()) = True) Then

                connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

                connection = New OleDbConnection(connetionString)

                connection.Open()

                oledbAdapter = New OleDbDataAdapter(sql, connection)

                cmd = New OleDbCommand(sql, connection)

                cmd.CommandText = "DROP TABLE [SheetTooling]"

                cmd.ExecuteNonQuery()

                cmd.Dispose()
                connection.Close()

                connection.Dispose()

                If connection IsNot Nothing Then connection = Nothing
                If cmd IsNot Nothing Then cmd = Nothing

            End If

        Catch ex As Exception

            MessageBox.Show("RemoveExistingResults: " & ex.Message)

        End Try


    End Sub

    Private Sub sbPrint_Click(sender As Object, e As EventArgs) Handles sbPrint.Click

        Try
            frmPrintNestReport.ShowDialog(frmMain)
        Catch ex As Exception
            MessageBox.Show("sbPrint_Click: " & ex.Message)
        End Try


    End Sub

    Private Sub sbExportCNC_Click(sender As Object, e As EventArgs) Handles sbExportCNC.Click

        g_bNestingAutoCode = True

#If CCS_FAB = False Then
        frmCodeGen.Init(CODE_OPTION.CODE_FILE)
#Else
       frmCodeGen.Init(CODE_OPTION.CCSFAB)
#End If


        frmCodeGen.ShowDialog(frmMain)

        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        'TODO: V21 Set up a loop to code all the sheets

        Windows.Forms.Cursor.Current = Cursors.Default


    End Sub


    Private Sub DeletePartVars(ByVal nPartID As Integer)
        Dim sql As String = Nothing
        Dim connection As OleDbConnection
        Dim connetionString As String
        Dim oledbAdapter As OleDbDataAdapter
        Dim ds As New DataSet
        Dim cmd As OleDbCommand = New OleDbCommand

        Try
            If (DoesTableExist("SheetTooling", PathPDB()) = True) Then

                connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

                connection = New OleDbConnection(connetionString)

                connection.Open()

                oledbAdapter = New OleDbDataAdapter(sql, connection)

                cmd = New OleDbCommand(sql, connection)

                cmd.CommandText = "DELETE [Part Variables].* From [Part Variables] Where ([Part Variables].[Part ID] =" & nPartID & ")"

                cmd.ExecuteNonQuery()

                cmd.Dispose()

                connection.Close()

                connection.Dispose()

                If connection IsNot Nothing Then connection = Nothing
                If cmd IsNot Nothing Then cmd = Nothing

            End If

        Catch ex As Exception

            MessageBox.Show("DeletePartVars: " & ex.Message)

        End Try
    End Sub


    Private Sub vgParts_MouseUp(sender As Object, e As MouseEventArgs) Handles vgParts.MouseUp
        If (bDraggingStarted = False) Then
            Dim hitInfo = vgParts.CalcHitInfo(New Point(e.X, e.Y))

            vgParts.FocusedRow = hitInfo.Row
        End If

    End Sub


    Public Sub CreateNewPDB()
        Dim Pdb_FileSave As New SaveFileDialog
        Dim currentCategoryRow As CategoryRow = Nothing

        TV_NestConfig.ClearNodes()

        m_pdb_filename = PathPDB()

        'ConditionalFileDelete(m_pdb_filename)

        ArePartsLoaded = False
        vgResults.Rows.Clear()
        vgMaterial.Rows.Clear()
        vgParts.Rows.Clear()


        PathPDB_Set(m_pdb_filename)

        m_isVirginDB = True

        Me.SuspendLayout()


        CreatePDB()

        CreateRootNodes(TV_NestConfig, m_pdb_filename)

        PopulateDefaultSettings()
        UpdateSetupTable()

        Me.ResumeLayout()

        cmdBuild.Enabled = False

        'End If

        If (m_pdb_filename <> "") Then
            lReturn = frmMain.AddToMru(m_pdb_filename, frmMain.MRU_ListAray)

        End If

        Windows.Forms.Cursor.Current = Cursors.Default
    End Sub

    Private Sub frmNesting_Load(sender As Object, e As EventArgs) Handles Me.Load

        IsMaterialLoaded = False

        If (IS_CCS_FAB_SILENT = True) Then

            If (InStr(1, UCase$(m_command), "~XLS") > 0) Then
                sbExportCNC.Enabled = False
            End If

            'cmdBuild_Click(sender, New System.EventArgs())

        End If

        m_is_nesting = True

        bIsNestLoaded = True
    End Sub

    Private Sub frmTemp_MouseWheel(sender As Object, e As MouseEventArgs) Handles Me.MouseWheel
        Dim pt As PortalPoint
        Dim cmd As String
        Dim factor As Double

        If ((My.Computer.Keyboard.CtrlKeyDown) And
            (My.Computer.Keyboard.ShiftKeyDown)) Then

            lReturn = IsModeActive(MODE_ZOOM) = True

            If (IsModeActive(MODE_ZOOM) = False) Then
                lReturn = ModeBegin(MODE_ZOOM)
            End If

            'Hack to get around conversion of Xpos,Ypos (which I don't know how to do).
            pt = modView.MousePosition()
            lReturn = PortalLocalToWorld(pt)

            'factor = IIf((Rotation > 0), VIEW_ZOOM_FACTOR, (1# / VIEW_ZOOM_FACTOR))
            factor = IIf((e.Delta > 0), (1.0# / VIEW_ZOOM_FACTOR), VIEW_ZOOM_FACTOR)

            cmd = "!View:Zoom: " &
                  " factor=" & factor &
                  ",x=" & pt.dx &
                  ",y=" & pt.dy

            lReturn = PortalExecute(cmd)
            LockWindowUpdate(0&)
        End If
    End Sub

    Private Sub frmTemp_Shown(sender As Object, e As EventArgs) Handles Me.Shown
        Windows.Forms.Cursor.Current = Cursors.Default
        If (bEmptyPdb = True) Then
            Me.Close()
        Else
            vgSettingsProplist.BestFit()
            vgSettingsProplist.Refresh()

        End If
    End Sub

    Private Sub bbClearAllMats_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbClearAllMats.ItemClick

        RemoveAllMats()

    End Sub

    Private Sub vgSettingsProplist_ShownEditor(sender As Object, e As EventArgs) Handles vgSettingsProplist.ShownEditor
        Dim edit As ComboBoxEdit = TryCast(vgSettingsProplist.ActiveEditor, ComboBoxEdit)
        Dim indx As Integer
        ' Dim DB_TOOLSETUP As Integer = 8
        Dim s_ToolSetup As String
        Dim n_ToolSetupID As Long
        Dim s_LayerSetup As String
        Dim n_LayerSetupID As Long

        Dim sOptions As String = sEmpty

        If nToolSetupRowId IsNot Nothing Then

            If vgSettingsProplist.FocusedRow Is nToolSetupRowId Then

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

                    m_toolsetup_id = n_ToolSetupID
                    edit.EditValue = s_ToolSetup

                    edit.Properties.Tag = EncodeTag("glb.ToolSetupID",
                                                                     "Tool Setup",
                                                                     s_ToolSetup,
                                                                     n_ToolSetupID,
                                                                     "Tool Setup ID",
                                                                     DB_TOOLSETUP,
                                                                     0,
                                                                     sOptions, 0)
                End If
            End If
        End If

        If nLayerSetupRowID IsNot Nothing Then

            If vgSettingsProplist.FocusedRow Is nLayerSetupRowID Then

                edit.Properties.Items.Clear()

                edit.EditValue = ""



                arrLayerSetup = PopulateComboList("Select * From [Layer Setups] " &
                                                  "Where ([Layer Setups].[Machine ID] = " & m_machine_id & " ) Order By [Layer Setups].Description", False, True)

                For indx = 1 To arrLayerSetup.Count

                    edit.Properties.Items.Add(New ComboBoxItem(arrLayerSetup(indx).Description))

                    If (indx = 1) Then
                        sOptions = arrLayerSetup(indx).Description
                    Else
                        sOptions = sOptions & ";" & arrLayerSetup(indx).Description
                    End If


                Next

                If (arrLayerSetup.Count > 0) Then
                    s_LayerSetup = arrLayerSetup(1).description

                    n_LayerSetupID = modTranslate.GetLayerSetupID(s_LayerSetup, m_machine_id)


                    edit.EditValue = s_LayerSetup

                    edit.Properties.Tag = EncodeTag("glb.LayerSetupID",
                                                                     "Layer Setup",
                                                                     s_LayerSetup,
                                                                     n_LayerSetupID,
                                                                     "Layer Setup ID",
                                                                     DB_LAYERSETUP,
                                                                     0,
                                                                     sOptions, 0)
                End If

            End If
        End If

        If edit IsNot Nothing Then
            AddHandler edit.EditValueChanged, AddressOf edit_EditValueChanged
        End If

    End Sub


    Private Sub edit_EditValueChanged(sender As Object, e As EventArgs)

        Dim edit As ComboBoxEdit = TryCast(sender, ComboBoxEdit)
        nToolSetupRowId = Nothing
        nLayerSetupRowID = Nothing
        Dim datfilename As String = Nothing
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim VbVarValue As Object
        Dim CVarValue As Object
        Dim m_cnSettings As New OleDb.OleDbConnection
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim m_dtSettings As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim nDataType As Integer


        editValue = edit.EditValue.ToString()

        If (vgSettingsProplist.FocusedRow.Properties.Caption = "Machine") Then

            SelectedMachineName = edit.EditValue.ToString()
            m_machine_id = GetMachineIDFromDescrip(SelectedMachineName)

            VbVarValue = SelectedMachineName 'e.Row.Properties.Value

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            m_cnSettings = New OleDbConnection(connetionString)

            m_cnSettings.Open()

            sql = "UPDATE [Setup] set [Setup].[VbValue] = '" & VbVarValue & "' Where ([Setup].[DatVarName] ='glb.Machine' )"

            m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

            m_daSettings.InsertCommand.ExecuteNonQuery()

            CVarValue = GetCValueBasedOnVarType(nDataType, VbVarValue, m_machine_id)

            sql = "UPDATE [Setup] set [Setup].[CValue] = '" & m_machine_id & "'Where ([Setup].[DatVarName] ='glb.Machine' )"

            m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

            m_daSettings.InsertCommand.ExecuteNonQuery()

            m_daSettings.Dispose()
            m_cnSettings.Dispose()

            m_cnSettings.Close()

            If m_daSettings IsNot Nothing Then m_daSettings = Nothing
            If m_cnSettings IsNot Nothing Then m_cnSettings = Nothing

            modProject.SetMachine(VbVarValue, m_machine_id)

            PopulateDefaultSettings()

            nToolSetupRowId = GetRowByCaption("Tool Setup")

            If (IsNothing(nToolSetupRowId) = False) Then
                nRowIndx = nToolSetupRowId.Index

                vgSettingsProplist.Rows(nToolSetupRowId.ParentRow.Index).ChildRows(nToolSetupRowId.Index).Properties.Value = ""

                vgSettingsProplist.FocusedRow = vgSettingsProplist.Rows(nToolSetupRowId.ParentRow.Index).ChildRows(nToolSetupRowId.Index)
                'vgSettingsProplist.FocusedRow.Properties.Value = ""
                'vgSettingsProplist.ShowEditor()
            End If


        End If

        If (vgSettingsProplist.FocusedRow.Properties.Caption = "Tool Setup") Then

            SelectedToolSetupName = edit.EditValue.ToString
            m_toolsetup_id = GetToolSetupIDFromDescription(SelectedToolSetupName)

            nLayerSetupRowID = GetRowByCaption("Layer Setup")

            If (IsNothing(nLayerSetupRowID) = False) Then
                nRowIndx = nLayerSetupRowID.Index

                vgSettingsProplist.Rows(nLayerSetupRowID.ParentRow.Index).ChildRows(nLayerSetupRowID.Index).Properties.Value = ""

                If (m_toolsetup_id <> 0) Then

                    vgSettingsProplist.FocusedRow = vgSettingsProplist.Rows(nLayerSetupRowID.ParentRow.Index).ChildRows(nLayerSetupRowID.Index)
                End If

            End If

        End If


    End Sub

    Private Function GetRowByCaption(ByVal s As String) As DevExpress.XtraVerticalGrid.Rows.BaseRow
        Dim r As DevExpress.XtraVerticalGrid.Rows.BaseRow
        For Each row As DevExpress.XtraVerticalGrid.Rows.BaseRow In vgSettingsProplist.Rows
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

    Private Sub vgMatVars_CellValueChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgMatVars.CellValueChanged
        Dim sFieldName As String
        Dim VbVarValue As Object
        Dim CVarValue As Object
        Dim m_cnSettings As New OleDb.OleDbConnection
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim m_dtSettings As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim nDataType As Integer
        Dim nSheetID As Long


        Try

            sFieldName = GetValueFromDelimitedData(e.Row.Tag, "DatVarName", sEmpty)
            nDataType = CInt(GetValueFromDelimitedData(e.Row.Tag, "VarDataType", sEmpty))
            nSheetID = CLng(GetValueFromDelimitedData(e.Row.Tag, "nRecordID", sEmpty))
            VbVarValue = e.Row.Properties.Value

            CVarValue = GetCValueBasedOnVarType(nDataType, VbVarValue, e.Row.Tag)

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            m_cnSettings = New OleDbConnection(connetionString)

            m_cnSettings.Open()
            sql = "UPDATE [Material Inventory] set [Material Inventory].[" & sFieldName & " ] = '" & CVarValue & "' Where ([Material Inventory].ID = " & nSheetID & " ) "

            m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnSettings)

            m_daSettings.InsertCommand.ExecuteNonQuery()

            m_daSettings.Dispose()
            m_cnSettings.Dispose()


            m_cnSettings.Close()

            If m_daSettings IsNot Nothing Then m_daSettings = Nothing
            If m_cnSettings IsNot Nothing Then m_cnSettings = Nothing

        Catch ex As Exception

            MessageBox.Show("vgMatVars_CellValueChanged: " & ex.Message)

        End Try

    End Sub

    Private Sub sbCodeViewer_Click(sender As Object, e As EventArgs) Handles sbCodeViewer.Click

        'pcCodeViewer.Visible = True
        xtcResults.SelectedTabPage = xtpCodeViewer

        ceCloseViewer.CheckState = CheckState.Unchecked
        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        lReturn = PortalExecute("*codegen:xrefcreate:")

        Windows.Forms.Cursor.Current = Cursors.Default

        UpdateButtons(m_code_gen)
        m_ncfile = ""

        m_currRecordID = 0
        m_searchString = ""
        m_searchIndex = -1
        sbExportCNC.Enabled = False
    End Sub

    Public Sub UpdateButtons(the_code_gen As String)

        If (the_code_gen = sEmpty) Then
            m_code_gen = "<Select Code Generator>"
        Else
            m_code_gen = the_code_gen
        End If


        cmdCodeGenFile.Text = TrimToFit(m_code_gen, 40)

        cmdGraphicsEditorRefresh.Enabled = (m_code_gen <> NEED_CODEGEN)
        cmdGraphicsEditorStep.Enabled = (m_code_gen <> NEED_CODEGEN)

    End Sub

    Private Sub ceCloseViewer_CheckedChanged(sender As Object, e As EventArgs) Handles ceCloseViewer.CheckedChanged
        If (ceCloseViewer.CheckState = CheckState.Checked) Then

            lReturn = PortalExecute("*codegen:xrefdestroy:")
            lReturn = PortalExecute("!view:highlight:id=0")
            lReturn = PortalExecute("Admin:GlobalModel:")  'for safety

            modSelection.DeselectAll()
            If (m_ncfile <> "") Then lReturn = HeaderStringSet("ncfile", m_ncfile)

            m_is_active = False
            'pcResults.Visible = True
            xtcResults.SelectedTabPage = xtpSheetResults

            sbExportCNC.Enabled = True
        End If
    End Sub

    Private Sub cmdCodeGenFile_Click(sender As Object, e As EventArgs) Handles cmdCodeGenFile.Click
        frmCodeGen.Init(CODE_OPTION.FROM_NESTING)
        frmCodeGen.ShowDialog(Me)
    End Sub

    Private Sub cmdGraphicsEditorRefresh_Click(sender As Object, e As EventArgs) Handles cmdGraphicsEditorRefresh.Click
        If (PartsAreZoned() = False) Then Exit Sub

        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        lstGraphicsEditor.Items.Clear()
        lstGraphicsEditor.Refresh()

        'NOTE: We must remember the 'ncfile' value so that we can
        'reset it upon leaving the code viewer.  Otherwise, said
        'value will be set to "", and then the file name will not
        'appear next time you try to export a physical cnc file.
        If (m_ncfile = "") Then m_ncfile = HeaderStringGet("ncfile", "")

        lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!

        ShowCode()

        Windows.Forms.Cursor.Current = Cursors.Default

    End Sub

    Private Sub ShowCode()

        Dim lNumRecs As Long
        Dim sCodeLine As String
        Dim iGE_CNT As Integer
        Dim lTempRec As Long

        If (m_code_gen = NEED_CODEGEN) Then Exit Sub


        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        lstGraphicsEditor.Items.Clear()


        lReturn = PortalExecute(sPortalCommand)

        lReturn = PortalExecute("*codegen:xrefcount:")
        lNumRecs = PortalGetInt("count", 0)

        For iGE_CNT = 0 To lNumRecs - 1

            lTempRec = CLng(iGE_CNT)
            lReturn = PortalExecute("*codegen:xreffetch:recno=" & lTempRec)

            sCodeLine = PortalGetString("text", "")
            lTempRec = PortalGetInt("id", 0)


            lstGraphicsEditor.Items.Add(New myItemData(sCodeLine, lTempRec))

        Next iGE_CNT

        Windows.Forms.Cursor.Current = Cursors.Default

    End Sub

    Private Sub cmdGraphicsEditorStep_Click(sender As Object, e As EventArgs) Handles cmdGraphicsEditorStep.Click
        Dim dInterval As Double

        If cmdGraphicsEditorStep.Text = "Step" Then
            dInterval = CInt(1) / 8
            Timer1.Interval = dInterval * 1000
            Timer1.Enabled = True
            cmdGraphicsEditorStep.Text = "Pause"
        Else
            Timer1.Enabled = False
            cmdGraphicsEditorStep.Text = "Step"
        End If
    End Sub

    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
        Dim lIndex As Long

        lIndex = lstGraphicsEditor.SelectedIndex + 1


        If lIndex > lstGraphicsEditor.Items.Count - 1 Then
            Timer1.Enabled = False
            cmdGraphicsEditorStep.Text = "Step"

            HighLightEntity()

        Else
            lstGraphicsEditor.SelectedIndex = lIndex
        End If
        HighLightEntity()
    End Sub

    Private Sub HighLightEntity()
        If (lstGraphicsEditor.SelectedIndex > 0) Then

            'In order to support the display of clamp avoidance moves we have
            'changed the way we access records are accessed. This should be a
            'safe thing to do because the blocks of code displayed in the
            'control are one-to-one with those in the CCodeGeoXref object.
            lReturn = PortalExecute("*codegen:highlight:rec=" & lstGraphicsEditor.SelectedIndex)
            'So that we can restart the search from the
            'most recently selected output block
            m_searchIndex = lstGraphicsEditor.SelectedIndex

        End If
    End Sub

    Private Sub lstGraphicsEditor_MouseHover(sender As Object, e As EventArgs) Handles lstGraphicsEditor.MouseHover
        HighLightEntity()
    End Sub

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

    Private Sub chkApplyCutBack_CheckStateChanged(sender As Object, e As EventArgs) Handles chkApplyCutBack.CheckStateChanged
        Dim sql As String
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim connetionString As String

        Try

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

            connection = New OleDbConnection(connetionString)

            sql = "Update [Remnant Control] set  [Remnant Control].[CutBack] =  " & chkApplyCutBack.CheckState & ""

            connection = New OleDbConnection(connetionString)

            connection.Open()

            Dim cmd As OleDbCommand = New OleDbCommand(sql, connection)


            cmd.ExecuteNonQuery()

            connection.Close()

            cmd.Dispose()

            If cmd IsNot Nothing Then cmd = Nothing

            If connection IsNot Nothing Then connection = Nothing


        Catch ex As Exception

            MessageBox.Show("chkApplyCutback_CheckStateChanged : " & ex.Message)
        End Try

    End Sub

    Private Sub GetRemnantControlValues()
        Dim sql As String
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim cmd As OleDbCommand = Nothing

        sql = "Select * FROM [Remnant Control]"

        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        connection = New OleDbConnection(connetionString)

        Try
            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(ds)

            oledbAdapter.Dispose()

            connection.Close()

            If (ds.Tables(0).Rows.Count > 0) Then
                Me.chkApplyCutBack.Checked = ds.Tables(0).Rows(0).Item("CutBack")
                Me.chkCreateRemnant.Checked = ds.Tables(0).Rows(0).Item("Remnant")

            End If

            ds.Dispose()

            If ds IsNot Nothing Then ds = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

        Catch ex As Exception

            MessageBox.Show("GetRemnantControlValues_" & ex.Message)

        End Try


    End Sub

    Private Sub CreateMRPRootNodes(ByVal tl As DevExpress.XtraTreeList.TreeList, sMRPDB_Name As String)
        Dim pdb_displayname As String
        Dim sPdb_Name_Tag As String
        Dim dsPDBs As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim cnt As Integer
        Dim i As Integer
        Dim nID As Integer
        Dim sql As String

        pdb_displayname = PathFileExt(sMRPDB_Name)
        tl.ClearNodes()

        TV_NestConfig.Visible = False

        tl.SuspendLayout()

        bFromCreateNodes = True


        CFG_ROOT_NODE = tl.AppendNode(New Object() {"Job(s) <" & pdb_displayname & ">"}, 0)
        CFG_ROOT_NODE.Tag = 0

        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

        sql = "Select * From PDB_Files"

        connection = New OleDbConnection(connetionString)

        connection.Open()

        oledbAdapter = New OleDbDataAdapter(sql, connection)

        oledbAdapter.Fill(dsPDBs)

        oledbAdapter.Dispose()

        connection.Close()

        cnt = dsPDBs.Tables(0).Rows.Count

        If (cnt > 0) Then
            frmMain.NestingImageCollection.Clear()

            For i = 0 To cnt - 1

                pdb_displayname = dsPDBs.Tables(0).Rows(i).Item("PDB_Name")

                nID = dsPDBs.Tables(0).Rows(i).Item("ID")

                sPdb_Name_Tag = TAG_SETTINGS_NODE & "," & pdb_displayname

                CFG_SETTINGS_NODE = tl.AppendNode(New Object() {PathFile(pdb_displayname)}, CFG_ROOT_NODE, sPdb_Name_Tag)

                CFG_PARTS_NODE = tl.AppendNode(New Object() {"Parts"}, CFG_SETTINGS_NODE, TAG_PARTS_NODE)
                CFG_MATERIAL_NODE = tl.AppendNode(New Object() {"Material"}, CFG_SETTINGS_NODE, TAG_MATERIAL_NODE)
                CFG_RESULTS_NODE = tl.AppendNode(New Object() {"Results (Pending)"}, CFG_SETTINGS_NODE, TAG_RESULTS_NODE)

                LoadResultsIntoTree(pdb_displayname)


                For Each column As DevExpress.XtraTreeList.Columns.TreeListColumn In TV_NestConfig.Columns
                    column.OptionsColumn.AllowMove = False
                Next

            Next

        End If

        dsPDBs.Dispose()

        If dsPDBs IsNot Nothing Then dsPDBs = Nothing

        If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        If connection IsNot Nothing Then connection = Nothing

        'tl.ResumeLayout()

    End Sub

    Private Sub FillPartResultsGrid(s_MM2File As String)
        Dim nQty As Integer
        Dim dsPartVars As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim cnt As Integer
        Dim i As Integer
        Dim nID As Integer
        Dim dArea As Double
        Dim sql As String
        Dim sPieces() As String
        Dim the_PDB_Node_Name As String
        Dim m_Temp_Value = Nothing
        Dim sCandidate As String
        Dim sql2 As String
        Dim dsPart As New DataSet

        sPieces = Split(TV_NestConfig.FocusedNode.Tag, ",")

        the_PDB_Node_Name = sPieces(2)

        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & the_PDB_Node_Name & "';"

        sql = "SELECT * FROM [Part Result] WHERE " &
        "([Part Name] = '" & sPieces(1) & "') AND " &
        "([Sheet Filename] = '" & sPieces(3) & "')"

        connection = New OleDbConnection(connetionString)

        connection.Open()

        oledbAdapter = New OleDbDataAdapter(sql, connection)

        oledbAdapter.Fill(dsPartVars)

        oledbAdapter.Dispose()

        If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing


        cnt = dsPartVars.Tables(0).Rows.Count

        If (cnt > 0) Then

            For i = 0 To cnt - 1

                nQty = dsPartVars.Tables(0).Rows(i).Item("Quantity")

                nID = dsPartVars.Tables(0).Rows(i).Item("Part ID")

                dArea = dsPartVars.Tables(0).Rows(i).Item("Area")

                sql2 = "Select Part.* From Part Where (Part.ID = " & nID & " )"

                oledbAdapter = New OleDbDataAdapter(sql2, connection)

                oledbAdapter.Fill(dsPart)

                oledbAdapter.Dispose()

                If (dsPart.Tables(0).Rows.Count > 0) Then

                    sCandidate = dsPart.Tables(0).Rows(i).Item("Filename")

                    sReturn = modFileIO.ImportMM2(sCandidate)

                End If


            Next


            For Each row As CategoryRow In vgPartResults.Rows

                For Each childRow As EditorRow In row.ChildRows

                    Select Case UCase(GetValueFromDelimitedData(childRow.Tag, "DialogDisplay", 0))

                        Case "QUANTITY"
                            m_Temp_Value = nQty

                        Case "PART ID"
                            m_Temp_Value = nID

                        Case "AREA"
                            m_Temp_Value = dArea

                    End Select
                    childRow.Properties.Value = m_Temp_Value

                    m_Temp_Value = sEmpty

                Next
            Next

        End If

        dsPart.Dispose()
        dsPartVars.Dispose()
        connection.Dispose()
        connection.Close()

        If dsPart IsNot Nothing Then dsPart = Nothing

        If dsPartVars IsNot Nothing Then dsPartVars = Nothing

        If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        If connection IsNot Nothing Then connection = Nothing
    End Sub

    Private Sub ClearResultTree()
        Dim ni As Integer = 0
        Dim nIndex As Long = 0
        Dim node As TreeListNode

        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        Try

            For Each node In TV_NestConfig.Nodes

                If (node.HasChildren = True) Then

                    For Each child As TreeListNode In node.Nodes
                        TV_NestConfig.Nodes.Remove(child)

                    Next
                End If
            Next

        Catch ex As Exception

            MessageBox.Show("Clear Nesting Results Failed" & ex.Message)

        End Try

        Windows.Forms.Cursor.Current = Cursors.Default

    End Sub

    Private Sub chkCreateRemnant_CheckStateChanged(sender As Object, e As EventArgs) Handles chkCreateRemnant.CheckStateChanged
        Dim sql As String
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim connetionString As String

        Try

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

            connection = New OleDbConnection(connetionString)

            sql = "Update [Remnant Control] set  [Remnant Control].[Remnant] =  " & chkCreateRemnant.CheckState & ""

            connection = New OleDbConnection(connetionString)

            connection.Open()

            Dim cmd As OleDbCommand = New OleDbCommand(sql, connection)


            cmd.ExecuteNonQuery()

            cmd.Dispose()

            connection.Close()

            If connection IsNot Nothing Then connection = Nothing

            If cmd IsNot Nothing Then cmd = Nothing

        Catch ex As Exception

            MessageBox.Show("chkApplyCutback_CheckStateChanged : " & ex.Message)
        End Try
    End Sub

    Private Function GetRawBytes(str As String) As Byte()
        Dim charcount As Integer = str.Length
        Dim byttemp As Byte() = New Byte(charcount - 1) {}

        For i As Integer = 0 To charcount - 1
            byttemp(i) = CByte(AscW(str(i)))
        Next

        Return byttemp
    End Function

    Private Function UTF8toUnicode(str As String) As String
        Dim bytUTF8 As Byte()
        Dim bytUnicode As Byte()
        Dim strUnicode As String = [String].Empty

        bytUTF8 = GetRawBytes(str)
        bytUnicode = Encoding.Convert(Encoding.UTF8, Encoding.Unicode, bytUTF8)
        strUnicode = Encoding.Unicode.GetString(bytUnicode)
        Return strUnicode
    End Function
    Private Sub sbTestingButton_Click(sender As Object, e As EventArgs) Handles sbTestingButton.Click
        Dim hDrw, hBlock As Integer
        Dim bRet As Integer
        Dim ImgWidth As Integer
        Dim Xmin, Ymin, Xmax, Ymax, W, Gap As Double
        Dim Lef, Bot, Rig, Top As Double
        Dim szFileName, szRasterFile As String

        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        szFileName = "C:\_JM\nestedsheet.dxf"
        szFileName = PathReplaceExt(PathCurrent(), "dxf")

        UndoPrepare()

        lReturn = PortalExecute("Pattern:Explode:all=1")

        UndoCommit()
        Dim Dxf_Macro_Name As String

        lReturn = HeaderStringSet("mac.nestdxf", PathReplaceExt(PathCurrent(), "dxf"))


        Dxf_Macro_Name = App_Path & "\macros\CCSNestDXF.java"
        UndoPrepare()
        lReturn = PortalExecute("Pattern:Explode:all=1")

        lReturn = PortalExecute(
            "admin:macrorun:" &
            " javafile=" & QStr(Dxf_Macro_Name) &
            ",compile=1")

        'lReturn = modEdit.EditUndo()
        UndoCommit()

        szRasterFile = "C:\_JM\nestedsheet.png"

        hDrw = Lcad.CreateDrawing()
        bRet = Lcad.DrwLoad(hDrw, szFileName, 0)
        If bRet = 1 Then
            Lcad.PropPutInt(hDrw, LC_PROP_DRW_LWMODE, LC_LW_PIXEL)
            Lcad.PropPutFloat(hDrw, LC_PROP_DRW_LWSCALE, 0.1)  ' line width screen scale, mm / pixel

            ' Set back color of Model space to White
            Lcad.PropPutInt(hDrw, LC_PROP_DRW_COLORBACKM, RGB(255, 255, 255))

            ' Set fore color of Model space to Black
            Lcad.PropPutInt(hDrw, LC_PROP_DRW_COLORFOREM, RGB(0, 0, 0))

            'Set The line width 
            Lcad.PropPutInt(hDrw, LC_PROP_DRW_LWIDTH, 3)

            ' get Model block
            hBlock = Lcad.PropGetHandle(hDrw, LC_PROP_DRW_BLOCK_MODEL)

            ' get extents
            Xmin = Lcad.PropGetFloat(hBlock, LC_PROP_BLOCK_XMIN)
            Ymin = Lcad.PropGetFloat(hBlock, LC_PROP_BLOCK_YMIN)
            Xmax = Lcad.PropGetFloat(hBlock, LC_PROP_BLOCK_XMAX)
            Ymax = Lcad.PropGetFloat(hBlock, LC_PROP_BLOCK_YMAX)

            W = Xmax - Xmin

            If W > 0.0 Then
                ' increase output rect
                Gap = W / 30.0  ' blank space
                Lef = Xmin - Gap
                Bot = Ymin - Gap
                Rig = Xmax + Gap
                Top = Ymax + Gap

                ' make raster image and save it in a file
                ImgWidth = 2000  ' image width, pixels

                bRet = Lcad.BlockRasterize(hBlock, szRasterFile, Lef, Bot, Rig, Top, ImgWidth, 0)

                If bRet = 1 Then
                    MessageBox.Show("Rasterize OK")
                End If

            End If

        End If

        Windows.Forms.Cursor.Current = Cursors.Default

    End Sub


    Private Sub toolTipController1_GetActiveObjectInfo(sender As Object, e As DevExpress.Utils.ToolTipControllerGetActiveObjectInfoEventArgs) Handles toolTipController1.GetActiveObjectInfo
        If TypeOf e.SelectedControl Is DevExpress.XtraTreeList.TreeList Then
            Dim tree As TreeList = CType(e.SelectedControl, TreeList)
            Dim hit As TreeListHitInfo = tree.CalcHitInfo(e.ControlMousePosition)
            Dim sPieces() As String
            Dim sToolTipText As String

            If hit.HitInfoType = HitInfoType.Cell Then
                sPieces = Split(hit.Node.Tag, ",")

                If (sPieces.Length > 1) Then
                    sToolTipText = sPieces(1)
                Else
                    sToolTipText = sEmpty
                End If

                Dim cellInfo As Object = New TreeListCellToolTipInfo(hit.Node, hit.Column, Nothing)
                'Dim toolTip As String = String.Format("{0} (Column: {1}, Node ID: {2})", hit.Node(hit.Column), hit.Column.FieldName, hit.Node.Tag)
                Dim toolTip As String = sToolTipText 'hit.Node.Tag
                e.Info = New DevExpress.Utils.ToolTipControlInfo(cellInfo, toolTip)
            End If
        End If
    End Sub

    Private Sub fcbXlsName_ButtonClick(sender As Object, e As ButtonPressedEventArgs) Handles fcbXlsName.ButtonClick
        Dim sXLS_File As String
        Dim sXLS_FileOpen As New OpenFileDialog

        sXLS_FileOpen.Filter = "XLS files (*.xls)|*.xls"
        sXLS_FileOpen.InitialDirectory = PathDriveFolder(fcbXlsName.Text)
        sXLS_FileOpen.FilterIndex = 1
        sXLS_FileOpen.RestoreDirectory = True

        If sXLS_FileOpen.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

            sXLS_File = sXLS_FileOpen.FileName
            fcbXlsName.Text = sXLS_File

            fcbXlsName.ToolTip = sXLS_File
            RegPutString("Fabrication\FileOpen", "XLS_Name", sXLS_File)
            m_xls_Filename = sXLS_File


            ReSetXLS()

        End If
    End Sub

    Private Sub cmdBuild_Click(sender As Object, e As EventArgs) Handles cmdBuild.Click
        Dim cmd As String
        Dim sTemp As String
        Dim m_Length As Double
        Dim m_Width As Double
        Dim m_Thickness As Double
        Dim m_Quantity As Long
        Dim b_No_mat As Boolean = False
        Dim materialID As Long
        Dim materialDescrip As String = Nothing
        Dim bDebugGraphics As Boolean
        Dim machineID As Long
        Dim ToolSetupID As Long
        Dim lCount As Integer
        Dim sql As String
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim stock_vis As Boolean
        Dim pdb_Node As TreeListNode
        Dim bMessageResult As Long
        Dim nparent As Integer
        Dim nMatID As Long
        Dim dLength As Double
        Dim dWidth As Double
        Dim dThickness As Double
        Dim iQuantity As Integer
        Dim dWeight As Double
        Dim dCostPerUnit As Double
        Dim nTypeID As Long

        Try

            b_From_Nesting = True

            If (TV_NestConfig.FocusedNode IsNot Nothing) Then
                If (TV_NestConfig.FocusedNode.Level <> 1) Then

                    TV_NestConfig.FocusedNode = TV_NestConfig.FocusedNode.ParentNode

                End If
            End If

            'first we decide if we are running all of the PDB's
            'Or just one. The easiest way to do this is by
            'checking the caption of the button because this is handle
            'by clicking the node of the tree.

            If (cmdBuild.Text = "Build All") Then
                'cleare the results from the tree
                'in case the user ran the nest already
                Dim operation As New MyIterator
                TV_NestConfig.NodesIterator.DoOperation(operation)

                RunAllPDBs()

                Exit Sub
            Else
                bDoAutoCode = DatFileBooleanGet("glb.AutoCode", False)
                sReturn = RegPutString("Global", "AutoCode", bDoAutoCode)
                bDoAutoPrint = DatFileBooleanGet("glb.AutoPrint", False)

                pdb_Node = TV_NestConfig.FocusedNode

                DeleteResultsForselectedNode(pdb_Node)

                If (DoesTableExist("Sheet Result", PathPDB()) = True) Then

                    RemoveExistingResults()

                End If

                If (DoesTableExist("OLDMaterial Inventory", PathPDB()) = True) Then

                    RemoveExistingTable("OLDMaterial Inventory", PathPDB())

                End If

                CreateOldMaterialTable(PathPDB)

                connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"
                sql = "INSERT INTO [OLDMaterial Inventory] SELECT * FROM [Material Inventory]"


                connection = New OleDbConnection(connetionString)

                connection.Open()

                Dim Sqlcmd As OleDbCommand = New OleDbCommand(sql, connection)

                Sqlcmd.ExecuteNonQuery()

                sql = "SELECT * FROM [Material Inventory]"

                oledbAdapter = New OleDbDataAdapter(sql, connection)

                oledbAdapter.Fill(ds)

                oledbAdapter.Dispose()

                connection.Close()

                If (ds.Tables(0).Rows.Count > 0) Then


                    m_Length = ds.Tables(0).Rows(0).Item("Length")
                    m_Width = ds.Tables(0).Rows(0).Item("Width")
                    m_Thickness = ds.Tables(0).Rows(0).Item("Thickness")
                    materialID = GetMaterialIDFromDescription(ds.Tables(0).Rows(0).Item("Description"))
                    m_Quantity = ds.Tables(0).Rows(0).Item("Quantity")
                    materialDescrip = ds.Tables(0).Rows(0).Item("Description")

                    If (m_Quantity = 0) Then
                        b_No_mat = True
                    End If

                    ds.Dispose()

                Else

                    sql = "SELECT * FROM [Remnant Inventory]"""

                    connection.Open()

                    oledbAdapter = New OleDbDataAdapter(sql, connection)

                    oledbAdapter.Fill(ds)

                    oledbAdapter.Dispose()

                    connection.Close()

                    If (ds.Tables(0).Rows.Count > 0) Then

                        m_Length = ds.Tables(0).Rows(0).Item("Length")
                        m_Width = ds.Tables(0).Rows(0).Item("Width")
                        m_Thickness = ds.Tables(0).Rows(0).Item("Thickness")
                        materialID = GetMaterialIDFromDescription(ds.Tables(0).Rows(0).Item("[description]"))
                        materialDescrip = ds.Tables(0).Rows(0).Item("Description")
                    End If

                    ds.Dispose()

                End If

                If ds IsNot Nothing Then ds = Nothing

                If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                If connection IsNot Nothing Then connection = Nothing

                If (IS_CCS_FAB = False And b_No_mat = True) Then

                    bMessageResult = MessageBox.Show("1 or more material's quantity is set to '0'! Do you want to continue?", "MaterialExtents Error!", MessageBoxButtons.YesNo, MessageBoxIcon.Error)

                    If (bMessageResult = System.Windows.Forms.DialogResult.No) Then
                        GoTo StopNest
                    End If

                End If

                nMatID = materialID
                dLength = modMaterial.GetMaterialLength(nMatID)
                dWidth = modMaterial.GetMaterialWidth(nMatID)
                dThickness = modMaterial.GetMaterialThickness(nMatID)
                iQuantity = modMaterial.GetMaterialQty(nMatID)
                dCostPerUnit = modMaterial.GetMaterialCost(nMatID)

                SetMaterial(materialDescrip, nMatID)

                RegPutString("Global", "MaterialThickness", CStr(dThickness))
                RegPutString("Global", "MaterialLength", CStr(dLength))
                RegPutString("Global", "MaterialWidth", CStr(dWidth))

                machineID = modProject.GetMachineID()

                ToolSetupID = modProject.GetToolSetupID()

                bDebugGraphics = True

                m_is_nesting = True

                stock_vis = modView.StockLayerVisible(True)
                modView.SetViewModes(VIEW_WIRE)

                'MsgBox("The material ID is < " & materialID & " >")

                'MsgBox("The material is < " & GetMaterialIDFromDescription(materialDescrip) & " >")
                'MsgBox("Thematerial weight is < " & GetMaterialWeight(materialID))

                sTemp = NewModel("<nesting>", machineID, ToolSetupID, materialID, m_Length, m_Width, m_Thickness)

                If (IS_CCS_FAB = True) Then

                    cmd = "nest:true:ccs=1," &
                          " configdb=" & QStr(PathCMDB()) &
                          ",partdb=" & QStr(PathPDB()) &
                          ",display=" & IIf(bDebugGraphics, 1, 0)

                Else

                    cmd = "nest:true:" &
                          " configdb=" & QStr(PathCMDB()) &
                          ",partdb=" & QStr(PathPDB()) &
                          ",display=" & IIf(bDebugGraphics, 1, 0)
                End If

                lReturn = PortalExecute(cmd)

                lCount = PortalGetInt("sheet count", 0)

                'Restore the previous stock layer visibility state.
                stock_vis = modView.StockLayerVisible(stock_vis)

                m_is_nesting = False

                '=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                If (lCount > 0) Then

                    NestResults()
                    If (frmNestResults.CompletionStatus() <> vbCancel) Then

                        If (bDoAutoCode = True) Then

                            frmCodeGen.Init(CODE_OPTION.AUTO_CODE_NEST)
                            frmCodeGen.ShowDialog(fMainForm)

                        End If

                        'If (bDoAutoPrint = True And TList2.ListCount > 0) Then
                        '    frmPrintNests.Set_If_From_Nesting(True)
                        '    lReturn = appMain.ExecuteFunction(109)
                        'End If

                    End If



                Else

                    frmNestResults.Show(Me)
                    DeleteNestResults()
                End If

            End If
StopNest:

            UpdateWeightPerPart()

            b_From_Nesting = False

        Catch ex As Exception

            MessageBox.Show("CmdBuild: " & ex.Message)

        End Try


    End Sub
    Private Sub RestoreOriginalMats()
        Dim m_dsMaterial As New DataSet
        Dim sql As String
        Dim connection As OleDbConnection
        Dim connetionString As String
        Dim cnt As Long
        Dim m_daPMaterial As New OleDb.OleDbDataAdapter
        Dim Sqlcmd As OleDbCommand
        Dim nMyQty As Long
        Dim nOldID As Long

        Try


            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            sql = "Select * From [OLDMaterial Inventory]"

            m_daPMaterial = New OleDbDataAdapter(sql, connection)

            m_daPMaterial.Fill(m_dsMaterial)

            m_daPMaterial.Dispose()

            cnt = m_dsMaterial.Tables(0).Rows.Count

            If (cnt > 0) Then

                For i = 0 To cnt - 1

                    nMyQty = m_dsMaterial.Tables(0).Rows(i).Item("Quantity")

                    nOldID = m_dsMaterial.Tables(0).Rows(i).Item("ID")

                    sql = "UPDATE [Material Inventory] set [Material Inventory].[Quantity]= '" & nMyQty & "' Where ([Material Inventory].ID = " & nOldID & " ) "

                    Sqlcmd = New OleDbCommand(sql, connection)

                    Sqlcmd.ExecuteNonQuery()

                    Sqlcmd.Dispose()

                Next

            End If

            m_dsMaterial.Dispose()

            connection.Close()

            If m_daPMaterial IsNot Nothing Then m_daPMaterial = Nothing
            If m_dsMaterial IsNot Nothing Then m_dsMaterial = Nothing

            If connection IsNot Nothing Then connection = Nothing

        Catch ex As Exception

            MessageBox.Show("RestoreOriginalMats: " & ex.Message)

        End Try
    End Sub
    Private Sub RemoveAllMats()
        Dim nID As Long
        Dim sql As String
        Dim connection As OleDbConnection
        Dim connetionString As String
        Dim cmd As OleDbCommand
        Dim childRows As VGridRows = vgMatVars.Rows(0).ChildRows

        Try

            If (vgMaterial.FocusedRow.ParentRow.Index = 0) Then 'We are under the Process order node

                connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

                connection = New OleDbConnection(connetionString)

                connection.Open()

                For Each childRow As EditorRow In vgMaterial.Rows(0).ChildRows
                    nID = GetValueFromDelimitedData(childRow.Tag, "PMatID", 0)

                    sql = "DELETE * From [Material Inventory] Where ([Material Inventory].ID =" & nID & " )"

                    cmd = New OleDbCommand(sql, connection)

                    cmd.ExecuteNonQuery()

                Next

                If (vgMaterial.Rows(0).ChildRows.Count > 0) Then

                    vgMaterial.Rows(0).ChildRows.Clear()

                End If

                'cmd.Dispose()
                connection.Dispose()

                connection.Close()

                'If cmd IsNot Nothing Then cmd = Nothing

                If connection IsNot Nothing Then connection = Nothing

            End If

        Catch ex As Exception

            MessageBox.Show("RemoveAllMats: " & ex.Message)

        End Try


    End Sub

    Private Sub vgParts_ProcessDragRow(sender As Object, e As DevExpress.XtraVerticalGrid.Events.DragRowEventArgs) Handles vgParts.ProcessDragRow
        If (e.Effect = DevExpress.XtraVerticalGrid.RowDragEffect.MoveChild) Then
            e.Effect = DevExpress.XtraVerticalGrid.RowDragEffect.InsertAfter

        End If
    End Sub

    Private Sub vgParts_StartDragRow(sender As Object, e As DevExpress.XtraVerticalGrid.Events.StartDragRowEventArgs) Handles vgParts.StartDragRow
        bDraggingStarted = True

    End Sub



    Private Sub TV_NestConfig_MouseUp(sender As Object, e As MouseEventArgs) Handles TV_NestConfig.MouseUp
        Dim itemtype As Long

        If e.Button = MouseButtons.Right Then

            Dim TreeList As TreeList = DirectCast(sender, TreeList)
            _hitInfo = TreeList.CalcHitInfo(e.Location)

            TV_NestConfig.FocusedNode = _hitInfo.Node

            If (_hitInfo.Node IsNot Nothing) Then
                If _hitInfo.Node.Level = 3 Then



                    itemtype = TreeItemType(TV_NestConfig.FocusedNode)
                    PopUp_TV_Config.ItemLinks.Clear()
                    'PopUPSheetDelete.ShowPopup(Control.MousePosition)
                    'PopUp_TV_Config.ShowPopup(e.Location)
                    If _hitInfo.HitInfoType = HitInfoType.Cell Then
                        For Each item As BarItem In CFG_Man_BarMan.Items
                            If item.Category.Name = "SheetDelete" Then
                                PopUp_TV_Config.AddItem(item)
                            End If
                        Next
                        PopUp_TV_Config.ShowPopup(Control.MousePosition)
                    End If
                End If
            End If

        End If

    End Sub

    Private Sub bbiSheetDelete_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbiSheetDelete.ItemClick
        Dim sSheetName As String
        Dim sql1 As String
        Dim sql2 As String
        Dim connetionString As String
        Dim sql As String
        Dim m_dtPart As New DataTable
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim spieces() As String
        Dim nSheetID As Integer


        Try

            spieces = Split(TV_NestConfig.FocusedNode.Tag, ",")
            sSheetName = spieces(1)

            nSheetID = GetSheetIDFromName(sSheetName)

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & m_pdb_filename & "';"

            m_cnPart = New OleDbConnection(connetionString)

            m_cnPart.Open()

            sql = "DELETE SheetTooling.* From SheetTooling Where (SheetTooling.SheetID = " & nSheetID & " )"

            If DoesTableExist("SheetTooling", m_pdb_filename) = True Then
                m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

                m_daPart.InsertCommand.ExecuteNonQuery()

            End If

            sql = "Delete [Part Result].* From [Part Result] Where ([Part Result].[Sheet Filename] = '" & sSheetName & "' ) "

            m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

            m_daPart.InsertCommand.ExecuteNonQuery()


            sql = "Delete [Sheet Result].* From [Sheet Result] Where ([Sheet Result].[Sheet Filename] = '" & sSheetName & "' )"

            m_daPart.InsertCommand = New OleDbCommand(sql, m_cnPart)

            m_daPart.InsertCommand.ExecuteNonQuery()

            m_cnPart.Dispose()
            m_cnPart.Close()

            TV_NestConfig.FocusedNode.Remove()


        Catch ex As Exception
            MessageBox.Show("bbiSheetDelete_ItemClick: " & ex.Message)
        End Try


    End Sub

    Private Function GetSheetIDFromName(nst_Sheet_Name As String) As Long
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim dsParts As New DataSet
        Dim sql As String
        Dim cnt2 As Integer

        GetSheetIDFromName = -1

        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

        sql = "Select [Sheet Result].* From [Sheet Result] " &
                "Where ([Sheet Result].[Sheet Filename] = '" & nst_Sheet_Name & "' )"

        connection = New OleDbConnection(connetionString)

        connection.Open()

        oledbAdapter = New OleDbDataAdapter(sql, connection)

        oledbAdapter.Fill(dsParts)

        oledbAdapter.Dispose()

        oledbAdapter.Dispose()

        connection.Dispose()
        connection.Close()

        cnt2 = dsParts.Tables(0).Rows.Count

        If (cnt2 > 0) Then

            GetSheetIDFromName = dsParts.Tables(0).Rows(0).Item("ID")

        End If

        dsParts.Dispose()

        If dsParts IsNot Nothing Then dsParts = Nothing

        If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        If connection IsNot Nothing Then connection = Nothing

    End Function
    Private Function GetMaterialWeight(ByVal m_Matid As Long) As Double
        Dim ds As New DataSet
        Dim connection As OleDb.OleDbConnection
        Dim oledbAdapter As OleDb.OleDbDataAdapter
        Dim connetionString As String
        Dim sql As String


        GetMaterialWeight = 0 'assume failure

        Try

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

            sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].ID = " & m_Matid & " )"

            connection = New OleDb.OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDb.OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(ds)

            oledbAdapter.Dispose()

            connection.Close()

            For i = 0 To ds.Tables(0).Rows.Count - 1

                GetMaterialWeight = CDbl(ds.Tables(0).Rows(i).Item("Weight").ToString)
            Next

            ds.Dispose()

            If ds IsNot Nothing Then ds = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

        Catch ex As Exception

            MessageBox.Show("GetMaterialWeight " & ex.Message)

        End Try

    End Function


    Public Sub UpdateCostPerPart(sFilename As String)
        Dim oledbAdapter As New OleDbDataAdapter
        Dim connection As OleDbConnection
        Dim connetionString As String
        Dim dsProps As New DataSet
        Dim sql As String
        Dim ChildRowTag As String
        Dim recordID As Long
        Dim sInternalName As String = Nothing
        Dim indx As Integer
        Dim VbVarValue As Double
        Dim m_cnSettings As New OleDb.OleDbConnection
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim m_dtSettings As New DataTable
        Dim CPP As String
        Dim cmd As OleDbCommand
        Dim dsParts As New DataSet
        Dim sMaterialCost As Double
        Dim lMaterialID As Long
        Dim cnt2 As Integer

        'This function updates the AppDb to store the last values used
        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathPDB() & "';"

        connection = New OleDbConnection(connetionString)

        connection.Open()

        connection = New OleDbConnection(connetionString)

        connection.Open()

        sql = "Select Part.* From Part"

        oledbAdapter = New OleDbDataAdapter(sql, connection)

        oledbAdapter.Fill(dsParts)

        oledbAdapter.Dispose()

        If (dsParts.Tables(0).Rows.Count > 0) Then
            lMaterialID = CLng(modProject.GetMaterialID)

            sMaterialCost = CDbl(modMaterial.GetMaterialCost(lMaterialID))

            CPP = sMaterialCost ' modMaterial.GetMaterialCost(modProject.GetMaterialID)

            sql = "UPDATE [Part] set [Part].[Cost Per Part] = '" & CPP & "'"

            cmd = New OleDbCommand(sql, connection)

            cmd.ExecuteNonQuery()

            connection.Close()

        End If

        m_daSettings.Dispose()

        m_cnSettings.Dispose()

        m_cnSettings.Close()

        oledbAdapter.Dispose()

        If dsParts IsNot Nothing Then dsParts = Nothing

        If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        If connection IsNot Nothing Then connection = Nothing

    End Sub

End Class
Public Class MyIterator
    Inherits TreeListOperation
    Private _text As String = "Results Completed"

    Public Overrides Sub Execute(node As TreeListNode)

        If node(0).ToString() = _text Then

            node.Nodes.Clear()
            node.SetValue(0, "Results (Pending)")

        End If

    End Sub

End Class
