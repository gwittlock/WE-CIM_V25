Imports FabV25_WIN8.CadEntity
Public Enum SelectionFilter
    None = 0
    Line = 1 << 0
    Circle = 1 << 1
    Arc = 1 << 2
    Polyline = 1 << 3
    Rectangle = 1 << 4
    Point = 1 << 5
    AllGeometry = Line Or Circle Or Arc Or Polyline Or Rectangle Or Point
End Enum
Public Class SelectionManager

#Region "State"

    Private _entities As List(Of CadEntity) = New List(Of CadEntity)
    Private _spatialIndex As QuadTree(Of CadEntity)

    Private ReadOnly _selected As New HashSet(Of isSelectable)
    Private _hovered As isSelectable

#End Region

#Region "Events"

    Public Event SelectionChanged As Action(Of IEnumerable(Of isSelectable))
    Public Event HoverChanged As Action(Of isSelectable)

#End Region

#Region "Selection icon states"

    'Public Enum SELECTION_FILTER
    '    SEL_NONE = &H0
    '    SEL_LAYER = &H1         'OBSOLETE
    '    SEL_WORK = &H2
    '    SEL_TOOL = &H4
    '    SEL_POINT = &H8
    '    SEL_LINE = &H10
    '    SEL_ARC = &H20
    '    SEL_HOLE = &H40
    '    SEL_PROFILE = &H80
    '    SEL_COMMAND = &H100
    '    SEL_FEATURE = &H200
    '    SEL_SEQUENCE = &H400
    '    SEL_PATTERN = &H800
    '    SEL_ALL = &HFFF
    'End Enum

#End Region

#Region "Public Access"

    Public ReadOnly Property Selected As IEnumerable(Of isSelectable)
        Get
            Return _selected
        End Get
    End Property

    Public ReadOnly Property Hovered As isSelectable
        Get
            Return _hovered
        End Get
    End Property

    Public ReadOnly Property HasSelection As Boolean
        Get
            Return _selected.Count > 0
        End Get
    End Property

#End Region

#Region "Initialization"

    Public Sub SetEntities(entities As IEnumerable(Of CadEntity))

        If entities Is Nothing Then
            _entities = New List(Of CadEntity)
        Else
            _entities = entities.ToList()
        End If

        _spatialIndex = Nothing

    End Sub

    Public Sub BuildSpatialIndex()

        If _entities Is Nothing OrElse _entities.Count = 0 Then
            _spatialIndex = Nothing
            Return
        End If

        Dim minX As Single = Single.MaxValue
        Dim minY As Single = Single.MaxValue
        Dim maxX As Single = Single.MinValue
        Dim maxY As Single = Single.MinValue

        For Each ent In _entities
            Dim b = GetBounds(ent)
            If b = RectangleF.Empty Then Continue For

            minX = Math.Min(minX, b.Left)
            minY = Math.Min(minY, b.Top)
            maxX = Math.Max(maxX, b.Right)
            maxY = Math.Max(maxY, b.Bottom)
        Next

        Dim rootBounds As New RectangleF(minX, minY, maxX - minX, maxY - minY)
        _spatialIndex = New QuadTree(Of CadEntity)(rootBounds, 8)

        For Each ent In _entities
            Dim b = GetBounds(ent)
            If b = RectangleF.Empty Then Continue For

            _spatialIndex.Insert(New QuadTree(Of CadEntity).QuadItem With {
                .Bounds = b,
                .Value = ent
            })
        Next


        Debug.Print("========== QUADTREE CHECK ==========")
        Debug.Print("Entity count = " & _entities.Count)

        Dim pointCount As Integer = 0

        For Each entity As CadEntity In _entities
            If TypeOf entity Is CadPoint Then
                pointCount += 1

                Dim cp As CadPoint = DirectCast(entity, CadPoint)

                Debug.Print(
                    "QuadTree input POINT: X=" &
                    cp.Position.X.ToString("0.######") &
                    ", Y=" &
                    cp.Position.Y.ToString("0.######"))
            End If
        Next

        Debug.Print("CadPoint count entering QuadTree = " & pointCount)
        Debug.Print("====================================")
    End Sub

#End Region

