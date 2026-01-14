Imports FabV25_WIN8.CadEntity



Module modSnap
    Public Enum SnapType
        None
        Endpoint
        Midpoint
        Center
        Intersection
    End Enum
    Public Structure SnapResult
        Public Type As SnapType
        Public WorldPoint As PointF
        Public Entity As CadEntity
        Public Distance As Single   ' world distance
    End Structure



End Module
