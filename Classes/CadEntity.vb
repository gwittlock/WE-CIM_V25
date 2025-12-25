Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities

Public Class CadEntity

    Public Enum EntityType
        Unknown = -1
        Line = 0
        Arc = 1
        Circle = 2
    End Enum

    ' --- Identity ---
    Public ReadOnly Property CadEntityType As EntityType
    Public ReadOnly Property LayerName As String

    ' --- DXF backing reference (exactly one) ---
    Public ReadOnly Property DxfEntity As DxfEntity

    Public Property DisplayColor As Color
    Public Property WindingDirection As String ' "CW" or "CCW"

    Public ReadOnly Property StartPoint As (X As Double, Y As Double)
    Public ReadOnly Property EndPoint As (X As Double, Y As Double)

    Public ReadOnly Property MinX As Double
    Public ReadOnly Property MinY As Double
    Public ReadOnly Property MaxX As Double
    Public ReadOnly Property MaxY As Double
    Public ReadOnly Property Length As Double
    Public Property _circleCenter As DxfPoint
    Public Property _circleRadius As Double
    Public Property _arcIsClockWise As Boolean
    Public Property _toolname As String
    Public Property _workZone As String
    Public Property _arcCenter As DxfPoint
    Public Property _arcRadius As Double

    Public ReadOnly Property ToolName As String
        Get
            Return _toolName
        End Get
    End Property

    Public ReadOnly Property WorkZone As String
        Get
            Return _workZone
        End Get
    End Property

    Private Sub New(
    entityType As EntityType,
    dxf As DxfEntity,
    layerName As String,
    startPt As (Double, Double),
    endPt As (Double, Double),
    minX As Double,
    minY As Double,
    maxX As Double,
    maxY As Double,
    length As Double
)
        Me.CadEntityType = entityType
        Me.DxfEntity = dxf
        Me.LayerName = layerName
        Me.StartPoint = startPt
        Me.EndPoint = endPt
        Me.MinX = minX
        Me.MinY = minY
        Me.MaxX = maxX
        Me.MaxY = maxY
        Me.Length = length
    End Sub
    '------------------------------------------------------------
    ' Compute distance from a point to a line segment in 2D
    '------------------------------------------------------------
    Public Shared Function DistancePointToLine(pt As PointF, p1 As DxfPoint, p2 As DxfPoint) As Double
        Dim x0 = pt.X
        Dim y0 = pt.Y
        Dim x1 = CSng(p1.X)
        Dim y1 = CSng(p1.Y)
        Dim x2 = CSng(p2.X)
        Dim y2 = CSng(p2.Y)

        Dim dx = x2 - x1
        Dim dy = y2 - y1

        If dx = 0 AndAlso dy = 0 Then
            ' Line segment is a point
            Return Math.Sqrt((x0 - x1) ^ 2 + (y0 - y1) ^ 2)
        End If

        ' Project point onto the line, clamped to segment
        Dim t = ((x0 - x1) * dx + (y0 - y1) * dy) / (dx * dx + dy * dy)
        t = Math.Max(0, Math.Min(1, t))

        Dim closestX = x1 + t * dx
        Dim closestY = y1 + t * dy

        Return Math.Sqrt((x0 - closestX) ^ 2 + (y0 - closestY) ^ 2)
    End Function

    '------------------------------------------------------------
    ' Distance from a point to an ImportedEntity
    '------------------------------------------------------------
    Public Shared Function DistanceToImportedEntity(ent As ImportedEntity, pt As PointF) As Double
        Select Case ent.GeometryType
            Case ImportedEntity.GeometryTypeEnum.Line
                Return DistancePointToLine(pt, ent.Line.P1, ent.Line.P2)
            Case ImportedEntity.GeometryTypeEnum.Arc
                ' Distance to nearest point on arc
                Dim dx = pt.X - ent.Arc.Center.X
                Dim dy = pt.Y - ent.Arc.Center.Y
                Dim d = Math.Sqrt(dx * dx + dy * dy)
                Return Math.Abs(d - ent.Arc.Radius)
            Case ImportedEntity.GeometryTypeEnum.Circle
                Dim dx = pt.X - ent.Circle.Center.X
                Dim dy = pt.Y - ent.Circle.Center.Y
                Dim d = Math.Sqrt(dx * dx + dy * dy)
                Return Math.Abs(d - ent.Circle.Radius)
            Case Else
                Return Double.MaxValue
        End Select
    End Function

    '------------------------------------------------------------
    ' Hit test for ImportedEntity list
    '------------------------------------------------------------
    Public Shared Function HitTestImportedEntity(
    entities As IEnumerable(Of ImportedEntity),
    worldPoint As PointF,
    tolerance As Double
) As ImportedEntity

        Dim closest As ImportedEntity = Nothing
        Dim bestDist As Double = Double.MaxValue

        For Each ent In entities
            If ent IsNot Nothing Then
                Dim dist As Double = DistanceToImportedEntity(ent, worldPoint)
                If dist <= tolerance AndAlso dist < bestDist Then
                    bestDist = dist
                    closest = ent
                End If
            End If
        Next

        Return closest
    End Function


    Private Shared Function DistanceToEntity(cad As CadEntity, pt As PointF) As Double
        Select Case cad.CadEntityType
            Case CadEntity.EntityType.Line
                Return DistancePointToSegment(pt, cad.StartPoint, cad.EndPoint)

            Case CadEntity.EntityType.Arc
                Return DistancePointToArc(pt, DirectCast(cad.DxfEntity, DxfArc))

            Case CadEntity.EntityType.Circle
                Return DistancePointToCircle(pt, DirectCast(cad.DxfEntity, DxfCircle))

            Case Else
                Return Double.MaxValue
        End Select
    End Function


    Private Shared Function DistancePointToSegment(
    p As PointF,
    a As (X As Double, Y As Double),
    b As (X As Double, Y As Double)
) As Double

        Dim vx = b.X - a.X
        Dim vy = b.Y - a.Y
        Dim wx = p.X - a.X
        Dim wy = p.Y - a.Y

        Dim c1 = vx * wx + vy * wy
        If c1 <= 0 Then
            Return Math.Sqrt(wx * wx + wy * wy)
        End If

        Dim c2 = vx * vx + vy * vy
        If c2 <= c1 Then
            Dim dx = p.X - b.X
            Dim dy = p.Y - b.Y
            Return Math.Sqrt(dx * dx + dy * dy)
        End If

        Dim t = c1 / c2
        Dim px = a.X + t * vx
        Dim py = a.Y + t * vy

        Dim dx2 = p.X - px
        Dim dy2 = p.Y - py
        Return Math.Sqrt(dx2 * dx2 + dy2 * dy2)
    End Function


    Private Shared Function DistancePointToCircle(
    p As PointF,
    circle As DxfCircle
) As Double

        Dim dx = p.X - circle.Center.X
        Dim dy = p.Y - circle.Center.Y
        Dim distToCenter = Math.Sqrt(dx * dx + dy * dy)

        Return Math.Abs(distToCenter - circle.Radius)
    End Function


    Private Shared Function DistancePointToArc(
    p As PointF,
    arc As DxfArc
) As Double

        Dim dx = p.X - arc.Center.X
        Dim dy = p.Y - arc.Center.Y
        Dim angle = Math.Atan2(dy, dx) * 180.0 / Math.PI
        If angle < 0 Then angle += 360

        Dim startAng = arc.StartAngle
        Dim endAng = arc.EndAngle

        Dim within As Boolean
        If startAng <= endAng Then
            within = angle >= startAng AndAlso angle <= endAng
        Else
            within = angle >= startAng OrElse angle <= endAng
        End If

        If Not within Then Return Double.MaxValue

        Dim distToCenter = Math.Sqrt(dx * dx + dy * dy)
        Return Math.Abs(distToCenter - arc.Radius)
    End Function

    Public Function GetInspectionInfo() As Dictionary(Of String, String)
        Dim info As New Dictionary(Of String, String)

        info("Entity Type") = CadEntityType.ToString()
        info("Layer") = LayerName

        If DisplayColor <> Color.Empty Then
            info("Display Color") = DisplayColor.Name
        End If

        info("Start Point") = $"({StartPoint.X:F4}, {StartPoint.Y:F4})"
        info("End Point") = $"({EndPoint.X:F4}, {EndPoint.Y:F4})"
        info("Length") = Length.ToString("F4")

        Select Case CadEntityType
            Case EntityType.Line
                Dim dx = EndPoint.X - StartPoint.X
                Dim dy = EndPoint.Y - StartPoint.Y
                Dim angle = Math.Atan2(dy, dx) * 180.0 / Math.PI
                info("Angle (deg)") = angle.ToString("F2")

            Case EntityType.Arc, EntityType.Circle
                Dim arc = TryCast(DxfEntity, DxfArc)
                Dim circ = TryCast(DxfEntity, DxfCircle)

                If arc IsNot Nothing Then
                    info("Center") = $"({arc.Center.X:F4}, {arc.Center.Y:F4})"
                    info("Radius") = arc.Radius.ToString("F4")
                    info("Direction") = If(arc.EndAngle >= arc.StartAngle, "CCW", "CW")
                ElseIf circ IsNot Nothing Then
                    info("Center") = $"({circ.Center.X:F4}, {circ.Center.Y:F4})"
                    info("Radius") = circ.Radius.ToString("F4")
                    info("Direction") = "CCW"
                End If
        End Select

        Return info
    End Function

    Public Class CadInspectionContext
        Public Property Cad As CadEntity
        Public Property ToolName As String
        Public Property WorkZone As String
    End Class

    ' -----------------------------
    ' Factory (ONLY entry point)
    Public Shared Function FromDxf(ent As DxfEntity) As CadEntity
        If ent Is Nothing Then
            Throw New ArgumentNullException(NameOf(ent))
        End If

        If TypeOf ent Is DxfLine Then
            Return CreateFromLine(DirectCast(ent, DxfLine))

        ElseIf TypeOf ent Is DxfArc Then
            Return CreateFromArc(DirectCast(ent, DxfArc))

        ElseIf TypeOf ent Is DxfCircle Then
            Return CreateFromCircle(DirectCast(ent, DxfCircle))
        End If

        Throw New InvalidOperationException(
        $"Unsupported DXF entity type: {ent.GetType().Name}"
    )
    End Function

    Public ReadOnly Property CenterPoint As DxfPoint
        Get
            Select Case CadEntityType
                Case EntityType.Arc
                    Return _arcCenter
                Case EntityType.Circle
                    Return _circleCenter
                Case Else
                    Return Nothing
            End Select
        End Get
    End Property

    Public ReadOnly Property Radius As Double
        Get
            Select Case CadEntityType
                Case EntityType.Arc
                    Return _arcRadius
                Case EntityType.Circle
                    Return _circleRadius
                Case Else
                    Return 0
            End Select
        End Get
    End Property

    Public ReadOnly Property IsClockwise As Boolean
        Get
            If CadEntityType <> EntityType.Arc Then Return False
            Return _arcIsClockwise
        End Get
    End Property

    Friend Sub SetTooling(toolName As String, workZone As String)
        _toolName = toolName
        _workZone = workZone
    End Sub

    ' -----------------------------
    ' Line
    ' -----------------------------
    Private Shared Function CreateFromLine(line As DxfLine) As CadEntity
        Dim x1 = line.P1.X
        Dim y1 = line.P1.Y
        Dim x2 = line.P2.X
        Dim y2 = line.P2.Y

        Return New CadEntity(
        EntityType.Line,
        line,
        line.Layer,
        (x1, y1),
        (x2, y2),
        Math.Min(x1, x2),
        Math.Min(y1, y2),
        Math.Max(x1, x2),
        Math.Max(y1, y2),
        Math.Sqrt((x2 - x1) ^ 2 + (y2 - y1) ^ 2)
    )
    End Function


    ' -----------------------------
    ' Arc
    ' -----------------------------
    Private Shared Function CreateFromArc(arc As DxfArc) As CadEntity
        Dim c = arc.Center
        Dim r = arc.Radius

        Dim startRad = arc.StartAngle * Math.PI / 180.0
        Dim endRad = arc.EndAngle * Math.PI / 180.0

        Dim sx = c.X + r * Math.Cos(startRad)
        Dim sy = c.Y + r * Math.Sin(startRad)
        Dim ex = c.X + r * Math.Cos(endRad)
        Dim ey = c.Y + r * Math.Sin(endRad)

        Dim sweep = Math.Abs(arc.EndAngle - arc.StartAngle) * Math.PI / 180.0
        Dim len = r * sweep

        Return New CadEntity(
        EntityType.Arc,
        arc,
        arc.Layer,
        (sx, sy),
        (ex, ey),
        c.X - r,
        c.Y - r,
        c.X + r,
        c.Y + r,
        len
    )
    End Function


    ' -----------------------------
    ' Circle
    ' -----------------------------
    Private Shared Function CreateFromCircle(circle As DxfCircle) As CadEntity
        Dim c = circle.Center
        Dim r = circle.Radius

        Return New CadEntity(
            EntityType.Circle,
            circle,
            circle.Layer,
            (c.X + r, c.Y),    ' StartPoint
            (c.X + r, c.Y),    ' EndPoint (closed)
            c.X - r,           ' MinX
            c.Y - r,           ' MinY
            c.X + r,           ' MaxX
            c.Y + r,           ' MaxY
            2 * Math.PI * r    ' Length
        )
    End Function




    ' Compute bounding box of a set of lines
    ''' <summary>
    ''' Computes the bounding box for a list of CadEntity objects.
    ''' </summary>
    ''' <param name="entities">List of CadEntity to compute bounds for</param>
    ''' <param name="minX">Output: minimum X coordinate</param>
    ''' <param name="minY">Output: minimum Y coordinate</param>
    ''' <param name="maxX">Output: maximum X coordinate</param>
    ''' <param name="maxY">Output: maximum Y coordinate</param>
    Public Shared Sub GetBoundingBox(entities As List(Of CadEntity), ByRef minX As Double, ByRef minY As Double, ByRef maxX As Double, ByRef maxY As Double)
        If entities Is Nothing OrElse entities.Count = 0 Then
            minX = 0
            minY = 0
            maxX = 0
            maxY = 0
            Return
        End If

        minX = Double.MaxValue
        minY = Double.MaxValue
        maxX = Double.MinValue
        maxY = Double.MinValue

        For Each ent In entities
            minX = Math.Min(minX, ent.MinX)
            minY = Math.Min(minY, ent.MinY)
            maxX = Math.Max(maxX, ent.MaxX)
            maxY = Math.Max(maxY, ent.MaxY)
        Next
    End Sub

    ''' <summary>
    ''' Returns the bounding box of a single CadEntity as a tuple.
    ''' </summary>
    Public Shared Function GetBoundingBox(entity As CadEntity) As (MinX As Double, MinY As Double, MaxX As Double, MaxY As Double)
        If entity Is Nothing Then Throw New ArgumentNullException(NameOf(entity))
        Return (entity.MinX, entity.MinY, entity.MaxX, entity.MaxY)
    End Function



End Class
