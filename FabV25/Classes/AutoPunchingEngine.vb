Imports System.Drawing
Imports FabV25_WIN8.PunchResult
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models

Public Class AutoPunchingEngine

    '=========================
    ' Enums
    '=========================
    Public Enum CutDirection
        Auto
        CW
        CCW
    End Enum

    Public Enum WindingDirection
        CW
        CCW
    End Enum

    Public Enum ProfileRole
        Outside
        Inside
    End Enum

    '=========================
    ' AutoPunch v1
    '=========================
    ''' <summary>
    ''' Apply tooling to a closed Polyline2D profile using a ToolSetup.
    ''' Returns a PunchResult with ResultType and ToolID.
    ''' </summary>
    Public Shared Function AutoPunch(
        profile As Polyline2D,
        toolSetupMembers As List(Of ToolSetupMember),
        layerName As String
    ) As List(Of PunchResult.PunchResult)

        Dim results As New List(Of PunchResult.PunchResult)

        If profile Is Nothing OrElse profile.Points.Count < 3 Then
            ' Invalid profile
            results.Add(New PunchResult.PunchResult With {
                .ResultType = PunchResultType.Failed,
                .ToolID = 0
            })
            Return results
        End If

        If toolSetupMembers Is Nothing OrElse toolSetupMembers.Count = 0 Then
            ' No tooling available
            results.Add(New PunchResult.PunchResult With {
                .ResultType = PunchResultType.Failed,
                .ToolID = 0
            })
            Return results
        End If

        '-------------------------
        ' Loop through each ToolSetupMember
        '-------------------------
        For Each member In toolSetupMembers
            ' Optionally filter by layer
            ' If String.Equals(member.LayerName, layerName, StringComparison.OrdinalIgnoreCase) OrElse String.IsNullOrEmpty(member.LayerName) Then
            Dim pr As New PunchResult.PunchResult With {
                    .ToolID = member.ToolID,
                    .ResultType = PunchResultType.ExactMatch
                }
                results.Add(pr)
            ' End If
        Next

        '-------------------------
        ' Return applied tooling
        '-------------------------
        If results.Count = 0 Then
            results.Add(New PunchResult.PunchResult With {
                .ToolID = 0,
                .ResultType = PunchResultType.Failed
            })
        End If

        Return results
    End Function

End Class
