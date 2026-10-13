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
                False))

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
        False))
            Return path

        End Function

        Public ReadOnly Property Width As Double
            Get
                Return CuttingProfile.BoundingBox.Width
            End Get
        End Property

        Public ReadOnly Property Length As Double
            Get
                Return CuttingProfile.BoundingBox.Height
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

            Dim halfWidth As Double = width / 2.0
            Dim halfHeight As Double = height / 2.0

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
