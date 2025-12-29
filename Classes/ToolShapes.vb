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

        Public ReadOnly Property StartPoint As PointF Implements IPathSegment.StartPoint
        Public ReadOnly Property EndPoint As PointF Implements IPathSegment.EndPoint

        Public Sub New(
            center As PointF,
            radius As Double,
            startAngle As Double,
            endAngle As Double
        )
            Me.Center = center
            Me.Radius = radius
            Me.StartAngle = startAngle
            Me.EndAngle = endAngle

            StartPoint = Geometry2D.PointOnCircle(center, radius, startAngle)
            EndPoint = Geometry2D.PointOnCircle(center, radius, endAngle)
        End Sub

        Public ReadOnly Property SegmentType As PathSegmentType Implements IPathSegment.SegmentType
            Get
                Return PathSegmentType.Arc
            End Get
        End Property

        Public ReadOnly Property Length As Double Implements IPathSegment.Length
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
            segments As IEnumerable(Of IPathSegment)
        ) As RectangleF

            Return RectangleF.Empty
        End Function

    End Class

#End Region

End Namespace
