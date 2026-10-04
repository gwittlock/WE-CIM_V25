Imports System.IO
Imports System.Runtime.InteropServices
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.ComponentModel

<TypeConverter(GetType(MaterialSheetTypeConverter))>
Public Class MaterialSheet
    '------------------------------
    '  MATERIAL SHEET CLASS

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

    End Class
