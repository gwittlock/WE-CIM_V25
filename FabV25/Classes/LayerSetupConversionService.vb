Imports System.IO
Imports System.Linq
Imports System.Text
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq

Public NotInheritable Class LayerSetupConversionService

    Private Sub New()
    End Sub

    Private Const MapFileName As String =
        "LayerSetupConversionMap.json"

    ' Returns True if conversion was performed.
    ' Returns False if a valid conversion already exists.
    Public Shared Function ConvertIfNeeded(
        dataFolder As String) As Boolean

        If String.IsNullOrWhiteSpace(dataFolder) OrElse
           Not Directory.Exists(dataFolder) Then

            Throw New DirectoryNotFoundException(
                "The configuration data folder does not exist.")
        End If

        If AppData.ToolSetups Is Nothing OrElse
           AppData.ToolSetups.Count = 0 Then

            Throw New InvalidOperationException(
                "Tool Setups must be loaded before conversion.")
        End If

        Dim layerPath =
            Path.Combine(dataFolder, "LayerSetups.json")

        Dim directivePath =
            Path.Combine(dataFolder, "Directives.json")

        Dim mapPath =
            Path.Combine(dataFolder, MapFileName)

        ' Never repeat a completed conversion.
        If File.Exists(mapPath) Then
            ValidateCompletedConversion(
                layerPath, directivePath, mapPath)

            Return False
        End If

        If Not File.Exists(layerPath) OrElse
           Not File.Exists(directivePath) Then

            Throw New FileNotFoundException(
                "LayerSetups.json and Directives.json are required.")
        End If

        ' Retain the exact original bytes for rollback.
        Dim originalLayerBytes =
            File.ReadAllBytes(layerPath)

        Dim originalDirectiveBytes =
            File.ReadAllBytes(directivePath)

        Dim sourceLayerTable =
            JObject.Parse(File.ReadAllText(layerPath))

        Dim sourceDirectiveTable =
            JObject.Parse(File.ReadAllText(directivePath))

        Dim sourceLayers =
            ReadRows(sourceLayerTable, "LayerSetups.json")

        Dim sourceDirectives =
            ReadRows(sourceDirectiveTable, "Directives.json")

        ValidateSource(
            sourceLayers, sourceDirectives)

        ' If the map is missing but the data appears converted,
        ' stop instead of risking a second conversion.
        If sourceLayers.Any(
            Function(x) GetRequiredInt(x, "ToolSetupID") <> 0) Then

            Throw New InvalidOperationException(
                "Layer Setups appear to have been converted, " &
                "but the conversion map is missing. " &
                "Restore or inspect the backups before continuing.")
        End If

        ' Back up both originals before building converted output.
        Dim backupDirectory =
            CreateBackup(dataFolder, layerPath, directivePath)

        Dim convertedLayers As New List(Of JObject)
        Dim convertedDirectives As New List(Of JObject)

        Dim layerMappings As New JArray()
        Dim directiveMappings As New JArray()

        Dim nextLayerID =
            sourceLayers.Max(Function(x) GetRequiredInt(x, "ID")) + 1

        Dim nextDirectiveID =
            sourceDirectives.Max(Function(x) GetRequiredInt(x, "ID")) + 1

        Dim toolSetups = AppData.ToolSetups.ToList()

        If toolSetups.Select(Function(x) x.ID).Distinct().Count() <>
           toolSetups.Count Then

            Throw New InvalidOperationException(
                "Duplicate Tool Setup IDs were found.")
        End If

        Dim matchedSourceLayerIDs As New HashSet(Of Integer)

        For Each toolSetup In toolSetups

            If String.IsNullOrWhiteSpace(toolSetup.Description) Then
                Throw New InvalidOperationException(
                    $"Tool Setup ID {toolSetup.ID} has no description.")
            End If

            Dim matchingLayers As List(Of JObject) =
     sourceLayers.Where(
         Function(layerRow)
             Return GetRequiredInt(layerRow, "MachineID") = toolSetup.MachineID
         End Function
     ).ToList()

            For Each sourceLayer In matchingLayers

                              Dim sourceLayerID =
                                  GetRequiredInt(sourceLayer, "ID")

                              matchedSourceLayerIDs.Add(sourceLayerID)

                              Dim newLayerID = nextLayerID
                              nextLayerID += 1

                          Dim newLayer =
                                  JObject.Parse(sourceLayer.ToString(Formatting.None))

                          newLayer("ID") = newLayerID
                              newLayer("ToolSetupID") = toolSetup.ID
                              newLayer("RequiresToolIDVerification") = True

                              newLayer("Description") =
                                  toolSetup.Description & " - " &
                                  CStr(sourceLayer("Description"))

                              convertedLayers.Add(newLayer)

                              layerMappings.Add(New JObject From {
                                  {"SourceLayerSetupID", sourceLayerID},
                                  {"ConvertedLayerSetupID", newLayerID},
                                  {"ToolSetupID", toolSetup.ID},
                                  {"MachineID", toolSetup.MachineID},
                                  {"Description", CStr(newLayer("Description"))}
                              })

                Dim relatedDirectives As List(Of JObject) =
                            sourceDirectives.Where(
                            Function(directiveRow)
                                Return GetRequiredInt(directiveRow, "LayerSetupID") = sourceLayerID
                            End Function
                            ).ToList()

                For Each sourceDirective In relatedDirectives

                                                Dim sourceDirectiveID =
                                      GetRequiredInt(sourceDirective, "ID")

                                                Dim oldDirection =
                                      GetRequiredInt(sourceDirective, "CutDirection")

                                                Dim newDirection =
                                      MapCutDirection(oldDirection)

                                                Dim newDirectiveID = nextDirectiveID
                                                nextDirectiveID += 1

                                        Dim newDirective =
                                      JObject.Parse(sourceDirective.ToString(Formatting.None))

                                        newDirective("ID") = newDirectiveID
                                                newDirective("LayerSetupID") = newLayerID
                                                newDirective("CutDirection") = newDirection

                                                convertedDirectives.Add(newDirective)

                                                directiveMappings.Add(New JObject From {
                                      {"SourceDirectiveID", sourceDirectiveID},
                                      {"ConvertedDirectiveID", newDirectiveID},
                                      {"SourceLayerSetupID", sourceLayerID},
                                      {"ConvertedLayerSetupID", newLayerID},
                                      {"ToolSetupID", toolSetup.ID},
                                      {"SourceCutDirection", oldDirection},
                                      {"ConvertedCutDirection", newDirection}
                                  })
                                            Next
            Next
        Next

        If matchedSourceLayerIDs.Count <> sourceLayers.Count Then
            Throw New InvalidOperationException(
                "At least one source Layer Setup has no matching " &
                "Tool Setup for its MachineID. Conversion cancelled.")
        End If

        Dim validToolSetupIDs As New HashSet(Of Integer)(toolSetups.Select(Function(toolSetupRow) toolSetupRow.ID))

        ValidateConvertedRows(convertedLayers, convertedDirectives, validToolSetupIDs)

        If convertedLayers.Count <> 16 OrElse
   convertedDirectives.Count <> 16 Then

            Throw New InvalidOperationException(
        "Unexpected conversion counts. Expected 16 Layer Setups " &
        "and 16 Directives, but generated " &
        convertedLayers.Count.ToString() & " Layer Setups and " &
        convertedDirectives.Count.ToString() & " Directives.")
        End If

        If convertedLayers.Count <> layerMappings.Count OrElse
           convertedDirectives.Count <> directiveMappings.Count Then

            Throw New InvalidOperationException(
                "Conversion map counts do not match the converted rows.")
        End If

        ' Preserve the original table names and column metadata.
        Dim outputLayerTable =
            JObject.Parse(sourceLayerTable.ToString(Formatting.None))

        Dim outputDirectiveTable =
            JObject.Parse(sourceDirectiveTable.ToString(Formatting.None))

        outputLayerTable("Rows") =
            JArray.FromObject(convertedLayers)

        outputDirectiveTable("Rows") =
            JArray.FromObject(convertedDirectives)

        Dim conversionMap As New JObject From {
            {"Version", 1},
            {"ConvertedAtUtc", DateTime.UtcNow},
            {"BackupDirectory", Path.Combine("ConversionBackup", Path.GetFileName(backupDirectory))},
            {"SourceLayerSetupCount", sourceLayers.Count},
            {"SourceDirectiveCount", sourceDirectives.Count},
            {"ConvertedLayerSetupCount", convertedLayers.Count},
            {"ConvertedDirectiveCount", convertedDirectives.Count},
            {"LayerSetupMappings", layerMappings},
            {"DirectiveMappings", directiveMappings}
        }

        SaveWithRollback(
            layerPath,
            directivePath,
            mapPath,
            originalLayerBytes,
            originalDirectiveBytes,
            JsonConvert.SerializeObject(
                outputLayerTable, Formatting.Indented),
            JsonConvert.SerializeObject(
                outputDirectiveTable, Formatting.Indented),
            JsonConvert.SerializeObject(
                conversionMap, Formatting.Indented))

        Return True
    End Function

    Private Shared Function ReadRows(
        table As JObject,
        filename As String) As List(Of JObject)

        Dim rows = TryCast(table("Rows"), JArray)

        If rows Is Nothing Then
            Throw New InvalidDataException(
                $"{filename} does not contain a Rows array.")
        End If

        Return rows.
            Select(Function(x) TryCast(x, JObject)).
            ToList()
    End Function

    Private Shared Function GetRequiredInt(
        row As JObject,
        field As String) As Integer

        Dim token = row(field)

        If token Is Nothing OrElse
           token.Type <> JTokenType.Integer Then

            Throw New InvalidDataException(
                $"Missing or invalid integer field '{field}'.")
        End If

        Return token.Value(Of Integer)()
    End Function

    Private Shared Sub ValidateSource(
        layers As List(Of JObject),
        directives As List(Of JObject))

        If layers.Count = 0 OrElse directives.Count = 0 Then
            Throw New InvalidDataException(
                "Source Layer Setups or Directives are empty.")
        End If

        If layers.Select(Function(x) GetRequiredInt(x, "ID")).
            Distinct().Count() <> layers.Count Then

            Throw New InvalidDataException(
                "Source Layer Setup IDs are not unique.")
        End If

        If directives.Select(Function(x) GetRequiredInt(x, "ID")).
            Distinct().Count() <> directives.Count Then

            Throw New InvalidDataException(
                "Source Directive IDs are not unique.")
        End If

        Dim layerIDs As New HashSet(Of Integer)(
            layers.Select(Function(x) GetRequiredInt(x, "ID")))

        For Each layer In layers

            If GetRequiredInt(layer, "ToolSetupID") <> 0 Then
                Throw New InvalidOperationException(
                    "Source Layer Setups contain nonzero ToolSetupIDs.")
            End If

            If String.IsNullOrWhiteSpace(
                CStr(layer("Description"))) Then

                Throw New InvalidDataException(
                    "A source Layer Setup has no description.")
            End If
        Next

        For Each directive In directives

            Dim parentID =
                GetRequiredInt(directive, "LayerSetupID")

            If Not layerIDs.Contains(parentID) Then
                Throw New InvalidDataException(
                    $"Directive ID {GetRequiredInt(directive, "ID")} " &
                    $"references missing Layer Setup ID {parentID}.")
            End If

            If String.IsNullOrWhiteSpace(
                CStr(directive("CADLayer"))) Then

                Throw New InvalidDataException(
                    "A source Directive has an empty CADLayer.")
            End If
        Next
    End Sub

    Private Shared Function MapCutDirection(
        sourceValue As Integer) As Integer

        Select Case sourceValue
            Case 0
                Return 1       ' CW -> V25 CW

            Case 1
                Return -1      ' CCW -> V25 CCW

            Case 2
                Return 0       ' Auto -> V25 Auto

            Case 3
                Return 2       ' As Specified -> V25 As Specified

            Case Else
                Throw New InvalidDataException(
                    $"Unsupported source CutDirection value: {sourceValue}.")
        End Select
    End Function

    Private Shared Sub ValidateConvertedRows(
        layers As List(Of JObject),
        directives As List(Of JObject),
        validToolSetupIDs As HashSet(Of Integer))

        If layers.Count = 0 OrElse directives.Count = 0 Then
            Throw New InvalidOperationException(
                "Conversion produced no Layer Setups or Directives.")
        End If

        If layers.Select(Function(x) GetRequiredInt(x, "ID")).
            Distinct().Count() <> layers.Count Then

            Throw New InvalidOperationException(
                "Converted Layer Setup IDs are not unique.")
        End If

        If directives.Select(Function(x) GetRequiredInt(x, "ID")).
            Distinct().Count() <> directives.Count Then

            Throw New InvalidOperationException(
                "Converted Directive IDs are not unique.")
        End If

        Dim layerIDs As New HashSet(Of Integer)(
            layers.Select(Function(x) GetRequiredInt(x, "ID")))

        For Each layer In layers

            Dim toolSetupID =
                GetRequiredInt(layer, "ToolSetupID")

            If Not validToolSetupIDs.Contains(toolSetupID) Then
                Throw New InvalidOperationException(
                    $"Layer Setup references unknown Tool Setup ID {toolSetupID}.")
            End If

            If layer("RequiresToolIDVerification") Is Nothing OrElse
               Not layer("RequiresToolIDVerification").Value(Of Boolean)() Then

                Throw New InvalidOperationException(
                    "Converted Layer Setup must require Tool ID verification.")
            End If
        Next

        For Each directive In directives

            Dim parentID =
                GetRequiredInt(directive, "LayerSetupID")

            If Not layerIDs.Contains(parentID) Then
                Throw New InvalidDataException("Directive ID " & GetRequiredInt(directive, "ID").ToString() & " references missing Layer Setup ID " & parentID.ToString() & ".")
            End If
        Next
    End Sub

    Private Shared Function CreateBackup(
        dataFolder As String,
        layerPath As String,
        directivePath As String) As String

        Dim backupRoot =
            Path.Combine(dataFolder, "ConversionBackup")

        Directory.CreateDirectory(backupRoot)

        Dim stamp = DateTime.UtcNow.ToString(
            "yyyyMMdd_HHmmss_fff")

        Dim backupDirectory =
            Path.Combine(backupRoot, stamp)

        Dim suffix As Integer = 1

        While Directory.Exists(backupDirectory)
            backupDirectory =
                Path.Combine(backupRoot, stamp & "_" & suffix)
            suffix += 1
        End While

        Directory.CreateDirectory(backupDirectory)

        ' No overwrite: each backup is kept independently.
        File.Copy(
            layerPath,
            Path.Combine(backupDirectory, "LayerSetups.json"),
            False)

        File.Copy(
            directivePath,
            Path.Combine(backupDirectory, "Directives.json"),
            False)

        Dim backupLayerPath =
            Path.Combine(backupDirectory, "LayerSetups.json")

        Dim backupDirectivePath =
            Path.Combine(backupDirectory, "Directives.json")

        If Not File.Exists(backupLayerPath) OrElse
           Not File.Exists(backupDirectivePath) Then

            Throw New IOException(
                "Both conversion backup files must exist before conversion.")
        End If

        If Not File.ReadAllBytes(layerPath).SequenceEqual(
            File.ReadAllBytes(backupLayerPath)) Then

            Throw New IOException(
                "LayerSetups.json backup does not match the original.")
        End If

        If Not File.ReadAllBytes(directivePath).SequenceEqual(
            File.ReadAllBytes(backupDirectivePath)) Then

            Throw New IOException(
                "Directives.json backup does not match the original.")
        End If


        Return backupDirectory
    End Function

    Private Shared Sub SaveWithRollback(
        layerPath As String,
        directivePath As String,
        mapPath As String,
        originalLayerBytes As Byte(),
        originalDirectiveBytes As Byte(),
        layerJson As String,
        directiveJson As String,
        mapJson As String)

        Dim suffix = Guid.NewGuid().ToString("N")

        Dim tempLayer = layerPath & "." & suffix & ".tmp"
        Dim tempDirective = directivePath & "." & suffix & ".tmp"
        Dim tempMap = mapPath & "." & suffix & ".tmp"

        Try
            File.WriteAllText(
                tempLayer, layerJson, New UTF8Encoding(False))

            File.WriteAllText(
                tempDirective, directiveJson, New UTF8Encoding(False))

            File.WriteAllText(
                tempMap, mapJson, New UTF8Encoding(False))

            ' The conversion map is written last and acts as the marker.
            File.Copy(tempLayer, layerPath, True)
            File.Copy(tempDirective, directivePath, True)
            File.Copy(tempMap, mapPath, False)

        Catch saveException As Exception

            Try
                File.WriteAllBytes(layerPath, originalLayerBytes)
                File.WriteAllBytes(directivePath, originalDirectiveBytes)

                If File.Exists(mapPath) Then
                    File.Delete(mapPath)
                End If

            Catch rollbackException As Exception

                Throw New InvalidOperationException(
                    "Conversion save failed, and automatic rollback " &
                    "also failed. Restore both JSON files from " &
                    "ConversionBackup. Save error: " &
                    saveException.Message &
                    ". Rollback error: " &
                    rollbackException.Message,
                    saveException)
            End Try

            Throw New InvalidOperationException(
                "Conversion save failed. Original JSON files were restored.",
                saveException)

        Finally
            Try
                If File.Exists(tempLayer) Then File.Delete(tempLayer)
            Catch
                ' Preserve the original save or rollback error.
            End Try

            Try
                If File.Exists(tempDirective) Then File.Delete(tempDirective)
            Catch
                ' Preserve the original save or rollback error.
            End Try

            Try
                If File.Exists(tempMap) Then File.Delete(tempMap)
            Catch
                ' Preserve the original save or rollback error.
            End Try
        End Try
    End Sub
    Private Shared Sub ValidateCompletedConversion(
    layerPath As String,
    directivePath As String,
    mapPath As String)

        If Not File.Exists(layerPath) OrElse
       Not File.Exists(directivePath) Then

            Throw New InvalidDataException(
            "The conversion map exists, but a converted data file is missing.")
        End If

        Dim map = JObject.Parse(File.ReadAllText(mapPath))
        Dim layerTable = JObject.Parse(File.ReadAllText(layerPath))
        Dim directiveTable = JObject.Parse(File.ReadAllText(directivePath))

        Dim layers = ReadRows(layerTable, "LayerSetups.json")
        Dim directives = ReadRows(directiveTable, "Directives.json")

        If layers.Count <>
       GetRequiredInt(map, "ConvertedLayerSetupCount") OrElse
       directives.Count <>
       GetRequiredInt(map, "ConvertedDirectiveCount") Then

            Throw New InvalidDataException(
            "Converted file counts do not match the conversion map.")
        End If

        If layers.Count = 0 OrElse directives.Count = 0 Then
            Throw New InvalidDataException(
            "The converted Layer Setups or Directives are empty.")
        End If

        ' Validate Layer Setup IDs.
        Dim layerIDs As New HashSet(Of Integer)(
        layers.Select(Function(x) GetRequiredInt(x, "ID")))

        If layerIDs.Count <> layers.Count Then
            Throw New InvalidDataException(
            "Converted Layer Setup IDs are duplicated.")
        End If

        ' Validate Directive IDs.
        Dim directiveIDs As New HashSet(Of Integer)(
        directives.Select(Function(x) GetRequiredInt(x, "ID")))

        If directiveIDs.Count <> directives.Count Then
            Throw New InvalidDataException(
            "Converted Directive IDs are duplicated.")
        End If

        ' Read the valid Tool Setup IDs currently loaded by AppData.
        If AppData.ToolSetups Is Nothing OrElse
       AppData.ToolSetups.Count = 0 Then

            Throw New InvalidOperationException(
            "Tool Setups must be loaded before validating converted data.")
        End If

        Dim validToolSetupIDs As New HashSet(Of Integer)(
        AppData.ToolSetups.Select(Function(x) x.ID))

        ' Validate each converted Layer Setup.
        For Each layer In layers

            Dim toolSetupID =
            GetRequiredInt(layer, "ToolSetupID")

            If Not validToolSetupIDs.Contains(toolSetupID) Then
                Throw New InvalidDataException(
                $"Layer Setup ID {GetRequiredInt(layer, "ID")} " &
                $"references unknown Tool Setup ID {toolSetupID}.")
            End If

            Dim verificationToken = layer("RequiresToolIDVerification")

            If verificationToken Is Nothing OrElse
           verificationToken.Type <> JTokenType.Boolean OrElse
           Not verificationToken.Value(Of Boolean)() Then

                Throw New InvalidDataException(
                $"Layer Setup ID {GetRequiredInt(layer, "ID")} " &
                "must require Tool ID verification.")
            End If
        Next

        ' Validate each converted Directive's parent.
        For Each directive In directives

            Dim parentID =
            GetRequiredInt(directive, "LayerSetupID")

            If Not layerIDs.Contains(parentID) Then
                Throw New InvalidDataException(
                "Directive ID " &
                GetRequiredInt(directive, "ID").ToString() &
                " references missing Layer Setup ID " &
                parentID.ToString() & ".")
            End If
        Next
    End Sub

End Class
