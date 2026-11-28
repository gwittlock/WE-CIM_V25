Imports System.Drawing
Imports FabV25_WIN8.ToolShapes

Public NotInheritable Class PreviewBounds

    Private _minX As Double = Double.MaxValue
    Private _minY As Double = Double.MaxValue
    Private _maxX As Double = Double.MinValue
    Private _maxY As Double = Double.MinValue

    Public ReadOnly Property HasBounds As Boolean
        Get
            Return _minX <> Double.MaxValue
        End Get
    End Property

    Public Sub Include(x As Double, y As Double)

        _minX = Math.Min(_minX, x)
        _minY = Math.Min(_minY, y)
        _maxX = Math.Max(_maxX, x)
        _maxY = Math.Max(_maxY, y)

    End Sub

    Public Sub Include(point As PointF)

        Include(point.X, point.Y)

    End Sub

    Public Sub Include(rect As RectangleF)

        Include(rect.Left, rect.Top)
        Include(rect.Right, rect.Bottom)

    End Sub

    Public Sub IncludeCircle(
    centerX As Double,
    centerY As Double,
    radius As Double)

        radius = Math.Abs(radius)

        Include(centerX - radius, centerY - radius)
        Include(centerX + radius, centerY + radius)

    End Sub

    Public Sub IncludeArc(
    centerX As Double,
    centerY As Double,
    radius As Double,
    startAngle As Double,
    endAngle As Double,
    isClockwise As Boolean)

        radius = Math.Abs(radius)

        Dim rawSweep As Double =
        If(isClockwise,
           startAngle - endAngle,
           endAngle - startAngle)

        If Math.Abs(rawSweep) >= 360.0 - 0.0000001 Then
            IncludeCircle(centerX, centerY, radius)
            Return
        End If

        startAngle = (startAngle Mod 360.0 + 360.0) Mod 360.0
        endAngle = (endAngle Mod 360.0 + 360.0) Mod 360.0

        Dim sweep As Double =
        If(isClockwise,
           (startAngle - endAngle + 360.0) Mod 360.0,
           (endAngle - startAngle + 360.0) Mod 360.0)

        ' Include both endpoints.
        For Each angle As Double In
        New Double() {startAngle, endAngle}

            Dim radians As Double =
            angle * Math.PI / 180.0

            Include(
            centerX + radius * Math.Cos(radians),
            centerY + radius * Math.Sin(radians))

        Next

        ' Include cardinal points within the arc sweep.
        For Each cardinal As Double In
        New Double() {0.0, 90.0, 180.0, 270.0}

            Dim distance As Double =
            If(isClockwise,
               (startAngle - cardinal + 360.0) Mod 360.0,
               (cardinal - startAngle + 360.0) Mod 360.0)

            If distance <= sweep + 0.0000001 Then

                Dim radians As Double =
                cardinal * Math.PI / 180.0

                Include(
                centerX + radius * Math.Cos(radians),
                centerY + radius * Math.Sin(radians))

            End If

        Next

    End Sub

    Public Sub IncludeGeometry(
    entity As GeoEntity,
    Optional instance As PatternInstance = Nothing)

        If entity Is Nothing Then Return

        If TypeOf entity Is GeoLine Then

            Dim line As GeoLine = DirectCast(entity, GeoLine)

            If line.StartPoint Is Nothing OrElse
           line.EndPoint Is Nothing Then Return

            IncludeGeometryPoint(
            line.StartPoint.X,
            line.StartPoint.Y,
            instance)

            IncludeGeometryPoint(
            line.EndPoint.X,
            line.EndPoint.Y,
            instance)

        ElseIf TypeOf entity Is GeoCircle Then

            Dim c As GeoCircle = DirectCast(entity, GeoCircle)

            If c.Center Is Nothing Then Return

            Dim center As PointF =
            GetTransformedPoint(
                c.Center.X,
                c.Center.Y,
                instance)

            IncludeCircle(center.X, center.Y, c.Radius)

        ElseIf TypeOf entity Is GeoArc Then

            Dim a As GeoArc = DirectCast(entity, GeoArc)

            If a.Center Is Nothing Then Return

            Dim center As PointF =
            GetTransformedPoint(
                a.Center.X,
                a.Center.Y,
                instance)

            Dim rotation As Double =
            If(instance Is Nothing, 0.0, instance.Angle)

            IncludeArc(
            center.X,
            center.Y,
            a.Radius,
            a.StartAngle + rotation,
            a.EndAngle + rotation,
            a.IsClockwise)

        ElseIf TypeOf entity Is GeoPoint Then

            Dim pt As GeoPoint = DirectCast(entity, GeoPoint)

            IncludeGeometryPoint(pt.X, pt.Y, instance)

        End If

    End Sub

    Private Sub IncludeGeometryPoint(
    x As Double,
    y As Double,
    instance As PatternInstance)

        Include(GetTransformedPoint(x, y, instance))

    End Sub

    Private Function GetTransformedPoint(
    x As Double,
    y As Double,
    instance As PatternInstance) As PointF

        If instance Is Nothing Then
            Return New PointF(CSng(x), CSng(y))
        End If

        Return PatternGeometry.TransformPatternPoint(
        x,
        y,
        instance)

    End Function


    Public Sub IncludeToolProfile(
    shape As ToolShape,
    centerX As Double,
    centerY As Double,
    angle As Double)

        If shape Is Nothing OrElse
       shape.CuttingProfile Is Nothing Then Return

        Dim radians As Double = angle * Math.PI / 180.0

        Dim cosA As Double = Math.Cos(radians)
        Dim sinA As Double = Math.Sin(radians)

        For Each segment As IPathSegment In
        shape.CuttingProfile.Segments

            If segment Is Nothing Then Continue For

            Select Case segment.SegmentType

                Case PathSegmentType.Line

                    Include(
                    centerX + segment.StartPoint.X * cosA -
                              segment.StartPoint.Y * sinA,
                    centerY + segment.StartPoint.X * sinA +
                              segment.StartPoint.Y * cosA)

                    Include(
                    centerX + segment.EndPoint.X * cosA -
                              segment.EndPoint.Y * sinA,
                    centerY + segment.EndPoint.X * sinA +
                              segment.EndPoint.Y * cosA)

                Case PathSegmentType.Arc

                    Dim arc As ArcSegment2D =
                    DirectCast(segment, ArcSegment2D)

                    Dim arcCenterX As Double =
                    centerX + arc.Center.X * cosA -
                              arc.Center.Y * sinA

                    Dim arcCenterY As Double =
                    centerY + arc.Center.X * sinA +
                              arc.Center.Y * cosA

                    IncludeArc(
                    arcCenterX,
                    arcCenterY,
                    arc.Radius,
                    arc.StartAngle * 180.0 / Math.PI + angle,
                    arc.EndAngle * 180.0 / Math.PI + angle,
                    arc.IsClockwise)

            End Select

        Next

    End Sub




    Public Function GetBounds() As RectangleF

        If Not HasBounds Then
            Return RectangleF.Empty
        End If

        Return RectangleF.FromLTRB(
            CSng(_minX),
            CSng(_minY),
            CSng(_maxX),
            CSng(_maxY))

    End Function

End Class