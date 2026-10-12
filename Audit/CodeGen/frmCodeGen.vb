Imports ADOX
Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraTreeList.Nodes
Imports DevExpress.XtraVerticalGrid.Rows
Imports System.Data.OleDb
Imports System.IO

Public Class frmCodeGen
    'Even though the radio button order is Grid/Closest/Progressive
    'we must maintain the historical "optimization mode" values.
    Private Const GRID_MODE = 1
    Private Const PROGRESSIVE_MODE = 2
    Private Const CLOSEST_PT_MODE = 3

    Private sPieces() As String
    Private nCurrentCharPos As Integer
    Private nEvalError As Integer
    Private sExpression As String

    ' Flags settings:
    Const bit00 As Integer = &H1S
    Const bit01 As Integer = &H2S
    Const bit02 As Integer = &H4S
    Const bit03 As Integer = &H8S
    Const bit04 As Integer = &H10S
    ' Flags for arithmetic
    Const push1 As Boolean = bit01 Or bit02 Or bit03 Or bit04 ' +-
    Const push2 As Boolean = bit02 Or bit03 Or bit04 ' */
    Const push3 As Boolean = bit03 Or bit04 ' ^
    Const push4 As Integer = bit04 ' negation -
    ' mathematic operators
    Const plus As Integer = 43
    Const minus As Integer = 45 '+-
    Const Multiply As Integer = 42
    Const Divide As Integer = 47 '*/
    Const Exponent As Integer = 94 '^
    Const OpenParen As Integer = 40
    Const CloseParen As Integer = 41 '()
    Const DecPoint As Integer = 46

    Private m_context As CODE_OPTION
    Private m_code_gen As String
    Private m_ncfile As String
    Private m_seq_order_mode As Long
    Private m_sPortalCommand As String
    Private m_is_initing As Boolean
    Private m_is_active As Boolean
    Private m_debug_code As Boolean
    Private m_PDB_Code_counter As Long
    Private nOptalgorithm As Long
    Private nSlice_Opt As Long

    Private WithEvents rbCodeGen As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit
    Private previouslyCheckedButton As CheckButton
    Private images As New Dictionary(Of CheckButton, Image())()
    Private fStream As FileStream
    Private Label_DB_Name As String


    Public Shadows Function IsActive() As Boolean
        IsActive = m_is_active
    End Function

    Public Overloads Function ClientMouseUp() As Long
        ClientMouseUp = 0  'Currently only fodder for frmPicModeler
    End Function
    Public Function EchoLocation(x_world As Double, y_world As Double) As Long
        Try

            mtbStartPointX.Text = Round(x_world, Places.TenThousandths)
            mtbStartPointY.Text = Round(y_world, Places.TenThousandths)
            EchoLocation = 1 'fodder


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Function

    Public Sub SetSeqOrderMode(Mode As Long)
        m_seq_order_mode = Mode
    End Sub

    Public Function GetPortalCommand() As String
        GetPortalCommand = m_sPortalCommand
    End Function

    Public Function GetSelectedCodeGen() As String
        GetSelectedCodeGen = m_code_gen
    End Function
    Public Sub Init(context As CODE_OPTION)

        m_context = context

        EnableOrDisable()

    End Sub
    Private Sub FillDefaultComboLists()

        lcbGridSequence.Properties.Items.Add("Preset Tool Order")
        lcbGridSequence.Properties.Items.Add("Containment Order")
        lcbGridSequence.Properties.Items.Add("Process Flowchart")
        lcbGridSequence.SelectedIndex = 0

    End Sub

    Private Sub frmCodeGen_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        Try
            m_is_active = False
            Me.Dispose()
        Catch ex As Exception

            MessageBox.Show("frmCodeGen_FormClosed: " & ex.Message)

        End Try
        bIsCodeGenLoaded = False
    End Sub

    Private Sub frmCodeGen_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing

        Try

            If (bIsSeqMoveLoaded = True) Then

                'We update frmSeqMove only when we actually optimized.
                If (m_sPortalCommand <> "") Then
                    frmSeqMove.PostOptimizeUpdate()
                End If

                frmMain.SetCurrentDialog(frmSeqMove)

            Else

                frmMain.SetCurrentDialog(Nothing)

            End If

        Catch ex As Exception

            MessageBox.Show("frmCodeGen_FormClosing: " & ex.Message)

        End Try
    End Sub

    Private Sub frmCodeGen_HandleCreated(sender As Object, e As EventArgs) Handles Me.HandleCreated

    End Sub

    Private Sub frmCodeGen_Load(sender As Object, e As EventArgs) Handles Me.Load
        LoadDialog()

    End Sub

    Public Sub LoadDialog()
        Dim sCGFilename As String

        Try
            bIsCodeGenLoaded = True

            rgOptMode.CalcBestSize()


            m_debug_code = (InStr(1, Command(), "~c") > 0)

            If (IS_CCS_FAB = True) Then

                rgOptMode.Properties.Items(2).Enabled = False
                'chkCreateMasterFile.Visible = True


                'ITI does not want to confuse the users with the compile button
                'my understanding is it is not really Necessary  anymore anyways
                'But we can still get to it if the command line param is set
                If (m_debug_code = True) Then

                    cmdCompile.Visible = True

                Else

                    cmdCompile.Visible = True

                End If


            Else

                rgOptMode.Properties.Items(2).Enabled = True
                ' chkCreateMasterFile.Visible = False

            End If

            'This inhibits EnableOrDisable() until state is stable.
            m_is_initing = True

            m_sPortalCommand = ""

            GetCNCExtensions()

            ' FillDefaultComboLists()
            lcbGridSequence.SelectedIndex = 0

            sCGFilename = HeaderStringGet("cgfile", "")

            If (sCGFilename <> "") Then

                'Entering this branch implies that we have generated
                'code at least once, either via the CodeViewer of Export.
                'This does not guarantee the code generator exists, however.
                'We defer that check till actual code generation.

                fcbCodeGenFile.Text = sCGFilename
                fcbCodeGenFile.Tag = sCGFilename
                m_code_gen = sCGFilename

                If (FileExists(sCGFilename) = True) Then
                    ' load up variables from the code generation file
                    LoadVariables(sCGFilename, "cg")

                End If



#If DIAMOND Then

                GetDefaultsFromReg()
#End If

            Else

                GetDefaultsFromReg()

            End If

            GetValuesFromHeader()

            InitExportTo()


            m_is_initing = False

            If (m_context = CODE_OPTION.SEQUENCE_ORDER) Then
                cbfOptimizeToolpath.Checked = True
                GetDefaultsFromReg()

            End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub LoadVariables( _
                ByVal sFileName As String, _
                ByVal sClass As String, _
                Optional bClear As Boolean = True)
        Dim lCount As Long
        Dim nRow As Long 'Actually represents the number of variables
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

        vgCodeGenVars.Rows.Clear()

        Try

            currentCategoryRow = CreateCategoryRow(vgCodeGenVars, "Code Generation Variables", "Code Generation Variables", "Code Generation Variables", 0, "CGVAR", DB_HEADER, 0, sEmpty, 0)
            lReturn = PortalExecute("admin:javavarsextract:" & _
           " file=" & QStr(sFileName) & ",class=" & QStr(sClass))

            lCount = PortalGetInt("count", 0)

            For nRow = 1 To lCount
                lReturn = PortalExecute("*admin:javavarget:index=" & nRow - 1)

                sData = PortalGetString("data", "")

                'NOTE: zero-based!
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
                        currentCategoryRow = CreateCategoryRow(vgCodeGenVars, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0)

                    Case DB_STR
                        currentCategoryRow.ChildRows.Add(CreateStringEditorRow(vgCodeGenVars, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))


                    Case DB_BOOL
                        currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(vgCodeGenVars, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                    Case DB_INT
                        currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(vgCodeGenVars, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                    Case DB_DBL
                        currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(vgCodeGenVars, my_DatVarName, my_DialogDisplay, my_VBValue, my_CValue, my_CVarName, my_DataType, my_RecodID, my_sOptions, 0))

                    Case DB_TOOL

                    Case DB_DCOMBO
                        Dim i As Integer

                        lstOptions.Items.Clear()

                        For i = 4 To sPieces.Count - 1
                            lstOptions.Items.Add(sPieces(i).ToString)

                            If (sPieces(i).ToString = my_VBValue) Then
                                my_CValue = i - 3
                            End If
                        Next
                        arrOptions = PopulateComboListFromDat(lstOptions)

                        currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgCodeGenVars, _
                                                                my_DatVarName, _
                                                                 my_DialogDisplay, _
                                                                 my_VBValue, _
                                                                 my_CValue, my_CVarName, my_DataType, my_RecodID, arrOptions, 0))
                        'Case DB_COMBO
                        '    Dim i As Integer

                        '    lstOptions.Items.Clear()

                        '    For i = 3 To sPieces.Count - 1
                        '        lstOptions.Items.Add(sPieces(i).ToString)

                        '        If (sPieces(i).ToString = my_VBValue) Then
                        '            my_CValue = i - 3
                        '        End If
                        '    Next
                        '    arrOptions = PopulateComboListFromDat(lstOptions)

                        '    currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(vgCodeGenVars, _
                        '                                            my_DatVarName, _
                        '                                             my_DialogDisplay, _
                        '                                             my_VBValue, _
                        '                                             my_CValue, my_CVarName, my_DataType, my_RecodID, arrOptions, 0))
                End Select
            Next


            vgCodeGenVars.BestFit()


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub
    Private Sub GetCNCExtensions()
        Dim nI As Integer
        Dim sEntry As String
        Dim sFormatted As String
        Dim sFilters As String
        Dim instance As Microsoft.Win32.RegistryKey


        Try
            instance = Microsoft.Win32.Registry.CurrentUser.OpenSubKey(RegAppKey() & "\\CodeGeneration\\CNCExtensions\\")

            If (instance IsNot Nothing) Then
                EntryName = instance.GetValueNames
            Else
                lstExtensions.Items.Clear()
                GoTo skip
            End If


            'Format String as follows: CNC Files (*.cnc)|*.cnc|Text Files (*.txt)|*.txt|PGM Files (*.pgm)|*.pgm|No Ext (*.)|*

            sFilters = ""

            lstExtensions.Items.Clear()

            nI = 0

            For nI = 0 To EntryName.Count() - 1
                sEntry = Trim(EntryName(nI))

                '2007.03.04 (PE) -- So that CNC does not appear twice in the filter list.
                If (UCase(sEntry) <> "CNC") Then
                    sFormatted = UCase$(sEntry) & " Files (*." & sEntry & ")|*." & sEntry
                    lstExtensions.Items.Insert(0, sFormatted)

                End If

            Next
