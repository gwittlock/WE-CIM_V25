Option Strict On
Option Explicit On
Imports FabV25_WIN8.AutoPunchingEngine

Public Class Polyline2D

    Public Function GetSignature() As ProfileSignature

        ' Stub implementation for now
        ' Will be filled in during circle / slot detection

        Return New ProfileSignature(ProfileShapeType.Unknown, 0)

    End Function

    Public ReadOnly Property Points As List(Of PointF)

    Public Sub New(points As IEnumerable(Of PointF))
        Me.Points = points.ToList()
    End Sub

    Public Function Clone() As Polyline2D
        Return New Polyline2D(Me.Points)
    End Function

    Public Function GetBoundingBox() As RectangleF
        Dim minX = Points.Min(Function(p) p.X)
        Dim minY = Points.Min(Function(p) p.Y)
        Dim maxX = Points.Max(Function(p) p.X)
        Dim maxY = Points.Max(Function(p) p.Y)
        Return RectangleF.FromLTRB(minX, minY, maxX, maxY)
    End Function

    Public Function GetArea() As Double
        ' Shoelace formula (absolute area; winding ignored)
        Dim area As Double = 0
        For i = 0 To Points.Count - 1
            Dim p1 = Points(i)
            Dim p2 = Points((i + 1) Mod Points.Count)
            area += (p1.X * p2.Y) - (p2.X * p1.Y)
        Next
        Return Math.Abs(area) / 2.0
    End Function

    Public Function Contains(other As Polyline2D) As Boolean
        ' Simple containment test: pick first point of other
        Return PointInPolygon(other.Points(0))
    End Function

    Private Function PointInPolygon(pt As PointF) As Boolean
        Dim inside As Boolean = False
        Dim j = Points.Count - 1

        For i = 0 To Points.Count - 1
            If ((Points(i).Y > pt.Y) <> (Points(j).Y > pt.Y)) AndAlso
               (pt.X < (Points(j).X - Points(i).X) * (pt.Y - Points(i).Y) /
                (Points(j).Y - Points(i).Y) + Points(i).X) Then
                inside = Not inside
            End If
            j = i
        Next

        Return inside
    End Function

    Public Function Offset(distance As Double, cutSide As CutSide) As Polyline2D
        ' STUB — real offset logic comes later
        ' Returning clone preserves pipeline integrity
        Return Me.Clone()
    End Function

    Public Sub SetWinding(direction As WindingDirection)
        Dim isCCW = SignedArea() > 0
        If (direction = WindingDirection.CCW AndAlso Not isCCW) OrElse
           (direction = WindingDirection.CW AndAlso isCCW) Then
            Points.Reverse()
        End If
    End Sub

    Private Function SignedArea() As Double
        Dim area As Double = 0
        For i = 0 To Points.Count - 1
            Dim p1 = Points(i)
            Dim p2 = Points((i + 1) Mod Points.Count)
            area += (p1.X * p2.Y) - (p2.X * p1.Y)
        Next
        Return area / 2.0
    End Function

    Public Function GetOrientationAngle() As Double
        ' Only meaningful for non-rotationally symmetric shapes
        ' Returns degrees [0–180)
        Dim p1 = Points(0)
        Dim p2 = Points(1)
        Dim angle = Math.Atan2(p2.Y - p1.Y, p2.X - p1.X) * 180.0 / Math.PI
        If angle < 0 Then angle += 180
        Return Math.Round(angle, 4)
    End Function

End Class
