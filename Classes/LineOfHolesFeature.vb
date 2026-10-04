Imports WE_ENG_V25_0.Core.Models

Public Class LineOfHolesFeature
    Inherits MachiningFeature

    Public Property StartX As Double

    Public Property StartY As Double

    Public Property Count As Integer

    Public Property Spacing As Double

    Public Property Direction As HoleRepeatDirection

    Public Property Tool As Tool

    Public Property Parameters As New HolePatternParameters

End Class