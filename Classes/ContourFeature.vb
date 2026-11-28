Imports FabV25_WIN8.ToolShapes

Public Class ContourFeature
    Inherits MachiningFeature

    Public Property OverLap As Double = 0.0
    Public Property DOff As Integer = 0
    Public Property Path As IPath2D
    ' Required tool orientation for this contour.
    ' Nothing means use the configured ToolSetupMember.IndexAngle.
    Public Property ToolIndexAngle As Double?

    ' Closure determined using the configured Gap Tolerance.
    ' Does not imply that endpoint coordinates are identical.
    Public Property IsLogicallyClosed As Boolean

    Public Sub New()
        Me.FeatureType = MachiningFeatureType.Profile
    End Sub

End Class