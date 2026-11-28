Imports System.Drawing

Namespace ToolShapes

#Region "Enums"

    Public Enum PathSegmentType
        Line
        Arc
    End Enum

    Public Enum ProfileContainmentResult
        Outside
        Inside
        Intersecting
        Touching
        Invalid
    End Enum

    Public Enum SegmentIntersectionResult
        None
        Crossing
        Touching
        Overlapping
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
        Obround
        Diamond
        CornerRadius
        Single_D
        Double_D
        Trapezoid
        Keyhole
        Hexagon
        Custom
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

        Public Sub RemoveLast()
            If _segments.Count > 0 Then
                _segments.RemoveAt(_segments.Count - 1)
            End If
        End Sub
        Public Sub Clear()
            _segments.Clear()
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
                If _segments.Count = 0 Then
                    Return False
                End If

                Const tolerance As Double = 0.000001

                ' Start of the current contour.
                Dim contourStart As PointF =
            _segments(0).StartPoint

                ' End of the previous segment.
                Dim previousEnd As PointF =
            _segments(0).EndPoint

                For index As Integer = 1 To _segments.Count - 1

                    Dim current As IPathSegment =
                _segments(index)

                    If PointsEqual(
                previousEnd,
                current.StartPoint,
                tolerance) Then

                        ' Continue the current contour.
                        previousEnd = current.EndPoint

                    Else

                        ' A new contour is starting.
                        ' The previous contour must already be closed.
                        If Not PointsEqual(
                    previousEnd,
                    contourStart,
                    tolerance) Then

                            Return False
                        End If

                        ' Start the new contour.
                        contourStart = current.StartPoint
                        previousEnd = current.EndPoint

                    End If

                Next

                ' The final contour must also be closed.
                Return PointsEqual(
            previousEnd,
            contourStart,
            tolerance)

            End Get
        End Property

        Public ReadOnly Property BoundingBox As RectangleF _
            Implements IPath2D.BoundingBox
            Get
                Return Geometry2D.ComputeBoundingBox(_segments)
            End Get
        End Property

        Private Shared Function PointsEqual(
    p1 As PointF,
    p2 As PointF,
    tolerance As Double) As Boolean

            Dim dx As Double =
                CDbl(p1.X) - CDbl(p2.X)

            Dim dy As Double =
                CDbl(p1.Y) - CDbl(p2.Y)

            Return Math.Sqrt(
                dx * dx +
                dy * dy) <= tolerance

        End Function
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

        ''' <summary>
        ''' Authoritative 2D geometry of the physical tool.
        ''' The geometry is defined in the tool's local coordinate system,
        ''' with (0,0) as the tool reference center.
        ''' The geometry may contain one or more open or closed contours.
        ''' </summary>
        Public ReadOnly Property CuttingProfile As IPath2D

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

        Public MustOverride Function CanCutInternalCorner(
        radius As Double) As Boolean

        Public MustOverride Function CanSlot(
        width As Double) As Boolean

        Public MustOverride Function GetProfileOffset() As Double

        Public MustOverride Function DecomposePath(
        inputPath As IPath2D) As IEnumerable(Of IToolMotion)

        Protected Sub New(
    shapeType As ToolShapeType,
    cuttingProfile As IPath2D)

            If cuttingProfile Is Nothing Then
                Throw New ArgumentNullException(NameOf(cuttingProfile))
            End If

            Me.ShapeType = shapeType
            Me.CuttingProfile = cuttingProfile
        End Sub

    End Class
