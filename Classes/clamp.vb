Public Class Clamp
    Public Property Name As String

    ' Sheet-relative center
    Public Property X As Double
    Public Property Y As Double

    ' Exclusion size
    Public Property Width As Double
    Public Property Height As Double

    Public ReadOnly Property ExclusionRect As RectangleF
        Get
            Return New RectangleF(
                CSng(X - Width / 2),
                CSng(Y - Height / 2),
                CSng(Width),
                CSng(Height))
        End Get
    End Property
End Class
