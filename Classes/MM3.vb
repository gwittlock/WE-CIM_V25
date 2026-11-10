Imports System.Collections.Generic


Public Class MM3

        Public Property WorkZones As New List(Of WorkZone)

        Public Property PartGeometry As New List(Of GeoEntity)

    Public Property ManufacturingGeometry As New List(Of MachiningFeature)

    Public Property WorkZoneAssignments As New List(Of MM3WorkZoneAssignment)


    Public Function GetNextFeatureID() As Integer

        If ManufacturingGeometry Is Nothing OrElse
       ManufacturingGeometry.Count = 0 Then

            Return 1

        End If

        Return ManufacturingGeometry.Max(Function(f) f.ID) + 1

    End Function

End Class

