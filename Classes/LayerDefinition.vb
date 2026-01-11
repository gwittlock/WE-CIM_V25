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

        'Public Function Resolve(imp As DxfGeometryHelpers.ImportedEntity) As Color
        '    Dim color As Color

        '    Select Case imp.ColorMode
        '        Case ColorSourceMode.ByEntity
        '            color = imp.ExplicitColor

        '        Case ColorSourceMode.ByLayer
        '            color = GetLayerColor(imp.CAMLayer)

        '        Case ColorSourceMode.Fallback
        '            color = Color.Black
        '    End Select

        '    Debug.WriteLine(
        '    $"Resolve → Layer='{imp.CAMLayer}', Mode={imp.ColorMode}, Result={color}")

        '    Return color
        'End Function

        'Private Function ResolveLayerColor(layerName As String) As Color
        '    Dim layer = _layerSetup.Layers _
        '    .FirstOrDefault(Function(l) _
        '        l.CADLayer.Equals(layerName, StringComparison.OrdinalIgnoreCase))

        '    If layer Is Nothing Then
        '        layer = _layerSetup.Layers _
        '        .FirstOrDefault(Function(l) _
        '            l.CADLayer.Equals("*default*", StringComparison.OrdinalIgnoreCase))
        '    End If

        '    If layer IsNot Nothing Then
        '        Return ResolveDirectiveColor(layer)
        '    End If

        '    Return Color.Black
        'End Function

    End Class

    'Public Shared Function GetLayerColor(layerName As String) As Color
    '    Debug.WriteLine($"GetLayerColor lookup: '{layerName}'")

    '    ' Exact layer match
    '    Dim layer = SelectedLayerSetup.Layers.FirstOrDefault(
    '        Function(l) l.CADLayer.Equals(layerName, StringComparison.OrdinalIgnoreCase))

    '    If layer IsNot Nothing Then
    '        Return ResolveDirectiveColor(layer)
    '    End If

    '    ' *default* fallback
    '    layer = SelectedLayerSetup.Layers.FirstOrDefault(
    '        Function(l) l.CADLayer.Equals("*default*", StringComparison.OrdinalIgnoreCase))

    '    If layer IsNot Nothing Then
    '        Return ResolveDirectiveColor(layer)
    '    End If

    '    ' Absolute fallback
    '    Return Color.Black
    'End Function
End Class
