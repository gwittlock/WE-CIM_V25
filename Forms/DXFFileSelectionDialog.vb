Imports System.Drawing
Imports System.Drawing.Drawing2D
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.IO
Imports System.Windows.Forms
Imports System.Linq

Public Class DXFFileSelectionDialog
    Inherits Form

    Private _initialFolder As String


    '-------------------------------
    ' Folder Icons
    '-------------------------------
    Private _folderImages As ImageList
    '-------------------------------
    ' Private Fields
    '-------------------------------
    Private _previewDxf As DxfFile = Nothing


    '-------------------------------
    ' Folder Icon Initialization
    Private Sub InitializeFolderIcons()
        _folderImages = New ImageList()
        _folderImages.ColorDepth = ColorDepth.Depth32Bit
        _folderImages.ImageSize = New Size(16, 16)


        ' Index 0 = closed folder, Index 1 = open folder
        _folderImages.Images.Add(SystemIcons.WinLogo.ToBitmap()) ' placeholder closed
        _folderImages.Images.Add(SystemIcons.Application.ToBitmap()) ' placeholder open


        tvFolders.ImageList = _folderImages
    End Sub


    Public ReadOnly Property SelectedDxfPath As String
        Get
            Return _selectedDxfPath
        End Get
    End Property

    Private Sub DXFFileSelectionDialog_Load(sender As Object, e As EventArgs) Handles Me.Load
        InitializeFolderIcons()
        LoadRootFolders()

        Dim defPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)
        _initialFolder = RegGetString("Fabrication\FileOpen", "LastDXF", defPath)

        If Directory.Exists(_initialFolder) Then
            SelectFolderInTree(_initialFolder)
        End If

        AddHandler pnlPreview.Paint, AddressOf pnlPreview_Paint
    End Sub


    '-------------------------------
    ' Load Root Folders (Lazy Loaded)
    '-------------------------------
    Public Sub LoadRootFolders()
        tvFolders.Nodes.Clear()

        For Each drive As DriveInfo In DriveInfo.GetDrives()
            Try
                If drive.IsReady Then
                    Dim driveNode As New TreeNode()

                    Dim nodeText As String
                    If String.IsNullOrEmpty(drive.VolumeLabel) Then
                        nodeText = $"{drive.Name} ({drive.DriveType})"
                    Else
                        nodeText = $"{drive.VolumeLabel} ({drive.Name.Substring(0, 1)})"
                    End If

                    driveNode.Text = nodeText
                    driveNode.Tag = drive.RootDirectory.FullName
                    driveNode.ImageIndex = 0
                    driveNode.SelectedImageIndex = 1

                    tvFolders.Nodes.Add(driveNode)
                    PopulateChildFolders(driveNode)
                End If
            Catch ex As Exception
                ' Optional: handle drive load error
            End Try
        Next
    End Sub

    '-------------------------------
    ' Populate Child Folders (Lazy Loaded, Safe)
    '-------------------------------
    Private Sub PopulateChildFolders(parentNode As TreeNode)
        Dim currentPath As String = TryCast(parentNode.Tag, String)
        If String.IsNullOrWhiteSpace(currentPath) Then Return

        Dim subDirs As IEnumerable(Of String)

        Try
            subDirs = Directory.EnumerateDirectories(currentPath)
        Catch ex As UnauthorizedAccessException
            parentNode.Nodes.Add("Access Denied")
            Return
        Catch ex As IOException
            Return
        End Try

        For Each dirPath As String In subDirs
            Dim childNode As New TreeNode(Path.GetFileName(dirPath)) With {
                .Tag = dirPath,
                .ImageIndex = 0,
                .SelectedImageIndex = 1
            }

            parentNode.Nodes.Add(childNode)

            Try
                Using enumerator = Directory.EnumerateDirectories(dirPath).GetEnumerator()
                    If enumerator.MoveNext() Then
                        childNode.Nodes.Add("Loading...")
                    End If
                End Using
            Catch ex As UnauthorizedAccessException
            Catch ex As IOException
            End Try
        Next
    End Sub

    Private Sub tvFolders_AfterSelect(sender As Object, e As TreeViewEventArgs) Handles tvFolders.AfterSelect
        LoadDxfFiles(e.Node.Tag.ToString())

    End Sub

    Private Sub LoadDxfFiles(folderPath As String)

        lstFiles.Items.Clear()
        _previewDxf = Nothing
        pnlPreview.Invalidate()

        If Not Directory.Exists(folderPath) Then Return

        For Each file As String In Directory.GetFiles(folderPath, "*.dxf")
            lstFiles.Items.Add(Path.GetFileName(file))
        Next
    End Sub


    Private Sub lstFiles_SelectedIndexChanged(sender As Object, e As EventArgs) Handles lstFiles.SelectedIndexChanged
        If lstFiles.SelectedItem Is Nothing OrElse tvFolders.SelectedNode Is Nothing Then Return

        _selectedDxfPath = Path.Combine(tvFolders.SelectedNode.Tag.ToString(), lstFiles.SelectedItem.ToString())

        Try
            _previewDxf = DxfFile.Load(_selectedDxfPath)
            pnlPreview.Invalidate()
        Catch ex As Exception
            _previewDxf = Nothing
            MessageBox.Show("Failed to load DXF: " & ex.Message, "DXF Preview", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub


    Private Sub btnOK_Click(sender As Object, e As EventArgs) Handles btnOK.Click
        If String.IsNullOrEmpty(_selectedDxfPath) Then
            MessageBox.Show("Please select a DXF file.", "DXF Import",
                        MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        Dim folder = Path.GetDirectoryName(_selectedDxfPath)
        If Directory.Exists(folder) Then
            RegPutString("Fabrication\FileOpen", "LastDXF", folder)
        End If

        Me.DialogResult = DialogResult.OK
        Me.Close()
    End Sub


    Private Sub pnlPreview_Paint(sender As Object, e As PaintEventArgs) Handles pnlPreview.Paint
        If _previewDxf Is Nothing Then
            e.Graphics.Clear(Color.White)
            Return
        End If


        DxfPreviewRenderer.DrawDxfPreview(_previewDxf, e.Graphics, pnlPreview.ClientRectangle)
    End Sub



    Private Sub DrawLine(p1 As DxfPoint, p2 As DxfPoint, g As Graphics, bounds As Rectangle, scale As Double, ox As Double, oy As Double)
        g.DrawLine(Pens.Black,
        CSng(p1.X * scale + ox), CSng(bounds.Height - (p1.Y * scale + oy)),
        CSng(p2.X * scale + ox), CSng(bounds.Height - (p2.Y * scale + oy)))
    End Sub


    Private Sub DrawCircle(c As DxfCircle, g As Graphics, bounds As Rectangle, scale As Double, ox As Double, oy As Double)
        Dim size = CSng(c.Radius * 2 * scale)
        Dim x = CSng((c.Center.X - c.Radius) * scale + ox)
        Dim y = CSng(bounds.Height - ((c.Center.Y + c.Radius) * scale + oy))
        g.DrawEllipse(Pens.Blue, x, y, size, size)
    End Sub
    Private Sub DrawArc(a As DxfArc, g As Graphics, bounds As Rectangle, scale As Double, ox As Double, oy As Double)
        Dim size = CSng(a.Radius * 2 * scale)
        Dim x = CSng((a.Center.X - a.Radius) * scale + ox)
        Dim y = CSng(bounds.Height - ((a.Center.Y + a.Radius) * scale + oy))

        ' DXF CCW -> GDI+ CW conversion, accounting for flipped Y-axis
        Dim startAngle As Single = CSng(-a.StartAngle)
        Dim sweepAngle As Single = CSng(-(a.EndAngle - a.StartAngle))

        g.DrawArc(Pens.Green, x, y, size, size, startAngle, sweepAngle)
    End Sub


    Private Sub DrawPolyline(pl As DxfPolyline, g As Graphics, bounds As Rectangle, scale As Double, ox As Double, oy As Double)
        If pl.Vertices.Count < 2 Then Return


        For i = 0 To pl.Vertices.Count - 2
            DrawLine(pl.Vertices(i).Location, pl.Vertices(i + 1).Location, g, bounds, scale, ox, oy)
        Next


        If pl.IsClosed Then
            DrawLine(pl.Vertices.Last().Location, pl.Vertices.First().Location, g, bounds, scale, ox, oy)
        End If
    End Sub

    Private Sub btnCancel_Click(sender As Object, e As EventArgs) Handles btnCancel.Click
        Me.DialogResult = DialogResult.Cancel
        Me.Close()
    End Sub

    Private Sub tvFolders_BeforeExpand(sender As Object, e As TreeViewCancelEventArgs) Handles tvFolders.BeforeExpand

        Dim node As TreeNode = e.Node

        If node.Nodes.Count = 1 AndAlso node.Nodes(0).Text = "Loading..." Then
            node.Nodes.Clear()
            PopulateChildFolders(node)
        End If
    End Sub

    Private Sub SelectFolderInTree(folderPath As String)
        Dim parts = folderPath.TrimEnd("\"c).Split("\"c)

        For Each rootNode As TreeNode In tvFolders.Nodes
            Dim rootPath = TryCast(rootNode.Tag, String)
            If rootPath Is Nothing Then Continue For

            If parts(0).Equals(rootPath.TrimEnd("\"c), StringComparison.OrdinalIgnoreCase) Then
                tvFolders.SelectedNode = rootNode
                ExpandToPath(rootNode, parts, 1)
                Exit For
            End If
        Next
    End Sub

    Private Sub ExpandToPath(parentNode As TreeNode, parts As String(), index As Integer)
        If index >= parts.Length Then Return

        parentNode.Expand()

        ' Force lazy-load if needed
        If parentNode.Nodes.Count = 1 AndAlso parentNode.Nodes(0).Text = "Loading..." Then
            parentNode.Nodes.Clear()
            PopulateChildFolders(parentNode)
        End If

        For Each child As TreeNode In parentNode.Nodes
            Dim childPath = TryCast(child.Tag, String)
            If childPath Is Nothing Then Continue For

            If parts(index).Equals(Path.GetFileName(childPath),
                                StringComparison.OrdinalIgnoreCase) Then
                tvFolders.SelectedNode = child
                ExpandToPath(child, parts, index + 1)
                Exit For
            End If
        Next
    End Sub




End Class
