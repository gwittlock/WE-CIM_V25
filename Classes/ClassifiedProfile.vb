Imports FabV25_WIN8.AutoPunchingEngine

Public Class ClassifiedProfile
    Public Property Profile As ClosedProfile
    Public Property Role As ProfileRole

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
