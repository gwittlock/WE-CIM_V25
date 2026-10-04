Public Interface ISelectable
    Function HitTest(pt As PointF, tolerance As Single) As Boolean
    Function GetEntityType() As DxfDrawable.DrawableTypeEnum
    Function GetLayer() As String
    Function GetTool() As String

    Property IsSelected As Boolean
    Property IsHovered As Boolean
End Interface