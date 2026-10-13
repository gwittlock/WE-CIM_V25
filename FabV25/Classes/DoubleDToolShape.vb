Imports System.Drawing
Imports System.Linq
Imports FabV25_WIN8.ToolShapes

Namespace ToolShapes

    Public Class DoubleDToolShape
        Inherits ToolShape

        Public Sub New(
            diameter As Double,
            width As Double)

            MyBase.New(
                ToolShapeType.Double_D,
                CreateDoubleDProfile(
                    diameter,
                    width))

        End Sub

        Private Shared Function CreateDoubleDProfile(
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

            Dim radius As Double =
                diameter / 2.0

            ' Distance from the center to each flat.
            Dim flatDistance As Double =
                width / 2.0

            ' Half the length of each flat.
            Dim halfChord As Double =
                Math.Sqrt(
                    Math.Max(
                        0.0,
                        radius * radius -
                        flatDistance * flatDistance))

            ' Angle of the upper arc endpoint.
            Dim angle As Double =
                Math.Acos(flatDistance / radius)

            Dim rightTop As New PointF(
                CSng(flatDistance),
                CSng(halfChord))

            Dim rightBottom As New PointF(
                CSng(flatDistance),
                CSng(-halfChord))

            Dim leftTop As New PointF(
                CSng(-flatDistance),
                CSng(halfChord))

            Dim leftBottom As New PointF(
                CSng(-flatDistance),
                CSng(-halfChord))

            Dim path As New Path2D()

            ' Right vertical flat.
            path.Add(
                New LineSegment2D(
                    rightBottom,
                    rightTop))

            ' Major circular arc across the top.
            path.Add(
                New ArcSegment2D(
                    New PointF(0.0F, 0.0F),
                    radius,
                    angle,
                    Math.PI - angle,
                    False))

            ' Left vertical flat.
            path.Add(
                New LineSegment2D(
                    leftTop,
                    leftBottom))

            ' Major circular arc across the bottom.
            path.Add(
                New ArcSegment2D(
                    New PointF(0.0F, 0.0F),
                    radius,
                    Math.PI + angle,
                    (2.0 * Math.PI) - angle,
                    False))

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
                Return CuttingProfile.BoundingBox.Width
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