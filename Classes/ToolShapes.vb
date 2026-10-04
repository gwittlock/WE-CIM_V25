Imports System.Drawing

Namespace ToolShapes

#Region "Enums"

    Public Enum PathSegmentType
        Line
        Arc
    End Enum

    Public Enum ToolMotionType
        Rapid
        LinearCut
        ArcCut
    End Enum

    Public Enum FeatureType
        Profile
        Pocket
        Slot
        Drill
        Engrave
    End Enum

    Public Enum ToolShapeType
        Round
        Square
        Rectangle
        CornerRadius
        CustomProfile
    End Enum

#End Region

#Region "Path Geometry"

    Public Interface IPathSegment
        ReadOnly Property SegmentType As PathSegmentType
        ReadOnly Property StartPoint As PointF
        ReadOnly Property EndPoint As PointF
        ReadOnly Property Length As Double
    End Interface

    Public Class LineSegment2D
        Implements IPathSegment

        Public ReadOnly Property StartPoint As PointF Implements IPathSegment.StartPoint
        Public ReadOnly Property EndPoint As PointF Implements IPathSegment.EndPoint

        Public Sub New(startPt As PointF, endPt As PointF)
            StartPoint = startPt
            EndPoint = endPt
        End Sub

        Public ReadOnly Property SegmentType As PathSegmentType Implements IPathSegment.SegmentType
            Get
                Return PathSegmentType.Line
            End Get
        End Property

        Public ReadOnly Property Length As Double Implements IPathSegment.Length
            Get
                Return Math.Sqrt(
                    (EndPoint.X - StartPoint.X) ^ 2 +
                    (EndPoint.Y - StartPoint.Y) ^ 2
                )
            End Get
        End Property
    End Class
    Public Class ArcSegment2D
        Implements IPathSegment

        Public ReadOnly Property Center As PointF
        Public ReadOnly Property Radius As Double
        Public ReadOnly Property StartAngle As Double
        Public ReadOnly Property EndAngle As Double
        Public ReadOnly Property IsClockwise As Boolean

        Public ReadOnly Property StartPoint As PointF Implements IPathSegment.StartPoint
        Public ReadOnly Property EndPoint As PointF Implements IPathSegment.EndPoint

        Public Sub New(
        center As PointF,
        radius As Double,
        startAngle As Double,
        endAngle As Double,
        isClockwise As Boolean
    )

            Me.Center = center
            Me.Radius = radius
            Me.StartAngle = startAngle
            Me.EndAngle = endAngle
            Me.IsClockwise = isClockwise

            StartPoint = Geometry2D.PointOnCircle(
            center,
            radius,
            startAngle)

            EndPoint = Geometry2D.PointOnCircle(
            center,
            radius,
            endAngle)

        End Sub

        Public ReadOnly Property SegmentType As PathSegmentType _
        Implements IPathSegment.SegmentType

            Get
                Return PathSegmentType.Arc
            End Get

        End Property

        Public ReadOnly Property Length As Double _
        Implements IPathSegment.Length

            Get
                Return Math.Abs(EndAngle - StartAngle) * Radius
            End Get

        End Property

    End Class

    Public Interface IPath2D
        ReadOnly Property Segments As IEnumerable(Of IPathSegment)
        ReadOnly Property IsClosed As Boolean
        ReadOnly Property BoundingBox As RectangleF
    End Interface

    Public Class Path2D
        Implements IPath2D

        Private ReadOnly _segments As New List(Of IPathSegment)

        Public Sub Add(segment As IPathSegment)
            _segments.Add(segment)
        End Sub

        Public ReadOnly Property Segments As IEnumerable(Of IPathSegment) _
            Implements IPath2D.Segments
            Get
                Return _segments
            End Get
        End Property

        Public ReadOnly Property IsClosed As Boolean _
            Implements IPath2D.IsClosed
            Get
                If _segments.Count = 0 Then Return False
                Return _segments.First.StartPoint = _segments.Last.EndPoint
            End Get
        End Property

        Public ReadOnly Property BoundingBox As RectangleF _
            Implements IPath2D.BoundingBox
            Get
                Return Geometry2D.ComputeBoundingBox(_segments)
            End Get
        End Property
    End Class

#End Region

