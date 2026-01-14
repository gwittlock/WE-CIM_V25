Imports FabV25_WIN8.AppData
Public Class WorkZoneFactory

    Private ReadOnly _machineMaxX As Double
    Private ReadOnly _sheetLengthX As Double
    Private ReadOnly _sheetLengthY As Double



    Public Shared Function CreateInitialZones(
        machineDef As MachineDefinition,
        sheetWidth As Double,
        sheetHeight As Double
    ) As List(Of WorkZone)

        Dim zones As New List(Of WorkZone)

        ' Machine X travel defines zone width
        Dim zoneWidth As Double = machineDef.Max_Travel_Limit_X

        ' FIRST ZONE ALWAYS STARTS AT X = 0
        Dim xMin As Double = 0
        Dim xMax As Double = Math.Min(zoneWidth, sheetWidth)

        zones.Add(New WorkZone With {
            .XMin = xMin,
            .XMax = xMax,
            .YMin = 0,
            .YMax = sheetHeight
        })

        Return zones
    End Function


    Public Sub New(machineMaxTravelX As Double,
                   sheetLengthX As Double,
                   sheetLengthY As Double)

        matWidth = GetMaterialWidth(AppData.MaterialSheets, _SelectedMaterialID)
        matLength = GetMaterialLength(AppData.MaterialSheets, _SelectedMaterialID)
        _machineMaxX = _machineDef.Max_Travel_Limit_X
        _sheetLengthX = matLength
        _sheetLengthY = matWidth

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
