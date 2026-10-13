Imports System.ComponentModel

Public Class DynamicPropertyBag
    Implements ICustomTypeDescriptor

    Private _props As List(Of DynamicProperty)
    Private _properties As List(Of DynamicProperty)


    Public Sub New(propTypes As List(Of DynamicPropertyType), values As Dictionary(Of String, Object))
        _props = New List(Of DynamicProperty)
        For Each pt In propTypes
            ' Use Name as key instead of ID
            Dim val As Object = If(values.ContainsKey(pt.Name), values(pt.Name), GetDefaultValue(pt))
            _props.Add(New DynamicProperty(pt, val))
        Next
    End Sub


    Private Function GetDefaultValue(pt As DynamicPropertyType) As Object
        Select Case pt.DataType
            Case DynamicDataType.StringType, DynamicDataType.File, DynamicDataType.Folder
                Return ""
            Case DynamicDataType.IntegerType
                Return 0
            Case DynamicDataType.DoubleType
                Return 0.0
            Case DynamicDataType.BooleanType
                Return False
            Case DynamicDataType.ColorType
                Return Color.Black
            Case DynamicDataType.Dropdown
                Return If(pt.Options?.FirstOrDefault(), "")
            Case Else
                Return Nothing
        End Select
    End Function

    ' ICustomTypeDescriptor implementation
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

    Public Function GetProperties() As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Dim arr(_properties.Count - 1) As PropertyDescriptor
        For i As Integer = 0 To _properties.Count - 1
            arr(i) = _properties(i)
        Next
        Return New PropertyDescriptorCollection(arr)
    End Function

    Public Function GetProperties(attributes() As Attribute) As PropertyDescriptorCollection Implements ICustomTypeDescriptor.GetProperties
        Return GetProperties()
    End Function

    Public Function GetPropertyOwner(pd As PropertyDescriptor) As Object Implements ICustomTypeDescriptor.GetPropertyOwner
        Return Me
    End Function
End Class
