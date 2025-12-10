Imports System.Data.OleDb


Public Module modFile
    Private m_FileName As String
    Public Function FileOpen() As Boolean

        Using dlg As New DXFFileSelectionDialog()
            If dlg.ShowDialog() = DialogResult.OK Then
                Dim selectedFile As String = GetFilename()
                If String.IsNullOrEmpty(selectedFile) Then Return -1

                ' Show Import Dialog to gather context (Machine, ToolSetup, Material, etc.)
                Using frmOpenProject As New frmOpenProject
                    If frmOpenProject.ShowDialog() = DialogResult.OK Then

                        ' Process DXF file and draw in pnlPicmodeler
                        frmMain.ProcessDXFAndDraw(selectedFile)
                    End If
                End Using
            End If
        End Using

        Return 0
    End Function

    'Private Sub CenterAndScaleDXF()
    '    If frmMain.DXFLines.Count = 0 Then Exit Sub

    '    Dim minX = frmMain.DXFLines.Min(Function(l) Math.Min(l.X1, l.X2))
    '    Dim maxX = frmMain.DXFLines.Max(Function(l) Math.Max(l.X1, l.X2))
    '    Dim minY = frmMain.DXFLines.Min(Function(l) Math.Min(l.Y1, l.Y2))
    '    Dim maxY = frmMain.DXFLines.Max(Function(l) Math.Max(l.Y1, l.Y2))

    '    Dim dxfWidth = maxX - minX
    '    Dim dxfHeight = maxY - minY

    '    Dim panelWidth = frmMain.pnlPicmodeler.Width
    '    Dim panelHeight = frmMain.pnlPicmodeler.Height

    '    frmMain.PicModelerScale = Math.Min(panelWidth / dxfWidth, panelHeight / dxfHeight) * 0.9F ' 10% margin
    '    frmMain.OffsetX = (panelWidth - dxfWidth * frmMain.PicModelerScale) / 2 - minX * frmMain.PicModelerScale
    '    frmMain.OffsetY = (panelHeight - dxfHeight * frmMain.PicModelerScale) / 2 - minY * frmMain.PicModelerScale
    'End Sub



    Public Sub SetFilename(ByVal Filename As String)
        m_FileName = Filename
    End Sub

    Public Function GetFilename() As String
        GetFilename = m_FileName
    End Function
End Module
