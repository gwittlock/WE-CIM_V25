Imports WE_ENG_V25_0.Core.Models

Public Class BarrierHoleFeature
    Inherits MachiningFeature

    '------------------------------------------------------------
    ' First hole location
    '------------------------------------------------------------
    Public Property StartX As Double

    Public Property StartY As Double

    '------------------------------------------------------------
    ' Last hole location
    '------------------------------------------------------------
    Public Property EndX As Double

    Public Property EndY As Double

    '------------------------------------------------------------
    ' Hole spacing
    '------------------------------------------------------------
    Public Property Spacing As Double

    '------------------------------------------------------------
    ' Tool information
    '------------------------------------------------------------
    Public Property Tool As Tool

    Public Property ToolIndexAngle As Double

    '------------------------------------------------------------
    ' Hole pattern parameters
    '------------------------------------------------------------
    Public Property Parameters As New HolePatternParameters

End Class