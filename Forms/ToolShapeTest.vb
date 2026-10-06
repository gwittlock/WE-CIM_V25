
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

            _testToolShape = CreateTestToolShape()

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

    Private Sub btnTestShape_MouseWheel(sender As Object, e As MouseEventArgs) Handles btnTestShape.MouseWheel

    End Sub
End Class