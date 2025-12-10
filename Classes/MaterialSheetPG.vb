Imports System.ComponentModel

' A custom PropertyDescriptor for MaterialParameter
Public Class MaterialParameterDescriptor
        Inherits PropertyDescriptor
    Private _param As MaterialParameter

    Public Sub New(param As MaterialParameter)
        MyBase.New(param.Display, Nothing)
        _param = param
    End Sub

    Public Overrides ReadOnly Property ComponentType As Type
        Get
            Return GetType(MaterialSheet)
        End Get
    End Property

    Public Overrides ReadOnly Property IsReadOnly As Boolean
        Get
            Return False
        End Get
    End Property

    Public Overrides ReadOnly Property PropertyType As Type
        Get
            Select Case _param.DataType
                Case 0 : Return GetType(String)
                Case 1 : Return GetType(Integer)
                Case 4 : Return GetType(Boolean)
                Case 7 : Return GetType(Double)
                Case Else : Return GetType(Object)
            End Select
        End Get
    End Property

    Public Overrides Function CanResetValue(component As Object) As Boolean
        Return True
    End Function

    Public Overrides Function GetValue(component As Object) As Object
        Return _param.Value
    End Function

    Public Overrides Sub ResetValue(component As Object)
        _param.Value = _param.DefaultValue
    End Sub

    Public Overrides Sub SetValue(component As Object, value As Object)
        _param.Value = value
    End Sub

    Public Overrides Function ShouldSerializeValue(component As Object) As Boolean
        Return True
    End Function

End Class

    ' A custom PropertyGrid adapter for MaterialSheet
    Public Class MaterialSheetPG
        Inherits PropertyDescriptorCollection

        Private _sheet As MaterialSheet

        Public Sub New(sheet As MaterialSheet)
            MyBase.New(Nothing)
            _sheet = sheet
        End Sub

        ' This function returns PropertyDescriptors for the PropertyGrid
        Public Function GetProperties() As PropertyDescriptorCollection
            Dim props As New List(Of PropertyDescriptor)

            For Each param In _sheet.Values
                props.Add(New MaterialParameterDescriptor(param))
            Next

            Return New PropertyDescriptorCollection(props.ToArray())
        End Function

    End Class


