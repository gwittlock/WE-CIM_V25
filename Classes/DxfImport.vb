Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.Drawing

Public Module DxfImport

    Public CurrentDXFLines As New List(Of LineF)

    Public Class LineF
        Public Property X1 As Single
        Public Property Y1 As Single
        Public Property X2 As Single
        Public Property Y2 As Single

        Public Sub New(x1 As Single, y1 As Single, x2 As Single, y2 As Single)
            Me.X1 = x1
            Me.Y1 = y1
            Me.X2 = x2
            Me.Y2 = y2
        End Sub
    End Class


    Public Function ParseDXFToLines(filePath As String) As List(Of LineF)
        Dim lines As New List(Of LineF)

        Dim dxfDoc As DxfFile = DxfFile.Load(filePath)

        For Each entity In dxfDoc.Entities
            If TypeOf entity Is DxfLine Then
                Dim ln = CType(entity, DxfLine)
                lines.Add(New LineF(CSng(ln.P1.X), CSng(ln.P1.Y), CSng(ln.P2.X), CSng(ln.P2.Y)))
            End If
        Next

        Return lines
    End Function


    ' Compute bounding box of a set of lines
    Public Sub GetBoundingBox(lines As List(Of LineF), ByRef minX As Single, ByRef minY As Single, ByRef maxX As Single, ByRef maxY As Single)
            If lines Is Nothing OrElse lines.Count = 0 Then
                minX = 0 : minY = 0 : maxX = 0 : maxY = 0
                Return
            End If

            minX = lines.Min(Function(l) Math.Min(l.X1, l.X2))
            minY = lines.Min(Function(l) Math.Min(l.Y1, l.Y2))
            maxX = lines.Max(Function(l) Math.Max(l.X1, l.X2))
            maxY = lines.Max(Function(l) Math.Max(l.Y1, l.Y2))
        End Sub

        ' Compute scale and offsets to fit lines in a panel
        Public Function ComputeScaleAndOffset(lines As List(Of LineF), panelWidth As Integer, panelHeight As Integer, padding As Integer,
                                              ByRef offsetX As Single, ByRef offsetY As Single, Optional ByRef scaleYFlip As Single = 1) As Single
            Dim minX As Single, minY As Single, maxX As Single, maxY As Single
            GetBoundingBox(lines, minX, minY, maxX, maxY)

            Dim dxfWidth As Single = maxX - minX
            Dim dxfHeight As Single = maxY - minY

            Dim scaleX As Single = (panelWidth - 2 * padding) / dxfWidth
            Dim scaleY As Single = (panelHeight - 2 * padding) / dxfHeight
            Dim scale As Single = Math.Min(scaleX, scaleY)

            ' Flip Y-axis for Windows drawing coordinates
            scaleYFlip = -1

            offsetX = (panelWidth - (dxfWidth * scale)) / 2 - minX * scale
            offsetY = (panelHeight + (dxfHeight * scale)) / 2 + minY * scale   ' Y flipped

            Return scale
        End Function

        ' Transform a line to panel coordinates with Y-axis flip
        Public Function TransformLine(line As LineF, scale As Single, offsetX As Single, offsetY As Single, Optional scaleYFlip As Single = -1) As LineF
            Return New LineF(
                line.X1 * scale + offsetX,
                line.Y1 * scale * scaleYFlip + offsetY,
                line.X2 * scale + offsetX,
                line.Y2 * scale * scaleYFlip + offsetY
            )
        End Function

    End Module
