
Imports System.Drawing
Imports System.Diagnostics
Imports FabV25_WIN8

Public Class HotSpotManager

    Private _entities As List(Of CadEntity)

    Private _currentHotSpot As HotSpot

    Public ReadOnly Property CurrentHotSpot As HotSpot
        Get
            Return _currentHotSpot
        End Get
    End Property

    Public Sub New(entities As List(Of CadEntity))

        _entities = entities
        _currentHotSpot = Nothing

    End Sub

    Public Sub Clear()

        _currentHotSpot = Nothing

    End Sub

    Public Function FindHotSpot(
    worldPoint As PointF,
    toleranceWorld As Single) As HotSpot

        _currentHotSpot = Nothing

        If _entities Is Nothing Then
            Return Nothing
        End If

        Dim bestDistance As Single = Single.MaxValue

        For Each ent As CadEntity In _entities

            If ent Is Nothing Then
                Continue For
            End If

            '==================================================
            ' Circle
            '==================================================

            If ent.GeometryType = GeometryTypeEnum.Circle Then

                Dim cadcircle As CadEntity.CadCircle =
        TryCast(ent, CadEntity.CadCircle)

                If cadcircle IsNot Nothing Then

                    '----------------------------------------------
                    ' Center
                    '----------------------------------------------

                    Dim circleDistance As Single =
            CalculateDistance(
                worldPoint,
                cadcircle.Center)

                    If circleDistance <= toleranceWorld AndAlso
           circleDistance < bestDistance Then

                        bestDistance = circleDistance

                        _currentHotSpot =
                New HotSpot(
                    HotSpotType.Center,
                    cadcircle.Center,
                    ent)

                    End If


                    '----------------------------------------------
                    ' 3 o'clock
                    '----------------------------------------------

                    Dim rightPoint As New PointF(
            cadcircle.Center.X + cadcircle.Radius,
            cadcircle.Center.Y)

                    Dim distanceRight As Single =
            CalculateDistance(
                worldPoint,
                rightPoint)

                    If distanceRight <= toleranceWorld AndAlso
           distanceRight < bestDistance Then

                        bestDistance = distanceRight

                        _currentHotSpot =
                New HotSpot(
                    HotSpotType.Quadrant,
                    rightPoint,
                    ent)

                    End If


                    '----------------------------------------------
                    ' 9 o'clock
                    '----------------------------------------------

                    Dim leftPoint As New PointF(
            cadcircle.Center.X - cadcircle.Radius,
            cadcircle.Center.Y)

                    Dim distanceLeft As Single =
            CalculateDistance(
                worldPoint,
                leftPoint)

                    If distanceLeft <= toleranceWorld AndAlso
           distanceLeft < bestDistance Then

                        bestDistance = distanceLeft

                        _currentHotSpot =
                New HotSpot(
                    HotSpotType.Quadrant,
                    leftPoint,
                    ent)

                    End If


                    '----------------------------------------------
                    ' 12 o'clock
                    '----------------------------------------------

                    Dim topPoint As New PointF(
            cadcircle.Center.X,
            cadcircle.Center.Y + cadcircle.Radius)

                    Dim distanceTop As Single =
            CalculateDistance(
                worldPoint,
                topPoint)

                    If distanceTop <= toleranceWorld AndAlso
           distanceTop < bestDistance Then

                        bestDistance = distanceTop

                        _currentHotSpot =
                New HotSpot(
                    HotSpotType.Quadrant,
                    topPoint,
                    ent)

                    End If


                    '----------------------------------------------
                    ' 6 o'clock
                    '----------------------------------------------

                    Dim bottomPoint As New PointF(
            cadcircle.Center.X,
            cadcircle.Center.Y - cadcircle.Radius)

                    Dim distanceBottom As Single =
            CalculateDistance(
                worldPoint,
                bottomPoint)

                    If distanceBottom <= toleranceWorld AndAlso
           distanceBottom < bestDistance Then

                        bestDistance = distanceBottom

                        _currentHotSpot =
                New HotSpot(
                    HotSpotType.Quadrant,
                    bottomPoint,
                    ent)

                    End If

                End If

                Continue For

            End If


            '==================================================
            ' Arc
            '==================================================

            If ent.GeometryType = GeometryTypeEnum.Arc Then

                Dim cadarc As CadEntity.CadArc =
                TryCast(ent, CadEntity.CadArc)

                If cadarc Is Nothing Then
                    Continue For
                End If


                '--------------------------------------------------
                ' Center
                '--------------------------------------------------

                Dim arcDistance As Single =
                CalculateDistance(
                    worldPoint,
                    cadarc.Center)

                If arcDistance <= toleranceWorld AndAlso
               arcDistance < bestDistance Then

                    bestDistance = arcDistance

                    _currentHotSpot =
                    New HotSpot(
                        HotSpotType.Center,
                        cadarc.Center,
                        ent)

                End If


                '--------------------------------------------------
                ' Start point
                '--------------------------------------------------

                Dim startRadians As Double =
                CDbl(cadarc.StartAngle) *
                Math.PI / 180.0

                Dim startPoint As New PointF(
                cadarc.Center.X +
                CSng(Math.Cos(startRadians) * cadarc.Radius),
                cadarc.Center.Y +
                CSng(Math.Sin(startRadians) * cadarc.Radius))


                Dim startDistance As Single =
                CalculateDistance(
                    worldPoint,
                    startPoint)

                If startDistance <= toleranceWorld AndAlso
               startDistance < bestDistance Then

                    bestDistance = startDistance

                    _currentHotSpot =
                    New HotSpot(
                        HotSpotType.Endpoint,
                        startPoint,
                        ent)

                End If


                '--------------------------------------------------
                ' End point
                '--------------------------------------------------

                Dim endRadians As Double =
                CDbl(cadarc.EndAngle) *
                Math.PI / 180.0

                Dim endPoint As New PointF(
                cadarc.Center.X +
                CSng(Math.Cos(endRadians) * cadarc.Radius),
                cadarc.Center.Y +
                CSng(Math.Sin(endRadians) * cadarc.Radius))


                Dim endDistance As Single =
                CalculateDistance(
                    worldPoint,
                    endPoint)

                If endDistance <= toleranceWorld AndAlso
               endDistance < bestDistance Then

                    bestDistance = endDistance

                    _currentHotSpot =
                    New HotSpot(
                        HotSpotType.Endpoint,
                        endPoint,
                        ent)

                End If


                '--------------------------------------------------
                ' Midpoint
                '--------------------------------------------------

                Dim midpointAngle As Double

                If cadarc.IsClockwise Then

                    Dim clockwiseSweep As Double =
                    (CDbl(cadarc.StartAngle) -
                     CDbl(cadarc.EndAngle) +
                     360.0) Mod 360.0

                    midpointAngle =
                    CDbl(cadarc.StartAngle) -
                    clockwiseSweep / 2.0

                Else

                    Dim counterClockwiseSweep As Double =
                    (CDbl(cadarc.EndAngle) -
                     CDbl(cadarc.StartAngle) +
                     360.0) Mod 360.0

                    midpointAngle =
                    CDbl(cadarc.StartAngle) +
                    counterClockwiseSweep / 2.0

                End If


                Dim midpointRadians As Double =
                midpointAngle * Math.PI / 180.0

                Dim arcMidpoint As New PointF(
                cadarc.Center.X +
                CSng(Math.Cos(midpointRadians) * cadarc.Radius),
                cadarc.Center.Y +
                CSng(Math.Sin(midpointRadians) * cadarc.Radius))


                Dim arcMidpointDistance As Single =
                CalculateDistance(
                    worldPoint,
                    arcMidpoint)

                If arcMidpointDistance <= toleranceWorld AndAlso
               arcMidpointDistance < bestDistance Then

                    bestDistance = arcMidpointDistance

                    _currentHotSpot =
                    New HotSpot(
                        HotSpotType.Midpoint,
                        arcMidpoint,
                        ent)

                End If


                Continue For

            End If


            '==================================================
            ' Line
            '==================================================

            If ent.GeometryType <> GeometryTypeEnum.Line Then
                Continue For
            End If


            Dim cadline As CadEntity.CadLine =
            TryCast(ent, CadEntity.CadLine)

            If cadline Is Nothing Then
                Continue For
            End If


            '--------------------------------------------------
            ' Start point
            '--------------------------------------------------

            Dim spotDistance As Single =
            CalculateDistance(
                worldPoint,
                cadline.StartPoint)

            If spotDistance <= toleranceWorld AndAlso
           spotDistance < bestDistance Then

                bestDistance = spotDistance

                _currentHotSpot =
                New HotSpot(
                    HotSpotType.Endpoint,
                    cadline.StartPoint,
                    ent)

            End If


            '--------------------------------------------------
            ' End point
            '--------------------------------------------------

            spotDistance =
            CalculateDistance(
                worldPoint,
                cadline.EndPoint)

            If spotDistance <= toleranceWorld AndAlso
           spotDistance < bestDistance Then

                bestDistance = spotDistance

                _currentHotSpot =
                New HotSpot(
                    HotSpotType.Endpoint,
                    cadline.EndPoint,
                    ent)

            End If


            '--------------------------------------------------
            ' Midpoint
            '--------------------------------------------------

            Dim midpoint As New PointF(
            (cadline.StartPoint.X +
             cadline.EndPoint.X) / 2.0F,
            (cadline.StartPoint.Y +
             cadline.EndPoint.Y) / 2.0F)


            spotDistance =
            CalculateDistance(
                worldPoint,
                midpoint)

            If spotDistance <= toleranceWorld AndAlso
           spotDistance < bestDistance Then

                bestDistance = spotDistance

                _currentHotSpot =
                New HotSpot(
                    HotSpotType.Midpoint,
                    midpoint,
                    ent)

            End If


            '==================================================
            ' Line-Line intersections
            '==================================================

            For i As Integer = 0 To _entities.Count - 1

                Dim otherEntity As CadEntity =
                _entities(i)

                If otherEntity Is Nothing Then
                    Continue For
                End If

                If otherEntity Is ent Then
                    Continue For
                End If

                If otherEntity.GeometryType <>
               GeometryTypeEnum.Line Then

                    Continue For

                End If


                Dim otherLine As CadEntity.CadLine =
                TryCast(
                    otherEntity,
                    CadEntity.CadLine)

                If otherLine Is Nothing Then
                    Continue For
                End If


                Dim intersectionPoint As PointF

                If Not FindLineLineIntersection(
                cadline,
                otherLine,
                intersectionPoint) Then

                    Continue For

                End If


                Dim intersectionDistance As Single =
                CalculateDistance(
                    worldPoint,
                    intersectionPoint)

                If intersectionDistance <= toleranceWorld AndAlso
               intersectionDistance < bestDistance Then

                    bestDistance = intersectionDistance

                    _currentHotSpot =
                    New HotSpot(
                        HotSpotType.Intersection,
                        intersectionPoint,
                        ent,
                        otherEntity)

                End If

            Next


            '==================================================
            ' Line-Circle intersections
            '==================================================

            For i As Integer = 0 To _entities.Count - 1

                Dim otherEntity As CadEntity =
                _entities(i)

                If otherEntity Is Nothing Then
                    Continue For
                End If

                If otherEntity.GeometryType <>
               GeometryTypeEnum.Circle Then

                    Continue For

                End If


                Dim otherCircle As CadEntity.CadCircle =
                TryCast(
                    otherEntity,
                    CadEntity.CadCircle)

                If otherCircle Is Nothing Then
                    Continue For
                End If


                Dim intersection1 As PointF
                Dim intersection2 As PointF

                Dim intersectionCount As Integer =
                FindLineCircleIntersections(
                    cadline,
                    otherCircle,
                    intersection1,
                    intersection2)


                '--------------------------------------------------
                ' First intersection
                '--------------------------------------------------

                If intersectionCount >= 1 Then

                    Dim intersectionDistance As Single =
                    CalculateDistance(
                        worldPoint,
                        intersection1)

                    If intersectionDistance <= toleranceWorld AndAlso
                   intersectionDistance < bestDistance Then

                        bestDistance = intersectionDistance

                        _currentHotSpot =
                        New HotSpot(
                            HotSpotType.Intersection,
                            intersection1,
                            ent,
                            otherEntity)

                    End If

                End If


                '--------------------------------------------------
                ' Second intersection
                '--------------------------------------------------

                If intersectionCount >= 2 Then

                    Dim intersectionDistance As Single =
                    CalculateDistance(
                        worldPoint,
                        intersection2)

                    If intersectionDistance <= toleranceWorld AndAlso
                   intersectionDistance < bestDistance Then

                        bestDistance = intersectionDistance

                        _currentHotSpot =
                        New HotSpot(
                            HotSpotType.Intersection,
                            intersection2,
                            ent,
                            otherEntity)

                    End If

                End If

            Next

        Next


        Return _currentHotSpot

    End Function


    Private Function FindLineCircleIntersections(
    line As CadEntity.CadLine,
    circle As CadEntity.CadCircle,
    ByRef intersection1 As PointF,
    ByRef intersection2 As PointF) As Integer

        intersection1 = PointF.Empty
        intersection2 = PointF.Empty

        Dim dx As Double =
        CDbl(line.EndPoint.X) -
        CDbl(line.StartPoint.X)

        Dim dy As Double =
        CDbl(line.EndPoint.Y) -
        CDbl(line.StartPoint.Y)

        Dim fx As Double =
        CDbl(line.StartPoint.X) -
        CDbl(circle.Center.X)

        Dim fy As Double =
        CDbl(line.StartPoint.Y) -
        CDbl(circle.Center.Y)


        Dim a As Double =
        dx * dx +
        dy * dy

        If Math.Abs(a) < 0.0000001 Then
            Return 0
        End If


        Dim b As Double =
        2.0 * (fx * dx + fy * dy)

        Dim c As Double =
        fx * fx +
        fy * fy -
        CDbl(circle.Radius) *
        CDbl(circle.Radius)


        Dim discriminant As Double =
        b * b -
        4.0 * a * c


        If discriminant < 0.0 Then
            Return 0
        End If


        If Math.Abs(discriminant) < 0.0000001 Then

            Dim t As Double =
            -b / (2.0 * a)

            If t < 0.0 OrElse t > 1.0 Then
                Return 0
            End If

            intersection1 =
            New PointF(
                CSng(
                    CDbl(line.StartPoint.X) +
                    t * dx),
                CSng(
                    CDbl(line.StartPoint.Y) +
                    t * dy))

            Return 1

        End If


        Dim sqrtDiscriminant As Double =
        Math.Sqrt(discriminant)


        Dim t1 As Double =
        (-b - sqrtDiscriminant) /
        (2.0 * a)

        Dim t2 As Double =
        (-b + sqrtDiscriminant) /
        (2.0 * a)


        Dim count As Integer = 0


        If t1 >= 0.0 AndAlso t1 <= 1.0 Then

            intersection1 =
            New PointF(
                CSng(
                    CDbl(line.StartPoint.X) +
                    t1 * dx),
                CSng(
                    CDbl(line.StartPoint.Y) +
                    t1 * dy))

            count += 1

        End If


        If t2 >= 0.0 AndAlso t2 <= 1.0 Then

            Dim point2 As New PointF(
            CSng(
                CDbl(line.StartPoint.X) +
                t2 * dx),
            CSng(
                CDbl(line.StartPoint.Y) +
                t2 * dy))


            If count = 0 Then

                intersection1 = point2

            Else

                intersection2 = point2

            End If

            count += 1

        End If


        Return count

    End Function


    Private Function CalculateDistance(
    p1 As PointF,
    p2 As PointF) As Single

        Dim dx As Double = CDbl(p1.X) - CDbl(p2.X)
        Dim dy As Double = CDbl(p1.Y) - CDbl(p2.Y)

        Return CSng(
        Math.Sqrt(
            dx * dx +
            dy * dy))

    End Function


    Private Function FindLineLineIntersection(
    line1 As CadEntity.CadLine,
    line2 As CadEntity.CadLine,
    ByRef intersection As PointF) As Boolean

        Dim x1 As Double = line1.StartPoint.X
        Dim y1 As Double = line1.StartPoint.Y

        Dim x2 As Double = line1.EndPoint.X
        Dim y2 As Double = line1.EndPoint.Y

        Dim x3 As Double = line2.StartPoint.X
        Dim y3 As Double = line2.StartPoint.Y

        Dim x4 As Double = line2.EndPoint.X
        Dim y4 As Double = line2.EndPoint.Y


        Dim denominator As Double =
        (x1 - x2) * (y3 - y4) -
        (y1 - y2) * (x3 - x4)


        ' Parallel or coincident lines.
        If Math.Abs(denominator) < 0.0000001 Then

            intersection = PointF.Empty

            Return False

        End If


        Dim numeratorX As Double =
        (x1 * y2 - y1 * x2) * (x3 - x4) -
        (x1 - x2) * (x3 * y4 - y3 * x4)


        Dim numeratorY As Double =
        (x1 * y2 - y1 * x2) * (y3 - y4) -
        (y1 - y2) * (x3 * y4 - y3 * x4)


        Dim ix As Double =
        numeratorX / denominator

        Dim iy As Double =
        numeratorY / denominator


        ' Make sure the intersection is actually
        ' inside both finite line segments.

        Dim tolerance As Double = 0.000001


        If ix < Math.Min(x1, x2) - tolerance OrElse
       ix > Math.Max(x1, x2) + tolerance OrElse
       iy < Math.Min(y1, y2) - tolerance OrElse
       iy > Math.Max(y1, y2) + tolerance Then

            intersection = PointF.Empty

            Return False

        End If


        If ix < Math.Min(x3, x4) - tolerance OrElse
       ix > Math.Max(x3, x4) + tolerance OrElse
       iy < Math.Min(y3, y4) - tolerance OrElse
       iy > Math.Max(y3, y4) + tolerance Then

            intersection = PointF.Empty

            Return False

        End If


        intersection =
        New PointF(
            CSng(ix),
            CSng(iy))

        Return True

    End Function

