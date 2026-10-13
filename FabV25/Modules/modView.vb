Imports FabV25_WIN8
Imports FabV25_WIN8.AppData

Public Module modView
    Public _pnlPicModelerHandle As IntPtr
    Public _pnlPicModelerGraphics As Graphics

    Public Structure ViewTransform
        Public Scale As Double
        Public OffsetX As Double
        Public OffsetY As Double
    End Structure

    Public Function BuildFitTransform(
    worldBounds As RectangleF,
    panelWidth As Integer,
    panelHeight As Integer,
    padding As Single) As ViewTransform

        Dim scaleX = (panelWidth - padding * 2) / worldBounds.Width
        Dim scaleY = (panelHeight - padding * 2) / worldBounds.Height

        Dim scale = Math.Min(scaleX, scaleY)

        Dim offsetX = padding - worldBounds.Left * scale
        Dim offsetY = padding - worldBounds.Top * scale

        Return New ViewTransform With {
        .Scale = scale,
        .OffsetX = offsetX,
        .OffsetY = offsetY
    }

    End Function




    Public Sub RedrawAll()
        frmMain.pnlPicmodeler.Invalidate()


    End Sub



End Module
