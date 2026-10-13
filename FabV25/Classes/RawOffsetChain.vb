Imports System.Collections.Generic

Namespace ToolShapes

    Friend Class RawOffsetChain

        Private ReadOnly _arcs As New List(Of ConvexHullArc)

        Public ReadOnly Property Arcs As IReadOnlyList(Of ConvexHullArc)
            Get
                Return _arcs.AsReadOnly()
            End Get
        End Property

        Public ReadOnly Property Count As Integer
            Get
                Return _arcs.Count
            End Get
        End Property

        Public Sub Add(arc As ConvexHullArc)

            If arc Is Nothing Then
                Throw New ArgumentNullException(NameOf(arc))
            End If

            _arcs.Add(arc)

        End Sub

    End Class

End Namespace