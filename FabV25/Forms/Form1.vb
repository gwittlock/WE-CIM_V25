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


Public Class Form1
        Inherits Form
        Private editValue As String
        Private nRowIndx As Integer
        Private nRowId As BaseRow

        Private CFG_ROOT_NODE As TreeListNode
        Private CFG_MATERIAL_ROOT_NODE As TreeListNode
        Private CFG_MATERIAL_CATEGORY_NODE As TreeListNode
        Private Const TAG_MATERIAL_ROOT_NODE = -200
        Private Const TAG_MATERIAL_CATEGORY_NODE = -201
        Private Const TAG_MATERIAL_NODE = -202
        Private CFG_MATERIAL_NODE As TreeListNode
        Private WithEvents rbMaterials As DevExpress.XtraEditors.Repository.RepositoryItemPopupContainerEdit

        Public Sub New()
            InitializeComponent()


            'Delimiters for formatting internal strings.
            CHR1 = Convert.ToChar(1)
            CHR2 = Convert.ToChar(2)

        End Sub
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
        Public CHR1 As String
        Public CHR2 As String




        Private Sub buttonEdit1_ButtonClick(ByVal sender As Object, ByVal e As DevExpress.XtraEditors.Controls.ButtonPressedEventArgs)
            Dim openFileDialog1 As New OpenFileDialog()


            openFileDialog1.Filter = "dat files (*.dat)|*.dat|All files (*.*)|*.*"
            openFileDialog1.FilterIndex = 1
            openFileDialog1.RestoreDirectory = True

            If openFileDialog1.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                fileName = openFileDialog1.FileName
                'buttonEdit1.EditValue = fileName
            End If
        End Sub


        Private Function CreateCategoryRow(ByVal rowCaption As String) As CategoryRow
            Dim categoryRow As New CategoryRow(rowCaption)
            vGridControl1.Rows.Add(categoryRow)
            Return categoryRow
        End Function

        Private Function CreateStringEditorRow(ByVal rowCaption As String, ByVal rowValue As String) As EditorRow
            Dim editorRow As New EditorRow()
            ' assigning the name and caption to the created editor row            
            editorRow.Properties.Caption = rowCaption
            editorRow.Properties.Value = rowValue
            Return editorRow
        End Function

        Private Function CreateBooleanEditorRow(ByVal rowCaption As String, ByVal rowValue As Integer) As EditorRow
            Dim editorRow As New EditorRow()
            ' assigning the name and caption to the created editor row
            editorRow.Properties.Caption = rowCaption
            editorRow.Properties.Value = If(rowValue = 1, True, False)
            Return editorRow
        End Function

        Private Function CreateIntegerEditorRow(ByVal rowCaption As String, ByVal rowValue As Integer) As EditorRow
            Dim editorRow As New EditorRow()
            ' assigning the name and caption to the created editor row
            editorRow.Properties.Caption = rowCaption
            editorRow.Properties.Value = rowValue

            Dim reSpinEdit As New RepositoryItemSpinEdit()
            reSpinEdit.IsFloatValue = False
            vGridControl1.RepositoryItems.Add(reSpinEdit)
            editorRow.Properties.RowEdit = reSpinEdit
            Return editorRow
        End Function

        Private Function CreateDoubleEditorRow(ByVal rowCaption As String, ByVal rowValue As Double) As EditorRow
            Dim editorRow As New EditorRow()
            ' assigning the name and caption to the created editor row
            editorRow.Properties.Caption = rowCaption
            editorRow.Properties.Value = rowValue

            Dim reCalcEdit As New RepositoryItemCalcEdit()
            vGridControl1.RepositoryItems.Add(reCalcEdit)
            editorRow.Properties.RowEdit = reCalcEdit
            Return editorRow
        End Function
        Public Function CreateComboBoxListEditorRow(ByRef mGridCtrl As DevExpress.XtraVerticalGrid.VGridControl,
                                                   ByVal DatVarName As String,
                                                   DialogDisplay As String,
                                                   VBValue As String,
                                                   CValue As String,
                                                   CVArName As String,
                                                   DataType As Integer,
                                                   RecodID As Integer,
                                                   Options As Collection) As EditorRow

            'ByVal rowCaption As String, _
            'ByVal sIname As String, _
            'ByVal nRecordID As Integer, _
            'ByVal rowValue As String, _
            'ByVal options As Collection) As EditorRow
            Dim sOptions As String = Nothing

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

            editorRow.Tag = EncodeTag(DatVarName, DialogDisplay, VBValue, CValue, CVArName, DataType, RecodID, sOptions)

            Return editorRow
        End Function

        Private Function CreateComboBoxEditorRow(ByVal stringArray() As String) As EditorRow
            Dim editorRow As New EditorRow()
            ' assigning the name and caption to the created editor row
            editorRow.Properties.Caption = stringArray(2)
            editorRow.Properties.Value = If(stringArray.Length > 3, stringArray(3), "")

            Dim reComboBoxEdit As New RepositoryItemComboBox()

            vGridControl1.RepositoryItems.Add(reComboBoxEdit)
            editorRow.Properties.RowEdit = reComboBoxEdit
            For i As Integer = 4 To stringArray.Length - 1
                reComboBoxEdit.Items.Add(stringArray(i))
            Next i
            Return editorRow
        End Function


        Public Function EncodeTag(DatVarname As String,
                           DialogDisplay As String,
                           VBValue As String,
                           CValue As String,
                           CVarName As String,
                           VarDataType As Integer,
                           nRecordID As Integer,
                           sOptions As String) As String

            EncodeTag = CHR1 & "DatVarName" & CHR2 & DatVarname &
                        CHR1 & "DialogDisplay" & CHR2 & DialogDisplay &
                        CHR1 & "VBValue" & CHR2 & VBValue &
                        CHR1 & "CValue" & CHR2 & CValue &
                        CHR1 & "CVarName" & CHR2 & CVarName &
                        CHR1 & "VarDataType" & CHR2 & VarDataType &
                        CHR1 & "nRecordID" & CHR2 & nRecordID &
                        CHR1 & "Options" & CHR2 & sOptions


        End Function

        Private Sub vGridControl1_ShownEditor(sender As Object, e As EventArgs) Handles vGridControl1.ShownEditor
            Dim edit As ComboBoxEdit = TryCast(vGridControl1.ActiveEditor, ComboBoxEdit)
            If nRowId IsNot Nothing Then
                If vGridControl1.FocusedRow Is nRowId Then
                    edit.Properties.Items.Clear()
                    edit.Properties.Items.Add(New ComboBoxItem(editValue))
                    edit.Properties.Items.Add(New ComboBoxItem(editValue))
                End If
            End If
            If edit IsNot Nothing Then
                AddHandler edit.EditValueChanged, AddressOf edit_EditValueChanged
            End If
        End Sub

        Private Sub edit_EditValueChanged(sender As Object, e As EventArgs)
            Dim edit As ComboBoxEdit = TryCast(sender, ComboBoxEdit)


            nRowId = GetRowByCaption("Tool Setup")

            editValue = edit.EditValue.ToString()

            nRowIndx = nRowId.Index


            vGridControl1.FocusedRow = vGridControl1.Rows(nRowId.ParentRow.Index).ChildRows(nRowId.Index)
            vGridControl1.ShowEditor()
        End Sub


        Private Function GetRowByCaption(ByVal s As String) As DevExpress.XtraVerticalGrid.Rows.BaseRow
            Dim r As DevExpress.XtraVerticalGrid.Rows.BaseRow
            For Each row As DevExpress.XtraVerticalGrid.Rows.BaseRow In vGridControl1.Rows
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

            CreateMaterialTypeNodes(tl)


            popupContainerControl1.Controls.Add(tl)
            'popupContainerEdit1.Properties.PopupControl = popupContainerControl1
            'popupContainerEdit1.Properties.ShowPopupCloseButton = False
            'AddHandler popupContainerEdit1.QueryDisplayText, AddressOf popupContainerEdit1_QueryDisplayText

            ' Used rbMaterials as the editor for Material record cell
            rbMaterials = New RepositoryItemPopupContainerEdit()
            rbMaterials.Name = "rbMaterials"
            rbMaterials.PopupControl = popupContainerControl1
            AddHandler rbMaterials.QueryDisplayText, AddressOf rbMaterials_QueryDisplayText

            vGridControl1.RepositoryItems.Add(rbMaterials)
            populateFromDatFile(vGridControl1, Application.ExecutablePath & "\nestparams.dat", currentCategoryRow, lbOptions)

            vGridControl1.Rows("categoryMaterial_Inventory").ChildRows(0).Properties.RowEdit = rbMaterials
        End Sub

        Private Sub rbMaterials_QueryDisplayText(sender As Object, e As QueryDisplayTextEventArgs) Handles rbMaterials.QueryDisplayText
            e.DisplayText = If(e.EditValue IsNot Nothing, e.EditValue.ToString(), "")

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
                        'popupContainerEdit1.EditValue = sCurr_Display ' e.Node.ParentNode.GetDisplayText(tl.Columns(1))
                        'popupContainerEdit1.Tag = e.Node.Tag

                        'popupContainerEdit1.ClosePopup()
                    End If
                End If

            Catch ex As Exception

                MessageBox.Show("tl_FocusedNodeChanged: " & ex.Message)

            End Try

        End Sub
    End Class


