Imports System.Drawing
Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities

Public Class ImportedEntity

    Public Enum GeometryTypeEnum
        Unknown
        Line
        Circle
        Arc
    End Enum

    ' Property for entity type
    Public Property GeometryType As GeometryTypeEnum = GeometryTypeEnum.Unknown

    ' DXF entities
    Public Property Line As DxfLine
    Public Property Circle As DxfCircle
    Public Property Arc As DxfArc

    ' Display color
    Public Property DisplayColor As Color = Color.Black

    ' Bounding box
    Public ReadOnly Property MinX As Double
        Get
            Select Case GeometryType
                Case GeometryTypeEnum.Line
                    Return Math.Min(Line.P1.X, Line.P2.X)
                Case GeometryTypeEnum.Circle
                    Return Circle.Center.X - Circle.Radius
                Case GeometryTypeEnum.Arc
                    Return Math.Min(Arc.Center.X - Arc.Radius, Arc.Center.X + Arc.Radius) ' approximate
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
                    Return Math.Min(Arc.Center.Y - Arc.Radius, Arc.Center.Y + Arc.Radius) ' approximate
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
                    Return Math.Max(Arc.Center.X - Arc.Radius, Arc.Center.X + Arc.Radius) ' approximate
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
                    Return Math.Max(Arc.Center.Y - Arc.Radius, Arc.Center.Y + Arc.Radius) ' approximate
                Case Else
                    Return 0
            End Select
        End Get
    End Property

End Class
