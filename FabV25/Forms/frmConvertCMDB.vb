
Imports System.ComponentModel
Imports System.Data.Common
Imports System.Data.OleDb
Imports System.IO
Imports Newtonsoft.Json.Linq

Public Class frmConvertCMDB


    Private Sub picCMDBFile_Click(sender As Object, e As EventArgs) Handles picCMDBFile.Click
        ofdCMDBFile.Title = "Select CMDB File"
        ofdCMDBFile.Filter = "CMDB Files (*.mdb)|*.mdb"
        If ofdCMDBFile.ShowDialog() = DialogResult.OK Then
            txtCMDBFile.Text = ofdCMDBFile.FileName
        End If
    End Sub

    Private Sub picJsonFolder_Click(sender As Object, e As EventArgs) Handles picJsonFolder.Click
        fbdJsonDataFolder.Description = "Select Folder to Save JSON Data"
        If fbdJsonDataFolder.ShowDialog() = DialogResult.OK Then
            txtDataFolder.Text = fbdJsonDataFolder.SelectedPath
        End If
    End Sub

    Private Sub BtnConvertCMDB_Click(sender As Object, e As EventArgs) Handles btnConvertCMDB.Click

        ExportAllTables(txtCMDBFile.Text, txtDataFolder.Text)
        AppData.LoadAllJson()
        '  ExportMaterialJson(txtCMDBFile.Text, txtDataFolder.Text)
    End Sub



    ' Helper: Convert OleDb data type number to readable name
    Private Function GetOleDbTypeName(dataType As Integer) As String
        Select Case dataType
            Case 2 : Return "SmallInt"
            Case 3 : Return "Integer"
            Case 4 : Return "Single"
            Case 5 : Return "Double"
            Case 6 : Return "Currency"
            Case 7 : Return "DateTime"
            Case 11 : Return "Boolean"
            Case 17 : Return "Byte"
            Case 20 : Return "Long"         ' 64-bit integer
            Case 72 : Return "ReplicationID"
            Case 130 : Return "Text/Memo"
            Case 202 : Return "Text"
            Case 203 : Return "Memo"
            Case 204 : Return "Binary"
            Case 205 : Return "OLE Object"
            Case Else : Return "Type" & dataType
        End Select
    End Function


    Private Sub btnCancel_Click(sender As Object, e As EventArgs) Handles btnCancel.Click
        Me.Dispose()

    End Sub

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click

        Dim filesToCheck As New List(Of String) From {
        "Directives.json",
        "LayerSetups.json",
        "MachineAttributes.json",
        "MachineAttributeTypes.json",
        "Machines.json",
        "MachineTools.json",
        "MachineToolTypes.json",
        "MachineTypeAttributes.json",
        "MachineTypes.json",
        "MaterialInventory.json",
        "MaterialTypes.json",
        "StationConfiguration.json",
        "ToolAttributeTypes.json",
        "ToolCrib.json",
        "ToolSetupMembers.json",
        "ToolSetups.json",
        "ToolTypeAttributes.json",
        "ToolTypes.json"
    }

        ValidateJsonFilesIgnoreRowOrder("C:\Program Files (x86)\WE-CIM\25.0\Data", "C:\Program Files (x86)\WE-CIM\24.0\Data", filesToCheck)
    End Sub



    Public Sub ValidateJsonFilesIgnoreRowOrder(referenceFolder As String, generatedFolder As String, files As List(Of String))

        Dim filePath As String = "C:\Program Files (x86)\WE-CIM\24.0\Data\ExportTable_Compare.txt"

        ' Using statement ensures the StreamWriter is disposed of properly
        Using writer As New StreamWriter(filePath, True) ' Setting the second parameter to True appends text

            For Each fileName In files
                Dim refPath = Path.Combine(referenceFolder, fileName)
                Dim genPath = Path.Combine(generatedFolder, fileName)

                If Not File.Exists(refPath) Then
                    writer.WriteLine($"Reference file missing: {refPath}")
                    Continue For
                End If

                If Not File.Exists(genPath) Then
                    writer.WriteLine($"Generated file missing: {genPath}")
                    Continue For
                End If

                Dim refJson As JObject = JObject.Parse(File.ReadAllText(refPath))
                Dim genJson As JObject = JObject.Parse(File.ReadAllText(genPath))

                ' Check TableName
                If refJson("TableName")?.ToString() <> genJson("TableName")?.ToString() Then
                    writer.WriteLine($"TableName mismatch in {fileName}")
                End If

                ' Check columns
                If refJson("Columns") IsNot Nothing Then
                    Dim refCols = refJson("Columns").Select(Function(c) c("Field")?.ToString()).ToList()
                    Dim genCols = If(genJson("Columns") IsNot Nothing, genJson("Columns").Select(Function(c) c("Field")?.ToString()).ToList(), New List(Of String)())
                    For Each col In refCols
                        If Not genCols.Contains(col) Then
                            writer.WriteLine($"Missing column '{col}' in {fileName}")
                        End If
                    Next
                End If

                ' Check rows with ignoring order
                Dim refRows = refJson("Rows")
                Dim genRows = genJson("Rows")
                If refRows IsNot Nothing AndAlso genRows IsNot Nothing Then
                    ' Create dictionaries keyed by ID if present, otherwise fallback to full row string
                    Dim refDict As New Dictionary(Of String, JObject)
                    For Each r As JObject In refRows
                        Dim key = If(r("ID") IsNot Nothing, r("ID").ToString(), r.ToString())
                        refDict(key) = r
                    Next

                    Dim genDict As New Dictionary(Of String, JObject)
                    For Each r As JObject In genRows
                        Dim key = If(r("ID") IsNot Nothing, r("ID").ToString(), r.ToString())
                        genDict(key) = r
                    Next

                    For Each kvp In refDict
                        Dim key = kvp.Key
                        Dim refRow = kvp.Value
                        If Not genDict.ContainsKey(key) Then
                            'writer.WriteLine($"Missing row with key '{key}' in {fileName}")
                            Continue For
                        End If

                        Dim genRow = genDict(key)
                        ' Compare fields
                        For Each prop As JProperty In refRow.Children(Of JProperty)()
                            Dim pKey = prop.Name
                            If TypeOf prop.Value Is JArray Then
                                ' Nested array
                                Dim refChildren = CType(prop.Value, JArray)
                                Dim genChildren = CType(genRow(pKey), JArray)
                                If refChildren.Count > 0 Then
                                    If genChildren Is Nothing OrElse genChildren.Count = 0 Then
                                        writer.WriteLine($"Missing child array '{pKey}' in row '{key}' of {fileName}")
                                        Continue For
                                    End If
                                    ' Compare child fields
                                    Dim refChildFields = refChildren(0).Select(Function(c) c.Path.Split(".").Last()).ToList()
                                    Dim genChildFields = genChildren(0).Select(Function(c) c.Path.Split(".").Last()).ToList()
                                    For Each f In refChildFields
                                        If Not genChildFields.Contains(f) Then
                                            writer.WriteLine($"Missing field '{f}' in child array '{pKey}' row '{key}' of {fileName}")
                                        End If
                                    Next
                                End If
                            Else
                                ' Normal field
                                If genRow(pKey) Is Nothing Then
                                    writer.WriteLine($"Missing field '{pKey}' in row '{key}' of {fileName}")
                                End If
                            End If
                        Next
                    Next
                End If

                writer.WriteLine($"Validation completed for {fileName}")
                writer.WriteLine("")
                writer.WriteLine("")

            Next
        End Using
        MessageBox.Show("Done")
    End Sub

    Private Sub frmConvertCMDB_Load(sender As Object, e As EventArgs) Handles Me.Load
        txtDataFolder.Text = "C:\Program Files (x86)\WE-CIM\24.0\Data"
    End Sub
End Class