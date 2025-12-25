Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities
Imports System.Drawing

Public Module DxfImport

    ' Store imported CadEntities
    Public CurrentDXFEntities As New List(Of CadEntity)

    ''' <summary>
    ''' Parse a DXF file and return a list of CadEntity objects.
    ''' Only lines, arcs, and circles are currently supported.
    ''' </summary>
    Public Function ParseDXFToCadEntities(filePath As String) As List(Of CadEntity)
        Dim entities As New List(Of CadEntity)

        Dim dxfDoc As DxfFile = DxfFile.Load(filePath)

        For Each ent In dxfDoc.Entities
            Dim cadEnt = CadEntity.FromDxf(ent)
            If cadEnt IsNot Nothing Then
                entities.Add(cadEnt)
            End If
        Next

        CurrentDXFEntities = entities
        Return entities
    End Function

    ''' <summary>
    ''' Compute scale and offsets to fit CadEntities in a panel.
    ''' </summary>
    Public Function ComputeScaleAndOffset(entities As List(Of CadEntity),
                                          panelWidth As Integer,
                                          panelHeight As Integer,
                                          padding As Integer,
                                          ByRef offsetX As Double,
                                          ByRef offsetY As Double,
                                          Optional ByRef scaleYFlip As Double = 1) As Double

        ' Get bounding box for all entities
        Dim minX As Double, minY As Double, maxX As Double, maxY As Double
        CadEntity.GetBoundingBox(entities, minX, minY, maxX, maxY)

        Dim dxfWidth As Double = maxX - minX
        Dim dxfHeight As Double = maxY - minY

        ' Compute scale to fit panel
        Dim scaleX As Double = (panelWidth - 2 * padding) / dxfWidth
        Dim scaleY As Double = (panelHeight - 2 * padding) / dxfHeight
        Dim scale As Double = Math.Min(scaleX, scaleY)

        ' Flip Y-axis for Windows drawing coordinates
        scaleYFlip = -1

        ' Compute centered offsets
        offsetX = (panelWidth - (dxfWidth * scale)) / 2 - minX * scale
        offsetY = (panelHeight + (dxfHeight * scale)) / 2 + minY * scale   ' Y flipped

        Return scale
    End Function

    ''' <summary>
    ''' Transform a CadEntity to panel coordinates with Y-axis flip.
    ''' Returns a tuple of transformed start and end points.
    ''' </summary>
    Public Function TransformEntityToPanel(entity As CadEntity,
                                           scale As Double,
                                           offsetX As Double,
                                           offsetY As Double,
                                           Optional scaleYFlip As Double = -1) As ((X As Double, Y As Double), (X As Double, Y As Double))
        Dim sp = entity.StartPoint
        Dim ep = entity.EndPoint

        Dim transformedStart = (
            sp.X * scale + offsetX,
            sp.Y * scale * scaleYFlip + offsetY
        )
        Dim transformedEnd = (
            ep.X * scale + offsetX,
            ep.Y * scale * scaleYFlip + offsetY
        )

        Return (transformedStart, transformedEnd)
    End Function

End Module
