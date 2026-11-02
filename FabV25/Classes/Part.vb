Imports System.Collections.Generic
Imports FabV25_WIN8.ToolShapes

Public Class Part
    Public Property ID As Integer
    Public Property Description As String

    Public Property CADGeometry As New List(Of CadEntity)

    Public Property Geometry As New List(Of GeoEntity)

    Public Property GeoProfiles As New List(Of GeoProfile)

    Public Property Features As New List(Of MachiningFeature)


    Public Function GetNextFeatureID() As Integer

        If Features Is Nothing OrElse Features.Count = 0 Then
            Return 1
        End If

        Return Features.Max(Function(f) f.ID) + 1

    End Function

    Public Function GetLayers() As List(Of Layer)

        Return Geometry.
            Where(Function(ent) ent IsNot Nothing).
            Select(Function(ent) TryCast(ent.Association, Layer)).
            Where(Function(layer) layer IsNot Nothing).
            Distinct().
            OrderBy(Function(layer) layer.Name).
            ToList()

    End Function
End Class