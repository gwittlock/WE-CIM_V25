Imports System.Drawing
Imports System.Linq
Imports FabV25_WIN8.ToolShapes

Namespace ToolShapes

    Public Class HexagonToolShape
        Inherits ToolShape

        Public Sub New(
            diameter As Double)

            MyBase.New(
                ToolShapeType.Hexagon,
                CreateHexagonProfile(diameter))
        End Sub

        Private Shared Function CreateHexagonProfile(
            diameter As Double) As IPath2D

            If diameter <= 0 Then
                Throw New ArgumentOutOfRangeException(NameOf(diameter))
            End If

            Dim radius As Double =
                diameter / 2.0

            Dim path As New Path2D()

            Dim points As New List(Of PointF)

            For i As Integer = 0 To 5

                Dim angle As Double =
                    i * Math.PI / 3.0

                points.Add(
                    New PointF(
                        CSng(radius * Math.Cos(angle)),
                        CSng(radius * Math.Sin(angle))))
            Next

            For i As Integer = 0 To 5

                Dim startPoint As PointF =
                    points(i)

                Dim endPoint As PointF =
                    points((i + 1) Mod 6)

                path.Add(
                    New LineSegment2D(
                        startPoint,
                        endPoint))
            Next

            Return path
        End Function

        Public ReadOnly Property Diameter As Double
            Get
                Return CuttingProfile.BoundingBox.Width
            End Get
        End Property

        Public ReadOnly Property SideLength As Double
            Get
                Dim side As LineSegment2D =
                    CuttingProfile.Segments.
                    OfType(Of LineSegment2D)().
                    FirstOrDefault()

                If side Is Nothing Then
                    Return 0.0
                End If

                Return side.Length
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