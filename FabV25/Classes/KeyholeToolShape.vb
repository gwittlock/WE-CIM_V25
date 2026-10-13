Imports System.Drawing
Imports System.Linq
Imports FabV25_WIN8.ToolShapes

Namespace ToolShapes

    Public Class KeyholeToolShape
        Inherits ToolShape

        Public Sub New(
            length As Double,
            majorDiameter As Double,
            minorDiameter As Double)

            MyBase.New(
                ToolShapeType.Keyhole,
                CreateKeyholeProfile(
                    length,
                    majorDiameter,
                    minorDiameter))
        End Sub

        Private Shared Function CreateKeyholeProfile(
    length As Double,
    majorDiameter As Double,
    minorDiameter As Double) As IPath2D

            If length <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(length))
            End If

            If majorDiameter <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(majorDiameter))
            End If

            If minorDiameter <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(minorDiameter))
            End If

            If minorDiameter > majorDiameter Then
                Throw New ArgumentException(
            "Minor diameter cannot exceed the major diameter.",
            NameOf(minorDiameter))
            End If

            Dim majorRadius As Double =
        majorDiameter / 2.0

            Dim minorRadius As Double =
        minorDiameter / 2.0

            Dim xMin As Double =
        -length / 2.0

            Dim xMax As Double =
        length / 2.0

            ' Large circular end is on the left.
            Dim majorCenterX As Double =
        xMin + majorRadius

            ' Small circular end is on the right.
            Dim minorCenterX As Double =
        xMax - minorRadius

            ' The two circular ends must not overlap.
            If minorCenterX <= majorCenterX Then
                Throw New ArgumentException(
            "Length is too short for the specified major and minor diameters.",
            NameOf(length))
            End If

            ' Intersection of the connecting lines with
            ' the major circular end.
            Dim majorHalfChord As Double =
        Math.Sqrt(
            Math.Max(
                0.0,
                majorRadius * majorRadius -
                minorRadius * minorRadius))

            Dim majorAngle As Double =
        Math.Asin(minorRadius / majorRadius)

            Dim majorTop As New PointF(
        CSng(majorCenterX + majorHalfChord),
        CSng(minorRadius))

            Dim majorBottom As New PointF(
        CSng(majorCenterX + majorHalfChord),
        CSng(-minorRadius))

            Dim minorTop As New PointF(
        CSng(minorCenterX),
        CSng(minorRadius))

            Dim minorBottom As New PointF(
        CSng(minorCenterX),
        CSng(-minorRadius))

            Dim majorCenter As New PointF(
        CSng(majorCenterX),
        0.0F)

            Dim minorCenter As New PointF(
        CSng(minorCenterX),
        0.0F)

            Dim path As New Path2D()

            ' Top connecting line.
            path.Add(
        New LineSegment2D(
            majorTop,
            minorTop))

            ' Small circular end.
            path.Add(
        New ArcSegment2D(
            minorCenter,
            minorRadius,
            Math.PI / 2.0,
            -Math.PI / 2.0,
            False))

            ' Bottom connecting line.
            path.Add(
        New LineSegment2D(
            minorBottom,
            majorBottom))

            ' Large circular end.
            ' Travel around the outside/left side
            ' of the major circle.
            path.Add(
    New ArcSegment2D(
        majorCenter,
        majorRadius,
        -majorAngle,
        -2.0 * Math.PI + majorAngle,
        False))

            Return path

        End Function

        Public ReadOnly Property Length As Double
            Get
                Return CuttingProfile.BoundingBox.Width
            End Get
        End Property

        Public ReadOnly Property MajorDiameter As Double
            Get
                Dim arcs =
                    CuttingProfile.Segments.
                    OfType(Of ArcSegment2D)().
                    ToList()

                If arcs.Count = 0 Then Return 0.0

                Return arcs.Max(
                    Function(a) a.Radius) * 2.0
            End Get
        End Property

        Public ReadOnly Property MinorDiameter As Double
            Get
                Dim arcs =
                    CuttingProfile.Segments.
                    OfType(Of ArcSegment2D)().
                    ToList()

                If arcs.Count = 0 Then Return 0.0

                Return arcs.Min(
                    Function(a) a.Radius) * 2.0
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveDiameter As Double
            Get
                Return MinorDiameter
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveRadius As Double
            Get
                Return MinorDiameter / 2.0
            End Get
        End Property

        Public Overrides Function IsCompatibleWithFeature(
            feature As IGeometricFeature) As Boolean

            If feature Is Nothing Then Return False

            If feature.MinSlotWidth < MinorDiameter Then
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

            Return width >= MinorDiameter
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