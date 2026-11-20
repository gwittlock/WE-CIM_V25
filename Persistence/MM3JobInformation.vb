
Public Class MM3JobInformation

    Public Property Customer As String

    Public Property JobNumber As String

    Public Property PONumber As String

    Public Property Description As String

    Public Property DueDate As DateTime?

    Public Property Notes As String

    Public Property UDPs As New List(Of MM3JobUDP)

End Class

Public Class MM3JobUDP

    Public Property Name As String

    Public Property DataType As String

    Public Property Value As Object

End Class