#Region "Selection Entry Point (PRIMARY API)"
    Public Function SelectAtPoint(
    ptWorld As PointF,
    tolWorld As Single,
    selectionMode As GeometryTypeEnum?,
    selectionFilter As SelectionFilter) As isSelectable

        Dim best As HitResult = Nothing
        Dim candidates As List(Of CadEntity)

        If _spatialIndex IsNot Nothing Then

            candidates = _spatialIndex.Query(
            New RectangleF(
                ptWorld.X - tolWorld,
                ptWorld.Y - tolWorld,
                tolWorld * 2,
                tolWorld * 2))

        Else

            candidates = _entities

        End If

        For Each ent In candidates

            If selectionMode.HasValue AndAlso ent.GeometryType <> selectionMode.Value Then
                Continue For
            End If

            If Not IsEntityAllowedBySelectionFilter(ent, selectionFilter) Then
                Continue For
            End If

                Dim d = EvaluateDistance(ent, ptWorld)

                If d > tolWorld Then Continue For

            Dim candidate As New HitResult With {
            .Entity = ent,
            .Distance = d,
            .Priority = GetPriority(ent)
        }


            If best Is Nothing OrElse IsBetter(candidate, best) Then
                best = candidate

            End If

        Next
        If best Is Nothing Then
            Return Nothing
        End If

        Dim result As isSelectable =
            TryCast(best.Entity, isSelectable)

        If result Is Nothing Then
            Return Nothing
        End If

        ApplySelection(result)

        Return result
    End Function

#End Region


#Region "Hit Test"

    Private Function HitTestAtPoint(ptWorld As PointF,
                                tolWorld As Single) As isSelectable

        Dim candidates As List(Of CadEntity)

        If _spatialIndex IsNot Nothing Then

            candidates = _spatialIndex.Query(
            New RectangleF(
                ptWorld.X - tolWorld,
                ptWorld.Y - tolWorld,
                tolWorld * 2,
                tolWorld * 2))


            If candidates.Count = 0 Then

                candidates = _entities

            End If

        Else

            candidates = _entities

        End If

        Dim best As HitResult = Nothing

        For Each ent In candidates

            Dim d As Single = EvaluateDistance(ent, ptWorld)

            If d > tolWorld Then Continue For

            Dim candidate As New HitResult With {
            .Entity = ent,
            .Distance = d,
            .Priority = GetPriority(ent)
        }

            If best Is Nothing OrElse IsBetter(candidate, best) Then
                best = candidate
            End If

        Next

        If best Is Nothing Then

            Return Nothing
        End If


        Return TryCast(best.Entity, isSelectable)

    End Function

#End Region


#Region "Hover"

    Public Sub UpdateHover(ptWorld As PointF, tolWorld As Single)

        Dim hit As isSelectable = HitTestAtPoint(ptWorld, tolWorld)

        If Not Object.ReferenceEquals(hit, _hovered) Then

            _hovered = hit

            RaiseEvent HoverChanged(_hovered)

        End If

    End Sub

#End Region

