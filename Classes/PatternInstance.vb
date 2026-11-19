Public Class PatternInstance
    Implements isSelectable

    Public Property ID As Integer

    Public Property PatternID As Integer

    Public Property X As Double

    Public Property Y As Double

    Public Property Angle As Double

    Public Property IsSelected As Boolean Implements isSelectable.IsSelected

    Public Property IsHovered As Boolean Implements isSelectable.IsHovered

End Class