Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool

Namespace WE_ENG_V25_0.Core.Models

    Public Class Tool
        Implements IGeoAssociation

        Public Property ID As Integer Implements IGeoAssociation.ID
        Public Property TypeID As Integer
        Public Property Description As String
        Public Property Attributes As Dictionary(Of String, Object)


        Public ReadOnly Property Color As System.Drawing.Color
            Get
                If Attributes Is Nothing OrElse Not Attributes.ContainsKey("COLOR") Then
                    Return System.Drawing.Color.White
                End If

                Try
                    Return System.Drawing.ColorTranslator.FromOle(
                Convert.ToInt32(Attributes("COLOR")))
                Catch
                    Return System.Drawing.Color.White
                End Try
            End Get
        End Property

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
        Public Shared Property ToolSetupMembers As List(Of ToolSetupMember)
        '  Public Property ToolSignatures As New List(Of ToolSignature)


        'Public Function GetToolByID(id As Integer) As Tool
        '    Return Tools.FirstOrDefault(Function(t) t.ID = id)
        'End Function

        Private Function NearlyEqualAngle(a As Double, b As Double) As Boolean
            Return Math.Abs(a - b) < 0.25
        End Function


        Public Function TryDecompose(profile As Polyline2D) As List(Of Integer)
            ' STUB — decomposition logic comes later
            Return Nothing
        End Function
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
    Public Class ToolSetupUDPValue
        Public Property ID As Integer
        Public Property ToolSetupID As Integer
        Public Property ToolSetupMemberID As Integer
        Public Property ParameterName As String
        Public Property Value As String
    End Class
End Namespace
