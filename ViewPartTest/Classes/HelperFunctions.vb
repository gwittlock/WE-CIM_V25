Imports Newtonsoft.Json.Linq

Public Class HelperFunctions
    Public Class JsonColumnDef
        Public Property Field As String
        Public Property Header As String
        Public Property Visible As Boolean = True
        Public Property DataType As Integer? = Nothing

        Public Sub New(field As String, header As String,
                   Optional visible As Boolean = True,
                   Optional dataType As Integer? = Nothing)
            Me.Field = field
            Me.Header = header
            Me.Visible = visible
            Me.DataType = dataType
        End Sub
    End Class

    ''' <summary>
    ''' Builds a JSON Columns array from a DataTable.
    ''' Each column will have Field, Header, Visible (True), and optional DataType (0 by default).
    ''' </summary>
    Public Function BuildColumns(dt As DataTable, Optional includeDataType As Boolean = True) As JArray
        Dim columns As New JArray()

        For Each col As DataColumn In dt.Columns
            Dim colObj As New JObject()
            colObj("Field") = col.ColumnName.Replace(" ", "")
            colObj("Header") = col.ColumnName
            colObj("Visible") = True
            If includeDataType Then
                colObj("DataType") = 0
            End If
            columns.Add(colObj)
        Next

        Return columns
    End Function

End Class
