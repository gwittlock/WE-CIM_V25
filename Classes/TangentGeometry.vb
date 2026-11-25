Public NotInheritable Class TangentGeometry

    Private Const AngleTolerance As Double = 0.0000001

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

    Private Sub New()
    End Sub

    Private Shared Function NormalizeAngle(
        angle As Double) As Double

        angle = angle Mod 360.0

        If angle < 0 Then
            angle += 360.0
        End If

        Return angle

    End Function

    Public Shared Function IsAngleOnArc(
        arc As GeoArc,
        angle As Double) As Boolean

        Dim angularDistance As Double

        If arc.IsClockwise Then

            angularDistance =
                NormalizeAngle(arc.StartAngle - angle)

        Else

            angularDistance =
                NormalizeAngle(angle - arc.StartAngle)

        End If

        'Account for floating-point error at the Start Point.
        If angularDistance >= 360.0 - AngleTolerance Then
            angularDistance = 0.0
        End If

        Return angularDistance <=
            arc.SweepAngle + AngleTolerance

    End Function


    Public Shared Function TryGetTangentPoint(
    pointX As Double,
    pointY As Double,
    selection As TangentSelection,
    ByRef tangentX As Double,
    ByRef tangentY As Double
) As Boolean

        Dim center As GeoPoint = Nothing
        Dim radius As Double
        Dim arc As GeoArc = Nothing

        If Not TryGetCircularGeometry(
        selection, center, radius, arc) Then

            Return False

        End If

        'Validate the external point coordinates.
        If Not AreFinite(pointX, pointY) Then
            Return False
        End If

        Dim dx As Double = pointX - center.X
        Dim dy As Double = pointY - center.Y
        Dim distance As Double =
    Math.Sqrt(dx * dx + dy * dy)

        'Reject non-finite calculated distances.
        If Not AreFinite(distance) Then
            Return False
        End If

        'The external point must be outside the circle.
        If distance <= radius Then
            Return False
        End If
        Dim baseAngle As Double =
            Math.Atan2(dy, dx)

        Dim offsetAngle As Double =
            Math.Acos(radius / distance)

        Dim candidateAngles As Double() = {
            baseAngle + offsetAngle,
            baseAngle - offsetAngle
        }

        Dim found As Boolean = False
        Dim bestDistanceSquared As Double = Double.MaxValue

        For Each candidateAngle As Double In candidateAngles

            Dim angleDegrees As Double =
                NormalizeAngle(candidateAngle * 180.0 / Math.PI)

            'Reject tangent points outside the actual Arc sweep.
            If arc IsNot Nothing Then

                If Not IsAngleOnArc(arc, angleDegrees) Then
                    Continue For
                End If

            End If

            Dim candidateX As Double =
                center.X + radius * Math.Cos(candidateAngle)

            Dim candidateY As Double =
                center.Y + radius * Math.Sin(candidateAngle)

            'Reject non-finite tangent coordinates.
            If Not AreFinite(candidateX, candidateY) Then
                Continue For
            End If


            'Compare the candidate with the original mouse pick.
            Dim pickDX As Double =
                candidateX - selection.PickPoint.X

            Dim pickDY As Double =
                candidateY - selection.PickPoint.Y

            Dim distanceSquared As Double =
                pickDX * pickDX + pickDY * pickDY

            If Not found OrElse
               distanceSquared < bestDistanceSquared Then

                bestDistanceSquared = distanceSquared

                tangentX = candidateX
                tangentY = candidateY

                found = True

            End If

        Next

        Return found

    End Function


    Private Shared Function TryGetCircularGeometry(
    selection As TangentSelection,
    ByRef center As GeoPoint,
    ByRef radius As Double,
    ByRef arc As GeoArc) As Boolean

        center = Nothing
        radius = 0.0
        arc = Nothing

        If selection Is Nothing OrElse
           selection.Entity Is Nothing Then

            Return False

        End If

        If TypeOf selection.Entity Is GeoCircle Then

            Dim circle As GeoCircle =
                DirectCast(selection.Entity, GeoCircle)

            center = circle.Center
            radius = circle.Radius

        ElseIf TypeOf selection.Entity Is GeoArc Then

            arc = DirectCast(selection.Entity, GeoArc)

            center = arc.Center
            radius = arc.Radius

        Else

            Return False

        End If
        If center Is Nothing Then Return False

        'Validate center coordinates, radius and graphical pick.
        If Not AreFinite(
    center.X,
    center.Y,
    radius,
    selection.PickPoint.X,
    selection.PickPoint.Y) Then

            Return False

        End If

        If radius <= 0.0 Then Return False

        'Validate angular values when the entity is an Arc.
        If arc IsNot Nothing Then

            If Not AreFinite(
        arc.StartAngle,
        arc.EndAngle,
        arc.SweepAngle) Then

                Return False

            End If

            If arc.SweepAngle <= 0.0 Then Return False

        End If

        Return True

    End Function


    Private Shared Function GetCircleIntersections(
    x1 As Double,
    y1 As Double,
    r1 As Double,
    x2 As Double,
    y2 As Double,
    r2 As Double
) As List(Of Tuple(Of Double, Double))

        Dim results As New List(Of Tuple(Of Double, Double))

        'Reject non-finite center coordinates and radii.
        If Not AreFinite(x1, y1, r1, x2, y2, r2) Then
            Return results
        End If

        'Both radii must be positive.
        If r1 <= 0.0 OrElse r2 <= 0.0 Then
            Return results
        End If

        Dim dx As Double = x2 - x1
        Dim dy As Double = y2 - y1

        Dim distance As Double =
        Math.Sqrt(dx * dx + dy * dy)

        If Double.IsNaN(distance) OrElse
       Double.IsInfinity(distance) Then

            Return results

        End If

        'Concentric circles do not provide a unique intersection.
        If distance = 0 Then Return results

        Dim tolerance As Double =
        Math.Max(Math.Max(r1, r2), distance) * 0.000000000001

        'Circles are too far apart.
        If distance > r1 + r2 + tolerance Then
            Return results
        End If

        'One circle is completely inside the other.
        If distance < Math.Abs(r1 - r2) - tolerance Then
            Return results
        End If

        'Distance along the center-to-center line.
        Dim a As Double =
        (r1 * r1 - r2 * r2 + distance * distance) /
        (2.0 * distance)

        'Perpendicular distance to the intersections.
        Dim hSquared As Double = r1 * r1 - a * a

        'Reject non-finite intermediate calculations.
        If Not AreFinite(a, hSquared) Then
            Return results
        End If

        Dim h As Double =
    Math.Sqrt(Math.Max(0.0, hSquared))

        Dim ux As Double = dx / distance
        Dim uy As Double = dy / distance

        Dim baseX As Double = x1 + a * ux
        Dim baseY As Double = y1 + a * uy

        'First intersection.
        Dim ix1 As Double = baseX - h * uy
        Dim iy1 As Double = baseY + h * ux

        If AreFinite(ix1, iy1) Then
            results.Add(Tuple.Create(ix1, iy1))
        End If

        'Second intersection, if distinct.
        If h > tolerance Then

            Dim ix2 As Double = baseX + h * uy
            Dim iy2 As Double = baseY - h * ux

            If AreFinite(ix2, iy2) Then
                results.Add(Tuple.Create(ix2, iy2))
            End If

        End If

        Return results

        Return results

    End Function
    Public Shared Function TryGetCommonTangent(
    startSelection As TangentSelection,
    endSelection As TangentSelection,
    ByRef x1 As Double,
    ByRef y1 As Double,
    ByRef x2 As Double,
    ByRef y2 As Double) As Boolean

        Dim center1 As GeoPoint = Nothing
        Dim center2 As GeoPoint = Nothing

        Dim radius1 As Double
        Dim radius2 As Double

        Dim arc1 As GeoArc = Nothing
        Dim arc2 As GeoArc = Nothing

        If Not TryGetCircularGeometry(
            startSelection, center1, radius1, arc1) Then

            Return False

        End If

        If Not TryGetCircularGeometry(
            endSelection, center2, radius2, arc2) Then

            Return False

        End If

        'Vector between the two centers.
        Dim dx As Double = center2.X - center1.X
        Dim dy As Double = center2.Y - center1.Y

        Dim distance As Double =
    Math.Sqrt(dx * dx + dy * dy)

        'Reject non-finite calculated distances.
        If Not AreFinite(distance) Then
            Return False
        End If

        'Concentric entities have no unique common tangent.
        If distance = 0.0 Then Return False

        Dim ux As Double = dx / distance
        Dim uy As Double = dy / distance

        Dim found As Boolean = False
        Dim bestScore As Double = Double.MaxValue

        '1 = External tangents.
        '-1 = Internal tangents.
        For Each tangentType As Integer In New Integer() {1, -1}

            Dim ratio As Double =
    (radius1 - tangentType * radius2) / distance

            'Reject non-finite tangent ratios.
            If Not AreFinite(ratio) Then
                Continue For
            End If

            'Reject geometrically impossible tangent solutions.
            If Math.Abs(ratio) > 1.0 Then
                Continue For
            End If

            Dim perpendicular As Double =
                Math.Sqrt(Math.Max(0.0, 1.0 - ratio * ratio))

            'Evaluate both sides of the centerline.
            For Each side As Integer In New Integer() {1, -1}

                Dim nx As Double =
                    ratio * ux - side * perpendicular * uy

                Dim ny As Double =
                    ratio * uy + side * perpendicular * ux

                'Calculate Start tangent point.
                Dim candidateX1 As Double =
                    center1.X + radius1 * nx

                Dim candidateY1 As Double =
                    center1.Y + radius1 * ny

                'Calculate End tangent point.
                Dim candidateX2 As Double =
                    center2.X + tangentType * radius2 * nx

                Dim candidateY2 As Double =
                    center2.Y + tangentType * radius2 * ny

                'Reject non-finite tangent endpoint coordinates.
                If Not AreFinite(
    candidateX1, candidateY1,
    candidateX2, candidateY2) Then

                    Continue For

                End If


                'Validate Start Arc sweep.
                If arc1 IsNot Nothing Then

                    Dim angle1 As Double =
                        Math.Atan2(ny, nx) * 180.0 / Math.PI

                    If Not IsAngleOnArc(arc1, angle1) Then
                        Continue For
                    End If

                End If

                'Validate End Arc sweep.
                If arc2 IsNot Nothing Then

                    Dim angle2 As Double =
                        Math.Atan2(
                            tangentType * ny,
                            tangentType * nx) * 180.0 / Math.PI

                    If Not IsAngleOnArc(arc2, angle2) Then
                        Continue For
                    End If

                End If

                'Reject coincident tangent endpoints.
                If candidateX1 = candidateX2 AndAlso
   candidateY1 = candidateY2 Then

                    Continue For

                End If

                'Distance from Start tangent to Start mouse pick.
                Dim pickDX1 As Double =
                    candidateX1 - startSelection.PickPoint.X

                Dim pickDY1 As Double =
                    candidateY1 - startSelection.PickPoint.Y

                'Distance from End tangent to End mouse pick.
                Dim pickDX2 As Double =
                    candidateX2 - endSelection.PickPoint.X

                Dim pickDY2 As Double =
                    candidateY2 - endSelection.PickPoint.Y

                Dim score As Double =
                    Math.Sqrt(pickDX1 * pickDX1 +
                              pickDY1 * pickDY1) +
                    Math.Sqrt(pickDX2 * pickDX2 +
                              pickDY2 * pickDY2)

                'Reject non-finite candidate scores.
                If Not AreFinite(score) Then
                    Continue For
                End If

                'Retain the solution closest to both mouse picks.
                If Not found OrElse score < bestScore Then

                    bestScore = score

                    x1 = candidateX1
                    y1 = candidateY1

                    x2 = candidateX2
                    y2 = candidateY2

                    found = True

                End If

            Next

        Next

        Return found

    End Function


    Public Shared Function TryGetPointTangentArc(
    pointX As Double,
    pointY As Double,
    selection As TangentSelection,
    radius As Double,
    ByRef centerX As Double,
    ByRef centerY As Double,
    ByRef tangentX As Double,
    ByRef tangentY As Double
) As Boolean

        If radius <= 0 OrElse
           Double.IsNaN(radius) OrElse
           Double.IsInfinity(radius) Then

            Return False

        End If

        Dim targetCenter As GeoPoint = Nothing
        Dim targetRadius As Double
        Dim targetArc As GeoArc = Nothing

        If Not TryGetCircularGeometry(
            selection,
            targetCenter,
            targetRadius,
            targetArc) Then

            Return False

        End If

        Dim found As Boolean = False
        Dim bestScore As Double = Double.PositiveInfinity

        'Evaluate external and internal tangency.
        For Each external As Boolean In New Boolean() {True, False}

            Dim centerDistance As Double

            If external Then
                centerDistance = targetRadius + radius
            Else
                centerDistance = Math.Abs(targetRadius - radius)
            End If

            'Coincident supporting circles do not provide
            'a unique tangent point.
            If centerDistance = 0 Then Continue For

            'Calculate possible centers of the new Arc.
            Dim candidates = GetCircleIntersections(
                pointX,
                pointY,
                radius,
                targetCenter.X,
                targetCenter.Y,
                centerDistance)

            For Each candidate In candidates

                Dim cx As Double = candidate.Item1
                Dim cy As Double = candidate.Item2

                Dim tx As Double
                Dim ty As Double

                If Not TryGetTangentContact(
                    cx, cy,
                    radius,
                    targetCenter,
                    targetRadius,
                    targetArc,
                    external,
                    tx, ty) Then

                    Continue For

                End If

                'Reject coincident Arc endpoints.
                Dim endDX As Double = tx - pointX
                Dim endDY As Double = ty - pointY

                Dim tolerance As Double = radius * 0.000000000001

                If endDX * endDX + endDY * endDY <=
                   tolerance * tolerance Then

                    Continue For

                End If

                'Score the tangent point against the
                'operator's actual graphical pick.
                Dim pickDX As Double = tx - selection.PickPoint.X
                Dim pickDY As Double = ty - selection.PickPoint.Y

                Dim score As Double =
                    pickDX * pickDX + pickDY * pickDY

                'Reject non-finite candidate scores.
                If Not AreFinite(score) Then
                    Continue For
                End If

                If score < bestScore Then

                    bestScore = score

                    centerX = cx
                    centerY = cy

                    tangentX = tx
                    tangentY = ty

                    found = True

                End If

            Next

        Next

        Return found

    End Function

    Private Shared Function TryGetTangentContact(
    centerX As Double,
    centerY As Double,
    radius As Double,
    targetCenter As GeoPoint,
    targetRadius As Double,
    targetArc As GeoArc,
    external As Boolean,
    ByRef tangentX As Double,
    ByRef tangentY As Double
) As Boolean

        If targetCenter Is Nothing Then Return False

        'Validate all numerical inputs.
        If Not AreFinite(
    centerX, centerY,
    radius,
    targetCenter.X, targetCenter.Y,
    targetRadius) Then

            Return False

        End If

        'Both radii must be positive.
        If radius <= 0.0 OrElse targetRadius <= 0.0 Then
            Return False
        End If

        Dim dx As Double = centerX - targetCenter.X
        Dim dy As Double = centerY - targetCenter.Y

        Dim distance As Double =
            Math.Sqrt(dx * dx + dy * dy)

        If distance = 0 OrElse
           Double.IsNaN(distance) OrElse
           Double.IsInfinity(distance) Then

            Return False

        End If

        Dim ux As Double = dx / distance
        Dim uy As Double = dy / distance

        Dim side As Double = 1.0

        'Internal tangency where the new Arc's
        'supporting circle encloses the target.
        If Not external AndAlso radius > targetRadius Then
            side = -1.0
        End If

        Dim tx As Double =
            targetCenter.X + side * targetRadius * ux

        Dim ty As Double =
            targetCenter.Y + side * targetRadius * uy

        'Verify that the tangent point lies on
        'the new Arc's supporting circle.
        Dim contactDX As Double = tx - centerX
        Dim contactDY As Double = ty - centerY

        Dim contactRadius As Double =
            Math.Sqrt(
                contactDX * contactDX +
                contactDY * contactDY)

        'Reject non-finite tangent contact calculations.
        If Not AreFinite(tx, ty, contactRadius) Then
            Return False
        End If

        Dim tolerance As Double =
            Math.Max(radius, targetRadius) * 0.000000001

        If Math.Abs(contactRadius - radius) > tolerance Then
            Return False
        End If

        'Restrict contact to the actual sweep
        'when the target entity is an Arc.
        If targetArc IsNot Nothing Then

            Dim tangentAngle As Double =
                Math.Atan2(
                    ty - targetCenter.Y,
                    tx - targetCenter.X) *
                180.0 / Math.PI

            If Not IsAngleOnArc(
                targetArc, tangentAngle) Then

                Return False

            End If

        End If

        tangentX = tx
        tangentY = ty

        Return True

    End Function

    Public Shared Function TryGetDoubleTangentArc(
    startSelection As TangentSelection,
    endSelection As TangentSelection,
    radius As Double,
    isClockwise As Boolean,
    isLarge As Boolean,
    ByRef centerX As Double,
    ByRef centerY As Double,
    ByRef startX As Double,
    ByRef startY As Double,
    ByRef endX As Double,
    ByRef endY As Double
) As Boolean

        If radius <= 0 OrElse
           Double.IsNaN(radius) OrElse
           Double.IsInfinity(radius) Then

            Return False

        End If

        Dim center1 As GeoPoint = Nothing
        Dim center2 As GeoPoint = Nothing

        Dim radius1 As Double
        Dim radius2 As Double

        Dim arc1 As GeoArc = Nothing
        Dim arc2 As GeoArc = Nothing

        If Not TryGetCircularGeometry(
            startSelection, center1, radius1, arc1) Then

            Return False

        End If

        If Not TryGetCircularGeometry(
            endSelection, center2, radius2, arc2) Then

            Return False

        End If

        Dim found As Boolean = False
        Dim bestScore As Double = Double.PositiveInfinity

        'Evaluate all four internal/external combinations.
        For Each external1 As Boolean In New Boolean() {True, False}

            Dim distance1 As Double

            If external1 Then
                distance1 = radius1 + radius
            Else
                distance1 = Math.Abs(radius1 - radius)
            End If

            If distance1 = 0 Then Continue For

            For Each external2 As Boolean In New Boolean() {True, False}

                Dim distance2 As Double

                If external2 Then
                    distance2 = radius2 + radius
                Else
                    distance2 = Math.Abs(radius2 - radius)
                End If

                If distance2 = 0 Then Continue For

                'Calculate candidate Arc centers.
                Dim candidates = GetCircleIntersections(
                    center1.X,
                    center1.Y,
                    distance1,
                    center2.X,
                    center2.Y,
                    distance2)

                For Each candidate In candidates

                    Dim cx As Double = candidate.Item1
                    Dim cy As Double = candidate.Item2

                    Dim sx As Double
                    Dim sy As Double

                    Dim ex As Double
                    Dim ey As Double

                    'Calculate Start Tan contact.
                    If Not TryGetTangentContact(
                        cx, cy, radius,
                        center1, radius1, arc1,
                        external1, sx, sy) Then

                        Continue For

                    End If

                    'Calculate End Tan contact.
                    If Not TryGetTangentContact(
                        cx, cy, radius,
                        center2, radius2, arc2,
                        external2, ex, ey) Then

                        Continue For

                    End If

                    'Reject coincident endpoints.
                    Dim dx As Double = ex - sx
                    Dim dy As Double = ey - sy

                    Dim tolerance As Double = radius * 0.000000000001

                    If dx * dx + dy * dy <=
                       tolerance * tolerance Then

                        Continue For

                    End If

                    'Calculate candidate angular sweep.
                    Dim startAngle As Double =
                        NormalizeAngle(
                            Math.Atan2(sy - cy, sx - cx) *
                            180.0 / Math.PI)

                    Dim endAngle As Double =
                        NormalizeAngle(
                            Math.Atan2(ey - cy, ex - cx) *
                            180.0 / Math.PI)

                    Dim sweep As Double

                    If isClockwise Then

                        sweep = NormalizeAngle(
                            startAngle - endAngle)

                    Else

                        sweep = NormalizeAngle(
                            endAngle - startAngle)

                    End If

                    'Reject non-finite or zero angular sweeps.
                    If Not AreFinite(sweep) OrElse sweep <= 0.0 Then
                        Continue For
                    End If

                    'Apply the operator's Small/Large selection.
                    If isLarge Then

                        If sweep < 180.0 - AngleTolerance Then
                            Continue For
                        End If

                    Else

                        If sweep > 180.0 + AngleTolerance Then
                            Continue For
                        End If

                    End If

                    'Score both tangent contact points against
                    'the operator's graphical pick locations.
                    Dim startDX As Double =
                        sx - startSelection.PickPoint.X

                    Dim startDY As Double =
                        sy - startSelection.PickPoint.Y

                    Dim endDX As Double =
                        ex - endSelection.PickPoint.X

                    Dim endDY As Double =
                        ey - endSelection.PickPoint.Y

                    Dim score As Double =
                        Math.Sqrt(
                            startDX * startDX +
                            startDY * startDY) +
                        Math.Sqrt(
                            endDX * endDX +
                            endDY * endDY)



                    'Reject non-finite candidate scores.
                    If Not AreFinite(score) Then
                        Continue For
                    End If

                    If score < bestScore Then

                        bestScore = score

                        centerX = cx
                        centerY = cy

                        startX = sx
                        startY = sy

                        endX = ex
                        endY = ey

                        found = True

                    End If

                Next

            Next

        Next

        Return found

    End Function

End Class