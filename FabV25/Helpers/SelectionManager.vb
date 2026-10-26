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

    Private _geoEntities As New List(Of GeoEntity)
    Private _geoSpatialIndex As QuadTree(Of GeoEntity)

    Private ReadOnly _selected As New HashSet(Of isSelectable)
    Private _hovered As isSelectable
    Private _nativeHovered As isSelectable

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

    Public Sub SetGeoEntities(entities As IEnumerable(Of GeoEntity))

        If entities Is Nothing Then
            _geoEntities = New List(Of GeoEntity)
        Else
            _geoEntities = entities.ToList()
        End If

        ' Invalidate the native spatial index.
        _geoSpatialIndex = Nothing

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


    End Sub

    Public Sub BuildGeoSpatialIndex()

        _geoSpatialIndex = Nothing

        If _geoEntities Is Nothing OrElse
       _geoEntities.Count = 0 Then
            Return
        End If

        Dim minX As Single = Single.MaxValue
        Dim minY As Single = Single.MaxValue
        Dim maxX As Single = Single.MinValue
        Dim maxY As Single = Single.MinValue

        Dim validEntities As New List(Of GeoEntity)

        For Each ent As GeoEntity In _geoEntities

            Dim b As RectangleF = GetBounds(ent)

            If b = RectangleF.Empty Then Continue For

            validEntities.Add(ent)

            minX = Math.Min(minX, b.Left)
            minY = Math.Min(minY, b.Top)
            maxX = Math.Max(maxX, b.Right)
            maxY = Math.Max(maxY, b.Bottom)

        Next

        ' No valid geometry bounds.
        If validEntities.Count = 0 Then Return

        Debug.WriteLine(
    "NATIVE GEO VALID BOUNDS = " &
    validEntities.Count)

        Dim rootBounds As New RectangleF(
        minX,
        minY,
        maxX - minX,
        maxY - minY)

        _geoSpatialIndex =
        New QuadTree(Of GeoEntity)(rootBounds, 8)

        For Each ent As GeoEntity In validEntities

            Dim b As RectangleF = GetBounds(ent)

            _geoSpatialIndex.Insert(
            New QuadTree(Of GeoEntity).QuadItem With {
                .Bounds = b,
                .Value = ent
            })

        Next

        Dim queryResults As List(Of GeoEntity) =
    _geoSpatialIndex.Query(rootBounds)

        Debug.WriteLine(
    "NATIVE GEO QUADTREE QUERY COUNT = " &
    queryResults.Count)

        Debug.WriteLine(
    "NATIVE GEO SPATIAL INDEX BUILT")
    End Sub
#End Region

#Region "Selection Entry Point (PRIMARY API)"
    Public Function SelectAtPoint(
    ptWorld As PointF,
    tolWorld As Single,
    selectionMode As GeometryTypeEnum?,
    selectionFilter As SelectionFilter,
    Optional deselect As Boolean = False) As isSelectable

        Dim nativeHit As isSelectable =
    HitTestGeoAtPoint(
        ptWorld,
        tolWorld,
        selectionMode,
        selectionFilter)

        If nativeHit IsNot Nothing Then

            If deselect Then
                ApplyDeselection(nativeHit)
            Else
                ApplySelection(nativeHit)
            End If

        End If

        Debug.WriteLine(
    "NATIVE SELECTION: " &
    If(nativeHit Is Nothing,
       "NO HIT",
       nativeHit.GetType().Name &
       " Selected=" & nativeHit.IsSelected.ToString()))
        Return nativeHit

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


    Private Function HitTestGeoAtPoint(
    ptWorld As PointF,
    tolWorld As Single,
    Optional selectionMode As GeometryTypeEnum? = Nothing,
    Optional selectionFilter As SelectionFilter? = Nothing) As isSelectable

        Dim candidates As List(Of GeoEntity)

        If _geoSpatialIndex IsNot Nothing Then

            candidates = _geoSpatialIndex.Query(
            New RectangleF(
                ptWorld.X - tolWorld,
                ptWorld.Y - tolWorld,
                tolWorld * 2,
                tolWorld * 2))

            If candidates.Count = 0 Then
                candidates = _geoEntities
            End If

        Else
            candidates = _geoEntities
        End If

        Dim best As HitResult = Nothing

        For Each ent As GeoEntity In candidates

            If Not IsGeoSelectionModeMatch(ent, selectionMode) Then
                Continue For
            End If

            If selectionFilter.HasValue AndAlso
                 Not IsEntityAllowedBySelectionFilter(ent, selectionFilter.Value) Then

                Continue For
            End If

            Dim d As Single =
            EvaluateDistance(ent, ptWorld)

            If d > tolWorld Then Continue For

            Dim candidate As New HitResult With {
            .Entity = ent,
            .Distance = d,
            .Priority = GetPriority(ent)
        }

            If best Is Nothing OrElse
           IsBetter(candidate, best) Then

                best = candidate

            End If

        Next

        If best Is Nothing Then Return Nothing

        Return best.Entity

    End Function
    Public Function TestGeoHitAtPoint(
    ptWorld As PointF,
    tolWorld As Single) As Boolean

        Dim hit As isSelectable =
        HitTestGeoAtPoint(ptWorld, tolWorld)

        If hit IsNot Nothing Then
            Debug.WriteLine(
            "NATIVE HIT ENTITY TYPE = " &
            hit.GetType().Name)
        End If

        Return hit IsNot Nothing

    End Function

