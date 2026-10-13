Imports System.Drawing
Imports System.Linq
Imports FabV25_WIN8.ToolShapes

Namespace ToolShapes

    Public Class TrapezoidToolShape
        Inherits ToolShape

        Public Sub New(
    length As Double,
    width As Double,
    angle As Double,
    cornerRadius As Double)

            MyBase.New(
        ToolShapeType.Trapezoid,
        CreateTrapezoidProfile(
            length,
            width,
            angle,
            cornerRadius))

        End Sub

        Private Shared Function CreateTrapezoidProfile(
    length As Double,
    width As Double,
    angle As Double,
    cornerRadius As Double) As IPath2D

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

            If cornerRadius < 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(cornerRadius))
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

            Dim vertices(3) As PointF

            vertices(0) = New PointF(
        CSng(-halfLength),
        CSng(-halfWidth))

            vertices(1) = New PointF(
        CSng(halfLength),
        CSng(-halfWidth))

            vertices(2) = New PointF(
        CSng(halfTopLength),
        CSng(halfWidth))

            vertices(3) = New PointF(
        CSng(-halfTopLength),
        CSng(halfWidth))

            Dim path As New Path2D()

            ' Preserve the original sharp trapezoid.
            If cornerRadius = 0 Then

                path.Add(
            New LineSegment2D(
                vertices(0),
                vertices(1)))

                path.Add(
            New LineSegment2D(
                vertices(1),
                vertices(2)))

                path.Add(
            New LineSegment2D(
                vertices(2),
                vertices(3)))

                path.Add(
            New LineSegment2D(
                vertices(3),
                vertices(0)))

                Return path

            End If

            Dim tangentBefore(3) As PointF
            Dim tangentAfter(3) As PointF
            Dim arcCenters(3) As PointF

            For i As Integer = 0 To 3

                Dim prevIndex As Integer = i - 1

                If prevIndex < 0 Then
                    prevIndex = 3
                End If

                Dim nextIndex As Integer = i + 1

                If nextIndex > 3 Then
                    nextIndex = 0
                End If

                Dim vx As Double =
            vertices(i).X

                Dim vy As Double =
            vertices(i).Y

                ' Vector from vertex toward previous vertex.
                Dim v1x As Double =
            vertices(prevIndex).X - vx

                Dim v1y As Double =
            vertices(prevIndex).Y - vy

                ' Vector from vertex toward next vertex.
                Dim v2x As Double =
            vertices(nextIndex).X - vx

                Dim v2y As Double =
            vertices(nextIndex).Y - vy

                Dim len1 As Double =
            Math.Sqrt(
                v1x * v1x +
                v1y * v1y)

                Dim len2 As Double =
            Math.Sqrt(
                v2x * v2x +
                v2y * v2y)

                v1x /= len1
                v1y /= len1

                v2x /= len2
                v2y /= len2

                Dim dot As Double =
            v1x * v2x +
            v1y * v2y

                dot =
            Math.Max(
                -1.0,
                Math.Min(
                    1.0,
                    dot))

                Dim cornerAngle As Double =
            Math.Acos(dot)

                Dim tangentDistance As Double =
            cornerRadius /
            Math.Tan(cornerAngle / 2.0)

                If tangentDistance * 2.0 >=
            Math.Min(len1, len2) Then

                    Throw New ArgumentOutOfRangeException(
                NameOf(cornerRadius),
                "Corner radius is too large for the trapezoid dimensions.")
                End If

                tangentBefore(i) =
            New PointF(
                CSng(
                    vx +
                    v1x * tangentDistance),
                CSng(
                    vy +
                    v1y * tangentDistance))

                tangentAfter(i) =
            New PointF(
                CSng(
                    vx +
                    v2x * tangentDistance),
                CSng(
                    vy +
                    v2y * tangentDistance))

                ' Internal angle bisector.
                Dim bx As Double =
            v1x + v2x

                Dim by As Double =
            v1y + v2y

                Dim bisectorLength As Double =
            Math.Sqrt(
                bx * bx +
                by * by)

                bx /= bisectorLength
                by /= bisectorLength

                Dim centerDistance As Double =
            cornerRadius /
            Math.Sin(cornerAngle / 2.0)

                arcCenters(i) =
            New PointF(
                CSng(
                    vx +
                    bx * centerDistance),
                CSng(
                    vy +
                    by * centerDistance))

            Next

            ' Build the rounded trapezoid.
            For i As Integer = 0 To 3

                Dim previousCorner As Integer =
            i - 1

                If previousCorner < 0 Then
                    previousCorner = 3
                End If

                ' Straight section between two corner fillets.
                path.Add(
            New LineSegment2D(
                tangentAfter(previousCorner),
                tangentBefore(i)))

                Dim center As PointF =
            arcCenters(i)

                Dim startAngle As Double =
            Math.Atan2(
                tangentBefore(i).Y - center.Y,
                tangentBefore(i).X - center.X)

                Dim endAngle As Double =
            Math.Atan2(
                tangentAfter(i).Y - center.Y,
                tangentAfter(i).X - center.X)

                Dim delta As Double =
            endAngle - startAngle

                ' Force the minor fillet arc.
                While delta > Math.PI
                    delta -= 2.0 * Math.PI
                End While

                While delta < -Math.PI
                    delta += 2.0 * Math.PI
                End While

                endAngle =
            startAngle + delta

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