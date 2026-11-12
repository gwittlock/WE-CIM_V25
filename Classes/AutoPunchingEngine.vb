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

    Public Shared Function FindExactRoundPunch(
    circle As GeoCircle,
    toolSetupID As Integer,
    machineDef As MachineDefinition) As Integer

        If circle Is Nothing Then Return 0

        If machineDef Is Nothing Then Return 0

        Dim circleDiameter As Double = circle.Radius * 2.0

        Const diameterTolerance As Double = 0.000001

        ' Machine-specific allowable hole tolerances.
        Dim plusTolerance As Double =
    Math.Max(0, machineDef.Hole_Plus_Tolerance)

        Dim minusTolerance As Double =
    Math.Max(0, machineDef.Hole_Minus_Tolerance)

        Dim minimumDiameter As Double =
    circleDiameter - minusTolerance

        Dim maximumDiameter As Double =
    circleDiameter + plusTolerance

        ' Remember the closest acceptable tolerance match.
        Dim bestToleranceMemberID As Integer = 0
        Dim bestToleranceDifference As Double = Double.MaxValue

        For Each member As ToolSetupMember In AppData.ToolSetupMembers

            If member.ToolSetupID <> toolSetupID Then
                Continue For
            End If

            Dim tool = AppData.ToolCrib.FirstOrDefault(
            Function(t) t.ID = member.ToolID)

            If tool Is Nothing Then Continue For

            '        Debug.WriteLine(
            '"ROUND PUNCH CANDIDATE | MemberID=" & member.ID &
            '" | ToolID=" & tool.ID &
            '" | TypeID=" & tool.TypeID &
            '" | Description=" & tool.Description)

            ' Round punch only
            If tool.TypeID <> 15 Then Continue For

            If tool.ID = 144 Then

                'Debug.WriteLine("TOOL 144 ATTRIBUTES:")

                If tool.Attributes Is Nothing Then
                    Debug.WriteLine("Attributes = Nothing")
                Else
                    For Each attribute In tool.Attributes
                        '        Debug.WriteLine(
                        '"Key=[" & attribute.Key &
                        '"] Value=[" & Convert.ToString(attribute.Value) & "]")
                    Next
                End If

            End If

            If tool.Attributes Is Nothing OrElse
           Not tool.Attributes.ContainsKey("DIAMETER") Then
                Continue For
            End If

            Dim toolDiameter As Double

            Try
                toolDiameter = Convert.ToDouble(
                tool.Attributes("DIAMETER"),
                Globalization.CultureInfo.InvariantCulture)
            Catch
                Continue For
            End Try

            Dim diameterDifference As Double =
    Math.Abs(circleDiameter - toolDiameter)

            ' Exact matches always take priority.
            If diameterDifference <= diameterTolerance Then
                Return member.ID
            End If

            ' Check the machine's allowable hole tolerance.
            If toolDiameter >= minimumDiameter AndAlso
   toolDiameter <= maximumDiameter Then

                If diameterDifference < bestToleranceDifference Then

                    bestToleranceDifference = diameterDifference
                    bestToleranceMemberID = member.ID

                End If

            End If

        Next

        Return bestToleranceMemberID

    End Function

    Public Shared Function TryGetRoundPunchDiameter(
        hole As HoleFeature,
        ByRef diameter As Double) As Boolean

        diameter = 0

        If hole Is Nothing Then Return False

        Dim member = AppData.ToolSetupMembers.
            FirstOrDefault(
                Function(m) m.ID = hole.ToolSetupMemberID)

        If member Is Nothing Then Return False

        Dim tool = AppData.ToolCrib.
            FirstOrDefault(
                Function(t) t.ID = member.ToolID)

        If tool Is Nothing OrElse
           tool.TypeID <> 15 OrElse
           tool.Attributes Is Nothing Then

            Return False
        End If

        If Not tool.Attributes.ContainsKey("DIAMETER") Then
            Return False
        End If

        Try
            diameter = Convert.ToDouble(
                tool.Attributes("DIAMETER"),
                Globalization.CultureInfo.InvariantCulture)

        Catch
            diameter = 0
            Return False
        End Try

        If Double.IsNaN(diameter) OrElse
           Double.IsInfinity(diameter) OrElse
           diameter <= 0 Then

            diameter = 0
            Return False
        End If

        Return True

    End Function
End Class
