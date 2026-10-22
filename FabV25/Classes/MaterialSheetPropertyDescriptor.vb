
Imports System
Imports System.ComponentModel
Imports System.Reflection

Public Class MaterialSheetPropertyDescriptor
    Inherits PropertyDescriptor

    Private ReadOnly _property As PropertyInfo
    Private ReadOnly _displayName As String

    Public Sub New(propertyName As String, displayName As String)
        MyBase.New(propertyName, Nothing)

        _property = GetType(MaterialSheet).GetProperty(propertyName)
        _displayName = displayName

        If _property Is Nothing Then
            Throw New ArgumentException(
                "Property not found on MaterialSheet: " & propertyName)
        End If
    End Sub

    Public Overrides ReadOnly Property ComponentType As Type
        Get
            Return GetType(MaterialSheet)
        End Get
    End Property

    Public Overrides ReadOnly Property PropertyType As Type
        Get
            Return _property.PropertyType
        End Get
    End Property

    Public Overrides ReadOnly Property IsReadOnly As Boolean
        Get
            Return Not _property.CanWrite
        End Get
    End Property

    Public Overrides ReadOnly Property DisplayName As String
        Get
            Return _displayName
        End Get
    End Property

    Public Overrides ReadOnly Property Category As String
        Get
            Return "Material"
        End Get
    End Property

    Public Overrides Function GetValue(component As Object) As Object
        Return _property.GetValue(component, Nothing)
    End Function

    Public Overrides Sub SetValue(component As Object, value As Object)
        If IsReadOnly Then Return

        _property.SetValue(component, value, Nothing)
        OnValueChanged(component, EventArgs.Empty)
    End Sub

    Public Overrides Function CanResetValue(component As Object) As Boolean
        Return False
    End Function

    Public Overrides Sub ResetValue(component As Object)
        ' No reset operation.
    End Sub

    Public Overrides Function ShouldSerializeValue(component As Object) As Boolean
        Return True
    End Function
End Class
