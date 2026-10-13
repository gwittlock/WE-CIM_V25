Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Drawing

' Enum for generalized dynamic data types
Public Enum DynamicDataType
    StringType = 0
    IntegerType = 1
    DoubleType = 7
    BooleanType = 8
    Dropdown = 9
    File = 10
    Folder = 11
    ColorType = 12
End Enum

' Type definition for each dynamic property
Public Class DynamicPropertyType
    Public Property Name As String          ' Internal Name
    Public Property Display As String       ' Display in PropertyGrid
    Public Property DataType As DynamicDataType
    Public Property Options As List(Of String) ' For dropdowns
End Class

' Represents a single dynamic property for the PropertyGrid
Public Class DynamicProperty
    Inherits PropertyDescriptor

    Private _type As DynamicPropertyType
    Private _value As Object

    ' Constructor
    Public Sub New(propType As DynamicPropertyType, val As Object)
        MyBase.New(propType.Name, Nothing)
        _type = propType
        _value = val
    End Sub

    ' Required Overrides for PropertyDescriptor
    Public Overrides ReadOnly Property ComponentType As Type
        Get
            Return GetType(DynamicPropertyBag)
        End Get
    End Property

    Public Overrides ReadOnly Property IsReadOnly As Boolean
        Get
            Return False
        End Get
    End Property

    Public Overrides ReadOnly Property PropertyType As Type
        Get
            Select Case _type.DataType
                Case DynamicDataType.StringType : Return GetType(String)
                Case DynamicDataType.IntegerType : Return GetType(Integer)
                Case DynamicDataType.DoubleType : Return GetType(Double)
                Case DynamicDataType.BooleanType : Return GetType(Boolean)
                Case DynamicDataType.Dropdown : Return GetType(String)
                Case DynamicDataType.File : Return GetType(String)
                Case DynamicDataType.Folder : Return GetType(String)
                Case DynamicDataType.ColorType : Return GetType(Color)
                Case Else : Return GetType(Object)
            End Select
        End Get
    End Property

    Public Overrides Function CanResetValue(component As Object) As Boolean
        Return False
    End Function

    Public Overrides Sub ResetValue(component As Object)
    End Sub

    Public Overrides Function ShouldSerializeValue(component As Object) As Boolean
        Return True
    End Function

    Public Overrides Function GetValue(component As Object) As Object
        Return _value
    End Function

    Public Overrides Sub SetValue(component As Object, value As Object)
        _value = value
    End Sub

    ' Optional: TypeConverter for dropdowns
    Public Overrides ReadOnly Property Converter As TypeConverter
        Get
            If _type.DataType = DynamicDataType.Dropdown AndAlso _type.Options IsNot Nothing Then
                Return New DropDownListConverter(_type.Options)
            End If
            Return MyBase.Converter
        End Get
    End Property
End Class


' Dropdown converter
Public Class DropDownListConverter
    Inherits StringConverter

    Private _options As List(Of String)

    Public Sub New(options As List(Of String))
        _options = options
    End Sub

    Public Overrides Function GetStandardValuesSupported(context As ITypeDescriptorContext) As Boolean
        Return True
    End Function

    Public Overrides Function GetStandardValuesExclusive(context As ITypeDescriptorContext) As Boolean
        Return True
    End Function

    Public Overrides Function GetStandardValues(context As ITypeDescriptorContext) As TypeConverter.StandardValuesCollection
        Return New StandardValuesCollection(_options)
    End Function
End Class
