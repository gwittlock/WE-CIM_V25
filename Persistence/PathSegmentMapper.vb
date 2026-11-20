Imports System.Drawing
Imports FabV25_WIN8.ToolShapes

Public Class PathSegmentMapper

    Public Shared Function ToRecord(
        segment As IPathSegment) As PathSegmentRecord

        If segment Is Nothing Then
            Throw New ArgumentNullException(NameOf(segment))
        End If

        If TypeOf segment Is LineSegment2D Then

            Dim line As LineSegment2D =
                DirectCast(segment, LineSegment2D)

            Return New PathSegmentRecord With {
                .SegmentType = PathSegmentType.Line,
                .StartX = line.StartPoint.X,
                .StartY = line.StartPoint.Y,
                .EndX = line.EndPoint.X,
                .EndY = line.EndPoint.Y
            }

        ElseIf TypeOf segment Is ArcSegment2D Then

            Dim arc As ArcSegment2D =
                DirectCast(segment, ArcSegment2D)

            Return New PathSegmentRecord With {
                .SegmentType = PathSegmentType.Arc,
                .CenterX = arc.Center.X,
                .CenterY = arc.Center.Y,
                .Radius = arc.Radius,
                .StartAngle = arc.StartAngle,
                .EndAngle = arc.EndAngle,
                .IsClockwise = arc.IsClockwise
            }

        Else

            Throw New NotSupportedException(
                "Unsupported path segment type: " &
                segment.GetType().FullName)

        End If

    End Function

    Public Shared Function FromRecord(
        record As PathSegmentRecord) As IPathSegment

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        Select Case record.SegmentType

            Case PathSegmentType.Line

                Return New LineSegment2D(
                    New PointF(
                        CSng(record.StartX),
                        CSng(record.StartY)),
                    New PointF(
                        CSng(record.EndX),
                        CSng(record.EndY)))

            Case PathSegmentType.Arc

                Return New ArcSegment2D(
                    New PointF(
                        CSng(record.CenterX),
                        CSng(record.CenterY)),
                    record.Radius,
                    record.StartAngle,
                    record.EndAngle,
                    record.IsClockwise)

            Case Else

                Throw New NotSupportedException(
                    "Unsupported path segment type: " &
                    record.SegmentType.ToString())

        End Select

    End Function

End Class