#Region "Selection Core"
    Private Sub ApplySelection(ent As isSelectable)


        If ent Is Nothing Then
            Return
        End If

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

        For Each s In _selected
            s.IsSelected = False
        Next

        _selected.Clear()
        RaiseEvent SelectionChanged(_selected)

    End Sub

    Public Sub SelectAll()

        If _entities Is Nothing OrElse _entities.Count = 0 Then
            Return
        End If

        For Each ent As CadEntity In _entities

            Dim selectable As isSelectable = TryCast(ent, isSelectable)

            If selectable Is Nothing Then
                Continue For
            End If

            If Not _selected.Contains(selectable) Then
                _selected.Add(selectable)
            End If

            selectable.IsSelected = True

        Next

        RaiseEvent SelectionChanged(_selected)

    End Sub

    Public Sub SelectAllByFilter(
    selectionMode As GeometryTypeEnum?,
    selectionFilter As SelectionFilter)

        If _entities Is Nothing OrElse _entities.Count = 0 Then
            Return
        End If

        For Each ent As CadEntity In _entities

            If selectionMode.HasValue AndAlso
           ent.GeometryType <> selectionMode.Value Then
                Continue For
            End If

            If Not IsEntityAllowedBySelectionFilter(
            ent,
            selectionFilter) Then

                Continue For
            End If

            Dim selectable As isSelectable = TryCast(ent, isSelectable)

            If selectable Is Nothing Then
                Continue For
            End If

            If Not _selected.Contains(selectable) Then
                _selected.Add(selectable)
            End If

            selectable.IsSelected = True

        Next

        RaiseEvent SelectionChanged(_selected)

    End Sub

    Public Sub SelectByLayer(layerName As String)

        If String.IsNullOrWhiteSpace(layerName) Then
            Return
        End If

        If _entities Is Nothing OrElse _entities.Count = 0 Then
            Return
        End If

        Dim selectionChanged As Boolean = False

        For Each ent As CadEntity In _entities

            If ent Is Nothing Then
                Continue For
            End If

            If Not String.Equals(
            ent.LayerName,
            layerName,
            StringComparison.OrdinalIgnoreCase) Then

                Continue For
            End If

            Dim selectable As isSelectable =
            TryCast(ent, isSelectable)

            If selectable Is Nothing Then
                Continue For
            End If

            If Not _selected.Contains(selectable) Then
                _selected.Add(selectable)
                selectionChanged = True
            End If

            selectable.IsSelected = True

        Next

        If selectionChanged Then
            RaiseEvent selectionChanged(_selected)
        End If

    End Sub


    Public Sub SelectByWindow(
    worldRect As RectangleF,
    selectionMode As GeometryTypeEnum?,
    selectionFilter As SelectionFilter)

        If worldRect.Width <= 0 OrElse worldRect.Height <= 0 Then
            Return
        End If

        Dim candidates As List(Of CadEntity)

        ' Use the spatial index when available.
        If _spatialIndex IsNot Nothing Then
            candidates = _spatialIndex.Query(worldRect)
        Else
            candidates = _entities
        End If

        Dim selectionChanged As Boolean = False

        For Each ent As CadEntity In candidates

            If ent Is Nothing Then Continue For

            ' Respect the current geometry mode.
            If selectionMode.HasValue AndAlso
           ent.GeometryType <> selectionMode.Value Then
                Continue For
            End If

            ' Respect the toolbar selection filter.
            If Not IsEntityAllowedBySelectionFilter(
            ent,
            selectionFilter) Then
                Continue For
            End If

            ' Entity must be completely contained by the window.
            If Not IsEntityFullyInsideWindow(ent, worldRect) Then
                Continue For
            End If

            Dim selectable As isSelectable =
            TryCast(ent, isSelectable)

            If selectable Is Nothing Then
                Continue For
            End If

            If Not _selected.Contains(selectable) Then
                _selected.Add(selectable)
                selectable.IsSelected = True
                selectionChanged = True
            Else
                ' Make sure the entity remains visually selected.
                selectable.IsSelected = True
            End If

        Next

        If selectionChanged Then
            RaiseEvent selectionChanged(_selected)
        End If

    End Sub

    Private Function IsEntityFullyInsideWindow(
    ent As CadEntity,
    worldRect As RectangleF) As Boolean

        If ent Is Nothing Then Return False

        Select Case ent.GeometryType

            Case GeometryTypeEnum.Line

                Dim line As CadLine = TryCast(ent, CadLine)
                If line Is Nothing Then Return False

                Return worldRect.Contains(line.StartPoint) AndAlso
                   worldRect.Contains(line.EndPoint)


            Case GeometryTypeEnum.Point

                Dim point As CadPoint = TryCast(ent, CadPoint)
                If point Is Nothing Then Return False

                Return worldRect.Contains(point.Position)


            Case GeometryTypeEnum.Circle

                Dim circle As CadCircle = TryCast(ent, CadCircle)
                If circle Is Nothing Then Return False

                Dim cx As Single = circle.Center.X
                Dim cy As Single = circle.Center.Y
                Dim r As Single = circle.Radius

                Return (
                            cx - r >= worldRect.Left AndAlso
                            cx + r <= worldRect.Right AndAlso
                            cy - r >= worldRect.Top AndAlso
                            cy + r <= worldRect.Bottom)


            Case GeometryTypeEnum.Arc

                Dim arc As CadArc = TryCast(ent, CadArc)
                If arc Is Nothing Then Return False

                Return IsArcFullyInsideWindow(arc, worldRect)


            Case Else

                ' Polyline / Rectangle and any future geometry
                ' are intentionally not selected until their
                ' exact containment rules are defined.
                Return False

        End Select

    End Function

    Private Function IsArcFullyInsideWindow(
    arc As CadArc,
    worldRect As RectangleF) As Boolean

        If arc Is Nothing Then Return False

        Dim cx As Double = arc.Center.X
        Dim cy As Double = arc.Center.Y
        Dim r As Double = arc.Radius

        ' First verify the complete circle containing the arc
        ' is not required to be inside the window. We only need
        ' to test the actual arc.
        '
        ' Test:
        '   1. Arc start point
        '   2. Arc end point
        '   3. Every quadrant direction that lies on the arc
        '
        ' Those points are sufficient because an arc's extrema
        ' occur at the cardinal angles.

        Dim testAngles As New List(Of Double)

        testAngles.Add(arc.StartAngle)
        testAngles.Add(arc.EndAngle)

        Dim cardinalAngles() As Double = {
        0.0,
        90.0,
        180.0,
        270.0
    }

        For Each angle As Double In cardinalAngles

            If IsAngleOnArc(
                                    angle,
                                    arc.StartAngle,
                                    arc.EndAngle,
                                    arc.IsClockwise) Then

                testAngles.Add(angle)

            End If

        Next

        For Each angle As Double In testAngles

            Dim radians As Double =
            angle * Math.PI / 180.0

            Dim x As Single =
            CSng(cx + r * Math.Cos(radians))

            Dim y As Single =
            CSng(cy + r * Math.Sin(radians))

            If Not worldRect.Contains(x, y) Then
                Return False
            End If

        Next

        Return True

    End Function



    Public Sub RemoveAllByFilter(
    selectionMode As GeometryTypeEnum?,
    selectionFilter As SelectionFilter)

        If _selected.Count = 0 Then
            Return
        End If

        Dim toRemove As New List(Of isSelectable)

        For Each selectable As isSelectable In _selected

            Dim ent As CadEntity = TryCast(selectable, CadEntity)

            If ent Is Nothing Then
                Continue For
            End If

            If selectionMode.HasValue AndAlso
           ent.GeometryType <> selectionMode.Value Then
                Continue For
            End If

            If Not IsEntityAllowedBySelectionFilter(
            ent,
            selectionFilter) Then

                Continue For
            End If

            toRemove.Add(selectable)

        Next

        For Each selectable As isSelectable In toRemove
            selectable.IsSelected = False
            _selected.Remove(selectable)
        Next

        RaiseEvent SelectionChanged(_selected)

    End Sub
