Imports Newtonsoft.Json
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.WE_ENG_V25_0.Core.State
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Helpers

Namespace WE_ENG_V25_0.Core.Services
    Public Class JsonDataService

        '----------------------------------------------------------
        ' Load ALL JSON files into AppState
        '----------------------------------------------------------
        Public Shared Sub LoadAllData()
            Try
                EnsureJsonFolderExists()

                AppState.Machines = LoadJson(Of Machine)("Machines.json")
                AppState.Stations = LoadJson(Of Station)("Stations.json")
                AppState.Tools = LoadJson(Of Tool)("Tools.json")
                AppState.ToolTypes = LoadJson(Of ToolType)("ToolTypes.json")
                AppState.ToolSetups = LoadJson(Of ToolSetup)("ToolSetups.json")
                AppState.Materials = LoadJson(Of Material)("Materials.json")
                AppState.MachineAttributeTypes = LoadJson(Of MachineAttributeType)("MachineAttributeTypes.json")
                AppState.UDPs = LoadJson(Of UDP)("UDPs.json")

            Catch ex As Exception
                Logging.Log($"FATAL: Error loading JSON data - {ex.Message}")
                Throw
            End Try
        End Sub


        '----------------------------------------------------------
        ' Create Json folder if missing
        '----------------------------------------------------------
        Private Shared Sub EnsureJsonFolderExists()
            Dim folder = FilePathService.BasePath
            If Not IO.Directory.Exists(folder) Then
                IO.Directory.CreateDirectory(folder)
                Logging.Log("Created missing JSON folder.")
            End If
        End Sub


        '----------------------------------------------------------
        ' Load a JSON file. If missing → create empty list.
        '----------------------------------------------------------
        Private Shared Function LoadJson(Of T)(fileName As String) As List(Of T)

            Dim fullPath = FilePathService.PathFor(fileName)

            Try
                ' If file doesn't exist → create empty file
                If Not IO.File.Exists(fullPath) Then
                    IO.File.WriteAllText(fullPath, "[]")
                    Logging.Log($"Created missing JSON file: {fileName}")
                    Return New List(Of T)
                End If

                Dim json = IO.File.ReadAllText(fullPath)

                If String.IsNullOrWhiteSpace(json) Then
                    Logging.Log($"Empty JSON file detected → {fileName}")
                    Return New List(Of T)
                End If

                Return JsonConvert.DeserializeObject(Of List(Of T))(json)

            Catch ex As Exception
                Logging.Log($"ERROR reading {fileName} → {ex.Message}")
                Return New List(Of T)
            End Try

        End Function


        '----------------------------------------------------------
        ' SAVE (Write) list back to JSON file
        '----------------------------------------------------------
        Public Shared Sub SaveJson(Of T)(fileName As String, data As List(Of T))
            Dim fullPath = FilePathService.PathFor(fileName)

            Try
                Dim json = JsonConvert.SerializeObject(data, Formatting.Indented)
                IO.File.WriteAllText(fullPath, json)
            Catch ex As Exception
                Logging.Log($"ERROR saving {fileName} → {ex.Message}")
            End Try
        End Sub

    End Class
End Namespace
