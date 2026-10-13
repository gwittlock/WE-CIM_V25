Public Class ViewHelper

    ' View history stacks
    Public Shared _viewHistory As New Stack(Of ViewTransform)
    Public Shared _redoHistory As New Stack(Of ViewTransform)

    Public Const MaxHistorySize As Integer = 20

    Public Class ViewState
        Public Property Zoom As Double
        Public Property PanX As Double
        Public Property PanY As Double
        Public Property ViewBounds As RectangleF
        Public Property Rotation As Double
    End Class









End Class
