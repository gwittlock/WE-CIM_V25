Imports System.ComponentModel

Public Class DynamicMachinePropertyBag
    Implements ICustomTypeDescriptor

    Private _properties As List(Of DynamicMachineProperty)

    Public Sub New(attrs As List(Of MachineAttribute), types As List(Of MachineAttributeType))
        _properties = New List(Of DynamicMachineProperty)()

        For Each attr In attrs
            Dim typeDef = types.FirstOrDefault(Function(t) t.ID = attr.AttributeTypeID)
            If typeDef IsNot Nothing Then
                _properties.Add(New DynamicMachineProperty(attr, typeDef))
            End If
        Next
    End Sub

    ' Indexer for convenience (optional)
    Default Public Property Item(name As String) As Object
        Get
            Dim prop = _properties.FirstOrDefault(Function(p) p.TypeDef.Display = name)
            Return If(prop?.Value, Nothing)
        End Get
        Set(value As Object)
            Dim prop = _properties.FirstOrDefault(Function(p) p.TypeDef.Display = name)
            If prop IsNot Nothing Then prop.Value = value
        End Set
    End Property

    ' --- ICustomTypeDescriptor overrides ---
    Public Function GetProperties() As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Dim descriptors = _properties.Select(Function(p) New DynamicMachinePropertyDescriptor(p)).ToArray()
        Return New PropertyDescriptorCollection(descriptors)
    End Function

    Public Function GetProperties(attributes() As Attribute) As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Return GetProperties()
    End Function

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
