Public Enum LeadGeometryType
    None
    Line
    Arc
    LineLine
    LineArc
End Enum

Public Class LeadGeometry

    Public Property Type As LeadGeometryType

    Public Property Length As Double

    Public Property Angle As Double

    Public Property IncludedAngle As Double

    Public Property Radius As Double

End Class