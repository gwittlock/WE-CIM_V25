
Public Enum HoleRepeatDirection
    X
    Y
End Enum

Public Class HolePatternParameters

    Public Property XMirror As Boolean

    Public Property YMirror As Boolean

    Public Property Centered As Boolean

    Public Property RepeatDirection As HoleRepeatDirection

    Public Property RepeatCount As Integer

    Public Property Gap As Double

    Public Property Orientation As Integer

End Class