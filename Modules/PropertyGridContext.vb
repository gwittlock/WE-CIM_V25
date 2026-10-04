Imports System.ComponentModel

Public Module PropertyGridContext
    Public Property CurrentToolSetupID As Integer = -1

    ''' <summary>
    ''' Returns the list of station/tool names for the current tool setup, starting with "None".
    ''' </summary>
    Public Function GetStationsForCurrentToolSetup() As List(Of String)
        Dim result As New List(Of String) From {"None"}

        If CurrentToolSetupID <= 0 OrElse AppData.ToolCrib Is Nothing OrElse AppData.ToolSetupMembers Is Nothing Then
            Return result
        End If

        ' Find ToolIDs assigned to the current ToolSetup
        Dim toolIDsInSetup = AppData.ToolSetupMembers.
            Where(Function(m) m.ToolSetupID = CurrentToolSetupID).
            Select(Function(m) m.ToolID).
            Distinct().
            ToList()

        ' Get the corresponding ToolCribItems
        Dim toolsInSetup = AppData.ToolCrib.
            Where(Function(t) toolIDsInSetup.Contains(t.ID)).
            OrderBy(Function(t) t.Description).
            Select(Function(t) t.Description).
            ToList()

        result.AddRange(toolsInSetup)
        Return result
    End Function

    Public Class StationTypeConverter
        Inherits StringConverter

        Public Overrides Function GetStandardValuesSupported(context As ITypeDescriptorContext) As Boolean
            Return True
        End Function

        Public Overrides Function GetStandardValuesExclusive(context As ITypeDescriptorContext) As Boolean
            Return True
        End Function

        Public Overrides Function GetStandardValues(context As ITypeDescriptorContext) As StandardValuesCollection
            Dim toolNames As New List(Of String)
            toolNames.Add("None") ' Always first

            ' Get the current node from the property grid context
            Dim currentNode As TreeNode = ConfigurationManagerForm.CurrentSelectedNode
            If currentNode IsNot Nothing Then
                ' Get the ToolSetupID from the node (even if it's a child)
                Dim toolSetupID = ConfigurationManagerForm.GetToolSetupIDFromNode(currentNode)

                If toolSetupID <> -1 Then
                    ' Lookup all tools for this ToolSetupID
                    Dim members = AppData.ToolSetupMembers.Where(Function(m) m.ToolSetupID = toolSetupID).ToList()

                    For Each member In members
                        ' Get Tool description from ToolCrib
                        Dim tool = AppData.ToolCrib.FirstOrDefault(Function(t) t.ID = member.ToolID)
                        If tool IsNot Nothing Then
                            toolNames.Add(tool.Description)
                        End If
                    Next
                End If
            End If

            Return New StandardValuesCollection(toolNames)
        End Function
    End Class

End Module
