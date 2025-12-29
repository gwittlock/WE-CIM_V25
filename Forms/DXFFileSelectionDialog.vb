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
    Private _selectedDxfPath As String
    Private _previewDxf As DxfFile = Nothing
    Private _previewEntities As List(Of ImportedEntity)

    ' Folder icons
    Private _folderImages As ImageList

    '-------------------------------
    ' Initialize folder icons
    '-------------------------------
    Private Sub InitializeFolderIcons()
        _folderImages = New ImageList()
        _folderImages.ColorDepth = ColorDepth.Depth32Bit
        _folderImages.ImageSize = New Size(16, 16)

        ' Placeholder icons
        _folderImages.Images.Add(SystemIcons.WinLogo.ToBitmap())
        _folderImages.Images.Add(SystemIcons.Application.ToBitmap())

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
    ' Load root folders
    '-------------------------------
    Public Sub LoadRootFolders()
        tvFolders.Nodes.Clear()

        For Each drive As DriveInfo In DriveInfo.GetDrives()
            Try
                If drive.IsReady Then
                    Dim driveNode As New TreeNode()
                    Dim nodeText As String = If(String.IsNullOrEmpty(drive.VolumeLabel),
                                                $"{drive.Name} ({drive.DriveType})",
                                                $"{drive.VolumeLabel} ({drive.Name.Substring(0, 1)})")
                    driveNode.Text = nodeText
                    driveNode.Tag = drive.RootDirectory.FullName
                    driveNode.ImageIndex = 0
                    driveNode.SelectedImageIndex = 1
                    tvFolders.Nodes.Add(driveNode)
                    PopulateChildFolders(driveNode)
                End If
            Catch ex As Exception
            End Try
        Next
    End Sub

    '-------------------------------
    ' Populate child folders
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
            Catch
            End Try
        Next
    End Sub

    Private Sub tvFolders_AfterSelect(sender As Object, e As TreeViewEventArgs) Handles tvFolders.AfterSelect
        LoadDxfFiles(e.Node.Tag.ToString())
    End Sub

    Private Sub LoadDxfFiles(folderPath As String)
        lstFiles.Items.Clear()
        _previewDxf = Nothing
        _previewEntities = Nothing
        pnlPreview.Invalidate()

        If Not Directory.Exists(folderPath) Then Return

        For Each file As String In Directory.GetFiles(folderPath, "*.dxf")
            lstFiles.Items.Add(Path.GetFileName(file))
        Next
    End Sub

    '-------------------------------
    ' DXF selection changed
    '-------------------------------
    Private Sub lstFiles_SelectedIndexChanged(sender As Object, e As EventArgs) Handles lstFiles.SelectedIndexChanged
        If lstFiles.SelectedItem Is Nothing OrElse tvFolders.SelectedNode Is Nothing Then Return

        _selectedDxfPath = Path.Combine(tvFolders.SelectedNode.Tag.ToString(), lstFiles.SelectedItem.ToString())

        Try
            _previewDxf = DxfFile.Load(_selectedDxfPath)

            ' Import entities for preview
            _previewEntities = ImportDxfForPreview(_previewDxf, pnlPreview.BackColor)

            pnlPreview.Invalidate()
        Catch ex As Exception
            _previewDxf = Nothing
            _previewEntities = Nothing
            pnlPreview.Invalidate()
            MessageBox.Show("Failed to load DXF: " & ex.Message, "DXF Preview", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    '-------------------------------
    ' Import entities for preview
    '-------------------------------
    Private Function ImportDxfForPreview(dxf As DxfFile, bg As System.Drawing.Color) As List(Of ImportedEntity)
        Dim results As New List(Of ImportedEntity)

        For Each ent As DxfEntity In dxf.Entities
            Dim displayColor As Color = Color.Black

            If ent.Color IsNot Nothing Then
                displayColor = DxfColorToDrawingColor(ent.Color, pnlPreview.BackColor)
            Else
                Dim layer As DxfLayer = dxf.Layers.FirstOrDefault(Function(l) l.Name = ent.Layer)
                If layer IsNot Nothing AndAlso layer.Color IsNot Nothing Then
                    displayColor = DxfColorToDrawingColor(layer.Color, pnlPreview.BackColor)
                End If
            End If

            Dim imported As New ImportedEntity()
            imported.DisplayColor = displayColor

            ' Correct classification: Arc before Circle
            If TypeOf ent Is DxfArc Then
                imported.GeometryType = ImportedEntity.GeometryTypeEnum.Arc
                imported.Arc = DirectCast(ent, DxfArc)

            ElseIf TypeOf ent Is DxfCircle Then
                If TypeOf ent Is DxfArc Then
                    imported.GeometryType = ImportedEntity.GeometryTypeEnum.Arc
                    imported.Arc = DirectCast(ent, DxfArc)
                ElseIf TypeOf ent Is DxfCircle Then
                    imported.GeometryType = ImportedEntity.GeometryTypeEnum.Circle
                    imported.Circle = DirectCast(ent, DxfCircle)
                End If

            ElseIf TypeOf ent Is DxfLine Then
                imported.GeometryType = ImportedEntity.GeometryTypeEnum.Line
                imported.Line = DirectCast(ent, DxfLine)

            Else
                imported.GeometryType = ImportedEntity.GeometryTypeEnum.Unknown
            End If

            results.Add(imported)
        Next

        Return results
    End Function

    '-------------------------------
    ' Paint preview panel
    '-------------------------------
    Private Sub pnlPreview_Paint(sender As Object, e As PaintEventArgs) Handles pnlPreview.Paint
        ' e.Graphics.Clear(Color.White)

        If _previewEntities Is Nothing OrElse _previewEntities.Count = 0 Then Return

        ' Calculate bounding box
        Dim minX = _previewEntities.Min(Function(ent) ent.MinX)
        Dim minY = _previewEntities.Min(Function(ent) ent.MinY)
        Dim maxX = _previewEntities.Max(Function(ent) ent.MaxX)
        Dim maxY = _previewEntities.Max(Function(ent) ent.MaxY)

        Dim panelWidth = pnlPreview.ClientSize.Width
        Dim panelHeight = pnlPreview.ClientSize.Height

        Dim scaleX = panelWidth / (maxX - minX)
        Dim scaleY = panelHeight / (maxY - minY)
        Dim scale = Math.Min(scaleX, scaleY) * 0.9 ' padding

        Dim offsetX = (panelWidth - (maxX - minX) * scale) / 2
        Dim offsetY = (panelHeight - (maxY - minY) * scale) / 2

        ' Draw each entity
        For Each ent As ImportedEntity In _previewEntities
            Using pen As New Pen(ent.DisplayColor, 1)
                Select Case ent.GeometryType
                    Case ImportedEntity.GeometryTypeEnum.Line
                        Dim x1 = (ent.Line.P1.X - minX) * scale + offsetX
                        Dim y1 = panelHeight - ((ent.Line.P1.Y - minY) * scale) - offsetY
                        Dim x2 = (ent.Line.P2.X - minX) * scale + offsetX
                        Dim y2 = panelHeight - ((ent.Line.P2.Y - minY) * scale) - offsetY
                        e.Graphics.DrawLine(pen, CSng(x1), CSng(y1), CSng(x2), CSng(y2))

                    Case ImportedEntity.GeometryTypeEnum.Circle
                        Dim cx = (ent.Circle.Center.X - minX) * scale + offsetX
                        Dim cy = panelHeight - ((ent.Circle.Center.Y - minY) * scale) - offsetY
                        Dim r = ent.Circle.Radius * scale
                        e.Graphics.DrawEllipse(pen, CSng(cx - r), CSng(cy - r), CSng(r * 2), CSng(r * 2))

                    Case ImportedEntity.GeometryTypeEnum.Arc
                        Dim cx = (ent.Arc.Center.X - minX) * scale + offsetX
                        Dim cy = panelHeight - ((ent.Arc.Center.Y - minY) * scale) - offsetY
                        Dim r = ent.Arc.Radius * scale

                        Dim startAngle = -ent.Arc.StartAngle
                        Dim sweepAngle = -(ent.Arc.EndAngle - ent.Arc.StartAngle)

                        e.Graphics.DrawArc(pen, CSng(cx - r), CSng(cy - r), CSng(r * 2), CSng(r * 2), CSng(startAngle), CSng(sweepAngle))
                End Select
            End Using
        Next
    End Sub

    Private Sub btnOK_Click(sender As Object, e As EventArgs) Handles btnOK.Click
        If String.IsNullOrEmpty(_selectedDxfPath) Then
            MessageBox.Show("Please select a DXF file.", "DXF Import", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        Dim folder = Path.GetDirectoryName(_selectedDxfPath)
        If Directory.Exists(folder) Then
            RegPutString("Fabrication\FileOpen", "LastDXF", folder)
        End If

        Me.DialogResult = DialogResult.OK
        Me.Close()
    End Sub

    Private Sub btnCancel_Click(sender As Object, e As EventArgs) Handles btnCancel.Click
        Me.DialogResult = DialogResult.Cancel
        Me.Close()
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

    Private Sub tvFolders_BeforeExpand(sender As Object, e As TreeViewCancelEventArgs) Handles tvFolders.BeforeExpand

        Dim node As TreeNode = e.Node

        ' Only populate if placeholder exists
        If node.Nodes.Count = 1 AndAlso node.Nodes(0).Text = "Loading..." Then
            node.Nodes.Clear()
            PopulateChildFolders(node)
        End If

    End Sub

    ' Helper to ensure entity color is visible against the panel background
    Private Function GetVisibleColor(entityColor As Color, bgColor As Color) As Color
        ' If the entity color is the same as the background, pick a contrasting color
        If entityColor.ToArgb() = bgColor.ToArgb() Then
            ' Simple brightness check
            Dim brightness As Double = (0.299 * bgColor.R + 0.587 * bgColor.G + 0.114 * bgColor.B) / 255
            Return If(brightness < 0.5, Color.White, Color.Black)
        End If
        Return entityColor
    End Function

    Private Function AcadColorIndexToColor(index As Integer) As Color
        ' Minimal AutoCAD color index to Color mapping (1-255)
        ' This can be expanded or replaced with a full mapping table
        Select Case index
            Case 1 : Return Color.Red
            Case 2 : Return Color.Yellow
            Case 3 : Return Color.Green
            Case 4 : Return Color.Cyan
            Case 5 : Return Color.Blue
            Case 6 : Return Color.Magenta
            Case 7 : Return Color.White
            Case Else : Return Color.Black
        End Select
    End Function



End Class
