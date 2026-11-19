Public Class GeoEntityRecord

    Public Property ID As Integer

    Public Property GeometryType As String
    Public Property X As Double
    Public Property Y As Double

    Public Property StartPointID As Integer?
    Public Property EndPointID As Integer?
    Public Property CenterPointID As Integer?

    Public Property Radius As Double?
    Public Property StartAngle As Double?
    Public Property EndAngle As Double?
    Public Property IsClockwise As Boolean?

    Public Property AssociationType As String

    Public Property AssociationID As Integer?

End Class