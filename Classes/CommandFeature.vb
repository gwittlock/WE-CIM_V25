Public Enum CommandTextPosition
    Above
    Below
End Enum


Public Class CommandFeature
    Inherits MachiningFeature

    Public Property X As Double

    Public Property Y As Double

    ' Text rotation angle in degrees
    Public Property Angle As Double

    Public Property Position As CommandTextPosition

    Public Property FontSize As Double

    Public Property CommandText As String

End Class