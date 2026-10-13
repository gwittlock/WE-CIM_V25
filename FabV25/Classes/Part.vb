Imports System.Collections.Generic
Imports FabV25_WIN8.ToolShapes

Public Class Part
    Public Property ID As Integer
    Public Property Description As String

    Public Property CADGeometry As New List(Of CadEntity)

    Public Property Profiles As New List(Of Profile)

    Public Property Features As New List(Of MachiningFeature)
End Class