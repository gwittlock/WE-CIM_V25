
Imports System.IO

Public Class PatternMapper

    Public Shared Function RestorePatterns(
    mm3File As MM3File,
    entitiesByID As Dictionary(Of Integer, GeoEntity),
    profilesByID As Dictionary(Of Integer, GeoProfile),
    featuresByID As Dictionary(Of Integer, MachiningFeature)
) As Dictionary(Of Integer, Pattern)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If mm3File.Patterns Is Nothing Then
            Throw New InvalidDataException(
            "MM3 Pattern collection is missing.")
        End If

        Dim patternsByID As New Dictionary(Of Integer, Pattern)

        For Each record As PatternRecord In mm3File.Patterns

            If record Is Nothing Then
                Throw New InvalidDataException(
                "MM3 contains a null Pattern record.")
            End If

            If patternsByID.ContainsKey(record.ID) Then
                Throw New InvalidDataException(
                "Duplicate Pattern ID: " & record.ID)
            End If

            Dim pattern As New Pattern With {
            .ID = record.ID,
            .Description = record.Description
        }

            For Each geometryID As Integer In record.GeometryIDs
                If Not entitiesByID.ContainsKey(geometryID) Then
                    Throw New InvalidDataException(
                    "Pattern references missing geometry ID: " & geometryID)
                End If

                pattern.Geometry.Add(entitiesByID(geometryID))
            Next

            For Each profileID As Integer In record.GeoProfileIDs
                If Not profilesByID.ContainsKey(profileID) Then
                    Throw New InvalidDataException(
                    "Pattern references missing GeoProfile ID: " & profileID)
                End If

                pattern.GeoProfiles.Add(profilesByID(profileID))
            Next

            For Each featureID As Integer In record.FeatureIDs
                If Not featuresByID.ContainsKey(featureID) Then
                    Throw New InvalidDataException(
                    "Pattern references missing manufacturing feature ID: " & featureID)
                End If

                pattern.Features.Add(featuresByID(featureID))
            Next

            patternsByID.Add(pattern.ID, pattern)

        Next

        Return patternsByID

    End Function


    Public Shared Sub RestoreJobSheetPatterns(
    jobSheet As JobSheet,
    mm3File As MM3File,
    patternsByID As Dictionary(Of Integer, Pattern)
)

        If jobSheet Is Nothing Then
            Throw New ArgumentNullException(NameOf(jobSheet))
        End If

        If jobSheet.ManufacturingModel Is Nothing Then
            Throw New InvalidDataException(
            "JobSheet Manufacturing Model is missing.")
        End If

        Dim restoredPatterns As New List(Of Pattern)

        For Each record As PatternRecord In mm3File.Patterns

            If Not patternsByID.ContainsKey(record.ID) Then
                Throw New InvalidDataException(
                "Restored Pattern not found: " & record.ID)
            End If

            restoredPatterns.Add(patternsByID(record.ID))

        Next

        jobSheet.ManufacturingModel.Patterns = restoredPatterns

    End Sub


    Public Shared Function RestorePatternInstances(
    mm3File As MM3File,
    patternsByID As Dictionary(Of Integer, Pattern)
) As List(Of PatternInstance)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If patternsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(patternsByID))
        End If

        If mm3File.PatternInstances Is Nothing Then
            Throw New InvalidDataException(
            "MM3 PatternInstance collection is missing.")
        End If

        Dim instances As New List(Of PatternInstance)
        Dim instanceIDs As New HashSet(Of Integer)

        For Each record As PatternInstanceRecord In mm3File.PatternInstances

            If record Is Nothing Then
                Throw New InvalidDataException(
                "MM3 contains a null PatternInstance record.")
            End If

            If Not instanceIDs.Add(record.ID) Then
                Throw New InvalidDataException(
                "Duplicate PatternInstance ID: " & record.ID)
            End If

            If Not patternsByID.ContainsKey(record.PatternID) Then
                Throw New InvalidDataException(
                "PatternInstance references missing Pattern ID: " &
                record.PatternID)
            End If

            Dim instance As New PatternInstance With {
            .ID = record.ID,
            .PatternID = record.PatternID,
            .X = record.X,
            .Y = record.Y,
            .Angle = record.Angle
        }

            instances.Add(instance)

        Next

        Return instances

    End Function



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




End Class
