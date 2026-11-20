
Public Class MM3NativeFile

    ' Section 1 - File Header
    Public Property Header As New MM3FileHeader()

    ' Section 2 - Job Information
    Public Property JobInformation As New MM3JobInformation()

    ' Section 3 - Manufacturing Environment
    Public Property ManufacturingEnvironment As New MM3ManufacturingEnvironment()

    ' Section 4 - Manufacturing Model
    Public Property ManufacturingModel As MM3File

    ' Section 5 - NC Code Generation
    Public Property NCCodeGeneration As New MM3NCCodeGeneration()

    ' Section 6 - Object References
    Public Property ObjectReferences As New MM3ObjectReferences()

End Class