#End Region


#Region "Hover"
    Public Sub UpdateHover(ptWorld As PointF, tolWorld As Single)

        Dim hit As isSelectable =
        HitTestAtPoint(ptWorld, tolWorld)

        ' -----------------------------------------
        ' TEMPORARY CAD / NATIVE HIT COMPARISON
        ' -----------------------------------------

        Dim nativeHit As isSelectable =
        HitTestGeoAtPoint(ptWorld, tolWorld)

        Dim cadType As String =
        If(hit Is Nothing, "None", hit.GetType().Name)

        Dim nativeType As String =
        If(nativeHit Is Nothing, "None", nativeHit.GetType().Name)

        If (hit Is Nothing) <> (nativeHit Is Nothing) OrElse
       (hit IsNot Nothing AndAlso
        nativeHit IsNot Nothing AndAlso
        cadType.Replace("Cad", "") <> nativeType.Replace("Geo", "")) Then

            Debug.WriteLine(
            "HIT MISMATCH: CAD=" & cadType &
            " NATIVE=" & nativeType)

        End If

        ' -----------------------------------------
        ' EXISTING CAD HOVER BEHAVIOR
        ' -----------------------------------------

        If Not Object.ReferenceEquals(hit, _hovered) Then

            _hovered = hit

            RaiseEvent HoverChanged(_hovered)

        End If

        ' -----------------------------------------
        ' NATIVE GEOMETRY HOVER STATE
        ' -----------------------------------------

        If Not Object.ReferenceEquals(nativeHit, _nativeHovered) Then

            If _nativeHovered IsNot Nothing Then
                _nativeHovered.IsHovered = False
            End If

            _nativeHovered = nativeHit

            If _nativeHovered IsNot Nothing Then
                _nativeHovered.IsHovered = True
            End If

            RaiseEvent HoverChanged(_hovered)

        End If

    End Sub

#End Region

