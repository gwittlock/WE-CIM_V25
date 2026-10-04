Imports System
Imports System.ComponentModel

Public Class MaterialParameterDescriptor
    Inherits PropertyDescriptor

    Private ReadOnly _param As MaterialParameter

    Public Sub New(param As MaterialParameter)
        MyBase.New(param.Name, Nothing)
        _param = param
    End Sub

    Public Overrides Function CanResetValue(component As Object) As Boolean
        Return False
    End Function

    Public Overrides Function GetValue(component As Object) As Object
        Return _param.Value
    End Function

    Public Overrides Sub SetValue(component As Object, value As Object)
        _param.Value = value
    End Sub

    Public Overrides ReadOnly Property ComponentType As Type
        Get
            Return GetType(MaterialParameter)
        End Get
    End Property

    Public Overrides ReadOnly Property IsReadOnly As Boolean
        Get
            Return False
        End Get
    End Property

    Public Overrides ReadOnly Property PropertyType As Type
        Get
            If _param.Value IsNot Nothing Then
                Return _param.Value.GetType()
            Else
                Return GetType(Object)
            End If
        End Get
    End Property

    Public Overrides Sub ResetValue(component As Object)
        ' No reset
    End Sub

    Public Overrides Function ShouldSerializeValue(component As Object) As Boolean
        Return True
    End Function

    Public Overrides ReadOnly Property Category As String
        Get
            Return "Values"
        End Get
    End Property

    Public Overrides ReadOnly Property DisplayName As String
        Get
            Return _param.Display
        End Get
    End Property
End Class
