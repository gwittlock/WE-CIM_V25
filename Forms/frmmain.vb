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
Imports FabV25_WIN8.ImportedEntity
Imports Newtonsoft.Json
Imports Formatting = Newtonsoft.Json.Formatting
Imports System.Drawing
Imports System.Drawing.Drawing2D
Imports System.Windows.Forms
Imports System.Linq

' IxMilia DXF imports
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.Reflection

Public Class frmMain
    Inherits RibbonForm

    Private _minX As Double
    Private _minY As Double
    Private _offsetX As Double
    Private _offsetY As Double
    Private _scale As Single = 1.0F
    Private _ox As Double
    Private _oy As Double

    Private _isPanning As Boolean = False
    Private _panStart As Point
    Private _offsetStartX As Double
    Private _offsetStartY As Double


    Private Structure ViewState
        Public MinX As Double
        Public MinY As Double
        Public Scale As Single
        Public OffsetX As Double
        Public OffsetY As Double
    End Structure

    Private _viewHistory As New Stack(Of ViewState)

    Private _windowSelecting As Boolean = False
    Private _windowStart As Point
    Private _windowEnd As Point
    Private _currentTool As String = "None"

    ' Optional: zoom factor per wheel notch
    Private Const ZoomFactor As Single = 1.2F


    Private _isMeasuring As Boolean = False
    Private _measureStart As Point
    Private _measureEnd As Point

    Private _measureTooltip As String = ""


    Public CurrentDXFLines As New List(Of LineF)          ' Lines for main panel
    Public CurrentPreviewLines As New List(Of LineF)      ' Lines for file open preview

    Private _loadedDxf As DxfFile = Nothing

    'Private _activeDxf As DxfFile
    Private _importedEntities As List(Of ImportedEntity)

    ' Public _selectedLayerSetup As AppData.LayerSetup

    Public Property IsDefaultMapped As Boolean

    Const NORMAL_WIDTH As Single = 1.0F
    Const DEFAULT_WIDTH As Single = 2.5F


    Private _modelBounds As RectangleF

    Private _offset As PointF


    Private lastEntity As ImportedEntity = Nothing
    '------------------------------------------
    Private WithEvents entityTip As New ToolTip()
    Private lastHoverEntity As ImportedEntity = Nothing
    Private _hoverEntity As ImportedEntity = Nothing
    Private _selectedEntity As ImportedEntity = Nothing
    Private _modelerToolTip As New ToolTip() With {
    .ShowAlways = False,
    .InitialDelay = 400,
    .ReshowDelay = 100,
    .AutoPopDelay = 4000
}


    Private Const HitTolerance As Double = 5 ' pixels


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

            EnableDoubleBuffering(pnlPicmodeler)
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
        _importedEntities = FileOpen()

        If _importedEntities IsNot Nothing AndAlso _importedEntities.Count > 0 Then
            pnlPicmodeler.Invalidate()
        End If
        ViewFull()


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
        RemoveHandler panel.Paint, AddressOf pnlPicModeler_Paint ' remove previous if any
        AddHandler panel.Paint, Sub(sender As Object, e As PaintEventArgs)
                                    Dim g = e.Graphics
                                    ' g.Clear(Color.White)
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
    Public Sub LoadImportedEntities(entities As List(Of ImportedEntity))
        _importedEntities = entities
        Me.Invalidate() ' trigger repaint
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

    Private Sub pnlPicModeler_Paint(sender As Object, e As PaintEventArgs) Handles pnlPicmodeler.Paint

        If _importedEntities Is Nothing OrElse _importedEntities.Count = 0 Then Return

        Dim g = e.Graphics
        Dim panelHeight = pnlPicmodeler.ClientSize.Height

        ' Helper function: convert model coordinates to screen coordinates
        Dim ModelToScreen =
            Function(p As DxfPoint) As PointF
                Return New PointF(
                    CSng((p.X - _minX) * _scale + _offsetX),
                    CSng(panelHeight - ((p.Y - _minY) * _scale) - _offsetY)
                )
            End Function

        ' Draw all entities
        For Each ent In _importedEntities
            ' Hovered entity drawn in red
            Dim color As Color = If(Object.ReferenceEquals(ent, _hoverEntity), Color.Red, ent.DisplayColor)
            Using pen As New Pen(color, 1)
                Select Case ent.GeometryType
                    Case ImportedEntity.GeometryTypeEnum.Line
                        Dim p1 = ModelToScreen(ent.Line.P1)
                        Dim p2 = ModelToScreen(ent.Line.P2)
                        g.DrawLine(pen, p1, p2)

                    Case ImportedEntity.GeometryTypeEnum.Circle
                        Dim c = ModelToScreen(ent.Circle.Center)
                        Dim r As Single = CSng(ent.Circle.Radius * _scale)
                        g.DrawEllipse(pen, c.X - r, c.Y - r, r * 2, r * 2)

                    Case ImportedEntity.GeometryTypeEnum.Arc
                        Dim c = ModelToScreen(ent.Arc.Center)
                        Dim r As Single = CSng(ent.Arc.Radius * _scale)
                        Dim startAngle As Single = CSng(-ent.Arc.StartAngle)
                        Dim sweepAngle As Single = CSng(-(ent.Arc.EndAngle - ent.Arc.StartAngle))
                        g.DrawArc(pen, c.X - r, c.Y - r, r * 2, r * 2, startAngle, sweepAngle)
                End Select
            End Using
        Next

        ' Draw View Window rubber-band rectangle
        If _windowSelecting Then
            Dim rect As New Rectangle(
                Math.Min(_windowStart.X, _windowEnd.X),
                Math.Min(_windowStart.Y, _windowEnd.Y),
                Math.Abs(_windowEnd.X - _windowStart.X),
                Math.Abs(_windowEnd.Y - _windowStart.Y)
            )

            ' Cyan dashed border
            Using pen As New Pen(Color.Cyan) With {.DashStyle = Drawing2D.DashStyle.Dash}
                g.DrawRectangle(pen, rect)
            End Using

            ' Optional: semi-transparent fill
            Using brush As New SolidBrush(Color.FromArgb(50, Color.Cyan))
                g.FillRectangle(brush, rect)
            End Using
        End If
        If _isMeasuring Then
            Using pen As New Pen(Color.Lime, 1)
                pen.DashStyle = Drawing2D.DashStyle.Dash
                g.DrawLine(pen, _measureStart, _measureEnd)
            End Using

            If Not String.IsNullOrEmpty(_measureTooltip) Then
                Using brush As New SolidBrush(Color.Lime)
                    g.DrawString(_measureTooltip, Me.Font, brush, _measureEnd.X + 5, _measureEnd.Y + 5)
                End Using
            End If
        End If

    End Sub

    Private Function WorldToScreen(p As DxfPoint,
                               minX As Double,
                               minY As Double,
                               scale As Double,
                               offsetX As Double,
                               offsetY As Double,
                               panelHeight As Integer) As PointF

        Dim sx As Single = CSng((p.X - minX) * scale + offsetX)
        Dim sy As Single = CSng(panelHeight - ((p.Y - minY) * scale) - offsetY)

        Return New PointF(sx, sy)
    End Function


    Private Function GetDirectiveByID(id As Integer) As AppData.Directive
        Return _selectedLayerSetup.Layers.
        FirstOrDefault(Function(d) d.ID = id)
    End Function
    '    Private Sub DrawLine(
    '    l As DxfLine,
    '    g As Graphics,
    '    scale As Double,
    '    ox As Double,
    '    oy As Double,
    '    color As Color,
    '    isDefaultMapped As Boolean
    ')


    '        Dim p1 = ToScreen(l.P1, scale, ox, oy)
    '        Dim p2 = ToScreen(l.P2, scale, ox, oy)

    '        Dim w As Single = If(isDefaultMapped, 3.0F, 1.0F)

    '        Using pen As New Pen(color, w)
    '            g.DrawLine(pen, p1, p2)
    '        End Using
    '    End Sub


    Private Sub DrawCircle(
    c As DxfCircle,
    g As Graphics,
    scale As Double,
    ox As Double,
    oy As Double,
    worldMaxY As Double,
    color As Color,
    isDefaultMapped As Boolean)

        ' Convert DXF center to screen space
        Dim centerWorld As PointF = DxfToPointF(c.Center)
        Dim centerScreen As PointF = ToScreen(centerWorld, scale, ox, oy, worldMaxY)

        ' Scale radius
        Dim r As Single = CSng(c.Radius * scale)

        ' GDI+ draws circles from top-left of bounding box
        Dim rect As New RectangleF(
        centerScreen.X - r,
        centerScreen.Y - r,
        r * 2.0F,
        r * 2.0F
    )

        Using pen As New Pen(color, If(isDefaultMapped, 1.0F, 2.0F))
            g.DrawEllipse(pen, rect)
        End Using
    End Sub

    Private Sub DrawArc(
    a As DxfArc,
    g As Graphics,
    scale As Double,
    ox As Double,
    oy As Double,
    worldMaxY As Double,
    color As Color,
    isDefaultMapped As Boolean)

        ' --------------------------------
        ' Center in screen coordinates
        ' --------------------------------
        Dim centerWorld As PointF = DxfToPointF(a.Center)
        Dim centerScreen As PointF = ToScreen(centerWorld, scale, ox, oy, worldMaxY)

        ' --------------------------------
        ' Radius
        ' --------------------------------
        Dim r As Single = CSng(a.Radius * scale)

        ' --------------------------------
        ' Bounding rectangle
        ' --------------------------------
        Dim rect As New RectangleF(
        centerScreen.X - r,
        centerScreen.Y - r,
        r * 2.0F,
        r * 2.0F
    )

        ' --------------------------------
        ' DXF → GDI+ angle conversion
        '
        ' DXF:
        '   - Degrees
        '   - Counter-clockwise
        '   - 0° at +X axis
        '
        ' GDI+:
        '   - Degrees
        '   - Clockwise
        '   - 0° at +X axis
        ' --------------------------------
        Dim startAngle As Single = CSng(-a.StartAngle)
        Dim sweepAngle As Single = CSng(-(a.EndAngle - a.StartAngle))

        ' Normalize sweep
        If sweepAngle <= 0 Then
            sweepAngle += 360.0F
        End If

        Using pen As New Pen(color, If(isDefaultMapped, 1.0F, 2.0F))
            g.DrawArc(pen, rect, startAngle, sweepAngle)
        End Using
    End Sub


    'Private Sub DrawPolyline(points As List(Of DxfPoint), g As Graphics,
    '                     scale As Double, ox As Double, oy As Double,
    '                     color As Color, isDefaultMapped As Boolean)

    '    If points Is Nothing OrElse points.Count < 2 Then Return

    '    ' Choose pen width based on default mapping
    '    Dim width As Single = If(isDefaultMapped, DEFAULT_WIDTH, NORMAL_WIDTH)

    '    Using pen As New Pen(color, width)
    '        ' Draw each segment
    '        For i As Integer = 0 To points.Count - 2
    '            Dim p1 = ToScreen(points(i), scale, ox, oy)
    '            Dim p2 = ToScreen(points(i + 1), scale, ox, oy)
    '            g.DrawLine(pen, p1.X, p1.Y, p2.X, p2.Y)
    '        Next

    '        ' Close the polyline if needed
    '        If points.Count > 2 Then
    '            Dim pFirst = ToScreen(points(0), scale, ox, oy)
    '            Dim pLast = ToScreen(points(points.Count - 1), scale, ox, oy)
    '            g.DrawLine(pen, pLast.X, pLast.Y, pFirst.X, pFirst.Y)
    '        End If
    '    End Using

    '    ' Draw default marker at midpoint if flagged
    '    If isDefaultMapped Then
    '        Dim midIndex As Integer = points.Count \ 2
    '        Dim midPoint = ToScreen(points(midIndex), scale, ox, oy)
    '        DrawDefaultMarker(g, midPoint, scale)
    '    End If

    'End Sub

    Private Function NormalizeAngle(deg As Double) As Double
        deg = deg Mod 360
        If deg < 0 Then deg += 360
        Return deg
    End Function

    ' Helper to draw the default marker (e.g., a "!") at a point
    Private Sub DrawDefaultMarker(g As Graphics, pt As PointF, scale As Double)
        Const markerSize As Single = 8.0F
        Using brush As New SolidBrush(Color.Red)
            g.DrawString("!", New Font("Arial", markerSize), brush, pt.X - markerSize / 2, pt.Y - markerSize / 2)
        End Using
    End Sub

    ''' <summary>
    ''' Converts a point from DXF world coordinates to panel screen coordinates.
    ''' Handles scaling, offset, and flipping of the Y-axis for correct display.
    ''' </summary>
    ''' <param name="worldPoint">The DXF point in world coordinates.</param>
    ''' <param name="scale">Scale factor (screen pixels per DXF unit).</param>
    ''' <param name="ox">X-axis offset in pixels.</param>
    ''' <param name="oy">Y-axis offset in pixels.</param>
    ''' <returns>PointF representing screen coordinates.</returns>
    'Public Function ToScreen(worldPoint As DxfPoint, scale As Double, ox As Double, oy As Double) As PointF
    '    ' Scale the coordinates
    '    Dim x As Single = CSng(worldPoint.X * scale + ox)
    '    Dim y As Single = CSng(worldPoint.Y * scale + oy)

    '    ' Flip Y-axis to match GDI+ coordinate system
    '    y = pnlPicmodeler.ClientRectangle.Height - y

    '    Return New PointF(x, y)
    'End Function

    Private Function DxfToPointF(p As IxMilia.Dxf.DxfPoint) As PointF
        Return New PointF(CSng(p.X), CSng(p.Y))
    End Function

    '------------------------------------------
    ' Hit-testing function
    '------------------------------------------
    Private Function HitTestImported(mousePos As Point,
                                 pnl As Panel,
                                 hitTolerance As Single,
                                 ByRef hitDistance As Single) As ImportedEntity

        hitDistance = Single.MaxValue

        If _importedEntities Is Nothing OrElse _importedEntities.Count = 0 Then
            Return Nothing
        End If

        '-------------------------------------------------
        ' Recompute SAME view transform as Paint
        '-------------------------------------------------
        Dim minX = _importedEntities.Min(Function(ent) ent.MinX)
        Dim minY = _importedEntities.Min(Function(ent) ent.MinY)
        Dim maxX = _importedEntities.Max(Function(ent) ent.MaxX)
        Dim maxY = _importedEntities.Max(Function(ent) ent.MaxY)

        Dim panelWidth = pnl.ClientSize.Width
        Dim panelHeight = pnl.ClientSize.Height

        Dim scaleX = panelWidth / (maxX - minX)
        Dim scaleY = panelHeight / (maxY - minY)
        Dim scale = Math.Min(scaleX, scaleY) * 0.9F

        Dim offsetX = (panelWidth - (maxX - minX) * scale) / 2.0F
        Dim offsetY = (panelHeight - (maxY - minY) * scale) / 2.0F

        '-------------------------------------------------
        ' Helpers
        '-------------------------------------------------
        Dim mouseF As New PointF(mousePos.X, mousePos.Y)

        Dim worldToScreen =
        Function(p As DxfPoint) As PointF
            Return New PointF(
                CSng((p.X - minX) * scale + offsetX),
                CSng(panelHeight - ((p.Y - minY) * scale) - offsetY)
            )
        End Function

        Dim bestEntity As ImportedEntity = Nothing

        '-------------------------------------------------
        ' Hit test each entity
        '-------------------------------------------------
        For Each ent In _importedEntities

            Dim dist As Single = Single.MaxValue

            Select Case ent.GeometryType

                Case ImportedEntity.GeometryTypeEnum.Line
                    Dim p1 = worldToScreen(ent.Line.P1)
                    Dim p2 = worldToScreen(ent.Line.P2)

                    dist = DistancePointToLine(mouseF, p1, p2)

                Case ImportedEntity.GeometryTypeEnum.Circle
                    Dim c = worldToScreen(ent.Circle.Center)
                    Dim r = CSng(ent.Circle.Radius * scale)

                    dist = Math.Abs(Distance(mouseF, c) - r)

                Case ImportedEntity.GeometryTypeEnum.Arc
                    Dim c = worldToScreen(ent.Arc.Center)
                    Dim r = CSng(ent.Arc.Radius * scale)

                    dist = Math.Abs(Distance(mouseF, c) - r)

            End Select

            If dist < hitDistance Then
                hitDistance = dist
                bestEntity = ent
            End If

        Next

        If hitDistance <= hitTolerance Then
            Return bestEntity
        End If

        Return Nothing

    End Function


    '------------------------------------------
    ' MouseMove handler for hover tooltip
    '------------------------------------------

    Private Sub pnlPicModeler_MouseMove(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseMove

        If _importedEntities Is Nothing OrElse _importedEntities.Count = 0 Then
            statusLocation.Caption = "Location: N/A"
            _hoverEntity = Nothing
            entityTip.Hide(pnlPicmodeler)
            Return
        End If

        '--------------------------------------------
        ' Convert screen → model coordinates using form-level transform
        '--------------------------------------------
        Dim modelX As Double = ((e.X - _offsetX) / _scale) + _minX
        Dim modelY As Double = ((pnlPicmodeler.ClientSize.Height - e.Y - _offsetY) / _scale) + _minY
        statusLocation.Caption = $"Location: X = {modelX:0.###}, Y = {modelY:0.###}"

        '--------------------------------------------
        ' Hit-test in screen space
        '--------------------------------------------
        Dim hitDist As Single
        Dim hit As ImportedEntity = HitTestImported(e.Location, pnlPicmodeler, HitTolerance, hitDist)

        ' Update hover entity only if changed
        If Not Object.ReferenceEquals(hit, _hoverEntity) Then
            _hoverEntity = hit
            pnlPicmodeler.Invalidate() ' redraw for hover highlight
        End If

        '--------------------------------------------
        ' Update tooltip
        '--------------------------------------------
        If hit IsNot Nothing Then
            entityTip.SetToolTip(pnlPicmodeler, BuildEntityToolTip(hit))
        Else
            entityTip.Hide(pnlPicmodeler)
        End If

        ' Pan
        If _isPanning Then
            Dim dx = e.X - _panStart.X
            Dim dy = e.Y - _panStart.Y
            _offsetX = _offsetStartX + dx
            _offsetY = _offsetStartY + dy
            pnlPicmodeler.Invalidate()
            Return
        End If

        ' View Window rubber-band
        If _windowSelecting Then
            _windowEnd = e.Location
            pnlPicmodeler.Invalidate()
        End If
        If _isMeasuring Then
            _measureEnd = e.Location

            ' Convert to model coordinates
            Dim startModel As DxfPoint = ScreenToModel(_measureStart)
            Dim endModel As DxfPoint = ScreenToModel(_measureEnd)

            ' Compute differences
            Dim dx As Double = endModel.X - startModel.X
            Dim dy As Double = endModel.Y - startModel.Y
            Dim dist As Double = Math.Sqrt(dx * dx + dy * dy)

            ' Format tooltip text
            _measureTooltip = $"ΔX = {dx:F3}, ΔY = {dy:F3}, Δ = {dist:F3}"

            pnlPicmodeler.Invalidate()
        End If

    End Sub

    '------------------------------------------
    ' Generate tooltip text with color swatch
    '------------------------------------------
    Private Function GetImportedEntityInfo(entity As ImportedEntity) As String
        Dim sb As New System.Text.StringBuilder()
        sb.AppendLine($"CAM Layer: {entity.CAMLayer}")
        sb.AppendLine($"Directive ID: {entity.DirectiveID}")

        ' Show color as HTML-style or RGB
        sb.AppendLine($"Color: #{entity.DisplayColor.R:X2}{entity.DisplayColor.G:X2}{entity.DisplayColor.B:X2}")

        Select Case entity.GeometryType
            Case ImportedEntity.GeometryTypeEnum.Line
                If entity.Line IsNot Nothing Then
                    Dim startPt As PointF = ToPointFFromDxfPoint(entity.Line.P1)
                    Dim endPt As PointF = ToPointFFromDxfPoint(entity.Line.P2)
                    sb.AppendLine($"Line: Start({startPt.X:F2},{startPt.Y:F2}), End({endPt.X:F2},{endPt.Y:F2})")
                End If

            Case ImportedEntity.GeometryTypeEnum.Circle
                If entity.Circle IsNot Nothing Then
                    Dim centerPt As PointF = ToPointFFromDxfPoint(entity.Circle.Center)
                    sb.AppendLine($"Circle: Center({centerPt.X:F2},{centerPt.Y:F2}), Radius {entity.Circle.Radius:F2}")
                End If

            Case ImportedEntity.GeometryTypeEnum.Arc
                If entity.Arc IsNot Nothing Then
                    Dim centerPt As PointF = ToPointFFromDxfPoint(entity.Arc.Center)
                    sb.AppendLine($"Arc: Center({centerPt.X:F2},{centerPt.Y:F2}), Radius {entity.Arc.Radius:F2}")
                End If

            Case Else
                sb.AppendLine("Unknown Geometry")
        End Select

        Return sb.ToString()
    End Function


    '------------------------------------------
    ' Distance between two PointF
    '------------------------------------------
    Private Function Distance(p1 As PointF, p2 As PointF) As Single
        Dim dx = p1.X - p2.X
        Dim dy = p1.Y - p2.Y
        Return CSng(Math.Sqrt(dx * dx + dy * dy))
    End Function

    '------------------------------------------
    ' Distance from point to line segment

    Private Function DistancePointToLine(p As PointF, a As PointF, b As PointF) As Single
        Dim dx = b.X - a.X
        Dim dy = b.Y - a.Y

        If dx = 0 AndAlso dy = 0 Then
            Return Distance(p, a)
        End If

        Dim t = ((p.X - a.X) * dx + (p.Y - a.Y) * dy) / (dx * dx + dy * dy)
        t = Math.Max(0, Math.Min(1, t))

        Dim proj As New PointF(a.X + t * dx, a.Y + t * dy)
        Return Distance(p, proj)
    End Function

    '------------------------------------------
    ' Convert Double() array to PointF
    '------------------------------------------
    Private Function ToPointF(coords As Double()) As PointF
        If coords Is Nothing OrElse coords.Length < 2 Then Return PointF.Empty
        Return New PointF(CSng(coords(0)), CSng(coords(1)))
    End Function
    '------------------------------------------
    ' Convert DxfPoint to PointF
    '------------------------------------------
    Private Function ToPointFFromDxfPoint(p As DxfPoint) As PointF
        Return New PointF(CSng(p.X), CSng(p.Y))
    End Function


    ' Convert Double() array (e.g., Line P1/P2) to PointF
    Private Function ToPointFFromArray(coords As Double()) As PointF
        If coords Is Nothing OrElse coords.Length < 2 Then Return PointF.Empty
        Return New PointF(CSng(coords(0)), CSng(coords(1)))
    End Function

    Private Sub pnlPicmodeler_MouseClick(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseClick

        If e.Button <> MouseButtons.Left Then Return

        Dim hitDist As Single
        Dim hitEntity = HitTestImported(e.Location, pnlPicmodeler, 5.0F, hitDist)

        _selectedEntity = hitEntity

        '------------------------------------------
        ' Selection feedback
        '------------------------------------------
        pnlPicmodeler.Invalidate()

        If hitEntity Is Nothing Then
            ' Optional: clear property grid / status
            Return
        End If

        '------------------------------------------
        ' Selection action
        '------------------------------------------
        HandleEntitySelected(hitEntity)
    End Sub

    Private Function BuildEntityToolTip(ent As ImportedEntity) As String

        If ent Is Nothing Then Return String.Empty

        Select Case ent.GeometryType

            Case ImportedEntity.GeometryTypeEnum.Line
                Return "Line" & Environment.NewLine &
                   $"P1: X={ent.Line.P1.X:0.###}, Y={ent.Line.P1.Y:0.###}" & Environment.NewLine &
                   $"P2: X={ent.Line.P2.X:0.###}, Y={ent.Line.P2.Y:0.###}"

            Case ImportedEntity.GeometryTypeEnum.Circle
                Return "Circle" & Environment.NewLine &
                   $"Center: X={ent.Circle.Center.X:0.###}, Y={ent.Circle.Center.Y:0.###}" & Environment.NewLine &
                   $"Radius: {ent.Circle.Radius:0.###}"

            Case ImportedEntity.GeometryTypeEnum.Arc
                Return "Arc" & Environment.NewLine &
                   $"Center: X={ent.Arc.Center.X:0.###}, Y={ent.Arc.Center.Y:0.###}" & Environment.NewLine &
                   $"Radius: {ent.Arc.Radius:0.###}" & Environment.NewLine &
                   $"Start: {ent.Arc.StartAngle:0.###}°  End: {ent.Arc.EndAngle:0.###}°"

            Case Else
                Return ent.GeometryType.ToString()

        End Select

    End Function

    Private Sub HandleEntitySelected(ent As ImportedEntity)

        If ent Is Nothing Then Exit Sub

        ' Placeholder for selection behavior
        ' Examples (future):
        '   PropertyGrid.SelectedObject = ent
        '   Show context menu
        '   Update status bar
        '   Highlight entity

    End Sub

    Private Sub ViewFull()
        If _importedEntities Is Nothing OrElse _importedEntities.Count = 0 Then Return

        SaveCurrentView() ' save current view for ViewPrevious

        Dim minX = _importedEntities.Min(Function(ent) ent.MinX)
        Dim minY = _importedEntities.Min(Function(ent) ent.MinY)
        Dim maxX = _importedEntities.Max(Function(ent) ent.MaxX)
        Dim maxY = _importedEntities.Max(Function(ent) ent.MaxY)

        Dim panelWidth = pnlPicmodeler.ClientSize.Width
        Dim panelHeight = pnlPicmodeler.ClientSize.Height

        Dim scaleX = panelWidth / (maxX - minX)
        Dim scaleY = panelHeight / (maxY - minY)
        _scale = Math.Min(scaleX, scaleY) * 0.9F ' 10% padding

        _minX = minX
        _minY = minY
        _offsetX = (panelWidth - (maxX - minX) * _scale) / 2
        _offsetY = (panelHeight - (maxY - minY) * _scale) / 2

        pnlPicmodeler.Invalidate()
    End Sub

    Private Sub ViewPrevious()
        If _viewHistory.Count = 0 Then Return
        Dim prev = _viewHistory.Pop()
        _minX = prev.MinX
        _minY = prev.MinY
        _scale = prev.Scale
        _offsetX = prev.OffsetX
        _offsetY = prev.OffsetY
        pnlPicmodeler.Invalidate()
    End Sub

    Private Sub pnlPicmodeler_MouseDown(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseDown
        ' Pan (Ctrl+Shift)
        If e.Button = MouseButtons.Left AndAlso (Control.ModifierKeys And Keys.Control) = Keys.Control _
       AndAlso (Control.ModifierKeys And Keys.Shift) = Keys.Shift Then
            _isPanning = True
            _panStart = e.Location
            _offsetStartX = _offsetX
            _offsetStartY = _offsetY
            pnlPicmodeler.Cursor = Cursors.Hand
            Return
        End If

        ' View Window
        If e.Button = MouseButtons.Left Then
            _windowSelecting = True
            _windowStart = e.Location
            _windowEnd = e.Location
        End If

        If _currentTool = "Measure" AndAlso e.Button = MouseButtons.Left Then
            _isMeasuring = True
            _measureStart = e.Location
            _measureEnd = e.Location
            _measureTooltip = ""
        End If
    End Sub

    Private Sub pnlPicmodeler_MouseUp(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseUp
        Dim PopUpPoint As Point

        If e.Button = Windows.Forms.MouseButtons.Right Then
            PopUpPoint.X = System.Windows.Forms.Cursor.Position.X

            PopUpPoint.Y = System.Windows.Forms.Cursor.Position.Y

            'super incattation of right mose click for popups
            mnuPopUp3.ShowPopup(PopUpPoint)
        End If

        ' End pan
        If _isPanning Then
            _isPanning = False
            pnlPicmodeler.Cursor = Cursors.Default
            Return
        End If

        ' End View Window
        If _windowSelecting Then
            _windowSelecting = False
            pnlPicmodeler.Invalidate()
            Dim rect As New Rectangle(
                Math.Min(_windowStart.X, _windowEnd.X),
                Math.Min(_windowStart.Y, _windowEnd.Y),
                Math.Abs(_windowEnd.X - _windowStart.X),
                Math.Abs(_windowEnd.Y - _windowStart.Y)
            )
            If rect.Width > 5 AndAlso rect.Height > 5 Then
                ZoomToRectangle(rect)
            End If
        End If

        ' Finish measurement
        If _isMeasuring AndAlso e.Button = MouseButtons.Left Then
            _isMeasuring = False
            pnlPicmodeler.Invalidate()
            ' Optionally, keep the tooltip visible for a moment or show a message box
            MessageBox.Show(_measureTooltip, "Measurement")
            _measureTooltip = ""
        End If
    End Sub

    Private Sub ZoomToRectangle(rect As Rectangle)
        If _importedEntities Is Nothing OrElse _importedEntities.Count = 0 Then Return
        If rect.Width < 2 OrElse rect.Height < 2 Then Return

        SaveCurrentView()

        Dim panelWidth = pnlPicmodeler.ClientSize.Width
        Dim panelHeight = pnlPicmodeler.ClientSize.Height

        ' Convert screen coordinates to model coordinates
        Dim x1 As Double = (rect.Left - _offsetX) / _scale + _minX
        Dim x2 As Double = (rect.Right - _offsetX) / _scale + _minX
        Dim y1 As Double = _minY + (panelHeight - rect.Top - _offsetY) / _scale
        Dim y2 As Double = _minY + (panelHeight - rect.Bottom - _offsetY) / _scale

        Dim zoomMinX = Math.Min(x1, x2)
        Dim zoomMaxX = Math.Max(x1, x2)
        Dim zoomMinY = Math.Min(y1, y2)
        Dim zoomMaxY = Math.Max(y1, y2)

        Dim scaleX = panelWidth / (zoomMaxX - zoomMinX)
        Dim scaleY = panelHeight / (zoomMaxY - zoomMinY)
        _scale = Math.Min(scaleX, scaleY) * 0.9F

        _minX = zoomMinX
        _minY = zoomMinY
        _offsetX = (panelWidth - (zoomMaxX - zoomMinX) * _scale) / 2
        _offsetY = (panelHeight - (zoomMaxY - zoomMinY) * _scale) / 2

        pnlPicmodeler.Invalidate()
    End Sub

    Private Function ScreenToModel(pt As Point) As DxfPoint
        Dim safeScale As Single = If(_scale = 0, 1.0F, _scale)
        Dim modelX As Double = _minX + (pt.X - _offsetX) / safeScale
        Dim modelY As Double = _minY + (pnlPicmodeler.ClientSize.Height - pt.Y - _offsetY) / safeScale
        Return New DxfPoint(modelX, modelY, 0)
    End Function

    Private Sub ApplyViewWindow(pt1 As Point, pt2 As Point)
        If _importedEntities Is Nothing OrElse _importedEntities.Count = 0 Then Return

        ' Push current view — create a proper ViewState
        Dim vs As New ViewState With {
        .MinX = _minX,
        .MinY = _minY,
        .Scale = _scale,
        .OffsetX = _offsetX,
        .OffsetY = _offsetY
    }
        _viewHistory.Push(vs)

        ' Convert to model space
        Dim x1 = ((Math.Min(pt1.X, pt2.X) - _offsetX) / _scale) + _minX
        Dim y1 = ((pnlPicmodeler.ClientSize.Height - Math.Max(pt1.Y, pt2.Y) - _offsetY) / _scale) + _minY
        Dim x2 = ((Math.Max(pt1.X, pt2.X) - _offsetX) / _scale) + _minX
        Dim y2 = ((pnlPicmodeler.ClientSize.Height - Math.Min(pt1.Y, pt2.Y) - _offsetY) / _scale) + _minY

        Dim width = x2 - x1
        Dim height = y2 - y1

        If width <= 0 OrElse height <= 0 Then Return

        ' Compute new scale
        Dim scaleX = pnlPicmodeler.ClientSize.Width / width
        Dim scaleY = pnlPicmodeler.ClientSize.Height / height
        _scale = Math.Min(scaleX, scaleY) * 0.9F

        ' Update offsets
        _minX = x1
        _minY = y1
        _offsetX = (pnlPicmodeler.ClientSize.Width - width * _scale) / 2
        _offsetY = (pnlPicmodeler.ClientSize.Height - height * _scale) / 2

        pnlPicmodeler.Invalidate()
    End Sub

    Private Sub ViewRefresh()
        ' Simply invalidate the panel to trigger a repaint
        pnlPicmodeler.Invalidate()
    End Sub

    Private Sub tbViewFull_Click(sender As Object, e As EventArgs) Handles tbViewFull.Click
        ViewFull()

    End Sub

    Private Sub tbViewWindow_Click(sender As Object, e As EventArgs) Handles tbViewWindow.Click
        _currentTool = "ViewWindow"
    End Sub

    Private Sub bbiMeasure_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbiMeasure.ItemClick
        _currentTool = "Measure"
    End Sub

    Public Function AllowPan(Button As Integer, Shift As Integer) As Boolean


        AllowPan =
            (Button = MouseButtons.Left) And
            (My.Computer.Keyboard.CtrlKeyDown) And
            (My.Computer.Keyboard.ShiftKeyDown)

    End Function

    Private Function ModelToScreen(p As DxfPoint) As PointF
        Dim panelHeight = pnlPicmodeler.ClientSize.Height
        Return New PointF(
        CSng((p.X - _minX) * _scale + _offsetX),
        CSng(panelHeight - ((p.Y - _minY) * _scale) - _offsetY)
    )
    End Function


    Public Sub EnableDoubleBuffering(ByVal control As Control)
        Dim propertyInfo As PropertyInfo = GetType(Control).GetProperty("DoubleBuffered",
            BindingFlags.Instance Or BindingFlags.NonPublic)
        propertyInfo.SetValue(control, True, Nothing)
    End Sub

    Private Sub tbViewPrevious_Click(sender As Object, e As EventArgs) Handles tbViewPrevious.Click
        ViewPrevious()
    End Sub

    Private Sub SaveCurrentView()
        Dim vs As New ViewState With {
            .MinX = _minX,
            .MinY = _minY,
            .Scale = _scale,
            .OffsetX = _offsetX,
            .OffsetY = _offsetY
        }
        _viewHistory.Push(vs)
    End Sub

    Private Sub tbViewRefresh_Click(sender As Object, e As EventArgs) Handles tbViewRefresh.Click
        ViewRefresh()
    End Sub

    Private Sub pnlPicmodeler_MouseWheel(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseWheel
        ' Only zoom if Ctrl+Shift are held
        If (Control.ModifierKeys And Keys.Control) = Keys.Control AndAlso
           (Control.ModifierKeys And Keys.Shift) = Keys.Shift Then

            ' Determine zoom direction
            Dim zoomIn As Boolean = e.Delta > 0
            Dim zoomAmount As Single = If(zoomIn, ZoomFactor, 1 / ZoomFactor)

            ' Convert mouse location to model coordinates
            Dim mouseModel As DxfPoint = ScreenToModel(e.Location)

            ' Apply zoom while keeping mouse position fixed
            _scale *= zoomAmount

            ' Adjust offsets so mouse remains at the same model point
            _offsetX = e.X - (mouseModel.X - _minX) * _scale
            _offsetY = e.Y - (pnlPicmodeler.ClientSize.Height - mouseModel.Y - _minY) * _scale

            pnlPicmodeler.Invalidate()
        End If
    End Sub

    Private Sub ZoomIn(center As Point)
        ZoomAtPoint(center, ZoomFactor)
    End Sub

    Private Sub ZoomOut(center As Point)
        ZoomAtPoint(center, 1 / ZoomFactor)
    End Sub

    Private Sub ZoomAtPoint(screenCenter As Point, zoomAmount As Single)
        ' Convert screen point to model coordinates
        Dim modelCenter As DxfPoint = ScreenToModel(screenCenter)

        ' Apply zoom
        _scale *= zoomAmount

        ' Optional: clamp scale to avoid extreme zoom
        _scale = Math.Max(0.01F, Math.Min(1000.0F, _scale))

        ' Recalculate offsets to keep the zoom centered on the chosen point
        _offsetX = screenCenter.X - (modelCenter.X - _minX) * _scale
        _offsetY = screenCenter.Y - (pnlPicmodeler.ClientSize.Height - modelCenter.Y - _minY) * _scale

        pnlPicmodeler.Invalidate()
    End Sub

    Private Sub tbViewZoomIn_Click(sender As Object, e As EventArgs) Handles tbViewZoomIn.Click
        Dim center As New Point(pnlPicmodeler.ClientSize.Width \ 2, pnlPicmodeler.ClientSize.Height \ 2)
        ZoomIn(center)
    End Sub

    Private Sub tbViewZoomOut_Click(sender As Object, e As EventArgs) Handles tbViewZoomOut.Click
        Dim center As New Point(pnlPicmodeler.ClientSize.Width \ 2, pnlPicmodeler.ClientSize.Height \ 2)
        ZoomOut(center)

    End Sub

    Private Sub pnlPicmodeler_DoubleClick(sender As Object, e As EventArgs) Handles pnlPicmodeler.DoubleClick

        ZoomIn(MousePosition)

    End Sub
End Class



