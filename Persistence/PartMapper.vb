
Public Class PartMapper

    Public Shared Function ToRecord(
        part As Part
    ) As PartRecord

        If part Is Nothing Then
            Throw New ArgumentNullException(NameOf(part))
        End If

        Dim record As New PartRecord With {
            .ID = part.ID,
            .Description = part.Description
        }

        For Each entity As GeoEntity In part.Geometry
            record.GeometryIDs.Add(entity.ID)
        Next

        For Each profile As GeoProfile In part.GeoProfiles
            record.GeoProfileIDs.Add(profile.ID)
        Next

        For Each feature As MachiningFeature In part.Features
            record.FeatureIDs.Add(feature.ID)
        Next

        Return record

    End Function


    Public Shared Function FromRecord(
    record As PartRecord,
    entitiesByID As Dictionary(Of Integer, GeoEntity),
    profilesByID As Dictionary(Of Integer, GeoProfile),
    featuresByID As Dictionary(Of Integer, MachiningFeature)
) As Part

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If entitiesByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(entitiesByID))
        End If

        If profilesByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(profilesByID))
        End If

        If featuresByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(featuresByID))
        End If

        Dim part As New Part With {
        .ID = record.ID,
        .Description = record.Description
    }

        For Each entityID As Integer In record.GeometryIDs

            Dim entity As GeoEntity = Nothing

            If Not entitiesByID.TryGetValue(entityID, entity) Then
                Throw New InvalidOperationException(
                "Part geometry ID could not be resolved: " &
                entityID.ToString())
            End If

            part.Geometry.Add(entity)

        Next

        For Each profileID As Integer In record.GeoProfileIDs

            Dim profile As GeoProfile = Nothing

            If Not profilesByID.TryGetValue(profileID, profile) Then
                Throw New InvalidOperationException(
                "Part profile ID could not be resolved: " &
                profileID.ToString())
            End If

            part.GeoProfiles.Add(profile)

        Next

        For Each featureID As Integer In record.FeatureIDs

            Dim feature As MachiningFeature = Nothing

            If Not featuresByID.TryGetValue(featureID, feature) Then
                Throw New InvalidOperationException(
                "Part feature ID could not be resolved: " &
                featureID.ToString())
            End If

            part.Features.Add(feature)

        Next

        Return part

    End Function


    Public Shared Sub SaveParts(
    parts As IEnumerable(Of Part),
    mm3File As MM3File
)

        If parts Is Nothing Then
            Throw New ArgumentNullException(NameOf(parts))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        Dim records As New List(Of PartRecord)
        Dim partIDs As New HashSet(Of Integer)

        For Each part As Part In parts

            If part Is Nothing Then
                Throw New InvalidOperationException(
                "Part collection contains a null Part.")
            End If

            If Not partIDs.Add(part.ID) Then
                Throw New InvalidOperationException(
                "Duplicate Part ID: " &
                part.ID.ToString())
            End If

            records.Add(ToRecord(part))

        Next

        mm3File.Parts = records

    End Sub


    Public Shared Function RestoreParts(
    mm3File As MM3File,
    entitiesByID As Dictionary(Of Integer, GeoEntity),
    profilesByID As Dictionary(Of Integer, GeoProfile),
    featuresByID As Dictionary(Of Integer, MachiningFeature)
) As Dictionary(Of Integer, Part)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If entitiesByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(entitiesByID))
        End If

        If profilesByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(profilesByID))
        End If

        If featuresByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(featuresByID))
        End If

        If mm3File.Parts Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 Parts collection is missing.")
        End If

        Dim partsByID As New Dictionary(Of Integer, Part)

        For Each record As PartRecord In mm3File.Parts

            If record Is Nothing Then
                Throw New InvalidOperationException(
                "MM3 Parts contains a null record.")
            End If

            If partsByID.ContainsKey(record.ID) Then
                Throw New InvalidOperationException(
                "Duplicate Part ID: " &
                record.ID.ToString())
            End If

            Dim part As Part = FromRecord(
            record,
            entitiesByID,
            profilesByID,
            featuresByID)

            partsByID.Add(part.ID, part)

        Next

        Return partsByID

    End Function


    Public Shared Sub RestoreJobSheetParts(
        jobSheet As JobSheet,
        mm3File As MM3File,
        partsByID As Dictionary(Of Integer, Part)
    )

        If jobSheet Is Nothing Then
            Throw New ArgumentNullException(NameOf(jobSheet))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If partsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(partsByID))
        End If

        If mm3File.Parts Is Nothing Then
            Throw New InvalidOperationException(
                "MM3 Parts collection is missing.")
        End If

        Dim restoredParts As New List(Of Part)

        ' Preserve the exact Part order from the MM3 file.
        For Each record As PartRecord In mm3File.Parts

            If record Is Nothing Then
                Throw New InvalidOperationException(
                    "MM3 Parts contains a null record.")
            End If

            Dim part As Part = Nothing

            If Not partsByID.TryGetValue(record.ID, part) Then
                Throw New InvalidOperationException(
                    "Restored Part could not be resolved. ID=" &
                    record.ID.ToString())
            End If

            If part Is Nothing Then
                Throw New InvalidOperationException(
                    "Restored Part is Nothing. ID=" &
                    record.ID.ToString())
            End If

            restoredParts.Add(part)

        Next

        jobSheet.Parts = restoredParts

    End Sub


    Public Shared Function ToRecord(
    pattern As Pattern
) As PatternRecord

        If pattern Is Nothing Then
            Throw New ArgumentNullException(NameOf(pattern))
        End If

        Dim record As New PatternRecord With {
        .ID = pattern.ID,
        .Description = pattern.Description
    }

        For Each entity As GeoEntity In pattern.Geometry
            record.GeometryIDs.Add(entity.ID)
        Next

        For Each profile As GeoProfile In pattern.GeoProfiles
            record.GeoProfileIDs.Add(profile.ID)
        Next

        For Each feature As MachiningFeature In pattern.Features
            record.FeatureIDs.Add(feature.ID)
        Next

        Return record

    End Function


    Public Shared Function ToRecord(
    instance As PatternInstance
) As PatternInstanceRecord

        If instance Is Nothing Then
            Throw New ArgumentNullException(NameOf(instance))
        End If

        Return New PatternInstanceRecord With {
        .ID = instance.ID,
        .PatternID = instance.PatternID,
        .X = instance.X,
        .Y = instance.Y,
        .Angle = instance.Angle
    }

    End Function




End Class
