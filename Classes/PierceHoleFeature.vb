Imports WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.ToolShapes

Public Class PierceHoleFeature
    Inherits MachiningFeature

    '------------------------------------------------------------
    ' Lead-in that this pierce hole belongs to
    '------------------------------------------------------------
    Public Property LeadIn As LeadIn

    '------------------------------------------------------------
    ' Distance that the edge of the round tool overlaps
    ' the start of the lead-in.
    '------------------------------------------------------------
    Public Property PierceOffset As Double

    '------------------------------------------------------------
    ' Diameter used to identify the round punch/tool.
    '------------------------------------------------------------
    Public Property PierceToolDiameter As Double

    '------------------------------------------------------------
    ' Tool selected for the pierce operation.
    ' This must be a round tool.
    '------------------------------------------------------------
    Public Property Tool As Tool

    '------------------------------------------------------------
    ' Pierce type from the existing WE-CIM configuration.
    '------------------------------------------------------------
    Public Property PierceTypeID As Integer

    '------------------------------------------------------------
    ' Existing WE-CIM pierce mode.
    '------------------------------------------------------------
    Public Property UsePierce As Integer

End Class