#Region "Selection Core"
    Private Sub ApplySelection(ent As isSelectable)

        If ent Is Nothing Then Return

        If Not _selected.Contains(ent) Then

            _selected.Add(ent)
            ent.IsSelected = True

            RaiseEvent SelectionChanged(_selected)

        End If

    End Sub


    Private Sub ApplyDeselection(ent As isSelectable)

        If ent Is Nothing Then Return

        If _selected.Contains(ent) Then

            _selected.Remove(ent)
            ent.IsSelected = False

            RaiseEvent SelectionChanged(_selected)

        End If

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


    Public Sub SelectGeoByWindow(
    worldRect As RectangleF,
    selectionFilter As SelectionFilter,
    Optional isReverseWindow As Boolean = False,
    Optional deselect As Boolean = False)


        If worldRect.Width <= 0 OrElse
       worldRect.Height <= 0 Then

            Return
        End If

        If _geoEntities Is Nothing Then Return

        For Each ent As GeoEntity In _geoEntities

            If ent Is Nothing Then Continue For

            ' Respect the toolbar selection filter.
            If Not IsEntityAllowedBySelectionFilter(ent, selectionFilter) Then

                Continue For

            End If

            ' Regular window requires full containment.
            Dim isInside As Boolean =
    IsGeoEntityFullyInsideWindow(ent, worldRect)

            Dim isIntersecting As Boolean = False

            If isReverseWindow Then

                If TypeOf ent Is GeoLine Then

                    isIntersecting = IsGeoLineIntersectingWindow(DirectCast(ent, GeoLine), worldRect)

                ElseIf TypeOf ent Is GeoCircle Then

                    isIntersecting = IsGeoCircleIntersectingWindow(DirectCast(ent, GeoCircle), worldRect)

                ElseIf TypeOf ent Is GeoArc Then

                    isIntersecting = IsGeoArcIntersectingWindow(
                        DirectCast(ent, GeoArc),
                        worldRect)

                End If

            End If

            If Not isInside AndAlso Not isIntersecting Then
                Continue For
            End If
            ' Select or deselect the native geometry.
            If deselect Then

                ApplyDeselection(ent)

            Else

                ApplySelection(ent)

            End If


            ' Selection and filter handling
            ' will be added in the next step.

        Next

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
        Public Entity As isSelectable
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

    Private Function GetPriority(ent As GeoEntity) As Integer

        If ent Is Nothing Then Return 10

        If TypeOf ent Is GeoPoint Then
            Return 0

        ElseIf TypeOf ent Is GeoLine Then
            Return 1

        ElseIf TypeOf ent Is GeoArc Then
            Return 2

        ElseIf TypeOf ent Is GeoCircle Then
            Return 3

        End If

        Return 10

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


    Private Function EvaluateDistance(
    ent As GeoEntity,
    pt As PointF) As Single

        If ent Is Nothing Then Return Single.MaxValue

        If TypeOf ent Is GeoPoint Then

            Dim p = DirectCast(ent, GeoPoint)

            Return Distance(
            pt,
            New PointF(CSng(p.X), CSng(p.Y)))

        ElseIf TypeOf ent Is GeoLine Then

            Dim l = DirectCast(ent, GeoLine)

            If l.StartPoint Is Nothing OrElse
           l.EndPoint Is Nothing Then
                Return Single.MaxValue
            End If

            Return DistancePointToSegment(
            pt,
            New PointF(
                CSng(l.StartPoint.X),
                CSng(l.StartPoint.Y)),
            New PointF(
                CSng(l.EndPoint.X),
                CSng(l.EndPoint.Y)))

        ElseIf TypeOf ent Is GeoCircle Then

            Dim c = DirectCast(ent, GeoCircle)

            If c.Center Is Nothing Then Return Single.MaxValue

            Dim center As New PointF(
            CSng(c.Center.X),
            CSng(c.Center.Y))

            Return CSng(Math.Abs(
            Distance(pt, center) - c.Radius))

        ElseIf TypeOf ent Is GeoArc Then

            Dim a = DirectCast(ent, GeoArc)

            If a.Center Is Nothing Then Return Single.MaxValue

            Dim center As New PointF(
            CSng(a.Center.X),
            CSng(a.Center.Y))

            Dim radialDist As Single = Distance(pt, center)

            Dim dist As Single =
            CSng(Math.Abs(radialDist - a.Radius))

            Dim rawAng As Double =
            Math.Atan2(
                pt.Y - a.Center.Y,
                pt.X - a.Center.X) *
            180.0 / Math.PI

            Dim ang As Double = NormalizeAngle(rawAng)

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

        End If

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
    Private Function GetBounds(ent As GeoEntity) As RectangleF

        If ent Is Nothing Then
            Return RectangleF.Empty
        End If

        If TypeOf ent Is GeoLine Then

            Dim l = DirectCast(ent, GeoLine)

            If l.StartPoint Is Nothing OrElse
           l.EndPoint Is Nothing Then
                Return RectangleF.Empty
            End If

            Return RectangleF.FromLTRB(
            CSng(Math.Min(l.StartPoint.X, l.EndPoint.X)),
            CSng(Math.Min(l.StartPoint.Y, l.EndPoint.Y)),
            CSng(Math.Max(l.StartPoint.X, l.EndPoint.X)),
            CSng(Math.Max(l.StartPoint.Y, l.EndPoint.Y)))

        ElseIf TypeOf ent Is GeoCircle Then

            Dim c = DirectCast(ent, GeoCircle)

            If c.Center Is Nothing Then
                Return RectangleF.Empty
            End If

            Return New RectangleF(
            CSng(c.Center.X - c.Radius),
            CSng(c.Center.Y - c.Radius),
            CSng(c.Radius * 2),
            CSng(c.Radius * 2))

        ElseIf TypeOf ent Is GeoArc Then

            Dim a = DirectCast(ent, GeoArc)

            If a.Center Is Nothing Then
                Return RectangleF.Empty
            End If

            ' Conservative bounds: full enclosing circle
            Return New RectangleF(
            CSng(a.Center.X - a.Radius),
            CSng(a.Center.Y - a.Radius),
            CSng(a.Radius * 2),
            CSng(a.Radius * 2))

        ElseIf TypeOf ent Is GeoPoint Then

            Dim p = DirectCast(ent, GeoPoint)

            Return New RectangleF(
            CSng(p.X),
            CSng(p.Y),
            0.0F,
            0.0F)

        End If

        Return RectangleF.Empty

    End Function
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


    Private Function IsEntityAllowedBySelectionFilter(
    ent As GeoEntity,
    selectionFilter As SelectionFilter) As Boolean

        If ent Is Nothing Then Return False

        If selectionFilter = SelectionFilter.None Then
            Return False
        End If

        If TypeOf ent Is GeoLine Then

            Return (CInt(selectionFilter) And
                    CInt(SelectionFilter.Line)) <> 0

        ElseIf TypeOf ent Is GeoCircle Then

            Return (CInt(selectionFilter) And
                    CInt(SelectionFilter.Circle)) <> 0

        ElseIf TypeOf ent Is GeoArc Then

            Return (CInt(selectionFilter) And
                    CInt(SelectionFilter.Arc)) <> 0

        ElseIf TypeOf ent Is GeoPoint Then

            Return (CInt(selectionFilter) And
                    CInt(SelectionFilter.Point)) <> 0

        End If

        Return False

    End Function

    Private Function IsGeoSelectionModeMatch(
    ent As GeoEntity,
    selectionMode As GeometryTypeEnum?) As Boolean

        If ent Is Nothing Then Return False

        ' No specific geometry mode means all types are eligible.
        If Not selectionMode.HasValue Then Return True

        Select Case selectionMode.Value

            Case GeometryTypeEnum.Line
                Return TypeOf ent Is GeoLine

            Case GeometryTypeEnum.Circle
                Return TypeOf ent Is GeoCircle

            Case GeometryTypeEnum.Arc
                Return TypeOf ent Is GeoArc

            Case GeometryTypeEnum.Point
                Return TypeOf ent Is GeoPoint

            Case Else
                Return False

        End Select

    End Function

    Private Function IsGeoLineFullyInsideWindow(
    line As GeoLine,
    worldRect As RectangleF) As Boolean

        If line Is Nothing OrElse
           line.StartPoint Is Nothing OrElse
           line.EndPoint Is Nothing Then

            Return False

        End If

        Return worldRect.Contains(
            CSng(line.StartPoint.X),
            CSng(line.StartPoint.Y)) AndAlso
            worldRect.Contains(
                CSng(line.EndPoint.X),
                CSng(line.EndPoint.Y))

    End Function

    Private Function IsGeoLineIntersectingWindow(
    line As GeoLine,
    worldRect As RectangleF) As Boolean

        If line Is Nothing OrElse
       line.StartPoint Is Nothing OrElse
       line.EndPoint Is Nothing Then

            Return False
        End If

        Dim x1 As Double = line.StartPoint.X
        Dim y1 As Double = line.StartPoint.Y
        Dim x2 As Double = line.EndPoint.X
        Dim y2 As Double = line.EndPoint.Y

        Dim dx As Double = x2 - x1
        Dim dy As Double = y2 - y1

        Dim tMin As Double = 0.0
        Dim tMax As Double = 1.0

        Dim p() As Double = {-dx, dx, -dy, dy}

        Dim q() As Double = {
        x1 - worldRect.Left,
        worldRect.Right - x1,
        y1 - worldRect.Top,
        worldRect.Bottom - y1
    }

        For i As Integer = 0 To 3

            If Math.Abs(p(i)) < 0.000000000001 Then

                If q(i) < 0 Then Return False

            Else

                Dim t As Double = q(i) / p(i)

                If p(i) < 0 Then

                    tMin = Math.Max(tMin, t)

                Else

                    tMax = Math.Min(tMax, t)

                End If

                If tMin > tMax Then Return False

            End If

        Next

        Return True

    End Function



    Private Function IsGeoPointFullyInsideWindow(
    pt As GeoPoint,
    worldRect As RectangleF) As Boolean

        If pt Is Nothing Then
            Return False
        End If

        Return worldRect.Contains(
            CSng(pt.X),
            CSng(pt.Y))

    End Function

    Private Function IsGeoCircleFullyInsideWindow(
    circle As GeoCircle,
    worldRect As RectangleF) As Boolean

        If circle Is Nothing OrElse
           circle.Center Is Nothing Then

            Return False

        End If

        Dim cx As Double = circle.Center.X
        Dim cy As Double = circle.Center.Y
        Dim r As Double = circle.Radius

        If r < 0 Then Return False

        Return cx - r >= worldRect.Left AndAlso
               cx + r <= worldRect.Right AndAlso
               cy - r >= worldRect.Top AndAlso
               cy + r <= worldRect.Bottom

    End Function

    Private Function IsGeoCircleIntersectingWindow(
    circle As GeoCircle,
    worldRect As RectangleF) As Boolean

        If circle Is Nothing OrElse
       circle.Center Is Nothing Then

            Return False
        End If

        Dim cx As Double = circle.Center.X
        Dim cy As Double = circle.Center.Y
        Dim r As Double = circle.Radius

        If r < 0 Then Return False

        ' Find the closest point on the rectangle
        ' to the circle center.

        Dim closestX As Double =
        Math.Max(worldRect.Left,
                 Math.Min(cx, worldRect.Right))

        Dim closestY As Double =
        Math.Max(worldRect.Top,
                 Math.Min(cy, worldRect.Bottom))

        Dim dx As Double = cx - closestX
        Dim dy As Double = cy - closestY

        Dim minDistSquared As Double =
        dx * dx + dy * dy

        ' Find the farthest rectangle corner
        ' from the circle center.

        Dim maxDx As Double =
        Math.Max(Math.Abs(cx - worldRect.Left),
                 Math.Abs(cx - worldRect.Right))

        Dim maxDy As Double =
        Math.Max(Math.Abs(cy - worldRect.Top),
                 Math.Abs(cy - worldRect.Bottom))

        Dim maxDistSquared As Double =
        maxDx * maxDx + maxDy * maxDy

        Dim radiusSquared As Double = r * r

        ' The circumference intersects the rectangle
        ' when its radius lies between the minimum
        ' and maximum distances.

        Return radiusSquared >= minDistSquared AndAlso
           radiusSquared <= maxDistSquared

    End Function

    Private Function IsGeoArcFullyInsideWindow(
    arc As GeoArc,
    worldRect As RectangleF) As Boolean

        If arc Is Nothing OrElse arc.Center Is Nothing Then
            Return False
        End If

        If arc.Radius < 0 Then Return False

        Dim testAngles As New List(Of Double) From {
            arc.StartAngle,
            arc.EndAngle
        }

        Dim cardinalAngles() As Double = {
            0.0, 90.0, 180.0, 270.0
        }

        For Each angle As Double In cardinalAngles

            If IsGeoAngleOnArc(
                angle,
                arc.StartAngle,
                arc.EndAngle,
                arc.IsClockwise) Then

                testAngles.Add(angle)

            End If

        Next

        For Each angle As Double In testAngles

            Dim radians As Double = angle * Math.PI / 180.0

            Dim x As Double =
                arc.Center.X + arc.Radius * Math.Cos(radians)

            Dim y As Double =
                arc.Center.Y + arc.Radius * Math.Sin(radians)

            If x < worldRect.Left OrElse
               x > worldRect.Right OrElse
               y < worldRect.Top OrElse
               y > worldRect.Bottom Then

                Return False

            End If

        Next

        Return True

    End Function



    Private Function IsGeoArcIntersectingWindow(
    arc As GeoArc,
    worldRect As RectangleF) As Boolean

        If arc Is Nothing OrElse
       arc.Center Is Nothing Then Return False

        If arc.Radius < 0 Then Return False

        Dim cx As Double = arc.Center.X
        Dim cy As Double = arc.Center.Y
        Dim r As Double = arc.Radius

        ' Check the arc endpoints.

        Dim endpointAngles() As Double = {
        arc.StartAngle,
        arc.EndAngle
    }

        For Each angle As Double In endpointAngles

            Dim radians As Double =
            angle * Math.PI / 180.0

            Dim x As Double = cx + r * Math.Cos(radians)
            Dim y As Double = cy + r * Math.Sin(radians)

            If worldRect.Contains(CSng(x), CSng(y)) Then
                Return True
            End If

        Next

        ' Check intersections with the vertical edges.

        Dim verticalEdges() As Double = {
        worldRect.Left,
        worldRect.Right
    }

        For Each edgeX As Double In verticalEdges

            Dim dx As Double = edgeX - cx
            Dim discriminant As Double = r * r - dx * dx

            If discriminant < 0 Then Continue For

            Dim dy As Double = Math.Sqrt(discriminant)

            Dim intersectionY() As Double = {
            cy + dy,
            cy - dy
        }

            For Each y As Double In intersectionY

                If y < worldRect.Top OrElse
               y > worldRect.Bottom Then Continue For

                Dim angle As Double =
                Math.Atan2(y - cy, edgeX - cx) *
                180.0 / Math.PI

                If IsGeoAngleOnArc(
                angle,
                arc.StartAngle,
                arc.EndAngle,
                arc.IsClockwise) Then

                    Return True

                End If

            Next

        Next

        ' Check intersections with the horizontal edges.

        Dim horizontalEdges() As Double = {
        worldRect.Top,
        worldRect.Bottom
    }

        For Each edgeY As Double In horizontalEdges

            Dim dy As Double = edgeY - cy
            Dim discriminant As Double = r * r - dy * dy

            If discriminant < 0 Then Continue For

            Dim dx As Double = Math.Sqrt(discriminant)

            Dim intersectionX() As Double = {
            cx + dx,
            cx - dx
        }

            For Each x As Double In intersectionX

                If x < worldRect.Left OrElse
               x > worldRect.Right Then Continue For

                Dim angle As Double =
                Math.Atan2(edgeY - cy, x - cx) *
                180.0 / Math.PI

                If IsGeoAngleOnArc(
                angle,
                arc.StartAngle,
                arc.EndAngle,
                arc.IsClockwise) Then

                    Return True

                End If

            Next

        Next

        Return False

    End Function


    Private Function IsGeoEntityFullyInsideWindow(
    ent As GeoEntity,
    worldRect As RectangleF) As Boolean

        If ent Is Nothing Then
            Return False
        End If

        If TypeOf ent Is GeoLine Then

            Return IsGeoLineFullyInsideWindow(
            DirectCast(ent, GeoLine),
            worldRect)

        ElseIf TypeOf ent Is GeoArc Then

            Return IsGeoArcFullyInsideWindow(
            DirectCast(ent, GeoArc),
            worldRect)

        ElseIf TypeOf ent Is GeoCircle Then

            Return IsGeoCircleFullyInsideWindow(
            DirectCast(ent, GeoCircle),
            worldRect)

        ElseIf TypeOf ent Is GeoPoint Then

            Return IsGeoPointFullyInsideWindow(
            DirectCast(ent, GeoPoint),
            worldRect)

        End If

        Return False

    End Function

    Private Function IsGeoAngleOnArc(
    testAngle As Double,
    startAngle As Double,
    endAngle As Double,
    clockwise As Boolean) As Boolean

        testAngle = ((testAngle Mod 360.0) + 360.0) Mod 360.0
        startAngle = ((startAngle Mod 360.0) + 360.0) Mod 360.0
        endAngle = ((endAngle Mod 360.0) + 360.0) Mod 360.0

        If clockwise Then

            Dim sweep As Double =
                (startAngle - endAngle + 360.0) Mod 360.0

            Dim distance As Double =
                (startAngle - testAngle + 360.0) Mod 360.0

            Return distance <= sweep

        Else

            Dim sweep As Double =
                (endAngle - startAngle + 360.0) Mod 360.0

            Dim distance As Double =
                (testAngle - startAngle + 360.0) Mod 360.0

            Return distance <= sweep

        End If

    End Function

End Class