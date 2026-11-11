Public Class CTGPoint

    Public Property Command As Integer
    Public Property X As Double
    Public Property Y As Double

End Class


Public Class CTGReader

    Public Shared Function ReadFile(filePath As String) As List(Of CTGPoint)

        Dim points As New List(Of CTGPoint)

        If String.IsNullOrWhiteSpace(filePath) Then
            Throw New ArgumentException("CTG file path is empty.")
        End If

        If Not IO.File.Exists(filePath) Then
            Throw New IO.FileNotFoundException(
            "CTG file not found.", filePath)
        End If

        Dim lineNumber As Integer = 0

        For Each line As String In IO.File.ReadLines(filePath)

            lineNumber += 1

            If String.IsNullOrWhiteSpace(line) Then
                Continue For
            End If

            Dim fields As String() =
            line.Trim().Split(
                CType(Nothing, Char()),
                StringSplitOptions.RemoveEmptyEntries)

            If fields.Length <> 3 Then
                Throw New FormatException(
                $"Invalid CTG record at line {lineNumber}.")
            End If

            Dim command As Integer
            Dim x As Double
            Dim y As Double

            If Not Integer.TryParse(fields(0), command) OrElse
           (command < 0 OrElse command > 3) OrElse
           Not Double.TryParse(
               fields(1),
               Globalization.NumberStyles.Float,
               Globalization.CultureInfo.InvariantCulture,
               x) OrElse
           Not Double.TryParse(
               fields(2),
               Globalization.NumberStyles.Float,
               Globalization.CultureInfo.InvariantCulture,
               y) Then

                Throw New FormatException(
                $"Invalid CTG data at line {lineNumber}.")
            End If

            points.Add(New CTGPoint With {
            .Command = command,
            .X = x,
            .Y = y
        })

        Next

        Return points

    End Function



    Public Shared Function ConvertToGeometry(
    points As List(Of CTGPoint)) As List(Of GeoEntity)

        Dim geometry As New List(Of GeoEntity)

        If points Is Nothing OrElse points.Count = 0 Then
            Return geometry
        End If

        Dim i As Integer = 1

        While i < points.Count

            Dim command As Integer = points(i).Command

            Select Case command

                Case 0
                ' Rapid move: no geometry.

                Case 1
                    ' Straight line.
                    Dim p0 = points(i - 1)
                    Dim p1 = points(i)

                    Dim length As Double =
                    Math.Sqrt((p1.X - p0.X) ^ 2 +
                              (p1.Y - p0.Y) ^ 2)

                    If length > 0.0000001 Then

                        geometry.Add(New GeoLine With {
                        .StartPoint = New GeoPoint With {
                            .X = p0.X,
                            .Y = p0.Y
                        },
                        .EndPoint = New GeoPoint With {
                            .X = p1.X,
                            .Y = p1.Y
                        }
                    })

                    End If

                Case 2, 3
                    ' Arc: previous point = start,
                    ' current point = center,
                    ' next point = end.

                    If i + 1 >= points.Count Then
                        Throw New FormatException(
                        "CTG arc is missing its endpoint.")
                    End If

                    Dim p0 = points(i - 1)
                    Dim pc = points(i)
                    Dim p1 = points(i + 1)

                    Dim radius As Double =
                    Math.Sqrt((p0.X - pc.X) ^ 2 +
                              (p0.Y - pc.Y) ^ 2)

                    If radius > 0.0000001 Then

                        Dim startAngle As Double =
                        Math.Atan2(
                            p0.Y - pc.Y,
                            p0.X - pc.X) * 180.0 / Math.PI

                        Dim endAngle As Double =
                        Math.Atan2(
                            p1.Y - pc.Y,
                            p1.X - pc.X) * 180.0 / Math.PI

                        geometry.Add(New GeoArc With {
                        .StartPoint = New GeoPoint With {
                            .X = p0.X,
                            .Y = p0.Y
                        },
                        .EndPoint = New GeoPoint With {
                            .X = p1.X,
                            .Y = p1.Y
                        },
                        .Center = New GeoPoint With {
                            .X = pc.X,
                            .Y = pc.Y
                        },
                        .Radius = radius,
                        .StartAngle = startAngle,
                        .EndAngle = endAngle,
                        .IsClockwise = (command = 2)
                    })

                    End If

                    ' Skip the endpoint record already
                    ' consumed by the arc.
                    i += 1

            End Select

            i += 1

        End While

        Return geometry

    End Function

End Class