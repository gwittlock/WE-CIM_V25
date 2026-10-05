Imports System.Drawing
Imports System.Linq
Imports FabV25_WIN8.ToolShapes

Namespace ToolShapes

    Public MustInherit Class TrapezoidToolShape
        Inherits ToolShape

        Public Sub New(
            length As Double,
            width As Double,
            angle As Double)

            MyBase.New(
                ToolShapeType.Trapezoid,
                CreateTrapezoidProfile(
                    length,
                    width,
                    angle))

        End Sub

        Private Shared Function CreateTrapezoidProfile(
            length As Double,
            width As Double,
            angle As Double) As IPath2D

            If length <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(length))
            End If

            If width <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(width))
            End If

            If angle <= 0 OrElse angle >= 90.0 Then
                Throw New ArgumentOutOfRangeException(
                    NameOf(angle),
                    "Angle must be greater than 0 and less than 90 degrees.")
            End If

            Dim angleRadians As Double =
                angle * Math.PI / 180.0

            Dim inset As Double =
                width / Math.Tan(angleRadians)

            Dim topLength As Double =
                length - (2.0 * inset)

            If topLength <= 0 Then
                Throw New ArgumentException(
                    "Length, Width, and Angle produce an invalid trapezoid.",
                    NameOf(angle))
            End If

            Dim halfLength As Double =
                length / 2.0

            Dim halfTopLength As Double =
                topLength / 2.0

            Dim halfWidth As Double =
                width / 2.0

            Dim bottomLeft As New PointF(
                CSng(-halfLength),
                CSng(-halfWidth))

            Dim bottomRight As New PointF(
                CSng(halfLength),
                CSng(-halfWidth))

            Dim topRight As New PointF(
                CSng(halfTopLength),
                CSng(halfWidth))

            Dim topLeft As New PointF(
                CSng(-halfTopLength),
                CSng(halfWidth))

            Dim path As New Path2D()

            path.Add(
                New LineSegment2D(
                    bottomLeft,
                    bottomRight))

            path.Add(
                New LineSegment2D(
                    bottomRight,
                    topRight))

            path.Add(
                New LineSegment2D(
                    topRight,
                    topLeft))

            path.Add(
                New LineSegment2D(
                    topLeft,
                    bottomLeft))

            Return path

        End Function

        Public ReadOnly Property Length As Double
            Get
                Return CuttingProfile.BoundingBox.Width
            End Get
        End Property

        Public ReadOnly Property Width As Double
            Get
                Return CuttingProfile.BoundingBox.Height
            End Get
        End Property

        Public ReadOnly Property Angle As Double
            Get
                Dim slopedLine As LineSegment2D =
            CuttingProfile.Segments.
            OfType(Of LineSegment2D)().
            FirstOrDefault(Function(s) Math.Abs(s.EndPoint.X - s.StartPoint.X) > 0.0 AndAlso Math.Abs(s.EndPoint.Y - s.StartPoint.Y) > 0.0)

                If slopedLine Is Nothing Then
                        Return 0.0
                    End If

                    Dim dx As Double =
            Math.Abs(
                slopedLine.EndPoint.X -
                slopedLine.StartPoint.X)

                    Dim dy As Double =
            Math.Abs(
                slopedLine.EndPoint.Y -
                slopedLine.StartPoint.Y)

                    Return Math.Atan2(dy, dx) *
               180.0 / Math.PI
    End Get
        End Property

        Public Overrides ReadOnly Property EffectiveDiameter As Double
            Get
                Return Math.Min(Length, Width)
            End Get
        End Property

        Public Overrides ReadOnly Property EffectiveRadius As Double
            Get
                Return EffectiveDiameter / 2.0
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