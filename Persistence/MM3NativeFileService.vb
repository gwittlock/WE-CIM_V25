
Imports System.IO
Imports Newtonsoft.Json

Public Class MM3NativeFileService

    Public Shared Function ReadNativeFile(
        filePath As String
    ) As MM3NativeFile

        If String.IsNullOrWhiteSpace(filePath) Then
            Throw New ArgumentException(
                "MM3 file path is missing.",
                NameOf(filePath))
        End If

        If Not File.Exists(filePath) Then
            Throw New FileNotFoundException(
                "MM3 file was not found.",
                filePath)
        End If

        Dim json As String = File.ReadAllText(filePath)

        Dim nativeFile As MM3NativeFile =
            JsonConvert.DeserializeObject(Of MM3NativeFile)(json)

        If nativeFile Is Nothing Then
            Throw New InvalidDataException(
                "MM3 file does not contain a valid native file object.")
        End If



        If nativeFile.Header Is Nothing Then
            Throw New InvalidDataException(
        "MM3 file header is missing.")
        End If

        If nativeFile.Header.Format <> "WE-CIM-MM3" Then
            Throw New InvalidDataException(
        "Invalid MM3 native file format.")
        End If

        If nativeFile.Header.Version <> 1 Then
            Throw New InvalidDataException(
        "Unsupported MM3 native file version: " &
        nativeFile.Header.Version.ToString())
        End If




        If nativeFile.ManufacturingModel Is Nothing Then
            Throw New InvalidDataException(
                "MM3 file is missing its Manufacturing Model section.")
        End If


        If nativeFile.ManufacturingModel.FormatVersion <> 1 Then

            Throw New InvalidDataException(
        "Unsupported MM3 Manufacturing Model format version: " &
        nativeFile.ManufacturingModel.FormatVersion.ToString())

        End If


        Return nativeFile

    End Function


    Public Shared Function RestoreJobSheet(
    nativeFile As MM3NativeFile
) As JobSheet

        If nativeFile Is Nothing Then
            Throw New ArgumentNullException(NameOf(nativeFile))
        End If

        Dim mm3File As MM3File = nativeFile.ManufacturingModel

        If mm3File Is Nothing Then
            Throw New InvalidDataException(
            "MM3 Manufacturing Model section is missing.")
        End If

        Dim jobSheet As New JobSheet With {
        .MaterialSheetID = mm3File.MaterialSheetID,
        .Length = mm3File.Length,
        .Width = mm3File.Width
    }

        WorkZoneAssignmentMapper.RestoreJobSheetWorkZones(jobSheet, mm3File)


        If nativeFile.ManufacturingEnvironment Is Nothing Then
            Throw New InvalidDataException(
        "MM3 Manufacturing Environment section is missing.")
        End If

        If nativeFile.ManufacturingEnvironment.Tools Is Nothing Then
            Throw New InvalidDataException(
        "MM3 saved tool collection is missing.")
        End If

        Dim savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool) =
    nativeFile.ManufacturingEnvironment.Tools


        Dim entitiesByID As Dictionary(Of Integer, GeoEntity) =
    GeoEntityMapper.RestoreGeometryCollections(
        mm3File,
        savedTools)


        GeoEntityMapper.RestorePartGeometry(
    jobSheet,
    mm3File,
    entitiesByID)


        Dim profilesByID As Dictionary(Of Integer, GeoProfile) =
    GeoProfileMapper.RestoreProfiles(
        mm3File,
        entitiesByID)


        Dim featuresByID As Dictionary(Of Integer, MachiningFeature) =
    MachiningFeatureMapper.RestoreManufacturingFeatures(
        mm3File,
        savedTools)


        MachiningFeatureMapper.RestoreJobSheetManufacturingGeometry(
    jobSheet,
    mm3File,
    featuresByID)


        Dim partsByID As Dictionary(Of Integer, Part) =
    PartMapper.RestoreParts(
        mm3File,
        entitiesByID,
        profilesByID,
        featuresByID)

        PartMapper.RestoreJobSheetParts(
    jobSheet,
    mm3File,
    partsByID)


        WorkZoneAssignmentMapper.RestoreWorkZoneAssignments(
    mm3File,
    jobSheet.ManufacturingModel)


        Dim patternsByID As Dictionary(Of Integer, Pattern) =
    PatternMapper.RestorePatterns(
        mm3File,
        entitiesByID,
        profilesByID,
        featuresByID)

        PatternMapper.RestoreJobSheetPatterns(
    jobSheet,
    mm3File,
    patternsByID)


        Dim restoredInstances As List(Of PatternInstance) =
    PatternMapper.RestorePatternInstances(
        mm3File,
        patternsByID)

        jobSheet.ManufacturingModel.PatternInstances =
    restoredInstances


        Return jobSheet

    End Function

    Public Shared Function BuildManufacturingEnvironment(
    job As Job,
    context As JobContext) As MM3ManufacturingEnvironment

        If job Is Nothing Then
            Throw New ArgumentNullException(NameOf(job))
        End If

        If context Is Nothing Then
            Throw New ArgumentNullException(NameOf(context))
        End If

        If context.MachineDefinition Is Nothing Then
            Throw New InvalidDataException(
            "Current machine definition is missing.")
        End If

        If job.MachineID <> context.MachineDefinition.ID Then
            Throw New InvalidDataException(
            "Job machine ID does not match the active machine definition.")
        End If

        Dim environment As New MM3ManufacturingEnvironment With {
        .Machine = context.MachineDefinition
    }

        Dim selectedMachine As Machine =
    AppData.Machines.FirstOrDefault(
        Function(m) m.ID = job.MachineID)

        If selectedMachine Is Nothing Then
            Throw New InvalidDataException(
        "Selected machine was not found in configuration data.")
        End If
        If selectedMachine.TypeID <>
    context.MachineDefinition.TypeID Then

            Throw New InvalidDataException(
        "Machine type does not match the active machine definition.")
        End If

        environment.MachineConfiguration = selectedMachine

        Dim selectedMachineType As MachineType =
    AppData.MachineTypes.FirstOrDefault(
        Function(mt) mt.ID = selectedMachine.TypeID)

        If selectedMachineType Is Nothing Then
            Throw New InvalidDataException(
        "Selected machine type was not found in configuration data.")
        End If

        environment.MachineType = selectedMachineType

        environment.MachineTypeAttributes =
    AppData.MachineTypeAttributes.
    Where(Function(a) a.MachineTypeID = selectedMachineType.ID).
    ToList()

        environment.MachineToolTypes =
    AppData.MachineToolTypes.
    Where(Function(mtt) mtt.MachineTypeID = selectedMachineType.ID).
    ToList()



        environment.MachineAttributes =
    AppData.MachineAttributes.
    Where(Function(a) a.MachineID = selectedMachine.ID).
    ToList()

        environment.MachineAttributeTypes =
    AppData.MachineAttributeTypes.
    Where(Function(at) environment.MachineAttributes.
        Any(Function(a) a.AttributeTypeID = at.ID)).
    ToList()

        Dim selectedToolSetup As WE_ENG_V25_0.Core.Models.ToolSetup =
    AppData.ToolSetups.FirstOrDefault(
        Function(ts) ts.ID = job.ToolSetupID)

        If selectedToolSetup Is Nothing Then
            Throw New InvalidDataException(
        "Selected Tool Setup was not found in configuration data.")
        End If

        If selectedToolSetup.MachineID <> job.MachineID Then
            Throw New InvalidDataException(
        "Selected Tool Setup does not belong to the job machine.")
        End If

        environment.ToolSetup = selectedToolSetup

        environment.ToolSetupMembers =
    AppData.ToolSetupMembers.
    Where(Function(m) m.ToolSetupID = selectedToolSetup.ID).
    ToList()

        Dim referencedToolIDs =
    environment.ToolSetupMembers.
    Select(Function(m) m.ToolID).
    Distinct().
    ToList()

        environment.Tools =
    AppData.ToolCrib.
    Where(Function(t) referencedToolIDs.Contains(t.ID)).
    ToList()

        If environment.Tools.Count <> referencedToolIDs.Count Then
            Throw New InvalidDataException(
        "One or more Tools referenced by the Tool Setup were not found.")
        End If

        Dim referencedStationIDs =
    environment.ToolSetupMembers.
    Select(Function(m) m.StationID).
    Distinct().
    ToList()

        environment.Stations =
    AppData.Stations.
    Where(Function(s) referencedStationIDs.Contains(s.ID)).
    ToList()

        If environment.Stations.Count <> referencedStationIDs.Count Then
            Throw New InvalidDataException(
        "One or more Stations referenced by the Tool Setup were not found.")
        End If


        Return environment

    End Function

    Public Shared Function BuildManufacturingModel(
    jobSheet As JobSheet
) As MM3File

        If jobSheet Is Nothing Then
            Throw New ArgumentNullException(NameOf(jobSheet))
        End If

        If jobSheet.ManufacturingModel Is Nothing Then
            Throw New InvalidDataException(
            "JobSheet Manufacturing Model is missing.")
        End If

        Dim mm3File As New MM3File With {
        .FormatVersion = 1,
        .MaterialSheetID = jobSheet.MaterialSheetID,
        .Length = jobSheet.Length,
        .Width = jobSheet.Width,
        .WorkZones = jobSheet.WorkZones
    }

        '==================================================
        ' COLLECT ALL GEOMETRY DEFINITIONS
        '==================================================

        Dim allGeometry As New List(Of GeoEntity)
        Dim geometryObjectsByID As New Dictionary(Of Integer, GeoEntity)

        'Sheet-level nominal geometry.
        For Each entity As GeoEntity In
        jobSheet.ManufacturingModel.PartGeometry

            RegisterGeometryForSave(
            entity,
            allGeometry,
            geometryObjectsByID)

        Next

        'Pattern-owned geometry.
        For Each pattern As Pattern In
        jobSheet.ManufacturingModel.Patterns

            If pattern Is Nothing Then
                Throw New InvalidOperationException(
                "Manufacturing Model contains a null Pattern.")
            End If

            For Each entity As GeoEntity In pattern.Geometry

                RegisterGeometryForSave(
                entity,
                allGeometry,
                geometryObjectsByID)

            Next

        Next

        'Part-owned geometry.
        For Each part As Part In jobSheet.Parts

            If part Is Nothing Then
                Throw New InvalidOperationException(
                "JobSheet contains a null Part.")
            End If

            For Each entity As GeoEntity In part.Geometry

                RegisterGeometryForSave(
                entity,
                allGeometry,
                geometryObjectsByID)

            Next

        Next

        'Preserve explicit sheet-level geometry membership.
        For Each entity As GeoEntity In
        jobSheet.ManufacturingModel.PartGeometry

            If entity Is Nothing Then
                Throw New InvalidOperationException(
                "Sheet-level PartGeometry contains a null entity.")
            End If

            mm3File.PartGeometryIDs.Add(entity.ID)

        Next

        'Preserve explicit sheet-level manufacturing membership.
        For Each feature As MachiningFeature In
        jobSheet.ManufacturingModel.ManufacturingGeometry

            If feature Is Nothing Then
                Throw New InvalidOperationException(
                "Sheet-level ManufacturingGeometry contains a null feature.")
            End If

            mm3File.ManufacturingGeometryIDs.Add(feature.ID)

        Next

        '==================================================
        ' COLLECT ALL MANUFACTURING FEATURES
        '==================================================

        Dim allFeatures As New List(Of MachiningFeature)
        Dim featureObjectsByID As New Dictionary(Of Integer, MachiningFeature)

        'Sheet-level manufacturing features.
        For Each feature As MachiningFeature In
        jobSheet.ManufacturingModel.ManufacturingGeometry

            RegisterFeatureForSave(
            feature,
            allFeatures,
            featureObjectsByID)

        Next

        'Pattern-owned manufacturing features.
        For Each pattern As Pattern In
        jobSheet.ManufacturingModel.Patterns

            For Each feature As MachiningFeature In pattern.Features

                RegisterFeatureForSave(
                feature,
                allFeatures,
                featureObjectsByID)

            Next

        Next

        '==================================================
        ' COLLECT GEOMETRY REFERENCED BY PROFILES
        '==================================================

        'Part GeoProfile geometry.
        For Each part As Part In jobSheet.Parts

            For Each profile As GeoProfile In part.GeoProfiles

                If profile Is Nothing Then
                    Throw New InvalidOperationException(
                    "Part contains a null GeoProfile.")
                End If

                For Each entity As GeoEntity In profile.Entities

                    RegisterGeometryForSave(
                    entity,
                    allGeometry,
                    geometryObjectsByID)

                Next

                For Each segment As GeoProfileSegment In profile.Segments

                    If segment Is Nothing Then
                        Throw New InvalidOperationException(
                        "GeoProfile contains a null segment.")
                    End If

                    RegisterGeometryForSave(
                    segment.Entity,
                    allGeometry,
                    geometryObjectsByID)

                Next

            Next

        Next

        'Pattern GeoProfile geometry.
        For Each pattern As Pattern In
        jobSheet.ManufacturingModel.Patterns

            For Each profile As GeoProfile In pattern.GeoProfiles

                If profile Is Nothing Then
                    Throw New InvalidOperationException(
                    "Pattern contains a null GeoProfile.")
                End If

                For Each entity As GeoEntity In profile.Entities

                    RegisterGeometryForSave(
                    entity,
                    allGeometry,
                    geometryObjectsByID)

                Next

                For Each segment As GeoProfileSegment In profile.Segments

                    If segment Is Nothing Then
                        Throw New InvalidOperationException(
                        "GeoProfile contains a null segment.")
                    End If

                    RegisterGeometryForSave(
                    segment.Entity,
                    allGeometry,
                    geometryObjectsByID)

                Next

            Next

        Next

        '==================================================
        ' SERIALIZE GEOMETRY DEFINITIONS
        '==================================================

        GeoEntityMapper.SaveGeometryCollections(
        allGeometry,
        mm3File)

        '==================================================
        ' COLLECT PART MANUFACTURING FEATURES
        '==================================================

        For Each part As Part In jobSheet.Parts

            For Each feature As MachiningFeature In part.Features

                RegisterFeatureForSave(
                feature,
                allFeatures,
                featureObjectsByID)

            Next

        Next

        '==================================================
        ' SERIALIZE MANUFACTURING FEATURES
        '==================================================

        For Each feature As MachiningFeature In allFeatures

            mm3File.ManufacturingFeatures.Add(
            MachiningFeatureMapper.SaveFeature(feature))

        Next

        '==================================================
        ' COLLECT AND SERIALIZE GEOPROFILES
        '==================================================

        Dim allProfiles As New List(Of GeoProfile)
        Dim profileObjectsByID As New Dictionary(Of Integer, GeoProfile)

        'Part GeoProfiles.
        For Each part As Part In jobSheet.Parts

            For Each profile As GeoProfile In part.GeoProfiles

                RegisterProfileForSave(
                profile,
                allProfiles,
                profileObjectsByID)

            Next

        Next

        'Pattern GeoProfiles.
        For Each pattern As Pattern In
        jobSheet.ManufacturingModel.Patterns

            For Each profile As GeoProfile In pattern.GeoProfiles

                RegisterProfileForSave(
                profile,
                allProfiles,
                profileObjectsByID)

            Next

        Next

        GeoProfileMapper.SaveProfiles(
        allProfiles,
        mm3File)

        '==================================================
        ' SERIALIZE PARTS
        '==================================================

        Dim partIDs As New HashSet(Of Integer)

        For Each part As Part In jobSheet.Parts

            If part Is Nothing Then
                Throw New InvalidOperationException(
                "JobSheet contains a null Part.")
            End If

            'Validate unique Part ID.
            If Not partIDs.Add(part.ID) Then
                Throw New InvalidOperationException(
                "Duplicate Part ID: " &
                part.ID.ToString())
            End If

            'Validate geometry references.
            For Each entity As GeoEntity In part.Geometry

                If entity Is Nothing Then
                    Throw New InvalidOperationException(
                    "Part ID " & part.ID.ToString() &
                    " contains a null geometry reference.")
                End If

                If Not geometryObjectsByID.ContainsKey(entity.ID) Then
                    Throw New InvalidOperationException(
                    "Part ID " & part.ID.ToString() &
                    " references missing GeoEntity ID " &
                    entity.ID.ToString())
                End If

            Next

            'Validate GeoProfile references.
            For Each profile As GeoProfile In part.GeoProfiles

                If profile Is Nothing Then
                    Throw New InvalidOperationException(
                    "Part ID " & part.ID.ToString() &
                    " contains a null GeoProfile reference.")
                End If

                If Not profileObjectsByID.ContainsKey(profile.ID) Then
                    Throw New InvalidOperationException(
                    "Part ID " & part.ID.ToString() &
                    " references missing GeoProfile ID " &
                    profile.ID.ToString())
                End If

            Next

            'Validate manufacturing feature references.
            For Each feature As MachiningFeature In part.Features

                If feature Is Nothing Then
                    Throw New InvalidOperationException(
                    "Part ID " & part.ID.ToString() &
                    " contains a null manufacturing feature.")
                End If

                If Not featureObjectsByID.ContainsKey(feature.ID) Then
                    Throw New InvalidOperationException(
                    "Part ID " & part.ID.ToString() &
                    " references missing manufacturing feature ID " &
                    feature.ID.ToString())
                End If

            Next

            mm3File.Parts.Add(
            PartMapper.ToRecord(part))

        Next

        '==================================================
        ' SERIALIZE PATTERN DEFINITIONS
        '==================================================

        Dim patternIDs As New HashSet(Of Integer)

        For Each pattern As Pattern In
        jobSheet.ManufacturingModel.Patterns

            If pattern Is Nothing Then
                Throw New InvalidOperationException(
                "Manufacturing Model contains a null Pattern.")
            End If

            'Validate unique Pattern ID.
            If Not patternIDs.Add(pattern.ID) Then
                Throw New InvalidOperationException(
                "Duplicate Pattern ID: " &
                pattern.ID.ToString())
            End If

            'Validate geometry references.
            For Each entity As GeoEntity In pattern.Geometry

                If entity Is Nothing Then
                    Throw New InvalidOperationException(
                    "Pattern ID " & pattern.ID.ToString() &
                    " contains a null geometry reference.")
                End If

                If Not geometryObjectsByID.ContainsKey(entity.ID) Then
                    Throw New InvalidOperationException(
                    "Pattern ID " & pattern.ID.ToString() &
                    " references missing GeoEntity ID " &
                    entity.ID.ToString())
                End If

            Next

            'Validate GeoProfile references.
            For Each profile As GeoProfile In pattern.GeoProfiles

                If profile Is Nothing Then
                    Throw New InvalidOperationException(
                    "Pattern ID " & pattern.ID.ToString() &
                    " contains a null GeoProfile reference.")
                End If

                If Not profileObjectsByID.ContainsKey(profile.ID) Then
                    Throw New InvalidOperationException(
                    "Pattern ID " & pattern.ID.ToString() &
                    " references missing GeoProfile ID " &
                    profile.ID.ToString())
                End If

            Next

            'Validate manufacturing feature references.
            For Each feature As MachiningFeature In pattern.Features

                If feature Is Nothing Then
                    Throw New InvalidOperationException(
                    "Pattern ID " & pattern.ID.ToString() &
                    " contains a null manufacturing feature.")
                End If

                If Not featureObjectsByID.ContainsKey(feature.ID) Then
                    Throw New InvalidOperationException(
                    "Pattern ID " & pattern.ID.ToString() &
                    " references missing manufacturing feature ID " &
                    feature.ID.ToString())
                End If

            Next

            mm3File.Patterns.Add(
            PatternMapper.ToRecord(pattern))

        Next

        '==================================================
        ' SERIALIZE PATTERN INSTANCES
        '==================================================

        Dim instanceIDs As New HashSet(Of Integer)

        For Each instance As PatternInstance In
        jobSheet.ManufacturingModel.PatternInstances

            If instance Is Nothing Then
                Throw New InvalidOperationException(
                "Manufacturing Model contains a null Pattern instance.")
            End If

            'Validate unique Instance ID.
            If Not instanceIDs.Add(instance.ID) Then
                Throw New InvalidOperationException(
                "Duplicate PatternInstance ID: " &
                instance.ID.ToString())
            End If

            'Validate referenced Pattern definition.
            If Not patternIDs.Contains(instance.PatternID) Then
                Throw New InvalidOperationException(
                "PatternInstance ID " &
                instance.ID.ToString() &
                " references missing Pattern ID " &
                instance.PatternID.ToString())
            End If

            mm3File.PatternInstances.Add(
            New PatternInstanceRecord With {
                .ID = instance.ID,
                .PatternID = instance.PatternID,
                .X = instance.X,
                .Y = instance.Y,
                .Angle = instance.Angle
            })

        Next



        'Collect existing WorkZone indexes.
        Dim workZoneIndexes As New HashSet(Of Integer)

        For Each zone As WorkZone In jobSheet.WorkZones

            If zone Is Nothing Then
                Throw New InvalidOperationException(
            "JobSheet contains a null WorkZone.")
            End If

            If Not workZoneIndexes.Add(zone.Index) Then
                Throw New InvalidOperationException(
            "Duplicate WorkZone Index: " &
            zone.Index.ToString())
            End If

        Next

        '==================================================
        ' SERIALIZE WORKZONE ASSIGNMENTS
        '==================================================

        'Serialize existing WorkZone assignments.
        For Each assignment As MM3WorkZoneAssignment In
    jobSheet.ManufacturingModel.WorkZoneAssignments

            Dim record As WorkZoneAssignmentRecord =
        WorkZoneAssignmentMapper.ToRecord(assignment)


            If Not workZoneIndexes.Contains(record.WorkZoneIndex) Then
                Throw New InvalidOperationException(
        "WorkZone assignment references missing WorkZone Index " &
        record.WorkZoneIndex.ToString())
            End If

            Select Case record.EntityType

                Case "GeoEntity"

                    If Not geometryObjectsByID.ContainsKey(record.EntityID) Then
                        Throw New InvalidOperationException(
                    "WorkZone assignment references missing GeoEntity ID " &
                    record.EntityID.ToString())
                    End If

                Case "MachiningFeature"

                    If Not featureObjectsByID.ContainsKey(record.EntityID) Then
                        Throw New InvalidOperationException(
                    "WorkZone assignment references missing MachiningFeature ID " &
                    record.EntityID.ToString())
                    End If

                Case Else

                    Throw New InvalidOperationException(
                "Unsupported WorkZone assignment entity type: " &
                record.EntityType)

            End Select

            mm3File.WorkZoneAssignments.Add(record)

        Next


        Return mm3File

    End Function



    Private Shared Sub RegisterGeometryForSave(
    entity As GeoEntity,
    allGeometry As List(Of GeoEntity),
    geometryObjectsByID As Dictionary(Of Integer, GeoEntity)
)

        If entity Is Nothing Then
            Throw New InvalidOperationException(
            "Cannot save a null geometry entity.")
        End If

        Dim existing As GeoEntity = Nothing

        If geometryObjectsByID.TryGetValue(entity.ID, existing) Then

            If Not Object.ReferenceEquals(existing, entity) Then
                Throw New InvalidOperationException(
                "Conflicting GeoEntity objects share ID " &
                entity.ID.ToString())
            End If

            'Same object referenced more than once.
            Return

        End If

        geometryObjectsByID.Add(entity.ID, entity)
        allGeometry.Add(entity)

    End Sub



    Private Shared Sub RegisterFeatureForSave(
    feature As MachiningFeature,
    allFeatures As List(Of MachiningFeature),
    featureObjectsByID As Dictionary(Of Integer, MachiningFeature)
)

        If feature Is Nothing Then
            Throw New InvalidOperationException(
            "Cannot save a null manufacturing feature.")
        End If

        Dim existing As MachiningFeature = Nothing

        If featureObjectsByID.TryGetValue(feature.ID, existing) Then

            If Not Object.ReferenceEquals(existing, feature) Then
                Throw New InvalidOperationException(
                "Conflicting manufacturing feature objects share ID " &
                feature.ID.ToString())
            End If

            'Same feature referenced by multiple collections.
            Return

        End If

        featureObjectsByID.Add(feature.ID, feature)
        allFeatures.Add(feature)

    End Sub


    Private Shared Sub RegisterProfileForSave(
    profile As GeoProfile,
    allProfiles As List(Of GeoProfile),
    profileObjectsByID As Dictionary(Of Integer, GeoProfile)
)

        If profile Is Nothing Then
            Throw New InvalidOperationException(
            "Cannot save a null GeoProfile.")
        End If

        Dim existing As GeoProfile = Nothing

        If profileObjectsByID.TryGetValue(profile.ID, existing) Then

            If Not Object.ReferenceEquals(existing, profile) Then
                Throw New InvalidOperationException(
                "Conflicting GeoProfile objects share ID " &
                profile.ID.ToString())
            End If

            'Same profile referenced by multiple collections.
            Return

        End If

        profileObjectsByID.Add(profile.ID, profile)
        allProfiles.Add(profile)

    End Sub



    Public Shared Sub WriteNativeFile(
    filePath As String,
    nativeFile As MM3NativeFile)

        If String.IsNullOrWhiteSpace(filePath) Then
            Throw New ArgumentException(
            "MM3 file path is missing.",
            NameOf(filePath))
        End If

        If nativeFile Is Nothing Then
            Throw New ArgumentNullException(NameOf(nativeFile))
        End If

        If nativeFile.ManufacturingModel Is Nothing Then
            Throw New InvalidDataException(
            "MM3 Manufacturing Model section is missing.")
        End If

        Dim json As String =
        Newtonsoft.Json.JsonConvert.SerializeObject(
            nativeFile,
            Newtonsoft.Json.Formatting.Indented)

        System.IO.File.WriteAllText(
        filePath,
        json,
        System.Text.Encoding.UTF8)

    End Sub

End Class
