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

End Class