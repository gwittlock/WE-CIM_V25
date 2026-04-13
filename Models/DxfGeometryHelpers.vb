Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities

Public Module DxfGeometryHelpers
    Public Property CADLayer As String
    Public Property CAMLayer As String


    Public Enum ColorSourceMode
        ByLayer
        ByEntity
        Fallback
    End Enum

    Public Class ImportedEntity

        '----------------------------------
        ' Geometry Type Enum
        '----------------------------------
        Public Enum GeometryTypeEnum
            Line = 0
            Circle = 1
            Arc = 2
            PolyLine = 3
            Rectangle = 4
            Unknown = -1
        End Enum

        Public Property ColorMode As ColorSourceMode
        Public Property ExplicitColor As Color
        '----------------------------------
        ' Properties
        '----------------------------------
        Public Property CAMLayer As String
        Public Property DirectiveID As Integer
        Public Property DisplayColor As Color
        Public Property GeometryType As GeometryTypeEnum

        ' Line
        'Public Property StartPoint As CadEntity.Point2D
        'Public Property EndPoint As CadEntity.Point2D

        '' Circle / Arc
        'Public Property Center As CadEntity.Point2D
        Public Property Radius As Double
        Public Property StartAngle As Double
        Public Property EndAngle As Double

        ' Polyline
        ' Public Property Points As List(Of CadEntity.Point2D)

        ' DXF entities
        Public Property Line As DxfLine
        Public Property Circle As DxfCircle
        Public Property Arc As DxfArc

        ' NEW: Original CadEntity reference
        Public Property CadEntity As CadEntity

        Public Property WindingDirection As String

        '----------------------------------
        ' Bounding Box Properties
        '----------------------------------
        Public ReadOnly Property MinX As Double
            Get
                Select Case GeometryType
                    Case GeometryTypeEnum.Line
                        Return Math.Min(Line.P1.X, Line.P2.X)
                    Case GeometryTypeEnum.Circle
                        Return Circle.Center.X - Circle.Radius
                    Case GeometryTypeEnum.Arc
                        ' Approximate bounding box for arc
                        Return Math.Min(Arc.Center.X - Arc.Radius, Arc.Center.X + Arc.Radius)
                    Case Else
                        Return 0
                End Select
            End Get
        End Property

        Public ReadOnly Property MinY As Double
            Get
                Select Case GeometryType
                    Case GeometryTypeEnum.Line
                        Return Math.Min(Line.P1.Y, Line.P2.Y)
                    Case GeometryTypeEnum.Circle
                        Return Circle.Center.Y - Circle.Radius
                    Case GeometryTypeEnum.Arc
                        ' Approximate bounding box for arc
                        Return Math.Min(Arc.Center.Y - Arc.Radius, Arc.Center.Y + Arc.Radius)
                    Case Else
                        Return 0
                End Select
            End Get
        End Property

        Public ReadOnly Property MaxX As Double
            Get
                Select Case GeometryType
                    Case GeometryTypeEnum.Line
                        Return Math.Max(Line.P1.X, Line.P2.X)
                    Case GeometryTypeEnum.Circle
                        Return Circle.Center.X + Circle.Radius
                    Case GeometryTypeEnum.Arc
                        ' Approximate bounding box for arc
                        Return Math.Max(Arc.Center.X - Arc.Radius, Arc.Center.X + Arc.Radius)
                    Case Else
                        Return 0
                End Select
            End Get
        End Property

        Public ReadOnly Property MaxY As Double
            Get
                Select Case GeometryType
                    Case GeometryTypeEnum.Line
                        Return Math.Max(Line.P1.Y, Line.P2.Y)
                    Case GeometryTypeEnum.Circle
                        Return Circle.Center.Y + Circle.Radius
                    Case GeometryTypeEnum.Arc
                        ' Approximate bounding box for arc
                        Return Math.Max(Arc.Center.Y - Arc.Radius, Arc.Center.Y + Arc.Radius)
                    Case Else
                        Return 0
                End Select
            End Get
        End Property

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



    End Class

End Module
