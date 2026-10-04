Imports System.ComponentModel

Public Class DropDownListConverter
    Inherits StringConverter

    Public Overrides Function GetStandardValuesSupported(context As ITypeDescriptorContext) As Boolean
        Return True
    End Function

    Public Overrides Function GetStandardValuesExclusive(context As ITypeDescriptorContext) As Boolean
        Return True
    End Function

    Public Overrides Function GetStandardValues(context As ITypeDescriptorContext) As StandardValuesCollection
        Dim propDesc = TryCast(context.PropertyDescriptor, DynamicMachinePropertyDescriptor)
        If propDesc IsNot Nothing AndAlso propDesc._prop.TypeDef.Options IsNot Nothing Then
            Return New StandardValuesCollection(propDesc._prop.TypeDef.Options)
        End If
        Return New StandardValuesCollection(New String() {})
    End Function
End Class