#Region "Tool Motion"

    Public Interface IToolMotion
        ReadOnly Property MotionType As ToolMotionType
        ReadOnly Property FeedRate As Double
        ReadOnly Property Geometry As Object
    End Interface

    Public Class LinearToolMotion
        Implements IToolMotion

        Public ReadOnly Property StartPoint As PointF
        Public ReadOnly Property EndPoint As PointF

        Public ReadOnly Property FeedRate As Double _
            Implements IToolMotion.FeedRate

        Public Sub New(startPt As PointF, endPt As PointF, feed As Double)
            StartPoint = startPt
            EndPoint = endPt
            FeedRate = feed
        End Sub

        Public ReadOnly Property MotionType As ToolMotionType _
            Implements IToolMotion.MotionType
            Get
                Return ToolMotionType.LinearCut
            End Get
        End Property

        Public ReadOnly Property Geometry As Object _
            Implements IToolMotion.Geometry
            Get
                Return Me
            End Get
        End Property
    End Class

#End Region

#Region "Features"

    Public Interface IGeometricFeature
        ReadOnly Property FeatureType As FeatureType
        ReadOnly Property SourcePath As IPath2D
        ReadOnly Property MinCornerRadius As Double
        ReadOnly Property MinSlotWidth As Double
        ReadOnly Property IsClosed As Boolean
        ReadOnly Property Area As Double
    End Interface

#End Region


#Region "Tool Shapes"

    Public MustInherit Class ToolShape

        Public ReadOnly Property ShapeType As ToolShapeType
        Public Overridable ReadOnly Property DisplayName As String

        Public MustOverride ReadOnly Property EffectiveDiameter As Double
        Public MustOverride ReadOnly Property EffectiveRadius As Double

        Public Overridable Function IsCompatibleWithFeature(
        feature As IGeometricFeature) As Boolean
            Return True
        End Function

        Public Overridable ReadOnly Property MinInternalCornerRadius As Double
            Get
                Return EffectiveRadius
            End Get
        End Property

        Public MustOverride Function CanCutInternalCorner(radius As Double) As Boolean
        Public MustOverride Function CanSlot(width As Double) As Boolean
        Public MustOverride Function GetProfileOffset() As Double

        Public MustOverride Function DecomposePath(
            inputPath As IPath2D
        ) As IEnumerable(Of IToolMotion)

        Protected Sub New(shapeType As ToolShapeType)
            Me.ShapeType = shapeType
        End Sub

    End Class
#End Region
#Region "ToolShape Compatibility Overrides"

    Public MustInherit Class RoundToolShape
        Inherits ToolShape

        Public ReadOnly Property Diameter As Double

        Public Sub New(diameter As Double)
            MyBase.New(ToolShapeType.Round)
            Me.Diameter = diameter
        End Sub

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature
    ) As Boolean
            ' Cannot cut internal corners smaller than tool radius
            If feature.MinCornerRadius < Me.EffectiveRadius Then Return False

            ' Cannot slot narrower than tool diameter
            If feature.MinSlotWidth < Me.EffectiveDiameter Then Return False

            Return True
        End Function
    End Class

    Public MustInherit Class SquareToolShape
        Inherits ToolShape

        Public ReadOnly Property Size As Double

        Public Sub New(size As Double)
            MyBase.New(ToolShapeType.Square)
            Me.Size = size
        End Sub

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature
    ) As Boolean
            ' Can cut any internal corner (sharp corners allowed)
            ' But cannot slot narrower than square size
            If feature.MinSlotWidth < Size Then Return False
            Return True
        End Function
    End Class

    Public MustInherit Class RectangularToolShape
        Inherits ToolShape

        Public ReadOnly Property Width As Double
        Public ReadOnly Property Height As Double

        Public Sub New(width As Double, height As Double)
            MyBase.New(ToolShapeType.Rectangle)
            Me.Width = width
            Me.Height = height
        End Sub

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature
    ) As Boolean
            ' Can cut any internal corner
            If feature.MinSlotWidth < Width Then Return False
            Return True
        End Function
    End Class

    Public MustInherit Class CornerRadiusToolShape
        Inherits ToolShape

        Public ReadOnly Property Diameter As Double
        Public ReadOnly Property CornerRadius As Double

        Public Sub New(diameter As Double, cornerRadius As Double)
            MyBase.New(ToolShapeType.CornerRadius)
            Me.Diameter = diameter
            Me.CornerRadius = cornerRadius
        End Sub

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature
    ) As Boolean
            ' Must respect corner radius
            If feature.MinCornerRadius < CornerRadius Then Return False

            ' Cannot slot narrower than diameter
            If feature.MinSlotWidth < Diameter Then Return False

            Return True
        End Function
    End Class
