Imports FabV25_WIN8.ToolShapes
Public Class PathSegmentRecord

    Public Property SegmentType As PathSegmentType

    ' Line endpoints
    Public Property StartX As Double
    Public Property StartY As Double

    Public Property EndX As Double
    Public Property EndY As Double

    ' Arc geometry
    Public Property CenterX As Double
    Public Property CenterY As Double

    Public Property Radius As Double

    Public Property StartAngle As Double
    Public Property EndAngle As Double

    Public Property IsClockwise As Boolean

End Class