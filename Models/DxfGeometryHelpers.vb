Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities

Public Module DxfGeometryHelpers

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

End Module
