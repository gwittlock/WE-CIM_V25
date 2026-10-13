Public Class Polyline2D

    Public ReadOnly Property Points As List(Of PointF)

    Public ReadOnly Property IsClosed As Boolean
        Get
            If Points.Count < 3 Then Return False
            Return Distance(Points.First, Points.Last) < 0.0001F
        End Get
    End Property

    Public Property LayerName As String

    Public Sub New(points As IEnumerable(Of PointF))
        Me.Points = New List(Of PointF)(points)
    End Sub

    Public Function Perimeter() As Single
        Dim total As Single = 0
        For i = 0 To Points.Count - 2
            total += Distance(Points(i), Points(i + 1))
        Next
        Return total
    End Function

    Private Shared Function Distance(a As PointF, b As PointF) As Single
        Dim dx = a.X - b.X
        Dim dy = a.Y - b.Y
        Return CSng(Math.Sqrt(dx * dx + dy * dy))
    End Function

End Class
