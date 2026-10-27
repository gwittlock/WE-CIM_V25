
Public Class GeoProfile
    Implements isSelectable

    Public Property ID As Integer

    ' Original geometric entities
    Public Property Entities As New List(Of GeoEntity)

    ' Ordered traversal of those entities
    Public Property Segments As New List(Of GeoProfileSegment)

    Public Property IsClosed As Boolean

    ' Profile selection state
    Public Property Selected As Boolean

    Public Property IsSelected As Boolean _
        Implements isSelectable.IsSelected
        Get
            Return Selected
        End Get
        Set(value As Boolean)
            Selected = value
        End Set
    End Property

    Public Property IsHovered As Boolean _
        Implements isSelectable.IsHovered

End Class
Public Class GeoProfileSegment

    Public Property Entity As GeoEntity
    Public Property Reversed As Boolean

End Class