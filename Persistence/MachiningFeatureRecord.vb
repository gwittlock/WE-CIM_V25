Public Class MachiningFeatureRecord

    Public Property ID As Integer

    Public Property FeatureType As MachiningFeatureType

    Public Property ConcreteType As String

    Public Property Description As String

    Public Property ToolSetupMemberID As Integer

    Public Property Data As New Dictionary(Of String, Object)

End Class