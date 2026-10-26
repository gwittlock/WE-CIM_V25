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
    Private _suppressEvents As Boolean = False
    Private nMyMaterialID As Integer
    Private sMyMaterial As String

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

        Dim loadTimer As Stopwatch = Stopwatch.StartNew()

        InitializeFolderIcons()
        LoadRootFolders()

        Debug.WriteLine("After LoadRootFolders: " &
                loadTimer.ElapsedMilliseconds & " ms")

        nMyMaterialID = RegGetInt("Global", "MaterialID", 5)
        sMyMaterial = RegGetString("Global", "Material", "ALUM .250 X 96 X 48")

        txtFilename.AutoCompleteMode = AutoCompleteMode.SuggestAppend
        txtFilename.AutoCompleteSource = AutoCompleteSource.CustomSource

        Dim defPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)
        _initialFolder = RegGetString("Fabrication\FileOpen", "LastDXF", defPath)

        If Directory.Exists(_initialFolder) Then
            SelectFolderInTree(_initialFolder)
            Debug.WriteLine("After SelectFolderInTree: " &
                loadTimer.ElapsedMilliseconds & " ms")
        End If

        AddHandler pnlPreview.Paint, AddressOf pnlPreview_Paint

        loadTimer.Stop()
        Debug.WriteLine("DXF Dialog Load Time: " &
                        loadTimer.ElapsedMilliseconds & " ms")

    End Sub

    Public Sub LoadRootFolders()

        tvFolders.Nodes.Clear()

        For Each drive As DriveInfo In DriveInfo.GetDrives()

            Dim driveNode As New TreeNode() With {
            .Text = drive.Name & " (" & drive.DriveType.ToString() & ")",
            .Tag = drive.RootDirectory.FullName,
            .ImageIndex = 0,
            .SelectedImageIndex = 1
        }

            tvFolders.Nodes.Add(driveNode)

            ' Load folders only when the drive is expanded.
            driveNode.Nodes.Add("Loading...")

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

        Dim auto As New AutoCompleteStringCollection()

        For Each file As String In Directory.GetFiles(folderPath, "*.dxf")
            Dim name = Path.GetFileName(file)

            lstFiles.Items.Add(name)
            auto.Add(name)
        Next

        txtFilename.AutoCompleteCustomSource = auto

    End Sub


    '-------------------------------
    ' DXF selection changed
    '-------------------------------
    Private Sub lstFiles_SelectedIndexChanged(sender As Object, e As EventArgs) Handles lstFiles.SelectedIndexChanged

        If _suppressEvents Then Return

        If lstFiles.SelectedItem Is Nothing OrElse tvFolders.SelectedNode Is Nothing Then
            Return
        End If

        Try
            _suppressEvents = True

            txtFilename.Text = lstFiles.SelectedItem.ToString()

            _selectedDxfPath =
                Path.Combine(tvFolders.SelectedNode.Tag.ToString(),
                             lstFiles.SelectedItem.ToString())

            PreviewFile(_selectedDxfPath)

        Catch ex As Exception
            _suppressEvents = False
            MessageBox.Show("Failed to load DXF: " & _selectedDxfPath & ".  " & ex.Message, "DXF Preview", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
        _suppressEvents = False
    End Sub



    '-------------------------------
    ' Import entities for preview
    '-------------------------------
    Private Function ImportDxfForPreview(dxf As DxfFile, bg As System.Drawing.Color) As List(Of ImportedEntity)
        Dim results As New List(Of ImportedEntity)

        For Each ent As DxfEntity In dxf.Entities
            Dim displayColor As Color = Color.Yellow

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

                        Dim startAngle As Single = CSng(-ent.Arc.StartAngle)

                        Dim sweep As Double = ent.Arc.EndAngle - ent.Arc.StartAngle
                        If sweep < 0 Then sweep += 360

                        Dim sweepAngle As Single = CSng(-sweep)

                        e.Graphics.DrawArc(pen, CSng(cx - r), CSng(cy - r), CSng(r * 2), CSng(r * 2), CSng(startAngle), CSng(sweepAngle))
                End Select
            End Using
        Next
    End Sub

    Private Sub btnOK_Click(sender As Object, e As EventArgs) Handles btnOK.Click

        If String.IsNullOrWhiteSpace(txtFilename.Text) Then
            MessageBox.Show("Please select a DXF file.")
            Return
        End If

        Dim folder = tvFolders.SelectedNode?.Tag?.ToString()

        If folder Is Nothing Then Return

        Dim full = Path.Combine(folder, txtFilename.Text)

        If Not File.Exists(full) Then
            MessageBox.Show("File does not exist.")
            Return
        End If

        _selectedDxfPath = full

        ' Remember last folder
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

    Private Sub tvFolders_BeforeExpand(
    sender As Object,
    e As TreeViewCancelEventArgs
) Handles tvFolders.BeforeExpand

        Dim node As TreeNode = e.Node

        If node.Nodes.Count = 1 AndAlso
       node.Nodes(0).Text = "Loading..." Then

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

    Private Sub txtFileName_TextChanged(sender As Object, e As EventArgs) Handles txtFilename.TextChanged

        If _suppressEvents Then Return

        If tvFolders.SelectedNode Is Nothing Then Return

        Dim folder = tvFolders.SelectedNode.Tag.ToString()

        If Not Directory.Exists(folder) Then Return

        Dim pattern = "*" & txtFilename.Text & "*.dxf"

        Dim auto As New AutoCompleteStringCollection()

        Try
            _suppressEvents = True

            lstFiles.Items.Clear()

            For Each file As String In Directory.GetFiles(folder, pattern)
                Dim name = Path.GetFileName(file)

                lstFiles.Items.Add(name)
                auto.Add(name)
            Next

            txtFilename.AutoCompleteCustomSource = auto

        Finally
            _suppressEvents = False
        End Try

    End Sub


    Private Sub txtFileName_KeyDown(sender As Object, e As KeyEventArgs) Handles txtFilename.KeyDown

        If e.KeyCode = Keys.Enter Then

            If tvFolders.SelectedNode Is Nothing Then Return

            Dim full = Path.Combine(
                tvFolders.SelectedNode.Tag.ToString(),
                txtFilename.Text)

            If File.Exists(full) Then
                _selectedDxfPath = full
                PreviewFile(full)
            End If

            e.SuppressKeyPress = True

        End If

    End Sub

    Private Sub PreviewFile(path As String)

        Try
            _previewDxf = DxfFile.Load(path)

            _previewEntities =
            ImportDxfForPreview(_previewDxf, pnlPreview.BackColor)

            pnlPreview.Invalidate()

        Catch ex As Exception

            _previewDxf = Nothing
            _previewEntities = Nothing
            pnlPreview.Invalidate()

            MessageBox.Show("Failed to load DXF: " & ex.Message,
                        "DXF Preview",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Error)
        End Try

    End Sub

End Class
