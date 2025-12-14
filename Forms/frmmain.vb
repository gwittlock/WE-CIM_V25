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

Public Class frmMain
    Inherits RibbonForm
    ' In frmMain or a global module
    Public CurrentDXFLines As New List(Of LineF)          ' Lines for main panel
    Public CurrentPreviewLines As New List(Of LineF)      ' Lines for file open preview


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

    Public Sub ProcessDXFAndDraw(filePath As String)
        ' Parse lines
        CurrentDXFLines = ParseDXFToLines(filePath)

        ' Trigger redraw
        pnlPicmodeler.Invalidate()
    End Sub


    Private Sub pnlPicmodeler_Paint(sender As Object, e As PaintEventArgs) Handles pnlPicmodeler.Paint
        Dim g = e.Graphics
        g.Clear(Color.White)
        If CurrentDXFLines Is Nothing OrElse CurrentDXFLines.Count = 0 Then Return

        Dim offsetX As Single, offsetY As Single, scaleYFlip As Single
        Dim scale As Single = DxfImport.ComputeScaleAndOffset(CurrentDXFLines, pnlPicmodeler.Width, pnlPicmodeler.Height, 10, offsetX, offsetY, scaleYFlip)

        DrawLinesOnMainPanel(pnlPicmodeler, CurrentDXFLines)
        'Using pen As New Pen(Color.Black, 1)
        '    For Each ln In CurrentDXFLines
        '        Dim tln = DxfImport.TransformLine(ln, scale, offsetX, offsetY, scaleYFlip)
        '        g.DrawLine(pen, tln.X1, tln.Y1, tln.X2, tln.Y2)
        '    Next
        'End Using
    End Sub

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
End Class



