
Public Class GeoProfileMapper

    Public Shared Function ToRecord(
        profile As GeoProfile
    ) As GeoProfileRecord

        If profile Is Nothing Then
            Throw New ArgumentNullException(NameOf(profile))
        End If

        Dim record As New GeoProfileRecord With {
            .ID = profile.ID,
            .IsClosed = profile.IsClosed,
            .SignedArea = profile.SignedArea
        }

        If profile.Entities Is Nothing OrElse
           profile.Segments Is Nothing Then

            Throw New InvalidOperationException(
                "GeoProfile contains a missing collection.")
        End If

        For Each entity As GeoEntity In profile.Entities

            If entity Is Nothing Then
                Throw New InvalidOperationException(
                    "GeoProfile contains a null entity.")
            End If

            record.EntityIDs.Add(entity.ID)

        Next

        For Each segment As GeoProfileSegment In profile.Segments

            If segment Is Nothing OrElse
               segment.Entity Is Nothing Then

                Throw New InvalidOperationException(
                    "GeoProfile contains an invalid segment.")
            End If

            record.Segments.Add(
                New GeoProfileSegmentRecord With {
                    .EntityID = segment.Entity.ID,
                    .Reversed = segment.Reversed
                })

        Next

        Return record

    End Function


    Public Shared Function FromRecord(
    record As GeoProfileRecord,
    entitiesByID As Dictionary(Of Integer, GeoEntity)
) As GeoProfile

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If entitiesByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(entitiesByID))
        End If

        If record.EntityIDs Is Nothing OrElse
       record.Segments Is Nothing Then

            Throw New InvalidOperationException(
            "GeoProfile record contains a missing collection.")
        End If

        Dim profile As New GeoProfile With {
        .ID = record.ID,
        .IsClosed = record.IsClosed,
        .SignedArea = record.SignedArea
    }

        ' Restore original entity membership.
        For Each entityID As Integer In record.EntityIDs

            Dim entity As GeoEntity = Nothing

            If Not entitiesByID.TryGetValue(entityID, entity) Then
                Throw New InvalidOperationException(
                "GeoProfile entity could not be resolved: " &
                entityID.ToString())
            End If

            profile.Entities.Add(entity)

        Next

        ' Restore original segment order and direction.
        For Each segmentRecord As GeoProfileSegmentRecord In
        record.Segments

            If segmentRecord Is Nothing Then
                Throw New InvalidOperationException(
                "GeoProfile contains a null segment record.")
            End If

            Dim entity As GeoEntity = Nothing

            If Not entitiesByID.TryGetValue(
            segmentRecord.EntityID, entity) Then

                Throw New InvalidOperationException(
                "GeoProfile segment entity could not be resolved: " &
                segmentRecord.EntityID.ToString())
            End If

            profile.Segments.Add(
            New GeoProfileSegment With {
                .Entity = entity,
                .Reversed = segmentRecord.Reversed
            })

        Next

        Return profile

    End Function


    Public Shared Sub SaveProfiles(
    profiles As IEnumerable(Of GeoProfile),
    mm3File As MM3File
)

        If profiles Is Nothing Then
            Throw New ArgumentNullException(NameOf(profiles))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        Dim records As New List(Of GeoProfileRecord)
        Dim profileIDs As New HashSet(Of Integer)

        For Each profile As GeoProfile In profiles

            If profile Is Nothing Then
                Throw New InvalidOperationException(
                "GeoProfile collection contains a null profile.")
            End If

            If Not profileIDs.Add(profile.ID) Then
                Throw New InvalidOperationException(
                "Duplicate GeoProfile ID: " &
                profile.ID.ToString())
            End If

            records.Add(ToRecord(profile))

        Next





        mm3File.GeoProfiles = records

    End Sub


    Public Shared Function RestoreProfiles(
    mm3File As MM3File,
    entitiesByID As Dictionary(Of Integer, GeoEntity)
) As Dictionary(Of Integer, GeoProfile)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If entitiesByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(entitiesByID))
        End If

        If mm3File.GeoProfiles Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 GeoProfiles collection is missing.")
        End If

        Dim profilesByID As New Dictionary(Of Integer, GeoProfile)

        For Each record As GeoProfileRecord In mm3File.GeoProfiles

            If record Is Nothing Then
                Throw New InvalidOperationException(
                "MM3 GeoProfiles contains a null record.")
            End If

            If profilesByID.ContainsKey(record.ID) Then
                Throw New InvalidOperationException(
                "Duplicate GeoProfile ID: " &
                record.ID.ToString())
            End If

            Dim profile As GeoProfile =
            FromRecord(record, entitiesByID)

            profilesByID.Add(profile.ID, profile)

        Next

        Return profilesByID

    End Function


End Class