#End Region
#Region "ToolShape Compatibility Overrides"

    Public Class RoundToolShape
        Inherits ToolShape

        Public Sub New(diameter As Double)

            MyBase.New(
            ToolShapeType.Round,
            CreateCircularProfile(diameter / 2.0))

        End Sub

        Private Shared Function CreateCircularProfile(
        radius As Double) As IPath2D

            If radius <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(radius))
            End If

            Dim path As New Path2D()

            ' A complete circle is represented as one 360-degree arc.
            path.Add(
            New ArcSegment2D(
                New PointF(0.0F, 0.0F),
                radius,
                0.0,
                Math.PI * 2.0,
                False))

            Return path

        End Function

        Public ReadOnly Property Diameter As Double
            Get
                Return CSng(CuttingProfile.BoundingBox.Width)
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveDiameter As Double
            Get
                Return Diameter
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveRadius As Double
            Get
                Return Diameter / 2.0
            End Get
        End Property

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature) As Boolean

            If feature Is Nothing Then
                Return False
            End If

            ' Cannot cut internal corners smaller than tool radius.
            If feature.MinCornerRadius < EffectiveRadius Then
                Return False
            End If

            ' Cannot slot narrower than tool diameter.
            If feature.MinSlotWidth < EffectiveDiameter Then
                Return False
            End If

            Return True

        End Function

        Public Overrides Function CanCutInternalCorner(
        radius As Double) As Boolean

            Return radius >= EffectiveRadius

        End Function

        Public Overrides Function CanSlot(
        width As Double) As Boolean

            Return width >= EffectiveDiameter

        End Function

        Public Overrides Function GetProfileOffset() As Double

            Return EffectiveRadius

        End Function

        Public Overrides Function DecomposePath(
        inputPath As IPath2D) As IEnumerable(Of IToolMotion)

            Return ToolpathGenerator.OffsetPath(
            inputPath,
            GetProfileOffset())

        End Function

    End Class

    Public Class SquareToolShape
        Inherits ToolShape

        Public Sub New(
    size As Double,
    cornerRadius As Double)

            MyBase.New(
        ToolShapeType.Square,
        CreateSquareProfile(size, cornerRadius))

        End Sub
        Private Shared Function CreateSquareProfile(
    size As Double,
    cornerRadius As Double) As IPath2D

            If size <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(size))
            End If

            If cornerRadius < 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(cornerRadius))
            End If

            If cornerRadius > size / 2.0 Then
                Throw New ArgumentOutOfRangeException(
            NameOf(cornerRadius),
            "Corner radius cannot exceed half the square size.")
            End If

            Dim half As Double = size / 2.0
            Dim path As New Path2D()

            ' ---------------------------------------------------------
            ' Sharp-corner square
            ' ---------------------------------------------------------
            If cornerRadius = 0 Then

                ' Bottom
                path.Add(
            New LineSegment2D(
                New PointF(CSng(-half), CSng(-half)),
                New PointF(CSng(half), CSng(-half))))

                ' Right
                path.Add(
            New LineSegment2D(
                New PointF(CSng(half), CSng(-half)),
                New PointF(CSng(half), CSng(half))))

                ' Top
                path.Add(
            New LineSegment2D(
                New PointF(CSng(half), CSng(half)),
                New PointF(CSng(-half), CSng(half))))

                ' Left
                path.Add(
            New LineSegment2D(
                New PointF(CSng(-half), CSng(half)),
                New PointF(CSng(-half), CSng(-half))))

                Return path

            End If

            ' ---------------------------------------------------------
            ' Rounded-corner square
            ' ---------------------------------------------------------

            Dim r As Double = cornerRadius

            ' Bottom
            path.Add(
        New LineSegment2D(
            New PointF(CSng(-half + r), CSng(-half)),
            New PointF(CSng(half - r), CSng(-half))))

            ' Bottom-right corner
            path.Add(
        New ArcSegment2D(
            New PointF(CSng(half - r), CSng(-half + r)),
            r,
            -Math.PI / 2.0,
            0.0,
            False))

            ' Right
            path.Add(
        New LineSegment2D(
            New PointF(CSng(half), CSng(-half + r)),
            New PointF(CSng(half), CSng(half - r))))

            ' Top-right corner
            path.Add(
        New ArcSegment2D(
            New PointF(CSng(half - r), CSng(half - r)),
            r,
            0.0,
            Math.PI / 2.0,
            False))

            ' Top
            path.Add(
        New LineSegment2D(
            New PointF(CSng(half - r), CSng(half)),
            New PointF(CSng(-half + r), CSng(half))))

            ' Top-left corner
            path.Add(
        New ArcSegment2D(
            New PointF(CSng(-half + r), CSng(half - r)),
            r,
            Math.PI / 2.0,
            Math.PI,
            False))

            ' Left
            path.Add(
        New LineSegment2D(
            New PointF(CSng(-half), CSng(half - r)),
            New PointF(CSng(-half), CSng(-half + r))))

            ' Bottom-left corner
            path.Add(
        New ArcSegment2D(
            New PointF(CSng(-half + r), CSng(-half + r)),
            r,
            Math.PI,
            3.0 * Math.PI / 2.0,
            False))

            Return path

        End Function

        Public ReadOnly Property Size As Double
            Get
                Return CSng(CuttingProfile.BoundingBox.Width)
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveDiameter As Double
            Get
                ' For a non-round tool this represents the minimum
                ' effective cutting width used by the selector.
                Return Size
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveRadius As Double
            Get
                Return Size / 2.0
            End Get
        End Property

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature) As Boolean

            If feature Is Nothing Then
                Return False
            End If

            ' Square tools cannot enter a slot narrower than the square.
            If feature.MinSlotWidth < Size Then
                Return False
            End If

            Return True

        End Function

        Public Overrides Function CanCutInternalCorner(
        radius As Double) As Boolean

            ' A square tool can reach a sharp internal corner.
            Return True

        End Function

        Public Overrides Function CanSlot(
        width As Double) As Boolean

            Return width >= Size

        End Function

        Public Overrides Function GetProfileOffset() As Double

            Return Size / 2.0

        End Function

        Public Overrides Function DecomposePath(
        inputPath As IPath2D) As IEnumerable(Of IToolMotion)

            Return ToolpathGenerator.OffsetPath(
            inputPath,
            GetProfileOffset())

        End Function

    End Class

    Public Class RectangularToolShape
        Inherits ToolShape

        Public Sub New(
    width As Double,
    height As Double,
    cornerRadius As Double)

            MyBase.New(
        ToolShapeType.Rectangle,
        CreateRectangleProfile(
            width,
            height,
            cornerRadius))

        End Sub

        Private Shared Function CreateRectangleProfile(
    width As Double,
    height As Double,
    cornerRadius As Double) As IPath2D

            If width <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(width))
            End If

            If height <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(height))
            End If

            If cornerRadius < 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(cornerRadius))
            End If

            If cornerRadius > Math.Min(width, height) / 2.0 Then
                Throw New ArgumentOutOfRangeException(
            NameOf(cornerRadius),
            "Corner radius cannot exceed half the smaller rectangle dimension.")
            End If

            Dim halfWidth As Double = width / 2.0
            Dim halfHeight As Double = height / 2.0

            Dim path As New Path2D()

            ' ---------------------------------------------------------
            ' Sharp-corner rectangle
            ' ---------------------------------------------------------
            If cornerRadius = 0 Then

                ' Bottom
                path.Add(
            New LineSegment2D(
                New PointF(CSng(-halfWidth), CSng(-halfHeight)),
                New PointF(CSng(halfWidth), CSng(-halfHeight))))

                ' Right
                path.Add(
            New LineSegment2D(
                New PointF(CSng(halfWidth), CSng(-halfHeight)),
                New PointF(CSng(halfWidth), CSng(halfHeight))))

                ' Top
                path.Add(
            New LineSegment2D(
                New PointF(CSng(halfWidth), CSng(halfHeight)),
                New PointF(CSng(-halfWidth), CSng(halfHeight))))

                ' Left
                path.Add(
            New LineSegment2D(
                New PointF(CSng(-halfWidth), CSng(halfHeight)),
                New PointF(CSng(-halfWidth), CSng(-halfHeight))))

                Return path

            End If

            ' ---------------------------------------------------------
            ' Rounded-corner rectangle
            ' ---------------------------------------------------------

            Dim r As Double = cornerRadius

            ' Bottom
            path.Add(
        New LineSegment2D(
            New PointF(CSng(-halfWidth + r), CSng(-halfHeight)),
            New PointF(CSng(halfWidth - r), CSng(-halfHeight))))

            ' Bottom-right
            path.Add(
        New ArcSegment2D(
            New PointF(CSng(halfWidth - r), CSng(-halfHeight + r)),
            r,
            -Math.PI / 2.0,
            0.0,
            False))

            ' Right
            path.Add(
        New LineSegment2D(
            New PointF(CSng(halfWidth), CSng(-halfHeight + r)),
            New PointF(CSng(halfWidth), CSng(halfHeight - r))))

            ' Top-right
            path.Add(
        New ArcSegment2D(
            New PointF(CSng(halfWidth - r), CSng(halfHeight - r)),
            r,
            0.0,
            Math.PI / 2.0,
            False))

            ' Top
            path.Add(
        New LineSegment2D(
            New PointF(CSng(halfWidth - r), CSng(halfHeight)),
            New PointF(CSng(-halfWidth + r), CSng(halfHeight))))

            ' Top-left
            path.Add(
        New ArcSegment2D(
            New PointF(CSng(-halfWidth + r), CSng(halfHeight - r)),
            r,
            Math.PI / 2.0,
            Math.PI,
            False))

            ' Left
            path.Add(
        New LineSegment2D(
            New PointF(CSng(-halfWidth), CSng(halfHeight - r)),
            New PointF(CSng(-halfWidth), CSng(-halfHeight + r))))

            ' Bottom-left
            path.Add(
        New ArcSegment2D(
            New PointF(CSng(-halfWidth + r), CSng(-halfHeight + r)),
            r,
            Math.PI,
            3.0 * Math.PI / 2.0,
            False))

            Return path

        End Function

        Public ReadOnly Property Width As Double
            Get
                Return CuttingProfile.BoundingBox.Width
            End Get
        End Property

        Public ReadOnly Property Height As Double
            Get
                Return CuttingProfile.BoundingBox.Height
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveDiameter As Double
            Get
                ' For a rectangular tool, use the smaller cutting dimension
                ' as the effective width for tool-selection purposes.
                Return Math.Min(Width, Height)
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveRadius As Double
            Get
                Return EffectiveDiameter / 2.0
            End Get
        End Property

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature) As Boolean

            If feature Is Nothing Then
                Return False
            End If

            If feature.MinSlotWidth < EffectiveDiameter Then
                Return False
            End If

            Return True

        End Function

        Public Overrides Function CanCutInternalCorner(
        radius As Double) As Boolean

            ' Rectangular tool has square corners.
            Return True

        End Function

        Public Overrides Function CanSlot(
        width As Double) As Boolean

            Return width >= EffectiveDiameter

        End Function

        Public Overrides Function GetProfileOffset() As Double

            Return EffectiveDiameter / 2.0

        End Function

        Public Overrides Function DecomposePath(
        inputPath As IPath2D) As IEnumerable(Of IToolMotion)

            Return ToolpathGenerator.OffsetPath(
            inputPath,
            GetProfileOffset())

        End Function

    End Class
    'Public Class CornerRadiusToolShape
    '    Inherits ToolShape

    '    Public Sub New(
    '    diameter As Double,
    '    cornerRadius As Double)
    '        MyBase.New(
    '        ToolShapeType.CornerRadius,
    '        CreateCornerRadiusProfile(
    '            diameter,
    '            cornerRadius))

    '    End Sub

    '    Private Shared Function CreateCornerRadiusProfile(
    '    diameter As Double,
    '    cornerRadius As Double) As IPath2D

    '        If diameter <= 0 Then
    '            Throw New ArgumentOutOfRangeException(NameOf(diameter))
    '        End If

    '        If cornerRadius < 0 Then
    '            Throw New ArgumentOutOfRangeException(NameOf(cornerRadius))
    '        End If

    '        ' Temporary compatibility constructor.
    '        '
    '        ' This creates a square tool profile with the supplied
    '        ' corner radius applied equally to all four corners.
    '        '
    '        ' The profile itself remains authoritative. The next
    '        ' refinement will allow the four corner radii to differ.

    '        Dim half As Double = diameter / 2.0

    '        If cornerRadius > half Then
    '            Throw New ArgumentException(
    '            "Corner radius cannot exceed half the tool diameter.",
    '            NameOf(cornerRadius))
    '        End If

    '        Dim path As New Path2D()

    '        If cornerRadius = 0 Then

    '            path.Add(
    '            New LineSegment2D(
    '                New PointF(CSng(-half), CSng(-half)),
    '                New PointF(CSng(half), CSng(-half))))

    '            path.Add(
    '            New LineSegment2D(
    '                New PointF(CSng(half), CSng(-half)),
    '                New PointF(CSng(half), CSng(half))))

    '            path.Add(
    '            New LineSegment2D(
    '                New PointF(CSng(half), CSng(half)),
    '                New PointF(CSng(-half), CSng(half))))

    '            path.Add(
    '            New LineSegment2D(
    '                New PointF(CSng(-half), CSng(half)),
    '                New PointF(CSng(-half), CSng(-half))))

    '            Return path

    '        End If

    '        ' Bottom edge
    '        path.Add(
    '        New LineSegment2D(
    '            New PointF(
    '                CSng(-half + cornerRadius),
    '                CSng(-half)),
    '            New PointF(
    '                CSng(half - cornerRadius),
    '                CSng(-half))))

    '        ' Bottom-right corner
    '        path.Add(
    '        New ArcSegment2D(
    '            New PointF(
    '                CSng(half - cornerRadius),
    '                CSng(-half + cornerRadius)),
    '            cornerRadius,
    '            -Math.PI / 2.0,
    '            0.0,
    '            False))

    '        ' Right edge
    '        path.Add(
    '        New LineSegment2D(
    '            New PointF(
    '                CSng(half),
    '                CSng(-half + cornerRadius)),
    '            New PointF(
    '                CSng(half),
    '                CSng(half - cornerRadius))))

    '        ' Top-right corner
    '        path.Add(
    '        New ArcSegment2D(
    '            New PointF(
    '                CSng(half - cornerRadius),
    '                CSng(half - cornerRadius)),
    '            cornerRadius,
    '            0.0,
    '            Math.PI / 2.0,
    '            False))

    '        ' Top edge
    '        path.Add(
    '        New LineSegment2D(
    '            New PointF(
    '                CSng(half - cornerRadius),
    '                CSng(half)),
    '            New PointF(
    '                CSng(-half + cornerRadius),
    '                CSng(half))))

    '        ' Top-left corner
    '        path.Add(
    '        New ArcSegment2D(
    '            New PointF(
    '                CSng(-half + cornerRadius),
    '                CSng(half - cornerRadius)),
    '            cornerRadius,
    '            Math.PI / 2.0,
    '            Math.PI,
    '            False))

    '        ' Left edge
    '        path.Add(
    '        New LineSegment2D(
    '            New PointF(
    '                CSng(-half),
    '                CSng(half - cornerRadius)),
    '            New PointF(
    '                CSng(-half),
    '                CSng(-half + cornerRadius))))

    '        ' Bottom-left corner
    '        path.Add(
    '        New ArcSegment2D(
    '            New PointF(
    '                CSng(-half + cornerRadius),
    '                CSng(-half + cornerRadius)),
    '            cornerRadius,
    '            Math.PI,
    '            3.0 * Math.PI / 2.0,
    '            False))

    '        Return path

    '    End Function

    '    Public ReadOnly Property Diameter As Double
    '        Get
    '            Return Math.Min(
    '            CuttingProfile.BoundingBox.Width,
    '            CuttingProfile.BoundingBox.Height)
    '        End Get
    '    End Property

    '    Public ReadOnly Property CornerRadius As Double
    '        Get
    '            ' Temporary convenience value.
    '            '
    '            ' The actual corner radii are represented by the
    '            ' ArcSegment2D objects in CuttingProfile.
    '            For Each segment As IPathSegment In CuttingProfile.Segments

    '                Dim arc As ArcSegment2D =
    '                TryCast(segment, ArcSegment2D)

    '                If arc IsNot Nothing Then
    '                    Return arc.Radius
    '                End If

    '            Next

    '            Return 0.0

    '        End Get
    '    End Property

    '    Public Overrides ReadOnly Property EffectiveDiameter As Double
    '        Get
    '            Return Diameter
    '        End Get
    '    End Property

    '    Public Overrides ReadOnly Property EffectiveRadius As Double
    '        Get
    '            Return Diameter / 2.0
    '        End Get
    '    End Property

    '    Public Overrides Function IsCompatibleWithFeature(
    '    feature As IGeometricFeature) As Boolean

    '        If feature Is Nothing Then
    '            Return False
    '        End If

    '        If feature.MinCornerRadius < CornerRadius Then
    '            Return False
    '        End If

    '        If feature.MinSlotWidth < EffectiveDiameter Then
    '            Return False
    '        End If

    '        Return True

    '    End Function

    '    Public Overrides Function CanCutInternalCorner(
    '    radius As Double) As Boolean

    '        Return radius >= CornerRadius

    '    End Function

    '    Public Overrides Function CanSlot(
    '    width As Double) As Boolean

    '        Return width >= EffectiveDiameter

    '    End Function

    '    Public Overrides Function GetProfileOffset() As Double

    '        Return EffectiveRadius

    '    End Function

    '    Public Overrides Function DecomposePath(
    '    inputPath As IPath2D) As IEnumerable(Of IToolMotion)

    '        Return ToolpathGenerator.OffsetPath(
    '        inputPath,
    '        GetProfileOffset())

    '    End Function

    'End Class
