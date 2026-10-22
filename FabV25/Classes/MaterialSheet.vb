Imports System.IO
Imports System.Runtime.InteropServices
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.ComponentModel
<JsonObject()>
<TypeConverter(GetType(MaterialSheetTypeConverter))>
Public Class MaterialSheet
    Implements ICustomTypeDescriptor    '  MATERIAL SHEET CLASS

    ' JSON-deserialized fields
    Public Property X As Double
    Public Property Y As Double
    Public Property ID As Integer
    Public Property TypeID As Integer
    Public Property Values As New List(Of MaterialParameter)

    ' Computed properties for common fields
    <JsonIgnore(), Browsable(True)>
    Public Property Description As String
        Get
            Return GetValue("Description")?.ToString()
        End Get
        Set(value As String)
            SetValue("Description", value)
        End Set
    End Property

    <JsonIgnore(), Browsable(True)>
    Public Property Length As Decimal
        Get
            Dim v = GetValue("Length")
            Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
        End Get
        Set(value As Decimal)
            SetValue("Length", value)
        End Set
    End Property

    <JsonIgnore(), Browsable(True)>
    Public Property Width As Decimal
        Get
            Dim v = GetValue("Width")
            Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
        End Get
        Set(value As Decimal)
            SetValue("Width", value)
        End Set
    End Property

    <JsonIgnore(), Browsable(True)>
    Public Property Thickness As Decimal
        Get
            Dim v = GetValue("Thickness")
            Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
        End Get
        Set(value As Decimal)
            SetValue("Thickness", value)
        End Set
    End Property

    <JsonIgnore(), Browsable(True)>
    Public Property Weight As Decimal
        Get
            Dim v = GetValue("Weight")
            Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
        End Get
        Set(value As Decimal)
            SetValue("Weight", value)
        End Set
    End Property


    <JsonIgnore(), Browsable(True)>
    Public Property Cost_Per_Unit As Decimal
        Get
            Dim v = GetValue("Cost Per Unit")
            Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
        End Get
        Set(value As Decimal)
            SetValue("Cost Per Unit", value)
        End Set
    End Property


    <JsonIgnore(), Browsable(True)>
    Public Property Quantity As Integer
        Get
            Dim v = GetValue("Quantity")
            Return If(v IsNot Nothing, Convert.ToInt32(v), 0)
        End Get
        Set(value As Integer)
            SetValue("Quantity", value)
        End Set
    End Property

    ' Helper to get a value from Values
    Private Function GetValue(name As String) As Object
        Dim param = Values.FirstOrDefault(Function(p) p.Name.Equals(name, StringComparison.OrdinalIgnoreCase))
        Return If(param IsNot Nothing, param.Value, Nothing)
    End Function

    ' Helper to set a value in Values
    Private Sub SetValue(name As String, val As Object)
        Dim param = Values.FirstOrDefault(Function(p) p.Name.Equals(name, StringComparison.OrdinalIgnoreCase))
        If param IsNot Nothing Then
            param.Value = val
        Else
            ' Add new param if missing
            Values.Add(New MaterialParameter With {
            .Name = name,
            .Display = name,
            .Value = val,
            .DataType = If(TypeOf val Is String, 0, If(TypeOf val Is Boolean, 4, 7)),
            .DefaultValue = val
        })
        End If
    End Sub


    Public Function GetProperties(
    attributes() As Attribute) As PropertyDescriptorCollection _
    Implements ICustomTypeDescriptor.GetProperties

        Dim props As PropertyDescriptor() = {
            New MaterialSheetPropertyDescriptor("Description", "Description"),
            New MaterialSheetPropertyDescriptor("Length", "Length"),
            New MaterialSheetPropertyDescriptor("Width", "Width"),
            New MaterialSheetPropertyDescriptor("Thickness", "Thickness"),
            New MaterialSheetPropertyDescriptor("Weight", "Weight"),
            New MaterialSheetPropertyDescriptor("Cost_Per_Unit", "Cost Per Unit"),
            New MaterialSheetPropertyDescriptor("Quantity", "Quantity")
        }

        Return New PropertyDescriptorCollection(props, True)

    End Function

    Public Function GetAttributes() As AttributeCollection _
    Implements ICustomTypeDescriptor.GetAttributes
        Return TypeDescriptor.GetAttributes(Me, True)
    End Function

    Public Function GetClassName() As String _
        Implements ICustomTypeDescriptor.GetClassName
        Return TypeDescriptor.GetClassName(Me, True)
    End Function

    Public Function GetComponentName() As String _
        Implements ICustomTypeDescriptor.GetComponentName
        Return TypeDescriptor.GetComponentName(Me, True)
    End Function

    Public Function GetConverter() As TypeConverter _
        Implements ICustomTypeDescriptor.GetConverter
        Return TypeDescriptor.GetConverter(Me, True)
    End Function

    Public Function GetDefaultEvent() As EventDescriptor _
        Implements ICustomTypeDescriptor.GetDefaultEvent
        Return TypeDescriptor.GetDefaultEvent(Me, True)
    End Function

    Public Function GetDefaultProperty() As PropertyDescriptor _
        Implements ICustomTypeDescriptor.GetDefaultProperty
        Return Nothing
    End Function

    Public Function GetEditor(editorBaseType As Type) As Object _
        Implements ICustomTypeDescriptor.GetEditor
        Return TypeDescriptor.GetEditor(Me, editorBaseType, True)
    End Function

    Public Function GetEvents() As EventDescriptorCollection _
        Implements ICustomTypeDescriptor.GetEvents
        Return TypeDescriptor.GetEvents(Me, True)
    End Function

    Public Function GetEvents(attributes() As Attribute) As EventDescriptorCollection _
        Implements ICustomTypeDescriptor.GetEvents
        Return TypeDescriptor.GetEvents(Me, attributes, True)
    End Function

    Public Function GetProperties() As PropertyDescriptorCollection _
        Implements ICustomTypeDescriptor.GetProperties
        Return GetProperties(Nothing)
    End Function

    Public Function GetPropertyOwner(pd As PropertyDescriptor) As Object _
        Implements ICustomTypeDescriptor.GetPropertyOwner
        Return Me
    End Function

End Class
