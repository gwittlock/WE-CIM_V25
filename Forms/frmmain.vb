
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
Imports FabV25_WIN8.CadEntity

Imports FabV25_WIN8.MateriaWrapper
Imports Newtonsoft.Json
Imports Formatting = Newtonsoft.Json.Formatting
Imports System.Drawing
Imports System.Drawing.Drawing2D
Imports System.Windows.Forms
Imports System.Linq
Imports System.Math
' IxMilia DXF imports
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.Reflection
Imports System.Text
Imports FabV25_WIN8.MachineEnvelope
Imports FabV25_WIN8.MaterialWrapper

Public Class frmMain
    Inherits RibbonForm


    Dim currentScale As Double
    Dim minScale As Double
    Dim maxScale As Double
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
    Private _gapPairs As List(Of Tuple(Of DxfPoint, DxfPoint))

    ' Each profile is a list of connected entities
    Private _profiles As New List(Of List(Of ImportedEntity))

    Private _importedProfiles As List(Of CamProfile)

    Private _inquireMode As Boolean = False
    Private _dxfImporter As IDxfImporter = New DxfImporter_IxMilia()


    Public _SelectedMaterialID As Integer



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
    Private Const ZoomFactor As Single = 1.5F


    Private _isMeasuring As Boolean = False
    Private _measureStart As Point
    Private _measureEnd As Point

    Private _measureTooltip As String = ""

    ' Replace old LineF lists
    Public CurrentDXFEntities As New List(Of CadEntity)         ' Entities for main panel
    Public CurrentPreviewEntities As New List(Of CadEntity)     ' Entities for file open preview


    Private _loadedDxf As DxfFile = Nothing

    'Private _activeDxf As DxfFile


    ' Public _selectedLayerSetup As AppData.LayerSetup

    Public Property IsDefaultMapped As Boolean

    Const NORMAL_WIDTH As Single = 1.0F
    Const DEFAULT_WIDTH As Single = 2.5F


    Private _modelBounds As RectangleF

    Private _offset As PointF


    Private lastEntity As ImportedEntity = Nothing
    '------------------------------------------
    Private WithEvents entityTip As New ToolTip()
    Private lastHoverEntity As CadEntity = Nothing
    Private _hoverEntity As ImportedEntity = Nothing
    Private _selectedEntity As ImportedEntity = Nothing
    Private _modelerToolTip As New ToolTip() With {
    .ShowAlways = False,
    .InitialDelay = 400,
    .ReshowDelay = 100,
    .AutoPopDelay = 4000
}


    Private Const HitTolerance As Double = 0.05 ' pixels


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

            ' EnableDoubleBuffering(pnlPicmodeler)
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
            panelWidth = pnlPicmodeler.Width
            panelHeight = pnlPicmodeler.Height

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

        FileOpen()



        ' Open DXF and import as CamProfiles
        '_importedProfiles = FileOpen()  ' returns List(Of CamProfile)
        ' FileOpen()

        'If _importedProfiles IsNot Nothing AndAlso _importedProfiles.Count > 0 Then
        '    Flatten all CadEntities from profiles into _importedEntities for legacy code
        '    _importedEntities = New List(Of ImportedEntity)

        '    For Each profile In _importedProfiles
        '        For Each cad In profile.Entities
        '            _importedEntities.Add(ConvertCadEntityToImportedEntity(cad, _selectedLayerSetup))
        '        Next
        '    Next

        '' Refresh the drawing panel
        'pnlPicmodeler.Invalidate()
        ''End If

        '' Ensure view is updated
        'ViewFull()
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
    ''' Draws a list of CadEntity objects on a specified panel, scaling and centering them automatically.
    ''' </summary>
    ''' <param name="panel">The target panel to draw on.</param>
    ''' <param name="entities">List of CadEntity to render.</param>
    ''' <param name="penColor">Optional: color of the pen used for drawing. Default is black.</param>
    ''' <param name="padding">Optional: padding inside the panel. Default is 10.</param>
    Public Sub DrawEntitiesOnPanel(panel As Panel,
                               entities As List(Of CadEntity),
                               Optional penColor As Color = Nothing,
                               Optional padding As Integer = 10)

        If entities Is Nothing OrElse entities.Count = 0 Then
            panel.Invalidate()
            Return
        End If

        ' Store entities in a panel-specific variable if needed
        ' Example: use CurrentDXFEntities for main panel or CurrentPreviewEntities for preview
        ' For demonstration, we'll assume CurrentDXFEntities
        CurrentDXFEntities = entities

        ' Panel size accounting for padding
        Dim panelWidth = panel.Width - 2 * padding
        Dim panelHeight = panel.Height - 2 * padding

        ' Compute scale and offsets
        Dim offsetX As Double, offsetY As Double, scaleYFlip As Double
        Dim scale = DxfImport.ComputeScaleAndOffset(entities, panelWidth, panelHeight, padding, offsetX, offsetY, scaleYFlip)

        ' Use default black pen if none specified
        If penColor = Nothing Then penColor = Color.Black

        ' Attach Paint event
        RemoveHandler panel.Paint, AddressOf pnlPicModeler_Paint
        AddHandler panel.Paint, Sub(sender As Object, e As PaintEventArgs)
                                    Dim g = e.Graphics
                                    Using pen As New Pen(penColor, 1)
                                        For Each ent In entities
                                            ' Transform entity to panel coordinates
                                            Dim transformed = DxfImport.TransformEntityToPanel(ent, scale, offsetX, offsetY, scaleYFlip)
                                            Dim startPt = transformed.Item1
                                            Dim endPt = transformed.Item2

                                            ' Draw line
                                            g.DrawLine(pen, CSng(startPt.X), CSng(startPt.Y), CSng(endPt.X), CSng(endPt.Y))
                                        Next
                                    End Using
                                End Sub


        panel.Invalidate()
    End Sub

    ''' <summary>
    ''' Draw a list of CadEntities on the given panel, scaling and centering them.
    ''' </summary>
    Public Sub DrawCadEntitiesOnPanel(panel As Panel, entities As List(Of CadEntity))
        If entities Is Nothing OrElse entities.Count = 0 Then
            panel.Invalidate()
            Return
        End If

        ' Compute bounding box for all entities
        Dim minX As Double = entities.Min(Function(e) Math.Min(e.StartPt.X, e.EndPt.X))
        Dim maxX As Double = entities.Max(Function(e) Math.Max(e.StartPt.X, e.EndPt.X))
        Dim minY As Double = entities.Min(Function(e) Math.Min(e.StartPt.Y, e.EndPt.Y))
        Dim maxY As Double = entities.Max(Function(e) Math.Max(e.StartPt.Y, e.EndPt.Y))

        Dim dx = maxX - minX
        Dim dy = maxY - minY
        If dx = 0 OrElse dy = 0 Then Return

        ' Panel size with optional padding
        Dim pad As Integer = 10
        Dim panelWidth = panel.Width - 2 * pad
        Dim panelHeight = panel.Height - 2 * pad

        ' Compute scale to fit
        Dim scaleX As Double = panelWidth / dx
        Dim scaleY As Double = panelHeight / dy
        Dim scale As Double = Math.Min(scaleX, scaleY)

        ' Compute offsets to center
        Dim offsetX As Double = pad + (panelWidth - dx * scale) / 2 - minX * scale
        Dim offsetY As Double = pad + (panelHeight - dy * scale) / 2 + maxY * scale
        Dim flipY As Double = -1

        ' Attach Paint event
        ' RemoveHandler panel.Paint, AddressOf panel.Paint
        AddHandler panel.Paint, Sub(sender As Object, e As PaintEventArgs)
                                    Dim g = e.Graphics
                                    Using pen As New Pen(Color.Black, 1)
                                        For Each cad In entities
                                            ' Assign tuple to a single variable
                                            Dim endpoints = DxfImport.TransformEntityToPanel(cad, scale, offsetX, offsetY, flipY)
                                            Dim startPt = endpoints.Item1
                                            Dim endPt = endpoints.Item2

                                            g.DrawLine(pen, CSng(startPt.X), CSng(startPt.Y), CSng(endPt.X), CSng(endPt.Y))
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

    Private Sub pnlPicModeler_Paint(sender As Object, e As PaintEventArgs) _
    Handles pnlPicmodeler.Paint

        _pnlPicModelerHandle = pnlPicmodeler.Handle
        'Dim margin As Integer = 20
        '' Compute scale to fit material inside panel with a margin
        'Dim panelWidth = pnlPicmodeler.Width
        'Dim panelHeight = pnlPicmodeler.Height

        'Dim materialHeight = GetMaterialWidth(MaterialSheets, _SelectedMaterialID)
        'Dim materialWidth = GetMaterialLength(MaterialSheets, _SelectedMaterialID)


        'Dim scaleX As Double = (panelWidth - 2 * margin) / materialWidth
        'Dim scaleY As Double = (panelHeight - 2 * margin) / materialHeight
        'Dim scale As Double = Math.Min(scaleX, scaleY)

        '' Compute offsets to center the material in the panel
        'Dim offsetX As Double = (panelWidth - materialWidth * scale) / 2
        'Dim offsetY As Double = (panelHeight - materialHeight * scale) / 2



        'If _drawables Is Nothing OrElse _drawables.Count = 0 Then Exit Sub

        'e.Graphics.SmoothingMode = Drawing2D.SmoothingMode.AntiAlias
        'Dim padding As Integer = 20

        '' Compute scale & offset for panel

        'ComputePanelTransform(_cadEntities,
        '                  pnlPicmodeler.Width,
        '                  pnlPicmodeler.Height,
        '                  padding,
        '                  scale,
        '                  offsetX,
        '                  offsetY)

        ' Draw each entity
        'For Each d In _drawables
        '    Using pen As New Pen(d.Color, 1)
        '        Select Case d.DrawableType
        '            Case DxfDrawable.DrawableTypeEnum.Line
        '                Dim sp = TransformToPanel(d.StartPoint.X, d.StartPoint.Y, myTransform.Scale, myTransform.OffsetX, myTransform.OffsetY)
        '                Dim ep = TransformToPanel(d.EndPoint.X, d.EndPoint.Y, myTransform.Scale, myTransform.OffsetX, myTransform.OffsetY)
        '                e.Graphics.DrawLine(pen, sp, ep)

        '            Case DxfDrawable.DrawableTypeEnum.Circle
        '                Dim center = TransformToPanel(d.Center.X, d.Center.Y, myTransform.Scale, myTransform.OffsetX, myTransform.OffsetY)
        '                Dim r As Single = CSng(d.Radius * myTransform.Scale)
        '                e.Graphics.DrawEllipse(pen, center.X - r, center.Y - r, 2 * r, 2 * r)

        '            Case DxfDrawable.DrawableTypeEnum.Arc

        '                Debug.Assert(d.EndAngle <> d.StartAngle, "Arc has zero sweep")

        '                Dim center = TransformToPanel(d.Center.X, d.Center.Y, myTransform.Scale, myTransform.OffsetX, myTransform.OffsetY)
        '                Dim r As Single = CSng(d.Radius * myTransform.Scale)

        '                ' Start and end angles are in degrees
        '                Dim startDeg As Single = CSng(d.StartAngle)
        '                Dim endDeg As Single = CSng(d.EndAngle)

        '                ' Compute sweep
        '                Dim sweep As Single = endDeg - startDeg
        '                If sweep < 0 Then sweep += 360  ' ensure positive sweep

        '                ' DrawArc expects clockwise sweep, negate to match CAD convention
        '                e.Graphics.DrawArc(New Pen(d.Color, 1),
        '                center.X - r, center.Y - r, 2 * r, 2 * r,
        '                -startDeg, -sweep)

        '                Debug.WriteLine($"Arc: Center=({d.Center.X},{d.Center.Y}) R={r} Start={startDeg} End={endDeg} Sweep = {sweep}")



        '        End Select
        '    End Using
        'Next

    End Sub



    ' Transforms a DXF circle or arc radius to panel pixels
    Public Function TransformRadius(radius As Double, scale As Double) As Single
        Return CSng(radius * scale)
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

        ' Define bounding box points
        Dim p1 As New PointF(centerScreen.X - r, centerScreen.Y - r) ' bottom-left / top-left
        Dim p2 As New PointF(centerScreen.X + r, centerScreen.Y + r) ' top-right / bottom-right

        ' Create rectangle from points (normalizes width/height automatically)
        Dim rect As RectangleF = HelperFunctions.RectFromPoints(p1, p2)

        ' Draw the ellipse
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
        ' Radius scaled
        ' --------------------------------
        Dim r As Single = CSng(a.Radius * scale)

        ' --------------------------------
        ' Define bounding rectangle points
        ' --------------------------------
        Dim p1 As New PointF(centerScreen.X - r, centerScreen.Y - r) ' top-left
        Dim p2 As New PointF(centerScreen.X + r, centerScreen.Y + r) ' bottom-right

        ' --------------------------------
        ' Create rectangle using helper
        ' --------------------------------
        Dim rect As RectangleF = HelperFunctions.RectFromPoints(p1, p2)

        ' --------------------------------
        ' DXF → GDI+ angle conversion
        ' DXF: counter-clockwise, 0° at +X axis
        ' GDI+: clockwise, 0° at +X axis
        ' --------------------------------
        Dim startAngle As Single = CSng(-a.StartAngle)
        Dim sweepAngle As Single = CSng(-(a.EndAngle - a.StartAngle))

        ' Normalize sweep
        If sweepAngle <= 0 Then
            sweepAngle += 360.0F
        End If

        ' Draw the arc
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
        Dim worldPt = ScreenToWorld(e.Location)

        ' Dim hit = HitTestImportedEntity(_importedEntities, worldPt, HitTolerance)


        '' Update hover entity only if changed
        'If Not Object.ReferenceEquals(hit, _hoverEntity) Then
        '    _hoverEntity = hit
        '    pnlPicmodeler.Invalidate() ' redraw for hover highlight
        'End If

        ''--------------------------------------------
        '' Update tooltip
        ''--------------------------------------------
        ''--------------------------------------------
        '' Inquire mode hit-test + tooltip
        ''--------------------------------------------
        'If _inquireMode Then


        '    If Not Object.ReferenceEquals(hit, _hoverEntity) Then
        '        _hoverEntity = hit
        '        pnlPicmodeler.Invalidate()
        '    End If

        '    If hit IsNot Nothing Then
        '        BuildEntityToolTip(hit)
        '        pnlPicmodeler.Cursor = Cursors.Help
        '    Else
        '        entityTip.Hide(pnlPicmodeler)
        '    End If

        'Else
        '    ' Normal mode – ensure tooltip is off
        '    entityTip.Hide(pnlPicmodeler)
        '    _hoverEntity = Nothing
        'End If



        '' Pan
        'If _isPanning Then
        '    Dim dx = e.X - _panStart.X
        '    Dim dy = e.Y - _panStart.Y
        '    _offsetX = _offsetStartX + dx
        '    _offsetY = _offsetStartY + dy
        '    pnlPicmodeler.Invalidate()
        '    Return
        'End If

        '' View Window rubber-band
        'If _windowSelecting Then
        '    _windowEnd = e.Location
        '    pnlPicmodeler.Invalidate()
        'End If
        'If _isMeasuring Then
        '    _measureEnd = e.Location

        '    ' Convert to model coordinates
        '    Dim startModel As DxfPoint = ScreenToModel(_measureStart)
        '    Dim endModel As DxfPoint = ScreenToModel(_measureEnd)

        '    ' Compute differences
        '    Dim dx As Double = endModel.X - startModel.X
        '    Dim dy As Double = endModel.Y - startModel.Y
        '    Dim dist As Double = Math.Sqrt(dx * dx + dy * dy)

        '    ' Format tooltip text
        '    _measureTooltip = $"ΔX = {dx:F3}, ΔY = {dy:F3}, Δ = {dist:F3}"

        '    pnlPicmodeler.Invalidate()
        'End If

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

    'Private Function BuildEntityToolTip(ent As CadEntity) As String

    'If ent Is Nothing Then Return String.Empty

    'Select Case ent.GeometryType

    '    Case ImportedEntity.GeometryTypeEnum.Line
    '        Return "Line" & Environment.NewLine &
    '           $"P1: X={ent.Line.P1.X:0.###}, Y={ent.Line.P1.Y:0.###}" & Environment.NewLine &
    '           $"P2: X={ent.Line.P2.X:0.###}, Y={ent.Line.P2.Y:0.###}"

    '    Case ImportedEntity.GeometryTypeEnum.Circle
    '        Return "Circle" & Environment.NewLine &
    '           $"Center: X={ent.Circle.Center.X:0.###}, Y={ent.Circle.Center.Y:0.###}" & Environment.NewLine &
    '           $"Radius: {ent.Circle.Radius:0.###}"

    '    Case ImportedEntity.GeometryTypeEnum.Arc
    '        Return "Arc" & Environment.NewLine &
    '           $"Center: X={ent.Arc.Center.X:0.###}, Y={ent.Arc.Center.Y:0.###}" & Environment.NewLine &
    '           $"Radius: {ent.Arc.Radius:0.###}" & Environment.NewLine &
    '           $"Start: {ent.Arc.StartAngle:0.###}°  End: {ent.Arc.EndAngle:0.###}°"

    '    Case Else
    '        Return ent.GeometryType.ToString()

    'End Select

    '    Dim info = ent.GetInspectionInfo()

    '    Dim sb As New StringBuilder()
    '    For Each kvp In info
    '        sb.AppendLine($"{kvp.Key}: {kvp.Value}")
    '    Next

    '    BuildEntityToolTip = sb.ToString

    'End Function

    Private Sub BuildEntityToolTip(ent As ImportedEntity)

        ' Fence-post safety
        If ent Is Nothing Then
            entityTip.Hide(pnlPicmodeler)
            pnlPicmodeler.Cursor = Cursors.Default
            Return
        End If

        Dim sb As New System.Text.StringBuilder()

        '--------------------------------------------
        ' Header
        '--------------------------------------------
        sb.AppendLine($"Type: {ent.GeometryType}")
        sb.AppendLine($"Layer: {ent.CAMLayer}")

        If ent.DirectiveID >= 0 Then
            sb.AppendLine($"Directive ID: {ent.DirectiveID}")
        End If

        sb.AppendLine(New String("-"c, 28))

        '--------------------------------------------
        ' Geometry-specific data
        '--------------------------------------------
        Select Case ent.GeometryType

            Case ImportedEntity.GeometryTypeEnum.Line
                Dim ln = ent.Line
                If ln IsNot Nothing Then
                    Dim dx = ln.P2.X - ln.P1.X
                    Dim dy = ln.P2.Y - ln.P1.Y
                    Dim length = Math.Sqrt(dx * dx + dy * dy)
                    Dim angle = Math.Atan2(dy, dx) * 180.0 / Math.PI

                    sb.AppendLine($"Start: ({F(ln.P1.X)}, {F(ln.P1.Y)})")
                    sb.AppendLine($"End:   ({F(ln.P2.X)}, {F(ln.P2.Y)})")
                    sb.AppendLine($"Length: {F(length)}")
                    sb.AppendLine($"Angle:  {F(angle)}°")
                End If

            Case ImportedEntity.GeometryTypeEnum.Circle
                Dim c = ent.Circle
                If c IsNot Nothing Then
                    sb.AppendLine($"Center: ({F(c.Center.X)}, {F(c.Center.Y)})")
                    sb.AppendLine($"Radius: {F(c.Radius)}")
                End If

            Case ImportedEntity.GeometryTypeEnum.Arc
                Dim a = ent.Arc
                If a IsNot Nothing Then
                    sb.AppendLine($"Center: ({F(a.Center.X)}, {F(a.Center.Y)})")
                    sb.AppendLine($"Radius: {F(a.Radius)}")
                    sb.AppendLine($"Start Angle: {F(a.StartAngle)}°")
                    sb.AppendLine($"End Angle:   {F(a.EndAngle)}°")
                End If

            Case Else
                sb.AppendLine("Geometry: Unknown")

        End Select

        '--------------------------------------------
        ' Show tooltip
        '--------------------------------------------
        entityTip.Show(
        sb.ToString(),
        pnlPicmodeler,
        pnlPicmodeler.PointToClient(Cursor.Position),
        2000
    )

        ' Change cursor while hovering entity
        pnlPicmodeler.Cursor = Cursors.Help

    End Sub




    Private Function ArcStartPoint(a As DxfArc) As DxfPoint
        Dim rad = a.StartAngle * Math.PI / 180.0
        Return New DxfPoint(
        a.Center.X + a.Radius * Math.Cos(rad),
        a.Center.Y + a.Radius * Math.Sin(rad),
        0
    )
    End Function

    Private Function ArcEndPoint(a As DxfArc) As DxfPoint
        Dim rad = a.EndAngle * Math.PI / 180.0
        Return New DxfPoint(
        a.Center.X + a.Radius * Math.Cos(rad),
        a.Center.Y + a.Radius * Math.Sin(rad),
        0
    )
    End Function


    Private Sub AppendLineInfo(sb As StringBuilder, cad As CadEntity)

        Dim sp = cad.StartPt
        Dim ep = cad.EndPt

        Dim dx As Double = ep.X - sp.X
        Dim dy As Double = ep.Y - sp.Y

        Dim length As Double = Math.Sqrt(dx * dx + dy * dy)
        Dim angleDeg As Double = Math.Atan2(dy, dx) * 180.0 / Math.PI
        If angleDeg < 0 Then angleDeg += 360

        sb.AppendLine("Type: Line")
        sb.AppendLine($"Start: ({sp.X:0.###}, {sp.Y:0.###})")
        sb.AppendLine($"End:   ({ep.X:0.###}, {ep.Y:0.###})")
        sb.AppendLine($"Length: {length:0.###}")
        sb.AppendLine($"Angle: {angleDeg:0.###}°")

    End Sub

    Private Sub AppendArcInfo(sb As StringBuilder, cad As CadEntity)
        Dim sp = cad.StartPt
        Dim ep = cad.EndPt
        Dim cp = cad.CircleCenter

        sb.AppendLine("Type: Arc")
        sb.AppendLine($"Start:  ({sp.X:0.###}, {sp.Y:0.###})")
        sb.AppendLine($"End:    ({ep.X:0.###}, {ep.Y:0.###})")

        If cp.HasValue Then
            sb.AppendLine($"Center: ({cp.Value.X:0.###}, {cp.Value.Y:0.###})")
        Else
            sb.AppendLine("Center: N/A")
        End If

        sb.AppendLine($"Radius: {If(cad.ArcRadius.HasValue, cad.ArcRadius.Value.ToString("0.###"), "N/A")}")
        sb.AppendLine($"Direction: {If(cad.ArcIsClockwise.HasValue AndAlso cad.ArcIsClockwise.Value, "CW", "CCW")}")
    End Sub

    Private Sub AppendCircleInfo(sb As StringBuilder, cad As CadEntity)
        Dim cp = cad.CircleCenter

        sb.AppendLine("Type: Circle")
        If cp.HasValue Then
            sb.AppendLine($"Center: ({cp.Value.X:0.###}, {cp.Value.Y:0.###})")
        Else
            sb.AppendLine("Center: N/A")
        End If

        sb.AppendLine($"Radius: {If(cad.CircleRadius.HasValue, cad.CircleRadius.Value.ToString("0.###"), "N/A")}")
    End Sub



    Private Sub HandleEntitySelected(ent As ImportedEntity)

        If ent Is Nothing Then Exit Sub

        ' Placeholder for selection behavior
        ' Examples (future):
        '   PropertyGrid.SelectedObject = ent
        '   Show context menu
        '   Update status bar
        '   Highlight entity

    End Sub
    Public Structure ViewTransform
        Public Scale As Double
        Public OffsetX As Double
        Public OffsetY As Double
    End Structure

    ' Convert machine coordinates to screen coordinates
    Public Function WorldToScreen(x As Double, y As Double, vt As ViewTransform, panelHeight As Integer) As PointF
        Dim sx As Single = CSng(x * vt.Scale + vt.OffsetX)
        Dim sy As Single = CSng(panelHeight - (y * vt.Scale + vt.OffsetY))
        Return New PointF(sx, sy)
    End Function


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

            ' Mouse position relative to panel
            Dim mouseX As Double = e.X
            Dim mouseY As Double = e.Y

            ' Zoom factor per wheel tick
            Dim zoomFactor As Double = 1.05
            If e.Delta < 0 Then zoomFactor = 1.0 / zoomFactor

            ' Compute new scale
            Dim newScale As Double = currentScale * zoomFactor
            newScale = Math.Max(minScale, Math.Min(maxScale, newScale)) ' clamp

            ' Adjust offsets to keep zoom centered at cursor
            OffsetX = mouseX - (mouseX - OffsetX) * (newScale / currentScale)
            OffsetY = mouseY - (mouseY - OffsetY) * (newScale / currentScale)

            ' Update current scale
            currentScale = newScale

            ' Trigger redraw
            pnlPicmodeler.Invalidate()
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

    Public Sub ApplyFilterTolerance(
    entities As List(Of ImportedEntity),
    filterTol As Double)
        Dim LineLength As Double
        Dim ArcLength As Double

        For i As Integer = entities.Count - 1 To 0 Step -1
            Dim e = entities(i)

            Select Case e.GeometryType

                Case ImportedEntity.GeometryTypeEnum.Line
                    If e.Line Is Nothing Then Continue For
                    Dim length = GetLineLength(e.Line)
                    If length < filterTol Then entities.RemoveAt(i)

                Case ImportedEntity.GeometryTypeEnum.Arc
                    If e.Arc Is Nothing Then Continue For
                    Dim length = GetArcLength(e.Arc)
                    If length < filterTol Then entities.RemoveAt(i)

                Case ImportedEntity.GeometryTypeEnum.Circle
                    ' FilterTolerance does not apply to circles

            End Select
        Next
    End Sub

    Public Function GetLineLength(line As DxfLine) As Double
        Dim dx As Double = line.P2.X - line.P1.X
        Dim dy As Double = line.P2.Y - line.P1.Y
        Dim dz As Double = line.P2.Z - line.P1.Z

        Return Math.Sqrt(dx * dx + dy * dy + dz * dz)

    End Function

    ' Calculate circumference of a DXF Circle
    Public Function GetCircleCircumference(circle As DxfCircle) As Double
        Return 2 * Math.PI * circle.Radius
    End Function

    ' Calculate arc length of a DXF Arc
    Public Function GetArcLength(arc As DxfArc) As Double
        Dim theta As Double = arc.EndAngle - arc.StartAngle
        If theta < 0 Then theta += 360  ' Handle arcs crossing 0° (always counterclockwise in DXF)

        Return (theta / 360) * (2 * PI * arc.Radius)
    End Function

    Public Sub ApplyCleanTolerance(
    entities As List(Of ImportedEntity),
    cleanTol As Double)

        If entities Is Nothing OrElse entities.Count < 2 Then Return

        For i As Integer = 0 To entities.Count - 2
            Dim e1 = entities(i)

            For j As Integer = i + 1 To entities.Count - 1
                Dim e2 = entities(j)

                '--------------------------------------------------
                ' Line - Line
                '--------------------------------------------------
                If e1.GeometryType = ImportedEntity.GeometryTypeEnum.Line AndAlso
               e2.GeometryType = ImportedEntity.GeometryTypeEnum.Line Then

                    CleanLineLine(e1.Line, e2.Line, cleanTol)
                End If

                '--------------------------------------------------
                ' Line - Arc
                '--------------------------------------------------
                If e1.GeometryType = ImportedEntity.GeometryTypeEnum.Line AndAlso
               e2.GeometryType = ImportedEntity.GeometryTypeEnum.Arc Then

                    CleanLineArc(e1.Line, e2.Arc, cleanTol)
                End If

                If e1.GeometryType = ImportedEntity.GeometryTypeEnum.Arc AndAlso
               e2.GeometryType = ImportedEntity.GeometryTypeEnum.Line Then

                    CleanLineArc(e2.Line, e1.Arc, cleanTol)
                End If

                '--------------------------------------------------
                ' Arc - Arc
                '--------------------------------------------------
                If e1.GeometryType = ImportedEntity.GeometryTypeEnum.Arc AndAlso
               e2.GeometryType = ImportedEntity.GeometryTypeEnum.Arc Then

                    CleanArcArc(e1.Arc, e2.Arc, cleanTol)
                End If

            Next
        Next
    End Sub

    Private Function Distance(p1 As DxfPoint, p2 As DxfPoint) As Double
        Dim dx = p2.X - p1.X
        Dim dy = p2.Y - p1.Y
        Dim dz = p2.Z - p1.Z
        Return Math.Sqrt(dx * dx + dy * dy + dz * dz)
    End Function

    Private Sub SnapPoints(ByRef p1 As DxfPoint, ByRef p2 As DxfPoint)
        Dim mx = (p1.X + p2.X) / 2.0
        Dim my = (p1.Y + p2.Y) / 2.0
        Dim mz = (p1.Z + p2.Z) / 2.0

        p1 = New DxfPoint(mx, my, mz)
        p2 = New DxfPoint(mx, my, mz)
    End Sub

    Private Sub CleanLineLine(
    l1 As DxfLine,
    l2 As DxfLine,
    tol As Double)

        If Distance(l1.P2, l2.P1) < tol Then
            SnapPoints(l1.P2, l2.P1)
        End If

        If Distance(l1.P1, l2.P2) < tol Then
            SnapPoints(l1.P1, l2.P2)
        End If

        If Distance(l1.P1, l2.P1) < tol Then
            SnapPoints(l1.P1, l2.P1)
        End If

        If Distance(l1.P2, l2.P2) < tol Then
            SnapPoints(l1.P2, l2.P2)
        End If
    End Sub

    Private Sub CleanLineArc(
    line As DxfLine,
    arc As DxfArc,
    tol As Double)

        Dim arcStart = GetArcStartPoint(arc)
        Dim arcEnd = GetArcEndPoint(arc)

        If Distance(line.P1, arcStart) < tol Then
            SnapPoints(line.P1, arcStart)
            SnapArcStartPoint(arc, arcStart)
        End If

        If Distance(line.P2, arcStart) < tol Then
            SnapPoints(line.P2, arcStart)
            SnapArcStartPoint(arc, arcStart)
        End If

        If Distance(line.P1, arcEnd) < tol Then
            SnapPoints(line.P1, arcEnd)
            SnapArcEndPoint(arc, arcEnd)
        End If

        If Distance(line.P2, arcEnd) < tol Then
            SnapPoints(line.P2, arcEnd)
            SnapArcEndPoint(arc, arcEnd)
        End If
    End Sub

    Private Sub CleanArcArc(
    a1 As DxfArc,
    a2 As DxfArc,
    tol As Double)

        Dim a1s = GetArcStartPoint(a1)
        Dim a1e = GetArcEndPoint(a1)
        Dim a2s = GetArcStartPoint(a2)
        Dim a2e = GetArcEndPoint(a2)

        If Distance(a1e, a2s) < tol Then
            SnapPoints(a1e, a2s)
            SnapArcEndPoint(a1, a1e)
            SnapArcStartPoint(a2, a2s)
        End If

        If Distance(a1s, a2e) < tol Then
            SnapPoints(a1s, a2e)
            SnapArcStartPoint(a1, a1s)
            SnapArcEndPoint(a2, a2e)
        End If
    End Sub

    Private Function GetArcStartPoint(arc As DxfArc) As DxfPoint
        Dim ang = arc.StartAngle * Math.PI / 180.0
        Return New DxfPoint(
        arc.Center.X + arc.Radius * Math.Cos(ang),
        arc.Center.Y + arc.Radius * Math.Sin(ang),
        arc.Center.Z)
    End Function

    Private Function GetArcEndPoint(arc As DxfArc) As DxfPoint
        Dim ang = arc.EndAngle * Math.PI / 180.0
        Return New DxfPoint(
        arc.Center.X + arc.Radius * Math.Cos(ang),
        arc.Center.Y + arc.Radius * Math.Sin(ang),
        arc.Center.Z)
    End Function

    Private Sub SnapArcStartPoint(arc As DxfArc, target As DxfPoint)
        ' Compute new StartAngle based on target point
        Dim dx = target.X - arc.Center.X
        Dim dy = target.Y - arc.Center.Y
        arc.StartAngle = Math.Atan2(dy, dx) * 180.0 / Math.PI
    End Sub

    Private Sub SnapArcEndPoint(arc As DxfArc, target As DxfPoint)
        ' Compute new EndAngle based on target point
        Dim dx = target.X - arc.Center.X
        Dim dy = target.Y - arc.Center.Y
        arc.EndAngle = Math.Atan2(dy, dx) * 180.0 / Math.PI
    End Sub

    ' Returns a list of endpoint pairs that are within gapTol
    '-------------------------------------------------
    ' Apply gap tolerance: store _gapPairs as points
    '-------------------------------------------------
    '-------------------------------------------------
    ' Apply gap tolerance to entity list
    '-------------------------------------------------
    Public Function ApplyGapTolerance(entities As List(Of ImportedEntity),
                                  gapTolerance As Double) As List(Of Tuple(Of ImportedEntity, ImportedEntity))
        ' Compute pairs that are within gap tolerance
        Dim gapPairs = ComputeGapPairs(entities, gapTolerance)

        ' This is where you could optionally "snap" entities together or store the pairs
        ' For now, just return the list
        Return gapPairs
    End Function

    '-------------------------------------------------
    ' Helper: get "start point" of an entity
    '-------------------------------------------------
    Private Function GetEntityStartPoint(e As ImportedEntity) As DxfPoint
        Select Case e.GeometryType
            Case ImportedEntity.GeometryTypeEnum.Line
                Return e.Line.P1
            Case ImportedEntity.GeometryTypeEnum.Arc
                ' Return start point of arc (compute from center, radius, start angle)
                Dim rad As Double = e.Arc.StartAngle * Math.PI / 180.0
                Return New DxfPoint(
                e.Arc.Center.X + e.Arc.Radius * Math.Cos(rad),
                e.Arc.Center.Y + e.Arc.Radius * Math.Sin(rad),
                0
            )
            Case ImportedEntity.GeometryTypeEnum.Circle
                ' Arbitrarily pick a point on circle as "start"
                Return New DxfPoint(e.Circle.Center.X + e.Circle.Radius, e.Circle.Center.Y, 0)
            Case Else
                Return New DxfPoint(0, 0, 0)
        End Select
    End Function


    '-------------------------------------------------
    ' Helper: compute minimal distance between 2 entities
    '-------------------------------------------------
    Private Function MinDistanceBetweenEntities(e1 As ImportedEntity, e2 As ImportedEntity) As Double
        ' Very simple approach: use start points
        Dim p1 = GetEntityStartPoint(e1)
        Dim p2 = GetEntityStartPoint(e2)

        Dim dx = p1.X - p2.X
        Dim dy = p1.Y - p2.Y
        Dim dz = p1.Z - p2.Z

        Return Math.Sqrt(dx * dx + dy * dy + dz * dz)
    End Function

    Private Function GetEndpoints(ent As ImportedEntity) As List(Of DxfPoint)
        Dim pts As New List(Of DxfPoint)

        Select Case ent.GeometryType
            Case ImportedEntity.GeometryTypeEnum.Line
                pts.Add(ent.Line.P1)
                pts.Add(ent.Line.P2)

            Case ImportedEntity.GeometryTypeEnum.Arc
                pts.Add(GetArcStartPoint(ent.Arc))
                pts.Add(GetArcEndPoint(ent.Arc))

            Case ImportedEntity.GeometryTypeEnum.Circle
                ' Circles are continuous; optionally skip or include a representative point
                ' pts.Add(ent.Circle.Center)  ' usually ignored for profile chaining
        End Select

        Return pts
    End Function

    '-----------------------------------------
    ' Build all profiles based on gap pairs
    '-----------------------------------------
    Public Function BuildProfiles(entities As List(Of ImportedEntity), gapTol As Double) As List(Of List(Of ImportedEntity))

        Dim profiles As New List(Of List(Of ImportedEntity))()
        Dim visited As New HashSet(Of ImportedEntity)()
        Dim gapPairs As List(Of Tuple(Of ImportedEntity, ImportedEntity)) = ComputeGapPairs(entities, gapTol)

        For Each e As ImportedEntity In entities
            If visited.Contains(e) Then Continue For

            Dim profile As New List(Of ImportedEntity)()
            Dim stack As New Stack(Of ImportedEntity)()
            stack.Push(e)

            While stack.Count > 0
                Dim current As ImportedEntity = stack.Pop()
                If visited.Contains(current) Then Continue While

                visited.Add(current)
                profile.Add(current)

                ' Add connected entities from gapPairs
                For Each pair As Tuple(Of ImportedEntity, ImportedEntity) In gapPairs
                    If pair.Item1 Is current AndAlso Not visited.Contains(pair.Item2) Then
                        stack.Push(pair.Item2)
                    ElseIf pair.Item2 Is current AndAlso Not visited.Contains(pair.Item1) Then
                        stack.Push(pair.Item1)
                    End If
                Next
            End While

            profiles.Add(profile)
        Next

        Return profiles
    End Function



    '-----------------------------------------
    ' Assign CW/CCW directions to profiles
    '-----------------------------------------
    '-----------------------------------------
    ' Assign CW/CCW directions to CadEntity profiles
    '-----------------------------------------
    Public Sub AssignProfileDirections(
    profiles As List(Of List(Of CadEntity)),
    directionSetting As String)

        'For Each profile As List(Of CadEntity) In profiles

        '    Select Case directionSetting.ToLower()

        '        Case "cw"
        '            For Each e As CadEntity In profile
        '                e.WindingDirection = "CW"
        '            Next

        '        Case "ccw"
        '            For Each e As CadEntity In profile
        '                e.WindingDirection = "CCW"
        '            Next

        '        Case "auto"
        '            ' Signed area determines orientation
        '            Dim signedArea As Double = ComputeSignedArea(profile)

        '            ' Positive area → CCW, Negative → CW
        '            Dim profileDirection As String =
        '            If(signedArea >= 0, "CCW", "CW")

        '            For Each e As CadEntity In profile
        '                e.WindingDirection = profileDirection
        '            Next

        '    End Select

        'Next
    End Sub


    'Private Function ComputeProfileCentroid(profile As List(Of ImportedEntity)) As DxfPoint
    '    ' Simple average of all entity points
    '    Dim sumX As Double = 0
    '    Dim sumY As Double = 0
    '    Dim count As Integer = 0

    '    For Each e As ImportedEntity In profile
    '        Dim endpoints = CamBuilder.GetEntityEndpoints(e)
    '        sumX += endpoints.Item1.X + endpoints.Item2.X
    '        sumY += endpoints.Item1.Y + endpoints.Item2.Y
    '        count += 2
    '    Next


    '    Return New DxfPoint(sumX / count, sumY / count, 0)
    'End Function


    Private Function PointInsideProfile(pt As PointF, profile As List(Of ImportedEntity)) As Boolean
        Dim crossings As Integer = 0
        Dim points As New List(Of PointF)

        ' Build points list from profile
        For Each ent In profile
            Select Case ent.GeometryType
                Case ImportedEntity.GeometryTypeEnum.Line
                    points.Add(New PointF(CSng(ent.Line.P1.X), CSng(ent.Line.P1.Y)))
                    points.Add(New PointF(CSng(ent.Line.P2.X), CSng(ent.Line.P2.Y)))

                Case ImportedEntity.GeometryTypeEnum.Arc
                    ' Approximate arc with center point only (simpler)
                    points.Add(New PointF(CSng(ent.Arc.Center.X), CSng(ent.Arc.Center.Y)))

                Case ImportedEntity.GeometryTypeEnum.Circle
                    points.Add(New PointF(CSng(ent.Circle.Center.X), CSng(ent.Circle.Center.Y)))
            End Select
        Next

        ' Remove duplicates
        points = points.Distinct().ToList()

        ' Ray-casting
        Dim n As Integer = points.Count
        For i As Integer = 0 To n - 1
            Dim j As Integer = (i + 1) Mod n
            If (((points(i).Y > pt.Y) Xor (points(j).Y > pt.Y)) AndAlso
                 (pt.X < ((points(j).X - points(i).X) * (pt.Y - points(i).Y) / (points(j).Y - points(i).Y) + points(i).X))) Then
                crossings += 1
            End If
        Next

        Return (crossings Mod 2) = 1
    End Function

    Private Sub pnlPicmodeler_MouseEnter(sender As Object, e As EventArgs) Handles pnlPicmodeler.MouseEnter
        pnlPicmodeler.Focus()

    End Sub


    Private Function ScreenToWorld(pt As Point) As PointF
        Dim worldX As Single =
        CSng(((pt.X - _offsetX) / _scale) + _minX)

        Dim worldY As Single =
        CSng(((pnlPicmodeler.ClientSize.Height - pt.Y - _offsetY) / _scale) + _minY)

        Return New PointF(worldX, worldY)
    End Function

    Private Function F(v As Double) As String
        Return v.ToString("0.###")
    End Function

    Private Sub pnlPicmodeler_MouseHover(sender As Object, e As EventArgs) Handles pnlPicmodeler.MouseHover
        If _hoverEntity IsNot Nothing Then
            pnlPicmodeler.Cursor = Cursors.Help
        Else
            pnlPicmodeler.Cursor = Cursors.Default
        End If

    End Sub

    Private Sub pnlPicmodeler_MouseLeave(sender As Object, e As EventArgs) Handles pnlPicmodeler.MouseLeave
        If _inquireMode Then
            entityTip.Hide(pnlPicmodeler)
        End If

        pnlPicmodeler.Cursor = If(_inquireMode, Cursors.Help, Cursors.Default)
    End Sub

    Private Sub bbInquire_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbInquire.ItemClick

    End Sub

    Private Sub frmMain_KeyDown(sender As Object, e As KeyEventArgs) Handles Me.KeyDown
        If e.KeyCode = Keys.Escape AndAlso _inquireMode Then
            ExitInquireMode()
            e.Handled = True
        End If
    End Sub

    Private ReadOnly Property IsInquireMode As Boolean
        Get
            Return _inquireMode
        End Get
    End Property

    Private Sub ExitInquireMode()
        _inquireMode = False
        _hoverEntity = Nothing
        entityTip.Hide(pnlPicmodeler)
        pnlPicmodeler.Cursor = Cursors.Default
        pnlPicmodeler.Invalidate()
    End Sub


    ' ------------------------------
    ' From ImportedEntity
    ' ------------------------------
    Public Shared Function FromImportedEntity(imp As ImportedEntity) As CadEntity
        If imp Is Nothing Then Return Nothing

        Dim cad As CadEntity = Nothing

        Select Case imp.GeometryType
            Case ImportedEntity.GeometryTypeEnum.Line
                ' --- Convert DXF line ---
                cad = CadEntity.CreateFromLine(
                startPt:=New CadEntity.Point2D(imp.Line.P1.X, imp.Line.P1.Y),
                endPt:=New CadEntity.Point2D(imp.Line.P2.X, imp.Line.P2.Y),
                layer:=imp.Line.Layer
            )

            Case ImportedEntity.GeometryTypeEnum.Arc
                ' --- Convert DXF arc ---
                Dim arcClockwise As Boolean = (imp.Arc.EndAngle - imp.Arc.StartAngle) < 0

                cad = CadEntity.CreateFromArc(
                center:=New CadEntity.Point2D(imp.Arc.Center.X, imp.Arc.Center.Y),
                radius:=imp.Arc.Radius,
                startAngle:=imp.Arc.StartAngle,
                endAngle:=imp.Arc.EndAngle,
                layer:=imp.Arc.Layer,
                clockwise:=arcClockwise
            )

            Case ImportedEntity.GeometryTypeEnum.Circle
                ' --- Convert DXF circle ---
                cad = CadEntity.CreateFromCircle(
                center:=New CadEntity.Point2D(imp.Circle.Center.X, imp.Circle.Center.Y),
                radius:=imp.Circle.Radius,
                layer:=imp.Circle.Layer
            )

            Case Else
                Return Nothing
        End Select

        Return cad
    End Function


    Private Sub SetupPanelHandlers()
        ' Redraw when panel is created
        AddHandler pnlPicmodeler.HandleCreated, Sub() RedrawAll()

        ' Redraw on panel resize
        AddHandler pnlPicmodeler.Resize, Sub() RedrawAll()

        ' Optional: force redraw when invalidated (e.g., uncovered)
        AddHandler pnlPicmodeler.Invalidated, Sub() RedrawAll()
    End Sub




End Class



