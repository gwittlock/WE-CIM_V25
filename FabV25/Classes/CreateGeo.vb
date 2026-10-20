Public Class CreateGeo


    Public Function CreateLine(
    x1 As Double,
    y1 As Double,
    x2 As Double,
    y2 As Double
) As GeoLine

        Return New GeoLine With {
        .StartPoint = New GeoPoint With {
            .X = x1,
            .Y = y1
        },
        .EndPoint = New GeoPoint With {
            .X = x2,
            .Y = y2
        }
    }

    End Function

    Public Function CreateCircle(
    centerX As Double,
    centerY As Double,
    radius As Double
) As GeoCircle

        Return New GeoCircle With {
            .Center = New GeoPoint With {
                .X = centerX,
                .Y = centerY
            },
            .Radius = radius
        }

    End Function

    Public Function CreatePoint(
    x As Double,
    y As Double
) As GeoPoint

        Return New GeoPoint With {
            .X = x,
            .Y = y
        }

    End Function

    Public Function CreateArc(
    startX As Double,
    startY As Double,
    endX As Double,
    endY As Double,
    centerX As Double,
    centerY As Double,
    radius As Double,
    startAngle As Double,
    endAngle As Double,
    isClockwise As Boolean
) As GeoArc

        Return New GeoArc With {
            .StartPoint = New GeoPoint With {
                .X = startX,
                .Y = startY
            },
            .EndPoint = New GeoPoint With {
                .X = endX,
                .Y = endY
            },
            .Center = New GeoPoint With {
                .X = centerX,
                .Y = centerY
            },
            .Radius = radius,
            .StartAngle = startAngle,
            .EndAngle = endAngle,
            .IsClockwise = isClockwise
        }

    End Function

End Class
