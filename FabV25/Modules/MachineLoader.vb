Imports Newtonsoft.Json
Imports System.IO
Imports System.Reflection
Imports FabV25_WIN8.AppData

Public Module MachineLoader

    ''' <summary>
    ''' Loads a machine definition from a JSON file dynamically into MachineDefinition.
    ''' </summary>
    ''' <param name="jsonFilePath">Full path to the JSON file.</param>
    ''' <returns>A populated MachineDefinition instance.</returns>
    'Load a Single machine definition With all attributes
    Public Function LoadFromJson(machineID As Integer) As MachineDefinition
        ' Load JSON tables (already implemented somewhere in your project)
        Dim machines As List(Of Machine) = LoadTableWrapped(Of Machine)("Machines.json")
        Dim machineAttributes As List(Of MachineAttribute) = LoadTableWrapped(Of MachineAttribute)("MachineAttributes.json")
        Dim machineAttributeTypes As List(Of MachineAttributeType) = LoadTableWrapped(Of MachineAttributeType)("MachineAttributeTypes.json")

        ' Find the machine row
        Dim machineRow As Machine = machines.FirstOrDefault(Function(m) m.ID = machineID)
        If machineRow Is Nothing Then Return Nothing

        ' Build a dictionary to map AttributeTypeID to MachineDefinition property name
        Dim attributeMap As Dictionary(Of Integer, String) =
            machineAttributeTypes.ToDictionary(Function(r As MachineAttributeType) r.ID, Function(r As MachineAttributeType) r.Description)

        ' Initialize the MachineDefinition
        Dim def As New MachineDefinition With {
            .ID = machineRow.ID,
            .Name = machineRow.Description,
            .TypeID = machineRow.TypeID,
            .WorkplaneTypeID = machineRow.WorkplaneTypeID,
            .Units = machineRow.Units,
            .CNC_Folder = machineRow.CNC_Folder
        }

        ' Filter attributes for this machine
        Dim attrsForMachine = machineAttributes.Where(Function(a) a.MachineID = machineID)

        ' Assign values dynamically
        For Each attr In attrsForMachine
            If attributeMap.ContainsKey(attr.AttributeTypeID) Then
                Dim propName As String = attributeMap(attr.AttributeTypeID)
                Dim propInfo As Reflection.PropertyInfo = GetType(MachineDefinition).GetProperty(propName)

                If propInfo IsNot Nothing AndAlso propInfo.CanWrite Then
                    Try
                        Dim propType = propInfo.PropertyType
                        Dim convertedValue As Object

                        If propType Is GetType(Boolean) Then
                            convertedValue = CBool(attr.Value)
                        ElseIf propType Is GetType(Integer) Then
                            convertedValue = CInt(attr.Value)
                        ElseIf propType Is GetType(Double) Then
                            convertedValue = CDbl(attr.Value)
                        Else
                            convertedValue = attr.Value
                        End If

                        propInfo.SetValue(def, convertedValue)
                    Catch ex As Exception
                        ' Optional: log any conversion error for debugging
                        Debug.Print($"Failed to set property {propName} with value {attr.Value}: {ex.Message}")
                    End Try
                End If
            End If
        Next

        Return def
    End Function
    ''' <summary>
    ''' Sets a property value safely, converting types as needed.
    ''' </summary>
    Private Sub SetPropertyValue(target As Object, prop As PropertyInfo, value As Object)
        If value Is Nothing Then Return

        Try
            Dim targetType As Type = prop.PropertyType

            ' Handle booleans, integers, doubles, enums, and strings
            If targetType Is GetType(Boolean) Then
                prop.SetValue(target, Convert.ToBoolean(value))
            ElseIf targetType Is GetType(Integer) Then
                prop.SetValue(target, Convert.ToInt32(value))
            ElseIf targetType Is GetType(Double) Then
                prop.SetValue(target, Convert.ToDouble(value))
            ElseIf targetType.IsEnum Then
                prop.SetValue(target, [Enum].Parse(targetType, value.ToString()))
            Else
                prop.SetValue(target, value.ToString())
            End If
        Catch ex As Exception
            ' Optional: log or ignore errors for unmapped or invalid values
        End Try
    End Sub

End Module
