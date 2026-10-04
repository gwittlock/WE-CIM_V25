Public Class CamOperation
    Public Enum OperationTypeEnum
        Unknown
        Cut
        Drill
        Reposition
    End Enum

    Public Property OperationType As OperationTypeEnum = OperationTypeEnum.Unknown
    Public Property ToolName As String
    Public Property WorkZone As String
    Public Property LeadIn As PointF?
    Public Property LeadOut As PointF?
    Public Property Offset As Double = 0
    Public Property FeedRate As Double = 0
    Public Property Speed As Double = 0
    Public Property IsReposition As Boolean = False
    Public Property ClampPositions As List(Of PointF)
    Public Property Tool As Tool
    Public Property Position As (X As Double, Y As Double)
    Public Property Angle As Double

    Public Sub New()
        ClampPositions = New List(Of PointF)
    End Sub


End Class