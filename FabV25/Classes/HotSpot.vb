Imports System.Drawing

Public Enum HotSpotType
    None = 0
    Endpoint = 1
    Midpoint = 2
    Center = 3
    Intersection = 4
    Quadrant = 5
End Enum

Public Class HotSpot

    Public Property Type As HotSpotType

    Public Property WorldPoint As PointF

    Public Property ScreenPoint As PointF

    Public Property Entity1 As CadEntity

    Public Property Entity2 As CadEntity

    Public Sub New()

        Type = HotSpotType.None
        WorldPoint = PointF.Empty
        ScreenPoint = PointF.Empty
        Entity1 = Nothing
        Entity2 = Nothing

    End Sub
    Public Sub New(
    spotType As HotSpotType,
    worldPoint As PointF,
    entity1 As CadEntity)

        Me.Type = spotType
        Me.WorldPoint = worldPoint
        Me.ScreenPoint = PointF.Empty
        Me.Entity1 = entity1
        Me.Entity2 = Nothing

    End Sub

    Public Sub New(
    spotType As HotSpotType,
    worldPoint As PointF,
    entity1 As CadEntity,
    entity2 As CadEntity)

        Me.Type = spotType
        Me.WorldPoint = worldPoint
        Me.ScreenPoint = PointF.Empty
        Me.Entity1 = entity1
        Me.Entity2 = entity2

    End Sub

End Class