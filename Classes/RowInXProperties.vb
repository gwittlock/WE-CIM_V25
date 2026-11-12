Imports System.ComponentModel
Imports System.Linq

Public Class RowInXProperties

    <Category("Hole Arrangement")>
    <DisplayName("Arrangement")>
    Public ReadOnly Property Arrangement As String

    <Category("Hole Arrangement")>
    <DisplayName("Start X")>
    Public ReadOnly Property StartX As Double

    <Category("Hole Arrangement")>
    <DisplayName("Start Y")>
    Public ReadOnly Property StartY As Double

    <Category("Hole Arrangement")>
    <DisplayName("Hole Count")>
    Public ReadOnly Property Count As Integer

    <Category("Hole Arrangement")>
    <DisplayName("Spacing")>
    Public ReadOnly Property Spacing As Double

    <Category("Punch Tool")>
    <DisplayName("NC Tool Number")>
    Public ReadOnly Property ToolNumber As Integer

    <Category("Punch Tool")>
    <DisplayName("Tool Description")>
    Public ReadOnly Property ToolDescription As String

    <Category("Punch Tool")>
    <DisplayName("Station Description")>
    Public ReadOnly Property StationDescription As String

    <Category("Tool Assignment")>
    <DisplayName("Tool Setup Member ID")>
    Public ReadOnly Property ToolSetupMemberID As Integer

    Public Sub New(feature As LineOfHolesFeature)

        If feature Is Nothing Then Exit Sub

        Arrangement = "Row In X"
        StartX = feature.StartX
        StartY = feature.StartY
        Count = feature.Count
        Spacing = feature.Spacing
        ToolSetupMemberID = feature.ToolSetupMemberID

        ToolDescription = "Not Found"
        StationDescription = "Not Found"

        If AppData.ToolSetupMembers Is Nothing Then Exit Sub

        Dim member = AppData.ToolSetupMembers.
            FirstOrDefault(
                Function(m) m.ID = feature.ToolSetupMemberID)

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
