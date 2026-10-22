
Public Class GeoProfile

    Public Property ID As Integer

    ' Original geometric entities
    Public Property Entities As New List(Of GeoEntity)

    ' Ordered traversal of those entities
    Public Property Segments As New List(Of GeoProfileSegment)

    Public Property IsClosed As Boolean

End Class
Public Class GeoProfileSegment

    Public Property Entity As GeoEntity
    Public Property Reversed As Boolean

End Class