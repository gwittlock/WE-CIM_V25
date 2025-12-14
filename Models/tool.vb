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
        Public Property TypeID As Integer
        Public Property Description As String
        Public Property Attributes As Dictionary(Of String, Object) ' <-- dynamic attributes
    End Class

    ' Tool Types
    Public Class ToolType
        Public Property ID As Integer
        Public Property Description As String
        Public Property Display As String
    End Class

    ' Tool Attribute Types
    Public Class ToolTypeAttribute
        Public Property ToolTypeID As Integer        ' ID of the Tool Type
        Public Property AttributeTypeID As Integer   ' ID of the Attribute (links to ToolAttributeType.ID)
    End Class


    ' Tool Type Attributes (maps ToolTypeID to ToolAttributeTypeID)
    Public Class ToolAttributeType
        Public Property ID As Integer
        Public Property Description As String    ' Display name
        Public Property Field As String           ' Dictionary / property key
        Public Property DataType As Integer       ' Lookup to datatype table
    End Class


    Public Class ToolSetup
        Public Property ID As Integer
        Public Property Description As String
        Public Property MachineID As Integer
        'Public Property Tools As List(Of Integer)
    End Class

    Public Class ToolSetupMember
        Public Property ID As Integer           ' Unique ID for this mapping
        Public Property ToolSetupID As Integer  ' Which ToolSetup this belongs to
        Public Property StationID As Integer    ' Which Station this maps to
        Public Property ToolID As Integer       ' Tool assigned to the Station

        ' These come from ToolSetupMembers.json
        Public Property FixedStation As Boolean
        Public Property IndexAngle As Double
    End Class
End Namespace
