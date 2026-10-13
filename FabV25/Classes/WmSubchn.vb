Imports FabV25_WIN8.ToolShapes

Public Class WmSubchn
    Inherits WmNode

    Private _owner As WmChain
    Private _prev As WmSubchn
    Private _next As WmSubchn
    Private _iIndex As Integer
    Private _intersection As WmIntersectionRecord

    Public Sub New(owner As WmChain)
        MyBase.New()

        _owner = owner
        _prev = Nothing
        _next = Nothing
        _iIndex = Integer.MinValue
        _intersection = Nothing
    End Sub

    Public Property Owner As WmChain
        Get
            Return _owner
        End Get
        Set(value As WmChain)
            _owner = value
        End Set
    End Property

    Public Property IIndex As Integer
        Get
            Return _iIndex
        End Get
        Set(value As Integer)
            _iIndex = value
        End Set
    End Property

    Public ReadOnly Property IsAssigned As Boolean
        Get
            Return _iIndex > Integer.MinValue
        End Get
    End Property

    Public ReadOnly Property Count As Integer
        Get
            If _intersection Is Nothing Then
                Return 0
            End If

            Return _intersection.Count
        End Get
    End Property

    Public ReadOnly Property IsTerminal As Boolean
        Get
            Return _next Is Nothing
        End Get
    End Property

    Friend Property Intersection As WmIntersectionRecord
        Get
            Return _intersection
        End Get
        Set(value As WmIntersectionRecord)
            _intersection = value
        End Set
    End Property

    Public ReadOnly Property SubchnPrev As WmSubchn
        Get
            Return _prev
        End Get
    End Property

    Public ReadOnly Property SubchnNext As WmSubchn
        Get
            Return _next
        End Get
    End Property

    Public Sub SetSubchnPrev(node As WmSubchn)
        _prev = node
    End Sub

    Public Sub SetSubchnNext(node As WmSubchn)
        _next = node
    End Sub

    Public ReadOnly Property First As WmElem
        Get
            Return TryCast(Me.Next, WmElem)
        End Get
    End Property

    Public ReadOnly Property Last As WmElem
        Get
            If _next Is Nothing Then
                Return Nothing
            End If

            Return TryCast(_next.Prev, WmElem)
        End Get
    End Property

    Public ReadOnly Property StartPoint As PointF
        Get
            Dim element As WmElem = First

            If element Is Nothing OrElse
               element.Element Is Nothing Then

                Return PointF.Empty
            End If

            Return element.Element.StartPoint
        End Get
    End Property

    Public ReadOnly Property EndPoint As PointF
        Get
            Dim element As WmElem = Last

            If element Is Nothing OrElse
               element.Element Is Nothing Then

                Return PointF.Empty
            End If

            Return element.Element.EndPoint
        End Get
    End Property

    '==========================================================
    ' Find the other subchain at this intersection having the
    ' same interference index as this subchain.
    '
    ' Corresponds to:
    '     CWmSubchn::Other()
    '==========================================================
    Public Function Other() As WmSubchn
        Return Other(_iIndex)
    End Function

    '==========================================================
    ' Find the other subchain at this intersection having the
    ' specified interference index.
    '
    ' Corresponds to:
    '     CWmSubchn::Other(int)
    '==========================================================
    Public Function Other(iIndex As Integer) As WmSubchn

        If _intersection Is Nothing Then
            Return Nothing
        End If

        If Not _intersection.IsValid() Then
            Return Nothing
        End If

        Dim count As Integer = _intersection.Count

        For index As Integer = 0 To count - 1

            Dim extData As WmCurveRecordExtData =
                _intersection.ExtData(index)

            If extData Is Nothing Then
                Continue For
            End If

            Dim subchn As WmSubchn =
                TryCast(extData.SubChain, WmSubchn)

            If subchn IsNot Nothing AndAlso
               Not Object.ReferenceEquals(subchn, Me) AndAlso
               subchn.IIndex = iIndex Then

                Return subchn
            End If
        Next

        Return Nothing
    End Function

    '==========================================================
    ' Return the other subchain when this intersection contains
    ' exactly two subchains.
    '
    ' Corresponds to:
    '     CWmSubchn::Sister()
    '==========================================================
    Public Function Sister() As WmSubchn

        If _intersection Is Nothing Then
            Return Nothing
        End If

        Dim count As Integer = _intersection.Count

        If count <> 2 Then
            Return Nothing
        End If

        For index As Integer = 0 To count - 1

            Dim extData As WmCurveRecordExtData =
                _intersection.ExtData(index)

            If extData Is Nothing Then
                Continue For
            End If

            Dim subchn As WmSubchn =
                TryCast(extData.SubChain, WmSubchn)

            If subchn IsNot Nothing AndAlso
               Not Object.ReferenceEquals(subchn, Me) Then

                Return subchn
            End If
        Next

        Return Nothing
    End Function

    Public Sub OwnerShipUpdate()
        Dim element As WmElem =
            TryCast(Me.Next, WmElem)

        While element IsNot Nothing

            element.Owner = Me

            element =
                TryCast(element.Next, WmElem)
        End While
    End Sub

    Public Overrides Sub Unlink()

        If _prev IsNot Nothing Then
            _prev._next = _next
        End If

        If _next IsNot Nothing Then
            _next._prev = _prev
        End If

        _prev = Nothing
        _next = Nothing

        MyBase.Unlink()
    End Sub

    Public Sub Purge()
        _iIndex = Integer.MinValue
        _intersection = Nothing
    End Sub

End Class