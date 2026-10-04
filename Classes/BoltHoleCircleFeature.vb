Imports WE_ENG_V25_0.Core.Models

Public Class BoltHoleCircleFeature
    Inherits MachiningFeature

    Public Property CenterX As Double

    Public Property CenterY As Double

    Public Property Radius As Double

    Public Property Count As Integer

    Public Property StartAngle As Double

    Public Property IncrementAngle As Double

    Public Property AutoIndex As Boolean

    Public Property Tool As Tool

    Public Property Parameters As New HolePatternParameters

End Class