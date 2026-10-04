Imports WE_ENG_V25_0.Core.Models

Public Class GridOfHolesFeature
    Inherits MachiningFeature

    Public Property StartX As Double

    Public Property StartY As Double

    Public Property XCount As Integer

    Public Property YCount As Integer

    Public Property XSpacing As Double

    Public Property YSpacing As Double

    Public Property PrimaryAxis As HoleRepeatDirection

    Public Property Tool As Tool

    Public Property Parameters As New HolePatternParameters

End Class