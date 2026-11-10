Imports System.Data.OleDb
Imports System.Drawing.Drawing2D
Imports System.IO
Imports System.Reflection
Imports DevExpress.Utils
Imports DevExpress.XtraBars
Imports DevExpress.XtraBars.Helpers
Imports DevExpress.XtraBars.Ribbon
Imports DevExpress.XtraTreeList
Imports DevExpress.XtraVerticalGrid.Events
Imports FabV25_WIN8.AppData
Imports FabV25_WIN8.CadEntity
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Helpers
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Services
' IxMilia DXF imports
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports Newtonsoft.Json
Imports Formatting = Newtonsoft.Json.Formatting


Public Class frmMain
    Inherits RibbonForm

    Private _materialDisplayColor As Color = System.Drawing.Color.LightGray

    ' Application lifecycle
    Private _uiInitialized As Boolean = False

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
    Private _updatingEntityListSelection As Boolean = False
    Private _selectionManager As New SelectionManager()
    Private _selectionEnabled As Boolean = False
    'Bitwise selection-filter state.
    Public _selectionFilter As Long

    ' Hot dot display toggle
    Private _hotSpotEnabled As Boolean = False


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


    ' View history stacks
    Private _viewHistory As New Stack(Of ViewTransform)
    Private _redoHistory As New Stack(Of ViewTransform)

    Private Const MaxHistorySize As Integer = 20

    Private _isRestoringView As Boolean = False ' to prevent pushing when restoring
    Private _windowSelecting As Boolean = False
    Private _windowStart As Point
    Private _windowEnd As Point

    Private _selectWindowActive As Boolean = False
    Private _selectWindowStart As Point
    Private _selectWindowEnd As Point


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
    Private WithEvents entityTip As New System.Windows.Forms.ToolTip()
    Private lastHoverEntity As CadEntity = Nothing

    Private _hoverMouseWorld As PointF
    Private _modelerToolTip As New System.Windows.Forms.ToolTip() With {
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
    Private _profileSelectionEnabled As Boolean = False

    Private _entityPropertiesForm As frmEntityProperties


    Private _holeParameterValues As New Dictionary(Of String, HoleParameterValue)(StringComparer.OrdinalIgnoreCase)
    Private _currentHoleArrangement As HoleArrangementDefinition = Nothing

    Private _holeArrangementDefinitions As HoleArrangementDefinitions


    Private _updatingHoleGrid As Boolean = False

    Private Declare Function LockWindowUpdate Lib "user32.dll" (ByVal hWndLock As IntPtr) As Boolean

    Public Sub New()
        Me.Location = Screen.AllScreens(UBound(Screen.AllScreens)).Bounds.Location
        'App.Title = "WE-CIM"
        Me.Text = "WE-CIM Fabrication Software"

        InitSkins()

        InitializeComponent()


        Me.InitSkinGallery()

        AddHandler _selectionManager.SelectionChanged, AddressOf OnSelectionChanged
        AddHandler _selectionManager.HoverChanged, AddressOf OnHoverChanged

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

    Private Sub OnSelectionChanged(
    selectedItems As IEnumerable(Of isSelectable))

        ' Update the selection count.
        statusSelection.Caption = "Selection: " & GetEffectiveSelectionCount(selectedItems).ToString()


        ' Refresh the drawing.
        pnlPicmodeler.Invalidate()

        SyncEntityListSelection()
    End Sub


    Private Sub rgbiSkins_GalleryInitDropDownGallery(ByVal sender As Object, ByVal e As DevExpress.XtraBars.Ribbon.InplaceGalleryEventArgs) Handles rgbiSkins.GalleryInitDropDownGallery

    End Sub


    Private Sub frmMain_Load(sender As Object, e As EventArgs) Handles Me.Load

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

            Dim savedMaterialColor As Integer = CInt(RegGetString("Preferences\ViewOptions", "MaterialColor", _materialDisplayColor.ToArgb()))

            _materialDisplayColor = System.Drawing.Color.FromArgb(savedMaterialColor)
            bbiMaterialColor.EditValue = _materialDisplayColor
            ConfigurationManagerForm.MaterialDisplayColor = _materialDisplayColor

            Logging.Log("AppState loaded successfully.")

        Catch ex As Exception
            MessageBox.Show($"Error loading application data: {ex.Message}", "Startup Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Logging.Log($"Startup error: {ex.ToString()}")
        End Try

    End Sub

    Private Sub bbConfigMan_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbConfigMan.ItemClick
        ConfigurationManagerForm.MaterialDisplayColor = _materialDisplayColor
        ConfigurationManagerForm.Show(Me)
    End Sub
    Private Sub iOpen_ItemClick(sender As Object, e As ItemClickEventArgs) Handles iOpen.ItemClick

        FileOpen()


    End Sub



    Private Sub frmMain_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        Me.Dispose()

    End Sub

    Private Sub frmMain_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
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

            ' Prepare Tools
            Dim toolCribList As New List(Of Tool)()

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

                ' Create Tool
                Dim tool As New Tool With {
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

        '    Debug.WriteLine(
        '"PAINT: DoPrepareUI=" & _DoPrepareUI.ToString() &
        '" Scale=" & myTransform.Scale.ToString() &
        '" OffsetX=" & myTransform.OffsetX.ToString() &
        '" OffsetY=" & myTransform.OffsetY.ToString())

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

        ' --------------------------------------------
        ' 3) Draw base scene
        ' --------------------------------------------
        RenderScene(e.Graphics) ' Draw sheets, workzones, clamps, entities, etc.

        ' --------------------------------------------
        ' 4) Draw overlays in proper order
        ' --------------------------------------------

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
        'Debug.WriteLine("DRAW OVERLAY CALLED")

        ' --------------------------
        ' 1) Hot-Dot
        ' --------------------------
        If _activeSnap.HasValue AndAlso _hotSpotEnabled Then
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

        ' 4) Hover direction arrow
        ' --------------------------
        DrawHoverLineArrow(g)

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
        DrawSheetAndWorkzones(g, matLength, matWidth)
        DrawProductionGeometry(g, myTransform, panelHeight)

        ' -------------------------------------------------
        ' 2) HOVER HIGHLIGHT (still View-owned for now)
        ' -------------------------------------------------
        'If _hoverEntity IsNot Nothing Then
        '    DrawEntityHighlight(g, _hoverEntity)
        'End If

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

                            Dim sweep As Single = CSng(arc.EndAngle - arc.StartAngle)

                            If sweep < 0 Then
                                sweep += 360
                            End If

                            Dim startAngle As Single = CSng(-arc.StartAngle)
                            Dim sweepAngle As Single = CSng(-sweep)

                            g.DrawArc(pen,
                                            CSng(center.X - r),
                                            CSng(center.Y - r),
                                            CSng(r * 2),
                                            CSng(r * 2),
                                            startAngle,
                                            sweepAngle)

                        Case GeometryTypeEnum.Point

                            Dim point As CadPoint = TryCast(ent, CadPoint)
                            If point Is Nothing Then Continue For

                            Dim screenPt As PointF =
                                WorldToScreen(
                                    point.Position.X,
                                    point.Position.Y,
                                    myTransform,
                                    panelHeight)

                            Const pointSize As Single = 8.0F

                            g.DrawRectangle(
                                pen,
                                screenPt.X - pointSize / 2.0F,
                                screenPt.Y - pointSize / 2.0F,
                                pointSize,
                                pointSize)

                    End Select

                Next

            End Using

        End If

        ' Select By Window rubber-band
        If _selectWindowActive AndAlso
           _currentTool = "SelectWindow" AndAlso
           _selectWindowStart <> System.Drawing.Point.Empty Then

            DrawRubberBand(
                _selectWindowStart,
                _selectWindowEnd,
                g)

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

            Case CadEntity.EntityType.Line

                Dim l = DirectCast(ent, CadLine)

                Dim p1 = WorldToScreen(
                l.StartPoint.X,
                l.StartPoint.Y,
                myTransform,
                pnlPicmodeler.Height)

                Dim p2 = WorldToScreen(
                l.EndPoint.X,
                l.EndPoint.Y,
                myTransform,
                pnlPicmodeler.Height)

                g.DrawLine(pen, p1, p2)


            Case CadEntity.EntityType.Circle

                Dim c = DirectCast(ent, CadCircle)

                Dim center = WorldToScreen(
                c.Center.X,
                c.Center.Y,
                myTransform,
                pnlPicmodeler.Height)

                Dim r = CSng(c.Radius * myTransform.Scale)

                g.DrawEllipse(
                pen,
                center.X - r,
                center.Y - r,
                2 * r,
                2 * r)



            Case CadEntity.EntityType.Arc

                Dim a = DirectCast(ent, CadArc)

                Dim center = WorldToScreen(
        a.Center.X,
        a.Center.Y,
        myTransform,
        pnlPicmodeler.Height)

                Dim r = CSng(a.Radius * myTransform.Scale)

                Dim sweep As Single

                ' World-space sweep
                If a.IsClockwise Then

                    sweep = a.StartAngle - a.EndAngle

                    If sweep < 0 Then
                        sweep += 360
                    End If

                Else

                    sweep = a.EndAngle - a.StartAngle

                    If sweep < 0 Then
                        sweep += 360
                    End If

                End If

                Dim startDeg As Single =
        -a.StartAngle

                Dim sweepDeg As Single

                ' World Y+ is up, screen Y+ is down.
                If a.IsClockwise Then
                    sweepDeg = sweep
                Else
                    sweepDeg = -sweep
                End If

                g.DrawArc(
        pen,
        center.X - r,
        center.Y - r,
        2 * r,
        2 * r,
        startDeg,
        sweepDeg)


            Case CadEntity.EntityType.Point

                Dim p = DirectCast(ent, CadPoint)

                Dim screenPt =
                    WorldToScreen(
                        p.Position.X,
                        p.Position.Y,
                        myTransform,
                        pnlPicmodeler.Height)

                Const pointSize As Single = 8.0F

                g.DrawRectangle(
                    pen,
                    screenPt.X - pointSize / 2.0F,
                    screenPt.Y - pointSize / 2.0F,
                    pointSize,
                    pointSize)
        End Select

    End Sub


    Private Sub DrawGeoEntities(
    entities As IEnumerable(Of GeoEntity),
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If entities Is Nothing Then Return

        Dim layer As Layer


        For Each entity As GeoEntity In entities

            If entity Is Nothing Then Continue For

            layer = TryCast(entity.Association, Layer)

            If layer IsNot Nothing AndAlso Not layer.Visible Then
                Continue For
            End If


            If TypeOf entity Is GeoLine Then
                DrawGeoLine(DirectCast(entity, GeoLine), g, vt, panelHeight)

            ElseIf TypeOf entity Is GeoArc Then
                DrawGeoArc(DirectCast(entity, GeoArc), g, vt, panelHeight)

            ElseIf TypeOf entity Is GeoCircle Then
                DrawGeoCircle(DirectCast(entity, GeoCircle), g, vt, panelHeight)

            ElseIf TypeOf entity Is GeoPoint Then
                DrawGeoPoint(DirectCast(entity, GeoPoint), g, vt, panelHeight)

            End If

        Next

    End Sub


    Private Sub DrawGeoLine(
    l As GeoLine,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If l Is Nothing OrElse
       l.StartPoint Is Nothing OrElse
       l.EndPoint Is Nothing Then
            Exit Sub
        End If

        Dim p1 = WorldToScreen(
        l.StartPoint.X,
        l.StartPoint.Y,
        vt,
        panelHeight)

        Dim p2 = WorldToScreen(
        l.EndPoint.X,
        l.EndPoint.Y,
        vt,
        panelHeight)

        Dim drawColor As System.Drawing.Color = l.Color
        Dim drawWidth As Single = 1.0F
        Dim drawDashed As Boolean = False

        If l.Selected OrElse IsEntityInSelectedProfile(l) Then
            Debug.WriteLine(
    $"DrawGeoLine: Selected={l.Selected}, " &
    $"ProfileSelected={IsEntityInSelectedProfile(l)}")

            drawColor = System.Drawing.Color.Red
            drawWidth = 1.0F

        ElseIf l.IsHovered OrElse IsEntityInHoveredProfile(l) Then

            drawColor = System.Drawing.Color.Red
            drawWidth = 2.0F
            drawDashed = True

        End If

        Using pen As New Pen(drawColor, drawWidth)

            If drawDashed Then
                pen.DashStyle = DashStyle.Dash
            End If

            g.DrawLine(pen, p1, p2)

        End Using

    End Sub


    Private Sub DrawGeoArc(
    a As GeoArc,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If a Is Nothing OrElse a.Center Is Nothing Then
            Exit Sub
        End If

        Dim centerScreen = WorldToScreen(
        a.Center.X,
        a.Center.Y,
        vt,
        panelHeight)

        Dim r As Single = CSng(a.Radius * vt.Scale)

        Dim p1 As New PointF(centerScreen.X - r, centerScreen.Y - r)
        Dim p2 As New PointF(centerScreen.X + r, centerScreen.Y + r)

        Dim rect As RectangleF = HelperFunctions.RectFromPoints(p1, p2)

        Dim sweep As Single

        If a.IsClockwise Then

            sweep = CSng(a.StartAngle - a.EndAngle)

            If sweep < 0 Then
                sweep += 360
            End If

        Else

            sweep = CSng(a.EndAngle - a.StartAngle)

            If sweep < 0 Then
                sweep += 360
            End If

        End If

        Dim startAngle As Single = CSng(-a.StartAngle)

        Dim sweepAngle As Single

        If a.IsClockwise Then
            sweepAngle = sweep
        Else
            sweepAngle = -sweep
        End If

        Dim displaySelected As Boolean = a.Selected OrElse IsEntityInSelectedProfile(a)
        Dim drawColor As System.Drawing.Color = a.Color
        Dim drawWidth As Single = 1.0F
        Dim drawDashed As Boolean = False

        If displaySelected Then

            drawColor = System.Drawing.Color.Red
            drawWidth = 1.0F

        ElseIf a.IsHovered OrElse IsEntityInHoveredProfile(a) Then

            drawColor = System.Drawing.Color.Red
            drawWidth = 2.0F
            drawDashed = True

        End If

        Using pen As New Pen(drawColor, drawWidth)


            If displaySelected Then

                pen.DashStyle = DashStyle.Dot

            ElseIf drawDashed Then

                pen.DashStyle = DashStyle.Dash

            End If


            '        Debug.WriteLine(
            '"DrawGeoArc: Selected=" & a.Selected.ToString() &
            '" Color=" & pen.Color.Name &
            '" DashStyle=" & pen.DashStyle.ToString())

            g.DrawArc(pen, rect, startAngle, sweepAngle)

        End Using


    End Sub


    Private Sub DrawGeoCircle(
    c As GeoCircle,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If c Is Nothing OrElse c.Center Is Nothing Then
            Exit Sub
        End If

        Dim center = WorldToScreen(
        c.Center.X,
        c.Center.Y,
        vt,
        panelHeight)

        Dim r As Single = CSng(c.Radius * vt.Scale)

        Dim p1 As New PointF(center.X - r, center.Y - r)
        Dim p2 As New PointF(center.X + r, center.Y + r)

        Dim rect = HelperFunctions.RectFromPoints(p1, p2)


        Dim displaySelected As Boolean = c.Selected OrElse IsEntityInSelectedProfile(c)


        Dim drawColor As System.Drawing.Color = c.Color
        Dim drawWidth As Single = 1.0F
        Dim drawDashed As Boolean = False

        If displaySelected Then

            drawColor = System.Drawing.Color.Red
            drawWidth = 1.0F

        ElseIf c.IsHovered OrElse IsEntityInHoveredProfile(c) Then
            drawColor = System.Drawing.Color.Red
            drawWidth = 2.0F
            drawDashed = True

        End If

        Using pen As New Pen(drawColor, drawWidth)

            If displaySelected Then

                pen.DashStyle = DashStyle.Dot

            ElseIf drawDashed Then

                pen.DashStyle = DashStyle.Dash

            End If

            g.DrawEllipse(pen, rect)

        End Using


    End Sub

    Private Sub DrawProductionGeometry(
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If CurrentProductionJob Is Nothing Then Exit Sub

        For Each sheet As JobSheet In CurrentProductionJob.JobSheets

            If sheet Is Nothing Then Continue For

            Dim mm As MM3 = sheet.ManufacturingModel

            If mm Is Nothing Then Continue For

            '---------------------------------------------
            ' Draw nominal Part Geometry from MM3
            '---------------------------------------------
            DrawGeoEntities(
            mm.PartGeometry,
            g,
            vt,
            panelHeight)

            '---------------------------------------------
            ' Draw Manufacturing Geometry from MM3
            '---------------------------------------------
            For Each feature As MachiningFeature In mm.ManufacturingGeometry

                If feature Is Nothing Then Continue For

                If TypeOf feature Is HoleFeature Then

                    DrawHoleFeature(
                        DirectCast(feature, HoleFeature),
                        g,
                        vt,
                        panelHeight)

                ElseIf TypeOf feature Is LineOfHolesFeature Then

                    DrawLineOfHolesFeature(
    DirectCast(feature, LineOfHolesFeature),
    g,
    vt,
    panelHeight,
    matLength,
    matWidth)

                End If



            Next

        Next

    End Sub



    Public Sub DrawSheetAndWorkzones(g As Graphics, sheetWidth As Double, sheetHeight As Double)
        Dim margin As Integer = 20

        ' --- Draw sheet background ---
        Dim bl As PointF = WorldToScreen(0, 0, myTransform, panelHeight)
        Dim tr As PointF = WorldToScreen(sheetWidth, sheetHeight, myTransform, panelHeight)
        Dim sheetRect = HelperFunctions.RectFromPoints(bl, tr)
        Using materialBrush As New SolidBrush(_materialDisplayColor)
            g.FillRectangle(materialBrush, sheetRect)
        End Using
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
    Public Sub BuildDrawables()


        Debug.WriteLine("ENTERED BuildDrawables")

        ' ---------------------------------
        ' IDENTIFY CAD GEOMETRY TYPES
        ' ---------------------------------
        For Each cad In _cadEntities

            If TypeOf cad Is CadLine Then

                cad.GeometryType = GeometryTypeEnum.Line

            ElseIf TypeOf cad Is CadCircle Then

                cad.GeometryType = GeometryTypeEnum.Circle

            ElseIf TypeOf cad Is CadArc Then

                cad.GeometryType = GeometryTypeEnum.Arc

            ElseIf TypeOf cad Is CadPoint Then

                cad.GeometryType = GeometryTypeEnum.Point

            Else

                cad.GeometryType = GeometryTypeEnum.Unknown

            End If

        Next

        Dim cadPointCount As Integer = 0

        For Each cad As CadEntity In _cadEntities

            If TypeOf cad Is CadPoint Then
                cadPointCount += 1
            End If

        Next

        Debug.WriteLine("CAD POINT COUNT = " & cadPointCount)


        ' ---------------------------------
        ' BUILD NATIVE GEO SPATIAL INDEX
        ' ---------------------------------

        Dim geoEntities As New List(Of GeoEntity)
        Dim geoProfiles As New List(Of GeoProfile)
        Dim machiningFeatures As New List(Of MachiningFeature)

        If CurrentProductionJob IsNot Nothing Then

            For Each jobSheet As JobSheet In CurrentProductionJob.JobSheets

                If jobSheet Is Nothing Then Continue For

                Dim mm As MM3 = jobSheet.ManufacturingModel

                If mm IsNot Nothing Then

                    geoEntities.AddRange(mm.PartGeometry)
                    machiningFeatures.AddRange(mm.ManufacturingGeometry)

                End If

                ' Profiles remain associated with their Parts.
                For Each part As Part In jobSheet.Parts

                    If part Is Nothing Then Continue For

                    geoProfiles.AddRange(part.GeoProfiles)

                Next

            Next

        End If

        _selectionManager.SetGeoEntities(geoEntities)
        _selectionManager.SetGeoProfiles(geoProfiles)
        _selectionManager.SetMachiningFeatures(machiningFeatures)
        _selectionManager.BuildGeoSpatialIndex()

        Debug.WriteLine(
    "NATIVE GEO ENTITIES REGISTERED = " &
    geoEntities.Count)


        Debug.WriteLine(
    "MACHINING FEATURES REGISTERED = " &
    machiningFeatures.Count)

        Debug.WriteLine(
            "HOLE FEATURES REGISTERED = " &
            machiningFeatures.OfType(Of HoleFeature)().Count())


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






    Private Sub DrawCadPoint(
    pt As CadPoint,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        Dim screenPt As PointF =
        WorldToScreen(
            pt.Position.X,
            pt.Position.Y,
            vt,
            panelHeight)

        Const PointSize As Single = 6.0F

        g.FillRectangle(
        Brushes.Black,
        screenPt.X - PointSize / 2.0F,
        screenPt.Y - PointSize / 2.0F,
        PointSize,
        PointSize)

    End Sub

    Private Sub DrawGeoPoint(
    pt As GeoPoint,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If pt Is Nothing Then Exit Sub

        Dim screenPt As PointF =
        WorldToScreen(
            pt.X,
            pt.Y,
            vt,
            panelHeight)

        Const PointSize As Single = 6.0F

        Dim rect As New RectangleF(
        screenPt.X - PointSize / 2.0F,
        screenPt.Y - PointSize / 2.0F,
        PointSize,
        PointSize)

        Dim drawColor As System.Drawing.Color = pt.Color
        Dim drawWidth As Single = 1.0F
        Dim drawDashed As Boolean = False
        If pt.Selected Then

            drawColor = System.Drawing.Color.Red
            drawWidth = 1.0F

        ElseIf pt.IsHovered Then

            drawColor = System.Drawing.Color.Red
            drawWidth = 2.0F
            drawDashed = True

        End If

        Using pen As New Pen(drawColor, drawWidth)

            If pt.Selected Then

                pen.DashStyle = DashStyle.Dot

            ElseIf drawDashed Then

                pen.DashStyle = DashStyle.Dash

            End If

            g.DrawRectangle(
        pen,
        rect.X,
        rect.Y,
        rect.Width,
        rect.Height)

        End Using

    End Sub

    ' Helper to draw the default marker (e.g., a "!") at a point
    Private Sub DrawDefaultMarker(g As Graphics, pt As PointF, scale As Double)
        Const markerSize As Single = 8.0F
        Using brush As New SolidBrush(System.Drawing.Color.Red)
            g.DrawString("!", New Font("Arial", markerSize), brush, pt.X - markerSize / 2, pt.Y - markerSize / 2)
        End Using
    End Sub


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
        _hoverMouseWorld = modelPt
        statusLocation.Caption = $"Location: X={modelPt.X:0.###}, Y={modelPt.Y:0.###}"

        ' 3) ViewWindow rubber-band
        ' --------------------------
        If _windowSelecting Then
            _windowEnd = e.Location
            pnlPicmodeler.Invalidate()
        End If

        ' 3a) Select By Window rubber-band
        If _selectWindowActive AndAlso
                                             _currentTool = "SelectWindow" Then

            _selectWindowEnd = e.Location
            pnlPicmodeler.Invalidate()
        End If

        ' --------------------------
        ' 4) Active Snap
        ' --------------------------

        If Not _hotSpotEnabled Then
            _activeSnap = Nothing
            _snapScreenPoint = Nothing
        Else
            Dim snapTolPx As Single = 12
            _activeSnap = GetActiveSnap(e.Location, snapTolPx)
        End If

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
        pnlPicmodeler.Invalidate()



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
        ' --------------------------
        Dim tolWorld As Single = HitTolerancePx / myTransform.Scale

        _selectionManager.UpdateHover(modelPt, tolWorld)

        ' ---------------------------------
        ' TEMPORARY NATIVE GEO HIT TEST
        ' ---------------------------------

        Dim nativeHit As Boolean =
            _selectionManager.TestGeoHitAtPoint(modelPt, tolWorld)

        If nativeHit Then
            Debug.WriteLine("NATIVE GEO HIT = TRUE")
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

        If e.Button = MouseButtons.Left AndAlso
           TryReturnGraphicPointToHoleControl(e.Location) Then

            Return
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

        ' 3a) SELECT BY WINDOW
        If _selectWindowActive AndAlso
                                           _currentTool = "SelectWindow" AndAlso
                                           e.Button = MouseButtons.Left Then

            _selectWindowStart = e.Location
            _selectWindowEnd = e.Location

            pnlPicmodeler.Invalidate()
            Return
        End If
        ' -----------------------------
        ' 4) SELECTION
        ' -----------------------------
        If Not _selectionEnabled Then
            Debug.Print("SELECTION BLOCKED: _selectionEnabled = False")
            Return
        End If

        If e.Button = MouseButtons.Right Then
            Debug.Print("SELECTION BLOCKED: Right mouse button")
            Return
        End If

        Debug.Print("Selection Mode = " & _selectionMode.ToString())
        Debug.Print("Current Tool = [" & _currentTool & "]")

        Dim selectionAllowed As Boolean = IsSelectionAllowedForCurrentTool()

        Debug.Print("IsSelectionAllowedForCurrentTool = " & selectionAllowed.ToString())

        If Not selectionAllowed Then
            Debug.Print("SELECTION BLOCKED: Current tool does not allow selection")
            Return
        End If

        Debug.Print("Selection Allowed = " & selectionAllowed.ToString())

        Dim worldPt = ScreenToWorld(
        e.Location,
        myTransform,
        pnlPicmodeler.ClientSize.Height)

        Dim scale As Single = Math.Max(CSng(myTransform.Scale), 0.0001F)

        ' Keep selection tolerance consistent with HitTestCadEntity.
        Dim tolWorld As Single = HitTolerancePx / scale
        'tolWorld = Math.Max(tolWorld, 0.1F)
        tolWorld = Math.Max(tolWorld, 0.1F)

        ' Ctrl explicitly requests deselection.
        Dim deselect As Boolean =
    (Control.ModifierKeys And Keys.Control) = Keys.Control


        ' MessageBox.Show("TEST: SelectAtPoint is about to execute")
        Debug.WriteLine("TEST: Selection Filter = " & _selectionFilter.ToString())
        Debug.WriteLine("TEST: Profile Selection = " & _profileSelectionEnabled.ToString())
        '    MessageBox.Show(
        '"Selection Filter = " & _selectionFilter.ToString() &
        'vbCrLf &
        '"Profile Selection = " & _profileSelectionEnabled.ToString(),
        '"Selection Diagnostics")
        Dim hit As isSelectable =
    _selectionManager.SelectAtPoint(
        worldPt,
        tolWorld,
        _selectionMode,
        _selectionFilter,
        deselect,
        _profileSelectionEnabled)

        Debug.Print(
    "POINT SELECTION RESULT: " &
    If(hit Is Nothing,
       "NO HIT",
       hit.GetType().Name) &
    " | Tolerance=" & tolWorld.ToString("F4") &
    " | Filter=" & _selectionFilter.ToString())



        Debug.Print("Profile Selection Enabled = " &
    _profileSelectionEnabled.ToString())

        UpdateEntityProperties()

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

        ' Select By Window
        If _selectWindowActive AndAlso
                                           _currentTool = "SelectWindow" AndAlso
                                           e.Button = MouseButtons.Left Then

            ' Convert the screen rectangle to world coordinates.
            Dim w1 As PointF = ScreenToWorld(
            _selectWindowStart,
            myTransform,
            pnlPicmodeler.ClientSize.Height)

            Dim w2 As PointF = ScreenToWorld(
            _selectWindowEnd,
            myTransform,
            pnlPicmodeler.ClientSize.Height)

            ' Normalize the world rectangle.
            Dim minX As Single = Math.Min(w1.X, w2.X)
            Dim maxX As Single = Math.Max(w1.X, w2.X)
            Dim minY As Single = Math.Min(w1.Y, w2.Y)
            Dim maxY As Single = Math.Max(w1.Y, w2.Y)

            Dim worldRect As New RectangleF(minX, minY, maxX - minX, maxY - minY)

            ' Perform the actual selection.

            Dim isReverseWindow As Boolean = _selectWindowEnd.X < _selectWindowStart.X

            Debug.WriteLine($"Selection Window: Reverse={isReverseWindow}")

            Dim deselect As Boolean = (Control.ModifierKeys And Keys.Control) = Keys.Control

            _selectionManager.SelectGeoByWindow(worldRect, _selectionFilter, isReverseWindow, deselect, _profileSelectionEnabled)
            ' Keep Window Selection active for the next operation.
            _selectWindowStart = System.Drawing.Point.Empty
            _selectWindowEnd = System.Drawing.Point.Empty

            pnlPicmodeler.Cursor = Cursors.Cross
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

                If Not String.Equals(e1.CAMLayer,
                     e2.CAMLayer,
                     StringComparison.OrdinalIgnoreCase) Then
                    Continue For
                End If

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

    Private Sub pnlPicmodeler_MouseLeave(sender As Object, e As EventArgs) Handles pnlPicmodeler.MouseLeave
        If _inquireMode Then
            entityTip.Hide(pnlPicmodeler)
        End If

        pnlPicmodeler.Cursor = If(_inquireMode, Cursors.Help, Cursors.Default)
    End Sub

    Private Sub bbInquire_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbInquire.ItemClick
        ShowEntityPropertiesForm()

    End Sub
    Private Sub frmMain_KeyDown(sender As Object, e As KeyEventArgs) Handles Me.KeyDown

        If e.KeyCode <> Keys.Escape Then Return

        ' Preserve existing Inquire Mode behavior.
        If _inquireMode Then

            ExitInquireMode()
            e.Handled = True
            Return

        End If

        ' Exit persistent Window Selection mode.
        If _selectWindowActive AndAlso
       _currentTool = "SelectWindow" Then

            _selectWindowActive = False

            _selectWindowStart = System.Drawing.Point.Empty
            _selectWindowEnd = System.Drawing.Point.Empty

            _currentTool = ""

            pnlPicmodeler.Cursor = Cursors.Default
            pnlPicmodeler.Invalidate()

            e.Handled = True
            Return

        End If

    End Sub

    Private ReadOnly Property IsInquireMode As Boolean
        Get
            Return _inquireMode
        End Get
    End Property

    Private Sub ExitInquireMode()
        _inquireMode = False

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
        'AddHandler pnlPicmodeler.Invalidated, Sub() RedrawAll()
    End Sub

    Private Sub bbViewFull_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbViewFull.ItemClick
        ViewFull()

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
        DrawHoverLineArrow(g)

    End Sub
    Private Sub DrawSnapIndicator(
    g As Graphics,
    snap As SnapResult)

        If g Is Nothing Then
            Return
        End If

        Dim pt As PointF = WorldToScreen(
        snap.WorldPoint.X,
        snap.WorldPoint.Y,
        myTransform,
        pnlPicmodeler.ClientSize.Height)

        ' Hot Spot marker size in screen pixels.
        Dim markerSize As Single = 10.0F

        Dim halfSize As Single = markerSize / 2.0F

        Dim rect As New RectangleF(
        pt.X - halfSize,
        pt.Y - halfSize,
        markerSize,
        markerSize)

        Using pen As New Pen(System.Drawing.Color.Yellow, 2.0F)
            g.DrawRectangle(
            pen,
            rect.X,
            rect.Y,
            rect.Width,
            rect.Height)
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

        If _hotSpotManager Is Nothing Then
            Return Nothing
        End If

        If _cadEntities Is Nothing OrElse
       _cadEntities.Count = 0 Then

            Return Nothing

        End If


        '--------------------------------------------------
        ' Convert mouse position to world coordinates
        '--------------------------------------------------

        Dim mouseWorld As PointF =
        ScreenToWorld(
            mouseScreen,
            myTransform,
            pnlPicmodeler.ClientSize.Height)


        '--------------------------------------------------
        ' Convert screen tolerance to world tolerance
        '--------------------------------------------------

        If myTransform.Scale <= 0.0F Then
            Return Nothing
        End If

        Dim tolWorld As Single =
        tolScreenPx / myTransform.Scale


        '--------------------------------------------------
        ' Ask HotSpotManager for the closest hot spot
        '--------------------------------------------------

        Dim hotSpot As HotSpot =
        _hotSpotManager.FindHotSpot(
            mouseWorld,
            tolWorld)


        If hotSpot Is Nothing Then
            Return Nothing
        End If


        '--------------------------------------------------
        ' Adapt HotSpot to existing SnapResult
        '--------------------------------------------------

        Dim result As New SnapResult

        Select Case hotSpot.Type
            Case HotSpotType.Endpoint
                result.Type = SnapType.Endpoint

            Case HotSpotType.Midpoint
                result.Type = SnapType.Midpoint

            Case HotSpotType.Center
                result.Type = SnapType.Center

            Case HotSpotType.Intersection
                result.Type = SnapType.Intersection

            Case HotSpotType.Quadrant
                result.Type = SnapType.Quadrant

            Case Else
                Return Nothing
        End Select


        result.WorldPoint = hotSpot.WorldPoint
        result.Entity = hotSpot.Entity1


        ' HotSpotManager resolved the active Hot Spot.
        result.Distance =
        Distance(
            mouseWorld,
            hotSpot.WorldPoint)


        Return result

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


        _hotSpotEnabled = Not _hotSpotEnabled

        tbSelect_HotDot.Checked = _hotSpotEnabled

        If Not _hotSpotEnabled Then
            _activeSnap = Nothing
            _snapScreenPoint = Nothing
            pnlPicmodeler.Cursor = Cursors.Default
        End If

        pnlPicmodeler.Invalidate()

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


    'Private Function GetHoverDrawable() As DxfDrawable

    '    If _hoverEntity Is Nothing Then Return Nothing

    '    Return _hoverEntity.drawable

    'End Function

    Private Sub tbSelect_Enable_Click(sender As Object, e As EventArgs) Handles tbSelect_Enable.Click
        _selectionEnabled = tbSelect_Enable.Checked

        If Not _selectionEnabled Then
            _selectionMode = Nothing
        End If

        UpdateSelectionIcons()
        pnlPicmodeler.Invalidate()

    End Sub
    Private Sub tbSelect_Lines_Click(sender As Object, e As EventArgs) Handles tbSelect_Lines.Click

        ' Disable Profile selection.
        _profileSelectionEnabled = False

        ' Toggle Line selection.
        _selectionFilter = CType(
        CInt(_selectionFilter) Xor CInt(SelectionFilter.Line),
        SelectionFilter)

        UpdateSelectionIcons()

        pnlPicmodeler.Invalidate()

    End Sub
    Private Sub tbSelect_Arcs_Click(sender As Object, e As EventArgs) Handles tbSelect_Arcs.Click

        _profileSelectionEnabled = False

        _selectionFilter = CType(CInt(_selectionFilter) Xor CInt(SelectionFilter.Arc), SelectionFilter)

        UpdateSelectionIcons()

        pnlPicmodeler.Invalidate()


    End Sub


    Private Sub OnHoverChanged(ent As isSelectable)

        pnlPicmodeler.Invalidate()

    End Sub


    'Public Sub SelectAllByFilter(
    ' selectionMode As GeometryTypeEnum?,
    ' selectionFilter As SelectionFilter,
    ' profileSelectionEnabled As Boolean)

    '    '-----------------------------------------
    '    ' Native profile selection
    '    '-----------------------------------------
    '    If profileSelectionEnabled Then

    '        If _geoProfiles IsNot Nothing Then

    '            For Each profile As GeoProfile In _geoProfiles

    '                If profile Is Nothing Then
    '                    Continue For
    '                End If

    '                _selected.Add(profile)
    '                profile.IsSelected = True

    '            Next

    '        End If

    '    Else

    '        '-----------------------------------------
    '        ' Native primitive selection
    '        '-----------------------------------------
    '        If _geoEntities IsNot Nothing Then

    '            For Each ent As GeoEntity In _geoEntities

    '                If ent Is Nothing Then
    '                    Continue For
    '                End If

    '                '-----------------------------------------
    '                ' Filter by requested geometry type
    '                '-----------------------------------------
    '                If selectionMode.HasValue Then

    '                    Dim typeMatches As Boolean = False

    '                    Select Case selectionMode.Value

    '                        Case GeometryTypeEnum.Line
    '                            typeMatches = TypeOf ent Is GeoLine

    '                        Case GeometryTypeEnum.Circle
    '                            typeMatches = TypeOf ent Is GeoCircle

    '                        Case GeometryTypeEnum.Arc
    '                            typeMatches = TypeOf ent Is GeoArc

    '                        Case GeometryTypeEnum.Point
    '                            typeMatches = TypeOf ent Is GeoPoint

    '                    End Select

    '                    If Not typeMatches Then
    '                        Continue For
    '                    End If

    '                End If

    '                '-----------------------------------------
    '                ' Apply native selection filter
    '                '-----------------------------------------
    '                If Not IsEntityAllowedBySelectionFilter(
    '                    ent,
    '                    selectionFilter) Then

    '                    Continue For

    '                End If

    '                '-----------------------------------------
    '                ' Add entity to selection
    '                '-----------------------------------------
    '                _selected.Add(ent)
    '                ent.IsSelected = True

    '            Next

    '        End If

    '    End If

    '    '-----------------------------------------
    '    ' Notify selection listeners
    '    '-----------------------------------------
    'RaiseEvent SelectionChanged(_selected)

    'End Sub

    Private Sub tbSelect_RemoveAll_Click(sender As Object, e As EventArgs) Handles tbSelect_RemoveAll.Click
        _selectionManager.Clear()
        pnlPicmodeler.Invalidate()
    End Sub
    Private Sub tbSelect_ByWindow_Click(sender As Object, e As EventArgs) Handles tbSelect_ByWindow.Click

        ' Activate Select By Window.
        _selectWindowActive = True

        ' Clear any previous selection-window rectangle.
        _selectWindowStart = System.Drawing.Point.Empty
        _selectWindowEnd = System.Drawing.Point.Empty

        ' Select By Window is a selection operation, not View Window.
        _currentTool = "SelectWindow"

        pnlPicmodeler.Cursor = Cursors.Cross
        pnlPicmodeler.Invalidate()

    End Sub

    Private Sub tbSelect_Command_Click(sender As Object, e As EventArgs) Handles tbSelect_Command.Click

    End Sub

    Private Sub tbSelect_Features_Click(sender As Object, e As EventArgs) Handles tbSelect_Features.Click

    End Sub

    Private Sub tbSelect_Holes_Click(sender As Object, e As EventArgs) Handles tbSelect_Holes.Click
        ' Disable Profile selection.
        _profileSelectionEnabled = False

        ' Toggle Line selection.
        _selectionFilter = CType(
        CInt(_selectionFilter) Xor CInt(SelectionFilter.Hole),
        SelectionFilter)

        UpdateSelectionIcons()

        pnlPicmodeler.Invalidate()
    End Sub
    Private Sub tbSelect_Layers_Click(
    sender As Object,
    e As EventArgs) Handles tbSelect_Layers.Click

        Using dlg As New frmSelectLayer()

            dlg.GeoEntities = _selectionManager.GeoEntities
            dlg.MaterialDisplayColor = _materialDisplayColor

            If dlg.ShowDialog(Me) = DialogResult.OK Then

                Dim selectedLayer As String = dlg.SelectedLayerName

                _selectionManager.SelectByLayer(selectedLayer)

            End If

        End Using

    End Sub
    Private Sub tbSelect_Points_Click(sender As Object, e As EventArgs) Handles tbSelect_Points.Click

        _profileSelectionEnabled = False

        _selectionFilter = CType(
        CInt(_selectionFilter) Xor CInt(SelectionFilter.Point),
        SelectionFilter)

        UpdateSelectionIcons()
        pnlPicmodeler.Invalidate()

    End Sub
    Private Sub tbSelect_Profiles_Click(sender As Object, e As EventArgs) Handles tbSelect_Profiles.Click

        _profileSelectionEnabled = Not _profileSelectionEnabled

        If _profileSelectionEnabled Then

            ' Disable individual geometry selection filters.
            _selectionFilter = CType(
            CInt(_selectionFilter) And Not CInt(SelectionFilter.AllGeometry),
            SelectionFilter)

        End If

        UpdateSelectionIcons()

        pnlPicmodeler.Invalidate()

    End Sub

    Private Sub tbSelect_RemoveAllByFilter_Click(
    sender As Object,
    e As EventArgs) Handles tbSelect_RemoveAllByFilter.Click

        Debug.WriteLine("ENTERED tbSelect_RemoveAllByFilter_Click")

        _selectionManager.RemoveAllByFilter(
        Nothing,
        CType(_selectionFilter, SelectionFilter),
        _profileSelectionEnabled)

        pnlPicmodeler.Invalidate()

    End Sub

    Private Sub tbSelect_Tools_Click(sender As Object, e As EventArgs) Handles tbSelect_Tools.Click

    End Sub

    Private Sub tbSelect_Click(sender As Object, e As EventArgs) Handles tbSelect.Click

    End Sub
    Private Sub UpdateSelectionIcons()

        '--------------------------------------------------
        ' Selection Enabled icon
        '--------------------------------------------------
        If _selectionEnabled Then
            tbSelect_Enable.Image =
            FabV25_WIN8.My.Resources.Blue_Select_Arrow
        Else
            tbSelect_Enable.Image =
            FabV25_WIN8.My.Resources.Orange_Select_Arrow
        End If


        '--------------------------------------------------
        ' Selection Mode icons
        '--------------------------------------------------

        ' Line
        Dim lineActive As Boolean =
    (CInt(_selectionFilter) And CInt(SelectionFilter.Line)) <> 0

        If lineActive Then
            tbSelect_Lines.Image = FabV25_WIN8.My.Resources.Blue_Select_By_Line
        Else
            tbSelect_Lines.Image = FabV25_WIN8.My.Resources.Orange_Select_By_Line
        End If

        ' Arc
        Dim ArcActive As Boolean =
    (CInt(_selectionFilter) And CInt(SelectionFilter.Arc)) <> 0

        If ArcActive Then
            tbSelect_Arcs.Image = FabV25_WIN8.My.Resources.Blue_Select_By_Arc
        Else
            tbSelect_Arcs.Image = FabV25_WIN8.My.Resources.Orange_Select_By_Arc
        End If

        'Circle
        Dim CircleActive As Boolean =
     (CInt(_selectionFilter) And CInt(SelectionFilter.Circle)) <> 0

        If CircleActive Then
            tbSelect_Circle.Image = FabV25_WIN8.My.Resources.Circle_Cyan1
        Else
            tbSelect_Circle.Image = FabV25_WIN8.My.Resources.Circle_orange1
        End If

        'Point

        Dim HoleActive As Boolean =
     (CInt(_selectionFilter) And CInt(SelectionFilter.Hole)) <> 0

        If HoleActive Then
            tbSelect_Holes.Image = FabV25_WIN8.My.Resources.Blue_Select_By_Hole
        Else
            tbSelect_Holes.Image = FabV25_WIN8.My.Resources.Orange_Select_BY_Hole
        End If

        Dim PointActive As Boolean =
     (CInt(_selectionFilter) And CInt(SelectionFilter.Point)) <> 0

        If PointActive Then
            tbSelect_Points.Image = FabV25_WIN8.My.Resources.Blue_Select_By_Point
        Else
            tbSelect_Points.Image = FabV25_WIN8.My.Resources.Orange_Select_By_Point
        End If

        Dim ProfileActive As Boolean = _profileSelectionEnabled

        If ProfileActive Then
            tbSelect_Profiles.Image =
            FabV25_WIN8.My.Resources.Blue_Select_By_Profile
        Else
            tbSelect_Profiles.Image =
            FabV25_WIN8.My.Resources.Orange_Select_By_Profile
        End If


    End Sub

    Private Sub tbSelect_Circle_Click(sender As Object, e As EventArgs) Handles tbSelect_Circle.Click

        _profileSelectionEnabled = False

        _selectionFilter = CType(
              CInt(_selectionFilter) Xor CInt(SelectionFilter.Circle), SelectionFilter)

        UpdateSelectionIcons()

        pnlPicmodeler.Invalidate()

    End Sub

    Private Sub pnlPicmodeler_Resize(
    sender As Object,
    e As EventArgs) Handles pnlPicmodeler.Resize

        ' Keep panel dimensions synchronized.
        panelWidth = pnlPicmodeler.ClientSize.Width
        panelHeight = pnlPicmodeler.ClientSize.Height

        ' Preserve the current zoom and pan.
        pnlPicmodeler.Invalidate()

    End Sub

    Private Sub BarUseWhiteBG_CheckedChanged(sender As Object, e As ItemClickEventArgs) Handles BarUseWhiteBG.CheckedChanged

    End Sub


    Private Sub tbSelect_Profiles_CheckStateChanged(sender As Object, e As EventArgs) Handles tbSelect_Profiles.CheckStateChanged
        MessageBox.Show("Which click am i <" & tbSelect_Profiles.CheckState & ">")
    End Sub

    Private Sub BarButtonItem19_ItemClick(sender As Object, e As ItemClickEventArgs) Handles BarButtonItem19.ItemClick
        ToolShapeTest.ShowDialog(Me)
    End Sub
    Private Sub bbiMaterialColor_ItemClick(sender As Object, e As ItemClickEventArgs) _
    Handles bbiMaterialColor.ItemClick

        If bbiMaterialColor.EditValue Is Nothing Then Exit Sub

        _materialDisplayColor = CType(bbiMaterialColor.EditValue, Color)
        Invalidate()

    End Sub
    Private Sub bbiMaterialColor_EditValueChanged(sender As Object, e As EventArgs) Handles bbiMaterialColor.EditValueChanged

        If bbiMaterialColor.EditValue Is Nothing Then Exit Sub

        _materialDisplayColor = CType(bbiMaterialColor.EditValue, Color)

        Call RegPutString(
        "Preferences\ViewOptions",
        "MaterialColor",
        _materialDisplayColor.ToArgb()
    )

        Invalidate()

    End Sub

    Private Function GetHoverLineArrow(ByRef arrowPoint As PointF,
                                  ByRef arrowDirection As PointF) As Boolean

        Dim line As GeoLine =
    TryCast(_selectionManager.NativeHovered, GeoLine)

        If line Is Nothing Then Return False

        Dim dx As Double = line.EndPoint.X - line.StartPoint.X
        Dim dy As Double = line.EndPoint.Y - line.StartPoint.Y

        Dim lengthSquared As Double = dx * dx + dy * dy

        If lengthSquared <= 0.000000000001 Then Return False

        ' Project the mouse onto the line segment.
        Dim mx As Double = _hoverMouseWorld.X - line.StartPoint.X
        Dim my As Double = _hoverMouseWorld.Y - line.StartPoint.Y

        Dim t As Double = (mx * dx + my * dy) / lengthSquared

        ' Keep the arrow on the line segment.
        t = Math.Max(0.0, Math.Min(1.0, t))

        arrowPoint = New PointF(
            CSng(line.StartPoint.X + t * dx),
            CSng(line.StartPoint.Y + t * dy)
        )

        ' Normalize the direction vector.
        Dim length As Double = Math.Sqrt(lengthSquared)

        arrowDirection = New PointF(
            CSng(dx / length),
            CSng(dy / length)
        )

        Return True

    End Function

    Private Sub DrawHoverLineArrow(g As Graphics)

        Dim arrowPoint As PointF
        Dim arrowDirection As PointF

        If Not GetHoverLineArrow(arrowPoint, arrowDirection) Then Return

        ' Convert arrow location to screen coordinates.
        Dim screenPt As PointF = WorldToScreen(
            arrowPoint.X,
            arrowPoint.Y,
            myTransform,
            pnlPicmodeler.ClientSize.Height)

        ' Convert world direction to screen direction.
        ' Screen Y is inverted.
        Dim dx As Single = arrowDirection.X
        Dim dy As Single = -arrowDirection.Y

        Dim arrowLength As Single = 14.0F
        Dim headLength As Single = 5.0F
        Dim headWidth As Single = 4.0F

        Dim tip As New PointF(
            screenPt.X + dx * arrowLength / 2,
            screenPt.Y + dy * arrowLength / 2)

        Dim tail As New PointF(
            screenPt.X - dx * arrowLength / 2,
            screenPt.Y - dy * arrowLength / 2)

        Dim basePt As New PointF(
            tip.X - dx * headLength,
            tip.Y - dy * headLength)

        Dim perpX As Single = -dy
        Dim perpY As Single = dx

        Dim wing1 As New PointF(
            basePt.X + perpX * headWidth,
            basePt.Y + perpY * headWidth)

        Dim wing2 As New PointF(
            basePt.X - perpX * headWidth,
            basePt.Y - perpY * headWidth)

        Using arrowPen As New Pen(System.Drawing.Color.Yellow, 2.0F)

            g.DrawLine(arrowPen, tail, tip)
            g.DrawLine(arrowPen, tip, wing1)
            g.DrawLine(arrowPen, tip, wing2)

        End Using

    End Sub

    Private Sub bbiMaterialList_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbiMaterialList.ItemClick

    End Sub



    Private Function IsEntityInSelectedProfile(
    entity As GeoEntity) As Boolean

        If entity Is Nothing OrElse
       CurrentProductionJob Is Nothing Then
            Return False
        End If

        For Each sheet In CurrentProductionJob.JobSheets

            For Each part In sheet.Parts

                For Each profile As GeoProfile In part.GeoProfiles

                    If Not profile.Selected Then
                        Continue For
                    End If

                    For Each member As GeoEntity In profile.Entities

                        If Object.ReferenceEquals(member, entity) Then
                            Return True
                        End If

                    Next

                Next

            Next

        Next

        Return False

    End Function



    Private Function IsEntityInHoveredProfile(
    entity As GeoEntity) As Boolean

        If entity Is Nothing OrElse
       CurrentProductionJob Is Nothing Then
            Return False
        End If

        If CurrentProductionJob.JobSheets Is Nothing Then
            Return False
        End If

        For Each sheet In CurrentProductionJob.JobSheets

            If sheet Is Nothing OrElse sheet.Parts Is Nothing Then
                Continue For
            End If

            For Each part In sheet.Parts

                If part Is Nothing OrElse part.GeoProfiles Is Nothing Then
                    Continue For
                End If

                For Each profile As GeoProfile In part.GeoProfiles

                    If profile Is Nothing OrElse Not profile.IsHovered Then
                        Continue For
                    End If

                    If profile.Entities Is Nothing Then
                        Continue For
                    End If

                    For Each member As GeoEntity In profile.Entities

                        If Object.ReferenceEquals(member, entity) Then
                            Return True
                        End If

                    Next

                Next

            Next

        Next

        Return False

    End Function




    Private Sub tbViewSetTools_Click(
    sender As Object,
    e As EventArgs) Handles tbViewSetTools.Click


        RemoveHandler frmSelectTool.ToolAssignmentRequested,
                      AddressOf HandleToolAssignmentRequested

        AddHandler frmSelectTool.ToolAssignmentRequested,
                   AddressOf HandleToolAssignmentRequested

        If frmSelectTool.Visible Then
            frmSelectTool.Activate()
            Return
        End If

        frmSelectTool.Show(Me)


    End Sub



    Private Sub TestSelectedCircles()

        Dim circles As List(Of GeoCircle) =
        _selectionManager.Selected.
        OfType(Of GeoCircle)().
        ToList()

        Debug.WriteLine(
        "Selected GeoCircles: " & circles.Count)

        For Each circle As GeoCircle In circles

            MessageBox.Show(
                "Circle ID = " & circle.ID &
                vbCrLf &
                "X = " & circle.Center.X &
                vbCrLf &
                "Y = " & circle.Center.Y &
                vbCrLf &
                "Radius = " & circle.Radius,
                "Selected Circle Test")

        Next
    End Sub


    'Private Sub HandleToolAssignmentRequested(
    'toolSetupMemberID As Integer)

    '    Dim circles As List(Of GeoCircle) =
    '    _selectionManager.Selected.
    '    OfType(Of GeoCircle)().
    '    ToList()

    '    MessageBox.Show(
    '    "ToolSetupMemberID: " & toolSetupMemberID &
    '    vbCrLf &
    '    "Selected circles: " & circles.Count,
    '    "Assignment Event Test")

    'End Sub


    Private Sub HandleToolAssignmentRequested(
    toolSetupMemberID As Integer)

        Dim circles As List(Of GeoCircle) =
        _selectionManager.Selected.
        OfType(Of GeoCircle)().
        ToList()

        Dim createdCount As Integer = 0

        For Each circle As GeoCircle In circles

            Dim owningPart As Part =
            FindPartForGeometry(circle)

            If owningPart Is Nothing Then Continue For
            If circle.Center Is Nothing Then Continue For

            Dim owningSheet As JobSheet =
    FindJobSheetForPart(owningPart)

            If owningSheet Is Nothing Then Continue For

            Dim existingHole As HoleFeature =
    owningPart.Features.
    OfType(Of HoleFeature)().
    FirstOrDefault(
        Function(h) h.SourceGeometryID = circle.ID)

            If existingHole IsNot Nothing Then

                existingHole.ToolSetupMemberID = toolSetupMemberID

                Continue For

            End If

            Dim hole As New HoleFeature With {
            .FeatureType = MachiningFeatureType.Hole,
            .Description = "Punch Hole",
            .X = circle.Center.X,
            .Y = circle.Center.Y,
            .Angle = 0,
            .SourceGeometryID = circle.ID,
            .ID = owningSheet.ManufacturingModel.GetNextFeatureID(),
            .ToolSetupMemberID = toolSetupMemberID
        }

            owningPart.Features.Add(hole)
            _selectionManager.RegisterMachiningFeature(hole)

            owningSheet.ManufacturingModel.ManufacturingGeometry.Add(hole)

            Dim sourceAssignment As MM3WorkZoneAssignment = Nothing

            For Each assignment As MM3WorkZoneAssignment In
    owningSheet.ManufacturingModel.WorkZoneAssignments

                If Object.ReferenceEquals(assignment.Entity, circle) Then
                    sourceAssignment = assignment
                    Exit For
                End If

            Next

            owningSheet.ManufacturingModel.WorkZoneAssignments.Add(
    New MM3WorkZoneAssignment With {
        .WorkZoneIndex = If(sourceAssignment Is Nothing,
                            CType(Nothing, Integer?),
                            sourceAssignment.WorkZoneIndex),
        .Entity = hole
    })

            Dim holeAssignment As MM3WorkZoneAssignment =
    owningSheet.ManufacturingModel.WorkZoneAssignments.
    FirstOrDefault(Function(a) Object.ReferenceEquals(a.Entity, hole))


            MessageBox.Show(
    "SourceGeometryID: " & hole.SourceGeometryID &
    vbCrLf &
    "ToolSetupMemberID: " & hole.ToolSetupMemberID &
    vbCrLf &
    "X: " & hole.X &
    vbCrLf &
    "Y: " & hole.Y &
    vbCrLf &
    "Part Feature Count: " & owningPart.Features.Count,
    "HoleFeature Verification")

            createdCount += 1


        Next

        pnlPicmodeler.Invalidate()


    End Sub

    Private Function FindPartForGeometry(
    geometry As GeoEntity) As Part

        If geometry Is Nothing OrElse
       CurrentProductionJob Is Nothing Then

            Return Nothing
        End If

        For Each sheet As JobSheet In CurrentProductionJob.JobSheets

            For Each part As Part In sheet.Parts

                If part.Geometry.Any(
                Function(ent) Object.ReferenceEquals(ent, geometry)) Then

                    Return part

                End If

            Next

        Next

        Return Nothing

    End Function

    Private Function FindJobSheetForPart(
    targetPart As Part) As JobSheet

        If targetPart Is Nothing OrElse
       CurrentProductionJob Is Nothing Then

            Return Nothing

        End If

        For Each sheet As JobSheet In CurrentProductionJob.JobSheets

            For Each part As Part In sheet.Parts

                If Object.ReferenceEquals(part, targetPart) Then
                    Return sheet
                End If

            Next

        Next

        Return Nothing

    End Function




    Private Sub PrintCrossData_ItemClick(sender As Object, e As ItemClickEventArgs) Handles PrintCrossData.ItemClick
        'TestSelectedCircles()
        dockPanelEntityList.Close()
    End Sub

    Private Sub DrawHoleFeature(
    hole As HoleFeature,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer)

        If hole Is Nothing Then Exit Sub

        ' Resolve the actual round punch diameter.
        Dim diameter As Double

        If Not AutoPunchingEngine.TryGetRoundPunchDiameter(
    hole, diameter) Then

            Exit Sub

        End If

        ' Resolve the installed tool for its display color.
        Dim member = AppData.ToolSetupMembers.
    FirstOrDefault(
        Function(m) m.ID = hole.ToolSetupMemberID)

        If member Is Nothing Then Exit Sub

        Dim tool = AppData.ToolCrib.
    FirstOrDefault(
        Function(t) t.ID = member.ToolID)

        If tool Is Nothing Then Exit Sub

        ' Convert punch center and radius to screen coordinates.
        Dim center As PointF =
            WorldToScreen(hole.X, hole.Y, vt, panelHeight)

        Dim radius As Single =
            CSng((diameter / 2.0) * vt.Scale)

        Dim rect As New RectangleF(
            center.X - radius,
            center.Y - radius,
            radius * 2.0F,
            radius * 2.0F)

        Dim drawColor As Color = tool.Color
        Dim drawStyle As Drawing2D.DashStyle =
    Drawing2D.DashStyle.Dash

        If hole.IsSelected Then

            drawColor = Color.Red

        ElseIf hole.IsHovered Then

            drawColor = Color.Red
            drawStyle = Drawing2D.DashStyle.Dash

        End If

        ' Draw the actual round punch outline.
        ' Dashed line distinguishes Manufacturing Geometry
        ' from the solid Part Geometry.

        Using pen As New Pen(drawColor, 2.0F)

            pen.DashStyle = drawStyle

            g.DrawEllipse(pen, rect)

        End Using


    End Sub


    Private Sub DrawLineOfHolesFeature(
    row As LineOfHolesFeature,
    g As Graphics,
    vt As ViewTransform,
    panelHeight As Integer,
    sheetXExtent As Double,
    sheetYExtent As Double)

        If row Is Nothing Then Exit Sub
        If row.Count <= 0 Then Exit Sub

        Debug.WriteLine(
    "Drawing LineOfHolesFeature ID = " & row.ID.ToString())

        'RepeatCount represents the total number of row instances.
        Dim repeatCount As Integer =
        Math.Max(1, row.Parameters.RepeatCount)

        Dim baseStartX As Double = row.StartX

        If row.Parameters.Centered Then

            'Primary row span: first hole through last hole.
            Dim patternSpanX As Double =
                (row.Count - 1) * row.Spacing

            'When repeated along X, include all repeated rows.
            If row.Parameters.RepeatDirection = HoleRepeatDirection.X Then

                patternSpanX +=
                    (repeatCount - 1) * row.Parameters.Gap

            End If

            'Center the full X span in the material.
            baseStartX =
                (sheetXExtent - patternSpanX) / 2.0

        End If

        For repeatIndex As Integer = 0 To repeatCount - 1

           Dim rowStartX As Double = baseStartX
            Dim rowStartY As Double = row.StartY

            Select Case row.Parameters.RepeatDirection

                Case HoleRepeatDirection.X
                    rowStartX += repeatIndex * row.Parameters.Gap

                Case HoleRepeatDirection.Y
                    rowStartY += repeatIndex * row.Parameters.Gap

            End Select

            For holeIndex As Integer = 0 To row.Count - 1

                'These are display-only HoleFeature objects.
                'Nothing is added to MM3 or to a Part.
                Dim displayHole As New HoleFeature With {
                .X = rowStartX + (holeIndex * row.Spacing),
                .Y = rowStartY,
                .ToolSetupMemberID = row.ToolSetupMemberID,
                .IsSelected = row.IsSelected,
                .IsHovered = row.IsHovered
            }

                DrawHoleFeature(displayHole, g, vt, panelHeight)

                If row.Parameters.XMirror Then

                    Dim mirroredX As Double =
                        sheetXExtent - displayHole.X

                    'Avoid a duplicate when the hole is on the centerline.
                    If Math.Abs(mirroredX - displayHole.X) > 0.000001 Then

                        Dim mirroredHole As New HoleFeature With {
                            .X = mirroredX,
                            .Y = displayHole.Y,
                            .ToolSetupMemberID = row.ToolSetupMemberID,
                            .IsSelected = row.IsSelected,
                            .IsHovered = row.IsHovered
                        }

                        DrawHoleFeature(mirroredHole, g, vt, panelHeight)

                    End If

                End If


                If row.Parameters.YMirror Then

                    Dim mirroredY As Double =
                        sheetYExtent - displayHole.Y

                    If Math.Abs(mirroredY - displayHole.Y) > 0.000001 Then

                        Dim mirroredHole As New HoleFeature With {
                            .X = displayHole.X,
                            .Y = mirroredY,
                            .ToolSetupMemberID = row.ToolSetupMemberID,
                            .IsSelected = row.IsSelected,
                            .IsHovered = row.IsHovered
                        }

                        DrawHoleFeature(mirroredHole, g, vt, panelHeight)

                    End If

                End If

                If row.Parameters.XMirror AndAlso
   row.Parameters.YMirror Then

                    Dim mirroredX As Double =
                        sheetXExtent - displayHole.X

                    Dim mirroredY As Double =
                        sheetYExtent - displayHole.Y

                    'If either coordinate is on center, this position
                    'was already drawn by one of the individual mirrors.
                    If Math.Abs(mirroredX - displayHole.X) > 0.000001 AndAlso
                       Math.Abs(mirroredY - displayHole.Y) > 0.000001 Then

                        Dim cornerHole As New HoleFeature With {
                            .X = mirroredX,
                            .Y = mirroredY,
                            .ToolSetupMemberID = row.ToolSetupMemberID,
                            .IsSelected = row.IsSelected,
                            .IsHovered = row.IsHovered
                        }

                        DrawHoleFeature(cornerHole, g, vt, panelHeight)

                    End If

                End If
            Next

        Next

    End Sub



    Private Sub tbSelect_AddAllByFilter_Click(
    sender As Object,
    e As EventArgs) Handles tbSelect_AddAllByFilter.Click

        '-----------------------------------------
        ' Debug selection state
        '-----------------------------------------
        Debug.WriteLine("ENTERED tbSelect_AddAllByFilter_Click")

        Debug.WriteLine(
        $"ProfileSelectionEnabled={_profileSelectionEnabled}, " &
        $"SelectionFilter={_selectionFilter}")

        '-----------------------------------------
        ' Select all native geometry by filter
        '-----------------------------------------
        _selectionManager.SelectAllByFilter(
        Nothing,
        CType(_selectionFilter, SelectionFilter),
        _profileSelectionEnabled)

        '-----------------------------------------
        ' Refresh drawing
        '-----------------------------------------
        pnlPicmodeler.Invalidate()

    End Sub
    Private Sub UpdateEntityProperties()

        If _entityPropertiesForm Is Nothing OrElse
       _entityPropertiesForm.IsDisposed Then
            Return
        End If

        Dim selected = _selectionManager.SelectedObjects

        If selected.Count = 1 Then

            Dim selectedObject = selected.First()

            If TypeOf selectedObject Is HoleFeature Then

                _entityPropertiesForm.ShowSelectedObject(
                New RoundPunchProperties(
                    DirectCast(selectedObject, HoleFeature)))

            Else

                _entityPropertiesForm.ShowSelectedObject(selectedObject)

            End If

        Else

            _entityPropertiesForm.ShowSelectedObject(Nothing)

        End If

    End Sub

    Private Sub ShowEntityPropertiesForm()

        If _entityPropertiesForm Is Nothing OrElse
           _entityPropertiesForm.IsDisposed Then

            _entityPropertiesForm = New frmEntityProperties()

        End If

        _entityPropertiesForm.Show(Me)
        _entityPropertiesForm.BringToFront()

        UpdateEntityProperties()

    End Sub

    Private Sub BarButtonItem17_ItemClick(sender As Object, e As ItemClickEventArgs) Handles BarButtonItem17.ItemClick


    End Sub

    Private Sub tbViewSetLayers_Click(sender As Object, e As EventArgs) Handles tbViewSetLayers.Click

        If CurrentProductionJob Is Nothing Then Return

        frmLayers.ProductionJob = CurrentProductionJob
        frmLayers.Show(Me)
    End Sub

    Private Sub BarButtonItem20_ItemClick(sender As Object, e As ItemClickEventArgs) Handles BarButtonItem20.ItemClick
        frmSelectLayer.Show(Me)
    End Sub
    Private Sub tbSelect_All_Click(sender As Object, e As EventArgs) Handles tbSelect_All.Click

        _selectionManager.SelectAll()

        pnlPicmodeler.Invalidate()

    End Sub

    Public Sub InitializeEntityList()


        tlEntityList.BeginUnboundLoad()

        Try
            tlEntityList.Nodes.Clear()

            Dim partGeoNode =
    tlEntityList.AppendNode(
        New Object() {"PartGeo"},
        CType(Nothing, DevExpress.XtraTreeList.Nodes.TreeListNode))

            Dim manufacturingGeoNode =
    tlEntityList.AppendNode(
        New Object() {"ManufacturingGeo"},
        CType(Nothing, DevExpress.XtraTreeList.Nodes.TreeListNode))

            partGeoNode.Expanded = True
            manufacturingGeoNode.Expanded = True

        Finally
            tlEntityList.EndUnboundLoad()
        End Try

    End Sub

    Public Sub RefreshEntityList()

        ' No production job loaded
        If CurrentProductionJob Is Nothing Then Exit Sub

        ' No job sheets available
        If CurrentProductionJob.JobSheets Is Nothing OrElse
       CurrentProductionJob.JobSheets.Count = 0 Then Exit Sub

        ' We will populate the TreeList here.
        ' Make sure the root nodes exist
        If tlEntityList.Nodes.Count < 2 Then Exit Sub

        Dim partGeoNode As DevExpress.XtraTreeList.Nodes.TreeListNode =
            tlEntityList.Nodes(0)

        Dim manufacturingGeoNode As DevExpress.XtraTreeList.Nodes.TreeListNode =
            tlEntityList.Nodes(1)

        ' Clear existing entries while preserving the root nodes.
        partGeoNode.Nodes.Clear()
        manufacturingGeoNode.Nodes.Clear()

        For Each sheet As JobSheet In CurrentProductionJob.JobSheets

            If sheet Is Nothing Then Continue For

            Dim mm As MM3 = sheet.ManufacturingModel

            If mm Is Nothing Then Continue For

            If mm.PartGeometry Is Nothing Then Continue For

            '---------------------------------------------
            ' Populate PartGeo profiles
            '---------------------------------------------
            For Each part As Part In sheet.Parts

                If part Is Nothing Then Continue For
                If part.GeoProfiles Is Nothing Then Continue For

                For Each profile As GeoProfile In part.GeoProfiles

                    If profile Is Nothing Then Continue For

                    Dim profileNode =
            tlEntityList.AppendNode(
                New Object() {"Profile " & profile.ID.ToString()},
                partGeoNode)

                    ' Reference the actual profile object
                    profileNode.Tag = profile


                    '---------------------------------------------
                    ' Add profile geometry in traversal order
                    '---------------------------------------------
                    If profile.Segments IsNot Nothing Then

                        For Each segment As GeoProfileSegment In profile.Segments

                            If segment Is Nothing OrElse
                               segment.Entity Is Nothing Then Continue For

                            Dim entity As GeoEntity = segment.Entity

                            Dim entityNode =
                                tlEntityList.AppendNode(
                                    New Object() {entity.GetType().Name},
                                    profileNode)

                            ' Retain the original geometry reference
                            entityNode.Tag = entity

                        Next

                    End If



                Next

            Next


            '---------------------------------------------
            ' Populate ManufacturingGeo features
            '---------------------------------------------
            If mm.ManufacturingGeometry IsNot Nothing Then

                For Each feature As MachiningFeature In mm.ManufacturingGeometry

                    If feature Is Nothing Then Continue For

                    Dim featureNode =
            tlEntityList.AppendNode(
                New Object() {
                    feature.FeatureType.ToString() &
                    " " &
                    feature.ID.ToString()
                },
                manufacturingGeoNode)

                    ' Reference the actual machining feature
                    featureNode.Tag = feature

                Next

            End If



            Debug.WriteLine(
                "Entity List: PartGeo Count = " &
                mm.PartGeometry.Count.ToString())

        Next
    End Sub


    Private Sub tlEntityList_Load(sender As Object, e As EventArgs) Handles tlEntityList.Load

        InitializeEntityList()

        RefreshEntityList()

    End Sub


    Private Sub tlEntityList_AfterCheckNode(
    sender As Object,
    e As NodeEventArgs) Handles tlEntityList.AfterCheckNode

        If _updatingEntityListSelection Then Return

        If e.Node Is Nothing Then Return

        Dim selectedObject As isSelectable =
        TryCast(e.Node.Tag, isSelectable)

        If selectedObject Is Nothing Then Return

        ' Special case:
        ' Unchecking an entity belonging to a selected profile.
        If Not e.Node.Checked Then

            Dim entity As GeoEntity =
            TryCast(selectedObject, GeoEntity)

            If entity IsNot Nothing AndAlso e.Node.ParentNode IsNot Nothing Then

                Dim profile As GeoProfile =
                TryCast(e.Node.ParentNode.Tag, GeoProfile)

                If profile IsNot Nothing AndAlso profile.IsSelected Then

                    _selectionManager.DeselectEntityFromProfile(
                    profile,
                    entity)

                    pnlPicmodeler.Invalidate()
                    Return

                End If

            End If

        End If

        ' Normal selection and deselection.
        _selectionManager.SetObjectSelected(
        selectedObject,
        e.Node.Checked)

        pnlPicmodeler.Invalidate()

    End Sub

    Private Sub SyncEntityListSelection()

        If tlEntityList.Nodes.Count = 0 Then Return

        _updatingEntityListSelection = True

        Try
            tlEntityList.BeginUpdate()

            Try
                For Each rootNode As DevExpress.XtraTreeList.Nodes.TreeListNode In
                    tlEntityList.Nodes

                    SyncEntityListNode(rootNode)

                Next

            Finally
                tlEntityList.EndUpdate()
            End Try

        Finally
            _updatingEntityListSelection = False
        End Try

    End Sub

    Private Sub SyncEntityListNode(
    node As DevExpress.XtraTreeList.Nodes.TreeListNode)

        Dim selectableObject As isSelectable =
        TryCast(node.Tag, isSelectable)

        If selectableObject IsNot Nothing Then

            Dim effectiveSelected As Boolean =
            selectableObject.IsSelected

            ' A selected profile makes all its
            ' constituent geometry effectively selected.
            If TypeOf selectableObject Is GeoEntity Then

                Dim parentNode As DevExpress.XtraTreeList.Nodes.TreeListNode =
                node.ParentNode

                If parentNode IsNot Nothing Then

                    Dim parentProfile As GeoProfile =
                    TryCast(parentNode.Tag, GeoProfile)

                    If parentProfile IsNot Nothing AndAlso
                   parentProfile.IsSelected Then

                        effectiveSelected = True

                    End If

                End If

            End If

            node.Checked = effectiveSelected

        End If

        ' Process child nodes.
        For Each childNode As DevExpress.XtraTreeList.Nodes.TreeListNode In
        node.Nodes

            SyncEntityListNode(childNode)

        Next

    End Sub

    Private Function GetEffectiveSelectionCount(
    selectedItems As IEnumerable(Of isSelectable)) As Integer

        Dim counted As New HashSet(Of isSelectable)

        For Each item As isSelectable In selectedItems

            If item Is Nothing Then Continue For

            counted.Add(item)

            Dim profile As GeoProfile = TryCast(item, GeoProfile)

            If profile IsNot Nothing AndAlso
               profile.Entities IsNot Nothing Then

                For Each entity As GeoEntity In profile.Entities

                    If entity IsNot Nothing Then
                        counted.Add(entity)
                    End If

                Next

            End If

        Next

        Return counted.Count

    End Function

    Private Sub tlEntityList_MouseMove(
        sender As Object,
        e As MouseEventArgs) Handles tlEntityList.MouseMove

        Dim hitInfo = tlEntityList.CalcHitInfo(e.Location)

        Dim hoveredObject As isSelectable = Nothing

        If hitInfo IsNot Nothing AndAlso
           hitInfo.Node IsNot Nothing Then

            hoveredObject =
                TryCast(hitInfo.Node.Tag, isSelectable)

        End If

        _selectionManager.SetHoveredObject(hoveredObject)

    End Sub


    Private Sub tlEntityList_MouseLeave(
    sender As Object,
    e As EventArgs) Handles tlEntityList.MouseLeave

        _selectionManager.SetHoveredObject(Nothing)

    End Sub

    Private Sub toolTipControllerEntityList_GetActiveObjectInfo(
        sender As Object,
        e As ToolTipControllerGetActiveObjectInfoEventArgs
    ) Handles toolTipControllerEntityList.GetActiveObjectInfo

        If e.SelectedControl IsNot tlEntityList Then Return

        Dim mousePoint As Point =
            tlEntityList.PointToClient(Control.MousePosition)

        Dim hitInfo = tlEntityList.CalcHitInfo(mousePoint)

        If hitInfo Is Nothing OrElse hitInfo.Node Is Nothing Then Return


        Dim profile As GeoProfile =
    TryCast(hitInfo.Node.Tag, GeoProfile)

        If profile IsNot Nothing Then

            Dim profileTooltipText As String =
    "Profile ID: " & profile.ID.ToString() & vbCrLf &
        "Entities: " & profile.Entities.Count.ToString() & vbCrLf &
        "Segments: " & profile.Segments.Count.ToString() & vbCrLf &
        "Status: " & If(profile.IsClosed, "Closed", "Open")

            e.Info = New DevExpress.Utils.ToolTipControlInfo(
    hitInfo.Node,
    profileTooltipText)

            Return

        End If



        Dim hole As HoleFeature =
    TryCast(hitInfo.Node.Tag, HoleFeature)

        If hole IsNot Nothing Then


            Dim toolMember As ToolSetupMember =
    AppData.ToolSetupMembers.FirstOrDefault(
        Function(m) m.ID = hole.ToolSetupMemberID)


            Dim toolDescription As String = "Unassigned"

            If toolMember IsNot Nothing Then

                Dim tool = AppData.ToolCrib.FirstOrDefault(
        Function(t) t.ID = toolMember.ToolID)

                If tool IsNot Nothing Then
                    toolDescription = tool.Description
                End If

            End If


            Dim toolNumber As Integer = -1

            If toolMember IsNot Nothing Then

                Dim station = AppData.StationsTable.Rows.FirstOrDefault(
        Function(s) s.Value(Of Integer)("ID") = toolMember.StationID)

                If station IsNot Nothing Then
                    toolNumber = station.Value(Of Integer)("NCCodeNumber")
                End If

            End If



            Dim holeTooltipText As String =
        "Hole ID: " & hole.ID.ToString() & vbCrLf &
        "Feature: " & hole.FeatureType.ToString() & vbCrLf &
        "X: " & hole.X.ToString("F4") & vbCrLf &
        "Y: " & hole.Y.ToString("F4") & vbCrLf &
        "Angle: " & hole.Angle.ToString("F4") & vbCrLf &
        "Source Geometry ID: " & hole.SourceGeometryID.ToString() & vbCrLf &
        "Tool Setup Member ID: " & hole.ToolSetupMemberID.ToString() & vbCrLf &
        "Tool: " & toolDescription & vbCrLf &
        "CNC Tool Number: " & If(toolNumber >= 0,
                         toolNumber.ToString(),
                         "Not Assigned")

            e.Info = New DevExpress.Utils.ToolTipControlInfo(
        hitInfo.Node,
        holeTooltipText)

            Return

        End If




        Dim entity As GeoEntity =
    TryCast(hitInfo.Node.Tag, GeoEntity)

        If entity Is Nothing Then Return



        Dim tooltipText As String = "Entity ID: " & entity.ID.ToString()



        If TypeOf entity Is GeoLine Then

            Dim line As GeoLine = DirectCast(entity, GeoLine)

            If line.StartPoint IsNot Nothing AndAlso
               line.EndPoint IsNot Nothing Then

                Dim dx As Double = line.EndPoint.X - line.StartPoint.X
                Dim dy As Double = line.EndPoint.Y - line.StartPoint.Y

                Dim length As Double = Math.Sqrt(dx * dx + dy * dy)
                Dim angle As Double = Math.Atan2(dy, dx) * 180.0 / Math.PI

                tooltipText =
                    "Entity ID: " & line.ID.ToString() & vbCrLf &
                    "Start: X=" & line.StartPoint.X.ToString("F4") &
                    "  Y=" & line.StartPoint.Y.ToString("F4") & vbCrLf &
                    "End:   X=" & line.EndPoint.X.ToString("F4") &
                    "  Y=" & line.EndPoint.Y.ToString("F4") & vbCrLf &
                    "Length: " & length.ToString("F4") & vbCrLf &
                    "Angle: " & angle.ToString("F4") & "°"

            End If

        End If



        If TypeOf entity Is GeoArc Then

            Dim arc As GeoArc = DirectCast(entity, GeoArc)

            If arc.StartPoint IsNot Nothing AndAlso
       arc.EndPoint IsNot Nothing AndAlso
       arc.Center IsNot Nothing Then

                Dim direction As String =
            If(arc.IsClockwise, "CW", "CCW")

                tooltipText =
            "Entity ID: " & arc.ID.ToString() & vbCrLf &
            "Start: X=" & arc.StartPoint.X.ToString("F4") &
            "  Y=" & arc.StartPoint.Y.ToString("F4") & vbCrLf &
            "End:   X=" & arc.EndPoint.X.ToString("F4") &
            "  Y=" & arc.EndPoint.Y.ToString("F4") & vbCrLf &
            "Center: X=" & arc.Center.X.ToString("F4") &
            "  Y=" & arc.Center.Y.ToString("F4") & vbCrLf &
            "Radius: " & arc.Radius.ToString("F4") & vbCrLf &
            "Start Angle: " & arc.StartAngle.ToString("F4") & vbCrLf &
            "End Angle: " & arc.EndAngle.ToString("F4") & vbCrLf &
            "Direction: " & direction

            End If

        End If


        If TypeOf entity Is GeoCircle Then

            Dim circle As GeoCircle = DirectCast(entity, GeoCircle)

            If circle.Center IsNot Nothing Then

                tooltipText =
            "Entity ID: " & circle.ID.ToString() & vbCrLf &
            "Center: X=" & circle.Center.X.ToString("F4") &
            "  Y=" & circle.Center.Y.ToString("F4") & vbCrLf &
            "Radius: " & circle.Radius.ToString("F4") & vbCrLf &
            "Diameter: " & (circle.Radius * 2.0).ToString("F4")

            End If

        End If


        If TypeOf entity.Association Is Layer Then

            Dim cadLayer As Layer =
        DirectCast(entity.Association, Layer)

            tooltipText &= vbCrLf & "Layer: " & cadLayer.Name

        End If


        e.Info = New DevExpress.Utils.ToolTipControlInfo(
            hitInfo.Node,
            tooltipText)


    End Sub

    Private Sub tlEntityList_NodeCellStyle(
        sender As Object,
        e As GetCustomNodeCellStyleEventArgs
    ) Handles tlEntityList.NodeCellStyle

        If e.Node Is Nothing Then Return

        Dim entity As GeoEntity =
            TryCast(e.Node.Tag, GeoEntity)



        Dim profile As GeoProfile =
    TryCast(e.Node.Tag, GeoProfile)

        If profile IsNot Nothing Then

            If profile.Entities IsNot Nothing AndAlso
       profile.Entities.Count > 0 Then

                Dim firstEntity As GeoEntity =
            profile.Entities(0)

                If firstEntity IsNot Nothing Then
                    e.Appearance.ForeColor = firstEntity.Color
                End If

            End If

            Return

        End If


        Dim hole As HoleFeature =
    TryCast(e.Node.Tag, HoleFeature)

        If hole Is Nothing Then Return

        Dim toolMember = AppData.ToolSetupMembers.FirstOrDefault(
    Function(m) m.ID = hole.ToolSetupMemberID)

        If toolMember Is Nothing Then Return

        Dim tool = AppData.ToolCrib.FirstOrDefault(
    Function(t) t.ID = toolMember.ToolID)

        If tool Is Nothing Then Return

        e.Appearance.ForeColor = tool.Color
    End Sub

    Private Sub bbiForm1_ItemClick(sender As Object, e As ItemClickEventArgs) Handles bbiForm1.ItemClick


    End Sub

    Private Sub TestHoleArrangementDefinitions()

        Try
            Dim filePath As String =
            IO.Path.Combine(dataFolder,
                "HoleArrangementDefinitions.json")


            Dim definitions As HoleArrangementDefinitions =
    HoleArrangementDefinitionLoader.Load(filePath)

            Dim definition As HoleArrangementDefinition =
    definitions.Arrangements.FirstOrDefault(
        Function(a) a.ID = 301)


            MessageBox.Show(
            "Arrangement: " & definition.Description &
            vbCrLf &
            "SNCS: " & definition.ID.ToString() &
            vbCrLf &
            "Parameters: " & definition.Parameters.Count.ToString(),
            "Hole Definition Test")

        Catch ex As Exception
            MessageBox.Show(
            ex.Message,
            "Hole Definition Error")
        End Try

    End Sub


    Private Sub InitializeHoleParameterValues(definition As HoleArrangementDefinition)

        _holeParameterValues.Clear()

        For Each parameter As HoleParameterDefinition In definition.Parameters

            If parameter.Editor = "Category" Then
                Continue For
            End If

            Dim value As New HoleParameterValue With {
            .Name = parameter.Name,
            .EnteredValue = If(parameter.Default Is Nothing,
                               "",
                               parameter.Default.ToString()),
            .EvaluatedValue = parameter.Default,
            .IsValid = True
        }

            _holeParameterValues(parameter.Name) = value

        Next

    End Sub
    Private Sub PopulateHoleArrangementGrid(
    definition As HoleArrangementDefinition)

        _currentHoleArrangement = definition

        InitializeHoleParameterValues(definition)

        For Each item In _holeParameterValues
            Debug.WriteLine(item.Key & " = " &
                    item.Value.EnteredValue)
        Next

        vgCreateHoles.BeginUpdate()

        Try
            vgCreateHoles.Rows.Clear()

            Dim category As New DevExpress.XtraVerticalGrid.Rows.CategoryRow()

            category.Properties.Caption = definition.Description
            category.Name = "catHoleArrangement"

            vgCreateHoles.Rows.Add(category)



            Dim section As DevExpress.XtraVerticalGrid.Rows.CategoryRow =
    category

            Dim sectionNumber As Integer = 0

            For Each parameter As HoleParameterDefinition In definition.Parameters

                If parameter.Editor = "Category" Then

                    sectionNumber += 1

                    section = New DevExpress.XtraVerticalGrid.Rows.CategoryRow()

                    section.Name = "catHoleSection" & sectionNumber.ToString()
                    section.Properties.Caption = parameter.Display

                    vgCreateHoles.Rows.Add(section)

                Else

                    Dim editorRow As New DevExpress.XtraVerticalGrid.Rows.EditorRow()

                    editorRow.Name = parameter.Name
                    editorRow.Properties.Caption = parameter.Display

                    If parameter.Editor = "Double" Then

                        editorRow.Properties.Value =
        If(parameter.Default Is Nothing,
           "",
           parameter.Default.ToString())

                    Else

                        editorRow.Properties.Value = parameter.Default

                    End If


                    If parameter.Editor = "ToolSelector" Then

                        Dim toolEditor As New DevExpress.XtraEditors.Repository.RepositoryItemLookUpEdit()

                        Dim installedTools As New List(Of ToolSelectionRow)()

                        If CurrentJob IsNot Nothing Then
                            installedTools =
            InstalledToolService.GetInstalledTools(
                CurrentJob.ToolSetupID)
                        End If

                        toolEditor.DataSource = installedTools

                        toolEditor.ValueMember = "ID"
                        toolEditor.DisplayMember = "Description"

                        toolEditor.Columns.Clear()

                        toolEditor.Columns.Add(
        New DevExpress.XtraEditors.Controls.LookUpColumnInfo(
            "ToolNumber", "Tool #", 60))

                        toolEditor.Columns.Add(
        New DevExpress.XtraEditors.Controls.LookUpColumnInfo(
            "Description", "Description", 200))

                        toolEditor.NullText = ""

                        editorRow.Properties.RowEdit = toolEditor

                    End If

                    section.ChildRows.Add(editorRow)


                    If parameter.Editor = "RepeatDirection" Then

                        Dim repeatEditor As New DevExpress.XtraEditors.Repository.RepositoryItemComboBox()

                        repeatEditor.Items.Add("X")
                        repeatEditor.Items.Add("Y")

                        repeatEditor.TextEditStyle =
        DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor

                        editorRow.Properties.RowEdit = repeatEditor

                    End If

                    If parameter.Editor = "Double" Then

                        Dim textEditor As New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()


                        editorRow.Properties.RowEdit = textEditor


                    End If


                    If parameter.Editor = "Integer" Then

                        Dim integerEditor As New DevExpress.XtraEditors.Repository.RepositoryItemSpinEdit()

                        integerEditor.IsFloatValue = False

                        integerEditor.Mask.EditMask = "d0"

                        editorRow.Properties.RowEdit = integerEditor

                    End If

                End If



            Next

        Finally
            vgCreateHoles.EndUpdate()
        End Try

    End Sub

    Private Sub vgCreateHoles_CellValueChanged(sender As Object, e As DevExpress.XtraVerticalGrid.Events.CellValueChangedEventArgs) Handles vgCreateHoles.CellValueChanged

        If _updatingHoleGrid Then Return

        If e.Row Is Nothing Then Return

        Dim parameterName As String = e.Row.Name

        Dim runtimeValue As HoleParameterValue = Nothing

        If Not _holeParameterValues.TryGetValue(
        parameterName, runtimeValue) Then
            Return
        End If

        runtimeValue.EnteredValue =
    If(e.Value Is Nothing, "", e.Value.ToString())

        If parameterName = "ps_x" OrElse
   parameterName = "ps_y" Then

            Dim result As Double = 0

            runtimeValue.IsValid =
        HoleExpressionEvaluator.TryEvaluate(
            runtimeValue.EnteredValue, result)


            If runtimeValue.IsValid Then

                runtimeValue.EvaluatedValue = result

                _updatingHoleGrid = True

                Try
                    e.Row.Properties.Value =
                        result.ToString(
                            Globalization.CultureInfo.InvariantCulture)
                Finally
                    _updatingHoleGrid = False
                End Try

            Else

                runtimeValue.EvaluatedValue = Nothing

            End If


        Else

            runtimeValue.EvaluatedValue = e.Value
            runtimeValue.IsValid = True


            If parameterName = "tool" Then

                Dim memberID As Integer =
                    Convert.ToInt32(runtimeValue.EvaluatedValue)

                Dim member As ToolSetupMember =
                    AppData.ToolSetupMembers.FirstOrDefault(
                        Function(m) m.ID = memberID)

                If member IsNot Nothing Then

                    Debug.WriteLine(
                        "ToolSetupMember.ID = " & member.ID.ToString())

                    Debug.WriteLine(
                        "Configured Index Angle = " &
                        member.IndexAngle.ToString())


                    Dim angleValue As HoleParameterValue = Nothing

                    If _holeParameterValues.TryGetValue(
    "toolang", angleValue) Then

                        angleValue.EnteredValue =
        member.IndexAngle.ToString(
            Globalization.CultureInfo.InvariantCulture)

                        angleValue.EvaluatedValue = member.IndexAngle
                        angleValue.IsValid = True

                        _updatingHoleGrid = True

                        Try
                            Dim angleRow As DevExpress.XtraVerticalGrid.Rows.BaseRow =
    vgCreateHoles.Rows.Cast(Of DevExpress.XtraVerticalGrid.Rows.BaseRow)().
    SelectMany(Function(r) r.ChildRows.Cast(Of DevExpress.XtraVerticalGrid.Rows.BaseRow)()).
    FirstOrDefault(Function(r) r.Name = "toolang")

                            If angleRow IsNot Nothing Then
                                angleRow.Properties.Value = member.IndexAngle
                            End If

                        Finally
                            _updatingHoleGrid = False
                        End Try

                    End If


                End If

            End If

        End If


    End Sub


    Private Sub TestHoleExpressionEvaluator()

        Dim expressions As String() = {
        "12+3",
        "1.5+0.375",
        "(10-2)/4",
        "10/0",
        "12+"
    }

        For Each expression As String In expressions

            Dim result As Double = 0

            Dim valid As Boolean =
            HoleExpressionEvaluator.TryEvaluate(expression, result)

            Debug.WriteLine(
            expression &
            " | Valid = " & valid.ToString() &
            " | Result = " & result.ToString(
                Globalization.CultureInfo.InvariantCulture))

        Next

    End Sub

    Private Sub vgCreateHoles_ShownEditor(
    sender As Object,
    e As EventArgs
) Handles vgCreateHoles.ShownEditor

        Dim textEditor As DevExpress.XtraEditors.TextEdit =
        TryCast(vgCreateHoles.ActiveEditor,
                DevExpress.XtraEditors.TextEdit)

        If textEditor Is Nothing Then Return

        Dim row = vgCreateHoles.FocusedRow

        If row IsNot Nothing Then

            Dim parameterName As String = row.Name

            If parameterName = "ps_x" OrElse
           parameterName = "ps_y" Then

                Dim runtimeValue As HoleParameterValue = Nothing

                If _holeParameterValues.TryGetValue(
                parameterName, runtimeValue) Then

                    ' Restore the original expression for editing.
                    textEditor.EditValue = runtimeValue.EnteredValue

                End If

            End If

        End If

        RemoveHandler textEditor.MouseUp,
        AddressOf HoleCoordinateEditor_MouseUp

        AddHandler textEditor.MouseUp,
        AddressOf HoleCoordinateEditor_MouseUp

        textEditor.SelectAll()

    End Sub

    Private Sub HoleCoordinateEditor_MouseUp(
    sender As Object,
    e As MouseEventArgs)

        If e.Button <> MouseButtons.Left Then Return

        Dim textEditor As DevExpress.XtraEditors.TextEdit =
        TryCast(sender, DevExpress.XtraEditors.TextEdit)

        If textEditor Is Nothing Then Return

        textEditor.SelectAll()

    End Sub

    Private Sub vgCreateHoles_CustomDrawRowValueCell(
        sender As Object,
        e As DevExpress.XtraVerticalGrid.Events.CustomDrawRowValueCellEventArgs
    ) Handles vgCreateHoles.CustomDrawRowValueCell

        If e.Row Is Nothing Then Return

        Dim parameterName As String = e.Row.Name

        If parameterName <> "ps_x" AndAlso
           parameterName <> "ps_y" Then
            Return
        End If

        Dim runtimeValue As HoleParameterValue = Nothing

        If Not _holeParameterValues.TryGetValue(
            parameterName, runtimeValue) Then
            Return
        End If

        If Not runtimeValue.IsValid Then

            e.Appearance.BackColor = Color.MistyRose
            e.Appearance.ForeColor = Color.DarkRed

        End If

    End Sub

    Private Sub vgCreateHoles_KeyUp(
        sender As Object,
        e As KeyEventArgs
    ) Handles vgCreateHoles.KeyUp

        If e.KeyCode = Keys.Enter Then

            e.Handled = True
            SendKeys.Send("{TAB}")

        End If

    End Sub

    Private Sub AcceptSingleHole()

        Dim requiredParameters As String() = {"ps_x", "ps_y", "tool"}

        For Each parameterName As String In requiredParameters

            Dim parameterValue As HoleParameterValue = Nothing

            If Not _holeParameterValues.TryGetValue(
                parameterName, parameterValue) Then

                MessageBox.Show(
                    "Missing required parameter: " & parameterName,
                    "Create Hole",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Warning)

                Return
            End If

            If Not parameterValue.IsValid OrElse
               parameterValue.EvaluatedValue Is Nothing Then

                MessageBox.Show(
                    "Please enter a valid value for " & parameterName,
                    "Create Hole",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Warning)

                Return
            End If

        Next


        Dim targetSheet As JobSheet = GetCurrentHoleJobSheet()

        If targetSheet Is Nothing Then

            MessageBox.Show(
        "A production job with exactly one sheet is required.",
        "Create Hole",
        MessageBoxButtons.OK,
        MessageBoxIcon.Warning)

            Return

        End If

        Debug.WriteLine(
    "Create Hole - Target Sheet: " &
    targetSheet.MaterialSheetID.ToString())

        Debug.WriteLine(
    "Existing Manufacturing Features: " &
    targetSheet.ManufacturingModel.ManufacturingGeometry.Count.ToString())



        Dim mm As MM3 = targetSheet.ManufacturingModel


        Dim holeX As Double =
    Convert.ToDouble(_holeParameterValues("ps_x").EvaluatedValue)

        Dim holeY As Double =
    Convert.ToDouble(_holeParameterValues("ps_y").EvaluatedValue)

        Dim centered As Boolean =
    Convert.ToBoolean(_holeParameterValues("soln(2)").EvaluatedValue)

        If centered Then
            holeX = targetSheet.Length / 2.0
            holeY = targetSheet.Width / 2.0
        End If



        Dim hole As New HoleFeature With {
    .ID = mm.GetNextFeatureID(),
    .FeatureType = MachiningFeatureType.Hole,
    .Description = "Single Hole",
    .X = holeX,
    .Y = holeY,
    .Angle = Convert.ToDouble(
        _holeParameterValues("toolang").EvaluatedValue),
    .ToolSetupMemberID = Convert.ToInt32(
        _holeParameterValues("tool").EvaluatedValue),
    .SourceGeometryID = 0
}

        mm.ManufacturingGeometry.Add(hole)

        _selectionManager.RegisterMachiningFeature(hole)


        Dim mirrorX As Boolean =
    Convert.ToBoolean(
        _holeParameterValues("soln(0)").EvaluatedValue)

        If Not centered AndAlso mirrorX Then

            Dim mirroredHole As New HoleFeature With {
        .ID = mm.GetNextFeatureID(),
        .FeatureType = MachiningFeatureType.Hole,
        .Description = "Single Hole",
        .X = targetSheet.Length - holeX,
        .Y = holeY,
        .Angle = hole.Angle,
        .ToolSetupMemberID = hole.ToolSetupMemberID,
        .SourceGeometryID = 0
    }

            mm.ManufacturingGeometry.Add(mirroredHole)

            _selectionManager.RegisterMachiningFeature(mirroredHole)

        End If


        Dim mirrorY As Boolean =
    Convert.ToBoolean(
        _holeParameterValues("soln(1)").EvaluatedValue)

        If Not centered AndAlso mirrorY Then

            Dim mirroredHoleY As New HoleFeature With {
        .ID = mm.GetNextFeatureID(),
        .FeatureType = MachiningFeatureType.Hole,
        .Description = "Single Hole",
        .X = holeX,
        .Y = targetSheet.Width - holeY,
        .Angle = hole.Angle,
        .ToolSetupMemberID = hole.ToolSetupMemberID,
        .SourceGeometryID = 0
    }

            mm.ManufacturingGeometry.Add(mirroredHoleY)

            _selectionManager.RegisterMachiningFeature(mirroredHoleY)

        End If


        If Not centered AndAlso mirrorX AndAlso mirrorY Then

            Dim mirroredHoleXY As New HoleFeature With {
        .ID = mm.GetNextFeatureID(),
        .FeatureType = MachiningFeatureType.Hole,
        .Description = "Single Hole",
        .X = targetSheet.Length - holeX,
        .Y = targetSheet.Width - holeY,
        .Angle = hole.Angle,
        .ToolSetupMemberID = hole.ToolSetupMemberID,
        .SourceGeometryID = 0
    }

            mm.ManufacturingGeometry.Add(mirroredHoleXY)

            _selectionManager.RegisterMachiningFeature(mirroredHoleXY)

        End If


        RefreshEntityList()

        Debug.WriteLine(
    "Created HoleFeature ID = " & hole.ID.ToString())

        Debug.WriteLine(
    "Manufacturing Feature Count = " &
    mm.ManufacturingGeometry.Count.ToString())

        pnlPicmodeler.Invalidate()

        MessageBox.Show(
            "Hole parameters validated.",
            "Create Hole",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)

    End Sub

    Private Sub btnAcceptHole_Click(sender As Object, e As EventArgs) Handles btnAcceptHole.Click

        If _currentHoleArrangement Is Nothing Then
            MessageBox.Show(
            "No hole arrangement is selected.",
            "Create Hole",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)
            Return
        End If

        Select Case _currentHoleArrangement.ID

            Case 301
                AcceptSingleHole()

            Case 302
                AcceptRowInX()
            Case Else
                MessageBox.Show(
                "Hole arrangement SNCS " &
                _currentHoleArrangement.ID.ToString() &
                " is not yet implemented.",
                "Create Hole",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information)

        End Select

    End Sub


    Private Function GetCurrentHoleJobSheet() As JobSheet

        If CurrentProductionJob Is Nothing OrElse
       CurrentProductionJob.JobSheets Is Nothing Then

            Return Nothing

        End If

        If CurrentProductionJob.JobSheets.Count <> 1 Then
            Return Nothing
        End If

        Return CurrentProductionJob.JobSheets(0)

    End Function

    Private Sub ProfileBlend_ItemClick(sender As Object, e As ItemClickEventArgs) Handles ProfileBlend.ItemClick
        dockPanelEntityList.Show()
    End Sub


    Private Sub cbeHoleDefinition_SelectedIndexChanged(
    sender As Object,
    e As EventArgs
) Handles cbeHoleDefinition.SelectedIndexChanged

        If _holeArrangementDefinitions Is Nothing Then Return

        Dim selectedIndex As Integer =
        cbeHoleDefinition.SelectedIndex

        If selectedIndex < 0 Then Return

        If selectedIndex >=
        _holeArrangementDefinitions.Arrangements.Count Then Return

        Dim definition As HoleArrangementDefinition =
        _holeArrangementDefinitions.Arrangements(selectedIndex)

        PopulateHoleArrangementGrid(definition)

    End Sub


    Private Sub bbCreateHole_ItemClick(
        sender As Object,
        e As ItemClickEventArgs
    ) Handles bbCreateHole.ItemClick

        Dim filePath As String =
            IO.Path.Combine(
                dataFolder,
                "HoleArrangementDefinitions.json")


        _holeArrangementDefinitions =
    HoleArrangementDefinitionLoader.Load(filePath)

        Dim definitions As HoleArrangementDefinitions =
    _holeArrangementDefinitions

        cbeHoleDefinition.Properties.Items.Clear()

        For Each arrangement As HoleArrangementDefinition In definitions.Arrangements
            cbeHoleDefinition.Properties.Items.Add(arrangement.Description)
        Next

        Dim definition As HoleArrangementDefinition =
            definitions.Arrangements.FirstOrDefault(
                Function(a) a.ID = 301)

        If definition Is Nothing Then
            MessageBox.Show("Single Hole definition not found.")
            Return
        End If

        cbeHoleDefinition.SelectedIndex = 0

        PopulateHoleArrangementGrid(definition)

    End Sub


    Private Sub AcceptRowInX()

        Dim requiredParameters As String() = {
        "ps_x",
        "ps_y",
        "tool",
        "qty_x",
        "spacing_x",
        "rptd",
        "rptcnt",
        "rptdst"
    }

        For Each parameterName As String In requiredParameters

            Dim parameterValue As HoleParameterValue = Nothing

            If Not _holeParameterValues.TryGetValue(
            parameterName, parameterValue) Then

                MessageBox.Show(
                "Missing required parameter: " & parameterName,
                "Create Hole",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning)

                Return
            End If

            If parameterValue Is Nothing OrElse
           Not parameterValue.IsValid OrElse
           parameterValue.EvaluatedValue Is Nothing Then

                MessageBox.Show(
                "Please enter a valid value for " & parameterName,
                "Create Hole",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning)

                Return
            End If

        Next

        Dim targetSheet As JobSheet = GetCurrentHoleJobSheet()

        If targetSheet Is Nothing Then

            MessageBox.Show(
                "A production job with exactly one sheet is required.",
                "Create Hole",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning)

            Return

        End If

        Dim mm As MM3 = targetSheet.ManufacturingModel


        Dim rowFeature As New LineOfHolesFeature With {
    .ID = mm.GetNextFeatureID(),
    .FeatureType = MachiningFeatureType.LineOfHoles,
    .Description = "Row In X",
    .StartX = Convert.ToDouble(
        _holeParameterValues("ps_x").EvaluatedValue),
    .StartY = Convert.ToDouble(
        _holeParameterValues("ps_y").EvaluatedValue),
    .Count = Convert.ToInt32(
        _holeParameterValues("qty_x").EvaluatedValue),
    .Spacing = Convert.ToDouble(
        _holeParameterValues("spacing_x").EvaluatedValue),
    .ToolSetupMemberID = Convert.ToInt32(
        _holeParameterValues("tool").EvaluatedValue)
}


        rowFeature.Parameters.RepeatDirection =
    If(_holeParameterValues("rptd").EvaluatedValue.ToString() = "Y",
       HoleRepeatDirection.Y,
       HoleRepeatDirection.X)

        rowFeature.Direction = rowFeature.Parameters.RepeatDirection

        rowFeature.Parameters.RepeatCount =
    Convert.ToInt32(_holeParameterValues("rptcnt").EvaluatedValue)

        rowFeature.Parameters.Gap =
    Convert.ToDouble(_holeParameterValues("rptdst").EvaluatedValue)

        rowFeature.Parameters.XMirror =
    Convert.ToBoolean(_holeParameterValues("soln(0)").EvaluatedValue)

        rowFeature.Parameters.YMirror =
    Convert.ToBoolean(_holeParameterValues("soln(1)").EvaluatedValue)

        rowFeature.Parameters.Centered =
    Convert.ToBoolean(_holeParameterValues("soln(2)").EvaluatedValue)

        rowFeature.Parameters.Orientation =
    Convert.ToInt32(_holeParameterValues("toolang").EvaluatedValue)


        Debug.WriteLine("=== Row In X Feature ===")
        Debug.WriteLine("Start X: " & rowFeature.StartX)
        Debug.WriteLine("Start Y: " & rowFeature.StartY)
        Debug.WriteLine("Quantity: " & rowFeature.Count)
        Debug.WriteLine("Spacing: " & rowFeature.Spacing)
        Debug.WriteLine("Repeat Along: " & rowFeature.Parameters.RepeatDirection.ToString())
        Debug.WriteLine("Repeat Count: " & rowFeature.Parameters.RepeatCount)
        Debug.WriteLine("Repeat Distance: " & rowFeature.Parameters.Gap)
        Debug.WriteLine("Mirror X: " & rowFeature.Parameters.XMirror)
        Debug.WriteLine("Mirror Y: " & rowFeature.Parameters.YMirror)
        Debug.WriteLine("Center: " & rowFeature.Parameters.Centered)
        Debug.WriteLine("Index Angle: " & rowFeature.Parameters.Orientation)
        Debug.WriteLine("ToolSetupMemberID: " & rowFeature.ToolSetupMemberID)


        Debug.WriteLine(
            "Row In X - Target Sheet: " &
            targetSheet.MaterialSheetID.ToString())

        Debug.WriteLine(
            "Existing Manufacturing Features: " &
            mm.ManufacturingGeometry.Count.ToString())

        MessageBox.Show(
            "Row In X target sheet validated.",
            "Create Hole",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)


        mm.ManufacturingGeometry.Add(rowFeature)

        pnlPicmodeler.Invalidate()

        Debug.WriteLine(
    "Created LineOfHolesFeature ID = " &
    rowFeature.ID.ToString())

        Debug.WriteLine(
    "Manufacturing Feature Count = " &
    mm.ManufacturingGeometry.Count.ToString())

    End Sub


    Private Function FindHoleGridRow(
    row As DevExpress.XtraVerticalGrid.Rows.BaseRow,
    rowName As String) _
    As DevExpress.XtraVerticalGrid.Rows.BaseRow

        If row Is Nothing Then Return Nothing

        If String.Equals(
            row.Name,
            rowName,
            StringComparison.OrdinalIgnoreCase) Then

            Return row
        End If

        For Each child As DevExpress.XtraVerticalGrid.Rows.BaseRow In row.ChildRows

            Dim found =
                FindHoleGridRow(child, rowName)

            If found IsNot Nothing Then
                Return found
            End If

        Next

        Return Nothing

    End Function


    Private Function FindHoleGridRow(
        rowName As String) _
        As DevExpress.XtraVerticalGrid.Rows.BaseRow

        For Each rootRow As DevExpress.XtraVerticalGrid.Rows.BaseRow In vgCreateHoles.Rows

            Dim found =
                FindHoleGridRow(rootRow, rowName)

            If found IsNot Nothing Then
                Return found
            End If

        Next

        Return Nothing

    End Function


    Private Sub SetHoleCoordinateFromGraphic(
        parameterName As String,
        value As Double)

        Dim parameterValue As HoleParameterValue = Nothing

        If Not _holeParameterValues.TryGetValue(
            parameterName,
            parameterValue) Then

            Return
        End If

        Dim valueText As String =
            value.ToString(
                Globalization.CultureInfo.InvariantCulture)

        parameterValue.EnteredValue = valueText
        parameterValue.EvaluatedValue = value
        parameterValue.IsValid = True

        Dim gridRow =
            FindHoleGridRow(parameterName)

        If gridRow Is Nothing Then Return

        _updatingHoleGrid = True

        Try
            gridRow.Properties.Value = valueText
        Finally
            _updatingHoleGrid = False
        End Try

    End Sub

    Private Function TryReturnGraphicPointToHoleControl(
    mouseScreen As Point) As Boolean

        If _currentHoleArrangement Is Nothing Then
            Return False
        End If

        Dim focusedRow As DevExpress.XtraVerticalGrid.Rows.BaseRow =
            vgCreateHoles.FocusedRow

        If focusedRow Is Nothing Then
            Return False
        End If

        Dim returnX As Boolean = False
        Dim returnY As Boolean = False

        If String.Equals(focusedRow.Name, "ps_x",
                         StringComparison.OrdinalIgnoreCase) Then

            returnX = True

        ElseIf String.Equals(focusedRow.Name, "ps_y",
                             StringComparison.OrdinalIgnoreCase) Then

            returnY = True

        Else
            'A category is Start Pt only when it contains both fields.
            Dim xRow = FindHoleGridRow(focusedRow, "ps_x")
            Dim yRow = FindHoleGridRow(focusedRow, "ps_y")

            If xRow IsNot Nothing AndAlso yRow IsNot Nothing Then
                returnX = True
                returnY = True
            End If

        End If

        If Not returnX AndAlso Not returnY Then
            Return False
        End If

        Dim worldPoint As PointF =
            If(_activeSnap.HasValue,
               _activeSnap.Value.WorldPoint,
               ScreenToWorld(
                   mouseScreen,
                   myTransform,
                   pnlPicmodeler.ClientSize.Height))

        If returnX Then
            SetHoleCoordinateFromGraphic("ps_x", worldPoint.X)
        End If

        If returnY Then
            SetHoleCoordinateFromGraphic("ps_y", worldPoint.Y)
        End If

        pnlPicmodeler.Invalidate()

        Return True

    End Function


End Class



