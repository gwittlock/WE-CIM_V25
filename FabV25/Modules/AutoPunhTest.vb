Imports System.Drawing
Imports FabV25_WIN8.PunchResult
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.AutoPunchingEngine

Module AutoPunchTest
    Public Sub RunTest()
        ' Ensure ToolSetupMembers is initialized
        If ToolSetup.ToolSetupMembers Is Nothing Then
            ToolSetup.ToolSetupMembers = AppData.LoadTableWrapped(Of ToolSetupMember)("ToolSetupMembers.json")
        End If

        ' Fall back to empty list if still nothing
        Dim allMembers As List(Of ToolSetupMember) = ToolSetup.ToolSetupMembers
        If allMembers Is Nothing Then
            allMembers = New List(Of ToolSetupMember)
        End If

        ' Filter members with StationID > 0
        Dim members As List(Of ToolSetupMember) = allMembers _
        .Where(Function(m) m.StationID > 0).ToList()

        ' If it’s just local to the test method
        Dim _testPolylines As New List(Of Polyline2D)

        ' Example: add a few dummy polylines for testing
        _testPolylines.Add(New Polyline2D(New List(Of PointF) From {
    New PointF(0, 0),
    New PointF(10, 0),
    New PointF(10, 10),
    New PointF(0, 10)
}))


        ' Example: run AutoPunch for each polyline
        For Each pl As Polyline2D In _testPolylines ' Replace with your actual polylines
            ' Use a layer name from somewhere; here just an example
            Dim layerName As String = "*DEFAULT*"

            ' AutoPunch returns List(Of PunchResult)
            Dim results As List(Of FabV25_WIN8.PunchResult.PunchResult) = AutoPunch(pl, members, layerName)

            ' Inspect results
            For Each r In results
                Console.WriteLine($"ToolID: {r.ToolID}, ResultType: {r.ResultType}")
            Next
        Next
    End Sub

End Module
