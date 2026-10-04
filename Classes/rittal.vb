Imports DevExpress.XtraPrinting
Imports DevExpress.XtraReports.Parameters
Imports DevExpress.XtraReports.UI
Imports DevExpress.XtraReports.UserDesigner
Imports System.Drawing.Imaging
Imports System.Globalization
Imports System.Text
Module rittal

    'Stores the information for which part is nested on whihc sheet. this can be a 1 to many
    Private m_PartInfo As New System.Collections.Generic.List(Of PartInfo)

    'Stores the information for each part to be nested
    Private m_Tooling As New List(Of PrintToolData)

    Dim sSendTo As String

    Dim sTemplate As String
    Dim OutputMode As Long

    Private Function GetPartInfo() As System.Collections.Generic.List(Of PartInfo)
        Dim machineDesc As String
        Dim toolSetupDesc As String
        Dim machineID As Long
        Dim sMaterialType As String
        Dim nMaterialTypeID As Long
        Dim sMaterialDescrip As String
        Dim cPartInfo As PartInfo

        GetPartInfo = Nothing

        Try

            sMaterialDescrip = HeaderStringGet("MatCfg", "")

            nMaterialTypeID = modMaterial.GetMaterialTypeIDFromDescription(sMaterialDescrip)

            sMaterialType = GetMaterialTypeName(nMaterialTypeID)


            toolSetupDesc = HeaderStringGet("MachCfg", "")
            machineID = modTranslate.GetMachineID(toolSetupDesc)
            machineDesc = modTranslate.GetMachineDescription(machineID)

            cPartInfo = New PartInfo

            cPartInfo.PartName = PathCurrent()
            cPartInfo.Clamp_1 = HeaderDoubleGet("Clamp1Pos", 0)
            cPartInfo.Clamp_2 = HeaderDoubleGet("Clamp2Pos", 0)
            cPartInfo.Clamp_3 = HeaderDoubleGet("Clamp3Pos", 0)
            cPartInfo.Clamp_4 = HeaderDoubleGet("Clamp4Pos", 0)
            cPartInfo.Creation_Date = Date.Now

            cPartInfo.Customer = HeaderStringGet("Customer", "")
            cPartInfo.Cycle_Time = HeaderStringGet("CycleTime", "")
            cPartInfo.Due_Date = HeaderStringGet("DueDate", "")
            cPartInfo.Machine = machineDesc
            cPartInfo.Material_Length = HeaderDoubleGet("Length", 0)
            cPartInfo.Material_Thickness = HeaderDoubleGet("Thickness", 0)
            cPartInfo.Material_Type = sMaterialType
            cPartInfo.Material_Descrip = sMaterialDescrip
            cPartInfo.Material_Width = HeaderDoubleGet("Width", 0)
            cPartInfo.NC_Filename = HeaderStringGet("ncfile", "")
            cPartInfo.P_O_Number = HeaderStringGet("PoNum", "")
            cPartInfo.Part_Image = PathImageFolder() & "printpreview.png"
            cPartInfo.PartArea = 0
            cPartInfo.PartCost = 0

            m_PartInfo.Add(cPartInfo)


        Catch ex As Exception

        End Try
    End Function

    Private Sub GetPartTooling()
        Dim lCount As Long

        Try
            m_Tooling.Clear()

            For Each clsToolData In g_colToolSetup

                lReturn = PortalExecute("Entity:RefCnt: id=" + CStr(clsToolData.Model_ID))
                lCount = PortalGetInt("count", 0)

                If (lCount > 0) Then

                    If clsToolData.NC_Code_Number > 0 Then
                        With clsToolData

                            m_Tooling.Add(New PrintToolData() With {.Id = clsToolData.id,
                                                                    .Description = clsToolData.Description,
                                                                    .NC_Code_Number = clsToolData.NC_Code_Number,
                                                                    .Width_Diameter = clsToolData.Width_Diameter,
                                                                    .Length = clsToolData.Length,
                                                                    .Loff = clsToolData.Loff,
                                                                    .Doff = clsToolData.Doff,
                                                                    .Radius = clsToolData.Radius,
                                                                    .Angle = clsToolData.Angle,
                                                                    .Shear = clsToolData.Shear,
                                                                    .cost = clsToolData.Cost,
                                                                    .Life = clsToolData.Life,
                                                                    .Filename = clsToolData.FileName,
                                                                    .Speed = clsToolData.Speed,
                                                                    .Feed = clsToolData.Feed,
                                                                    .Gas = clsToolData.Gas,
                                                                    .Power = clsToolData.Power,
                                                                    .Type_Description = clsToolData.Type_Description,
                                                                    .type_id = clsToolData.Type_ID,
                                                                    .Frequency = clsToolData.Frequency,
                                                                    .Focus = clsToolData.Focus,
                                                                    .Die_Clearance = clsToolData.Die_Clearance,
                                                                    .Slowdown_Angle = clsToolData.Slowdown_Angle,
                                                                    .Slowdown_Setback = clsToolData.Slowdown_Setback,
                                                                    .Slowdown_Divisions = clsToolData.Slowdown_Divisions,
                                                                    .Slowdown_Formula = clsToolData.Slowdown_Formula,
                                                                    .Corner_Radius = clsToolData.Corner_Radius,
                                                                    .Diameter = clsToolData.Diameter,
                                                                    .Index_Angle = clsToolData.Index_Angle,
                                                                    .Kerf = clsToolData.Kerf,
                                                                    .Major_Diameter = clsToolData.Major_Diameter,
                                                                    .Minor_Diameter = clsToolData.Minor_Diameter,
                                                                    .Pitch = clsToolData.Pitch,
                                                                    .Web = clsToolData.Web,
                                                                    .Auto_Index = clsToolData.Auto_Index,
                                                                    .Fixed_Station = clsToolData.Fixed_Station,
                                                                    .Station_Location_X = clsToolData.Station_Location_X,
                                                                    .Station_Location_Y = clsToolData.Station_Location_Y,
                                                                    .Station_Size = clsToolData.Station_Size,
                                                                    .FeedMode = clsToolData.FeedMode,
                                                                    .Workplane = clsToolData.Workplane,
                                                                    .show = clsToolData.Show,
                                                                    .CutSide = clsToolData.CutSide,
                                                                    .Operation = clsToolData.Operation,
                                                                    .Model_ID = clsToolData.Model_ID,
                                                                    .level = clsToolData.Level,
                                                                    .Distance = clsToolData.Distance,
                                                                    .CutType = clsToolData.CutType,
                                                                    .Amperage = clsToolData.Amperage,
                                                                    .MaxPressure = clsToolData.MaxPressure,
                                                                    .CutQuality = clsToolData.CutQuality,
                                                                    .Plunge = clsToolData.Plunge,
                                                                    .SharpAngle = clsToolData.SharpAngle,
                                                                    .PrimaryCode = clsToolData.PrimaryCode,
                                                                    .SecondaryCode = clsToolData.SecondaryCode,
                                                                    .UseLongSide = clsToolData.UseLongSide})


                        End With

                    End If

                End If


            Next

        Catch ex As Exception

            MessageBox.Show("GetPartTooling: " & ex.Message)

        End Try
    End Sub
    Public Sub DoReports()
        Dim cmd As String
        Dim sFirstJava As String
        Dim sSecondJava As String

        sFirstJava = RegGetString("Customizations", "FirstJava", "V:\Program Files (x86)\WE-CIM\22.0\Macros\Trumpf600codeB.java")
        sSecondJava = RegGetString("Customizations", "SecondJava", "V:\Program Files (x86)\WE-CIM\22.0\Macros\Trumpf600codeA.java")

        cmd = "admin:macrorun: javafile=" & QStr(sFirstJava)

        sReturn = PortalExecute(cmd)

        'Print FIrst Report
        PrintFirstReport()

        'Run the second macro program for 009

        cmd = "admin:macrorun: javafile=" & QStr(sSecondJava)

        lReturn = PortalExecute(cmd)

        PrintSecondReport()

        'sSendTo = "Preview"

        'GetPartImage(PathImageFolder() & "printpreview.png")

        'OutputMode = PRINT_MODE.TO_PREVIEW

        'sSendTo = "Preview"

        ' GetPartInfo()
        'GetPartTooling()

        'Run first macro program 114

    End Sub
    Private Sub GetPartImage(sImageFilename As String)
        Dim cmd As String
        Dim bounds As Rectangle = frmMain.pnlPicmodeler.Bounds
        Dim pt As Point = frmMain.pnlPicmodeler.PointToScreen(bounds.Location)
        Dim myBitmap As New Bitmap(bounds.Width, bounds.Height)
        Dim lSleepMilliSeconds As Integer = 0

        Try

            'TODO:1-6  frmMain.pnlSheetPatternCombo.Visible = False

            lSleepMilliSeconds = RegGetString("Preferences", "GraphicsDelay", 0)

            frmMain.StatusBar.Visible = False

            cmd = "!view:mode:" &
                  " id=" & modView.ViewIdGet(VIEW_WIRE) &
                  ",mono=1, targets=0"

            ''We suppress display of handles by setting targets=0.
            lReturn = PortalExecute(cmd)

            modView.ViewRefresh()

            LockWindowUpdate(frmMain.pnlPicmodeler.Handle)

            'modLiteCadPrint.CreatePrintDxf(sTempDXF)


            Using m_g As Graphics = Graphics.FromImage(myBitmap)

                m_g.CopyFromScreen(New Point(pt.X - frmMain.pnlPicmodeler.Location.X, pt.Y - frmMain.pnlPicmodeler.Location.Y), Point.Empty, bounds.Size)

                Threading.Thread.Sleep(lSleepMilliSeconds)

                m_g.Dispose()

            End Using

            myBitmap.Save(sImageFilename)

            Threading.Thread.Sleep(lSleepMilliSeconds)

            myBitmap.Dispose()



            cmd = "!view:mode:" &
                    " id=" & modView.ViewIdGet(VIEW_WIRE) &
                    ",mono=0, targets=1"

            lReturn = PortalExecute(cmd)

            modView.ViewRefresh()
            LockWindowUpdate(0&)

            frmMain.StatusBar.Visible = True
        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try

    End Sub

    Private Sub PrintSecondReport()
        Dim src As New BindingSource
        Dim src1 As New BindingSource
        'Dim ReportFileName As String
        'Dim OutputMode As Long
        Dim sReportName As String



        Try

            sReportName = RegGetString("Customizations", "ReportName", "C:\Program Files (x86)\WE-CIM\22.0\Report Templates\Rittal_Trumpf.mrep")


            sReturn = RegPutString("Printing", "PrintGraphicsTemplate", sReportName)
            'sReturn = RegPutString("Printing", "PrintGraphicsSendTo", PRINT_MODE.TO_PREVIEW)

            frmPrintGraphics.ShowDialog()

            ''Stores the information for which part is nested on whihc sheet. this can be a 1 to many
            'Dim m_PartInfo As New System.Collections.Generic.List(Of PartInfo)

            ''Stores the information for each part to be nested
            'Dim m_Tooling As New List(Of PrintToolData)

            'Dim sSendTo As String


            'ReportFileName = "C:\Program Files (x86)\WE-CIM\22.0\Report Templates\Rittal_Trumpf.mrep"

            'Try
            '    Windows.Forms.Cursor.Current = Cursors.WaitCursor

            '    src.DataSource = m_PartInfo

            '    src1.DataSource = m_Tooling

            '    ' Create a report instance.
            '    Dim report As New repPrintGraphics()


            '    ' Create a report. 
            '    report.DataSource = src


            '    report.LoadLayout(ReportFileName)

            '    Dim detailReportBand As DetailReportBand = DirectCast(report.FindControl("DetailReport", False), DetailReportBand)

            '    detailReportBand.DataSource = src1

            '    detailReportBand.Visible = True

            '    GetCG_VArs(report)

            '    ' Show the report's preview. 
            '    Dim Tool As ReportPrintTool = New ReportPrintTool(report)

            '    Tool.PreviewForm.PrintBarManager.AllowQuickCustomization = False
            '    Tool.PreviewForm.ShowInTaskbar = False

            '    Tool.ShowPreview()

            '    Windows.Forms.Cursor.Current = Cursors.Default


        Catch ex As Exception

            MessageBox.Show("PrintToPrinter: " & ex.Message)

        End Try

    End Sub
    Private Sub GetCG_VArs(mReport As repPrintGraphics)
        Dim sVarCaption As String
        Dim sVarValue As String = Nothing
        Dim cgFilename As String
        Dim lReturn As Long
        Dim lCount As Long
        Dim indx As Long
        Dim sData As String
        Dim pieces() As String

        Try
            'mReport.Parameters.Clear()

            cgFilename = HeaderStringGet("cgfile", "")

            'MessageBox.Show("The CG FIlename is < " + cgFilename + " >")

            If (cgFilename <> "") Then

                ' get cg variables from cgfile
                lReturn = PortalExecute("*admin:javavarsextract:file=" & Chr(34) & cgFilename & Chr(34) &
                    ",class=" & Chr(34) & "cg" & Chr(34))

                lCount = PortalGetInt("count", 0)

                If (lCount > 0) Then

                    For indx = 0 To lCount - 1
                        lReturn = PortalExecute("*admin:javavarget:index=" & indx)

                        sData = PortalGetString("data", "")

                        pieces = Split(sData, "|")

                        pieces(2) = LTrim(pieces(2))

                        sVarCaption = pieces(2)

                        ' Create a parameter and specify its name.
                        Dim param1 As New Parameter()
                        param1.Name = "CG_" & Replace(sVarCaption, Chr(32), "_")


                        Select Case pieces(1)

                            Case "str", "combo", "dcombo"
                                sVarValue = HeaderStringGet(pieces(0), "")

                                param1.Type = GetType(System.String)

                            Case "dbl"
                                sVarValue = CStr(HeaderDoubleGet(pieces(0), 0))
                                param1.Type = GetType(System.Decimal)

                            Case "bool"

                                sVarValue = HeaderIntGet(pieces(0), 0)
                                param1.Type = GetType(System.Int32)


                            Case "int"
                                sVarValue = CStr(HeaderIntGet(pieces(0), -1))
                                param1.Type = GetType(System.Int32)

                        End Select

                        'MessageBox.Show("The svalue is < " + sVarValue + " >")

                        ' Specify other parameter properties.
                        param1.Value = sVarValue
                        param1.Visible = True

                        ' Add the parameter to the report.
                        mReport.Parameters.Add(param1)

                    Next

                End If

            End If
        Catch ex As Exception

            MessageBox.Show("GetCG_VArs: " & ex.Message)

        End Try
    End Sub
    Private Sub PrintFirstReport()
        Dim src As New BindingSource
        Dim src1 As New BindingSource
        'Dim ReportFileName As String
        'Dim OutputMode As Long
        Dim sReportName As String



        Try

            sReportName = RegGetString("Customizations", "ReportName", "C:\Program Files (x86)\WE-CIM\22.0\Report Templates\Rittal_Trumpf.mrep")


            sReturn = RegPutString("Printing", "PrintGraphicsTemplate", sReportName)
            'sReturn = RegPutString("Printing", "PrintGraphicsSendTo", PRINT_MODE.TO_PREVIEW)

            frmPrintGraphics.ShowDialog()


            'GetPartImage(PathImageFolder() & "printpreview.png")

            'OutputMode = CLng(RegGetString("Printing", "PrintGraphicsSendTo", PRINT_MODE.TO_PREVIEW))

            ''Stores the information for which part is nested on whihc sheet. this can be a 1 to many
            'Dim m_PartInfo As New System.Collections.Generic.List(Of PartInfo)

            ''Stores the information for each part to be nested
            'Dim m_Tooling As New List(Of PrintToolData)

            'Dim sSendTo As String

            'ReportFileName = "C:\Program Files (x86)\WE-CIM\22.0\Report Templates\Rittal_Trumpf.mrep"

            'Try
            '    Windows.Forms.Cursor.Current = Cursors.WaitCursor

            '    src.DataSource = m_PartInfo

            '    src1.DataSource = m_Tooling

            '    ' Create a report instance.
            '    Dim report As New repPrintGraphics()


            '    ' Create a report. 
            '    report.DataSource = src


            '    report.LoadLayout(ReportFileName)

            '    Dim detailReportBand As DetailReportBand = DirectCast(report.FindControl("DetailReport", False), DetailReportBand)

            '    detailReportBand.DataSource = src1

            '    detailReportBand.Visible = True

            '    GetCG_VArs(report)

            '    ' Show the report's preview. 
            '    Dim Tool As ReportPrintTool = New ReportPrintTool(report)

            '    Tool.PreviewForm.PrintBarManager.AllowQuickCustomization = False
            '    Tool.PreviewForm.ShowInTaskbar = False

            '    Tool.ShowPreview()

            '    'Tool.Dispose()

            '    'm_PartInfo.Clear()
            '    'm_Tooling.Clear()
            '    'detailReportBand.Dispose()

            '    Windows.Forms.Cursor.Current = Cursors.Default

            '    OutputMode = PRINT_MODE.TO_PDF
        Catch ex As Exception

            MessageBox.Show("PrintToPrinter: " & ex.Message)

        End Try

    End Sub


End Module
