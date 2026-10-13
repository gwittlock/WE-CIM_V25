Imports System.ComponentModel

' ===============================================
' DynamicMachineProperty
' ===============================================
Public Class DynamicMachineProperty
    Public Property Attribute As MachineAttribute
    Public Property TypeDef As MachineAttributeType
    Public Property Value As Object

    Public Sub New(attr As MachineAttribute, typeDef As MachineAttributeType)
        Attribute = attr
        Me.TypeDef = typeDef   ' ✅ fixed assignment

        ' Defensive fallback if either attr or typeDef is Nothing
        If Me.TypeDef Is Nothing OrElse attr Is Nothing OrElse attr.Value Is Nothing Then
            Value = If(attr?.Value, "")
            Return
        End If

        ' Convert string value to correct type
        Dim s As String = attr.Value

        Select Case Me.TypeDef.DataType
            Case 1 ' Integer
                Dim i As Integer
                If Integer.TryParse(s, i) Then
                    Value = i
                Else
                    Value = 0
                End If

            Case 7 ' Double
                Dim d As Double
                If Double.TryParse(s, d) Then
                    Value = d
                Else
                    Value = 0.0
                End If

            Case 8 ' Boolean
                Dim b As Boolean
                If Boolean.TryParse(s, b) Then
                    Value = b
                Else
                    ' Handle "0"/"1" as boolean
                    If s = "1" Then
                        Value = True
                    Else
                        Value = False
                    End If
                End If

            Case Else
                Value = s
        End Select
    End Sub
End Class

' ===============================================
' DynamicMachinePropertyDescriptor
' ===============================================
Public Class DynamicMachinePropertyDescriptor
    Inherits PropertyDescriptor

    Public _prop As DynamicMachineProperty

    ' VB.NET-compatible constructor: must call base first
    Public Sub New(prop As DynamicMachineProperty)
        MyBase.New("Unknown", Nothing) ' literal name for base
        _prop = prop
    End Sub

    ' Override DisplayName to compute it dynamically
    Public Overrides ReadOnly Property DisplayName As String
        Get
            If _prop Is Nothing Then Return "Unknown"
            If _prop.TypeDef IsNot Nothing AndAlso Not String.IsNullOrEmpty(_prop.TypeDef.Display) Then
                Return _prop.TypeDef.Display
            End If
            If _prop.Attribute IsNot Nothing Then
                Return "AttrID-" & _prop.Attribute.ID.ToString()
            End If
            Return "Unknown"
        End Get
    End Property

    ' --- Rest of PropertyDescriptor overrides ---
    Public Overrides Function CanResetValue(component As Object) As Boolean
        Return False
    End Function

    Public Overrides ReadOnly Property ComponentType As Type
        Get
            Return GetType(DynamicMachinePropertyBag)
        End Get
    End Property

    Public Overrides Function GetValue(component As Object) As Object
        Return If(_prop?.Value, Nothing)
    End Function

    Public Overrides ReadOnly Property IsReadOnly As Boolean
        Get
            Return False
        End Get
    End Property

    Public Overrides ReadOnly Property PropertyType As Type
        Get
            Dim dt = _prop?.TypeDef?.DataType
            If dt Is Nothing Then
                Return GetType(String)
            End If

            Select Case dt
                Case 1 : Return GetType(Integer)
                Case 7 : Return GetType(Double)
                Case 8 : Return GetType(Boolean)
                Case 9 : Return GetType(String)
                Case Else : Return GetType(String)
            End Select
        End Get
    End Property

    Public Overrides Sub ResetValue(component As Object)
    End Sub

    Public Overrides Sub SetValue(component As Object, value As Object)
        If _prop Is Nothing Then Return
        _prop.Value = value
        If _prop.Attribute IsNot Nothing Then
            _prop.Attribute.Value = If(value, "").ToString()
        End If
    End Sub

    Public Overrides Function ShouldSerializeValue(component As Object) As Boolean
        Return True
    End Function

    Public Overrides ReadOnly Property Attributes As AttributeCollection
        Get
            Dim attrs As New List(Of Attribute)
            If _prop?.TypeDef?.DataType = 9 AndAlso _prop.TypeDef.Options IsNot Nothing Then
                attrs.Add(New TypeConverterAttribute(GetType(DropDownListConverter)))
            End If
            Return New AttributeCollection(attrs.ToArray())
        End Get
    End Property
End Class
