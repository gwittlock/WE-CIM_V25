Imports System.Data.OleDb

Module modTooling

    Public g_colToolSetup As ToolSetup
    Public g_clsActiveToolData As ToolData
    Public g_DescripChar_len As Integer
    Public g_num_Pierces As Integer
    Public g_CutTravel As Double

    Public Function DoesToolSetupExist(Optional ByVal sToolSetupName As String = "") As Boolean
        Dim ds As New DataSet
        Dim connection As OleDbConnection
        Dim oledbAdapter As OleDbDataAdapter
        Dim connetionString As String
        Dim sql As String



        sql = "Select [Tool Setups].* From [Tool Setups] Where ([Tool Setups].Description = '" & sToolSetupName & "' )"

        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "SELECT * FROM Icon WHERE (Icon.ToolBarID = 1500 )"

        connection = New OleDbConnection(connetionString)
        connection.Open()

        oledbAdapter = New OleDbDataAdapter(sql, connection)

        oledbAdapter.Fill(ds)


        If (ds.Tables(0).Rows.Count > 0) Then
            DoesToolSetupExist = True
        Else
            DoesToolSetupExist = False
        End If

        ds.Dispose()

        oledbAdapter.Dispose()

        connection.Close()



    End Function
    Public Sub GetToolsFromModel()

        Dim clsToolData As ToolData
        Dim lCount As Integer
        Dim lCounter As Integer
        Dim lEntityID As Integer
        Dim sTemp As String
        Dim lTypeID As Integer
        Dim MyConnection As OleDbConnection = OpenTheCmdb()


        g_colToolSetup = New ToolSetup

        lReturn = PortalExecute("*Model:EntityCount:type=" & c_DBTOOL)

        lCount = PortalGetInt("count", 0)

        For lCounter = 0 To (lCount - 1)

            lReturn = PortalExecute("*Model:Get2:indx=" & lCounter & ",type=" & c_DBTOOL)
            lEntityID = PortalGetInt("id", 0)

            'NOTE: Only tools have a Type_ID attribute.
            lTypeID = EntityIntGet(lEntityID, "Type_ID", 0)
            If (lTypeID > 0) Then

                sTemp = EntityStringGet(lEntityID, "NC_Code_Number", "0")

                ' add new tool to collection
                clsToolData = g_colToolSetup.Add("T" & sTemp) ' use as key into collection

                clsToolData.type_id = lTypeID
                clsToolData.NC_Code_Number = CInt(sTemp)
                clsToolData.Model_ID = lEntityID

                ' fill in the tool information
                clsToolData.Angle = EntityDoubleGet(lEntityID, "Angle", 0)
                clsToolData.Auto_Index = EntityIntGet(lEntityID, "Auto_Index", 0)
                clsToolData.TlColor = EntityIntGet(lEntityID, "Color", System.Drawing.ColorTranslator.ToOle(System.Drawing.Color.White))
                clsToolData.Corner_Radius = EntityDoubleGet(lEntityID, "Corner_Radius", 0)
                clsToolData.cost = EntityDoubleGet(lEntityID, "Cost", 0)
                clsToolData.Description = EntityStringGet(lEntityID, "Description", "<error>")

                ' we want to get the diameter value now as other values may overwrite it later
                ' we use this property to represent the diameter of the tool regardless of
                ' the actual property to make life easier throughout the system
                ' when a kerf for example gets updated then it will also update the width
                clsToolData.Kerf = EntityDoubleGet(lEntityID, "Kerf", 0)

                If (lTypeID > 8 And lTypeID < 15) Then
                    clsToolData.Diameter = EntityDoubleGet(lEntityID, "Kerf", 0)
                    clsToolData.Width_Diameter = clsToolData.Diameter
                Else
                    clsToolData.Diameter = EntityDoubleGet(lEntityID, "Diameter", 0)
                    clsToolData.Width_Diameter = EntityDoubleGet(lEntityID, "Width", 0)
                End If


                clsToolData.Doff = EntityIntGet(lEntityID, "Doff", 0)
                clsToolData.FeedMode = EntityIntGet(lEntityID, "Feed_Mode", 0)
                clsToolData.Filename = EntityStringGet(lEntityID, "Filename", "")
                clsToolData.Fixed_Station = EntityIntGet(lEntityID, "Fixed_Station", 0)
                clsToolData.Id = EntityIntGet(lEntityID, "ID", 0)
                clsToolData.Index_Angle = EntityDoubleGet(lEntityID, "Index_Angle", 0)
                clsToolData.Kerf = EntityDoubleGet(lEntityID, "Kerf", 0)
                clsToolData.Length = EntityDoubleGet(lEntityID, "Length", 0)
                clsToolData.Life = EntityDoubleGet(lEntityID, "Life", 0)
                clsToolData.Loff = EntityIntGet(lEntityID, "Loff", 0)
                clsToolData.Major_Diameter = EntityDoubleGet(lEntityID, "Major_Diameter", 0)
                clsToolData.Minor_Diameter = EntityDoubleGet(lEntityID, "Minor_Diameter", 0)
                clsToolData.Pitch = EntityDoubleGet(lEntityID, "Pitch", 0)
                clsToolData.Radius = EntityDoubleGet(lEntityID, "Radius", 0)
                clsToolData.Shear = EntityDoubleGet(lEntityID, "Shear", 0)
                clsToolData.Station_Location_X = EntityDoubleGet(lEntityID, "Station_Location_X", 0)
                clsToolData.Station_Location_Y = EntityDoubleGet(lEntityID, "Station_Location_Y", 0)
                clsToolData.Station_Size = EntityDoubleGet(lEntityID, "Station_Size", 0)
                clsToolData.Type_Description = EntityStringGet(lEntityID, "Type", "<error>")
                clsToolData.Web = EntityDoubleGet(lEntityID, "Web", 0)
                'clsToolData.Width_Diameter = EntityDoubleGet(lEntityID, "Width", 0)
                clsToolData.Workplane = UCase(EntityStringGet(lEntityID, "Workplane", "<error>"))
                clsToolData.PrimaryCode = UCase(EntityStringGet(lEntityID, "Primary_Code", "0"))
                clsToolData.SecondaryCode = UCase(EntityStringGet(lEntityID, "Secondary_Code", "0"))


                    'GetToolOperations(clsToolData, MyConnection)
                    'GetMaterialAttributes(clsToolData, MyConnection)
                End If

        Next lCounter

        ' '' now lets get the tool operations
        For Each clsToolData In g_colToolSetup
            GetToolOperations(clsToolData, MyConnection)
        Next clsToolData


        '' now lets get the material attributes
        For Each clsToolData In g_colToolSetup
            GetMaterialAttributes(clsToolData, MyConnection)
        Next clsToolData
        clsToolData = Nothing
        MyConnection.Close()

        MyConnection.Dispose()

    End Sub

    Public Function OpenTheCmdb() As OleDbConnection
        Dim connection As OleDbConnection

        Dim connetionString As String

        connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        connection = New OleDbConnection(connetionString)

        connection.Open()

        OpenTheCmdb = connection

    End Function
    Public Sub GetToolOperations(ByRef clsToolData As ToolData, connection As OleDbConnection)
        Dim sql As String
        Dim ds As New DataSet
        Dim oledbAdapter As OleDbDataAdapter
        Dim cmd As OleDbCommand = Nothing
        Dim m_table As DataTable
        Dim Table_Name As String
        Dim row As DataRow
        Dim sDescription As String = ""
        Dim clsOperation As Operation
        Dim bDoAdd As Boolean = True
        Dim ni As Long

        Table_Name = "toolOps"

        sql = "SELECT [Operation Types].*" & " FROM [Tool Types] INNER JOIN ([Operation Types] INNER JOIN [Tool Operations] ON [Operation Types].ID = [Tool Operations].[Operation Type ID]) ON [Tool Types].ID = [Tool Operations].[Tool Type ID]" & " WHERE ((([Tool Types].Description) = '" & clsToolData.Type_Description & "'))"

        Try

            oledbAdapter = New OleDbDataAdapter(sql, connection)

            oledbAdapter.Fill(ds, Table_Name)

            m_table = ds.Tables(Table_Name)

            For Each row In m_table.Rows
                bDoAdd = True

                sDescription = row("Description")

                For ni = 1 To clsToolData.ToolOps.Count

                    If (clsToolData.ToolOps.Item(ni).Description = sDescription) Then

                        bDoAdd = False

                        Exit For

                    End If

                Next

                If (bDoAdd = True) Then
                    ' add new operation to collection
                    clsOperation = clsToolData.ToolOps.Add(sDescription)

                    clsOperation.Description = sDescription
                    clsOperation.id = row("ID")
                    clsOperation.Parameters = row("Parameters")
                End If
            Next

            ds.Dispose()

            oledbAdapter.Dispose()

            If ds IsNot Nothing Then ds = Nothing

            If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing


        Catch ex As Exception

            MessageBox.Show("GetToolOperations! " & ex.Message)

        End Try

    End Sub
    Public Sub GetMaterialAttributes(ByRef clsToolData As ToolData, connection As OleDbConnection)
        Dim sql As String
        Dim ds As New DataSet
        Dim oledbAdapter As OleDbDataAdapter
        Dim cmd As OleDbCommand = Nothing
        Dim m_table As DataTable
        Dim Table_Name As String
        Dim row As DataRow
        Dim sDescription As String = ""
        Dim clsOperation As Operation
        Dim materialDesc As String
        Dim materialID As Integer


        On Error Resume Next

        materialDesc = HeaderStringGet("MatCfg", "")
        materialID = modTranslate.GetMaterialIDFromDescription(materialDesc)

        'Table_Name = "mat_attribs"
        Table_Name = "Material Attributes"

        sql = "SELECT [Material Attributes].*" & _
            " FROM [Tool Types] INNER JOIN ([Material Inventory] INNER JOIN [Material Attributes] ON [Material Inventory].ID = " & _
            "[Material Attributes].[Material ID]) ON [Tool Types].ID = [Material Attributes].[Tool Type ID]" & _
            " WHERE ((([Material Attributes].[Diameter Limit]) >= " & clsToolData.Diameter & ")" & " AND (([Material Inventory].ID) = " & _
            materialID & ")" & " AND (([Tool Types].Description) = '" & _
            clsToolData.Type_Description & "'))" & " ORDER BY [Material Attributes].[Diameter Limit]"

        ' Try


        oledbAdapter = New OleDbDataAdapter(sql, connection)

        oledbAdapter.Fill(ds, Table_Name)

        m_table = ds.Tables(Table_Name)

        For Each row In m_table.Rows

            sDescription = row("Description")

            ' add new operation to collection
            clsOperation = clsToolData.ToolOps.Add(sDescription)


            If (clsOperation IsNot Nothing) Then
                clsOperation.Description = clsToolData.ToolOps.mCol.Items(4)
                clsOperation.id = row("ID")
                clsOperation.Parameters = row("Parameters")
            End If

        Next

        ds.Dispose()

        oledbAdapter.Dispose()

        If ds IsNot Nothing Then ds = Nothing

        If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        ' Catch ex As Exception

        ' MessageBox.Show("Can not open connection ! ")

        'End Try
    End Sub

    Public Function GetToolLayer(clsToolData As ToolData) As Long

        Dim tool_name As String

        tool_name = "_tool" & clsToolData.Model_ID

        lReturn = PortalExecute( _
            "*model:get:" & _
            " name=" & QStr(tool_name) & _
            ",lb=" & c_DBTOOL & _
            ",ub=" & c_DBTOOL)

        GetToolLayer = PortalGetInt("id", 0)

    End Function
    Public Function GetTotalPartTravel() As Double


        g_CutTravel = 0
        g_num_Pierces = 0

        lReturn = PortalExecute("Model:StatisticsGet: file=" & QStr(PathCurrent()))
        g_CutTravel = PortalGetDouble("travel", 0)
        g_num_Pierces = PortalGetInt("pierces", 0)

    End Function

    Public Sub SetActiveToolAttributes(lEntityID As Long)

        Dim clsOperation As Operation

        For Each clsOperation In g_clsActiveToolData.ToolOps
            Select Case clsOperation.Description

                Case "Amperage"
                    lReturn = EntityDoubleSet(lEntityID, "amperage", g_clsActiveToolData.Amperage)

                Case "CutQuality"
                    lReturn = EntityIntSet(lEntityID, "cutquality", g_clsActiveToolData.CutQuality)

                Case "CutSide"
                    lReturn = EntityIntSet(lEntityID, "cutside", g_clsActiveToolData.CutSide)

                Case "CutType"
                    lReturn = EntityIntSet(lEntityID, "cuttype", g_clsActiveToolData.CutType)

                Case "Doff"
                    lReturn = EntityIntSet(lEntityID, "doff", g_clsActiveToolData.Doff)

                Case "Feed"
                    lReturn = EntityDoubleSet(lEntityID, "feed", g_clsActiveToolData.Feed)

                Case "FeedMode"
                    lReturn = EntityIntSet(lEntityID, "feedmode", g_clsActiveToolData.FeedMode)

                Case "Loff"
                    lReturn = EntityIntSet(lEntityID, "loff", g_clsActiveToolData.Loff)

                Case "MaxPressure"
                    lReturn = EntityDoubleSet(lEntityID, "maxpressure", g_clsActiveToolData.MaxPressure)

                Case "Plunge"
                    lReturn = EntityDoubleSet(lEntityID, "plunge", g_clsActiveToolData.Plunge)

                Case "Speed"
                    lReturn = EntityDoubleSet(lEntityID, "speed", g_clsActiveToolData.Speed)

            End Select
        Next

        clsOperation = Nothing

    End Sub

    Public Function FindToolByModelID(modelID As Long) As ToolData

        Dim clsToolData As ToolData

        FindToolByModelID = Nothing

        For Each clsToolData In g_colToolSetup
            If (clsToolData.Model_ID = modelID) Then
                FindToolByModelID = clsToolData
                Exit For
            End If
        Next

    End Function

    Public Function FindModelIDFromToolDescrip(sDescrip As String) As Long
        Dim sActualDescrip As String

        Try
            If (InStr(sDescrip, "] ") > 0) Then

                sActualDescrip = Mid(sDescrip, InStr(sDescrip, "] ") + 2)
            Else

                sActualDescrip = sDescrip

            End If
            FindModelIDFromToolDescrip = -1

            Dim clsToolData As ToolData

            For Each clsToolData In g_colToolSetup
                If (clsToolData.Description = sActualDescrip) Then
                    FindModelIDFromToolDescrip = clsToolData.Model_ID
                    Exit For
                End If
            Next


        Catch ex As Exception

        End Try

    End Function


    Public Sub SetToolAttributesOnGroup(clsToolData As ToolData)

        Dim clsOperation As Operation

        For Each clsOperation In clsToolData.ToolOps
            Select Case clsOperation.Description

                Case "Amperage"
                    lReturn = SelectedDoubleSet("amperage", clsToolData.Amperage)

                Case "CutQuality"
                    lReturn = SelectedIntSet("cutquality", clsToolData.CutQuality)

                Case "CutSide"
                    lReturn = SelectedIntSet("cutside", clsToolData.CutSide)

                Case "CutType"
                    lReturn = SelectedIntSet("cuttype", clsToolData.CutType)

                Case "Doff"
                    lReturn = SelectedIntSet("doff", clsToolData.Doff)

                Case "Feed"
                    lReturn = SelectedDoubleSet("feed", clsToolData.Feed)

                Case "FeedMode"
                    lReturn = SelectedIntSet("feedmode", clsToolData.FeedMode)

                Case "Loff"
                    lReturn = SelectedIntSet("loff", clsToolData.Loff)

                Case "MaxPressure"
                    lReturn = SelectedDoubleSet("maxpressure", clsToolData.MaxPressure)

                Case "Plunge"
                    lReturn = SelectedDoubleSet("plunge", clsToolData.Plunge)

                Case "Speed"
                    lReturn = SelectedDoubleSet("speed", clsToolData.Speed)

            End Select
        Next

        lReturn = SelectedIntSet("color", clsToolData.TlColor)

        clsOperation = Nothing

    End Sub


End Module
