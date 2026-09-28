Imports System

Public Enum MachiningFeatureType
    Profile
    Hole
    BoltHoleCircle
    GridOfHoles
    LineOfHoles
    LineOfHolesAtAngle
    PierceHole
    LeadIn
    LeadOut
    Command
    RapidPoint
    Pocket
    Slot
    Engrave
    Reposition
End Enum

Public Class MachiningFeature

    '------------------------------------------------------------
    ' Feature identification
    '------------------------------------------------------------
    Public Property ID As Integer
    Public Property Description As String

    '------------------------------------------------------------
    ' CAM feature type
    '------------------------------------------------------------
    Public Property FeatureType As MachiningFeatureType

End Class