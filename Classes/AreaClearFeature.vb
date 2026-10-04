Imports WE_ENG_V25_0.Core.Models

Public Enum AreaClearType
    Linear
    Spiral
End Enum


Public Enum AreaClearStartPosition
    FromCenter
    FromEdge
End Enum


Public Enum AreaClearCutDirection
    CW
    CCW
End Enum


Public Class AreaClearFeature
    Inherits MachiningFeature

    Public Property Type As AreaClearType

    Public Property ProfileEntity As CadEntity

    Public Property Tool As Tool

    Public Property IndexAngle As Double

    Public Property Angle As Double

    Public Property StepOverDistance As Double

    Public Property StartPosition As AreaClearStartPosition

    Public Property CutDirection As AreaClearCutDirection

End Class