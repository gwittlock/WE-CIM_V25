Imports System.Drawing
Imports System.Drawing.Drawing2D
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.IO
Imports System.Windows.Forms
Imports System.Linq
Imports FabV25_WIN8.ToolShapes
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models

Public Class DXFFileSelectionDialog
    Inherits Form

    Private _initialFolder As String
    Private _selectedDxfPath As String

    Private _selectedFilePath As String

    Public ReadOnly Property SelectedFilePath As String
        Get
            Return _selectedFilePath
        End Get
    End Property

    Public ReadOnly Property SelectedFileType As String
        Get
            Select Case cboFileTypes.SelectedIndex
                Case 0
                    Return "MM3"

                Case 1
                    Return "NST"

                Case 2
                    Return "DXF"

                Case 3
                    Return "CTG"

                Case Else
                    Return String.Empty
            End Select
        End Get
    End Property


    Private _previewGeoEntities As New List(Of GeoEntity)
    Private _previewJobSheet As JobSheet = Nothing
    Private _previewEnvironment As MM3ManufacturingEnvironment = Nothing
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

        cboFileTypes.Items.Clear()

        cboFileTypes.Items.Add("Model Files (*.mm3)")
        cboFileTypes.Items.Add("Nested Files (*.nst)")
        cboFileTypes.Items.Add("DXF Cad Files (*.dxf)")
        cboFileTypes.Items.Add("CTG Files (*.ctg)")

        txtFilename.AutoCompleteMode = AutoCompleteMode.SuggestAppend
        txtFilename.AutoCompleteSource = AutoCompleteSource.CustomSource


        'Restore the last successfully opened file type.
        Dim lastFileType As String =
    RegGetString("Fabrication\FileOpen", "LastEXT", "DXF")

        Select Case lastFileType.ToUpperInvariant()

            Case "MM3"
                cboFileTypes.SelectedIndex = 0

            Case "NST"
                cboFileTypes.SelectedIndex = 1

            Case "DXF"
                cboFileTypes.SelectedIndex = 2

            Case "CTG"
                cboFileTypes.SelectedIndex = 3

            Case Else
                cboFileTypes.SelectedIndex = 2

        End Select

        'Debug.WriteLine("After LoadRootFolders: " &
        '        loadTimer.ElapsedMilliseconds & " ms")

        nMyMaterialID = RegGetInt("Global", "MaterialID", 5)
        sMyMaterial = RegGetString("Global", "Material", "ALUM .250 X 96 X 48")

        loadTimer.Stop()
        'Debug.WriteLine("DXF Dialog Load Time: " &
        '                loadTimer.ElapsedMilliseconds & " ms")

    End Sub
    Private Sub cboFileTypes_SelectedIndexChanged(
    sender As Object,
    e As EventArgs
) Handles cboFileTypes.SelectedIndexChanged

        pnlDxfOptions.Visible = (SelectedFileType = "DXF")

        If _suppressEvents Then Return

        Dim registryKey As String = GetFolderRegistryKey()

        If String.IsNullOrWhiteSpace(registryKey) Then Return

        Dim defPath As String =
        Environment.GetFolderPath(
            Environment.SpecialFolder.MyDocuments)

        Dim folderPath As String =
        RegGetString("Fabrication\FileOpen", registryKey, defPath)

        If Not Directory.Exists(folderPath) Then
            folderPath = defPath
        End If

        'Clear the previous file selection and preview.
        txtFilename.Clear()
        txtAddress.Clear()

        _selectedDxfPath = String.Empty
        ClearPreview()

        'Navigate to the folder associated with this file type.
        Try

            _suppressEvents = True

            SelectFolderInTree(folderPath)

        Finally

            _suppressEvents = False

        End Try

        'Refresh even if the selected folder has not changed.
        If tvFolders.SelectedNode IsNot Nothing Then

            Dim selectedFolder As String =
            TryCast(tvFolders.SelectedNode.Tag, String)

            If Not String.IsNullOrWhiteSpace(selectedFolder) Then
                LoadFiles(selectedFolder)
            End If

        End If

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

            'Load folders only when the drive is expanded.
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

    Private Sub tvFolders_AfterSelect(
    sender As Object,
    e As TreeViewEventArgs
) Handles tvFolders.AfterSelect

        If _suppressEvents Then Return

        Dim folderPath As String =
        TryCast(e.Node.Tag, String)

        If String.IsNullOrWhiteSpace(folderPath) Then Return

        LoadFiles(folderPath)

    End Sub

    Private Sub LoadFiles(folderPath As String)

        'Clear the previous file selection.
        lvFiles.Items.Clear()

        txtFilename.Clear()
        txtAddress.Clear()

        _selectedDxfPath = String.Empty

        'Clear the previous graphical preview.
        ClearPreview()

        If Not Directory.Exists(folderPath) Then Return

        Dim auto As New AutoCompleteStringCollection()

        Dim filePattern As String

        Select Case cboFileTypes.SelectedIndex
            Case 0
                filePattern = "*.mm3"

            Case 1
                filePattern = "*.nst"

            Case 2
                filePattern = "*.dxf"

            Case 3
                filePattern = "*.ctg"

            Case Else
                Return
        End Select

        lvFiles.BeginUpdate()

        Try

            For Each file As String In Directory.GetFiles(folderPath, filePattern)

                Dim info As New FileInfo(file)

                Dim item As New ListViewItem(info.Name)

                'Store the complete file path.
                item.Tag = info.FullName

                'File size.
                Dim sizeKB As Long =
            CLng(Math.Ceiling(info.Length / 1024.0))

                item.SubItems.Add(sizeKB.ToString("N0") & " KB")

                'File type.
                item.SubItems.Add(
            info.Extension.TrimStart("."c).ToUpperInvariant() & " File")

                'Date modified.
                item.SubItems.Add(
            info.LastWriteTime.ToString("M/d/yyyy h:mm tt"))

                lvFiles.Items.Add(item)

                auto.Add(info.Name)

            Next

        Finally

            lvFiles.EndUpdate()

        End Try

        txtFilename.AutoCompleteCustomSource = auto

    End Sub

    '-------------------------------
    ' Import entities for preview
    '-------------------------------

    '-------------------------------
    ' Paint preview panel
    '-------------------------------
    Private Sub pnlPreview_Paint(
    sender As Object,
    e As PaintEventArgs) Handles pnlPreview.Paint

        e.Graphics.Clear(Color.Black)

        '---------------------------------------------
        ' Calculate nominal Part Geometry extents.
        '---------------------------------------------
        Dim partBounds As New PreviewBounds

        IncludePreviewPartBounds(partBounds)

        If partBounds.HasBounds Then

            Dim partRect As RectangleF =
        partBounds.GetBounds()

            lblPartExtents.Text =
        $"Model extents: X={partRect.Width:F4}, Y={partRect.Height:F4}"

        Else

            lblPartExtents.Text = "Model extents:"

        End If

        '---------------------------------------------
        ' Calculate complete preview drawing bounds.
        '---------------------------------------------
        Dim drawingBounds As New PreviewBounds

        If partBounds.HasBounds Then

            drawingBounds.Include(partBounds.GetBounds())

        End If

        IncludePreviewManufacturingBounds(drawingBounds)

        If Not drawingBounds.HasBounds Then Return

        Dim bounds As RectangleF =
    drawingBounds.GetBounds()

        Dim panelWidth As Integer = pnlPreview.ClientSize.Width
        Dim panelHeight As Integer = pnlPreview.ClientSize.Height

        If panelWidth <= 0 OrElse panelHeight <= 0 Then Return

        '---------------------------------------------
        ' Calculate preview scale
        '---------------------------------------------
        Dim scaleX As Double = Double.MaxValue
        Dim scaleY As Double = Double.MaxValue

        If bounds.Width > 0 Then
            scaleX = panelWidth / CDbl(bounds.Width)
        End If

        If bounds.Height > 0 Then
            scaleY = panelHeight / CDbl(bounds.Height)
        End If

        Dim scale As Double = Math.Min(scaleX, scaleY)

        If scale = Double.MaxValue Then scale = 1.0

        scale *= 0.9

        '---------------------------------------------
        ' Center geometry within preview
        '---------------------------------------------
        Dim offsetX As Double =
        (panelWidth - bounds.Width * scale) / 2.0 -
        bounds.X * scale

        Dim offsetY As Double =
        (panelHeight - bounds.Height * scale) / 2.0 -
        bounds.Y * scale

        '---------------------------------------------
        ' Draw native geometry
        '---------------------------------------------
        Using pen As New Pen(Color.Yellow, 1.0F)

            For Each entity As GeoEntity In _previewGeoEntities

                If entity Is Nothing Then Continue For

                If TypeOf entity Is GeoLine Then

                    Dim line = DirectCast(entity, GeoLine)

                    Dim x1 As Double =
                    line.StartPoint.X * scale + offsetX

                    Dim y1 As Double =
                    panelHeight -
                    (line.StartPoint.Y * scale + offsetY)

                    Dim x2 As Double =
                    line.EndPoint.X * scale + offsetX

                    Dim y2 As Double =
                    panelHeight -
                    (line.EndPoint.Y * scale + offsetY)

                    e.Graphics.DrawLine(
                    pen,
                    CSng(x1), CSng(y1),
                    CSng(x2), CSng(y2))

                ElseIf TypeOf entity Is GeoCircle Then

                    Dim circle = DirectCast(entity, GeoCircle)

                    Dim cx As Double =
                    circle.Center.X * scale + offsetX

                    Dim cy As Double =
                    panelHeight -
                    (circle.Center.Y * scale + offsetY)

                    Dim r As Double =
                    Math.Abs(circle.Radius) * scale

                    If r <= 0 Then Continue For

                    e.Graphics.DrawEllipse(
                    pen,
                    CSng(cx - r),
                    CSng(cy - r),
                    CSng(r * 2),
                    CSng(r * 2))

                ElseIf TypeOf entity Is GeoArc Then

                    Dim arc = DirectCast(entity, GeoArc)

                    Dim cx As Double =
                    arc.Center.X * scale + offsetX

                    Dim cy As Double =
                    panelHeight -
                    (arc.Center.Y * scale + offsetY)

                    Dim r As Double =
                    Math.Abs(arc.Radius) * scale

                    If r <= 0 Then Continue For

                    Dim sweep As Double

                    If arc.IsClockwise Then

                        sweep =
                        ((arc.StartAngle - arc.EndAngle) Mod
                         360.0 + 360.0) Mod 360.0

                    Else

                        sweep =
                        -(((arc.EndAngle - arc.StartAngle) Mod
                           360.0 + 360.0) Mod 360.0)

                    End If

                    e.Graphics.DrawArc(
                    pen,
                    CSng(cx - r),
                    CSng(cy - r),
                    CSng(r * 2),
                    CSng(r * 2),
                    CSng(-arc.StartAngle),
                    CSng(sweep))

                ElseIf TypeOf entity Is GeoPoint Then

                    Dim pt = DirectCast(entity, GeoPoint)

                    Dim x As Single =
                    CSng(pt.X * scale + offsetX)

                    Dim y As Single =
                    CSng(panelHeight -
                         (pt.Y * scale + offsetY))

                    e.Graphics.DrawLine(
                    pen, x - 2, y, x + 2, y)

                    e.Graphics.DrawLine(
                    pen, x, y - 2, x, y + 2)

                End If

            Next

        End Using

        '---------------------------------------------
        ' Draw native Manufacturing Geometry.
        '---------------------------------------------
        If _previewJobSheet IsNot Nothing AndAlso
           _previewJobSheet.ManufacturingModel IsNot Nothing Then

            Dim vt As New ViewTransform With {
                .Scale = scale,
                .OffsetX = offsetX,
                .OffsetY = offsetY
            }

            For Each feature As MachiningFeature In _previewJobSheet.ManufacturingModel.ManufacturingGeometry

                If TypeOf feature Is HoleFeature Then

                    DrawPreviewHoleFeature(DirectCast(feature, HoleFeature), e.Graphics, vt, panelHeight)

                ElseIf TypeOf feature Is LineOfHolesFeature Then

                    DrawPreviewLineOfHolesFeature(DirectCast(feature, LineOfHolesFeature), e.Graphics, vt, panelHeight)

                ElseIf TypeOf feature Is ContourFeature Then

                    DrawPreviewContourFeature(DirectCast(feature, ContourFeature), e.Graphics, vt, panelHeight)

                End If

            Next


            Dim mm As MM3 = _previewJobSheet.ManufacturingModel

            For Each instance As PatternInstance In mm.PatternInstances

                If instance Is Nothing Then Continue For

                Dim pattern As Pattern =
                    mm.Patterns.FirstOrDefault(
                        Function(p) p.ID = instance.PatternID)

                If pattern Is Nothing Then Continue For

                DrawPreviewPatternInstance(
                    pattern,
                    instance,
                    e.Graphics,
                    vt,
                    panelHeight)

            Next

        End If


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

        _selectedFilePath = full

        _selectedDxfPath = full


        'Remember the folder independently for each file type.
        Dim registryKey As String = GetFolderRegistryKey()

        If Not String.IsNullOrWhiteSpace(registryKey) AndAlso
            Directory.Exists(folder) Then

            RegPutString("Fabrication\FileOpen", registryKey, folder)

        End If

        'Remember the last selected file type.
        RegPutString("Fabrication\FileOpen", "LastEXT", SelectedFileType)

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
    Private Sub txtFileName_TextChanged(
    sender As Object,
    e As EventArgs
) Handles txtFilename.TextChanged

        If _suppressEvents Then Return

        'Clear the previous file preview.
        _selectedDxfPath = String.Empty
        txtAddress.Clear()

        ClearPreview()

    End Sub

    Private Sub txtFileName_KeyDown(
    sender As Object,
    e As KeyEventArgs
) Handles txtFilename.KeyDown

        If e.KeyCode <> Keys.Enter Then Return

        e.SuppressKeyPress = True

        If tvFolders.SelectedNode Is Nothing Then Return

        If String.IsNullOrWhiteSpace(txtFilename.Text) Then Return

        Dim folder As String =
        TryCast(tvFolders.SelectedNode.Tag, String)

        If String.IsNullOrWhiteSpace(folder) Then Return

        Dim full As String =
        Path.Combine(folder, txtFilename.Text.Trim())

        If Not File.Exists(full) Then Return

        _selectedDxfPath = full
        txtAddress.Text = full

        ClearPreview()

        Select Case Path.GetExtension(full).ToLowerInvariant()

            Case ".dxf"
                PreviewFile(full)

            Case ".mm3", ".nst"
                PreviewNativeFile(full)

            Case ".ctg"
                'CTG preview to be implemented.

        End Select

    End Sub
    Private Sub PreviewFile(path As String)

        Try

            Dim cadEntities As List(Of CadEntity) =
                DxfImport.ImportDxf(path)

            _previewGeoEntities =
                PartGeometryBuilder.CreatePreviewGeometry(cadEntities)

            pnlPreview.Invalidate()

        Catch ex As Exception

            ClearPreview()

            MessageBox.Show(
                "Failed to load DXF: " & ex.Message,
                "DXF Preview",
                MessageBoxButtons.OK,
                MessageBoxIcon.Error)

        End Try

    End Sub

    Private Sub PreviewNativeFile(filePath As String)

        ClearPreview()

        Try

            _previewJobSheet = MM3NativeFileService.LoadPreviewJobSheet(filePath, _previewEnvironment)

            For Each part As Part In _previewJobSheet.Parts

                _previewGeoEntities.AddRange(part.Geometry)

            Next

            pnlPreview.Invalidate()

        Catch ex As Exception

            ClearPreview()

            MessageBox.Show(
            "Failed to load native file preview: " & ex.Message,
            "Native File Preview",
            MessageBoxButtons.OK,
            MessageBoxIcon.Error)

        End Try

    End Sub

    Private Sub SplitContainer2_Panel1_Paint(sender As Object, e As PaintEventArgs) Handles SplitContainer2.Panel1.Paint

    End Sub

    Private Sub lvFiles_SelectedIndexChanged(sender As Object, e As EventArgs) Handles lvFiles.SelectedIndexChanged


        If _suppressEvents Then Return

        If lvFiles.SelectedItems.Count = 0 Then Return

        Dim item As ListViewItem = lvFiles.SelectedItems(0)

        Dim filePath As String = TryCast(item.Tag, String)

        If String.IsNullOrWhiteSpace(filePath) Then Return

        Try

            _suppressEvents = True

            txtFilename.Text = item.Text
            txtAddress.Text = filePath

            _selectedDxfPath = filePath

            'Clear the previous preview.
            ClearPreview()

            Select Case SelectedFileType

                Case "DXF"
                    PreviewFile(filePath)

                Case "MM3", "NST"
                    PreviewNativeFile(filePath)

            End Select

        Catch ex As Exception

            MessageBox.Show(
            "Failed to preview file: " & ex.Message,
            "File Preview",
            MessageBoxButtons.OK,
            MessageBoxIcon.Error)

        Finally

            _suppressEvents = False

        End Try


    End Sub

    Private Function GetFolderRegistryKey() As String

        Select Case SelectedFileType

            Case "MM3"
                Return "LastMM3"

            Case "NST"
                Return "LastNST"

            Case "DXF"
                Return "LastDXF"

            Case "CTG"
                Return "LastCTG"

            Case Else
                Return String.Empty

        End Select

    End Function
    Private Sub ClearPreview()

        _previewGeoEntities.Clear()
        _previewJobSheet = Nothing
        _previewEnvironment = Nothing

        lblPartExtents.Text = "Model extents:"

        pnlPreview.Invalidate()

    End Sub

    Private Sub DrawPreviewHoleFeature(
    hole As HoleFeature,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If hole Is Nothing OrElse
           _previewEnvironment Is Nothing Then Return

        Dim tool As Tool = Nothing
        Dim shape As ToolShape = Nothing

        If Not ToolShapeResolver.TryResolve(
            hole.ToolSetupMemberID,
            _previewEnvironment.ToolSetupMembers,
            _previewEnvironment.Tools,
            tool,
            shape) Then

            Return

        End If

        Dim drawColor As Color =
            GetVisibleColor(tool.Color, Color.Black)

        Using pen As New Pen(drawColor, 1.0F)

            pen.DashStyle = DashStyle.Dash

            ToolProfileRenderer.DrawToolCuttingProfile(
                g,
                shape,
                hole,
                vt,
                panelHeight,
                pen)

        End Using

    End Sub

    Private Sub DrawPreviewLineOfHolesFeature(
        row As LineOfHolesFeature,
        g As Graphics,
        vt As ViewTransform,
        panelHeight As Integer)

        If row Is Nothing OrElse
           _previewEnvironment Is Nothing OrElse
           _previewJobSheet Is Nothing Then Return

        Dim tool As Tool = Nothing
        Dim shape As ToolShape = Nothing

        If Not ToolShapeResolver.TryResolve(
            row.ToolSetupMemberID,
            _previewEnvironment.ToolSetupMembers,
            _previewEnvironment.Tools,
            tool,
            shape) Then Return

        Dim drawColor As Color =
            GetVisibleColor(tool.Color, Color.Black)

        Using pen As New Pen(drawColor, 1.0F)

            pen.DashStyle = DashStyle.Dash

            For Each holeCenter As PointF In
                HolePatternGeometry.GetLineOfHolesPoints(
                    row,
                    _previewJobSheet.Length,
                    _previewJobSheet.Width)

                ToolProfileRenderer.DrawToolCuttingProfile(
                    g,
                    shape,
                    holeCenter.X,
                    holeCenter.Y,
                    0.0,
                    vt,
                    panelHeight,
                    pen)

            Next

        End Using

    End Sub

    Private Sub DrawPreviewContourFeature(
    contour As ContourFeature,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If contour Is Nothing OrElse
           contour.Path Is Nothing OrElse
           _previewEnvironment Is Nothing Then Return

        Dim tool As Tool = Nothing
        Dim shape As ToolShape = Nothing

        If Not ToolShapeResolver.TryResolve(
            contour.ToolSetupMemberID,
            _previewEnvironment.ToolSetupMembers,
            _previewEnvironment.Tools,
            tool,
            shape) Then Return

        Dim toolMember = _previewEnvironment.ToolSetupMembers.
            FirstOrDefault(
                Function(m) m.ID = contour.ToolSetupMemberID)

        If toolMember Is Nothing Then Return

        Dim drawColor As Color =
            GetVisibleColor(tool.Color, Color.Black)

        Dim toolAngle As Double =
            If(contour.ToolIndexAngle.HasValue,
               contour.ToolIndexAngle.Value,
               toolMember.IndexAngle)

        Using pen As New Pen(drawColor, 1.0F),
              toolPen As New Pen(drawColor, 1.0F)

            pen.DashStyle = DashStyle.Dash
            toolPen.DashStyle = DashStyle.Solid

            For Each segment As IPathSegment In contour.Path.Segments

                If segment Is Nothing Then Continue For

                If segment.SegmentType <> PathSegmentType.Line Then
                    Continue For
                End If

                Dim p1 As New PointF(
                    CSng(segment.StartPoint.X * vt.Scale + vt.OffsetX),
                    CSng(panelHeight -
                         (segment.StartPoint.Y * vt.Scale + vt.OffsetY)))

                Dim p2 As New PointF(
                    CSng(segment.EndPoint.X * vt.Scale + vt.OffsetX),
                    CSng(panelHeight -
                         (segment.EndPoint.Y * vt.Scale + vt.OffsetY)))

                g.DrawLine(pen, p1, p2)

                ToolProfileRenderer.DrawToolCuttingProfile(
                    g,
                    shape,
                    CDbl(segment.StartPoint.X),
                    CDbl(segment.StartPoint.Y),
                    toolAngle,
                    vt,
                    panelHeight,
                    toolPen)

            Next

        End Using

    End Sub

    Private Sub DrawPreviewPatternInstance(
        pattern As Pattern,
        instance As PatternInstance,
        g As Graphics,
        vt As ViewTransform,
        panelHeight As Integer)

        If pattern Is Nothing OrElse
           instance Is Nothing Then Return

        For Each entity As GeoEntity In pattern.Geometry

            If entity Is Nothing Then Continue For

            Dim layer As Layer =
                TryCast(entity.Association, Layer)

            If Not TypeOf entity Is GeoPoint Then
                If layer IsNot Nothing AndAlso
                   Not layer.Visible Then Continue For
            End If

            Using pen As New Pen(Color.Yellow, 1.0F)

                If TypeOf entity Is GeoLine Then

                    Dim line As GeoLine =
                        DirectCast(entity, GeoLine)

                    If line.StartPoint Is Nothing OrElse
                       line.EndPoint Is Nothing Then Continue For

                    Dim worldStart As PointF =
                        PatternGeometry.TransformPatternPoint(
                            line.StartPoint.X,
                            line.StartPoint.Y,
                            instance)

                    Dim worldEnd As PointF =
                        PatternGeometry.TransformPatternPoint(
                            line.EndPoint.X,
                            line.EndPoint.Y,
                            instance)

                    Dim p1 As PointF =
                        PreviewWorldToScreen(
                            worldStart, vt, panelHeight)

                    Dim p2 As PointF =
                        PreviewWorldToScreen(
                            worldEnd, vt, panelHeight)

                    g.DrawLine(pen, p1, p2)

                ElseIf TypeOf entity Is GeoCircle Then

                    Dim c As GeoCircle =
                        DirectCast(entity, GeoCircle)

                    If c.Center Is Nothing Then Continue For

                    Dim worldCenter As PointF =
                        PatternGeometry.TransformPatternPoint(
                            c.Center.X,
                            c.Center.Y,
                            instance)

                    Dim center As PointF =
                        PreviewWorldToScreen(
                            worldCenter, vt, panelHeight)

                    Dim r As Single =
                        CSng(Math.Abs(c.Radius * vt.Scale))

                    If r <= 0 Then Continue For

                    g.DrawEllipse(
                        pen,
                        center.X - r,
                        center.Y - r,
                        r * 2.0F,
                        r * 2.0F)

                ElseIf TypeOf entity Is GeoArc Then

                    Dim a As GeoArc =
                        DirectCast(entity, GeoArc)

                    If a.Center Is Nothing Then Continue For

                    Dim worldCenter As PointF =
                        PatternGeometry.TransformPatternPoint(
                            a.Center.X,
                            a.Center.Y,
                            instance)

                    Dim center As PointF =
                        PreviewWorldToScreen(
                            worldCenter, vt, panelHeight)

                    Dim r As Single =
                        CSng(Math.Abs(a.Radius * vt.Scale))

                    If r <= 0 Then Continue For

                    Dim rect As New RectangleF(
                        center.X - r,
                        center.Y - r,
                        r * 2.0F,
                        r * 2.0F)

                    Dim sweep As Double

                    If a.IsClockwise Then
                        sweep = a.StartAngle - a.EndAngle
                    Else
                        sweep = a.EndAngle - a.StartAngle
                    End If

                    If sweep < 0 Then sweep += 360.0

                    Dim startAngle As Single =
                        CSng(-(a.StartAngle + instance.Angle))

                    Dim sweepAngle As Single =
                        CSng(If(a.IsClockwise, sweep, -sweep))

                    g.DrawArc(
                        pen,
                        rect,
                        startAngle,
                        sweepAngle)

                ElseIf TypeOf entity Is GeoPoint Then

                    Dim pt As GeoPoint =
                        DirectCast(entity, GeoPoint)

                    Dim worldPoint As PointF =
                        PatternGeometry.TransformPatternPoint(
                            pt.X,
                            pt.Y,
                            instance)

                    Dim screenPoint As PointF =
                        PreviewWorldToScreen(
                            worldPoint, vt, panelHeight)

                    g.DrawLine(
                        pen,
                        screenPoint.X - 2.0F,
                        screenPoint.Y,
                        screenPoint.X + 2.0F,
                        screenPoint.Y)

                    g.DrawLine(
                        pen,
                        screenPoint.X,
                        screenPoint.Y - 2.0F,
                        screenPoint.X,
                        screenPoint.Y + 2.0F)

                End If

            End Using

        Next

    End Sub

    Private Function PreviewWorldToScreen(
    worldPoint As PointF,
    vt As ViewTransform,
    panelHeight As Integer) As PointF

        Return New PointF(
            CSng(worldPoint.X * vt.Scale + vt.OffsetX),
            CSng(panelHeight -
                 (worldPoint.Y * vt.Scale + vt.OffsetY)))

    End Function

    Private Sub IncludePreviewManufacturingBounds(
        bounds As PreviewBounds)

        If _previewJobSheet Is Nothing OrElse
           _previewJobSheet.ManufacturingModel Is Nothing OrElse
           _previewEnvironment Is Nothing Then Return

        For Each feature As MachiningFeature In
            _previewJobSheet.ManufacturingModel.ManufacturingGeometry

            If feature Is Nothing Then Continue For

            If Not (TypeOf feature Is HoleFeature OrElse
                    TypeOf feature Is LineOfHolesFeature OrElse
                    TypeOf feature Is ContourFeature) Then Continue For

            Dim tool As Tool = Nothing
            Dim shape As ToolShape = Nothing

            If Not ToolShapeResolver.TryResolve(
                feature.ToolSetupMemberID,
                _previewEnvironment.ToolSetupMembers,
                _previewEnvironment.Tools,
                tool,
                shape) Then Continue For

            If TypeOf feature Is HoleFeature Then

                Dim hole As HoleFeature =
                    DirectCast(feature, HoleFeature)

                bounds.IncludeToolProfile(
                    shape,
                    hole.X,
                    hole.Y,
                    hole.Angle)

            ElseIf TypeOf feature Is LineOfHolesFeature Then

                Dim row As LineOfHolesFeature =
                    DirectCast(feature, LineOfHolesFeature)

                For Each center As PointF In
                    HolePatternGeometry.GetLineOfHolesPoints(
                        row,
                        _previewJobSheet.Length,
                        _previewJobSheet.Width)

                    bounds.IncludeToolProfile(
                        shape,
                        center.X,
                        center.Y,
                        0.0)

                Next

            ElseIf TypeOf feature Is ContourFeature Then

                Dim contour As ContourFeature =
                    DirectCast(feature, ContourFeature)

                If contour.Path Is Nothing Then Continue For

                Dim toolMember =
                    _previewEnvironment.ToolSetupMembers.
                    FirstOrDefault(
                        Function(m) m.ID = contour.ToolSetupMemberID)

                If toolMember Is Nothing Then Continue For

                Dim toolAngle As Double =
                    If(contour.ToolIndexAngle.HasValue,
                       contour.ToolIndexAngle.Value,
                       toolMember.IndexAngle)

                For Each segment As IPathSegment In
                    contour.Path.Segments

                    If segment Is Nothing Then Continue For

                    If segment.SegmentType <>
                       PathSegmentType.Line Then Continue For

                    bounds.Include(segment.StartPoint)
                    bounds.Include(segment.EndPoint)

                    bounds.IncludeToolProfile(
                        shape,
                        segment.StartPoint.X,
                        segment.StartPoint.Y,
                        toolAngle)

                Next

            End If

        Next

    End Sub

    Private Sub IncludePreviewPartBounds(
    bounds As PreviewBounds)

        ' Include independent nominal Part Geometry.
        For Each entity As GeoEntity In _previewGeoEntities

            bounds.IncludeGeometry(entity)

        Next

        If _previewJobSheet Is Nothing OrElse
           _previewJobSheet.ManufacturingModel Is Nothing Then Return

        Dim mm As MM3 = _previewJobSheet.ManufacturingModel

        ' Include nominal geometry of placed Pattern Instances.
        For Each instance As PatternInstance In mm.PatternInstances

            If instance Is Nothing Then Continue For

            Dim pattern As Pattern =
                mm.Patterns.FirstOrDefault(
                    Function(p) p.ID = instance.PatternID)

            If pattern Is Nothing Then Continue For

            For Each entity As GeoEntity In pattern.Geometry

                bounds.IncludeGeometry(entity, instance)

            Next

        Next

    End Sub






End Class