Skip:
            If (nI = 0) Then
                'ALWAYS provide the following options.
                lstExtensions.Items.Insert(0, "CNC Files (*.cnc)|*.cnc")
                lstExtensions.Items.Insert(1, "All Files (*.*)|*.*")
                lstExtensions.Items.Insert(2, "No Ext. (*.*)|*.")

            Else
                lstExtensions.Items.Insert(0, "CNC Files (*.cnc)|*.cnc")
                lstExtensions.Items.Insert(1, "All Files (*.*)|*.*")
                lstExtensions.Items.Insert(2, "No Ext. (*.*)|*.")
            End If


            For nI = 0 To lstExtensions.Items.Count - 1
                sFilters = sFilters & lstExtensions.Items(nI).ToString() & "|"
            Next

            'TOD V21
            'fcbExportTo.Filter = sFilters

            ''reset the filter index to the same as the current filename
            'If (fcbExportTo.Filename <> "") Then
            '    fcbExportTo.FilterIndex = GetExtensionIndex(PathExt(fcbExportTo.Filename))
            'End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub EnableOrDisable()

        'Early exit until state is stable.
        If (m_is_initing = True) Then Exit Sub
        lblExportTo.Visible = False
        fcbExportTo.Visible = False
        vgCodeGenVars.Visible = False
        fcbCodeGenFile.Visible = False
        chkSubRoutines.Visible = False
        cmdCNCFileExtensions.Visible = False

        'So that the c2dStartPt does not appear superimposed
        'on top of Toolbar1 when Optimize is disabled.
        gcClosestPoint.Visible = False

        Try

            Select Case m_context
                Case CODE_OPTION.CODE_FILE, CODE_OPTION.DYNATORCH_FILE
                    lblExportTo.Visible = True
                    fcbExportTo.Visible = True
                    vgCodeGenVars.Visible = True
                    fcbCodeGenFile.Visible = True
                    chkSubRoutines.Visible = True
                    cmdCNCFileExtensions.Visible = True
                Case CODE_OPTION.CODE_VIEWER, CODE_OPTION.CCSFAB, CODE_OPTION.CCSFAB_CODE_VIEWER, CODE_OPTION.AUTO_CODE_NEST
                    vgCodeGenVars.Visible = True
                    chkSubRoutines.Visible = True
                    fcbCodeGenFile.Visible = True
                Case CODE_OPTION.SEQUENCE_ORDER
                    'Handle in the form Load event so we do not cause an infinite loop

            End Select

            If (cbfOptimizeToolpath.Checked = False) Then
                gcStartConer.Visible = False
                gcClosestPoint.Visible = False
                lblSticky.Enabled = False
                txtSticky.Enabled = False
                cmdFlowchart.Enabled = False
                chkZigZag.Enabled = False
            Else
                gcStartConer.Visible = True

                '(0) Grid / (1) Closest / (2) Progressive
                If (rgOptMode.SelectedIndex = 1) Then
                    gcClosestPoint.BringToFront()
                    gcClosestPoint.Visible = True
                    gcStartConer.Visible = False
                    lblSticky.Enabled = False
                    txtSticky.Enabled = False
                    chkZigZag.Enabled = False

                Else
                    'The default behavior is Grid mode.
                    gcStartConer.BringToFront()
                    gcStartConer.Visible = True
                    gcClosestPoint.Visible = False


                    lblSticky.Enabled = True
                    txtSticky.Enabled = True
                    chkZigZag.Enabled = True

                End If


                cmdFlowchart.Enabled = True

            End If

            If (lcbGridSequence.SelectedIndex <> 2) Then
                cmdFlowchart.Visible = False
            Else
                cmdFlowchart.Visible = True
            End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub GetValuesFromHeader()
        Dim nDoSubs As Long
        Dim sNCFilename As String
        Dim nDoOptimize As Long
        Dim nOptimizeMode As Long
        ' Dim nSlice_Opt As Long
        Dim nSticky As Long
        Dim nSeqOpt As Long
        Dim nBiDir As Long
        Dim DefaultExtension As String
        Dim sVarName As String
        Dim sData As String
        Dim sTempMach As String

        Try

            frmFlowchart.FlowChartInitMode(1)

            'Set the checkbox for subs
            nDoSubs = HeaderIntGet("subs", 0)
            chkSubRoutines.Checked = nDoSubs

            'Get the NC Filename and extract the extension
            sNCFilename = HeaderStringGet("ncfile", "")
            If (sNCFilename <> "") Then
                'set the NC filename filebox
                fcbExportTo.Text = sNCFilename

                'DefaultExtension = PathExt(sNCFilename)
                sTempMach = HeaderStringGet("MachCfg", "")

                If InStr(1, DefaultExtension, "*.") = 0 Then
                    DefaultExtension = "*." & DefaultExtension
                End If

                'fcbExportTo.FilterIndex = GetExtensionIndex(DefaultExtension)
            End If


#If APIHeatTransfer Then
            If InStr(1, UCase(sTempMach), "VIPROS") > 0 Then
                DefaultExtension = "cnc"
            Else
                DefaultExtension = "LCD"
            End If
