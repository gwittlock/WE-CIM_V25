Imports DevExpress.Spreadsheet
Imports System.Data.OleDb
Imports System.IO
Imports System.Data
Imports System
Imports System.Windows.Forms
Imports ADOX

Public Class XtraForm4
    Dim nToolSetupMembers As Long
    Private sMDBFilename As String
    Private Excel03ConString As String = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source={0};Extended Properties='Excel 8.0;HDR={1}'"
    Private Excel07ConString As String = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source={0};Extended Properties='Excel 8.0;HDR={1}'"

    Private Sub XtraForm4_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        'Dim isOkay As Boolean = False


        'sMDBFilename = "C:\1wittlockeng\Customers\longsin\ProcessParameters\MAXPRO200 (Cut, HC, in)_2.mdb"

        'If (FileExists(sMDBFilename) = True) Then
        '    Kill(sMDBFilename)
        'End If

        'Dim connection As New OleDb.OleDbConnection("PROVIDER=Microsoft.Jet.OLEDB.4.0;Data Source =" & sMDBFilename)

        'Dim cat As Catalog = New Catalog()
        'Dim sTablename As String = "Cut"
        'Try

        '    cat.Create("Provider=Microsoft.Jet.OLEDB.4.0;" &
        '           "Data Source=" & sMDBFilename & ";" &
        '           "Jet OLEDB:Engine Type=5")



        '    cat.ActiveConnection.close()

        '    Dim cmd As New OleDb.OleDbCommand("CREATE TABLE [" & sTablename &
        '                                    "] ([ID] AUTOINCREMENT , " &
        '                                     "[Material] TEXT(255), " &
        '                                    "[Thickness] TEXT(255), " &
        '                                    "[Class] TEXT(255), " &
        '                                    "[Profile Type] TEXT(255), " &
        '                                    "[Diameter] TEXT(255), " &
        '                                    "[Profile Area] TEXT(255), " &
        '                                    "[Base Feedrate] TEXT(255), " &
        '                                    "[Feedrate %] TEXT(255), " &
        '                                    "[Feedrate] TEXT(255), " &
        '                                    "[Lead In Feedrate] TEXT(255), " &
        '                                    "[Kerf] TEXT(255), " &
        '    "[Disable AHC] TEXT(255), " &
        '    "[Cut Chart Current] TEXT(255), " &
        '    "[Pierce Height] TEXT(255), " &
        '    "[Pierce Time] TEXT(255), " &
        '    "[Cut Height] TEXT(255), " &
        '    "[Arc Voltage] TEXT(255), " &
        '    "[Pierce Radius] TEXT(255), " &
        '    "[Ignore] TEXT(255), " &
        '    "[Manual Ignore] TEXT(255), " &
        '    "[MS] TEXT(255), " &
        '    "[SS] TEXT(255), " &
        '    "[AL] TEXT(255), " &
        '    "[50] TEXT(255), " &
        '    "[130] TEXT(255), " &
        '    "[200] TEXT(255), " &
        '    "[Edge Pierce Only] TEXT(255))", connection)

        '    'Dim cmd As New OleDb.OleDbCommand("CREATE TABLE [" & sTablename &
        '    '                                "] ([ID] AUTOINCREMENT , " &
        '    '                                "[Material] TEXT(255), " &
        '    '                                "[Thickness] FLOAT, " &
        '    '                                "[Class] TEXT(255), " &
        '    '                                "[Profile Type] TEXT(255), " &
        '    '                                "[Diameter] FLOAT, " &
        '    '                                "[Profile Area] FLOAT, " &
        '    '                                "[Base Feedrate] FLOAT, " &
        '    '                                "[Feedrate %] TEXT(255), " &
        '    '                                "[Feedrate] FLOAT, " &
        '    '                                "[Lead In Feedrate] TEXT(255), " &
        '    '                                "[Kerf] FLOAT, " &
        '    '                                "[Disable AHC] TEXT(255), " &
        '    '                                "[Cut Chart Current] TEXT(255), " &
        '    '                                "[Pierce Height] INTEGER, " &
        '    '                                "[Pierce Time] FLOAT, " &
        '    '                                "[Cut Height] FLOAT, " &
        '    '                                "[Arc Voltage] INTEGER)", connection)


        '    connection.Open()
        '    cmd.ExecuteNonQuery()

        '    connection.Close()

        'Catch ex As Exception
        '    MessageBox.Show("XtraForm4_Load: " & ex.Message)
        'End Try

        BuildParamsDB()

        Fill_Plasma_Params_DB()


    End Sub

    Private Sub BuildParamsDB()
        Dim isOkay As Boolean = False


        sMDBFilename = "C:\1wittlockeng\Customers\longsin\ProcessParameters\MAXPRO200 (Cut, HC, in)_2.mdb"

        If (FileExists(sMDBFilename) = True) Then
            Kill(sMDBFilename)
        End If

        Dim connection As New OleDb.OleDbConnection("PROVIDER=Microsoft.Jet.OLEDB.4.0;Data Source =" & sMDBFilename)

        Dim cat As Catalog = New Catalog()
        Dim sTablename As String = "Cut"
        Try

            cat.Create("Provider=Microsoft.Jet.OLEDB.4.0;" &
                   "Data Source=" & sMDBFilename & ";" &
                   "Jet OLEDB:Engine Type=5")

            cat.ActiveConnection.close()

            Dim cmd As New OleDb.OleDbCommand("CREATE TABLE [" & sTablename &
                                            "] ([ID] AUTOINCREMENT , " &
                                             "[Material] TEXT(255), " &
                                            "[Thickness] FLOAT, " &
                                            "[Class] TEXT(255), " &
                                            "[Profile Type] TEXT(255), " &
                                            "[Diameter] FLOAT, " &
                                            "[Profile Area] FLOAT, " &
                                            "[Base Feedrate] FLOAT, " &
                                            "[Feedrate %] FLOAT, " &
                                            "[Feedrate] FLOAT, " &
                                            "[Lead In Feedrate] FLOAT, " &
                                            "[Kerf] FLOAT, " &
                                            "[Disable AHC] TEXT(255), " &
                                            "[Cut Chart Current] FLOAT, " &
                                            "[Pierce Height] FLOAT, " &
                                            "[Pierce Time] FLOAT, " &
                                            "[Cut Height] FLOAT, " &
                                            "[Arc Voltage] FLOAT)", connection)

            'Dim cmd As New OleDb.OleDbCommand("CREATE TABLE [" & sTablename &
            '                                "] ([ID] AUTOINCREMENT , " &
            '                                "[Material] TEXT(255), " &
            '                                "[Thickness] FLOAT, " &
            '                                "[Class] TEXT(255), " &
            '                                "[Profile Type] TEXT(255), " &
            '                                "[Diameter] FLOAT, " &
            '                                "[Profile Area] FLOAT, " &
            '                                "[Base Feedrate] FLOAT, " &
            '                                "[Feedrate %] TEXT(255), " &
            '                                "[Feedrate] FLOAT, " &
            '                                "[Lead In Feedrate] TEXT(255), " &
            '                                "[Kerf] FLOAT, " &
            '                                "[Disable AHC] TEXT(255), " &
            '                                "[Cut Chart Current] TEXT(255), " &
            '                                "[Pierce Height] INTEGER, " &
            '                                "[Pierce Time] FLOAT, " &
            '                                "[Cut Height] FLOAT, " &
            '                                "[Arc Voltage] INTEGER)", connection)


            connection.Open()
            cmd.ExecuteNonQuery()

            connection.Close()

        Catch ex As Exception
            MessageBox.Show("BuildPramsDB: " & ex.Message)
        End Try

    End Sub

    Private Function mySaveMaterial(ByVal sNewMaterialName As String, ByVal nTheMaterialType As Long, dMatWidth As Double, dmatlength As Double, dMatThickness As Double) As Long

        Dim m_cnMaterial As New OleDb.OleDbConnection
        Dim m_daMaterial As OleDb.OleDbDataAdapter
        Dim m_cbMaterial As OleDb.OleDbCommandBuilder
        Dim m_dtMaterial As New DataTable
        Dim sql As String
        Dim ds As New DataSet
        Dim drNewRow As DataRow = m_dtMaterial.NewRow()
        Dim cmdGetID As New OleDbCommand

        sql = "SELECT [Material Inventory].* FROM [Material Inventory]"

        Try
            If (IsNumeric(dmatlength) = False) Then
                dmatlength = 0
            End If


            m_cnMaterial.ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & PathCMDB()
            m_cnMaterial.Open()
            m_daMaterial = New OleDb.OleDbDataAdapter(sql, m_cnMaterial)
            m_cbMaterial = New OleDb.OleDbCommandBuilder(m_daMaterial)
            m_cbMaterial.QuotePrefix = "["
            m_cbMaterial.QuoteSuffix = "]"
            m_daMaterial.Fill(m_dtMaterial)

            drNewRow("Type ID") = nTheMaterialType
            drNewRow("Description") = sNewMaterialName
            drNewRow("Length") = dmatlength
            drNewRow("Width") = dMatWidth
            drNewRow("Thickness") = dMatThickness
            drNewRow("Weight") = 0
            drNewRow("Quantity") = -1
            drNewRow("Cost Per Unit") = 0


            m_dtMaterial.Rows.Add(drNewRow)
            m_daMaterial.Update(m_dtMaterial)


            cmdGetID.Connection = m_cnMaterial
            cmdGetID.CommandText = "SELECT MAX(ID) FROM [Material Inventory]"



            mySaveMaterial = cmdGetID.ExecuteScalar
            m_dtMaterial.Dispose()
            m_daMaterial.Dispose()

            m_cnMaterial.Close()

        Catch ex As Exception

            mySaveMaterial = -1

            MessageBox.Show("SaveMaterial: " & ex.Message)

        End Try



    End Function
    Private Sub Fill_Plasma_Params_DB()
        'Dim sXLSXFilename As String
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter

        Dim m_dtPart As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim cmdGetID As New OleDbCommand
        Dim sFieldNames As String
        Dim sFieldValues As String


        Dim sData As String
        Dim sPieces() As String
        'Dim dtToolSetupTable As DataTable
        Dim sToolSetupName As String
        'Dim n_ToolSetupID As Long
        'Dim dtCutRec As DataTable
        'Dim oledbAdapter As OleDbDataAdapter
        Dim ds As New DataSet
        Dim ds2 As New DataSet
        'Dim sMaterial As String
        'Dim sClass As String
        Dim sThickness As String
        Dim sLastMaterial As String
        Dim sLastThickness As String
        Dim m_daToolCrib As New OleDb.OleDbDataAdapter
        Dim rnd1 As New Random
        Dim iColor As Integer
        Dim sColor As String
        Dim m_cnToolCrib As New OleDb.OleDbConnection
        Dim Kerf As Double
        Dim dFeedrate As Double
        Dim myColor(0 To 9) As Color
        Dim ni As Integer
        Dim nID As Long
        Dim lMyStationID As Long = 820
        Dim drRow As DataRow
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim sDiameter As String
        Dim sPierceHeight As String
        Dim sPierceDelay As String
        Dim sCutHeight As String
        Dim sParamsFile As String = "C:\1wittlockeng\Customers\longsin\ProcessParameters\MAXPRO200 (Cut, HC, in)3.csv"
        Dim sParamsText As String
        'Dim sMyFile As IO.StreamWriter
        'Dim ni2 As Integer

        sMDBFilename = "C:\1wittlockeng\Customers\longsin\ProcessParameters\MAXPRO200 (Cut, HC, in)_2.mdb"

        Dim connection As New OleDb.OleDbConnection("PROVIDER=Microsoft.Jet.OLEDB.4.0;Data Source =" & sMDBFilename)
        connection.Open()

        sFieldNames = sEmpty
        sFieldValues = sEmpty
        sFieldNames =
                        "[Material], " &
                        "[Thickness], " &
                        "[Class], " &
                        "[Profile Type], " &
                        "[Diameter], " &
                        "[Profile Area], " &
                        "[Base Feedrate], " &
                        "[Feedrate %], " &
                        "[Feedrate], " &
                        "[Lead In Feedrate], " &
                        "[Kerf], " &
                        "[Disable AHC], " &
                        "[Cut Chart Current], " &
                        "[Pierce Height], " &
                        "[Pierce Time], " &
                        "[Cut Height], " &
                        "[Arc Voltage]"

        Using sr As StreamReader = New StreamReader(sParamsFile)

            sData = sr.ReadLine()

            While (sr.EndOfStream = False)

                sData = sr.ReadLine()

                sPieces = Split(sData, ",")

                For ni = 0 To 16

                    If (sPieces(ni) = "*") Then
                        sPieces(ni) = "-1"
                    End If

                Next

                sFieldValues = "'" & sPieces(0) & "'" & "," &
                                 sPieces(1) & "," &
                                "'" & sPieces(2) & "'" & "," &
                                "'" & sPieces(3) & "'" & "," &
                                 sPieces(4) & "," &
                                sPieces(5) & "," &
                                sPieces(6) & "," &
                                sPieces(7) & "," &
                                sPieces(8) & "," &
                                sPieces(9) & "," &
                                sPieces(10) & "," &
                                "'" & sPieces(11) & "'" & "," &
                                sPieces(12) & "," &
                               sPieces(13) & "," &
                               sPieces(14) & "," &
                               sPieces(15) & "," &
                               sPieces(16)


                sql = "INSERT INTO [CUT] (" & sFieldNames & " ) VALUES(" & sFieldValues & ")"

                m_daPart.InsertCommand = New OleDbCommand(sql, connection)

                m_daPart.InsertCommand.ExecuteNonQuery()
            End While

            connection.Close()

        End Using

    End Sub
    Private Sub SimpleButton1_Click(sender As Object, e As EventArgs) Handles SimpleButton1.Click
        'Dim sXLSXFilename As String
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim connection As OleDbConnection
        Dim m_dtPart As New DataTable
        Dim connetionString As String
        Dim sql As String
        Dim cmdGetID As New OleDbCommand
        Dim sFieldNames As String
        Dim sFieldValues As String

        'Dim sline As String
        Dim sData As String
        Dim sPieces() As String
        Dim dtToolSetupTable As DataTable
        Dim sToolSetupName As String
        Dim n_ToolSetupID As Long
        'Dim dtCutRec As DataTable
        Dim oledbAdapter As OleDbDataAdapter
        Dim ds As New DataSet
        Dim ds2 As New DataSet
        Dim sMaterial As String
        Dim sClass As String
        Dim sThickness As String
        Dim sLastMaterial As String
        Dim sLastThickness As String
        Dim m_daToolCrib As OleDb.OleDbDataAdapter
        Dim rnd1 As New Random
        Dim iColor As Integer
        Dim sColor As String
        Dim m_cnToolCrib As New OleDb.OleDbConnection
        Dim Kerf As Double
        Dim dFeedrate As Double
        Dim myColor(0 To 9) As Color
        Dim ni As Integer
        Dim nID As Long
        Dim lMyStationID As Long = 820
        Dim drRow As DataRow
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim sDiameter As String
        Dim sPierceHeight As String
        Dim sPierceDelay As String
        Dim sCutHeight As String
        Dim sParamsFile As String = "C:\1wittlockeng\Customers\longsin\ProcessParameters\Plasma.Params"
        Dim sParamsText As String
        Dim sMyFile As IO.StreamWriter
        'Dim ni2 As Integer

        sMyFile = New System.IO.StreamWriter(sParamsFile, True) ' True for appending

        sDiameter = "0"
        sLastMaterial = ""
        sLastThickness = "0"

        '[Material],[Thickness],[Diameter],[Feedrate],[Kerf],[PierceHeight],[PierceDelay],[CutHeight]
        sFieldNames = "[Description]," &
                      "[Machine ID]"

        'To restore:
        myColor(0) = Color.LawnGreen
        myColor(1) = Color.DarkOrange
        myColor(2) = Color.Gold
        myColor(3) = Color.Gainsboro
        myColor(4) = Color.SpringGreen
        myColor(5) = Color.DeepSkyBlue
        myColor(6) = Color.Violet
        myColor(7) = Color.Fuchsia
        myColor(8) = Color.PaleTurquoise
        myColor(9) = Color.Chartreuse

        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        m_cnToolCrib = New OleDbConnection(connetionString)

        m_cnToolCrib.Open()


        'Dim connection As New OleDb.OleDbConnection("PROVIDER=Microsoft.Jet.OLEDB.4.0;Data Source =" & sMDBFilename)

        'connection.Open()

        'sFieldNames = sEmpty
        'sFieldValues = sEmpty

        'sFieldNames = "[Material] , " &
        '                "[Thickness] , " &
        '                "[Class] , " &
        '                "[Profile Type] , " &
        '                "[Diameter] , " &
        '                "[Profile Area] , " &
        '                "[Base Feedrate] , " &
        '                "[Feedrate %] , " &
        '                "[Feedrate] , " &
        '                "[Lead In Feedrate] , " &
        '                "[Kerf] , " &
        '                "[Disable AHC] , " &
        '                "[Cut Chart Current] , " &
        '                "[Pierce Height] , " &
        '                "[Pierce Time] , " &
        '                "[Cut Height] , " &
        '                "[Arc Voltage]"

        'Using sr As StreamReader = New StreamReader(sCSVFilename)

        '    sData = sr.ReadLine()

        '    While (sr.EndOfStream = False)

        '        sData = sr.ReadLine()

        '        sPieces = Split(sData, ",")

        '        sFieldValues = sPieces(0) & "," &
        '                        sPieces(1) & "," &
        '                        sPieces(2) & "," &
        '                        sPieces(3) & "," &
        '                        sPieces(4) & "," &
        '                        sPieces(5) & "," &
        '                        sPieces(6) & "," &
        '                        sPieces(7) & "," &
        '                        sPieces(8) & "," &
        '                        sPieces(9) & "," &
        '                        sPieces(10) & "," &
        '                        sPieces(11) & "," &
        '                        sPieces(12) & "," &
        '                        sPieces(13) & "," &
        '                        sPieces(14) & "," &
        '                        sPieces(15) & "," &
        '                        sPieces(16)

        '        sql = "INSERT INTO [CUT] (" & sFieldNames & " ) VALUES(" & sFieldValues & ")"

        '        m_daPart.InsertCommand = New OleDbCommand(sql, connection)

        '        m_daPart.InsertCommand.ExecuteNonQuery()

        'sToolSetupName = sMaterial & "_" & CStr(sThickness) & "_" & sClass
        ''dtToolSetupTable = ConnectToCMDB(sql)

        'drRow("Machine ID") = 8
        'drRow("Description") = sToolSetupName

        'dtToolSetupTable.Rows.Add(drRow)
        'm_daCMDB.Update(dtToolSetupTable)

        'dtToolSetupTable.Dispose()

        'n_ToolSetupID = GetNewID("Tool Setups")


        sql = "Select * From [Station Configuration] Where ([Station Configuration].[Machine ID] = 9 )"


        sql = "Select * From [Tool Setups]"

        dtToolSetupTable = ConnectToCMDB(sql)

        ni = 0

        connection = New OleDbConnection(connetionString)

        connection.Open()

        oledbAdapter = New OleDbDataAdapter(sql, connection)

        oledbAdapter.Fill(ds)

        oledbAdapter.Dispose()

        connection.Dispose()
        connection.Close()

        For i = 0 To ds.Tables(0).Rows.Count - 1

            If ni > 10 Then ni = 0

            iColor = myColor(ni).ToArgb()
            sColor = iColor.ToString
            ' myColor = Color.FromArgb(iColor)

            If IsDBNull(ds.Tables(0).Rows(i).Item("Material")) = True Then
                GoTo ExitEarly
            End If
            sMaterial = ds.Tables(0).Rows(i).Item("Material")
            sClass = UCase(ds.Tables(0).Rows(i).Item("Class"))
            sThickness = ds.Tables(0).Rows(i).Item("Thickness")
            Kerf = ds.Tables(0).Rows(i).Item("Kerf")
            dFeedrate = ds.Tables(0).Rows(i).Item("Feedrate")

            If IsDBNull(ds.Tables(0).Rows(i).Item("Diameter")) = True Then

                sDiameter = "0"
            ElseIf sDiameter = "*" Then
                sDiameter = 0
            Else
                sDiameter = ds.Tables(0).Rows(i).Item("Diameter")

            End If

            If IsDBNull(ds.Tables(0).Rows(i).Item("Pierce Height")) = True Then
                sPierceHeight = "0"
            Else
                sPierceHeight = ds.Tables(0).Rows(i).Item("Pierce Height")

            End If

            If IsDBNull(ds.Tables(0).Rows(i).Item("Pierce Time")) = True Then
                sPierceDelay = "0"
            Else
                sPierceDelay = ds.Tables(0).Rows(i).Item("Pierce Time")

            End If

            If IsDBNull(ds.Tables(0).Rows(i).Item("Cut Height")) = True Then
                sCutHeight = "0"
            Else
                sCutHeight = ds.Tables(0).Rows(i).Item("Cut Height")

            End If


            sParamsText = sMaterial & "|" & sClass & "|" & sThickness & "|" & Kerf & "|" & dFeedrate &
                        "|" & sDiameter & "|" & sPierceHeight & "|" & sPierceDelay & "|" & sCutHeight




            sMyFile.WriteLine(sParamsText)




            If sMaterial = sLastMaterial And sThickness = sLastThickness Then
                GoTo ExitEarly
            End If

            sLastMaterial = sMaterial
            sLastThickness = sThickness

            If CDbl(sThickness) < 0.125 Then

                GoTo ExitEarly
            End If

            'mColor = mColor.FromArgb(rnd1.Next(0, 256), rnd1.Next(0, 256), rnd1.Next(0, 256))


            If Kerf = 0.09 Then
                GoTo ExitEarly


            End If
            If (InStr(sClass, "AIR/AIR")) Then

                sToolSetupName = sMaterial & "_" & CStr(sThickness) & "_" & sClass & "_F" & dFeedrate
                ''dtToolSetupTable = ConnectToCMDB(sql)

                If DoesToolSetupExist(sToolSetupName) = True Then
                    GoTo ExitEarly
                End If

                sFieldValues = "'" & sToolSetupName & "','9" & "'"

                'sql = "INSERT INTO [Tool Setups] (" & sFieldNames & " ) VALUES(" & sFieldValues & ")"

                m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnToolCrib)

                'm_daSettings.InsertCommand.ExecuteNonQuery()



                'drRow = dtToolSetupTable.NewRow()

                'drRow("Machine ID") = 8
                'drRow("Description") = sToolSetupName

                'dtToolSetupTable.Rows.Add(drRow)
                'm_daCMDB.Update(dtToolSetupTable)

                dtToolSetupTable.Dispose()

                n_ToolSetupID = GetNewID("Tool Setups")


                If CheckEntryToListBox(Kerf, lbToolList) = False And Kerf <> 0.09 Then
                    lbToolList.Items.Add(Kerf)

                    sql = "INSERT INTO [Tool Crib] ([Type ID],[Color],[Description],[Kerf],[Reqd Station Size] ) VALUES(9," & iColor & ",'" & Kerf & " Dia Burner','" & Kerf & "','0')"

                    m_daToolCrib.InsertCommand = New OleDbCommand(sql, m_cnToolCrib)

                    m_daToolCrib.InsertCommand.ExecuteNonQuery()

                    cmdGetID.Connection = m_cnToolCrib
                    cmdGetID.CommandText = "SELECT MAX(ID) FROM [Tool Crib]"

                    nID = cmdGetID.ExecuteScalar

                    'lMyStationID = GetStationID(n_ToolSetupID)

                    nToolSetupMembers = GetToolSetUpMemeberID(n_ToolSetupID)
                    UpdateToolingInfo(n_ToolSetupID, lMyStationID, nToolSetupMembers, nID)
                Else

                    sql = "Select [Tool Crib].* From [Tool Crib] Where ([Tool Crib].Description = '" & Kerf & " Dia Burner" & "' )"

                    'connection = New OleDbConnection(connetionString)
                    'connection.Open()

                    oledbAdapter = New OleDbDataAdapter(sql, m_cnToolCrib)

                    oledbAdapter.Fill(ds2)

                    For ni = 0 To ds2.Tables(0).Rows.Count - 1
                        nID = ds2.Tables(0).Rows(ni).Item("ID")
                    Next

                    'sDisplayStr = ds.Tables(0).Rows(i).Item("Display")

                    ds2.Dispose()

                    oledbAdapter.Dispose()

                    nToolSetupMembers = GetToolSetUpMemeberID(n_ToolSetupID)
                    UpdateToolingInfo(n_ToolSetupID, lMyStationID, nToolSetupMembers, nID)

                End If


            End If
