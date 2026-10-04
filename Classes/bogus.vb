Public Class bogus
    Class CamEntity
        Property Type As CamEntityType   ' Line, Arc, Circle, Point, Hole, etc.
        Property Geometry As Object       ' Could be a DXF entity or custom struct
        Property Layer As String
        Property Feature As String        ' Optional: only for tooled entities
        Property Tool As Tool             ' Optional, reference to associated tool
        Property Text As String
    End Class

    Enum CamEntityType
        Line
        Arc
        Circle
        Point
        Hole
        Profile
        Command
        WorkZone
    End Enum

End Class
