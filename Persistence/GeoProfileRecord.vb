Public Class GeoProfileRecord

    Public Property ID As Integer

    Public Property EntityIDs As New List(Of Integer)

    Public Property IsClosed As Boolean

    Public Property SignedArea As Double

    Public Property Segments As New List(Of GeoProfileSegmentRecord)

End Class