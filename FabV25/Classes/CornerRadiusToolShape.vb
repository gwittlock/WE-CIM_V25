Imports System.Drawing
Imports System.Linq
Imports FabV25_WIN8.ToolShapes

Namespace ToolShapes

    Public Class CornerRadiusToolShape
        Inherits ToolShape
        Private ReadOnly _radius As Double
        Private ReadOnly _web As Double
        Private ReadOnly _width As Double
        Public Sub New(
            radius As Double,
            web As Double,
            width As Double)

            MyBase.New(
                ToolShapeType.CornerRadius,
                CreateCornerRadiusProfile(
                    radius,
                    web,
                    width))
        End Sub

        Private Shared Function CreateCornerRadiusProfile(
            radius As Double,
            web As Double,
            width As Double) As IPath2D

            If radius <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(radius))
            End If

            If web <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(web))
            End If

            If width <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(width))
            End If

            If web >= width Then
                Throw New ArgumentException(
                    "Web must be less than the overall width.",
                    NameOf(web))
            End If

            Dim halfWidth As Double =
                width / 2.0

            Dim halfWeb As Double =
                web / 2.0

            ' The radius must fit between the
            ' inside edge of one leg and the
            ' centerline of the opposite geometry.
            Dim available As Double =
                halfWidth - halfWeb

            If radius > available Then
                Throw New ArgumentException(
                    "Radius is too large for the specified Web and Width.",
                    NameOf(radius))
            End If

            Dim path As New Path2D()

            ' -------------------------------------------------
            ' Start at the top-left outer corner.
            ' Travel clockwise around the plus profile.
            ' -------------------------------------------------

            Dim p1 As New PointF(
                CSng(-halfWeb),
                CSng(halfWidth))

            Dim p2 As New PointF(
                CSng(halfWeb),
                CSng(halfWidth))

            Dim p3 As New PointF(
                CSng(halfWeb),
                CSng(halfWeb + radius))

            path.Add(
                New LineSegment2D(
                    p1,
                    p2))

            path.Add(
                New LineSegment2D(
                    p2,
                    p3))

            ' Top-right inside radius.
            Dim center1 As New PointF(
                CSng(halfWeb + radius),
                CSng(halfWeb + radius))

            path.Add(
                New ArcSegment2D(
                    center1,
                    radius,
                    Math.PI,
                    3.0 * Math.PI / 2.0,
                    False))

            Dim p4 As New PointF(
                CSng(halfWeb + radius),
                CSng(halfWeb))

            Dim p5 As New PointF(
                CSng(halfWidth),
                CSng(halfWeb))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(halfWeb + radius),
                        CSng(halfWeb)),
                    p5))

            path.Add(
                New LineSegment2D(
                    p5,
                    New PointF(
                        CSng(halfWidth),
                        CSng(-halfWeb))))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(halfWidth),
                        CSng(-halfWeb)),
                    New PointF(
                        CSng(halfWeb + radius),
                        CSng(-halfWeb))))

            ' Bottom-right inside radius.
            Dim center2 As New PointF(
                CSng(halfWeb + radius),
                CSng(-halfWeb - radius))

            path.Add(
                New ArcSegment2D(
                    center2,
                    radius,
                    Math.PI / 2.0,
                    Math.PI,
                    False))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(halfWeb),
                        CSng(-halfWeb - radius)),
                    New PointF(
                        CSng(halfWeb),
                        CSng(-halfWidth))))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(halfWeb),
                        CSng(-halfWidth)),
                    New PointF(
                        CSng(-halfWeb),
                        CSng(-halfWidth))))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(-halfWeb),
                        CSng(-halfWidth)),
                    New PointF(
                        CSng(-halfWeb),
                        CSng(-halfWeb - radius))))

            ' Bottom-left inside radius.
            Dim center3 As New PointF(
                CSng(-halfWeb - radius),
                CSng(-halfWeb - radius))

            path.Add(
                New ArcSegment2D(
                    center3,
                    radius,
                    0.0,
                    Math.PI / 2.0,
                    False))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(-halfWeb - radius),
                        CSng(-halfWeb)),
                    New PointF(
                        CSng(-halfWidth),
                        CSng(-halfWeb))))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(-halfWidth),
                        CSng(-halfWeb)),
                    New PointF(
                        CSng(-halfWidth),
                        CSng(halfWeb))))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(-halfWidth),
                        CSng(halfWeb)),
                    New PointF(
                        CSng(-halfWeb - radius),
                        CSng(halfWeb))))

            ' Top-left inside radius.
            Dim center4 As New PointF(
                CSng(-halfWeb - radius),
                CSng(halfWeb + radius))

            path.Add(
                New ArcSegment2D(
                    center4,
                    radius,
                    -Math.PI / 2.0,
                    0.0,
                    False))

            path.Add(
                New LineSegment2D(
                    New PointF(
                        CSng(-halfWeb),
                        CSng(halfWeb + radius)),
                    p1))

            Return path

        End Function

        Public ReadOnly Property Radius As Double
            Get
                Dim arc As ArcSegment2D =
                    CuttingProfile.Segments.
                    OfType(Of ArcSegment2D)().
                    FirstOrDefault()

                If arc Is Nothing Then
                    Return 0.0
                End If

                Return arc.Radius
            End Get
        End Property

        Public ReadOnly Property Web As Double
            Get
                Return CuttingProfile.BoundingBox.Width
            End Get
        End Property

        Public ReadOnly Property Width As Double
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
                Return Radius
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

            Return radius >= Me.Radius
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

End Namespace