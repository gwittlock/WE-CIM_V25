
Public Class MM3FileHeader

    Public Property Format As String = "WE-CIM-MM3"

    Public Property Version As Integer = 1

    Public Property ModelGUID As Guid

    Public Property ModelType As String

    Public Property DerivedFromGUID As Guid?

    Public Property CreatedWith As String

    Public Property LastSavedWith As String

    Public Property CreatedAt As DateTimeOffset

    Public Property LastSavedAt As DateTimeOffset

End Class
