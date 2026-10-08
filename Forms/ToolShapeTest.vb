Imports System.Linq
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models

Imports FabV25_WIN8.ToolShapes

Public Class ToolShapeTest

    Private _testToolShape As ToolShape
    Private Sub ToolShapeTest_Load(sender As Object, e As EventArgs) Handles MyBase.Load

        cboToolShape.Items.Clear()

        cboToolShape.Items.Add("Round")
        cboToolShape.Items.Add("Square")
        cboToolShape.Items.Add("Rectangle")
        cboToolShape.Items.Add("Obround")
        cboToolShape.Items.Add("Diamond")
        cboToolShape.Items.Add("Corner Radius")
        cboToolShape.Items.Add("Single-D")
        cboToolShape.Items.Add("Double-D")
        cboToolShape.Items.Add("Trapezoid")
        cboToolShape.Items.Add("Keyhole")
        cboToolShape.Items.Add("Hexagon")
        cboToolShape.Items.Add("Custom")

        cboToolShape.SelectedIndex = 0


    End Sub


    Private Sub cboToolShape_SelectedIndexChanged(
    sender As Object,
    e As EventArgs) Handles cboToolShape.SelectedIndexChanged

        ' Hide all parameter controls first.
        lblSize.Visible = False
        txtSize.Visible = False

        lblLength.Visible = False
        txtLength.Visible = False

        lblWidth.Visible = False
        txtWidth.Visible = False

        lblHeight.Visible = False
        txtHeight.Visible = False

        lblDiameter.Visible = False
        txtDiameter.Visible = False

        lblCornerRadius.Visible = False
        txtCornerRadius.Visible = False

        lblAngle.Visible = False
        txtAngle.Visible = False

        lblMajorDiameter.Visible = False
        txtMajorDiameter.Visible = False

        lblMinorDiameter.Visible = False
        txtMinorDiameter.Visible = False

        lblRadius.Visible = False
        txtRadius.Visible = False
        lblWeb.Visible = False
        txtWeb.Visible = False
        txtFilename.Visible = False

        Select Case cboToolShape.Text

            Case "Round"

                ShowParameter(
                lblDiameter,
                txtDiameter)

            Case "Square"

                ShowParameter(
                lblSize,
                txtSize)
                ShowParameter(lblCornerRadius, txtCornerRadius)

            Case "Rectangle"

                ShowParameter(
                lblLength,
                txtLength)
                ShowParameter(lblCornerRadius, txtCornerRadius)

                ShowParameter(
                lblWidth,
                txtWidth)

            Case "Obround"

                ShowParameter(
                lblLength,
                txtLength)

                ShowParameter(
                lblWidth,
                txtWidth)

            Case "Diamond"

                ShowParameter(
                lblWidth,
                txtWidth)

                ShowParameter(
                lblHeight,
                txtHeight)
                ShowParameter(lblCornerRadius, txtCornerRadius)

            Case "Corner Radius"
                ShowParameter(lblRadius, txtRadius)
                ShowParameter(lblWeb, txtWeb)
                ShowParameter(lblWidth, txtWidth)

            Case "Single-D"

                ShowParameter(
                lblDiameter,
                txtDiameter)

                ShowParameter(
                lblWidth,
                txtWidth)

            Case "Double-D"

                ShowParameter(
                lblDiameter,
                txtDiameter)

                ShowParameter(
                lblWidth,
                txtWidth)

            Case "Trapezoid"

                ShowParameter(
                lblLength,
                txtLength)

                ShowParameter(
                lblWidth,
                txtWidth)

                ShowParameter(
                lblAngle,
                txtAngle)
                ShowParameter(lblCornerRadius, txtCornerRadius)

            Case "Keyhole"

                ShowParameter(
                lblLength,
                txtLength)

                ShowParameter(
                lblMajorDiameter,
                txtMajorDiameter)

                ShowParameter(
                lblMinorDiameter,
                txtMinorDiameter)

            Case "Hexagon"

                ShowParameter(
                lblDiameter,
                txtDiameter)

            Case "Custom"
                ShowParameter(
               lblLength,
               txtLength)

                ShowParameter(
                lblWidth,
                txtWidth)

                ShowParameter(
               lblFilename,
               txtFilename)

        End Select

    End Sub


    Private Sub ShowParameter(
    labelControl As Label,
    textControl As TextBox)

        labelControl.Visible = True
        textControl.Visible = True

    End Sub

    Private Function CreateTestToolShape() As ToolShape

        Select Case cboToolShape.Text

            Case "Round"

                Return New RoundToolShape(
                    CDbl(txtDiameter.Text))

            Case "Square"

                Return New SquareToolShape(
                    CDbl(txtSize.Text),
                    CDbl(txtCornerRadius.Text))

            Case "Rectangle"

                Return New RectangularToolShape(
                    CDbl(txtLength.Text),
                    CDbl(txtWidth.Text),
                    CDbl(txtCornerRadius.Text))

            Case "Obround"

                Return New ObroundToolShape(
                    CDbl(txtWidth.Text),
                    CDbl(txtLength.Text))

            Case "Diamond"
                Return New DiamondToolShape(
                    CDbl(txtWidth.Text),
                    CDbl(txtHeight.Text),
                    CDbl(txtCornerRadius.Text))


            Case "Corner Radius"

                Return New CornerRadiusToolShape(
        CDbl(txtRadius.Text),
        CDbl(txtWeb.Text),
        CDbl(txtWidth.Text))

            Case "Single-D"

                Return New SingleDToolShape(
                    CDbl(txtDiameter.Text),
                    CDbl(txtWidth.Text))

            Case "Double-D"

                Return New DoubleDToolShape(
                    CDbl(txtDiameter.Text),
                    CDbl(txtWidth.Text))

            Case "Trapezoid"

                Return New TrapezoidToolShape(
        CDbl(txtLength.Text),
        CDbl(txtWidth.Text),
        CDbl(txtAngle.Text),
        CDbl(txtCornerRadius.Text))

            Case "Keyhole"

                Return New KeyholeToolShape(
                    CDbl(txtLength.Text),
                    CDbl(txtMajorDiameter.Text),
                    CDbl(txtMinorDiameter.Text))

            Case "Hexagon"

                Return New HexagonToolShape(
                    CDbl(txtDiameter.Text))

            Case "Custom"

                Return CustomToolShape.FromFile(
                    txtFilename.Text,
                    CDbl(txtLength.Text),
                    CDbl(txtWidth.Text))

            Case Else

                Throw New InvalidOperationException(
                    "No ToolShape is defined for the selected shape.")

        End Select

    End Function

    Private Sub cmdOK_Click(sender As Object, e As EventArgs) Handles cmdOK.Click


        Try



            '_testToolShape = CreateTestToolShape()

            _testToolShape = CreateToolShapeFromToolCrib()

            pnlToolShape.Invalidate()


            Dim customShape As CustomToolShape =
    TryCast(_testToolShape, CustomToolShape)

            Dim filename As String = ""
            Dim length As String = ""
            Dim width As String = ""

            If customShape IsNot Nothing Then
                filename = If(customShape.Filename, "")
                length = customShape.Length.ToString()
                width = customShape.Width.ToString()
            End If

            MessageBox.Show(
    "ToolShape created successfully." &
    Environment.NewLine &
    Environment.NewLine &
    "Shape: " & _testToolShape.ShapeType.ToString() &
    Environment.NewLine &
    "Closed: " & _testToolShape.CuttingProfile.IsClosed.ToString() &
    Environment.NewLine &
    "Segments: " &
        _testToolShape.CuttingProfile.Segments.Count().ToString() &
    Environment.NewLine &
    Environment.NewLine &
    "Filename: " & filename &
    Environment.NewLine &
    "Length: " & length &
    Environment.NewLine &
    "Width: " & width,
    "ToolShape Test",
    MessageBoxButtons.OK,
    MessageBoxIcon.Information)

        Catch ex As Exception

            MessageBox.Show(
            ex.Message,
            "ToolShape Test Error",
            MessageBoxButtons.OK,
            MessageBoxIcon.Error)

        End Try


    End Sub

    Private Sub btnTestShape_Click(sender As Object, e As EventArgs) Handles btnTestShape.Click

        Try

            _testToolShape = CreateTestToolShape()

            pnlToolShape.Invalidate()

            MessageBox.Show(
            "ToolShape created successfully." &
            Environment.NewLine &
            Environment.NewLine &
            "Shape: " & _testToolShape.ShapeType.ToString() &
            Environment.NewLine &
            "Closed: " & _testToolShape.CuttingProfile.IsClosed.ToString() &
            Environment.NewLine &
            "Segments: " & _testToolShape.CuttingProfile.Segments.Count().ToString(),
            "ToolShape Test",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)

        Catch ex As Exception

            MessageBox.Show(
            ex.Message,
            "ToolShape Test Error",
            MessageBoxButtons.OK,
            MessageBoxIcon.Error)

        End Try

    End Sub

    Private Sub pnlToolShape_Paint(
    sender As Object,
    e As PaintEventArgs) Handles pnlToolShape.Paint

        Dim g As Graphics = e.Graphics

        g.Clear(Color.White)

        ' Coordinate origin.
        Dim originX As Single =
        pnlToolShape.ClientSize.Width / 2.0F

        Dim originY As Single =
        pnlToolShape.ClientSize.Height / 2.0F

        Using axisPen As New Pen(Color.LightGray, 1.0F)

            ' X axis.
            g.DrawLine(
            axisPen,
            0,
            originY,
            pnlToolShape.ClientSize.Width,
            originY)

            ' Y axis.
            g.DrawLine(
            axisPen,
            originX,
            0,
            originX,
            pnlToolShape.ClientSize.Height)

        End Using

        ' Reference center.
        Using centerBrush As New SolidBrush(Color.Red)

            g.FillEllipse(
            centerBrush,
            originX - 3.0F,
            originY - 3.0F,
            6.0F,
            6.0F)

        End Using

        If _testToolShape IsNot Nothing Then

            DrawToolShape(
                g,
                _testToolShape,
                originX,
                originY,
                100.0F)

        End If

    End Sub

    Private Sub DrawToolShape(
    g As Graphics,
    shape As ToolShape,
    originX As Single,
    originY As Single,
    scale As Single)

        If shape Is Nothing Then
            Return
        End If

        Using profilePen As New Pen(Color.Blue, 2.0F)

            For Each segment As IPathSegment In
                shape.CuttingProfile.Segments

                Select Case segment.SegmentType

                    Case PathSegmentType.Line

                        Dim startPoint As PointF =
                            segment.StartPoint

                        Dim endPoint As PointF =
                            segment.EndPoint

                        Dim screenStart As New PointF(
                            originX + CSng(startPoint.X * scale),
                            originY - CSng(startPoint.Y * scale))

                        Dim screenEnd As New PointF(
                            originX + CSng(endPoint.X * scale),
                            originY - CSng(endPoint.Y * scale))

                        g.DrawLine(
                            profilePen,
                            screenStart,
                            screenEnd)

                    Case PathSegmentType.Arc

                        Dim arc As ArcSegment2D =
                            DirectCast(segment, ArcSegment2D)

                        Dim center As PointF =
                            arc.Center

                        Dim diameter As Single =
                            CSng(arc.Radius * 2.0 * scale)

                        Dim left As Single =
                            originX +
                            CSng((center.X - arc.Radius) * scale)

                        Dim top As Single =
                            originY -
                            CSng((center.Y + arc.Radius) * scale)

                        ' Arc angles are stored in radians.
                        ' Graphics.DrawArc uses degrees.
                        Dim startAngle As Single =
                            CSng(-arc.StartAngle * 180.0 / Math.PI)

                        Dim sweepAngle As Single =
                            CSng(
                                -(arc.EndAngle - arc.StartAngle) *
                                180.0 / Math.PI)

                        g.DrawArc(
                            profilePen,
                            left,
                            top,
                            diameter,
                            diameter,
                            startAngle,
                            sweepAngle)

                End Select

            Next

        End Using

    End Sub
    Private Function CreateToolShapeFromToolCrib() As ToolShape

        Dim tool As Tool =
        AppData.ToolCrib.FirstOrDefault(
            Function(t) t.TypeID = 29)

        If tool Is Nothing Then
            Throw New InvalidOperationException(
            "No Type 29 Custom tool was found in ToolCrib.json.")
        End If

        Return ToolShapeFactory.Create(tool)

    End Function
    Private Sub btnTestLineIntersections_Click(
    sender As Object,
    e As EventArgs) Handles btnTestLineIntersections.Click

        Try

            Dim segments As New List(Of IPathSegment)

            ' Diagonal A: (0,0) -> (10,10)
            segments.Add(
    New LineSegment2D(
        New PointF(0.0F, 0.0F),
        New PointF(10.0F, 10.0F)))

            ' Diagonal B: (0,10) -> (10,0)
            segments.Add(
    New LineSegment2D(
        New PointF(0.0F, 10.0F),
        New PointF(10.0F, 0.0F)))

            Dim intersections As List(Of WmIntersectionRecord) = Nothing
            Dim curveRecords As List(Of WmCurveRecord) = Nothing

            Dim doesIntersect As Boolean =
            ConvexHullGeometry.BuildLineIntersectionRecords(
                segments,
                0.000001,
                intersections,
                curveRecords)

            Dim message As String =
            "Adjacent Shared Endpoint Test" &
            Environment.NewLine &
            Environment.NewLine &
            "Does Intersect: " &
            doesIntersect.ToString() &
            Environment.NewLine &
            "Intersection Records: " &
            intersections.Count.ToString() &
            Environment.NewLine &
            "Curve Records: " &
            curveRecords.Count.ToString() &
            Environment.NewLine &
            Environment.NewLine

            For i As Integer = 0 To curveRecords.Count - 1

                Dim record As WmCurveRecord =
                curveRecords(i)

                message &= "Curve " &
                       i.ToString() &
                       " Records: " &
                       record.Count.ToString() &
                       Environment.NewLine

                For j As Integer = 0 To record.Count - 1

                    Dim ext As WmCurveRecordExtData =
                    record.ExtData(j)

                    message &= "    U=" &
                           ext.UParam.ToString("0.000000") &
                           "  Point=(" &
                           ext.Point.X.ToString("0.000000") &
                           ", " &
                           ext.Point.Y.ToString("0.000000") &
                           ")" &
                           Environment.NewLine

                Next

            Next

            MessageBox.Show(
            message,
            "Line Intersection Test",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)

        Catch ex As Exception

            MessageBox.Show(
            ex.ToString(),
            "Line Intersection Test Error",
            MessageBoxButtons.OK,
            MessageBoxIcon.Error)

        End Try

    End Sub
    Private Sub btnTestLineArcIntersection_Click(
    sender As Object,
    e As EventArgs) Handles btnTestLineArcIntersection.Click

        Dim arc As New ArcSegment2D(
        New PointF(5.0F, 5.0F),
        5.0,
        0.0,
        Math.PI,
        False)

        Dim line As New LineSegment2D(
        New PointF(0.0F, 5.0F),
        New PointF(10.0F, 5.0F))

        Dim points As List(Of PointF)
        Dim uArc As List(Of Double)
        Dim uLine As List(Of Double)

        Dim result As Boolean =
        ConvexHullGeometry.TestIntersectArcLine(
            arc,
            line,
            0.000001,
            points,
            uArc,
            uLine)

        Debug.WriteLine("")
        Debug.WriteLine("========== LINE / ARC TEST ==========")
        Debug.WriteLine("Does Intersect: " & result)
        Debug.WriteLine("Intersection Count: " & points.Count)

        For index As Integer = 0 To points.Count - 1

            Debug.WriteLine(
            "Point " & index &
            " = (" &
            points(index).X.ToString("F6") &
            ", " &
            points(index).Y.ToString("F6") &
            ")" &
            "  Arc U = " &
            uArc(index).ToString("F6") &
            "  Line U = " &
            uLine(index).ToString("F6"))

        Next

    End Sub

    Private Sub btnTestLineArcRecords_Click(
    sender As Object,
    e As EventArgs) Handles btnTestLineArcRecords.Click

        Dim segments As New List(Of IPathSegment)

        ' Line through the center of the arc.
        segments.Add(
    New LineSegment2D(
        New PointF(-1.0F, 5.0F),
        New PointF(11.0F, 5.0F)))

        ' Upper semicircle.
        segments.Add(
        New ArcSegment2D(
            New PointF(5.0F, 5.0F),
            5.0,
            0.0,
            Math.PI,
            False))

        Dim intersections As List(Of WmIntersectionRecord)
        Dim curveRecords As List(Of WmCurveRecord)

        Dim doesIntersect As Boolean =
        ConvexHullGeometry.BuildLineArcIntersectionRecords(
            segments,
            0.000001,
            intersections,
            curveRecords)

        Debug.WriteLine("")
        Debug.WriteLine("========== LINE / ARC RECORD TEST ==========")
        Debug.WriteLine(
        "Does Intersect: " &
        doesIntersect.ToString())

        Debug.WriteLine(
        "Intersection Records: " &
        intersections.Count.ToString())

        For i As Integer = 0 To intersections.Count - 1

            Dim record As WmIntersectionRecord =
            intersections(i)

            Debug.WriteLine(
            "Intersection " &
            i.ToString() &
            " = (" &
            record.Point.X.ToString("F6") &
            ", " &
            record.Point.Y.ToString("F6") &
            ")" &
            "  References = " &
            record.Count.ToString())

        Next

        Debug.WriteLine(
        "Curve Records: " &
        curveRecords.Count.ToString())

        For i As Integer = 0 To curveRecords.Count - 1

            Dim curveRecord As WmCurveRecord =
            curveRecords(i)

            Debug.WriteLine("")
            Debug.WriteLine(
            "Curve " &
            i.ToString() &
            "  SegmentIndex = " &
            curveRecord.SegmentIndex.ToString() &
            "  Count = " &
            curveRecord.Count.ToString())

            For j As Integer = 0 To curveRecord.Count - 1

                Dim extData As WmCurveRecordExtData =
                curveRecord.ExtData(j)

                Debug.WriteLine(
                "    U = " &
                extData.UParam.ToString("F6") &
                "  Point = (" &
                extData.Point.X.ToString("F6") &
                ", " &
                extData.Point.Y.ToString("F6") &
                ")" &
                "  Intersection = " &
                (extData.Intersection IsNot Nothing).ToString())

            Next

        Next

        Debug.WriteLine(
        "============================================")

    End Sub
    Private Sub btnTestArcArcIntersection_Click(
    sender As Object,
    e As EventArgs) Handles btnTestArcArcIntersection.Click

        Dim arcA As New ArcSegment2D(
        New PointF(0.0F, 0.0F),
        5.0,
        0.0,
        2.0 * Math.PI,
        False)

        Dim arcB As New ArcSegment2D(
        New PointF(6.0F, 0.0F),
        5.0,
        0.0,
        2.0 * Math.PI,
        False)

        Dim points As List(Of PointF)
        Dim uA As List(Of Double)
        Dim uB As List(Of Double)

        Dim result As Boolean =
        ConvexHullGeometry.TestIntersectArcArc(
            arcA,
            arcB,
            0.000001,
            points,
            uA,
            uB)

        Debug.WriteLine("")
        Debug.WriteLine("========== ARC / ARC TEST ==========")
        Debug.WriteLine(
        "Does Intersect: " &
        result.ToString())

        Debug.WriteLine(
        "Intersection Count: " &
        points.Count.ToString())

        For index As Integer = 0 To points.Count - 1

            Debug.WriteLine(
            "Point " &
            index.ToString() &
            " = (" &
            points(index).X.ToString("F6") &
            ", " &
            points(index).Y.ToString("F6") &
            ")" &
            "  Arc A U = " &
            uA(index).ToString("F6") &
            "  Arc B U = " &
            uB(index).ToString("F6"))

        Next

        Debug.WriteLine(
        "====================================")

    End Sub
    Private Sub btnTestArcArcRecords_Click(
    sender As Object,
    e As EventArgs) Handles btnTestArcArcRecords.Click

        Dim segments As New List(Of IPathSegment)

        segments.Add(
        New ArcSegment2D(
            New PointF(0.0F, 0.0F),
            5.0,
            0.0,
            2.0 * Math.PI,
            False))

        segments.Add(
        New ArcSegment2D(
            New PointF(6.0F, 0.0F),
            5.0,
            0.0,
            2.0 * Math.PI,
            False))

        Dim intersections As List(Of WmIntersectionRecord)
        Dim curveRecords As List(Of WmCurveRecord)

        Dim doesIntersect As Boolean =
        ConvexHullGeometry.BuildArcArcIntersectionRecords(
            segments,
            0.000001,
            intersections,
            curveRecords)

        Debug.WriteLine("")
        Debug.WriteLine("========== ARC / ARC RECORD TEST ==========")
        Debug.WriteLine(
        "Does Intersect: " &
        doesIntersect.ToString())

        Debug.WriteLine(
        "Intersection Records: " &
        intersections.Count.ToString())

        For i As Integer = 0 To intersections.Count - 1

            Dim record As WmIntersectionRecord =
            intersections(i)

            Debug.WriteLine(
            "Intersection " &
            i.ToString() &
            " = (" &
            record.Point.X.ToString("F6") &
            ", " &
            record.Point.Y.ToString("F6") &
            ")" &
            "  References = " &
            record.Count.ToString())

        Next

        Debug.WriteLine(
        "Curve Records: " &
        curveRecords.Count.ToString())

        For i As Integer = 0 To curveRecords.Count - 1

            Dim curveRecord As WmCurveRecord =
            curveRecords(i)

            Debug.WriteLine("")
            Debug.WriteLine(
            "Curve " &
            i.ToString() &
            "  SegmentIndex = " &
            curveRecord.SegmentIndex.ToString() &
            "  Count = " &
            curveRecord.Count.ToString())

            For j As Integer = 0 To curveRecord.Count - 1

                Dim extData As WmCurveRecordExtData =
                curveRecord.ExtData(j)

                Debug.WriteLine(
                "    U = " &
                extData.UParam.ToString("F6") &
                "  Point = (" &
                extData.Point.X.ToString("F6") &
                ", " &
                extData.Point.Y.ToString("F6") &
                ")" &
                "  Intersection = " &
                (extData.Intersection IsNot Nothing).ToString())

            Next

        Next

        Debug.WriteLine(
        "============================================")

    End Sub
    Private Sub btnTestAllIntersections_Click(
    sender As Object,
    e As EventArgs) Handles btnTestAllIntersections.Click

        Dim segments As New List(Of IPathSegment)

        ' ------------------------------------------------------------
        ' 0 - Line / Line
        ' ------------------------------------------------------------

        segments.Add(
        New LineSegment2D(
            New PointF(0.0F, 0.0F),
            New PointF(10.0F, 10.0F)))

        segments.Add(
        New LineSegment2D(
            New PointF(0.0F, 10.0F),
            New PointF(10.0F, 0.0F)))

        ' ------------------------------------------------------------
        ' 2 - Line / Arc
        ' ------------------------------------------------------------

        segments.Add(
        New LineSegment2D(
            New PointF(-1.0F, 20.0F),
            New PointF(11.0F, 20.0F)))

        segments.Add(
        New ArcSegment2D(
            New PointF(5.0F, 20.0F),
            5.0,
            0.0,
            Math.PI,
            False))

        ' ------------------------------------------------------------
        ' 4 - Arc / Arc
        ' ------------------------------------------------------------

        segments.Add(
        New ArcSegment2D(
            New PointF(0.0F, 40.0F),
            5.0,
            0.0,
            2.0 * Math.PI,
            False))

        segments.Add(
        New ArcSegment2D(
            New PointF(6.0F, 40.0F),
            5.0,
            0.0,
            2.0 * Math.PI,
            False))

        Dim intersections As List(Of WmIntersectionRecord)
        Dim curveRecords As List(Of WmCurveRecord)

        Dim doesIntersect As Boolean =
        ConvexHullGeometry.BuildIntersectionRecords(
            segments,
            0.000001,
            intersections,
            curveRecords)

        Debug.WriteLine("")
        Debug.WriteLine("========== ALL INTERSECTION TEST ==========")

        Debug.WriteLine(
        "Does Intersect: " &
        doesIntersect.ToString())

        Debug.WriteLine(
        "Segment Count: " &
        segments.Count.ToString())

        Debug.WriteLine(
        "Intersection Records: " &
        intersections.Count.ToString())

        For i As Integer = 0 To intersections.Count - 1

            Dim record As WmIntersectionRecord =
            intersections(i)

            Debug.WriteLine(
            "Intersection " &
            i.ToString() &
            " = (" &
            record.Point.X.ToString("F6") &
            ", " &
            record.Point.Y.ToString("F6") &
            ")" &
            "  References = " &
            record.Count.ToString())

        Next

        Debug.WriteLine(
        "Curve Records: " &
        curveRecords.Count.ToString())

        For i As Integer = 0 To curveRecords.Count - 1

            Dim curveRecord As WmCurveRecord =
            curveRecords(i)

            Debug.WriteLine("")
            Debug.WriteLine(
            "Curve " &
            i.ToString() &
            "  SegmentIndex = " &
            curveRecord.SegmentIndex.ToString() &
            "  Count = " &
            curveRecord.Count.ToString())

            For j As Integer = 0 To curveRecord.Count - 1

                Dim extData As WmCurveRecordExtData =
                curveRecord.ExtData(j)

                Debug.WriteLine(
                "    U = " &
                extData.UParam.ToString("F6") &
                "  Point = (" &
                extData.Point.X.ToString("F6") &
                ", " &
                extData.Point.Y.ToString("F6") &
                ")" &
                "  Intersection = " &
                (extData.Intersection IsNot Nothing).ToString())

            Next

        Next

        Debug.WriteLine(
        "===========================================")

    End Sub
End Class