Imports System.Collections.Generic

Public Class Part

    '------------------------------------------------------------
    ' Part identification
    '------------------------------------------------------------
    Public Property ID As Integer
    Public Property Description As String
    Public Property Features As New List(Of MachiningFeature)

    '------------------------------------------------------------
    ' Part placement on the JobSheet
    ' Sheet-relative coordinates
    '------------------------------------------------------------
    Public Property X As Double
    Public Property Y As Double
    Public Property Angle As Double

End Class