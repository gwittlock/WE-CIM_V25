Imports FabV25_WIN8.AppData
Public Class Clamp
    Public Property Name As String
    Public Property TorchSize As SizeF
    ' Sheet-relative center
    Public Property X As Double
    Public Property Y As Double

    ' Exclusion size
    Public Property Width As Double
    Public Property Height As Double

    Public ReadOnly Property ExclusionRect As RectangleF
        Get
            Dim matWidth = GetMaterialWidth(MaterialSheets, _SelectedMaterialID)
            Dim matLength = GetMaterialLength(MaterialSheets, _SelectedMaterialID)
            Dim bl As PointF = WorldToScreen(0, 0, myTransform, panelHeight)
            Dim tr As PointF = WorldToScreen(matLength, matWidth, myTransform, panelHeight)

            Return HelperFunctions.RectFromPoints(bl, tr) ' where bl/tr = calculated points

        End Get
    End Property
End Class
