Imports DevExpress.XtraTreeList

Imports DevExpress.XtraTreeList.Nodes

Public Class frmMAtTree
    Private CFG_ROOT_NODE As TreeListNode
    Private CFG_MACHINE_ROOT_NODE As TreeListNode
    Private CFG_MACHINE_TYPE_NODE As Nodes.TreeListNode
    Private CFG_MACHINE_NODE As TreeListNode
    Private CFG_STATION_ROOT_NODE As TreeListNode
    Private CFG_TOOL_SETUP_ROOT_NODE As TreeListNode
    Private CFG_TOOL_SETUP_NODE As TreeListNode
    Private CFG_LAYER_SETUP_ROOT_NODE As TreeListNode
    Private CFG_LAYER_SETUP_NODE As TreeListNode
    Private CFG_LAYER_MAP_NODE As TreeListNode
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

    Private Sub frmMAtTree_Load(sender As Object, e As EventArgs) Handles MyBase.Load

        tl.ClearNodes()
        tl.BeginUnboundLoad()

        CFG_ROOT_NODE = Nothing

        'CFG_MACHINE_ROOT_NODE = TL.AppendNode(New Object() {"Machine Types"}, 0)
        CFG_MATERIAL_ROOT_NODE = tl.AppendNode(New Object() {"Materials"}, CFG_ROOT_NODE, TAG_MATERIAL_ROOT_NODE)
        'CFG_OPERATION_ROOT_NODE = TL.AppendNode(New Object() {"Operations"}, CFG_ROOT_NODE, TAG_OPERATION_ROOT_NODE)
        'CFG_OPERATION_LEAD_ROOT_NODE = TL.AppendNode(New Object() {"Leads"}, CFG_OPERATION_ROOT_NODE, TAG_OPERATION_LEAD_ROOT_NODE)
        'CFG_OPERATION_LEAD_PARAM_ROOT_NODE = TL.AppendNode(New Object() {"Lead Parameters"}, CFG_OPERATION_LEAD_ROOT_NODE, TAG_OPERATION_LEAD_PARAM_ROOT_NODE)
        'CFG_OPERATION_LEAD_SETUP_ROOT_NODE = TL.AppendNode(New Object() {"Lead Setups"}, CFG_OPERATION_LEAD_ROOT_NODE, TAG_OPERATION_LEAD_SETUP_ROOT_NODE)
        CreateMaterialTypeNodes(tl)

        tl.EndUnboundLoad()

        popupContainerControl1.Controls.Add(tl)
        popupContainerEdit1.Properties.PopupControl = popupContainerControl1
        popupContainerEdit1.Properties.ShowPopupCloseButton = False
        AddHandler popupContainerEdit1.QueryDisplayText, AddressOf popupContainerEdit1_QueryDisplayText
        AddHandler popupContainerEdit1.QueryResultValue, AddressOf popupContainerEdit1_QueryResultValue

    End Sub
    Private Sub popupContainerEdit1_QueryResultValue(ByVal sender As Object, ByVal e As DevExpress.XtraEditors.Controls.QueryResultValueEventArgs)
        'e.Value = gridView1.GetFocusedRowCellValue("Fruit")
    End Sub

    Private Sub popupContainerEdit1_QueryDisplayText(ByVal sender As Object, ByVal e As DevExpress.XtraEditors.Controls.QueryDisplayTextEventArgs)
        e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")
    End Sub
    Private Sub CreateMaterialTypeNodes(ByVal tl As DevExpress.XtraTreeList.TreeList)
        Dim sql As String
        Dim ds As New System.Data.DataSet

        Dim s_MaterialCategory As String
        Dim n_MaterialID As Integer
        Dim i As Integer
        Dim dtMaterialType As System.Data.DataTable



        s_MaterialCategory = ""

        sql = "Select [Material Types].* From [Material Types]"

        Try

            dtMaterialType = ConnectToCMDB(sql)


            For i = 0 To dtMaterialType.Rows.Count - 1
                s_MaterialCategory = dtMaterialType.Rows(i).Item("Description")

                n_MaterialID = dtMaterialType.Rows(i).Item("ID")

                CFG_MATERIAL_CATEGORY_NODE = tl.AppendNode(New Object() {s_MaterialCategory}, CFG_MATERIAL_ROOT_NODE, TAG_MATERIAL_CATEGORY_NODE)

                FillMaterialNodes(n_MaterialID)

                dtMaterialType.Dispose()

            Next

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

                CFG_MATERIAL_NODE = tl.AppendNode(New Object() {s_MaterialDescription}, CFG_MATERIAL_CATEGORY_NODE, TAG_MATERIAL_NODE)

            Next

            dtMaterialNode.Dispose()

        Catch ex As Exception

            MessageBox.Show("FillMaterialNodes: " & ex.Message)

        End Try

    End Sub

    Private Sub tl_Click(sender As Object, e As EventArgs) Handles tl.Click

    End Sub

    Private Sub tl_AfterCheckNode(sender As Object, e As NodeEventArgs) Handles tl.AfterCheckNode
        MessageBox.Show("Hello")
    End Sub

    Private Sub tl_FocusedNodeChanged(sender As Object, e As FocusedNodeChangedEventArgs) Handles tl.FocusedNodeChanged
        Dim itemtype As Long
        Dim nCurr_Node_Tag As Long
        Dim sCurr_Display As String
        Dim nTestNode

        'Try

        If (e.Node IsNot Nothing) Then
            'itemtype = TreeItemType(e.Node)



            sCurr_Display = e.Node.GetDisplayText(tl.Columns(0))
            nCurr_Node_Tag = e.Node.Tag


            If (e.Node.ParentNode IsNot Nothing) Then
                popupContainerEdit1.EditValue = sCurr_Display ' e.Node.ParentNode.GetDisplayText(tl.Columns(1))
                popupContainerEdit1.ClosePopup()

            End If

            'HideControls(itemtype)
        End If
    End Sub

End Class