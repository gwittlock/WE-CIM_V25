Imports System.Drawing.Imaging


Module SaveImage

    Private Declare Auto Function BitBlt Lib "gdi32.dll" (ByVal pHdc As IntPtr, ByVal iX As Integer, _
    ByVal iY As Integer, ByVal iWidth As Integer, ByVal iHeight As Integer, ByVal pHdcSource As IntPtr, _
    ByVal iXSource As Integer, ByVal iYSource As Integer, ByVal dw As System.Int32) As Boolean

    Private Const SRC As Integer = &HCC0020


    Public Sub Convert2BMP(ByVal theObject As Control, ByVal sFilePath As String)
        Try

            theObject.Refresh()
            theObject.Select()

            Dim g As Graphics = theObject.CreateGraphics
            Dim theBitMap As New Bitmap(theObject.ClientSize.Width, theObject.ClientSize.Height, g)
            Dim theBitMap_gr As Graphics = Graphics.FromImage(theBitMap)
            Dim iBitMap_hdc As IntPtr = theBitMap_gr.GetHdc
            Dim me_hdc As IntPtr = g.GetHdc

            BitBlt(iBitMap_hdc, 0, 0, theObject.ClientSize.Width, theObject.ClientSize.Height, me_hdc, 0, 0, SRC)
            g.ReleaseHdc(me_hdc)
            theBitMap_gr.ReleaseHdc(iBitMap_hdc)

            If sFilePath = "" Then Exit Sub
            theBitMap.Save(sFilePath, ImageFormat.Bmp)

        Catch ex As Exception
            MessageBox.Show("Convert2BMP " & ex.Message)
        End Try

    End Sub

End Module
