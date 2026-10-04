Public Class ClosedProfile

    Public ReadOnly Property SourceEntities As List(Of Object)
    Public ReadOnly Property Polyline As Polyline2D
    Public ReadOnly Property BoundingBox As RectangleF
    Public ReadOnly Property Area As Double

    Public Sub New(
        sourceEntities As List(Of Object),
        polyline As Polyline2D
    )
        Me.SourceEntities = sourceEntities
        Me.Polyline = polyline
        Me.BoundingBox = polyline.GetBoundingBox()
        Me.Area = polyline.GetArea()
    End Sub

End Class
