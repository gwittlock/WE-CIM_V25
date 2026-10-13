Imports System.Drawing
Imports System.Linq

Namespace ToolShapes

    Friend Class ConvexPartProfile

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
                    "Part profile must be closed.",
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

    End Class

End Namespace