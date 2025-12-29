Option Strict On
Option Explicit On
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.ClassifiedProfile

Public Class AutoPunchingEngine
    Public Enum MachineType
        Punch
        PunchPlasma
        PunchLaser
        Plasma
        Laser
        Mill
        Router
    End Enum

    Public Enum StationMode
        None
        Auto
        Specific
    End Enum

    Public Enum ProfileRole
        Outside
        Inside
    End Enum

    Public Enum CutDirection
        Auto
        CW
        CCW
    End Enum

    Public Enum CutSide
        Inside
        Outside
    End Enum

    Public Enum WindingDirection
        CW
        CCW
    End Enum

    Public Enum PunchResultType
        ExactMatch
        Decomposed
        Failed
    End Enum


    Public Function Process(
        machineType As MachineType,
        closedProfiles As List(Of ClosedProfile),
        layerRules As List(Of LayerPunchRule),
        toolSetup As ToolSetup
    ) As List(Of PunchOperation)

        Dim results As New List(Of PunchOperation)

        If Not IsPunchCapable(machineType) Then
            Return results
        End If

        Dim classified = ClassifyProfiles(closedProfiles)

        For Each cp In classified

            Dim rule = ResolveLayerRule(cp, layerRules)
            If rule Is Nothing Then Continue For
            If rule.Station = StationMode.None Then Continue For

            Dim workingProfile = CreateWorkingProfile(
                cp.Profile,
                rule,
                cp.Role
            )

            Dim punchResult = AutoPunch(workingProfile, toolSetup, rule)

            Select Case punchResult.ResultType

                Case PunchResultType.ExactMatch,
                     PunchResultType.Decomposed

                    For Each toolId In punchResult.ToolIDs
                        results.Add(New PunchOperation With {
                            .ToolID = toolId,
                            .Profile = workingProfile,
                            .Role = cp.Role
                        })
                    Next

                Case PunchResultType.Failed

                    If rule.Station = StationMode.Specific AndAlso rule.ToolID.HasValue Then
                        results.Add(New PunchOperation With {
                            .ToolID = rule.ToolID.Value,
                            .Profile = workingProfile,
                            .Role = cp.Role
                        })
                    End If

            End Select

        Next

        Return results

    End Function

    Private Function ClassifyProfiles(
        profiles As List(Of ClosedProfile)
    ) As List(Of ClassifiedProfile)

        Dim sorted = profiles.
            OrderByDescending(Function(p) Math.Abs(p.Area)).
            ToList()

        Dim result As New List(Of ClassifiedProfile)

        For i = 0 To sorted.Count - 1

            Dim role As ProfileRole = ProfileRole.Outside

            For j = 0 To i - 1
                If sorted(j).Polyline.Contains(sorted(i).Polyline) Then
                    role = ProfileRole.Inside
                    Exit For
                End If
            Next

            result.Add(New ClassifiedProfile With {
                .Profile = sorted(i),
                .Role = role
            })

        Next

        Return result

    End Function

    Private Function AutoPunch(
    profile As Polyline2D,
    toolSetup As ToolSetup,
    rule As LayerPunchRule
) As PunchResult

        ' 1) Exact tool match
        Dim exact = toolSetup.FindExactMatch(profile, rule)
        If exact IsNot Nothing Then
            Return New PunchResult With {
                .ResultType = PunchResultType.ExactMatch,
                .ToolIDs = New List(Of Integer) From {exact.ToolID}
            }
        End If

        ' 2) Decomposition
        Dim combo = toolSetup.TryDecompose(profile)
        If combo IsNot Nothing AndAlso combo.Any() Then
            Return New PunchResult With {
                .ResultType = PunchResultType.Decomposed,
                .ToolIDs = combo
            }
        End If

        Return New PunchResult With {
            .ResultType = PunchResultType.Failed,
            .ToolIDs = New List(Of Integer)
        }

    End Function

    Private Function IsPunchCapable(machineType As MachineType) As Boolean
        Return machineType = MachineType.Punch _
            OrElse machineType = MachineType.PunchPlasma _
            OrElse machineType = MachineType.PunchLaser
    End Function

    Private Function ResolveLayerRule(
        cp As ClassifiedProfile,
        rules As List(Of LayerPunchRule)
    ) As LayerPunchRule

        ' Assumes layer name can be obtained from source entities
        Dim layerName As String = GetLayerName(cp.Profile)

        Return rules.FirstOrDefault(
            Function(r) String.Equals(r.LayerName, layerName, StringComparison.OrdinalIgnoreCase)
        )

    End Function


    Private Function GetLayerName(profile As ClosedProfile) As String
        ' Implementation depends on DXF entity model
        Return String.Empty
    End Function

    Private Function CreateWorkingProfile(
      baseProfile As ClosedProfile,
      rule As LayerPunchRule,
      role As ProfileRole
  ) As Polyline2D

        Dim poly = baseProfile.Polyline.Clone()

        If rule.Distance <> 0 Then
            poly = poly.Offset(rule.Distance, rule.CutSide)
        End If

        Dim winding = ResolveWinding(role, rule.CutDirection)
        poly.SetWinding(winding)

        Return poly

    End Function

    Private Function ResolveWinding(
       role As ProfileRole,
       cutDirection As CutDirection
   ) As WindingDirection

        Select Case cutDirection
            Case CutDirection.Auto
                Return If(role = ProfileRole.Inside,
                          WindingDirection.CCW,
                          WindingDirection.CW)

            Case CutDirection.CW
                Return WindingDirection.CW

            Case CutDirection.CCW
                Return WindingDirection.CCW

            Case Else
                Throw New InvalidOperationException("Invalid CutDirection")
        End Select

    End Function



End Class
