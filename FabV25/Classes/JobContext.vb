Public Class JobContext
    Public Property MachineDefinition As MachineDefinition
    Public Property ToolSetupID As Integer
    Public Property MaterialID As Integer
    Public Property LayerSetupID As Integer

    Public Property WorkZones As New List(Of WorkZone)
    Public Property DxfPlacement As DxfPlacementMode = DxfPlacementMode.AlignLowerLeftToSheetOrigin


    ' Core geometry for the job
    Public Property Entities As List(Of CadEntity)

    Public Sub New()
        Entities = New List(Of CadEntity)
    End Sub
End Class
