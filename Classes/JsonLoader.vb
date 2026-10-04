Imports System.IO
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq

Public Module JsonLoader

    Public Function LoadList(Of T)(filePath As String) As List(Of T)
        If Not File.Exists(filePath) Then Return New List(Of T)
        Dim json = File.ReadAllText(filePath)
        Return JsonConvert.DeserializeObject(Of List(Of T))(json)
    End Function

End Module
