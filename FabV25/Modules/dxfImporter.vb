Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports FabV25_WIN8.CadEntity

Module dxfImporter

    '--------------------------------------------
    ' Interface for DXF importers
    '--------------------------------------------
    Public Interface IDxfImporter
        Function Import(filePath As String) As List(Of ModelEntity)
    End Interface

    '--------------------------------------------
    ' IxMilia implementation of DXF importer
    '--------------------------------------------
    Public Class DxfImporter_IxMilia
        Implements IDxfImporter

        Public Function Import(filePath As String) As List(Of ModelEntity) _
            Implements IDxfImporter.Import

            Dim doc As DxfFile = DxfFile.Load(filePath)
            Dim results As New List(Of ModelEntity)

            For Each ent As DxfEntity In doc.Entities
                Dim model = ConvertEntity(ent)
                If model IsNot Nothing Then
                    model.Layer = ent.Layer
                    results.Add(model)
                End If
            Next

            Return results
        End Function


        '-------------------------------------------------
        ' Convert IxMilia entity to our internal ModelEntity
        '-------------------------------------------------
        Private Function ConvertEntity(ent As DxfEntity) As ModelEntity

            If ent.EntityTypeString = "LINE" Then
                Dim l = DirectCast(ent, DxfLine)
                Return New ModelLine(
                    New Point2D(l.P1.X, l.P1.Y),
                    New Point2D(l.P2.X, l.P2.Y)
                )

            ElseIf ent.EntityTypeString = "CIRCLE" Then
                Dim c = DirectCast(ent, DxfCircle)
                Return New ModelCircle(
                    New Point2D(c.Center.X, c.Center.Y),
                    c.Radius
                )

            ElseIf ent.EntityTypeString = "ARC" Then
                Dim a = DirectCast(ent, DxfArc)
                ' Ensure angles are stored in degrees
                Return New ModelArc(
                    New Point2D(a.Center.X, a.Center.Y),
                    a.Radius,
                    a.StartAngle,   ' already in degrees
                    a.EndAngle      ' already in degrees
                )

            ElseIf ent.EntityTypeString = "POLYLINE" Then
                Dim pl = DirectCast(ent, DxfPolyline)
                Dim pts = pl.Vertices _
                            .Select(Function(v) New Point2D(v.Location.X, v.Location.Y)) _
                            .ToList()
                Return New ModelPolyline(pts, pl.IsClosed)
            End If

        End Function

    End Class

    Public Structure Point2D
        Public Property X As Double
        Public Property Y As Double

        Public Sub New(x As Double, y As Double)
            Me.X = x
            Me.Y = y
        End Sub
    End Structure

    Public MustInherit Class ModelEntity

        Public Property Layer As String = String.Empty

        ' Bounding box (model space)
        Public MustOverride ReadOnly Property MinX As Double
        Public MustOverride ReadOnly Property MinY As Double
        Public MustOverride ReadOnly Property MaxX As Double
        Public MustOverride ReadOnly Property MaxY As Double

    End Class

    Public Class ModelLine
        Inherits ModelEntity

        Public Property StartPoint As Point2D
        Public Property EndPoint As Point2D

        Public Sub New(p1 As Point2D, p2 As Point2D)
            StartPoint = p1
            EndPoint = p2
        End Sub

        Public Overrides ReadOnly Property MinX As Double
            Get
                Return Math.Min(StartPoint.X, EndPoint.X)
            End Get
        End Property

        Public Overrides ReadOnly Property MinY As Double
            Get
                Return Math.Min(StartPoint.Y, EndPoint.Y)
            End Get
        End Property

        Public Overrides ReadOnly Property MaxX As Double
            Get
                Return Math.Max(StartPoint.X, EndPoint.X)
            End Get
        End Property

        Public Overrides ReadOnly Property MaxY As Double
            Get
                Return Math.Max(StartPoint.Y, EndPoint.Y)
            End Get
        End Property
    End Class

    Public Class ModelCircle
        Inherits ModelEntity

        Public Property Center As Point2D
        Public Property Radius As Double

        Public Sub New(center As Point2D, radius As Double)
            Me.Center = center
            Me.Radius = radius
        End Sub

        Public Overrides ReadOnly Property MinX As Double
            Get
                Return Center.X - Radius
            End Get
        End Property

        Public Overrides ReadOnly Property MinY As Double
            Get
                Return Center.Y - Radius
            End Get
        End Property

        Public Overrides ReadOnly Property MaxX As Double
            Get
                Return Center.X + Radius
            End Get
        End Property

        Public Overrides ReadOnly Property MaxY As Double
            Get
                Return Center.Y + Radius
            End Get
        End Property
    End Class

    Public Class ModelArc
        Inherits ModelEntity

        Public Property Center As Point2D
        Public Property Radius As Double
        Public Property StartAngle As Double   ' degrees
        Public Property EndAngle As Double     ' degrees

        Public Sub New(center As Point2D, radius As Double, startAngle As Double, endAngle As Double)
            Me.Center = center
            Me.Radius = radius
            Me.StartAngle = startAngle
            Me.EndAngle = endAngle
        End Sub

        ' Conservative bounding box (full circle)
        Public Overrides ReadOnly Property MinX As Double
            Get
                Return Center.X - Radius
            End Get
        End Property

        Public Overrides ReadOnly Property MinY As Double
            Get
                Return Center.Y - Radius
            End Get
        End Property

        Public Overrides ReadOnly Property MaxX As Double
            Get
                Return Center.X + Radius
            End Get
        End Property

        Public Overrides ReadOnly Property MaxY As Double
            Get
                Return Center.Y + Radius
            End Get
        End Property
    End Class

    Public Class ModelPolyline
        Inherits ModelEntity

        Public Property Points As List(Of Point2D)
        Public Property IsClosed As Boolean

        Public Sub New(points As List(Of Point2D), isClosed As Boolean)
            Me.Points = points
            Me.IsClosed = isClosed
        End Sub

        Public Overrides ReadOnly Property MinX As Double
            Get
                Return Points.Min(Function(p) p.X)
            End Get
        End Property

        Public Overrides ReadOnly Property MinY As Double
            Get
                Return Points.Min(Function(p) p.Y)
            End Get
        End Property

        Public Overrides ReadOnly Property MaxX As Double
            Get
                Return Points.Max(Function(p) p.X)
            End Get
        End Property

        Public Overrides ReadOnly Property MaxY As Double
            Get
                Return Points.Max(Function(p) p.Y)
            End Get
        End Property
    End Class


End Module
