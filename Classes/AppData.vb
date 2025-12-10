' ============================================
'   AppData - Global JSON and Application Data
'   Used by entire application
' ============================================
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.IO
'Imports System.IO.Compression

Public Class AppData

    ' Folder where JSON files are stored
    ' App data folder
    Public Shared DataFolder As String = Path.Combine(Application.StartupPath, "Data")

    ' -------------------------------
    ' MACHINES / MATERIALS
    ' -------------------------------
    Public Shared MachineTypes As List(Of MachineType)
    Public Shared Machines As List(Of Machine)
    Public Shared MachineAttributeTypes As List(Of MachineAttributeType)
    Public Shared MachineAttributes As List(Of MachineAttribute)
    Public Shared Stations As List(Of JObject)
    Public Shared ToolSetups As List(Of ToolSetup)

    Public Shared MaterialTypes As List(Of MaterialType)
    Public Shared MaterialSheets As List(Of MaterialSheet)

        ' -------------------------------
        ' TOOLS
        ' -------------------------------
        Public Shared ToolCrib As List(Of ToolCribItem)
        Public Shared ToolTypes As List(Of ToolType)
        Public Shared ToolAttributeTypes As List(Of ToolAttributeType)
        Public Shared ToolTypeAttributes As List(Of ToolTypeAttribute)

    ' ------------------------------------------
    '  LOAD ALL JSON FILES (once)
    ' ------------------------------------------
    ' ------------------------------------------
    ' LOAD ALL JSON FILES (once)
    ' ------------------------------------------
    '  LOAD ALL JSON FILES (once)
    ' ------------------------------------------
    Public Shared Sub LoadAllJson()
        If Not Directory.Exists(DataFolder) Then
            Directory.CreateDirectory(DataFolder)
        End If

        ' ---- MACHINE DATA ----
        MachineTypes = LoadTableWrapped(Of MachineType)("MachineTypes.json")
        Machines = LoadTableWrapped(Of Machine)("Machines.json")
        MachineAttributeTypes = LoadTableWrapped(Of MachineAttributeType)("MachineAttributeTypes.json")
        MachineAttributes = LoadTableWrapped(Of MachineAttribute)("MachineAttributes.json")
        Stations = LoadJObjectWrapped("Stations.json")

        ' ---- TOOL DATA ----
        ' Tool Setups (wrapped JSON)
        ToolSetups = LoadTableWrapped(Of ToolSetup)("ToolSetups.json")
        ToolCrib = LoadTableWrapped(Of ToolCribItem)("ToolCrib.json")
        ToolTypes = LoadTableWrapped(Of ToolType)("ToolTypes.json")
        ToolAttributeTypes = LoadTableWrapped(Of ToolAttributeType)("ToolAttributeTypes.json")
        ToolTypeAttributes = LoadTableWrapped(Of ToolTypeAttribute)("ToolTypeAttributes.json")

        ' ---- MATERIAL DATA ----
        MaterialTypes = LoadTableWrapped(Of MaterialType)("MaterialTypes.json")
        MaterialSheets = LoadMaterialSheets("MaterialInventory.json")
    End Sub

    ' ------------------------------------------
    '  GENERIC WRAPPED JSON TABLE LOADER
    '  Expects JSON in the form { "TableName": "...", "Rows": [ ... ] }
    ' ------------------------------------------
    Private Shared Function LoadTableWrapped(Of T)(fileName As String) As List(Of T)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Dim json = File.ReadAllText(spath)
            Dim wrapper = JsonConvert.DeserializeObject(Of JObject)(json)
            Dim rows = wrapper("Rows").ToString()
            Return JsonConvert.DeserializeObject(Of List(Of T))(rows)
        Else
            Return New List(Of T)()
        End If
    End Function

    Private Shared Function LoadJObjectWrapped(fileName As String) As List(Of JObject)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Dim json = File.ReadAllText(spath)
            Dim wrapper = JsonConvert.DeserializeObject(Of JObject)(json)
            Dim rows = wrapper("Rows").ToString()
            Return JsonConvert.DeserializeObject(Of List(Of JObject))(rows)
        Else
            Return New List(Of JObject)()
        End If
    End Function

    Private Shared Function LoadMaterialSheetsWrapped(fileName As String) As List(Of MaterialSheet)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Dim json = File.ReadAllText(spath)
            Dim wrapper = JsonConvert.DeserializeObject(Of JObject)(json)
            Dim rows = wrapper("Rows").ToString()
            Dim sheets = JsonConvert.DeserializeObject(Of List(Of MaterialSheet))(rows)

            ' Populate Values collection for each sheet
            For Each sheet As MaterialSheet In sheets
                If sheet.Values Is Nothing Then
                    sheet.Values = New List(Of MaterialParameter)
                End If

                ' Add all properties from the original JObject except ID and TypeID
                Dim rowObj As JObject = JObject.Parse(JsonConvert.SerializeObject(sheet)) ' serialize to get full props
                For Each prop As JProperty In rowObj.Properties()
                    If prop.Name <> "ID" AndAlso prop.Name <> "TypeID" Then
                        Dim val As Object = Nothing
                        If prop.Value IsNot Nothing AndAlso prop.Value.Type <> JTokenType.Null Then
                            val = prop.Value
                        End If

                        sheet.Values.Add(New MaterialParameter With {
                        .Name = prop.Name,
                        .Display = prop.Name,
                        .Value = val,
                        .DataType = If(TypeOf val Is String, 0, 7),
                        .DefaultValue = val
                    })
                    End If
                Next
            Next

            Return sheets
        Else
            Return New List(Of MaterialSheet)()
        End If
    End Function



    ' ------------------------------------------
    ' GENERIC WRAPPER LOADER
    ' ------------------------------------------
    ' ------------------------------------------
    ' Generic loader for wrapped JSON tables
    ' Supports JSON objects with "TableName" and "Rows", or plain arrays
    ' Optional converter allows special handling (e.g., MaterialSheet)
    ' ------------------------------------------
    Private Shared Function LoadTableWrapped(Of T)(fileName As String, Optional converter As Func(Of JObject, T) = Nothing) As List(Of T)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        Dim result As New List(Of T)

        If Not File.Exists(spath) Then Return result

        Dim json = File.ReadAllText(spath)
        Dim jToken As JToken

        Try
            jToken = JToken.Parse(json)
        Catch ex As Exception
            MessageBox.Show($"Error parsing JSON {fileName}: {ex.Message}")
            Return result
        End Try

        Dim rows As JArray

        If TypeOf jToken Is JObject AndAlso jToken("Rows") IsNot Nothing Then
            rows = jToken("Rows")
        ElseIf TypeOf jToken Is JArray Then
            rows = jToken
        Else
            Return result
        End If

        For Each row As JObject In rows
            If converter IsNot Nothing Then
                result.Add(converter(row))
            Else
                result.Add(row.ToObject(Of T)())
            End If
        Next

        Return result
    End Function


    ' ------------------------------------------
    ' GENERIC JSON TABLE LOADER
    ' ------------------------------------------
    Private Shared Function LoadTable(Of T)(fileName As String) As List(Of T)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Return JsonConvert.DeserializeObject(Of List(Of T))(File.ReadAllText(spath))
        Else
            Return New List(Of T)()
            End If
        End Function

    Private Shared Function LoadJObjectList(fileName As String) As List(Of JObject)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Return JsonConvert.DeserializeObject(Of List(Of JObject))(File.ReadAllText(spath))
        Else
            Return New List(Of JObject)()
        End If
    End Function
    Private Shared Function LoadMaterialSheets(fileName As String) As List(Of MaterialSheet)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If Not File.Exists(spath) Then Return New List(Of MaterialSheet)()

        Dim json = File.ReadAllText(spath)
        Dim wrapper As JObject = JsonConvert.DeserializeObject(Of JObject)(json)
        Dim rows As JArray = wrapper("Rows")

        Dim sheets As New List(Of MaterialSheet)

        For Each row As JObject In rows
            Dim sheet As New MaterialSheet With {
            .ID = row("ID").Value(Of Integer)(),
            .TypeID = row("TypeID").Value(Of Integer)()
        }

            ' Deserialize Values array directly
            Dim valuesArray As JArray = row("Values")
            If valuesArray IsNot Nothing Then
                For Each v As JObject In valuesArray
                    sheet.Values.Add(New MaterialParameter With {
                    .Name = v("Name").ToString(),
                    .Display = v("Display").ToString(),
                    .Value = v("Value"),
                    .DataType = v("DataType").Value(Of Integer)(),
                    .DefaultValue = v("DefaultValue"),
                    .Visible = If(v("Visible") IsNot Nothing, v("Visible").Value(Of Boolean)(), True)
                })
                Next
            End If

            sheets.Add(sheet)
        Next

        Return sheets
    End Function




End Class
