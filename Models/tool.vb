Namespace WE_ENG_V25_0.Core.Models
    Public Class Tool
        Public Property ID As Integer
        Public Property Description As String
        Public Property ToolTypeID As Integer
        Public Property Diameter As Double
    End Class

    ' Tool Crib
    Public Class ToolCribItem
        Public Property ID As Integer
        Public Property ToolTypeID As Integer
        Public Property Name As String
        Public Property Attributes As Dictionary(Of String, Object) ' <-- dynamic attributes
    End Class

    ' Tool Types
    Public Class ToolType
        Public Property ID As Integer
        Public Property Description As String
        Public Property Display As String
    End Class

    ' Tool Attribute Types
    Public Class ToolAttributeType
        Public Property ID As Integer
        Public Property Description As String
        Public Property Field As String
        Public Property DataType As Integer
    End Class

    ' Tool Type Attributes (maps ToolTypeID to ToolAttributeTypeID)
    Public Class ToolTypeAttribute
        Public Property ID As Integer
        Public Property ToolTypeID As Integer
        Public Property AttributeTypeID As Integer
    End Class
End Namespace
