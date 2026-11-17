
Public Class InstalledToolService

    Public Shared Function GetInstalledTools(
        toolSetupID As Integer) As List(Of ToolSelectionRow)

        If toolSetupID <= 0 Then
            Return New List(Of ToolSelectionRow)()
        End If


        ' --- Identify the machine for this Tool Setup ---
        Dim toolSetup = AppData.ToolSetups.FirstOrDefault(
    Function(ts) ts.ID = toolSetupID)

        If toolSetup Is Nothing Then
            Return New List(Of ToolSelectionRow)()
        End If

        Dim machineID As Integer = toolSetup.MachineID


        Dim installedTools =
            (From member In AppData.ToolSetupMembers
             Join tool In AppData.ToolCrib
                 On member.ToolID Equals tool.ID
             Join station In AppData.StationsTable.Rows
                 On member.StationID Equals
                     station.Value(Of Integer)("ID")
             Where member.ToolSetupID = toolSetupID
             Order By member.OrderID,
         station.Value(Of Integer)("NCCodeNumber")
             Select New ToolSelectionRow With {
                 .ID = member.ID,
                 .ToolNumber = station.Value(Of Integer)("NCCodeNumber"),
                 .Description = tool.Description,
                 .Color = tool.Color,
                 .Show = True
             }).ToList()


        ' --- Identify stations without an installed tool ---
        Dim occupiedStationIDs As New HashSet(Of Integer)(
    AppData.ToolSetupMembers.
        Where(Function(m) m.ToolSetupID = toolSetupID).
        Select(Function(m) m.StationID)
)

        Dim emptyStations =
    AppData.StationsTable.Rows.
        Where(Function(s) _
            s.Value(Of Integer)("MachineID") = machineID AndAlso
            Not occupiedStationIDs.Contains(
                s.Value(Of Integer)("ID"))).
        OrderBy(Function(s) s.Value(Of Integer)("NCCodeNumber")).
        ToList()

        ' --- Append empty stations after installed tools ---
        For Each station In emptyStations

            installedTools.Add(New ToolSelectionRow With {
        .ID = 0,
       .ToolNumber = station.Value(Of Integer)("NCCodeNumber"),
        .Description = "",
        .Color = System.Drawing.Color.Empty,
        .Show = False,
        .IsEmptyStation = True
    })

        Next



        Return installedTools

    End Function

End Class
