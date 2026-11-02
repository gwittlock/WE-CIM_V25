
Imports System.ComponentModel
Imports System.Reflection

Public Class CoreMachinePropertyDescriptor
    Inherits PropertyDescriptor

    Private ReadOnly _propertyInfo As PropertyInfo
    Private ReadOnly _displayName As String

    Public Sub New(propertyName As String,
                   displayName As String)

        MyBase.New(propertyName, Nothing)

        _propertyInfo = GetType(Machine).GetProperty(propertyName)

        If _propertyInfo Is Nothing Then
            Throw New ArgumentException(
                "Unknown Machine property: " & propertyName)
        End If

        _displayName = displayName

    End Sub

    Public Overrides ReadOnly Property DisplayName As String
        Get
            Return _displayName
        End Get
    End Property



    Public Overrides ReadOnly Property Attributes As AttributeCollection
        Get
            Return New AttributeCollection(
           New CategoryAttribute("01 - Machine Configuration"))
        End Get
    End Property


    Public Overrides ReadOnly Property ComponentType As Type
        Get
            Return GetType(DynamicMachinePropertyBag)
        End Get
    End Property

    Public Overrides ReadOnly Property PropertyType As Type
        Get
            Return _propertyInfo.PropertyType
        End Get
    End Property

    Public Overrides ReadOnly Property IsReadOnly As Boolean
        Get
            Return Not _propertyInfo.CanWrite
        End Get
    End Property

    Public Overrides Function GetValue(component As Object) As Object

        Dim bag = TryCast(component, DynamicMachinePropertyBag)

        If bag Is Nothing OrElse bag.MachineRecord Is Nothing Then
            Return Nothing
        End If

        Return _propertyInfo.GetValue(bag.MachineRecord, Nothing)

    End Function

    Public Overrides Sub SetValue(component As Object, value As Object)

        Dim bag = TryCast(component, DynamicMachinePropertyBag)

        If bag Is Nothing OrElse bag.MachineRecord Is Nothing Then
            Return
        End If

        Dim convertedValue As Object =
            Convert.ChangeType(value, _propertyInfo.PropertyType)

        _propertyInfo.SetValue(
            bag.MachineRecord,
            convertedValue,
            Nothing)

        OnValueChanged(component, EventArgs.Empty)

    End Sub

    Public Overrides Function CanResetValue(component As Object) As Boolean
        Return False
    End Function

    Public Overrides Sub ResetValue(component As Object)
    End Sub

    Public Overrides Function ShouldSerializeValue(component As Object) As Boolean
        Return True
    End Function

End Class
