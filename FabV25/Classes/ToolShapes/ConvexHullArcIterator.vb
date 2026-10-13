Namespace ToolShapes

    Friend Class ConvexHullArcIterator

        Private ReadOnly _arcs As IReadOnlyList(Of ConvexHullArc)
        Private _index As Integer


        Public Sub New(arcs As IReadOnlyList(Of ConvexHullArc))

            If arcs Is Nothing Then
                Throw New ArgumentNullException(NameOf(arcs))
            End If

            If arcs.Count = 0 Then
                Throw New ArgumentException(
            "Arc list cannot be empty.",
            NameOf(arcs))
            End If

            _arcs = arcs
            _index = 0

        End Sub

        Public ReadOnly Property Current As ConvexHullArc
            Get
                Return _arcs(_index)
            End Get
        End Property

        Public ReadOnly Property Position As Integer
            Get
                Return _index
            End Get
        End Property

        Public ReadOnly Property AtEnd As Boolean
            Get
                Return _index = _arcs.Count - 1
            End Get
        End Property

        Public Sub MoveNext()

            _index += 1

            If _index >= _arcs.Count Then
                _index = 0

            End If

        End Sub

        Public Sub MovePrevious()

            _index -= 1

            If _index < 0 Then
                _index = _arcs.Count - 1
            End If

        End Sub

        Public Sub Reset()

            _index = 0

        End Sub

        Public Function PeekNext() As ConvexHullArc

            Dim nextIndex As Integer = _index + 1

            If nextIndex >= _arcs.Count Then
                nextIndex = 0
            End If

            Return _arcs(nextIndex)

        End Function


        Public Function SavePosition() As Integer

            Return _index

        End Function

        Public Sub RestorePosition(
            position As Integer)

            If position < 0 OrElse
               position >= _arcs.Count Then

                Throw New ArgumentOutOfRangeException(
                    NameOf(position))

            End If

            _index = position

        End Sub
        Public Function Advance(
    partArc As ConvexHullArc,
    offsetDirection As Integer) As ArcOverlapClassification

            If partArc Is Nothing Then
                Throw New ArgumentNullException(NameOf(partArc))
            End If

            Dim index As Integer

            If partArc.ArcType = ConvexHullArcType.Move Then

                If offsetDirection > 0 Then
                    index = PreviousIndex(_index)
                Else
                    index = NextIndex(_index)
                End If

            Else

                If partArc.IsClockwise Then
                    index = PreviousIndex(_index)
                Else
                    index = NextIndex(_index)
                End If

            End If

            Dim classification As ArcOverlapClassification =
        ConvexHullGeometry.ClassifyArcOverlap(
            partArc,
            _arcs(index),
            offsetDirection)

            If classification <> ArcOverlapClassification.None Then
                _index = index
            End If

            Return classification

        End Function

        Public Function StartPosition(
            partIterator As ConvexHullArcIterator,
            offsetDirection As Integer) As ArcOverlapClassification

            If partIterator Is Nothing Then
                Throw New ArgumentNullException(NameOf(partIterator))
            End If

            Dim result As ArcOverlapClassification =
                ArcOverlapClassification.None

            Dim bestIndex As Integer = -1
            Dim bestClassification As ArcOverlapClassification =
                ArcOverlapClassification.None

            Dim atEnd As Boolean =
                partIterator.AtEnd

            Dim partArc As ConvexHullArc =
                partIterator.Current

            If partArc Is Nothing Then
                Return ArcOverlapClassification.None
            End If

            Dim index As Integer = _index

            Do
                Dim toolArc As ConvexHullArc =
                    _arcs(index)

                Dim classification As ArcOverlapClassification =
                    ConvexHullGeometry.ClassifyArcOverlap(
                        partArc,
                        toolArc,
                        offsetDirection)

                If classification = ArcOverlapClassification.None Then

                    If bestIndex >= 0 Then
                        _index = bestIndex
                        result = bestClassification
                        Exit Do
                    End If

                Else

                    bestIndex = index
                    bestClassification = classification

                End If

                If atEnd Then

                    If partArc.ArcType = ConvexHullArcType.Move Then

                        If offsetDirection > 0 Then
                            index = NextIndex(index)
                        Else
                            index = PreviousIndex(index)
                        End If

                    Else

                        If partArc.IsClockwise Then
                            index = PreviousIndex(index)
                        Else
                            index = NextIndex(index)
                        End If

                    End If

                Else

                    If partArc.ArcType = ConvexHullArcType.Move Then

                        If offsetDirection > 0 Then
                            index = PreviousIndex(index)
                        Else
                            index = NextIndex(index)
                        End If

                    Else

                        If partArc.IsClockwise Then
                            index = NextIndex(index)
                        Else
                            index = PreviousIndex(index)
                        End If

                    End If

                End If

            Loop

            Return result

        End Function

        Private Function NextIndex(index As Integer) As Integer
            Dim nextValue As Integer = index + 1

            If nextValue >= _arcs.Count Then
                nextValue = 0
            End If

            Return nextValue
        End Function

        Private Function PreviousIndex(index As Integer) As Integer
            Dim previousValue As Integer = index - 1

            If previousValue < 0 Then
                previousValue = _arcs.Count - 1
            End If

            Return previousValue
        End Function




    End Class

End Namespace