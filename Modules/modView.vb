Imports FabV25_WIN8
Imports FabV25_WIN8.AppData

Public Module modView
    Public _pnlPicModelerHandle As IntPtr
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
        ' Check that the panel handle exists
        If Not frmMain.pnlPicmodeler.IsHandleCreated Then Return

        Dim matWidth = GetMaterialWidth(MaterialSheets, _SelectedMaterialID)
        Dim matLength = GetMaterialLength(MaterialSheets, _SelectedMaterialID)

        ' Optional: clear the panel background
        Using g As Graphics = Graphics.FromHwnd(_pnlPicModelerHandle)
            g.Clear(Color.White)  ' or whatever background color you use


            ' Restore state and draw everything
            RestoreAndDrawJob()

            ' Draw sheet, workzones, clamps, etc.
            DrawSheetAndWorkzones(g, matLength, matWidth)

            DrawCadEntities(g, myTransform, panelHeight)

        End Using
    End Sub



End Module
