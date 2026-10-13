Module modPath
    Public _selectedDxfPath As String


    Public Function NormalizePath(ByVal sPath As String) As String

        If Right$(sPath, 1) <> "\" Then
            NormalizePath = sPath & "\"
        Else
            NormalizePath = sPath
        End If

    End Function

End Module
