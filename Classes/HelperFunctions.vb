Imports Newtonsoft.Json.Linq
Imports FabV25_WIN8.CadEntity

Public Class HelperFunctions
    Public Class JsonColumnDef
        Public Property Field As String
        Public Property Header As String
        Public Property Visible As Boolean = True
        Public Property DataType As Integer? = Nothing

        Public Sub New(field As String, header As String,
                   Optional visible As Boolean = True,
                   Optional dataType As Integer? = Nothing)
            Me.Field = field
            Me.Header = header
            Me.Visible = visible
            Me.DataType = dataType
        End Sub
    End Class
    ''' <summary>
    ''' Creates a RectangleF from two points, automatically normalizing
    ''' so width and height are positive, regardless of point order.
    ''' </summary>
    Public Shared Function RectFromPoints(p1 As PointF, p2 As PointF) As RectangleF
        Dim x As Single = Math.Min(p1.X, p2.X)
        Dim y As Single = Math.Min(p1.Y, p2.Y)
        Dim w As Single = Math.Abs(p2.X - p1.X)
        Dim h As Single = Math.Abs(p2.Y - p1.Y)

        Return New RectangleF(x, y, w, h)
    End Function


    Public Function RectFromWorldBounds(
    minX As Single,
    minY As Single,
    maxX As Single,
    maxY As Single,
    transform As ViewTransform,
    panelHeight As Integer
) As RectangleF

        ' Convert world min/max to screen coordinates
        Dim p1 As PointF = WorldToScreen(minX, minY, transform, panelHeight)
        Dim p2 As PointF = WorldToScreen(maxX, maxY, transform, panelHeight)

        ' Use safe rectangle creation
        Return RectFromPoints(p1, p2)
    End Function

    ''' <summary>
    ''' Builds a JSON Columns array from a DataTable.
    ''' Each column will have Field, Header, Visible (True), and optional DataType (0 by default).
    ''' </summary>
    Public Function BuildColumns(dt As DataTable, Optional includeDataType As Boolean = True) As JArray
        Dim columns As New JArray()

        For Each col As DataColumn In dt.Columns
            Dim colObj As New JObject()
            colObj("Field") = col.ColumnName.Replace(" ", "")
            colObj("Header") = col.ColumnName
            colObj("Visible") = True
            If includeDataType Then
                colObj("DataType") = 0
            End If
            columns.Add(colObj)
        Next

        Return columns
    End Function

    Public Shared Function GetCadBounds(entities As List(Of CadEntity)) As RectangleF

        Dim minX As Single = Single.MaxValue
        Dim minY As Single = Single.MaxValue
        Dim maxX As Single = Single.MinValue
        Dim maxY As Single = Single.MinValue

        For Each e In entities

            If TypeOf e Is CadLine Then
                Dim l = CType(e, CadLine)
                UpdateBounds(l.StartPoint, minX, minY, maxX, maxY)
                UpdateBounds(l.EndPoint, minX, minY, maxX, maxY)

            ElseIf TypeOf e Is CadArc Then
                Dim a = CType(e, CadArc)

                ' Conservative: full circle bounds
                Dim r As Single = CSng(a.Radius)
                UpdateBounds(New PointF(a.Center.X - r, a.Center.Y - r), minX, minY, maxX, maxY)
                UpdateBounds(New PointF(a.Center.X + r, a.Center.Y + r), minX, minY, maxX, maxY)
            End If

        Next

        If minX = Single.MaxValue Then
            Return RectangleF.Empty
        End If

        Return RectangleF.FromLTRB(minX, minY, maxX, maxY)

    End Function

    Private Shared Sub UpdateBounds(
    p As PointF,
    ByRef minX As Single, ByRef minY As Single,
    ByRef maxX As Single, ByRef maxY As Single)

        minX = Math.Min(minX, p.X)
        minY = Math.Min(minY, p.Y)
        maxX = Math.Max(maxX, p.X)
        maxY = Math.Max(maxY, p.Y)

    End Sub


End Class
