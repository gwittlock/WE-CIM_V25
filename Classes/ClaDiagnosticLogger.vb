Imports System.IO

Public NotInheritable Class DiagnosticLogger

    Private Shared ReadOnly LogPath As String =
        Path.Combine(
            Environment.GetFolderPath(
                Environment.SpecialFolder.LocalApplicationData),
            "WE-CIM",
            "V25_SelectionDebug.txt")

    Public Shared Sub Write(message As String)

        Try
            Dim folder As String =
                Path.GetDirectoryName(LogPath)

            Directory.CreateDirectory(folder)

            Dim logEntry As String =
                DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") &
                " | " & message

            File.AppendAllText(
                LogPath,
                logEntry & Environment.NewLine)

        Catch ex As Exception
            'Debug.WriteLine(
            '    "DiagnosticLogger ERROR: " & ex.Message)
        End Try

    End Sub

End Class