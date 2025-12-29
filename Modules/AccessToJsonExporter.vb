Imports System.Data.OleDb
Imports System.IO
Imports System.Text.Json
Imports System.Text.Json.Serialization
Imports System.Data
Imports System.Linq
Imports Newtonsoft.Json
Imports System.Data.DataSetExtensions
Imports JsonSerializer = System.Text.Json.JsonSerializer
Public Module AccessToJsonExporter
    Public Class ValueCell
        Public Property Name As String
        Public Property Display As String
        Public Property Value As Object
        Public Property DataType As Integer
        Public Property DefaultValue As Object
        Public Property Visible As Boolean
    End Class


    '''' Exports every table from an MDB to its own clean JSON file (one file per table)
    '''' Spaces are removed from field names (both in Columns and in Rows)
    '''' </summary>
    Private Sub ExportFlatTable(mdbPath As String, outputFolder As String, tableName As String, sJsonName As String)
        If Not File.Exists(mdbPath) Then Throw New FileNotFoundException("MDB not found", mdbPath)
        IO.Directory.CreateDirectory(outputFolder)

        Using conn As New OleDbConnection($"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};")
            conn.Open()

            ' Get columns
            Dim schemaCols = conn.GetOleDbSchemaTable(OleDbSchemaGuid.Columns, New Object() {Nothing, Nothing, tableName})
            schemaCols.DefaultView.Sort = "ORDINAL_POSITION"
            schemaCols = schemaCols.DefaultView.ToTable()

            ' Build column definitions
            Dim columns As New List(Of Object)
            For Each col As DataRow In schemaCols.Rows
                Dim colName = col("COLUMN_NAME").ToString()
                columns.Add(New With {
                .Field = colName.Replace(" ", ""),
                .Header = Globalization.CultureInfo.CurrentCulture.TextInfo.ToTitleCase(colName.Replace("_", " ").ToLower()),
                .Visible = True,
                .DataType = GetColumnDataType(col)
            })
            Next

            ' Read rows
            Dim rows As New List(Of Object)
            Using cmd As New OleDbCommand($"SELECT * FROM [{tableName}]", conn)
                Using reader = cmd.ExecuteReader()
                    While reader.Read()
                        Dim row As New Dictionary(Of String, Object)
                        For Each col As DataRow In schemaCols.Rows
                            Dim colName = col("COLUMN_NAME").ToString()
                            row(colName.Replace(" ", "")) = If(reader(colName) Is DBNull.Value, Nothing, reader(colName))
                        Next
                        rows.Add(row)
                    End While
                End Using
            End Using

            ' Build final table object
            Dim tableObj = New With {
            .TableName = tableName,
            .Columns = columns,
            .Rows = rows
        }

            ' Serialize
            Dim options As New JsonSerializerOptions With {.WriteIndented = True}
            Dim json As String = JsonSerializer.Serialize(tableObj, options)

            ' Save using sJsonName
            Dim filePath = IO.Path.Combine(outputFolder, sJsonName & ".json")
            IO.File.WriteAllText(filePath, json)

            Console.WriteLine($"Exported Flat Table: {filePath}")
        End Using
    End Sub




    Private sJsonName As String
    Public Sub ExportAllTables(mdbPath As String, outputFolder As String)
        If Not File.Exists(mdbPath) Then
            Throw New FileNotFoundException("MDB not found", mdbPath)
        End If

        ' Ensure output folder exists
        IO.Directory.CreateDirectory(outputFolder)

        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};"

        Using conn As New OleDbConnection(connStr)
            conn.Open()

            ' Get all user tables
            Dim schemaTables = conn.GetOleDbSchemaTable(OleDbSchemaGuid.Tables,
            New Object() {Nothing, Nothing, Nothing, "TABLE"})

            For Each tblRow As DataRow In schemaTables.Rows
                Dim tableName As String = tblRow("TABLE_NAME").ToString()

                ' Skip system tables
                If tableName.StartsWith("MSys") OrElse tableName.Contains("~") Then Continue For

                ' Check if this table is in our 18-table map
                If Not IsValueTableByName(tableName) AndAlso String.IsNullOrEmpty(sJsonName) Then
                    ' Table not in map → skip
                    Console.WriteLine($"Skipping table: {tableName}")
                    Continue For
                End If

                ' Export according to type
                If IsValueTableByName(tableName) Then
                    ' Value table
                    ExportValueTable(mdbPath, outputFolder, tableName, sJsonName)
                Else
                    ' Flat table
                    ExportFlatTable(mdbPath, outputFolder, tableName, sJsonName)
                End If
            Next
        End Using

        MessageBox.Show("Export complete!" & vbCrLf &
                    "JSON files written to: " & outputFolder,
                    "Complete", MessageBoxButtons.OK, MessageBoxIcon.Information)
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
    ' Module-level dictionary to define Value Tables and their key columns
    Private valueTableKeyColumns As New Dictionary(Of String, (IDCol As String, TypeIDCol As String)) From {
    {"Material Inventory", ("ID", "Type ID")},
    {"Tool Crib", ("ID", "Type ID")},
    {"Machine Tool Types", ("ID", "ToolTypeID")},
    {"Station Configuration", ("ID", "Type ID")},
    {"Tool Setup Members", ("ID", "Type ID")}
}

    Private Sub ExportValueTable(mdbPath As String, outputFolder As String, tableName As String, sJsonName As String)
        If Not File.Exists(mdbPath) Then Throw New FileNotFoundException("MDB not found", mdbPath)

        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};"
        IO.Directory.CreateDirectory(outputFolder)

        Using conn As New OleDbConnection(connStr)
            conn.Open()

            ' Get columns
            Dim schemaCols = conn.GetOleDbSchemaTable(OleDbSchemaGuid.Columns,
                    New Object() {Nothing, Nothing, tableName})
            schemaCols.DefaultView.Sort = "ORDINAL_POSITION"
            schemaCols = schemaCols.DefaultView.ToTable()

            ' Read rows
            Dim rows As New List(Of Object)
            Using cmd As New OleDbCommand($"SELECT * FROM [{tableName}]", conn)
                Using reader = cmd.ExecuteReader()
                    While reader.Read()
                        Dim rowObj As New Dictionary(Of String, Object)

                        ' Identify top-level columns: ID + TypeID (normalized)
                        If schemaCols.Rows.Cast(Of DataRow)().Any(Function(c) c("COLUMN_NAME").ToString().Equals("ID", StringComparison.OrdinalIgnoreCase)) Then
                            rowObj("ID") = reader("ID")
                        End If
                        If schemaCols.Rows.Cast(Of DataRow)().Any(Function(c) c("COLUMN_NAME").ToString().Equals("Type ID", StringComparison.OrdinalIgnoreCase)) Then
                            rowObj("TypeID") = reader("Type ID")
                        End If

                        ' Build Values array
                        Dim valuesList As New List(Of Object)
                        For Each col As DataRow In schemaCols.Rows
                            Dim cName = col("COLUMN_NAME").ToString()
                            Dim normalizedName As String = If(cName.Equals("Type ID", StringComparison.OrdinalIgnoreCase), "TypeID", cName.Replace(" ", ""))
                            Dim cValue = If(reader(cName) Is DBNull.Value, Nothing, reader(cName))
                            valuesList.Add(New With {
                            .Name = normalizedName,
                            .Display = cName,
                            .Value = cValue,
                            .DataType = GetColumnDataType(col),
                            .DefaultValue = cValue,
                            .Visible = True
                        })
                        Next
                        rowObj("Values") = valuesList
                        rows.Add(rowObj)
                    End While
                End Using
            End Using

            ' Build table object
            Dim tableObj = New With {
            .TableName = tableName,
            .Columns = New List(Of Object), ' empty, handled by Values
            .Rows = rows
        }

            ' Serialize
            Dim options As New JsonSerializerOptions With {.WriteIndented = True}
            Dim json As String = JsonSerializer.Serialize(tableObj, options)

            ' Save JSON
            Dim filePath As String = IO.Path.Combine(outputFolder, sJsonName & ".json")
            IO.File.WriteAllText(filePath, json)

            Console.WriteLine($"Exported Value Table: {filePath}")
        End Using
    End Sub


    ' Helper to get a simple DataType number for JSON
    Private Function GetColumnDataType(col As DataRow) As Integer
        ' Adjust based on your mapping: 1=int, 7=double, 2=bool, 0=string
        Dim typeName = col("DATA_TYPE") ' OleDb type number
        Select Case Convert.ToInt32(typeName)
            Case 2, 3 : Return 1      ' SmallInt, Integer
            Case 4, 5, 6 : Return 7  ' Single, Double, Currency
            Case 11 : Return 2        ' Boolean
            Case Else : Return 0      ' String/Other
        End Select
    End Function




    Private Function ToDisplayName(fieldName As String) As String
        Return Globalization.CultureInfo.CurrentCulture.TextInfo.
        ToTitleCase(fieldName.Replace("_", " ").ToLower())
    End Function

    Private Function MapOleDbTypeToDataType(dataType As Integer) As Integer

        Select Case CType(dataType, OleDbType)

            Case OleDbType.Integer, OleDbType.SmallInt
                Return 1   ' Int

            Case OleDbType.Double, OleDbType.Single, OleDbType.Decimal, OleDbType.Currency
                Return 7   ' Double

            Case OleDbType.Boolean
                Return 2   ' Bool

            Case Else
                Return 0   ' String

        End Select

    End Function

    Private Function IsValueTable(schemaCols As DataTable) As Boolean

        Dim hasID As Boolean = False
        Dim hasTypeID As Boolean = False

        For Each colRow As DataRow In schemaCols.Rows
            Dim colName = colRow("COLUMN_NAME").ToString()

            If colName.Equals("ID", StringComparison.OrdinalIgnoreCase) Then
                hasID = True
            ElseIf colName.Equals("Type ID", StringComparison.OrdinalIgnoreCase) Then
                hasTypeID = True
            End If
        Next

        Return hasID AndAlso hasTypeID
    End Function



    Public Sub ExportDatabaseToJson(mdbPath As String, outputFolder As String)
        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

        Using conn As New OleDbConnection(connStr)
            conn.Open()

            Dim tables = conn.GetOleDbSchemaTable(OleDbSchemaGuid.Tables, Nothing)

            For Each row As DataRow In tables.Rows

                Dim tableName = row("TABLE_NAME").ToString()
                If tableName.StartsWith("MSys") Then Continue For

                Dim schema = conn.GetOleDbSchemaTable(
                OleDbSchemaGuid.Columns,
                New Object() {Nothing, Nothing, tableName, Nothing}
            )

                schema.DefaultView.Sort = "ORDINAL_POSITION"
                schema = schema.DefaultView.ToTable()

                If IsValueTableByName(tableName) Then
                    Debug.WriteLine("We are exporting table <" & tableName & "> as Value table")


                End If
                If IsValueTableByName(tableName) Then


                    ExportValueTable(mdbPath, outputFolder, tableName, sJsonName)
                Else
                    ExportFlatTable(mdbPath, outputFolder, tableName, sJsonName)
                End If


            Next
        End Using
    End Sub


    Private Function IsValueTableByName(tableName As String) As Boolean
        Select Case tableName
            Case "Material Inventory"
                sJsonName = "MaterialInventory"
                Return True

            Case "Tool Crib"
                sJsonName = "toolcrib"
                Return True

            Case "Machine Tool Types"
                sJsonName = "MachineToolTypes"
                Return True

            Case "Station Configuration"
                sJsonName = "Stations"
                Return True

            Case "Tool Setup Members"
                sJsonName = "ToolSetupMembers"
                Return True

            Case "Directives"
                sJsonName = "Directives"
                Return False

            Case "Layer Setups"
                sJsonName = "LayerSetups"
                Return False

            Case "Machine Attributes"
                sJsonName = "MachineAttributes"
                Return False

            Case "Machine Attributes Type"
                sJsonName = "MachineAttributeTypes"
                Return False

            Case "Machines"
                sJsonName = "Machines"
                Return False

            Case "Machine Tools"
                sJsonName = "MachineTools"
                Return False

            Case "Machine Type Attributes"
                sJsonName = "MachineTypeAttributes"
                Return False

            Case "Machine Types"
                sJsonName = "MachineTypes"
                Return False

            Case "Material Types"
                sJsonName = "MaterialTypes"
                Return False

            Case "Tool Attribute Types"
                sJsonName = "ToolAttributeTypes"
                Return False

            Case "Tool Setups"
                sJsonName = "ToolSetups"
                Return False

            Case "Tool Type Attributes"
                sJsonName = "ToolTypeAttributes"
                Return False

            Case "Tool Types"
                sJsonName = "ToolTypes"
                Return False
            Case Else
                sJsonName = Nothing
                Return Nothing


        End Select
    End Function


End Module