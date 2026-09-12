Imports System.Drawing

Public Class QuadTree(Of T)

    Private ReadOnly _bounds As RectangleF
    Private ReadOnly _capacity As Integer

    Private _items As New List(Of QuadItem)
    Private _divided As Boolean

    Private _nw As QuadTree(Of T)
    Private _ne As QuadTree(Of T)
    Private _sw As QuadTree(Of T)
    Private _se As QuadTree(Of T)

    Public Structure QuadItem
        Public Bounds As RectangleF
        Public Value As T
    End Structure

    Public Sub New(bounds As RectangleF, capacity As Integer)
        _bounds = bounds
        _capacity = capacity
    End Sub

    Public Sub Insert(item As QuadItem)

        ' Keep the item if its bounds touch/intersect this node.
        If Not BoundsIntersect(_bounds, item.Bounds) Then
            Return
        End If

        ' Store items locally until capacity is reached.
        If _items.Count < _capacity AndAlso Not _divided Then
            _items.Add(item)
            Return
        End If

        ' Divide once capacity is exceeded.
        If Not _divided Then

            Subdivide()

            Dim existing As List(Of QuadItem) = _items.ToList()
            _items.Clear()

            For Each existingItem As QuadItem In existing
                Insert(existingItem)
            Next

        End If

        ' Only move an item into a child if the child completely
        ' contains the item. Items crossing a boundary remain here.
        If _nw._bounds.Contains(item.Bounds) Then
            _nw.Insert(item)

        ElseIf _ne._bounds.Contains(item.Bounds) Then
            _ne.Insert(item)

        ElseIf _sw._bounds.Contains(item.Bounds) Then
            _sw.Insert(item)

        ElseIf _se._bounds.Contains(item.Bounds) Then
            _se.Insert(item)

        Else
            _items.Add(item)
        End If

    End Sub

    Private Sub Subdivide()

        Dim x As Single = _bounds.X
        Dim y As Single = _bounds.Y
        Dim w As Single = _bounds.Width / 2.0F
        Dim h As Single = _bounds.Height / 2.0F

        _nw = New QuadTree(Of T)(
            New RectangleF(x, y, w, h),
            _capacity)

        _ne = New QuadTree(Of T)(
            New RectangleF(x + w, y, w, h),
            _capacity)

        _sw = New QuadTree(Of T)(
            New RectangleF(x, y + h, w, h),
            _capacity)

        _se = New QuadTree(Of T)(
            New RectangleF(x + w, y + h, w, h),
            _capacity)

        _divided = True

    End Sub

    Public Function Query(range As RectangleF) As List(Of T)

        Dim found As New List(Of T)

        If Not BoundsIntersect(_bounds, range) Then
            Return found
        End If

        For Each item As QuadItem In _items

            If BoundsIntersect(item.Bounds, range) Then
                found.Add(item.Value)
            End If

        Next

        If _divided Then

            found.AddRange(_nw.Query(range))
            found.AddRange(_ne.Query(range))
            found.AddRange(_sw.Query(range))
            found.AddRange(_se.Query(range))

        End If

        Return found

    End Function

    Private Shared Function BoundsIntersect(a As RectangleF, b As RectangleF) As Boolean
        Return (a.X <= b.X + b.Width) AndAlso
           (a.X + a.Width >= b.X) AndAlso
           (a.Y <= b.Y + b.Height) AndAlso
           (a.Y + a.Height >= b.Y)
    End Function

End Class