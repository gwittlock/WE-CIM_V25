Public Class DxfDrawable

    Public Property GeometryType As CadEntity.EntityType
    ' Rectangle
    Public Property Width As Single
    Public Property Height As Single

    Public Enum DrawableTypeEnum
            Line
            Circle
        Arc
        PolyLine
        Rectangle
    End Enum

    ' Type of entity
    Public DrawableType As DrawableTypeEnum


    ' Line geometry
    Public StartPoint As PointF
    Public EndPoint As PointF


    ' Circle / Arc geometry
    Public Center As PointF
    Public Radius As Single

    Public StartAngle As Single
    Public EndAngle As Single
    Public IsClockwise As Boolean

    ' Color for drawing
    Public Color As Color

    Public LayerName
End Class
