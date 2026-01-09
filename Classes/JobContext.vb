Public Class JobContext
    Public Property MachineDefinition As MachineDefinition
    Public Property ToolSetupID As Integer
    Public Property MaterialID As Integer
    Public Property LayerSetupID As Integer

    Public Property WorkZones As New List(Of WorkZone)
End Class
