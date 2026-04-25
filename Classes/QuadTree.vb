Public Class QuadTree(Of T)

    Private ReadOnly _bounds As RectangleF
    Private ReadOnly _capacity As Integer

    Private _items As New List(Of QuadItem)
    Private _divided As Boolean

    Private _nw, _ne, _sw, _se As QuadTree(Of T)

    Public Structure QuadItem
        Public Bounds As RectangleF
        Public Value As T
    End Structure

    Public Sub New(bounds As RectangleF, capacity As Integer)
        _bounds = bounds
        _capacity = capacity
    End Sub

    ' =========================================================
    ' INSERT
    ' =========================================================
    Public Sub Insert(item As QuadItem)

        If Not _bounds.IntersectsWith(item.Bounds) Then Return

        ' If we have room and not subdivided, store here
        If _items.Count < _capacity AndAlso Not _divided Then
            _items.Add(item)
            Return
        End If

        ' Subdivide if needed
        If Not _divided Then
            Subdivide()

            ' IMPORTANT: redistribute existing items
            Dim existing = _items.ToList()
            _items.Clear()

            For Each i In existing
                Insert(i)
            Next
        End If

        ' Try to push into a child
        If _nw._bounds.Contains(item.Bounds) Then
            _nw.Insert(item)
        ElseIf _ne._bounds.Contains(item.Bounds) Then
            _ne.Insert(item)
        ElseIf _sw._bounds.Contains(item.Bounds) Then
            _sw.Insert(item)
        ElseIf _se._bounds.Contains(item.Bounds) Then
            _se.Insert(item)
        Else
            ' If it doesn't fully fit in a child, keep it here
            _items.Add(item)
        End If

    End Sub

    ' =========================================================
    ' SUBDIVIDE
    ' =========================================================
    Private Sub Subdivide()

        Dim x = _bounds.X
        Dim y = _bounds.Y
        Dim w = _bounds.Width / 2
        Dim h = _bounds.Height / 2

        _nw = New QuadTree(Of T)(New RectangleF(x, y, w, h), _capacity)
        _ne = New QuadTree(Of T)(New RectangleF(x + w, y, w, h), _capacity)
        _sw = New QuadTree(Of T)(New RectangleF(x, y + h, w, h), _capacity)
        _se = New QuadTree(Of T)(New RectangleF(x + w, y + h, w, h), _capacity)

        _divided = True

    End Sub

    ' =========================================================
    ' QUERY
    ' =========================================================
    Public Function Query(range As RectangleF) As List(Of T)

        Dim found As New List(Of T)

        If Not _bounds.IntersectsWith(range) Then Return found

        ' Check items at this level
        For Each i In _items
            If range.IntersectsWith(i.Bounds) Then
                found.Add(i.Value)
            End If
        Next

        ' Check children
        If _divided Then
            found.AddRange(_nw.Query(range))
            found.AddRange(_ne.Query(range))
            found.AddRange(_sw.Query(range))
            found.AddRange(_se.Query(range))
        End If

        Return found

    End Function

End Class