Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.Drawing

Public Module DxfGeometryHelpers

    Public Property CADLayer As String
    Public Property CAMLayer As String

    '----------------------------------
    ' GLOBAL ENUM (FIXES YOUR ERROR)
    '----------------------------------
    Public Enum GeometryTypeEnum
        Line = 0
        Circle = 1
        Arc = 2
        PolyLine = 3
        Rectangle = 4
        Point = 5
        Unknown = -1
    End Enum

    Public Enum ColorSourceMode
        ByLayer
        ByEntity
        Fallback
    End Enum

    '----------------------------------
    ' IMPORTED ENTITY
    '----------------------------------
    Public Class ImportedEntity

        Public Property ColorMode As ColorSourceMode
        Public Property ExplicitColor As Color

        '----------------------------------
        ' Properties
        '----------------------------------
        Public Property CAMLayer As String
        Public Property DirectiveID As Integer
        Public Property DisplayColor As Color
        Public Property GeometryType As GeometryTypeEnum

        Public Property Radius As Double
        Public Property StartAngle As Double
        Public Property EndAngle As Double

        ' DXF entities
        Public Property Line As DxfLine
        Public Property Circle As DxfCircle
        Public Property Arc As DxfArc
        Public Property Point As DxfPoint

        ' Link back to runtime entity
        Public Property CadEntity As CadEntity

        Public Property WindingDirection As String

        '----------------------------------
        ' GLOBAL ENUM (FIXES YOUR ERROR)
        '----------------------------------
        Public Enum GeometryTypeEnum
            Line = 0
            Circle = 1
            Arc = 2
            PolyLine = 3
            Rectangle = 4
            Point = 5
            Unknown = -1
        End Enum
        '----------------------------------
        ' BOUNDING BOX (SAFE + POINT SUPPORT)
        '----------------------------------
        Public ReadOnly Property MinX As Double
            Get
                Select Case GeometryType
                    Case GeometryTypeEnum.Line
                        If Line Is Nothing Then Return 0
                        Return Math.Min(Line.P1.X, Line.P2.X)

                    Case GeometryTypeEnum.Circle
                        If Circle Is Nothing Then Return 0
                        Return Circle.Center.X - Circle.Radius

                    Case GeometryTypeEnum.Arc
                        If Arc Is Nothing Then Return 0
                        Return Arc.Center.X - Arc.Radius

                    Case GeometryTypeEnum.Point
                        Return Point.X

                    Case Else
                        Return 0
                End Select
            End Get
        End Property

        Public ReadOnly Property MinY As Double
            Get
                Select Case GeometryType
                    Case GeometryTypeEnum.Line
                        If Line Is Nothing Then Return 0
                        Return Math.Min(Line.P1.Y, Line.P2.Y)

                    Case GeometryTypeEnum.Circle
                        If Circle Is Nothing Then Return 0
                        Return Circle.Center.Y - Circle.Radius

                    Case GeometryTypeEnum.Arc
                        If Arc Is Nothing Then Return 0
                        Return Arc.Center.Y - Arc.Radius

                    Case GeometryTypeEnum.Point

                        Return Point.Y

                    Case Else
                        Return 0
                End Select
            End Get
        End Property

        Public ReadOnly Property MaxX As Double
            Get
                Select Case GeometryType
                    Case GeometryTypeEnum.Line
                        If Line Is Nothing Then Return 0
                        Return Math.Max(Line.P1.X, Line.P2.X)

                    Case GeometryTypeEnum.Circle
                        If Circle Is Nothing Then Return 0
                        Return Circle.Center.X + Circle.Radius

                    Case GeometryTypeEnum.Arc
                        If Arc Is Nothing Then Return 0
                        Return Arc.Center.X + Arc.Radius

                    Case GeometryTypeEnum.Point
                        Return Point.X

                    Case Else
                        Return 0
                End Select
            End Get
        End Property

        Public ReadOnly Property MaxY As Double
            Get
                Select Case GeometryType
                    Case GeometryTypeEnum.Line
                        If Line Is Nothing Then Return 0
                        Return Math.Max(Line.P1.Y, Line.P2.Y)

                    Case GeometryTypeEnum.Circle
                        If Circle Is Nothing Then Return 0
                        Return Circle.Center.Y + Circle.Radius

                    Case GeometryTypeEnum.Arc
                        If Arc Is Nothing Then Return 0
                        Return Arc.Center.Y + Arc.Radius

                    Case GeometryTypeEnum.Point
                        Return Point.Y
                    Case Else
                        Return 0
                End Select
            End Get
        End Property

        Public Function GetBounds() As RectangleF

            Select Case GeometryType

                Case GeometryTypeEnum.Line
                    If Line Is Nothing Then Return RectangleF.Empty

                    Dim minX = Math.Min(Line.P1.X, Line.P2.X)
                    Dim maxX = Math.Max(Line.P1.X, Line.P2.X)
                    Dim minY = Math.Min(Line.P1.Y, Line.P2.Y)
                    Dim maxY = Math.Max(Line.P1.Y, Line.P2.Y)

                    Return RectangleF.FromLTRB(minX, minY, maxX, maxY)

                Case GeometryTypeEnum.Circle
                    If Circle Is Nothing Then Return RectangleF.Empty

                    Return New RectangleF(
                Circle.Center.X - Circle.Radius,
                Circle.Center.Y - Circle.Radius,
                Circle.Radius * 2,
                Circle.Radius * 2)

                Case GeometryTypeEnum.Arc
                    If Arc Is Nothing Then Return RectangleF.Empty

                    Return New RectangleF(
                Arc.Center.X - Arc.Radius,
                Arc.Center.Y - Arc.Radius,
                Arc.Radius * 2,
                Arc.Radius * 2)

                Case GeometryTypeEnum.Point

                    Const eps As Single = 0.001F

                    Return New RectangleF(CSng(Point.X - eps), CSng(Point.Y - eps), eps * 2, eps * 2)

                Case Else
                    Return RectangleF.Empty

            End Select

        End Function

        '----------------------------------
        ' HELPERS
        '----------------------------------

        ' LINE
        Public Function GetLineStart(line As DxfLine) As DxfPoint
            Return line.P1
        End Function

        Public Function GetLineEnd(line As DxfLine) As DxfPoint
            Return line.P2
        End Function

        ' CIRCLE
        Public Function GetCircleCenter(circle As DxfCircle) As DxfPoint
            Return circle.Center
        End Function

        Public Function GetCircleRadius(circle As DxfCircle) As Double
            Return circle.Radius
        End Function

        ' ARC
        Public Function GetArcCenter(arc As DxfArc) As DxfPoint
            Return arc.Center
        End Function

        Public Function GetArcRadius(arc As DxfArc) As Double
            Return arc.Radius
        End Function

        ' POLYLINE
        Public Function GetPolylinePoints(pl As DxfPolyline) As IEnumerable(Of DxfPoint)
            Return pl.Vertices.Select(Function(v) v.Location)
        End Function

    End Class

End Module