#End Region

#Region "Geometry2D (Stub)"

    Public NotInheritable Class Geometry2D

        Private Sub New()
        End Sub

        Public Shared Function PointOnCircle(
            center As PointF,
            radius As Double,
            angleRad As Double
        ) As PointF

            Return New PointF(
                CSng(center.X + radius * Math.Cos(angleRad)),
                CSng(center.Y + radius * Math.Sin(angleRad))
            )
        End Function

        Public Shared Function ComputeBoundingBox(
    segments As IEnumerable(Of IPathSegment)) As RectangleF

            If segments Is Nothing Then
                Return RectangleF.Empty
            End If

            Dim segmentList As List(Of IPathSegment) =
        segments.ToList()

            If segmentList.Count = 0 Then
                Return RectangleF.Empty
            End If

            Dim minX As Double = Double.MaxValue
            Dim minY As Double = Double.MaxValue
            Dim maxX As Double = Double.MinValue
            Dim maxY As Double = Double.MinValue

            For Each segment As IPathSegment In segmentList

                If segment Is Nothing Then
                    Continue For
                End If

                minX = Math.Min(minX, segment.StartPoint.X)
                minY = Math.Min(minY, segment.StartPoint.Y)
                maxX = Math.Max(maxX, segment.StartPoint.X)
                maxY = Math.Max(maxY, segment.StartPoint.Y)

                minX = Math.Min(minX, segment.EndPoint.X)
                minY = Math.Min(minY, segment.EndPoint.Y)
                maxX = Math.Max(maxX, segment.EndPoint.X)
                maxY = Math.Max(maxY, segment.EndPoint.Y)

                ' Arcs can extend beyond their endpoints.
                Dim arc As ArcSegment2D =
            TryCast(segment, ArcSegment2D)

                If arc Is Nothing Then
                    Continue For
                End If

                ' Check the four cardinal points on the circle.
                Dim angles() As Double = {
            0.0,
            Math.PI / 2.0,
            Math.PI,
            3.0 * Math.PI / 2.0
        }

                For Each angle As Double In angles

                    If IsAngleOnArc(
                angle,
                arc.StartAngle,
                arc.EndAngle,
                arc.IsClockwise) Then

                        Dim p As PointF =
                    PointOnCircle(
                        arc.Center,
                        arc.Radius,
                        angle)

                        minX = Math.Min(minX, p.X)
                        minY = Math.Min(minY, p.Y)
                        maxX = Math.Max(maxX, p.X)
                        maxY = Math.Max(maxY, p.Y)

                    End If

                Next

            Next

            If minX = Double.MaxValue Then
                Return RectangleF.Empty
            End If

            Return New RectangleF(
        CSng(minX),
        CSng(minY),
        CSng(maxX - minX),
        CSng(maxY - minY))

        End Function

        Public Shared Function IsAngleOnArc(
    angle As Double,
    startAngle As Double,
    endAngle As Double,
    isClockwise As Boolean) As Boolean

            Const TwoPi As Double = Math.PI * 2.0

            ' Normalize all angles to 0 .. 2π.
            angle = angle Mod TwoPi
            If angle < 0 Then angle += TwoPi

            startAngle = startAngle Mod TwoPi
            If startAngle < 0 Then startAngle += TwoPi

            endAngle = endAngle Mod TwoPi
            If endAngle < 0 Then endAngle += TwoPi

            If isClockwise Then

                Dim sweep As Double =
                    startAngle - endAngle

                If sweep < 0 Then
                    sweep += TwoPi
                End If

                Dim fromStart As Double =
                    startAngle - angle

                If fromStart < 0 Then
                    fromStart += TwoPi
                End If

                Return fromStart <= sweep

            Else

                Dim sweep As Double =
                    endAngle - startAngle

                If sweep < 0 Then
                    sweep += TwoPi
                End If

                Dim fromStart As Double =
                    angle - startAngle

                If fromStart < 0 Then
                    fromStart += TwoPi
                End If

                Return fromStart <= sweep

            End If

        End Function

        Public Shared Function IsPointInsidePath(
            path As IPath2D,
            testPoint As PointF) As Boolean

            If path Is Nothing OrElse
               Not path.IsClosed Then
                Return False
            End If

            Dim intersections As Integer = 0

            For Each segment As IPathSegment In path.Segments

                If segment Is Nothing Then
                    Continue For
                End If

                Select Case segment.SegmentType

                    Case PathSegmentType.Line

                        Dim line As LineSegment2D =
                            TryCast(segment, LineSegment2D)

                        If line Is Nothing Then Continue For

                        If RayIntersectsLine(
                            testPoint,
                            line.StartPoint,
                            line.EndPoint) Then

                            intersections += 1

                        End If

                    Case PathSegmentType.Arc

                        Dim arc As ArcSegment2D =
                            TryCast(segment, ArcSegment2D)

                        If arc Is Nothing Then Continue For

                        intersections +=
                            CountRayArcIntersections(
                                testPoint,
                                arc)

                End Select

            Next

            Return (intersections Mod 2) = 1

        End Function

        Private Shared Function RayIntersectsLine(
    testPoint As PointF,
    p1 As PointF,
    p2 As PointF) As Boolean

            ' Horizontal ray extending to +X.
            If p1.Y = p2.Y Then
                Return False
            End If

            ' The test point must lie between the segment's Y values.
            If testPoint.Y < Math.Min(p1.Y, p2.Y) OrElse
               testPoint.Y >= Math.Max(p1.Y, p2.Y) Then
                Return False
            End If

            Dim xIntersection As Double =
                p1.X +
                (testPoint.Y - p1.Y) *
                (p2.X - p1.X) /
                (p2.Y - p1.Y)

            Return xIntersection > testPoint.X

        End Function

        Private Shared Function CountRayArcIntersections(
    testPoint As PointF,
    arc As ArcSegment2D) As Integer

            Dim dy As Double =
                testPoint.Y - arc.Center.Y

            ' A horizontal ray intersects the circle at these X values.
            Dim value As Double =
                arc.Radius * arc.Radius -
                dy * dy

            If value <= 0 Then
                Return 0
            End If

            Dim dx As Double =
                Math.Sqrt(value)

            Dim x1 As Double =
                arc.Center.X - dx

            Dim x2 As Double =
                arc.Center.X + dx

            Dim count As Integer = 0

            ' Check the first circle intersection.
            If x1 > testPoint.X Then

                Dim angle1 As Double =
                    Math.Atan2(
                        testPoint.Y - arc.Center.Y,
                        x1 - arc.Center.X)

                If IsAngleOnArc(
                    angle1,
                    arc.StartAngle,
                    arc.EndAngle,
                    arc.IsClockwise) Then

                    count += 1

                End If

            End If

            ' Check the second circle intersection.
            If x2 > testPoint.X Then

                Dim angle2 As Double =
                    Math.Atan2(
                        testPoint.Y - arc.Center.Y,
                        x2 - arc.Center.X)

                If IsAngleOnArc(
                    angle2,
                    arc.StartAngle,
                    arc.EndAngle,
                    arc.IsClockwise) Then

                    count += 1

                End If

            End If

            Return count

        End Function

        Public Shared Function IsPathInsidePath(
            innerPath As IPath2D,
            outerPath As IPath2D) As Boolean

            If innerPath Is Nothing OrElse
               outerPath Is Nothing Then
                Return False
            End If

            If Not innerPath.IsClosed OrElse
               Not outerPath.IsClosed Then
                Return False
            End If

            Dim firstSegment As IPathSegment =
                innerPath.Segments.FirstOrDefault()

            If firstSegment Is Nothing Then
                Return False
            End If

            Dim testPoint As PointF =
                firstSegment.StartPoint

            Return IsPointInsidePath(
                outerPath,
                testPoint)

        End Function





    End Class

#End Region

End Namespace
