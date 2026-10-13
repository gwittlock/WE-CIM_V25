Imports System.Collections.Generic

Public Class WorkZone
    Public Property Index As Integer

    ' Sheet-relative envelope
    Public Property XMin As Double
    Public Property XMax As Double
    Public Property YMin As Double = 0
    Public Property YMax As Double

    ' Mapping
    Public Property RepositionOffsetX As Double

    ' Children
    Public Property Clamps As New List(Of Clamp)
    Public Property Patterns As New List(Of Pattern)

    ' State
    Public Property IsActive As Boolean
End Class