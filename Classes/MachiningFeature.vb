Imports System

Public Enum MachiningFeatureType

    Profile

    Hole

    BoltHoleCircle

    GridOfHoles

    LineOfHoles

    LineOfHolesAtAngleNormal

    LineOfHolesAtAngleParallel

    BarrierHole

    PierceHole

    Command

    RapidPoint

    AreaClear

End Enum


Public Class MachiningFeature

    '------------------------------------------------------------
    ' Feature identification
    '------------------------------------------------------------
    Public Property ID As Integer
    Public Property Description As String

    '------------------------------------------------------------
    ' Feature type
    '------------------------------------------------------------
    Public Property FeatureType As MachiningFeatureType

End Class