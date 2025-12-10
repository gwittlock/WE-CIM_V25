Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.IO
Imports System.Windows.Forms

Public Class DXFFileSelectionDialog
    Inherits Form

    '' Controls
    'Private tvFolders As New TreeView
    'Private lstFiles As New ListBox
    'Private pbPreview As New PictureBox
    'Private btnOK As New Button
    'Private btnCancel As New Button
    'Private SplitContainer1 As New SplitContainer

    ' Selected DXF file

    Private PreviewLines As New List(Of LineF)
    Private PreviewScale As Single
    Private PreviewOffsetX As Single
    Private PreviewOffsetY As Single


    Public Sub New()

        InitializeComponent()

        'Me.Text = "Select DXF File"
        'Me.Size = New Drawing.Size(800, 600)
        '

        ' Event handlers
        'AddHandler Me.Load, AddressOf DXFFileSelectionDialog_Load
        'AddHandler tvFolders.BeforeExpand, AddressOf tvFolders_BeforeExpand
        'AddHandler tvFolders.AfterSelect, AddressOf tvFolders_AfterSelect
        'AddHandler lstFiles.SelectedIndexChanged, AddressOf lstFiles_SelectedIndexChanged
        'AddHandler btnOK.Click, AddressOf btnOK_Click
        'AddHandler btnCancel.Click, AddressOf btnCancel_Click
    End Sub


    '-------------------------
    ' Load Root Drives
    '-------------------------
    Private Sub LoadRootNodes()

        tvFolders.Nodes.Clear()

        For Each drive As String In Directory.GetLogicalDrives()
            Dim rootNode As New TreeNode(drive) With {.Tag = drive}
            rootNode.Nodes.Add(New TreeNode("...")) ' dummy node for lazy expand
            tvFolders.Nodes.Add(rootNode)
        Next

        ' Optional Browse node for UNC/network paths
        Dim browseNode As New TreeNode("Browse...") With {.Tag = "BROWSE"}
        tvFolders.Nodes.Insert(0, browseNode)
    End Sub



    Private Sub LoadDirectories(node As TreeNode, path As String)
        Try
            node.Nodes.Clear()
            Dim dirs As String() = Directory.GetDirectories(path)
            For Each subDir In dirs
                Try
                    Dim childNode As New TreeNode(IO.Path.GetFileName(subDir)) With {.Tag = subDir}

                    ' Add dummy node if has subfolders
                    If Directory.GetDirectories(subDir).Length > 0 Then
                        childNode.Nodes.Add(New TreeNode("..."))
                    End If
                    node.Nodes.Add(childNode)
                Catch ex As UnauthorizedAccessException
                    ' Skip inaccessible folders
                Catch ex As Exception
                    ' Optional logging
                End Try
            Next
        Catch ex As UnauthorizedAccessException
            ' Skip inaccessible root
        Catch ex As Exception
            ' Optional logging
        End Try
    End Sub



    Private Function LoadDxfPreview(filePath As String) As Drawing.Image
        '' TODO: Implement actual DXF preview
        '' For now, return a blank bitmap with file name
        'Dim bmp As New Drawing.Bitmap(pbPreview.Width, pbPreview.Height)
        'Using g = Drawing.Graphics.FromImage(bmp)
        '    g.Clear(Drawing.Color.LightGray)
        '    g.DrawString(Path.GetFileName(filePath), New Drawing.Font("Arial", 12), Drawing.Brushes.Black, 10, 10)
        'End Using
        'Return bmp
    End Function




    Private Sub tvFolders_BeforeExpand(sender As Object, e As TreeViewCancelEventArgs) Handles tvFolders.BeforeExpand
        '-------------------------
        ' Lazy Load Folders
        '-------------------------

        Dim node = e.Node
        ' Skip Browse node
        If node.Tag IsNot Nothing AndAlso node.Tag.ToString() = "BROWSE" Then Return

        If node.Nodes.Count = 1 AndAlso node.Nodes(0).Text = "..." Then
            LoadDirectories(node, node.Tag.ToString())
        End If

    End Sub

    Private Sub btnCancel_Click(sender As Object, e As EventArgs) Handles btnCancel.Click

        Me.DialogResult = DialogResult.Cancel
        Me.Close()

    End Sub

    Private Sub btnOK_Click(sender As Object, e As EventArgs) Handles btnOK.Click

        '-------------------------
        ' OK / Cancel
        '-------------------------

        If lstFiles.SelectedItem Is Nothing OrElse tvFolders.SelectedNode Is Nothing Then
            MessageBox.Show("Please select a DXF file first.")
            Return
        End If

        Dim folderPath = tvFolders.SelectedNode.Tag.ToString()
        Dim fileName = lstFiles.SelectedItem.ToString()
        SetFilename(Path.Combine(folderPath, fileName))
        Me.DialogResult = DialogResult.OK
        Me.Close()

    End Sub

    Private Sub lstFiles_SelectedIndexChanged(sender As Object, e As EventArgs) Handles lstFiles.SelectedIndexChanged
        If lstFiles.SelectedItem Is Nothing Then Exit Sub
        Dim folderPath = tvFolders.SelectedNode.Tag.ToString()
        Dim fileName = lstFiles.SelectedItem.ToString()
        SetFilename(Path.Combine(folderPath, fileName))


        Dim filePath As String = GetFilename()
        frmMain.CurrentPreviewLines = ParseDXFToLines(filePath)

        DrawLinesOnPanel(pnlPreview, frmMain.CurrentPreviewLines)
        'pnlPreview.Invalidate()


    End Sub

    Private Sub tvFolders_AfterSelect(sender As Object, e As TreeViewEventArgs) Handles tvFolders.AfterSelect

        '-------------------------
        ' Folder Selected
        '-------------------------

        Dim node = e.Node
        If node.Tag Is Nothing Then Return

        ' Handle Browse node
        If node.Tag.ToString() = "BROWSE" Then
            Using fbd As New FolderBrowserDialog()
                fbd.Description = "Select root folder to browse DXF files"
                fbd.ShowNewFolderButton = False
                If fbd.ShowDialog() = DialogResult.OK Then
                    Dim browseRoot As New TreeNode(fbd.SelectedPath) With {.Tag = fbd.SelectedPath}
                    browseRoot.Nodes.Add(New TreeNode("..."))
                    tvFolders.Nodes.Remove(node)
                    tvFolders.Nodes.Insert(0, browseRoot)
                    tvFolders.SelectedNode = browseRoot
                End If
            End Using
            Return
        End If

        ' Load DXF files in selected folder
        lstFiles.Items.Clear()
        Try
            Dim files = Directory.GetFiles(node.Tag.ToString(), "*.dxf")
            For Each f In files
                lstFiles.Items.Add(Path.GetFileName(f))
            Next
        Catch ex As Exception
            ' Optional logging
        End Try

    End Sub

    Private Sub DXFFileSelectionDialog_Load(sender As Object, e As EventArgs) Handles Me.Load

        '-------------------------
        ' Form Load
        '-------------------------

        LoadRootNodes()

    End Sub

    Private Sub pbPreview_Paint(sender As Object, e As PaintEventArgs)
        Dim g = e.Graphics
        g.Clear(Color.White)

        If PreviewLines Is Nothing OrElse PreviewLines.Count = 0 Then Return

        Dim offsetX As Single, offsetY As Single, scaleYFlip As Single
        Dim scale As Single = DxfImport.ComputeScaleAndOffset(PreviewLines, pnlPreview.Width, pnlPreview.Height, 5, offsetX, offsetY, scaleYFlip)

        Using pen As New Pen(Color.Black, 1)
            For Each ln In PreviewLines
                Dim tln = DxfImport.TransformLine(ln, scale, offsetX, offsetY, scaleYFlip)
                g.DrawLine(pen, tln.X1, tln.Y1, tln.X2, tln.Y2)
            Next
        End Using
    End Sub

    Private Sub CenterAndScalePreview()
        If PreviewLines.Count = 0 Then Exit Sub

        Dim minX = PreviewLines.Min(Function(l) Math.Min(l.X1, l.X2))
        Dim maxX = PreviewLines.Max(Function(l) Math.Max(l.X1, l.X2))
        Dim minY = PreviewLines.Min(Function(l) Math.Min(l.Y1, l.Y2))
        Dim maxY = PreviewLines.Max(Function(l) Math.Max(l.Y1, l.Y2))

        Dim dxfWidth = maxX - minX
        Dim dxfHeight = maxY - minY

        Dim panelWidth = pnlPreview.Width
        Dim panelHeight = pnlPreview.Height

        PreviewScale = Math.Min(panelWidth / dxfWidth, panelHeight / dxfHeight) * 0.9F
        PreviewOffsetX = (panelWidth - dxfWidth * PreviewScale) / 2 - minX * PreviewScale
        PreviewOffsetY = (panelHeight - dxfHeight * PreviewScale) / 2 - minY * PreviewScale
    End Sub

    Private Sub pnlPreview_Paint(sender As Object, e As PaintEventArgs) Handles pnlPreview.Paint
        e.Graphics.Clear(Color.White)
        Using pen As New Pen(Color.Black, 1)
            For Each ln In frmMain.CurrentPreviewLines
                e.Graphics.DrawLine(pen, ln.X1, ln.Y1, ln.X2, ln.Y2)
            Next
        End Using
    End Sub

    Public ReadOnly Property SelectedFilePath As String
        Get
            If lstFiles.SelectedItem Is Nothing Then Return ""
            Return lstFiles.SelectedItem.ToString()
        End Get
    End Property
    ''' <summary>
    ''' Draws the given DXF lines on a panel, scaling and centering to fit the panel.
    ''' </summary>
    ''' <param name="panel">The panel to draw on (e.g., pnlPreview).</param>
    ''' <param name="lines">List of LineF to draw.</param>
    Public Sub DrawLinesOnPanel(panel As Panel, lines As List(Of LineF))
        If lines Is Nothing OrElse lines.Count = 0 Then
            panel.Invalidate()
            Return
        End If

        ' Compute bounding box
        Dim minX = lines.Min(Function(ln) Math.Min(ln.X1, ln.X2))
        Dim maxX = lines.Max(Function(ln) Math.Max(ln.X1, ln.X2))
        Dim minY = lines.Min(Function(ln) Math.Min(ln.Y1, ln.Y2))
        Dim maxY = lines.Max(Function(ln) Math.Max(ln.Y1, ln.Y2))

        Dim dx = maxX - minX
        Dim dy = maxY - minY
        If dx = 0 OrElse dy = 0 Then Return

        ' Panel size with optional padding
        Dim pad As Integer = 5
        Dim panelWidth = panel.Width - 2 * pad
        Dim panelHeight = panel.Height - 2 * pad

        ' Compute scale to fit
        Dim scaleX = panelWidth / dx
        Dim scaleY = panelHeight / dy
        Dim scale = Math.Min(scaleX, scaleY)

        ' Compute offset to center
        Dim offsetX = pad + (panelWidth - dx * scale) / 2 - minX * scale
        Dim offsetY = pad + (panelHeight - dy * scale) / 2 + maxY * scale
        Dim flipY As Single = -1

        ' Attach Paint event
        RemoveHandler panel.Paint, AddressOf pnlPreview_Paint ' remove previous if any
        AddHandler panel.Paint, Sub(sender As Object, e As PaintEventArgs)
                                    Dim g = e.Graphics
                                    g.Clear(Color.White)
                                    Using pen As New Pen(Color.Black, 1)
                                        For Each ln In lines
                                            Dim x1 = ln.X1 * scale + offsetX
                                            Dim y1 = ln.Y1 * scale * flipY + offsetY
                                            Dim x2 = ln.X2 * scale + offsetX
                                            Dim y2 = ln.Y2 * scale * flipY + offsetY
                                            g.DrawLine(pen, x1, y1, x2, y2)
                                        Next
                                    End Using
                                End Sub

        panel.Invalidate()
    End Sub

End Class
