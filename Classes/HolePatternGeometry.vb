Public NotInheritable Class HolePatternGeometry

    Private Sub New()
    End Sub

    Public Shared Function GetLineOfHolesPoints(
        row As LineOfHolesFeature,
        sheetXExtent As Double,
        sheetYExtent As Double) As List(Of PointF)

        Dim points As New List(Of PointF)

        If row Is Nothing OrElse row.Count <= 0 Then
            Return points
        End If

        Dim repeatCount As Integer =
            Math.Max(1, row.Parameters.RepeatCount)

        Dim baseStartX As Double = row.StartX

        If row.Parameters.Centered Then

            Dim patternSpanX As Double =
                (row.Count - 1) * row.Spacing

            If row.Parameters.RepeatDirection =
                HoleRepeatDirection.X Then

                patternSpanX +=
                    (repeatCount - 1) * row.Parameters.Gap
            End If

            baseStartX =
                (sheetXExtent - patternSpanX) / 2.0

        End If

        For repeatIndex As Integer = 0 To repeatCount - 1

            Dim rowStartX As Double = baseStartX
            Dim rowStartY As Double = row.StartY

            Select Case row.Parameters.RepeatDirection

                Case HoleRepeatDirection.X
                    rowStartX += repeatIndex * row.Parameters.Gap

                Case HoleRepeatDirection.Y
                    rowStartY += repeatIndex * row.Parameters.Gap

            End Select

            For holeIndex As Integer = 0 To row.Count - 1

                Dim holeX As Double =
                    rowStartX + (holeIndex * row.Spacing)

                Dim holeY As Double = rowStartY

                points.Add(New PointF(
                    CSng(holeX),
                    CSng(holeY)))

                If row.Parameters.XMirror Then

                    Dim mirroredX As Double =
                        sheetXExtent - holeX

                    If Math.Abs(mirroredX - holeX) > 0.000001 Then
                        points.Add(New PointF(
                            CSng(mirroredX),
                            CSng(holeY)))
                    End If

                End If

                If row.Parameters.YMirror Then

                    Dim mirroredY As Double =
                        sheetYExtent - holeY

                    If Math.Abs(mirroredY - holeY) > 0.000001 Then
                        points.Add(New PointF(
                            CSng(holeX),
                            CSng(mirroredY)))
                    End If

                End If

                If row.Parameters.XMirror AndAlso
                   row.Parameters.YMirror Then

                    Dim mirroredX As Double =
                        sheetXExtent - holeX

                    Dim mirroredY As Double =
                        sheetYExtent - holeY

                    If Math.Abs(mirroredX - holeX) > 0.000001 AndAlso
                       Math.Abs(mirroredY - holeY) > 0.000001 Then

                        points.Add(New PointF(
                            CSng(mirroredX),
                            CSng(mirroredY)))
                    End If

                End If

            Next

        Next

        Return points

    End Function

End Class