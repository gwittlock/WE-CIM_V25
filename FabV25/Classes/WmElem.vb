Imports FabV25_WIN8.ToolShapes

Public Class WmElem
    Inherits WmNode

    Private _owner As WmSubchn
    Private _element As IPathSegment

    Public Sub New(owner As WmSubchn, element As IPathSegment)
        MyBase.New()

        _owner = owner
        _element = element
    End Sub

    Public Property Owner As WmSubchn
        Get
            Return _owner
        End Get
        Set(value As WmSubchn)
            _owner = value
        End Set
    End Property

    Public Property Element As IPathSegment
        Get
            Return _element
        End Get
        Set(value As IPathSegment)
            _element = value
        End Set
    End Property

    Public Overrides Sub Unlink()
        _owner = Nothing
        MyBase.Unlink()
    End Sub

End Class