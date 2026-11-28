Public Class MachiningFeatureMapper

    Public Shared Function ToRecord(
        feature As HoleFeature) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
            .ID = feature.ID,
            .FeatureType = feature.FeatureType,
            .ConcreteType = NameOf(HoleFeature),
            .Description = feature.Description,
            .ToolSetupMemberID = feature.ToolSetupMemberID
        }

        record.Data("X") = feature.X
        record.Data("Y") = feature.Y
        record.Data("Angle") = feature.Angle
        record.Data("SourceGeometryID") = feature.SourceGeometryID

        If feature.Tool Is Nothing Then
            record.Data("ToolID") = Nothing
        Else
            record.Data("ToolID") = feature.Tool.ID
        End If
        record.Data("Parameters") = feature.Parameters

        Return record

    End Function


    Public Shared Function ToRecord(
    feature As GridOfHolesFeature) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
        .ID = feature.ID,
        .FeatureType = feature.FeatureType,
        .ConcreteType = NameOf(GridOfHolesFeature),
        .Description = feature.Description,
        .ToolSetupMemberID = feature.ToolSetupMemberID
    }

        record.Data("StartX") = feature.StartX
        record.Data("StartY") = feature.StartY

        record.Data("XCount") = feature.XCount
        record.Data("YCount") = feature.YCount

        record.Data("XSpacing") = feature.XSpacing
        record.Data("YSpacing") = feature.YSpacing

        record.Data("PrimaryAxis") = feature.PrimaryAxis

        If feature.Tool Is Nothing Then
            record.Data("ToolID") = Nothing
        Else
            record.Data("ToolID") = feature.Tool.ID
        End If

        record.Data("Parameters") = feature.Parameters

        Return record

    End Function

    Public Shared Function ToRecord(
    feature As LineOfHolesFeature) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
        .ID = feature.ID,
        .FeatureType = feature.FeatureType,
        .ConcreteType = NameOf(LineOfHolesFeature),
        .Description = feature.Description,
        .ToolSetupMemberID = feature.ToolSetupMemberID
    }

        record.Data("StartX") = feature.StartX
        record.Data("StartY") = feature.StartY
        record.Data("Count") = feature.Count
        record.Data("Spacing") = feature.Spacing
        record.Data("Direction") = feature.Direction

        If feature.Tool Is Nothing Then
            record.Data("ToolID") = Nothing
        Else
            record.Data("ToolID") = feature.Tool.ID
        End If

        record.Data("Parameters") = feature.Parameters

        Return record

    End Function



    Public Shared Function FromRecord(
    record As MachiningFeatureRecord,
    savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
    ) As HoleFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <> NameOf(HoleFeature) Then
            Throw New InvalidOperationException(
                "Record is not a HoleFeature.")
        End If

        Dim hole As New HoleFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .X = Convert.ToDouble(record.Data("X")),
            .Y = Convert.ToDouble(record.Data("Y")),
           .Angle = Convert.ToDouble(record.Data("Angle")),
            .SourceGeometryID = Convert.ToInt32(record.Data("SourceGeometryID"))
        }

        Dim savedParameters = record.Data("Parameters")

        If savedParameters Is Nothing Then

            hole.Parameters = Nothing

        ElseIf TypeOf savedParameters Is Newtonsoft.Json.Linq.JObject Then

            hole.Parameters =
        DirectCast(savedParameters,
                   Newtonsoft.Json.Linq.JObject).
        ToObject(Of HolePatternParameters)()

        ElseIf TypeOf savedParameters Is HolePatternParameters Then

            hole.Parameters =
        DirectCast(savedParameters, HolePatternParameters)

        Else

            Throw New InvalidOperationException(
        "Unsupported HolePatternParameters data type.")

        End If

        If Not record.Data.ContainsKey("ToolID") Then
            Throw New InvalidOperationException(
        "HoleFeature record is missing ToolID.")
        End If

        Dim savedToolID = record.Data("ToolID")

        If TypeOf savedToolID Is Newtonsoft.Json.Linq.JValue Then

            savedToolID =
        DirectCast(savedToolID,
                   Newtonsoft.Json.Linq.JValue).Value

        End If

        If savedToolID Is Nothing Then

            hole.Tool = Nothing

        Else

            Dim toolID As Integer = Convert.ToInt32(savedToolID)

            If savedTools Is Nothing Then
                Throw New InvalidOperationException(
            "Saved tool collection is missing.")
            End If

            hole.Tool = savedTools.FirstOrDefault(
        Function(t) t.ID = toolID)

            If hole.Tool Is Nothing Then
                Throw New InvalidOperationException(
            "Saved tool not found. ToolID=" & toolID)
            End If

        End If




        Return hole

    End Function

    Public Shared Function FromLineOfHolesRecord(
    record As MachiningFeatureRecord,
    savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
    ) As LineOfHolesFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <> NameOf(LineOfHolesFeature) Then
            Throw New InvalidOperationException(
                "Record is not a LineOfHolesFeature.")
        End If

        Dim row As New LineOfHolesFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .StartX = Convert.ToDouble(record.Data("StartX")),
            .StartY = Convert.ToDouble(record.Data("StartY")),
            .Count = Convert.ToInt32(record.Data("Count")),
            .Spacing = Convert.ToDouble(record.Data("Spacing")),
            .Direction = CType(
                Convert.ToInt32(record.Data("Direction")),
                HoleRepeatDirection)
        }


        Dim savedParameters = record.Data("Parameters")

        If savedParameters Is Nothing Then

            row.Parameters = Nothing

        ElseIf TypeOf savedParameters Is Newtonsoft.Json.Linq.JObject Then

            row.Parameters =
        DirectCast(savedParameters,
                   Newtonsoft.Json.Linq.JObject).
        ToObject(Of HolePatternParameters)()

        ElseIf TypeOf savedParameters Is HolePatternParameters Then

            row.Parameters =
        DirectCast(savedParameters, HolePatternParameters)

        Else

            Throw New InvalidOperationException(
        "Unsupported LineOfHolesFeature parameters data type.")

        End If


        If Not record.Data.ContainsKey("ToolID") Then
            Throw New InvalidOperationException(
        "LineOfHolesFeature record is missing ToolID.")
        End If

        Dim savedToolID As Object = record.Data("ToolID")

        If TypeOf savedToolID Is Newtonsoft.Json.Linq.JValue Then
            savedToolID =
        DirectCast(savedToolID,
                   Newtonsoft.Json.Linq.JValue).Value
        End If

        If savedToolID Is Nothing Then

            row.Tool = Nothing

        Else

            Dim toolID As Integer = Convert.ToInt32(savedToolID)

            If savedTools Is Nothing Then
                Throw New InvalidOperationException(
            "Saved tool collection is missing.")
            End If

            row.Tool = savedTools.FirstOrDefault(
        Function(t) t IsNot Nothing AndAlso t.ID = toolID)

            If row.Tool Is Nothing Then
                Throw New InvalidOperationException(
            "Saved tool not found. ToolID=" & toolID)
            End If

        End If





        Return row

    End Function


    Public Shared Function FromGridOfHolesRecord(
    record As MachiningFeatureRecord,
    savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
) As GridOfHolesFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <> NameOf(GridOfHolesFeature) Then
            Throw New InvalidOperationException(
                "Record is not a GridOfHolesFeature.")
        End If

        Dim grid As New GridOfHolesFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .StartX = Convert.ToDouble(record.Data("StartX")),
            .StartY = Convert.ToDouble(record.Data("StartY")),
            .XCount = Convert.ToInt32(record.Data("XCount")),
            .YCount = Convert.ToInt32(record.Data("YCount")),
            .XSpacing = Convert.ToDouble(record.Data("XSpacing")),
            .YSpacing = Convert.ToDouble(record.Data("YSpacing")),
            .PrimaryAxis = CType(
                Convert.ToInt32(record.Data("PrimaryAxis")),
                HoleRepeatDirection)
        }

        ' Restore the original pattern parameters.
        Dim savedParameters As Object = record.Data("Parameters")

        If savedParameters Is Nothing Then

            grid.Parameters = Nothing

        ElseIf TypeOf savedParameters Is Newtonsoft.Json.Linq.JObject Then

            grid.Parameters =
                DirectCast(
                    savedParameters,
                    Newtonsoft.Json.Linq.JObject
                ).ToObject(Of HolePatternParameters)()

        ElseIf TypeOf savedParameters Is HolePatternParameters Then

            grid.Parameters =
                DirectCast(savedParameters, HolePatternParameters)

        Else

            Throw New InvalidOperationException(
                "Unsupported GridOfHolesFeature parameters data type.")

        End If

        ' Restore the tool reference from the saved environment.
        If Not record.Data.ContainsKey("ToolID") Then
            Throw New InvalidOperationException(
                "GridOfHolesFeature record is missing ToolID.")
        End If

        Dim savedToolID As Object = record.Data("ToolID")

        If TypeOf savedToolID Is Newtonsoft.Json.Linq.JValue Then

            savedToolID =
                DirectCast(
                    savedToolID,
                    Newtonsoft.Json.Linq.JValue
                ).Value

        End If

        If savedToolID Is Nothing Then

            grid.Tool = Nothing

        Else

            Dim toolID As Integer = Convert.ToInt32(savedToolID)

            If savedTools Is Nothing Then
                Throw New InvalidOperationException(
                    "Saved tool collection is missing.")
            End If

            grid.Tool = savedTools.FirstOrDefault(
                Function(t) t IsNot Nothing AndAlso t.ID = toolID)

            If grid.Tool Is Nothing Then
                Throw New InvalidOperationException(
                    "Saved tool not found. ToolID=" & toolID)
            End If

        End If

        Return grid

    End Function

    Public Shared Function ToRecord(
    feature As BoltHoleCircleFeature) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
            .ID = feature.ID,
            .FeatureType = feature.FeatureType,
            .ConcreteType = NameOf(BoltHoleCircleFeature),
            .Description = feature.Description,
            .ToolSetupMemberID = feature.ToolSetupMemberID
        }

        record.Data("CenterX") = feature.CenterX
        record.Data("CenterY") = feature.CenterY

        record.Data("Radius") = feature.Radius
        record.Data("Count") = feature.Count

        record.Data("StartAngle") = feature.StartAngle
        record.Data("IncrementAngle") = feature.IncrementAngle

        record.Data("AutoIndex") = feature.AutoIndex

        If feature.Tool Is Nothing Then
            record.Data("ToolID") = Nothing
        Else
            record.Data("ToolID") = feature.Tool.ID
        End If

        record.Data("Parameters") = feature.Parameters

        Return record

    End Function

    Public Shared Function FromBoltHoleCircleRecord(
        record As MachiningFeatureRecord,
        savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
    ) As BoltHoleCircleFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <> NameOf(BoltHoleCircleFeature) Then
            Throw New InvalidOperationException(
                "Record is not a BoltHoleCircleFeature.")
        End If

        Dim circle As New BoltHoleCircleFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .CenterX = Convert.ToDouble(record.Data("CenterX")),
            .CenterY = Convert.ToDouble(record.Data("CenterY")),
            .Radius = Convert.ToDouble(record.Data("Radius")),
            .Count = Convert.ToInt32(record.Data("Count")),
            .StartAngle = Convert.ToDouble(record.Data("StartAngle")),
            .IncrementAngle = Convert.ToDouble(record.Data("IncrementAngle")),
            .AutoIndex = Convert.ToBoolean(record.Data("AutoIndex"))
        }

        ' Restore the saved pattern parameters.
        Dim savedParameters As Object = record.Data("Parameters")

        If savedParameters Is Nothing Then

            circle.Parameters = Nothing

        ElseIf TypeOf savedParameters Is Newtonsoft.Json.Linq.JObject Then

            circle.Parameters =
                DirectCast(
                    savedParameters,
                    Newtonsoft.Json.Linq.JObject
                ).ToObject(Of HolePatternParameters)()

        ElseIf TypeOf savedParameters Is HolePatternParameters Then

            circle.Parameters =
                DirectCast(savedParameters, HolePatternParameters)

        Else

            Throw New InvalidOperationException(
                "Unsupported BoltHoleCircleFeature parameters data type.")

        End If

        ' Restore the tool from the saved manufacturing environment.
        If Not record.Data.ContainsKey("ToolID") Then
            Throw New InvalidOperationException(
                "BoltHoleCircleFeature record is missing ToolID.")
        End If

        Dim savedToolID As Object = record.Data("ToolID")

        If TypeOf savedToolID Is Newtonsoft.Json.Linq.JValue Then

            savedToolID =
                DirectCast(
                    savedToolID,
                    Newtonsoft.Json.Linq.JValue
                ).Value

        End If

        If savedToolID Is Nothing Then

            circle.Tool = Nothing

        Else

            Dim toolID As Integer = Convert.ToInt32(savedToolID)

            If savedTools Is Nothing Then
                Throw New InvalidOperationException(
                    "Saved tool collection is missing.")
            End If

            circle.Tool = savedTools.FirstOrDefault(
                Function(t) t IsNot Nothing AndAlso t.ID = toolID)

            If circle.Tool Is Nothing Then
                Throw New InvalidOperationException(
                    "Saved tool not found. ToolID=" & toolID)
            End If

        End If

        Return circle

    End Function

    Public Shared Function ToRecord(
    feature As LineOfHolesAtAngleParallelFeature
    ) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
        .ID = feature.ID,
        .FeatureType = feature.FeatureType,
        .ConcreteType = NameOf(LineOfHolesAtAngleParallelFeature),
        .Description = feature.Description,
        .ToolSetupMemberID = feature.ToolSetupMemberID
    }

        record.Data("StartX") = feature.StartX
        record.Data("StartY") = feature.StartY
        record.Data("Count") = feature.Count
        record.Data("Spacing") = feature.Spacing
        record.Data("Angle") = feature.Angle

        If feature.Tool Is Nothing Then

            record.Data("ToolID") = Nothing

        Else

            record.Data("ToolID") = feature.Tool.ID

        End If

        record.Data("Parameters") = feature.Parameters

        Return record

    End Function

    Public Shared Function ToRecord(
    feature As BarrierHoleFeature) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
            .ID = feature.ID,
            .FeatureType = feature.FeatureType,
            .ConcreteType = NameOf(BarrierHoleFeature),
            .Description = feature.Description,
            .ToolSetupMemberID = feature.ToolSetupMemberID
        }

        record.Data("StartX") = feature.StartX
        record.Data("StartY") = feature.StartY

        record.Data("EndX") = feature.EndX
        record.Data("EndY") = feature.EndY

        record.Data("Spacing") = feature.Spacing

        record.Data("ToolIndexAngle") = feature.ToolIndexAngle

        If feature.Tool Is Nothing Then
            record.Data("ToolID") = Nothing
        Else
            record.Data("ToolID") = feature.Tool.ID
        End If

        record.Data("Parameters") = feature.Parameters

        Return record

    End Function

    Public Shared Function FromBarrierHoleRecord(
        record As MachiningFeatureRecord,
        savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
    ) As BarrierHoleFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <> NameOf(BarrierHoleFeature) Then
            Throw New InvalidOperationException(
                "Record is not a BarrierHoleFeature.")
        End If

        Dim barrier As New BarrierHoleFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .StartX = Convert.ToDouble(record.Data("StartX")),
            .StartY = Convert.ToDouble(record.Data("StartY")),
            .EndX = Convert.ToDouble(record.Data("EndX")),
            .EndY = Convert.ToDouble(record.Data("EndY")),
            .Spacing = Convert.ToDouble(record.Data("Spacing")),
            .ToolIndexAngle = Convert.ToDouble(
                record.Data("ToolIndexAngle"))
        }

        ' Restore the original pattern parameters.
        Dim savedParameters As Object = record.Data("Parameters")

        If savedParameters Is Nothing Then

            barrier.Parameters = Nothing

        ElseIf TypeOf savedParameters Is Newtonsoft.Json.Linq.JObject Then

            barrier.Parameters =
                DirectCast(
                    savedParameters,
                    Newtonsoft.Json.Linq.JObject
                ).ToObject(Of HolePatternParameters)()

        ElseIf TypeOf savedParameters Is HolePatternParameters Then

            barrier.Parameters =
                DirectCast(savedParameters, HolePatternParameters)

        Else

            Throw New InvalidOperationException(
                "Unsupported BarrierHoleFeature parameters data type.")

        End If

        ' Restore the tool from the saved manufacturing environment.
        If Not record.Data.ContainsKey("ToolID") Then
            Throw New InvalidOperationException(
                "BarrierHoleFeature record is missing ToolID.")
        End If

        Dim savedToolID As Object = record.Data("ToolID")

        If TypeOf savedToolID Is Newtonsoft.Json.Linq.JValue Then

            savedToolID =
                DirectCast(
                    savedToolID,
                    Newtonsoft.Json.Linq.JValue
                ).Value

        End If

        If savedToolID Is Nothing Then

            barrier.Tool = Nothing

        Else

            Dim toolID As Integer = Convert.ToInt32(savedToolID)

            If savedTools Is Nothing Then
                Throw New InvalidOperationException(
                    "Saved tool collection is missing.")
            End If

            barrier.Tool = savedTools.FirstOrDefault(
                Function(t) t IsNot Nothing AndAlso t.ID = toolID)

            If barrier.Tool Is Nothing Then
                Throw New InvalidOperationException(
                    "Saved tool not found. ToolID=" & toolID)
            End If

        End If

        Return barrier

    End Function

    Public Shared Function ToRecord(
    feature As CommandFeature) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
            .ID = feature.ID,
            .FeatureType = feature.FeatureType,
            .ConcreteType = NameOf(CommandFeature),
            .Description = feature.Description,
            .ToolSetupMemberID = feature.ToolSetupMemberID
        }

        record.Data("X") = feature.X
        record.Data("Y") = feature.Y

        record.Data("Angle") = feature.Angle
        record.Data("Position") = feature.Position

        record.Data("FontSize") = feature.FontSize
        record.Data("CommandText") = feature.CommandText

        Return record

    End Function

    Public Shared Function FromCommandFeatureRecord(
        record As MachiningFeatureRecord
    ) As CommandFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <> NameOf(CommandFeature) Then
            Throw New InvalidOperationException(
                "Record is not a CommandFeature.")
        End If

        Dim command As New CommandFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .X = Convert.ToDouble(record.Data("X")),
            .Y = Convert.ToDouble(record.Data("Y")),
            .Angle = Convert.ToDouble(record.Data("Angle")),
            .Position = CType(
                Convert.ToInt32(record.Data("Position")),
                CommandTextPosition),
            .FontSize = Convert.ToDouble(record.Data("FontSize"))
        }

        ' Preserve the command text exactly, including Nothing.
        Dim savedText As Object = record.Data("CommandText")

        If TypeOf savedText Is Newtonsoft.Json.Linq.JValue Then
            savedText =
                DirectCast(
                    savedText,
                    Newtonsoft.Json.Linq.JValue
                ).Value
        End If

        If savedText Is Nothing Then
            command.CommandText = Nothing
        ElseIf TypeOf savedText Is String Then
            command.CommandText = DirectCast(savedText, String)
        Else
            Throw New InvalidOperationException(
                "Unsupported CommandFeature text data type.")
        End If

        Return command

    End Function

    Public Shared Function ToRecord(
    feature As RapidPointFeature) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
        .ID = feature.ID,
        .FeatureType = feature.FeatureType,
        .ConcreteType = NameOf(RapidPointFeature),
        .Description = feature.Description,
        .ToolSetupMemberID = feature.ToolSetupMemberID
    }

        record.Data("X") = feature.X
        record.Data("Y") = feature.Y
        record.Data("Feed") = feature.Feed

        record.Data("ToolID") =
        If(feature.Tool Is Nothing,
           CType(Nothing, Integer?),
           feature.Tool.ID)

        Return record

    End Function

    Public Shared Function ToRecord(
    feature As ContourFeature) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
            .ID = feature.ID,
            .FeatureType = feature.FeatureType,
            .ConcreteType = NameOf(ContourFeature),
            .Description = feature.Description,
            .ToolSetupMemberID = feature.ToolSetupMemberID
        }

        ' Preserve the operator's logical closure decision.
        record.Data("IsLogicallyClosed") = feature.IsLogicallyClosed

        ' Nothing is meaningful for ToolIndexAngle.
        record.Data("ToolIndexAngle") = feature.ToolIndexAngle

        record.Data("OverLap") = feature.OverLap
        record.Data("DOff") = feature.DOff

        ' Preserve Nothing versus an existing empty path.
        If feature.Path Is Nothing Then

            record.Data("Path") = Nothing

        Else

            Dim segments As New List(Of PathSegmentRecord)

            For Each segment As FabV25_WIN8.ToolShapes.IPathSegment _
                In feature.Path.Segments

                segments.Add(PathSegmentMapper.ToRecord(segment))

            Next

            record.Data("Path") = segments

        End If

        Return record

    End Function

    Public Shared Function FromContourFeatureRecord(
        record As MachiningFeatureRecord
    ) As ContourFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <> NameOf(ContourFeature) Then
            Throw New InvalidOperationException(
                "Record is not a ContourFeature.")
        End If

        Dim contour As New ContourFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .IsLogicallyClosed =
                Convert.ToBoolean(record.Data("IsLogicallyClosed"))
        }

        ' Restore the nullable tool index angle.
        Dim savedAngle As Object = record.Data("ToolIndexAngle")

        If TypeOf savedAngle Is Newtonsoft.Json.Linq.JValue Then
            savedAngle =
                DirectCast(
                    savedAngle,
                    Newtonsoft.Json.Linq.JValue
                ).Value
        End If

        If savedAngle Is Nothing Then
            contour.ToolIndexAngle = Nothing
        Else
            contour.ToolIndexAngle = Convert.ToDouble(savedAngle)
        End If


        ' Restore process parameters when present.
        ' Older MM3 files retain the ContourFeature defaults.

        If record.Data.ContainsKey("OverLap") Then

            Dim savedOverlap As Object = record.Data("OverLap")

            If TypeOf savedOverlap Is Newtonsoft.Json.Linq.JValue Then
                savedOverlap =
            DirectCast(
                savedOverlap,
                Newtonsoft.Json.Linq.JValue
            ).Value
            End If

            If savedOverlap IsNot Nothing Then
                contour.OverLap = Convert.ToDouble(savedOverlap)
            End If

        End If

        If record.Data.ContainsKey("DOff") Then

            Dim savedDOff As Object = record.Data("DOff")

            If TypeOf savedDOff Is Newtonsoft.Json.Linq.JValue Then
                savedDOff =
            DirectCast(
                savedDOff,
                Newtonsoft.Json.Linq.JValue
            ).Value
            End If

            If savedDOff IsNot Nothing Then
                contour.DOff = Convert.ToInt32(savedDOff)
            End If

        End If



        ' Restore Nothing versus an existing path.
        Dim savedPath As Object = record.Data("Path")

        If savedPath Is Nothing Then

            contour.Path = Nothing

        Else

            Dim segmentRecords As List(Of PathSegmentRecord)

            If TypeOf savedPath Is Newtonsoft.Json.Linq.JArray Then

                segmentRecords =
                    DirectCast(
                        savedPath,
                        Newtonsoft.Json.Linq.JArray
                    ).ToObject(Of List(Of PathSegmentRecord))()

            ElseIf TypeOf savedPath Is List(Of PathSegmentRecord) Then

                segmentRecords =
                    DirectCast(savedPath, List(Of PathSegmentRecord))

            Else

                Throw New InvalidOperationException(
                    "Unsupported ContourFeature path data type.")

            End If

            If segmentRecords Is Nothing Then
                Throw New InvalidOperationException(
                    "ContourFeature path segment collection is missing.")
            End If

            Dim path As New FabV25_WIN8.ToolShapes.Path2D()

            For Each segmentRecord As PathSegmentRecord In segmentRecords

                path.Add(
                    PathSegmentMapper.FromRecord(segmentRecord))

            Next

            contour.Path = path

        End If

        Return contour

    End Function


    Public Shared Function ToRecord(
    feature As LineOfHolesAtAngleNormalFeature
    ) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        Dim record As New MachiningFeatureRecord With {
            .ID = feature.ID,
            .FeatureType = feature.FeatureType,
            .ConcreteType = NameOf(LineOfHolesAtAngleNormalFeature),
            .Description = feature.Description,
            .ToolSetupMemberID = feature.ToolSetupMemberID
        }

        record.Data("StartX") = feature.StartX
        record.Data("StartY") = feature.StartY
        record.Data("Count") = feature.Count
        record.Data("Spacing") = feature.Spacing
        record.Data("Angle") = feature.Angle

        If feature.Tool Is Nothing Then

            record.Data("ToolID") = Nothing

        Else

            record.Data("ToolID") = feature.Tool.ID

        End If

        record.Data("Parameters") = feature.Parameters

        Return record

    End Function

    Public Shared Function FromLineOfHolesAtAngleNormalRecord(
        record As MachiningFeatureRecord,
        savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
        ) As LineOfHolesAtAngleNormalFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <>
           NameOf(LineOfHolesAtAngleNormalFeature) Then

            Throw New InvalidOperationException(
                "Record is not a LineOfHolesAtAngleNormalFeature.")
        End If

        Dim row As New LineOfHolesAtAngleNormalFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .StartX = Convert.ToDouble(record.Data("StartX")),
            .StartY = Convert.ToDouble(record.Data("StartY")),
            .Count = Convert.ToInt32(record.Data("Count")),
            .Spacing = Convert.ToDouble(record.Data("Spacing")),
            .Angle = Convert.ToDouble(record.Data("Angle"))
        }

        ' Restore the complete hole pattern parameters.
        Dim savedParameters As Object = record.Data("Parameters")

        If savedParameters Is Nothing Then

            row.Parameters = Nothing

        ElseIf TypeOf savedParameters Is Newtonsoft.Json.Linq.JObject Then

            row.Parameters =
                DirectCast(
                    savedParameters,
                    Newtonsoft.Json.Linq.JObject
                ).ToObject(Of HolePatternParameters)()

        ElseIf TypeOf savedParameters Is HolePatternParameters Then

            row.Parameters =
                DirectCast(savedParameters, HolePatternParameters)

        Else

            Throw New InvalidOperationException(
                "Unsupported LineOfHolesAtAngleNormalFeature " &
                "parameters data type.")

        End If

        ' ToolID must exist, even when its value is Nothing.
        If Not record.Data.ContainsKey("ToolID") Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleNormalFeature record " &
                "is missing ToolID.")

        End If

        Dim savedToolID As Object = record.Data("ToolID")

        ' Newtonsoft may deserialize a value as JValue.
        If TypeOf savedToolID Is Newtonsoft.Json.Linq.JValue Then

            savedToolID =
                DirectCast(
                    savedToolID,
                    Newtonsoft.Json.Linq.JValue
                ).Value

        End If

        If savedToolID Is Nothing Then

            row.Tool = Nothing

        Else

            Dim toolID As Integer = Convert.ToInt32(savedToolID)

            If savedTools Is Nothing Then

                Throw New InvalidOperationException(
                    "Saved tool collection is missing.")

            End If

            row.Tool = savedTools.FirstOrDefault(
                Function(t) t IsNot Nothing AndAlso t.ID = toolID)

            If row.Tool Is Nothing Then

                Throw New InvalidOperationException(
                    "Saved tool not found. ToolID=" & toolID)

            End If

        End If

        Return row

    End Function


    Public Shared Function FromLineOfHolesAtAngleParallelRecord(
    record As MachiningFeatureRecord,
    savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
    ) As LineOfHolesAtAngleParallelFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <>
           NameOf(LineOfHolesAtAngleParallelFeature) Then

            Throw New InvalidOperationException(
                "Record is not a LineOfHolesAtAngleParallelFeature.")
        End If

        Dim row As New LineOfHolesAtAngleParallelFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .StartX = Convert.ToDouble(record.Data("StartX")),
            .StartY = Convert.ToDouble(record.Data("StartY")),
            .Count = Convert.ToInt32(record.Data("Count")),
            .Spacing = Convert.ToDouble(record.Data("Spacing")),
            .Angle = Convert.ToDouble(record.Data("Angle"))
        }

        ' Restore the complete hole pattern parameters.
        Dim savedParameters As Object = record.Data("Parameters")

        If savedParameters Is Nothing Then

            row.Parameters = Nothing

        ElseIf TypeOf savedParameters Is Newtonsoft.Json.Linq.JObject Then

            row.Parameters =
                DirectCast(
                    savedParameters,
                    Newtonsoft.Json.Linq.JObject
                ).ToObject(Of HolePatternParameters)()

        ElseIf TypeOf savedParameters Is HolePatternParameters Then

            row.Parameters =
                DirectCast(savedParameters, HolePatternParameters)

        Else

            Throw New InvalidOperationException(
                "Unsupported LineOfHolesAtAngleParallelFeature " &
                "parameters data type.")

        End If

        ' ToolID must exist, even if its value is Nothing.
        If Not record.Data.ContainsKey("ToolID") Then

            Throw New InvalidOperationException(
                "LineOfHolesAtAngleParallelFeature record " &
                "is missing ToolID.")

        End If

        Dim savedToolID As Object = record.Data("ToolID")

        ' Newtonsoft may deserialize the value as JValue.
        If TypeOf savedToolID Is Newtonsoft.Json.Linq.JValue Then

            savedToolID =
                DirectCast(
                    savedToolID,
                    Newtonsoft.Json.Linq.JValue
                ).Value

        End If

        If savedToolID Is Nothing Then

            row.Tool = Nothing

        Else

            Dim toolID As Integer = Convert.ToInt32(savedToolID)

            If savedTools Is Nothing Then

                Throw New InvalidOperationException(
                    "Saved tool collection is missing.")

            End If

            row.Tool = savedTools.FirstOrDefault(
                Function(t) t IsNot Nothing AndAlso t.ID = toolID)

            If row.Tool Is Nothing Then

                Throw New InvalidOperationException(
                    "Saved tool not found. ToolID=" & toolID)

            End If

        End If

        Return row

    End Function

    Public Shared Function FromRapidPointRecord(
    record As MachiningFeatureRecord,
    savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
    ) As RapidPointFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.ConcreteType <> NameOf(RapidPointFeature) Then
            Throw New InvalidOperationException(
                "Record is not a RapidPointFeature.")
        End If

        Dim rapid As New RapidPointFeature With {
            .ID = record.ID,
            .FeatureType = record.FeatureType,
            .Description = record.Description,
            .ToolSetupMemberID = record.ToolSetupMemberID,
            .X = Convert.ToDouble(record.Data("X")),
            .Y = Convert.ToDouble(record.Data("Y")),
            .Feed = Convert.ToDouble(record.Data("Feed"))
        }

        If Not record.Data.ContainsKey("ToolID") Then
            Throw New InvalidOperationException(
                "RapidPointFeature record is missing ToolID.")
        End If

        Dim savedToolID = record.Data("ToolID")

        If TypeOf savedToolID Is Newtonsoft.Json.Linq.JValue Then

            savedToolID =
                DirectCast(savedToolID,
                           Newtonsoft.Json.Linq.JValue).Value

        End If

        If savedToolID Is Nothing Then

            rapid.Tool = Nothing

        Else

            Dim toolID As Integer = Convert.ToInt32(savedToolID)

            If savedTools Is Nothing Then
                Throw New InvalidOperationException(
                    "Saved tool collection is missing.")
            End If

            rapid.Tool = savedTools.FirstOrDefault(
                Function(t) t.ID = toolID)

            If rapid.Tool Is Nothing Then
                Throw New InvalidOperationException(
                    "Saved tool not found. ToolID=" & toolID)
            End If

        End If

        Return rapid

    End Function


    Public Shared Function RestoreFeature(
    record As MachiningFeatureRecord,
    savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
) As MachiningFeature

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        Select Case record.ConcreteType

            Case NameOf(HoleFeature)
                Return FromRecord(record, savedTools)

            Case NameOf(LineOfHolesFeature)
                Return FromLineOfHolesRecord(record, savedTools)

            Case NameOf(GridOfHolesFeature)
                Return FromGridOfHolesRecord(record, savedTools)

            Case NameOf(BoltHoleCircleFeature)
                Return FromBoltHoleCircleRecord(record, savedTools)

            Case NameOf(BarrierHoleFeature)
                Return FromBarrierHoleRecord(record, savedTools)

            Case NameOf(CommandFeature)
                Return FromCommandFeatureRecord(record)

            Case NameOf(ContourFeature)
                Return FromContourFeatureRecord(record)

            Case NameOf(LineOfHolesAtAngleNormalFeature)
                Return FromLineOfHolesAtAngleNormalRecord(
                record, savedTools)

            Case NameOf(LineOfHolesAtAngleParallelFeature)
                Return FromLineOfHolesAtAngleParallelRecord(
                record, savedTools)

            Case NameOf(RapidPointFeature)
                Return FromRapidPointRecord(record, savedTools)

            Case Else
                Throw New InvalidOperationException(
                "Unsupported manufacturing feature type: " &
                Convert.ToString(record.ConcreteType))

        End Select

    End Function


    Public Shared Function RestoreManufacturingFeatures(
    mm3File As MM3File,
    savedTools As IEnumerable(Of WE_ENG_V25_0.Core.Models.Tool)
) As Dictionary(Of Integer, MachiningFeature)

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If mm3File.ManufacturingFeatures Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 ManufacturingFeatures collection is missing.")
        End If

        Dim featuresByID As New Dictionary(Of Integer, MachiningFeature)

        For Each record As MachiningFeatureRecord In mm3File.ManufacturingFeatures

            If record Is Nothing Then
                Throw New InvalidOperationException(
                "MM3 ManufacturingFeatures contains a null record.")
            End If

            Dim feature As MachiningFeature =
            RestoreFeature(record, savedTools)

            If feature Is Nothing Then
                Throw New InvalidOperationException(
                "Manufacturing feature restoration returned Nothing.")
            End If

            If featuresByID.ContainsKey(feature.ID) Then
                Throw New InvalidOperationException(
                "Duplicate manufacturing feature ID: " &
                feature.ID.ToString())
            End If

            featuresByID.Add(feature.ID, feature)

        Next

        Return featuresByID

    End Function


    Public Shared Sub RestoreJobSheetManufacturingGeometry(
    jobSheet As JobSheet,
    mm3File As MM3File,
    featuresByID As Dictionary(Of Integer, MachiningFeature)
)

        If jobSheet Is Nothing Then
            Throw New ArgumentNullException(NameOf(jobSheet))
        End If

        If mm3File Is Nothing Then
            Throw New ArgumentNullException(NameOf(mm3File))
        End If

        If featuresByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(featuresByID))
        End If

        If jobSheet.ManufacturingModel Is Nothing Then
            Throw New InvalidOperationException(
            "JobSheet ManufacturingModel is missing.")
        End If

        If mm3File.ManufacturingFeatures Is Nothing Then
            Throw New InvalidOperationException(
            "MM3 ManufacturingFeatures collection is missing.")
        End If

        Dim restoredFeatures As New List(Of MachiningFeature)

        ' Preserve the original manufacturing feature order.

        For Each featureID As Integer In mm3File.ManufacturingGeometryIDs

            Dim feature As MachiningFeature = Nothing

            If Not featuresByID.TryGetValue(featureID, feature) Then
                Throw New InvalidOperationException(
            "Manufacturing feature could not be resolved. ID=" &
            featureID.ToString())
            End If

            restoredFeatures.Add(feature)

        Next


        jobSheet.ManufacturingModel.ManufacturingGeometry = restoredFeatures

    End Sub


    Public Shared Function SaveFeature(
    feature As MachiningFeature
) As MachiningFeatureRecord

        If feature Is Nothing Then
            Throw New ArgumentNullException(NameOf(feature))
        End If

        If TypeOf feature Is BarrierHoleFeature Then
            Return ToRecord(DirectCast(feature, BarrierHoleFeature))

        ElseIf TypeOf feature Is BoltHoleCircleFeature Then
            Return ToRecord(DirectCast(feature, BoltHoleCircleFeature))

        ElseIf TypeOf feature Is CommandFeature Then
            Return ToRecord(DirectCast(feature, CommandFeature))

        ElseIf TypeOf feature Is ContourFeature Then
            Return ToRecord(DirectCast(feature, ContourFeature))

        ElseIf TypeOf feature Is GridOfHolesFeature Then
            Return ToRecord(DirectCast(feature, GridOfHolesFeature))

        ElseIf TypeOf feature Is HoleFeature Then
            Return ToRecord(DirectCast(feature, HoleFeature))

        ElseIf TypeOf feature Is LineOfHolesAtAngleNormalFeature Then
            Return ToRecord(DirectCast(feature, LineOfHolesAtAngleNormalFeature))

        ElseIf TypeOf feature Is LineOfHolesAtAngleParallelFeature Then
            Return ToRecord(DirectCast(feature, LineOfHolesAtAngleParallelFeature))

        ElseIf TypeOf feature Is LineOfHolesFeature Then
            Return ToRecord(DirectCast(feature, LineOfHolesFeature))

        ElseIf TypeOf feature Is RapidPointFeature Then
            Return ToRecord(DirectCast(feature, RapidPointFeature))

        End If

        Throw New NotSupportedException(
        "Unsupported manufacturing feature type: " &
        feature.GetType().FullName)

    End Function

End Class