#End Region

    Public Class ObroundToolShape
        Inherits ToolShape

        Public Sub New(
        width As Double,
        length As Double)

            MyBase.New(
            ToolShapeType.Obround,
            CreateObroundProfile(width, length))

        End Sub

        Private Shared Function CreateObroundProfile(
        width As Double,
        length As Double) As IPath2D

            If width <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(width))
            End If

            If length <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(length))
            End If

            If length < width Then
                Throw New ArgumentException(
                "Obround length cannot be less than its width.",
                NameOf(length))
            End If

            Dim radius As Double = width / 2.0
            Dim halfLength As Double = length / 2.0

            ' Distance from the center to each semicircle center.
            Dim halfStraight As Double =
            halfLength - radius

            Dim path As New Path2D()

            ' Top straight segment.
            path.Add(
            New LineSegment2D(
                New PointF(
                    CSng(-halfStraight),
                    CSng(radius)),
                New PointF(
                    CSng(halfStraight),
                    CSng(radius))))

            ' Right semicircle.
            path.Add(
            New ArcSegment2D(
                New PointF(
                    CSng(halfStraight),
                    0.0F),
                radius,
                Math.PI / 2.0,
                -Math.PI / 2.0,
                True))

            ' Bottom straight segment.
            path.Add(
            New LineSegment2D(
                New PointF(
                    CSng(halfStraight),
                    CSng(-radius)),
                New PointF(
                    CSng(-halfStraight),
                    CSng(-radius))))

            ' Left semicircle.
            path.Add(
    New ArcSegment2D(
        New PointF(
            CSng(-halfStraight),
            0.0F),
        radius,
        -Math.PI / 2.0,
        -3.0 * Math.PI / 2.0,
        True))
            Return path

        End Function

        Public ReadOnly Property Width As Double
            Get
                Return CuttingProfile.BoundingBox.Height
            End Get
        End Property

        Public ReadOnly Property Length As Double
            Get
                Return CuttingProfile.BoundingBox.Width
            End Get
        End Property
        Public Overrides ReadOnly Property EffectiveDiameter As Double
            Get
                Return Width
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveRadius As Double
            Get
                Return Width / 2.0
            End Get
        End Property

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature) As Boolean

            If feature Is Nothing Then
                Return False
            End If

            If feature.MinSlotWidth < Width Then
                Return False
            End If

            Return True

        End Function

        Public Overrides Function CanCutInternalCorner(
        radius As Double) As Boolean

            Return radius >= EffectiveRadius

        End Function

        Public Overrides Function CanSlot(
        width As Double) As Boolean

            Return width >= EffectiveDiameter

        End Function

        Public Overrides Function GetProfileOffset() As Double

            Return EffectiveRadius

        End Function

        Public Overrides Function DecomposePath(
        inputPath As IPath2D) As IEnumerable(Of IToolMotion)

            Return ToolpathGenerator.OffsetPath(
            inputPath,
            GetProfileOffset())

        End Function

    End Class

    Public Class DiamondToolShape
        Inherits ToolShape

        Public Sub New(
    width As Double,
    height As Double,
    cornerRadius As Double)

            MyBase.New(
        ToolShapeType.Diamond,
        CreateDiamondProfile(
            width,
            height,
            cornerRadius))

        End Sub

        Private Shared Function CreateDiamondProfile(
    width As Double,
    height As Double,
    cornerRadius As Double) As IPath2D

            If width <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(width))
            End If

            If height <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(height))
            End If

            If cornerRadius < 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(cornerRadius))
            End If

            ' Length is along X; Width is along Y.
            Dim halfWidth As Double = height / 2.0
            Dim halfHeight As Double = width / 2.0

            Dim path As New Path2D()

            ' ---------------------------------------------------------
            ' Sharp-corner diamond
            ' ---------------------------------------------------------
            If cornerRadius = 0 Then

                path.Add(
            New LineSegment2D(
                New PointF(0.0F, CSng(halfHeight)),
                New PointF(CSng(halfWidth), 0.0F)))

                path.Add(
            New LineSegment2D(
                New PointF(CSng(halfWidth), 0.0F),
                New PointF(0.0F, CSng(-halfHeight))))

                path.Add(
            New LineSegment2D(
                New PointF(0.0F, CSng(-halfHeight)),
                New PointF(CSng(-halfWidth), 0.0F)))

                path.Add(
            New LineSegment2D(
                New PointF(CSng(-halfWidth), 0.0F),
                New PointF(0.0F, CSng(halfHeight))))

                Return path

            End If

            ' ---------------------------------------------------------
            ' Diamond vertices
            ' ---------------------------------------------------------

            Dim vertices(3) As PointF

            vertices(0) = New PointF(
    0.0F,
    CSng(halfHeight))

            vertices(1) = New PointF(
    CSng(halfWidth),
    0.0F)

            vertices(2) = New PointF(
    0.0F,
    CSng(-halfHeight))

            vertices(3) = New PointF(
    CSng(-halfWidth),
    0.0F)

            Dim tangentBefore(3) As PointF
            Dim tangentAfter(3) As PointF
            Dim arcCenters(3) As PointF

            ' ---------------------------------------------------------
            ' Calculate tangent points and arc centers
            ' ---------------------------------------------------------

            For i As Integer = 0 To 3

                Dim prevIndex As Integer = i - 1

                If prevIndex < 0 Then
                    prevIndex = 3
                End If

                Dim nextIndex As Integer = i + 1

                If nextIndex > 3 Then
                    nextIndex = 0
                End If

                Dim vx As Double = vertices(i).X
                Dim vy As Double = vertices(i).Y

                Dim v1x As Double =
            vertices(prevIndex).X - vx

                Dim v1y As Double =
            vertices(prevIndex).Y - vy

                Dim v2x As Double =
            vertices(nextIndex).X - vx

                Dim v2y As Double =
            vertices(nextIndex).Y - vy

                Dim len1 As Double =
            Math.Sqrt(v1x * v1x + v1y * v1y)

                Dim len2 As Double =
            Math.Sqrt(v2x * v2x + v2y * v2y)

                v1x /= len1
                v1y /= len1

                v2x /= len2
                v2y /= len2

                Dim dot As Double =
            v1x * v2x + v1y * v2y

                dot = Math.Max(-1.0, Math.Min(1.0, dot))

                Dim cornerAngle As Double =
            Math.Acos(dot)

                Dim tangentDistance As Double =
            cornerRadius /
            Math.Tan(cornerAngle / 2.0)

                If tangentDistance * 2.0 >= Math.Min(len1, len2) Then
                    Throw New ArgumentOutOfRangeException(
                NameOf(cornerRadius),
                "Corner radius is too large for the diamond dimensions.")
                End If

                tangentBefore(i) =
            New PointF(
                CSng(vx + v1x * tangentDistance),
                CSng(vy + v1y * tangentDistance))

                tangentAfter(i) =
            New PointF(
                CSng(vx + v2x * tangentDistance),
                CSng(vy + v2y * tangentDistance))

                Dim bx As Double = v1x + v2x
                Dim by As Double = v1y + v2y

                Dim bisectorLength As Double =
            Math.Sqrt(bx * bx + by * by)

                bx /= bisectorLength
                by /= bisectorLength

                Dim centerDistance As Double =
            cornerRadius /
            Math.Sin(cornerAngle / 2.0)

                arcCenters(i) =
            New PointF(
                CSng(vx + bx * centerDistance),
                CSng(vy + by * centerDistance))

            Next

            ' ---------------------------------------------------------
            ' Build the rounded diamond
            ' ---------------------------------------------------------

            For i As Integer = 0 To 3

                Dim previousCorner As Integer = i - 1

                If previousCorner < 0 Then
                    previousCorner = 3
                End If

                path.Add(
            New LineSegment2D(
                tangentAfter(previousCorner),
                tangentBefore(i)))

                Dim center As PointF = arcCenters(i)

                Dim startAngle As Double =
    Math.Atan2(
        tangentBefore(i).Y - center.Y,
        tangentBefore(i).X - center.X)

                Dim endAngle As Double =
    Math.Atan2(
        tangentAfter(i).Y - center.Y,
        tangentAfter(i).X - center.X)

                Dim delta As Double = endAngle - startAngle

                While delta > Math.PI
                    delta -= 2.0 * Math.PI
                End While

                While delta < -Math.PI
                    delta += 2.0 * Math.PI
                End While

                endAngle = startAngle + delta

                path.Add(
    New ArcSegment2D(
        center,
        cornerRadius,
        startAngle,
        endAngle,
        True))
            Next

            Return path

        End Function

        Public ReadOnly Property Width As Double
            Get
                Return CuttingProfile.BoundingBox.Height
            End Get
        End Property

        Public ReadOnly Property Length As Double
            Get
                Return CuttingProfile.BoundingBox.Width
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveDiameter As Double
            Get
                Return Math.Min(Width, Length)
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveRadius As Double
            Get
                Return EffectiveDiameter / 2.0
            End Get
        End Property

        Public Overrides Function IsCompatibleWithFeature(
        feature As IGeometricFeature) As Boolean

            If feature Is Nothing Then
                Return False
            End If

            If feature.MinSlotWidth < EffectiveDiameter Then
                Return False
            End If

            Return True

        End Function

        Public Overrides Function CanCutInternalCorner(
        radius As Double) As Boolean

            ' The diamond has four straight-sided corners.
            Return True

        End Function

        Public Overrides Function CanSlot(
        width As Double) As Boolean

            Return width >= EffectiveDiameter

        End Function

        Public Overrides Function GetProfileOffset() As Double

            Return EffectiveRadius

        End Function

        Public Overrides Function DecomposePath(
        inputPath As IPath2D) As IEnumerable(Of IToolMotion)

            Return ToolpathGenerator.OffsetPath(
            inputPath,
            GetProfileOffset())

        End Function

    End Class

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


        Public Shared Function ComputeTransformedBoundingBox(
    segments As IEnumerable(Of IPathSegment),
    centerX As Double,
    centerY As Double,
    angleDegrees As Double) As RectangleF

            If segments Is Nothing Then
                Return RectangleF.Empty
            End If

            Dim angleRadians As Double =
        angleDegrees * Math.PI / 180.0

            Dim cosA As Double = Math.Cos(angleRadians)
            Dim sinA As Double = Math.Sin(angleRadians)

            Dim transformedSegments As New List(Of IPathSegment)

            For Each segment As IPathSegment In segments

                If segment Is Nothing Then Continue For

                If TypeOf segment Is LineSegment2D Then

                    Dim x1 As Double =
                segment.StartPoint.X * cosA -
                segment.StartPoint.Y * sinA + centerX

                    Dim y1 As Double =
                segment.StartPoint.X * sinA +
                segment.StartPoint.Y * cosA + centerY

                    Dim x2 As Double =
                segment.EndPoint.X * cosA -
                segment.EndPoint.Y * sinA + centerX

                    Dim y2 As Double =
                segment.EndPoint.X * sinA +
                segment.EndPoint.Y * cosA + centerY

                    transformedSegments.Add(
                New LineSegment2D(
                    New PointF(CSng(x1), CSng(y1)),
                    New PointF(CSng(x2), CSng(y2))))

                ElseIf TypeOf segment Is ArcSegment2D Then

                    Dim arc As ArcSegment2D =
                DirectCast(segment, ArcSegment2D)

                    Dim x As Double =
                arc.Center.X * cosA -
                arc.Center.Y * sinA + centerX

                    Dim y As Double =
                arc.Center.X * sinA +
                arc.Center.Y * cosA + centerY

                    transformedSegments.Add(
                New ArcSegment2D(
                    New PointF(CSng(x), CSng(y)),
                    arc.Radius,
                    arc.StartAngle + angleRadians,
                    arc.EndAngle + angleRadians,
                    arc.IsClockwise))

                End If

            Next

            Return ComputeBoundingBox(transformedSegments)

        End Function

        Public Shared Function GetLinearNibbleHitPositions(
    startPoint As PointF,
    endPoint As PointF,
    cuttingLength As Double,
    overlap As Double) As List(Of PointF)

            Dim hits As New List(Of PointF)

            If cuttingLength <= 0.0 Then
                Throw New ArgumentOutOfRangeException(
            NameOf(cuttingLength))
            End If

            If overlap < 0.0 OrElse overlap >= cuttingLength Then
                Throw New ArgumentOutOfRangeException(
            NameOf(overlap))
            End If

            Dim dx As Double =
        CDbl(endPoint.X) - CDbl(startPoint.X)

            Dim dy As Double =
        CDbl(endPoint.Y) - CDbl(startPoint.Y)

            Dim length As Double =
        Math.Sqrt(dx * dx + dy * dy)

            If length = 0.0 Then
                hits.Add(startPoint)
                Return hits
            End If

            Dim maximumSpacing As Double =
        cuttingLength - overlap

            Dim intervalCount As Integer =
        CInt(Math.Ceiling(length / maximumSpacing))

            For i As Integer = 0 To intervalCount

                Dim fraction As Double =
            CDbl(i) / CDbl(intervalCount)

                hits.Add(
            New PointF(
                CSng(startPoint.X + dx * fraction),
                CSng(startPoint.Y + dy * fraction)))

            Next

            Return hits

        End Function




        Public Shared Function IsAngleOnArc(
    angle As Double,
    startAngle As Double,
    endAngle As Double,
    isClockwise As Boolean) As Boolean

            Const TwoPi As Double = Math.PI * 2.0

            ' A full 360-degree arc includes every angle.
            Dim originalSweep As Double =
    Math.Abs(endAngle - startAngle)

            If originalSweep >= TwoPi - 0.000001 Then
                Return True
            End If




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

            If arc Is Nothing OrElse arc.Radius <= 0.0 Then
                Return 0
            End If

            Const TwoPi As Double = Math.PI * 2.0

            Dim direction As Double =
        If(arc.IsClockwise, -1.0, 1.0)

            Dim sweep As Double =
        direction * (arc.EndAngle - arc.StartAngle)

            If Math.Abs(arc.EndAngle - arc.StartAngle) >=
        TwoPi - 0.000001 Then

                sweep = TwoPi

            Else

                sweep = sweep Mod TwoPi

                If sweep < 0.0 Then
                    sweep += TwoPi
                End If

            End If

            If sweep = 0.0 Then Return 0

            Dim breaks As New List(Of Double) From {
        0.0,
        sweep
    }

            ' Divide the arc at its Y extrema.
            Dim extrema() As Double = {
        Math.PI / 2.0,
        3.0 * Math.PI / 2.0
    }

            For Each angle As Double In extrema

                Dim offset As Double =
            direction * (angle - arc.StartAngle)

                offset = offset Mod TwoPi

                If offset < 0.0 Then
                    offset += TwoPi
                End If

                If offset > 0.0 AndAlso offset < sweep Then
                    breaks.Add(offset)
                End If

            Next

            breaks.Sort()

            Dim count As Integer = 0

            For i As Integer = 0 To breaks.Count - 2

                Dim a1 As Double =
            arc.StartAngle + direction * breaks(i)

                Dim a2 As Double =
            arc.StartAngle + direction * breaks(i + 1)

                Dim p1 As PointF =
            PointOnCircle(arc.Center, arc.Radius, a1)

                Dim p2 As PointF =
            PointOnCircle(arc.Center, arc.Radius, a2)

                If i = 0 Then
                    p1 = arc.StartPoint
                End If

                If i = breaks.Count - 2 Then
                    p2 = arc.EndPoint
                End If

                ' Apply the same endpoint convention
                ' used by RayIntersectsLine().
                If testPoint.Y < Math.Min(p1.Y, p2.Y) OrElse
           testPoint.Y >= Math.Max(p1.Y, p2.Y) Then

                    Continue For

                End If

                Dim dy As Double =
            CDbl(testPoint.Y) - CDbl(arc.Center.Y)

                Dim value As Double =
            arc.Radius * arc.Radius - dy * dy

                Dim dx As Double =
            Math.Sqrt(Math.Max(0.0, value))

                ' Each monotonic section occupies either
                ' the left or right half of the circle.
                Dim middleAngle As Double =
            (a1 + a2) / 2.0

                Dim xIntersection As Double =
            CDbl(arc.Center.X) +
            If(Math.Cos(middleAngle) >= 0.0, dx, -dx)

                ' Use the stored endpoint when the ray
                ' passes through that endpoint.
                If testPoint.Y = p1.Y Then

                    xIntersection = p1.X

                ElseIf testPoint.Y = p2.Y Then

                    xIntersection = p2.X

                End If

                If xIntersection > testPoint.X Then
                    count += 1
                End If

            Next

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


        Public Shared Function ClassifyProfileContainment(
    innerPath As IPath2D,
    outerPath As IPath2D) As ProfileContainmentResult

            Const tolerance As Double = 0.000001

            If innerPath Is Nothing OrElse
       outerPath Is Nothing Then

                Return ProfileContainmentResult.Invalid

            End If

            If Not innerPath.IsClosed OrElse
       Not outerPath.IsClosed Then

                Return ProfileContainmentResult.Invalid

            End If

            Dim innerSegments = innerPath.Segments.ToList()
            Dim outerSegments = outerPath.Segments.ToList()

            If innerSegments.Count = 0 OrElse
       outerSegments.Count = 0 Then

                Return ProfileContainmentResult.Invalid

            End If

            ' Validate supported segment types.
            For Each segment As IPathSegment In
        innerSegments.Concat(outerSegments)

                If TypeOf segment Is LineSegment2D Then

                    Continue For

                ElseIf TypeOf segment Is ArcSegment2D Then

                    Dim arc As ArcSegment2D =
                DirectCast(segment, ArcSegment2D)

                    If arc.Radius <= 0.0 Then
                        Return ProfileContainmentResult.Invalid
                    End If

                Else

                    Return ProfileContainmentResult.Invalid

                End If

            Next

            ' Check every inner boundary segment against
            ' every outer boundary segment.
            For Each innerSegment As IPathSegment In innerSegments

                For Each outerSegment As IPathSegment In outerSegments

                    If SegmentsIntersect(
                innerSegment,
                outerSegment,
                tolerance) Then

                        Return ProfileContainmentResult.Intersecting

                    End If

                Next

            Next

            ' With no boundary intersections, determine
            ' which side of the outer boundary we occupy.
            Dim testPoint As PointF =
        innerSegments(0).StartPoint

            If IsPointInsidePath(outerPath, testPoint) Then

                Return ProfileContainmentResult.Inside

            End If

            Return ProfileContainmentResult.Outside

        End Function

        Private Shared Function LineSegmentsIntersect(
            a As LineSegment2D,
            b As LineSegment2D) As Boolean

            If a Is Nothing OrElse b Is Nothing Then
                Return False
            End If

            Const tolerance As Double = 0.0000001

            Dim ax As Double = a.EndPoint.X - a.StartPoint.X
            Dim ay As Double = a.EndPoint.Y - a.StartPoint.Y

            Dim bx As Double = b.EndPoint.X - b.StartPoint.X
            Dim by As Double = b.EndPoint.Y - b.StartPoint.Y

            Dim cx As Double = b.StartPoint.X - a.StartPoint.X
            Dim cy As Double = b.StartPoint.Y - a.StartPoint.Y

            Dim denominator As Double = ax * by - ay * bx

            If Math.Abs(denominator) <= tolerance Then

                ' Parallel lines may also be collinear.
                Dim cross As Double = cx * ay - cy * ax

                If Math.Abs(cross) > tolerance Then
                    Return False
                End If

                ' Collinear lines: check whether their
                ' projected intervals overlap or touch.
                Dim aMin As Double
                Dim aMax As Double
                Dim bMin As Double
                Dim bMax As Double

                If Math.Abs(ax) >= Math.Abs(ay) Then

                    aMin = Math.Min(a.StartPoint.X, a.EndPoint.X)
                    aMax = Math.Max(a.StartPoint.X, a.EndPoint.X)

                    bMin = Math.Min(b.StartPoint.X, b.EndPoint.X)
                    bMax = Math.Max(b.StartPoint.X, b.EndPoint.X)

                Else

                    aMin = Math.Min(a.StartPoint.Y, a.EndPoint.Y)
                    aMax = Math.Max(a.StartPoint.Y, a.EndPoint.Y)

                    bMin = Math.Min(b.StartPoint.Y, b.EndPoint.Y)
                    bMax = Math.Max(b.StartPoint.Y, b.EndPoint.Y)

                End If

                Return aMax >= bMin - tolerance AndAlso
           bMax >= aMin - tolerance

            End If

            Dim t As Double = (cx * by - cy * bx) / denominator
            Dim u As Double = (cx * ay - cy * ax) / denominator

            Return t >= -tolerance AndAlso
                   t <= 1.0 + tolerance AndAlso
                   u >= -tolerance AndAlso
                   u <= 1.0 + tolerance

        End Function

        Public Shared Sub TestPointContainment()

            Dim passed As Integer = 0
            Dim failed As Integer = 0

            Dim check As Action(Of String, IPath2D, PointF, Boolean) =
                Sub(testName, path, testPoint, expected)

                    Dim actual As Boolean =
                        IsPointInsidePath(path, testPoint)

                    Dim success As Boolean =
                        path.IsClosed AndAlso actual = expected

                    If success Then
                        passed += 1
                    Else
                        failed += 1
                    End If

                    Debug.WriteLine(
                        If(success, "PASS", "FAIL") &
                        " | " & testName &
                        " | Closed=" & path.IsClosed.ToString() &
                        " | Expected=" & expected.ToString() &
                        " | Actual=" & actual.ToString())

                End Sub

            '-----------------------------------------
            ' Rectangle
            '-----------------------------------------
            Dim rectangle As New Path2D

            rectangle.Add(New LineSegment2D(
                New PointF(0, 0), New PointF(10, 0)))

            rectangle.Add(New LineSegment2D(
                New PointF(10, 0), New PointF(10, 6)))

            rectangle.Add(New LineSegment2D(
                New PointF(10, 6), New PointF(0, 6)))

            rectangle.Add(New LineSegment2D(
                New PointF(0, 6), New PointF(0, 0)))

            check("Rectangle - Inside",
                  rectangle, New PointF(5, 3), True)

            check("Rectangle - Outside",
                  rectangle, New PointF(12, 3), False)

            '-----------------------------------------
            ' Full circle - CCW
            '-----------------------------------------
            Dim circleCCW As New Path2D

            circleCCW.Add(New ArcSegment2D(
                New PointF(5, 5),
                3.0,
                0.0,
                Math.PI * 2.0,
                False))

            check("Circle CCW - Inside",
                  circleCCW, New PointF(5, 5), True)

            check("Circle CCW - Outside",
                  circleCCW, New PointF(9, 5), False)

            check("Circle CCW - Two crossings",
                  circleCCW, New PointF(0, 5), False)

            check("Circle CCW - Upper tangent",
                  circleCCW, New PointF(0, 8), False)

            check("Circle CCW - Lower tangent",
                  circleCCW, New PointF(0, 2), False)

            '-----------------------------------------
            ' Full circle - CW
            '-----------------------------------------
            Dim circleCW As New Path2D

            circleCW.Add(New ArcSegment2D(
                New PointF(5, 5),
                3.0,
                Math.PI * 2.0,
                0.0,
                True))

            check("Circle CW - Inside",
                  circleCW, New PointF(5, 5), True)

            check("Circle CW - Outside",
                  circleCW, New PointF(0, 5), False)

            '-----------------------------------------
            ' Mixed Line-Arc profile
            '-----------------------------------------
            Dim mixed As New Path2D

            mixed.Add(New LineSegment2D(
                New PointF(0, 0), New PointF(5, 0)))

            mixed.Add(New ArcSegment2D(
                New PointF(5, 2),
                2.0,
                -Math.PI / 2.0,
                Math.PI / 2.0,
                False))

            mixed.Add(New LineSegment2D(
                New PointF(5, 4), New PointF(0, 4)))

            mixed.Add(New LineSegment2D(
                New PointF(0, 4), New PointF(0, 0)))

            check("Mixed - Inside rectangular portion",
                  mixed, New PointF(2, 2), True)

            check("Mixed - Inside curved portion",
                  mixed, New PointF(6, 2), True)

            check("Mixed - Outside",
                  mixed, New PointF(8, 2), False)

            check("Mixed - Upper tangent",
                  mixed, New PointF(-1, 4), False)

            check("Mixed - Lower tangent",
                  mixed, New PointF(-1, 0), False)

            '-----------------------------------------
            ' Shared Line-Arc endpoints
            '-----------------------------------------
            Dim halfCircle As New Path2D

            halfCircle.Add(New LineSegment2D(
                New PointF(2, 5), New PointF(8, 5)))

            halfCircle.Add(New ArcSegment2D(
                New PointF(5, 5),
                3.0,
                0.0,
                Math.PI,
                False))

            check("Half circle - Inside",
                  halfCircle, New PointF(5, 6), True)

            check("Half circle - Outside",
                  halfCircle, New PointF(5, 4), False)

            check("Half circle - Shared endpoints",
                  halfCircle, New PointF(0, 5), False)

            '-----------------------------------------
            ' Circle constructed from two arcs
            '-----------------------------------------
            Dim twoArcs As New Path2D

            twoArcs.Add(New ArcSegment2D(
                New PointF(5, 5),
                3.0,
                0.0,
                Math.PI,
                False))

            twoArcs.Add(New ArcSegment2D(
                New PointF(5, 5),
                3.0,
                Math.PI,
                Math.PI * 2.0,
                False))

            check("Two arcs - Shared endpoint inside",
                  twoArcs, New PointF(5, 5), True)

            check("Two arcs - Shared endpoints outside",
                  twoArcs, New PointF(0, 5), False)

            '-----------------------------------------
            ' Arc crossing zero degrees
            '-----------------------------------------
            Dim wraparound As New Path2D

            wraparound.Add(New ArcSegment2D(
                New PointF(5, 5),
                3.0,
                3.0 * Math.PI / 2.0,
                Math.PI / 2.0,
                False))

            wraparound.Add(New LineSegment2D(
                New PointF(5, 8), New PointF(5, 2)))

            check("Wraparound - Inside",
                  wraparound, New PointF(6, 5), True)

            check("Wraparound - Outside",
                  wraparound, New PointF(4, 5), False)

            '-----------------------------------------
            ' Results
            '-----------------------------------------
            Debug.WriteLine(
                "====================================")

            Debug.WriteLine(
                "POINT CONTAINMENT TEST RESULTS")

            Debug.WriteLine("Passed: " & passed)
            Debug.WriteLine("Failed: " & failed)

            Debug.WriteLine(
                "====================================")

        End Sub


        Private Shared Function CoincidentArcsIntersect(
    arcA As ArcSegment2D,
    arcB As ArcSegment2D,
    tolerance As Double) As Boolean

            If arcA Is Nothing OrElse
               arcB Is Nothing Then

                Return False

            End If

            If arcA.Radius <= 0.0 OrElse
               arcB.Radius <= 0.0 Then

                Return False

            End If

            Dim dx As Double =
                CDbl(arcB.Center.X) - CDbl(arcA.Center.X)

            Dim dy As Double =
                CDbl(arcB.Center.Y) - CDbl(arcA.Center.Y)

            Dim centerDistance As Double =
                Math.Sqrt((dx * dx) + (dy * dy))

            ' Arcs must share the same supporting circle.
            If centerDistance > tolerance Then
                Return False
            End If

            If Math.Abs(arcA.Radius - arcB.Radius) > tolerance Then
                Return False
            End If

            ' Determine whether either arc contains an
            ' angular endpoint belonging to the other arc.
            Return IsAngleOnArc(
                arcB.StartAngle,
                arcA.StartAngle,
                arcA.EndAngle,
                arcA.IsClockwise) OrElse
                   IsAngleOnArc(
                arcB.EndAngle,
                arcA.StartAngle,
                arcA.EndAngle,
                arcA.IsClockwise) OrElse
                   IsAngleOnArc(
                arcA.StartAngle,
                arcB.StartAngle,
                arcB.EndAngle,
                arcB.IsClockwise) OrElse
                   IsAngleOnArc(
                arcA.EndAngle,
                arcB.StartAngle,
                arcB.EndAngle,
                arcB.IsClockwise)

        End Function

        Private Shared Function SegmentsIntersect(
    segmentA As IPathSegment,
    segmentB As IPathSegment,
    tolerance As Double) As Boolean

            If segmentA Is Nothing OrElse
               segmentB Is Nothing Then

                Return False

            End If

            Dim lineA As LineSegment2D =
                TryCast(segmentA, LineSegment2D)

            Dim lineB As LineSegment2D =
                TryCast(segmentB, LineSegment2D)

            Dim arcA As ArcSegment2D =
                TryCast(segmentA, ArcSegment2D)

            Dim arcB As ArcSegment2D =
                TryCast(segmentB, ArcSegment2D)

            Dim points As List(Of PointF) = Nothing
            Dim uA As List(Of Double) = Nothing
            Dim uB As List(Of Double) = Nothing

            ' Line / Line
            If lineA IsNot Nothing AndAlso
               lineB IsNot Nothing Then

                Return LineSegmentsIntersect(lineA, lineB)

            End If

            ' Line / Arc
            If lineA IsNot Nothing AndAlso
               arcB IsNot Nothing Then

                Return ConvexHullGeometry.TestIntersectArcLine(
                    arcB,
                    lineA,
                    tolerance,
                    points,
                    uB,
                    uA)

            End If

            ' Arc / Line
            If arcA IsNot Nothing AndAlso
               lineB IsNot Nothing Then

                Return ConvexHullGeometry.TestIntersectArcLine(
                    arcA,
                    lineB,
                    tolerance,
                    points,
                    uA,
                    uB)

            End If

            ' Arc / Arc
            If arcA IsNot Nothing AndAlso
               arcB IsNot Nothing Then

                If CoincidentArcsIntersect(
                    arcA,
                    arcB,
                    tolerance) Then

                    Return True

                End If

                Return ConvexHullGeometry.TestIntersectArcArc(
                    arcA,
                    arcB,
                    tolerance,
                    points,
                    uA,
                    uB)

            End If

            Return False

        End Function

        Public Shared Sub TestProfileContainment()

            Dim passed As Integer = 0
            Dim failed As Integer = 0

            Dim check As Action(Of String,
                                   IPath2D,
                                   IPath2D,
                                   ProfileContainmentResult) =
                Sub(description, inner, outer, expected)

                    Dim actual As ProfileContainmentResult =
                        ClassifyProfileContainment(inner, outer)

                    Dim success As Boolean =
                        actual = expected

                    If success Then
                        passed += 1
                    Else
                        failed += 1
                    End If

                    Debug.WriteLine(
                        $"{If(success, "PASS", "FAIL")} | " &
                        $"{description} | " &
                        $"Expected={expected} | Actual={actual}")

                End Sub

            Dim rectangle As Func(Of Single, Single,
                                      Single, Single, Path2D) =
                Function(x, y, width, height)

                    Dim path As New Path2D

                    Dim p1 As New PointF(x, y)
                    Dim p2 As New PointF(x + width, y)
                    Dim p3 As New PointF(x + width, y + height)
                    Dim p4 As New PointF(x, y + height)

                    path.Add(New LineSegment2D(p1, p2))
                    path.Add(New LineSegment2D(p2, p3))
                    path.Add(New LineSegment2D(p3, p4))
                    path.Add(New LineSegment2D(p4, p1))

                    Return path

                End Function

            Dim circle As Func(Of Single, Single,
                                   Double, Path2D) =
                Function(x, y, radius)

                    Dim path As New Path2D

                    path.Add(
                        New ArcSegment2D(
                            New PointF(x, y),
                            radius,
                            0.0,
                            Math.PI * 2.0,
                            False))

                    Return path

                End Function

            Dim outerRectangle As Path2D =
                rectangle(0, 0, 10, 10)

            Dim outerCircle As Path2D =
                circle(5, 5, 5)

            Debug.WriteLine("")
            Debug.WriteLine("===== PROFILE CONTAINMENT TEST =====")

            ' --------------------------------------------------
            ' Rectangle / Rectangle
            ' --------------------------------------------------

            check("Rectangle inside rectangle",
                  rectangle(2, 2, 3, 3),
                  outerRectangle,
                  ProfileContainmentResult.Inside)

            check("Rectangle outside rectangle",
                  rectangle(12, 2, 3, 3),
                  outerRectangle,
                  ProfileContainmentResult.Outside)

            check("Rectangles crossing",
                  rectangle(8, 2, 4, 4),
                  outerRectangle,
                  ProfileContainmentResult.Intersecting)

            check("Rectangles sharing edge",
                  rectangle(2, 0, 3, 3),
                  outerRectangle,
                  ProfileContainmentResult.Intersecting)

            check("Rectangles sharing corner",
                  rectangle(10, 10, 2, 2),
                  outerRectangle,
                  ProfileContainmentResult.Intersecting)

            check("Identical rectangles",
                  rectangle(0, 0, 10, 10),
                  outerRectangle,
                  ProfileContainmentResult.Intersecting)

            ' --------------------------------------------------
            ' Circle / Rectangle
            ' --------------------------------------------------

            check("Circle inside rectangle",
                  circle(5, 5, 2),
                  outerRectangle,
                  ProfileContainmentResult.Inside)

            check("Circle outside rectangle",
                  circle(15, 5, 2),
                  outerRectangle,
                  ProfileContainmentResult.Outside)

            check("Circle crossing rectangle",
                  circle(9, 5, 2),
                  outerRectangle,
                  ProfileContainmentResult.Intersecting)

            check("Circle internally tangent to rectangle",
                  circle(2, 5, 2),
                  outerRectangle,
                  ProfileContainmentResult.Intersecting)

            check("Circle externally tangent to rectangle",
                  circle(12, 5, 2),
                  outerRectangle,
                  ProfileContainmentResult.Intersecting)

            ' --------------------------------------------------
            ' Rectangle / Circle
            ' --------------------------------------------------

            check("Rectangle inside circle",
                  rectangle(4, 4, 2, 2),
                  outerCircle,
                  ProfileContainmentResult.Inside)

            check("Rectangle outside circle",
                  rectangle(12, 12, 2, 2),
                  outerCircle,
                  ProfileContainmentResult.Outside)

            check("Rectangle crossing circle",
                  rectangle(8, 4, 4, 2),
                  outerCircle,
                  ProfileContainmentResult.Intersecting)

            ' --------------------------------------------------
            ' Circle / Circle
            ' --------------------------------------------------

            check("Concentric smaller circle",
                  circle(5, 5, 2),
                  outerCircle,
                  ProfileContainmentResult.Inside)

            check("Nonconcentric smaller circle",
                  circle(6, 5, 2),
                  outerCircle,
                  ProfileContainmentResult.Inside)

            check("Separated circles",
                  circle(15, 5, 2),
                  outerCircle,
                  ProfileContainmentResult.Outside)

            check("Crossing circles",
                  circle(9, 5, 3),
                  outerCircle,
                  ProfileContainmentResult.Intersecting)

            check("Internally tangent circles",
                  circle(8, 5, 2),
                  outerCircle,
                  ProfileContainmentResult.Intersecting)

            check("Externally tangent circles",
                  circle(12, 5, 2),
                  outerCircle,
                  ProfileContainmentResult.Intersecting)

            check("Coincident circles",
                  circle(5, 5, 5),
                  outerCircle,
                  ProfileContainmentResult.Intersecting)

            check("Inner candidate surrounds outer circle",
                  circle(5, 5, 8),
                  outerCircle,
                  ProfileContainmentResult.Outside)

            ' --------------------------------------------------
            ' Invalid paths
            ' --------------------------------------------------

            Dim openPath As New Path2D

            openPath.Add(
                New LineSegment2D(
                    New PointF(1, 1),
                    New PointF(3, 3)))

            check("Open inner path",
                  openPath,
                  outerRectangle,
                  ProfileContainmentResult.Invalid)

            check("Missing inner path",
                  Nothing,
                  outerRectangle,
                  ProfileContainmentResult.Invalid)

            check("Missing outer path",
                  outerRectangle,
                  Nothing,
                  ProfileContainmentResult.Invalid)


            ' --------------------------------------------------
            ' Mixed Line / Arc profiles
            ' --------------------------------------------------

            Dim mixedPath As New Path2D

            mixedPath.Add(
    New LineSegment2D(
        New PointF(0, 0),
        New PointF(6, 0)))

            mixedPath.Add(
    New ArcSegment2D(
        New PointF(6, 4),
        4.0,
        -Math.PI / 2.0,
        Math.PI / 2.0,
        False))

            mixedPath.Add(
    New LineSegment2D(
        New PointF(6, 8),
        New PointF(0, 8)))

            mixedPath.Add(
    New LineSegment2D(
        New PointF(0, 8),
        New PointF(0, 0)))

            check("Rectangle inside straight portion",
      rectangle(1, 2, 2, 2),
      mixedPath,
      ProfileContainmentResult.Inside)

            check("Rectangle inside curved portion",
      rectangle(7, 3, 1, 2),
      mixedPath,
      ProfileContainmentResult.Inside)

            check("Rectangle outside curved portion",
      rectangle(11, 0.5, 2, 1.5),
      mixedPath,
      ProfileContainmentResult.Outside)

            check("Rectangle crossing curved boundary",
      rectangle(9, 3.5, 2, 1),
      mixedPath,
      ProfileContainmentResult.Intersecting)

            check("Rectangle crossing lower arc quadrant",
      rectangle(8, 0.5, 2, 2),
      mixedPath,
      ProfileContainmentResult.Intersecting)

            check("Rectangle crossing upper arc quadrant",
      rectangle(8, 5.5, 2, 2),
      mixedPath,
      ProfileContainmentResult.Intersecting)

            check("Rectangle tangent to curved boundary",
      rectangle(10, 3, 2, 2),
      mixedPath,
      ProfileContainmentResult.Intersecting)

            check("Circle inside curved portion",
      circle(7, 4, 1),
      mixedPath,
      ProfileContainmentResult.Inside)

            check("Circle crossing curved boundary",
      circle(9, 4, 2),
      mixedPath,
      ProfileContainmentResult.Intersecting)

            check("Circle internally tangent to curved boundary",
      circle(8, 4, 2),
      mixedPath,
      ProfileContainmentResult.Intersecting)

            check("Circle externally tangent to curved boundary",
      circle(12, 4, 2),
      mixedPath,
      ProfileContainmentResult.Intersecting)

            check("Circle outside curved boundary",
      circle(12, 4, 1),
      mixedPath,
      ProfileContainmentResult.Outside)

            check("Circle coincident with partial arc",
      circle(6, 4, 4),
      mixedPath,
      ProfileContainmentResult.Intersecting)

            ' --------------------------------------------------
            ' Arc direction and angular wraparound
            ' --------------------------------------------------

            Dim wrapPath As New Path2D

            wrapPath.Add(
    New LineSegment2D(
        New PointF(0, 0),
        New PointF(6, 0)))

            wrapPath.Add(
    New ArcSegment2D(
        New PointF(6, 4),
        4.0,
        3.0 * Math.PI / 2.0,
        Math.PI / 2.0,
        False))

            wrapPath.Add(
    New LineSegment2D(
        New PointF(6, 8),
        New PointF(0, 8)))

            wrapPath.Add(
    New LineSegment2D(
        New PointF(0, 8),
        New PointF(0, 0)))

            check("CCW wraparound - inside",
      rectangle(7, 3, 1, 2),
      wrapPath,
      ProfileContainmentResult.Inside)

            check("CCW wraparound - lower crossing",
      rectangle(8, 0.5, 2, 2),
      wrapPath,
      ProfileContainmentResult.Intersecting)

            check("CCW wraparound - upper crossing",
      rectangle(8, 5.5, 2, 2),
      wrapPath,
      ProfileContainmentResult.Intersecting)

            check("CCW wraparound - outside",
      circle(12, 4, 1),
      wrapPath,
      ProfileContainmentResult.Outside)

            ' --------------------------------------------------
            ' Same physical profile, reversed clockwise
            ' --------------------------------------------------

            Dim cwPath As New Path2D

            cwPath.Add(
    New LineSegment2D(
        New PointF(0, 0),
        New PointF(0, 8)))

            cwPath.Add(
    New LineSegment2D(
        New PointF(0, 8),
        New PointF(6, 8)))

            cwPath.Add(
    New ArcSegment2D(
        New PointF(6, 4),
        4.0,
        Math.PI / 2.0,
        3.0 * Math.PI / 2.0,
        True))

            cwPath.Add(
    New LineSegment2D(
        New PointF(6, 0),
        New PointF(0, 0)))

            check("CW wraparound - inside",
      rectangle(7, 3, 1, 2),
      cwPath,
      ProfileContainmentResult.Inside)

            check("CW wraparound - lower crossing",
      rectangle(8, 0.5, 2, 2),
      cwPath,
      ProfileContainmentResult.Intersecting)

            check("CW wraparound - upper crossing",
      rectangle(8, 5.5, 2, 2),
      cwPath,
      ProfileContainmentResult.Intersecting)

            check("CW wraparound - outside",
      circle(12, 4, 1),
      cwPath,
      ProfileContainmentResult.Outside)

            check("CW wraparound - tangent circle",
      circle(12, 4, 2),
      cwPath,
      ProfileContainmentResult.Intersecting)

            check("Identical profiles with reversed direction",
      cwPath,
      mixedPath,
      ProfileContainmentResult.Intersecting)

            ' --------------------------------------------------
            ' Direct segment intersection tests
            ' --------------------------------------------------

            Dim checkSegments As Action(Of String,
                              IPathSegment,
                              IPathSegment,
                              Boolean) =
    Sub(description, segmentA, segmentB, expected)

        Dim actual As Boolean =
            SegmentsIntersect(
                segmentA,
                segmentB,
                0.000001)

        Dim success As Boolean =
            actual = expected

        If success Then
            passed += 1
        Else
            failed += 1
        End If

        Debug.WriteLine(
            $"{If(success, "PASS", "FAIL")} | " &
            $"{description} | " &
            $"Expected={expected} | Actual={actual}")

    End Sub

            Dim makeArc As Func(Of Double, Double,
                        Double, Boolean, ArcSegment2D) =
    Function(radius, startDegrees, endDegrees, clockwise)

        Return New ArcSegment2D(
            New PointF(5, 5),
            radius,
            startDegrees * Math.PI / 180.0,
            endDegrees * Math.PI / 180.0,
            clockwise)

    End Function

            checkSegments("Coincident partial arcs",
              makeArc(4, 0, 90, False),
              makeArc(4, 0, 90, False),
              True)

            checkSegments("Overlapping partial arcs",
              makeArc(4, 0, 90, False),
              makeArc(4, 45, 135, False),
              True)

            checkSegments("Partial arc contained in another arc",
              makeArc(4, 0, 180, False),
              makeArc(4, 45, 90, False),
              True)

            checkSegments("Partial arcs sharing endpoint",
              makeArc(4, 0, 90, False),
              makeArc(4, 90, 180, False),
              True)

            checkSegments("Disjoint arcs on same circle",
              makeArc(4, 0, 90, False),
              makeArc(4, 180, 270, False),
              False)

            checkSegments("Concentric arcs with different radii",
              makeArc(4, 0, 90, False),
              makeArc(3, 0, 90, False),
              False)

            checkSegments("Overlapping arcs across zero degrees",
              makeArc(4, 350, 30, False),
              makeArc(4, 10, 60, False),
              True)

            checkSegments("Coincident arcs with opposite directions",
              makeArc(4, 350, 30, False),
              makeArc(4, 30, 350, True),
              True)

            checkSegments("Disjoint wraparound arcs",
              makeArc(4, 350, 30, False),
              makeArc(4, 90, 180, False),
              False)

            checkSegments("Arcs sharing zero-degree endpoint",
              makeArc(4, 270, 360, False),
              makeArc(4, 0, 90, False),
              True)


            Debug.WriteLine("------------------------------------")
            Debug.WriteLine($"Passed: {passed}")
            Debug.WriteLine($"Failed: {failed}")
            Debug.WriteLine("====================================")

        End Sub



    End Class


#End Region

End Namespace
