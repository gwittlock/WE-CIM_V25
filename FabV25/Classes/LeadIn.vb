Public Enum LeadJunction
    Exact
    Gap
    Overlap
End Enum

Public Enum LeadLocation
    OneOClock
    TwoOClock
    ThreeOClock
    FourOClock
    FiveOClock
    SixOClock
    SevenOClock
    EightOClock
    NineOClock
    TenOClock
    ElevenOClock
    TwelveOClock
End Enum

Public Enum LeadSplitLocation
    AtIntersection = 1
    ThirtyPercentFromIntersection = 3
    TwentyPercentFromIntersection = 5
    AlwaysSplit = 100
End Enum

Public Enum PierceAssociation
    Separate
    WithLead
End Enum

Public Class LeadIn

    Public Property Geometry As LeadGeometry

    Public Property Junction As LeadJunction

    Public Property LeadLocation As LeadLocation

    Public Property SplitLocation As LeadSplitLocation

    Public Property IncludePierceHole As Boolean

    Public Property PierceAssociation As PierceAssociation

    Public Property PierceDistance As Double

    Public Property ToolTypeID As Integer

    Public Property PierceToolDia As Double

End Class