#End Region

#Region "Hit Testing"

    Private Class HitResult
        Public Entity As CadEntity
        Public Distance As Single
        Public Priority As Integer
    End Class

    Private Function IsBetter(a As HitResult, b As HitResult) As Boolean
        If a.Priority <> b.Priority Then Return a.Priority < b.Priority
        Return a.Distance < b.Distance
    End Function

    Private Function GetPriority(ent As CadEntity) As Integer
        Select Case ent.GeometryType
            Case GeometryTypeEnum.Point : Return 0
            Case GeometryTypeEnum.Line : Return 1
            Case GeometryTypeEnum.Arc : Return 2
            Case GeometryTypeEnum.Circle : Return 3
            Case Else : Return 10
        End Select
    End Function
    Private Function EvaluateDistance(ent As CadEntity, pt As PointF) As Single

        Select Case ent.GeometryType
            Case GeometryTypeEnum.Point

                Dim p = DirectCast(ent, CadPoint)
                Dim dist As Single = Distance(pt, p.Position)

                Return dist

            Case GeometryTypeEnum.Line

                Dim l = DirectCast(ent, CadLine)
                Dim dist As Single = DistancePointToSegment(pt, l.StartPoint, l.EndPoint)

                Return dist


            Case GeometryTypeEnum.Circle

                Dim c = DirectCast(ent, CadCircle)
                Dim radialDist As Single = Distance(pt, c.Center)
                Dim dist As Single = Math.Abs(radialDist - c.Radius)


                Return dist

            Case GeometryTypeEnum.Arc

                Dim a = DirectCast(ent, CadArc)

                Dim radialDist As Single = Distance(pt, a.Center)

                Dim dist As Single =
                Math.Abs(radialDist - a.Radius)

                Dim rawAng As Double =
                Math.Atan2(
                    pt.Y - a.Center.Y,
                    pt.X - a.Center.X) *
                180.0 / Math.PI

                Dim ang As Double =
                NormalizeAngle(rawAng)

                Dim angleOnArc As Boolean =
                IsAngleOnArc(
                    ang,
                    a.StartAngle,
                    a.EndAngle,
                    a.IsClockwise)


                If Not angleOnArc Then
                    Return Single.MaxValue
                End If

                Return dist

        End Select

        Return Single.MaxValue

    End Function

#End Region

