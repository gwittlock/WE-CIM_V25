Public Class MM3File

    Public Property FormatVersion As Integer = 1
    Public Property WorkZones As New List(Of WorkZone)
    Public Property MaterialSheetID As Integer
    Public Property Length As Double
    Public Property Width As Double

    Public Property Clamps As New List(Of Clamp)
    Public Property Parts As New List(Of PartRecord)

    Public Property Patterns As New List(Of PatternRecord)
    Public Property PatternInstances As New List(Of PatternInstanceRecord)

    Public Property Geometry As New List(Of GeoEntityRecord)

    Public Property GeometryPoints As New List(Of GeoEntityRecord)

    Public Property Layers As New List(Of LayerRecord)

    Public Property GeoProfiles As New List(Of GeoProfileRecord)

    Public Property ManufacturingFeatures As New List(Of MachiningFeatureRecord)

    Public Property WorkZoneAssignments As New List(Of WorkZoneAssignmentRecord)




End Class