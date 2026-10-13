Imports System.Drawing
Imports System.Globalization
Imports System.IO

Namespace ToolShapes

    Public Class CustomToolShape
        Inherits ToolShape
        Public ReadOnly Property Filename As String
        Public ReadOnly Property Length As Double
        Public ReadOnly Property Width As Double

        Private Sub New(
            cuttingProfile As IPath2D,
            filename As String,
            length As Double,
            width As Double)

            MyBase.New(
                ToolShapeType.Custom,
                cuttingProfile)

            Me.Filename = filename
            Me.Length = length
            Me.Width = width
        End Sub

        Public Shared Function FromFile(
    fileName As String,
    fallbackLength As Double,
    fallbackWidth As Double) As CustomToolShape

            Dim profile As IPath2D = Nothing

            If Not String.IsNullOrWhiteSpace(fileName) AndAlso
       File.Exists(fileName) Then

                profile = LoadCtgProfile(fileName)
            End If

            If profile Is Nothing Then
                profile = CreateFallbackRectangle(
            fallbackLength,
            fallbackWidth)
            End If

            Return New CustomToolShape(
        profile,
        fileName,
        fallbackLength,
        fallbackWidth)
        End Function
        Public Overrides ReadOnly Property DisplayName As String
        Get
            Return "Custom"
        End Get
    End Property

    Public Overrides ReadOnly Property EffectiveDiameter As Double
        Get
            Return Math.Max(
                CuttingProfile.BoundingBox.Width,
                CuttingProfile.BoundingBox.Height)
        End Get
    End Property

    Public Overrides ReadOnly Property EffectiveRadius As Double
        Get
            Return EffectiveDiameter / 2.0
        End Get
    End Property

    Public Overrides Function CanCutInternalCorner(
        radius As Double) As Boolean

        Return radius >= MinInternalCornerRadius
    End Function

    Public Overrides Function CanSlot(
        width As Double) As Boolean

        Return width >= 0.0
    End Function

    Public Overrides Function GetProfileOffset() As Double
        Return 0.0
    End Function

    Public Overrides Function DecomposePath(
        inputPath As IPath2D) As IEnumerable(Of IToolMotion)

        Throw New NotImplementedException(
            "CustomToolShape path decomposition has not been implemented.")
    End Function

        Private Shared Function LoadCtgProfile(
    fileName As String) As IPath2D

            Dim records As New List(Of CtgRecord)

            Using reader As New StreamReader(fileName)

                While Not reader.EndOfStream

                    Dim line As String = reader.ReadLine()

                    If String.IsNullOrWhiteSpace(line) Then
                        Continue While
                    End If

                    line = line.Trim()

                    If line.StartsWith("#", StringComparison.Ordinal) Then
                        Continue While
                    End If

                    Dim parts() As String =
                line.Split(
                    New Char() {" "c, ChrW(9)},
                    StringSplitOptions.RemoveEmptyEntries)

                    If parts.Length < 3 Then
                        Throw New FormatException(
                    "Invalid CTG record: " & line)
                    End If

                    Dim recordType As Integer

                    If Not Integer.TryParse(
                parts(0),
                NumberStyles.Integer,
                CultureInfo.InvariantCulture,
                recordType) Then

                        Throw New FormatException(
                    "Invalid CTG record type: " & line)
                    End If

                    Dim x As Double
                    Dim y As Double

                    If Not Double.TryParse(
                parts(1),
                NumberStyles.Float,
                CultureInfo.InvariantCulture,
                x) Then

                        Throw New FormatException(
                    "Invalid CTG X coordinate: " & line)
                    End If

                    If Not Double.TryParse(
                parts(2),
                NumberStyles.Float,
                CultureInfo.InvariantCulture,
                y) Then

                        Throw New FormatException(
                    "Invalid CTG Y coordinate: " & line)
                    End If

                    records.Add(
                New CtgRecord(
                    recordType,
                    New PointF(
                        CSng(x),
                        CSng(y))))
                End While
            End Using

            If records.Count = 0 Then
                Throw New InvalidDataException(
            "Custom CTG file contains no geometry.")
            End If

            Dim segments As New List(Of IPathSegment)

            Dim currentPoint As PointF = Nothing
            Dim haveCurrentPoint As Boolean = False

            Dim index As Integer = 0

            While index < records.Count

                Dim record As CtgRecord = records(index)

                Select Case record.Type

                    Case 0
                        '==================================================
                        ' Start a new independent contour.
                        '
                        ' Legacy CTG type 0 is NOT a NOP.
                        ' It establishes the current/start point of
                        ' a contour.
                        '==================================================

                        currentPoint = record.Point
                        haveCurrentPoint = True

                        index += 1

                    Case 1
                        '==================================================
                        ' Line endpoint.
                        '
                        ' Current point -> record point.
                        '==================================================

                        If Not haveCurrentPoint Then
                            Throw New InvalidDataException(
                        "CTG line endpoint has no starting point.")
                        End If

                        segments.Add(
                    New LineSegment2D(
                        currentPoint,
                        record.Point))

                        currentPoint = record.Point

                        index += 1

                    Case 2, 3
                        '==================================================
                        ' Arc center.
                        '
                        ' Current point = arc start
                        ' Current record = arc center
                        ' Next record = arc endpoint
                        '
                        ' Type 2 = clockwise
                        ' Type 3 = counterclockwise
                        '==================================================

                        If Not haveCurrentPoint Then
                            Throw New InvalidDataException(
                        "CTG arc center has no starting point.")
                        End If

                        If index + 1 >= records.Count Then
                            Throw New InvalidDataException(
                        "CTG arc record is missing its endpoint.")
                        End If

                        Dim startPoint As PointF =
                    currentPoint

                        Dim center As PointF =
                    record.Point

                        Dim endPoint As PointF =
                    records(index + 1).Point

                        Dim radius As Double =
                    Distance(startPoint, center)

                        If radius <= 0.0 Then
                            Throw New InvalidDataException(
                        "CTG arc has zero radius.")
                        End If

                        Dim startAngle As Double =
                    Math.Atan2(
                        startPoint.Y - center.Y,
                        startPoint.X - center.X)

                        Dim endAngle As Double =
                    Math.Atan2(
                        endPoint.Y - center.Y,
                        endPoint.X - center.X)

                        Dim isClockwise As Boolean =
                    (record.Type = 2)

                        If isClockwise Then

                            If endAngle >= startAngle Then
                                endAngle -= 2.0 * Math.PI
                            End If

                        Else

                            If endAngle <= startAngle Then
                                endAngle += 2.0 * Math.PI
                            End If

                        End If

                        segments.Add(
                    New ArcSegment2D(
                        center,
                        radius,
                        startAngle,
                        endAngle,
                        isClockwise))

                        ' The endpoint record has been consumed.
                        currentPoint = endPoint

                        index += 2

                    Case Else
                        Throw New InvalidDataException(
                    "Unsupported CTG record type: " &
                    record.Type.ToString(
                        CultureInfo.InvariantCulture))

                End Select

            End While

            Dim path As New Path2D()

            For Each segment As IPathSegment In segments
                path.Add(segment)
            Next

            Return path

        End Function

        Private Shared Function CreateFallbackRectangle(
        length As Double,
        width As Double) As IPath2D

        If length <= 0.0 Then
            Throw New ArgumentOutOfRangeException(
                NameOf(length),
                "Custom tool Length must be greater than zero.")
        End If

        If width <= 0.0 Then
            Throw New ArgumentOutOfRangeException(
                NameOf(width),
                "Custom tool Width must be greater than zero.")
        End If

        Dim halfLength As Double = length / 2.0
        Dim halfWidth As Double = width / 2.0

        Dim p1 As New PointF(
            CSng(-halfLength),
            CSng(-halfWidth))

        Dim p2 As New PointF(
            CSng(halfLength),
            CSng(-halfWidth))

        Dim p3 As New PointF(
            CSng(halfLength),
            CSng(halfWidth))

        Dim p4 As New PointF(
            CSng(-halfLength),
            CSng(halfWidth))

        Dim segments As New List(Of IPathSegment) From {
            New LineSegment2D(p1, p2),
            New LineSegment2D(p2, p3),
            New LineSegment2D(p3, p4),
            New LineSegment2D(p4, p1)
        }

            Dim path As New Path2D()

            For Each segment As IPathSegment In segments
                path.Add(segment)
            Next

            Return path
        End Function

    Private Shared Function Distance(
        p1 As PointF,
        p2 As PointF) As Double

        Dim dx As Double = p1.X - p2.X
        Dim dy As Double = p1.Y - p2.Y

        Return Math.Sqrt(
            (dx * dx) +
            (dy * dy))
    End Function

        Private Class CtgRecord

            Public ReadOnly Property Type As Integer
            Public ReadOnly Property Point As PointF

            Public Sub New(
        recordType As Integer,
        point As PointF)

                Me.Type = recordType
                Me.Point = point
            End Sub

        End Class

    End Class

End Namespace