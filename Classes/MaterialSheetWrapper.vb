' Wrapper for PropertyGrid
Imports System.ComponentModel

Public Class MaterialSheetWrapper
    Implements ICustomTypeDescriptor

    Private _sheet As MaterialSheet

    Public Sub New(sheet As MaterialSheet)
        _sheet = sheet
    End Sub

    ' Expose each MaterialParameter as a property
    Public Function GetProperties() As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Dim props As New List(Of PropertyDescriptor)
        For Each p In _sheet.Values
            props.Add(New MaterialParameterDescriptor(p))
        Next
        Return New PropertyDescriptorCollection(props.ToArray())
    End Function

    Public Function GetProperties(attributes() As Attribute) As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Return GetProperties()
    End Function

    ' The rest can return defaults
    Public Function GetAttributes() As AttributeCollection Implements ICustomTypeDescriptor.GetAttributes
        Return AttributeCollection.Empty
    End Function
    Public Function GetClassName() As String Implements ICustomTypeDescriptor.GetClassName
        Return _sheet.Description
    End Function
    Public Function GetComponentName() As String Implements ICustomTypeDescriptor.GetComponentName
        Return _sheet.Description
    End Function
    Public Function GetConverter() As TypeConverter Implements ICustomTypeDescriptor.GetConverter
        Return Nothing
    End Function
    Public Function GetDefaultEvent() As EventDescriptor Implements ICustomTypeDescriptor.GetDefaultEvent
        Return Nothing
    End Function
    Public Function GetDefaultProperty() As PropertyDescriptor Implements ICustomTypeDescriptor.GetDefaultProperty
        Return Nothing
    End Function
    Public Function GetEditor(editorBaseType As Type) As Object Implements ICustomTypeDescriptor.GetEditor
        Return Nothing
    End Function
    Public Function GetEvents() As EventDescriptorCollection Implements ICustomTypeDescriptor.GetEvents
        Return EventDescriptorCollection.Empty
    End Function
    Public Function GetEvents(attributes() As Attribute) As EventDescriptorCollection Implements ICustomTypeDescriptor.GetEvents
        Return EventDescriptorCollection.Empty
    End Function
    Public Function GetPropertyOwner(pd As PropertyDescriptor) As Object Implements ICustomTypeDescriptor.GetPropertyOwner
        Return _sheet
    End Function
End Class
