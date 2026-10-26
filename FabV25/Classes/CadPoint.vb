Imports System.Drawing
Imports FabV25_WIN8.DxfGeometryHelpers   ' <-- for GeometryTypeEnum
Imports FabV25_WIN8                      ' <-- adjust if DrawableTypeEnum lives elsewhere

Public Class CadPoint
    Inherits CadEntity

    ' =========================================================
    ' DATA
    ' =========================================================
    Public Property Position As PointF

    ' =========================================================
    ' ISelectable STATE
    ' =========================================================
    'Public Property IsSelected As Boolean Implements ISelectable.IsSelected
    'Public Property IsHovered As Boolean Implements ISelectable.IsHovered

    ' =========================================================
    ' CONSTRUCTOR
    ' =========================================================
    Public Sub New(pos As PointF)
        Me.Position = pos
        Me.GeometryType = DxfGeometryHelpers.GeometryTypeEnum.Point
    End Sub

    ' =========================================================
    ' REQUIRED OVERRIDES (CadEntity)
    ' =========================================================
    Public Overrides Sub Translate(dx As Single, dy As Single)
        Position = New PointF(Position.X + dx, Position.Y + dy)
    End Sub

    ' =========================================================
    ' ISelectable IMPLEMENTATION
    ' =========================================================
    Public Function HitTest(pt As PointF, tolerance As Single) As Boolean
        Return Distance(pt, Position) <= tolerance
    End Function


    Public Function GetLayer() As String
        Return Me.LayerName   ' <-- adjust based on your base class
    End Function
    Public Function GetTool() As String
    End Function

    ' =========================================================
    ' HELPERS
    ' =========================================================
    Private Function Distance(a As PointF, b As PointF) As Single
        Return CSng(Math.Sqrt((a.X - b.X) ^ 2 + (a.Y - b.Y) ^ 2))
    End Function

End Class