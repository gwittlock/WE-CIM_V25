Imports System.Collections.Generic

Public Class JobSheet

    Public Property MaterialSheetID As Integer
    Public Property Length As Double
    Public Property Width As Double

    Public Property WorkZones As New List(Of WorkZone)

    Public Property Parts As New List(Of Part)

End Class