
Public Class Layer
    Implements IGeoAssociation

    Public Property ID As Integer Implements IGeoAssociation.ID
    Public Property Name As String
    Public Property Color As System.Drawing.Color

    Public Property Visible As Boolean = True

End Class