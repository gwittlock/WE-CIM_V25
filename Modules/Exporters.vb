Imports Newtonsoft.Json.Linq
Imports System.Data.OleDb
Imports System.IO

Module Exporters

    ' ============================
    ' Export ToolSetups Table
    ' ============================
    Public Sub ExportToolSetups(mdbPath As String, outputJsonPath As String)
        ' Create connection to MDB
        Dim connString = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"
        Using conn As New OleDb.OleDbConnection(connString)
            conn.Open()

            Dim cmd As New OleDb.OleDbCommand("SELECT * FROM [Tool Setups]", conn)
            Dim reader = cmd.ExecuteReader()

            ' Define JSON structure
            Dim jsonObj As New JObject()
            jsonObj("TableName") = "Tool Setups"

            ' Build Columns metadata
            Dim columns As New JArray()
            columns.Add(New JObject From {
            {"Field", "ID"},
            {"Header", "Id"},
            {"Visible", True},
            {"DataType", 0}
        })
            columns.Add(New JObject From {
            {"Field", "Description"},
            {"Header", "Description"},
            {"Visible", True},
            {"DataType", 0}
        })
            columns.Add(New JObject From {
            {"Field", "MachineID"},  ' normalized
            {"Header", "Machine Id"},
            {"Visible", True},
            {"DataType", 0}
        })
            jsonObj("Columns") = columns

            ' Build Rows
            Dim rows As New JArray()
            While reader.Read()
                Dim rowObj As New JObject()
                rowObj("ID") = If(IsDBNull(reader("ID")), Nothing, JToken.FromObject(reader("ID")))
                rowObj("Description") = If(IsDBNull(reader("Description")), Nothing, JToken.FromObject(reader("Description")))
                rowObj("MachineID") = If(IsDBNull(reader("Machine ID")), Nothing, JToken.FromObject(reader("Machine ID")))  ' normalize key

                rows.Add(rowObj)
            End While
            jsonObj("Rows") = rows

            ' Save to file
            Dim outputPath As String =
            Path.Combine(outputJsonPath, "ToolSetups.json")
            File.WriteAllText(outputPath, jsonObj.ToString())

        End Using
    End Sub



    ' ============================
    ' Export ToolSetupMembers Table
    ' ============================
    Public Sub ExportToolSetupMembers(mdbPath As String, outputJsonPath As String)
        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbPath};Persist Security Info=False;"

        Using conn As New OleDb.OleDbConnection(connStr)
            conn.Open()
            Dim cmd As New OleDb.OleDbCommand("SELECT ID, [Tool Setup ID], [Station ID], [Tool ID], [Fixed Station], [Index Angle] FROM ToolSetupMembers", conn)
            Using reader As OleDb.OleDbDataReader = cmd.ExecuteReader()
                Dim tableObj As New JObject()
                tableObj("TableName") = "ToolSetupMembers"
                Dim rows As New JArray()

                While reader.Read()
                    Dim rowObj As New JObject()
                    rowObj("ID") = JToken.FromObject(If(IsDBNull(reader("ID")), Nothing, reader("ID")))
                    rowObj("ToolSetupID") = JToken.FromObject(If(IsDBNull(reader("Tool Setup ID")), Nothing, reader("Tool Setup ID")))
                    rowObj("StationID") = JToken.FromObject(If(IsDBNull(reader("Station ID")), Nothing, reader("Station ID")))
                    rowObj("ToolID") = JToken.FromObject(If(IsDBNull(reader("Tool ID")), Nothing, reader("Tool ID")))
                    rowObj("FixedStation") = JToken.FromObject(If(IsDBNull(reader("Fixed Station")), Nothing, reader("Fixed Station")))
                    rowObj("IndexAngle") = JToken.FromObject(If(IsDBNull(reader("Index Angle")), Nothing, reader("Index Angle")))
                    rows.Add(rowObj)
                End While

                tableObj("Rows") = rows
                File.WriteAllText(outputJsonPath, tableObj.ToString())
            End Using
        End Using
    End Sub

End Module
