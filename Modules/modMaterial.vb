Imports System.Data.OleDb
Imports FabV25_WIN8.AppData

Module modMaterial
    Public myTransform As ViewTransform
    Public Function CheckMaterialWeight(ByRef materialID As Integer) As Double
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim sql As String


        'CheckMaterialWeight = 0 'assume failure

        'Try

        '    connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        '    sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].ID = " & materialID & " )"

        '    connection = New OleDbConnection(connetionString)

        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        CheckMaterialWeight = CDbl(ds.Tables(0).Rows(i).Item("Weight").ToString)
        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("GetMaterialWeight " & ex.Message)

        'End Try



    End Function


    Public Function GetMaterialLength(sheets As List(Of MaterialSheet), ByVal targetSheetId As Long) As Double

        ' Use LINQ to find the sheet by ID, then find the parameter named "Width"
        Dim sheet = sheets.FirstOrDefault(Function(s) s.ID = targetSheetId)

        If sheet IsNot Nothing Then
            Dim widthParam = sheet.Values.FirstOrDefault(Function(p) p.Name.Equals("Length", StringComparison.OrdinalIgnoreCase))

            If widthParam IsNot Nothing Then
                Return widthParam.Value
            End If
        End If

        Return Nothing
    End Function

    Public Function GetMaterialThickness(sheets As List(Of MaterialSheet), ByVal targetSheetId As Long) As Double

        ' Use LINQ to find the sheet by ID, then find the parameter named "Width"
        Dim sheet = sheets.FirstOrDefault(Function(s) s.ID = targetSheetId)

        If sheet IsNot Nothing Then
            Dim widthParam = sheet.Values.FirstOrDefault(Function(p) p.Name.Equals("Thickness", StringComparison.OrdinalIgnoreCase))

            If widthParam IsNot Nothing Then
                Return widthParam.Value
            End If
        End If

        Return Nothing
    End Function
    Public Function GetMaterialCost(ByVal lMaterialID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "SELECT * FROM [Material Inventory] WHERE [ID] = " & lMaterialID

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        GetMaterialCost = ds.Tables(0).Rows(i).Item("Cost Per Unit")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return GetMaterialCost

    End Function

    Public Function GetMaterialQty(ByVal lMaterialID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "SELECT * FROM [Material Inventory] WHERE [ID] = " & lMaterialID

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        GetMaterialQty = ds.Tables(0).Rows(i).Item("Quantity")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return GetMaterialQty

    End Function

    Public Function GetMaterialCategoryNameAndID() As String
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer
        'Dim nTempmaterialID As Long
        'Dim nTempMaterialInventoryID As Long
        'Dim sMaterialDescrip As String


        'GetMaterialCategoryNameAndID = ""

        'sMaterialDescrip = HeaderStringGet("MatCfg", "")

        'If (sMaterialDescrip <> "") Then

        '    nTempmaterialID = modTranslate.GetMaterialIDFromDescription(sMaterialDescrip)

        'End If

        'nTempMaterialInventoryID = GetMaterialType(nTempmaterialID)

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Types].* From [Material Types] Where ([Material Types].ID = " & nTempMaterialInventoryID & " )"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        GetMaterialCategoryNameAndID = ds.Tables(0).Rows(i).Item("Display") & "," & nTempmaterialID

        '    Next

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try



    End Function
    Public Function GetMaterialType(ByVal lMaterialID As Long) As Long

        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "SELECT * FROM [Material Inventory] WHERE [ID] = " & lMaterialID

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        GetMaterialType = ds.Tables(0).Rows(i).Item("Type ID")

        '    Next

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return GetMaterialType

    End Function


    Public Function GetMaterialWidth(sheets As List(Of MaterialSheet), ByVal targetSheetId As Long) As Double

        ' Use LINQ to find the sheet by ID, then find the parameter named "Width"
        Dim sheet = sheets.FirstOrDefault(Function(s) s.ID = targetSheetId)

        If sheet IsNot Nothing Then
            Dim widthParam = sheet.Values.FirstOrDefault(Function(p) p.Name.Equals("Width", StringComparison.OrdinalIgnoreCase))

            If widthParam IsNot Nothing Then
                Return widthParam.Value
            End If
        End If

        Return Nothing
    End Function
    Public Function WorldToScreen(x As Double,
                               y As Double,
                               vt As ViewTransform,
                               panelHeight As Integer) As PointF

        Dim sx As Single = CSng(x * vt.Scale + vt.OffsetX)
        Dim sy As Single = CSng(panelHeight - (y * vt.Scale + vt.OffsetY))

        Return New PointF(sx, sy)
    End Function


    Public Sub DrawMaterial(sheets As List(Of MaterialSheet), ByVal panelHandle As IntPtr, matWid As Double, matLength As Double, x As Integer, y As Integer)
        '    Dim margin As Integer = 20



        '    Dim scaleX As Double = (panelWidth - 2 * margin) / matLength
        '    Dim scaleY As Double = (panelHeight - 2 * margin) / matWid
        '    Dim scale As Double = Math.Min(scaleX, scaleY)

        '    Dim offsetX As Double = (panelWidth - matLength * scale) / 2
        '    Dim offsetY As Double = (panelHeight - matWid * scale) / 2

        '    'myTransform = ViewTransform
        '    myTransform = New ViewTransform With {
        '    .Scale = scale,
        '    .OffsetX = offsetX,
        '    .OffsetY = offsetY
        '}

        '    ' World-space corners
        '    Dim bl As PointF = WorldToScreen(0, 0, myTransform, panelHeight)
        '    Dim tr As PointF = WorldToScreen(matLength, matWid, myTransform, panelHeight)

        '    ' Build rectangle safely
        '    x = Math.Min(bl.X, tr.X)
        '    y = Math.Min(bl.Y, tr.Y)
        '    Dim w As Single = Math.Abs(tr.X - bl.X)
        '    Dim h As Single = Math.Abs(tr.Y - bl.Y)
        '    Debug.WriteLine($"ViewTransform Scale={myTransform.Scale}, ViewTransform OffsetX={myTransform.OffsetX} ViewTransform OffsetY = {myTransform.OffsetY} ")
        '    Debug.WriteLine($"bl={bl}, tr={tr}")
        '    Debug.WriteLine($"scaleX={scaleX}, scaleY={scaleY} scale = {scale} offsetX= {offsetX} offsetY={offsetX}")

        '    Debug.WriteLine($"PanelWidth={panelWidth}, Height={panelHeight} MatWidth = {matWid} matLength= {matLength}")
        '    Debug.WriteLine($"Material X={x}, Y={y} W = {w} H= {h}")

        '    Dim rectF = HelperFunctions.RectFromPoints(topLeft, bottomRight)

        '    Debug.WriteLine($"Screen rect W={rectF.Width}, H={rectF.Height}")

        '    Using g As Graphics = Graphics.FromHwnd(panelHandle)
        '        ' Example: Drawing a red rectangle
        '        Using p As New Pen(Color.Red, 2)
        '            g.DrawRectangle(p, Rectangle.Round(rectF))
        '        End Using

        '        '' Example: Drawing text
        '        'Using f As New Font("Arial", 12)
        '        '    g.DrawString("Drawn via Handle", f, Brushes.Blue, 15, 15)
        '        'End Using
        '    End Using



        '    'Using Pen As New Pen(Color.CadetBlue, 1)
        '    'g.DrawRectangle(pen, Rectangle.Round(rectF))
        '    'End Using


        '    ' Material dimensions in inches
        '    'Dim materialWidth As Double = 96
        '    'Dim materialHeight As Double = 48

        '    '' Panel size
        '    'Dim panelWidth As Integer = frmMain.pnlPicmodeler.Width
        '    'Dim panelHeight As Integer = frmMain.pnlPicmodeler.Height

        '    '' Compute scale to fit material inside panel with a margin
        '    'Dim margin As Double = 20 ' pixels
        '    'Dim scaleX As Double = (panelWidth - 2 * margin) / matLength
        '    'Dim scaleY As Double = (panelHeight - 2 * margin) / matWid
        '    'Dim scale As Double = Math.Min(scaleX, scaleY)

        '    '' Compute offsets to center the material in the panel
        '    'Dim offsetX As Double = (panelWidth - matLength * scale) / 2
        '    'Dim offsetY As Double = (panelHeight - matWid * scale) / 2

        '    '' Convert world (material) coordinates to screen
        '    'Dim topLeft As New PointF(CSng(0 * scale + offsetX), CSng(panelHeight - (matWid * scale + offsetY)))
        '    'Dim bottomRight As New PointF(CSng(matLength * scale + offsetX), CSng(panelHeight - (0 * scale + offsetY)))

        '    '' Build rectangle
        '    'Dim rectF As New RectangleF(topLeft.X, topLeft.Y, bottomRight.X - topLeft.X, bottomRight.Y - topLeft.Y)

        '    ' Draw the material
        '    ' Using pen As New Pen(Color.CadetBlue, 2)
        '    'g.DrawRectangle(pen, rectF.X, rectF.Y, rectF.Width, rectF.Height)
        '    'End Using

    End Sub
    Public Function GetMatlTypeIDFromTypesDescription(sDescrip As String) As Long
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Types].* From [Material Types] Where ([Material Types].Description = '" & sDescrip & "' )"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        GetMatlTypeIDFromTypesDescription = ds.Tables(0).Rows(i).Item("ID")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing


        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return GetMatlTypeIDFromTypesDescription

    End Function

    Public Function GetMaterialTypeIDFromDisplay(sDisplay As String) As Long
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        ''sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].Description = '" & sDescrip & "' )"
        'sql = "Select [Material Types].* From [Material Types] Where ([Material Types].Display = '" & sDisplay & "' )"


        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        GetMaterialTypeIDFromDisplay = ds.Tables(0).Rows(i).Item("ID")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing


        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return GetMaterialTypeIDFromDisplay
    End Function


    Public Function GetMaterialTypeIDFromDescription(sDescrip As String) As Long
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].Description = '" & sDescrip & "' )"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        GetMaterialTypeIDFromDescription = ds.Tables(0).Rows(i).Item("Type ID")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing


        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return GetMaterialTypeIDFromDescription

    End Function
    Public Function GetNewMaterialNameWithType(n_MaterialID As Long) As String
        'Dim ds As New DataSet
        'Dim sMatType As String
        'Dim nMaterialTypeID As Integer
        'Dim sMatSdescrip As String

        'GetNewMaterialNameWithType = ""

        'Try



        '    sMatSdescrip = GetMaterialDescription(n_MaterialID)
        '    nMaterialTypeID = newGetMaterialTypeIDFromMaterialID(n_MaterialID)
        '    sMatType = GetMaterialTypeName(nMaterialTypeID)

        '    'MsgBox("The materialID is < " & n_MaterialID & " >")
        '    'MsgBox("Material Type ID is < " & nMaterialTypeID & " ")
        '    'MsgBox("The Material Type name is < " & sMatType & " >")
        '    'MsgBox("The material description is < " & sMatSdescrip & " >")

        '    GetNewMaterialNameWithType = sMatType & "," & sMatSdescrip

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try
    End Function

    Public Function GetMaterialTypeName(nMaterialTypeID) As String
        'Dim sql As String

        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'GetMaterialTypeName = ""

        'Try
        '    connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        '    sql = "Select [Material Types].* From [Material Types] Where ([Material Types].ID = " & nMaterialTypeID & " )"


        '    connection = New OleDbConnection(connetionString)

        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        GetMaterialTypeName = ds.Tables(0).Rows(i).Item("Description")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try




    End Function

    Public Function Get1stMaterialLength(ByVal m_MaterialTYpeID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & m_MaterialTYpeID & ")"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        Get1stMaterialLength = ds.Tables(0).Rows(i).Item("Length")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return Get1stMaterialLength


    End Function

    Public Function Get1stMaterialWidth(ByVal m_MaterialTYpeID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & m_MaterialTYpeID & ")"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        Get1stMaterialWidth = ds.Tables(0).Rows(i).Item("Width")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return Get1stMaterialWidth


    End Function

    Public Function Get1stMaterialThickness(ByVal m_MaterialTYpeID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & m_MaterialTYpeID & ")"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        Get1stMaterialThickness = ds.Tables(0).Rows(i).Item("Thickness")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return Get1stMaterialThickness


    End Function

    Public Function Get1stMaterialDescrip(ByVal m_MaterialTYpeID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & m_MaterialTYpeID & ")"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        Get1stMaterialDescrip = ds.Tables(0).Rows(i).Item("Description")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return Get1stMaterialDescrip


    End Function

    Public Function Get1stMaterialWeight(ByVal m_MaterialTYpeID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & m_MaterialTYpeID & ")"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        Get1stMaterialWeight = ds.Tables(0).Rows(i).Item("Weight")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return Get1stMaterialWeight


    End Function

    Public Function Get1stMaterialCost(ByVal m_MaterialTYpeID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & m_MaterialTYpeID & ")"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        Get1stMaterialCost = ds.Tables(0).Rows(i).Item("Cost Per Unit")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return Get1stMaterialCost


    End Function

    Public Function Get1stMaterialID(ByVal m_MaterialTYpeID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & m_MaterialTYpeID & ")"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        Get1stMaterialID = ds.Tables(0).Rows(i).Item("ID")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return Get1stMaterialID


    End Function

    Public Function Get1stMaterialQty(ByVal m_MaterialTYpeID As Long) As Double
        'Dim sql As String
        'Dim ds As New DataSet
        'Dim connection As OleDbConnection
        'Dim oledbAdapter As OleDbDataAdapter
        'Dim connetionString As String
        'Dim i As Integer

        'connetionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source='" & PathCMDB() & "';"

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[Type ID] = " & m_MaterialTYpeID & ")"

        'connection = New OleDbConnection(connetionString)

        'Try
        '    connection.Open()

        '    oledbAdapter = New OleDbDataAdapter(sql, connection)

        '    oledbAdapter.Fill(ds)

        '    oledbAdapter.Dispose()

        '    connection.Close()

        '    For i = 0 To ds.Tables(0).Rows.Count - 1

        '        Get1stMaterialQty = ds.Tables(0).Rows(i).Item("Quantity")

        '    Next

        '    ds.Dispose()

        '    If ds IsNot Nothing Then ds = Nothing

        '    If oledbAdapter IsNot Nothing Then oledbAdapter = Nothing

        '    If connection IsNot Nothing Then connection = Nothing

        'Catch ex As Exception

        '    MessageBox.Show("Can not open connection ! ")

        'End Try

        'Return Get1stMaterialQty


    End Function
    Public Function GetMaterialWeightFromPDB(nMaterialID) As Double
        'Dim nMatID As Long
        'Dim sql As String
        'Dim dtMaterialType As DataTable = Nothing

        ''MessageBox.Show("The Material Id is < " & nMaterialID & " > ")

        'sql = "Select [Material Inventory].* From [Material Inventory] Where ([Material Inventory].[ID] = " & nMaterialID & ")"
        'dtMaterialType = ConnectToCMDB(sql)

        'If (dtMaterialType.Rows.Count > 0) Then

        '    For i = 0 To dtMaterialType.Rows.Count - 1
        '        GetMaterialWeightFromPDB = dtMaterialType.Rows(i).Item("Weight")
        '        'MessageBox.Show("The Material Weight is < " & dtMaterialType.Rows(i).Item("Weight") & " > ")
        '    Next

        'End If

    End Function
End Module

