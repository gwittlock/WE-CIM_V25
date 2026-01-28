Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities

Public Module DxfGeometryHelpers
    Public Property CADLayer As String
    Public Property CAMLayer As String

    ' ---------- LINE ----------
    Public Function GetLineStart(line As DxfLine) As DxfPoint
        Return line.P1
    End Function

    Public Function GetLineEnd(line As DxfLine) As DxfPoint
        Return line.P2
    End Function

    ' ---------- CIRCLE ----------
    Public Function GetCircleCenter(circle As DxfCircle) As DxfPoint
        Return circle.Center
    End Function

    Public Function GetCircleRadius(circle As DxfCircle) As Double
        Return circle.Radius
    End Function

    ' ---------- ARC ----------
    Public Function GetArcCenter(arc As DxfArc) As DxfPoint
        Return arc.Center
    End Function

    Public Function GetArcRadius(arc As DxfArc) As Double
        Return arc.Radius
    End Function

    ' ---------- POLYLINE ----------
    Public Function GetPolylinePoints(pl As DxfPolyline) As IEnumerable(Of DxfPoint)
        Return pl.Vertices.Select(Function(v) v.Location)
    End Function

    Public Class ImportedEntity
        Public Property Entity As DxfEntity
        Public Property LayerSetupID As Integer
        Public Property DirectiveID As Integer?
    End Class

    Public Function ToScreen(
    pt As PointF,
    scale As Double,
    ox As Double,
    oy As Double,
    worldMaxY As Double
) As PointF

        Dim sx As Single = CSng((pt.X * scale) + ox)
        Dim sy As Single = CSng((worldMaxY - pt.Y) * scale + oy)

        Return New PointF(sx, sy)
    End Function


End Module
