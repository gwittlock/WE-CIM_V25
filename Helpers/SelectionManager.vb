Imports FabV25_WIN8.CadEntity
Imports FabV25_WIN8.ImportedEntity
Public Class SelectionManager

    ' =========================================================
    ' STATE
    ' =========================================================
    Private ReadOnly _selected As New HashSet(Of ISelectable)
    Private _hovered As ISelectable

    Private _allEntities As List(Of CadEntity)
    Private _spatialIndex As QuadTree(Of CadEntity)

    ' =========================================================
    ' EVENTS
    ' =========================================================
    Public Event SelectionChanged As Action(Of IEnumerable(Of ISelectable))
    Public Event HoverChanged As Action(Of ISelectable)

    ' =========================================================
    ' PUBLIC STATE ACCESS
    ' =========================================================
    Public ReadOnly Property Selected As IEnumerable(Of ISelectable)
        Get
            Return _selected
        End Get
    End Property

    Public ReadOnly Property SelectedEntities As IEnumerable(Of ISelectable)
        Get
            Return _selected
        End Get
    End Property

    Public ReadOnly Property Hovered As ISelectable
        Get
            Return _hovered
        End Get
    End Property

    Public ReadOnly Property HasSelection As Boolean
        Get
            Return _selected.Count > 0
        End Get
    End Property
    Private Class HitResult
        Public Entity As CadEntity
        Public Distance As Single
        Public Priority As Integer
    End Class

    Private Function GetPriority(ent As CadEntity) As Integer

        Select Case ent.GeometryType
            Case GeometryTypeEnum.Point : Return 0
            Case GeometryTypeEnum.Line : Return 1
            Case GeometryTypeEnum.Arc : Return 2
            Case GeometryTypeEnum.Circle : Return 3
            Case Else : Return 10
        End Select

    End Function

    Private Function DistanceToArc(pt As PointF, a As CadArc) As Single

        Dim distToCircle = Math.Abs(Distance(pt, a.Center) - a.Radius)

        Dim angle = Math.Atan2(pt.Y - a.Center.Y, pt.X - a.Center.X) * 180 / Math.PI

        If IsAngleOnArc(angle, a.StartAngle, a.EndAngle, a.IsClockwise) Then
            Return distToCircle
        End If

        Return Single.MaxValue ' IMPORTANT: means "not on arc"

    End Function




    ' =========================================================
    ' QUADTREE BUILD
    ' =========================================================
    Public Sub BuildSpatialIndex(entities As IEnumerable(Of CadEntity))

        _allEntities = entities.ToList()

        ' TODO: replace with real model bounds if available
        Dim bounds As New RectangleF(-100000, -100000, 200000, 200000)

        _spatialIndex = New QuadTree(Of CadEntity)(bounds, 8)

        For Each ent In _allEntities

            Dim b = GetBounds(ent)

            If b = RectangleF.Empty Then Continue For

            _spatialIndex.Insert(New QuadTree(Of CadEntity).QuadItem With {
            .Bounds = b,
            .Value = ent
        })

        Next

    End Sub

    ' =========================================================
    ' PUBLIC HIT ENTRY POINT (AUTHORITATIVE)
    ' =========================================================
    Public Function HitTest(pt As PointF, tolPx As Single) As ISelectable
        Return HitTestInternal(pt, tolPx)
    End Function

    Public Function ResolveBestHit(pt As PointF, tol As Single) As ISelectable
        Return HitTest(pt, tol)
    End Function

    ' =========================================================
    ' HOVER PIPELINE
    ' =========================================================
    Public Sub UpdateHover(screenPt As PointF, tolerance As Single)

        Dim hit = HitTest(screenPt, tolerance)

        If Not Object.ReferenceEquals(hit, _hovered) Then
            _hovered = hit
            RaiseEvent HoverChanged(_hovered)
        End If

    End Sub

    ' =========================================================
    ' SELECTION PIPELINE
    ' =========================================================
    Public Sub SelectAtPoint(screenPt As PointF, tolerance As Single)

        Dim hit = HitTest(screenPt, tolerance)
        If hit Is Nothing Then Return

        ApplySelection(hit)

    End Sub

    Private Sub ApplySelection(ent As ISelectable)

        If _selected.Contains(ent) Then
            _selected.Remove(ent)
            ent.IsSelected = False
        Else
            _selected.Add(ent)
            ent.IsSelected = True
        End If

        RaiseEvent SelectionChanged(_selected)

    End Sub

    Public Sub Clear()

        For Each item In _selected
            item.IsSelected = False
        Next

        _selected.Clear()

        RaiseEvent SelectionChanged(_selected)

    End Sub
    Private Function HitTestInternal(pt As PointF, tolPx As Single) As ISelectable

        If _spatialIndex Is Nothing Then Return Nothing

        Dim searchArea As New RectangleF(
        pt.X - tolPx,
        pt.Y - tolPx,
        tolPx * 2,
        tolPx * 2)

        Dim candidates = _spatialIndex.Query(searchArea)

        Dim best As HitResult = Nothing

        For Each ent In candidates

            Dim d As Single = Single.MaxValue

            Select Case ent.GeometryType

                Case GeometryTypeEnum.Line
                    Dim l = DirectCast(ent, CadLine)
                    d = DistancePointToLineWorld(pt, l.StartPoint, l.EndPoint)

                Case GeometryTypeEnum.Circle
                    Dim c = DirectCast(ent, CadCircle)
                    d = Math.Abs(Distance(pt, c.Center) - c.Radius)

                Case GeometryTypeEnum.Arc
                    Dim a = DirectCast(ent, CadArc)
                    d = Math.Abs(Distance(pt, a.Center) - a.Radius)

                Case GeometryTypeEnum.Point
                    Dim p = DirectCast(ent, CadPoint)
                    d = Distance(pt, p.Position)

            End Select

            If d <= tolPx Then

                Dim candidate As New HitResult With {
                .Entity = ent,
                .Distance = d,
                .Priority = GetPriority(ent)
            }

                If best Is Nothing Then
                    best = candidate
                ElseIf candidate.Priority < best.Priority OrElse
                   (candidate.Priority = best.Priority AndAlso candidate.Distance < best.Distance) Then
                    best = candidate
                End If

            End If

        Next

        If best Is Nothing Then Return Nothing
        Return TryCast(best.Entity, ISelectable)

    End Function

    Private Function IsAngleOnArc(angle As Double,
                             startAngle As Double,
                             endAngle As Double,
                             isClockwise As Boolean) As Boolean

        If isClockwise Then
            Return angle <= startAngle AndAlso angle >= endAngle
        Else
            Return angle >= startAngle AndAlso angle <= endAngle
        End If

    End Function

    ' =========================================================
    ' GEOMETRY BOUNDS (QUADTREE SUPPORT)
    ' =========================================================
    Private Function GetBounds(ent As CadEntity) As RectangleF

        Select Case ent.GeometryType

            Case GeometryTypeEnum.Line
                Dim l = DirectCast(ent, CadLine)

                Dim minX = Math.Min(l.StartPoint.X, l.EndPoint.X)
                Dim maxX = Math.Max(l.StartPoint.X, l.EndPoint.X)
                Dim minY = Math.Min(l.StartPoint.Y, l.EndPoint.Y)
                Dim maxY = Math.Max(l.StartPoint.Y, l.EndPoint.Y)

                Return RectangleF.FromLTRB(minX, minY, maxX, maxY)

            Case GeometryTypeEnum.Circle
                Dim c = DirectCast(ent, CadCircle)

                Return New RectangleF(
                    c.Center.X - c.Radius,
                    c.Center.Y - c.Radius,
                    c.Radius * 2,
                    c.Radius * 2
                )

            Case GeometryTypeEnum.Arc
                Dim a = DirectCast(ent, CadArc)

                Return New RectangleF(
                    a.Center.X - a.Radius,
                    a.Center.Y - a.Radius,
                    a.Radius * 2,
                    a.Radius * 2
                )

            Case Else
                Return RectangleF.Empty

        End Select

    End Function


    Private Function DistancePointToLineWorld(p As PointF,
                                          a As PointF,
                                          b As PointF) As Single

        Dim ap = New PointF(p.X - a.X, p.Y - a.Y)
        Dim ab = New PointF(b.X - a.X, b.Y - a.Y)

        Dim ab2 = ab.X * ab.X + ab.Y * ab.Y
        If ab2 = 0 Then Return Distance(p, a)

        Dim t = (ap.X * ab.X + ap.Y * ab.Y) / ab2
        t = Math.Max(0, Math.Min(1, t))

        Dim closest = New PointF(a.X + ab.X * t, a.Y + ab.Y * t)

        Return Distance(p, closest)

    End Function

    Private Function Distance(a As PointF, b As PointF) As Single
        Return CSng(Math.Sqrt((a.X - b.X) ^ 2 + (a.Y - b.Y) ^ 2))
    End Function
End Class