#End If
            'Set the checkbox for optimizing the toolpath
            nDoOptimize = HeaderIntGet("opttoolpath", 1)
            cbfOptimizeToolpath.Checked = nDoOptimize

            'The radio button order is Grid/Closest/Progressive
            nOptimizeMode = HeaderIntGet("OptAlgorithm", GRID_MODE)
            If (nOptimizeMode = CLOSEST_PT_MODE) Then
                rgOptMode.SelectedIndex = 1

            ElseIf (nOptimizeMode = PROGRESSIVE_MODE) Then
                rgOptMode.SelectedIndex = 2

            Else
                rgOptMode.SelectedIndex = 0

            End If

            'set the start corner and direction
            nSlice_Opt = HeaderIntGet("slice_opt", 0)

            Select Case nSlice_Opt
                Case 0

                    btnSlice1.Checked = True
                Case 1
                    btnSlice2.Checked = True
                Case 2
                    btnSlice3.Checked = True
                Case 3
                    btnSlice4.Checked = True
                Case 4
                    btnSlice5.Checked = True
                Case 5
                    btnSlice6.Checked = True
                Case 6
                    btnSlice7.Checked = True
                Case 7
                    btnSlice8.Checked = True
                Case Else
                    btnSlice1.Checked = True

            End Select

            'Get the Linear Trend Value
            nSticky = HeaderIntGet("Sticky", 100)
            txtSticky.Text = CStr(nSticky)

            'Get Sequence By Mode (Preset Tool Order,Containment Order,Process Flowchart)
            nSeqOpt = HeaderIntGet("seq_opt", -1)

            If (nSeqOpt = -1) Then
                nSeqOpt = RegGetInt("CodeGeneration", "GridSequence", "2")
            Else
                lcbGridSequence.SelectedIndex = nSeqOpt
            End If


            If (nSeqOpt = 2) Then
                Me.cmdFlowchart.Visible = True
            Else
                Me.cmdFlowchart.Visible = False

            End If

            'Set Zig Zag Checkbox
            nBiDir = HeaderIntGet("bidir", -1)

            If (nBiDir = -1) Then
                chkZigZag.Checked = True
            Else
                chkZigZag.Checked = nBiDir
            End If


            For Each row As CategoryRow In vgCodeGenVars.Rows

                For Each childRow As EditorRow In row.ChildRows

                    sVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")
                    sData = GetValueFromDelimitedData(childRow.Tag, "VBValue", "")
                    childRow.Properties.Value = HeaderStringGet(sVarName, sData)

                Next
            Next


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.

        rbCodeGen = New RepositoryItemButtonEdit()

        For Each Control In gcStartConer.Controls
            If TypeOf Control Is CheckButton Then
                Dim cb As CheckButton = TryCast(Control, CheckButton)
                AddHandler cb.CheckedChanged, AddressOf CheckChanged
            End If
        Next Control

        images.Add(btnSlice1, New Image() {My.Resources.LowerLeftX, My.Resources.NELowerLeftX})
        images.Add(btnSlice2, New Image() {My.Resources.LowerRightX, My.Resources.NELowerRightX})
        images.Add(btnSlice3, New Image() {My.Resources.UpperRightX, My.Resources.NEUpperRightX})
        images.Add(btnSlice4, New Image() {My.Resources.UpperLeftX, My.Resources.NEUpperLeftX})
        images.Add(btnSlice5, New Image() {My.Resources.LowerLeftY, My.Resources.NELowerLeftY})
        images.Add(btnSlice6, New Image() {My.Resources.LowerRightY, My.Resources.NELowerRightY})
        images.Add(btnSlice7, New Image() {My.Resources.UpperRightY, My.Resources.NEUpperRightY})
        images.Add(btnSlice8, New Image() {My.Resources.UpperLeftY, My.Resources.NEUpperLeftY})

        'EnableOrDisable()

        ' m_is_active = True

        'frmMain.SetCurrentDialog(Me)
    End Sub

    Private Sub cmdFlowchart_Click(sender As Object, e As EventArgs) Handles cmdFlowchart.Click
        frmFlowchart.ShowDialog(Me)

    End Sub

    Private Sub fcbCodeGenFile_ButtonClick(sender As Object, e As DevExpress.XtraEditors.Controls.ButtonPressedEventArgs) Handles fcbCodeGenFile.ButtonClick
        Dim edit As DevExpress.XtraEditors.ButtonEdit = TryCast(sender, DevExpress.XtraEditors.ButtonEdit)

        Dim dialog As New OpenFileDialog()
        dialog.Multiselect = False
        dialog.Title = "Select Java file"
        dialog.Filter = "Java Files(*.java)|*.java"
        dialog.InitialDirectory = PathDriveFolder(fcbCodeGenFile.Text)

        If dialog.ShowDialog(Me) = DialogResult.OK Then
            edit.Text = TrimToFit(dialog.FileName, 35)

            m_code_gen = (dialog.FileName)
            edit.Tag = m_code_gen
            LoadVariables(m_code_gen, "cg")
        End If

    End Sub

    Private Sub GetDefaultsFromReg()
        Dim lOptimizeMode As Long
        Dim lStartCorner As Long
        Dim sJavaFile As String
        Dim nSeqOpt As Long

        Try

            chkSubRoutines.Checked = IIf(RegGetString("CodeGeneration", "DoSubs", "0") = 1, True, False)

            cbfOptimizeToolpath.Checked = IIf(RegGetString("CodeGeneration", "OptimizeToolpath", "0") = 1, True, False)


            lOptimizeMode = RegGetString("CodeGeneration", "OptimizeType", CStr(GRID_MODE))

            mtbStartPointX.Text = RegGetString("CodeGeneration", "StartX", 0)
            mtbStartPointY.Text = RegGetString("CodeGeneration", "StartY", 0)

            'The radio button order is Grid/Closest/Progressive
            If (lOptimizeMode = PROGRESSIVE_MODE) Then

                rgOptMode.SelectedIndex = 2

            ElseIf (lOptimizeMode = CLOSEST_PT_MODE) Then

                rgOptMode.SelectedIndex = 1

            Else
                rgOptMode.SelectedIndex = 0

            End If

            lStartCorner = RegGetString("CodeGeneration", "StartCorner", "0")
            If lStartCorner < 0 Or lStartCorner > 7 Then

                btnSlice1.Checked = True

            Else
                Select Case lStartCorner
                    Case 0
                        btnSlice1.Checked = True
                    Case 1
                        btnSlice2.Checked = True
                    Case 2
                        btnSlice3.Checked = True
                    Case 3
                        btnSlice4.Checked = True
                    Case 4
                        btnSlice5.Checked = True
                    Case 5
                        btnSlice6.Checked = True
                    Case 6
                        btnSlice7.Checked = True
                    Case 7
                        btnSlice8.Checked = True
                End Select
            End If

            txtSticky.Text = RegGetString("CodeGeneration", "Sticky", "1")


            nSeqOpt = RegGetString("CodeGeneration", "GridSequence", "0")
            lcbGridSequence.SelectedIndex = nSeqOpt


            If (nSeqOpt = 2) Then
                Me.cmdFlowchart.Visible = True
            Else
                Me.cmdFlowchart.Visible = False
            End If

            chkZigZag.Checked = IIf(RegGetString("CodeGeneration", "ZigZag", "1") = 1, True, False)

            frmFlowchart.FlowChartInitMode(0)

            sJavaFile = PathCG()

            fcbCodeGenFile.Text = TrimToFit(sJavaFile, 35)
            fcbCodeGenFile.Tag = sJavaFile

            If FileExists(sJavaFile) Then

                LoadVariables(sJavaFile, "cg")
            End If


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    Private Sub fcbExportTo_ButtonClick(sender As Object, e As DevExpress.XtraEditors.Controls.ButtonPressedEventArgs) Handles fcbExportTo.ButtonClick
        Dim edit As DevExpress.XtraEditors.ButtonEdit = TryCast(sender, DevExpress.XtraEditors.ButtonEdit)
        Dim sFilename As String
        Dim dialog As New SaveFileDialog
        Dim nFilterIndex As Integer
        Dim DefaultExtension As String
        Dim sFilter As String = sEmpty
        Dim ni As Integer
        Dim nExtCount As Integer


        Try

            nExtCount = lstExtensions.ItemCount

            For ni = 0 To nExtCount - 1

                If (ni = nExtCount - 1) Then
                    sFilter = sFilter & lstExtensions.Items(ni).ToString
                Else
                    sFilter = sFilter & lstExtensions.Items(ni).ToString & "|"

                End If


            Next


            DefaultExtension = RegGetString("CodeGeneration", "NCFileExt", "*.*")

            If InStr(1, DefaultExtension, "*.") = 0 Then
                DefaultExtension = "*." & DefaultExtension
            End If

            dialog.Title = "Export to file"
            dialog.Filter = sFilter
            nFilterIndex = GetExtensionIndex(DefaultExtension)
            dialog.FilterIndex = nFilterIndex
            dialog.FileName = fcbExportTo.Tag
            dialog.InitialDirectory = PathDriveFolder(fcbExportTo.Tag)
            If (dialog.FilterIndex = 3) Then

            End If
            If dialog.ShowDialog(Me) = DialogResult.OK Then
                nFilterIndex = dialog.FilterIndex

                If (nFilterIndex = 3) Then

                    fcbExportTo.Text = PathRemoveExt(dialog.FileName)
                Else
                    fcbExportTo.Text = dialog.FileName
                End If

                sFilename = fcbExportTo.Text
                fcbExportTo.Tag = sFilename
            End If



        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    Private Function GetExtensionIndex(CNCExtensionText As String) As Long

        Dim nI As Integer

        GetExtensionIndex = 1

        Try

            For nI = 0 To (lstExtensions.ItemCount - 1)
                If (InStr(1, lstExtensions.Items(nI), CNCExtensionText) > 0) Then
                    'Add 1 because the common dialog filter index is 1-based(?)
                    GetExtensionIndex = nI + 1
                    Exit For
                End If
            Next

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Function

    'NOTE: This monkey-stuff is here to manage ncfile defaulting,
    'which can get screwed up when moving between the Code Viewer
    'and File/Export.
    Private Sub InitExportTo()

        Dim NCFile As String
        Dim mm2file As String
        Dim root_path As String
        Dim file_name As String
        Dim file_ext As String
        Dim fileType As Long

        Try

            NCFile = HeaderStringGet("ncfile", "")
            If (NCFile = "") Then

                mm2file = PathCurrent()
                fileType = modPath.FileTypeGet(PathExt(mm2file))

                'Default the parameters based on the MRU.
                root_path = NormalizePath(PathDriveFolder(PathCNC()))
                file_ext = PathExt(PathCNC())

                Select Case fileType
                    Case FILE_MM2
                        file_name = PathFile(PathMM2())

                    Case FILE_NST
                        file_name = PathFile(PathNST())

                    Case Else
                        file_name = "untitled"

                End Select

                If (Len(file_ext) <= 0) Then
                    NCFile = root_path & file_name & "."
                Else
                    NCFile = root_path & file_name & "." & file_ext
                End If


                m_ncfile = ""

            Else

                m_ncfile = NCFile

            End If
            fcbExportTo.Text = NCFile

            fcbExportTo.Tag = NCFile

            'Make sure our default extension is in the list
            RegPutString("CodeGeneration\CNCExtensions", "cnc", "cnc")

            'TODO V21 may not need the following with the new implementation
            ''reset the filter index to the same as the current filename
            'If (fcbExportTo.Tag <> "") Then
            '    fcbExportTo.FilterIndex = GetExtensionIndex(PathExt(fcbExportTo.Filename))
            'End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    Private Sub cbfOptimizeToolpath_Click(Sender As Object, e As EventArgs) Handles cbfOptimizeToolpath.Click
        If (m_context = CODE_OPTION.SEQUENCE_ORDER And cbfOptimizeToolpath.Checked = False) Then
            cbfOptimizeToolpath.Checked = True
        End If

        EnableOrDisable()

    End Sub

    Private Sub cmdCNCFileExtensions_Click(sender As Object, e As EventArgs) Handles cmdCNCFileExtensions.Click
        frmExtensions.ShowDialog(Me)

        GetCNCExtensions()
    End Sub

    Private Sub cmdOk_Click(sender As Object, e As EventArgs) Handles cmdOk.Click

        OKButtonClick()

    End Sub
    Public Sub OKButtonClick()
        Dim The_Shift_Value As Long
        Dim s_Export_Path As String
        'Dim stempfilename As System.IO.StreamWriter

        Try

            s_Export_Path = NormalizePath(PathDriveFolder(fcbExportTo.Tag))

            If (m_context = CODE_OPTION.CODE_FILE Or _
                m_context = CODE_OPTION.CODE_VIEWER Or _
                m_context = CODE_OPTION.CCSFAB_CODE_VIEWER Or _
                m_context = CODE_OPTION.CCSFAB Or _
                m_context = CODE_OPTION.DYNATORCH_FILE Or _
                m_context = CODE_OPTION.DYNATORCH_VIEWER Or _
                m_context = CODE_OPTION.AUTO_CODE_NEST) Then

                If (FileExists(fcbCodeGenFile.Tag) = True) Then

                    'Set the current values as defaults in the reg
                    SetDefaultsInReg()
                    SetValuesInHeader()

                    If (m_context = CODE_OPTION.CODE_FILE Or m_context = CODE_OPTION.DYNATORCH_FILE) Then
                        If (PartsAreZoned() = False) Then Exit Sub
                    End If

                    Windows.Forms.Cursor.Current = Cursors.WaitCursor

                    If (m_context = CODE_OPTION.CCSFAB) Then
                        'This stores the sift value of the selected node in the tree.
                        'Used just to keep code cleaner
                        'The_Shift_Value = frmCcsFabNesting.tlsNestDbs.Shift(frmCcsFabNesting.tlsNestDbs.ListIndex)

                        The_Shift_Value = frmNesting.TV_NestConfig.FocusedNode.Level



                        If (The_Shift_Value = 0) Then 'We ar coding all the pdbs so create a master file

                            fcbExportTo.Tag = PathForceExt(frmNesting.fcbXlsName.Text, ".CNC")

                        ElseIf (The_Shift_Value = 1) Then 'We are trying to code a single PDB

                            sPieces = Split(frmNesting.TV_NestConfig.FocusedNode.Tag, ",")

                            fcbExportTo.Tag = PathForceExt(sPieces(1), ".CNC")

                        ElseIf (The_Shift_Value = 3) Then 'We are trying to code a single sheet

                            fcbExportTo.Tag = PathForceExt(PathCurrent(), ".CNC")

                        End If


                    End If

                    FormatPortalCommand()

                    Select Case nOptalgorithm
                        Case GRID_MODE

                            lReturn = HeaderIntSet("OptAlgorithm", GRID_MODE)

                        Case CLOSEST_PT_MODE

                            lReturn = HeaderIntSet("OptAlgorithm", CLOSEST_PT_MODE)

                        Case PROGRESSIVE_MODE

                            lReturn = HeaderIntSet("OptAlgorithm", PROGRESSIVE_MODE)

                    End Select

                    Select Case m_context
                        Case CODE_OPTION.CODE_FILE
