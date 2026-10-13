Imports FabV25_WIN8.ToolShapes

Public Class WmChain

    Private _head As WmSubchn
    Private _tail As WmSubchn
    Private _setNo As Integer

    Public Sub New()
        _head = Nothing
        _tail = Nothing
        _setNo = 0
        CondInit()
    End Sub

    Public ReadOnly Property First As WmSubchn
        Get
            Return _head
        End Get
    End Property

    Public ReadOnly Property Last As WmSubchn
        Get
            Return _tail
        End Get
    End Property

    Public Property SetNo As Integer
        Get
            Return _setNo
        End Get
        Set(value As Integer)
            _setNo = value
        End Set
    End Property

    Private Sub CondInit()

        If _head IsNot Nothing Then Return

        _head = New WmSubchn(Me)
        _tail = New WmSubchn(Me)

        _head.SetSubchnNext(_tail)
        _tail.SetSubchnPrev(_head)

        _head.InsertAfter(_tail)

    End Sub

    Public Sub Append(element As IPathSegment)

        If element Is Nothing Then
            Throw New ArgumentNullException(NameOf(element))
        End If

        Dim owner As WmSubchn = _tail.SubchnPrev
        Dim wmElem As New WmElem(owner, element)

        _tail.InsertBefore(wmElem)

    End Sub

    Public Sub Prepend(element As IPathSegment)

        If element Is Nothing Then
            Throw New ArgumentNullException(NameOf(element))
        End If

        Dim wmElem As New WmElem(_head, element)

        _head.InsertAfter(wmElem)

    End Sub

    Public ReadOnly Property Count As Integer
        Get

            Dim countValue As Integer = 0
            Dim node As WmNode = _head.Next

            While node IsNot Nothing AndAlso
                  Not Object.ReferenceEquals(node, _tail)

                If TypeOf node Is WmElem Then
                    countValue += 1
                End If

                node = node.Next

            End While

            Return countValue

        End Get
    End Property

    Public ReadOnly Property StartPoint As PointF
        Get

            Dim element As WmElem = _head.First

            If element Is Nothing OrElse
               element.Element Is Nothing Then

                Return PointF.Empty
            End If

            Return element.Element.StartPoint

        End Get
    End Property

    Public ReadOnly Property EndPoint As PointF
        Get

            Dim element As WmElem = _tail.Prev

            If element Is Nothing OrElse
               Not TypeOf element Is WmElem Then

                Return PointF.Empty
            End If

            Return DirectCast(element, WmElem).Element.EndPoint

        End Get
    End Property

    Public ReadOnly Property IsClosed As Boolean
        Get

            If Count = 0 Then Return False

            Dim dx As Double =
                CDbl(EndPoint.X) - CDbl(StartPoint.X)

            Dim dy As Double =
                CDbl(EndPoint.Y) - CDbl(StartPoint.Y)

            Return Math.Sqrt((dx * dx) + (dy * dy)) < 0.000001

        End Get
    End Property

    Public Sub PurgeSubchns()

        Dim current As WmSubchn = _head.SubchnNext

        While current IsNot Nothing AndAlso
              Not Object.ReferenceEquals(current, _tail)

            Dim nextSubchn As WmSubchn =
                current.SubchnNext

            Dim node As WmNode = current.Next

            While node IsNot Nothing AndAlso
                  Not TypeOf node Is WmSubchn

                Dim element As WmElem = TryCast(node, WmElem)

                If element IsNot Nothing Then
                    element.Owner = _head
                End If

                node = node.Next

            End While

            current.Unlink()

            current = nextSubchn

        End While

        _setNo = 0

    End Sub

    Public Function ElementAt(index As Integer) As WmElem

        If index < 0 Then Return Nothing

        Dim currentIndex As Integer = 0
        Dim node As WmNode = _head.Next

        While node IsNot Nothing AndAlso
              Not Object.ReferenceEquals(node, _tail)

            Dim element As WmElem = TryCast(node, WmElem)

            If element IsNot Nothing Then

                If currentIndex = index Then
                    Return element
                End If

                currentIndex += 1

            End If

            node = node.Next

        End While

        Return Nothing

    End Function

End Class