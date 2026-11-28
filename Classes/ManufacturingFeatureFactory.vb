Imports System.Drawing
Imports FabV25_WIN8.ToolShapes

Public Class ManufacturingFeatureFactory

    Public Function CreateCircularContour(
        sourceCircle As GeoCircle) As ContourFeature

        If sourceCircle Is Nothing Then
            Throw New ArgumentNullException(NameOf(sourceCircle))
        End If

        If sourceCircle.Center Is Nothing Then
            Throw New ArgumentException(
                "Circle center is not defined.",
                NameOf(sourceCircle))
        End If

        If sourceCircle.Radius <= 0 Then
            Throw New ArgumentOutOfRangeException(
                NameOf(sourceCircle),
                "Circle radius must be greater than zero.")
        End If

        Dim path As New Path2D()

        path.Add(
            New ArcSegment2D(
                New PointF(
                    CSng(sourceCircle.Center.X),
                    CSng(sourceCircle.Center.Y)),
                sourceCircle.Radius,
                0.0,
                Math.PI * 2.0,
                False))

        Return New ContourFeature With {
            .Path = path,
            .Description = "Circular Contour"
        }

    End Function


    Public Function CreateLinearContour(
    x1 As Double,
    y1 As Double,
    x2 As Double,
    y2 As Double) As ContourFeature

        If x1 = x2 AndAlso y1 = y2 Then
            Throw New ArgumentException(
                "Start Point and End Point cannot be identical.")
        End If

        Dim path As New Path2D()

        path.Add(
            New LineSegment2D(
                New PointF(CSng(x1), CSng(y1)),
                New PointF(CSng(x2), CSng(y2))))

        Return New ContourFeature With {
            .Path = path,
            .Description = "Linear Contour",
            .IsLogicallyClosed = False
        }

    End Function

    Public Function CreateLinearContour(
    x1 As Double,
    y1 As Double,
    x2 As Double,
    y2 As Double,
    shape As ToolShape,
    indexAngle As Double,
    cutSide As String,
    distance As Double) As ContourFeature

        If cutSide = "Center" Then
            Return CreateLinearContour(x1, y1, x2, y2)
        End If

        If shape Is Nothing Then
            Throw New ArgumentNullException(NameOf(shape))
        End If

        If distance < 0.0 Then
            Throw New ArgumentOutOfRangeException(NameOf(distance))
        End If

        Dim dx As Double = x2 - x1
        Dim dy As Double = y2 - y1

        Dim lineLength As Double =
            Math.Sqrt(dx * dx + dy * dy)

        If lineLength = 0.0 Then
            Throw New ArgumentException(
                "Start Point and End Point cannot be identical.")
        End If

        ' Unit vector along the cutting direction.
        Dim ux As Double = dx / lineLength
        Dim uy As Double = dy / lineLength

        ' Unit vector perpendicular to the left.
        Dim nx As Double = -uy
        Dim ny As Double = ux

        Dim lineAngle As Double =
            Math.Atan2(dy, dx) * 180.0 / Math.PI

        ' Obtain tool extents relative to the cutting direction.
        Dim bounds As RectangleF =
            Geometry2D.ComputeTransformedBoundingBox(
                shape.CuttingProfile.Segments,
                0.0,
                0.0,
                indexAngle - lineAngle)

        Dim startComp As Double = -CDbl(bounds.Left)
        Dim endComp As Double = CDbl(bounds.Right)

        Dim offset As Double

        Select Case cutSide

            Case "Left"
                offset = -CDbl(bounds.Top) + distance

            Case "Right"
                offset = -(CDbl(bounds.Bottom) + distance)

            Case Else
                Throw New ArgumentException(
                    "Invalid Cut Side.")

        End Select

        If lineLength <= startComp + endComp Then
            Throw New ArgumentException(
                "The compensated line does not produce a positive-length toolpath.")
        End If

        Dim sx As Double =
            x1 + ux * startComp + nx * offset

        Dim sy As Double =
            y1 + uy * startComp + ny * offset

        Dim ex As Double =
            x2 - ux * endComp + nx * offset

        Dim ey As Double =
            y2 - uy * endComp + ny * offset

        Return CreateLinearContour(sx, sy, ex, ey)

    End Function
End Class