#If DEMO_VERSION Then

                    MsgBox "This feature is disabled in the Demo Version!", vbExclamation

#Else
#If DIAMOND Then
                            'WE always overwrite the fie
#Else

                            If (FileExists(fcbExportTo.Tag) = True) Then

                                lReturn = MsgBox("Overwrite this existing file?" & vbCrLf & _
                                    fcbExportTo.Tag, vbYesNo)

                                If (lReturn = vbNo) Then
                                    Windows.Forms.Cursor.Current = Cursors.Default
                                    Exit Sub
                                End If

                            End If
#End If

                            'check to make sure the path exists. This is because
                            'the path stored in the mm2 file may no longer exist
                            If (FolderExists(s_Export_Path) = False) Then

                                MsgBox("The folder specified no longer exists. Process terminated!", vbCritical)
                                Exit Sub

                            End If

                            Me.Hide()

                            'Dim Message, Title, sDefaultVAl, MyValue
                            'Message = "Potal Command"    ' Set prompt.
                            'Title = "FOrmatted Portal Command"    ' Set title.
                            'sDefaultVAl = m_sPortalCommand    ' Set default.
                            '' Display message, title, and default value.
                            'MyValue = InputBox(Message, Title, sDefaultVAl)


                            'MessageBox.Show("The Command is < " & m_sPortalCommand & " >")

                            lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!
                            lReturn = PortalExecute(m_sPortalCommand)

                            lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!

                            m_code_gen = fcbCodeGenFile.Tag

                            lReturn = HeaderStringSet("ncfile", fcbExportTo.Tag)
#If DIAMOND Then
                            'we don't need to show the dialog
#Else


                            Me.Show()
#End If


                            MsgBox("Code Generation Process Completed.", vbOKOnly)

                            'We save the model so as to make the values (we have written
                            'to the header) available to report printing.  This is especially
                            'important because the model is reloaded in order to create the
                            'WMF (formerly this caused the values to be lost).
                            lReturn = modFileIO.ExportMM2(PathCurrent())




