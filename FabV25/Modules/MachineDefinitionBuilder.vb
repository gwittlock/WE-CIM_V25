Imports System.Reflection

Public Module MachineDefinitionBuilder

    Public Function BuildFromMachine(machineRow As Machine) As MachineDefinition

        Dim def As New MachineDefinition

        For Each prop As PropertyInfo In GetType(MachineDefinition).GetProperties()

            Dim mapAttr = prop.GetCustomAttributes(GetType(MachineMapAttribute), False).
                               Cast(Of MachineMapAttribute)().
                               FirstOrDefault()

            If mapAttr Is Nothing Then Continue For

            Dim srcProp = GetType(Machine).GetProperty(mapAttr.SourceProperty)
            If srcProp Is Nothing Then Continue For

            Dim value = srcProp.GetValue(machineRow)

            If value Is Nothing Then Continue For

            ' Handle enums safely
            If prop.PropertyType.IsEnum Then
                value = [Enum].ToObject(prop.PropertyType, value)
            End If

            prop.SetValue(def, value)

        Next

        Return def

    End Function

End Module
