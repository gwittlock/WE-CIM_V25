
Public Class InstalledToolService

    Public Shared Function GetInstalledTools(
        toolSetupID As Integer) As List(Of ToolSelectionRow)

        If toolSetupID <= 0 Then
            Return New List(Of ToolSelectionRow)()
        End If

        Dim installedTools =
            (From member In AppData.ToolSetupMembers
             Join tool In AppData.ToolCrib
                 On member.ToolID Equals tool.ID
             Join station In AppData.StationsTable.Rows
                 On member.StationID Equals
                     station.Value(Of Integer)("ID")
             Where member.ToolSetupID = toolSetupID
             Order By station.Value(Of Integer)("NCCodeNumber")
             Select New ToolSelectionRow With {
                 .ID = member.ID,
                 .ToolNumber = station.Value(Of Integer)("NCCodeNumber"),
                 .Description = tool.Description,
                 .Color = tool.Color,
                 .Show = True
             }).ToList()

        Return installedTools

    End Function

End Class
