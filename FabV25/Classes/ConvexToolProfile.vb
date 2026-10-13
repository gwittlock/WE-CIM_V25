Imports System.Drawing
Imports System.Linq

Namespace ToolShapes

    Friend Class ConvexToolProfile

        Public ReadOnly Property Path As IPath2D

        Public ReadOnly Property IsClosed As Boolean

        Public ReadOnly Property Segments As IReadOnlyList(Of IPathSegment)

        Public ReadOnly Property Arcs As IReadOnlyList(Of ConvexHullArc)

        Public Sub New(path As IPath2D)

            If path Is Nothing Then
                Throw New ArgumentNullException(NameOf(path))
            End If

            Me.Path = path
            Me.IsClosed = path.IsClosed

            If Not IsClosed Then
                Throw New ArgumentException(
                    "Convex tool profile must be closed.",
                    NameOf(path))
            End If

            Me.Segments =
                path.Segments.ToList().AsReadOnly()

            Dim converted As New List(Of ConvexHullArc)

            For Each segment As IPathSegment In Me.Segments
                converted.Add(
                    ConvexHullGeometry.ConvertSegment(segment))
            Next

            Me.Arcs =
                converted.AsReadOnly()

        End Sub

        Public Function IsConvex() As Boolean

            If Segments.Count < 3 Then
                Return False
            End If

            Dim sign As Integer = 0

            For i As Integer = 0 To Segments.Count - 1

                Dim current As IPathSegment =
                    Segments(i)

                Dim nextSegment As IPathSegment =
                    Segments((i + 1) Mod Segments.Count)

                Dim v1x As Double =
                    nextSegment.StartPoint.X -
                    current.StartPoint.X

                Dim v1y As Double =
                    nextSegment.StartPoint.Y -
                    current.StartPoint.Y

                Dim v2x As Double =
                    nextSegment.EndPoint.X -
                    nextSegment.StartPoint.X

                Dim v2y As Double =
                    nextSegment.EndPoint.Y -
                    nextSegment.StartPoint.Y

                Dim cross As Double =
                    v1x * v2y -
                    v1y * v2x

                If Math.Abs(cross) < 0.0000001 Then
                    Continue For
                End If

                Dim currentSign As Integer =
                    Math.Sign(cross)

                If sign = 0 Then
                    sign = currentSign
                ElseIf currentSign <> sign Then
                    Return False
                End If

            Next

            Return sign <> 0

        End Function



    End Class

End Namespace