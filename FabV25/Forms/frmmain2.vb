
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
Imports FabV25_WIN8.modView
Imports FabV25_WIN8.MateriaWrapper

Imports FabV25_WIN8.DxfDrawable
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
Imports FabV25_WIN8.SelectionSet
Imports System.Runtime.CompilerServices


Public Class frmMain2
    Inherits RibbonForm

    ' Application lifecycle
    Private _uiInitialized As Boolean = False

    Private _hoverDrawable As DxfDrawable = Nothing

    Dim currentScale As Double

    Private _minX As Double
    Private _minY As Double
    Private _offsetX As Double
    Private _offsetY As Double
    Private _scale As Single = 1.0F
    Private _ox As Double
    Private _oy As Double
    Private Const ZoomFactor As Single = 1.1F
    Private Const MinScale As Single = 0.01F
    Private Const MaxScale As Single = 1000.0F


    Private isPanning As Boolean
    Private panStartMouse As Point
    Private panStartOffsetX As Double
    Private panStartOffsetY As Double

    Private _measureActive As Boolean = False
    Private _measureCurrentScreen As Point

    Private _selectionManager As New SelectionManager()
    Private _selectionEnabled As Boolean = False



    ' Hot dot display toggle
    Private _hotDotsEnabled As Boolean = True

    ' Current snap point in world coordinates
    Private _currentHotDot As PointF? = Nothing
    ' ---------- Snap ----------
    Private _snapEnabled As Boolean = True
    Private _activeSnap As SnapResult? = Nothing   ' Nullable structure
    Private _snapScreenPoint As PointF? = Nothing



    ' Radius in pixels for the hot dot
    Private Const HotDotRadius As Integer = 4


    Private lastMousePos As Point
    Private _panStart As Point
    Private _offsetStartX As Double
    Private _offsetStartY As Double
    Private _gapPairs As List(Of Tuple(Of DxfPoint, DxfPoint))

    ' Each profile is a list of connected entities
    Private _profiles As New List(Of List(Of ImportedEntity))

    'Private _importedProfiles As List(Of CamProfile)

    Private _inquireMode As Boolean = False
    Private _dxfImporter As IDxfImporter = New DxfImporter_IxMilia()


    ' View history stacks
    Private _viewHistory As New Stack(Of ViewTransform)
    Private _redoHistory As New Stack(Of ViewTransform)

    Private Const MaxHistorySize As Integer = 20

    Private _isRestoringView As Boolean = False ' to prevent pushing when restoring
    Private _windowSelecting As Boolean = False
    Private _windowStart As Point
    Private _windowEnd As Point
    Private _currentTool As String = "None"

    Private _isMeasuring As Boolean = False
    Private _measureStartWorld As PointF
    Private _measureEndWorld As PointF
    Private _measureTooltip As String = ""


    ' Replace old LineF lists
    Public CurrentDXFEntities As New List(Of CadEntity)         ' Entities for main panel
    Public CurrentPreviewEntities As New List(Of CadEntity)     ' Entities for file open preview


    Private _loadedDxf As DxfFile = Nothing

    Public Property IsDefaultMapped As Boolean

    Const NORMAL_WIDTH As Single = 1.0F
    Const DEFAULT_WIDTH As Single = 2.5F


    Private _modelBounds As RectangleF

    Private _offset As PointF


    Private lastEntity As ImportedEntity = Nothing
    '------------------------------------------
    Private WithEvents entityTip As New ToolTip()
    Private lastHoverEntity As CadEntity = Nothing
    Private _hoverEntity As CadEntity = Nothing
    Private _modelerToolTip As New ToolTip() With {
    .ShowAlways = False,
    .InitialDelay = 400,
    .ReshowDelay = 100,
    .AutoPopDelay = 4000
}

    Private _selectionMode As GeometryTypeEnum? = Nothing

    Private Const HitTolerancePx As Single = 6.0F

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

        AddHandler _selectionManager.SelectionChanged, AddressOf OnSelectionChanged

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


    Private Sub OnSelectionChanged()

        Me.Invalidate()

    End Sub

    Private Sub rgbiSkins_GalleryInitDropDownGallery(ByVal sender As Object, ByVal e As DevExpress.XtraBars.Ribbon.InplaceGalleryEventArgs) Handles rgbiSkins.GalleryInitDropDownGallery

    End Sub


    Private Sub frmMain2_Load(sender As Object, e As EventArgs) Handles Me.Load

        Try

            EnableDoubleBuffering(pnlPicmodeler)

            _cadEntities = New List(Of CadEntity)
            _visibleEntities = New List(Of CadEntity)

            _uiInitialized = True
            _sceneReady = False   ' Nothing loaded yet

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



    Private Sub frmMain2_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        Me.Dispose()

    End Sub

    Private Sub frmMain2_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        End
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

        ' UI not ready yet (very early startup)
        If Not _uiInitialized Then Return

        '' Scene not loaded yet → draw empty workspace
        'If Not _sceneReady OrElse CurrentJob Is Nothing Then
        '    DrawEmptyWorkspace(e.Graphics)
        '    Return
        'End If

        ' --------------------------------------------
        ' 1) First-time preparation
        ' --------------------------------------------

        If _selectedDxfPath IsNot Nothing AndAlso _DoPrepareUI Then
            PrePareUI(_selectedDxfPath)  ' should initialize CurrentJob, _visibleEntities, _cadEntities

            BuildDrawables()

            _DoPrepareUI = False
            _DoDrawUI = True
        End If

        ' --------------------------------------------
        ' 2) Ensure essential objects exist
        ' --------------------------------------------
        If Not _DoDrawUI OrElse CurrentJob Is Nothing Then
            ' Nothing to draw yet
            e.Graphics.Clear(System.Drawing.Color.Black)
            Return
        End If


        If _cadEntities Is Nothing Then _cadEntities = New List(Of CadEntity)

        ' --------------------------------------------
        ' 3) Draw base scene
        ' --------------------------------------------
        RenderScene(e.Graphics) ' Draw sheets, workzones, clamps, entities, etc.

        ' --------------------------------------------
        ' 4) Draw overlays in proper order
        ' --------------------------------------------
        ' Hot-dots / snap indicator
        If _activeSnap.HasValue AndAlso _hotDotsEnabled Then
            DrawSnapIndicator(e.Graphics, _activeSnap.Value)
        End If

        ' ViewWindow rubber-band
        If _windowSelecting Then
            DrawRubberBand(_windowStart, _windowEnd, e.Graphics)
        End If

        ' Measure line & tooltip
        If _isMeasuring Then
            DrawMeasureLine(_measureStartWorld, _measureEndWorld, e.Graphics)
            DrawOverlayText(e.Graphics, _measureTooltip, _measureEndWorld)
        End If

        ' Any additional overlays
        DrawOverlay(e.Graphics)

    End Sub



    ' Central overlay routine
    Private Sub DrawOverlay(g As Graphics)
        ' --------------------------
        ' 1) Hot-Dot
        ' --------------------------
        If _activeSnap.HasValue AndAlso _hotDotsEnabled Then
            DrawSnapIndicator(g, _activeSnap.Value)
        End If

        ' --------------------------
        ' 2) Measurement line + tooltip
        ' --------------------------
        If _isMeasuring Then
            DrawMeasureLine(_measureStartWorld, _measureEndWorld, g)
            DrawOverlayText(g, _measureTooltip, WorldToScreen(_measureEndWorld.X, _measureEndWorld.Y,
                                                          myTransform, pnlPicmodeler.Height))
        End If

        ' --------------------------
        ' 3) ViewWindow rubber-band
        ' --------------------------
        If _windowSelecting Then
            DrawRubberBand(_windowStart, _windowEnd, g)
        End If
    End Sub

    ''' <summary>
    ''' Draw a simple overlay text with background for readability.
    ''' </summary>
    Private Sub DrawOverlayText(text As String, screenPt As PointF, g As Graphics)
        Using brushBack As New SolidBrush(System.Drawing.Color.FromArgb(160, System.Drawing.Color.Black))
            Using brushText As New SolidBrush(System.Drawing.Color.White)
                Using fnt As New Font("Segoe UI", 9, FontStyle.Regular)
                    Dim size As SizeF = g.MeasureString(text, fnt)
                    Dim rect As New RectangleF(screenPt.X + 5, screenPt.Y - size.Height / 2, size.Width + 4, size.Height + 2)
                    g.FillRectangle(brushBack, rect)
                    g.DrawString(text, fnt, brushText, rect.X + 2, rect.Y + 1)
                End Using
            End Using
        End Using
    End Sub

    Private Sub RenderScene(g As Graphics)

        If CurrentJob Is Nothing Then Exit Sub

        g.SmoothingMode = Drawing2D.SmoothingMode.AntiAlias
        g.Clear(System.Drawing.Color.Black)

        ' -------------------------------------------------
        ' 1) BASE SCENE
        ' -------------------------------------------------
        RestoreAndDrawJob()
        DrawSheetAndWorkzones(g, matLength, matWidth)
        DrawCadEntities(g, myTransform, panelHeight)

        ' -------------------------------------------------
        ' 2) HOVER HIGHLIGHT (still View-owned for now)
        ' -------------------------------------------------
        If _hoverEntity IsNot Nothing Then
            DrawEntityHighlight(g, _hoverEntity)
        End If

        ' -------------------------------------------------
        ' 3) SELECTION HIGHLIGHT (SelectionManager-owned)
        ' -------------------------------------------------
        If _selectionManager.HasSelection Then

            Using pen As New Pen(System.Drawing.Color.Yellow, 2.0F)

                For Each sel In _selectionManager.Selected

                    Dim ent As CadEntity = TryCast(sel, CadEntity)
                    If ent Is Nothing Then Continue For

                    Select Case ent.GeometryType

                        Case GeometryTypeEnum.Line

                            Dim line As CadLine = TryCast(ent, CadLine)
                            If line Is Nothing Then Continue For

                            Dim p1 = WorldToScreen(line.StartPoint.X, line.StartPoint.Y, myTransform, panelHeight)
                            Dim p2 = WorldToScreen(line.EndPoint.X, line.EndPoint.Y, myTransform, panelHeight)

                            g.DrawLine(pen, p1, p2)

                        Case GeometryTypeEnum.Circle

                            Dim circle As CadCircle = TryCast(ent, CadCircle)
                            If circle Is Nothing Then Continue For

                            Dim center = WorldToScreen(circle.Center.X, circle.Center.Y, myTransform, panelHeight)
                            Dim r As Single = CSng(circle.Radius * myTransform.Scale)

                            g.DrawEllipse(pen,
                                      CSng(center.X - r),
                                      CSng(center.Y - r),
                                      CSng(r * 2),
                                      CSng(r * 2))

                        Case GeometryTypeEnum.Arc

                            Dim arc As CadArc = TryCast(ent, CadArc)
                            If arc Is Nothing Then Continue For

                            Dim center = WorldToScreen(arc.Center.X, arc.Center.Y, myTransform, panelHeight)
                            Dim r As Single = CSng(arc.Radius * myTransform.Scale)

                            Dim startAngle As Single = CSng(arc.StartAngle)
                            Dim sweepAngle As Single = CSng(arc.EndAngle - arc.StartAngle)

                            g.DrawArc(pen,
                                  CSng(center.X - r),
                                  CSng(center.Y - r),
                                  CSng(r * 2),
                                  CSng(r * 2),
                                  startAngle,
                                  sweepAngle)

                    End Select

                Next

            End Using

        End If

    End Sub

    Private Sub DrawEntityHighlight(g As Graphics, ent As CadEntity)

        Using pen As New Pen(System.Drawing.Color.Red, 2.0F)
            pen.DashStyle = DashStyle.Dash
            DrawCadEntity(g, ent, pen)
        End Using

    End Sub


    Private Sub DrawEntitySelection(g As Graphics, ent As CadEntity)

        Using pen As New Pen(System.Drawing.Color.LimeGreen, 2.5F)
            DrawCadEntity(g, ent, pen)
        End Using

    End Sub

    Private Sub DrawCadEntity(g As Graphics, ent As CadEntity, pen As Pen)

        Select Case ent.GeometryType

            Case GeometryTypeEnum.Line
                Dim l = DirectCast(ent, CadLine)

                Dim p1 = WorldToScreen(l.StartPoint.X, l.StartPoint.Y, myTransform, pnlPicmodeler.Height)
                Dim p2 = WorldToScreen(l.EndPoint.X, l.EndPoint.Y, myTransform, pnlPicmodeler.Height)

                g.DrawLine(pen, p1, p2)

            Case GeometryTypeEnum.Circle
                Dim c = DirectCast(ent, CadCircle)

                Dim center = WorldToScreen(c.Center.X, c.Center.Y, myTransform, pnlPicmodeler.Height)
                Dim r = CSng(c.Radius * myTransform.Scale)

                g.DrawEllipse(
                pen,
                center.X - r,
                center.Y - r,
                2 * r,
                2 * r
            )

            Case GeometryTypeEnum.Arc

                Dim a = DirectCast(ent, CadArc)

                Dim center = WorldToScreen(a.Center.X, a.Center.Y, myTransform, pnlPicmodeler.Height)
                Dim r = CSng(a.Radius * myTransform.Scale)

                Dim sweep As Single
                If a.IsClockwise Then
                    sweep = a.EndAngle - a.StartAngle
                Else
                    sweep = a.StartAngle - a.EndAngle
                End If

                If sweep < 0 Then sweep += 360

                Dim startDeg As Single = -a.StartAngle
                Dim sweepDeg As Single = -sweep

                g.DrawArc(
                                    pen,
                                    center.X - r,
                                    center.Y - r,
                                    2 * r,
                                    2 * r,
                                    startDeg,
                                    sweepDeg
                                )

        End Select

    End Sub

    Public Sub DrawSheetAndWorkzones(g As Graphics, sheetWidth As Double, sheetHeight As Double)
        Dim margin As Integer = 20

        ' --- Draw sheet background ---
        Dim bl As PointF = WorldToScreen(0, 0, myTransform, panelHeight)
        Dim tr As PointF = WorldToScreen(sheetWidth, sheetHeight, myTransform, panelHeight)
        Dim sheetRect = HelperFunctions.RectFromPoints(bl, tr)
        g.FillRectangle(Brushes.LightGray, sheetRect)
        g.DrawRectangle(Pens.Black, sheetRect.X, sheetRect.Y, sheetRect.Width, sheetRect.Height)

        ' --- Draw workzones ---
        For i = 0 To CurrentJob.WorkZones.Count - 1
            Dim wz = CurrentJob.WorkZones(i)
            Dim wzColor As Color = WorkzoneColors(i Mod WorkzoneColors.Length)
            Dim wzBrush As New SolidBrush(System.Drawing.Color.FromArgb(50, wzColor))
            Dim wzPen As New Pen(wzColor, 2)

            ' Flip Y for workzone bottom-left origin
            Dim wzBL As PointF = WorldToScreen(wz.XMin, sheetHeight - wz.YMax, myTransform, panelHeight)
            Dim wzTR As PointF = WorldToScreen(wz.XMax, sheetHeight - wz.YMin, myTransform, panelHeight)
            Dim wzRect = HelperFunctions.RectFromPoints(wzBL, wzTR)

            g.FillRectangle(wzBrush, wzRect)
            g.DrawRectangle(wzPen, wzRect.X, wzRect.Y, wzRect.Width, wzRect.Height)

            For Each clamp In wz.Clamps

                ' Machine coordinates: clamp.Y is distance FROM BOTTOM of sheet
                ' Convert to top-origin before screen transform
                Dim yBottom = clamp.Y
                Dim yTop = clamp.Y + clamp.Height

                ' --- Punch rectangle ---
                Dim punchBL As PointF = WorldToScreen(clamp.X - clamp.Width / 2, yBottom, myTransform, panelHeight)

                Dim punchTR As PointF = WorldToScreen(clamp.X + clamp.Width / 2, yTop, myTransform, panelHeight)

                Dim punchRect = HelperFunctions.RectFromPoints(punchBL, punchTR)

                g.FillRectangle(wzBrush, punchRect)
                g.DrawRectangle(wzPen,
                    punchRect.X, punchRect.Y,
                    punchRect.Width, punchRect.Height)

                ' --- Torch rectangle ---
                Dim torchBL As PointF = WorldToScreen(clamp.X - clamp.TorchSize.Width / 2, yBottom, myTransform, panelHeight)

                Dim torchTR As PointF = WorldToScreen(clamp.X + clamp.TorchSize.Width / 2, yBottom + clamp.TorchSize.Height, myTransform, panelHeight)

                Dim torchRect = HelperFunctions.RectFromPoints(torchBL, torchTR)

                g.FillRectangle(wzBrush, torchRect)
                g.DrawRectangle(wzPen,
                    torchRect.X, torchRect.Y,
                    torchRect.Width, torchRect.Height)

            Next

        Next
    End Sub

    'Public Sub BuildDrawables()
    '    _drawables = New List(Of DxfDrawable)

    '    For Each cad In _cadEntities
    '        Dim d As New DxfDrawable With {
    '            .Entity = cad,
    '            .LayerName = cad.LayerName,
    '            .Color = System.Drawing.Color.Black
    '        }

    '        ' Map geometry by type
    '        If TypeOf cad Is CadEntity.CadLine Then
    '            Dim line = DirectCast(cad, CadEntity.CadLine)
    '            d.DrawableType = DxfDrawable.DrawableTypeEnum.Line
    '            d.StartPoint = line.StartPoint
    '            d.EndPoint = line.EndPoint

    '        ElseIf TypeOf cad Is CadEntity.CadCircle Then
    '            Dim circle = DirectCast(cad, CadEntity.CadCircle)
    '            d.DrawableType = DxfDrawable.DrawableTypeEnum.Circle
    '            d.Center = circle.Center
    '            d.Radius = circle.Radius

    '        ElseIf TypeOf cad Is CadEntity.CadArc Then
    '            Dim arc = DirectCast(cad, CadEntity.CadArc)
    '            d.DrawableType = DxfDrawable.DrawableTypeEnum.Arc
    '            d.Center = arc.Center
    '            d.Radius = arc.Radius
    '            d.StartAngle = arc.StartAngle
    '            d.EndAngle = arc.EndAngle

    '            'ElseIf TypeOf cad Is CadEntity.CadPolyline Then
    '            '    Dim pl = DirectCast(cad, CadEntity.CadPolyline)
    '            '    d.DrawableType = DxfDrawable.DrawableTypeEnum.PolyLine
    '            '    d.Points = New List(Of PointF)(pl.Points)

    '            'ElseIf TypeOf cad Is CadEntity.CadRectangle Then
    '            '    Dim rect = DirectCast(cad, CadEntity.CadRectangle)
    '            '    d.DrawableType = DxfDrawable.DrawableTypeEnum.Rectangle
    '            '    d.StartPoint = rect.StartPoint
    '            '    d.Width = rect.Width
    '            '    d.Height = rect.Height
    '        End If

    '        ' Add to drawables
    '        _drawables.Add(d)

    '        Dim cad As CadEntity = sourceCad

    '        Dim d As New DxfDrawable()

    '        d.Entity = cad

    '        cad.Drawable = d

    '    Next

    '    Debug.Print("MODEL EXTENTS CHECK")


    '    Dim allBounds As New List(Of RectangleF)

    '    For Each d In _drawables
    '        allBounds.Add(GetBounds(d))
    '    Next

    '    Dim minX = allBounds.Min(Function(r) r.Left)
    '    Dim maxX = allBounds.Max(Function(r) r.Right)
    '    Dim minY = allBounds.Min(Function(r) r.Top)
    '    Dim maxY = allBounds.Max(Function(r) r.Bottom)

    '    Debug.Print($"EXTENTS: X {minX} → {maxX}   Y {minY} → {maxY}")
    '    Debug.Print($"EXTENTS: X {minX} → {maxX}   Y {minY} → {maxY}")

    'End Sub

    Public Sub BuildDrawables()

        ' ---------------------------------
        ' INIT (ONLY ONCE)
        ' ---------------------------------
        _drawables = New List(Of DxfDrawable)

        ' ---------------------------------
        ' BUILD DRAWABLES FROM CAD ENTITIES
        ' ---------------------------------
        For Each cad In _cadEntities

            Dim d As New DxfDrawable With {
            .Entity = cad,
            .LayerName = cad.LayerName,
            .Color = System.Drawing.Color.Black
        }

            ' Link BOTH directions (important for selection + rendering)
            cad.Drawable = d

            ' ---------------------------------
            ' GEOMETRY MAPPING (SINGLE SOURCE)
            ' ---------------------------------
            If TypeOf cad Is CadLine Then

                Dim line = DirectCast(cad, CadLine)

                d.DrawableType = DxfDrawable.DrawableTypeEnum.Line
                d.StartPoint = line.StartPoint
                d.EndPoint = line.EndPoint

                cad.GeometryType = GeometryTypeEnum.Line

            ElseIf TypeOf cad Is CadCircle Then

                Dim circle = DirectCast(cad, CadCircle)

                d.DrawableType = DxfDrawable.DrawableTypeEnum.Circle
                d.Center = circle.Center
                d.Radius = circle.Radius

                cad.GeometryType = GeometryTypeEnum.Circle

            ElseIf TypeOf cad Is CadArc Then

                Dim arc = DirectCast(cad, CadArc)

                d.DrawableType = DxfDrawable.DrawableTypeEnum.Arc
                d.Center = arc.Center
                d.Radius = arc.Radius
                d.StartAngle = arc.StartAngle
                d.EndAngle = arc.EndAngle

                cad.GeometryType = GeometryTypeEnum.Arc

            ElseIf TypeOf cad Is CadPoint Then

                Dim pt = DirectCast(cad, CadPoint)

                d.DrawableType = DxfDrawable.DrawableTypeEnum.Point
                d.Center = pt.Position   ' assuming Position is PointF

                cad.GeometryType = GeometryTypeEnum.Point

            Else
                cad.GeometryType = GeometryTypeEnum.Unknown
            End If

            ' ---------------------------------
            ' ADD ONCE
            ' ---------------------------------
            _drawables.Add(d)

        Next

        ' ---------------------------------
        ' DEBUG EXTENTS (NOW TRUSTWORTHY)
        ' ---------------------------------
        Debug.Print("MODEL EXTENTS CHECK")

        If _drawables.Count = 0 Then
            Debug.Print("NO DRAWABLES")
            Return
        End If

        Dim allBounds = _drawables.
        Select(Function(d) DxfDrawable.GetBounds(d)).
        Where(Function(r) r <> RectangleF.Empty).
        ToList()

        If allBounds.Count = 0 Then
            Debug.Print("NO VALID BOUNDS")
            Return
        End If

        Dim minX = allBounds.Min(Function(r) r.Left)
        Dim maxX = allBounds.Max(Function(r) r.Right)
        Dim minY = allBounds.Min(Function(r) r.Top)
        Dim maxY = allBounds.Max(Function(r) r.Bottom)

        Debug.Print($"EXTENTS: X {minX} → {maxX}   Y {minY} → {maxY}")

        ' ---------------------------------
        ' BUILD SPATIAL INDEX (CRITICAL)
        ' ---------------------------------
        ' ---------------------------------
        ' SYNC SELECTION MANAGER
        ' ---------------------------------
        _selectionManager.SetEntities(_cadEntities)

        ' ---------------------------------
        ' BUILD SPATIAL INDEX
        ' ---------------------------------
        _selectionManager.BuildSpatialIndex()

    End Sub


    Private Sub DrawEmptyWorkspace(g As Graphics)
        Using f As New Font("Segoe UI", 10)
            g.DrawString(
            "Load a DXF to begin",
            f,
            Brushes.Gray,
            New PointF(20, 20)
        )
        End Using
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
        Using brush As New SolidBrush(System.Drawing.Color.Red)
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

    Private Function HitTestCadEntity(mouseScreen As PointF,
                                  tolPx As Single,
                                  ByRef hitDistWorld As Single) As CadEntity

        Dim tolWorld As Single = tolPx / myTransform.Scale

        If myTransform.Scale <= 0.000001 OrElse
   Single.IsNaN(myTransform.Scale) OrElse
   Single.IsInfinity(myTransform.Scale) Then

            Debug.Print("HitTestCadEntity ABORT - invalid transform scale = " &
                myTransform.Scale.ToString())

            Return Nothing
        End If


        Dim worldPt As PointF =
    ScreenToWorld(mouseScreen, myTransform, pnlPicmodeler.Height)

        If Single.IsNaN(worldPt.X) OrElse
   Single.IsNaN(worldPt.Y) OrElse
   Single.IsInfinity(worldPt.X) OrElse
   Single.IsInfinity(worldPt.Y) Then

            Debug.Print("HitTestCadEntity ABORT - invalid world point")
            Return Nothing
        End If


        If Single.IsNaN(tolWorld) OrElse
   Single.IsInfinity(tolWorld) Then

            Debug.Print("HitTestCadEntity ABORT - invalid tolerance")
            Return Nothing
        End If
        'Dim tolWorld As Single = tolPx / myTransform.Scale
        ' =========================================================
        ' SCREEN -> WORLD
        ' =========================================================
        Dim mouseWorld As PointF =
        ScreenToWorld(
            mouseScreen,
            myTransform,
            pnlPicmodeler.ClientSize.Height
        )

        ' =========================================================
        ' PIXEL TOLERANCE -> WORLD TOLERANCE
        '
        ' Scale = pixels per world unit
        ' Therefore:
        '
        '     tolWorld = tolPx / Scale
        ' =========================================================
        Dim scale As Single =
        If(myTransform.Scale > 0.0001F,
           myTransform.Scale,
           0.0001F)

        'Dim tolWorld As Single = tolPx / scale

        ' Minimum usable tolerance in model units.
        tolWorld = Math.Max(tolWorld, 0.1F)

        hitDistWorld = Single.MaxValue

        Debug.Print("========================================")
        Debug.Print("HitTestCadEntity")
        Debug.Print($"Screen     = {mouseScreen.X:F3}, {mouseScreen.Y:F3}")
        Debug.Print($"World      = {mouseWorld.X:F3}, {mouseWorld.Y:F3}")
        Debug.Print($"Scale      = {scale:F6}")
        Debug.Print($"TolPx      = {tolPx:F3}")
        Debug.Print($"TolWorld   = {tolWorld:F6}")
        Debug.Print("========================================")

        ' =========================================================
        ' HIT CANDIDATES
        ' =========================================================
        Dim results As New List(Of HitResult)

        For Each ent As CadEntity In _cadEntities

            Dim d As Single = Single.MaxValue

            Select Case ent.GeometryType

            ' =================================================
            ' LINE
            ' =================================================
                Case GeometryTypeEnum.Line

                    Dim line As CadLine =
                    DirectCast(ent, CadLine)

                    d =
                    DistancePointToLineWorld(
                        mouseWorld,
                        line.StartPoint,
                        line.EndPoint
                    )


            ' =================================================
            ' CIRCLE
            ' =================================================
                Case GeometryTypeEnum.Circle

                    Dim circle As CadCircle =
                    DirectCast(ent, CadCircle)

                    Dim centerDistance As Single =
                    Distance(
                        mouseWorld,
                        circle.Center
                    )

                    d =
                    Math.Abs(
                        centerDistance -
                        circle.Radius
                    )


            ' =================================================
            ' ARC
            ' =================================================
                Case GeometryTypeEnum.Arc

                    Dim arc As CadArc =
                    DirectCast(ent, CadArc)

                    Dim centerDistance As Single =
                    Distance(
                        mouseWorld,
                        arc.Center
                    )

                    Dim radialDistance As Single =
                    Math.Abs(
                        centerDistance -
                        arc.Radius
                    )

                    ' ---------------------------------------------
                    ' Determine angle of mouse point around arc
                    ' ---------------------------------------------
                    Dim angle As Double =
                    Math.Atan2(
                        mouseWorld.Y - arc.Center.Y,
                        mouseWorld.X - arc.Center.X
                    ) * 180.0 / Math.PI

                    ' Normalize to 0-360.
                    angle = angle Mod 360.0

                    If angle < 0 Then
                        angle += 360.0
                    End If

                    Dim startAngle As Double =
                    arc.StartAngle Mod 360.0

                    If startAngle < 0 Then
                        startAngle += 360.0
                    End If

                    Dim endAngle As Double =
                    arc.EndAngle Mod 360.0

                    If endAngle < 0 Then
                        endAngle += 360.0
                    End If

                    ' ---------------------------------------------
                    ' Determine whether point lies on arc sweep
                    ' ---------------------------------------------
                    Dim inArc As Boolean

                    If Not arc.IsClockwise Then

                        ' -----------------------------------------
                        ' Counter-clockwise
                        ' -----------------------------------------
                        If startAngle <= endAngle Then

                            inArc =
                            angle >= startAngle AndAlso
                            angle <= endAngle

                        Else

                            ' Arc crosses 0 degrees.
                            inArc =
                            angle >= startAngle OrElse
                            angle <= endAngle

                        End If

                    Else

                        ' -----------------------------------------
                        ' Clockwise
                        ' -----------------------------------------
                        If endAngle <= startAngle Then

                            inArc =
                            angle <= startAngle AndAlso
                            angle >= endAngle

                        Else

                            ' Arc crosses 0 degrees.
                            inArc =
                            angle <= startAngle OrElse
                            angle >= endAngle

                        End If

                    End If

                    If inArc Then
                        d = radialDistance
                    Else
                        d = Single.MaxValue
                    End If


                    ' =================================================
                    ' UNSUPPORTED ENTITY
                    ' =================================================
                Case Else

                    d = Single.MaxValue

            End Select

            ' =====================================================
            ' ADD HIT CANDIDATE
            ' =====================================================
            If d <= tolWorld Then

                results.Add(
                New HitResult With {
                    .Entity = ent,
                    .Distance = d
                }
            )

                Debug.Print(
                $"HIT CANDIDATE: " &
                $"{ent.GetType().Name} " &
                $"Distance={d:F6}"
            )

            End If

        Next

        ' =========================================================
        ' RESOLVE CLOSEST HIT
        ' =========================================================
        Dim best As HitResult =
        results.
        OrderBy(Function(h) h.Distance).
        FirstOrDefault()

        If best Is Nothing Then

            Debug.Print("NO HIT ON CLICK")
            Return Nothing

        End If

        hitDistWorld = best.Distance

        Debug.Print(
        $"BEST HIT: {best.Entity.GetType().Name} " &
        $"Distance={best.Distance:F6}"
    )

        Return best.Entity

    End Function




    Public Class HitResult
        Public Property Entity As CadEntity
        Public Property Distance As Single
    End Class
    Private Function PassesFilter(ent As CadEntity, filter As SELECTION_FILTER) As Boolean

        If filter = SELECTION_FILTER.SEL_ALL Then Return True

        Select Case ent.GeometryType

            Case GeometryTypeEnum.Line
                Return (filter And SELECTION_FILTER.SEL_LINE) <> 0

            Case GeometryTypeEnum.Arc
                Return (filter And SELECTION_FILTER.SEL_ARC) <> 0

            Case GeometryTypeEnum.Circle
                ' You need to decide this:
                ' Is a circle a HOLE or just geometry?
                Return (filter And SELECTION_FILTER.SEL_HOLE) <> 0

            Case Else
                Return False

        End Select

    End Function
    ''------------------------------------------
    '' Hit-testing function
    ''------------------------------------------
    'Private Function HitTestImported(mousePos As Point,
    '                             pnl As Panel,
    '                             hitTolerance As Single,
    '                             ByRef hitDistance As Single) As ImportedEntity

    '    hitDistance = Single.MaxValue

    '    If _importedEntities Is Nothing OrElse _importedEntities.Count = 0 Then
    '        Return Nothing
    '    End If



    '    Dim panelWidth = pnl.ClientSize.Width
    '    Dim panelHeight = pnl.ClientSize.Height

    '    Dim scaleX = panelWidth / (maxX - minX)
    '    Dim scaleY = panelHeight / (maxY - minY)
    '    Dim scale = Math.Min(scaleX, scaleY) * 0.9F

    '    Dim offsetX = (panelWidth - (maxX - minX) * scale) / 2.0F
    '    Dim offsetY = (panelHeight - (maxY - minY) * scale) / 2.0F

    '    '-------------------------------------------------
    '    ' Helpers
    '    '-------------------------------------------------
    '    Dim mouseF As New PointF(mousePos.X, mousePos.Y)

    '    Dim bestEntity As ImportedEntity = Nothing

    '    '-------------------------------------------------
    '    ' Hit test each entity
    '    '-------------------------------------------------
    '    For Each ent In _importedEntities

    '        Dim dist As Single = Single.MaxValue

    '        Select Case ent.GeometryType

    '            Case ImportedEntity.GeometryTypeEnum.Line
    '                Dim p1 = worldToScreen(ent.Line.P1)
    '                Dim p2 = worldToScreen(ent.Line.P2)

    '                dist = DistancePointToLine(mouseF, p1, p2)

    '            Case ImportedEntity.GeometryTypeEnum.Circle
    '                Dim c = worldToScreen(ent.Circle.Center)
    '                Dim r = CSng(ent.Circle.Radius * scale)

    '                dist = Math.Abs(Distance(mouseF, c) - r)

    '            Case ImportedEntity.GeometryTypeEnum.Arc
    '                Dim c = worldToScreen(ent.Arc.Center)
    '                Dim r = CSng(ent.Arc.Radius * scale)

    '                dist = Math.Abs(Distance(mouseF, c) - r)

    '        End Select

    '        If dist < hitDistance Then
    '            hitDistance = dist
    '            bestEntity = ent
    '        End If

    '    Next

    '    If hitDistance <= hitTolerance Then
    '        Return bestEntity
    '    End If



    '    Return Nothing

    'End Function

    Private Sub pnlPicmodeler_MouseMove(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseMove

        ' --------------------------
        ' 1) Handle panning
        ' --------------------------
        If isPanning Then
            SaveCurrentView()

            Dim dxScreen As Integer = e.X - panStartMouse.X
            Dim dyScreen As Integer = e.Y - panStartMouse.Y

            myTransform.OffsetX = panStartOffsetX + dxScreen
            myTransform.OffsetY = panStartOffsetY - dyScreen

            lastMousePos = e.Location
            pnlPicmodeler.Invalidate()
            Return
        End If

        ' --------------------------
        ' 2) Update model coordinates display
        ' --------------------------
        Dim modelPt As PointF = ScreenToWorld(e.Location, myTransform, pnlPicmodeler.ClientSize.Height)
        statusLocation.Caption = $"Location: X={modelPt.X:0.###}, Y={modelPt.Y:0.###}"

        ' --------------------------
        ' 3) ViewWindow rubber-band
        ' --------------------------
        If _windowSelecting Then
            _windowEnd = e.Location
            pnlPicmodeler.Invalidate()
        End If

        ' --------------------------
        ' 4) Active Snap
        ' --------------------------
        Dim snapTolPx As Single = 10
        _activeSnap = GetActiveSnap(e.Location, snapTolPx)

        If _activeSnap.HasValue Then
            _snapScreenPoint = WorldToScreen(
            _activeSnap.Value.WorldPoint.X,
            _activeSnap.Value.WorldPoint.Y,
            myTransform,
            pnlPicmodeler.Height)

            pnlPicmodeler.Cursor = Cursors.Cross
        Else
            _snapScreenPoint = Nothing
            pnlPicmodeler.Cursor = Cursors.Default
        End If

        ' --------------------------
        ' 5) Measurement tool
        ' --------------------------
        If _isMeasuring Then
            _measureEndWorld = If(_activeSnap.HasValue,
                             _activeSnap.Value.WorldPoint,
                             modelPt)

            Dim dx As Double = _measureEndWorld.X - _measureStartWorld.X
            Dim dy As Double = _measureEndWorld.Y - _measureStartWorld.Y
            Dim dist As Double = Math.Sqrt(dx * dx + dy * dy)

            _measureTooltip = $"ΔX={dx:F3}, ΔY={dy:F3}, Δ={dist:F3}"
        End If

        ' --------------------------
        ' 6) Hover detection (ALWAYS ACTIVE)

        Dim hoverDist As Single
        Dim newHover As CadEntity = HitTestCadEntity(e.Location, HitTolerancePx, hoverDist)

        If Not Object.ReferenceEquals(newHover, _hoverEntity) Then

            _hoverEntity = newHover

            If _hoverEntity IsNot Nothing Then
                _modelerToolTip.SetToolTip(pnlPicmodeler, _hoverEntity.ToString())
            Else
                _modelerToolTip.Hide(pnlPicmodeler)
            End If

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

    Private Function DistancePointToLineWorld(
    p As PointF,
    a As PointF,
    b As PointF
) As Single

        Dim dx = b.X - a.X
        Dim dy = b.Y - a.Y

        If dx = 0 AndAlso dy = 0 Then
            Return Distance(p, a)
        End If

        Dim t = ((p.X - a.X) * dx + (p.Y - a.Y) * dy) / (dx * dx + dy * dy)
        t = Math.Max(0.0F, Math.Min(1.0F, t))

        Dim proj As New PointF(
        a.X + t * dx,
        a.Y + t * dy
    )

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

        Debug.Print($"Screen Click: {e.Location.X}, {e.Location.Y}")
        Debug.Print($"Panel Height Used: {pnlPicmodeler.ClientSize.Height}")
        Debug.Print($"OffsetY: {myTransform.OffsetY}, Scale: {myTransform.Scale}")
        '' Trigger redraw to show selection
        'pnlPicmodeler.Invalidate()
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

    Private Sub HandleEntitySelected(ent As CadEntity)

        If ent Is Nothing Then Exit Sub

        ' Placeholder for selection behavior
        ' Examples (future):
        '   PropertyGrid.SelectedObject = ent
        '   Show context menu
        '   Update status bar
        '   Highlight entity

    End Sub

    ' Convert machine coordinates to screen coordinates
    Public Function WorldToScreen(x As Double, y As Double, vt As ViewTransform, panelHeight As Integer) As PointF
        Dim sx As Single = CSng(x * vt.Scale + vt.OffsetX)
        Dim sy As Single = CSng(panelHeight - (y * vt.Scale + vt.OffsetY))
        Return New PointF(sx, sy)
    End Function


    Private Sub ViewFull()
        SaveViewIfChanged()

        Dim margin As Integer = 20
        matWidth = GetMaterialWidth(MaterialSheets, _SelectedMaterialID)
        matLength = GetMaterialLength(MaterialSheets, _SelectedMaterialID)

        Dim scaleX As Double = (panelWidth - 2 * margin) / matLength
        Dim scaleY As Double = (panelHeight - 2 * margin) / matWidth
        Dim scale As Double = Math.Min(scaleX, scaleY)


        Dim offsetX As Double = (panelWidth - matLength * scale) / 2
        Dim offsetY As Double = panelHeight - (matWidth * scale) - margin


        myTransform.Scale = scale
        myTransform.OffsetX = offsetX
        myTransform.OffsetY = offsetY

        RedrawAll()
    End Sub

    Private Sub ViewPrevious()
        If _viewHistory.Count = 0 Then Return

        ' Save the current view to redo stack
        _redoHistory.Push(myTransform)

        ' Pop the last view from history
        myTransform = _viewHistory.Pop()

        ' Redraw everything
        RedrawAll()
    End Sub


    Private Sub ViewNext()
        If _redoHistory.Count = 0 Then Return

        ' Save current view to history
        _viewHistory.Push(myTransform)

        ' Pop from redo stack
        myTransform = _redoHistory.Pop()

        ' Redraw everything
        RedrawAll()
    End Sub
    Private Function IsSelectionAllowedForCurrentTool() As Boolean

        If Not _selectionEnabled Then Return False

        Debug.Print("CurrentTool = [" & _currentTool & "]")

        Select Case _currentTool

            Case "", "Select", "Selection", "Pointer", "None"
                Return True

            Case "Measure",
             "ViewWindow",
             "DrawLine",
             "DrawArc",
             "Edit",
             "Trim",
             "Move"
                Return False

            Case Else
                ' Default permissive behavior
                Return True

        End Select

    End Function

    Private Sub pnlPicmodeler_MouseDown(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseDown

        Debug.Print("MouseDown - SelectionEnabled = " & _selectionEnabled.ToString())

        ' -----------------------------
        ' 1) PANNING
        ' -----------------------------
        If e.Button = MouseButtons.Left OrElse e.Button = MouseButtons.Right Then
            If (Control.ModifierKeys And Keys.Control) = Keys.Control AndAlso
           (Control.ModifierKeys And Keys.Shift) = Keys.Shift Then

                _windowSelecting = False
                isPanning = True
                SaveViewIfChanged()

                panStartMouse = e.Location
                panStartOffsetX = myTransform.OffsetX
                panStartOffsetY = myTransform.OffsetY

                lastMousePos = e.Location
                Return
            End If
        End If

        ' -----------------------------
        ' 2) MEASURE TOOL
        ' -----------------------------
        If _currentTool = "Measure" AndAlso e.Button = MouseButtons.Left Then

            _isMeasuring = True
            _windowSelecting = False

            _measureStartWorld =
            If(_activeSnap.HasValue,
               _activeSnap.Value.WorldPoint,
               ScreenToWorld(e.Location, myTransform, pnlPicmodeler.ClientSize.Height))

            _measureEndWorld = _measureStartWorld

            pnlPicmodeler.Invalidate()
            Return
        End If

        ' -----------------------------
        ' 3) VIEW WINDOW
        ' -----------------------------
        If _currentTool = "ViewWindow" AndAlso e.Button = MouseButtons.Left Then

            _windowSelecting = True
            _windowStart = e.Location
            _windowEnd = e.Location

            pnlPicmodeler.Invalidate()
            Return
        End If


        'vb
        ' -----------------------------
        ' 4) SELECTION
        ' -----------------------------
        If Not _selectionEnabled Then Return
        If e.Button = MouseButtons.Right Then Return

        Debug.Print("Selection Mode = " & _selectionMode.ToString())
        Debug.Print("Current Tool = [" & _currentTool & "]")

        'Dim selectionAllowed As Boolean = IsSelectionAllowedForCurrentTool()

        If Not _selectionEnabled Then
            Debug.Print("SELECTION BLOCKED: _selectionEnabled = False")
            Return
        End If

        If e.Button = MouseButtons.Right Then
            Debug.Print("SELECTION BLOCKED: Right mouse button")
            Return
        End If

        Dim selectionAllowed As Boolean = IsSelectionAllowedForCurrentTool()

        Debug.Print("Current Tool = [" & _currentTool & "]")
        Debug.Print("IsSelectionAllowedForCurrentTool = " & selectionAllowed.ToString())

        If Not selectionAllowed Then
            Debug.Print("SELECTION BLOCKED: Current tool does not allow selection")
            Return
        End If



        Debug.Print("Selection Allowed = " & selectionAllowed.ToString())

        If Not selectionAllowed Then
            Debug.Print("SELECTION BLOCKED")
            Return
        End If

        Dim worldPt = ScreenToWorld(
    e.Location,
    myTransform,
    pnlPicmodeler.ClientSize.Height)

        Dim scale = Math.Max(myTransform.Scale, 0.0001F)
        Dim tolWorld As Single = HitTolerancePx / scale

        Const minTolWorld As Single = 0.001F
        tolWorld = Math.Max(tolWorld, minTolWorld)

        Debug.Print("Calling SelectAtPoint...")
        Debug.Print("WorldPt = " & worldPt.ToString())
        Debug.Print("TolWorld = " & tolWorld.ToString())

        Dim hit = _selectionManager.SelectAtPoint(worldPt, tolWorld, SelectionMode)

        If hit IsNot Nothing Then
            Debug.Print("SELECTED: " & hit.GetType().Name)
            Debug.Print("IsSelected = " & hit.IsSelected.ToString())
        Else
            Debug.Print("NO HIT FROM SELECTION MANAGER")
        End If

        pnlPicmodeler.Invalidate()


    End Sub

    Private Sub pnlPicmodeler_MouseUp(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseUp
        Dim PopUpPoint As Point

        If isPanning Then
            isPanning = False
            pnlPicmodeler.Cursor = Cursors.Default
            Return
        End If

        If e.Button = Windows.Forms.MouseButtons.Right Then
            PopUpPoint.X = System.Windows.Forms.Cursor.Position.X

            PopUpPoint.Y = System.Windows.Forms.Cursor.Position.Y

            'super incattation of right mose click for popups
            mnuPopUp3.ShowPopup(PopUpPoint)
        End If

        ' End pan

        If _isMeasuring Then
            _isMeasuring = False
            pnlPicmodeler.Invalidate()
            Return
        End If

        If _windowSelecting Then
            _windowSelecting = False
            ApplyViewWindow(_windowStart, _windowEnd)
            pnlPicmodeler.Invalidate()
            Return
        End If

    End Sub

    Private Sub ZoomToRectangle(rect As Rectangle)
        If _cadEntities Is Nothing OrElse _cadEntities.Count = 0 Then Return
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

        RedrawAll()
    End Sub

    Private Function ScreenToModel(pt As Point) As DxfPoint
        Dim safeScale As Single = If(_scale = 0, 1.0F, _scale)
        Dim modelX As Double = _minX + (pt.X - _offsetX) / safeScale
        Dim modelY As Double = _minY + (pnlPicmodeler.ClientSize.Height - pt.Y - _offsetY) / safeScale
        Return New DxfPoint(modelX, modelY, 0)
    End Function

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
        _windowSelecting = False

        _currentTool = "Measure"
    End Sub

    Public Function AllowPan(Button As Integer, Shift As Integer) As Boolean

        _windowSelecting = False

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


    Private Sub tbViewRefresh_Click(sender As Object, e As EventArgs) Handles tbViewRefresh.Click
        ViewRefresh()
    End Sub

    Private Sub pnlPicmodeler_MouseWheel(sender As Object, e As MouseEventArgs) Handles pnlPicmodeler.MouseWheel

        ' Ctrl + Shift required
      
        If (ModifierKeys And Keys.Control) = 0 OrElse
       (ModifierKeys And Keys.Shift) = 0 Then Exit Sub

        Dim zoomFactor As Double =
        If(e.Delta > 0, 1.1, 1 / 1.1)

        ' Save the current view before changing
        SaveCurrentView()

        ZoomAtMouse(e.Location, zoomFactor)

    End Sub

    Public Sub ViewPart()
        If _cadEntities Is Nothing OrElse _cadEntities.Count = 0 Then Exit Sub

        SaveViewIfChanged()

        Dim bounds As RectangleF = HelperFunctions.GetCadBounds(_cadEntities)
        If bounds.IsEmpty Then Exit Sub

        Dim margin As Integer = 20

        Dim scaleX As Double = (panelWidth - 2 * margin) / bounds.Width
        Dim scaleY As Double = (panelHeight - 2 * margin) / bounds.Height
        Dim scale As Double = Math.Min(scaleX, scaleY)

        Dim offsetX As Double = margin - bounds.X * scale
        Dim offsetY As Double = panelHeight - (bounds.Y + bounds.Height) * scale - margin

        myTransform.Scale = scale
        myTransform.OffsetX = offsetX
        myTransform.OffsetY = offsetY

        RedrawAll()
    End Sub




    Private Sub ZoomAtMouse(mousePt As Point, zoomFactor As Double)

        Dim oldScale As Double = myTransform.Scale
        Dim newScale As Double = oldScale * zoomFactor

        ' Clamp scale if you want
        If newScale < 0.01 OrElse newScale > 1000 Then Exit Sub

        ' Get world point under mouse (using OLD transform)
        Dim worldPt As PointF =
        ScreenToWorld(mousePt, myTransform, pnlPicmodeler.ClientSize.Height)

        ' Apply new scale
        myTransform.Scale = newScale

        ' Recompute offsets so mouse stays fixed
        myTransform.OffsetX = mousePt.X - worldPt.X * newScale
        myTransform.OffsetY =
        pnlPicmodeler.ClientSize.Height - mousePt.Y - worldPt.Y * newScale

        RedrawAll()

    End Sub




    Private Sub Zoom(factor As Double)
        Dim cx As Double = panelWidth / 2
        Dim cy As Double = panelHeight / 2

        myTransform.OffsetX = cx + (myTransform.OffsetX - cx) * factor
        myTransform.OffsetY = cy + (myTransform.OffsetY - cy) * factor
        myTransform.Scale *= factor

        RedrawAll()
    End Sub

    Private Sub ZoomIn()
        SaveViewIfChanged()
        Zoom(1.25)
    End Sub

    Private Sub ZoomOut()
        SaveViewIfChanged()
        Zoom(0.8)
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
        'ZoomIn(center)
        ZoomIn()
    End Sub

    Private Sub tbViewZoomOut_Click(sender As Object, e As EventArgs) Handles tbViewZoomOut.Click
        Dim center As New Point(pnlPicmodeler.ClientSize.Width \ 2, pnlPicmodeler.ClientSize.Height \ 2)
        ' ZoomOut(center)
        ZoomOut()


    End Sub

    Private Sub pnlPicmodeler_DoubleClick(sender As Object, e As EventArgs) Handles pnlPicmodeler.DoubleClick

        'ZoomIn(MousePosition)

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


    '    Private Function ScreenToWorld(
    '    pt As Point,
    '    vt As ViewTransform,
    '    panelHeight As Integer
    ') As PointF

    '        Dim worldX As Single =
    '        CSng((pt.X - vt.OffsetX) / vt.Scale)

    '        Dim worldY As Single =
    '        CSng((panelHeight - pt.Y - vt.OffsetY) / vt.Scale)

    '        Return New PointF(worldX, worldY)

    '    End Function
    Private Function ScreenToWorld(pt As PointF, vt As ViewTransform, panelHeight As Integer) As PointF

        Dim worldX As Single =
        CSng((pt.X - vt.OffsetX) / vt.Scale)

        Dim worldY As Single =
        CSng((panelHeight - pt.Y - vt.OffsetY) / vt.Scale)

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

    Private Sub frmMain2_KeyDown(sender As Object, e As KeyEventArgs) Handles Me.KeyDown
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
    Private Sub SetupPanelHandlers()
        ' Redraw when panel is created
        AddHandler pnlPicmodeler.HandleCreated, Sub() RedrawAll()

        ' Redraw on panel resize
        AddHandler pnlPicmodeler.Resize, Sub() RedrawAll()

        ' Optional: force redraw when invalidated (e.g., uncovered)
        AddHandler pnlPicmodeler.Invalidated, Sub() RedrawAll()
    End Sub

    Private Sub bbViewFull_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbViewFull.ItemClick
        ViewFull()

    End Sub

    Public Sub FitToPart()
        If _cadEntities Is Nothing OrElse _cadEntities.Count = 0 Then Exit Sub

        Dim bounds As RectangleF = HelperFunctions.GetCadBounds(_cadEntities)
        If bounds.IsEmpty Then Exit Sub

        Dim margin As Integer = 20

        Dim scaleX = (panelWidth - 2 * margin) / bounds.Width
        Dim scaleY = (panelHeight - 2 * margin) / bounds.Height
        Dim scale = Math.Min(scaleX, scaleY)

        Dim offsetX = panelWidth / 2 - (bounds.X + bounds.Width / 2) * scale
        Dim offsetY = panelHeight / 2 - (bounds.Y + bounds.Height / 2) * scale

        myTransform = New ViewTransform With {
            .Scale = scale,
            .OffsetX = offsetX,
            .OffsetY = offsetY
        }

        RedrawAll()
    End Sub
    Private Sub ApplyViewWindow(p1 As Point, p2 As Point)

        ' Convert screen points → world points
        Dim w1 = ScreenToWorld(p1, myTransform, panelHeight)
        Dim w2 = ScreenToWorld(p2, myTransform, panelHeight)

        ' Normalize world rectangle
        Dim minX = Math.Min(w1.X, w2.X)
        Dim maxX = Math.Max(w1.X, w2.X)
        Dim minY = Math.Min(w1.Y, w2.Y)
        Dim maxY = Math.Max(w1.Y, w2.Y)

        Dim worldWidth = maxX - minX
        Dim worldHeight = maxY - minY

        If worldWidth <= 0 OrElse worldHeight <= 0 Then Exit Sub

        ' Compute new scale
        Dim margin As Integer = 20
        Dim scaleX = (panelWidth - 2 * margin) / worldWidth
        Dim scaleY = (panelHeight - 2 * margin) / worldHeight
        Dim newScale = Math.Min(scaleX, scaleY)

        ' Compute new offsets
        Dim newOffsetX = margin - minX * newScale
        Dim newOffsetY = margin - minY * newScale

        myTransform = New ViewTransform With {
            .Scale = newScale,
            .OffsetX = newOffsetX,
            .OffsetY = newOffsetY
        }

        pnlPicmodeler.Invalidate()
    End Sub

    Private Sub bbViewWindow_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbViewWindow.ItemClick
        _currentTool = "ViewWindow"
    End Sub

    Private Sub SaveCurrentView()
        ' Push a copy of the current transform onto the history stack
        _viewHistory.Push(myTransform)

        ' Clear redo stack when a new view is saved
        _redoHistory.Clear()
    End Sub


    Private Function AreTransformsEqual(a As ViewTransform, b As ViewTransform) As Boolean
        Const epsilon As Double = 0.0001
        Return Math.Abs(a.Scale - b.Scale) < epsilon AndAlso
           Math.Abs(a.OffsetX - b.OffsetX) < epsilon AndAlso
           Math.Abs(a.OffsetY - b.OffsetY) < epsilon
    End Function

    Private Function CloneViewTransform(vt As ViewTransform) As ViewTransform
        Return New ViewTransform With {
        .Scale = vt.Scale,
        .OffsetX = vt.OffsetX,
        .OffsetY = vt.OffsetY
    }
    End Function

    Private Sub SaveViewIfChanged()
        ' Compare the current transform with the last saved one
        If _viewHistory.Count = 0 OrElse Not AreTransformsEqual(myTransform, _viewHistory.Peek()) Then
            SaveCurrentView()
        End If
    End Sub

    Private Sub DrawOverlayText(g As Graphics, text As String, anchor As PointF)

        Using f As New Font("Segoe UI", 9),
          bg As New SolidBrush(System.Drawing.Color.FromArgb(200, System.Drawing.Color.LightYellow)),
          fg As New SolidBrush(System.Drawing.Color.Black)

            Dim size = g.MeasureString(text, f)
            Dim r As New RectangleF(anchor.X + 8, anchor.Y + 8, size.Width + 6, size.Height + 4)

            g.FillRectangle(bg, r)
            g.DrawRectangle(Pens.Black, Rectangle.Round(r))
            g.DrawString(text, f, fg, r.Left + 3, r.Top + 2)
        End Using

    End Sub

    Private Sub DrawSnapIndicator(g As Graphics, snap As SnapResult)

        Dim p = WorldToScreen(
        snap.WorldPoint.X,
        snap.WorldPoint.Y,
        myTransform,
        pnlPicmodeler.ClientSize.Height)

        Const s As Integer = 6

        Using b As New SolidBrush(System.Drawing.Color.Lime)
            g.FillRectangle(b, p.X - s \ 2, p.Y - s \ 2, s, s)
        End Using

    End Sub

    Private Sub CancelMeasure()
        _isMeasuring = False

        pnlPicmodeler.Invalidate()
    End Sub
    Private Function GetActiveSnap(
    mouseScreen As Point,
    tolScreenPx As Single
) As SnapResult?

        Dim mouseWorld = ScreenToWorld(mouseScreen, myTransform, pnlPicmodeler.ClientSize.Height)
        Dim tolWorld = tolScreenPx / myTransform.Scale

        Dim best As SnapResult
        Dim bestDist As Single = Single.MaxValue
        Dim found As Boolean = False

        For Each ent In _cadEntities
            For Each cand In GetSnapCandidates(ent)

                Dim d = Distance(mouseWorld, cand.WorldPoint)
                If d <= tolWorld AndAlso d < bestDist Then
                    cand.Distance = d
                    best = cand
                    bestDist = d
                    found = True
                End If

            Next
        Next

        If found Then
            Return best
        Else
            Return Nothing
        End If

    End Function
    '    Private Function TryEndpointSnap(
    '    mouseWorld As PointF,
    '    tolWorld As Single
    ') As SnapResult

    '        Dim best As SnapResult
    '        best.Type = SnapType.None
    '        best.Distance = Single.MaxValue

    '        For Each ent In _cadEntities
    '            If TypeOf ent Is CadLine Then
    '                Dim l = DirectCast(ent, CadLine)

    '                CheckPoint(l.StartPoint, ent)
    '                CheckPoint(l.EndPoint, ent)
    '            End If
    '        Next

    '        Return best
    '    End Function



    Private Function TryMidpointSnap(mouseWorld As PointF, tolWorld As Single) As SnapResult
        For Each ent In _cadEntities
            If TypeOf ent Is CadLine Then
                Dim l = DirectCast(ent, CadLine)
                Dim mid As New PointF(
                (l.StartPoint.X + l.EndPoint.X) / 2.0F,
                (l.StartPoint.Y + l.EndPoint.Y) / 2.0F)

                If Distance(mid, mouseWorld) < tolWorld Then
                    Return New SnapResult With {
                    .Type = SnapType.Midpoint,
                    .WorldPoint = mid,
                    .Entity = ent
                }
                End If
            End If
        Next

        Return Nothing
    End Function

    Private Function TryCenterSnap(mouseWorld As PointF, tolWorld As Single) As SnapResult

        For Each ent In _cadEntities
            If TypeOf ent Is CadCircle Then
                Dim c = DirectCast(ent, CadCircle)
                If Distance(c.Center, mouseWorld) < tolWorld Then
                    Return New SnapResult With {
                    .Type = SnapType.Center,
                    .WorldPoint = c.Center,
                    .Entity = ent
                }
                End If
            End If

            If TypeOf ent Is CadArc Then
                Dim a = DirectCast(ent, CadArc)
                If Distance(a.Center, mouseWorld) < tolWorld Then
                    Return New SnapResult With {
                    .Type = SnapType.Center,
                    .WorldPoint = a.Center,
                    .Entity = ent
                }
                End If
            End If
        Next

        Return Nothing
    End Function

    Private Sub tbSelect_HotDot_Click(sender As Object, e As EventArgs) Handles tbSelect_HotDot.Click
        _hotDotsEnabled = tbSelect_HotDot.Checked
        pnlPicmodeler.Invalidate() ' redraw panel to reflect change
    End Sub

    Private Function GetSnapCandidates(ent As CadEntity) As IEnumerable(Of SnapResult)

        Dim list As New List(Of SnapResult)

        Select Case True

            Case TypeOf ent Is CadLine
                Dim l = DirectCast(ent, CadLine)

                list.Add(New SnapResult With {
                .Type = SnapType.Endpoint,
                .WorldPoint = l.StartPoint,
                .Entity = ent
            })

                list.Add(New SnapResult With {
                .Type = SnapType.Endpoint,
                .WorldPoint = l.EndPoint,
                .Entity = ent
            })

                list.Add(New SnapResult With {
                .Type = SnapType.Midpoint,
                .WorldPoint = New PointF(
                    (l.StartPoint.X + l.EndPoint.X) / 2.0F,
                    (l.StartPoint.Y + l.EndPoint.Y) / 2.0F),
                .Entity = ent
            })

            Case TypeOf ent Is CadCircle
                Dim c = DirectCast(ent, CadCircle)

                list.Add(New SnapResult With {
                .Type = SnapType.Center,
                .WorldPoint = c.Center,
                .Entity = ent
            })

            Case TypeOf ent Is CadArc
                Dim a = DirectCast(ent, CadArc)

                list.Add(New SnapResult With {
                .Type = SnapType.Center,
                .WorldPoint = a.Center,
                .Entity = ent
            })
        End Select

        Return list
    End Function

    Private Sub DrawMeasureLine(startPt As PointF, endPt As PointF, g As Graphics)
        ' Convert world → screen
        Dim p1 = WorldToScreen(startPt.X, startPt.Y, myTransform, pnlPicmodeler.ClientSize.Height)
        Dim p2 = WorldToScreen(endPt.X, endPt.Y, myTransform, pnlPicmodeler.ClientSize.Height)

        Using pen As New Pen(System.Drawing.Color.Red, 1)
            g.DrawLine(pen, p1, p2)
        End Using
    End Sub

    Private Sub DrawRubberBand(startPt As Point, endPt As Point, g As Graphics)
        Using pen As New Pen(System.Drawing.Color.Blue, 1)
            pen.DashStyle = Drawing2D.DashStyle.Dash
            g.DrawRectangle(pen, Math.Min(startPt.X, endPt.X),
                             Math.Min(startPt.Y, endPt.Y),
                             Math.Abs(endPt.X - startPt.X),
                             Math.Abs(endPt.Y - startPt.Y))
        End Using
    End Sub

    Private Sub bbZoomToPart_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbZoomToPart.ItemClick
        ViewPart()

    End Sub

    Private Sub iExit_ItemClick(sender As Object, e As ItemClickEventArgs) Handles iExit.ItemClick
        End
    End Sub


    Private Function GetHoverDrawable() As DxfDrawable

        If _hoverEntity Is Nothing Then Return Nothing

        Return _hoverEntity.drawable

    End Function

    Private Sub tbSelect_Enable_Click(sender As Object, e As EventArgs) Handles tbSelect_Enable.Click

        _selectionEnabled = tbSelect_Enable.Checked

        If Not _selectionEnabled Then
            _selectionMode = Nothing
        End If

        Debug.Print("Selection Enabled: " & _selectionEnabled.ToString())
        pnlPicmodeler.Invalidate()
    End Sub

    Private Sub tbSelect_Lines_Click(sender As Object, e As EventArgs) Handles tbSelect_Lines.Click
        _selectionEnabled = True
        _selectionMode = GeometryTypeEnum.Line

        Debug.Print("Selection Mode = LINE")
    End Sub

    Private Sub tbSelect_Arcs_Click(sender As Object, e As EventArgs) Handles tbSelect_Arcs.Click
        _selectionEnabled = True
        _selectionMode = GeometryTypeEnum.Arc

        Debug.Print("Selection Mode = CIRCLE")
    End Sub

End Class



