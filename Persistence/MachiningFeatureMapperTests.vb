Public Class MachiningFeatureMapperTests

    Public Shared Sub TestHoleFeatureRoundTrip()

        Dim original As New HoleFeature With {
    .ID = 101,
    .FeatureType = MachiningFeatureType.Hole,
    .Description = "Round Trip Test Hole",
    .ToolSetupMemberID = 1534,
    .X = 12.5,
    .Y = 8.75,
    .Angle = 45.0,
    .SourceGeometryID = 200,
    .Parameters = New HolePatternParameters With {
        .XMirror = True,
        .YMirror = False,
        .Centered = True,
        .RepeatDirection = HoleRepeatDirection.Y,
        .RepeatCount = 3,
        .Gap = 2.5,
        .Orientation = 90
    }
}

        Dim record As MachiningFeatureRecord =
    MachiningFeatureMapper.ToRecord(original)
        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool)

        Dim restored As HoleFeature =
            MachiningFeatureMapper.FromRecord(
                restoredRecord,
                savedTools)

        If restored.ID <> original.ID Then
            Throw New InvalidOperationException(
                "HoleFeature round-trip failed: ID mismatch.")
        End If


        If restored.Tool IsNot Nothing Then

            Throw New InvalidOperationException(
        "HoleFeature round-trip failed: " &
        "Null tool reference was not preserved.")

        End If

        If restored.Parameters Is Nothing Then

            Throw New InvalidOperationException(
        "HoleFeature round-trip failed: Parameters are missing.")

        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
   restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
   restored.Parameters.Centered <> original.Parameters.Centered Then

            Throw New InvalidOperationException(
        "HoleFeature round-trip failed: Mirror/Centered mismatch.")

        End If

        If restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
   restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
   restored.Parameters.Gap <> original.Parameters.Gap OrElse
   restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
        "HoleFeature round-trip failed: Repeat/Orientation mismatch.")

        End If

        If restored.X <> original.X OrElse
   restored.Y <> original.Y OrElse
   restored.Angle <> original.Angle Then

            Throw New InvalidOperationException(
        "HoleFeature round-trip failed: Coordinate mismatch.")

        End If


        If restored.SourceGeometryID <> original.SourceGeometryID OrElse
   restored.ToolSetupMemberID <> original.ToolSetupMemberID Then

            Throw New InvalidOperationException(
        "HoleFeature round-trip failed: Reference ID mismatch.")

        End If

        System.Diagnostics.Debug.WriteLine(
    "HoleFeature JSON round-trip test PASSED.")



    End Sub


    Public Shared Sub TestHoleFeatureWithAssignedTool()

        Dim savedTool As New WE_ENG_V25_0.Core.Models.Tool With {
    .ID = 167,
    .TypeID = 16,
    .Description = "Round Trip Test Tool"
}

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool) From {
            savedTool
        }


        Dim original As New HoleFeature With {
    .ID = 102,
    .FeatureType = MachiningFeatureType.Hole,
    .Description = "Assigned Tool Round Trip Test",
    .ToolSetupMemberID = 1534,
    .X = 15.0,
    .Y = 10.0,
    .Angle = 0.0,
    .SourceGeometryID = 201,
    .Tool = savedTool
}

        Dim record As MachiningFeatureRecord =
    MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As HoleFeature =
            MachiningFeatureMapper.FromRecord(
                restoredRecord,
                savedTools)

        If restored.Tool Is Nothing Then

            Throw New InvalidOperationException(
                "Assigned tool round-trip failed: Tool is missing.")

        End If

        If Not Object.ReferenceEquals(restored.Tool, savedTool) Then

            Throw New InvalidOperationException(
                "Assigned tool round-trip failed: " &
                "Incorrect saved tool reference.")

        End If

        If restored.ToolSetupMemberID <> original.ToolSetupMemberID Then

            Throw New InvalidOperationException(
                "Assigned tool round-trip failed: " &
                "ToolSetupMemberID mismatch.")

        End If
        System.Diagnostics.Debug.WriteLine(
    "HoleFeature assigned-tool round-trip test PASSED.")

        If restored.Tool.ID <> original.Tool.ID Then

            Throw New InvalidOperationException(
                "Assigned tool round-trip failed: ToolID mismatch.")

        End If
    End Sub


    Public Shared Sub TestLineOfHolesRoundTrip()
        Dim original As New LineOfHolesFeature With {
    .ID = 103,
    .FeatureType = MachiningFeatureType.LineOfHoles,
    .Description = "Line of Holes Round Trip Test",
    .ToolSetupMemberID = 1534,
    .StartX = 10.0,
    .StartY = 20.0,
    .Count = 8,
    .Spacing = 1.25,
    .Direction = HoleRepeatDirection.X,
    .Parameters = New HolePatternParameters With {
        .XMirror = True,
        .YMirror = False,
        .Centered = True,
        .RepeatDirection = HoleRepeatDirection.Y,
        .RepeatCount = 2,
        .Gap = 5.0,
        .Orientation = 90
    }
}






        Dim record As MachiningFeatureRecord =
    MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool)

        Dim restored As LineOfHolesFeature =
            MachiningFeatureMapper.FromLineOfHolesRecord(
                restoredRecord,
                savedTools)

        If restored.Tool IsNot Nothing Then

            Throw New InvalidOperationException(
        "LineOfHolesFeature round-trip failed: " &
        "Unassigned tool was not preserved.")

        End If

        If restored.ID <> original.ID OrElse
   restored.FeatureType <> original.FeatureType OrElse
   restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
   restored.StartX <> original.StartX OrElse
   restored.StartY <> original.StartY OrElse
   restored.Count <> original.Count OrElse
   restored.Spacing <> original.Spacing OrElse
   restored.Direction <> original.Direction Then

            Throw New InvalidOperationException(
                "LineOfHolesFeature round-trip failed: " &
                "Arrangement property mismatch.")

        End If

        If restored.Parameters Is Nothing Then

            Throw New InvalidOperationException(
                "LineOfHolesFeature round-trip failed: Parameters missing.")

        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
           restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
           restored.Parameters.Centered <> original.Parameters.Centered OrElse
           restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
           restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
           restored.Parameters.Gap <> original.Parameters.Gap OrElse
           restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
                "LineOfHolesFeature round-trip failed: " &
                "Pattern parameter mismatch.")

        End If

        System.Diagnostics.Debug.WriteLine(
    "LineOfHolesFeature JSON round-trip test PASSED.")

    End Sub
    Public Shared Sub TestLineOfHolesWithAssignedTool()

        ' Create the saved tool independently of Configuration Manager.
        Dim savedTool As New WE_ENG_V25_0.Core.Models.Tool With {
        .ID = 167,
        .TypeID = 16,
        .Description = "1.0 Square 0.0 rad 0.0 deg."
    }

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool) From {
        savedTool
    }

        ' Create the original manufacturing arrangement.
        Dim original As New LineOfHolesFeature With {
        .ID = 104,
        .FeatureType = MachiningFeatureType.LineOfHoles,
        .Description = "Line of Holes Assigned Tool Test",
        .ToolSetupMemberID = 1534,
        .StartX = 15.0,
        .StartY = 25.0,
        .Count = 12,
        .Spacing = 1.5,
        .Direction = HoleRepeatDirection.X,
        .Tool = savedTool,
        .Parameters = New HolePatternParameters With {
            .XMirror = True,
            .YMirror = False,
            .Centered = True,
            .RepeatDirection = HoleRepeatDirection.Y,
            .RepeatCount = 3,
            .Gap = 6.0,
            .Orientation = 90
        }
    }

        ' Convert the feature to its persistence record.
        Dim record As MachiningFeatureRecord =
        MachiningFeatureMapper.ToRecord(original)

        ' Serialize and deserialize through JSON.
        Dim json As String =
        Newtonsoft.Json.JsonConvert.SerializeObject(
            record,
            Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
        Newtonsoft.Json.JsonConvert.DeserializeObject(
            Of MachiningFeatureRecord)(json)

        ' Restore using ONLY the saved tool collection.
        Dim restored As LineOfHolesFeature =
        MachiningFeatureMapper.FromLineOfHolesRecord(
            restoredRecord,
            savedTools)

        ' Verify the arrangement properties.
        If restored.ID <> original.ID OrElse
       restored.FeatureType <> original.FeatureType OrElse
       restored.Description <> original.Description OrElse
       restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
       restored.StartX <> original.StartX OrElse
       restored.StartY <> original.StartY OrElse
       restored.Count <> original.Count OrElse
       restored.Spacing <> original.Spacing OrElse
       restored.Direction <> original.Direction Then

            Throw New InvalidOperationException(
            "LineOfHolesFeature assigned-tool test failed: " &
            "Arrangement property mismatch.")

        End If

        ' Verify the pattern parameters.
        If restored.Parameters Is Nothing Then

            Throw New InvalidOperationException(
            "LineOfHolesFeature assigned-tool test failed: " &
            "Parameters missing.")

        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
       restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
       restored.Parameters.Centered <> original.Parameters.Centered OrElse
       restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
       restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
       restored.Parameters.Gap <> original.Parameters.Gap OrElse
       restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
            "LineOfHolesFeature assigned-tool test failed: " &
            "Pattern parameter mismatch.")

        End If

        ' Verify that the exact saved tool was restored.
        If restored.Tool Is Nothing Then

            Throw New InvalidOperationException(
            "LineOfHolesFeature assigned-tool test failed: " &
            "Tool was not restored.")

        End If

        If restored.Tool.ID <> savedTool.ID Then

            Throw New InvalidOperationException(
            "LineOfHolesFeature assigned-tool test failed: " &
            "Tool ID mismatch.")

        End If

        If Not Object.ReferenceEquals(restored.Tool, savedTool) Then

            Throw New InvalidOperationException(
            "LineOfHolesFeature assigned-tool test failed: " &
            "Tool was not resolved from the saved collection.")

        End If

        System.Diagnostics.Debug.WriteLine(
        "LineOfHolesFeature assigned-tool round-trip test PASSED.")

    End Sub


    Public Shared Sub TestGridOfHolesRoundTrip()

        Dim original As New GridOfHolesFeature With {
            .ID = 105,
            .FeatureType = MachiningFeatureType.GridOfHoles,
            .Description = "Grid of Holes Round Trip Test",
            .ToolSetupMemberID = 1534,
            .StartX = 10.0,
            .StartY = 20.0,
            .XCount = 5,
            .YCount = 4,
            .XSpacing = 1.25,
            .YSpacing = 2.5,
            .PrimaryAxis = HoleRepeatDirection.X,
            .Tool = Nothing,
            .Parameters = New HolePatternParameters With {
                .XMirror = True,
                .YMirror = False,
                .Centered = True,
                .RepeatDirection = HoleRepeatDirection.Y,
                .RepeatCount = 3,
                .Gap = 6.0,
                .Orientation = 90
            }
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool)

        Dim restored As GridOfHolesFeature =
            MachiningFeatureMapper.FromGridOfHolesRecord(
                restoredRecord,
                savedTools)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.StartX <> original.StartX OrElse
           restored.StartY <> original.StartY OrElse
           restored.XCount <> original.XCount OrElse
           restored.YCount <> original.YCount OrElse
           restored.XSpacing <> original.XSpacing OrElse
           restored.YSpacing <> original.YSpacing OrElse
           restored.PrimaryAxis <> original.PrimaryAxis Then

            Throw New InvalidOperationException(
                "GridOfHolesFeature round-trip failed: " &
                "Grid property mismatch.")

        End If

        If restored.Parameters Is Nothing Then
            Throw New InvalidOperationException(
                "GridOfHolesFeature round-trip failed: Parameters missing.")
        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
           restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
           restored.Parameters.Centered <> original.Parameters.Centered OrElse
           restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
           restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
           restored.Parameters.Gap <> original.Parameters.Gap OrElse
           restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
                "GridOfHolesFeature round-trip failed: " &
                "Pattern parameter mismatch.")

        End If

        If restored.Tool IsNot Nothing Then
            Throw New InvalidOperationException(
                "GridOfHolesFeature round-trip failed: " &
                "Unassigned tool was not preserved.")
        End If

        System.Diagnostics.Debug.WriteLine(
            "GridOfHolesFeature JSON round-trip test PASSED.")

    End Sub


    Public Shared Sub TestGridOfHolesWithAssignedTool()

        Dim savedTool As New WE_ENG_V25_0.Core.Models.Tool With {
            .ID = 167,
            .TypeID = 16,
            .Description = "1.0 Square 0.0 rad 0.0 deg."
        }

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool) From {
            savedTool
        }

        Dim original As New GridOfHolesFeature With {
            .ID = 106,
            .FeatureType = MachiningFeatureType.GridOfHoles,
            .Description = "Grid of Holes Assigned Tool Test",
            .ToolSetupMemberID = 1534,
            .StartX = 15.0,
            .StartY = 25.0,
            .XCount = 8,
            .YCount = 6,
            .XSpacing = 1.5,
            .YSpacing = 2.0,
            .PrimaryAxis = HoleRepeatDirection.Y,
            .Tool = savedTool,
            .Parameters = New HolePatternParameters With {
                .XMirror = False,
                .YMirror = True,
                .Centered = False,
                .RepeatDirection = HoleRepeatDirection.X,
                .RepeatCount = 2,
                .Gap = 5.0,
                .Orientation = 180
            }
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As GridOfHolesFeature =
            MachiningFeatureMapper.FromGridOfHolesRecord(
                restoredRecord,
                savedTools)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.StartX <> original.StartX OrElse
           restored.StartY <> original.StartY OrElse
           restored.XCount <> original.XCount OrElse
           restored.YCount <> original.YCount OrElse
           restored.XSpacing <> original.XSpacing OrElse
           restored.YSpacing <> original.YSpacing OrElse
           restored.PrimaryAxis <> original.PrimaryAxis Then

            Throw New InvalidOperationException(
                "GridOfHolesFeature assigned-tool test failed: " &
                "Grid property mismatch.")

        End If

        If restored.Parameters Is Nothing Then
            Throw New InvalidOperationException(
                "GridOfHolesFeature assigned-tool test failed: " &
                "Parameters missing.")
        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
           restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
           restored.Parameters.Centered <> original.Parameters.Centered OrElse
           restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
           restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
           restored.Parameters.Gap <> original.Parameters.Gap OrElse
           restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
                "GridOfHolesFeature assigned-tool test failed: " &
                "Pattern parameter mismatch.")

        End If

        If restored.Tool Is Nothing Then
            Throw New InvalidOperationException(
                "GridOfHolesFeature assigned-tool test failed: " &
                "Tool was not restored.")
        End If

        If restored.Tool.ID <> savedTool.ID Then
            Throw New InvalidOperationException(
                "GridOfHolesFeature assigned-tool test failed: " &
                "Tool ID mismatch.")
        End If

        If Not Object.ReferenceEquals(restored.Tool, savedTool) Then
            Throw New InvalidOperationException(
                "GridOfHolesFeature assigned-tool test failed: " &
                "Tool was not resolved from saved collection.")
        End If

        System.Diagnostics.Debug.WriteLine(
            "GridOfHolesFeature assigned-tool round-trip test PASSED.")

    End Sub


    Public Shared Sub TestBoltHoleCircleRoundTrip()

        Dim original As New BoltHoleCircleFeature With {
            .ID = 107,
            .FeatureType = MachiningFeatureType.BoltHoleCircle,
            .Description = "Bolt Hole Circle Round Trip Test",
            .ToolSetupMemberID = 1534,
            .CenterX = 25.0,
            .CenterY = 35.0,
            .Radius = 12.5,
            .Count = 8,
            .StartAngle = 22.5,
            .IncrementAngle = 45.0,
            .AutoIndex = False,
            .Tool = Nothing,
            .Parameters = New HolePatternParameters With {
                .XMirror = True,
                .YMirror = False,
                .Centered = True,
                .RepeatDirection = HoleRepeatDirection.Y,
                .RepeatCount = 2,
                .Gap = 5.0,
                .Orientation = 90
            }
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool)

        Dim restored As BoltHoleCircleFeature =
            MachiningFeatureMapper.FromBoltHoleCircleRecord(
                restoredRecord,
                savedTools)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.CenterX <> original.CenterX OrElse
           restored.CenterY <> original.CenterY OrElse
           restored.Radius <> original.Radius OrElse
           restored.Count <> original.Count OrElse
           restored.StartAngle <> original.StartAngle OrElse
           restored.IncrementAngle <> original.IncrementAngle OrElse
           restored.AutoIndex <> original.AutoIndex Then

            Throw New InvalidOperationException(
                "BoltHoleCircleFeature round-trip failed: " &
                "Feature property mismatch.")

        End If

        If restored.Parameters Is Nothing Then
            Throw New InvalidOperationException(
                "BoltHoleCircleFeature round-trip failed: Parameters missing.")
        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
           restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
           restored.Parameters.Centered <> original.Parameters.Centered OrElse
           restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
           restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
           restored.Parameters.Gap <> original.Parameters.Gap OrElse
           restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
                "BoltHoleCircleFeature round-trip failed: " &
                "Pattern parameter mismatch.")

        End If

        If restored.Tool IsNot Nothing Then
            Throw New InvalidOperationException(
                "BoltHoleCircleFeature round-trip failed: " &
                "Unassigned tool was not preserved.")
        End If

        System.Diagnostics.Debug.WriteLine(
            "BoltHoleCircleFeature JSON round-trip test PASSED.")

    End Sub


    Public Shared Sub TestBoltHoleCircleWithAssignedTool()

        Dim savedTool As New WE_ENG_V25_0.Core.Models.Tool With {
            .ID = 167,
            .TypeID = 16,
            .Description = "1.0 Square 0.0 rad 0.0 deg."
        }

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool) From {
            savedTool
        }

        Dim original As New BoltHoleCircleFeature With {
            .ID = 108,
            .FeatureType = MachiningFeatureType.BoltHoleCircle,
            .Description = "Bolt Hole Circle Assigned Tool Test",
            .ToolSetupMemberID = 1534,
            .CenterX = 40.0,
            .CenterY = 50.0,
            .Radius = 18.75,
            .Count = 12,
            .StartAngle = 15.0,
            .IncrementAngle = 30.0,
            .AutoIndex = True,
            .Tool = savedTool,
            .Parameters = New HolePatternParameters With {
                .XMirror = False,
                .YMirror = True,
                .Centered = False,
                .RepeatDirection = HoleRepeatDirection.X,
                .RepeatCount = 3,
                .Gap = 7.5,
                .Orientation = 180
            }
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As BoltHoleCircleFeature =
            MachiningFeatureMapper.FromBoltHoleCircleRecord(
                restoredRecord,
                savedTools)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.CenterX <> original.CenterX OrElse
           restored.CenterY <> original.CenterY OrElse
           restored.Radius <> original.Radius OrElse
           restored.Count <> original.Count OrElse
           restored.StartAngle <> original.StartAngle OrElse
           restored.IncrementAngle <> original.IncrementAngle OrElse
           restored.AutoIndex <> original.AutoIndex Then

            Throw New InvalidOperationException(
                "BoltHoleCircleFeature assigned-tool test failed: " &
                "Feature property mismatch.")

        End If

        If restored.Parameters Is Nothing Then
            Throw New InvalidOperationException(
                "BoltHoleCircleFeature assigned-tool test failed: " &
                "Parameters missing.")
        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
           restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
           restored.Parameters.Centered <> original.Parameters.Centered OrElse
           restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
           restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
           restored.Parameters.Gap <> original.Parameters.Gap OrElse
           restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
                "BoltHoleCircleFeature assigned-tool test failed: " &
                "Pattern parameter mismatch.")

        End If

        If restored.Tool Is Nothing Then
            Throw New InvalidOperationException(
                "BoltHoleCircleFeature assigned-tool test failed: " &
                "Tool was not restored.")
        End If

        If restored.Tool.ID <> savedTool.ID Then
            Throw New InvalidOperationException(
                "BoltHoleCircleFeature assigned-tool test failed: " &
                "Tool ID mismatch.")
        End If

        If Not Object.ReferenceEquals(restored.Tool, savedTool) Then
            Throw New InvalidOperationException(
                "BoltHoleCircleFeature assigned-tool test failed: " &
                "Tool was not resolved from saved collection.")
        End If

        System.Diagnostics.Debug.WriteLine(
            "BoltHoleCircleFeature assigned-tool round-trip test PASSED.")

    End Sub

    Public Shared Sub TestBarrierHoleRoundTrip()

        Dim original As New BarrierHoleFeature With {
            .ID = 109,
            .FeatureType = MachiningFeatureType.BarrierHole,
            .Description = "Barrier Hole Round Trip Test",
            .ToolSetupMemberID = 1534,
            .StartX = 10.0,
            .StartY = 20.0,
            .EndX = 80.0,
            .EndY = 45.0,
            .Spacing = 1.25,
            .ToolIndexAngle = 0.0,
            .Tool = Nothing,
            .Parameters = New HolePatternParameters With {
                .XMirror = True,
                .YMirror = False,
                .Centered = True,
                .RepeatDirection = HoleRepeatDirection.Y,
                .RepeatCount = 2,
                .Gap = 5.0,
                .Orientation = 90
            }
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool)

        Dim restored As BarrierHoleFeature =
            MachiningFeatureMapper.FromBarrierHoleRecord(
                restoredRecord,
                savedTools)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.StartX <> original.StartX OrElse
           restored.StartY <> original.StartY OrElse
           restored.EndX <> original.EndX OrElse
           restored.EndY <> original.EndY OrElse
           restored.Spacing <> original.Spacing OrElse
           restored.ToolIndexAngle <> original.ToolIndexAngle Then

            Throw New InvalidOperationException(
                "BarrierHoleFeature round-trip failed: " &
                "Feature property mismatch.")

        End If

        If restored.Parameters Is Nothing Then
            Throw New InvalidOperationException(
                "BarrierHoleFeature round-trip failed: Parameters missing.")
        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
           restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
           restored.Parameters.Centered <> original.Parameters.Centered OrElse
           restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
           restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
           restored.Parameters.Gap <> original.Parameters.Gap OrElse
           restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
                "BarrierHoleFeature round-trip failed: " &
                "Pattern parameter mismatch.")

        End If

        If restored.Tool IsNot Nothing Then
            Throw New InvalidOperationException(
                "BarrierHoleFeature round-trip failed: " &
                "Unassigned tool was not preserved.")
        End If

        System.Diagnostics.Debug.WriteLine(
            "BarrierHoleFeature JSON round-trip test PASSED.")

    End Sub


    Public Shared Sub TestBarrierHoleWithAssignedTool()

        Dim savedTool As New WE_ENG_V25_0.Core.Models.Tool With {
            .ID = 167,
            .TypeID = 16,
            .Description = "1.0 Square 0.0 rad 0.0 deg."
        }

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool) From {
            savedTool
        }

        Dim original As New BarrierHoleFeature With {
            .ID = 110,
            .FeatureType = MachiningFeatureType.BarrierHole,
            .Description = "Barrier Hole Assigned Tool Test",
            .ToolSetupMemberID = 1534,
            .StartX = 15.0,
            .StartY = 25.0,
            .EndX = 95.0,
            .EndY = 65.0,
            .Spacing = 2.5,
            .ToolIndexAngle = 37.5,
            .Tool = savedTool,
            .Parameters = New HolePatternParameters With {
                .XMirror = False,
                .YMirror = True,
                .Centered = False,
                .RepeatDirection = HoleRepeatDirection.X,
                .RepeatCount = 3,
                .Gap = 7.5,
                .Orientation = 180
            }
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As BarrierHoleFeature =
            MachiningFeatureMapper.FromBarrierHoleRecord(
                restoredRecord,
                savedTools)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.StartX <> original.StartX OrElse
           restored.StartY <> original.StartY OrElse
           restored.EndX <> original.EndX OrElse
           restored.EndY <> original.EndY OrElse
           restored.Spacing <> original.Spacing OrElse
           restored.ToolIndexAngle <> original.ToolIndexAngle Then

            Throw New InvalidOperationException(
                "BarrierHoleFeature assigned-tool test failed: " &
                "Feature property mismatch.")

        End If

        If restored.Parameters Is Nothing Then
            Throw New InvalidOperationException(
                "BarrierHoleFeature assigned-tool test failed: " &
                "Parameters missing.")
        End If

        If restored.Parameters.XMirror <> original.Parameters.XMirror OrElse
           restored.Parameters.YMirror <> original.Parameters.YMirror OrElse
           restored.Parameters.Centered <> original.Parameters.Centered OrElse
           restored.Parameters.RepeatDirection <> original.Parameters.RepeatDirection OrElse
           restored.Parameters.RepeatCount <> original.Parameters.RepeatCount OrElse
           restored.Parameters.Gap <> original.Parameters.Gap OrElse
           restored.Parameters.Orientation <> original.Parameters.Orientation Then

            Throw New InvalidOperationException(
                "BarrierHoleFeature assigned-tool test failed: " &
                "Pattern parameter mismatch.")

        End If

        If restored.Tool Is Nothing Then
            Throw New InvalidOperationException(
                "BarrierHoleFeature assigned-tool test failed: " &
                "Tool was not restored.")
        End If

        If restored.Tool.ID <> savedTool.ID Then
            Throw New InvalidOperationException(
                "BarrierHoleFeature assigned-tool test failed: " &
                "Tool ID mismatch.")
        End If

        If Not Object.ReferenceEquals(restored.Tool, savedTool) Then
            Throw New InvalidOperationException(
                "BarrierHoleFeature assigned-tool test failed: " &
                "Tool was not resolved from saved collection.")
        End If

        System.Diagnostics.Debug.WriteLine(
            "BarrierHoleFeature assigned-tool round-trip test PASSED.")

    End Sub


    Public Shared Sub TestCommandFeatureRoundTrip()

        Dim original As New CommandFeature With {
            .ID = 111,
            .FeatureType = MachiningFeatureType.Command,
            .Description = "Command Feature Round Trip Test",
            .ToolSetupMemberID = 0,
            .X = 12.5,
            .Y = 27.75,
            .Angle = 45.0,
            .Position = CommandTextPosition.Above,
            .FontSize = 2.5,
            .CommandText = "M08" & vbCrLf & "G04 P1.5"
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As CommandFeature =
            MachiningFeatureMapper.FromCommandFeatureRecord(
                restoredRecord)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.X <> original.X OrElse
           restored.Y <> original.Y OrElse
           restored.Angle <> original.Angle OrElse
           restored.Position <> original.Position OrElse
           restored.FontSize <> original.FontSize OrElse
           Not String.Equals(
               restored.CommandText,
               original.CommandText,
               StringComparison.Ordinal) Then

            Throw New InvalidOperationException(
                "CommandFeature round-trip failed: " &
                "Feature property mismatch.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "CommandFeature JSON round-trip test PASSED.")

    End Sub


    Public Shared Sub TestCommandFeatureWithNullText()

        Dim original As New CommandFeature With {
            .ID = 112,
            .FeatureType = MachiningFeatureType.Command,
            .Description = "Command Feature Null Text Test",
            .ToolSetupMemberID = 0,
            .X = 40.0,
            .Y = 60.0,
            .Angle = 270.0,
            .Position = CommandTextPosition.Below,
            .FontSize = 3.75,
            .CommandText = Nothing
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As CommandFeature =
            MachiningFeatureMapper.FromCommandFeatureRecord(
                restoredRecord)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.X <> original.X OrElse
           restored.Y <> original.Y OrElse
           restored.Angle <> original.Angle OrElse
           restored.Position <> original.Position OrElse
           restored.FontSize <> original.FontSize OrElse
           restored.CommandText IsNot Nothing Then

            Throw New InvalidOperationException(
                "CommandFeature null-text round-trip failed: " &
                "Feature property mismatch.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "CommandFeature null-text round-trip test PASSED.")

    End Sub

    Public Shared Sub TestContourFeatureRoundTrip()

        Dim path As New FabV25_WIN8.ToolShapes.Path2D()

        path.Add(New FabV25_WIN8.ToolShapes.LineSegment2D(
            New System.Drawing.PointF(10.0F, 20.0F),
            New System.Drawing.PointF(40.0F, 20.0F)))

        path.Add(New FabV25_WIN8.ToolShapes.ArcSegment2D(
            New System.Drawing.PointF(40.0F, 30.0F),
            10.0,
            -Math.PI / 2.0,
            0.0,
            False))

        path.Add(New FabV25_WIN8.ToolShapes.LineSegment2D(
            New System.Drawing.PointF(50.0F, 30.0F),
            New System.Drawing.PointF(10.0005F, 20.0F)))

        Dim original As New ContourFeature With {
            .ID = 113,
            .FeatureType = MachiningFeatureType.Profile,
            .Description = "Contour Round Trip Test",
            .ToolSetupMemberID = 1534,
            .Path = path,
            .ToolIndexAngle = Nothing,
            .IsLogicallyClosed = True
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As ContourFeature =
            MachiningFeatureMapper.FromContourFeatureRecord(
                restoredRecord)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.IsLogicallyClosed <> original.IsLogicallyClosed OrElse
           restored.ToolIndexAngle.HasValue Then

            Throw New InvalidOperationException(
                "ContourFeature round-trip failed: Feature property mismatch.")

        End If

        If restored.Path Is Nothing Then
            Throw New InvalidOperationException(
                "ContourFeature round-trip failed: Path missing.")
        End If

        Dim originalSegments =
            original.Path.Segments.ToList()

        Dim restoredSegments =
            restored.Path.Segments.ToList()

        If restoredSegments.Count <> originalSegments.Count Then
            Throw New InvalidOperationException(
                "ContourFeature round-trip failed: Segment count mismatch.")
        End If

        For i As Integer = 0 To originalSegments.Count - 1

            Dim before = originalSegments(i)
            Dim after = restoredSegments(i)

            If before.SegmentType <> after.SegmentType Then
                Throw New InvalidOperationException(
                    "ContourFeature round-trip failed: Segment type mismatch.")
            End If

            If TypeOf before Is FabV25_WIN8.ToolShapes.LineSegment2D Then

                Dim lineBefore =
                    DirectCast(before, FabV25_WIN8.ToolShapes.LineSegment2D)

                Dim lineAfter =
                    DirectCast(after, FabV25_WIN8.ToolShapes.LineSegment2D)

                If lineBefore.StartPoint <> lineAfter.StartPoint OrElse
                   lineBefore.EndPoint <> lineAfter.EndPoint Then

                    Throw New InvalidOperationException(
                        "ContourFeature round-trip failed: Line geometry mismatch.")
                End If

            ElseIf TypeOf before Is FabV25_WIN8.ToolShapes.ArcSegment2D Then

                Dim arcBefore =
                    DirectCast(before, FabV25_WIN8.ToolShapes.ArcSegment2D)

                Dim arcAfter =
                    DirectCast(after, FabV25_WIN8.ToolShapes.ArcSegment2D)

                If arcBefore.Center <> arcAfter.Center OrElse
                   arcBefore.Radius <> arcAfter.Radius OrElse
                   arcBefore.StartAngle <> arcAfter.StartAngle OrElse
                   arcBefore.EndAngle <> arcAfter.EndAngle OrElse
                   arcBefore.IsClockwise <> arcAfter.IsClockwise Then

                    Throw New InvalidOperationException(
                        "ContourFeature round-trip failed: Arc geometry mismatch.")
                End If

            Else

                Throw New InvalidOperationException(
                    "ContourFeature round-trip failed: Unsupported segment.")

            End If

        Next

        If restored.Path.IsClosed Then
            Throw New InvalidOperationException(
                "ContourFeature round-trip failed: " &
                "Geometric closure was unexpectedly changed.")
        End If

        System.Diagnostics.Debug.WriteLine(
            "ContourFeature JSON round-trip test PASSED.")

    End Sub

    Public Shared Sub TestContourFeatureWithEmptyPath()

        ' Create a valid but empty machining path.
        Dim emptyPath As New FabV25_WIN8.ToolShapes.Path2D()

        Dim original As New ContourFeature With {
            .ID = 114,
            .FeatureType = MachiningFeatureType.Profile,
            .Description = "Contour Empty Path Round Trip Test",
            .ToolSetupMemberID = 1534,
            .Path = emptyPath,
            .ToolIndexAngle = 37.5,
            .IsLogicallyClosed = False
        }

        ' Convert the feature to its persistence record.
        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        ' Serialize and deserialize through JSON.
        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        ' Restore the manufacturing feature.
        Dim restored As ContourFeature =
            MachiningFeatureMapper.FromContourFeatureRecord(
                restoredRecord)

        ' Verify the feature properties.
        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.IsLogicallyClosed <> original.IsLogicallyClosed Then

            Throw New InvalidOperationException(
                "ContourFeature empty-path test failed: " &
                "Feature property mismatch.")

        End If

        ' Verify the explicitly assigned tool index angle.
        If Not restored.ToolIndexAngle.HasValue Then

            Throw New InvalidOperationException(
                "ContourFeature empty-path test failed: " &
                "ToolIndexAngle was lost.")

        End If

        If restored.ToolIndexAngle.Value <>
           original.ToolIndexAngle.Value Then

            Throw New InvalidOperationException(
                "ContourFeature empty-path test failed: " &
                "ToolIndexAngle mismatch.")

        End If

        ' Verify that the empty path still exists.
        If restored.Path Is Nothing Then

            Throw New InvalidOperationException(
                "ContourFeature empty-path test failed: " &
                "Empty path was replaced with Nothing.")

        End If

        ' Verify that no segments were introduced.
        If restored.Path.Segments.Count() <> 0 Then

            Throw New InvalidOperationException(
                "ContourFeature empty-path test failed: " &
                "Unexpected path segments.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "ContourFeature empty-path round-trip test PASSED.")

    End Sub

    Public Shared Sub TestContourFeatureWithNullPath()

        Dim original As New ContourFeature With {
            .ID = 115,
            .FeatureType = MachiningFeatureType.Profile,
            .Description = "Contour Null Path Round Trip Test",
            .ToolSetupMemberID = 1534,
            .Path = Nothing,
            .ToolIndexAngle = Nothing,
            .IsLogicallyClosed = False
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As ContourFeature =
            MachiningFeatureMapper.FromContourFeatureRecord(
                restoredRecord)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.IsLogicallyClosed <> original.IsLogicallyClosed Then

            Throw New InvalidOperationException(
                "ContourFeature null-path test failed: " &
                "Feature property mismatch.")

        End If

        If restored.Path IsNot Nothing Then

            Throw New InvalidOperationException(
                "ContourFeature null-path test failed: " &
                "Nothing was replaced with a path.")

        End If

        If restored.ToolIndexAngle.HasValue Then

            Throw New InvalidOperationException(
                "ContourFeature null-path test failed: " &
                "Nullable tool index angle mismatch.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "ContourFeature null-path round-trip test PASSED.")

    End Sub

    Public Shared Sub TestLineOfHolesAtAngleNormalRoundTrip()

        Dim original As New LineOfHolesAtAngleNormalFeature With {
            .ID = 116,
            .FeatureType = MachiningFeatureType.LineOfHolesAtAngleNormal,
            .Description = "Angled Normal Row Round Trip Test",
            .ToolSetupMemberID = 1534,
            .StartX = 12.5,
            .StartY = 27.75,
            .Count = 8,
            .Spacing = 1.25,
            .Angle = 37.5,
            .Tool = Nothing,
            .Parameters = New HolePatternParameters()
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As LineOfHolesAtAngleNormalFeature =
            MachiningFeatureMapper.FromLineOfHolesAtAngleNormalRecord(
                restoredRecord,
                Nothing)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.StartX <> original.StartX OrElse
           restored.StartY <> original.StartY OrElse
           restored.Count <> original.Count OrElse
           restored.Spacing <> original.Spacing OrElse
           restored.Angle <> original.Angle Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature round-trip failed: " &
                "Feature property mismatch.")

        End If

        If restored.Tool IsNot Nothing Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature round-trip failed: " &
                "Unexpected tool assignment.")

        End If

        If restored.Parameters Is Nothing Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature round-trip failed: " &
                "Parameters were lost.")

        End If

        Dim originalParametersJson As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                original.Parameters)

        Dim restoredParametersJson As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                restored.Parameters)

        If Not Newtonsoft.Json.Linq.JToken.DeepEquals(
            Newtonsoft.Json.Linq.JToken.Parse(originalParametersJson),
            Newtonsoft.Json.Linq.JToken.Parse(restoredParametersJson)) Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature round-trip failed: " &
                "Hole pattern parameters mismatch.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "LineOfHolesAtAngleNormalFeature JSON round-trip test PASSED.")

    End Sub

    Public Shared Sub TestLineOfHolesAtAngleNormalWithAssignedTool()

        ' Create a saved tool independently of Configuration Manager.
        Dim savedTool As New WE_ENG_V25_0.Core.Models.Tool With {
            .ID = 167,
            .TypeID = 16,
            .Description = "1.0 Square 0.0 rad 0.0 deg."
        }

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool) From {
            savedTool
        }

        ' Create the original angled manufacturing arrangement.
        Dim original As New LineOfHolesAtAngleNormalFeature With {
            .ID = 117,
            .FeatureType = MachiningFeatureType.LineOfHolesAtAngleNormal,
            .Description = "Angled Normal Row Assigned Tool Test",
            .ToolSetupMemberID = 1534,
            .StartX = 15.0,
            .StartY = 25.0,
            .Count = 12,
            .Spacing = 1.5,
            .Angle = 37.5,
            .Tool = savedTool,
            .Parameters = New HolePatternParameters With {
                .XMirror = True,
                .YMirror = False,
                .Centered = True,
                .RepeatDirection = HoleRepeatDirection.Y,
                .RepeatCount = 3,
                .Gap = 6.0,
                .Orientation = 90
            }
        }

        ' Convert the feature to its persistence record.
        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        ' Serialize and deserialize through JSON.
        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        ' Restore using ONLY the saved tool collection.
        Dim restored As LineOfHolesAtAngleNormalFeature =
            MachiningFeatureMapper.FromLineOfHolesAtAngleNormalRecord(
                restoredRecord,
                savedTools)

        ' Verify the arrangement properties.
        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.StartX <> original.StartX OrElse
           restored.StartY <> original.StartY OrElse
           restored.Count <> original.Count OrElse
           restored.Spacing <> original.Spacing OrElse
           restored.Angle <> original.Angle Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature assigned-tool test failed: " &
                "Arrangement property mismatch.")

        End If

        ' Verify the complete pattern parameters.
        If restored.Parameters Is Nothing Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature assigned-tool test failed: " &
                "Parameters missing.")

        End If

        Dim originalParametersJson As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(original.Parameters)

        Dim restoredParametersJson As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(restored.Parameters)

        If Not Newtonsoft.Json.Linq.JToken.DeepEquals(
            Newtonsoft.Json.Linq.JToken.Parse(originalParametersJson),
            Newtonsoft.Json.Linq.JToken.Parse(restoredParametersJson)) Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature assigned-tool test failed: " &
                "Pattern parameter mismatch.")

        End If

        ' Verify the exact saved tool was restored.
        If restored.Tool Is Nothing Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature assigned-tool test failed: " &
                "Tool was not restored.")

        End If

        If restored.Tool.ID <> savedTool.ID Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature assigned-tool test failed: " &
                "Tool ID mismatch.")

        End If

        If Not Object.ReferenceEquals(restored.Tool, savedTool) Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature assigned-tool test failed: " &
                "Tool was not resolved from the saved collection.")

        End If

        ' Verify that a missing saved tool produces an error.
        Dim missingToolRejected As Boolean = False

        Try

            MachiningFeatureMapper.FromLineOfHolesAtAngleNormalRecord(
                restoredRecord,
                New List(Of WE_ENG_V25_0.Core.Models.Tool)())

        Catch ex As InvalidOperationException

            If ex.Message.Contains("Saved tool not found.") Then
                missingToolRejected = True
            Else
                Throw
            End If

        End Try

        If Not missingToolRejected Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature assigned-tool test failed: " &
                "Missing saved tool was not rejected.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "LineOfHolesAtAngleNormalFeature assigned-tool round-trip test PASSED.")

    End Sub

    Public Shared Sub TestLineOfHolesAtAngleParallelRoundTrip()

        Dim original As New LineOfHolesAtAngleParallelFeature With {
            .ID = 118,
            .FeatureType = MachiningFeatureType.LineOfHolesAtAngleParallel,
            .Description = "Angled Parallel Row Round Trip Test",
            .ToolSetupMemberID = 1534,
            .StartX = 18.5,
            .StartY = 32.75,
            .Count = 9,
            .Spacing = 2.25,
            .Angle = 42.5,
            .Tool = Nothing,
            .Parameters = New HolePatternParameters With {
                .XMirror = True,
                .YMirror = False,
                .Centered = True,
                .RepeatDirection = HoleRepeatDirection.Y,
                .RepeatCount = 3,
                .Gap = 5.5,
                .Orientation = 90
            }
        }

        ' Convert to the persistence record.
        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        ' JSON serialization and deserialization.
        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        ' Restore without an assigned tool.
        Dim restored As LineOfHolesAtAngleParallelFeature =
            MachiningFeatureMapper.FromLineOfHolesAtAngleParallelRecord(
                restoredRecord,
                Nothing)

        ' Verify feature identity and geometry.
        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.StartX <> original.StartX OrElse
           restored.StartY <> original.StartY OrElse
           restored.Count <> original.Count OrElse
           restored.Spacing <> original.Spacing OrElse
           restored.Angle <> original.Angle Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleParallelFeature round-trip failed: " &
                "Feature property mismatch.")

        End If

        ' Verify no tool was automatically assigned.
        If restored.Tool IsNot Nothing Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleParallelFeature round-trip failed: " &
                "Unexpected tool assignment.")

        End If

        ' Verify complete pattern parameters.
        If restored.Parameters Is Nothing Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleParallelFeature round-trip failed: " &
                "Parameters missing.")

        End If

        Dim originalParametersJson As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                original.Parameters)

        Dim restoredParametersJson As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                restored.Parameters)

        If Not Newtonsoft.Json.Linq.JToken.DeepEquals(
            Newtonsoft.Json.Linq.JToken.Parse(originalParametersJson),
            Newtonsoft.Json.Linq.JToken.Parse(restoredParametersJson)) Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleParallelFeature round-trip failed: " &
                "Pattern parameter mismatch.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "LineOfHolesAtAngleParallelFeature JSON round-trip test PASSED.")

    End Sub

    Public Shared Sub TestLineOfHolesAtAngleParallelWithAssignedTool()

        Dim savedTool As New WE_ENG_V25_0.Core.Models.Tool With {
            .ID = 167,
            .TypeID = 16,
            .Description = "1.0 Square 0.0 rad 0.0 deg."
        }

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool) From {
            savedTool
        }

        Dim original As New LineOfHolesAtAngleParallelFeature With {
            .ID = 119,
            .FeatureType = MachiningFeatureType.LineOfHolesAtAngleParallel,
            .Description = "Angled Parallel Row Assigned Tool Test",
            .ToolSetupMemberID = 1534,
            .StartX = 21.5,
            .StartY = 34.25,
            .Count = 14,
            .Spacing = 1.75,
            .Angle = 52.5,
            .Tool = savedTool,
            .Parameters = New HolePatternParameters With {
                .XMirror = False,
                .YMirror = True,
                .Centered = True,
                .RepeatDirection = HoleRepeatDirection.X,
                .RepeatCount = 4,
                .Gap = 7.5,
                .Orientation = 45
            }
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As LineOfHolesAtAngleParallelFeature =
            MachiningFeatureMapper.FromLineOfHolesAtAngleParallelRecord(
                restoredRecord,
                savedTools)

        ' Verify the original manufacturing feature properties.
        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.StartX <> original.StartX OrElse
           restored.StartY <> original.StartY OrElse
           restored.Count <> original.Count OrElse
           restored.Spacing <> original.Spacing OrElse
           restored.Angle <> original.Angle Then

            Throw New InvalidOperationException(
                "Parallel assigned-tool test failed: " &
                "Feature property mismatch.")

        End If

        ' Verify the complete pattern parameters.
        If restored.Parameters Is Nothing Then

            Throw New InvalidOperationException(
                "Parallel assigned-tool test failed: Parameters missing.")

        End If

        Dim originalParametersJson As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(original.Parameters)

        Dim restoredParametersJson As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(restored.Parameters)

        If Not Newtonsoft.Json.Linq.JToken.DeepEquals(
            Newtonsoft.Json.Linq.JToken.Parse(originalParametersJson),
            Newtonsoft.Json.Linq.JToken.Parse(restoredParametersJson)) Then

            Throw New InvalidOperationException(
                "Parallel assigned-tool test failed: " &
                "Pattern parameter mismatch.")

        End If

        ' Verify the exact saved tool object.
        If restored.Tool Is Nothing Then

            Throw New InvalidOperationException(
                "Parallel assigned-tool test failed: Tool missing.")

        End If

        If restored.Tool.ID <> savedTool.ID Then

            Throw New InvalidOperationException(
                "Parallel assigned-tool test failed: Tool ID mismatch.")

        End If

        If Not Object.ReferenceEquals(restored.Tool, savedTool) Then

            Throw New InvalidOperationException(
                "Parallel assigned-tool test failed: " &
                "Tool was not resolved from the saved collection.")

        End If

        ' Verify that an unresolved tool reference is rejected.
        Dim missingToolRejected As Boolean = False

        Try

            MachiningFeatureMapper.FromLineOfHolesAtAngleParallelRecord(
                restoredRecord,
                New List(Of WE_ENG_V25_0.Core.Models.Tool)())

        Catch ex As InvalidOperationException

            If ex.Message.Contains("Saved tool not found.") Then
                missingToolRejected = True
            Else
                Throw
            End If

        End Try

        If Not missingToolRejected Then

            Throw New InvalidOperationException(
                "Parallel assigned-tool test failed: " &
                "Missing saved tool was not rejected.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "LineOfHolesAtAngleParallelFeature assigned-tool round-trip test PASSED.")

    End Sub


    Public Shared Sub TestRapidPointFeatureRoundTrip()

        Dim original As New RapidPointFeature With {
            .ID = 112,
            .FeatureType = MachiningFeatureType.RapidPoint,
            .Description = "Rapid Point Round Trip Test",
            .ToolSetupMemberID = 0,
            .X = 125.5,
            .Y = 87.25,
            .Feed = 250.75,
            .Tool = Nothing
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As RapidPointFeature =
            MachiningFeatureMapper.FromRapidPointRecord(
                restoredRecord,
                Nothing)

        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.X <> original.X OrElse
           restored.Y <> original.Y OrElse
           restored.Feed <> original.Feed OrElse
           restored.Tool IsNot Nothing Then

            Throw New InvalidOperationException(
                "RapidPointFeature round-trip failed: " &
                "Feature property mismatch.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "RapidPointFeature JSON round-trip test PASSED.")

    End Sub

    Public Shared Sub TestRapidPointFeatureWithAssignedTool()

        Dim savedTool As New WE_ENG_V25_0.Core.Models.Tool With {
            .ID = 167,
            .TypeID = 16,
            .Description = "1.0 Square 0.0 rad 0.0 deg."
        }

        Dim savedTools As New List(Of WE_ENG_V25_0.Core.Models.Tool) From {
            savedTool
        }

        Dim original As New RapidPointFeature With {
            .ID = 120,
            .FeatureType = MachiningFeatureType.RapidPoint,
            .Description = "Rapid Point Assigned Tool Test",
            .ToolSetupMemberID = 1534,
            .X = 125.5,
            .Y = 87.25,
            .Feed = 250.75,
            .Tool = savedTool
        }

        Dim record As MachiningFeatureRecord =
            MachiningFeatureMapper.ToRecord(original)

        Dim json As String =
            Newtonsoft.Json.JsonConvert.SerializeObject(
                record,
                Newtonsoft.Json.Formatting.Indented)

        Dim restoredRecord As MachiningFeatureRecord =
            Newtonsoft.Json.JsonConvert.DeserializeObject(
                Of MachiningFeatureRecord)(json)

        Dim restored As RapidPointFeature =
            MachiningFeatureMapper.FromRapidPointRecord(
                restoredRecord,
                savedTools)

        ' Verify original manufacturing feature properties.
        If restored.ID <> original.ID OrElse
           restored.FeatureType <> original.FeatureType OrElse
           restored.Description <> original.Description OrElse
           restored.ToolSetupMemberID <> original.ToolSetupMemberID OrElse
           restored.X <> original.X OrElse
           restored.Y <> original.Y OrElse
           restored.Feed <> original.Feed Then

            Throw New InvalidOperationException(
                "RapidPoint assigned-tool test failed: " &
                "Feature property mismatch.")

        End If

        ' Verify the exact saved tool object.
        If restored.Tool Is Nothing Then

            Throw New InvalidOperationException(
                "RapidPoint assigned-tool test failed: Tool missing.")

        End If

        If restored.Tool.ID <> savedTool.ID Then

            Throw New InvalidOperationException(
                "RapidPoint assigned-tool test failed: Tool ID mismatch.")

        End If

        If Not Object.ReferenceEquals(restored.Tool, savedTool) Then

            Throw New InvalidOperationException(
                "RapidPoint assigned-tool test failed: " &
                "Tool was not resolved from the saved collection.")

        End If

        ' Verify that an unresolved tool reference is rejected.
        Dim missingToolRejected As Boolean = False

        Try

            MachiningFeatureMapper.FromRapidPointRecord(
                restoredRecord,
                New List(Of WE_ENG_V25_0.Core.Models.Tool)())

        Catch ex As InvalidOperationException

            If ex.Message.Contains("Saved tool not found.") Then
                missingToolRejected = True
            Else
                Throw
            End If

        End Try

        If Not missingToolRejected Then

            Throw New InvalidOperationException(
                "RapidPoint assigned-tool test failed: " &
                "Missing saved tool was not rejected.")

        End If

        System.Diagnostics.Debug.WriteLine(
            "RapidPointFeature assigned-tool round-trip test PASSED.")

    End Sub

End Class