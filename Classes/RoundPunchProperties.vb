Imports System.ComponentModel
Imports System.Linq

Public Class RoundPunchProperties

    <Category("Punch Tool")>
    <DisplayName("NC Tool Number")>
    Public ReadOnly Property ToolNumber As Integer

    <Category("Punch Tool")>
    <DisplayName("Tool Description")>
    Public ReadOnly Property ToolDescription As String

    <Category("Punch Tool")>
    <DisplayName("Station Description")>
    Public ReadOnly Property StationDescription As String

    <Category("Punch Tool")>
    <DisplayName("Punch Diameter")>
    Public ReadOnly Property PunchDiameter As Double

    <Category("Tool Assignment")>
    <DisplayName("Tool Setup Member ID")>
    Public ReadOnly Property ToolSetupMemberID As Integer

    <Category("Hole Geometry")>
    <DisplayName("X Position")>
    Public ReadOnly Property X As Double

    <Category("Hole Geometry")>
    <DisplayName("Y Position")>
    Public ReadOnly Property Y As Double

    <Category("Hole Geometry")>
    <DisplayName("Source Geometry ID")>
    Public ReadOnly Property SourceGeometryID As Integer

    Public Sub New(hole As HoleFeature)

        If hole Is Nothing Then Exit Sub

        ToolSetupMemberID = hole.ToolSetupMemberID
        SourceGeometryID = hole.SourceGeometryID
        X = hole.X
        Y = hole.Y

        ToolDescription = "Not Found"
        StationDescription = "Not Found"

        Dim diameter As Double = 0

        If AutoPunchingEngine.TryGetRoundPunchDiameter(
            hole, diameter) Then

            PunchDiameter = diameter

        End If

        If AppData.ToolSetupMembers Is Nothing Then Exit Sub

        Dim member = AppData.ToolSetupMembers.
            FirstOrDefault(
                Function(m) m.ID = hole.ToolSetupMemberID)

        If member Is Nothing Then Exit Sub

        If AppData.StationList IsNot Nothing Then

            Dim station = AppData.StationList.
                FirstOrDefault(
                    Function(s) s.ID = member.StationID)

            If station IsNot Nothing Then

                ToolNumber = station.NCCodeNumber
                StationDescription = station.Description

            End If

        End If

        If AppData.ToolCrib IsNot Nothing Then

            Dim tool = AppData.ToolCrib.
                FirstOrDefault(
                    Function(t) t.ID = member.ToolID)

            If tool IsNot Nothing Then
                ToolDescription = tool.Description
            End If

        End If

    End Sub

End Class