#End If
                        Case CODE_OPTION.FROM_NESTING
                            m_code_gen = fcbCodeGenFile.Tag
                            frmNesting.SetPortalCommand(m_sPortalCommand)

                            frmNesting.UpdateButtons(m_code_gen)

                        Case CODE_OPTION.CODE_VIEWER

                            m_code_gen = fcbCodeGenFile.Tag
                            frmGraphicsEditor.SetPortalCommand(m_sPortalCommand)

                            frmGraphicsEditor.UpdateButtons(m_code_gen)

                        Case CODE_OPTION.CCSFAB_CODE_VIEWER
                            m_code_gen = fcbCodeGenFile.Tag

                            'TODO V21
                            'frmCcsFabNesting.UpdateButtons m_code_gen

                        Case CODE_OPTION.CCSFAB
                            'MsgBox("We should be processing CCSFAB")
                            lReturn = Process_CCSFAB_CODEGEN(m_sPortalCommand)

                        Case CODE_OPTION.DYNATORCH_FILE
                            'TODO v21
                            ' lReturn = Process_DYNATORCH_CODEGEN(m_sPortalCommand)

                        Case CODE_OPTION.AUTO_CODE_NEST
                            lReturn = Process_Auto_Code_Nest(m_sPortalCommand)

                    End Select


                    Windows.Forms.Cursor.Current = Cursors.Default

                    'Me.Dispose()

                Else

                    MsgBox("The code generator " & fcbCodeGenFile.Tag & " does not exist." & _
                            vbCrLf & "Select another code generator.")

                End If

            ElseIf (m_context = CODE_OPTION.SEQUENCE_ORDER) Then

                Windows.Forms.Cursor.Current = Cursors.WaitCursor

                'Set the current values as defaults in the reg
                SetDefaultsInReg()
                SetValuesInHeader()

                FormatPortalCommand()
                lReturn = PortalExecute(m_sPortalCommand)


                Windows.Forms.Cursor.Current = Cursors.Default

                m_code_gen = ""
                Me.Dispose()

                Me.Close()

            End If

        Catch ex As Exception

            MessageBox.Show("cmdOK_Click: " & ex.Message)

        End Try
    End Sub


    Private Sub FormatPortalCommand()
        Dim nAdvanced As Long
        Dim nSticky As Long
        Dim nBiDir As Long
        Dim nSeq_Opt As Long
        Dim nAllHolesFirst As Long
        Dim nHolesByToolOrder As Long
        Dim nByCompletePart As Long
        Dim nHolesAcrossLocalNest As Long
        Dim nSubs As Long
        Dim sNCFile As String
        Dim sCGFile As String
        Dim sPDBFile As String
        Dim sNCFilePath As String
        Dim xs As String
        Dim ys As String
        Dim SelectCount As Long

        Try

            'Get the CNC Filepath
            sNCFilePath = PathDriveFolder(fcbExportTo.Tag)

            'Get The Java file
            sCGFile = fcbCodeGenFile.Tag

            'This is required by C++ to indicate validity.
            lReturn = HeaderStringSet("cgfile", sCGFile)
            lReturn = HeaderStringSet("mac.nestdxf", PathReplaceExt(PathCurrent(), "dxf"))

            'Get the PDB filename if we are coding across sheets from Nesting
            'This was previously 'sPDBFile = sPDBFilename' but will not
            'compile when using 'Option Explicit'.
            sPDBFile = PathPDB()

            'Get the fully qualified path and filename of the CNC file
            sNCFile = fcbExportTo.Tag


            If (cbfOptimizeToolpath.Checked = False) Then
                nOptalgorithm = 0
            Else
                'The radio button order is Grid/Closest/Progressive
                If (rgOptMode.SelectedIndex = 0) Then
                    nOptalgorithm = GRID_MODE
                ElseIf (rgOptMode.SelectedIndex = 1) Then
                    nOptalgorithm = CLOSEST_PT_MODE
                Else
                    nOptalgorithm = PROGRESSIVE_MODE
                End If
            End If

            'Set The linear Trend Value
            nSticky = CInt(txtSticky.Text)

            'Set whether we should Zig Zag or not
            nBiDir = IIf(chkZigZag.Checked = True, 1, 0)

            'Set the Sequence By mode
            nSeq_Opt = RegGetString("CodeGeneration", "GridSequence", "0")

            'Set whether we do all holes first or not
            nAllHolesFirst = RegGetString("CodeGeneration", "AllHolesFirst", "0")

            'Set if we should be doing Holes By Tool Order or not
            nHolesByToolOrder = RegGetString("CodeGeneration", "HolesByToolOrder", "0")

            'Set if we should do  By Complete Part OR not
            nByCompletePart = RegGetString("CodeGeneration", "ByCompletePart", "0")

            'Set If we are doin Holes Across Local Nest  or not
            nHolesAcrossLocalNest = RegGetString("CodeGeneration", "HolesAcrossLocalNest", "0")

            'Set whether we are doing subs or not
            nSubs = RegGetString("CodeGeneration", "DoSubs", "0")

            Select Case m_context
                Case CODE_OPTION.CODE_FILE, CODE_OPTION.DYNATORCH_FILE
                    'Set the Java file name
                    m_sPortalCommand = "codegen:generate:cgfile=" & QStr(sCGFile)

                    'This is not used when using Viewer or Sequence Order
                    m_sPortalCommand = m_sPortalCommand & ",ncfile=" & QStr(sNCFile)

                    m_sPortalCommand = m_sPortalCommand & ",Subs=" & nSubs
                    m_sPortalCommand = m_sPortalCommand & ",adv=" & nAdvanced

                Case CODE_OPTION.AUTO_CODE_NEST
                    'Set the Java file name
                    m_sPortalCommand = "codegen:generate:cgfile=" & QStr(sCGFile)
                    m_sPortalCommand = m_sPortalCommand & ",Subs=" & nSubs
                    m_sPortalCommand = m_sPortalCommand & ",adv=" & nAdvanced


                Case CODE_OPTION.CODE_VIEWER, CODE_OPTION.CCSFAB_CODE_VIEWER

                    'Set the Java file name
                    m_sPortalCommand = "codegen:generate:cgfile=" & QStr(sCGFile)

                    m_sPortalCommand = m_sPortalCommand & ",Subs=" & nSubs
                    m_sPortalCommand = m_sPortalCommand & ",adv=" & nAdvanced

                Case CODE_OPTION.SEQUENCE_ORDER

                    SelectCount = SelectedCount()

                    Dim nMyIndex As Integer = frmSeqMove.tl_SeqOrder.GetNodeIndex(frmSeqMove.tl_SeqOrder.FocusedNode)

                    'TODO V21
                    If (m_SeqOrderWorkZoneID = 0) Then

                        '    'The root node is selected, so optimize the entire model.
                        m_sPortalCommand = "CodeGen:Optimize:adv=" & nAdvanced


                    Else
                        'm_SeqOrderWorkZoneID = 496

                        '    'We are going to optimize this work-zone.
                        m_sPortalCommand = "CodeGen:Optimize:id=" & m_SeqOrderWorkZoneID

                        If SelectCount > 0 Then
                            'We are going to optimize only the selected entities in this work-zone.
                            m_sPortalCommand = m_sPortalCommand & ",mode=" & m_seq_order_mode

                        End If
                        m_sPortalCommand = m_sPortalCommand & ",adv=" & nAdvanced

                    End If

                Case CODE_OPTION.CCSFAB 'Special Nesting Mode
                    'Set the Java file name
                    m_sPortalCommand = "codegen:generate:cgfile=" & QStr(sCGFile)

                    'This is not used when using Viewer or Sequence Order
                    m_sPortalCommand = m_sPortalCommand & ",ncfile=" & QStr(sNCFile)
                    m_sPortalCommand = m_sPortalCommand & ",Subs=" & nSubs

                    m_sPortalCommand = m_sPortalCommand & ",adv=" & nAdvanced
                    m_sPortalCommand = m_sPortalCommand & ",pdb=" & QStr(sPDBFile)

            End Select

            If ((nOptalgorithm >= GRID_MODE) And (nOptalgorithm <= CLOSEST_PT_MODE)) Then

                'NOTE: Slice_Opt, Sticky & BiDir are relevant to GRID_MODE only (?)
                'but we encorporate them into the portal command just in case ....
                m_sPortalCommand = m_sPortalCommand & ",Optalgorithm=" & nOptalgorithm
                m_sPortalCommand = m_sPortalCommand & ",Slice_Opt=" & nSlice_Opt

                'MsgBox("The direction icon selected is < " & nSlice_Opt & " >")
                m_sPortalCommand = m_sPortalCommand & ",Sticky=" & nSticky
                m_sPortalCommand = m_sPortalCommand & ",BiDir=" & nBiDir
                m_sPortalCommand = m_sPortalCommand & ",Seq_Opt=" & nSeq_Opt

                If (nOptalgorithm = CLOSEST_PT_MODE) Then
                    xs = mtbStartPointX.Text()
                    ys = mtbStartPointY.Text()
                    If ((Len(xs) > 0) And (Len(ys) > 0)) Then
                        '2006.11.19 (PE) -- Fortunately, it appears the c2dStartPt
                        'control prevents us from getting complete rubbish input.
                        m_sPortalCommand = m_sPortalCommand & ",xs=" & xs
                        m_sPortalCommand = m_sPortalCommand & ",ys=" & ys
                    End If
                End If

                '(0) Preset Tool Order / (1) Containment Order / (2) Process Flowchart
                If (nSeq_Opt = 2) Then
                    m_sPortalCommand = m_sPortalCommand & ",AllHolesFirst=" & nAllHolesFirst
                    m_sPortalCommand = m_sPortalCommand & ",HolesByToolOrder=" & nHolesByToolOrder
                    m_sPortalCommand = m_sPortalCommand & ",ByCompletePart=" & nByCompletePart
                    m_sPortalCommand = m_sPortalCommand & ",HolesAcrossLocalNest=" & nHolesAcrossLocalNest
                End If

            End If

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub
    Private Sub SetDefaultsInReg()

        Try
            'MsgBox("WECREATE are setting values in the reg")

            RegPutString("CodeGeneration", "DoSubs", IIf(chkSubRoutines.Checked = True, 1, 0))
            RegPutString("CodeGeneration", "NCFileExt", PathExt(fcbExportTo.Tag))

            RegPutString("CodeGeneration", "OptimizeToolpath", IIf(cbfOptimizeToolpath.Checked = True, 1, 0))

            'The radio button order is Grid/Closest/Progressive
            If (rgOptMode.SelectedIndex = 0) Then
                RegPutString("CodeGeneration", "OptimizeType", CStr(GRID_MODE))
            ElseIf (rgOptMode.SelectedIndex = 1) Then
                RegPutString("CodeGeneration", "OptimizeType", CStr(CLOSEST_PT_MODE))
            Else
                RegPutString("CodeGeneration", "OptimizeType", CStr(PROGRESSIVE_MODE))
            End If

            RegPutString("CodeGeneration", "StartCorner", CStr(nSlice_Opt))

            RegPutString("CodeGeneration", "Sticky", txtSticky.Text)
            RegPutString("CodeGeneration", "GridSequence", lcbGridSequence.SelectedIndex)
            RegPutString("CodeGeneration", "ZigZag", IIf(chkZigZag.Checked = True, 1, 0))
            If (mtbStartPointX.Text <> "") Then
                RegPutString("CodeGeneration", "StartX", mtbStartPointX.Text)
                RegPutString("CodeGeneration", "StartY", mtbStartPointY.Text)
            End If

            PathCG_Set(fcbCodeGenFile.Tag)
            PathCNC_Set(fcbExportTo.Tag)


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub
    Private Sub SaveVariables()
        Dim sVarName As String
        Dim sData As String

        Try

            For Each row As CategoryRow In vgCodeGenVars.Rows

                For Each childRow As EditorRow In row.ChildRows
                    sVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")
                    sData = childRow.Properties.Value

                    lReturn = HeaderStringSet(sVarName, sData)

                Next
            Next

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub
    Private Sub SetValuesInHeader()

        Try

            'CRITICAL! Record the Java Filename
            lReturn = HeaderStringSet("cgfile", fcbCodeGenFile.Tag)

            'Record the NC Filename
            If (m_context = CODE_OPTION.CODE_FILE) Then
                lReturn = HeaderStringSet("ncfile", fcbExportTo.Tag)
            Else
                'Commentted out the following line because if the user
                'Went to Code Viewer just to check the code
                'This line overwrites the cnc filename stored in the header
                'By overwriting the var, the user would have to recode the file
                'in order to get the var set correcttly again
                'Overwriting the var in the viewer screws up printing
                'lReturn = HeaderStringSet("ncfile", "")
            End If

            'Record the checkbox for subs
            lReturn = HeaderIntSet("subs", If(chkSubRoutines.Checked = True, 1, 0))

            'Record the checkbox for optimizing the toolpath
            lReturn = HeaderIntSet("opttoolpath", IIf(cbfOptimizeToolpath.Checked = True, 1, 0))

            'Record the optimization mode.
            'The radio button order is Grid/Closest/Progressive
            If (rgOptMode.SelectedIndex = 0) Then
                lReturn = HeaderIntSet("OptAlgorithm", GRID_MODE)
            ElseIf (rgOptMode.SelectedIndex = 1) Then
                lReturn = HeaderIntSet("OptAlgorithm", CLOSEST_PT_MODE)
            Else
                lReturn = HeaderIntSet("OptAlgorithm", PROGRESSIVE_MODE)
            End If


            'TODO V21
            ''Record the start corner and direction
            'For nI = 1 To Toolbar1.buttons.Count
            '    If (Toolbar1.buttons(nI).Value = tbrPressed) Then
            '        lReturn = HeaderIntSet("slice_opt", (nI - 1))
            '    End If
            'Next

            'Record the Linear Trend Value
            lReturn = HeaderIntSet("Sticky", CLng(txtSticky.Text))

            'Record Sequence By Mode (Preset Tool Order,Containment Order,Process Flowchart)
            lReturn = HeaderIntSet("seq_opt", lcbGridSequence.SelectedIndex)

            'Record Zig Zag Checkbox
            lReturn = HeaderIntSet("bidir", IIf(chkZigZag.Checked = True, 1, 0))

            'NOTE: It is CRITCAL that SetDefaultsInReg() is called prior to these.
            'The alternate is to add quary functions to frmFlowChart.

            lReturn = HeaderIntSet("AllHolesFirst", _
                RegGetString("CodeGeneration", "AllHolesFirst", "0"))

            lReturn = HeaderIntSet("HolesByToolOrder", _
                RegGetString("CodeGeneration", "HolesByToolOrder", "0"))

            lReturn = HeaderIntSet("ByCompletePart", _
                RegGetString("CodeGeneration", "ByCompletePart", "0"))

            lReturn = HeaderIntSet("HolesAcrossLocalNest", _
                RegGetString("CodeGeneration", "HolesAcrossLocalNest", "0"))

            If (m_context <> CODE_OPTION.SEQUENCE_ORDER) Then

                'Save the codegen Vars to the header
                SaveVariables()

            End If


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub
    Private Function CodeAllThePDBS(sPortalCommand As String) As Long
        Dim pdb_name As String = Nothing
        Dim Out_File_Name As String
        Dim Label_DB_Name As String
        Dim Tree_Count As Long
        Dim i As Long
        Dim Temp_Str As String = Nothing
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim sql As String
        Dim Label_Txt_File As String
        Dim output_type As Integer
        Dim sFileName As String = Nothing
        Dim nParentNode As TreeListNode

        Try
            If (IS_CCS_FAB = True) Then
                Label_Txt_File = NormalizePath(PathDriveFolder(frmNesting.fcbXlsName.EditValue)) & "labeldb.lst"
            Else
                Label_Txt_File = NormalizePath(Application.StartupPath) & "nests\labeldb.lst"
            End If


            'Clean up label text temp file if needed
            If (FileExists(Label_Txt_File) = True) Then

                If (m_debug_code) Then MessageBox.Show("We are deleteing the label list file")

                ConditionalFileDelete(Label_Txt_File)

            End If

            CodeAllThePDBS = -1


            nParentNode = frmNesting.TV_NestConfig.Nodes(0)


            If (IS_CCS_FAB = True) Then
                Out_File_Name = NormalizePath(PathDriveFolder(frmNesting.fcbXlsName.Text)) & "CodeGen_List.txt"
            Else
                Out_File_Name = NormalizePath(Application.StartupPath) & "nests\CodeGen_List.txt"
            End If

            If (IS_CCS_FAB = True) Then
                Label_DB_Name = NormalizePath(PathDriveFolder(frmNesting.fcbXlsName.Text)) & "labeldb.mdb"
            Else
                Label_DB_Name = NormalizePath(Application.StartupPath) & "nests\labeldb.mdb"
            End If

            If (FileExists(Out_File_Name) = True) Then
                Kill(Out_File_Name)
            End If

            For Each childnode In nParentNode.Nodes

                If childnode.level = 1 Then

                    frmNesting.TV_NestConfig.FocusedNode = childnode



                    ' fStream = New FileStream(Name, FileMode.Append, FileAccess.Write, FileShare.Write)







                    Tree_Count = frmNesting.TV_NestConfig.Selection.Count

                    'While nIndex < Tree_Count
                    ' If (frmNesting.TV_NestConfig.Selection.Item(nIndex).Level = 1) Then

                    'Build the nest for the current pdb
                    ' nSheet_Index = nIndex + 2
                    sPieces = Split(frmNesting.TV_NestConfig.FocusedNode.Tag, ",")

                    pdb_name = sPieces(1)

                    Label_DB_Name = PathDriveFolder(pdb_name) & "\LabelDb.mdb"


                    m_PDB_Code_counter = 0

                    connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & pdb_name & "';"

                    sql = "SELECT * FROM [Sheet Result]"

                    connection = New OleDbConnection(connetionString)


                    ds = New DataSet

                    connection.Open()

                    oledbAdapter = New OleDbDataAdapter(sql, connection)

                    oledbAdapter.Fill(ds)

                    oledbAdapter.Dispose()

                    connection.Close()

                    If connection IsNot Nothing Then connection = Nothing

                    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing


                    If (ds.Tables(0).Rows.Count > 0) Then
                        For i = 0 To ds.Tables(0).Rows.Count - 1
                            sFileName = ds.Tables(0).Rows(i).Item("Sheet Filename")

                            lReturn = PortalExecute("admin:new:")

                            lReturn = PortalExecute("file:importmm2: file=" & QStr(sFileName))

                            SetValuesInHeader()

                            lReturn = WRITE_PDB_NAME_TO_FILE(pdb_name & "|" & ds.Tables(0).Rows(i).Item("ID"), Out_File_Name)

                            lReturn = PortalExecute("file:exportmm2: file=" & QStr(sFileName))
                        Next
                    End If

                    'oledbAdapter.Dispose()
                    ds.Dispose()

                    If connection IsNot Nothing Then connection = Nothing

                    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                    If ds IsNot Nothing Then ds = Nothing

                End If

            Next

            If (m_debug_code) Then MsgBox("Stop to check codegen list file")
            If (chkCreateMasterFile.Checked = True) Then

                output_type = 1

            Else

                output_type = 2

            End If


            sPortalCommand = Replace(sPortalCommand, "ncfile=" & QStr(".CNC"), "ncfile=" & QStr(PathReplaceExt(sFileName, "cnc")))


            m_sPortalCommand = sPortalCommand & ",ccs=" & output_type & ", pdb=" & QStr(Out_File_Name) & ",label_db=" & QStr(Label_Txt_File)

            If (m_debug_code) Then MessageBox.Show("Portal Cmd <" & m_sPortalCommand & ">")

            lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!
            lReturn = PortalExecute(m_sPortalCommand)
            lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!

            ''TODO V21
            '' frmCcsFabNesting.UpdateButtons m_code_gen

            If (m_debug_code) Then MessageBox.Show(" The temp label file name is " & Label_Txt_File & " ands the label mdb filename is " & Label_DB_Name)

            lReturn = Create_LabelMDB_From_Label_List(Label_Txt_File)

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Function
    Private Function Process_Auto_Code_Nest(sPortalCommand As String) As Long
        Dim dsSheets As DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim SheetFileName As String
        Dim cnt As Integer
        Dim i As Integer
        Dim sql As String
        Dim s_sheet_filename As String
        Dim pdb_name As String


        pdb_name = RegPutString("Fabrication\FileOpen", "LastPDB", "") 'HeaderStringGet("pdb_file", "")

        If (DoesTableExist("Sheet Result", pdb_name) = True) Then

            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & pdb_name & "';"

            sql = "SELECT * FROM [Sheet Result]"

            connection = New OleDbConnection(connetionString)

            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            dsSheets = New DataSet

            oledbAdapter.Fill(dsSheets)

            oledbAdapter.Dispose()

            connection.Close()

            If connection IsNot Nothing Then connection = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            cnt = dsSheets.Tables(0).Rows.Count

            If (cnt > 0) Then
                For i = 0 To cnt - 1
                    SheetFileName = dsSheets.Tables(0).Rows(i).Item("Sheet Filename")

                    s_sheet_filename = modFileIO.ImportMM2(SheetFileName)

                    'If (s_sheet_filename <> "") Then
                    '    fPicModelerForm.PrepareUI(s_sheet_filename)
                    '    lReturn = modToolbar.EnableAllToolbars(True)
                    '    lReturn = modToolbar.EnableAllMenus(True)
                    'End If


                    sPortalCommand = ""

                    FormatPortalCommand()

                    m_sPortalCommand = sPortalCommand & ",ncfile=" & QStr(PathRemoveExt(s_sheet_filename) + "." + RegGetString("CodeGeneration", "NCFileExt", "*.*"))

                    lReturn = modFileIO.ExportMM2(s_sheet_filename)

                    lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!
                    lReturn = PortalExecute(m_sPortalCommand)
                    If (m_debug_code) Then MsgBox("the return value is " + CStr(lReturn), vbOKOnly)
                    'MsgBox "The portal is " & m_sPortalCommand
                    lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!

                    If (m_debug_code) Then MsgBox(m_sPortalCommand, vbOKOnly)

                Next
            End If

            dsSheets.Dispose()
            If dsSheets IsNot Nothing Then dsSheets = Nothing


            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        End If



    End Function

    Private Function Process_CCSFAB_CODEGEN(sPortalCommand As String) As Long
        'There are 3 cases we need to check for
        'A) They have selected the main node which means they want to code all the pdbs and sheets
        'B) They have a pdb selected either by itself or in a multi select use case
        'C) The have a single sheet selected either by itself or in a multi select case
        Dim The_Shift_Value As Long
        Dim pdb_name As String = Nothing
        Dim Out_File_Name As String
        Dim Label_DB_Name As String
        Dim nIndex As Long
        Dim Tree_Count As Long
        Dim i As Long
        Dim Temp_Str As String = Nothing
        Dim nSheet_Index As Long
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim sql As String
        Dim Label_Txt_File As String
        Dim output_type As Integer
        Dim sFileName As String

        Try


            m_PDB_Code_counter = 0

            If (frmNesting.TV_NestConfig.Selection.Count = 0) Then
                MsgBox("No files have been selected. Coding process will be terminated!", vbCritical)
                Exit Function
            End If

            PathTempFolderCreate()

            'This stores the sift value of the selected node in the tree.
            'Used just to keep code cleaner
            The_Shift_Value = frmNesting.TV_NestConfig.Selection.Item(0).Level

            Process_CCSFAB_CODEGEN = 0

            If (The_Shift_Value) = 0 And frmNesting.TV_NestConfig.Nodes(0).HasChildren = True Then 'We are trying to code all of the pdbs
                If (m_debug_code) Then MessageBox.Show("WE should code them all ")

                CodeAllThePDBS(sPortalCommand)

                modAdmin.ModelDirtyClear()

                MsgBox("Code Generation Process Completed.", vbOKOnly)

            Else

                If (IS_CCS_FAB = True) Then
                    Out_File_Name = NormalizePath(PathDriveFolder(PathPDB())) & "CodeGen_List.txt"
                Else
                    Out_File_Name = NormalizePath(Application.StartupPath) & "nests\CodeGen_List.txt"
                End If


                If (FileExists(Out_File_Name) = True) Then
                    Kill(Out_File_Name)
                End If

                fStream = New FileStream(Name, FileMode.Append, FileAccess.Write, FileShare.Write)

                If (IS_CCS_FAB = True) Then
                    Label_Txt_File = NormalizePath(PathDriveFolder(PathPDB())) & "labeldb.lst"
                Else
                    Label_Txt_File = NormalizePath(Application.StartupPath) & "nests\labeldb.lst"
                End If


                'Clean up label text temp file if needed
                If (FileExists(Label_Txt_File) = True) Then

                    If (m_debug_code) Then MessageBox.Show("We are deleteing the label list file")

                    ConditionalFileDelete(Label_Txt_File)

                End If


                If (IS_CCS_FAB = True) Then
                    Label_DB_Name = NormalizePath(PathDriveFolder(PathPDB())) & "labeldb.mdb"
                Else
                    Label_DB_Name = NormalizePath(Application.StartupPath) & "nests\labeldb.mdb"
                End If


                Tree_Count = frmNesting.TV_NestConfig.Selection.Count

                While nIndex < Tree_Count
                    If (frmNesting.TV_NestConfig.Selection.Item(nIndex).Level = 1) Then

                        'Build the nest for the current pdb
                        nSheet_Index = nIndex + 2
                        sPieces = Split(frmNesting.TV_NestConfig.Selection.Item(nIndex).Tag, ",")

                        pdb_name = sPieces(1)

                        Label_DB_Name = PathDriveFolder(pdb_name) & "\LabelDb.mdb"


                        m_PDB_Code_counter = 0

                        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & pdb_name & "';"

                        sql = "SELECT * FROM [Sheet Result]"

                        connection = New OleDbConnection(connetionString)

                        Try
                            connection.Open()

                            oledbAdapter = New OleDbDataAdapter(sql, connection)

                            oledbAdapter.Fill(ds)

                            oledbAdapter.Dispose()

                            connection.Close()

                            If connection IsNot Nothing Then connection = Nothing
                            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                            If (ds.Tables(0).Rows.Count > 0) Then
                                For i = 0 To ds.Tables(0).Rows.Count - 1
                                    sFileName = ds.Tables(0).Rows(i).Item("Sheet Filename")

                                    lReturn = PortalExecute("admin:new:")

                                    lReturn = PortalExecute("file:importmm2: file=" & QStr(sFileName))

                                    SetValuesInHeader()

                                    lReturn = WRITE_PDB_NAME_TO_FILE(pdb_name & "|" & ds.Tables(0).Rows(i).Item("ID"), Out_File_Name)

                                    lReturn = PortalExecute("file:exportmm2: file=" & QStr(sFileName))
                                Next
                            End If

                            ds.Dispose()

                            If ds IsNot Nothing Then ds = Nothing

                        Catch
                        End Try

                    End If

                    nIndex = nIndex + 1

                End While

                'Clean up labeldb temp file if needed
                If (FileExists(Label_Txt_File) = True) Then

                    ConditionalFileDelete(Label_Txt_File)

                End If


                If (m_debug_code) Then MsgBox("We are coding them all")

                'We either have multiple files selected or
                'we have either a PDB or NST File selected
                'So we need to check which it is

                If (IS_CCS_FAB = True) Then
                    Label_DB_Name = NormalizePath(PathDriveFolder(PathPDB())) & "labeldb.mdb"
                Else
                    'Label_DB_Name = NormalizePath(PathTempFolder()) & "labeldb.mdb" pdb_name
                    Label_DB_Name = NormalizePath(PathDriveFolder(pdb_name)) & "labeldb.mdb"
                End If

                For i = 0 To frmNesting.TV_NestConfig.Selection.Count - 1

                    'Reset the shift value for the selected item
                    The_Shift_Value = frmNesting.TV_NestConfig.Nodes.Item(i).Level
                    'The_Shift_Value = frmCcsFabNesting.tlsNestDbs.Shift(frmCcsFabNesting.tlsNestDbs.SelItemIndex(i))

                    If (The_Shift_Value = 1) Then
                        'Store the itemTag value so we can find the PDB name
                        sPieces = Split(frmNesting.TV_NestConfig.Nodes.Item(i).Tag, ",")
                        Temp_Str = sPieces(1)

                    End If

                    If (The_Shift_Value = 3) Then
                        'We have a NST File Selected

                        m_PDB_Code_counter = 0

                        pdb_name = frmNesting.TV_NestConfig.Nodes.Item(i).ParentNode.ParentNode.Tag
                        ' pdb_name = frmCcsFabNesting.tlsNestDbs.ItemStrValue(frmCcsFabNesting.tlsNestDbs.SelItemIndex(i))

                        Label_DB_Name = PathDriveFolder(pdb_name) & "\LabelDb.mdb"

                        lReturn = WRITE_PDB_NAME_TO_FILE(pdb_name & "|" & Temp_Str, Out_File_Name)

                        If (m_debug_code) Then MsgBox("We have a NST Selected")

                    Else
                        'Check to make sure that the selected Item is a PDB File
                        'And that The user did not Select another type of node
                        'by mistake otherwise coding will error out
                        If (UCase$(PathExt(Temp_Str)) = "PDB") Then

                            pdb_name = Temp_Str

                            Label_Txt_File = PathDriveFolder(pdb_name) & "\LabelDb.lst"
                            m_PDB_Code_counter = 0

                            connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & pdb_name & "';"

                            sql = "SELECT * FROM [Sheet Result]"

                            connection = New OleDbConnection(connetionString)

                            Try
                                connection.Open()

                                oledbAdapter = New OleDbDataAdapter(sql, connection)

                                ds = New DataSet

                                oledbAdapter.Fill(ds)

                                oledbAdapter.Dispose()

                                connection.Close()

                                If connection IsNot Nothing Then connection = Nothing
                                If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                                If (m_debug_code) Then MsgBox("The Number of records in the sheet table is " & ds.Tables(0).Rows.Count)

                                If (ds.Tables(0).Rows.Count > 0) Then
                                    For i2 = 0 To ds.Tables(0).Rows.Count - 1

                                        lReturn = PortalExecute("admin:new:")

                                        sFileName = ds.Tables(0).Rows(i).Item("Sheet Filename")

                                        lReturn = PortalExecute("file:importmm2: file=" & QStr(sFileName))

                                        SetValuesInHeader()

                                        lReturn = WRITE_PDB_NAME_TO_FILE(pdb_name & "|" & ds.Tables(0).Rows(i).Item("ID"), Out_File_Name)

                                        lReturn = PortalExecute("file:exportmm2: file=" & QStr(sFileName))

                                        ' lReturn = WRITE_PDB_NAME_TO_FILE(pdb_name & "|" & ds.Tables(0).Rows(i).Item("ID"), Out_File_Name)

                                    Next
                                End If

                                ds.Dispose()

                                If ds IsNot Nothing Then ds = Nothing

                            Catch
                            End Try

                        End If

                        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & pdb_name & "';"

                        sql = "SELECT * FROM [Sheet Result]"

                        connection = New OleDbConnection(connetionString)

                        Try
                            connection.Open()

                            oledbAdapter = New OleDbDataAdapter(sql, connection)

                            ds = New DataSet

                            oledbAdapter.Fill(ds)

                            oledbAdapter.Dispose()

                            connection.Close()
                            ds.Dispose()

                            If ds IsNot Nothing Then ds = Nothing

                            If connection IsNot Nothing Then connection = Nothing

                            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

                        Catch
                        End Try


                        If (m_debug_code) Then MsgBox("We have a pdb selected")

                    End If
                Next

                'swOutFile.Close()


                If (chkCreateMasterFile.Checked = True) Then

                    output_type = 1

                Else

                    output_type = 2

                End If

                fStream.Close()


                m_sPortalCommand = sPortalCommand & ",ccs=" & output_type & ", pdb=" & QStr(Out_File_Name) & ",label_db=" & QStr(Label_Txt_File)

                If (m_debug_code) Then MessageBox.Show("Portal Cmd <" & m_sPortalCommand & ">")

                lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!
                lReturn = PortalExecute(m_sPortalCommand)
                lReturn = PortalExecute("Admin:GlobalModel:")  'CRITICAL!

                'TODO V21
                ' frmCcsFabNesting.UpdateButtons m_code_gen

                If (m_debug_code) Then MessageBox.Show(" The temp label file name is " & Label_Txt_File & " ands the label mdb filename is " & Label_DB_Name)

                lReturn = Create_LabelMDB_From_Label_List(Label_Txt_File)


                modAdmin.ModelDirtyClear()

                MsgBox("Code Generation Process Completed.", vbOKOnly)

            End If


        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Function

    Private Function Create_LabelMDB_From_Label_List(LBL_File As String) As Long
        Dim sTablename As String = "Setup"
        Dim sFieldNames As String
        Dim sFieldValues As String
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim sql As String
        Dim sData As String
        Dim cat As ADOX.Catalog = New Catalog()


        Create_LabelMDB_From_Label_List = -1

        Try
            Label_DB_Name = NormalizePath(PathDriveFolder(m_xls_Filename)) & "labeldb.mdb"

            If (FileExists(Label_DB_Name) = True) Then

                ConditionalFileDelete(Label_DB_Name)

            End If

            cat.Create("Provider=Microsoft.Jet.OLEDB.4.0;" & _
                  "Data Source=" & Label_DB_Name & ";" & _
                  "Jet OLEDB:Engine Type=5")

            cat.ActiveConnection.close()


            sTablename = "Label Result"
            Dim connection As New OleDb.OleDbConnection("PROVIDER=Microsoft.Jet.OLEDB.4.0;Data Source =" & Label_DB_Name)


            Dim cmd As New OleDb.OleDbCommand("CREATE TABLE [" & sTablename & _
                                                    "] ([ID] AUTOINCREMENT PRIMARY KEY  , " & _
                                                    "[PDB PATH] TEXT(255), " & _
                                                    "[Sheet ID] TEXT(255), " & _
                                                    "[Part ID] TEXT(255))", connection)
            connection.Open()
            cmd.ExecuteNonQuery()

            sFieldNames = "[PDB Path]," & _
                                 "[Sheet ID]," & _
                                 "[Part ID]"

            Using sr As StreamReader = New StreamReader(LBL_File)

                While (sr.EndOfStream = False)

                    sData = sr.ReadLine()

                    sPieces = Split(sData, "|")


                    sFieldValues = "'" & sPieces(0) & "','" & sPieces(1) & "','" & sPieces(2) & "'"

                    sql = "INSERT INTO [Label Result] (" & sFieldNames & " ) VALUES(" & sFieldValues & ")"

                    m_daSettings.InsertCommand = New OleDbCommand(sql, connection)

                    m_daSettings.InsertCommand.ExecuteNonQuery()

                End While

                sr.Close()

            End Using

            connection.Close()
            ' m_daSettings.Dispose()
            cmd.Dispose()

            Create_LabelMDB_From_Label_List = 1

        Catch ex As Exception
            MessageBox.Show("Create_LabelMDB_From_Label_List: " & ex.Message)
        End Try

    End Function
    Private Function WRITE_PDB_NAME_TO_FILE(pdb_file As String, Temp_Filename As String) As Long

        m_PDB_Code_counter = m_PDB_Code_counter + 1

        If (m_debug_code) Then MessageBox.Show("This is the " & m_PDB_Code_counter & " entry to code. The PDB Being Coded is " & pdb_file)

        WRITE_PDB_NAME_TO_FILE = m_PDB_Code_counter

        ' Append the first line of text to a new file.
        Using writer As StreamWriter =
            New StreamWriter(Temp_Filename, True)

            writer.WriteLine(pdb_file)
        End Using


        Try


        Catch myException As Exception

            ' Show a message box if an error occurs
            MessageBox.Show(myException.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)

        End Try
    End Function
    Private Sub SaveToKnownFile(LV As ListView, FilePathAndName As String)

        Dim sw As New StreamWriter(FilePathAndName)

        Try

            For i As Integer = 0 To LV.Items.Count - 1
                sw.Write(LV.Items(i).Text)
                For subItemIndex As Integer = 1 To LV.Items(i).SubItems.Count - 1
                    sw.Write(":::" & LV.Items(i).SubItems(subItemIndex).Text)
                Next
                sw.Write(System.Environment.NewLine)

            Next
            sw.Close()
        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Private Sub rgOptMode_SelectedIndexChanged(sender As Object, e As EventArgs) Handles rgOptMode.SelectedIndexChanged
        EnableOrDisable()

    End Sub
    Private Sub CheckChanged(ByVal sender As Object, ByVal e As EventArgs)
        Dim cb As CheckButton = TryCast(sender, CheckButton)

        Try

            If images.ContainsKey(cb) Then
                If cb.Checked Then
                    cb.Image = images(cb)(0)

                    RegPutString("CodeGeneration", "StartCorner", cb.Tag)

                Else
                    cb.Image = images(cb)(1)
                End If

            End If
            If (cb.Checked) AndAlso (previouslyCheckedButton IsNot Nothing) AndAlso images.ContainsKey(previouslyCheckedButton) Then
                previouslyCheckedButton.Image = images(previouslyCheckedButton)(1)
            End If
            previouslyCheckedButton = cb

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    'Private Sub lcbGridSequence_TextChange() Handles lcbGridSequence2.TextChange
    '    EnableOrDisable()
    'End Sub

    Private Sub cmdCompile_Click(sender As Object, e As EventArgs) Handles cmdCompile.Click
        Windows.Forms.Cursor.Current = Cursors.WaitCursor

        lReturn = PortalExecute( _
                    "admin:MacroCompile:" & _
                    " javafile=" & QStr(fcbCodeGenFile.Tag))

        Windows.Forms.Cursor.Current = Cursors.Default
    End Sub

    Private Sub btnSlice1_Click(sender As Object, e As EventArgs) Handles btnSlice1.Click
        lReturn = HeaderIntSet("slice_opt", 0)
        nSlice_Opt = 0
    End Sub

    Private Sub btnSlice2_Click(sender As Object, e As EventArgs) Handles btnSlice2.Click
        lReturn = HeaderIntSet("slice_opt", 1)
        nSlice_Opt = 1
    End Sub

    Private Sub btnSlice3_Click(sender As Object, e As EventArgs) Handles btnSlice3.Click
        lReturn = HeaderIntSet("slice_opt", 2)
        nSlice_Opt = 2
    End Sub

    Private Sub btnSlice4_Click(sender As Object, e As EventArgs) Handles btnSlice4.Click
        lReturn = HeaderIntSet("slice_opt", 3)
        nSlice_Opt = 3
    End Sub

    Private Sub btnSlice5_Click(sender As Object, e As EventArgs) Handles btnSlice5.Click
        lReturn = HeaderIntSet("slice_opt", 4)
        nSlice_Opt = 4
    End Sub

    Private Sub btnSlice6_Click(sender As Object, e As EventArgs) Handles btnSlice6.Click
        lReturn = HeaderIntSet("slice_opt", 5)
        nSlice_Opt = 5
    End Sub

    Private Sub btnSlice7_Click(sender As Object, e As EventArgs) Handles btnSlice7.Click
        lReturn = HeaderIntSet("slice_opt", 6)
        nSlice_Opt = 6
    End Sub

    Private Sub btnSlice8_Click(sender As Object, e As EventArgs) Handles btnSlice8.Click
        lReturn = HeaderIntSet("slice_opt", 7)
        nSlice_Opt = 7
    End Sub

    Private Sub cmdCancel_Click(sender As Object, e As EventArgs) Handles cmdCancel.Click
        Me.Close()

    End Sub

    Private Sub lcbGridSequence_TextChanged(sender As Object, e As EventArgs) Handles lcbGridSequence.TextChanged
        EnableOrDisable()
    End Sub


    Private Function Evaluate(ByRef dItem As Object, ByRef decFlag As Object) As Double

        ' This function Evaluates the shared sExpression
        ' holding an arithmetic expression.

        Static nChar As Integer
        Static sTemp As String
        Static sCollection As String
        Static dTest As Double

        dTest = 0

        If nEvalError Then Exit Function
        Do While nCurrentCharPos < Len(sExpression)
            nCurrentCharPos = nCurrentCharPos + 1
            sTemp = Mid(sExpression, nCurrentCharPos, 1)
            nChar = Asc(sTemp)

            Select Case nChar

                Case 48 To 57 ' ignore blanks
                    sCollection = sCollection & sTemp

                Case DecPoint
                    If decFlag And bit00 Then
                        ' we've got a fractional part already
                        nEvalError = 2
                        Exit Do
                    Else
                        ' turn on bit00 to indicate we're collecting the fractional part
                        decFlag = decFlag Or bit00
                        sCollection = sCollection & sTemp
                    End If

                Case plus

                    If decFlag And push1 Then Exit Do

                    GoTo collectItem
                    dItem = dItem + Evaluate(0.0#, bit01)

                Case minus
                    If decFlag And push1 Then Exit Do

                    GoTo collectItem

                    dItem = dItem - Evaluate(0.0#, bit01)

                Case Multiply
                    If decFlag And push2 Then Exit Do

                    GoTo collectItem

                    dItem = dItem * Evaluate(0.0#, bit02)

                Case Divide
                    If decFlag And push2 Then Exit Do

                    GoTo collectItem
                    dTest = Evaluate(0.0#, bit02)

                    If dTest Then

                        dItem = dItem / dTest
                    Else
                        nEvalError = 11
                    End If

                Case Exponent
                    If decFlag And push3 Then Exit Do


                    GoTo collectItem

                    dItem = dItem ^ Evaluate(0.0#, bit03)

                Case OpenParen

                    dItem = Evaluate(0.0#, 0)

                Case CloseParen
                    nCurrentCharPos = nCurrentCharPos + 1
                    Exit Do

                Case Else
                    nEvalError = 2
                    Exit Do

            End Select
        Loop

push:

        If Len(sCollection) Then
            Evaluate = Val(sCollection)
            sCollection = ""
        Else
            Evaluate = dItem
        End If

        If nCurrentCharPos < Len(sExpression) Then nCurrentCharPos = nCurrentCharPos - 1

        Exit Function
collectItem:

        If Len(sCollection) Then dItem = Val(sCollection)

        sCollection = ""


    End Function

    Private Function Calculate(ByVal sFormula As String, teCtrl As TextEdit) As String

        Dim lReturn As Integer
        Dim dTemp As Decimal

        'On Error Resume Next
        Calculate = "" 'assume failure.

        sExpression = sFormula
        nCurrentCharPos = 0
        nEvalError = 0


        lReturn = PortalExecute("*solve:eval:exp=" & QStr(sFormula))

        If (lReturn = 0) Then nEvalError = 1

        dTemp = PortalGetDouble("val", 0)


        If (nEvalError = 0) Then


            Calculate = FormatNumber(dTemp, 5)

        Else

            MsgBox("Invalid expresion.", MsgBoxStyle.Exclamation, My.Application.Info.Title)
            teCtrl.Focus()

        End If

    End Function

    Private Sub mtbStartPointX_Enter(sender As Object, e As EventArgs) Handles mtbStartPointX.Enter
        If Len(mtbStartPointX.Tag) > 0 Then
            mtbStartPointX.Text = mtbStartPointX.Tag ' restore from tagged information
        End If

        mtbStartPointX.SelectionStart = 0
        mtbStartPointX.SelectionLength = Len(mtbStartPointX.Text)
    End Sub

    Private Sub mtbStartPointX_KeyPress(sender As Object, e As KeyPressEventArgs) Handles mtbStartPointX.KeyPress
        Dim KeyAscii As Integer = Asc(e.KeyChar)

        If KeyAscii = 13 Then
            KeyAscii = 0
            System.Windows.Forms.SendKeys.Send("{TAB}") ' treat as a tab to calculate
        End If

        If KeyAscii < 48 Or KeyAscii > 57 Then
            Select Case KeyAscii

                Case 8 ' backspace

                Case 13 ' return
                    KeyAscii = 0
                    System.Windows.Forms.SendKeys.Send("{TAB}") ' treat as a tab to calculate

                Case 27 ' escape

                Case 32 ' space

                Case 40 ' (

                Case 41 ' )

                Case 42 ' *

                Case 43 ' +

                Case 45 ' -

                Case 46 ' .

                Case 47 ' /

                Case 94 ' ^

                Case Else
                    KeyAscii = 0

            End Select
        End If


        e.KeyChar = Chr(KeyAscii)
        If KeyAscii = 0 Then
            e.Handled = True
        End If

    End Sub

    Private Sub mtbStartPointX_Leave(sender As Object, e As EventArgs) Handles mtbStartPointX.Leave
        If (Len(mtbStartPointX.Text) > 0) Then

            mtbStartPointX.Text = Calculate(mtbStartPointX.Text, mtbStartPointX)

            If (nEvalError <> 0) Then

                'NOTE: At this point, txtMath.text should be the empty string.
                nEvalError = 0 ' reset

                mtbStartPointX.Text = mtbStartPointX.Tag 'restore the original input
                mtbStartPointX.Focus()

            Else

                mtbStartPointX.Tag = mtbStartPointX.Text 'save for restoration

            End If

        End If
    End Sub

    Private Sub mtbStartPointY_Enter(sender As Object, e As EventArgs) Handles mtbStartPointY.Enter
        If Len(mtbStartPointY.Tag) > 0 Then
            mtbStartPointY.Text = mtbStartPointY.Tag ' restore from tagged information
        End If

        mtbStartPointY.SelectionStart = 0
        mtbStartPointY.SelectionLength = Len(mtbStartPointY.Text)
    End Sub

    Private Sub mtbStartPointY_KeyPress(sender As Object, e As KeyPressEventArgs) Handles mtbStartPointY.KeyPress
        Dim KeyAscii As Integer = Asc(e.KeyChar)

        If KeyAscii = 13 Then
            KeyAscii = 0
            System.Windows.Forms.SendKeys.Send("{TAB}") ' treat as a tab to calculate
        End If

        If KeyAscii < 48 Or KeyAscii > 57 Then
            Select Case KeyAscii

                Case 8 ' backspace

                Case 13 ' return
                    KeyAscii = 0
                    System.Windows.Forms.SendKeys.Send("{TAB}") ' treat as a tab to calculate

                Case 27 ' escape

                Case 32 ' space

                Case 40 ' (

                Case 41 ' )

                Case 42 ' *

                Case 43 ' +

                Case 45 ' -

                Case 46 ' .

                Case 47 ' /

                Case 94 ' ^

                Case Else
                    KeyAscii = 0

            End Select
        End If


        e.KeyChar = Chr(KeyAscii)
        If KeyAscii = 0 Then
            e.Handled = True
        End If

    End Sub

    Private Sub mtbStartPointY_Leave(sender As Object, e As EventArgs) Handles mtbStartPointY.Leave
        If (Len(mtbStartPointY.Text) > 0) Then

            mtbStartPointY.Text = Calculate(mtbStartPointY.Text, mtbStartPointY)

            If (nEvalError <> 0) Then

                'NOTE: At this point, txtMath.text should be the empty string.
                nEvalError = 0 ' reset

                mtbStartPointY.Text = mtbStartPointY.Tag 'restore the original input
                mtbStartPointY.Focus()

            Else

                mtbStartPointY.Tag = mtbStartPointY.Text 'save for restoration

            End If

        End If
    End Sub

    Private Sub fcbCodeGenFile_EditValueChanged(sender As Object, e As EventArgs) Handles fcbCodeGenFile.EditValueChanged

    End Sub

    Private Sub fcbExportTo_EditValueChanged(sender As Object, e As EventArgs) Handles fcbExportTo.EditValueChanged
        Dim sFilename As String
        sFilename = fcbExportTo.Text
        fcbExportTo.Tag = sFilename
    End Sub
End Class