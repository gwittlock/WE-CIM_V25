Imports System.Data.OleDb
Imports System.IO
Imports System.Text.Json
Imports System.Text.Json.Serialization
Imports Newtonsoft.Json
Imports JsonSerializer = System.Text.Json.JsonSerializer

Public Module AccessToJsonExporter

    '''' Exports every table from an MDB to its own clean JSON file (one file per table)
    '''' Spaces are removed from field names (both in Columns and in Rows)
    '''' </summary>
    Public Sub ExportMdbToJson_OneFilePerTable(mdbPath As String, outputFolder As String)

        If Not File.Exists(mdbPath) Then Throw New FileNotFoundException("MDB not found", mdbPath)

        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};"

        ' Ensure output folder exists
        IO.Directory.CreateDirectory(outputFolder)

        Using conn As New OleDbConnection(connStr)
            conn.Open()

            Dim schemaTables = conn.GetOleDbSchemaTable(OleDbSchemaGuid.Tables,
                New Object() {Nothing, Nothing, Nothing, "TABLE"})

            For Each tblRow As DataRow In schemaTables.Rows
                Dim tableName As String = tblRow("TABLE_NAME").ToString()
                If tableName.StartsWith("MSys") OrElse tableName.Contains("~") Then Continue For

                ' Get columns in order
                Dim schemaCols = conn.GetOleDbSchemaTable(OleDbSchemaGuid.Columns,
                    New Object() {Nothing, Nothing, tableName})
                schemaCols.DefaultView.Sort = "ORDINAL_POSITION"
                schemaCols = schemaCols.DefaultView.ToTable()

                Dim columns As New List(Of ColumnDef)
                Dim fieldMap As New Dictionary(Of String, String) ' Original → Clean name

                For Each colRow As DataRow In schemaCols.Rows
                    Dim originalField = colRow("COLUMN_NAME").ToString()
                    Dim cleanField = originalField.Replace(" ", "")   ' <<< STRIP SPACES

                    ' Nice header (still human readable)
                    Dim header = Globalization.CultureInfo.CurrentCulture.TextInfo.ToTitleCase(
                        originalField.Replace("_", " ").ToLower())

                    columns.Add(New ColumnDef With {
                        .Field = cleanField,
                        .Header = header,
                        .Visible = True
                    })

                    fieldMap(originalField) = cleanField
                Next

                ' Read rows
                Dim rows As New List(Of Dictionary(Of String, Object))
                Using cmd As New OleDbCommand($"SELECT * FROM [{tableName}]", conn)
                    Using reader = cmd.ExecuteReader()
                        While reader.Read()
                            Dim row As New Dictionary(Of String, Object)
                            For Each kv In fieldMap
                                Dim value = reader(kv.Key)
                                row(kv.Value) = If(value Is DBNull.Value OrElse value Is Nothing, Nothing, value)
                            Next
                            rows.Add(row)
                        End While
                    End Using
                End Using

                ' Build final object for this table
                Dim tableObj = New With {
                    .TableName = tableName,
                    .Columns = columns,
                    .Rows = rows
                }

                ' Serialize this table only
                Dim options As New JsonSerializerOptions With {.WriteIndented = True}
                Dim json As String = JsonSerializer.Serialize(tableObj, options)

                ' Save as:  outputFolder\TableName.json
                'Dim safeFileName As String = String.Join("_", tableName.Split(IO.Path.GetInvalidFileNameChars()))
                Dim safeFileName As String = GetSafeFileName(tableName)
                Dim filePath As String = IO.Path.Combine(outputFolder, safeFileName & ".json")

                IO.Directory.CreateDirectory(IO.Path.GetDirectoryName(filePath))
                IO.File.WriteAllText(filePath, json)

                Console.WriteLine($"Exported: {filePath}")
            Next
        End Using

        MessageBox.Show("All tables exported successfully!" & vbCrLf & "One JSON file per table in:" & vbCrLf & outputFolder,
                        "Complete!", MessageBoxButtons.OK, MessageBoxIcon.Information)
    End Sub

    ' Clean table name: remove spaces and invalid filename characters
    Function GetSafeFileName(tableName As String) As String
        ' Remove all invalid filename characters
        Dim invalidChars = IO.Path.GetInvalidFileNameChars()
        Dim cleanName = New String(tableName.Where(Function(c) Not invalidChars.Contains(c)).ToArray())

        ' Replace spaces with underscores (or remove entirely if you prefer)
        cleanName = cleanName.Replace(" ", "")

        Return cleanName
    End Function
    Public Sub ExportMaterialJson(accessDbPath As String, outputFolder As String)
        ' ---------------------------
        ' Export Material Types
        ' ---------------------------

        Dim materialTypesPath = Path.Combine(dataFolder, "MaterialTypes.json")
        Dim materialInventoryPath = Path.Combine(dataFolder, "MaterialInventory.json")

        Using conn As New OleDb.OleDbConnection($"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={accessDbPath}")
            conn.Open()

            Dim materialTypesTable As New JsonTable(Of MaterialType)()
            Using cmd As New OleDb.OleDbCommand("SELECT ID, Description FROM [Material Types]", conn)
                Using reader = cmd.ExecuteReader()
                    While reader.Read()
                        materialTypesTable.Rows.Add(New MaterialType() With {
                            .ID = Convert.ToInt32(reader("ID")),
                            .Description = reader("Description").ToString(),
                            .Display = reader("Description").ToString(),  ' Can adjust if Display differs
                            .IsActive = True
                        })
                    End While
                End Using
            End Using

            ' Write to JSON
            File.WriteAllText(materialTypesPath, JsonConvert.SerializeObject(materialTypesTable, Formatting.Indented))
        End Using

        ' ---------------------------
        ' Export Material Inventory
        ' ---------------------------
        Using conn As New OleDb.OleDbConnection($"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={accessDbPath}")
            conn.Open()

            Dim materialInventoryTable As New JsonTable(Of MaterialSheet)()
            Using cmd As New OleDb.OleDbCommand("SELECT * FROM [Material Inventory]", conn)
                Using reader = cmd.ExecuteReader()
                    While reader.Read()
                        ' Build list of MaterialParameters from each column
                        Dim paramList As New List(Of MaterialParameter)()
                        For i As Integer = 0 To reader.FieldCount - 1
                            Dim colName = reader.GetName(i)
                            Dim value = If(IsDBNull(reader(i)), Nothing, reader(i))
                            Dim dataType As Integer = If(TypeOf value Is Integer, 1,
                                                    If(TypeOf value Is Decimal Or TypeOf value Is Double, 7,
                                                    If(TypeOf value Is Boolean, 4, 0))) ' 0=string

                            paramList.Add(New MaterialParameter() With {
                                .Name = colName,
                                .Display = colName,
                                .Value = value,
                                .DefaultValue = value,
                                .DataType = dataType,
                                .Visible = True
                            })
                        Next

                        ' Add sheet to MaterialInventoryTable
                        materialInventoryTable.Rows.Add(New MaterialSheet() With {
                            .ID = Convert.ToInt32(reader("ID")),
                            .TypeID = Convert.ToInt32(reader("Type ID")),
                            .Values = paramList
                        })
                    End While
                End Using
            End Using

            ' Write to JSON
            File.WriteAllText(materialInventoryPath, JsonConvert.SerializeObject(materialInventoryTable, Formatting.Indented))
        End Using

    End Sub


    ' Helper: map .NET type to your DataType integer code
    Private Function GetDataTypeCode(t As Type) As Integer
        If t Is GetType(String) Then Return 0
        If t Is GetType(Integer) Then Return 1
        If t Is GetType(Boolean) Then Return 4
        If t Is GetType(Double) OrElse t Is GetType(Decimal) Then Return 7
        ' Add other mappings if needed
        Return 0
    End Function



    Public Class ColumnDef
        Public Property Field As String
        Public Property Header As String
        Public Property Visible As Boolean
    End Class

    Public Class TableExport
        Public Property TableName As String
        Public Property Columns As List(Of ColumnDef)
        Public Property Rows As List(Of Dictionary(Of String, Object))
    End Class


End Module