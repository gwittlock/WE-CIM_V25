Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports DevExpress.XtraVerticalGrid.Rows
Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraEditors
Imports DevExpress.XtraTreeList.Nodes
Imports DevExpress.XtraTreeList
Imports System.Data.OleDb
Imports DevExpress.XtraEditors.Popup

Partial Public Class dlgEditProject
    Inherits Form
    Dim currentCategoryRow As CategoryRow = Nothing
    Dim my_DatVarName As String
    Dim my_DialogDisplay As String
    Dim my_VBValue As String
    Dim my_CValue As String
    Dim my_CVarName As String
    Dim my_DataType As Integer
    Dim my_RecodID As Integer
    Dim my_sOptions As String
    Private fileName As String = ""

    Private editValue As String
    Private nRowIndx As Integer
    Private nRowId As BaseRow

    Private nToolSetupRowId As BaseRow
    Private SelectedMachineName As String
    Private SelectedToolSetupName As String

    Private CFG_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_CATEGORY_NODE As TreeListNode
    Private Const TAG_MATERIAL_ROOT_NODE = -200
    Private Const TAG_MATERIAL_CATEGORY_NODE = -201
    Private Const TAG_MATERIAL_NODE = -202
    Private CFG_MATERIAL_NODE As TreeListNode
    Public WithEvents rbMaterialSettings As DevExpress.XtraEditors.Repository.RepositoryItemPopupContainerEdit

    Public Sub New()
        InitializeComponent()

        'Delimiters for formatting internal strings.
        CHR1 = Convert.ToChar(1)
        CHR2 = Convert.ToChar(2)

    End Sub


    Private Sub vGridControl1_ShownEditor(sender As Object, e As EventArgs) Handles vgSettingsProplist.ShownEditor

        'Dim edit As ComboBoxEdit = TryCast(vGridControl1.ActiveEditor, ComboBoxEdit)
        'If nRowId IsNot Nothing Then
        '    If vGridControl1.FocusedRow Is nRowId Then
        '        edit.Properties.Items.Clear()
        '        edit.Properties.Items.Add(New ComboBoxItem(editValue))

        '    End If
        'End If
        'If edit IsNot Nothing Then
        '    AddHandler edit.EditValueChanged, AddressOf edit_EditValueChanged
        'End If

        Dim edit As ComboBoxEdit = TryCast(vgSettingsProplist.ActiveEditor, ComboBoxEdit)


        Dim indx As Integer
        Dim DB_TOOLSETUP As Integer = 8
        Dim DB_LAYERSETUP As Integer = 42
        Dim s_ToolSetup As String
        Dim n_ToolSetupID As Long


        Dim sOptions As String = sEmpty

        Try
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

            If edit IsNot Nothing Then
                AddHandler edit.EditValueChanged, AddressOf edit_EditValueChanged
            End If
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try


    End Sub

    Private Sub edit_EditValueChanged(sender As Object, e As EventArgs)
        'Dim edit As ComboBoxEdit = TryCast(sender, ComboBoxEdit)

        'editValue = edit.EditValue.ToString()

        'If (nRowId IsNot Nothing) Then
        '    nRowId = GetRowByCaption("Tool Setup")

        '    nRowIndx = nRowId.Index


        '    vGridControl1.FocusedRow = vGridControl1.Rows(nRowId.ParentRow.Index).ChildRows(nRowId.Index)
        'End If

        'vGridControl1.ShowEditor()

        Dim edit As ComboBoxEdit = TryCast(sender, ComboBoxEdit)
        Dim my_DefnID As Integer
        Dim childrow As BaseRow
        Dim nMaterialID As Long
        Dim my_Cvalue As String
        Dim nVarType As Integer


        Try
            nToolSetupRowId = Nothing

            editValue = edit.EditValue.ToString()

            nVarType = CInt(GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "VarDataType", 0))


            If (vgSettingsProplist.FocusedRow.Properties.Caption = "Machine") Then

                SelectedMachineName = edit.EditValue.ToString()
                m_machine_id = GetMachineIDFromDescrip(SelectedMachineName)


                nToolSetupRowId = GetRowByCaption("Tool Setup")

                If (IsNothing(nToolSetupRowId) = False) Then
                    nRowIndx = nToolSetupRowId.Index


                    vgSettingsProplist.FocusedRow = vgSettingsProplist.Rows(nRowIndx)
                    vgSettingsProplist.FocusedRow.Properties.Value = ""
                    vgSettingsProplist.ShowEditor()
                End If
            End If

            If (vgSettingsProplist.FocusedRow.Properties.Caption = "Material Type") Then
                my_Cvalue = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "CValue", "")

                nMaterialID = CLng(GetMaterialIDFromDescription(edit.EditValue))

                childrow = GetRowByCaption("Material Length")

                vgSettingsProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialLength(CLng(nMaterialID))
                m_accumulator.mat_len = CDbl(modMaterial.GetMaterialLength(CLng(nMaterialID)))

                childrow = GetRowByCaption("Material Width")
                vgSettingsProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialWidth(CLng(nMaterialID))
                m_accumulator.mat_wid = CDbl(modMaterial.GetMaterialWidth(CLng(nMaterialID)))

                childrow = GetRowByCaption("Material thickness")
                vgSettingsProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialThickness(CLng(nMaterialID))
                m_accumulator.mat_thick = CDbl(modMaterial.GetMaterialThickness(CLng(nMaterialID)))

            End If

            my_DefnID = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "DefnID", 0)

            If (my_DefnID > 0 And nVarType = DB_USERVAR) Then


                lReturn = GetPropsForUserVar(vgSettingsProplist, vgSettingsProplist.FocusedRow, edit.SelectedIndex, my_DefnID)
                vgSettingsProplist.FocusedRow.Properties.Value = editValue
                m_accumulator.soln_Set(5, edit.SelectedIndex)

            End If


        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try


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

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles Me.Load
        Dim sql As String

        CreateMaterialTypeNodes(tl)


        popupContainerControl1.Controls.Add(tl)
        ' Used rbMaterialSettings as the editor for Material record cell
        rbMaterialSettings = New RepositoryItemPopupContainerEdit()
        rbMaterialSettings.Name = "rbMaterialSettings"
        rbMaterialSettings.PopupControl = popupContainerControl1
        AddHandler rbMaterialSettings.QueryDisplayText, AddressOf rbMaterialSettings_QueryDisplayText

        vgSettingsProplist.RepositoryItems.Add(rbMaterialSettings)

        'populateFromDatFile(vGridControl1, Application.StartupPath & "\EditProject.dat", currentCategoryRow, lbOptions)

        m_sncs = 2021

        sql = "SELECT * FROM ModifyVar WHERE (ModifyID=" & m_sncs & ") Order By ModifyVar.ID"
        'lReturn = MyInitFromDB(vGridControl1, PathAppDB(), sql)
        populateFromAppDB(vgSettingsProplist, PathAppDB(), sql, currentCategoryRow, lbOptions)

        GetProjectInfo()

        'CreateMaterialTypeNodes(tl)

        'vGridControl1.Rows("categoryMaterial_Inventory").ChildRows(0).Properties.RowEdit = rbMaterialSettings
    End Sub


    Private Sub popupContainerEdit1_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs) Handles popupContainerEdit1.QueryDisplayText
        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")

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

                'If InStr(UCase(s_MaterialCategory), UCase(sUsage)) Then
                n_MaterialID = dtMaterialType.Rows(i).Item("ID")

                CFG_MATERIAL_CATEGORY_NODE = tl.AppendNode(New Object() {s_MaterialCategory}, CFG_MATERIAL_ROOT_NODE, TAG_MATERIAL_CATEGORY_NODE)

                FillMaterialNodes(n_MaterialID)

                dtMaterialType.Dispose()
                'End If

            Next

            tl.EndUnboundLoad()



        Catch ex As Exception

            MessageBox.Show("CreateMaterialTypeNodes: " & ex.Message)

        End Try

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

    Private Sub tl_FocusedNodeChanged(sender As Object, e As DevExpress.XtraTreeList.FocusedNodeChangedEventArgs) Handles tl.FocusedNodeChanged
        Dim nCurr_Node_Tag As Long
        Dim sCurr_Display As String
        Dim sMaterialType As String
        Dim treeList As TreeList = TryCast(sender, TreeList)
        Try


            If (e.Node IsNot Nothing) Then


                If (e.Node.ParentNode IsNot Nothing) Then

                    sMaterialType = e.Node.ParentNode.GetDisplayText(tl.Columns(0))
                    sCurr_Display = sMaterialType & ", " & e.Node.GetDisplayText(tl.Columns(0))
                    nCurr_Node_Tag = e.Node.Tag

                    Dim popupContainerControl = TryCast(treeList.Parent, PopupContainerControl)
                    popupContainerControl.OwnerEdit.EditValue = sCurr_Display
                    popupContainerControl.OwnerEdit.Tag = e.Node.Tag
                    popupContainerControl.OwnerEdit.ClosePopup()
                    popupContainerEdit1.EditValue = sCurr_Display ' e.Node.ParentNode.GetDisplayText(tl.Columns(1))
                    popupContainerEdit1.Tag = e.Node.Tag

                    'popupContainerEdit1.ClosePopup()
                End If
            End If

        Catch ex As Exception

            MessageBox.Show("tl_FocusedNodeChanged: " & ex.Message)

        End Try

    End Sub

    Private Function CreatePlainCategoryRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl, ByVal rowCaption As String) As CategoryRow
        Dim categoryRow As New CategoryRow(rowCaption)
        mGridCtrl.Rows.Add(categoryRow)
        Return categoryRow
    End Function
    Private Function CreateCategoryRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                     ByVal DatVarName As String,
                                            DialogDisplay As String,
                                            VBValue As String,
                                            CValue As String,
                                            CVArName As String,
                                            DataType As Integer,
                                            RecodID As Integer,
                                            sOptions As String,
                                            nDefnID As Integer) As CategoryRow

        Dim categoryRow As New CategoryRow(DialogDisplay)
        mGridCtrl.Rows.Add(categoryRow)

        categoryRow.OptionsRow.DblClickExpanding = False
        categoryRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)


        Return categoryRow
    End Function
    Private Function CreateStringEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                           ByVal DatVarName As String,
                                            DialogDisplay As String,
                                            VBValue As String,
                                            CValue As String,
                                            CVArName As String,
                                            DataType As Integer,
                                            RecodID As Integer,
                                            sOptions As String,
                                            nDefnID As Integer) As EditorRow

        Dim editorRow As New EditorRow()
        ' assigning the name and caption to the created editor row            
        editorRow.Properties.Caption = DialogDisplay
        editorRow.Properties.Value = VBValue
        editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)


        Return editorRow
    End Function

    Private Function CreateBooleanEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                           ByVal DatVarName As String,
                                            DialogDisplay As String,
                                            VBValue As String,
                                            CValue As String,
                                            CVArName As String,
                                            DataType As Integer,
                                            RecodID As Integer,
                                            sOptions As String,
                                            nDefnID As Integer) As EditorRow

        Dim editorRow As New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = DialogDisplay
        editorRow.Properties.Value = IIf(VBValue = True, True, False)

        editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)

        Return editorRow

    End Function

    Private Function CreateFileEditEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                            ByVal DatVarName As String,
                                            DialogDisplay As String,
                                            VBValue As String,
                                            CValue As String,
                                            CVArName As String,
                                            DataType As Integer,
                                            RecodID As Integer,
                                            sOptions As String,
                                            nDefnID As Integer) As EditorRow

        Dim editorRow As New EditorRow()
        Dim bEdit As New RepositoryItemButtonEdit

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = DialogDisplay
        editorRow.Properties.Value = TrimToFit(VBValue, 30)
        editorRow.Properties.CustomizationCaption = VBValue
        editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)


        bEdit.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor

        mGridCtrl.RepositoryItems.Add(bEdit)
        editorRow.Properties.RowEdit = bEdit
        Return editorRow

    End Function
    Private Function CreateButtonEditEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                                DatVarName As String,
                                                DialogDisplay As String,
                                                VBValue As String,
                                                CValue As String,
                                                CVArName As String,
                                                DataType As Integer,
                                                RecodID As Integer,
                                                sOptions As String,
                                                nDefnID As Integer) As EditorRow

        Dim editorRow As New EditorRow()

        Dim bEdit As New RepositoryItemButtonEdit
        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = DialogDisplay
        editorRow.Properties.Value = TrimToFit(VBValue, 30)
        editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)


        bEdit.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor

        mGridCtrl.RepositoryItems.Add(bEdit)
        editorRow.Properties.RowEdit = bEdit
        Return editorRow

    End Function
    Private Function CreateIntegerEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                            DatVarName As String,
                                            DialogDisplay As String,
                                            VBValue As String,
                                            CValue As String,
                                            CVArName As String,
                                            DataType As Integer,
                                            RecodID As Integer,
                                            sOptions As String,
                                            nDefnID As Integer) As EditorRow

        Dim editorRow As New EditorRow()
        Dim IntValue As Integer

        IntValue = Convert.ToInt32(VBValue)

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = DialogDisplay
        editorRow.Properties.Value = IntValue
        editorRow.Appearance.Options.UseTextOptions = True
        editorRow.Appearance.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Near

        editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)


        Dim reSpinEdit As New RepositoryItemSpinEdit()
        reSpinEdit.IsFloatValue = False

        mGridCtrl.RepositoryItems.Add(reSpinEdit)
        editorRow.Properties.RowEdit = reSpinEdit

        If (IsDBNull(DatVarName) = False) Then
            'lReturn = UpdateAccumulatorDirect(mGridCtrl, DatVarName, VBValue)
        End If

        Return editorRow
    End Function

    Private Function CreateDoubleEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                           DatVarName As String,
                                            DialogDisplay As String,
                                            VBValue As String,
                                            CValue As String,
                                            CVarName As String,
                                            DataType As Integer,
                                            RecodID As Integer,
                                            sOptions As String,
                                            nDefnID As Integer) As EditorRow

        Dim editorRow As New EditorRow()
        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = DialogDisplay
        editorRow.Properties.Value = CDbl(VBValue)
        editorRow.Appearance.Options.UseTextOptions = True
        editorRow.Appearance.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Near
        editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVarName, DataType, RecodID, sOptions, nDefnID)

        Dim reCalcEdit As New RepositoryItemCalcEdit()
        mGridCtrl.RepositoryItems.Add(reCalcEdit)
        editorRow.Properties.RowEdit = reCalcEdit

        ' lReturn = UpdateAccumulatorDirect(mGridCtrl, DatVarName, VBValue)

        Return editorRow
    End Function

    '    Private Function CreateComboBoxEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
    '                                            ByVal rowCaption As String,
    '                                            ByVal sIname As String,
    '                                            ByVal nRecordID As Integer,
    '                                            ByVal rowValue As String,
    '                                            ByVal options As Collection) As EditorRow

    '        Dim editorRow As New EditorRow()
    '        ' assigning the name and caption to the created editor row
    '        editorRow.Properties.Caption = rowCaption
    '        editorRow.Properties.Value = rowValue
    '    'editorRow.Tag = Encode(nRecordID, rowValue, DB_DBL, sEmpty, DB_COMBO)
    '    editorRow.Tag = EncodeTag(rowCaption, rowCaption, 0, 0, 0, 1, nRecordID, options, 0)

    '    Dim reComboBoxEdit As New RepositoryItemComboBox()

    '        reComboBoxEdit.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor

    '        mGridCtrl.RepositoryItems.Add(reComboBoxEdit)
    '        editorRow.Properties.RowEdit = reComboBoxEdit

    '    For i As Integer = 1 To options.Count
    '        reComboBoxEdit.Items.Add(options(i))
    '    Next i



    '    Return editorRow

    'End Function

    Private Function CreateMatListEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                           ByVal DatVarName As String,
                                            DialogDisplay As String,
                                            VBValue As String,
                                            CValue As String,
                                            CVArName As String,
                                            DataType As Integer,
                                            RecodID As Integer,
                                            sOptions As String,
                                            nDefnID As Integer) As EditorRow

        Dim editorRow As New EditorRow()

        'fTheParentform = mGridCtrl.Parent.FindForm


        ' assigning the name and caption to the created editor row            
        editorRow.Properties.Caption = DialogDisplay

        'editorRow.Properties.Value = VBValue

        'Return editorRow

        Dim fMy_Form As New dlgEditProject




        '        MessageBox.Show("The forms name is < " & fMy_Form.Name & " >")

        Dim rbMaterialSettings As DevExpress.XtraEditors.Repository.RepositoryItemPopupContainerEdit


        fMy_Form.rbMaterialSettings = New RepositoryItemPopupContainerEdit()

        mGridCtrl.RepositoryItems.Add(rbMaterialSettings)



        AddHandler fMy_Form.rbMaterialSettings.QueryDisplayText, AddressOf fMy_Form.rbMaterialSettings_QueryDisplayText


        editorRow.Properties.RowEdit = fMy_Form.rbMaterialSettings


        fMy_Form.rbMaterialSettings.Name = "rbMaterialSettings"


        fMy_Form.rbMaterialSettings.PopupControl = fMy_Form.popupContainerControl1

        editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)


        Return editorRow



    End Function

    Private Function CreateMaterialListEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                                    puMatlist As PopupContainerControl,
                                                    ByVal DatVarName As String,
                                                    DialogDisplay As String,
                                                    VBValue As String,
                                                    CValue As String,
                                                    CVArName As String,
                                                    DataType As Integer,
                                                    RecodID As Integer,
                                                    Options As Collection,
                                                    nDefnID As Integer) As EditorRow

        Dim sOptions As String = sEmpty

        Dim editorRow As New EditorRow()
        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = DialogDisplay
        editorRow.Properties.Value = VBValue


        Dim reMaterialList As New RepositoryItemPopupContainerEdit

        reMaterialList.PopupControl = Me.popupContainerControl1

        reMaterialList.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor

        'mGridCtrl.RepositoryItems.Add(reComboBoxEdit)
        'editorRow.Properties.RowEdit = reMaterialList

        'For i As Integer = 1 To Options.Count
        '    Dim tempMachine As MachineListCombo = Options(i)
        '    reComboBoxEdit.Items.Add(tempMachine.Description)

        '    If (i = 1) Then
        '        sOptions = tempMachine.Description
        '    Else
        '        sOptions = sOptions & ";" & tempMachine.Description
        '    End If

        'Next i

        'editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)

        Return editorRow
    End Function

    Private Function CreateComboBoxListEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                                    ByVal DatVarName As String,
                                                    DialogDisplay As String,
                                                    VBValue As String,
                                                    CValue As String,
                                                    CVArName As String,
                                                    DataType As Integer,
                                                    RecodID As Integer,
                                                    Options As Collection,
                                                    nDefnID As Integer) As EditorRow

        Dim sOptions As String = sEmpty

        Dim editorRow As New EditorRow()
        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = DialogDisplay
        editorRow.Properties.Value = VBValue


        Dim reComboBoxEdit As New RepositoryItemComboBox()
        reComboBoxEdit.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor

        mGridCtrl.RepositoryItems.Add(reComboBoxEdit)
        editorRow.Properties.RowEdit = reComboBoxEdit

        For i As Integer = 1 To Options.Count
            Dim tempMachine As MachineListCombo = Options(i)
            reComboBoxEdit.Items.Add(tempMachine.Description)

            If (i = 1) Then
                sOptions = tempMachine.Description
            Else
                sOptions = sOptions & ";" & tempMachine.Description
            End If

        Next i

        editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions, nDefnID)

        Return editorRow
    End Function
    Private Function CreateColorEditEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                                     ByVal rowCaption As String,
                                                    ByVal sIname As String,
                                                    ByVal nRecordID As Integer,
                                                    ByVal rowValue As System.Drawing.Color) As EditorRow

        Dim editorRow As New EditorRow()

        ' assigning the name and caption to the created editor row
        editorRow.Properties.Caption = rowCaption
        editorRow.Properties.Value = rowValue
        editorRow.Tag = Encode(nRecordID, rowValue.ToArgb, DB_DBL, sEmpty, DB_COLOR)


        Dim cEdit As New RepositoryItemColorEdit
        'cEdit.StoreColorAsInteger = True
        mGridCtrl.RepositoryItems.Add(cEdit)
        editorRow.Properties.RowEdit = cEdit


        Return editorRow

    End Function

    Private Function GetRepositoryItem(type As Type) As String
        If type = GetType(DateTime) OrElse type = GetType(System.Nullable(Of DateTime)) Then
            Return "DateEdit"
        End If
        If type = GetType([Boolean]) OrElse type = GetType(System.Nullable(Of [Boolean])) Then
            Return "CheckEdit"
        End If
        If type = GetType([Byte]()) OrElse type = GetType(System.Data.SqlTypes.SqlBinary) Then
            Return "PictureEdit"
        End If
        Return "TextEdit"
    End Function

    Private Function PopulateComboListFromDat(ByRef lbOptionsList As DevExpress.XtraEditors.ListBoxControl) As Collection

        PopulateComboListFromDat = New Collection
        'lbOptionsList.Items.Clear()

        PopulateComboListFromDat.Clear()

        For i = 0 To lbOptionsList.Items.Count - 1
            Dim myMachine As MachineListCombo = New MachineListCombo()
            myMachine.Description = lbOptionsList.Items(i).ToString
            myMachine.ID = i

            PopulateComboListFromDat.Add(myMachine, myMachine.Description)
        Next

    End Function
    Private Function PopulateComboList(sql As String, bIncludeTypeID As Boolean, bIncludeNone As Boolean) As Collection

        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim cmd As OleDbCommand = Nothing
        Dim sTemp As String = ""
        Dim ni As Integer
        Dim bDoAdd As Boolean



        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        PopulateComboList = New Collection


        connection = New OleDbConnection(connetionString)

        Try
            connection.Open()

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(ds)

            oledbAdapter.Dispose()

            connection.Close()

            If (bIncludeNone = True) Then
                Dim myMachine As MachineListCombo = New MachineListCombo()
                myMachine.Description = "None"
                myMachine.ID = 0
                PopulateComboList.Add(myMachine, myMachine.Description)

            End If

            For i = 0 To ds.Tables(0).Rows.Count - 1
                'If (bIncludeNone = False) Then
                Dim myMachine As MachineListCombo = New MachineListCombo()
                'End If

                bDoAdd = True

                If (ds.Tables(0).Columns.Contains("NC Code Number") = True) Then

                    sTemp = "[" & ds.Tables(0).Rows(i).Item("NC Code Number") & "] "

                End If


                myMachine.ID = ds.Tables(0).Rows(i).Item("ID")

                If (bIncludeTypeID = True) Then

                    myMachine.TypeID = ds.Tables(0).Rows(i).Item("Type ID")

                    If (sql = "SELECT * FROM [Material Inventory]") Then

                        sTemp = GetMaterialTypeName(ds.Tables(0).Rows(i).Item("ID").ToString)

                    End If

                End If

                myMachine.Description = ds.Tables(0).Rows(i).Item("Description")

                For ni = 1 To PopulateComboList.Count
                    If myMachine.Description = PopulateComboList.Item(ni).description Then
                        bDoAdd = False
                        Exit For
                    End If
                Next

                If (bDoAdd = True) Then
                    PopulateComboList.Add(myMachine, myMachine.Description)
                End If

                sTemp = ""

            Next

            ds.Dispose()

            If ds IsNot Nothing Then ds = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

            If connection IsNot Nothing Then connection = Nothing

        Catch ex As Exception

            MessageBox.Show("PopulateComboList: " & ex.Message)
        End Try

    End Function
    Private Function GenericFileIO(m_Title As String, m_Usage As EXPLORER_OPTION, m_filters As Long) As String

        GenericFileIO = Nothing

        Select Case m_Usage

            Case EXPLORER_OPTION.EXPLORER_OPEN
                Dim newFileDialog1 As New OpenFileDialog
                newFileDialog1.Filter = "PDB files (*.pdb)|*.pdb"
                newFileDialog1.FilterIndex = 1
                newFileDialog1.RestoreDirectory = True

                If newFileDialog1.ShowDialog() = System.Windows.Forms.DialogResult.OK Then

                    If (FileExists(GenericFileIO) = True) Then

                        lReturn = MsgBox("Overwrite this existing file?" & vbCrLf & GenericFileIO, MsgBoxStyle.YesNo)

                        If (lReturn = MsgBoxResult.Yes) Then

                            GenericFileIO = newFileDialog1.FileName

                        End If

                    End If

                End If

            Case EXPLORER_OPTION.EXPLORER_SAVE
                Dim newFileDialog1 As New SaveFileDialog
        End Select


    End Function

    Private Function Encode(
                            recordID As Long,
                            Value As String,
                            attType As Integer,
                            options As String,
                            fieldType As Integer) As String


        Encode = CHR1 & "ID" & CHR2 & recordID & CHR1 & "VAL" & CHR2 & Value & CHR1 & "AttType" & CHR2 & attType & CHR1 & "Options" & CHR2 & options & CHR1 & "FieldType" & CHR2 & fieldType

    End Function

    Private Function MyInitFromDB(s_proplist As DevExpress.XtraVerticalGrid.VGridControl,
                                db_name As String,
                                sql As String) As Long

        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer
        Dim my_sOptions As String
        Dim my_DefnID As Long
        Dim currentCategoryRow As CategoryRow = Nothing
        Dim dtAttributes As DataTable = Nothing
        Dim toolSetupName As String
        Dim nToolSetupID As Integer
        Dim machineID As Long
        Dim sMaterial As String
        Dim m_accumulator = New clsPickAccumulator
        Dim sIdName As String
        Dim bDoHeader As Boolean = True
        Dim sMy_DB_Name As String

        nMyFocusedRow = Nothing
        m_proplist = s_proplist
        m_proplist.Rows.Clear()

        sMy_DB_Name = db_name
        Try
            dtAttributes = ConnectToAppDB(sql)


            If (dtAttributes.Columns.Contains("DefnID") = True) Then

                sIdName = "DefnId"

            Else

                sIdName = "ModifyID"

            End If



            For i = 0 To dtAttributes.Rows.Count - 1
                my_DatVarName = CStr(dtAttributes.Rows(i).Item("InternalName"))
                my_DialogDisplay = CStr(dtAttributes.Rows(i).Item("Display"))
                my_RecodID = CInt(dtAttributes.Rows(i).Item("ID"))
                my_DataType = CInt(dtAttributes.Rows(i).Item("Type"))
                my_sOptions = CStr(dtAttributes.Rows(i).Item("Options"))
                my_DefnID = CInt(dtAttributes.Rows(i).Item(sIdName))



                my_VBValue = 0
                my_CValue = 0
                my_CVarName = my_DatVarName


                'my_sOptions = sEmpty

                If (IsHeaderVar(my_DataType)) Then

                    'lReturn = AddHeader(display, iname, data_type, record_id)
                    currentCategoryRow = CreateCategoryRow(m_proplist, my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    DB_HEADER,
                                                                    my_RecodID,
                                                                    my_sOptions, 0)
                    If (i < 3) Then
                        If (my_DataType = 0) Then
                            nMyFocusedRow = currentCategoryRow.Index + 1

                        End If
                    End If


                Else
                    Select Case my_DataType

                        Case DB_INT
                            currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(m_proplist,
                                                                                    my_DatVarName,
                                                                                    my_DialogDisplay,
                                                                                    my_VBValue, my_CValue,
                                                                                    my_CVarName,
                                                                                    my_DataType,
                                                                                    my_RecodID,
                                                                                    my_sOptions, 0))


                        Case DB_COMBO
                            arrOptions = GetOptionCollection(my_sOptions, 0)
                            Dim tempstr As String = GetOptionText2(my_sOptions, 0)

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                      my_DatVarName,
                                                                   my_DialogDisplay,
                                                                   tempstr,
                                                                   my_CValue,
                                                                   my_CVarName,
                                                                   my_DataType,
                                                                   my_RecodID,
                                                                   arrOptions, 0))

                        Case DB_BOOL

                            currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(m_proplist,
                                                                    my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    sEmpty, 0))



                        Case DB_STATION
                            currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(m_proplist,
                                                                    my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    my_sOptions, 0))
                        Case DB_MACHINE

                            toolSetupName = HeaderStringGet("MachCfg", "")
                            my_CValue = modTranslate.GetMachineID(toolSetupName)
                            my_VBValue = modTranslate.GetMachineDescription(my_CValue)
                            my_RecodID = my_CValue


                            arrMachine = PopulateComboList("SELECT * FROM [Machines] ORDER BY [Description]", True, False)

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                       my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    arrMachine, 0))

                        Case DB_MATERIAL
                            sMaterial = HeaderStringGet("MatCfg", "")

                            arrMaterial = PopulateComboList("SELECT * FROM [Material Inventory]", True, False)

                            my_CValue = modTranslate.GetMaterialIDFromDescription(sMaterial)
                            my_VBValue = sMaterial
                            my_RecodID = my_CValue

                            currentCategoryRow.ChildRows.Add(CreateMatListEditorRow(s_proplist,
                                                                                    my_DatVarName,
                                                                                    my_DialogDisplay,
                                                                                    my_VBValue,
                                                                                    my_CValue,
                                                                                    my_CVarName,
                                                                                    my_DataType,
                                                                                    my_RecodID,
                                                                                    my_sOptions, 0))

                            'currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                            '                                        my_DatVarName,
                            '                                        my_DialogDisplay,
                            '                                        my_VBValue,
                            '                                        my_CValue,
                            '                                        my_CVarName,
                            '                                        my_DataType,
                            '                                        my_RecodID,
                            '                                        arrMaterial, 0))

                        Case DB_TOOLSETUP

                            my_VBValue = HeaderStringGet("MachCfg", "")
                            machineID = modTranslate.GetMachineID(my_VBValue)
                            my_CValue = modTranslate.GetToolSetupIDFromDescription(my_VBValue)
                            my_RecodID = my_CValue

                            arrToolSetup = PopulateComboList("SELECT * FROM [Tool Setups] " &
                                                            "WHERE ([Machine ID]=" & machineID & ") ORDER BY [Description]", False, False)

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                       my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    arrToolSetup, 0))
                        Case DB_DIR, DB_SOLN

                            arrOptions = GetOptionCollection(my_sOptions, 0)
                            Dim tempstr As String = GetOptionText2(my_sOptions, -1)

                            If (tempstr = "CW") Then
                                my_CValue = -1
                                lReturn = UpdateAccumulatorDirect(m_proplist, my_CVarName, my_CValue)
                            ElseIf (tempstr = "CCW") Then
                                my_CValue = 1
                                lReturn = UpdateAccumulatorDirect(m_proplist, my_CVarName, my_CValue)
                            ElseIf (tempstr = Nothing) Then
                                tempstr = GetOptionText2(my_sOptions, 0)
                            End If
                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                        my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    tempstr,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    arrOptions, 0))

                        Case DB_CRIB
                            toolSetupName = HeaderStringGet("MachCfg", "")
                            nToolSetupID = GetToolSetupIDFromDescription(toolSetupName)  'RegGetString("Global", "ToolSetupID", 0)
                            my_CValue = 0
                            my_VBValue = "None"
                            my_RecodID = 0

                            sql = "SELECT [Station Configuration].[NC Code Number], [Tool Crib].Description, [Tool Setup Members].[ID]" &
                                " FROM [Tool Crib] INNER JOIN ([Station Configuration] INNER JOIN [Tool Setup Members] " &
                                "ON [Station Configuration].ID = [Tool Setup Members].[Station ID]) ON " &
                                "[Tool Crib].ID = [Tool Setup Members].[Tool ID]" &
                                " WHERE ((([Tool Setup Members].[Tool Setup ID]) = " & nToolSetupID & "))" &
                                " ORDER BY [Tool Crib].Description"

                            arrMachine = PopulateComboList(sql, False, True)

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                       my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    arrMachine, 4))

                        Case DB_USERVAR
                            arrOptions = GetOptionCollection(my_sOptions, 0)
                            Dim tempstr As String = GetOptionText2(my_sOptions, 0)

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                      my_DatVarName,
                                                                   my_DialogDisplay,
                                                                   tempstr,
                                                                   my_CValue,
                                                                   my_CVarName,
                                                                   my_DataType,
                                                                   my_RecodID,
                                                                   arrOptions, my_DefnID))
                            m_proplist.FocusLast()

                            lReturn = GetPropsForUserVar(m_proplist, m_proplist.FocusedRow, 0, my_DefnID)
                            lReturn = UpdateAccumulatorDirect(m_proplist, my_CVarName, 0)

                        Case DB_LEADSETUP
                            If (bDoHeader = True) Then
                                currentCategoryRow = CreateCategoryRow(m_proplist, my_DatVarName,
                                                                       my_DialogDisplay,
                                                                       my_VBValue,
                                                                       my_CValue,
                                                                       my_CVarName,
                                                                       DB_HEADER,
                                                                       my_RecodID,
                                                                       my_sOptions, 0)
                                bDoHeader = False
                            End If

                            arrOptions = PopulateComboList("SELECT * FROM [Lead Setup]", False, True)

                            Dim tempstr As String = "None"

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                      my_DatVarName,
                                                                   my_DialogDisplay,
                                                                   tempstr,
                                                                   my_CValue,
                                                                   my_CVarName,
                                                                   my_DataType,
                                                                   my_RecodID,
                                                                   arrOptions, 0))



                        Case DB_FILE
                            currentCategoryRow.ChildRows.Add(CreateButtonEditEditorRow(m_proplist,
                                                                    my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    my_sOptions, 0))

                        Case DB_MATLIST

                            currentCategoryRow.ChildRows.Add(CreateMatListEditorRow(s_proplist,
                                                                                    my_DatVarName,
                                                                                    my_DialogDisplay,
                                                                                    my_VBValue,
                                                                                    my_CValue,
                                                                                    my_CVarName,
                                                                                    my_DataType,
                                                                                    my_RecodID,
                                                                                    my_sOptions, 0))


                        Case Else
                            currentCategoryRow.ChildRows.Add(CreateStringEditorRow(m_proplist,
                                                        my_DatVarName,
                                                        my_DialogDisplay,
                                                        my_VBValue,
                                                        my_CValue,
                                                        my_CVarName,
                                                        my_DataType,
                                                        my_RecodID,
                                                        my_sOptions, 0))
                    End Select

                End If
            Next

            dtAttributes.Dispose()

            ' m_proplist.FocusedRow = m_proplist.Rows(nMyFocusedRow)
            'modView.ViewRegen()
            'm_proplist.Refresh()
        Catch ex As Exception

            MessageBox.Show("MyInitFromDB " & ex.Message)
        End Try


    End Function


    Public Function GetPropsForUserVar(s_proplist As DevExpress.XtraVerticalGrid.VGridControl, CurrentRow As DevExpress.XtraVerticalGrid.Rows.BaseRow, prop_indx As Long, my_DefnID As Long) As Long
        Dim dtProps As DataTable = Nothing
        Dim my_Sql As String
        Dim PropLinkID As Long
        Dim my_DatVarName As String
        Dim my_DialogDisplay As String
        Dim my_VBValue As String
        Dim my_CValue As String
        Dim my_CVarName As String
        Dim my_DataType As Integer
        Dim my_RecodID As Integer
        Dim my_sOptions As String
        Dim currentCategoryRow As CategoryRow
        Dim cRow As DevExpress.XtraVerticalGrid.Rows.BaseRow
        Dim my_ChildrowCount As Integer


        m_proplist = s_proplist
        currentCategoryRow = CurrentRow.ParentRow



        Try

            If (my_DefnID > 0) Then

                my_ChildrowCount = currentCategoryRow.ChildRows.Count

                'clear all rows after the current dropdown row so we can repopulate with new vars
                If (currentCategoryRow.ChildRows.Count > 1) Then

                    For ni = currentCategoryRow.ChildRows.Count - 1 To 1 Step -1
                        cRow = currentCategoryRow.ChildRows(ni)
                        currentCategoryRow.ChildRows.Remove(cRow)
                    Next


                End If

                PropLinkID = "1" & prop_indx & my_DefnID

                my_Sql = "SELECT * FROM DefnVar WHERE (DefnID = " & PropLinkID & ")"

                dtProps = ConnectToAppDB(my_Sql)

                For i = 0 To dtProps.Rows.Count - 1
                    my_RecodID = CInt(dtProps.Rows(i).Item("ID"))
                    my_DatVarName = CStr(dtProps.Rows(i).Item("InternalName"))
                    my_CVarName = my_DatVarName
                    my_DialogDisplay = CStr(dtProps.Rows(i).Item("Display"))
                    my_VBValue = CStr(dtProps.Rows(i).Item("Value"))
                    my_CValue = my_VBValue
                    my_DataType = CInt(dtProps.Rows(i).Item("Type"))
                    my_sOptions = CStr(dtProps.Rows(i).Item("Options"))
                    'my_DefnID = CInt(dtProps.Rows(i).Item("DefnID"))

                    Select Case my_DataType
                        'Case 4
                        '    currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(m_proplist, _
                        '                                            my_DatVarName, _
                        '                                            my_DialogDisplay, _
                        '                                            my_VBValue, _
                        '                                            my_CValue, _
                        '                                            my_CVarName, _
                        '                                            my_DataType, _
                        '                                            my_RecodID, _
                        '                                            my_sOptions))
                        Case DB_COMBO
                            arrOptions = GetOptionCollection(my_sOptions, 0)
                            Dim tempstr As String = GetOptionText2(my_sOptions, 0)

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                      my_DatVarName,
                                                                   my_DialogDisplay,
                                                                   tempstr,
                                                                   my_CValue,
                                                                   my_CVarName,
                                                                   my_DataType,
                                                                   my_RecodID,
                                                                   arrOptions, 0))

                        Case DB_BOOL

                            currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(m_proplist,
                                                                    my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    sEmpty, 0))



                        Case DB_STATION
                            currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(m_proplist,
                                                                    my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    my_sOptions, 0))

                        Case DB_DIR

                            arrOptions = GetOptionCollection(my_sOptions, 0)
                            Dim tempstr As String = GetOptionText2(my_sOptions, -1)

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                        my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    tempstr,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    arrOptions, 0))



                        Case DB_USERVAR
                            arrOptions = GetOptionCollection(my_sOptions, 0)
                            Dim tempstr As String = GetOptionText2(my_sOptions, 0)

                            currentCategoryRow.ChildRows.Add(CreateComboBoxListEditorRow(m_proplist,
                                                                      my_DatVarName,
                                                                   my_DialogDisplay,
                                                                   tempstr,
                                                                   my_CValue,
                                                                   my_CVarName,
                                                                   my_DataType,
                                                                   my_RecodID,
                                                                   arrOptions, my_DefnID))
                            '    'lReturn = GetPropsForUserVar(0, my_DefnID)

                        Case DB_BOOL
                            currentCategoryRow.ChildRows.Add(CreateBooleanEditorRow(m_proplist,
                                                                                    my_DatVarName,
                                                                                    my_DialogDisplay,
                                                                                    my_VBValue, my_CValue,
                                                                                    my_CVarName,
                                                                                    my_DataType,
                                                                                    my_RecodID,
                                                                                    my_sOptions, 0))

                        Case DB_INT
                            currentCategoryRow.ChildRows.Add(CreateIntegerEditorRow(m_proplist,
                                                                                    my_DatVarName,
                                                                                    my_DialogDisplay,
                                                                                    my_VBValue, my_CValue,
                                                                                    my_CVarName,
                                                                                    my_DataType,
                                                                                    my_RecodID,
                                                                                    my_sOptions, 0))

                        Case DB_DBL, DB_RAD, DB_ANG, DB_LEN
                            currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(m_proplist,
                                                                                   my_DatVarName,
                                                                                    my_DialogDisplay,
                                                                                    my_VBValue, my_CValue,
                                                                                    my_CVarName,
                                                                                    my_DataType,
                                                                                    my_RecodID,
                                                                                    my_sOptions, 0))

                            'Case DB_RAD
                            '    currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(m_proplist, _
                            '                                                           my_DatVarName, _
                            '                                                            my_DialogDisplay, _
                            '                                                            my_VBValue, my_CValue, _
                            '                                                            my_CVarName, _
                            '                                                            my_DataType, _
                            '                                                            my_RecodID, _
                            '                                                            my_sOptions, 0))

                            'Case DB_ANG
                            '    currentCategoryRow.ChildRows.Add(CreateDoubleEditorRow(m_proplist, _
                            '                                                           my_DatVarName, _
                            '                                                            my_DialogDisplay, _
                            '                                                            my_VBValue, my_CValue, _
                            '                                                            my_CVarName, _
                            '                                                            my_DataType, _
                            '                                                            my_RecodID, _
                            '                                                            my_sOptions, 0))


                        Case Else
                            currentCategoryRow.ChildRows.Add(CreateStringEditorRow(m_proplist,
                                                                    my_DatVarName,
                                                                    my_DialogDisplay,
                                                                    my_VBValue,
                                                                    my_CValue,
                                                                    my_CVarName,
                                                                    my_DataType,
                                                                    my_RecodID,
                                                                    my_sOptions, 0))
                    End Select



                Next

                dtProps.Dispose()

            End If

        Catch ex As Exception

        End Try

    End Function

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


            For Each row As CategoryRow In vgSettingsProplist.Rows

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

                    'vgridcontrol1.BeginDataUpdate()
                    childRow.Properties.Value = my_Value
                    'vgridcontrol1.EndDataUpdate()

                Next childRow

            Next
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Function

    Private Function IsHeaderVar(data_type As Long) As Boolean

        Select Case data_type
            Case DB_NOP,
                 DB_C2D,
                 DB_C3D

                IsHeaderVar = True

            Case Else

                IsHeaderVar = False

        End Select

    End Function

    Private Sub frmMatListTest_DoubleClick(sender As Object, e As EventArgs) Handles Me.DoubleClick

    End Sub

    Private Sub rbMaterials_PopupAllowClick(sender As Object, e As PopupAllowClickEventArgs) Handles rbMaterialSettings.PopupAllowClick

    End Sub

    Public Sub rbMaterialSettings_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs) Handles rbMaterialSettings.QueryDisplayText

        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")
    End Sub

End Class


