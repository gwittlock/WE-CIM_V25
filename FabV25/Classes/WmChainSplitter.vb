Imports FabV25_WIN8.ToolShapes

Friend Class WmChainSplitter

    Private Const SmallTolerance As Double = 0.000001

    Public Sub New()
    End Sub

    Public Sub Split(
        chain As WmChain,
        curveRecords As IReadOnlyList(Of WmCurveRecord),
        intersections As IReadOnlyList(Of WmIntersectionRecord))

        If chain Is Nothing Then
            Throw New ArgumentNullException(NameOf(chain))
        End If

        If curveRecords Is Nothing Then
            Throw New ArgumentNullException(NameOf(curveRecords))
        End If

        If intersections Is Nothing Then
            Throw New ArgumentNullException(NameOf(intersections))
        End If

        For Each curveRecord As WmCurveRecord In curveRecords

            If curveRecord Is Nothing Then
                Continue For
            End If

            Dim currElem As WmElem =
                chain.ElementAt(curveRecord.SegmentIndex)

            If currElem Is Nothing Then
                Continue For
            End If

            Dim currSubchn As WmSubchn =
                currElem.Owner

            If currSubchn Is Nothing Then
                Continue For
            End If

            For jndx As Integer = 0 To curveRecord.Count - 1

                Dim extA As WmCurveRecordExtData =
                    curveRecord.ExtData(jndx)

                If extA Is Nothing Then
                    Continue For
                End If

                Dim intrec As WmIntersectionRecord =
                    extA.Intersection

                If intrec Is Nothing Then
                    Continue For
                End If

                Dim nextSubchn As WmSubchn =
                    currSubchn.SubchnNext

                Dim subchn As WmSubchn = Nothing

                Dim okaySplit As Boolean = True

                '==================================================
                ' Determine whether this intersection creates a
                ' subchain boundary.
                '==================================================

                If jndx = 0 Then

                    Dim otherElem As WmElem =
                        TryCast(currElem.Prev, WmElem)

                    If otherElem IsNot Nothing Then

                        Dim pointA As PointF =
                            currElem.Element.StartPoint

                        Dim pointB As PointF =
                            otherElem.Element.EndPoint

                        okaySplit =
                            PointsWithinTolerance(
                                pointA,
                                pointB,
                                SmallTolerance)

                    Else

                        subchn =
                            TryCast(currElem.Prev, WmSubchn)

                    End If

                ElseIf jndx = curveRecord.Count - 1 Then

                    Dim otherElem As WmElem =
                        TryCast(currElem.Next, WmElem)

                    If otherElem IsNot Nothing Then

                        Dim pointA As PointF =
                            currElem.Element.EndPoint

                        Dim pointB As PointF =
                            otherElem.Element.StartPoint

                        okaySplit =
                            PointsWithinTolerance(
                                pointA,
                                pointB,
                                SmallTolerance)

                    Else

                        subchn =
                            TryCast(currElem.Next, WmSubchn)

                    End If

                Else

                    Dim intersectionPoint As PointF =
                        intrec.Point

                    Dim element As IPathSegment =
                        currElem.Element

                    Dim pointA As PointF =
                        element.StartPoint

                    okaySplit =
                        Not PointsWithinTolerance(
                            pointA,
                            intersectionPoint,
                            SmallTolerance)

                    If okaySplit Then

                        Dim pointB As PointF =
                            element.EndPoint

                        okaySplit =
                            Not PointsWithinTolerance(
                                pointB,
                                intersectionPoint,
                                SmallTolerance)

                    End If

                End If

                '==================================================
                ' Create a new subchain when required.
                '==================================================

                If subchn Is Nothing AndAlso okaySplit Then

                    subchn = New WmSubchn(chain)

                    subchn.SetSubchnPrev(currSubchn)
                    subchn.SetSubchnNext(nextSubchn)

                    currSubchn.SetSubchnNext(subchn)

                    If nextSubchn IsNot Nothing Then
                        nextSubchn.SetSubchnPrev(subchn)
                    End If

                    '==================================================
                    ' Intersection at the beginning of the element.
                    '==================================================

                    If jndx = 0 Then

                        currElem.InsertBefore(subchn)

                        '==================================================
                        ' Intersection at the end of the element.
                        '==================================================

                    ElseIf jndx = curveRecord.Count - 1 Then

                        currElem.InsertAfter(subchn)

                        '==================================================
                        ' Interior intersection.
                        '
                        ' Legacy behavior:
                        '
                        '   clone = first portion
                        '   original = second portion
                        '
                        '   newElem -> subchn -> currElem
                        '==================================================

                    Else

                        Dim intersectionPoint As PointF =
                            intrec.Point

                        Dim firstElement As IPathSegment = Nothing
                        Dim secondElement As IPathSegment = Nothing

                        If Not SplitSegment(
                            currElem.Element,
                            intersectionPoint,
                            firstElement,
                            secondElement) Then

                            extA.Tag()
                            Continue For

                        End If

                        currElem.Element = secondElement

                        Dim newElem As New WmElem(
                            currSubchn,
                            firstElement)

                        currElem.InsertBefore(newElem)

                        newElem.InsertAfter(subchn)

                    End If

                    subchn.OwnerShipUpdate()

                Else

                    extA.Tag()

                End If

                '==================================================
                ' Legacy behavior:
                '
                ' if okaySplit:
                '     extA->Subchn(subchn)
                '     currSubchn = subchn
                '==================================================

                If okaySplit Then

                    extA.SubChain = subchn
                    currSubchn = subchn

                End If

            Next

        Next

    End Sub

    Private Function SplitSegment(
        element As IPathSegment,
        intersectionPoint As PointF,
        ByRef firstElement As IPathSegment,
        ByRef secondElement As IPathSegment) As Boolean

        firstElement = Nothing
        secondElement = Nothing

        If element Is Nothing Then
            Return False
        End If

        '==========================================================
        ' Line
        '==========================================================

        Dim line As LineSegment2D =
            TryCast(element, LineSegment2D)

        If line IsNot Nothing Then

            firstElement =
                New LineSegment2D(
                    line.StartPoint,
                    intersectionPoint)

            secondElement =
                New LineSegment2D(
                    intersectionPoint,
                    line.EndPoint)

            Return True

        End If

        '==========================================================
        ' Arc
        '==========================================================

        Dim arc As ArcSegment2D =
            TryCast(element, ArcSegment2D)

        If arc IsNot Nothing Then

            Dim dx As Double =
                CDbl(intersectionPoint.X) -
                CDbl(arc.Center.X)

            Dim dy As Double =
                CDbl(intersectionPoint.Y) -
                CDbl(arc.Center.Y)

            Dim intersectionAngle As Double =
                Math.Atan2(dy, dx)

            If intersectionAngle < 0.0 Then
                intersectionAngle += 2.0 * Math.PI
            End If

            firstElement =
                New ArcSegment2D(
                    arc.Center,
                    arc.Radius,
                    arc.StartAngle,
                    intersectionAngle,
                    arc.IsClockwise)

            secondElement =
                New ArcSegment2D(
                    arc.Center,
                    arc.Radius,
                    intersectionAngle,
                    arc.EndAngle,
                    arc.IsClockwise)

            Return True

        End If

        Return False

    End Function

End Class