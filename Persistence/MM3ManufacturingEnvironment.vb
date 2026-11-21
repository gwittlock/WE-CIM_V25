

Public Class MM3ManufacturingEnvironment

    Public Property Machine As MachineDefinition

    Public Property ToolSetup As WE_ENG_V25_0.Core.Models.ToolSetup

    Public Property ToolSetupMembers As New List(Of WE_ENG_V25_0.Core.Models.ToolSetupMember)


    Public Property Tools As New List(Of WE_ENG_V25_0.Core.Models.Tool)

    Public Property ToolTypes As New List(Of WE_ENG_V25_0.Core.Models.ToolType)

    Public Property ToolAttributeTypes As New List(Of WE_ENG_V25_0.Core.Models.ToolAttributeType)

    Public Property ToolTypeAttributes As New List(Of WE_ENG_V25_0.Core.Models.ToolTypeAttribute)


    Public Property ToolSetupUDPDefinitions As New List(Of AppData.ToolSetupUDPDefinition)

    Public Property ToolSetupUDPValues As New List(Of WE_ENG_V25_0.Core.Models.ToolSetupUDPValue)


    Public Property Stations As New List(Of AppData.Station)

    Public Property Material As MaterialSheet

    Public Property MaterialType As MaterialType

    Public Property LayerSetup As AppData.layersetup

    Public Property MachineConfiguration As Machine

    Public Property MachineType As MachineType

    Public Property MachineTypeAttributes As New List(Of MachineTypeAttribute)

    Public Property MachineToolTypes As New List(Of MachineToolType)

    Public Property MachineAttributes As New List(Of MachineAttribute)

    Public Property MachineAttributeTypes As New List(Of MachineAttributeType)



End Class
