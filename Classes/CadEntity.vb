' =============================
' CadEntity base class
' =============================
Public MustInherit Class CadEntity

    Public Property LayerName As String
    Public Property Color As Color

    ' Required for DXF → sheet normalization
    Public MustOverride Sub Translate(dx As Single, dy As Single)

    ' =============================
    ' Entity type enum
    ' =============================
    Public Enum EntityType
        Line
        Circle
        Arc
        Polyline
    End Enum

    Public Class CadLine
        Inherits CadEntity

        Public Property StartPoint As PointF
        Public Property EndPoint As PointF

        Public Overrides Sub Translate(dx As Single, dy As Single)
            StartPoint = New PointF(StartPoint.X + dx, StartPoint.Y + dy)
            EndPoint = New PointF(EndPoint.X + dx, EndPoint.Y + dy)
        End Sub
    End Class

    Public Class CadCircle
        Inherits CadEntity

        Public Property Center As PointF
        Public Property Radius As Single

        Public Overrides Sub Translate(dx As Single, dy As Single)
            Center = New PointF(Center.X + dx, Center.Y + dy)
        End Sub
    End Class

    Public Class CadArc
        Inherits CadEntity

        Public Property Center As PointF
        Public Property Radius As Single
        Public Property StartAngle As Single
        Public Property EndAngle As Single

        Public Overrides Sub Translate(dx As Single, dy As Single)
            Center = New PointF(Center.X + dx, Center.Y + dy)
        End Sub
    End Class

End Class
