Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool

Namespace WE_ENG_V25_0.Core.Models
    Public Class Tool

        Public Property ID As Integer
        Public Property Description As String
        Public Property ToolTypeID As Integer
        Public Property Diameter As Double

        Public Class ToolMatch
            Public Property ToolID As Integer
        End Class

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
        Public Shared Property ToolSetupMembers As List(Of ToolSetupMember)
        '  Public Property ToolSignatures As New List(Of ToolSignature)


        'Public Function GetToolByID(id As Integer) As Tool
        '    Return Tools.FirstOrDefault(Function(t) t.ID = id)
        'End Function

        Private Function NearlyEqualAngle(a As Double, b As Double) As Boolean
            Return Math.Abs(a - b) < 0.25
        End Function

        '        Public Function FindExactMatch(
        '    profile As Polyline2D,
        '    rule As LayerPunchRule
        ') As ToolMatch

        '            Dim sig = profile.GetSignature()
        '            If sig.ShapeType = ProfileShapeType.Unknown Then
        '                Return Nothing
        '            End If

        '            Dim geomAngle As Double = profile.GetOrientationAngle()

        '            For Each tool In ToolSignatures

        '                ' Shape + size must match first
        '                If Not tool.Signature.Equals(sig) Then Continue For

        '                ' Indexing rules
        '                If rule.AutoIndex Then
        '                    Return New ToolMatch With {.ToolID = tool.ToolID}
        '                End If

        '                ' AutoIndex = False → angle must match
        '                If NearlyEqualAngle(tool.IndexAngle, geomAngle) Then
        '                    Return New ToolMatch With {.ToolID = tool.ToolID}
        '                End If

        '            Next

        '            Return Nothing

        '        End Function


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

    'Public Class ToolSignature
    '    Public Property ToolID As Integer
    '    Public Property Signature As ProfileSignature
    '    Public Property IndexAngle As Double   ' Degrees
    'End Class
End Namespace
