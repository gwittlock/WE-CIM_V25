Public Class GeoEntityMapper


    Public Shared Function ToPointRecord(
    point As GeoPoint
) As GeoEntityRecord

        If point Is Nothing Then
            Throw New ArgumentNullException(NameOf(point))
        End If

        Return New GeoEntityRecord With {
        .ID = point.ID,
        .GeometryType = "GeoPoint",
        .X = point.X,
        .Y = point.Y
    }

    End Function


    Public Shared Function FromPointRecord(
    record As GeoEntityRecord
) As GeoPoint

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.GeometryType <> "GeoPoint" Then
            Throw New ArgumentException(
            "Record is not a GeoPoint.")
        End If


        Dim point As New GeoPoint With {
            .ID = record.ID,
            .X = record.X,
            .Y = record.Y
        }

        GeoEntity.EnsureNextIDAbove(point.ID)

        Return point


    End Function


    Public Shared Function ToLineRecord(
    line As GeoLine
) As GeoEntityRecord

        If line Is Nothing Then
            Throw New ArgumentNullException(NameOf(line))
        End If

        If line.StartPoint Is Nothing OrElse
       line.EndPoint Is Nothing Then

            Throw New ArgumentException(
            "GeoLine has a missing endpoint.")
        End If

        Return New GeoEntityRecord With {
        .ID = line.ID,
        .GeometryType = "GeoLine",
        .StartPointID = line.StartPoint.ID,
        .EndPointID = line.EndPoint.ID
    }

    End Function


    Public Shared Function FromLineRecord(
    record As GeoEntityRecord,
    pointsByID As Dictionary(Of Integer, GeoPoint)
) As GeoLine

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.GeometryType <> "GeoLine" Then
            Throw New ArgumentException(
            "Record is not a GeoLine.")
        End If

        If pointsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(pointsByID))
        End If

        If Not record.StartPointID.HasValue OrElse
       Not record.EndPointID.HasValue Then

            Throw New ArgumentException(
            "GeoLine record has missing endpoint IDs.")
        End If

        Dim startPoint As GeoPoint = Nothing
        Dim endPoint As GeoPoint = Nothing

        If Not pointsByID.TryGetValue(
        record.StartPointID.Value, startPoint) Then

            Throw New InvalidOperationException(
            "GeoLine start point could not be resolved.")
        End If

        If Not pointsByID.TryGetValue(
        record.EndPointID.Value, endPoint) Then

            Throw New InvalidOperationException(
            "GeoLine end point could not be resolved.")
        End If


        Dim line As New GeoLine With {
            .ID = record.ID,
            .StartPoint = startPoint,
            .EndPoint = endPoint
        }

        GeoEntity.EnsureNextIDAbove(line.ID)

        Return line


    End Function


    Public Shared Function ToCircleRecord(
    circle As GeoCircle
) As GeoEntityRecord

        If circle Is Nothing Then
            Throw New ArgumentNullException(NameOf(circle))
        End If

        If circle.Center Is Nothing Then
            Throw New ArgumentException(
            "GeoCircle has a missing center point.")
        End If

        Return New GeoEntityRecord With {
        .ID = circle.ID,
        .GeometryType = "GeoCircle",
        .CenterPointID = circle.Center.ID,
        .Radius = circle.Radius
    }

    End Function


    Public Shared Function FromCircleRecord(
    record As GeoEntityRecord,
    pointsByID As Dictionary(Of Integer, GeoPoint)
) As GeoCircle

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.GeometryType <> "GeoCircle" Then
            Throw New ArgumentException(
            "Record is not a GeoCircle.")
        End If

        If pointsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(pointsByID))
        End If

        If Not record.CenterPointID.HasValue Then
            Throw New ArgumentException(
            "GeoCircle record has no center point ID.")
        End If

        If Not record.Radius.HasValue Then
            Throw New ArgumentException(
            "GeoCircle record has no radius.")
        End If

        Dim centerPoint As GeoPoint = Nothing

        If Not pointsByID.TryGetValue(
        record.CenterPointID.Value, centerPoint) Then

            Throw New InvalidOperationException(
            "GeoCircle center point could not be resolved.")
        End If


        Dim circle As New GeoCircle With {
            .ID = record.ID,
            .Center = centerPoint,
            .Radius = record.Radius.Value
        }

        GeoEntity.EnsureNextIDAbove(circle.ID)

        Return circle


    End Function


    Public Shared Function ToArcRecord(
    arc As GeoArc
) As GeoEntityRecord

        If arc Is Nothing Then
            Throw New ArgumentNullException(NameOf(arc))
        End If

        If arc.StartPoint Is Nothing OrElse
       arc.EndPoint Is Nothing OrElse
       arc.Center Is Nothing Then

            Throw New ArgumentException(
            "GeoArc has a missing point reference.")
        End If

        Return New GeoEntityRecord With {
        .ID = arc.ID,
        .GeometryType = "GeoArc",
        .StartPointID = arc.StartPoint.ID,
        .EndPointID = arc.EndPoint.ID,
        .CenterPointID = arc.Center.ID,
        .Radius = arc.Radius,
        .StartAngle = arc.StartAngle,
        .EndAngle = arc.EndAngle,
        .IsClockwise = arc.IsClockwise
    }

    End Function


    Public Shared Function FromArcRecord(
    record As GeoEntityRecord,
    pointsByID As Dictionary(Of Integer, GeoPoint)
) As GeoArc

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.GeometryType <> "GeoArc" Then
            Throw New ArgumentException(
            "Record is not a GeoArc.")
        End If

        If pointsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(pointsByID))
        End If

        If Not record.StartPointID.HasValue OrElse
       Not record.EndPointID.HasValue OrElse
       Not record.CenterPointID.HasValue Then

            Throw New ArgumentException(
            "GeoArc record has missing point IDs.")
        End If

        If Not record.Radius.HasValue OrElse
       Not record.StartAngle.HasValue OrElse
       Not record.EndAngle.HasValue OrElse
       Not record.IsClockwise.HasValue Then

            Throw New ArgumentException(
            "GeoArc record has missing geometry properties.")
        End If

        Dim startPoint As GeoPoint = Nothing
        Dim endPoint As GeoPoint = Nothing
        Dim centerPoint As GeoPoint = Nothing

        If Not pointsByID.TryGetValue(
        record.StartPointID.Value, startPoint) Then

            Throw New InvalidOperationException(
            "GeoArc start point could not be resolved.")
        End If

        If Not pointsByID.TryGetValue(
        record.EndPointID.Value, endPoint) Then

            Throw New InvalidOperationException(
            "GeoArc end point could not be resolved.")
        End If

        If Not pointsByID.TryGetValue(
        record.CenterPointID.Value, centerPoint) Then

            Throw New InvalidOperationException(
            "GeoArc center point could not be resolved.")
        End If


        Dim arc As New GeoArc With {
            .ID = record.ID,
            .StartPoint = startPoint,
            .EndPoint = endPoint,
            .Center = centerPoint,
            .Radius = record.Radius.Value,
            .StartAngle = record.StartAngle.Value,
            .EndAngle = record.EndAngle.Value,
            .IsClockwise = record.IsClockwise.Value
        }

        GeoEntity.EnsureNextIDAbove(arc.ID)

        Return arc


    End Function


    Public Shared Function ToLayerRecord(
    layer As Layer,
    referenceID As Integer
) As LayerRecord

        If layer Is Nothing Then
            Throw New ArgumentNullException(NameOf(layer))
        End If

        Return New LayerRecord With {
        .ReferenceID = referenceID,
        .ID = layer.ID,
        .Name = layer.Name,
        .Color = layer.Color.ToArgb(),
        .Visible = layer.Visible
    }

    End Function


    Public Shared Function FromLayerRecord(
    record As LayerRecord
) As Layer

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        Return New Layer With {
        .ID = record.ID,
        .Name = record.Name,
        .Color = System.Drawing.Color.FromArgb(record.Color),
        .Visible = record.Visible
    }

    End Function


    Public Shared Sub SetLayerAssociation(
    entity As GeoEntity,
    record As GeoEntityRecord,
    layerReferenceID As Integer
)

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        Dim layer As Layer = TryCast(entity.Association, Layer)

        If layer Is Nothing Then
            Throw New ArgumentException(
            "GeoEntity does not have a Layer association.")
        End If

        If layerReferenceID <= 0 Then
            Throw New ArgumentOutOfRangeException(
            NameOf(layerReferenceID))
        End If

        record.AssociationType = "Layer"
        record.AssociationID = layer.ID
        record.AssociationReferenceID = layerReferenceID

    End Sub


    Public Shared Sub RestoreLayerAssociation(
    entity As GeoEntity,
    record As GeoEntityRecord,
    layersByReferenceID As Dictionary(Of Integer, Layer)
)

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If layersByReferenceID Is Nothing Then
            Throw New ArgumentNullException(NameOf(layersByReferenceID))
        End If

        If record.AssociationType <> "Layer" Then
            Throw New ArgumentException(
            "Record does not contain a Layer association.")
        End If

        If Not record.AssociationReferenceID.HasValue Then
            Throw New ArgumentException(
            "Layer association reference ID is missing.")
        End If

        Dim layer As Layer = Nothing

        If Not layersByReferenceID.TryGetValue(
        record.AssociationReferenceID.Value, layer) Then

            Throw New InvalidOperationException(
            "Layer association could not be resolved.")
        End If

        entity.Association = layer

    End Sub


    Public Shared Sub SetToolAssociation(
    entity As GeoEntity,
    record As GeoEntityRecord
)

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        Dim tool As FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool =
        TryCast(
            entity.Association,
            FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool)

        If tool Is Nothing Then
            Throw New ArgumentException(
            "GeoEntity does not have a Tool association.")
        End If

        record.AssociationType = "Tool"
        record.AssociationID = tool.ID
        record.AssociationReferenceID = Nothing

    End Sub


    Public Shared Sub RestoreToolAssociation(
    entity As GeoEntity,
    record As GeoEntityRecord,
    toolsByID As Dictionary(
        Of Integer,
        FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool)
)

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If toolsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(toolsByID))
        End If

        If record.AssociationType <> "Tool" Then
            Throw New ArgumentException(
            "Record does not contain a Tool association.")
        End If

        If Not record.AssociationID.HasValue Then
            Throw New ArgumentException(
            "Tool association ID is missing.")
        End If

        Dim tool As FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool = Nothing

        If Not toolsByID.TryGetValue(
        record.AssociationID.Value, tool) Then

            Throw New InvalidOperationException(
            "Tool association could not be resolved.")
        End If

        entity.Association = tool

    End Sub


    Public Shared Sub ClearAssociationRecord(
    record As GeoEntityRecord
)

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        record.AssociationType = Nothing
        record.AssociationID = Nothing
        record.AssociationReferenceID = Nothing

    End Sub


    Public Shared Sub SaveAssociation(
    entity As GeoEntity,
    record As GeoEntityRecord,
    layerReferenceID As Integer?
)

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If entity.Association Is Nothing Then

            ClearAssociationRecord(record)

        ElseIf TypeOf entity.Association Is Layer Then

            If Not layerReferenceID.HasValue Then
                Throw New ArgumentException(
                "Layer reference ID is required.")
            End If

            SetLayerAssociation(
            entity,
            record,
            layerReferenceID.Value)

        ElseIf TypeOf entity.Association Is
        FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool Then

            SetToolAssociation(entity, record)

        Else

            Throw New NotSupportedException(
            "Unsupported GeoEntity association type: " &
            entity.Association.GetType().FullName)

        End If

    End Sub


    Public Shared Sub RestoreAssociation(
    entity As GeoEntity,
    record As GeoEntityRecord,
    layersByReferenceID As Dictionary(Of Integer, Layer),
    toolsByID As Dictionary(
        Of Integer,
        FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool)
)

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        Select Case record.AssociationType

            Case Nothing, ""

                entity.Association = Nothing

            Case "Layer"

                RestoreLayerAssociation(
                entity,
                record,
                layersByReferenceID)

            Case "Tool"

                RestoreToolAssociation(
                entity,
                record,
                toolsByID)

            Case Else

                Throw New NotSupportedException(
                "Unsupported GeoEntity association type: " &
                record.AssociationType)

        End Select

    End Sub


    Public Shared Function ToRecord(
    entity As GeoEntity
) As GeoEntityRecord

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        If TypeOf entity Is GeoPoint Then

            Return ToPointRecord(
            DirectCast(entity, GeoPoint))

        ElseIf TypeOf entity Is GeoLine Then

            Return ToLineRecord(
            DirectCast(entity, GeoLine))

        ElseIf TypeOf entity Is GeoCircle Then

            Return ToCircleRecord(
            DirectCast(entity, GeoCircle))

        ElseIf TypeOf entity Is GeoArc Then

            Return ToArcRecord(
            DirectCast(entity, GeoArc))

        Else

            Throw New NotSupportedException(
            "Unsupported geometry type: " &
            entity.GetType().FullName)

        End If

    End Function


    Public Shared Function FromRecord(
    record As GeoEntityRecord,
    pointsByID As Dictionary(Of Integer, GeoPoint)
) As GeoEntity

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        Select Case record.GeometryType

            Case "GeoPoint"

                Return FromPointRecord(record)

            Case "GeoLine"

                Return FromLineRecord(
                record,
                pointsByID)

            Case "GeoCircle"

                Return FromCircleRecord(
                record,
                pointsByID)

            Case "GeoArc"

                Return FromArcRecord(
                record,
                pointsByID)

            Case Else

                Throw New NotSupportedException(
                "Unsupported geometry type: " &
                record.GeometryType)

        End Select

    End Function


    Public Shared Function GetReferencedPoints(
    entity As GeoEntity
) As List(Of GeoPoint)

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        Dim points As New List(Of GeoPoint)

        If TypeOf entity Is GeoLine Then

            Dim line = DirectCast(entity, GeoLine)

            points.Add(line.StartPoint)
            points.Add(line.EndPoint)

        ElseIf TypeOf entity Is GeoCircle Then

            Dim circle = DirectCast(entity, GeoCircle)

            points.Add(circle.Center)

        ElseIf TypeOf entity Is GeoArc Then

            Dim arc = DirectCast(entity, GeoArc)

            points.Add(arc.StartPoint)
            points.Add(arc.EndPoint)
            points.Add(arc.Center)

        ElseIf Not TypeOf entity Is GeoPoint Then

            Throw New NotSupportedException(
            "Unsupported geometry type: " &
            entity.GetType().FullName)

        End If

        If points.Any(Function(p) p Is Nothing) Then
            Throw New InvalidOperationException(
            "Geometry contains a missing point reference.")
        End If

        Return points

    End Function


    Public Shared Function CollectReferencedPoints(
    entities As IEnumerable(Of GeoEntity)
) As Dictionary(Of Integer, GeoPoint)

        If entities Is Nothing Then
            Throw New ArgumentNullException(NameOf(entities))
        End If

        Dim pointsByID As New Dictionary(Of Integer, GeoPoint)

        For Each entity As GeoEntity In entities

            For Each point As GeoPoint In GetReferencedPoints(entity)

                Dim existingPoint As GeoPoint = Nothing

                If pointsByID.TryGetValue(point.ID, existingPoint) Then

                    If Not Object.ReferenceEquals(existingPoint, point) Then
                        Throw New InvalidOperationException(
                        "Different GeoPoint objects share ID " &
                        point.ID.ToString() & ".")
                    End If

                Else

                    pointsByID.Add(point.ID, point)

                End If

            Next

        Next

        Return pointsByID

    End Function


    Public Shared Sub SaveReferencedPoints(
        entities As IEnumerable(Of GeoEntity),
        mm3File As MM3File,
        layersByReferenceID As Dictionary(Of Integer, Layer)
    )


        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        Dim pointsByID As Dictionary(Of Integer, GeoPoint) =
        CollectReferencedPoints(entities)

        mm3File.GeometryPoints.Clear()

        For Each point As GeoPoint In pointsByID.Values


            Dim record As GeoEntityRecord = ToPointRecord(point)

            SaveEntityAssociation(
                point,
                record,
                layersByReferenceID)

            mm3File.GeometryPoints.Add(record)


        Next

    End Sub


    Public Shared Function RestoreReferencedPoints(
        mm3File As MM3File,
        layersByReferenceID As Dictionary(Of Integer, Layer),
        toolsByID As Dictionary(
            Of Integer,
            FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool)
    ) As Dictionary(Of Integer, GeoPoint)


        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If mm3File.GeometryPoints Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 GeometryPoints collection is missing.")
        End If

        Dim pointsByID As New Dictionary(Of Integer, GeoPoint)

        For Each record As GeoEntityRecord In mm3File.GeometryPoints

            Dim point As GeoPoint = FromPointRecord(record)


            RestoreAssociation(
                point,
                record,
                layersByReferenceID,
                toolsByID)


            If pointsByID.ContainsKey(point.ID) Then
                Throw New InvalidOperationException(
                "Duplicate GeoPoint ID in MM3 file: " &
                point.ID.ToString())
            End If

            pointsByID.Add(point.ID, point)

        Next

        Return pointsByID

    End Function


    Public Shared Sub SaveGeometry(
        entities As IEnumerable(Of GeoEntity),
        mm3File As MM3File,
        layersByReferenceID As Dictionary(Of Integer, Layer)
    )


        If entities Is Nothing Then
            Throw New ArgumentNullException(NameOf(entities))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If mm3File.Geometry Is Nothing Then
            mm3File.Geometry = New List(Of GeoEntityRecord)
        End If


        ValidateRuntimeGeometryIDs(entities)

        Dim records As New List(Of GeoEntityRecord)
        Dim entityIDs As New HashSet(Of Integer)

        For Each entity As GeoEntity In entities

            If entity Is Nothing Then
                Throw New InvalidOperationException(
                "Geometry collection contains a null entity.")
            End If

            If Not entityIDs.Add(entity.ID) Then
                Throw New InvalidOperationException(
                "Duplicate geometry ID: " &
                entity.ID.ToString())
            End If


            Dim record As GeoEntityRecord = ToRecord(entity)

            SaveEntityAssociation(
                entity,
                record,
                layersByReferenceID)

            records.Add(record)


        Next

        mm3File.Geometry.Clear()
        mm3File.Geometry.AddRange(records)

    End Sub


    Public Shared Function RestoreGeometry(
    mm3File As MM3File,
    pointsByID As Dictionary(Of Integer, GeoPoint),
    layersByReferenceID As Dictionary(Of Integer, Layer),
    toolsByID As Dictionary(
        Of Integer,
        FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool)
) As Dictionary(Of Integer, GeoEntity)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If pointsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(pointsByID))
        End If


        If layersByReferenceID Is Nothing Then
            Throw New ArgumentNullException(
        NameOf(layersByReferenceID))
        End If

        If toolsByID Is Nothing Then
            Throw New ArgumentNullException(
        NameOf(toolsByID))
        End If


        If mm3File.Geometry Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 Geometry collection is missing.")
        End If

        If mm3File.GeometryPoints Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 GeometryPoints collection is missing.")
        End If

        ' Validate saved geometry IDs before restoration.
        ValidateGeometryIDs(mm3File)

        Dim entitiesByID As New Dictionary(Of Integer, GeoEntity)

        For Each record As GeoEntityRecord In mm3File.Geometry

            If record Is Nothing Then
                Throw New InvalidOperationException(
                "MM3 Geometry contains a null record.")
            End If

            Dim entity As GeoEntity = Nothing

            ' A standalone GeoPoint may also appear in
            ' GeometryPoints if other entities reference it.
            If record.GeometryType = "GeoPoint" AndAlso
           pointsByID.ContainsKey(record.ID) Then

                Dim existingPoint As GeoPoint =
                pointsByID(record.ID)

                ' Verify coordinates match.
                If existingPoint.X <> record.X OrElse
               existingPoint.Y <> record.Y Then

                    Throw New InvalidOperationException(
                    "Conflicting GeoPoint coordinates for ID " &
                    record.ID.ToString())
                End If

                ' Find the corresponding referenced point record.
                Dim pointRecord As GeoEntityRecord = Nothing

                For Each savedPoint As GeoEntityRecord In
                mm3File.GeometryPoints

                    If savedPoint.ID = record.ID Then
                        pointRecord = savedPoint
                        Exit For
                    End If

                Next

                If pointRecord Is Nothing Then
                    Throw New InvalidOperationException(
                    "Shared GeoPoint record could not be found.")
                End If

                ' Verify association information matches.
                If pointRecord.AssociationType <>
                    record.AssociationType OrElse
               pointRecord.AssociationID <>
                    record.AssociationID OrElse
               pointRecord.AssociationReferenceID <>
                    record.AssociationReferenceID Then

                    Throw New InvalidOperationException(
                    "Conflicting GeoPoint associations for ID " &
                    record.ID.ToString())
                End If

                ' Preserve the original shared point object.
                entity = existingPoint

            Else

                ' Restore the geometry using its saved IDs
                ' and previously restored point references.
                entity = FromRecord(record, pointsByID)

            End If

            ' Reject duplicate drawing entity IDs.
            If entitiesByID.ContainsKey(entity.ID) Then
                Throw New InvalidOperationException(
                "Duplicate geometry ID in MM3 file: " &
                entity.ID.ToString())
            End If

            ' Restore the original Layer, Tool,
            ' or null association.
            RestoreAssociation(
            entity,
            record,
            layersByReferenceID,
            toolsByID)

            entitiesByID.Add(entity.ID, entity)

        Next

        Return entitiesByID

    End Function


    Public Shared Sub ValidateGeometryIDs(
    mm3File As MM3File
)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If mm3File.Geometry Is Nothing OrElse
       mm3File.GeometryPoints Is Nothing Then

            Throw New InvalidOperationException(
            "MM3 geometry collections are missing.")
        End If

        Dim ids As New Dictionary(Of Integer, String)

        For Each record As GeoEntityRecord In mm3File.GeometryPoints

            If record Is Nothing OrElse
           record.GeometryType <> "GeoPoint" Then

                Throw New InvalidOperationException(
                "Invalid referenced GeoPoint record.")
            End If

            If ids.ContainsKey(record.ID) Then
                Throw New InvalidOperationException(
                "Duplicate referenced GeoPoint ID: " &
                record.ID.ToString())
            End If

            ids.Add(record.ID, "GeoPoint")

        Next

        For Each record As GeoEntityRecord In mm3File.Geometry

            If record Is Nothing Then
                Throw New InvalidOperationException(
                "Null geometry record.")
            End If

            Dim existingType As String = Nothing

            If ids.TryGetValue(record.ID, existingType) Then

                If record.GeometryType <> "GeoPoint" OrElse
               existingType <> "GeoPoint" Then

                    Throw New InvalidOperationException(
                    "Geometry ID collision: " &
                    record.ID.ToString())
                End If

            Else

                ids.Add(record.ID, record.GeometryType)

            End If

        Next

    End Sub


    Public Shared Sub ValidateRuntimeGeometryIDs(
    entities As IEnumerable(Of GeoEntity)
)

        If entities Is Nothing Then
            Throw New ArgumentNullException(NameOf(entities))
        End If

        Dim objectsByID As New Dictionary(Of Integer, GeoEntity)

        For Each entity As GeoEntity In entities

            If entity Is Nothing Then
                Throw New InvalidOperationException(
                "Geometry collection contains a null entity.")
            End If

            Dim allEntities As New List(Of GeoEntity)
            allEntities.Add(entity)
            allEntities.AddRange(GetReferencedPoints(entity))

            For Each item As GeoEntity In allEntities

                Dim existing As GeoEntity = Nothing

                If objectsByID.TryGetValue(item.ID, existing) Then

                    If Not Object.ReferenceEquals(existing, item) Then
                        Throw New InvalidOperationException(
                        "Different geometry objects share ID " &
                        item.ID.ToString() & ".")
                    End If

                Else

                    objectsByID.Add(item.ID, item)

                End If

            Next

        Next

    End Sub


    Public Shared Function CollectLayers(
        entities As IEnumerable(Of GeoEntity)
    ) As List(Of Layer)

        If entities Is Nothing Then
            Throw New ArgumentNullException(NameOf(entities))
        End If

        Dim layers As New List(Of Layer)

        For Each entity As GeoEntity In entities

            If entity Is Nothing Then
                Throw New InvalidOperationException(
                    "Geometry collection contains a null entity.")
            End If

            Dim allEntities As New List(Of GeoEntity)
            allEntities.Add(entity)
            allEntities.AddRange(GetReferencedPoints(entity))

            For Each item As GeoEntity In allEntities

                Dim layer As Layer =
                    TryCast(item.Association, Layer)

                If layer Is Nothing Then
                    Continue For
                End If


                Dim alreadyAdded As Boolean = False

                For Each existingLayer As Layer In layers

                    If Object.ReferenceEquals(existingLayer, layer) Then
                        alreadyAdded = True
                        Exit For
                    End If

                Next

                For Each existingLayer As Layer In layers

                    If Object.ReferenceEquals(existingLayer, layer) Then
                        alreadyAdded = True
                        Exit For
                    End If

                Next

                If Not alreadyAdded Then
                    layers.Add(layer)
                End If

            Next

        Next

        Return layers

    End Function

    Public Shared Function BuildLayerReferences(
    layers As IEnumerable(Of Layer)
) As Dictionary(Of Integer, Layer)

        If layers Is Nothing Then
            Throw New ArgumentNullException(NameOf(layers))
        End If

        Dim layersByReferenceID As New Dictionary(Of Integer, Layer)
        Dim referenceID As Integer = 1

        For Each layer As Layer In layers

            If layer Is Nothing Then
                Throw New InvalidOperationException(
                "Layer collection contains a null Layer.")
            End If

            For Each existingLayer As Layer In layersByReferenceID.Values

                If Object.ReferenceEquals(existingLayer, layer) Then
                    Throw New InvalidOperationException(
                    "Layer collection contains a duplicate object reference.")
                End If

            Next

            layersByReferenceID.Add(referenceID, layer)
            referenceID += 1

        Next

        Return layersByReferenceID

    End Function


    Public Shared Sub SaveLayers(
    layersByReferenceID As Dictionary(Of Integer, Layer),
    mm3File As MM3File
)

        If layersByReferenceID Is Nothing Then
            Throw New ArgumentNullException(
            NameOf(layersByReferenceID))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        Dim records As New List(Of LayerRecord)

        For Each entry As KeyValuePair(Of Integer, Layer) In
        layersByReferenceID.OrderBy(Function(item) item.Key)

            Dim record As LayerRecord =
            ToLayerRecord(entry.Value, entry.Key)

            records.Add(record)

        Next

        mm3File.Layers = records

    End Sub


    Public Shared Function FindLayerReferenceID(
    layer As Layer,
    layersByReferenceID As Dictionary(Of Integer, Layer)
) As Integer

        If layer Is Nothing Then
            Throw New ArgumentNullException(NameOf(layer))
        End If

        If layersByReferenceID Is Nothing Then
            Throw New ArgumentNullException(
            NameOf(layersByReferenceID))
        End If

        For Each entry As KeyValuePair(Of Integer, Layer) In
        layersByReferenceID

            If Object.ReferenceEquals(entry.Value, layer) Then
                Return entry.Key
            End If

        Next

        Throw New InvalidOperationException(
        "Layer object was not registered for MM3 Save.")

    End Function


    Public Shared Sub SaveEntityAssociation(
    entity As GeoEntity,
    record As GeoEntityRecord,
    layersByReferenceID As Dictionary(Of Integer, Layer)
)

        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        Dim layerReferenceID As Integer? = Nothing

        If TypeOf entity.Association Is Layer Then

            Dim layer As Layer =
            DirectCast(entity.Association, Layer)

            layerReferenceID = FindLayerReferenceID(
            layer,
            layersByReferenceID)

        End If

        SaveAssociation(
        entity,
        record,
        layerReferenceID)

    End Sub


    Public Shared Sub SaveGeometryCollections(
    entities As IEnumerable(Of GeoEntity),
    mm3File As MM3File
)

        If entities Is Nothing Then
            Throw New ArgumentNullException(NameOf(entities))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        ' Materialize once so every operation processes
        ' the same geometry collection.
        Dim geometry As List(Of GeoEntity) = entities.ToList()

        ' Validate all runtime geometry IDs.
        ValidateRuntimeGeometryIDs(geometry)

        ' Collect distinct Layer objects.
        Dim layers As List(Of Layer) =
        CollectLayers(geometry)

        ' Assign file-level Layer reference IDs.
        Dim layersByReferenceID As Dictionary(Of Integer, Layer) =
        BuildLayerReferences(layers)

        ' Save the Layer definitions.
        SaveLayers(layersByReferenceID, mm3File)

        ' Save referenced endpoints and center points.
        SaveReferencedPoints(
        geometry,
        mm3File,
        layersByReferenceID)

        ' Save actual drawing geometry.
        SaveGeometry(
        geometry,
        mm3File,
        layersByReferenceID)

        ' Validate the completed persistence records.
        ValidateGeometryIDs(mm3File)

    End Sub


    Public Shared Function RestoreGeometryCollections(
    mm3File As MM3File,
    toolsByID As Dictionary(
        Of Integer,
        FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool)
) As Dictionary(Of Integer, GeoEntity)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If toolsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(toolsByID))
        End If

        ' Validate saved geometry before restoration.
        ValidateGeometryIDs(mm3File)

        ' Restore original Layer objects.
        Dim layersByReferenceID As Dictionary(Of Integer, Layer) =
        RestoreLayers(mm3File)

        ' Restore shared endpoints and center points.
        Dim pointsByID As Dictionary(Of Integer, GeoPoint) =
        RestoreReferencedPoints(
            mm3File,
            layersByReferenceID,
            toolsByID)

        ' Restore drawing geometry and associations.
        Dim entitiesByID As Dictionary(Of Integer, GeoEntity) =
        RestoreGeometry(
            mm3File,
            pointsByID,
            layersByReferenceID,
            toolsByID)

        Return entitiesByID

    End Function


    Public Shared Function RestoreLayers(
    mm3File As MM3File
) As Dictionary(Of Integer, Layer)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If mm3File.Layers Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 Layers collection is missing.")
        End If

        Dim layersByReferenceID As New Dictionary(Of Integer, Layer)

        For Each record As LayerRecord In mm3File.Layers

            If record Is Nothing Then
                Throw New InvalidOperationException(
                "MM3 Layers contains a null record.")
            End If

            If record.ReferenceID <= 0 Then
                Throw New InvalidOperationException(
                "Invalid Layer reference ID.")
            End If

            If layersByReferenceID.ContainsKey(record.ReferenceID) Then
                Throw New InvalidOperationException(
                "Duplicate Layer reference ID: " &
                record.ReferenceID.ToString())
            End If

            Dim layer As Layer = FromLayerRecord(record)

            layersByReferenceID.Add(
            record.ReferenceID,
            layer)

        Next

        Return layersByReferenceID

    End Function


    Public Shared Sub RestorePartGeometry(
    jobSheet As JobSheet,
    mm3File As MM3File,
    entitiesByID As Dictionary(Of Integer, GeoEntity)
)

        If jobSheet Is Nothing Then
            Throw New ArgumentNullException(NameOf(jobSheet))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If entitiesByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(entitiesByID))
        End If

        If jobSheet.ManufacturingModel Is Nothing Then
            Throw New InvalidOperationException(
            "JobSheet ManufacturingModel is missing.")
        End If

        If mm3File.Geometry Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 Geometry collection is missing.")
        End If

        Dim geometry As New List(Of GeoEntity)

        ' Preserve the original saved geometry order.
        For Each record As GeoEntityRecord In mm3File.Geometry

            If record Is Nothing Then
                Throw New InvalidOperationException(
                "MM3 Geometry contains a null record.")
            End If

            Dim entity As GeoEntity = Nothing

            If Not entitiesByID.TryGetValue(record.ID, entity) Then
                Throw New InvalidOperationException(
                "PartGeometry entity could not be resolved: " &
                record.ID.ToString())
            End If

            geometry.Add(entity)

        Next

        jobSheet.ManufacturingModel.PartGeometry = geometry

    End Sub


End Class
