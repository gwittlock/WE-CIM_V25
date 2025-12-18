Imports DevExpress.UserSkins
Imports DevExpress.Skins
Imports DevExpress.LookAndFeel
Imports DevExpress.Spreadsheet
Imports DevExpress.Utils
Imports DevExpress.Utils.Drawing
Imports DevExpress.XtraBars
Imports DevExpress.XtraBars.Helpers
Imports DevExpress.XtraBars.Ribbon
Imports DevExpress.XtraBars.Ribbon.ViewInfo
Imports DevExpress.XtraEditors.Controls
Imports DevExpress.XtraEditors.Drawing
Imports DevExpress.XtraSplashScreen
Imports Microsoft.Win32
Imports System.ComponentModel
Imports System.Data.OleDb
Imports System.Drawing.Imaging
Imports System.IO
Imports System.Numerics
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Helpers
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Services
Imports FabV25_WIN8.AppData
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports Newtonsoft.Json
Imports Formatting = Newtonsoft.Json.Formatting
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities


Public Class frmMain
    Inherits RibbonForm
    ' In frmMain or a global module
    Public CurrentDXFLines As New List(Of LineF)          ' Lines for main panel
    Public CurrentPreviewLines As New List(Of LineF)      ' Lines for file open preview

    Private _loadedDxf As DxfFile = Nothing

    Private _activeDxf As DxfFile
    Private _modelBounds As RectangleF
    Private _scale As Single = 1.0F
    Private _offset As PointF



    Public PicModelerScale As Single = 1.0F
    Public OffsetX As Single = 0
    Public OffsetY As Single = 0
    Private Declare Function LockWindowUpdate Lib "user32.dll" (ByVal hWndLock As IntPtr) As Boolean

    Public Sub New()
        Me.Location = Screen.AllScreens(UBound(Screen.AllScreens)).Bounds.Location
        'App.Title = "WE-CIM"
        Me.Text = "WE-CIM Fabrication Software"

        InitSkins()

        InitializeComponent()

        Me.InitSkinGallery()

        Me.splitContainerControl.SuspendLayout()
        Me.splitContainerControl.PanelVisibility = DevExpress.XtraEditors.SplitPanelVisibility.Panel2

        Me.splitContainerControl.ResumeLayout()

    End Sub
    Sub InitSkins()
        Try
            DevExpress.Skins.SkinManager.EnableFormSkins()
            DevExpress.UserSkins.BonusSkins.Register()

        Catch ex As Exception
            MessageBox.Show("InitSkins: " & ex.Message)
        End Try

    End Sub
    Private Sub InitSkinGallery()
        Dim indx As Integer = 0
        Try
            indx = 0
            SkinHelper.InitSkinGallery(rgbiSkins, True)
            AddHandler rgbiSkins.GalleryInitDropDownGallery, AddressOf rgbiSkins_GalleryInitDropDownGallery
            Dim i As Integer = 0
            For Each itemItem As GalleryItem In rgbiSkins.Gallery.Groups(0).Items
                If itemItem.Caption.Contains("DevExpress") Then
                    Dim item As GalleryItem = itemItem
                    item.Caption = itemItem.Caption.Replace("DevExpress", "WE_CIM")
                    item.Hint = item.Caption
                    If (indx <= 1) Then

                        item.Image = New Bitmap(rgbSkins.Images(indx), New System.Drawing.Size(16, 16))
                        item.HoverImage = New Bitmap(rgbSkins.Images(indx), New System.Drawing.Size(48, 48))
                        indx = indx + 1
                    End If
                End If
            Next itemItem
        Catch ex As Exception
            MessageBox.Show("InitSkinGallery: " & ex.Message)
        End Try
    End Sub

    Private Sub rgbiSkins_GalleryInitDropDownGallery(ByVal sender As Object, ByVal e As DevExpress.XtraBars.Ribbon.InplaceGalleryEventArgs) Handles rgbiSkins.GalleryInitDropDownGallery

    End Sub


    Private Sub frmMain_Load(sender As Object, e As EventArgs) Handles Me.Load

        Try
            '-----------------------------
            ' 1. Load all core JSON data
            '-----------------------------
            'LoadJsonData()
            AppData.LoadAllJson()

            '-----------------------------
            ' 2. Optional: Validate data
            '-----------------------------
            ValidationService.ValidateAllData()


            '-----------------------------
            ' 3. AppState is now ready
            '    No central combo boxes here
            '-----------------------------


            statusDate.Caption = "Date: " & Now.ToString("MMMM dd, yyyy")
            statusTime.Caption = "Time: " & Now.ToString("hh:mm tt")

            Logging.Log("AppState loaded successfully.")

        Catch ex As Exception
            MessageBox.Show($"Error loading application data: {ex.Message}", "Startup Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Logging.Log($"Startup error: {ex.ToString()}")
        End Try

    End Sub

    Private Sub bbConfigMan_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbConfigMan.ItemClick

        ConfigurationManagerForm.Show(Me)
    End Sub

    Private Sub iOpen_ItemClick(sender As Object, e As ItemClickEventArgs) Handles iOpen.ItemClick
        lReturn = appMain.ExecuteFunction(102)
    End Sub

    Private Sub frmMain_Closing(sender As Object, e As CancelEventArgs) Handles Me.Closing

    End Sub

    Private Sub frmMain_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        Me.Dispose()

    End Sub

    Private Sub frmMain_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        End
    End Sub

    'Public Sub ProcessDXFAndDraw(filePath As String)
    '    ' Parse lines
    '    CurrentDXFLines = ParseDXFToLines(filePath)

    '    _loadedDxf = filePath


    '    ' Trigger redraw
    '    pnlPicmodeler.Invalidate()
    'End Sub

    ''' <summary>
    ''' Processes DXF lines, stores them, and draws them scaled and centered on the main panel.
    ''' </summary>
    ''' <param name="panel">The main drawing panel (e.g., pnlPicmodeler).</param>
    ''' <param name="lines">List of LineF from DXF parsing.</param>
    Public Sub DrawLinesOnMainPanel(panel As Panel, lines As List(Of LineF))
        ' Store lines for later
        CurrentDXFLines = lines

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
        Dim pad As Integer = 10
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
        RemoveHandler panel.Paint, AddressOf pnlPicmodeler_Paint ' remove previous if any
        AddHandler panel.Paint, Sub(sender As Object, e As PaintEventArgs)
                                    Dim g = e.Graphics
                                    g.Clear(Color.White)
                                    Using pen As New Pen(Color.Black, 1)
                                        For Each ln In CurrentDXFLines
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

    Private Class ComboBoxItem
        Public Property Text As String
        Public Property Value As Integer

        Public Sub New(t As String, v As Integer)
            Text = t
            Value = v
        End Sub

        Public Overrides Function ToString() As String
            Return Text
        End Function
    End Class

    Private Sub btmHeatTransfer_ItemClick(sender As Object, e As ItemClickEventArgs) Handles btmHeatTransfer.ItemClick
        Dim mdbFile = "C:\1wittlockeng\Customers\Greg King\BoundingBoxNest\CMDB-Global.mdb"
        Dim tableName = "Tool Crib"
        Dim outputFolder = "C:\_Gary\AI\FabV25Test\Data"
        'Dim attributeTypes As List(Of ToolAttributeType)
        Dim attributeTypes As List(Of ToolAttributeType) = LoadTableWrapped(Of ToolAttributeType)("ToolAttributeTypes.json")

        ConvertToolCribTableToJson(mdbFile, tableName, outputFolder, attributeTypes)
    End Sub
    Public Sub ConvertToolCribTableToJson(mdbFile As String, tableName As String, outputFolder As String, attributeTypes As List(Of ToolAttributeType))
        ' Build the connection string for Access MDB
        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={mdbFile};"
        Using conn As New OleDbConnection(connStr)
            conn.Open()

            ' Load the table into a DataTable
            Dim dt As New DataTable()
            Using cmd As New OleDbCommand($"SELECT * FROM [{tableName}]", conn)
                Using adapter As New OleDbDataAdapter(cmd)
                    adapter.Fill(dt)
                End Using
            End Using

            ' Prepare ToolCribItems
            Dim toolCribList As New List(Of ToolCribItem)()

            For Each row As DataRow In dt.Rows
                Dim attributes As New Dictionary(Of String, Object)()

                ' Populate attributes based on ToolAttributeType mapping
                For Each attrType In attributeTypes
                    Dim columnName = attrType.Description.Trim() ' Column name in MDB
                    Dim keyName = attrType.Field.Trim()          ' Dictionary key

                    If dt.Columns.Contains(columnName) Then
                        Dim value = row(columnName)
                        If value Is DBNull.Value Then value = Nothing
                        attributes(keyName) = value
                    Else
                        attributes(keyName) = Nothing
                    End If
                Next

                ' Create ToolCribItem
                Dim tool As New ToolCribItem With {
                    .ID = CInt(row("ID")),
                    .Description = row("Description").ToString(),
                    .TypeID = CInt(row("Type ID")),
                    .Attributes = attributes
                }

                toolCribList.Add(tool)
            Next

            ' Ensure output folder exists
            If Not Directory.Exists(outputFolder) Then Directory.CreateDirectory(outputFolder)

            ' Write JSON
            Dim savePath = Path.Combine(outputFolder, "ToolCrib.json")
            Dim json = JsonConvert.SerializeObject(toolCribList, Formatting.Indented)
            File.WriteAllText(savePath, json)
        End Using
    End Sub

    Public Sub LoadDXFToModeler(dxf As DxfFile)

        If dxf Is Nothing Then
            MessageBox.Show("No DXF data to load.", "DXF Import",
                        MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' ----------------------------------
        ' Store active DXF
        ' ----------------------------------
        _activeDxf = dxf

        ' ----------------------------------
        ' Compute model bounds (world extents)
        ' ----------------------------------
        _modelBounds = ComputeDxfBounds(dxf)

        ' ----------------------------------
        ' Reset view (zoom to fit)
        ' ----------------------------------
        ResetModelView()

        ' ----------------------------------
        ' Trigger repaint
        ' ----------------------------------
        pnlPicmodeler.Invalidate()
        'pnlPicmodelerPanel.Invalidate()

    End Sub



    Private Sub ResetModelView()

        If _modelBounds.IsEmpty Then Return

        Dim panelSize = pnlPicmodeler.Size   'pnlPicmodelerPanel.ClientSize
        If panelSize.Width = 0 OrElse panelSize.Height = 0 Then Return

        Dim scaleX = panelSize.Width / _modelBounds.Width
        Dim scaleY = panelSize.Height / _modelBounds.Height

        _scale = CSng(Math.Min(scaleX, scaleY) * 0.9F)

        _offset = New PointF(
            -_modelBounds.Left * _scale + (panelSize.Width - _modelBounds.Width * _scale) / 2,
            -_modelBounds.Top * _scale + (panelSize.Height - _modelBounds.Height * _scale) / 2
        )

    End Sub

    'Private Sub DrawDxf(g As Graphics, dxf As DxfFile)

    '    g.SmoothingMode = Drawing2D.SmoothingMode.AntiAlias
    '    g.TranslateTransform(_offset.X, _offset.Y)
    '    g.ScaleTransform(_scale, -_scale)
    '    g.TranslateTransform(0, -_modelBounds.Bottom)

    '    Using pen As New Pen(Color.White, 0)

    '        For Each ent In dxf.Entities

    '            Select Case True

    '                Case TypeOf ent Is DxfLine
    '                    Dim l = CType(ent, DxfLine)
    '                    g.DrawLine(pen,
    '                    CSng(l.P1.X), CSng(l.P1.Y),
    '                    CSng(l.P2.X), CSng(l.P2.Y))

    '                Case TypeOf ent Is DxfCircle
    '                    Dim c = CType(ent, DxfCircle)
    '                    g.DrawEllipse(pen,
    '                    CSng(c.Center.X - c.Radius),
    '                    CSng(c.Center.Y - c.Radius),
    '                    CSng(c.Radius * 2),
    '                    CSng(c.Radius * 2))

    '                Case TypeOf ent Is DxfArc
    '                    DrawArcCorrectly(
    '                        CType(ent, DxfArc),
    '                        g,
    '                        Bounds,
    '                        Scale,
    '                        OffsetX,
    '                        OffsetY)

    '            End Select
    '        Next
    '    End Using
    'End Sub

    Private Sub pnlPicModeler_Paint(sender As Object, e As PaintEventArgs) Handles pnlPicmodeler.Paint
        Dim g As Graphics = e.Graphics
        g.Clear(Color.White)

        If _activeDxf Is Nothing OrElse _activeDxf.Entities.Count = 0 Then Return

        ' ----------------------------
        ' 1. Compute bounding box
        ' ----------------------------
        Dim minX As Double = Double.MaxValue
        Dim minY As Double = Double.MaxValue
        Dim maxX As Double = Double.MinValue
        Dim maxY As Double = Double.MinValue

        For Each entity In _activeDxf.Entities
            If TypeOf entity Is DxfLine Then
                Dim line = CType(entity, DxfLine)
                minX = Math.Min(minX, Math.Min(line.P1.X, line.P2.X))
                minY = Math.Min(minY, Math.Min(line.P1.Y, line.P2.Y))
                maxX = Math.Max(maxX, Math.Max(line.P1.X, line.P2.X))
                maxY = Math.Max(maxY, Math.Max(line.P1.Y, line.P2.Y))
            ElseIf TypeOf entity Is DxfCircle Then
                Dim circle = CType(entity, DxfCircle)
                minX = Math.Min(minX, circle.Center.X - circle.Radius)
                minY = Math.Min(minY, circle.Center.Y - circle.Radius)
                maxX = Math.Max(maxX, circle.Center.X + circle.Radius)
                maxY = Math.Max(maxY, circle.Center.Y + circle.Radius)
            ElseIf TypeOf entity Is DxfArc Then
                Dim arc = CType(entity, DxfArc)
                ' approximate bounding box for arc
                minX = Math.Min(minX, arc.Center.X - arc.Radius)
                minY = Math.Min(minY, arc.Center.Y - arc.Radius)
                maxX = Math.Max(maxX, arc.Center.X + arc.Radius)
                maxY = Math.Max(maxY, arc.Center.Y + arc.Radius)
            ElseIf TypeOf entity Is DxfPolyline Then
                Dim pl = CType(entity, DxfPolyline)
                For Each v In pl.Vertices
                    minX = Math.Min(minX, v.Location.X)
                    minY = Math.Min(minY, v.Location.Y)
                    maxX = Math.Max(maxX, v.Location.X)
                    maxY = Math.Max(maxY, v.Location.Y)
                Next
            End If
        Next

        ' Avoid zero-size bounding box
        If minX = maxX Then maxX = minX + 1
        If minY = maxY Then maxY = minY + 1

        ' ----------------------------
        ' 2. Compute scale and offsets
        ' ----------------------------
        Dim panelRect As Rectangle = pnlPicmodeler.ClientRectangle
        Dim scaleX As Double = panelRect.Width / (maxX - minX)
        Dim scaleY As Double = panelRect.Height / (maxY - minY)
        Dim scale As Double = Math.Min(scaleX, scaleY) * 0.9 ' 10% margin

        ' Offsets to center the drawing
        Dim ox As Double = (panelRect.Width - (maxX - minX) * scale) / 2 - minX * scale
        Dim oy As Double = (panelRect.Height - (maxY - minY) * scale) / 2 - minY * scale

        ' ----------------------------
        ' 3. Draw entities
        ' ----------------------------
        For Each entity In _activeDxf.Entities
            If TypeOf entity Is DxfLine Then
                Dim line = CType(entity, DxfLine)
                DrawLine(line.P1, line.P2, g, panelRect, scale, ox, oy)
            ElseIf TypeOf entity Is DxfCircle Then
                Dim circle = CType(entity, DxfCircle)
                DrawCircle(circle, g, panelRect, scale, ox, oy)
            ElseIf TypeOf entity Is DxfArc Then
                Dim arc = CType(entity, DxfArc)
                DrawArc(arc, g, panelRect, scale, ox, oy)
            ElseIf TypeOf entity Is DxfPolyline Then
                Dim pl = CType(entity, DxfPolyline)
                DrawPolyline(pl, g, panelRect, scale, ox, oy)
            End If
        Next
    End Sub



    Private Sub DrawLine(p1 As DxfPoint, p2 As DxfPoint, g As Graphics, bounds As Rectangle, scale As Double, ox As Double, oy As Double)
        g.DrawLine(Pens.Black,
               CSng(p1.X * scale + ox), CSng(bounds.Height - (p1.Y * scale + oy)),
               CSng(p2.X * scale + ox), CSng(bounds.Height - (p2.Y * scale + oy)))
    End Sub


    ' Draw a circle
    Private Sub DrawCircle(c As DxfCircle, g As Graphics, bounds As Rectangle, scale As Double, ox As Double, oy As Double)
        Dim radiusScaled As Single = CSng(c.Radius * scale)
        Dim x As Single = CSng(c.Center.X * scale + ox - radiusScaled)
        Dim y As Single = CSng(bounds.Height - (c.Center.Y * scale + oy) - radiusScaled)
        Dim size As Single = radiusScaled * 2
        g.DrawEllipse(Pens.Blue, x, y, size, size)
    End Sub

    Private Sub DrawArc(a As DxfArc, g As Graphics, bounds As Rectangle, scale As Double, ox As Double, oy As Double)
        Dim radiusScaled As Single = CSng(a.Radius * scale)
        Dim x As Single = CSng(a.Center.X * scale + ox - radiusScaled)
        Dim y As Single = CSng(bounds.Height - (a.Center.Y * scale + oy) - radiusScaled)
        Dim size As Single = radiusScaled * 2

        ' DXF CCW angles -> GDI+ CW angles, Y-axis flipped
        Dim startAngle As Single = CSng(-a.StartAngle)
        Dim sweepAngle As Single = CSng(-(a.EndAngle - a.StartAngle))

        g.DrawArc(Pens.Green, x, y, size, size, startAngle, sweepAngle)
    End Sub


    ' Draw a polyline
    Private Sub DrawPolyline(pl As DxfPolyline, g As Graphics, bounds As Rectangle, scale As Double, ox As Double, oy As Double)
        If pl.Vertices.Count < 2 Then Return

        For i As Integer = 0 To pl.Vertices.Count - 2
            Dim p1 As DxfPoint = pl.Vertices(i).Location
            Dim p2 As DxfPoint = pl.Vertices(i + 1).Location
            DrawLine(p1, p2, g, bounds, scale, ox, oy)
        Next

        ' Close the polyline if needed
        If pl.IsClosed Then
            DrawLine(pl.Vertices.Last().Location, pl.Vertices.First().Location, g, bounds, scale, ox, oy)
        End If
    End Sub


End Class



