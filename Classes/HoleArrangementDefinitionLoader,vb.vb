
Imports System.IO
Imports Newtonsoft.Json

Public Class HoleArrangementDefinitionLoader

    Public Shared Function Load(filePath As String) _
        As HoleArrangementDefinitions

        If Not File.Exists(filePath) Then
            Throw New FileNotFoundException(
                "Hole arrangement definition file not found.",
                filePath)
        End If

        Dim json As String = File.ReadAllText(filePath)

        Dim definitions As HoleArrangementDefinitions =
            JsonConvert.DeserializeObject(Of HoleArrangementDefinitions)(json)

        If definitions Is Nothing OrElse
           definitions.Arrangements Is Nothing OrElse
           definitions.Arrangements.Count = 0 Then

            Throw New InvalidDataException(
                "No hole arrangement definitions were found.")
        End If

        Return definitions

    End Function

End Class
