
Imports System.ComponentModel
Imports System.Data.Common
Imports System.Data.OleDb

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

End Class