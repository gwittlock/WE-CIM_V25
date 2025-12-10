Imports System.ComponentModel

Public Class DynamicMachinePropertyBag
    Implements ICustomTypeDescriptor

    Private _properties As New Dictionary(Of String, DynamicMachineProperty)

    Public Sub New(attrs As List(Of MachineAttribute), types As List(Of MachineAttributeType))
        For Each attr In attrs
            Dim typeDef = types.FirstOrDefault(Function(t) t.ID = attr.AttributeTypeID)
            If typeDef IsNot Nothing Then
                _properties(typeDef.Display) = New DynamicMachineProperty(attr, typeDef)
            End If
        Next
    End Sub

    Default Public Property Item(name As String) As Object
        Get
            Return If(_properties.ContainsKey(name), _properties(name).Value, Nothing)
        End Get
        Set(value As Object)
            If _properties.ContainsKey(name) Then
                _properties(name).Value = value
            End If
        End Set
    End Property

    ' --- Implement ICustomTypeDescriptor ---
    Public Function GetProperties() As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Dim list As New List(Of PropertyDescriptor)
        For Each p In _properties.Values
            list.Add(New DynamicMachinePropertyDescriptor(p))
        Next
        Return New PropertyDescriptorCollection(list.ToArray())
    End Function

    Public Function GetProperties(attributes() As Attribute) As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Return GetProperties()
    End Function

    ' Other ICustomTypeDescriptor members can return default implementations
    Public Function GetAttributes() As AttributeCollection Implements ICustomTypeDescriptor.GetAttributes
        Return AttributeCollection.Empty
    End Function
    Public Function GetClassName() As String Implements ICustomTypeDescriptor.GetClassName
        Return Nothing
    End Function
    Public Function GetComponentName() As String Implements ICustomTypeDescriptor.GetComponentName
        Return Nothing
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
        Return Me
    End Function
End Class
