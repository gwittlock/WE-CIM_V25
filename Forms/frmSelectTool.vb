Imports DevExpress.XtraGrid.Views.Base

Public Class frmSelectTool
    Private Sub frmSelectTool_Load(
    sender As Object,
    e As EventArgs) Handles MyBase.Load

        LoadInstalledTools()



    End Sub

    Public Event ToolAssignmentRequested(toolSetupMemberID As Integer)

    Private Sub LoadInstalledTools()

        If CurrentJob Is Nothing Then Exit Sub

        Dim setupID As Integer = CurrentJob.ToolSetupID

        Dim installedTools =
        (From member In AppData.ToolSetupMembers
         Join tool In AppData.ToolCrib
             On member.ToolID Equals tool.ID
         Join station In AppData.StationsTable.Rows
             On member.StationID Equals
                 station.Value(Of Integer)("ID")
         Where member.ToolSetupID = setupID
         Order By station.Value(Of Integer)("NCCodeNumber")
         Select New ToolSelectionRow With {
             .ID = member.ID,
             .ToolNumber = station.Value(Of Integer)("NCCodeNumber"),
             .Description = tool.Description,
             .Color = tool.Color,
             .Show = True
         }).ToList()

        grdToolLayer.DataSource = installedTools

        gViewToolLayer.RefreshData()


    End Sub



    Private Sub gViewToolLayer_RowStyle(
    sender As Object,
    e As DevExpress.XtraGrid.Views.Grid.RowStyleEventArgs
) Handles gViewToolLayer.RowStyle

        If e.RowHandle < 0 Then Return

        Dim view As DevExpress.XtraGrid.Views.Grid.GridView =
        CType(sender, DevExpress.XtraGrid.Views.Grid.GridView)

        Dim toolColor As Object =
        view.GetRowCellValue(e.RowHandle, "Color")

        If toolColor Is Nothing OrElse
       toolColor Is DBNull.Value Then Return

        If TypeOf toolColor Is System.Drawing.Color Then

            e.Appearance.ForeColor =
            CType(toolColor, System.Drawing.Color)

            e.HighPriority = True

        End If
        e.Appearance.BackColor = frmMain.pnlPicmodeler.BackColor    'ColorTranslator.FromOle(RegGetString("Preferences\ViewOptions", "MaterialColor", System.Drawing.Color.Gray))
    End Sub

    Private Sub SetAllToolVisibility(visible As Boolean)

        Dim rows =
        TryCast(grdToolLayer.DataSource,
                List(Of ToolSelectionRow))

        If rows Is Nothing Then Return

        For Each row As ToolSelectionRow In rows
            row.Show = visible
        Next

        gViewToolLayer.RefreshData()

    End Sub

    Private Sub sbShowAll_Click(sender As Object, e As EventArgs) Handles sbShowAll.Click

        SetAllToolVisibility(True)

    End Sub

    Private Sub sbHideAll_Click(sender As Object, e As EventArgs) Handles sbHideAll.Click

        SetAllToolVisibility(False)

    End Sub

    Private Sub sbAssign_Click(sender As Object, e As EventArgs) Handles sbAssign.Click

        Dim selectedRow As ToolSelectionRow =
        TryCast(gViewToolLayer.GetFocusedRow(),
                ToolSelectionRow)

        If selectedRow Is Nothing Then

            MessageBox.Show(
            "Please select a tool.",
            "Assign Tool",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)

            Return

        End If

        Dim selectedMemberID As Integer = selectedRow.ID

        MessageBox.Show(
        "Selected Tool Number: " &
        selectedRow.ToolNumber.ToString() &
        vbCrLf &
        "ToolSetupMemberID: " &
        selectedMemberID.ToString(),
        "Tool Selection Test",
        MessageBoxButtons.OK,
        MessageBoxIcon.Information)

        RaiseEvent ToolAssignmentRequested(selectedMemberID)

    End Sub

    Private Sub gViewToolLayer_RowCellDefaultAlignment(sender As Object, e As RowCellAlignmentEventArgs) Handles gViewToolLayer.RowCellDefaultAlignment

    End Sub
End Class

Public Class ToolSelectionRow

    Public Property ID As Integer
    Public Property ToolNumber As Integer
    Public Property Description As String
    Public Property Color As System.Drawing.Color
    Public Property Show As Boolean = True

End Class
