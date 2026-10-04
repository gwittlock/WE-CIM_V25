Namespace PunchResult

    Public Enum PunchResultType
        ExactMatch
        Decomposed
        Failed
    End Enum

    Public Class PunchResult
        Public Property ResultType As PunchResultType
        Public Property ToolID As Integer
    End Class

End Namespace
