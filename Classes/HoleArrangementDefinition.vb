
Public Class HoleArrangementDefinition

    Public Property ID As Integer
    Public Property Description As String
    Public Property Parameters As New List(Of HoleParameterDefinition)

End Class

Public Class HoleParameterDefinition

    Public Property Name As String
    Public Property Display As String
    Public Property DataType As Integer
    Public Property Editor As String
    Public Property [Default] As Object

End Class


Public Class HoleArrangementDefinitions

    Public Property Arrangements As New List(Of HoleArrangementDefinition)

End Class

