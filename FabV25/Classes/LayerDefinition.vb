Public Class LayerDefinition


    Public Class ImportedEntity
        Public Property CAMLayer As String
        Public Property ColorMode As DxfGeometryHelpers.ColorSourceMode
        Public Property ExplicitColor As Color   ' Only valid if ByEntity
        Public Property DisplayColor As Color    ' Final, resolved (late)
    End Class




    Public Class LayerColorResolver

        Private ReadOnly _layerSetup As AppData.layersetup

        Public Sub New(layerSetup As AppData.layersetup)
            _layerSetup = layerSetup
        End Sub

    End Class

End Class
