Imports System.Collections.Generic
Imports FabV25_WIN8.ToolShapes

Public Class Part
    Public Property ID As Integer
    Public Property Description As String

    Public Property CADGeometry As New List(Of CadEntity)

    Public Property Geometry As New List(Of GeoEntity)

    Public Property Profiles As New List(Of Profile)
    Public Property GeoProfiles As New List(Of GeoProfile)

    Public Property Features As New List(Of MachiningFeature)
End Class