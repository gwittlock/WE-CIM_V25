' =============================
' CadEntity class
' =============================
Imports FabV25_WIN8.DxfImporter_IxMilia
Imports IxMilia.Dxf.Entities

Public Class CadEntity

    ' =============================
    ' Point2D structure
    ' =============================
    ' --- Structures for points ---
    Public Structure Point2D
        Public Property X As Double
        Public Property Y As Double

        Public Sub New(x As Double, y As Double)
            Me.X = x
            Me.Y = y
        End Sub
    End Structure

    ' =============================
    ' Enum for type of entity
    ' =============================
    Public Enum EntityType
        Line
        Circle
        Arc
        Polyline
    End Enum

    ' =============================
    ' Properties
    ' =============================
    Public ReadOnly Type As EntityType
    Public LayerName As String

    ' Bounding box
    Public ReadOnly MinX As Double
    Public ReadOnly MinY As Double
    Public ReadOnly MaxX As Double
    Public ReadOnly MaxY As Double

    ' Geometry

    ' --- Properties common to all types ---

    Public ReadOnly StartPt As Point2D
    Public ReadOnly EndPt As Point2D
    Public ReadOnly CircleCenter As Point2D?
    Public ReadOnly CircleRadius As Double?
    Public ReadOnly ArcIsClockwise As Boolean?
    Public ReadOnly ArcRadius As Double?
    Public ReadOnly ArcStartAngle As Double?
    Public ReadOnly ArcEndAngle As Double?
    Public ReadOnly Length As Double
    Public Property IsClockwise As Boolean?
    Public Property DisplayColor As Color = Color.Black
    Public Property StartAngle As Double
    Public Property EndAngle As Double

    ' Polyline
    Public Property Points As List(Of Point2D)


    ' =============================
    ' Constructor
    ' =============================
    Private Sub New(type As EntityType,
                    layer As String,
                    startPt As Point2D,
                    endPt As Point2D,
                    minX As Double,
                    minY As Double,
                    maxX As Double,
                    maxY As Double,
                    length As Double,
                    Optional circleCenter As Point2D? = Nothing,
                    Optional circleRadius As Double? = Nothing,
                    Optional arcIsClockwise As Boolean? = Nothing,
                    Optional arcRadius As Double? = Nothing,
                    Optional arcStartAngle As Double? = Nothing,
                    Optional arcEndAngle As Double? = Nothing)
        Me.Type = type
        Me.LayerName = layer
        Me.StartPt = startPt
        Me.EndPt = endPt
        Me.MinX = minX
        Me.MinY = minY
        Me.MaxX = maxX
        Me.MaxY = maxY
        Me.Length = length
        Me.CircleCenter = circleCenter
        Me.CircleRadius = circleRadius
        Me.ArcIsClockwise = arcIsClockwise
        Me.ArcRadius = arcRadius
        Me.ArcStartAngle = arcStartAngle
        Me.ArcEndAngle = arcEndAngle
    End Sub

    ' =============================
    ' CreateFromLine
    ' =============================
    Public Shared Function CreateFromLine(startPt As Point2D,
                                          endPt As Point2D,
                                          layer As String) As CadEntity
        Dim minX = Math.Min(startPt.X, endPt.X)
        Dim minY = Math.Min(startPt.Y, endPt.Y)
        Dim maxX = Math.Max(startPt.X, endPt.X)
        Dim maxY = Math.Max(startPt.Y, endPt.Y)
        Dim length = Math.Sqrt((endPt.X - startPt.X) ^ 2 + (endPt.Y - startPt.Y) ^ 2)

        Return New CadEntity(EntityType.Line, layer, startPt, endPt, minX, minY, maxX, maxY, length)
    End Function

    ' =============================
    ' CreateFromCircle
    ' =============================
    Public Shared Function CreateFromCircle(center As Point2D,
                                            radius As Double,
                                            layer As String) As CadEntity
        Dim minX = center.X - radius
        Dim minY = center.Y - radius
        Dim maxX = center.X + radius
        Dim maxY = center.Y + radius
        Dim length = 2 * Math.PI * radius

        Return New CadEntity(EntityType.Circle,
                             layer,
                             New Point2D(center.X - radius, center.Y),
                             New Point2D(center.X + radius, center.Y),
                             minX, minY, maxX, maxY, length,
                             circleCenter:=center,
                             circleRadius:=radius)
    End Function

    ' =============================
    ' CreateFromArc
    ' =============================
    Public Shared Function CreateFromArc(center As Point2D,
                                         radius As Double,
                                         startAngle As Double,
                                         endAngle As Double,
                                         layer As String,
                                         Optional clockwise As Boolean = False) As CadEntity
        Dim startPt As New Point2D(center.X + radius * Math.Cos(startAngle * Math.PI / 180.0),
                                   center.Y + radius * Math.Sin(startAngle * Math.PI / 180.0))
        Dim endPt As New Point2D(center.X + radius * Math.Cos(endAngle * Math.PI / 180.0),
                                 center.Y + radius * Math.Sin(endAngle * Math.PI / 180.0))

        Dim minX = center.X - radius
        Dim minY = center.Y - radius
        Dim maxX = center.X + radius
        Dim maxY = center.Y + radius
        Dim sweep = endAngle - startAngle
        If sweep < 0 Then sweep += 360
        Dim length = radius * sweep * Math.PI / 180.0

        Return New CadEntity(EntityType.Arc,
                             layer,
                             startPt,
                             endPt,
                             minX, minY, maxX, maxY,
                             length,
                             circleCenter:=center,
                             circleRadius:=radius,
                             arcIsClockwise:=clockwise,
                             arcRadius:=radius,
                             arcStartAngle:=startAngle,
                             arcEndAngle:=endAngle)
    End Function

    ' =============================
    ' CreateFromPolyline
    ' =============================
    Public Shared Function CreateFromPolyline(vertices As IReadOnlyList(Of DxfVertex), layer As String, isClosed As Boolean) As List(Of CadEntity)
        Dim results As New List(Of CadEntity)
        Dim segmentCount = vertices.Count

        For i As Integer = 0 To segmentCount - 1
            Dim v1 = vertices(i)
            Dim v2 = If(i = segmentCount - 1, vertices(0), vertices(i + 1))

            If Math.Abs(v1.Bulge) < 0.0000001 Then
                results.Add(CreateLineSegment(v1, v2, layer))
            Else
                results.Add(CreateArcSegment(v1, v2, layer))
            End If
        Next

        Return results
    End Function

    ' =============================
    ' Helper: CreateLineSegment
    ' =============================
    Private Shared Function CreateLineSegment(v1 As DxfVertex, v2 As DxfVertex, layer As String) As CadEntity
        Dim p1 As New Point2D(v1.Location.X, v1.Location.Y)
        Dim p2 As New Point2D(v2.Location.X, v2.Location.Y)
        Return CreateFromLine(p1, p2, layer)
    End Function

    ' =============================
    ' Helper: CreateArcSegment (bulge)
    ' =============================
    Private Shared Function CreateArcSegment(v1 As DxfVertex, v2 As DxfVertex, layer As String) As CadEntity
        Dim p1 As New Point2D(v1.Location.X, v1.Location.Y)
        Dim p2 As New Point2D(v2.Location.X, v2.Location.Y)
        Dim bulge = v1.Bulge

        ' Chord
        Dim dx = p2.X - p1.X
        Dim dy = p2.Y - p1.Y
        Dim chordLength = Math.Sqrt(dx * dx + dy * dy)

        ' Included angle
        Dim includedAngle = 4 * Math.Atan(bulge)

        ' Radius
        Dim radius = chordLength / (2 * Math.Sin(Math.Abs(includedAngle) / 2))

        ' Midpoint
        Dim mx = (p1.X + p2.X) / 2
        Dim my = (p1.Y + p2.Y) / 2

        ' Normal direction
        Dim perpX = -dy / chordLength
        Dim perpY = dx / chordLength

        ' Distance from midpoint to center
        Dim h = Math.Sqrt(radius * radius - (chordLength / 2) ^ 2)
        If bulge < 0 Then h = -h

        ' Center
        Dim cx = mx + perpX * h
        Dim cy = my + perpY * h
        Dim center As Point2D? = New Point2D(cx, cy)

        ' Angles
        Dim startAngle = Math.Atan2(p1.Y - cy, p1.X - cx) * 180 / Math.PI
        Dim endAngle = Math.Atan2(p2.Y - cy, p2.X - cx) * 180 / Math.PI

        ' Direction
        Dim clockwise As Boolean = bulge < 0

        ' Bounds
        Dim minX = Math.Min(p1.X, p2.X)
        Dim minY = Math.Min(p1.Y, p2.Y)
        Dim maxX = Math.Max(p1.X, p2.X)
        Dim maxY = Math.Max(p1.Y, p2.Y)

        ' Arc length
        Dim arcLength = Math.Abs(includedAngle * radius)

        Return New CadEntity(EntityType.Arc,
                             layer,
                             p1,
                             p2,
                             minX, minY, maxX, maxY,
                             arcLength,
                             circleCenter:=center,
                             arcRadius:=radius,
                             arcIsClockwise:=clockwise,
                             arcStartAngle:=startAngle,
                             arcEndAngle:=endAngle)
    End Function

    Friend Shared Function CadEntitiesFromModelEntityIxMilia(m As ModelEntity) As List(Of CadEntity)
        Dim results As New List(Of CadEntity)

        If TypeOf m Is ModelLine Then
            Dim l = DirectCast(m, ModelLine)
            results.Add(CadEntity.CreateFromLine(
            startPt:=New Point2D(l.StartPoint.X, l.StartPoint.Y),
            endPt:=New Point2D(l.EndPoint.X, l.EndPoint.Y),
            layer:=l.Layer
        ))

        ElseIf TypeOf m Is ModelArc Then
            Dim arc = DirectCast(m, ModelArc)

            Dim center As New Point2D(arc.Center.X, arc.Center.Y)
            Dim radius As Double = arc.Radius
            Dim startAngle As Double = arc.StartAngle
            Dim endAngle As Double = arc.EndAngle

            'Optional infer winding if your CadEntity requires it
            Dim isClockwise As Boolean = (endAngle < startAngle)

            results.Add(
            CadEntity.CreateFromArc(
                center,
                radius,
                startAngle,
                endAngle,
                arc.Layer,
                isClockwise
            )
        )


        ElseIf TypeOf m Is ModelCircle Then
            Dim c = DirectCast(m, ModelCircle)

            results.Add(
        CadEntity.CreateFromCircle(
            New Point2D(c.Center.X, c.Center.Y),
            c.Radius,
            c.Layer
        )
    )

        ElseIf TypeOf m Is ModelPolyline Then
            Dim p = DirectCast(m, ModelPolyline)
            results.AddRange(CadEntity.CreateFromPolyline(
            vertices:=p.Points.Select(Function(v) New Point2D(v.X, v.Y)).ToList(),
            layer:=p.Layer,
            isClosed:=p.IsClosed
        ))
        End If

        Return results
    End Function

    Public Shared Function FromDxf(dxf As DxfEntity) As CadEntity
        Select Case dxf.EntityType
            Case DxfEntityType.Line
                Dim line = DirectCast(dxf, DxfLine)
                Return CreateFromLine(New Point2D(line.P1.X, line.P1.Y),
                                  New Point2D(line.P2.X, line.P2.Y),
                                  line.Layer)
            Case DxfEntityType.Arc
                Dim arc = DirectCast(dxf, DxfArc)
                Return CreateFromArc(New Point2D(arc.Center.X, arc.Center.Y),
                                 arc.Radius, arc.StartAngle, arc.EndAngle,
                                 arc.Layer, (arc.EndAngle - arc.StartAngle) < 0)
            Case DxfEntityType.Circle
                Dim circle = DirectCast(dxf, DxfCircle)
                Return CreateFromCircle(New Point2D(circle.Center.X, circle.Center.Y),
                                    circle.Radius, circle.Layer)
            Case Else
                Return Nothing
        End Select
    End Function

    Public Shared Sub GetBoundingBox(entities As List(Of CadEntity),
                                 ByRef minX As Double,
                                 ByRef minY As Double,
                                 ByRef maxX As Double,
                                 ByRef maxY As Double)
        If entities Is Nothing OrElse entities.Count = 0 Then
            minX = 0 : minY = 0 : maxX = 0 : maxY = 0
            Return
        End If

        minX = entities.Min(Function(c) c.MinX)
        minY = entities.Min(Function(c) c.MinY)
        maxX = entities.Max(Function(c) c.MaxX)
        maxY = entities.Max(Function(c) c.MaxY)
    End Sub







    Friend Shared Function CalculateArcClockwise(a As ModelArc) As Boolean
        Dim startAngle As Double = NormalizeAngle(a.StartAngle)
        Dim endAngle As Double = NormalizeAngle(a.EndAngle)

        ' Compute sweep assuming CCW
        Dim sweep As Double = endAngle - startAngle
        If sweep < 0 Then sweep += 360.0

        ' If CCW sweep is greater than 180, DXF typically implies CW
        Return sweep > 180.0
    End Function

    Private Shared Function NormalizeAngle(deg As Double) As Double
        deg = deg Mod 360
        If deg < 0 Then deg += 360
        Return deg
    End Function
End Class
