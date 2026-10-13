
Imports System.Drawing
Imports System.Linq


Namespace ToolShapes




    Public Class SingleDToolShape
        Inherits ToolShape

        Public Sub New(
        diameter As Double,
        width As Double)

            MyBase.New(
            ToolShapeType.Single_D,
            CreateSingleDProfile(
                diameter,
                width))

        End Sub

        Private Shared Function CreateSingleDProfile(
        diameter As Double,
        width As Double) As IPath2D

            If diameter <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(diameter))
            End If

            If width <= 0 OrElse width > diameter Then
                Throw New ArgumentOutOfRangeException(
                NameOf(width),
                "Width must be greater than zero and cannot exceed the tool diameter.")
            End If

            Dim radius As Double = diameter / 2.0

            ' Amount removed from the original diameter.
            Dim removed As Double =
            diameter - width

            ' Distance from the tool center to the flat.
            Dim flatDistance As Double =
            radius - removed

            If flatDistance < 0 OrElse flatDistance > radius Then
                Throw New ArgumentException(
                "Width produces an invalid Single-D profile.",
                NameOf(width))
            End If

            ' Half the length of the flat.
            Dim halfChord As Double =
            Math.Sqrt(
                Math.Max(
                    0.0,
                    radius * radius -
                    flatDistance * flatDistance))

            ' Angle of the upper flat endpoint.
            Dim angle As Double =
            Math.Acos(flatDistance / radius)

            Dim startPoint As New PointF(
            CSng(flatDistance),
            CSng(halfChord))

            Dim endPoint As New PointF(
            CSng(flatDistance),
            CSng(-halfChord))

            Dim path As New Path2D()

            ' Major circular arc.
            path.Add(
            New ArcSegment2D(
                New PointF(0.0F, 0.0F),
                radius,
                angle,
                (2.0 * Math.PI) - angle,
                False))

            ' Flat side.
            path.Add(
            New LineSegment2D(
                endPoint,
                startPoint))

            Return path

        End Function

        Public ReadOnly Property Diameter As Double
            Get
                Dim arc As ArcSegment2D =
                CuttingProfile.Segments.
                OfType(Of ArcSegment2D)().
                FirstOrDefault()

                If arc Is Nothing Then Return 0.0

                Return arc.Radius * 2.0
            End Get
        End Property

        Public ReadOnly Property Width As Double
            Get
                Dim box As RectangleF =
                CuttingProfile.BoundingBox

                Return box.Width
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

            If feature Is Nothing Then Return False

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
End Namespace
