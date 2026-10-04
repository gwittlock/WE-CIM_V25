Imports FabV25_WIN8.ToolShapes

Public Class ProfileFeature
    Inherits MachiningFeature

    Public Property SourceProfile As Profile

    Public Property ToolID As Integer

    Public Property CutSide As AppData.CutSideEnum

    Public Property CutDirection As AppData.CutDirectionEnum

    Public Property LeadIn As LeadIn

    Public Property TooledProfile As TooledProfile

    Public Property LeadOut As LeadOut

End Class


Public Class ProfileBuildResult
    Public Property Profile As Profile
    Public Property DirectiveID As Integer
End Class