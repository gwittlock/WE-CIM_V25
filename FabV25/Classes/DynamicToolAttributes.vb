Imports System.ComponentModel
Public Class DynamicToolAttributes
    Implements ICustomTypeDescriptor


    Public _attributes As Dictionary(Of String, Object)
    Private _types As Dictionary(Of String, Type)

    Public Sub New(attributes As Dictionary(Of String, Object))
        _attributes = attributes
        _types = New Dictionary(Of String, Type)()

        ' Determine the property type based on the value
        For Each kvp In attributes
            If kvp.Value IsNot Nothing Then
                _types(kvp.Key) = kvp.Value.GetType()
            Else
                _types(kvp.Key) = GetType(Object)
            End If
        Next
    End Sub

    ' ---------------------------
    ' ICustomTypeDescriptor Implementation
    ' ---------------------------

    Public Function GetProperties() As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Return GetProperties(Nothing)
    End Function

    Public Function GetProperties(attributes() As Attribute) As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Dim props As New List(Of PropertyDescriptor)

        For Each kvp In _attributes
            props.Add(New DynamicPropertyDescriptor(kvp.Key, _types(kvp.Key)))
        Next

        Return New PropertyDescriptorCollection(props.ToArray())
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

    Default Public Property Item(propertyName As String) As Object
        Get
            If _attributes.ContainsKey(propertyName) Then
                Return _attributes(propertyName)
            End If
            Return Nothing
        End Get
        Set(value As Object)
            If _attributes.ContainsKey(propertyName) Then
                _attributes(propertyName) = value
            End If
        End Set
    End Property
End Class

' ---------------------------
' Dynamic Property Descriptor
' ---------------------------
Public Class DynamicPropertyDescriptor
        Inherits PropertyDescriptor

        Private _propertyType As Type

        Public Sub New(name As String, propertyType As Type)
            MyBase.New(name, Nothing)
            _propertyType = propertyType
        End Sub

        Public Overrides ReadOnly Property PropertyType As Type
            Get
                Return _propertyType
            End Get
        End Property

        Public Overrides ReadOnly Property ComponentType As Type
            Get
                Return GetType(DynamicToolAttributes)
            End Get
        End Property

        Public Overrides ReadOnly Property IsReadOnly As Boolean
            Get
                Return False
            End Get
        End Property

        Public Overrides Function CanResetValue(component As Object) As Boolean
            Return False
        End Function

        Public Overrides Function GetValue(component As Object) As Object
            Return CType(component, DynamicToolAttributes)(Name)
        End Function

        Public Overrides Sub ResetValue(component As Object)
            ' Not implemented
        End Sub

        Public Overrides Sub SetValue(component As Object, value As Object)
            CType(component, DynamicToolAttributes)(Name) = value
        End Sub

        Public Overrides Function ShouldSerializeValue(component As Object) As Boolean
            Return True
        End Function
    End Class
