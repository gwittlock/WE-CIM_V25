Public Class WmNode

    Private _prev As WmNode
    Private _next As WmNode

    Public Sub New()
        _prev = Nothing
        _next = Nothing
    End Sub

    Public ReadOnly Property Prev As WmNode
        Get
            Return _prev
        End Get
    End Property

    Public ReadOnly Property [Next] As WmNode
        Get
            Return _next
        End Get
    End Property

    ' Equivalent to CWmNode::Prev(node)
    ' Inserts node immediately before this node.
    Public Function InsertBefore(node As WmNode) As WmNode

        If node Is Nothing Then
            Return _prev
        End If

        Dim currentPrevious As WmNode = _prev

        node._prev = currentPrevious
        node._next = Me

        If currentPrevious IsNot Nothing Then
            currentPrevious._next = node
        End If

        _prev = node

        Return currentPrevious
    End Function

    ' Equivalent to CWmNode::Next(node)
    ' Inserts node immediately after this node.
    Public Function InsertAfter(node As WmNode) As WmNode

        If node Is Nothing Then
            Return _next
        End If

        Dim currentNext As WmNode = _next

        node._prev = Me
        node._next = currentNext

        If currentNext IsNot Nothing Then
            currentNext._prev = node
        End If

        _next = node

        Return currentNext
    End Function

    ' Equivalent to CWmNode::Unlink()
    Public Overridable Sub Unlink()

        Dim currentPrevious As WmNode = _prev
        Dim currentNext As WmNode = _next

        If currentPrevious IsNot Nothing Then
            currentPrevious._next = currentNext
        End If

        If currentNext IsNot Nothing Then
            currentNext._prev = currentPrevious
        End If

        _prev = Nothing
        _next = Nothing

    End Sub

End Class
