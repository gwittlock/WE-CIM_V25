Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.Drawing
Imports System.Runtime.CompilerServices

Public Class CadEntity

    '==========================
    ' Entity Type Enum
    '==========================
    Public Enum EntityType
        Unknown = -1
        Line = 0
        Arc = 1
        Circle = 2
        Polyline = 3
    End Enum

    '==========================
    ' Identity & DXF
    '==========================
    Public ReadOnly Property CadEntityType As EntityType
    Public ReadOnly Property LayerName As String
    Public ReadOnly Property DxfEntity As DxfEntity

    ' Circle / Arc properties
    Public Center As PointF

    ' Arc-specific angles (degrees)
    Public StartAngle As Single
    Public EndAngle As Single

    '==========================
    ' Display & tooling
    '==========================
    Public Shared Property DisplayColor As Color
    Public Property WindingDirection As String ' "CW" or "CCW"

    Private _toolName As String
    Private _workZone As String
    Public Shared _cadEntities As List(Of CadEntity)



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

    Friend Sub SetTooling(toolName As String, workZone As String)
        _toolName = toolName
        _workZone = workZone
    End Sub
    Private Shared Function PointFromAngle(
    center As DxfPoint,
    radius As Double,
    angleDegrees As Double
) As (Double, Double)

        Dim radians = angleDegrees * Math.PI / 180.0
        Dim x = center.X + radius * Math.Cos(radians)
        Dim y = center.Y + radius * Math.Sin(radians)

        Return (x, y)
    End Function

    '==========================
    ' Geometry Info
    '==========================
    Public ReadOnly Property StartPoint As (X As Double, Y As Double)
    Public ReadOnly Property EndPoint As (X As Double, Y As Double)

    Public ReadOnly Property MinX As Double
    Public ReadOnly Property MinY As Double
    Public ReadOnly Property MaxX As Double
    Public ReadOnly Property MaxY As Double
    Public ReadOnly Property Length As Double

    Private _circleCenter As DxfPoint
    Private _circleRadius As Double
    Private _arcCenter As DxfPoint
    Private _arcRadius As Double
    Private _arcIsClockwise As Boolean

    '==========================
    ' CAM Operation support
    '==========================
    Private _operation As CamOperation
    Public Property Operation As CamOperation
        Get
            Return _operation
        End Get
        Set(value As CamOperation)
            _operation = value
        End Set
    End Property

    Friend Sub SetOperation(op As CamOperation)
        _operation = op
    End Sub

    '==========================
    ' Constructors
    '==========================
    '==========================
    ' Private Constructor (enforce factory usage)
    '==========================
    Private Sub New(entityType As EntityType,
                    dxf As DxfEntity,
                    layerName As String,
                    startPt As (Double, Double),
                    endPt As (Double, Double),
                    minX As Double,
                    minY As Double,
                    maxX As Double,
                    maxY As Double,
                    length As Double,
                    Optional toolName As String = "",
                    Optional workZone As String = "",
                    Optional circleCenter As DxfPoint = Nothing,
                    Optional circleRadius As Double = 0,
                    Optional arcCenter As DxfPoint = Nothing,
                    Optional arcRadius As Double = 0,
                    Optional arcIsClockwise As Boolean = False)

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

        Me._toolName = toolName
        Me._workZone = workZone
        Me._circleCenter = circleCenter
        Me._circleRadius = circleRadius
        Me._arcCenter = arcCenter
        Me._arcRadius = arcRadius
        Me._arcIsClockwise = arcIsClockwise
    End Sub
    '==========================
    ' Arc / Circle Readonly properties
    '==========================
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

    ' ------------------------------
    ' From DXF entity
    ' ------------------------------
    ' ------------------------------
    ' From DXF entity to CadEntity
    ' ------------------------------
    Public Shared Function FromDxf(
    ent As DxfEntity,
    Optional toolName As String = "",
    Optional workZone As String = ""
) As CadEntity

        If ent Is Nothing Then Throw New ArgumentNullException(NameOf(ent))

        Dim cad As CadEntity
        Dim layerName As String = ent.Layer ' THIS IS CRITICAL

        Select Case True
            Case TypeOf ent Is DxfLine
                cad = CreateFromLine(DirectCast(ent, DxfLine), layerName)

            Case TypeOf ent Is DxfArc
                cad = CreateFromArc(DirectCast(ent, DxfArc), layerName)

            Case TypeOf ent Is DxfCircle
                cad = CreateFromCircle(DirectCast(ent, DxfCircle), layerName)

            Case Else
                Return Nothing ' ignore unsupported types safely
        End Select

        If Not String.IsNullOrEmpty(toolName) OrElse Not String.IsNullOrEmpty(workZone) Then
            cad.SetTooling(toolName, workZone)
        End If

        Return cad
    End Function


    ' ------------------------------
    ' Create CadEntity from DXF Line
    ' ------------------------------
    Public Shared Function CreateFromLine(line As DxfLine, layerName As String) As CadEntity
        Dim x1 = line.P1.X
        Dim y1 = line.P1.Y
        Dim x2 = line.P2.X
        Dim y2 = line.P2.Y

        Return New CadEntity(
        EntityType.Line,
        line,
        layerName,
        (x1, y1),
        (x2, y2),
        Math.Min(x1, x2),
        Math.Min(y1, y2),
        Math.Max(x1, x2),
        Math.Max(y1, y2),
        Math.Sqrt((x2 - x1) ^ 2 + (y2 - y1) ^ 2)
    )
    End Function

    ' ------------------------------
    ' Create CadEntity from DXF Arc
    ' ------------------------------
    Public Shared Function CreateFromArc(arc As DxfArc, layerName As String) As CadEntity

        Dim center = arc.Center
        Dim r = arc.Radius

        ' Compute start / end points
        Dim startPt = PointFromAngle(center, r, arc.StartAngle)
        Dim endPt = PointFromAngle(center, r, arc.EndAngle)

        ' Bounding box (safe overestimate)
        Dim minX = center.X - r
        Dim minY = center.Y - r
        Dim maxX = center.X + r
        Dim maxY = center.Y + r

        ' Arc length
        Dim sweepDeg = arc.EndAngle - arc.StartAngle
        If sweepDeg < 0 Then sweepDeg += 360
        Dim length = r * (sweepDeg * Math.PI / 180.0)

        Return New CadEntity(
        EntityType.Arc,
        arc,
        layerName,
        startPt,
        endPt,
        minX,
        minY,
        maxX,
        maxY,
        length,
        arcCenter:=center,
        arcRadius:=r,
        arcIsClockwise:=False
    )
    End Function


    ' ------------------------------
    ' Create CadEntity from DXF Circle
    ' ------------------------------
    Public Shared Function CreateFromCircle(circle As DxfCircle, layerName As String) As CadEntity
        Dim cx = circle.Center.X
        Dim cy = circle.Center.Y
        Dim r = circle.Radius

        Dim startPt = (cx - r, cy)
        Dim endPt = (cx + r, cy)

        Dim minX = cx - r
        Dim minY = cy - r
        Dim maxX = cx + r
        Dim maxY = cy + r

        Dim length = 2 * Math.PI * r

        Return New CadEntity(
        EntityType.Circle,
        circle,
        layerName,
        startPt,
        endPt,
        minX,
        minY,
        maxX,
        maxY,
        length,
        circleCenter:=circle.Center,
        circleRadius:=r
    )
    End Function

    '==========================
    ' Hit Testing & Distance
    '==========================
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
        If c1 <= 0 Then Return Math.Sqrt(wx * wx + wy * wy)

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

    Private Shared Function DistanceToEntity(cad As CadEntity, pt As PointF) As Double
        Select Case cad.CadEntityType
            Case EntityType.Line
                Return DistancePointToSegment(pt, cad.StartPoint, cad.EndPoint)
            Case EntityType.Arc
                Return DistancePointToArc(pt, DirectCast(cad.DxfEntity, DxfArc))
            Case EntityType.Circle
                Return DistancePointToCircle(pt, DirectCast(cad.DxfEntity, DxfCircle))
            Case Else
                Return Double.MaxValue
        End Select
    End Function

    Public Shared Function HitTestImportedEntity(
        entities As IEnumerable(Of ImportedEntity),
        worldPoint As PointF,
        tolerance As Double
    ) As ImportedEntity
        Dim closest As ImportedEntity = Nothing
        Dim bestDist As Double = Double.MaxValue
        For Each ent In entities
            If ent IsNot Nothing Then
                Dim dist = CadEntity.DistanceToImportedEntity(ent, worldPoint)
                If dist <= tolerance AndAlso dist < bestDist Then
                    bestDist = dist
                    closest = ent
                End If
            End If
        Next
        Return closest
    End Function

    Public Shared Function DistanceToImportedEntity(ent As ImportedEntity, pt As PointF) As Double
        Select Case ent.GeometryType
            Case ImportedEntity.GeometryTypeEnum.Line
                Return DistancePointToLine(pt, ent.Line.P1, ent.Line.P2)
            Case ImportedEntity.GeometryTypeEnum.Arc
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

    Private Shared Function DistancePointToLine(pt As PointF, p1 As DxfPoint, p2 As DxfPoint) As Double
        Dim x0 = pt.X
        Dim y0 = pt.Y
        Dim x1 = CSng(p1.X)
        Dim y1 = CSng(p1.Y)
        Dim x2 = CSng(p2.X)
        Dim y2 = CSng(p2.Y)
        Dim dx = x2 - x1
        Dim dy = y2 - y1
        If dx = 0 AndAlso dy = 0 Then Return Math.Sqrt((x0 - x1) ^ 2 + (y0 - y1) ^ 2)
        Dim t = ((x0 - x1) * dx + (y0 - y1) * dy) / (dx * dx + dy * dy)
        t = Math.Max(0, Math.Min(1, t))
        Dim px = x1 + t * dx
        Dim py = y1 + t * dy
        Dim dx2 = x0 - px
        Dim dy2 = y0 - py
        Return Math.Sqrt(dx2 * dx2 + dy2 * dy2)
    End Function

    '==========================
    ' Bounding Box helpers
    '==========================
    Public Shared Sub GetBoundingBox(entities As List(Of CadEntity), ByRef minX As Double, ByRef minY As Double, ByRef maxX As Double, ByRef maxY As Double)
        If entities Is Nothing OrElse entities.Count = 0 Then
            minX = 0 : minY = 0 : maxX = 0 : maxY = 0
            Return
        End If
        minX = Double.MaxValue : minY = Double.MaxValue
        maxX = Double.MinValue : maxY = Double.MinValue
        For Each ent In entities
            minX = Math.Min(minX, ent.MinX)
            minY = Math.Min(minY, ent.MinY)
            maxX = Math.Max(maxX, ent.MaxX)
            maxY = Math.Max(maxY, ent.MaxY)
        Next
    End Sub

    Public Shared Function GetBoundingBox(entity As CadEntity) As (MinX As Double, MinY As Double, MaxX As Double, MaxY As Double)
        If entity Is Nothing Then Throw New ArgumentNullException(NameOf(entity))
        Return (entity.MinX, entity.MinY, entity.MaxX, entity.MaxY)
    End Function

    '==========================
    ' Inspection info
    '==========================
    Public Function GetInspectionInfo() As Dictionary(Of String, String)
        Dim info As New Dictionary(Of String, String)

        info("Entity Type") = CadEntityType.ToString()
        info("Layer") = LayerName
        If DisplayColor <> Color.Empty Then info("Display Color") = DisplayColor.Name
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

        ' Include CAM Operation if assigned
        If Operation IsNot Nothing Then
            info("CAM Operation") = Operation.ToString()
        End If

        Return info
    End Function


    ' Helper to assign tooling

    '==========================
    ' Properties
    '==========================
    Public ReadOnly Property ToolNameProp As String
        Get
            Return _toolName
        End Get
    End Property

    Public ReadOnly Property WorkZoneProp As String
        Get
            Return _workZone
        End Get
    End Property

    Public ReadOnly Property ArcCenter As DxfPoint
        Get
            Return _arcCenter
        End Get
    End Property

    Public ReadOnly Property ArcRadius As Double
        Get
            Return _arcRadius
        End Get
    End Property

    Public ReadOnly Property CircleCenter As DxfPoint
        Get
            Return _circleCenter
        End Get
    End Property

    Public ReadOnly Property CircleRadius As Double
        Get
            Return _circleRadius
        End Get
    End Property

    Public ReadOnly Property ArcIsClockwise As Boolean
        Get
            Return _arcIsClockwise
        End Get
    End Property

    '==========================
    ' Optional inspection context
    '==========================
    Public Class CadInspectionContext
        Public Property Cad As CadEntity
        Public Property ToolName As String
        Public Property WorkZone As String
    End Class

End Class
