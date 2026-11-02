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
    Implements isSelectable

    Public Property ID As Integer
    Public Property Description As String
    Public Property FeatureType As MachiningFeatureType

    Public Property IsSelected As Boolean _
        Implements isSelectable.IsSelected

    Public Property IsHovered As Boolean _
        Implements isSelectable.IsHovered

End Class