Imports FabV25_WIN8.AppData
Imports System.Collections.Generic

Public Class WorkZoneFactory

    ' =========================================================
    ' Create the initial WorkZone collection for a JobSheet
    '
    ' Length = X dimension
    ' Width  = Y dimension
    ' =========================================================
    Public Shared Function CreateInitialZones(
        machineDef As MachineDefinition,
        sheetLength As Double,
        sheetWidth As Double
    ) As List(Of WorkZone)

        Dim zones As New List(Of WorkZone)

        If machineDef Is Nothing Then
            Return zones
        End If

        If sheetLength <= 0 OrElse sheetWidth <= 0 Then
            Return zones
        End If

        Dim machineTravelX As Double =
            machineDef.Max_Travel_Limit_X

        If machineTravelX <= 0 Then
            Return zones
        End If

        ' -----------------------------------------------------
        ' First WorkZone always starts at sheet X = 0.
        ' -----------------------------------------------------
        Dim xMin As Double = 0
        Dim xMax As Double =
            Math.Min(machineTravelX, sheetLength)

        Dim firstZone As New WorkZone With {
            .Index = 0,
            .XMin = xMin,
            .XMax = xMax,
            .YMin = 0,
            .YMax = sheetWidth,
            .RepositionOffsetX = 0,
            .IsActive = True
        }

        zones.Add(firstZone)

        Return zones

    End Function


    ' =========================================================
    ' Create a subsequent WorkZone
    '
    ' requestedStartX is in SHEET coordinates.
    ' =========================================================
    Public Shared Function CreateNextZone(
        machineDef As MachineDefinition,
        sheetLength As Double,
        sheetWidth As Double,
        requestedStartX As Double,
        zoneIndex As Integer
    ) As WorkZone

        If machineDef Is Nothing Then
            Return Nothing
        End If

        If sheetLength <= 0 OrElse sheetWidth <= 0 Then
            Return Nothing
        End If

        Dim machineTravelX As Double =
            machineDef.Max_Travel_Limit_X

        If machineTravelX <= 0 Then
            Return Nothing
        End If

        ' -----------------------------------------------------
        ' Requested position is sheet-relative.
        ' -----------------------------------------------------
        Dim xMin As Double =
            Math.Max(0, requestedStartX)

        ' -----------------------------------------------------
        ' Cannot create a zone beyond the sheet.
        ' -----------------------------------------------------
        If xMin >= sheetLength Then
            Return Nothing
        End If

        Dim xMax As Double =
            Math.Min(xMin + machineTravelX, sheetLength)

        Dim zone As New WorkZone With {
            .Index = zoneIndex,
            .XMin = xMin,
            .XMax = xMax,
            .YMin = 0,
            .YMax = sheetWidth,
            .RepositionOffsetX = xMin,
            .IsActive = False
        }

        Return zone

    End Function

End Class