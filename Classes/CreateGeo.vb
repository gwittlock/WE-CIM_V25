Public Class CreateGeo

    Private Shared Function AreFinite(
    ParamArray values As Double()
) As Boolean

        For Each value As Double In values

            If Double.IsNaN(value) OrElse
           Double.IsInfinity(value) Then

                Return False

            End If

        Next

        Return True

    End Function
    Public Function CreateLine(
    x1 As Double,
    y1 As Double,
    x2 As Double,
    y2 As Double
) As GeoLine

        'Reject non-finite coordinates.
        If Not AreFinite(x1, y1, x2, y2) Then
            Return Nothing
        End If

        'Reject coincident endpoints.
        If x1 = x2 AndAlso y1 = y2 Then
            Return Nothing
        End If

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
    radius As Double,
    Optional isClockwise As Boolean = True
) As GeoCircle

        'Reject non-finite coordinates and radius.
        If Not AreFinite(centerX, centerY, radius) Then
            Return Nothing
        End If

        'Reject zero or negative radius.
        If radius <= 0.0 Then
            Return Nothing
        End If

        Return New GeoCircle With {
        .Center = New GeoPoint With {
            .X = centerX,
            .Y = centerY
        },
        .Radius = radius,
        .IsClockwise = isClockwise
    }

    End Function
    Public Function CreatePoint(
    x As Double,
    y As Double
) As GeoPoint

        'Reject non-finite coordinates.
        If Not AreFinite(x, y) Then
            Return Nothing
        End If

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

        'Reject non-finite geometry values.
        If Not AreFinite(
        startX, startY,
        endX, endY,
        centerX, centerY,
        radius,
        startAngle, endAngle) Then

            Return Nothing

        End If

        'Reject zero or negative radius.
        If radius <= 0.0 Then
            Return Nothing
        End If

        'Reject coincident endpoints.
        If startX = endX AndAlso startY = endY Then
            Return Nothing
        End If

        'Reject zero angular sweep.
        If (endAngle - startAngle) Mod 360.0 = 0.0 Then
            Return Nothing
        End If

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
    Public Function CreateArcThreePoints(
    startX As Double,
    startY As Double,
    endX As Double,
    endY As Double,
    intX As Double,
    intY As Double
) As GeoArc

        'Translate coordinates relative to the Start Point.
        Dim ux As Double = endX - startX
        Dim uy As Double = endY - startY

        Dim vx As Double = intX - startX
        Dim vy As Double = intY - startY

        Dim uSquared As Double = ux * ux + uy * uy
        Dim vSquared As Double = vx * vx + vy * vy

        Dim wx As Double = intX - endX
        Dim wy As Double = intY - endY

        Dim wSquared As Double = wx * wx + wy * wy

        'Reject coincident points.
        If uSquared = 0 OrElse
           vSquared = 0 OrElse
           wSquared = 0 Then

            Return Nothing

        End If

        Dim cross As Double = ux * vy - uy * vx

        'Reject collinear or numerically near-collinear points.
        Dim tolerance As Double =
            0.000000000001 * Math.Sqrt(uSquared) * Math.Sqrt(vSquared)

        If Math.Abs(cross) <= tolerance Then
            Return Nothing
        End If

        'Calculate the center relative to the Start Point.
        Dim offsetX As Double =
            (uSquared * vy - vSquared * uy) / (2.0 * cross)

        Dim offsetY As Double =
            (ux * vSquared - vx * uSquared) / (2.0 * cross)

        Dim centerX As Double = startX + offsetX
        Dim centerY As Double = startY + offsetY

        Dim radius As Double =
            Math.Sqrt(offsetX * offsetX + offsetY * offsetY)

        'Calculate Start and End angles in degrees.
        Dim startAngle As Double =
            Math.Atan2(startY - centerY,
                       startX - centerX) * 180.0 / Math.PI

        Dim endAngle As Double =
            Math.Atan2(endY - centerY,
                       endX - centerX) * 180.0 / Math.PI

        'Normalize angles to 0 through 360 degrees.
        If startAngle < 0 Then startAngle += 360.0
        If endAngle < 0 Then endAngle += 360.0

        'Determine the direction that passes through
        'the Intermediate Point.
        Dim isClockwise As Boolean = cross > 0

        Return CreateArc(
            startX,
            startY,
            endX,
            endY,
            centerX,
            centerY,
            radius,
            startAngle,
            endAngle,
            isClockwise)

    End Function

    Public Function CreateArcStartEndRadius(
    startX As Double,
    startY As Double,
    endX As Double,
    endY As Double,
    radius As Double,
    isClockwise As Boolean,
    isLarge As Boolean
) As GeoArc

        If radius <= 0 OrElse
           Double.IsNaN(radius) OrElse
           Double.IsInfinity(radius) Then

            Return Nothing

        End If

        'Calculate the chord.
        Dim dx As Double = endX - startX
        Dim dy As Double = endY - startY

        Dim chordLength As Double =
            Math.Sqrt(dx * dx + dy * dy)

        'Reject coincident endpoints.
        If chordLength = 0 Then Return Nothing

        Dim halfChord As Double = chordLength / 2.0

        'Radius must be at least half the chord length.
        If radius < halfChord Then Return Nothing

        'Calculate the chord midpoint.
        Dim midX As Double = startX + dx / 2.0
        Dim midY As Double = startY + dy / 2.0

        'Distance from chord midpoint to circle center.
        Dim centerOffset As Double =
            Math.Sqrt(Math.Max(0.0,
                               (radius - halfChord) *
                               (radius + halfChord)))

        'Unit vector perpendicular to the chord.
        Dim normalX As Double = -dy / chordLength
        Dim normalY As Double = dx / chordLength

        'Determine which side contains the center
        'based on the requested Direction and Solution.
        Dim side As Double = 1.0

        If isClockwise Xor isLarge Then
            side = -1.0
        End If

        Dim centerX As Double =
            midX + side * centerOffset * normalX

        Dim centerY As Double =
            midY + side * centerOffset * normalY

        'Calculate Start and End angles.
        Dim startAngle As Double =
            Math.Atan2(startY - centerY,
                       startX - centerX) * 180.0 / Math.PI

        Dim endAngle As Double =
            Math.Atan2(endY - centerY,
                       endX - centerX) * 180.0 / Math.PI

        If startAngle < 0 Then startAngle += 360.0
        If endAngle < 0 Then endAngle += 360.0

        Return CreateArc(
            startX,
            startY,
            endX,
            endY,
            centerX,
            centerY,
            radius,
            startAngle,
            endAngle,
            isClockwise)

    End Function

    Public Function CreateArcStartCenterAngle(
    startX As Double,
    startY As Double,
    centerX As Double,
    centerY As Double,
    angle As Double,
    isClockwise As Boolean
) As GeoEntity

        'Validate the requested sweep.
        If Double.IsNaN(angle) OrElse
           Double.IsInfinity(angle) OrElse
           angle <= 0 OrElse
           angle > 360.0 Then

            Return Nothing

        End If

        'Calculate the radius.
        Dim dx As Double = startX - centerX
        Dim dy As Double = startY - centerY

        Dim radius As Double =
            Math.Sqrt(dx * dx + dy * dy)

        If radius <= 0 OrElse
           Double.IsNaN(radius) OrElse
           Double.IsInfinity(radius) Then

            Return Nothing

        End If

        'A complete revolution creates a Circle.
        If angle = 360.0 Then

            Return CreateCircle(
                centerX,
                centerY,
                radius,
                isClockwise)

        End If

        'Calculate the Start Angle.
        Dim startAngle As Double =
            Math.Atan2(dy, dx) * 180.0 / Math.PI

        If startAngle < 0 Then startAngle += 360.0

        'Calculate the End Angle using the requested direction.
        Dim endAngle As Double

        If isClockwise Then
            endAngle = startAngle - angle
        Else
            endAngle = startAngle + angle
        End If

        'Normalize the End Angle.
        endAngle = endAngle Mod 360.0

        If endAngle < 0 Then endAngle += 360.0

        'Calculate the End Point.
        Dim endRadians As Double =
            endAngle * Math.PI / 180.0

        Dim endX As Double =
            centerX + radius * Math.Cos(endRadians)

        Dim endY As Double =
            centerY + radius * Math.Sin(endRadians)

        Return CreateArc(
            startX,
            startY,
            endX,
            endY,
            centerX,
            centerY,
            radius,
            startAngle,
            endAngle,
            isClockwise)

    End Function

    Public Function CreateArcStartCenterEnd(
    startX As Double,
    startY As Double,
    centerX As Double,
    centerY As Double,
    endX As Double,
    endY As Double,
    isClockwise As Boolean
) As GeoArc

        'Calculate the Start Point radius.
        Dim startDX As Double = startX - centerX
        Dim startDY As Double = startY - centerY

        Dim startRadius As Double =
            Math.Sqrt(startDX * startDX + startDY * startDY)

        'Calculate the End Point radius.
        Dim endDX As Double = endX - centerX
        Dim endDY As Double = endY - centerY

        Dim endRadius As Double =
            Math.Sqrt(endDX * endDX + endDY * endDY)

        'Reject invalid radii.
        If startRadius <= 0 OrElse
           endRadius <= 0 OrElse
           Double.IsNaN(startRadius) OrElse
           Double.IsNaN(endRadius) OrElse
           Double.IsInfinity(startRadius) OrElse
           Double.IsInfinity(endRadius) Then

            Return Nothing

        End If

        'Both endpoints must lie on the same circumference.
        Dim tolerance As Double =
            Math.Max(startRadius, endRadius) * 0.000000001

        If Math.Abs(startRadius - endRadius) > tolerance Then
            Return Nothing
        End If

        'Reject coincident Start and End Points.
        If startX = endX AndAlso startY = endY Then
            Return Nothing
        End If

        'Calculate the Start and End angles.
        Dim startAngle As Double =
            Math.Atan2(startDY, startDX) * 180.0 / Math.PI

        Dim endAngle As Double =
            Math.Atan2(endDY, endDX) * 180.0 / Math.PI

        If startAngle < 0 Then startAngle += 360.0
        If endAngle < 0 Then endAngle += 360.0

        'Reject zero angular sweep.
        If startAngle = endAngle Then Return Nothing

        Return CreateArc(
            startX,
            startY,
            endX,
            endY,
            centerX,
            centerY,
            startRadius,
            startAngle,
            endAngle,
            isClockwise)

    End Function


    Public Function CreateArcStartPointEndTangent(
    startX As Double,
    startY As Double,
    selection As TangentSelection,
    radius As Double,
    isClockwise As Boolean
) As GeoArc

        Dim centerX As Double
        Dim centerY As Double

        Dim endX As Double
        Dim endY As Double

        If Not TangentGeometry.TryGetPointTangentArc(
            startX,
            startY,
            selection,
            radius,
            centerX,
            centerY,
            endX,
            endY) Then

            Return Nothing

        End If

        'Calculate the Start and End angles.
        Dim startAngle As Double =
            Math.Atan2(
                startY - centerY,
                startX - centerX) * 180.0 / Math.PI

        Dim endAngle As Double =
            Math.Atan2(
                endY - centerY,
                endX - centerX) * 180.0 / Math.PI

        'Normalize angles.
        If startAngle < 0 Then startAngle += 360.0
        If endAngle < 0 Then endAngle += 360.0

        'Reject zero angular sweep.
        If startAngle = endAngle Then Return Nothing

        Return CreateArc(
            startX,
            startY,
            endX,
            endY,
            centerX,
            centerY,
            radius,
            startAngle,
            endAngle,
            isClockwise)

    End Function

    Public Function CreateArcStartTangentEndPoint(
    selection As TangentSelection,
    endX As Double,
    endY As Double,
    radius As Double,
    isClockwise As Boolean
) As GeoArc

        Dim centerX As Double
        Dim centerY As Double

        Dim startX As Double
        Dim startY As Double

        If Not TangentGeometry.TryGetPointTangentArc(
            endX,
            endY,
            selection,
            radius,
            centerX,
            centerY,
            startX,
            startY) Then

            Return Nothing

        End If

        'Calculate the Start and End angles.
        Dim startAngle As Double =
            Math.Atan2(
                startY - centerY,
                startX - centerX) * 180.0 / Math.PI

        Dim endAngle As Double =
            Math.Atan2(
                endY - centerY,
                endX - centerX) * 180.0 / Math.PI

        'Normalize angles.
        If startAngle < 0 Then startAngle += 360.0
        If endAngle < 0 Then endAngle += 360.0

        'Reject zero angular sweep.
        If startAngle = endAngle Then Return Nothing

        Return CreateArc(
            startX,
            startY,
            endX,
            endY,
            centerX,
            centerY,
            radius,
            startAngle,
            endAngle,
            isClockwise)

    End Function

    Public Function CreateArcStartTangentEndTangent(
    startSelection As TangentSelection,
    endSelection As TangentSelection,
    radius As Double,
    isClockwise As Boolean,
    isLarge As Boolean
) As GeoArc

        Dim centerX As Double
        Dim centerY As Double

        Dim startX As Double
        Dim startY As Double

        Dim endX As Double
        Dim endY As Double

        If Not TangentGeometry.TryGetDoubleTangentArc(
            startSelection,
            endSelection,
            radius,
            isClockwise,
            isLarge,
            centerX,
            centerY,
            startX,
            startY,
            endX,
            endY) Then

            Return Nothing

        End If

        'Calculate the Start and End angles.
        Dim startAngle As Double =
            Math.Atan2(
                startY - centerY,
                startX - centerX) * 180.0 / Math.PI

        Dim endAngle As Double =
            Math.Atan2(
                endY - centerY,
                endX - centerX) * 180.0 / Math.PI

        'Normalize angles.
        If startAngle < 0 Then startAngle += 360.0
        If endAngle < 0 Then endAngle += 360.0

        'Reject zero angular sweep.
        If startAngle = endAngle Then Return Nothing

        Return CreateArc(
            startX,
            startY,
            endX,
            endY,
            centerX,
            centerY,
            radius,
            startAngle,
            endAngle,
            isClockwise)

    End Function

End Class