#If DEBUG Then

    Public Sub DebugTestLineHotSpots()

        Dim testLine As New CadEntity.CadLine()

        testLine.StartPoint = New PointF(0.0F, 0.0F)
        testLine.EndPoint = New PointF(100.0F, 0.0F)

        Dim testEntities As New List(Of CadEntity)
        testEntities.Add(testLine)

        Dim testManager As New HotSpotManager(testEntities)

        Dim tolerance As Single = 1.0F

        '--------------------------------------------------
        ' Test start endpoint
        '--------------------------------------------------

        Dim result As HotSpot =
            testManager.FindHotSpot(
                New PointF(0.25F, 0.0F),
                tolerance)

        Debug.Assert(
            result IsNot Nothing,
            "Start endpoint was not detected.")

        Debug.Assert(
            result.Type = HotSpotType.Endpoint,
            "Start endpoint returned incorrect HotSpotType.")

        Debug.Assert(
            result.WorldPoint = testLine.StartPoint,
            "Start endpoint returned incorrect WorldPoint.")

        '--------------------------------------------------
        ' Test end endpoint
        '--------------------------------------------------

        result =
            testManager.FindHotSpot(
                New PointF(99.75F, 0.0F),
                tolerance)

        Debug.Assert(
            result IsNot Nothing,
            "End endpoint was not detected.")

        Debug.Assert(
    Math.Abs(result.WorldPoint.X - testLine.EndPoint.X) < 0.001F AndAlso
    Math.Abs(result.WorldPoint.Y - testLine.EndPoint.Y) < 0.001F,
    "End endpoint returned incorrect WorldPoint. " &
    "Expected=(" &
    testLine.EndPoint.X.ToString() &
    "," &
    testLine.EndPoint.Y.ToString() &
    ") Actual=(" &
    result.WorldPoint.X.ToString() &
    "," &
    result.WorldPoint.Y.ToString() &
    ").")

        Debug.Assert(
            result.Type = HotSpotType.Endpoint,
            "End endpoint returned incorrect HotSpotType.")

        Debug.Assert(
            result.WorldPoint = testLine.EndPoint,
            "End endpoint returned incorrect WorldPoint.")

        '--------------------------------------------------
        ' Test midpoint
        '--------------------------------------------------

        result =
            testManager.FindHotSpot(
                New PointF(50.25F, 0.0F),
                tolerance)

        Debug.Assert(
            result IsNot Nothing,
            "Midpoint was not detected.")

        Debug.Assert(
            result.Type = HotSpotType.Midpoint,
            "Midpoint returned incorrect HotSpotType.")

        Debug.Assert(
            result.WorldPoint =
                New PointF(50.0F, 0.0F),
            "Midpoint returned incorrect WorldPoint.")

        '--------------------------------------------------
        ' Test closest candidate
        '--------------------------------------------------

        result =
            testManager.FindHotSpot(
                New PointF(0.8F, 0.0F),
                tolerance)

        Debug.Assert(
            result IsNot Nothing,
            "Closest-candidate test found no Hot Spot.")

        Debug.Assert(
            result.Type = HotSpotType.Endpoint,
            "Closest-candidate test returned incorrect type.")

        Debug.Assert(
            result.WorldPoint = testLine.StartPoint,
            "Closest-candidate test did not select start endpoint.")

        '--------------------------------------------------
        ' Test CurrentHotSpot
        '--------------------------------------------------

        Debug.Assert(
            testManager.CurrentHotSpot Is result,
            "CurrentHotSpot was not updated correctly.")


        '--------------------------------------------------
        ' Circle center
        '--------------------------------------------------

        Dim testCircle As New CadEntity.CadCircle()
        testCircle.Center = New PointF(25.0F, 25.0F)
        testCircle.Radius = 10.0F

        Dim circleEntities As New List(Of CadEntity)
        circleEntities.Add(testCircle)

        Dim circleManager As New HotSpotManager(circleEntities)

        Dim circleResult As HotSpot =
    circleManager.FindHotSpot(
        New PointF(25.5F, 25.0F),
        tolerance)

        Debug.Assert(
    circleResult IsNot Nothing,
    "Circle center hot spot was not found.")

        Debug.Assert(
    circleResult.Type = HotSpotType.Center,
    "Circle center returned incorrect HotSpotType.")

        Debug.Assert(
    Math.Abs(circleResult.WorldPoint.X - testCircle.Center.X) < 0.001F AndAlso
    Math.Abs(circleResult.WorldPoint.Y - testCircle.Center.Y) < 0.001F,
    "Circle center returned incorrect WorldPoint. " &
    "Expected=(" &
    testCircle.Center.X.ToString() &
    "," &
    testCircle.Center.Y.ToString() &
    ") Actual=(" &
    circleResult.WorldPoint.X.ToString() &
    "," &
    circleResult.WorldPoint.Y.ToString() &
    ").")

        '--------------------------------------------------
        ' Arc center
        '--------------------------------------------------

        Dim testArc As New CadEntity.CadArc()
        testArc.Center = New PointF(50.0F, 50.0F)
        testArc.Radius = 20.0F
        testArc.StartAngle = 0.0F
        testArc.EndAngle = 90.0F
        testArc.IsClockwise = False

        Dim arcEntities As New List(Of CadEntity)
        arcEntities.Add(testArc)


        Dim arcManager As New HotSpotManager(arcEntities)

        Dim arcStartResult As HotSpot =
    arcManager.FindHotSpot(
        New PointF(70.5F, 50.0F),
        tolerance)

        Debug.Assert(
    arcStartResult IsNot Nothing,
    "Arc start endpoint hot spot was not found.")

        Debug.Assert(
    arcStartResult.Type = HotSpotType.Endpoint,
    "Arc start endpoint returned incorrect HotSpotType.")

        Debug.Assert(
    Math.Abs(arcStartResult.WorldPoint.X - 70.0F) < 0.001F AndAlso
    Math.Abs(arcStartResult.WorldPoint.Y - 50.0F) < 0.001F,
    "Arc start endpoint returned incorrect WorldPoint.")

        Dim arcEndResult As HotSpot =
    arcManager.FindHotSpot(
        New PointF(50.5F, 70.0F),
        tolerance)

        Debug.Assert(
    arcEndResult IsNot Nothing,
    "Arc end endpoint hot spot was not found.")

        Debug.Assert(
    arcEndResult.Type = HotSpotType.Endpoint,
    "Arc end endpoint returned incorrect HotSpotType.")

        Debug.Assert(
    Math.Abs(arcEndResult.WorldPoint.X - 50.0F) < 0.001F AndAlso
    Math.Abs(arcEndResult.WorldPoint.Y - 70.0F) < 0.001F,
    "Arc end endpoint returned incorrect WorldPoint.")


        Dim arcResult As HotSpot =
    arcManager.FindHotSpot(
        New PointF(50.5F, 50.0F),
        tolerance)

        Debug.Assert(
    arcResult IsNot Nothing,
    "Arc center hot spot was not found.")

        Debug.Assert(
    arcResult.Type = HotSpotType.Center,
    "Arc center returned incorrect HotSpotType.")

        Debug.Assert(
    Math.Abs(arcResult.WorldPoint.X - testArc.Center.X) < 0.001F AndAlso
    Math.Abs(arcResult.WorldPoint.Y - testArc.Center.Y) < 0.001F,
    "Arc center returned incorrect WorldPoint.")


        '--------------------------------------------------
        ' Clockwise arc midpoint
        '--------------------------------------------------

        Dim clockwiseArc As New CadEntity.CadArc()
        clockwiseArc.Center = New PointF(50.0F, 50.0F)
        clockwiseArc.Radius = 20.0F
        clockwiseArc.StartAngle = 90.0F
        clockwiseArc.EndAngle = 0.0F
        clockwiseArc.IsClockwise = True

        Dim clockwiseEntities As New List(Of CadEntity)
        clockwiseEntities.Add(clockwiseArc)

        Dim clockwiseManager As New HotSpotManager(clockwiseEntities)

        Dim clockwiseResult As HotSpot =
    clockwiseManager.FindHotSpot(
        New PointF(64.5F, 64.142F),
        tolerance)

        Debug.Assert(
    clockwiseResult IsNot Nothing,
    "Clockwise arc midpoint hot spot was not found.")

        Debug.Assert(
    clockwiseResult.Type = HotSpotType.Midpoint,
    "Clockwise arc midpoint returned incorrect HotSpotType.")

        Debug.Assert(
    Math.Abs(clockwiseResult.WorldPoint.X - 64.142F) < 0.01F AndAlso
    Math.Abs(clockwiseResult.WorldPoint.Y - 64.142F) < 0.01F,
    "Clockwise arc midpoint returned incorrect WorldPoint.")

        '--------------------------------------------------
        ' CW arc crossing zero
        '--------------------------------------------------

        Dim cwWrapArc As New CadEntity.CadArc()
        cwWrapArc.Center = New PointF(50.0F, 50.0F)
        cwWrapArc.Radius = 20.0F
        cwWrapArc.StartAngle = 10.0F
        cwWrapArc.EndAngle = 350.0F
        cwWrapArc.IsClockwise = True

        Dim cwWrapEntities As New List(Of CadEntity)
        cwWrapEntities.Add(cwWrapArc)

        Dim cwWrapManager As New HotSpotManager(cwWrapEntities)

        Dim cwWrapResult As HotSpot =
    cwWrapManager.FindHotSpot(
        New PointF(70.5F, 50.0F),
        tolerance)

        Debug.Assert(
    cwWrapResult IsNot Nothing,
    "CW wrap midpoint hot spot was not found.")

        Debug.Assert(
    cwWrapResult.Type = HotSpotType.Midpoint,
    "CW wrap midpoint returned incorrect HotSpotType.")

        Debug.Assert(
    Math.Abs(cwWrapResult.WorldPoint.X - 70.0F) < 0.01F AndAlso
    Math.Abs(cwWrapResult.WorldPoint.Y - 50.0F) < 0.01F,
    "CW wrap midpoint returned incorrect WorldPoint.")

        '--------------------------------------------------
        ' CCW wrap midpoint test
        ' Start = 350°, End = 10°
        ' Midpoint should be 0°
        '--------------------------------------------------

        Dim ccwWrapArc As New CadEntity.CadArc()

        ccwWrapArc.Center = New PointF(50.0F, 50.0F)
        ccwWrapArc.Radius = 20.0F
        ccwWrapArc.StartAngle = 350.0F
        ccwWrapArc.EndAngle = 10.0F
        ccwWrapArc.IsClockwise = False

        Dim ccwWrapEntities As New List(Of CadEntity)

        ccwWrapEntities.Add(ccwWrapArc)

        Dim ccwWrapManager As New HotSpotManager(
    ccwWrapEntities)

        Dim ccwWrapResult As HotSpot =
    ccwWrapManager.FindHotSpot(
        New PointF(70.5F, 50.0F),
        tolerance)

        Debug.Assert(
    ccwWrapResult IsNot Nothing,
    "CCW wrap midpoint hot spot was not found.")

        Debug.Assert(
    ccwWrapResult.Type = HotSpotType.Midpoint,
    "CCW wrap midpoint returned incorrect HotSpotType.")

        Debug.Assert(
    Math.Abs(ccwWrapResult.WorldPoint.X - 70.0F) < 0.01F AndAlso
    Math.Abs(ccwWrapResult.WorldPoint.Y - 50.0F) < 0.01F,
    "CCW wrap midpoint returned incorrect WorldPoint.")


        '--------------------------------------------------
        ' Line-Line intersection test
        '
        ' Line 1: (0,0) -> (100,100)
        ' Line 2: (0,100) -> (100,0)
        '
        ' Expected intersection: (50,50)
        '--------------------------------------------------

        Dim intersectionLine1 As New CadEntity.CadLine()

        intersectionLine1.StartPoint =
    New PointF(0.0F, 0.0F)

        intersectionLine1.EndPoint =
    New PointF(80.0F, 100.0F)


        Dim intersectionLine2 As New CadEntity.CadLine()

        intersectionLine2.StartPoint =
    New PointF(0.0F, 100.0F)

        intersectionLine2.EndPoint =
    New PointF(100.0F, 0.0F)

        Dim intersectionEntities As New List(Of CadEntity)

        intersectionEntities.Add(intersectionLine1)
        intersectionEntities.Add(intersectionLine2)


        Dim intersectionManager As New HotSpotManager(
    intersectionEntities)
        '
        ' This confirms the test geometry is valid.
        ' Intersection detection itself has not yet
        ' been added.

        Dim intersectionResult As HotSpot =
    intersectionManager.FindHotSpot(
        New PointF(50.5F, 50.0F),
        tolerance)

        Debug.Assert(
    intersectionResult IsNot Nothing,
    "Test geometry did not produce an existing line hot spot.")

        Debug.Assert(
    intersectionResult.Type = HotSpotType.Midpoint,
    "Existing line midpoint hot spot was not returned.")

        Debug.Assert(
    Math.Abs(intersectionResult.WorldPoint.X - 50.0F) < 0.01F AndAlso
    Math.Abs(intersectionResult.WorldPoint.Y - 50.0F) < 0.01F,
    "Existing line midpoint returned incorrect WorldPoint.")


        '--------------------------------------------------
        ' Parallel line test
        '
        ' These lines never intersect.
        ' Expected result: Nothing
        '--------------------------------------------------

        Dim parallelLine1 As New CadEntity.CadLine()

        parallelLine1.StartPoint =
    New PointF(0.0F, 0.0F)

        parallelLine1.EndPoint =
    New PointF(100.0F, 0.0F)


        Dim parallelLine2 As New CadEntity.CadLine()

        parallelLine2.StartPoint =
    New PointF(0.0F, 20.0F)

        parallelLine2.EndPoint =
    New PointF(100.0F, 20.0F)


        Dim parallelEntities As New List(Of CadEntity)

        parallelEntities.Add(parallelLine1)
        parallelEntities.Add(parallelLine2)


        Dim parallelManager As New HotSpotManager(
    parallelEntities)


        Dim parallelResult As HotSpot =
    parallelManager.FindHotSpot(
        New PointF(50.0F, 10.0F),
        tolerance)


        Debug.Assert(
    parallelResult Is Nothing,
    "Parallel lines incorrectly produced an intersection Hot Spot.")

        '--------------------------------------------------
        ' Intersection outside finite segments
        '
        ' The infinite lines intersect, but the actual
        ' line segments do not.
        '
        ' Expected result: Nothing
        '--------------------------------------------------

        Dim outsideLine1 As New CadEntity.CadLine()

        outsideLine1.StartPoint =
    New PointF(0.0F, 0.0F)

        outsideLine1.EndPoint =
    New PointF(10.0F, 10.0F)


        Dim outsideLine2 As New CadEntity.CadLine()

        outsideLine2.StartPoint =
    New PointF(20.0F, 0.0F)

        outsideLine2.EndPoint =
    New PointF(30.0F, 10.0F)


        Dim outsideEntities As New List(Of CadEntity)

        outsideEntities.Add(outsideLine1)
        outsideEntities.Add(outsideLine2)


        Dim outsideManager As New HotSpotManager(
    outsideEntities)


        Dim outsideResult As HotSpot =
    outsideManager.FindHotSpot(
        New PointF(15.0F, 15.0F),
        tolerance)


        Debug.Assert(
    outsideResult Is Nothing,
    "Segments with an intersection outside their bounds produced a Hot Spot.")

        '--------------------------------------------------
        ' Line-Circle intersection test
        '
        ' Line:   (0,50) -> (100,50)
        ' Circle: Center (50,50), Radius 20
        '
        ' Expected intersections:
        '   (30,50)
        '   (70,50)
        '--------------------------------------------------

        Dim lineCircleLine As New CadEntity.CadLine()

        lineCircleLine.StartPoint =
    New PointF(0.0F, 50.0F)

        lineCircleLine.EndPoint =
    New PointF(100.0F, 50.0F)


        Dim lineCircleCircle As New CadEntity.CadCircle()

        lineCircleCircle.Center =
    New PointF(50.0F, 50.0F)

        lineCircleCircle.Radius = 20.0F


        Dim lineCircleIntersection1 As PointF
        Dim lineCircleIntersection2 As PointF


        Dim lineCircleCount As Integer =
    FindLineCircleIntersections(
        lineCircleLine,
        lineCircleCircle,
        lineCircleIntersection1,
        lineCircleIntersection2)


        Debug.Assert(
    lineCircleCount = 2,
    "Line-circle intersection did not return two intersections.")


        Debug.Assert(
    (Math.Abs(lineCircleIntersection1.X - 30.0F) < 0.01F AndAlso
     Math.Abs(lineCircleIntersection1.Y - 50.0F) < 0.01F) OrElse
    (Math.Abs(lineCircleIntersection1.X - 70.0F) < 0.01F AndAlso
     Math.Abs(lineCircleIntersection1.Y - 50.0F) < 0.01F),
    "First line-circle intersection returned incorrect WorldPoint.")


        Debug.Assert(
    (Math.Abs(lineCircleIntersection2.X - 30.0F) < 0.01F AndAlso
     Math.Abs(lineCircleIntersection2.Y - 50.0F) < 0.01F) OrElse
    (Math.Abs(lineCircleIntersection2.X - 70.0F) < 0.01F AndAlso
     Math.Abs(lineCircleIntersection2.Y - 50.0F) < 0.01F),
    "Second line-circle intersection returned incorrect WorldPoint.")


        Debug.Assert(
    Math.Abs(
        lineCircleIntersection1.X -
        lineCircleIntersection2.X) > 0.01F,
    "Line-circle intersections were not distinct.")

        '--------------------------------------------------
        ' Test Clear
        '--------------------------------------------------

        testManager.Clear()

        Debug.Assert(
            testManager.CurrentHotSpot Is Nothing,
            "Clear() did not remove CurrentHotSpot.")

    End Sub

#End If

End Class