ExitEarly:
            ni = ni + 1
            If ni = 10 Then
                ni = 0

            End If

        Next

        MessageBox.Show("done")
        'End While

        'connection.Close()

        'End Using

        sMyFile.Flush()
        sMyFile.Close()

    End Sub

    Private Function GetStationID(ToolSetupID As Long) As Long
        Dim sql As String
        Dim ds = New DataSet()
        Dim sMyNCCodeNumber As String
        Dim lMyStationID As Long
        Dim sMyToolDescription As String
        Dim lMyOrderID As Long
        Dim lMyToolID As Long
        Dim dMyIndexAngle As Double
        Dim bMyFixedSation As Boolean
        'Dim DoAdd As Boolean
        'Dim rowHandle As Long
        'Dim lDescriptionLength As Long
        'Dim lStationLength As Long
        'Dim lFixedAngleLength As Long
        'Dim lIndexAngleLength As Long
        'Dim nOverallGridWidth As Long

        Dim connection = New OleDbConnection("Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';")

        sql = "SELECT [Tool Setup Members].[Tool Setup ID], [Tool Setup Members].[Index Angle]," &
                "[Station Configuration].[NC Code Number], [Tool Crib].Description, [Tool Setup Members].[Tool ID]," &
                "[Tool Setup Members].[Station ID], [Tool Setup Members].[Fixed Station], [Tool Setup Members].ID" &
                " FROM [Tool Crib] INNER JOIN ([Station Configuration] INNER JOIN [Tool Setup Members] ON " &
                "[Station Configuration].ID = [Tool Setup Members].[Station ID]) ON [Tool Crib].ID = " &
                "[Tool Setup Members].[Tool ID] WHERE ((([Tool Setup Members].[Tool Setup ID])=" &
                ToolSetupID & " ))" & " ORDER BY [Tool Setup Members].ID"

        InitFillDS(ds, Nothing, sql, connection)

        Dim tableNumber = 0
        For i = 0 To ds.Tables(tableNumber).Rows.Count - 1
            sMyNCCodeNumber = Convert.ToString(ds.Tables(tableNumber).Rows(i)("NC Code Number"))
            lMyStationID = ds.Tables(tableNumber).Rows(i)("Station ID")
            sMyToolDescription = Convert.ToString(ds.Tables(tableNumber).Rows(i)("Description"))
            lMyOrderID = ds.Tables(tableNumber).Rows(i)("ID")
            lMyToolID = ds.Tables(tableNumber).Rows(i)("Tool ID")
            dMyIndexAngle = ds.Tables(tableNumber).Rows(i)("Index Angle")
            bMyFixedSation = ds.Tables(tableNumber).Rows(i)("Fixed Station")

        Next i
        ds.Dispose()

        connection.Close()

        GetStationID = lMyStationID

    End Function

    Private Sub UpdateToolingInfo(ntoolSetupID As Long, StationId As Long, ToolSetupMembersID As Long, nToolID As Long)
        Dim l_tlid As Long

        Dim dIndexAng As Double
        Dim bFixedStation As Boolean
        Dim sql As String
        Dim connetionString As String
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim cmd As OleDbCommand

        bFixedStation = False
        dIndexAng = 0


        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"


        connection = New OleDbConnection(connetionString)


        connection.Open()

        sql = "Update [Tool Setup Members] set  [Tool Setup Members].[Tool Setup ID] =  '" & ntoolSetupID & "' Where " &
                    "[Tool Setup Members].[ID] = " & ToolSetupMembersID & ""

        cmd = New OleDbCommand(sql, connection)

        cmd.ExecuteNonQuery()

        sql = "Update [Tool Setup Members] set  [Tool Setup Members].[Station ID] =  '" & StationId & "' Where " &
                    "[Tool Setup Members].[ID] = " & ToolSetupMembersID & ""

        cmd = New OleDbCommand(sql, connection)

        cmd.ExecuteNonQuery()

        sql = "Update [Tool Setup Members] set  [Tool Setup Members].[Tool ID] =  '" & nToolID & "' Where " &
                    "[Tool Setup Members].[ID] = " & ToolSetupMembersID & ""

        cmd = New OleDbCommand(sql, connection)

        cmd.ExecuteNonQuery()

        sql = "Update [Tool Setup Members] set  [Tool Setup Members].[Fixed Station] =  '" & bFixedStation & "' Where " &
                    "[Tool Setup Members].[ID] = " & ToolSetupMembersID & ""

        cmd = New OleDbCommand(sql, connection)

        cmd.ExecuteNonQuery()


        sql = "Update [Tool Setup Members] set  [Tool Setup Members].[Index Angle] =  '" & dIndexAng & "' Where " &
                    "[Tool Setup Members].[ID] = " & ToolSetupMembersID & ""

        cmd = New OleDbCommand(sql, connection)

        cmd.ExecuteNonQuery()

        connection.Close()
        AssignToolToStation(ntoolSetupID, StationId, dIndexAng, bFixedStation, nToolID)
    End Sub

    Private Function GetToolSetUpMemeberID(nToolSetupID As Long) As Long
        Dim sql As String
        Dim ds As New DataSet
        Dim connection = New OleDbConnection("Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';")

        sql = "SELECT [Tool Setup Members].[Tool Setup ID], [Tool Setup Members].[Index Angle]," &
                "[Station Configuration].[NC Code Number], [Tool Crib].Description, [Tool Setup Members].[Tool ID]," &
                "[Tool Setup Members].[Station ID], [Tool Setup Members].[Fixed Station], [Tool Setup Members].ID" &
                " FROM [Tool Crib] INNER JOIN ([Station Configuration] INNER JOIN [Tool Setup Members] ON " &
                "[Station Configuration].ID = [Tool Setup Members].[Station ID]) ON [Tool Crib].ID = " &
                "[Tool Setup Members].[Tool ID] WHERE ((([Tool Setup Members].[Tool Setup ID])=" &
                nToolSetupID & " ))" & " ORDER BY [Tool Setup Members].ID"

        InitFillDS(ds, Nothing, sql, connection)

        Dim tableNumber = 0
        For i = 0 To ds.Tables(tableNumber).Rows.Count - 1

            nToolSetupMembers = ds.Tables(tableNumber).Rows(i)("ID")

        Next i
        ds.Dispose()


    End Function

    Public Function SaveMat(ByVal sNewMaterialName As String, ByVal nTheMaterialType As Long) As Long

        Dim m_cnMaterial As New OleDb.OleDbConnection
        Dim m_daMaterial As OleDb.OleDbDataAdapter
        Dim m_cbMaterial As OleDb.OleDbCommandBuilder
        Dim m_dtMaterial As New DataTable
        Dim sql As String
        Dim ds As New DataSet
        Dim drNewRow As DataRow = m_dtMaterial.NewRow()
        Dim cmdGetID As New OleDbCommand

        sql = "SELECT [Material Inventory].* FROM [Material Inventory]"

        Try

            m_cnMaterial.ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & PathCMDB()
            m_cnMaterial.Open()
            m_daMaterial = New OleDb.OleDbDataAdapter(sql, m_cnMaterial)
            m_cbMaterial = New OleDb.OleDbCommandBuilder(m_daMaterial)
            m_cbMaterial.QuotePrefix = "["
            m_cbMaterial.QuoteSuffix = "]"
            m_daMaterial.Fill(m_dtMaterial)

            drNewRow("Type ID") = nTheMaterialType
            drNewRow("Description") = sNewMaterialName
            drNewRow("Length") = 0
            drNewRow("Width") = 0
            drNewRow("Thickness") = 0
            drNewRow("Weight") = 0
            drNewRow("Quantity") = 0
            drNewRow("Cost Per Unit") = 0


            m_dtMaterial.Rows.Add(drNewRow)
            m_daMaterial.Update(m_dtMaterial)


            cmdGetID.Connection = m_cnMaterial
            cmdGetID.CommandText = "SELECT MAX(ID) FROM [Material Inventory]"



            SaveMat = cmdGetID.ExecuteScalar
            m_dtMaterial.Dispose()
            m_daMaterial.Dispose()

            m_cnMaterial.Close()

        Catch ex As Exception

            SaveMat = -1

            MessageBox.Show("SaveMaterial: " & ex.Message)

        End Try



    End Function
    Private Sub CreateLayerSetups()
        'Dim sXLSXFilename As String
        Dim m_cnPart As New OleDb.OleDbConnection
        Dim m_daPart As New OleDb.OleDbDataAdapter
        Dim m_dtPart As New DataTable
        Dim connetionString As String = ""
        Dim sql As String
        Dim cmdGetID As New OleDbCommand
        Dim sFieldNames As String
        'Dim sFieldValues As String

        'Dim sline As String
        'Dim sData As String
        Dim sPieces() As String
        'Dim dtToolSetupTable As DataTable
        'Dim sToolSetupName As String
        'Dim n_ToolSetupID As Long
        'Dim dtCutRec As DataTable
        Dim oledbAdapter As OleDbDataAdapter
        Dim ds As New DataSet
        Dim ds2 As New DataSet
        'Dim sMaterial As String
        Dim sClass As String
        Dim sThickness As String
        Dim sLastMaterial As String
        Dim sLastThickness As String
        Dim m_daToolCrib As New OleDb.OleDbDataAdapter
        Dim rnd1 As New Random
        Dim m_cnToolCrib As New OleDb.OleDbConnection
        Dim Kerf As Double
        Dim myColor(0 To 9) As Color
        Dim ni As Integer
        Dim iColor As Integer
        Dim sColor As String
        Dim nID As Long
        Dim lMyStationID As Long = 820
        Dim dFeedrate As Double
        Dim drRow As DataRow
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        Dim sDiameter As String
        Dim sPierceHeight As String
        Dim sPierceDelay As String
        Dim sCutHeight As String
        Dim sParamsFile As String = "C:\1wittlockeng\Customers\longsin\ProcessParameters\Plasma.Params"
        Dim sParamsText As String
        'Dim sMyFile As IO.StreamWriter
        'Dim ni2 As Integer
        Dim connection As New OleDbConnection
        Dim sLayerSetupName As String
        Dim m_dtLayerSetup As New DataTable
        Dim drNewRow As DataRow = m_dtLayerSetup.NewRow()
        Dim m_cnLayerSetup As New OleDb.OleDbConnection
        'Dim m_daLayerSetup As OleDb.OleDbDataAdapter
        'Dim m_cbLayerSetup As OleDb.OleDbCommandBuilder
        Dim n_LayerSetupID As Long


        sql = "Select [Tool Setups].* From [Tool Setups] Where ([Tool Setups].ID > 0 ) Order By [Tool Setups].Description"

        connection = New OleDbConnection(connetionString)
        connection.Open()

        oledbAdapter = New OleDbDataAdapter(sql, connection)

        oledbAdapter.Fill(ds)

        oledbAdapter.Dispose()

        connection.Close()

        For i = 0 To ds.Tables(0).Rows.Count - 1

            sLayerSetupName = ds.Tables(0).Rows(i).Item("Description")
            n_LayerSetupID = SaveLayerSetup(sLayerSetupName, 9)
        Next

        ds.Dispose()

        connection.Close()

    End Sub
    Private Function SaveLayerSetup(ByVal sAddModeString As String, ByVal lMachineId As Long) As Long
        Dim m_cnLayerSetup As New OleDb.OleDbConnection
        Dim m_daLayerSetup As OleDb.OleDbDataAdapter
        Dim m_cbLayerSetup As OleDb.OleDbCommandBuilder
        Dim m_dtLayerSetup As New DataTable
        Dim sql As String
        Dim ds As New DataSet
        Dim drNewRow As DataRow = m_dtLayerSetup.NewRow()
        Dim cmdGetID As New OleDbCommand
        Dim DEFAULT_TOL = 0.0001

        sql = "SELECT [Layer Setups].* FROM [Layer Setups]"

        Try

            m_cnLayerSetup.ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & PathCMDB()
            m_cnLayerSetup.Open()
            m_daLayerSetup = New OleDb.OleDbDataAdapter(sql, m_cnLayerSetup)
            m_cbLayerSetup = New OleDb.OleDbCommandBuilder(m_daLayerSetup)
            m_cbLayerSetup.QuotePrefix = "["
            m_cbLayerSetup.QuoteSuffix = "]"
            m_daLayerSetup.Fill(m_dtLayerSetup)

            drNewRow("Description") = sAddModeString
            drNewRow("Machine ID") = lMachineId
            drNewRow("Z Level Mode") = 1
            drNewRow("Sharp Angle") = 60
            drNewRow("Clean Tolerance") = DEFAULT_TOL
            drNewRow("Gap Tolerance") = DEFAULT_TOL
            drNewRow("Filter Tolerance") = DEFAULT_TOL
            drNewRow("ProcessText") = False
            drNewRow("Restrict_Offset") = False


            m_dtLayerSetup.Rows.Add(drNewRow)
            m_daLayerSetup.Update(m_dtLayerSetup)


            cmdGetID.Connection = m_cnLayerSetup
            cmdGetID.CommandText = "SELECT MAX(ID) FROM [Layer Setups]"



            CreateLayer(cmdGetID.ExecuteScalar)

            m_dtLayerSetup.Dispose()
            m_daLayerSetup.Dispose()

            m_cnLayerSetup.Close()

            If m_cnLayerSetup IsNot Nothing Then m_cnLayerSetup = Nothing
            If m_daLayerSetup IsNot Nothing Then m_daLayerSetup = Nothing
            If m_dtLayerSetup IsNot Nothing Then m_dtLayerSetup = Nothing


        Catch ex As Exception

            SaveLayerSetup = -1

            MessageBox.Show("SaveMachine: " & ex.Message)

        End Try

    End Function

    Private Sub CreateLayer(m_layerSetupID As Long)
        Dim m_cnLayerSetup As New OleDb.OleDbConnection
        Dim m_daLayerSetup As OleDb.OleDbDataAdapter
        Dim m_cbLayerSetup As OleDb.OleDbCommandBuilder
        Dim m_dtLayerSetup As New DataTable
        Dim sql As String
        Dim ds As New DataSet
        Dim drNewRow As DataRow = m_dtLayerSetup.NewRow()
        Dim cmdGetID As New OleDbCommand
        Dim n_LayerNameID As Long
        'Dim n_MachineID As Integer

        sql = "Select Directives.* From Directives"

        Try


            m_cnLayerSetup.ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & PathCMDB()
            m_cnLayerSetup.Open()
            m_daLayerSetup = New OleDb.OleDbDataAdapter(sql, m_cnLayerSetup)
            m_cbLayerSetup = New OleDb.OleDbCommandBuilder(m_daLayerSetup)
            m_cbLayerSetup.QuotePrefix = "["
            m_cbLayerSetup.QuoteSuffix = "]"
            m_daLayerSetup.Fill(m_dtLayerSetup)


            drNewRow("Layer Setup ID") = m_layerSetupID
            drNewRow("CAD Layer") = "0"
            drNewRow("CAM Layer") = "CAM Layer"
            drNewRow("Z Level") = 0
            drNewRow("Color") = 16776960
            drNewRow("Station") = "1"
            drNewRow("Cut Direction") = 2
            drNewRow("Cut Side") = 1
            drNewRow("Distance") = 0



            m_dtLayerSetup.Rows.Add(drNewRow)
            m_daLayerSetup.Update(m_dtLayerSetup)


            cmdGetID.Connection = m_cnLayerSetup
            cmdGetID.CommandText = "SELECT MAX(ID) FROM [Directives]"

            n_LayerNameID = cmdGetID.ExecuteScalar

            m_dtLayerSetup.Dispose()


            If m_cnLayerSetup IsNot Nothing Then m_cnLayerSetup = Nothing
            If m_daLayerSetup IsNot Nothing Then m_daLayerSetup = Nothing
            If m_dtLayerSetup IsNot Nothing Then m_dtLayerSetup = Nothing

        Catch ex As Exception

            MessageBox.Show(ex.Message)

        End Try
    End Sub

    Private Sub SimpleButton2_Click(sender As Object, e As EventArgs) Handles SimpleButton2.Click
        Dim sql As String
        Dim ds As New DataSet
        Dim s_ToolSetupDescrip As String
        'Dim n_ToolSetupID As Integer
        Dim i As Integer
        Dim dtToolSetups As DataTable = Nothing

        s_ToolSetupDescrip = ""

        sql = "Select [Tool Setups].* From [Tool Setups] Where ([Tool Setups].[Machine ID] = 9)"

        Try
            dtToolSetups = ConnectToCMDB(sql)

            For i = 0 To dtToolSetups.Rows.Count - 1

                s_ToolSetupDescrip = dtToolSetups.Rows(i).Item("Description")

                SaveLayerSetup(s_ToolSetupDescrip, 9)
            Next

            dtToolSetups.Dispose()

        Catch ex As Exception

            MessageBox.Show("FillToolSetups: " & ex.Message)

        End Try

    End Sub

    Private Sub SimpleButton3_Click(sender As Object, e As EventArgs) Handles sbToolSetups.Click
        Dim cat As ADOX.Catalog = New ADOX.Catalog()
        Dim Gag_FileSave As New SaveFileDialog
        'dtGags = New DataTable()
        Dim sTablename As String = "Gags"
        Dim sFieldNames As String
        Dim sFieldValues As String
        Dim sMaterial As String
        Dim sClass As String
        Dim sThickness As String
        Dim sLastMaterial As String
        Dim sLastThickness As String
        Dim Kerf As Double
        Dim dFeedrate As Double
        Dim sDiameter As String
        Dim sPierceHeight As String
        Dim sPierceDelay As String
        Dim sCutHeight As String
        Dim myColor(0 To 9) As Color
        Dim rnd1 As New Random
        Dim iColor As Integer
        Dim sColor As String
        Dim ni As Integer
        Dim sToolSetupName As String
        Dim n_ToolSetupID As Long
        Dim sql As String
        Dim m_dtToolSetup As New DataTable
        Dim m_daSettings As New OleDb.OleDbDataAdapter
        'Dim dtToolSetupTable As DataTable
        Dim m_cnToolCrib As New OleDb.OleDbConnection
        Dim connetionString As String

        sDiameter = "0"
        sLastMaterial = ""
        sLastThickness = "0"

        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        m_cnToolCrib = New OleDbConnection(connetionString)

        m_cnToolCrib.Open()


        sql = "Select PLasma_Params.* From PLasma_Params Where (PLasma_Params.ID > 0 ) Order By PLasma_Params.sMaterial,sClass,sThickness"

        sFieldNames = "[Description]," &
                      "[Machine ID]"

        m_dtToolSetup = ConnectToAndy(sql)

        For i = 0 To m_dtToolSetup.Rows.Count - 1

            If ni > 10 Then ni = 0

            iColor = myColor(ni).ToArgb()
            sColor = iColor.ToString
            ' myColor = Color.FromArgb(iColor)

            If IsDBNull(m_dtToolSetup.Rows(i).Item("sMaterial")) = True Then
                GoTo ExitEarly
            End If

            sMaterial = m_dtToolSetup.Rows(i).Item("sMaterial")
            sClass = UCase(m_dtToolSetup.Rows(i).Item("sClass"))
            sThickness = m_dtToolSetup.Rows(i).Item("sThickness")
            Kerf = m_dtToolSetup.Rows(i).Item("Kerf")
            dFeedrate = m_dtToolSetup.Rows(i).Item("dFeedrate")


            If IsDBNull(m_dtToolSetup.Rows(i).Item("sDiameter")) = True Then

                sDiameter = "0"
            ElseIf sDiameter = "*" Then
                sDiameter = 0
            Else
                sDiameter = m_dtToolSetup.Rows(i).Item("sDiameter")

            End If

            If IsDBNull(m_dtToolSetup.Rows(i).Item("sPierceHeight")) = True Then
                sPierceHeight = "0"
            Else
                sPierceHeight = m_dtToolSetup.Rows(i).Item("sPierceHeight")

            End If

            If IsDBNull(m_dtToolSetup.Rows(i).Item("sPierceTime")) = True Then
                sPierceDelay = "0"
            Else
                sPierceDelay = m_dtToolSetup.Rows(i).Item("sPierceTime")

            End If

            If IsDBNull(m_dtToolSetup.Rows(i).Item("sCutHeight")) = True Then
                sCutHeight = "0"
            Else
                sCutHeight = m_dtToolSetup.Rows(i).Item("sCutHeight")

            End If

            If sMaterial = sLastMaterial And sThickness = sLastThickness Then
                GoTo ExitEarly
            End If

            sLastMaterial = sMaterial
            sLastThickness = sThickness

            If CDbl(sThickness) < 0.125 Then

                GoTo ExitEarly

            End If


            If Kerf = 0.09 Then

                GoTo ExitEarly

            End If

            'sToolSetupName = sMaterial & "_" & CStr(sThickness) & "_" & sClass & "_F" & CStr(dFeedrate)

            If (InStr(sClass, "AIR/AIR")) Then

                sToolSetupName = sMaterial & "_" & CStr(sThickness) & "_" & sClass & "_F" & dFeedrate
                ''dtToolSetupTable = ConnectToCMDB(sql)

                If DoesToolSetupExist(sToolSetupName) = True Then
                    GoTo ExitEarly
                End If

                sFieldValues = "'" & sToolSetupName & "','9" & "'"

                sql = "INSERT INTO [Tool Setups] (" & sFieldNames & " ) VALUES(" & sFieldValues & ")"

                m_daSettings.InsertCommand = New OleDbCommand(sql, m_cnToolCrib)

                m_daSettings.InsertCommand.ExecuteNonQuery()



                'drRow = dtToolSetupTable.NewRow()

                'drRow("Machine ID") = 8
                'drRow("Description") = sToolSetupName

                'dtToolSetupTable.Rows.Add(drRow)
                'm_daCMDB.Update(dtToolSetupTable)

                'dtToolSetupTable.Dispose()

                n_ToolSetupID = GetNewID("Tool Setups")
            End If


ExitEarly:
            ni = ni + 1
            If ni = 10 Then
                ni = 0

            End If

        Next

        m_daSettings.Dispose()

        m_cnToolCrib.Dispose()
        m_dtToolSetup.Dispose()

    End Sub

End Class