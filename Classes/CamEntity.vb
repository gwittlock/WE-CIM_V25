Public Class CamEntity
    Class CamEntity
        Property Type As CamEntityType       ' Line, Arc, Circle, Point, Hole, Profile, etc.
        Property Geometry As Object           ' Raw CAD entity or custom struct
        Property Layer As String              ' Layer from CAD
        Property Feature As String            ' Optional: only for tooled entities
        Property Tool As Tool                 ' Optional: associated tool
        Property Text As String               ' Optional text (annotations, commands)
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
