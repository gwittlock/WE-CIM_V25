Imports System.Data.OleDb
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities


Public Module modFile

    Private m_FileName As String
    Public Function FileOpen() As Boolean

        Using dlg As New DXFFileSelectionDialog()
            If dlg.ShowDialog() = DialogResult.OK Then

                ' Get the selected DXF file path and DxfFile object
                Dim selectedPath As String = dlg.SelectedDxfPath
                Dim loadedDxf As DxfFile = Nothing

                Try
                    loadedDxf = DxfFile.Load(selectedPath)
                Catch ex As Exception
                    MessageBox.Show("Failed to load DXF: " & ex.Message, "DXF Import", MessageBoxButtons.OK, MessageBoxIcon.Error)
                    Return False
                End Try

                ' Show Import Dialog to gather context (Machine, ToolSetup, Material, etc.)
                Using frmOpenProject As New frmOpenProject(selectedPath)
                    If frmOpenProject.ShowDialog() = DialogResult.OK Then

                        ' Process DXF file and draw in pnlPicmodeler
                        ' Use the loaded DxfFile from the dialog instead of re-loading
                        frmMain.LoadDXFToModeler(loadedDxf)
                    End If
                End Using
            End If
        End Using

        Return True
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
