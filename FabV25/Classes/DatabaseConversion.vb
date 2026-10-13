Namespace DatabaseConversion

    Public Class JsonTable
        Public Property TableName As String
        Public Property Columns As New List(Of String)
        Public Property Rows As New List(Of Object)
    End Class

    Public Class JsonRowWithValues
        Public Property ID As Integer
        Public Property TypeID As Integer
        Public Property Values As New List(Of JsonValue)
    End Class

    Public Class JsonValue
        Public Property Name As String
        Public Property Display As String
        Public Property Value As Object
        Public Property DataType As Integer
        Public Property DefaultValue As Object
        Public Property Visible As Boolean
    End Class

    Public NotInheritable Class MaterialInventoryConverter

        Private Sub New()
        End Sub

        Public Shared Function Convert(table As DataTable) As JsonTable

            Dim result As New JsonTable()
            result.TableName = table.TableName

            ' Columns (you keep this empty in your good JSON, but included for completeness)
            For Each col As DataColumn In table.Columns
                result.Columns.Add(col.ColumnName)
            Next

            ' Rows
            For Each row As DataRow In table.Rows

                Dim jsonRow As New JsonRowWithValues()

                jsonRow.ID = If(IsDBNull(row("ID")), 0, System.Convert.ToInt32(row("ID")))
                jsonRow.TypeID = If(IsDBNull(row("TypeID")), 0, System.Convert.ToInt32(row("TypeID")))
                For Each col As DataColumn In table.Columns

                    Dim rawValue As Object = row(col)
                    If rawValue Is DBNull.Value Then rawValue = Nothing

                    Dim valueItem As New JsonValue() With {
                        .Name = col.ColumnName,
                        .Display = col.ColumnName,
                        .Value = rawValue,
                        .DataType = MapDataType(col.DataType),
                        .DefaultValue = rawValue,
                        .Visible = True
                    }

                    jsonRow.Values.Add(valueItem)

                Next

                result.Rows.Add(jsonRow)

            Next

            Return result

        End Function

        Private Shared Function MapDataType(t As Type) As Integer
            If t Is GetType(String) Then Return 0
            If t Is GetType(Integer) OrElse t Is GetType(Long) OrElse t Is GetType(Short) Then Return 1
            If t Is GetType(Boolean) Then Return 2
            If t Is GetType(DateTime) Then Return 3
            If t Is GetType(Double) OrElse t Is GetType(Decimal) OrElse t Is GetType(Single) Then Return 7
            Return 0
        End Function

    End Class


    Public NotInheritable Class MaterialTypesConverter

        Private Sub New()
        End Sub

        Public Shared Function Convert(table As DataTable) As JsonTable

            Dim result As New JsonTable()
            result.TableName = table.TableName

            For Each col As DataColumn In table.Columns
                result.Columns.Add(col.ColumnName)
            Next

            For Each row As DataRow In table.Rows

                Dim jsonRow As New Dictionary(Of String, Object)

                For Each col As DataColumn In table.Columns
                    Dim val As Object = row(col)
                    If val Is DBNull.Value Then val = Nothing
                    jsonRow(col.ColumnName) = val
                Next

                result.Rows.Add(jsonRow)

            Next

            Return result

        End Function

    End Class
End Namespace
