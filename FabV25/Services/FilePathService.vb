Namespace WE_ENG_V25_0.Core.Services
    Public Class FilePathService
        Public Shared ReadOnly Property BasePath As String
            Get
                Return IO.Path.Combine(Application.StartupPath, "Json")
            End Get
        End Property

        Public Shared Function PathFor(fileName As String) As String
            Return IO.Path.Combine(BasePath, fileName)
        End Function
    End Class
End Namespace
