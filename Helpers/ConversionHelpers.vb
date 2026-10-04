Namespace WE_ENG_V25_0.Core.Helpers
    Public Class ConversionHelpers
        ' Conversion utilities go here

        ''' <summary>
        ''' Computes scale and offsets to fit a material into a panel with optional padding
        ''' </summary>
        Public Shared Sub ComputeMaterialView(materialWidth As Double,
                                   materialHeight As Double,
                                   panelWidth As Integer,
                                   panelHeight As Integer,
                                   ByRef scale As Double,
                                   ByRef offsetX As Double,
                                   ByRef offsetY As Double,
                                       Optional padding As Integer = 20)

            ' Compute scale to fit material inside panel (maintaining aspect ratio)
            Dim scaleX As Double = (panelWidth - 2 * padding) / materialWidth
            Dim scaleY As Double = (panelHeight - 2 * padding) / materialHeight
            scale = Math.Min(scaleX, scaleY)

            ' Center the material in the panel
            offsetX = padding + ((panelWidth - 2 * padding) - (materialWidth * scale)) / 2
            offsetY = padding + ((panelHeight - 2 * padding) - (materialHeight * scale)) / 2
        End Sub
    End Class

End Namespace
