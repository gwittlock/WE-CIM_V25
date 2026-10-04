Imports System.Data.OleDb
Imports System.IO
Imports System.Data
Imports System.Linq
Imports Newtonsoft.Json
Imports System.Data.DataSetExtensions
Imports Newtonsoft.Json.Linq
Imports Newtonsoft.Json.JsonConverter
Imports FabV25_WIN8.AppData
Imports System.Text.Json
Imports FabV25_WIN8.HelperFunctions
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models

Public Module AccessToJsonExporter
    Public Class ValueCell
        Public Property Name As String
        Public Property Display As String
        Public Property Value As Object
        Public Property DataType As Integer
        Public Property DefaultValue As Object
        Public Property Visible As Boolean
    End Class

    Public Enum TableExportType
        Value
        Hybrid
        Hierarchical
        HybridProjected
        Ignore
        ValueProjected
        MaterialInventory
        Directives
        LayerSetups
        MacAtt
        MacAttType
        Machines
        MachineTools
        MachineToolTypes
        MachineTypeAttributes
        MachineType
        Stations
        ToolAttributeTypes
        ToolCrib
        ToolSetupMembers
        ToolTypes
        ToolSetups
        ToolTypeAttributes
    End Enum
    Public sExportType As String

    Private Sub ExportLayerSetupsWithLayers(
    mdbPath As String,
    outputFolder As String
)

        Dim parentRows As New List(Of Dictionary(Of String, Object))
        Dim childRows As New List(Of Dictionary(Of String, Object))

        Using conn As New OleDb.OleDbConnection(
        $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
    )
            conn.Open()

            ' --------------------------------------
            ' Load Layer Setups (parent)
            ' --------------------------------------
            Using cmd As New OleDb.OleDbCommand("SELECT * FROM [Layer Setups]", conn)
                Using rdr = cmd.ExecuteReader()
                    While rdr.Read()
                        Dim row As New Dictionary(Of String, Object)
                        For i = 0 To rdr.FieldCount - 1
                            Dim colName = rdr.GetName(i)
                            row(colName.Replace(" ", "")) =
                            If(IsDBNull(rdr(i)), Nothing, rdr(i))
                        Next
                        parentRows.Add(row)
                    End While
                End Using
            End Using

            ' --------------------------------------
            ' Load Directives (children)
            ' --------------------------------------
            Using cmd As New OleDb.OleDbCommand("SELECT * FROM [Directives]", conn)
                Using rdr = cmd.ExecuteReader()
                    While rdr.Read()
                        Dim row As New Dictionary(Of String, Object)
                        For i = 0 To rdr.FieldCount - 1
                            Dim colName = rdr.GetName(i)
                            row(colName.Replace(" ", "")) =
                            If(IsDBNull(rdr(i)), Nothing, rdr(i))
                        Next
                        childRows.Add(row)
                    End While
                End Using
            End Using
        End Using

        ' --------------------------------------
        ' Attach Directives as Layers
        ' --------------------------------------
        For Each parent In parentRows

            Dim parentID As Integer = Convert.ToInt32(parent("ID"))

            Dim layers = childRows.
            Where(Function(c) Convert.ToInt32(c("LayerSetupID")) = parentID).
            Select(Function(c)
                       ' Keep LayerSetupID (matches good JSON)
                       Return New Dictionary(Of String, Object)(c)
                   End Function).
            ToList()

            parent("Layers") = layers
        Next

        ' --------------------------------------
        ' Build final JSON root
        ' --------------------------------------
        Dim root As New Dictionary(Of String, Object) From {
        {"TableName", "Layer Setups"},
        {"Rows", parentRows}
    }

        ' --------------------------------------
        ' Write JSON
        ' --------------------------------------
        Dim json = Newtonsoft.Json.JsonConvert.SerializeObject(
        root,
        Newtonsoft.Json.Formatting.Indented
    )

        Dim outPath = IO.Path.Combine(outputFolder, "LayerSetups.json")
        IO.File.WriteAllText(outPath, json)

    End Sub





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
            Dim json As String = System.Text.Json.JsonSerializer.Serialize(tableObj, options)

            ' Save using sJsonName
            Dim filePath = IO.Path.Combine(outputFolder, sJsonName & ".json")
            IO.File.WriteAllText(filePath, json)

            Console.WriteLine($"Exported Flat Table: {filePath}")
        End Using
    End Sub

    '    Private Function GetTableExportTypea(
    '    tableName As String,
    '    ByRef jsonName As String,
    '    ByRef parentTable As String,
    '    ByRef childTable As String,
    '    ByRef parentPK As String,
    '    ByRef childFK As String,
    '    ByRef childrenPropertyName As String
    ') As TableExportType

    '        Select Case tableName
    '        ' ------------------------------
    '        ' Material Inventory
    '        ' ------------------------------
    '            Case "Material Inventory"
    '                jsonName = "MaterialInventory"
    '                Return TableExportType.Value

    '        ' ------------------------------
    '        ' Directives
    '        ' ------------------------------
    '            Case "Directives"
    '                jsonName = "Directives"
    '                Return TableExportType.Value

    '        ' ------------------------------
    '        ' Layer Setups
    '        ' ------------------------------
    '            Case "Layer Setups"
    '                jsonName = "LayerSetups"
    '                parentTable = "Layer Setups"
    '                childTable = "Directives"
    '                parentPK = "ID"
    '                childFK = "Layer Setup ID"
    '                childrenPropertyName = "Layers"
    '                Return TableExportType.HybridProjected

    '        ' ------------------------------
    '        ' Machines
    '        ' ------------------------------
    '            Case "Machines"
    '                jsonName = "Machines"
    '                Return TableExportType.Value

    '        ' ------------------------------
    '        ' Machine Attributes
    '        ' ------------------------------
    '            Case "Machine Attributes"
    '                jsonName = "MachineAttributes"
    '                Return TableExportType.Value

    '                ' ------------------------------
    '                ' Default case: try simple export
    '                ' ------------------------------
    '            Case Else
    '                jsonName = tableName.Replace(" ", "")
    '                Return TableExportType.Value
    '        End Select
    '        '^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    Private Function GetTableExportType(
    tableName As String,
    ByRef jsonName As String,
    ByRef parentTable As String,
    ByRef childTable As String,
    ByRef parentPK As String,
    ByRef childFK As String,
    ByRef childrenPropertyName As String,
    ByRef fieldMap As Dictionary(Of String, String)
) As TableExportType

        Select Case tableName
        ' ---------------------------
        ' Value Tables
        ' ---------------------------
            Case "Tool Attribute Types"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.ToolAttributeTypes

            Case "Machine Tool Types"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.MachineToolTypes

            Case "Station Configuration"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.Stations

            Case "Tool Crib"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.ToolCrib

            Case "Material Types", "Tool Attribute Types"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.Value

            Case "Tool Type Attributes"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.ToolTypeAttributes

            Case "Machine Attribute Types"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.MacAttType

            Case "Material Inventory"
                jsonName = "MaterialInventory"
                Return TableExportType.MaterialInventory
        ' ---------------------------
        ' Flat Tables (still Value export)
        ' ---------------------------
            Case "Machine Attributes"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.MacAtt

            Case "Machines"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.Machines

            Case "Machine Tools"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.MachineTools

            Case "Machine Types"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.MachineType

            Case "Machine Type Attributes"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.MachineTypeAttributes

            Case "Tool Setups"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.ToolSetups

            Case "Tool Types"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.ToolTypes

            Case "Machine Tool Types"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.MachineToolTypes

        ' ---------------------------
        ' Hybrid Tables
        ' ---------------------------
            Case "Tool Setup Members"
                jsonName = tableName.Replace(" ", "")
                fieldMap = Nothing
                Return TableExportType.ToolSetupMembers

        ' ---------------------------
        ' Hierarchical / HybridProjected Tables
        ' ---------------------------
            Case "Layer Setups"
                jsonName = "LayerSetups"
                parentTable = "Layer Setups"
                childTable = "Directives"
                parentPK = "ID"
                childFK = "Layer Setup ID"
                childrenPropertyName = "Layers"

                Return TableExportType.LayerSetups

            Case "Directives"
                jsonName = "Directives"
                Return TableExportType.Directives

        ' ---------------------------
        ' Ignore Tables (do not export)
        ' ---------------------------
            Case "SomeLegacyTable", "UnusedDemoData"
                fieldMap = Nothing
                Return TableExportType.Ignore

                ' ---------------------------
                ' Default case: ignore unknown tables
                ' ---------------------------
            Case Else
                fieldMap = Nothing
                Return TableExportType.Ignore
        End Select

    End Function

    Public Sub ExportMachineTypes(mdbPath As String, outputFolder As String)
        Try
            Dim connString As String =
            $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

            ' Initialize JSON root
            Dim outputJson As New JObject
            outputJson("TableName") = "Machine Types"

            ' Columns
            outputJson("Columns") = New JArray(
            New JObject From {
                {"Field", "ID"},
                {"Header", "Id"},
                {"Visible", True}
            },
            New JObject From {
                {"Field", "Description"},
                {"Header", "Description"},
                {"Visible", True}
            },
            New JObject From {
                {"Field", "Display"},
                {"Header", "Display"},
                {"Visible", True}
            }
        )

            ' Rows
            Dim rows As New JArray
            outputJson("Rows") = rows

            Using conn As New OleDbConnection(connString)
                conn.Open()

                Using cmd As New OleDbCommand(
                "SELECT ID, Description, Display FROM [Machine Types]",
                conn
            )
                    Using reader As OleDbDataReader = cmd.ExecuteReader()
                        While reader.Read()
                            Dim row As New JObject

                            row("ID") =
                            If(IsDBNull(reader("ID")),
                               JValue.CreateNull(),
                               JToken.FromObject(reader("ID")))

                            row("Description") =
                            If(IsDBNull(reader("Description")),
                               JValue.CreateNull(),
                               JToken.FromObject(reader("Description")))

                            row("Display") =
                            If(IsDBNull(reader("Display")),
                               JValue.CreateNull(),
                               JToken.FromObject(reader("Display")))

                            rows.Add(row)
                        End While
                    End Using
                End Using
            End Using

            ' Write file
            Dim outputPath As String = Path.Combine(outputFolder, "MachineTypes.json")
            File.WriteAllText(outputPath, outputJson.ToString())

        Catch ex As Exception
            Throw New Exception("Failed to export table Machine Types.", ex)
        End Try
    End Sub



    Private Function NormalizeFieldName(name As String) As String
        If String.IsNullOrWhiteSpace(name) Then Return String.Empty

        Return name.
        Replace(" ", "").
        Replace("_", "").
        Replace("-", "").
        Trim().
        ToUpperInvariant()
    End Function


    Private Sub ExportMaterialInventoryTable(
    mdbPath As String,
    outputFolder As String
)

        Dim tableName As String = "Material Inventory"
        Dim jsonName As String = "MaterialInventory"

        Dim dt As DataTable = LoadTableFromMDB(mdbPath, tableName)

        Dim root As New JObject From {
        {"TableName", Nothing},
        {"Columns", New JArray()},
        {"Rows", New JArray()}
    }

        Dim rowsArray As JArray = CType(root("Rows"), JArray)

        For Each row As DataRow In dt.Rows

            Dim rowObj As New JObject()

            ' --- Required identity fields ---
            rowObj("ID") = JToken.FromObject(row("ID"))

            If dt.Columns.Contains("Type ID") Then
                rowObj("TypeID") = JToken.FromObject(row("Type ID"))
            Else
                rowObj("TypeID") = JToken.FromObject(row("TypeID"))
            End If

            ' --- Values projection ---
            Dim valuesArray As New JArray()

            For Each col As DataColumn In dt.Columns

                Dim valueObj As New JObject()
                Dim rawValue As Object = row(col)
                Dim safeValue As Object = If(rawValue Is DBNull.Value, Nothing, rawValue)

                valueObj("Name") = col.ColumnName
                valueObj("Display") = col.ColumnName
                valueObj("Value") = JToken.FromObject(safeValue)
                valueObj("DataType") = GetValueDataType(col.DataType)
                valueObj("DefaultValue") = JToken.FromObject(safeValue)
                valueObj("Visible") = True

                valuesArray.Add(valueObj)

            Next

            rowObj("Values") = valuesArray
            rowsArray.Add(rowObj)

        Next

        Dim outputPath As String = Path.Combine(outputFolder, jsonName & ".json")

        File.WriteAllText(
        outputPath,
        JsonConvert.SerializeObject(root, Formatting.Indented)
    )

    End Sub




    Private Sub ExportValueProjectedTable(
    mdbPath As String,
    outputFolder As String,
    tableName As String,
    jsonName As String
)

        Dim dt As DataTable = LoadTableFromMDB(mdbPath, tableName)

        Dim root As New JObject From {
        {"TableName", Nothing},
        {"Columns", New JArray()},
        {"Rows", New JArray()}
    }

        Dim rowsArray As JArray = CType(root("Rows"), JArray)

        For Each row As DataRow In dt.Rows

            Dim rowObj As New JObject()

            ' ---------------------------
            ' Required identity fields
            ' ---------------------------
            If dt.Columns.Contains("ID") Then
                rowObj("ID") = JToken.FromObject(row("ID"))
            End If

            If dt.Columns.Contains("Type ID") Then
                rowObj("TypeID") = JToken.FromObject(row("Type ID"))
            ElseIf dt.Columns.Contains("TypeID") Then
                rowObj("TypeID") = JToken.FromObject(row("TypeID"))
            End If

            ' ---------------------------
            ' Values projection
            ' ---------------------------
            Dim valuesArray As New JArray()

            For Each col As DataColumn In dt.Columns

                Dim valueObj As New JObject()

                Dim rawValue As Object = row(col)
                Dim safeValue As Object = If(rawValue Is DBNull.Value, Nothing, rawValue)

                valueObj("Name") = col.ColumnName
                valueObj("Display") = col.ColumnName
                valueObj("Value") = JToken.FromObject(safeValue)
                valueObj("DataType") = GetValueDataType(col.DataType)
                valueObj("DefaultValue") = JToken.FromObject(safeValue)
                valueObj("Visible") = True

                valuesArray.Add(valueObj)

            Next

            rowObj("Values") = valuesArray
            rowsArray.Add(rowObj)

        Next

        Dim outputPath As String = Path.Combine(outputFolder, jsonName & ".json")

        File.WriteAllText(
        outputPath,
        JsonConvert.SerializeObject(root, Formatting.Indented)
    )

    End Sub

    Private Function GetValueDataType(t As Type) As Integer
        If t Is GetType(String) Then Return 0
        If t Is GetType(Integer) OrElse t Is GetType(Int32) Then Return 1
        If t Is GetType(Boolean) Then Return 2
        If t Is GetType(Date) OrElse t Is GetType(DateTime) Then Return 3
        If t Is GetType(Double) OrElse t Is GetType(Single) OrElse t Is GetType(Decimal) Then Return 7
        Return 0
    End Function




    '    Private Function GetTableExportType(
    '    tableName As String,
    '    ByRef jsonName As String,
    '    ByRef parentTable As String,
    '    ByRef childTable As String,
    '    ByRef parentPK As String,
    '    ByRef childFK As String,
    '    ByRef childrenPropertyName As String
    ') As TableExportType

    '        Select Case tableName
    '        ' ---------------------------
    '        ' Value Tables
    '        ' ---------------------------
    '            Case "Tool Crib", "Machine Tool Types",
    '             "Material Types", "Tool Attribute Types", "Tool Type Attributes",
    '             "Machine Attribute Types", "Material Inventory", "Directives"
    '                jsonName = tableName.Replace(" ", "")
    '                Return TableExportType.Value

    '        ' ---------------------------
    '        ' Flat Tables
    '        ' ---------------------------
    '            Case "Machines", "Machine Types", "Machine Tools", "Machine Attributes",
    '             "Machine Type Attributes", "Tool Types", "Tool Setups", "Stations"
    '                jsonName = tableName.Replace(" ", "")
    '                Return TableExportType.HybridProjected ' Flat tables use HybridProjected

    '        ' ---------------------------
    '        ' Hybrid Tables
    '        ' ---------------------------
    '            Case "Tool Setup Members", "Station Configuration"
    '                jsonName = tableName.Replace(" ", "")
    '                Return TableExportType.Hybrid

    '        ' ---------------------------
    '        ' Hierarchical Tables
    '        ' ---------------------------
    '            Case "Layer Setups"
    '                jsonName = "LayerSetups"
    '                parentTable = "Layer Setups"
    '                childTable = "Directives"
    '                parentPK = "ID"
    '                childFK = "Layer Setup ID"
    '                childrenPropertyName = "Layers"
    '                Return TableExportType.Hierarchical

    '            Case Else
    '                jsonName = tableName.Replace(" ", "")
    '                Return TableExportType.Other
    '        End Select
    '    End Function


    Public Sub ExportMDBTable(mdbPath As String, outputFolder As String, tableName As String)
        Dim jsonName As String = ""
        Dim parentTable As String = ""
        Dim childTable As String = ""
        Dim parentPK As String = ""
        Dim childFK As String = ""
        Dim childrenPropertyName As String = ""
        Dim fieldMap As Dictionary(Of String, String) = Nothing

        Dim Type = GetTableExportType(tableName, jsonName, parentTable, childTable, parentPK, childFK, childrenPropertyName, fieldMap)

        Select Case Type
            Case TableExportType.ToolTypeAttributes
                sExportType = "Tool Type Attributes"
                ExportToolTypeAttributes(mdbPath, outputFolder)


            Case TableExportType.ToolTypes
                sExportType = "ToolTypes"
                ExportToolTypes(mdbPath, outputFolder)

            Case TableExportType.ToolSetups
                sExportType = "Tool Setups"
                ExportToolSetupsFromAccess(mdbPath, outputFolder)

            Case TableExportType.ToolSetupMembers
                sExportType = "Tool Setup Members "
                ExportToolSetupMembers(mdbPath, outputFolder)


            Case TableExportType.ToolCrib
                sExportType = "ToolCrib"

                ExportToolCrib(mdbPath, outputFolder)


            Case TableExportType.ToolAttributeTypes
                sExportType = "ToolAttributeTypes"
                ExportToolAttributeTypes(mdbPath, outputFolder)

            Case TableExportType.MachineType
                sExportType = "MachineType"
                ExportMachineTypes(mdbPath, outputFolder)

            Case TableExportType.Stations
                sExportType = "Stations"
                ExportStations(mdbPath, outputFolder)

            Case TableExportType.MachineTypeAttributes
                sExportType = "MachineTypeAttributes"
                ExportMachineTypeAttributes(mdbPath, outputFolder)

            Case TableExportType.MachineToolTypes
                sExportType = "MachineToolTypes"
                ExportMachineToolTypes(mdbPath, outputFolder)

            Case TableExportType.MachineTools
                sExportType = "MachineTools"
                ExportMachineTools(mdbPath, outputFolder)

            Case TableExportType.Machines
                sExportType = "Machines"
                ExportMachines(mdbPath, outputFolder)

            Case TableExportType.MacAttType
                sExportType = "MacAttType"
                ExportMachineAttributeTypes(mdbPath, outputFolder, tableName, jsonName)

            Case TableExportType.MacAtt
                Dim dt As DataTable
                sExportType = "MacAtt"
                ExportMachineAttributesTable(mdbPath, outputFolder & "\" & jsonName & ".json")
            Case TableExportType.Value
                sExportType = "Value"
                ExportSimpleTable(mdbPath, outputFolder, tableName, jsonName)
            Case TableExportType.Hybrid
                sExportType = "Hybrid"
                ExportHybridTable(mdbPath, outputFolder, parentTable, childTable, jsonName, parentPK, childFK, childrenPropertyName)
            Case TableExportType.ValueProjected
                sExportType = "ValueProjected"
                ExportValueProjectedTable(mdbPath, outputFolder, tableName, jsonName)
            Case TableExportType.MaterialInventory
                sExportType = "MaterialInventory"
                ExportMaterialInventoryTable(mdbPath, outputFolder)
            Case TableExportType.LayerSetups
                sExportType = "LayerSetups"
                ExportLayerSetups(mdbPath, outputFolder)
            Case TableExportType.Directives
                jsonName = "Directives"
                ' fieldMap = New Dictionary(Of String, String) From {{"Color", "DisplayColor"}}
                ExportDirectivesTable(mdbPath, outputFolder, tableName)
            Case TableExportType.Hierarchical
                sExportType = "Hierarchical"
                ExportHierarchicalTable(mdbPath, outputFolder, parentTable, childTable, jsonName, parentPK, childFK, childrenPropertyName)
            Case TableExportType.HybridProjected
                sExportType = "HybridProjected"
                ExportHierarchicalTable(
        mdbPath,
        outputFolder,
        parentTable,
        childTable,
        jsonName,
        parentPK,
        childFK,
        childrenPropertyName)

                'ExportHybridTable(mdbPath, outputFolder, parentTable, childTable, jsonName, parentPK, childFK, childrenPropertyName)
            Case TableExportType.Ignore
                sExportType = "Ignore"
                ' Do nothing
        End Select
    End Sub

    Public Sub ExportToolTypeAttributes(mdbPath As String, outputFolder As String)

        Dim connStr As String =
        $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

        Dim rootObj As New JObject()
        rootObj("TableName") = "Tool Type Attributes"

        ' -----------------------------
        ' Columns
        ' -----------------------------
        Dim columnsArray As New JArray()

        columnsArray.Add(New JObject From {
        {"Field", "ID"},
        {"Header", "Id"},
        {"Visible", True}
    })

        columnsArray.Add(New JObject From {
        {"Field", "ToolTypeID"},
        {"Header", "Tool Type Id"},
        {"Visible", True}
    })

        columnsArray.Add(New JObject From {
        {"Field", "AttributeTypeID"},
        {"Header", "Attribute Type Id"},
        {"Visible", True}
    })

        rootObj("Columns") = columnsArray

        ' -----------------------------
        ' Rows
        ' -----------------------------
        Dim rowsArray As New JArray()

        Using conn As New OleDbConnection(connStr)
            conn.Open()

            Dim sql As String =
            "SELECT [ID], [Tool Type  ID], [Attribute Type ID] " &
            "FROM [Tool Type Attributes] " &
            "ORDER BY [ID]"

            Using cmd As New OleDbCommand(sql, conn)
                Using reader As OleDbDataReader = cmd.ExecuteReader()

                    While reader.Read()
                        Dim rowObj As New JObject()

                        rowObj("ID") = JToken.FromObject(reader("ID"))
                        rowObj("ToolTypeID") = JToken.FromObject(reader("Tool Type  ID"))
                        rowObj("AttributeTypeID") = JToken.FromObject(reader("Attribute Type ID"))

                        rowsArray.Add(rowObj)
                    End While

                End Using
            End Using
        End Using

        rootObj("Rows") = rowsArray

        ' -----------------------------
        ' Write File
        ' -----------------------------
        If Not Directory.Exists(outputFolder) Then
            Directory.CreateDirectory(outputFolder)
        End If

        Dim outputPath As String =
        Path.Combine(outputFolder, "ToolTypeAttributes.json")

        File.WriteAllText(
        outputPath,
        rootObj.ToString(Newtonsoft.Json.Formatting.Indented)
    )

    End Sub





    Public Sub ExportToolTypes(mdbPath As String, outputFolder As String)

        Dim connStr As String =
        $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

        Dim rootObj As New JObject()
        rootObj("TableName") = "Tool Types"

        ' -----------------------------
        ' Columns
        ' -----------------------------
        Dim columnsArray As New JArray()

        columnsArray.Add(New JObject From {
        {"Field", "ID"},
        {"Header", "Id"},
        {"Visible", True},
        {"DataType", 0}
    })

        columnsArray.Add(New JObject From {
        {"Field", "Description"},
        {"Header", "Description"},
        {"Visible", True},
        {"DataType", 0}
    })

        columnsArray.Add(New JObject From {
        {"Field", "Display"},
        {"Header", "Display"},
        {"Visible", True},
        {"DataType", 0}
    })

        rootObj("Columns") = columnsArray

        ' -----------------------------
        ' Rows
        ' -----------------------------
        Dim rowsArray As New JArray()

        Using conn As New OleDbConnection(connStr)
            conn.Open()

            Dim sql As String =
            "SELECT [ID], [Description], [Display] FROM [Tool Types] ORDER BY [ID]"

            Using cmd As New OleDbCommand(sql, conn)
                Using reader As OleDbDataReader = cmd.ExecuteReader()

                    While reader.Read()
                        Dim rowObj As New JObject()

                        rowObj("ID") = JToken.FromObject(reader("ID"))
                        rowObj("Description") = JToken.FromObject(reader("Description"))
                        rowObj("Display") = JToken.FromObject(reader("Display"))

                        rowsArray.Add(rowObj)
                    End While

                End Using
            End Using
        End Using

        rootObj("Rows") = rowsArray

        ' -----------------------------
        ' Write File
        ' -----------------------------
        If Not Directory.Exists(outputFolder) Then
            Directory.CreateDirectory(outputFolder)
        End If

        Dim outputPath As String = Path.Combine(outputFolder, "ToolTypes.json")
        File.WriteAllText(outputPath, rootObj.ToString(Newtonsoft.Json.Formatting.Indented))

    End Sub







    Public Sub ExportToolSetupsFromAccess(mdbPath As String, outputPath As String)
        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
        Using conn As New OleDbConnection(connStr)
            conn.Open()
            Using cmd As New OleDbCommand("SELECT * FROM [Tool Setups]", conn)
                Using reader As OleDbDataReader = cmd.ExecuteReader()
                    Exporters.ExportToolSetups(mdbPath, outputPath)
                End Using
            End Using
        End Using
    End Sub




    Public Sub ExportToolSetupMembers(mdbPath As String, outputFolder As String)
        ' Create JSON root
        Dim outputJson As New JObject()
        outputJson("TableName") = "ToolSetupMembers"
        outputJson("Rows") = New JArray()

        ' Build connection string
        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

        Using conn As New OleDbConnection(connStr)
            conn.Open()
            Dim cmd As New OleDbCommand("SELECT * FROM [Tool Setup Members]", conn)
            Using reader As OleDbDataReader = cmd.ExecuteReader()
                ' Get column names
                Dim columnNames As New List(Of String)()
                For i As Integer = 0 To reader.FieldCount - 1
                    columnNames.Add(reader.GetName(i).ToUpper())
                Next

                ' Read each row
                While reader.Read()
                    Dim rowObj As New JObject()

                    ' ID
                    rowObj("ID") = If(columnNames.Contains("ID"), JToken.FromObject(reader("ID")), JValue.CreateNull())
                    rowObj("ToolSetupID") = If(columnNames.Contains("TOOL SETUP ID") OrElse columnNames.Contains("TOOLSETUPID"),
                                           JToken.FromObject(reader("Tool Setup ID")), JValue.CreateNull())
                    rowObj("StationID") = If(columnNames.Contains("STATION ID") OrElse columnNames.Contains("STATIONID"),
                                         JToken.FromObject(reader("Station ID")), JValue.CreateNull())
                    rowObj("ToolID") = If(columnNames.Contains("TOOL ID") OrElse columnNames.Contains("TOOLID"),
                                      JToken.FromObject(reader("Tool ID")), JValue.CreateNull())

                    ' FixedStation as boolean
                    Dim fixedStationVal As Object = If(columnNames.Contains("FIXED STATION") OrElse columnNames.Contains("FIXEDSTATION"),
                                                   reader("Fixed Station"), Nothing)
                    If fixedStationVal IsNot Nothing AndAlso Not IsDBNull(fixedStationVal) Then
                        rowObj("FixedStation") = Convert.ToBoolean(fixedStationVal)
                    Else
                        rowObj("FixedStation") = False
                    End If

                    ' IndexAngle as double
                    Dim indexAngleVal As Object = If(columnNames.Contains("INDEX ANGLE") OrElse columnNames.Contains("INDEXANGLE"),
                                                 reader("Index Angle"), Nothing)
                    If indexAngleVal IsNot Nothing AndAlso Not IsDBNull(indexAngleVal) Then
                        rowObj("IndexAngle") = Convert.ToDouble(indexAngleVal)
                    Else
                        rowObj("IndexAngle") = 0.0
                    End If

                    ' Add row to JSON array
                    CType(outputJson("Rows"), JArray).Add(rowObj)
                End While
            End Using
        End Using

        ' Save JSON
        Dim outputPath As String =
            Path.Combine(outputFolder, "ToolSetupMembers.json")
        File.WriteAllText(outputPath, outputJson.ToString())

    End Sub

    Public Sub ExportToolCrib(mdbPath As String, outputFolder As String)
        ' Ensure output folder exists
        If Not Directory.Exists(outputFolder) Then
            Directory.CreateDirectory(outputFolder)
        End If

        Dim connectionString = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

        Dim toolCribItems As New List(Of ToolCribItem)

        Using conn As New OleDbConnection(connectionString)
            conn.Open()

            Dim cmd As New OleDbCommand("SELECT * FROM [Tool Crib]", conn)
            Using reader = cmd.ExecuteReader()
                While reader.Read()
                    Dim item As New ToolCribItem With {
                        .ID = Convert.ToInt32(reader("ID")),
                        .TypeID = Convert.ToInt32(reader("Type ID")), ' Always has a value
                        .Description = reader("Description").ToString(),
                        .Attributes = New Dictionary(Of String, Object) From {
                            {"TYPE ID", Convert.ToInt32(reader("Type ID"))},
                            {"COLOR", Convert.ToInt32(reader("Color"))},
                            {"LOFF", Convert.ToInt32(reader("Loff"))},
                            {"DOFF", Convert.ToInt32(reader("Doff"))},
                            {"LENGTH", Convert.ToDouble(reader("Length"))},
                            {"WIDTH", Convert.ToDouble(reader("Width"))},
                            {"DIAMETER", Convert.ToDouble(reader("Diameter"))},
                            {"KERF", Convert.ToDouble(reader("Kerf"))},
                            {"CORNER RADIUS", Convert.ToDouble(reader("Corner Radius"))},
                            {"ANGLE", Convert.ToDouble(reader("Angle"))},
                            {"PITCH", Convert.ToDouble(reader("Pitch"))},
                            {"RADIUS", Convert.ToDouble(reader("Radius"))},
                            {"WEB", Convert.ToDouble(reader("Web"))},
                            {"MAJOR DIAMETER", Convert.ToDouble(reader("Major Diameter"))},
                            {"MINOR DIAMETER", Convert.ToDouble(reader("Minor Diameter"))},
                            {"SHEAR", Convert.ToDouble(reader("Shear"))},
                            {"COST", Convert.ToDouble(reader("Cost"))},
                            {"LIFE", Convert.ToDouble(reader("Life"))},
                            {"FILENAME", If(reader("Filename") Is DBNull.Value, Nothing, reader("Filename").ToString())},
                            {"REQD STATION SIZE", Convert.ToDouble(reader("Reqd Station Size"))},
                            {"REQD AUTO INDEX", Convert.ToBoolean(reader("Reqd Auto Index"))},
                            {"DIE_CLEARANCE", If(reader("Die_Clearance") Is DBNull.Value, Nothing, reader("Die_Clearance").ToString())}
                        }
                    }
                    toolCribItems.Add(item)
                End While
            End Using
        End Using

        ' Build JSON structure
        Dim jsonObject As New Dictionary(Of String, Object) From {
            {"TableName", "Tool Crib"},
            {"Rows", toolCribItems}
        }

        ' Serialize to JSON
        Dim json = JsonConvert.SerializeObject(jsonObject, Formatting.Indented)

        ' Write to output
        File.WriteAllText(Path.Combine(outputFolder, "ToolCrib.json"), json)
    End Sub
    Public Sub ExportToolAttributeTypes(mdbPath As String, outputFolder As String)
        Try
            Dim connString As String =
            $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

            ' -------------------------------
            ' Initialize JSON root
            ' -------------------------------
            Dim outputJson As New JObject()
            outputJson("TableName") = "Tool Attribute Types"

            ' -------------------------------
            ' Columns definition (STATIC)
            ' -------------------------------
            outputJson("Columns") = New JArray(
            New JObject From {
                {"Field", "ID"},
                {"Header", "Id"},
                {"Visible", True}
            },
            New JObject From {
                {"Field", "Description"},
                {"Header", "Description"},
                {"Visible", True}
            },
            New JObject From {
                {"Field", "Field"},
                {"Header", "Field"},
                {"Visible", True}
            },
            New JObject From {
                {"Field", "DataType"},
                {"Header", "Datatype"},
                {"Visible", True}
            }
        )

            outputJson("Rows") = New JArray()

            ' -------------------------------
            ' Database read
            ' -------------------------------
            Using conn As New OleDbConnection(connString)
                conn.Open()

                Dim sql As String =
                "SELECT ID, Description, [Field], DataType " &
                "FROM [Tool Attribute Types] " &
                "ORDER BY ID"

                Using cmd As New OleDbCommand(sql, conn)
                    Using reader As OleDbDataReader = cmd.ExecuteReader()
                        While reader.Read()
                            Dim row As New JObject

                            row("ID") =
                            JToken.FromObject(reader("ID"))

                            row("Description") =
                            JToken.FromObject(reader("Description").ToString())

                            row("Field") =
                            JToken.FromObject(reader("Field").ToString())

                            row("DataType") =
                            JToken.FromObject(reader("DataType"))

                            CType(outputJson("Rows"), JArray).Add(row)
                        End While
                    End Using
                End Using
            End Using

            ' -------------------------------
            ' Write file
            ' -------------------------------
            Dim outputPath As String =
            Path.Combine(outputFolder, "ToolAttributeTypes.json")

            File.WriteAllText(outputPath, outputJson.ToString(Newtonsoft.Json.Formatting.Indented))

        Catch ex As Exception
            Throw New Exception(
            $"Failed to export Tool Attribute Types: {ex.Message}", ex)
        End Try
    End Sub

    Public Sub ExportStations(mdbPath As String, outputFolder As String)
        Try
            Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
            Dim sql As String = "SELECT * FROM [Station Configuration]"

            Dim stations As New List(Of Station)

            ' Read stations from MDB
            Using conn As New OleDbConnection(connString)
                conn.Open()
                Using cmd As New OleDbCommand(sql, conn)
                    Using reader As OleDbDataReader = cmd.ExecuteReader()
                        While reader.Read()
                            Dim station As New Station With {
                            .ID = If(IsDBNull(reader("ID")), 0, CInt(reader("ID"))),
                            .MachineID = If(IsDBNull(reader("Machine ID")), 0, CInt(reader("Machine ID"))),
                            .WorkplaneID = If(IsDBNull(reader("Workplane ID")), 0, CInt(reader("Workplane ID"))),
                            .Description = "",
                            .Position = "",
                            .NCCodeNumber = If(IsDBNull(reader("NC Code Number")), 0, CInt(reader("NC Code Number"))),
                            .PrimaryCode = If(IsDBNull(reader("Primary Code")), "0", reader("Primary Code").ToString()),
                            .SecondaryCode = If(IsDBNull(reader("Secondary Code")), "0", reader("Secondary Code").ToString()),
                            .StationSize = If(IsDBNull(reader("Station Size")), CType(Nothing, Integer?), CInt(reader("Station Size"))),
                            .Properties = New Dictionary(Of String, Object) From {
                                {"StationLocationX", If(IsDBNull(reader("Station Location X")), 0.0, CDbl(reader("Station Location X")))},
                                {"StationLocationY", If(IsDBNull(reader("Station Location Y")), 0.0, CDbl(reader("Station Location Y")))},
                                {"AutoIndex", If(IsDBNull(reader("Auto Index")), False, CBool(reader("Auto Index")))}
                            }
                        }
                            stations.Add(station)
                        End While
                    End Using
                End Using
            End Using

            ' Build JSON object
            Dim outputJson As New JObject()
            outputJson("TableName") = "Station Configuration"

            ' Columns definition
            outputJson("Columns") = New JArray(
            New JObject From {{"Field", "ID"}, {"Header", "ID"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "MachineID"}, {"Header", "Machine ID"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "WorkplaneID"}, {"Header", "Workplane ID"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "StationLocationX"}, {"Header", "Station Location X"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "StationLocationY"}, {"Header", "Station Location Y"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "AutoIndex"}, {"Header", "Auto Index"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "PrimaryCode"}, {"Header", "Primary Code"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "SecondaryCode"}, {"Header", "Secondary Code"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "NCCodeNumber"}, {"Header", "NC Code Number"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "StationSize"}, {"Header", "Station Size"}, {"Visible", True}, {"DataType", 0}}
        )

            ' Rows
            ' Rows
            Dim rows As New JArray()
            For Each s In stations
                Dim row As New JObject()
                row.Add("ID", JToken.FromObject(s.ID))
                row.Add("MachineID", JToken.FromObject(s.MachineID))
                row.Add("WorkplaneID", JToken.FromObject(s.WorkplaneID))
                row.Add("StationLocationX", JToken.FromObject(If(s.Properties.ContainsKey("StationLocationX"), s.Properties("StationLocationX"), 0.0)))
                row.Add("StationLocationY", JToken.FromObject(If(s.Properties.ContainsKey("StationLocationY"), s.Properties("StationLocationY"), 0.0)))
                row.Add("AutoIndex", JToken.FromObject(If(s.Properties.ContainsKey("AutoIndex"), s.Properties("AutoIndex"), False)))
                row.Add("PrimaryCode", JToken.FromObject(s.PrimaryCode))
                row.Add("SecondaryCode", JToken.FromObject(s.SecondaryCode))
                row.Add("NCCodeNumber", JToken.FromObject(s.NCCodeNumber))
                row.Add("StationSize", If(s.StationSize.HasValue, JToken.FromObject(s.StationSize.Value), JValue.CreateNull()))

                rows.Add(row)
            Next


            outputJson("Rows") = rows

            ' Save JSON to file
            Dim outputPath As String = Path.Combine(outputFolder, "Stations.json")
            File.WriteAllText(outputPath, outputJson.ToString())

        Catch ex As Exception
            Throw New Exception($"Failed to export table Station Configuration: {ex.Message}", ex)
        End Try
    End Sub

    Public Sub ExportMachineTypeAttributes(mdbPath As String, outputFolder As String)
        Try

            Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

            Dim fieldMap As New Dictionary(Of String, String) From {
            {"ID", "ID"},
            {"MachineTypeID", "MachineTypeID"},
            {"AttributeTypeID", "AttributeTypeID"}
        }

            Dim outputJson As New JObject(
            New JProperty("TableName", "Machine Type Attributes"),
            New JProperty("Columns", New JArray(fieldMap.Keys.Select(Function(f) New JObject(
                New JProperty("Field", f),
                New JProperty("Header", f),
                New JProperty("Visible", True)
            )))),
            New JProperty("Rows", New JArray())
        )

            Using cn As New OleDb.OleDbConnection(connString)
                cn.Open()

                ' SQL alias to match JSON keys
                Dim sql As String = "SELECT [ID], [Machine Type  ID] AS MachineTypeID, [Attribute Type ID] AS AttributeTypeID FROM [Machine Type Attributes]"

                Using cmd As New OleDb.OleDbCommand(sql, cn)
                    Using dr = cmd.ExecuteReader()
                        Dim readerCols As New List(Of String)
                        For i As Integer = 0 To dr.FieldCount - 1
                            readerCols.Add(dr.GetName(i))
                        Next

                        While dr.Read()
                            Dim row As New JObject()
                            For Each jsonField In fieldMap.Keys
                                Dim colName = fieldMap(jsonField)
                                If readerCols.Contains(colName) Then
                                    row(jsonField) = If(IsDBNull(dr(colName)), Nothing, JToken.FromObject(dr(colName)))
                                Else
                                    row(jsonField) = Nothing
                                End If
                            Next
                            ' Cast Rows to JArray to use Add
                            CType(outputJson("Rows"), JArray).Add(row)
                        End While
                    End Using
                End Using
            End Using

            Dim jsonFile As String = IO.Path.Combine(outputFolder, "MachineTypeAttributes.json")
            IO.File.WriteAllText(jsonFile, outputJson.ToString())


        Catch ex As Exception
            Throw New Exception($"Failed to export table MachineTypeAttributes: {ex.Message}", ex)
        End Try
    End Sub

    Public Sub ExportMachineToolTypes(mdbpath As String, outputFolder As String)
        Dim dt As New DataTable
        Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbpath};Persist Security Info=False;"

        Using cn As New OleDb.OleDbConnection(connString)
            cn.Open()
            Using cmd As New OleDb.OleDbCommand("SELECT * FROM [Machine Tool Types]", cn)
                Using da As New OleDb.OleDbDataAdapter(cmd)
                    da.Fill(dt)
                End Using
            End Using
        End Using

        ' Map JSON field names to MDB column names
        Dim fieldMap As New Dictionary(Of String, String) From {
        {"ID", "ID"},
        {"MachineTypeID", "Machine Type ID"},
        {"ToolTypeID", "Tool Type ID"}
    }

        ' Build JSON structure
        Dim json As New JObject
        json("TableName") = "Machine Tool Types"

        ' Columns definition
        Dim columns As New JArray
        For Each kvp In fieldMap
            Dim colDef As New JObject
            colDef("Field") = kvp.Key
            colDef("Header") = kvp.Key.Replace("ID", " Id")
            colDef("Visible") = True
            columns.Add(colDef)
        Next
        json("Columns") = columns

        ' Rows
        Dim rows As New JArray
        For Each dr As DataRow In dt.Rows
            Dim row As New JObject
            For Each jsonField In fieldMap.Keys
                Dim colName = fieldMap(jsonField)
                row(jsonField) = If(IsDBNull(dr(colName)), Nothing, JToken.FromObject(dr(colName)))
            Next
            rows.Add(row)
        Next
        json("Rows") = rows

        ' Write to file
        Dim outputFile As String = Path.Combine(outputFolder, "MachineToolTypes.json")
        File.WriteAllText(outputFile, json.ToString())
    End Sub

    Public Sub ExportTableToJson(mdbPath As String, accessTableName As String, outputFolder As String, jsonTableName As String, fieldMap As Dictionary(Of String, String))
        Try
            Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
            Using conn As New OleDbConnection(connString)
                conn.Open()

                ' Read Access table
                Dim sql As String = $"SELECT * FROM [{accessTableName}]"
                Dim cmd As New OleDbCommand(sql, conn)
                Dim adapter As New OleDbDataAdapter(cmd)
                Dim dt As New DataTable()
                adapter.Fill(dt)

                ' Build JSON Rows
                Dim rowsArray As New JArray()
                For Each dr As DataRow In dt.Rows
                    Dim row As New JObject()
                    For Each kvp In fieldMap
                        Dim jsonColName As String = kvp.Key
                        Dim mdbColName As String = kvp.Value
                        If dt.Columns.Contains(mdbColName) Then
                            row(jsonColName) = If(IsDBNull(dr(mdbColName)), JValue.CreateNull(), JToken.FromObject(dr(mdbColName)))
                        Else
                            row(jsonColName) = JValue.CreateNull()
                        End If
                    Next
                    rowsArray.Add(row)
                Next

                ' Build JSON Columns array
                Dim columnsArray As New JArray()
                For Each kvp In fieldMap
                    columnsArray.Add(New JObject From {
                                  {"Field", kvp.Key},
                                  {"Header", kvp.Key}, ' Could be customized if you want proper display names
                                  {"Visible", True},
                                  {"DataType", 0} ' Optional, can add type mapping if needed
                              })
                Next

                ' Build root object
                Dim root As New JObject()
                root("TableName") = jsonTableName
                root("Columns") = columnsArray
                root("Rows") = rowsArray

                ' Write JSON
                Dim outputFile As String = Path.Combine(outputFolder, $"{jsonTableName}.json")
                File.WriteAllText(outputFile, root.ToString(Formatting.Indented))
            End Using

        Catch ex As Exception
            Throw New Exception($"Failed to export table {accessTableName}: {ex.Message}", ex)
        End Try
    End Sub

    Public Sub ExportMachineTools(mdbPath As String, outputFolder As String)

        Try
            Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
            Using conn As New OleDbConnection(connString)
                conn.Open()

                ' Read MachineTools table
                Dim sql As String = "SELECT * FROM [Machine Tools]"
                Dim cmd As New OleDbCommand(sql, conn)
                Dim adapter As New OleDbDataAdapter(cmd)
                Dim dt As New DataTable()
                adapter.Fill(dt)

                ' Define JSON columns
                Dim columns As New JArray From {
                New JObject From {{"Field", "ID"}, {"Header", "ID"}, {"Visible", True}},
                New JObject From {{"Field", "MachineID"}, {"Header", "Machine ID"}, {"Visible", True}},
                New JObject From {{"Field", "ToolID"}, {"Header", "Tool Id"}, {"Visible", True}}
            }

                ' Build JSON rows
                Dim rows As New JArray()
                Dim fieldMap As New Dictionary(Of String, String) From {
                {"ID", "ID"},
                {"MachineID", "Machine ID"},
                {"ToolID", "Tool ID"}   ' <- maps JSON field to MDB column
            }

                For Each dr As DataRow In dt.Rows
                    Dim row As New JObject()
                    For Each jsonField In fieldMap.Keys
                        Dim colName = fieldMap(jsonField)
                        row(jsonField) = If(IsDBNull(dr(colName)), Nothing, JToken.FromObject(dr(colName)))
                    Next
                    rows.Add(row)
                Next

                ' Build final JSON
                Dim root As New JObject()
                root("TableName") = "MachineTools"
                root("Columns") = columns
                root("Rows") = rows

                ' Write JSON file
                Dim outputFile As String = Path.Combine(outputFolder, "MachineTools.json")
                File.WriteAllText(outputFile, root.ToString(Formatting.Indented))
            End Using

        Catch ex As Exception
            Throw New Exception($"Failed to export MachineTools: {ex.Message}")
        End Try

    End Sub

    ' Export function using Newtonsoft.Json

    Public Sub ExportLayerSetups(mdbPath As String, outputFolder As String)


        Dim tableName As String = "Layer Setups"
        Dim jsonName As String = "LayerSetups"  ' internal name for other code

        Dim layerSetupRows As New List(Of Dictionary(Of String, Object))

        Using conn As New OleDbConnection($"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};")
            conn.Open()

            ' Read Layer Setups
            Using cmd As New OleDbCommand($"SELECT * FROM [{tableName}]", conn)
                Using reader As OleDbDataReader = cmd.ExecuteReader()
                    While reader.Read()
                        ' Build LayerSetup row from actual columns
                        Dim row As New Dictionary(Of String, Object) From {
                        {"ID", reader("ID")},
                        {"Description", reader("Description")},
                        {"MachineID", reader("Machine ID")},
                        {"ZLevelMode", reader("Z Level Mode")},
                        {"GapTolerance", reader("Gap Tolerance")},
                        {"CleanTolerance", reader("Clean Tolerance")},
                        {"FilterTolerance", reader("Filter Tolerance")},
                        {"SharpAngle", reader("Sharp Angle")},
                        {"ProcessText", reader("ProcessText")},
                        {"RestrictOffset", reader("Restrict_Offset")}
                    }

                        ' Read child Directives for this LayerSetup
                        Dim directives As New List(Of Dictionary(Of String, Object))
                        Using cmdDir As New OleDbCommand($"SELECT * FROM [Directives] WHERE [Layer Setup ID]={row("ID")}", conn)
                            Using readerDir As OleDbDataReader = cmdDir.ExecuteReader()
                                While readerDir.Read()
                                    Dim dirRow As New Dictionary(Of String, Object) From {
                                    {"ID", readerDir("ID")},
                                    {"LayerSetupID", readerDir("Layer Setup ID")},
                                    {"CADLayer", readerDir("CAD Layer")},
                                    {"CAMLayer", readerDir("CAM Layer")},
                                    {"Color", readerDir("Color")},
                                    {"DisplayColor", ColorTranslator.FromOle(CInt(readerDir("Color"))).Name},
                                    {"Station", readerDir("Station")},
                                    {"ToolType", readerDir("Tool Type")},
                                    {"ZLevel", readerDir("Z Level")},
                                    {"CutSide", readerDir("Cut Side")},
                                    {"Distance", readerDir("Distance")},
                                    {"CutDirection", readerDir("Cut Direction")}
                                }
                                    directives.Add(dirRow)
                                End While
                            End Using
                        End Using

                        ' Attach Directives to LayerSetup row
                        row("Layers") = directives
                        layerSetupRows.Add(row)
                    End While
                End Using
            End Using
        End Using

        ' Write JSON file
        Dim outputPath = Path.Combine(outputFolder, jsonName & ".json")
        Dim settings As New JsonSerializerSettings With {
        .Formatting = Formatting.Indented,
        .NullValueHandling = NullValueHandling.Include
    }

        Using sw As New StreamWriter(outputPath)
            Using writer As New JsonTextWriter(sw)
                writer.Formatting = Formatting.Indented
                writer.WriteStartObject()

                ' Output wrapper TableName exactly like good JSON
                writer.WritePropertyName("TableName")
                writer.WriteValue("Layer Setup")  ' matches reference JSON

                writer.WritePropertyName("Rows")
                Newtonsoft.Json.JsonSerializer.Create(settings).Serialize(writer, layerSetupRows)

                writer.WriteEndObject()
            End Using
        End Using
    End Sub

    Public Sub ExportMachineAttributesTable(mdbPath As String, outputFile As String)
        ' Build the DataTable from Access
        Dim dt As New DataTable()
        Using conn As New OleDbConnection($"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath}")
            conn.Open()
            Dim cmd As New OleDbCommand("SELECT * FROM [Machine Attributes]", conn)
            Dim adapter As New OleDbDataAdapter(cmd)
            adapter.Fill(dt)
        End Using

        ' Define the JSON object
        Dim json As New JObject()
        json("TableName") = "Machine Attributes"

        ' Define Columns array
        Dim columns As New JArray From {
        New JObject From {{"Field", "ID"}, {"Header", "Id"}, {"Visible", True}, {"DataType", 0}},
        New JObject From {{"Field", "AttributeTypeID"}, {"Header", "Attribute Type Id"}, {"Visible", True}, {"DataType", 0}},
        New JObject From {{"Field", "MachineID"}, {"Header", "Machine Id"}, {"Visible", True}, {"DataType", 0}},
        New JObject From {{"Field", "Value"}, {"Header", "Value"}, {"Visible", True}, {"DataType", 0}}
    }
        json("Columns") = columns



        ' Populate Rows
        Dim rows As New JArray()
        For Each dr As DataRow In dt.Rows

            ' Ensure column names match your DataTable exactly
            Dim row As New JObject From {
    {"ID", If(IsDBNull(dr("ID")), 0, CInt(dr("ID")))},
    {"MachineID", If(IsDBNull(dr("Machine ID")), 0, CInt(dr("Machine ID")))},
    {"AttributeTypeID", If(IsDBNull(dr("Attribute Type ID")), 0, CInt(dr("Attribute Type ID")))},
    {"Value", If(IsDBNull(dr("Value")), Nothing, dr("Value").ToString())}
}

            rows.Add(row)
        Next
        json("Rows") = rows

        ' Write JSON to file
        Using sw As New StreamWriter(outputFile)
            sw.Write(JsonConvert.SerializeObject(json, Formatting.Indented))
        End Using
    End Sub

    Public Sub ExportDirectivesTable(mdbPath As String, outputFolder As String, jsonName As String)
        Dim tableName As String = "Directives"
        jsonName = "Directives"

        ' Field mapping: Access column -> JSON property
        Dim fieldMap As New Dictionary(Of String, String) From {
        {"ID", "ID"},
        {"Layer Setup ID", "LayerSetupID"},
        {"CAD Layer", "CADLayer"},
        {"CAM Layer", "CAMLayer"},
        {"Color", "Color"},
        {"Station", "Station"},
        {"Tool Type", "ToolType"},
        {"Z Level", "ZLevel"},
        {"Cut Side", "CutSide"},
        {"Distance", "Distance"},
        {"Cut Direction", "CutDirection"}
    }

        Dim rows As New List(Of Dictionary(Of String, Object))

        Using conn As New OleDbConnection($"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};")
            conn.Open()

            Using cmd As New OleDbCommand($"SELECT * FROM [{tableName}]", conn)
                Using reader As OleDbDataReader = cmd.ExecuteReader()
                    While reader.Read()
                        Dim row As New Dictionary(Of String, Object)

                        ' Map fields according to fieldMap
                        For Each kvp In fieldMap
                            Dim srcName = kvp.Key
                            Dim tgtName = kvp.Value

                            If reader.GetOrdinal(srcName) >= 0 AndAlso Not reader.IsDBNull(reader.GetOrdinal(srcName)) Then
                                row(tgtName) = reader(srcName)
                            Else
                                row(tgtName) = Nothing
                            End If
                        Next

                        ' Compute DisplayColor from OLE Color integer
                        Dim oleColor As Integer = If(row.ContainsKey("Color") AndAlso row("Color") IsNot Nothing, CInt(row("Color")), 0)
                        row("DisplayColor") = ColorTranslator.FromOle(oleColor)

                        rows.Add(row)
                    End While
                End Using
            End Using
        End Using

        ' Write JSON
        Dim outputPath = Path.Combine(outputFolder, jsonName & ".json")

        Dim settings As New JsonSerializerSettings With {
        .Formatting = Formatting.Indented,
        .NullValueHandling = NullValueHandling.Include
    }

        Using sw As New StreamWriter(outputPath)
            Using writer As New JsonTextWriter(sw)
                writer.Formatting = Formatting.Indented
                writer.WriteStartObject()

                writer.WritePropertyName("TableName")
                writer.WriteValue(jsonName)

                writer.WritePropertyName("Rows")
                Newtonsoft.Json.JsonSerializer.Create(settings).Serialize(writer, rows)

                writer.WriteEndObject()
            End Using
        End Using
    End Sub

    ' JsonConverter to serialize System.Drawing.Color as integer
    Public Class ColorToIntegerJsonConverter
        Inherits JsonConverter

        Public Overrides Function CanConvert(objectType As Type) As Boolean
            Return objectType Is GetType(Color)
        End Function

        Public Overrides Sub WriteJson(
        writer As JsonWriter,
        value As Object,
        serializer As Newtonsoft.Json.JsonSerializer)

            Dim c As Color = DirectCast(value, Color)

            ' Write OLE RGB integer (no alpha)
            Dim rgb As Integer = c.ToArgb() And &HFFFFFF
            writer.WriteValue(rgb)
        End Sub

        Public Overrides Function ReadJson(
        reader As JsonReader,
        objectType As Type,
        existingValue As Object,
        serializer As Newtonsoft.Json.JsonSerializer) As Object

            If reader.TokenType = JsonToken.Integer Then
                Return ColorTranslator.FromOle(CInt(reader.Value))
            End If

            Return Color.Empty
        End Function
    End Class

    '    Public Sub ExportMDBTable(mdbPath As String, outputFolder As String, tableName As String, jsonName As String)
    '        Select Case tableName
    '        ' ---------------------------
    '        ' Value Tables
    '        ' ---------------------------
    '            Case "Tool Crib", "Machine Tool Types",
    '             "Material Types", "Tool Attribute Types", "Tool Type Attributes",
    '             "Machine Attribute Types", "Material Inventory", "Directives"
    '                ExportValueTable(mdbPath, outputFolder, tableName, jsonName)

    '        ' ---------------------------
    '        ' Flat Tables
    '        ' ---------------------------
    '            Case "Machines", "Machine Types", "Machine Tools", "Machine Attributes",
    '             "Machine Type Attributes", "Tool Types", "Tool Setups", "Stations"
    '                ExportFlatTable(mdbPath, outputFolder, tableName, jsonName)

    '        ' ---------------------------
    '        ' Hybrid Tables
    '        ' ---------------------------
    '            Case "Tool Setup Members", "Station Configuration"
    '                ExportHybridTable(mdbPath, outputFolder, tableName, jsonName)

    '        ' ---------------------------
    '        ' Hierarchical Tables
    '        ' ---------------------------
    '            Case "Layer Setups"
    '                Dim parentMap As New Dictionary(Of String, String) From {
    '     {"Machine ID", "MachineID"},
    '     {"Z Level Mode", "ZLevelMode"},
    '     {"Gap Tolerance", "GapTolerance"},
    '     {"Clean Tolerance", "CleanTolerance"},
    '     {"Filter Tolerance", "FilterTolerance"},
    '     {"Sharp Angle", "SharpAngle"}
    ' }
    '                Dim childMap As New Dictionary(Of String, String) From {
    '                    {"LayerSetupID", "LayerSetupID"},
    '                    {"CADLayer", "CADLayer"},
    '                    {"CAMLayer", "CAMLayer"},
    '                    {"Z Level", "ZLevel"},
    '                    {"Cut Side", "CutSide"},
    '                    {"Cut Direction", "CutDirection"}
    '                }

    '                ExportHierarchicalTable(
    '    mdbPath,
    '    outputFolder,
    '    "Layer Setups",   ' parent table
    '    "Directives",     ' child table
    '    "LayerSetups",    ' JSON file name
    '    "ID",             ' parent PK
    '    "Layer Setup ID",   ' child FK
    '    "Directives"      ' child array name
    ')



    '            Case Else
    '                Throw New ArgumentException($"Table '{tableName}' not classified for export.")
    '        End Select
    '    End Sub

    ''' <summary>
    ''' Export a hierarchical table as JSON (parent-child) with normalized field names.
    ''' </summary>
    Private Sub ExportHierarchicalTable(
    mdbPath As String,
    outputFolder As String,
    jsonName As String,
    parentTable As String,
    childTable As String,
    parentPK As String,
    childFK As String,
    childrenPropertyName As String,
    Optional fieldMap As Dictionary(Of String, String) = Nothing
)

        ' ---------------------------
        ' Load tables
        ' ---------------------------
        If String.IsNullOrWhiteSpace(parentTable) OrElse String.IsNullOrWhiteSpace(childTable) Then
            Throw New Exception("ParentTable or ChildTable not specified for hierarchical export.")
        End If

        Dim dtParent As DataTable = LoadTableFromMDB(mdbPath, parentTable)
        Dim dtChild As DataTable = LoadTableFromMDB(mdbPath, childTable)

        ' ---------------------------
        ' Root JSON object
        ' ---------------------------
        Dim root As New JObject From {
        {"TableName", jsonName},
        {"Rows", New JArray()}
    }

        Dim rowsArray As JArray = CType(root("Rows"), JArray)

        ' ---------------------------
        ' Iterate parent rows
        ' ---------------------------
        For Each parentRow As DataRow In dtParent.Rows

            Dim parentObj As New JObject()

            ' ---------------------------
            ' Parent fields
            ' ---------------------------
            For Each col As DataColumn In dtParent.Columns
                Dim rawValue As Object = parentRow(col)
                Dim safeValue As Object = If(rawValue Is DBNull.Value, Nothing, rawValue)

                Dim jsonFieldName As String
                If fieldMap IsNot Nothing AndAlso fieldMap.ContainsKey(col.ColumnName) Then
                    jsonFieldName = fieldMap(col.ColumnName)
                Else
                    jsonFieldName = col.ColumnName.Replace(" ", "")
                End If

                parentObj(jsonFieldName) = JToken.FromObject(safeValue)
            Next

            ' ---------------------------
            ' Build child array
            ' ---------------------------
            Dim childrenArray As New JArray()

            Dim parentKeyValue As Object = parentRow(parentPK)

            Dim filter As String
            If TypeOf parentKeyValue Is String Then
                filter = $"[{childFK}] = '{parentKeyValue.ToString().Replace("'", "''")}'"
            Else
                filter = $"[{childFK}] = {parentKeyValue}"
            End If

            For Each childRow As DataRow In dtChild.Select(filter)

                Dim childObj As New JObject()

                For Each col As DataColumn In dtChild.Columns
                    Dim rawValue As Object = childRow(col)
                    Dim safeValue As Object = If(rawValue Is DBNull.Value, Nothing, rawValue)

                    Dim jsonFieldName As String
                    If fieldMap IsNot Nothing AndAlso fieldMap.ContainsKey(col.ColumnName) Then
                        jsonFieldName = fieldMap(col.ColumnName)
                    Else
                        jsonFieldName = col.ColumnName.Replace(" ", "")
                    End If

                    childObj(jsonFieldName) = JToken.FromObject(safeValue)
                Next

                childrenArray.Add(childObj)
            Next

            parentObj(childrenPropertyName) = childrenArray
            rowsArray.Add(parentObj)

        Next

        ' ---------------------------
        ' Write file
        ' ---------------------------
        Dim outputPath As String = Path.Combine(outputFolder, jsonName & ".json")

        File.WriteAllText(
        outputPath,
        JsonConvert.SerializeObject(root, Formatting.Indented)
    )

    End Sub

    ''' <summary>
    ''' Export a simple flat table as JSON with normalized field names.
    ''' </summary>


    'Public Sub ExportSimpleTable(
    'mdbPath As String,
    'outputFolder As String,
    'tableName As String,
    'jsonName As String)

    '    Dim rows As New List(Of Dictionary(Of String, Object))

    '    Using conn As New OleDbConnection(
    '    $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};")

    '        conn.Open()

    '        Using cmd As New OleDbCommand($"SELECT * FROM [{tableName}]", conn)
    '            Using reader As OleDbDataReader = cmd.ExecuteReader()

    '                While reader.Read()
    '                    Dim row As New Dictionary(Of String, Object)

    '                    For i As Integer = 0 To reader.FieldCount - 1
    '                        Dim fieldName = reader.GetName(i)

    '                        If reader.IsDBNull(i) Then
    '                            row(fieldName) = Nothing
    '                        Else
    '                            row(fieldName) = reader.GetValue(i)
    '                        End If
    '                    Next

    '                    rows.Add(row)
    '                End While
    '            End Using
    '        End Using
    '    End Using

    '    Dim outputPath = Path.Combine(outputFolder, jsonName & ".json")

    '    Dim settings As New JsonSerializerSettings With {
    '    .Formatting = Formatting.Indented,
    '    .NullValueHandling = NullValueHandling.Include
    '}

    '    Using sw As New StreamWriter(outputPath)
    '        Using writer As New JsonTextWriter(sw)

    '            writer.Formatting = Formatting.Indented
    '            writer.WriteStartObject()

    '            writer.WritePropertyName("TableName")
    '            writer.WriteValue(jsonName)

    '            writer.WritePropertyName("Rows")
    '            Newtonsoft.Json.JsonSerializer.Create(settings).Serialize(writer, rows)

    '            writer.WriteEndObject()
    '        End Using
    '    End Using
    'End Sub

    Public Sub ExportSimpleTable(mdbPath As String, outputFolder As String, tableName As String, jsonName As String)
        Dim dt As New DataTable

        ' Load the table from MDB
        Using cn As New OleDb.OleDbConnection($"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath}")
            cn.Open()
            Using cmd As New OleDb.OleDbCommand($"SELECT * FROM [{tableName}]", cn)
                Using reader = cmd.ExecuteReader()
                    dt.Load(reader)
                End Using
            End Using
        End Using

        ' Build JSON
        Dim json As JObject = New JObject()

        If tableName = "Machine Attributes" Then
            ' --- Special case: MachineAttributes ---
            json("TableName") = "Machine Attributes"

            ' Columns array
            Dim cols As New JArray From {
            New JObject From {{"Field", "ID"}, {"Header", "Id"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "AttributeTypeID"}, {"Header", "Attribute Type Id"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "MachineID"}, {"Header", "Machine Id"}, {"Visible", True}, {"DataType", 0}},
            New JObject From {{"Field", "Value"}, {"Header", "Value"}, {"Visible", True}, {"DataType", 0}}
        }
            json("Columns") = cols

            ' Rows array
            Dim rows As New JArray
            For Each dr As DataRow In dt.Rows
                Dim row As New JObject From {
                {"ID", dr("ID")},
                {"MachineID", dr("Machine ID")},
                {"AttributeTypeID", dr("Attribute Type ID")},
                {"Value", dr("Value")}
            }
                rows.Add(row)
            Next
            json("Rows") = rows

        Else
            ' --- Default simple table case ---
            json("TableName") = jsonName
            Dim rows As New JArray
            For Each dr As DataRow In dt.Rows
                Dim row As New JObject
                For Each col As DataColumn In dt.Columns
                    row(col.ColumnName.Replace(" ", "")) = JToken.FromObject(dr(col))
                Next
                rows.Add(row)
            Next
            json("Rows") = rows
        End If

        ' Write JSON to file
        Dim outputFile = Path.Combine(outputFolder, $"{jsonName}.json")
        File.WriteAllText(outputFile, json.ToString())
    End Sub

    ''' <summary>
    ''' Normalize MDB column names to JSON field names.
    ''' Removes spaces and applies special mappings if needed.
    ''' </summary>
    Private Function NormalizeColumnName(columnName As String) As String
        Dim normalized = columnName.Replace(" ", "")

        ' Example special renames
        Select Case columnName
            Case "Type ID"
                normalized = "TypeID"
            Case "Machine ID"
                normalized = "MachineID"
            Case "Layer Setup ID"
                normalized = "LayerSetupID"
            Case "Z Level"
                normalized = "ZLevel"
            Case "Cut Side"
                normalized = "CutSide"
            Case "Cut Direction"
                normalized = "CutDirection"
                ' Add more as needed
        End Select

        Return normalized
    End Function




    Public Function LoadTableFromMDB(mdbPath As String, tableName As String) As DataTable
        Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
        Dim dt As New DataTable()

        Using conn As New OleDbConnection(connString)
            conn.Open()
            Dim query As String = $"SELECT * FROM [{tableName}]"
            Using cmd As New OleDbCommand(query, conn)
                Using adapter As New OleDbDataAdapter(cmd)
                    adapter.Fill(dt)
                End Using
            End Using
        End Using

        Return dt
    End Function




    '    Private Sub ExportHierarchicalTable(
    '    mdbPath As String,
    '    outputFolder As String,
    '    tableName As String,
    '    jsonName As String
    ')

    '        Dim parentRows As New List(Of Dictionary(Of String, Object))
    '        Dim childRows As New List(Of Dictionary(Of String, Object))

    '        Using conn As New OleDb.OleDbConnection(
    '        $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
    '    )
    '            conn.Open()

    '            ' ----------------------------
    '            ' 1) Load parent table
    '            ' ----------------------------
    '            Using parentCmd As New OleDb.OleDbCommand($"SELECT * FROM [{tableName}]", conn)
    '                Using rdr = parentCmd.ExecuteReader()
    '                    While rdr.Read()
    '                        Dim row As New Dictionary(Of String, Object)
    '                        For i = 0 To rdr.FieldCount - 1
    '                            row(rdr.GetName(i)) =
    '                            If(IsDBNull(rdr(i)), Nothing, rdr(i))
    '                        Next
    '                        parentRows.Add(row)
    '                    End While
    '                End Using
    '            End Using

    '            ' ----------------------------
    '            ' 2) Load child table (Layers)
    '            ' ----------------------------
    '            Using childCmd As New OleDb.OleDbCommand("SELECT * FROM [Layers]", conn)
    '                Using rdr = childCmd.ExecuteReader()
    '                    While rdr.Read()
    '                        Dim row As New Dictionary(Of String, Object)
    '                        For i = 0 To rdr.FieldCount - 1
    '                            row(rdr.GetName(i)) =
    '                            If(IsDBNull(rdr(i)), Nothing, rdr(i))
    '                        Next
    '                        childRows.Add(row)
    '                    End While
    '                End Using
    '            End Using
    '        End Using

    '        ' ----------------------------
    '        ' 3) Attach children to parents
    '        ' ----------------------------
    '        For Each parent In parentRows
    '            Dim parentID = Convert.ToInt32(parent("ID"))

    '            Dim layers = childRows.
    '            Where(Function(c) Convert.ToInt32(c("LayerSetupID")) = parentID).
    '            Select(Function(c)
    '                       ' Remove FK from child JSON
    '                       c.Remove("LayerSetupID")
    '                       Return c
    '                   End Function).
    '            ToList()

    '            parent("Layers") = layers
    '        Next

    '        ' ----------------------------
    '        ' 4) Build final JSON object
    '        ' ----------------------------
    '        Dim root As New Dictionary(Of String, Object) From {
    '        {"TableName", tableName},
    '        {"Rows", parentRows}
    '    }

    '        ' ----------------------------
    '        ' 5) Write JSON
    '        ' ----------------------------
    '        Dim json = Newtonsoft.Json.JsonConvert.SerializeObject(
    '        root,
    '        Newtonsoft.Json.Formatting.Indented
    '    )

    '        Dim outPath = IO.Path.Combine(outputFolder, jsonName & ".json")
    '        IO.File.WriteAllText(outPath, json)

    '    End Sub


    Private sJsonName As String


    Public Sub ExportAllTables(mdbPath As String, outputFolder As String)

        Dim filePath As String = "C:\Program Files (x86)\WE-CIM\24.0\Data\ExportTable_errors.txt"

        ' Using statement ensures the StreamWriter is disposed of properly
        Using writer As New StreamWriter(filePath, True) ' Setting the second parameter to True appends text



            Using conn As New OleDb.OleDbConnection(
            $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
        )
                conn.Open()

                ' Get all user tables in the MDB
                Dim dt = conn.GetSchema("Tables")
                For Each row As DataRow In dt.Rows
                    Dim tableType = row("TABLE_TYPE").ToString()
                    If tableType = "TABLE" Then
                        Dim tableName = row("TABLE_NAME").ToString()

                        ' Skip system tables if needed
                        If Not tableName.StartsWith("MSys") Then
                            Try
                                ExportMDBTable(mdbPath, outputFolder, tableName)
                            Catch ex As Exception

                                writer.WriteLine($"Failed to export table {tableName} ")
                                writer.WriteLine($"Using ExportTableType. {sExportType}")
                                writer.WriteLine($" The error is : {ex.Message}")
                                writer.WriteLine("")
                                writer.WriteLine("")

                            End Try
                        End If
                    End If
                Next
            End Using

            MessageBox.Show("Export complete!" & vbCrLf &
                        "JSON files written to: " & outputFolder,
                        "Complete", MessageBoxButtons.OK, MessageBoxIcon.Information)


        End Using

    End Sub
    Public Sub ExportMachineAttributeTypes(mdbPath As String, outputFolder As String, tableName As String, jsonName As String)
        Try
            Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
            Using conn As New OleDbConnection(connString)
                conn.Open()

                ' Read the table
                Dim sql As String = $"SELECT * FROM [{tableName}]"
                Dim cmd As New OleDbCommand(sql, conn)
                Dim adapter As New OleDbDataAdapter(cmd)
                Dim dt As New DataTable()
                adapter.Fill(dt)

                ' Build JSON
                Dim root As New JObject()
                root("TableName") = "Machine Attribute Types"

                ' Columns
                root("Columns") = BuildColumns(dt)

                ' Rows
                Dim rows As New JArray()
                For Each dr As DataRow In dt.Rows
                    Dim row As New JObject()
                    For Each col As DataColumn In dt.Columns
                        row(col.ColumnName.Replace(" ", "")) = JToken.FromObject(dr(col))
                    Next
                    rows.Add(row)
                Next
                root("Rows") = rows

                ' Write JSON
                Dim outputFile As String = Path.Combine(outputFolder, $"{jsonName}.json")
                File.WriteAllText(outputFile, root.ToString(Formatting.Indented))
            End Using

        Catch ex As Exception
            Throw New Exception($"Failed to export table {tableName}: {ex.Message}")
        End Try
    End Sub




    'If Not File.Exists(mdbPath) Then
    '    Throw New FileNotFoundException("MDB not found", mdbPath)
    'End If

    '' Ensure output folder exists
    'IO.Directory.CreateDirectory(outputFolder)

    'Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};"

    'Using conn As New OleDbConnection(connStr)
    '    conn.Open()

    '    ' Get all user tables
    '    Dim schemaTables = conn.GetOleDbSchemaTable(OleDbSchemaGuid.Tables,
    '    New Object() {Nothing, Nothing, Nothing, "TABLE"})

    '    For Each tblRow As DataRow In schemaTables.Rows
    '        Dim tableName As String = tblRow("TABLE_NAME").ToString()

    '        ' Skip system tables
    '        If tableName.StartsWith("MSys") OrElse tableName.Contains("~") Then Continue For

    '        ' Check if this table is in our 18-table map
    '        If Not IsValueTableByName(tableName) AndAlso String.IsNullOrEmpty(sJsonName) Then
    '            ' Table not in map → skip
    '            Console.WriteLine($"Skipping table: {tableName}")
    '            Continue For
    '        End If

    ' Export according to type
    'Dim sjsonName As String
    'Dim exportType = GetTableExportType(tableName, sJsonName)
    '               ExportMDBTable(mdbPath, outputFolder, tableName)

    'Select Case exportType
    '    Case TableExportType.Flat

    '        ExportFlatTable(mdbPath, outputFolder, tableName, sJsonName)

    '    Case TableExportType.Value
    '        ExportValueTable(mdbPath, outputFolder, tableName, sJsonName)

    '    Case TableExportType.Hybrid
    '        ExportHybridTable(mdbPath, outputFolder, tableName, sJsonName)

    '    Case TableExportType.Hierarchical
    '        ExportHierarchicalTable(mdbPath, outputFolder, tableName, sJsonName)
    'End Select

    'If IsValueTableByName(tableName) Then
    '    ' Value table
    '    ExportValueTable(mdbPath, outputFolder, tableName, sJsonName)
    'Else
    '    If tableName = "Tool Setup Members" Then
    '        ' Flattened export for AutoPunch
    '        Dim members = AppData.ToolSetupMembers  ' or whatever your object instance is
    '        Dim rows = members.Select(Function(m) New With {
    '                Key .ID = m.ID,
    '                Key .ToolSetupID = m.ToolSetupID,
    '                Key .StationID = m.StationID,
    '                Key .ToolID = m.ToolID,
    '                Key .FixedStation = m.FixedStation,
    '                Key .IndexAngle = m.IndexAngle
    '            }).ToList()

    '        Dim json = JsonConvert.SerializeObject(New With {
    '            Key .TableName = "ToolSetupMembers",
    '            Key .Rows = rows
    '        }, Formatting.Indented)
    '        Dim outputPath = IO.Path.Combine(outputFolder, sJsonName & ".json")
    '        File.WriteAllText(outputPath, json)
    '    Else
    '        ' Normal export for all other tables
    '        ExportFlatTable(mdbPath, outputFolder, tableName, sJsonName)
    '    End If
    '    '' Flat table
    '    'ExportFlatTable(mdbPath, outputFolder, tableName, sJsonName)
    'End If
    '            Next
    '       End Using

    '    MessageBox.Show("Export complete!" & vbCrLf &
    '                "JSON files written to: " & outputFolder,
    '                "Complete", MessageBoxButtons.OK, MessageBoxIcon.Information)
    'End Sub

    Public Sub ExportHybridTable(
    mdbPath As String,
    outputFolder As String,
    parentTable As String,
    childTable As String,
    jsonName As String,
    parentPK As String,
    childFK As String,
    childrenPropertyName As String,
    Optional fieldMap As Dictionary(Of String, String) = Nothing,
    Optional tableDisplayName As String = Nothing,
    Optional columns As JArray = Nothing
)


        Try
            Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
            Using conn As New OleDbConnection(connString)
                conn.Open()

                ' Read parent table
                Dim sqlParent As String = $"SELECT * FROM [{parentTable}]"
                Dim cmdParent As New OleDbCommand(sqlParent, conn)
                Dim adapterParent As New OleDbDataAdapter(cmdParent)
                Dim dtParent As New DataTable()
                adapterParent.Fill(dtParent)

                ' Read child table
                Dim sqlChild As String = $"SELECT * FROM [{childTable}]"
                Dim cmdChild As New OleDbCommand(sqlChild, conn)
                Dim adapterChild As New OleDbDataAdapter(cmdChild)
                Dim dtChild As New DataTable()
                adapterChild.Fill(dtChild)

                ' Build JSON
                Dim root As New JObject()
                ' root("TableName") = jsonName
                root("TableName") = If(tableDisplayName, jsonName)

                If columns IsNot Nothing Then
                    root("Columns") = columns
                End If
                Dim rowsArray As New JArray()

                For Each parentRow As DataRow In dtParent.Rows
                    Dim jsonParent As New JObject()
                    For Each col As DataColumn In dtParent.Columns
                        jsonParent(col.ColumnName.Replace(" ", "")) = JToken.FromObject(parentRow(col))
                    Next

                    ' Add children
                    Dim children As New JArray()
                    Dim parentValue = parentRow(parentPK)
                    For Each childRow As DataRow In dtChild.Select($"[{childFK}] = {parentValue}")
                        Dim jsonChild As New JObject()
                        For Each col As DataColumn In dtChild.Columns
                            Dim jsonFieldName As String

                            If fieldMap IsNot Nothing AndAlso fieldMap.ContainsKey(col.ColumnName) Then
                                jsonFieldName = fieldMap(col.ColumnName)
                            Else
                                jsonFieldName = col.ColumnName.Replace(" ", "")
                            End If

                            jsonChild(jsonFieldName) = JToken.FromObject(childRow(col))

                        Next
                        children.Add(jsonChild)
                    Next

                    jsonParent(childrenPropertyName) = children
                    rowsArray.Add(jsonParent)
                Next

                root("Rows") = rowsArray

                ' Write JSON
                Dim outputFile As String = Path.Combine(outputFolder, $"{jsonName}.json")
                File.WriteAllText(outputFile, root.ToString(Formatting.Indented))
            End Using

        Catch ex As Exception
            Throw New Exception($"Failed to export table {parentTable}: {ex.Message}")
        End Try
    End Sub

    Public Function BuildColumns(dt As DataTable) As JArray
        Dim columns As New JArray()
        For Each col As DataColumn In dt.Columns
            Dim colObj As New JObject From {
            {"Field", col.ColumnName.Replace(" ", "")},
            {"Header", col.ColumnName.Replace("_", " ")},
            {"Visible", True}
        }
            columns.Add(colObj)
        Next
        Return columns
    End Function

    Public Sub ExportMachines(mdbPath As String, outputFolder As String)
        Try
            Dim connString As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
            Using conn As New OleDbConnection(connString)
                conn.Open()

                ' Read Machines table
                Dim sql As String = "SELECT * FROM [Machines]"
                Dim cmd As New OleDbCommand(sql, conn)
                Dim adapter As New OleDbDataAdapter(cmd)
                Dim dt As New DataTable()
                adapter.Fill(dt)

                ' Column mapping: JSON field name -> Access column name
                Dim fieldMap As New Dictionary(Of String, String) From {
                {"ID", "ID"},
                {"Description", "Description"},
                {"TypeID", "Type ID"},
                {"WorkplaneTypeID", "Workplane Type ID"},
                {"Units", "Units"},
                {"CodePartProfile", "Code Part Profile"},
                {"HoldDown_CTG", "HoldDown_CTG"},
                {"Clamp_CTG", "Clamp_CTG"},
                {"Table_CTG", "Table_CTG"},
                {"CNC_Folder", "CNC_Folder"}
            }

                ' Build JSON Rows
                Dim rowsArray As New JArray()
                For Each dr As DataRow In dt.Rows
                    Dim row As New JObject()
                    For Each kvp In fieldMap
                        Dim jsonColName As String = kvp.Key
                        Dim mdbColName As String = kvp.Value
                        If dt.Columns.Contains(mdbColName) Then
                            row(jsonColName) = If(IsDBNull(dr(mdbColName)), JValue.CreateNull(), JToken.FromObject(dr(mdbColName)))
                        Else
                            row(jsonColName) = JValue.CreateNull()
                        End If
                    Next
                    rowsArray.Add(row)
                Next

                ' Build JSON Columns array
                Dim columnsArray As New JArray()
                For Each kvp In fieldMap
                    columnsArray.Add(New JObject From {
                                  {"Field", kvp.Key},
                                  {"Header", kvp.Key}, ' Could be customized if needed
                                  {"Visible", True},
                                  {"DataType", 0} ' Optional, adjust if you want proper types
                              })
                Next

                ' Build root object
                Dim root As New JObject()
                root("TableName") = "Machines"
                root("Columns") = columnsArray
                root("Rows") = rowsArray

                ' Write JSON
                Dim outputFile As String = Path.Combine(outputFolder, "Machines.json")
                File.WriteAllText(outputFile, root.ToString(Formatting.Indented))
            End Using

        Catch ex As Exception
            Throw New Exception($"Failed to export Machines table: {ex.Message}", ex)
        End Try
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

    Private Sub ExportValueTable(
    mdbPath As String,
    outputFolder As String,
    tableName As String,
    jsonName As String,
    Optional fieldMap As Dictionary(Of String, String) = Nothing
)
        Dim dt As DataTable = LoadTableFromMDB(mdbPath, tableName)
        Dim root As New JObject()
        root("TableName") = tableName

        Dim rowsArray As New JArray()
        For Each row As DataRow In dt.Rows
            Dim jsonRow As New JObject()
            For Each col As DataColumn In dt.Columns
                Dim jsonFieldName As String
                If fieldMap IsNot Nothing AndAlso fieldMap.ContainsKey(col.ColumnName) Then
                    jsonFieldName = fieldMap(col.ColumnName)
                Else
                    jsonFieldName = col.ColumnName.Replace(" ", "")
                End If
                jsonRow(jsonFieldName) = JToken.FromObject(row(col))
            Next
            rowsArray.Add(jsonRow)
        Next

        root("Rows") = rowsArray

        ' Write JSON file
        Dim filePath As String = Path.Combine(outputFolder, jsonName & ".json")
        File.WriteAllText(filePath, root.ToString())
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
                Return False

            Case "Tool Setup Members"
                sJsonName = "ToolSetupMembers"
                Return False

            Case "Directives"
                sJsonName = "Directives"
                Return True

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