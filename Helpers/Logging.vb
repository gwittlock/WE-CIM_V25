Namespace WE_ENG_V25_0.Core.Helpers
    Public Class Logging
        Public Shared Sub Log(msg As String)
            IO.File.AppendAllText("log.txt", DateTime.Now.ToString() & " - " & msg & Environment.NewLine)
        End Sub
    End Class
End Namespace
