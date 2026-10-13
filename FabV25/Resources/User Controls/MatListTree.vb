
Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraTreeList.Nodes

Public Class MatListTree
    Private CFG_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_ROOT_NODE As TreeListNode
    Private CFG_MATERIAL_CATEGORY_NODE As TreeListNode
    Private CFG_MATERIAL_NODE As TreeListNode
    Private CFG_OPERATION_ROOT_NODE As TreeListNode
    Private CFG_OPERATION_LEAD_ROOT_NODE As TreeListNode
    Private CFG_OPERATION_LEAD_PARAM_ROOT_NODE As TreeListNode
    Private CFG_OPERATION_LEAD_PARAM_NODE As TreeListNode
    Private CFG_OPERATION_LEAD_SETUP_ROOT_NODE As TreeListNode
    Private CFG_OPERATION_LEAD_SETUP_NODE As TreeListNode
    Private Const TAG_ROOT_NODE = 0
    Private Const TAG_MACHINE_ROOT_NODE = -100
    Private Const TAG_MACHINE_NODE = -101
    Private Const TAG_STATION_ROOT_NODE = -102
    Private Const TAG_TOOL_SETUP_ROOT_NODE = -103
    Private Const TAG_TOOL_SETUP_NODE = -104
    Private Const TAG_LAYER_SETUP_ROOT_NODE = -105
    Private Const TAG_LAYER_SETUP_NODE = -106
    Private Const TAG_LAYER_MAP_NODE = -107
    Private Const TAG_MATERIAL_ROOT_NODE = -200
    Private Const TAG_MATERIAL_CATEGORY_NODE = -201
    Private Const TAG_MATERIAL_NODE = -202
    Private Const TAG_OPERATION_ROOT_NODE = -300
    Private Const TAG_OPERATION_LEAD_ROOT_NODE = -301
    Private Const TAG_OPERATION_LEAD_PARAM_ROOT_NODE = -302
    Private Const TAG_OPERATION_LEAD_PARAM_NODE = -303
    Private Const TAG_OPERATION_LEAD_SETUP_ROOT_NODE = -304
    Private Const TAG_OPERATION_LEAD_SETUP_NODE = -305


    Private sPieces() As String
    Private sMatPieces() As String
    Private sMaterialWidth As String
    Private sMaterialLength As String
    Private sMaterialThickness As String
    Private bHeaderDone As Boolean

    Dim dCheckX As Double
    Dim dCheckY As Double
    Dim sUsage As String
    Public MatListProps As New List(Of clsMatList)()
    Public dTheMaterialLength As Decimal
    Public dTheMaterialWidth As Decimal
    Public dTheMaterialThickness As Decimal
    Public nTheMaterialID As Integer
    Public sTheMatCategoryName As String
    Public sTheMaterialName As String
    Public nTheMatCategoryID As Integer


    Private Sub CreateMaterialTypeNodes(ByVal tl As DevExpress.XtraTreeList.TreeList)
        Dim sql As String
        Dim ds As New System.Data.DataSet

        Dim s_MaterialCategory As String

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
                    nTheMaterialID = dtMaterialType.Rows(i).Item("ID")

                    CFG_MATERIAL_CATEGORY_NODE = tl.AppendNode(New Object() {s_MaterialCategory}, CFG_MATERIAL_ROOT_NODE, TAG_MATERIAL_CATEGORY_NODE)

                    FillMaterialNodes(nTheMaterialID)

                    dtMaterialType.Dispose()
                End If

            Next

            tl.EndUnboundLoad()



        Catch ex As Exception

            MessageBox.Show("CreateMaterialTypeNodes: " & ex.Message)

        End Try

    End Sub


    Private Sub FillMaterialNodes(n_TypeMaterialID As Long)
        Dim sql As String
        Dim ds As New System.Data.DataSet
        Dim s_MaterialDescription As String
        Dim i As Integer
        Dim dtMaterialNode As System.Data.DataTable

        s_MaterialDescription = ""

        sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & n_TypeMaterialID & " )"

        Try

            dtMaterialNode = ConnectToCMDB(sql)

            For i = 0 To dtMaterialNode.Rows.Count - 1
                s_MaterialDescription = dtMaterialNode.Rows(i).Item("Description")
                nTheMaterialID = dtMaterialNode.Rows(i).Item("ID")




                CFG_MATERIAL_NODE = tl.AppendNode(New Object() {s_MaterialDescription}, CFG_MATERIAL_CATEGORY_NODE, TAG_MATERIAL_NODE & "," & nTheMaterialID)

            Next
            tl.ExpandAll()

            dtMaterialNode.Dispose()

        Catch ex As Exception

            MessageBox.Show("FillMaterialNodes: " & ex.Message)

        End Try

    End Sub

    Private Sub popupContainerEdit1_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs) Handles popupContainerEdit1.QueryDisplayText
        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")
    End Sub

    Private Sub tl_FocusedNodeChanged(sender As Object, e As DevExpress.XtraTreeList.FocusedNodeChangedEventArgs) Handles tl.FocusedNodeChanged

        Dim nCurr_Node_Tag As String
        Dim sCurr_Display As String
        Dim sMaterialType As String
        Dim sPieces()
        'Dim MatListProps As New List(Of clsMatList)()

        Dim s_MaterialDescription As String

        Try


            If (e.Node IsNot Nothing) Then


                If (e.Node.ParentNode IsNot Nothing) Then
                    ' MsgBox("The listindex is < " & e.Node.Id & " >")
                    sMaterialType = e.Node.ParentNode.GetDisplayText(tl.Columns(0))
                    sPieces = Split(e.Node.Tag, ",")
                    nTheMaterialID = sPieces(1)
                    s_MaterialDescription = modTranslate.GetMaterialDescription(nTheMaterialID)
                    MatListProps.Add(New clsMatList() With {.dMaterialLength = modMaterial.GetMaterialLength(nTheMaterialID),
                                                            .dMaterialWidth = modMaterial.GetMaterialWidth(nTheMaterialID),
                                                            .dMaterialThickness = modMaterial.GetMaterialThickness(nTheMaterialID),
                                                            .nMaterialID = nTheMaterialID,
                                                            .sMatListIndex = e.Node.Id,
                                                            .sMatCategoryName = sMaterialType,
                                                            .sMaterialName = s_MaterialDescription,
                                                            .nMatCategoryID = GetMaterialTypeID(sMaterialType)})

                    SetMatPropValues(nTheMaterialID)

                    '.GetDisplayText(tl.Columns(0))
                    '.nMatCategoryID = dtMaterialNode.Rows(i).Item("Type ID"),
                    ' GetMaterialTypeName(dtMaterialNode.Rows(i).Item("Type ID")),

                    sCurr_Display = sMaterialType & ", " & e.Node.GetDisplayText(tl.Columns(0))
                    nCurr_Node_Tag = e.Node.Tag

                    popupContainerEdit1.EditValue = sCurr_Display ' e.Node.ParentNode.GetDisplayText(tl.Columns(1))
                    popupContainerEdit1.Tag = sCurr_Display 'e.Node.Tag


                    dTheMaterialLength = MatListProps(0).dMaterialLength
                    RegPutString("Global", "Material", e.Node.GetDisplayText(tl.Columns(0)))
                    RegPutString("Global", "MaterialID", nTheMaterialID)
                    RegPutString("Global", "MaterialLength", modMaterial.GetMaterialLength(nTheMaterialID))
                    RegPutString("Global", "MaterialThickness", modMaterial.GetMaterialThickness(nTheMaterialID))
                    RegPutString("Global", "MaterialWidth", modMaterial.GetMaterialWidth(nTheMaterialID))


                    popupContainerEdit1.ClosePopup()

                End If
            End If

        Catch ex As Exception

            MessageBox.Show("tl_FocusedNodeChanged: " & ex.Message)

        End Try
    End Sub


    Private Sub popupContainerEdit1_Validated(sender As Object, e As EventArgs) Handles popupContainerEdit1.Validated

        'MsgBox("The material ID is < " & e.ToString)

    End Sub

    Private Sub MatListTree_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        CreateMaterialTypeNodes(tl)

        popupContainerControl1.Controls.Add(tl)
        popupContainerEdit1.Properties.PopupControl = popupContainerControl1
        popupContainerEdit1.Properties.ShowPopupCloseButton = False
        AddHandler popupContainerEdit1.QueryDisplayText, AddressOf popupContainerEdit1_QueryDisplayText

        'LoadMaterialTypes("Nothing")

    End Sub

    Private Sub LoadMaterialTypes(s_CompareStr As String)
        Dim sql As String
        Dim dtMaterialType As DataTable = Nothing

        Dim dtMaterials As DataTable = Nothing
        Dim s_MaterialCategory As String

        'cbeMaterialType.Properties.Items.Clear()

        sql = "Select [Material Types].* From [Material Types] Where ([Material Types].ID > 0 ) Order By [Material Types].Display"
        dtMaterialType = ConnectToCMDB(sql)

        For i = 0 To dtMaterialType.Rows.Count - 1

            s_MaterialCategory = dtMaterialType.Rows(i).Item("Description")
            nTheMaterialID = dtMaterialType.Rows(i).Item("ID")

            'If (InStr(s_MaterialCategory, s_CompareStr) > 0) Then

            'cbeMaterialType.Properties.Items.Add(New myItemData(s_MaterialCategory, nTheMaterialID))

            'End If

        Next

        'cbeMaterialType.SelectedIndex = 0

        DatabaseClose()

        dtMaterialType.Dispose()

        dtMaterialType = Nothing

        'rgProductType.SelectedIndex = 0

    End Sub


    Public Sub SetMatPropValues(lMaterialID As Long)
        Dim sMaterialTYpeAndSize As String

        sMaterialTYpeAndSize = GetMaterialTypeName(GetMaterialType(lMaterialID))
        sMaterialTYpeAndSize = sMaterialTYpeAndSize & "," & GetMaterialDescription(lMaterialID)
        popupContainerEdit1.ToolTip = sMaterialTYpeAndSize
        popupContainerEdit1.Text = sMaterialTYpeAndSize
        popupContainerEdit1.Tag = TAG_MATERIAL_NODE & "," & lMaterialID
        dTheMaterialLength = modMaterial.GetMaterialLength(lMaterialID)
        dTheMaterialWidth = modMaterial.GetMaterialWidth(lMaterialID)
        dTheMaterialThickness = modMaterial.GetMaterialThickness(lMaterialID)
        sTheMaterialName = GetMaterialDescription(lMaterialID)
        sTheMatCategoryName = modMaterial.GetMaterialTypeName(lMaterialID)


    End Sub

    public Sub popupContainerEdit1_EditValueChanged(sender As Object, e As EventArgs) Handles popupContainerEdit1.EditValueChanged

        SetMatPropValues(nTheMaterialID)
    End Sub
End Class
