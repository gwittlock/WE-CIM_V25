Public Class JobSheet

    '------------------------------------------------------------
    ' Reference to the Configuration Manager MaterialSheet
    '------------------------------------------------------------
    Public Property MaterialSheetID As Integer

    '------------------------------------------------------------
    ' Sheet dimensions
    ' These are the dimensions of this JobSheet.
    '------------------------------------------------------------
    Public Property Length As Double
    Public Property Width As Double

    '------------------------------------------------------------
    ' Work zones for this JobSheet
    ' Each WorkZone contains its own Clamp collection.
    '------------------------------------------------------------
    Public Property WorkZones As New List(Of WorkZone)

End Class