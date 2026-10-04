Public Class WorkZoneFactory

    Private ReadOnly _machineMaxX As Double
    Private ReadOnly _sheetLengthX As Double
    Private ReadOnly _sheetLengthY As Double

    Public ReadOnly Property WorkZones As New List(Of WorkZone)

    Public Sub New(machineMaxTravelX As Double,
                   sheetLengthX As Double,
                   sheetLengthY As Double)

        _machineMaxX = machineMaxTravelX
        _sheetLengthX = sheetLengthX
        _sheetLengthY = sheetLengthY

        CreateInitialWorkZone()
    End Sub

    Private Sub CreateInitialWorkZone()
        Dim xMax As Double = Math.Min(_machineMaxX, _sheetLengthX)

        Dim wz As New WorkZone With {
            .Index = 0,
            .XMin = 0,
            .XMax = xMax,
            .YMin = 0,
            .YMax = _sheetLengthY,
            .RepositionOffsetX = 0,
            .IsActive = True
        }

        WorkZones.Add(wz)
    End Sub

    Public Function CreateNextWorkZone(requestedStartX As Double) As WorkZone
        ' Clamp requested start
        Dim offsetX As Double = Math.Max(0, requestedStartX)

        ' Ensure we do not exceed sheet
        If offsetX >= _sheetLengthX Then
            Return Nothing
        End If

        Dim xMax As Double = Math.Min(offsetX + _machineMaxX, _sheetLengthX)

        Dim wz As New WorkZone With {
            .Index = WorkZones.Count,
            .XMin = offsetX,
            .XMax = xMax,
            .YMin = 0,
            .YMax = _sheetLengthY,
            .RepositionOffsetX = offsetX,
            .IsActive = False
        }

        WorkZones.Add(wz)
        Return wz
    End Function

End Class
