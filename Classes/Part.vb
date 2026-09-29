Imports System.Collections.Generic

Public Class Part

    '------------------------------------------------------------
    ' Part identification
    '------------------------------------------------------------
    Public Property ID As Integer
    Public Property Description As String

    '------------------------------------------------------------
    ' CAD geometry belonging to this Part
    '------------------------------------------------------------
    Public Property CADGeometry As New List(Of CadEntity)

    '------------------------------------------------------------
    ' Machining features belonging to this Part
    '------------------------------------------------------------
    Public Property Features As New List(Of MachiningFeature)

End Class