#Region "Bounds"

    Private Function GetBounds(ent As CadEntity) As RectangleF

        Select Case ent.GeometryType

            Case GeometryTypeEnum.Line
                Dim l = DirectCast(ent, CadLine)
                Return RectangleF.FromLTRB(
                    Math.Min(l.StartPoint.X, l.EndPoint.X),
                    Math.Min(l.StartPoint.Y, l.EndPoint.Y),
                    Math.Max(l.StartPoint.X, l.EndPoint.X),
                    Math.Max(l.StartPoint.Y, l.EndPoint.Y))

            Case GeometryTypeEnum.Circle
                Dim c = DirectCast(ent, CadCircle)
                Return New RectangleF(
                    c.Center.X - c.Radius,
                    c.Center.Y - c.Radius,
                    c.Radius * 2,
                    c.Radius * 2)

            Case GeometryTypeEnum.Arc
                Dim a = DirectCast(ent, CadArc)
                Return New RectangleF(
                    a.Center.X - a.Radius,
                    a.Center.Y - a.Radius,
                    a.Radius * 2,
                    a.Radius * 2)

            Case GeometryTypeEnum.Point
                Dim p = DirectCast(ent, CadPoint)

                Return New RectangleF(
                    p.Position.X,
                    p.Position.Y,
                    0.0F,
                    0.0F)
        End Select

        Return RectangleF.Empty

    End Function

#End Region

#Region "Geometry Helpers"

    Private Function Distance(a As PointF, b As PointF) As Single
        Return CSng(Math.Sqrt((a.X - b.X) ^ 2 + (a.Y - b.Y) ^ 2))
    End Function

    Private Function DistancePointToSegment(p As PointF, a As PointF, b As PointF) As Single

        Dim ap = New PointF(p.X - a.X, p.Y - a.Y)
        Dim ab = New PointF(b.X - a.X, b.Y - a.Y)

        Dim ab2 = ab.X * ab.X + ab.Y * ab.Y
        If ab2 = 0 Then Return Distance(p, a)

        Dim t = (ap.X * ab.X + ap.Y * ab.Y) / ab2
        t = Math.Max(0, Math.Min(1, t))

        Dim closest = New PointF(a.X + ab.X * t, a.Y + ab.Y * t)
        Return Distance(p, closest)

    End Function

    Private Function NormalizeAngle(a As Double) As Double
        a = a Mod 360
        If a < 0 Then a += 360
        Return a
    End Function

    Private Function IsAngleOnArc(testAngle As Double,
                                  startAngle As Double,
                                  endAngle As Double,
                                  cw As Boolean) As Boolean

        testAngle = NormalizeAngle(testAngle)
        startAngle = NormalizeAngle(startAngle)
        endAngle = NormalizeAngle(endAngle)

        If Not cw Then
            If startAngle <= endAngle Then
                Return testAngle >= startAngle AndAlso testAngle <= endAngle
            Else
                Return testAngle >= startAngle OrElse testAngle <= endAngle
            End If
        Else
            If endAngle <= startAngle Then
                Return testAngle <= startAngle AndAlso testAngle >= endAngle
            Else
                Return testAngle <= startAngle OrElse testAngle >= endAngle
            End If
        End If

    End Function

#End Region

    Private Function IsEntityAllowedBySelectionFilter(
    ent As CadEntity,
    selectionFilter As SelectionFilter) As Boolean

        If ent Is Nothing Then Return False

        If selectionFilter = selectionFilter.None Then
            Return False
        End If

        Select Case ent.GeometryType

            Case GeometryTypeEnum.Line
                Return (CInt(selectionFilter) And
                        CInt(selectionFilter.Line)) <> 0

            Case GeometryTypeEnum.Circle
                Return (CInt(selectionFilter) And
                        CInt(selectionFilter.Circle)) <> 0

            Case GeometryTypeEnum.Arc
                Return (CInt(selectionFilter) And
                        CInt(selectionFilter.Arc)) <> 0

            Case GeometryTypeEnum.Point
                Return (CInt(selectionFilter) And
                        CInt(selectionFilter.Point)) <> 0

            Case GeometryTypeEnum.PolyLine
                Return (CInt(selectionFilter) And
                        CInt(selectionFilter.Polyline)) <> 0

            Case GeometryTypeEnum.Rectangle
                Return (CInt(selectionFilter) And
                        CInt(selectionFilter.Rectangle)) <> 0

            Case Else
                Return False

        End Select

    End Function

End Class