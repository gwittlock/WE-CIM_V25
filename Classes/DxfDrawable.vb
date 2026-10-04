Public Class DxfDrawable
    Public Property Entity As CadEntity      ' Link to original CAD entity
    Public Property GeometryType As CadEntity.EntityType
    Public Property IsSelected As Boolean = False

    ' Rectangle
    Public Property Width As Single
    Public Property Height As Single


    Public Property IsHovered As Boolean = False

    Public Enum DrawableTypeEnum
        Line
        Circle
        Arc
        PolyLine
        Rectangle
        Point
    End Enum


    ' Type of entity
    Public DrawableType As DrawableTypeEnum

    ' New property for PolyLine
    Public Points As New List(Of PointF)


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
    ' Future CAM placeholders
    Public Property AssignedTool As String = Nothing
    Public Property FeedRate As Double = 0
    Public Property OffsetSide As String = Nothing

    Public Shared Function GetBounds(d As DxfDrawable) As RectangleF

        Select Case d.DrawableType

            Case DrawableTypeEnum.Line
                Dim x1 = d.StartPoint.X
                Dim y1 = d.StartPoint.Y
                Dim x2 = d.EndPoint.X
                Dim y2 = d.EndPoint.Y

                Dim minX = Math.Min(x1, x2)
                Dim minY = Math.Min(y1, y2)
                Dim maxX = Math.Max(x1, x2)
                Dim maxY = Math.Max(y1, y2)

                Return RectangleF.FromLTRB(minX, minY, maxX, maxY)

            Case DrawableTypeEnum.Circle
                Return New RectangleF(
                d.Center.X - d.Radius,
                d.Center.Y - d.Radius,
                d.Radius * 2,
                d.Radius * 2)

            Case DrawableTypeEnum.Arc
                Return New RectangleF(
                d.Center.X - d.Radius,
                d.Center.Y - d.Radius,
                d.Radius * 2,
                d.Radius * 2)

            Case Else
                Return RectangleF.Empty

        End Select

    End Function
    Public Interface ISelectable
        Function HitTest(pt As PointF, tolerance As Single) As Boolean
        Function GetEntityType() As DrawableTypeEnum
        Function GetLayer() As String
        Function GetTool() As String

        Property IsSelected As Boolean
        Property IsHovered As Boolean
    End Interface
End Class
