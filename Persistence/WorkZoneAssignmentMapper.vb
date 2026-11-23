
Public Class WorkZoneAssignmentMapper
    Public Shared Function ToRecord(
    assignment As MM3WorkZoneAssignment
) As WorkZoneAssignmentRecord

        If assignment Is Nothing OrElse
       assignment.Entity Is Nothing Then

            Throw New ArgumentException(
            "WorkZone assignment has no entity.")
        End If

        Dim record As New WorkZoneAssignmentRecord With {
        .WorkZoneIndex = assignment.WorkZoneIndex
    }

        If TypeOf assignment.Entity Is GeoEntity Then

            Dim geo As GeoEntity =
            DirectCast(assignment.Entity, GeoEntity)

            record.EntityType = "GeoEntity"
            record.EntityID = geo.ID

        ElseIf TypeOf assignment.Entity Is GeoProfile Then

            Dim profile As GeoProfile =
            DirectCast(assignment.Entity, GeoProfile)

            record.EntityType = "GeoProfile"
            record.EntityID = profile.ID

        ElseIf TypeOf assignment.Entity Is MachiningFeature Then

            Dim feature As MachiningFeature =
            DirectCast(assignment.Entity, MachiningFeature)

            record.EntityType = "MachiningFeature"
            record.EntityID = feature.ID

        ElseIf TypeOf assignment.Entity Is PatternInstance Then

            Dim instance As PatternInstance =
            DirectCast(assignment.Entity, PatternInstance)

            record.EntityType = "PatternInstance"
            record.EntityID = instance.ID

        Else

            Throw New NotSupportedException(
            "Unsupported WorkZone assignment entity type: " &
            assignment.Entity.GetType().FullName)

        End If

        Return record

    End Function
    Public Shared Function FromRecord(
    record As WorkZoneAssignmentRecord,
    model As MM3,
    profilesByID As Dictionary(Of Integer, GeoProfile),
    entitiesByID As Dictionary(Of Integer, GeoEntity)
) As MM3WorkZoneAssignment

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If model Is Nothing Then
            Throw New ArgumentNullException(NameOf(model))
        End If

        Dim entity As isSelectable = Nothing

        Select Case record.EntityType

            Case "GeoEntity"

                If entitiesByID Is Nothing Then
                    Throw New ArgumentNullException(NameOf(entitiesByID))
                End If

                Dim geoEntity As GeoEntity = Nothing

                If entitiesByID.TryGetValue(record.EntityID, geoEntity) Then
                    entity = geoEntity
                End If

            Case "GeoProfile"

                If profilesByID Is Nothing Then
                    Throw New ArgumentNullException(NameOf(profilesByID))
                End If

                Dim profile As GeoProfile = Nothing

                If profilesByID.TryGetValue(record.EntityID, profile) Then
                    entity = profile
                End If

            Case "MachiningFeature"

                entity = model.ManufacturingGeometry.
                FirstOrDefault(Function(f) f.ID = record.EntityID)

            Case "PatternInstance"

                entity = model.PatternInstances.
                FirstOrDefault(Function(i) i.ID = record.EntityID)

            Case Else

                Throw New NotSupportedException(
                "Unsupported WorkZone assignment entity type: " &
                record.EntityType)

        End Select

        If entity Is Nothing Then
            Throw New InvalidOperationException(
            "WorkZone assignment references a missing entity: " &
            record.EntityType & " ID=" & record.EntityID)
        End If

        Return New MM3WorkZoneAssignment With {
        .WorkZoneIndex = record.WorkZoneIndex,
        .Entity = entity
    }

    End Function
    Public Shared Sub RestoreWorkZoneAssignments(
    mm3File As MM3File,
    model As MM3,
    profilesByID As Dictionary(Of Integer, GeoProfile),
    entitiesByID As Dictionary(Of Integer, GeoEntity)
)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If model Is Nothing Then
            Throw New ArgumentNullException(NameOf(model))
        End If

        If mm3File.WorkZoneAssignments Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 WorkZoneAssignments collection is missing.")
        End If

        Dim restoredAssignments As New List(Of MM3WorkZoneAssignment)

        For Each record As WorkZoneAssignmentRecord In mm3File.WorkZoneAssignments

            If record Is Nothing Then
                Throw New InvalidOperationException(
                "MM3 WorkZoneAssignments contains a null record.")
            End If

            Dim assignment As MM3WorkZoneAssignment =
            FromRecord(record, model, profilesByID, entitiesByID)

            restoredAssignments.Add(assignment)

        Next

        model.WorkZoneAssignments = restoredAssignments

    End Sub


    Public Shared Sub RestoreJobSheetWorkZones(
    jobSheet As JobSheet,
    mm3File As MM3File
)

        If jobSheet Is Nothing Then
            Throw New ArgumentNullException(NameOf(jobSheet))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If mm3File.WorkZones Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 WorkZones collection is missing.")
        End If

        If jobSheet.ManufacturingModel Is Nothing Then
            Throw New InvalidOperationException(
            "JobSheet ManufacturingModel is missing.")
        End If

        ' Both properties reference the same restored list.
        jobSheet.WorkZones = mm3File.WorkZones
        jobSheet.ManufacturingModel.WorkZones = jobSheet.WorkZones

    End Sub



End Class
