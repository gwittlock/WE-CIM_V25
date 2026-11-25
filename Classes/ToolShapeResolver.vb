
Imports System.Linq
Imports FabV25_WIN8.ToolShapes
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models

Public NotInheritable Class ToolShapeResolver

    Private Sub New()
    End Sub
    Public Shared Function TryResolve(
    toolSetupMemberID As Integer,
    ByRef tool As Tool,
    ByRef shape As ToolShape) As Boolean

        Return TryResolve(
        toolSetupMemberID,
        AppData.ToolSetupMembers,
        AppData.ToolCrib,
        tool,
        shape)

    End Function

    Public Shared Function TryResolve(
    toolSetupMemberID As Integer,
    members As IEnumerable(Of ToolSetupMember),
    tools As IEnumerable(Of Tool),
    ByRef tool As Tool,
    ByRef shape As ToolShape) As Boolean

        tool = Nothing
        shape = Nothing

        If toolSetupMemberID <= 0 Then Return False

        If members Is Nothing OrElse
           tools Is Nothing Then
            Return False
        End If

        Dim member = members.
            FirstOrDefault(Function(m) m.ID = toolSetupMemberID)

        If member Is Nothing Then Return False

        Dim resolvedTool = tools.
            FirstOrDefault(Function(t) t.ID = member.ToolID)

        If resolvedTool Is Nothing Then Return False

        Try

            Dim resolvedShape As ToolShape =
                ToolShapeFactory.Create(resolvedTool)

            If resolvedShape Is Nothing OrElse
               resolvedShape.CuttingProfile Is Nothing Then
                Return False
            End If

            tool = resolvedTool
            shape = resolvedShape

            Return True

        Catch ex As Exception

            System.Diagnostics.Debug.WriteLine(
                "ToolShapeResolver: " & ex.Message)

            Return False

        End Try

    End Function
End Class
