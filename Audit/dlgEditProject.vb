Imports DevExpress.XtraVerticalGrid.Rows
Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraEditors.Repository
Imports DevExpress.XtraTreeList.Nodes
Imports DevExpress.XtraTreeList
Imports DevExpress.XtraVerticalGrid.Events

Public Class dlgEditProject
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
    Private m_is_active As Boolean
    Private editValue As String
    Private nRowIndx As Integer
    Private nRowId As BaseRow
    Private nToolSetupRowId As BaseRow
    Private SelectedMachineName As String
    Private m_In_Edit_Mode As Boolean
    Private n_materialID As Long

    Private SelectedToolSetupName As String

    Private CFG_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_CATEGORY_NODE As TreeListNode
    Private Const TAG_MATERIAL_ROOT_NODE = -200
    Private Const TAG_MATERIAL_CATEGORY_NODE = -201
    Private Const TAG_MATERIAL_NODE = -202
    Private CFG_MATERIAL_NODE As TreeListNode


    Public WithEvents rbMaterialSettings As DevExpress.XtraEditors.Repository.RepositoryItemPopupContainerEdit

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

    Private Sub dlgEditProject_Activated(sender As Object, e As EventArgs) Handles Me.Activated
        m_curr_dlg = Me
    End Sub


    Private Sub dlgEditProject_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        m_is_active = False
        frmMain.CloseFirstPanel()
        modView.ViewResize()
    End Sub

    Private Sub sbAccept_Click(sender As Object, e As EventArgs) Handles sbAccept.Click
        Dim MatWidth As Double
        Dim dMatLength As Double
        Dim dMatThick As Double

        Me.vgSettingsProplist.FocusFirst()

        'MsgBox("HEY GREG, The fullnae should be < " & GetNewMaterialNameWithType(n_materialID) & " >")
        SetTheAccumulator()
        dMatLength = modMaterial.GetMaterialLength(n_materialID)
        MatWidth = modMaterial.GetMaterialWidth(n_materialID)
        dMatThick = modMaterial.GetMaterialThickness(n_materialID)

        modProject.SetMaterial(GetMaterialDescription(n_materialID), n_materialID)

        lReturn = HeaderDoubleSet("MaterialCost", modMaterial.GetMaterialCost(n_materialID))

        modProject.SetMaterialDims(dMatLength, MatWidth, dMatThick)

        lReturn = modEdit.Execute(2021, GetAccumulator)
        Set_In_Edit_Mode(False)
        modView.ViewRegen()


    End Sub
    Private Sub SetTheAccumulator()
        Dim sPieces() As String
        Dim iname As String
        Dim sOptions As String
        Dim nVarType As Integer
        Dim sValue As String = Nothing

        Try

            For Each row As CategoryRow In vgSettingsProplist.Rows
                Dim childRows As VGridRows = row.ChildRows

                For Each childRow As EditorRow In childRows

                    nVarType = CInt(GetValueFromDelimitedData(childRow.Tag, "VarDataType", ""))
                    iname = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")

                    sOptions = GetValueFromDelimitedData(childRow.Tag, "Options", "")

                    Dim sDatVarName As String

                    sDatVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")

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

                        Case DB_MATLIST

                            sPieces = Split(childRow.Properties.Value, ",")

                            If (UBound(sPieces) > 0) Then
                                n_materialID = modTranslate.GetMaterialIDFromDescription(sPieces(1))

                                sValue = n_materialID
                            Else

                                sValue = n_materialID

                            End If


                        Case Else
                            sValue = childRow.Properties.Value

                    End Select

                    lReturn = UpdateAccumulatorDirect(vgSettingsProplist, iname, sValue)
                Next
            Next

        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try


    End Sub
    Public Function Set_In_Edit_Mode(Mode As Boolean) As Long
        m_In_Edit_Mode = Mode
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
                            my_Value = GetNewMaterialNameWithType(materialID)

                        Case "MATERIAL LENGTH"
                            my_Value = Round(HeaderDoubleGet("Length", 0), Places.Thousandths, True)

                        Case "MATERIAL WIDTH"
                            my_Value = Round(HeaderDoubleGet("Width", 0), Places.Thousandths, True)

                        Case "MATERIAL THICKNESS"
                            my_Value = Round(HeaderDoubleGet("thickness", 0), Places.Thousandths, True)


                    End Select

                    vgSettingsProplist.BeginDataUpdate()
                    childRow.Properties.Value = my_Value
                    vgSettingsProplist.EndDataUpdate()

                Next childRow

            Next
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Function

    Private Sub vgSettingsProplist_CellValueChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgSettingsProplist.CellValueChanged
        Dim sPieces() As String
        Dim iname As String
        Dim sOptions As String
        Dim nVarType As Integer
        Dim sValue As String
        Dim childrow As BaseRow


        Try
            nVarType = CInt(GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "VarDataType", ""))
            iname = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "DatVarName", "")

            sOptions = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "Options", "")

            Dim sDatVarName As String

            sDatVarName = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "DatVarName", "")

            childrow = GetRowByCaptionChild(vgSettingsProplist.FocusedRow, vgSettingsProplist.FocusedRow.Properties.Caption)

            Select Case nVarType
                Case DB_DIR
                    If (e.Value = "CW") Then
                        sValue = -1
                    Else
                        sValue = 1
                    End If

                Case DB_MACHINE
                    sValue = GetMachineIDFromDescrip(e.Value)

                Case DB_TOOLSETUP
                    sValue = GetToolSetupIDFromDescription(e.Value)

                Case DB_COMBO, DB_USERVAR
                    sValue = GetOptionIndex(sOptions, e.Value)


                Case DB_CRIB

                    sValue = FindModelIDFromToolDescrip(e.Value)
                    g_clsActiveToolData = FindToolByModelID(CLng(sValue))
                    Name = GetEntityName(CLng(sValue))

                    SetActiveLayer(Name, g_clsActiveToolData.TlColor)

                Case DB_LEADSETUP
                    sValue = modTranslate.GetLeadSetupID(e.Value)

                Case DB_MATERIAL
                    'MessageBox.Show("We changed the value to  < " & e.Value & " >")
                    sValue = modTranslate.GetMaterialIDFromDescription(e.Value)


                Case DB_DBL
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetDouble("val", 0)

                        childrow.Properties.Value = Round(sValue, Places.TenThousandths)
                    Else
                        sValue = e.Value
                    End If

                Case DB_INT
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetInt("val", 0)

                        childrow.Properties.Value = sValue
                    Else
                        sValue = e.Value
                    End If
                Case DB_STR
                    sValue = e.Value

                Case DB_MATLIST
                    sPieces = Split(e.Value, ",")
                    'MessageBox.Show("We changed the value to  < " & e.Value & " >")

                    If (UBound(sPieces) > 1) Then
                        n_materialID = modTranslate.GetMaterialIDFromDescription(sPieces(1))

                        sValue = n_materialID
                    Else

                        sValue = n_materialID

                    End If
                    'MessageBox.Show("We changed the value to  < " & e.Value & " > So Greg says the Material ID should be something else so Lets chwck It < " & sValue & " >")

                Case DB_MAT_LEN
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetDouble("val", 0)

                        childrow.Properties.Value = Round(sValue, Places.TenThousandths)
                    Else
                        sValue = e.Value
                    End If
                Case DB_MAT_WID
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetDouble("val", 0)

                        childrow.Properties.Value = Round(sValue, Places.TenThousandths)
                    Else
                        sValue = e.Value
                    End If
                Case DB_MAT_THICK
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = PortalGetDouble("val", 0)

                        childrow.Properties.Value = Round(sValue, Places.TenThousandths)
                    Else
                        sValue = e.Value
                    End If

                Case Else
                    If (PortalExecute("*solve:eval:exp=" & QStr(e.Value)) > 0) Then

                        sValue = CStr(Round(PortalGetDouble("val", 0), Places.TenThousandths, False))

                        childrow.Properties.Value = sValue
                    Else
                        sValue = e.Value
                    End If

            End Select

            lReturn = UpdateAccumulatorDirect(vgSettingsProplist, iname, sValue)
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub

    Private Sub vgSettingsProplist_CellValueChanging(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgSettingsProplist.CellValueChanging
        Dim sDatVarName As String
        'Dim sDataType As Integer
        Dim nVarType As Integer
        Dim spieces() As String

        'Exit Sub
        nVarType = CInt(GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "VarDataType", ""))

        If (nVarType = DB_MATLIST) Then

            'MessageBox.Show("The dsataType is < " & nVarType & " >")
            spieces = Split(e.Value, ",")

            'MessageBox.Show("OK GReg this is getting frustrating  < " & e.Value & " >")

            n_materialID = modTranslate.GetMaterialIDFromDescription(spieces(1))

            'MessageBox.Show("OK this better be the right ID  < " & n_materialID & " >")

            Dim childRows As VGridRows = vgSettingsProplist.FocusedRow.ParentRow.ChildRows


            For Each childRow As EditorRow In vgSettingsProplist.FocusedRow.ParentRow.ChildRows

                sDatVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")
                Select Case UCase(sDatVarName)
                    Case "MAT_LEN"
                        childRow.Properties.Value = modMaterial.GetMaterialLength(n_materialID)

                    Case "MAT_WID"
                        childRow.Properties.Value = modMaterial.GetMaterialWidth(n_materialID)

                    Case "MAT_THICK"
                        childRow.Properties.Value = modMaterial.GetMaterialThickness(n_materialID)

                End Select

            Next

        End If
    End Sub

    Private Sub vgSettingsProplist_FocusedRowChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.FocusedRowChangedEventArgs) Handles vgSettingsProplist.FocusedRowChanged
        Dim sDatVarName As String
        Dim n_materialID As Long
        Dim nVarType As Integer
        Dim sPieces() As String

        nVarType = CInt(GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "VarDataType", ""))


        If (nVarType = DB_MATLIST) Then
            'MessageBox.Show("The dsataType is < " & nVarType & " >")

            n_materialID = modTranslate.GetMaterialIDFromDescription(e.Row.Properties.Value)


            sPieces = Split(e.Row.Properties.Value, ",")

            If (UBound(sPieces) > 0) Then
                n_materialID = modTranslate.GetMaterialIDFromDescription(sPieces(1))

            Else

                n_materialID = modTranslate.GetMaterialIDFromDescription(sPieces(0))

            End If

            Dim childRows As VGridRows = vgSettingsProplist.FocusedRow.ParentRow.ChildRows


            For Each childRow As EditorRow In vgSettingsProplist.FocusedRow.ParentRow.ChildRows

                sDatVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")

                If (n_materialID <> 0) Then
                    Select Case UCase(sDatVarName)
                        Case "MAT_LEN", "MATERIAL LENGTH"
                            childRow.Properties.Value = modMaterial.GetMaterialLength(n_materialID)

                        Case "MAT_WID", "MATERIAL WIDTH"
                            childRow.Properties.Value = modMaterial.GetMaterialWidth(n_materialID)

                        Case "MAT_THICK", "MATERIAL THICKNESS"
                            childRow.Properties.Value = modMaterial.GetMaterialThickness(n_materialID)

                    End Select

                End If

            Next

        End If

    End Sub

    Private Sub vgSettingsProplist_ShownEditor(sender As Object, e As EventArgs) Handles vgSettingsProplist.ShownEditor
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
        Dim edit As ComboBoxEdit = TryCast(sender, ComboBoxEdit)

        nToolSetupRowId = Nothing


        Try
            editValue = edit.EditValue.ToString()

            If (vgSettingsProplist.FocusedRow.Properties.Caption = "Machine") Then

                SelectedMachineName = edit.EditValue.ToString()
                m_machine_id = GetMachineIDFromDescrip(SelectedMachineName)

                nToolSetupRowId = GetRowByCaption("Tool Setup")

                If (IsNothing(nToolSetupRowId) = False) Then
                    nRowIndx = nToolSetupRowId.Index


                    vgSettingsProplist.FocusedRow = vgSettingsProplist.Rows(nToolSetupRowId.ParentRow.Index).ChildRows(nToolSetupRowId.Index)
                    vgSettingsProplist.FocusedRow.Properties.Value = ""
                    vgSettingsProplist.ShowEditor()
                End If
            End If
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try


    End Sub

    Private Function GetRowByCaption(ByVal s As String) As DevExpress.XtraVerticalGrid.Rows.BaseRow
        Dim r As DevExpress.XtraVerticalGrid.Rows.BaseRow
        Try
            For Each row As DevExpress.XtraVerticalGrid.Rows.BaseRow In vgSettingsProplist.Rows
                r = GetRowByCaptionChild(row, s)
                If Not r Is Nothing Then
                    Return r
                End If
            Next row
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
        Return Nothing
    End Function

    Private Function GetRowByCaptionChild(ByVal row As DevExpress.XtraVerticalGrid.Rows.BaseRow, ByVal s As String) As DevExpress.XtraVerticalGrid.Rows.BaseRow
        Try
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
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
        Return Nothing
    End Function

    Private Sub dlgEditProject_Load(sender As Object, e As EventArgs) Handles Me.Load
        Dim sql As String

        Try
            CreateMaterialTypeNodes(tl)


            popupContainerControl1.Controls.Add(tl)
            ' Used rbMaterials as the editor for Material record cell
            rbMaterialSettings = New RepositoryItemPopupContainerEdit()
            rbMaterialSettings.Name = "rbMaterials"
            rbMaterialSettings.PopupControl = popupContainerControl1
            AddHandler rbMaterialSettings.QueryDisplayText, AddressOf rbMaterialSettings_QueryDisplayText

            vgSettingsProplist.RepositoryItems.Add(rbMaterialSettings)

            m_sncs = 2021

            sql = "SELECT * FROM ModifyVar WHERE (ModifyID=" & m_sncs & ") Order By ModifyVar.ID"
            'Return = modPropList.InitFromDB(vgSettingsProplist, PathAppDB(), sql)


            populateFromAppDB(vgSettingsProplist, PathAppDB(), sql, currentCategoryRow, lbOptions)

            vgSettingsProplist.SuspendLayout()

            GetProjectInfo()

            vgSettingsProplist.Rows("categoryMaterial").ChildRows(0).Properties.RowEdit = rbMaterialSettings

            vgSettingsProplist.BestFit()
            vgSettingsProplist.ResumeLayout()

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub
    Public Shadows Function IsActive() As Boolean
        IsActive = m_is_active
    End Function

    Private Sub rbMaterialSettings_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs) Handles rbMaterialSettings.QueryDisplayText
        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")
    End Sub
    Private Sub CreateMaterialTypeNodes(ByVal tl As DevExpress.XtraTreeList.TreeList)
        Dim sql As String
        Dim ds As New System.Data.DataSet

        Dim s_MaterialCategory As String
        ' Dim n_MaterialID As Integer
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

                n_materialID = dtMaterialType.Rows(i).Item("ID")

                CFG_MATERIAL_CATEGORY_NODE = tl.AppendNode(New Object() {s_MaterialCategory}, CFG_MATERIAL_ROOT_NODE, TAG_MATERIAL_CATEGORY_NODE)

                FillMaterialNodes(n_materialID)

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

    Private Sub tl_FocusedNodeChanged(sender As Object, e As FocusedNodeChangedEventArgs) Handles tl.FocusedNodeChanged
        Dim sCurr_Display As String
        Dim sMaterialType As String
        Dim treeList As TreeList = TryCast(sender, TreeList)
        Dim sPieces() As String
        Dim childrow As BaseRow

        Try


            If (e.Node IsNot Nothing) Then


                If (e.Node.ParentNode IsNot Nothing) Then

                    sMaterialType = e.Node.ParentNode.GetDisplayText(tl.Columns(0))
                    sCurr_Display = sMaterialType & ", " & e.Node.GetDisplayText(tl.Columns(0))

                    sPieces = Split(sCurr_Display, ",")

                    If (sPieces.Length > 0) Then
                        n_materialID = CLng(GetMaterialIDFromDescription(sPieces(1)))
                    Else
                        n_materialID = CLng(GetMaterialIDFromDescription(sPieces(0)))
                    End If


                    childrow = GetRowByCaption("Material Length")

                    If (vgSettingsProplist.FocusedRow.Properties.Caption = "Material Type") Then
                        my_CValue = GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "CValue", "")

                        'nMaterialID = CLng(GetMaterialIDFromDescription(edit.EditValue))

                        childrow = GetRowByCaption("Material Length")

                        vgSettingsProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialLength(CLng(n_materialID))
                        m_accumulator.mat_len = CDbl(modMaterial.GetMaterialLength(CLng(n_materialID)))

                        childrow = GetRowByCaption("Material Width")
                        vgSettingsProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialWidth(CLng(n_materialID))
                        m_accumulator.mat_wid = CDbl(modMaterial.GetMaterialWidth(CLng(n_materialID)))

                        childrow = GetRowByCaption("Material thickness")
                        vgSettingsProplist.Rows(childrow.ParentRow.Index).ChildRows(childrow.Index).Properties.Value = modMaterial.GetMaterialThickness(CLng(n_materialID))
                        m_accumulator.mat_thick = CDbl(modMaterial.GetMaterialThickness(CLng(n_materialID)))

                    End If

                    Dim popupContainerControl = TryCast(treeList.Parent, PopupContainerControl)
                    popupContainerControl.OwnerEdit.EditValue = sCurr_Display
                    popupContainerControl.OwnerEdit.Tag = e.Node.Tag
                    popupContainerControl.OwnerEdit.ClosePopup()
                    'popupContainerEdit1.EditValue = sCurr_Display ' e.Node.ParentNode.GetDisplayText(tl.Columns(1))
                    'popupContainerEdit1.Tag = e.Node.Tag

                    'popupContainerEdit1.ClosePopup()
                End If
            End If

        Catch ex As Exception

            MessageBox.Show("tl_FocusedNodeChanged: " & ex.Message)

        End Try
    End Sub

    Private Sub vgSettingsProplist_Click(sender As Object, e As EventArgs) Handles vgSettingsProplist.Click
        Dim sDatVarName As String
        'Dim nMatID As Long
        Dim nVarType As Integer

        nVarType = CInt(GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "VarDataType", ""))


        If (nVarType = DB_MATLIST) Then

            'nMatID = modTranslate.GetMaterialIDFromDescription(e.Row.Properties.Value)

            Dim childRows As VGridRows = vgSettingsProplist.FocusedRow.ParentRow.ChildRows


            For Each childRow As EditorRow In vgSettingsProplist.FocusedRow.ParentRow.ChildRows

                sDatVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")

                If (n_materialID <> 0) Then
                    Select Case UCase(sDatVarName)
                        Case "MAT_LEN", "MATERIAL LENGTH"
                            childRow.Properties.Value = modMaterial.GetMaterialLength(n_materialID)

                        Case "MAT_WID", "MATERIAL WIDTH"
                            childRow.Properties.Value = modMaterial.GetMaterialWidth(n_materialID)

                        Case "MAT_THICK", "MATERIAL THICKNESS"
                            childRow.Properties.Value = modMaterial.GetMaterialThickness(n_materialID)

                    End Select

                End If

            Next

        End If
    End Sub

    Private Sub sbCancel_Click(sender As Object, e As EventArgs) Handles sbCancel.Click
        frmMain.CloseFirstPanel()
        Me.Dispose()
    End Sub

    Private Sub vgSettingsProplist_MouseEnter(sender As Object, e As EventArgs) Handles vgSettingsProplist.MouseEnter
        'Dim sDatVarName As String
        'Dim nMatID As Long
        'Dim nVarType As Integer

        'nVarType = CInt(GetValueFromDelimitedData(vgSettingsProplist.FocusedRow.Tag, "VarDataType", ""))


        'If (nVarType = DB_MATLIST) Then
        '    MessageBox.Show("Found Material CLick >")

        '    ' nMatID = modTranslate.GetMaterialIDFromDescription(e.Row.Properties.Value)

        '    Dim childRows As VGridRows = vgSettingsProplist.FocusedRow.ParentRow.ChildRows


        '    For Each childRow As EditorRow In vgSettingsProplist.FocusedRow.ParentRow.ChildRows

        '        sDatVarName = GetValueFromDelimitedData(childRow.Tag, "DatVarName", "")

        '        If (n_materialID <> 0) Then
        '            Select Case UCase(sDatVarName)
        '                Case "MAT_LEN", "MATERIAL LENGTH"
        '                    childRow.Properties.Value = modMaterial.GetMaterialLength(n_materialID)

        '                Case "MAT_WID", "MATERIAL WIDTH"
        '                   'childRow.Properties.Value = modMaterial.GetMaterialWidth(n_materialID)

        '                Case "MAT_THICK", "MATERIAL THICKNESS"
        '                    childRow.Properties.Value = modMaterial.GetMaterialThickness(n_materialID)

        '            End Select

        '        End If

        '    Next

        'End If

    End Sub

End Class