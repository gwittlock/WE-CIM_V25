Imports FabV25_WIN8.ToolShapes

Public Class ContourFeature
    Inherits MachiningFeature

    Public Property Path As IPath2D

    ' Closure determined using the configured Gap Tolerance.
    ' Does not imply that endpoint coordinates are identical.
    Public Property IsLogicallyClosed As Boolean

    Public Sub New()
        Me.FeatureType = MachiningFeatureType.Profile
    End Sub

End Class