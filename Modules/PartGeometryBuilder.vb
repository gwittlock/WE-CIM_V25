Imports FabV25_WIN8.ToolShapes

Public Enum GeoProfileClassification
    Invalid = 0
    Outside = 1
    Inside = 2
    Ambiguous = 3
End Enum
Public Module PartGeometryBuilder

    Public Function CreatePartGeometry(
    cadEntities As List(Of CadEntity),
   layerSetup As AppData.layersetup) As Part

        Dim geometry As New List(Of GeoEntity)()

        Dim layers As New Dictionary(Of String, Layer)(
        StringComparer.OrdinalIgnoreCase)

        Dim points As New Dictionary(Of (Double, Double), GeoPoint)()

        For Each cad As CadEntity In cadEntities

            If Not TypeOf cad Is CadEntity.CadLine AndAlso
               Not TypeOf cad Is CadEntity.CadArc AndAlso
               Not TypeOf cad Is CadEntity.CadCircle AndAlso
               Not TypeOf cad Is CadPoint Then

                Continue For
            End If

            Dim directive = FindLayerDirective(
            cad.LayerName,
            layerSetup)

            If directive Is Nothing Then
                Continue For
            End If

            Dim layer As Layer = GetOrCreateLayer(
            directive.CAMLayer,
            directive.DisplayColor,
            layers)

            If TypeOf cad Is CadEntity.CadLine Then

                Dim line As GeoLine = ConvertCadLine(
                DirectCast(cad, CadEntity.CadLine),
                layer,
                points)

                geometry.Add(line)

            ElseIf TypeOf cad Is CadEntity.CadArc Then

                Dim arc As GeoArc = ConvertCadArc(
                DirectCast(cad, CadEntity.CadArc),
                layer,
                points)

                geometry.Add(arc)

            ElseIf TypeOf cad Is CadEntity.CadCircle Then

                Dim circle As GeoCircle = ConvertCadCircle(
                DirectCast(cad, CadEntity.CadCircle),
                layer,
                points)

                geometry.Add(circle)

            ElseIf TypeOf cad Is CadPoint Then

                Dim cadPoint As CadPoint =
                    DirectCast(cad, CadPoint)

                Dim geoPoint As New GeoPoint With {
                    .X = CDbl(cadPoint.Position.X),
                    .Y = CDbl(cadPoint.Position.Y),
                    .Association = layer
                }

                geometry.Add(geoPoint)
            End If

        Next

        ApplyGeoFilterTolerance(
        geometry,
        layerSetup.FilterTolerance)

        ApplyGeoCleanTolerance(
        geometry,
        layerSetup.CleanTolerance)

        Dim geoProfiles As List(Of GeoProfile) =
        BuildGeoProfiles(
            geometry,
            layerSetup.GapTolerance)

        'Debug.WriteLine(
        '"Native GeoProfile Count: " & geoProfiles.Count)

        Dim part As New Part()

        part.Geometry = geometry
        part.GeoProfiles = geoProfiles
        part.Layers = layers.Values.ToList()

        Return part

    End Function

    Private Function BuildGeoProfiles(
    geometry As List(Of GeoEntity),
    gapTolerance As Double) As List(Of GeoProfile)

        Dim profiles As New List(Of GeoProfile)()

        Dim rejectedProfiles As New HashSet(Of GeoProfile)

        If geometry Is Nothing OrElse geometry.Count = 0 Then
            Return profiles
        End If

        For Each entity As GeoEntity In geometry

            If Not TypeOf entity Is GeoLine Then Continue For

            '    Debug.WriteLine(
            '"Line " & entity.ID &
            '" | In Branched Group: " &
            'IsInBranchedGroup(
            '    entity, geometry, gapTolerance))

        Next
        '-----------------------------------------
        ' Step 1 - Create an initial profile
        ' for each native line.
        '-----------------------------------------

        For Each entity As GeoEntity In geometry

            ' Only lines and arcs participate in profiles.
            If Not TypeOf entity Is GeoLine AndAlso
       Not TypeOf entity Is GeoArc Then

                Continue For

            End If

            If IsInBranchedGroup(
        entity, geometry, gapTolerance) Then

                Continue For

            End If

            Dim profile As New GeoProfile With {
            .ID = profiles.Count + 1,
            .IsClosed = False
        }
            'Debug.WriteLine("profile " & profile.ID)

            profile.Entities.Add(entity)
            profiles.Add(profile)

        Next


        '-----------------------------------------
        ' Step 2 - Merge compatible profiles.
        '
        ' Continue until no further merges occur.
        '-----------------------------------------

        Dim merged As Boolean

        Do

            merged = False

            Dim i As Integer = 0

            While i < profiles.Count

                Dim j As Integer = i + 1

                While j < profiles.Count

                    If profiles(i).Entities.Count = 7 AndAlso
                     profiles(j).Entities.Count = 1 Then

                        ' Debug.WriteLine(
                        '"FINAL MERGE CHECK: " &
                        '"Profile " & profiles(i).ID &
                        '" + Profile " & profiles(j).ID &
                        '" | SameAssociation=" &
                        'SameGeoAssociation(
                        '    profiles(i).Entities(0),
                        '    profiles(j).Entities(0)).ToString())

                    End If

                    Dim ends1 As List(Of GeoPoint) =
    GetProfileEndpoints(profiles(i), gapTolerance)

                    Dim ends2 As List(Of GeoPoint) =
    GetProfileEndpoints(profiles(j), gapTolerance)

                    '                Debug.WriteLine(
                    '"GapTolerance=" &
                    'gapTolerance.ToString("G17"))

                    For Each p1 As GeoPoint In ends1
                        For Each p2 As GeoPoint In ends2

                            Dim dx As Double = p1.X - p2.X
                            Dim dy As Double = p1.Y - p2.Y
                            Dim distance As Double = Math.Sqrt(dx * dx + dy * dy)

                            '                Debug.WriteLine(
                            '"ENDPOINT DISTANCE=" &
                            'distance.ToString("G17") &
                            '" | WithinGap=" &
                            'PointsWithinGap(p1, p2, gapTolerance).ToString())

                        Next
                    Next
                    If TryMergeGeoProfiles(
                profiles(i),
                profiles(j),
                gapTolerance) Then

                        profiles(i).Entities.AddRange(
                    profiles(j).Entities)

                        profiles.RemoveAt(j)

                        merged = True

                        Continue While

                    End If

                    j += 1

                End While

                i += 1

            End While

        Loop While merged


        '-----------------------------------------
        ' Step 3 - Identify closed profiles.
        '
        ' An open line chain has two free
        ' endpoints.
        '
        ' A closed line chain has none.
        '-----------------------------------------

        For Each profile As GeoProfile In profiles

            Dim endpoints As List(Of GeoPoint) =
            GetProfileEndpoints(profile, gapTolerance)

            profile.IsClosed =
    (profile.Entities.Count >= 2 AndAlso
     endpoints.Count = 0)

        Next

        For Each profile As GeoProfile In profiles
            Dim ordered As Boolean =
    OrderGeoProfileSegments(profile, gapTolerance)
            Dim valid As Boolean = False

            If ordered Then
                valid = ValidateGeoProfileSegments(profile, gapTolerance)
            End If

            profile.IsClosed = profile.IsClosed AndAlso ordered AndAlso valid

            If Not ordered OrElse Not valid Then
                rejectedProfiles.Add(profile)
                Continue For
            End If

            ' Reject closed profiles containing overlapping lines.

            '---------------------------------------------
            ' Validate closed profile geometry.
            '---------------------------------------------
            If profile.IsClosed Then

                If HasOverlappingProfileLines(
                        profile,
                        geometry,
                        0.000001) OrElse
                       HasCrossingProfileLines(
                        profile,
                        geometry,
                        0.000001) OrElse
                       HasCrossingProfileLineArcs(
                        profile,
                        geometry,
                        0.000001) OrElse
                       HasCrossingProfileArcs(
                        profile,
                        geometry,
                        0.000001) Then

                    profile.IsClosed = False
                    rejectedProfiles.Add(profile)

                End If

            End If


            '    Debug.WriteLine(
            '"GeoProfile " & profile.ID &
            '" | Ordered: " & ordered &
            '" | Connected: " & valid &
            '" | Entities: " & profile.Entities.Count &
            '" | Segments: " & profile.Segments.Count)

            If ordered AndAlso valid AndAlso profile.IsClosed Then

                Dim signedArea As Double =
            CalculateGeoProfileSignedArea(profile)

                profile.SignedArea = signedArea

                Dim direction As String = "Degenerate"

                If signedArea > 0 Then
                    direction = "CCW"
                ElseIf signedArea < 0 Then
                    direction = "CW"
                End If

                '    Debug.WriteLine(
                '"GeoProfile " & profile.ID &
                '" | Signed Area: " & signedArea &
                '" | Direction: " & direction)


                Dim bounds As RectangleF =
    GetGeoProfileBounds(profile)

                ''Debug.WriteLine(
                ''    "GeoProfile " & profile.ID &
                ''    " | Bounds X: " & bounds.X &
                ''    " | Y: " & bounds.Y &
                ''    " | Width: " & bounds.Width &
                ''    " | Height: " & bounds.Height)
            End If

        Next

        '-----------------------------------------
        ' Step 4 - Diagnostic output.
        '-----------------------------------------

        For Each profile As GeoProfile In profiles

            Dim endpoints As List(Of GeoPoint) =
            GetProfileEndpoints(profile, gapTolerance)

            'Debug.WriteLine(
            '"GeoProfile " & profile.ID &
            '" | Lines: " & profile.Entities.Count &
            '" | Free Endpoints: " & endpoints.Count &
            '" | IsClosed: " & profile.IsClosed)

            '        Debug.WriteLine(
            '"PROFILE " & profile.ID &
            '" | Entities=" & profile.Entities.Count &
            '" | Free Endpoints=" & endpoints.Count &
            '" | IsClosed=" & profile.IsClosed)

            For Each entity As GeoEntity In profile.Entities

                Dim p1 As GeoPoint = Nothing
                Dim p2 As GeoPoint = Nothing

                If GetGeoEntityEndpoints(entity, p1, p2) Then

                    'Debug.WriteLine(
                    '    "  Entity=" & entity.ID &
                    '    " | Type=" & entity.GetType().Name &
                    '    " | Start=(" & p1.X.ToString("F6") &
                    '    ", " & p1.Y.ToString("F6") & ")" &
                    '    " | End=(" & p2.X.ToString("F6") &
                    '    ", " & p2.Y.ToString("F6") & ")")

                End If

            Next
        Next


        '-----------------------------------------
        ' Step 5 - Build closed circle profiles
        '-----------------------------------------
        Dim nextProfileID As Integer =
    If(profiles.Count > 0,
       profiles.Max(Function(p) p.ID) + 1,
       1)

        For Each entity As GeoEntity In geometry

            If Not TypeOf entity Is GeoCircle Then
                Continue For
            End If

            Dim circle As GeoCircle = DirectCast(entity, GeoCircle)

            Dim profile As New GeoProfile With {
    .ID = nextProfileID,
    .IsClosed = True
}

            nextProfileID += 1


            ' Preserve the original circle entity.
            profile.Entities.Add(circle)

            ' A circle is inherently closed and requires
            ' no endpoint ordering.
            profile.Segments.Add(
        New GeoProfileSegment With {
            .Entity = circle,
            .Reversed = False
        })

            profiles.Add(profile)

            '    Debug.WriteLine(
            '"GeoCircle Profile " & profile.ID &
            '" | Closed: " & profile.IsClosed &
            '" | Entities: " & profile.Entities.Count &
            '" | Segments: " & profile.Segments.Count)

        Next

        ' Remove invalid profile groupings.
        ' Original GeoEntity objects remain in Part.Geometry.
        profiles.RemoveAll(
    Function(p) rejectedProfiles.Contains(p))

        '    ' Retain only profiles containing multiple entities.
        '    ' Single lines and arcs remain independent geometry.
        '    profiles.RemoveAll(
        'Function(p) p.Entities.Count < 2)
        profiles.RemoveAll(
    Function(p) p.Entities.Count < 2 AndAlso
                Not (p.Entities.Count = 1 AndAlso
                     TypeOf p.Entities(0) Is GeoCircle))

        For Each p As GeoProfile In profiles

            Debug.WriteLine(
        "PROFILE " & p.ID &
        " | Entities=" & p.Entities.Count &
        " | Segments=" & p.Segments.Count &
        " | Closed=" & p.IsClosed)

            For Each entity As GeoEntity In p.Entities
                Debug.WriteLine("    " & entity.GetType().Name)
            Next

        Next


        Return profiles

    End Function

    Private Function LinesShareEndpoint(
    line1 As GeoLine,
    line2 As GeoLine) As Boolean

        Return Object.ReferenceEquals(line1.StartPoint, line2.StartPoint) OrElse
           Object.ReferenceEquals(line1.StartPoint, line2.EndPoint) OrElse
           Object.ReferenceEquals(line1.EndPoint, line2.StartPoint) OrElse
           Object.ReferenceEquals(line1.EndPoint, line2.EndPoint)

    End Function

    Private Function GetProfileEndpoints(
    profile As GeoProfile,
    gapTolerance As Double) As List(Of GeoPoint)

        Dim allEndpoints As New List(Of GeoPoint)

        If profile Is Nothing Then
            Return allEndpoints
        End If

        For Each entity As GeoEntity In profile.Entities

            Dim startPoint As GeoPoint = Nothing
            Dim endPoint As GeoPoint = Nothing

            If GetGeoEntityEndpoints(
            entity, startPoint, endPoint) Then

                allEndpoints.Add(startPoint)
                allEndpoints.Add(endPoint)

            End If

        Next

        Dim freeEndpoints As New List(Of GeoPoint)

        For i As Integer = 0 To allEndpoints.Count - 1

            Dim connectionCount As Integer = 0

            For j As Integer = 0 To allEndpoints.Count - 1

                If i = j Then Continue For

                If PointsWithinGap(
                allEndpoints(i),
                allEndpoints(j),
                gapTolerance) Then

                    connectionCount += 1

                End If

            Next

            If connectionCount = 0 Then
                freeEndpoints.Add(allEndpoints(i))
            End If

        Next

        Return freeEndpoints

    End Function
    Private Function CanMergeProfiles(
    profile1 As GeoProfile,
    profile2 As GeoProfile,
    gapTolerance As Double) As Boolean

        If profile1 Is Nothing OrElse profile2 Is Nothing Then
            Return False
        End If

        If profile1.Entities.Count = 0 OrElse
       profile2.Entities.Count = 0 Then
            Return False
        End If

        If Not SameGeoAssociation(
        profile1.Entities(0),
        profile2.Entities(0)) Then

            Return False
        End If

        Dim endpoints1 As List(Of GeoPoint) =
        GetProfileEndpoints(profile1, gapTolerance)

        Dim endpoints2 As List(Of GeoPoint) =
        GetProfileEndpoints(profile2, gapTolerance)

        If endpoints1.Count <> 2 OrElse
       endpoints2.Count <> 2 Then

            Return False
        End If

        For Each p1 As GeoPoint In endpoints1

            For Each p2 As GeoPoint In endpoints2

                If PointsWithinGap(p1, p2, gapTolerance) Then
                    Return True
                End If

            Next

        Next

        Return False

    End Function


    Private Function TryMergeGeoProfiles(
    profile1 As GeoProfile,
    profile2 As GeoProfile,
    gapTolerance As Double) As Boolean

        If Not CanMergeProfiles(
    profile1,
    profile2,
    gapTolerance) Then
            Return False
        End If

        Dim endpoints1 As List(Of GeoPoint) =
        GetProfileEndpoints(profile1, gapTolerance)

        Dim endpoints2 As List(Of GeoPoint) =
        GetProfileEndpoints(profile2, gapTolerance)

        If endpoints1.Count <> 2 OrElse
       endpoints2.Count <> 2 Then
            Return False
        End If

        For Each point1 As GeoPoint In endpoints1

            For Each point2 As GeoPoint In endpoints2

                If PointsWithinGap(point1, point2, gapTolerance) Then
                    Return True
                End If


            Next

        Next

        Return False

    End Function



    Private Function SameGeoAssociation(
    entity1 As GeoEntity,
    entity2 As GeoEntity) As Boolean

        If entity1 Is Nothing OrElse
       entity2 Is Nothing Then
            Return False
        End If

        If entity1.Association Is Nothing OrElse
       entity2.Association Is Nothing Then
            Return False
        End If

        Return Object.ReferenceEquals(
        entity1.Association,
        entity2.Association)

    End Function


    Private Function FindLayerDirective(
    cadLayerName As String,
    layerSetup As AppData.layersetup) As AppData.Directive

        If layerSetup Is Nothing OrElse
           layerSetup.Layers Is Nothing Then

            Return Nothing
        End If

        Dim directive = layerSetup.Layers.FirstOrDefault(
            Function(d) String.Equals(
                d.CADLayer,
                cadLayerName,
                StringComparison.OrdinalIgnoreCase))

        If directive IsNot Nothing Then
            Return directive
        End If

        Return layerSetup.Layers.FirstOrDefault(
            Function(d) String.Equals(
                d.CADLayer,
                "*DEFAULT*",
                StringComparison.OrdinalIgnoreCase))

    End Function


    Public Function FindDirectiveByCAMLayer(
    camLayerName As String,
    layerSetup As AppData.layersetup) As AppData.Directive

        If layerSetup Is Nothing OrElse
       layerSetup.Layers Is Nothing Then
            Return Nothing
        End If

        Return layerSetup.Layers.FirstOrDefault(
        Function(d) String.Equals(
            d.CAMLayer,
            camLayerName,
            StringComparison.OrdinalIgnoreCase))

    End Function



    Private Function GetOrCreateLayer(
    camLayerName As String,
    layerColor As System.Drawing.Color,
    layers As Dictionary(Of String, Layer)) As Layer

        Dim existingLayer As Layer = Nothing

        If layers.TryGetValue(camLayerName, existingLayer) Then
            Return existingLayer
        End If

        Dim newLayer As New Layer With {
            .ID = layers.Count + 1,
            .Name = camLayerName,
            .Color = layerColor
        }

        layers.Add(camLayerName, newLayer)

        Return newLayer

    End Function

    Private Function GetOrCreatePoint(x As Double, y As Double, points As Dictionary(Of (Double, Double), GeoPoint)) As GeoPoint

        Dim key = (x, y)
        Dim existingPoint As GeoPoint = Nothing

        If points.TryGetValue(key, existingPoint) Then
            Return existingPoint
        End If

        Dim newPoint As New GeoPoint With {.X = x, .Y = y}

        points.Add(key, newPoint)

        Return newPoint

    End Function

    Private Function ConvertCadLine(
    cad As CadEntity.CadLine,
    layer As Layer,
    points As Dictionary(Of (Double, Double), GeoPoint)) As GeoLine

        Dim startPoint As GeoPoint =
            GetOrCreatePoint(
                CDbl(cad.StartPoint.X),
                CDbl(cad.StartPoint.Y),
                points)

        Dim endPoint As GeoPoint =
            GetOrCreatePoint(
                CDbl(cad.EndPoint.X),
                CDbl(cad.EndPoint.Y),
                points)

        Dim line As New GeoLine With {
            .StartPoint = startPoint,
            .EndPoint = endPoint,
            .Association = layer
        }

        Return line

    End Function

    Private Function ConvertCadCircle(
    cad As CadEntity.CadCircle,
    layer As Layer,
    points As Dictionary(Of (Double, Double), GeoPoint)) As GeoCircle

        Dim centerPoint As GeoPoint =
        GetOrCreatePoint(
            CDbl(cad.Center.X),
            CDbl(cad.Center.Y),
            points)

        Dim circle As New GeoCircle With {
        .Center = centerPoint,
        .Radius = cad.Radius,
        .Association = layer
    }

        Return circle

    End Function

    Private Function ConvertCadArc(
    cad As CadEntity.CadArc, layer As Layer, points As Dictionary(Of (Double, Double), GeoPoint)) As GeoArc

        Dim startRadians As Double = CDbl(cad.StartAngle) * Math.PI / 180.0

        '' Calculate the counterclockwise angular sweep.
        'Dim rawSweep As Double = CDbl(cad.EndAngle) - CDbl(cad.StartAngle)

        'Dim sweep As Double = ((rawSweep Mod 360.0) + 360.0) Mod 360.0

        Dim endRadians As Double = CDbl(cad.EndAngle) * Math.PI / 180.0

        Dim centerX As Double = CDbl(cad.Center.X)
        Dim centerY As Double = CDbl(cad.Center.Y)
        Dim radius As Double = CDbl(cad.Radius)

        Dim centerPoint As GeoPoint = GetOrCreatePoint(centerX, centerY, points)

        Dim startPoint As GeoPoint = GetOrCreatePoint(centerX + radius * Math.Cos(startRadians), centerY + radius * Math.Sin(startRadians), points)

        Dim endPoint As GeoPoint = GetOrCreatePoint(centerX + radius * Math.Cos(endRadians), centerY + radius * Math.Sin(endRadians), points)

        Dim arc As New GeoArc With {.Center = centerPoint, .Radius = radius,
            .StartAngle = CDbl(cad.StartAngle), .EndAngle = CDbl(cad.EndAngle),
            .IsClockwise = cad.IsClockwise, .StartPoint = startPoint, .EndPoint = endPoint, .Association = layer}

        Return arc

    End Function


    Private Function GetSegmentEndPoint(
    segment As GeoProfileSegment) As GeoPoint

        If segment Is Nothing OrElse segment.Entity Is Nothing Then
            Return Nothing
        End If

        If TypeOf segment.Entity Is GeoLine Then

            Dim line As GeoLine =
            DirectCast(segment.Entity, GeoLine)

            If segment.Reversed Then
                Return line.StartPoint
            End If

            Return line.EndPoint

        ElseIf TypeOf segment.Entity Is GeoArc Then

            Dim arc As GeoArc =
            DirectCast(segment.Entity, GeoArc)

            If segment.Reversed Then
                Return arc.StartPoint
            End If

            Return arc.EndPoint

        End If

        Return Nothing

    End Function

    Private Function GetSegmentStartPoint(
    segment As GeoProfileSegment) As GeoPoint

        If segment Is Nothing OrElse segment.Entity Is Nothing Then
            Return Nothing
        End If

        If TypeOf segment.Entity Is GeoLine Then

            Dim line As GeoLine =
            DirectCast(segment.Entity, GeoLine)

            If segment.Reversed Then
                Return line.EndPoint
            End If

            Return line.StartPoint

        ElseIf TypeOf segment.Entity Is GeoArc Then

            Dim arc As GeoArc =
            DirectCast(segment.Entity, GeoArc)

            If segment.Reversed Then
                Return arc.EndPoint
            End If

            Return arc.StartPoint

        End If

        Return Nothing

    End Function

    Private Function OrderGeoProfileSegments(
    profile As GeoProfile,
    gapTolerance As Double) As Boolean

        If profile Is Nothing OrElse
       profile.Entities.Count = 0 Then

            Return False

        End If

        profile.Segments.Clear()

        '-----------------------------------------
        ' Validate that all entities have endpoints.
        ' Supports both GeoLine and GeoArc.
        '-----------------------------------------

        For Each entity As GeoEntity In profile.Entities

            Dim p1 As GeoPoint = Nothing
            Dim p2 As GeoPoint = Nothing

            If Not GetGeoEntityEndpoints(
            entity, p1, p2) Then

                Return False

            End If

            If p1 Is Nothing OrElse p2 Is Nothing Then
                Return False
            End If

        Next

        '-----------------------------------------
        ' Select the starting entity.
        '-----------------------------------------

        Dim startEntity As GeoEntity = profile.Entities(0)
        Dim reverseFirst As Boolean = False

        If Not profile.IsClosed Then

            Dim endpoints As List(Of GeoPoint) =
            GetProfileEndpoints(profile, gapTolerance)

            'Debug.WriteLine(
            '"ORDER CHECK: Free Endpoints = " &
            'endpoints.Count)

            If endpoints.Count <> 2 Then
                Return False
            End If

            Dim startPoint As GeoPoint = endpoints(0)
            Dim foundStart As Boolean = False

            For Each entity As GeoEntity In profile.Entities

                Dim entityStart As GeoPoint = Nothing
                Dim entityEnd As GeoPoint = Nothing

                GetGeoEntityEndpoints(
                entity, entityStart, entityEnd)

                If PointsWithinGap(
                entityStart,
                startPoint,
                gapTolerance) Then

                    startEntity = entity
                    reverseFirst = False
                    foundStart = True
                    Exit For

                ElseIf PointsWithinGap(
                entityEnd,
                startPoint,
                gapTolerance) Then

                    startEntity = entity
                    reverseFirst = True
                    foundStart = True
                    Exit For

                End If

            Next

            If Not foundStart Then
                Return False
            End If

        End If

        '-----------------------------------------
        ' Add the starting segment.
        '-----------------------------------------

        profile.Segments.Add(
        New GeoProfileSegment With {
            .Entity = startEntity,
            .Reversed = reverseFirst
        })

        Dim remaining As New List(Of GeoEntity)(
        profile.Entities)

        remaining.Remove(startEntity)

        '-----------------------------------------
        ' Order all remaining entities.
        ' Supports GeoLine and GeoArc.
        '-----------------------------------------

        While remaining.Count > 0

            Dim currentPoint As GeoPoint =
            GetSegmentEndPoint(
                profile.Segments(
                    profile.Segments.Count - 1))

            Dim nextIndex As Integer = -1
            Dim reverseNext As Boolean = False

            For i As Integer = 0 To remaining.Count - 1

                Dim entity As GeoEntity = remaining(i)

                Dim entityStart As GeoPoint = Nothing
                Dim entityEnd As GeoPoint = Nothing

                If Not GetGeoEntityEndpoints(
                entity,
                entityStart,
                entityEnd) Then

                    Continue For

                End If

                If PointsWithinGap(
                entityStart,
                currentPoint,
                gapTolerance) Then

                    nextIndex = i
                    reverseNext = False
                    Exit For

                ElseIf PointsWithinGap(
                entityEnd,
                currentPoint,
                gapTolerance) Then

                    nextIndex = i
                    reverseNext = True
                    Exit For

                End If

            Next

            '-----------------------------------------
            ' Report an ordering failure.
            '-----------------------------------------

            If nextIndex = -1 Then

                For Each remainingEntity As GeoEntity In remaining

                    Dim p1 As GeoPoint = Nothing
                    Dim p2 As GeoPoint = Nothing

                    If GetGeoEntityEndpoints(
                    remainingEntity, p1, p2) Then

                        'Debug.WriteLine(
                        '"REMAINING ENTITY: " &
                        'remainingEntity.ID &
                        '" | Type: " &
                        'remainingEntity.GetType().Name &
                        '" | Start: (" &
                        'p1.X & ", " & p1.Y & ")" &
                        '" | End: (" &
                        'p2.X & ", " & p2.Y & ")")

                    End If

                Next

                'Debug.WriteLine(
                '"ORDER FAILED | Profile ID: " &
                'profile.ID &
                '" | Ordered Segments: " &
                'profile.Segments.Count &
                '" | Remaining Entities: " &
                'remaining.Count &
                '" | Current Point: (" &
                'currentPoint.X & ", " &
                'currentPoint.Y & ")" &
                '" | Gap Tolerance: " &
                'gapTolerance)

                profile.Segments.Clear()
                Return False

            End If

            '-----------------------------------------
            ' Add the next segment.
            '-----------------------------------------

            profile.Segments.Add(
            New GeoProfileSegment With {
                .Entity = remaining(nextIndex),
                .Reversed = reverseNext
            })

            remaining.RemoveAt(nextIndex)

        End While

        Return True

    End Function



    Private Function ValidateGeoProfileSegments(
    profile As GeoProfile,
    gapTolerance As Double) As Boolean

        If profile Is Nothing OrElse
           profile.Segments.Count = 0 Then
            Return False
        End If

        If profile.Segments.Count <> profile.Entities.Count Then
            Return False
        End If

        For i As Integer = 0 To profile.Segments.Count - 2

            Dim endPoint As GeoPoint =
                GetSegmentEndPoint(profile.Segments(i))

            Dim nextStart As GeoPoint =
                GetSegmentStartPoint(profile.Segments(i + 1))

            If Not PointsWithinGap(
    endPoint, nextStart, gapTolerance) Then

                Return False
            End If

        Next

        If profile.IsClosed Then

            Dim lastEnd As GeoPoint =
                GetSegmentEndPoint(
                    profile.Segments(profile.Segments.Count - 1))

            Dim firstStart As GeoPoint =
                GetSegmentStartPoint(profile.Segments(0))

            If Not PointsWithinGap(lastEnd, firstStart, gapTolerance) Then

                Return False

            End If

        End If

        Return True

    End Function

    Private Function CalculateGeoProfileSignedArea(
    profile As GeoProfile) As Double

        If profile Is Nothing OrElse
       Not profile.IsClosed OrElse
       profile.Segments.Count = 0 Then

            Return 0.0

        End If

        Const TwoPi As Double = Math.PI * 2.0

        Dim area As Double = 0.0

        For Each segment As GeoProfileSegment In profile.Segments

            If segment Is Nothing OrElse
           segment.Entity Is Nothing Then

                Return 0.0

            End If

            ' Complete circle
            If TypeOf segment.Entity Is GeoCircle Then

                Dim circle As GeoCircle =
                DirectCast(segment.Entity, GeoCircle)

                If circle.Center Is Nothing OrElse
               circle.Radius <= 0.0 Then

                    Return 0.0

                End If

                Dim circleArea As Double =
                Math.PI * circle.Radius * circle.Radius

                area += If(segment.Reversed,
                       -circleArea,
                       circleArea)

                Continue For

            End If

            ' Only lines and arcs are supported here.
            If Not TypeOf segment.Entity Is GeoLine AndAlso
           Not TypeOf segment.Entity Is GeoArc Then

                Return 0.0

            End If

            Dim startPt As GeoPoint =
            GetSegmentStartPoint(segment)

            Dim endPt As GeoPoint =
            GetSegmentEndPoint(segment)

            If startPt Is Nothing OrElse
           endPt Is Nothing Then

                Return 0.0

            End If

            ' Straight-line or arc chord contribution
            area += 0.5 *
            ((CDbl(startPt.X) * CDbl(endPt.Y)) -
             (CDbl(endPt.X) * CDbl(startPt.Y)))

            ' Additional circular segment contribution
            If TypeOf segment.Entity Is GeoArc Then

                Dim arc As GeoArc =
                DirectCast(segment.Entity, GeoArc)

                If arc.Center Is Nothing OrElse
               arc.Radius <= 0.0 Then

                    Return 0.0

                End If

                Dim startAngle As Double =
                arc.StartAngle * Math.PI / 180.0

                Dim endAngle As Double =
                arc.EndAngle * Math.PI / 180.0

                Dim clockwise As Boolean =
                arc.IsClockwise

                If segment.Reversed Then

                    Dim temp As Double = startAngle

                    startAngle = endAngle
                    endAngle = temp

                    clockwise = Not clockwise

                End If

                Dim direction As Double =
                If(clockwise, -1.0, 1.0)

                Dim sweep As Double

                If Math.Abs(endAngle - startAngle) >=
               TwoPi - 0.000001 Then

                    sweep = TwoPi

                Else

                    sweep =
                    direction * (endAngle - startAngle)

                    sweep = sweep Mod TwoPi

                    If sweep < 0.0 Then
                        sweep += TwoPi
                    End If

                End If

                Dim signedSweep As Double =
                direction * sweep

                area +=
                0.5 * arc.Radius * arc.Radius *
                (signedSweep - Math.Sin(signedSweep))

            End If

        Next

        Return area

    End Function
    Private Function GetGeoProfileBounds(
    profile As GeoProfile) As RectangleF

        If profile Is Nothing OrElse
       profile.Segments.Count = 0 Then

            Return RectangleF.Empty

        End If

        Dim path As ToolShapes.IPath2D =
        ConvertGeoProfileToPath(
            profile,
            requireClosed:=False)

        If path Is Nothing Then
            Return RectangleF.Empty
        End If

        Return ToolShapes.Geometry2D.ComputeBoundingBox(
        path.Segments)

    End Function


    Private Function TryGetReliableProfileBounds(
        profile As GeoProfile,
        ByRef bounds As (
            MinX As Double,
            MinY As Double,
            MaxX As Double,
            MaxY As Double)) As Boolean

        bounds = (0.0, 0.0, 0.0, 0.0)

        If profile Is Nothing Then Return False

        If profile.Entities Is Nothing OrElse
       profile.Entities.Count = 0 Then Return False

        Dim minX As Double = Double.PositiveInfinity
        Dim minY As Double = Double.PositiveInfinity
        Dim maxX As Double = Double.NegativeInfinity
        Dim maxY As Double = Double.NegativeInfinity

        Dim isFinite As Func(Of Double, Boolean) =
        Function(value)
            Return Not Double.IsNaN(value) AndAlso
                   Not Double.IsInfinity(value)
        End Function

        Dim includePoint As Func(Of Double, Double, Boolean) =
        Function(x, y)

            If Not isFinite(x) OrElse
               Not isFinite(y) Then Return False

            minX = Math.Min(minX, x)
            minY = Math.Min(minY, y)
            maxX = Math.Max(maxX, x)
            maxY = Math.Max(maxY, y)

            Return True

        End Function

        For Each entity As GeoEntity In profile.Entities

            If entity Is Nothing Then Return False

            If TypeOf entity Is GeoLine Then

                Dim line As GeoLine =
                DirectCast(entity, GeoLine)

                If line.StartPoint Is Nothing OrElse
               line.EndPoint Is Nothing Then Return False

                If Not includePoint(
                line.StartPoint.X,
                line.StartPoint.Y) Then Return False

                If Not includePoint(
                line.EndPoint.X,
                line.EndPoint.Y) Then Return False

            ElseIf TypeOf entity Is GeoCircle Then

                Dim circle As GeoCircle =
                DirectCast(entity, GeoCircle)

                If circle.Center Is Nothing Then Return False

                If Not isFinite(circle.Radius) OrElse
               circle.Radius <= 0.0 Then Return False

                Dim radius As Double = circle.Radius

                If Not includePoint(
                circle.Center.X - radius,
                circle.Center.Y - radius) Then Return False

                If Not includePoint(
                circle.Center.X + radius,
                circle.Center.Y + radius) Then Return False

            ElseIf TypeOf entity Is GeoArc Then

                Dim arc As GeoArc =
                DirectCast(entity, GeoArc)

                If arc.Center Is Nothing OrElse
               arc.StartPoint Is Nothing OrElse
               arc.EndPoint Is Nothing Then Return False

                If Not isFinite(arc.Radius) OrElse
               arc.Radius <= 0.0 Then Return False

                If Not isFinite(arc.StartAngle) OrElse
               Not isFinite(arc.EndAngle) Then Return False

                Dim radius As Double = arc.Radius

                ' Use conservative full-circle bounds.
                ' Do not depend on the arc sweep.

                If Not includePoint(
                arc.Center.X - radius,
                arc.Center.Y - radius) Then Return False

                If Not includePoint(
                arc.Center.X + radius,
                arc.Center.Y + radius) Then Return False

                ' Include actual endpoints in case they
                ' disagree with the mathematical arc.

                If Not includePoint(
                arc.StartPoint.X,
                arc.StartPoint.Y) Then Return False

                If Not includePoint(
                arc.EndPoint.X,
                arc.EndPoint.Y) Then Return False

            Else

                ' Unsupported boundary geometry.
                Return False

            End If

        Next

        Dim width As Double = maxX - minX
        Dim height As Double = maxY - minY

        If Not isFinite(width) OrElse
       Not isFinite(height) Then Return False

        If width < 0.0 OrElse
       height < 0.0 Then Return False

        bounds = (minX, minY, maxX, maxY)

        Return True

    End Function

    Private Function PointsWithinGap(
    point1 As GeoPoint,
    point2 As GeoPoint,
    gapTolerance As Double) As Boolean

        If point1 Is Nothing OrElse point2 Is Nothing Then
            Return False
        End If

        If gapTolerance < 0 Then
            Return False
        End If

        Dim dx As Double = point1.X - point2.X
        Dim dy As Double = point1.Y - point2.Y

        Dim distanceSquared As Double =
            dx * dx + dy * dy

        Dim effectiveTolerance As Double =
    Math.Max(gapTolerance, 0.000001R)

        Return distanceSquared <=
    effectiveTolerance * effectiveTolerance

    End Function


    Private Function GetGeoEntityEndpoints(
    entity As GeoEntity,
    ByRef startPoint As GeoPoint,
    ByRef endPoint As GeoPoint) As Boolean

        startPoint = Nothing
        endPoint = Nothing

        If TypeOf entity Is GeoLine Then

            Dim line As GeoLine = DirectCast(entity, GeoLine)

            startPoint = line.StartPoint
            endPoint = line.EndPoint

        ElseIf TypeOf entity Is GeoArc Then

            Dim arc As GeoArc = DirectCast(entity, GeoArc)

            startPoint = arc.StartPoint
            endPoint = arc.EndPoint

        Else
            Return False
        End If

        Return startPoint IsNot Nothing AndAlso
           endPoint IsNot Nothing

    End Function


    Private Function FindNextGeoCurve(
    currentPoint As GeoPoint,
    remaining As List(Of GeoEntity),
    gapTolerance As Double,
    ByRef reverseNext As Boolean) As GeoEntity

        reverseNext = False

        If currentPoint Is Nothing OrElse
       remaining Is Nothing Then
            Return Nothing
        End If

        Dim bestEntity As GeoEntity = Nothing
        Dim bestDistanceSquared As Double = Double.MaxValue

        For Each entity As GeoEntity In remaining

            Dim startPoint As GeoPoint = Nothing
            Dim endPoint As GeoPoint = Nothing

            If Not GetGeoEntityEndpoints(
            entity, startPoint, endPoint) Then
                Continue For
            End If

            Dim candidatePoints As GeoPoint() = {
            startPoint, endPoint
        }

            For endpointIndex As Integer = 0 To 1

                Dim pt As GeoPoint = candidatePoints(endpointIndex)

                If Not PointsWithinGap(
                currentPoint, pt, gapTolerance) Then
                    Continue For
                End If

                Dim dx As Double = currentPoint.X - pt.X
                Dim dy As Double = currentPoint.Y - pt.Y

                Dim distanceSquared As Double =
                dx * dx + dy * dy

                If distanceSquared < bestDistanceSquared Then

                    bestDistanceSquared = distanceSquared
                    bestEntity = entity

                    reverseNext = (endpointIndex = 1)

                End If

            Next

        Next

        Return bestEntity

    End Function

    Private Function IsThreeWayJunction(
    targetPoint As GeoPoint,
    geometry As List(Of GeoEntity),
    sourceEntity As GeoEntity,
    gapTolerance As Double) As Boolean

        If targetPoint Is Nothing OrElse
           geometry Is Nothing OrElse
           sourceEntity Is Nothing Then Return False

        Dim connectionCount As Integer = 0

        For Each entity As GeoEntity In geometry

            If Not TypeOf entity Is GeoLine AndAlso
                 Not TypeOf entity Is GeoArc Then

                Continue For

            End If

            If Not SameGeoAssociation(sourceEntity, entity) Then
                Continue For
            End If

            Dim startPoint As GeoPoint = Nothing
            Dim endPoint As GeoPoint = Nothing

            If Not GetGeoEntityEndpoints(
                entity, startPoint, endPoint) Then Continue For

            If PointsWithinGap(targetPoint, startPoint, gapTolerance) OrElse
               PointsWithinGap(targetPoint, endPoint, gapTolerance) Then

                connectionCount += 1

                If connectionCount >= 3 Then Return True
            End If

        Next

        Return False

    End Function


    Private Sub ApplyGeoFilterTolerance(
    geometry As List(Of GeoEntity),
    filterTolerance As Double)

        If geometry Is Nothing OrElse filterTolerance <= 0 Then
            Return
        End If

        For i As Integer = geometry.Count - 1 To 0 Step -1

            If Not TypeOf geometry(i) Is GeoLine Then
                Continue For
            End If

            Dim line As GeoLine =
                DirectCast(geometry(i), GeoLine)

            Dim dx As Double =
                line.EndPoint.X - line.StartPoint.X

            Dim dy As Double =
                line.EndPoint.Y - line.StartPoint.Y

            Dim length As Double =
                Math.Sqrt(dx * dx + dy * dy)

            If length = 0 OrElse
    (filterTolerance > 0 AndAlso length < filterTolerance) Then

                'Debug.WriteLine(
                '    "FILTER REMOVED LINE: Length = " &
                '    length.ToString("F6"))

                geometry.RemoveAt(i)

            End If

        Next

    End Sub

    Private Function IsInBranchedGroup(
    startEntity As GeoEntity,
    geometry As List(Of GeoEntity),
    gapTolerance As Double) As Boolean

        If startEntity Is Nothing OrElse geometry Is Nothing Then
            Return False
        End If

        Dim visited As New HashSet(Of GeoEntity)()
        Dim pending As New Queue(Of GeoEntity)()

        pending.Enqueue(startEntity)
        visited.Add(startEntity)

        While pending.Count > 0

            Dim current As GeoEntity = pending.Dequeue()

            Dim currentStart As GeoPoint = Nothing
            Dim currentEnd As GeoPoint = Nothing

            If Not GetGeoEntityEndpoints(
                current, currentStart, currentEnd) Then Continue While

            If IsThreeWayJunction(
                currentStart, geometry, current, gapTolerance) OrElse
               IsThreeWayJunction(
                currentEnd, geometry, current, gapTolerance) Then

                Return True

            End If

            For Each candidate As GeoEntity In geometry

                If visited.Contains(candidate) Then Continue For

                If Not TypeOf candidate Is GeoLine AndAlso
                         Not TypeOf candidate Is GeoArc Then

                    Continue For

                End If

                Dim candidateStart As GeoPoint = Nothing
                Dim candidateEnd As GeoPoint = Nothing

                If Not GetGeoEntityEndpoints(
                    candidate, candidateStart, candidateEnd) Then Continue For

                If PointsWithinGap(currentStart, candidateStart, gapTolerance) OrElse
                   PointsWithinGap(currentStart, candidateEnd, gapTolerance) OrElse
                   PointsWithinGap(currentEnd, candidateStart, gapTolerance) OrElse
                   PointsWithinGap(currentEnd, candidateEnd, gapTolerance) Then

                    visited.Add(candidate)
                    pending.Enqueue(candidate)

                End If

            Next

        End While

        Return False

    End Function


    Private Sub ApplyGeoCleanTolerance(
    geometry As List(Of GeoEntity),
    cleanTol As Double)

        '    Debug.WriteLine("========== ENTERING GEO CLEAN ==========")

        '    Debug.WriteLine(
        '"Geometry Count = " &
        'If(geometry Is Nothing, -1, geometry.Count))

        '    Debug.WriteLine(
        '"Clean Tolerance = " & cleanTol)


        If geometry Is Nothing OrElse
           geometry.Count < 2 OrElse
           cleanTol <= 0 Then Return

        Dim lines As List(Of GeoLine) =
            geometry.OfType(Of GeoLine)().ToList()

        Dim usedPoints As New HashSet(Of GeoPoint)()

        For i As Integer = 0 To lines.Count - 2

            Dim line1 As GeoLine = lines(i)

            For j As Integer = i + 1 To lines.Count - 1

                Dim line2 As GeoLine = lines(j)

                '            Debug.WriteLine(
                '"CLEAN ASSOCIATION CHECK: " &
                'SameGeoAssociation(line1, line2).ToString())


                If Not SameGeoAssociation(line1, line2) Then
                    Continue For
                End If

                Dim points1 As GeoPoint() = {
                    line1.StartPoint, line1.EndPoint
                }

                Dim points2 As GeoPoint() = {
                    line2.StartPoint, line2.EndPoint
                }

                For Each p1 As GeoPoint In points1
                    For Each p2 As GeoPoint In points2

                        '                    Debug.WriteLine(
                        '"CLEAN POINT CHECK: " &
                        '"P1 Nothing=" & (p1 Is Nothing).ToString() &
                        '" | P2 Nothing=" & (p2 Is Nothing).ToString() &
                        '" | Same Reference=" & Object.ReferenceEquals(p1, p2).ToString() &
                        '" | P1 Used=" & usedPoints.Contains(p1).ToString() &
                        '" | P2 Used=" & usedPoints.Contains(p2).ToString())

                        If p1 Is Nothing OrElse p2 Is Nothing Then
                            Continue For
                        End If

                        If Object.ReferenceEquals(p1, p2) Then
                            Continue For
                        End If

                        If usedPoints.Contains(p1) OrElse
                           usedPoints.Contains(p2) Then
                            Continue For
                        End If

                        Dim dx As Double = p1.X - p2.X
                        Dim dy As Double = p1.Y - p2.Y

                        Dim distance As Double =
                            Math.Sqrt(dx * dx + dy * dy)

                        '                    Debug.WriteLine(
                        '"CLEAN DISTANCE: " &
                        'distance.ToString("F6") &
                        '" | Tolerance: " &
                        'cleanTol.ToString("F6"))

                        If distance >= cleanTol Then
                            Continue For
                        End If

                        Dim midX As Double = (p1.X + p2.X) / 2.0
                        Dim midY As Double = (p1.Y + p2.Y) / 2.0

                        p1.X = midX
                        p1.Y = midY

                        p2.X = midX
                        p2.Y = midY

                        usedPoints.Add(p1)
                        usedPoints.Add(p2)

                        'Debug.WriteLine(
                        '    "CLEAN Line-Line: " &
                        '    distance.ToString("F6") &
                        '    " → Midpoint (" &
                        '    midX.ToString("F6") & ", " &
                        '    midY.ToString("F6") & ")")

                    Next
                Next

            Next

        Next

    End Sub
    Public Function ConvertGeoProfileToPath(
    profile As GeoProfile,
    Optional requireClosed As Boolean = True) As ToolShapes.IPath2D

        If profile Is Nothing Then
            Return Nothing
        End If

        If requireClosed AndAlso Not profile.IsClosed Then
            Return Nothing
        End If

        Dim path As New ToolShapes.Path2D()

        For Each segment As GeoProfileSegment In profile.Segments

            If segment Is Nothing OrElse segment.Entity Is Nothing Then
                Return Nothing
            End If

            If TypeOf segment.Entity Is GeoLine Then

                Dim line As GeoLine =
                    DirectCast(segment.Entity, GeoLine)

                If line.StartPoint Is Nothing OrElse
                   line.EndPoint Is Nothing Then
                    Return Nothing
                End If

                Dim startPt As GeoPoint =
                    If(segment.Reversed, line.EndPoint, line.StartPoint)

                Dim endPt As GeoPoint =
                    If(segment.Reversed, line.StartPoint, line.EndPoint)

                path.Add(
                    New ToolShapes.LineSegment2D(
                        New PointF(CSng(startPt.X), CSng(startPt.Y)),
                        New PointF(CSng(endPt.X), CSng(endPt.Y))))

            ElseIf TypeOf segment.Entity Is GeoArc Then

                Dim arc As GeoArc =
                    DirectCast(segment.Entity, GeoArc)

                If arc.Center Is Nothing OrElse arc.Radius <= 0 Then
                    Return Nothing
                End If

                Dim startAngle As Double =
                    arc.StartAngle * Math.PI / 180.0

                Dim endAngle As Double =
                    arc.EndAngle * Math.PI / 180.0

                Dim clockwise As Boolean = arc.IsClockwise

                If segment.Reversed Then

                    Dim temp As Double = startAngle
                    startAngle = endAngle
                    endAngle = temp

                    clockwise = Not clockwise

                End If

                path.Add(
                    New ToolShapes.ArcSegment2D(
                        New PointF(
                            CSng(arc.Center.X),
                            CSng(arc.Center.Y)),
                        arc.Radius,
                        startAngle,
                        endAngle,
                        clockwise))

            ElseIf TypeOf segment.Entity Is GeoCircle Then

                Dim circle As GeoCircle =
        DirectCast(segment.Entity, GeoCircle)

                If circle.Center Is Nothing OrElse circle.Radius <= 0 Then
                    Return Nothing
                End If

                Dim startAngle As Double =
        If(segment.Reversed, Math.PI * 2.0, 0.0)

                Dim endAngle As Double =
        If(segment.Reversed, 0.0, Math.PI * 2.0)

                path.Add(
        New ToolShapes.ArcSegment2D(
            New PointF(
                CSng(circle.Center.X),
                CSng(circle.Center.Y)),
            circle.Radius,
            startAngle,
            endAngle,
            segment.Reversed))

            Else

                Return Nothing

            End If

        Next

        Return path

    End Function

    Private Function CreateContainmentPath(
    profile As GeoProfile,
    gapTolerance As Double) As ToolShapes.IPath2D

        If profile Is Nothing OrElse
       Not profile.IsClosed Then

            Return Nothing

        End If

        Dim sourcePath As ToolShapes.IPath2D =
        ConvertGeoProfileToPath(profile)

        If sourcePath Is Nothing Then
            Return Nothing
        End If

        Dim segments As List(Of ToolShapes.IPathSegment) =
        sourcePath.Segments.ToList()

        If segments.Count = 0 Then
            Return Nothing
        End If

        If Double.IsNaN(gapTolerance) OrElse
       Double.IsInfinity(gapTolerance) Then

            Return Nothing

        End If

        Dim tolerance As Double =
        Math.Max(gapTolerance, 0.000001)

        Dim containmentPath As New ToolShapes.Path2D()

        For i As Integer = 0 To segments.Count - 1

            Dim current As ToolShapes.IPathSegment =
            segments(i)

            Dim following As ToolShapes.IPathSegment =
            segments((i + 1) Mod segments.Count)

            containmentPath.Add(current)

            Dim endPoint As PointF = current.EndPoint
            Dim nextStart As PointF = following.StartPoint

            Dim dx As Double =
            CDbl(nextStart.X) - CDbl(endPoint.X)

            Dim dy As Double =
            CDbl(nextStart.Y) - CDbl(endPoint.Y)

            Dim distance As Double =
            Math.Sqrt(dx * dx + dy * dy)

            If Double.IsNaN(distance) OrElse
           Double.IsInfinity(distance) Then

                Return Nothing

            End If

            If distance > tolerance Then
                Return Nothing
            End If

            ' Bridge accepted gaps for analytical use only.
            If distance > 0.0 Then

                containmentPath.Add(
                New ToolShapes.LineSegment2D(
                    endPoint,
                    nextStart))

            End If

        Next

        If Not containmentPath.IsClosed Then
            Return Nothing
        End If

        Return containmentPath

    End Function

    Public Function ShouldReverseProfileDirection(
    profile As GeoProfile,
    isInsideProfile As Boolean) As Boolean

        If profile Is Nothing OrElse Not profile.IsClosed Then
            Return False
        End If

        If Math.Abs(profile.SignedArea) < 0.000001 Then
            Return False
        End If

        Dim currentlyClockwise As Boolean =
            profile.SignedArea < 0.0

        Dim requiredClockwise As Boolean =
            Not isInsideProfile

        Return currentlyClockwise <> requiredClockwise

    End Function


    Private Function DoLineSegmentsOverlap(
    lineA As LineSegment2D,
    lineB As LineSegment2D,
    tolerance As Double) As Boolean

        If lineA Is Nothing OrElse lineB Is Nothing Then
            Return False
        End If

        Dim ax As Double = CDbl(lineA.EndPoint.X) - lineA.StartPoint.X
        Dim ay As Double = CDbl(lineA.EndPoint.Y) - lineA.StartPoint.Y

        Dim bx As Double = CDbl(lineB.EndPoint.X) - lineB.StartPoint.X
        Dim by As Double = CDbl(lineB.EndPoint.Y) - lineB.StartPoint.Y

        Dim lengthA As Double = Math.Sqrt(ax * ax + ay * ay)
        Dim lengthB As Double = Math.Sqrt(bx * bx + by * by)

        If lengthA <= tolerance OrElse lengthB <= tolerance Then
            Return False
        End If

        ' Check whether the directions are parallel.
        Dim cross As Double = ax * by - ay * bx

        If Math.Abs(cross) > tolerance * lengthA * lengthB Then
            Return False
        End If

        ' Check whether B lies on the same infinite line as A.
        Dim cx As Double = CDbl(lineB.StartPoint.X) - lineA.StartPoint.X
        Dim cy As Double = CDbl(lineB.StartPoint.Y) - lineA.StartPoint.Y

        Dim distance As Double =
        Math.Abs(cx * ay - cy * ax) / lengthA

        If distance > tolerance Then
            Return False
        End If

        ' Project both segments onto A's unit direction.
        Dim ux As Double = ax / lengthA
        Dim uy As Double = ay / lengthA

        Dim bStart As Double = cx * ux + cy * uy

        Dim dx As Double = CDbl(lineB.EndPoint.X) - lineA.StartPoint.X
        Dim dy As Double = CDbl(lineB.EndPoint.Y) - lineA.StartPoint.Y

        Dim bEnd As Double = dx * ux + dy * uy

        Dim overlapStart As Double = Math.Max(0.0, Math.Min(bStart, bEnd))
        Dim overlapEnd As Double = Math.Min(lengthA, Math.Max(bStart, bEnd))

        ' A shared endpoint alone is not an overlap.
        Return (overlapEnd - overlapStart) > tolerance

    End Function


    Private Function DoLineSegmentsCross(
    lineA As LineSegment2D,
    lineB As LineSegment2D,
    tolerance As Double) As Boolean

        If lineA Is Nothing OrElse lineB Is Nothing Then
            Return False
        End If

        Dim ax As Double =
        CDbl(lineA.EndPoint.X) - lineA.StartPoint.X
        Dim ay As Double =
        CDbl(lineA.EndPoint.Y) - lineA.StartPoint.Y

        Dim bx As Double =
        CDbl(lineB.EndPoint.X) - lineB.StartPoint.X
        Dim by As Double =
        CDbl(lineB.EndPoint.Y) - lineB.StartPoint.Y

        Dim cx As Double =
        CDbl(lineB.StartPoint.X) - lineA.StartPoint.X
        Dim cy As Double =
        CDbl(lineB.StartPoint.Y) - lineA.StartPoint.Y

        Dim denominator As Double = ax * by - ay * bx

        Dim lengthA As Double = Math.Sqrt(ax * ax + ay * ay)
        Dim lengthB As Double = Math.Sqrt(bx * bx + by * by)

        If lengthA <= tolerance OrElse lengthB <= tolerance Then
            Return False
        End If

        ' Parallel and collinear overlaps are handled separately.
        If Math.Abs(denominator) <= tolerance * lengthA * lengthB Then
            Return False
        End If

        Dim t As Double = (cx * by - cy * bx) / denominator
        Dim u As Double = (cx * ay - cy * ax) / denominator

        ' Intersection must lie on both finite segments.
        Dim paramTolA As Double = tolerance / lengthA
        Dim paramTolB As Double = tolerance / lengthB

        If t < -paramTolA OrElse t > 1.0 + paramTolA OrElse
       u < -paramTolB OrElse u > 1.0 + paramTolB Then

            Return False
        End If

        ' Two segments meeting at their endpoints are allowed.
        Dim atEndpointA As Boolean =
        Math.Abs(t) <= paramTolA OrElse
        Math.Abs(t - 1.0) <= paramTolA

        Dim atEndpointB As Boolean =
        Math.Abs(u) <= paramTolB OrElse
        Math.Abs(u - 1.0) <= paramTolB

        Return Not (atEndpointA AndAlso atEndpointB)

    End Function


    Private Function DoLineArcSegmentsCross(
    line As ToolShapes.LineSegment2D,
    arc As ToolShapes.ArcSegment2D,
    tolerance As Double) As Boolean

        If line Is Nothing OrElse arc Is Nothing Then
            Return False
        End If

        Dim points As List(Of PointF) = Nothing
        Dim uArc As List(Of Double) = Nothing
        Dim uLine As List(Of Double) = Nothing

        If Not ToolShapes.ConvexHullGeometry.TestIntersectArcLine(
        arc,
        line,
        tolerance,
        points,
        uArc,
        uLine) Then

            Return False
        End If

        For Each intersection As PointF In points

            Dim atLineEndpoint As Boolean =
            PointsWithinTolerance(
                intersection,
                line.StartPoint,
                tolerance) OrElse
            PointsWithinTolerance(
                intersection,
                line.EndPoint,
                tolerance)

            Dim atArcEndpoint As Boolean =
            PointsWithinTolerance(
                intersection,
                arc.StartPoint,
                tolerance) OrElse
            PointsWithinTolerance(
                intersection,
                arc.EndPoint,
                tolerance)

            ' A shared endpoint is a valid connection.
            ' All other intersections invalidate the profile.
            If Not (atLineEndpoint AndAlso atArcEndpoint) Then
                Return True
            End If

        Next

        Return False

    End Function


    Private Function DoArcSegmentsCross(
    arcA As ToolShapes.ArcSegment2D,
    arcB As ToolShapes.ArcSegment2D,
    tolerance As Double) As Boolean

        If arcA Is Nothing OrElse arcB Is Nothing Then
            Return False
        End If

        Dim points As List(Of PointF) = Nothing
        Dim uA As List(Of Double) = Nothing
        Dim uB As List(Of Double) = Nothing

        If Not ToolShapes.ConvexHullGeometry.TestIntersectArcArc(
        arcA,
        arcB,
        tolerance,
        points,
        uA,
        uB) Then

            Return False
        End If

        For Each intersection As PointF In points

            Dim atEndpointA As Boolean =
            PointsWithinTolerance(
                intersection,
                arcA.StartPoint,
                tolerance) OrElse
            PointsWithinTolerance(
                intersection,
                arcA.EndPoint,
                tolerance)

            Dim atEndpointB As Boolean =
            PointsWithinTolerance(
                intersection,
                arcB.StartPoint,
                tolerance) OrElse
            PointsWithinTolerance(
                intersection,
                arcB.EndPoint,
                tolerance)

            ' Shared endpoints are legitimate connections.
            ' Other intersections invalidate the profile.
            If Not (atEndpointA AndAlso atEndpointB) Then
                Return True
            End If

        Next

        Return False

    End Function


    Private Function DoArcSegmentsOverlap(
    arcA As ArcSegment2D,
    arcB As ArcSegment2D,
    tolerance As Double) As Boolean

        If arcA Is Nothing OrElse arcB Is Nothing Then
            Return False
        End If

        If Not PointsWithinTolerance(
        arcA.Center, arcB.Center, tolerance) Then
            Return False
        End If

        If Math.Abs(arcA.Radius - arcB.Radius) > tolerance Then
            Return False
        End If

        If arcA.Radius <= 0.0 Then Return False

        Dim twoPi As Double = 2.0 * Math.PI
        Dim angularTolerance As Double =
        tolerance / arcA.Radius

        Dim intervalsA As New List(Of Tuple(Of Double, Double))
        Dim intervalsB As New List(Of Tuple(Of Double, Double))

        For Each arc As ArcSegment2D In
        New ArcSegment2D() {arcA, arcB}

            Dim startAngle As Double =
            If(arc.IsClockwise, arc.EndAngle, arc.StartAngle)

            Dim endAngle As Double =
            If(arc.IsClockwise, arc.StartAngle, arc.EndAngle)

            Dim sweep As Double = endAngle - startAngle

            While sweep < 0.0
                sweep += twoPi
            End While

            While sweep > twoPi
                sweep -= twoPi
            End While

            If sweep <= angularTolerance Then Continue For

            Dim normalizedStart As Double =
            startAngle Mod twoPi

            If normalizedStart < 0.0 Then
                normalizedStart += twoPi
            End If

            Dim target As List(Of Tuple(Of Double, Double)) =
            If(Object.ReferenceEquals(arc, arcA),
               intervalsA, intervalsB)

            Dim normalizedEnd As Double =
            normalizedStart + sweep

            If normalizedEnd <= twoPi Then
                target.Add(Tuple.Create(
                normalizedStart, normalizedEnd))
            Else
                target.Add(Tuple.Create(
                normalizedStart, twoPi))

                target.Add(Tuple.Create(
                0.0, normalizedEnd - twoPi))
            End If

        Next

        For Each intervalA In intervalsA
            For Each intervalB In intervalsB

                Dim overlapStart As Double =
                Math.Max(intervalA.Item1, intervalB.Item1)

                Dim overlapEnd As Double =
                Math.Min(intervalA.Item2, intervalB.Item2)

                If overlapEnd - overlapStart >
               angularTolerance Then
                    Return True
                End If

            Next
        Next

        Return False

    End Function


    Private Function HasCrossingProfileArcs(
    profile As GeoProfile,
    geometry As List(Of GeoEntity),
    tolerance As Double) As Boolean

        If profile Is Nothing OrElse geometry Is Nothing Then
            Return False
        End If

        For Each profileEntity As GeoEntity In profile.Entities

            Dim profileArc As GeoArc =
            TryCast(profileEntity, GeoArc)

            If profileArc Is Nothing Then Continue For

            Dim arcA As ArcSegment2D =
            CreateArcSegment(profileArc)

            If arcA Is Nothing Then Continue For

            For Each candidate As GeoEntity In geometry

                If Object.ReferenceEquals(profileEntity, candidate) Then
                    Continue For
                End If

                Dim candidateArc As GeoArc =
                TryCast(candidate, GeoArc)

                If candidateArc Is Nothing Then Continue For

                If Not SameGeoAssociation(
                profileEntity, candidate) Then
                    Continue For
                End If

                Dim arcB As ArcSegment2D =
                CreateArcSegment(candidateArc)

                If arcB Is Nothing Then Continue For

                If DoArcSegmentsOverlap(
                        arcA, arcB, tolerance) OrElse
                       DoArcSegmentsCross(
                        arcA, arcB, tolerance) Then

                    Return True

                End If

            Next

        Next

        Return False

    End Function


    Private Function HasOverlappingProfileLines(
    profile As GeoProfile,
    geometry As List(Of GeoEntity),
    tolerance As Double) As Boolean

        If profile Is Nothing OrElse
       geometry Is Nothing Then

            Return False
        End If

        For Each profileEntity As GeoEntity In profile.Entities

            Dim profileLine As GeoLine =
            TryCast(profileEntity, GeoLine)

            If profileLine Is Nothing Then Continue For

            Dim startA As GeoPoint = Nothing
            Dim endA As GeoPoint = Nothing

            If Not GetGeoEntityEndpoints(
            profileLine, startA, endA) Then Continue For

            Dim lineA As New LineSegment2D(
            New PointF(CSng(startA.X), CSng(startA.Y)),
            New PointF(CSng(endA.X), CSng(endA.Y)))

            For Each candidate As GeoEntity In geometry

                If Object.ReferenceEquals(
                profileEntity, candidate) Then Continue For

                If Not TypeOf candidate Is GeoLine Then
                    Continue For
                End If

                If Not SameGeoAssociation(
                profileEntity, candidate) Then Continue For

                Dim startB As GeoPoint = Nothing
                Dim endB As GeoPoint = Nothing

                If Not GetGeoEntityEndpoints(
                candidate, startB, endB) Then Continue For

                Dim lineB As New LineSegment2D(
                New PointF(CSng(startB.X), CSng(startB.Y)),
                New PointF(CSng(endB.X), CSng(endB.Y)))

                If DoLineSegmentsOverlap(
                lineA, lineB, tolerance) Then

                    Return True

                End If

            Next

        Next

        Return False

    End Function


    Private Function HasCrossingProfileLines(
    profile As GeoProfile,
    geometry As List(Of GeoEntity),
    tolerance As Double) As Boolean

        If profile Is Nothing OrElse
       geometry Is Nothing Then Return False

        For Each profileEntity As GeoEntity In profile.Entities

            Dim profileLine As GeoLine =
            TryCast(profileEntity, GeoLine)

            If profileLine Is Nothing Then Continue For

            Dim startA As GeoPoint = Nothing
            Dim endA As GeoPoint = Nothing

            If Not GetGeoEntityEndpoints(
            profileLine, startA, endA) Then Continue For

            Dim lineA As New LineSegment2D(
            New PointF(CSng(startA.X), CSng(startA.Y)),
            New PointF(CSng(endA.X), CSng(endA.Y)))

            For Each candidate As GeoEntity In geometry

                If Object.ReferenceEquals(
                profileEntity, candidate) Then Continue For

                If Not TypeOf candidate Is GeoLine Then Continue For

                If Not SameGeoAssociation(
                profileEntity, candidate) Then Continue For

                Dim startB As GeoPoint = Nothing
                Dim endB As GeoPoint = Nothing

                If Not GetGeoEntityEndpoints(
                candidate, startB, endB) Then Continue For

                Dim lineB As New LineSegment2D(
                New PointF(CSng(startB.X), CSng(startB.Y)),
                New PointF(CSng(endB.X), CSng(endB.Y)))

                If DoLineSegmentsCross(
                lineA, lineB, tolerance) Then

                    Return True

                End If

            Next

        Next

        Return False

    End Function


    Private Function HasCrossingProfileLineArcs(
    profile As GeoProfile,
    geometry As List(Of GeoEntity),
    tolerance As Double) As Boolean

        If profile Is Nothing OrElse geometry Is Nothing Then
            Return False
        End If

        For Each profileEntity As GeoEntity In profile.Entities

            If Not TypeOf profileEntity Is GeoLine AndAlso
           Not TypeOf profileEntity Is GeoArc Then
                Continue For
            End If

            For Each candidate As GeoEntity In geometry

                If Object.ReferenceEquals(profileEntity, candidate) Then
                    Continue For
                End If

                If Not SameGeoAssociation(profileEntity, candidate) Then
                    Continue For
                End If

                Dim geoLine As GeoLine = Nothing
                Dim geoArc As GeoArc = Nothing

                If TypeOf profileEntity Is GeoLine AndAlso
               TypeOf candidate Is GeoArc Then

                    geoLine = DirectCast(profileEntity, GeoLine)
                    geoArc = DirectCast(candidate, GeoArc)

                ElseIf TypeOf profileEntity Is GeoArc AndAlso
                   TypeOf candidate Is GeoLine Then

                    geoLine = DirectCast(candidate, GeoLine)
                    geoArc = DirectCast(profileEntity, GeoArc)

                Else
                    Continue For
                End If

                If geoLine.StartPoint Is Nothing OrElse
               geoLine.EndPoint Is Nothing Then
                    Continue For
                End If

                Dim line As New LineSegment2D(
                New PointF(
                    CSng(geoLine.StartPoint.X),
                    CSng(geoLine.StartPoint.Y)),
                New PointF(
                    CSng(geoLine.EndPoint.X),
                    CSng(geoLine.EndPoint.Y)))

                Dim arc As ArcSegment2D =
                CreateArcSegment(geoArc)

                If arc Is Nothing Then Continue For

                If DoLineArcSegmentsCross(
                line, arc, tolerance) Then

                    Return True

                End If

            Next

        Next

        Return False

    End Function


    Private Function CreateArcSegment(
    arc As GeoArc) As ToolShapes.ArcSegment2D

        If arc Is Nothing OrElse arc.Center Is Nothing Then
            Return Nothing
        End If

        Dim startRadians As Double =
        arc.StartAngle * Math.PI / 180.0

        Dim endRadians As Double =
        arc.EndAngle * Math.PI / 180.0

        Return New ToolShapes.ArcSegment2D(
        New PointF(
            CSng(arc.Center.X),
            CSng(arc.Center.Y)),
        arc.Radius,
        startRadians,
        endRadians,
        arc.IsClockwise)

    End Function

    Public Function CreatePreviewGeometry(
    cadEntities As List(Of CadEntity)) As List(Of GeoEntity)

        Dim geometry As New List(Of GeoEntity)

        Dim points As New Dictionary(Of (Double, Double), GeoPoint)

        If cadEntities Is Nothing Then Return geometry

        For Each cad As CadEntity In cadEntities

            If TypeOf cad Is CadEntity.CadLine Then

                geometry.Add(
                    ConvertCadLine(
                        DirectCast(cad, CadEntity.CadLine),
                        Nothing,
                        points))

            ElseIf TypeOf cad Is CadEntity.CadArc Then

                geometry.Add(
                    ConvertCadArc(
                        DirectCast(cad, CadEntity.CadArc),
                        Nothing,
                        points))

            ElseIf TypeOf cad Is CadEntity.CadCircle Then

                geometry.Add(
                    ConvertCadCircle(
                        DirectCast(cad, CadEntity.CadCircle),
                        Nothing,
                        points))

            ElseIf TypeOf cad Is CadPoint Then

                Dim cp As CadPoint = DirectCast(cad, CadPoint)

                geometry.Add(New GeoPoint With {
                    .X = CDbl(cp.Position.X),
                    .Y = CDbl(cp.Position.Y)
                })

            End If

        Next

        Return geometry

    End Function


    Public Sub TestProfileSignedArea()

        Const tolerance As Double = 0.000001

        Dim passed As Integer = 0
        Dim failed As Integer = 0

        Debug.WriteLine("===== PROFILE SIGNED AREA TEST =====")

        Dim check As Action(Of String, GeoProfile, Double) =
            Sub(description, profile, expected)

                Dim actual As Double =
                    CalculateGeoProfileSignedArea(profile)

                Dim success As Boolean =
                    Math.Abs(actual - expected) <= tolerance

                If success Then
                    passed += 1
                Else
                    failed += 1
                End If

                Debug.WriteLine(
                    $"{If(success, "PASS", "FAIL")} | " &
                    $"{description} | " &
                    $"Expected={expected:F8} | " &
                    $"Actual={actual:F8}")

            End Sub

        ' --------------------------------------------------
        ' Local geometry construction helpers
        ' --------------------------------------------------

        Dim makePoint As Func(Of Double, Double, GeoPoint) =
            Function(x, y)
                Return New GeoPoint With {
                    .X = x,
                    .Y = y
                }
            End Function

        Dim addSegment As Action(Of GeoProfile,
                                     GeoEntity,
                                     Boolean) =
            Sub(profile, entity, reversed)

                profile.Entities.Add(entity)

                profile.Segments.Add(
                    New GeoProfileSegment With {
                        .Entity = entity,
                        .Reversed = reversed
                    })

            End Sub

        ' --------------------------------------------------
        ' Rectangle - counterclockwise
        ' --------------------------------------------------

        Dim p1 As GeoPoint = makePoint(0, 0)
        Dim p2 As GeoPoint = makePoint(10, 0)
        Dim p3 As GeoPoint = makePoint(10, 6)
        Dim p4 As GeoPoint = makePoint(0, 6)

        Dim line1 As New GeoLine With {
            .StartPoint = p1,
            .EndPoint = p2
        }

        Dim line2 As New GeoLine With {
            .StartPoint = p2,
            .EndPoint = p3
        }

        Dim line3 As New GeoLine With {
            .StartPoint = p3,
            .EndPoint = p4
        }

        Dim line4 As New GeoLine With {
            .StartPoint = p4,
            .EndPoint = p1
        }

        Dim rectangleCCW As New GeoProfile With {
            .IsClosed = True
        }

        addSegment(rectangleCCW, line1, False)
        addSegment(rectangleCCW, line2, False)
        addSegment(rectangleCCW, line3, False)
        addSegment(rectangleCCW, line4, False)

        check("Rectangle CCW", rectangleCCW, 60.0)

        ' --------------------------------------------------
        ' Same rectangle - reversed clockwise
        ' --------------------------------------------------

        Dim rectangleCW As New GeoProfile With {
            .IsClosed = True
        }

        addSegment(rectangleCW, line4, True)
        addSegment(rectangleCW, line3, True)
        addSegment(rectangleCW, line2, True)
        addSegment(rectangleCW, line1, True)

        check("Rectangle CW", rectangleCW, -60.0)

        ' --------------------------------------------------
        ' Complete circle
        ' --------------------------------------------------

        Dim circle As New GeoCircle With {
            .Center = makePoint(5, 5),
            .Radius = 3.0
        }

        Dim circleCCW As New GeoProfile With {
            .IsClosed = True
        }

        addSegment(circleCCW, circle, False)

        check("Complete circle CCW",
              circleCCW,
              Math.PI * 9.0)

        Dim circleCW As New GeoProfile With {
            .IsClosed = True
        }

        addSegment(circleCW, circle, True)

        check("Complete circle CW",
              circleCW,
              -Math.PI * 9.0)

        ' --------------------------------------------------
        ' Open, empty and missing profiles
        ' --------------------------------------------------

        Dim openProfile As New GeoProfile With {
            .IsClosed = False
        }

        addSegment(openProfile, line1, False)

        check("Open profile", openProfile, 0.0)

        Dim emptyProfile As New GeoProfile With {
            .IsClosed = True
        }

        check("Empty profile", emptyProfile, 0.0)

        check("Missing profile", Nothing, 0.0)


        ' --------------------------------------------------
        ' Local arc construction helper
        ' --------------------------------------------------

        Dim makeArc As Func(Of Double, Double, Double,
                       Double, Double, Boolean, GeoArc) =
    Function(cx, cy, radius, startDegrees,
             endDegrees, clockwise)

        Dim a1 As Double =
            startDegrees * Math.PI / 180.0

        Dim a2 As Double =
            endDegrees * Math.PI / 180.0

        Return New GeoArc With {
            .Center = makePoint(cx, cy),
            .Radius = radius,
            .StartAngle = startDegrees,
            .EndAngle = endDegrees,
            .IsClockwise = clockwise,
            .StartPoint = makePoint(
                cx + radius * Math.Cos(a1),
                cy + radius * Math.Sin(a1)),
            .EndPoint = makePoint(
                cx + radius * Math.Cos(a2),
                cy + radius * Math.Sin(a2))
        }

    End Function

        ' --------------------------------------------------
        ' Two semicircles forming a complete circle
        ' --------------------------------------------------

        Dim upperArc As GeoArc =
    makeArc(5, 5, 3, 0, 180, False)

        Dim lowerArc As GeoArc =
    makeArc(5, 5, 3, 180, 360, False)

        Dim twoArcsCCW As New GeoProfile With {
    .IsClosed = True
}

        addSegment(twoArcsCCW, upperArc, False)
        addSegment(twoArcsCCW, lowerArc, False)

        check("Two semicircles CCW",
      twoArcsCCW,
      Math.PI * 9.0)

        Dim twoArcsCW As New GeoProfile With {
    .IsClosed = True
}

        addSegment(twoArcsCW, lowerArc, True)
        addSegment(twoArcsCW, upperArc, True)

        check("Two semicircles CW",
      twoArcsCW,
      -Math.PI * 9.0)

        ' --------------------------------------------------
        ' Semicircle closed by a straight line
        ' --------------------------------------------------

        Dim closingLine As New GeoLine With {
    .StartPoint = upperArc.EndPoint,
    .EndPoint = upperArc.StartPoint
}

        Dim semicircleCCW As New GeoProfile With {
    .IsClosed = True
}

        addSegment(semicircleCCW, upperArc, False)
        addSegment(semicircleCCW, closingLine, False)

        check("Semicircle and line CCW",
      semicircleCCW,
      Math.PI * 9.0 / 2.0)

        Dim semicircleCW As New GeoProfile With {
    .IsClosed = True
}

        addSegment(semicircleCW, closingLine, True)
        addSegment(semicircleCW, upperArc, True)

        check("Semicircle and line CW",
      semicircleCW,
      -Math.PI * 9.0 / 2.0)

        ' --------------------------------------------------
        ' Negative angles and angular wraparound
        ' --------------------------------------------------

        Dim negativeArc As GeoArc =
    makeArc(5, 5, 3, -90, 90, False)

        Dim wrapArc As GeoArc =
    makeArc(5, 5, 3, 270, 90, False)

        Dim clockwiseArc As GeoArc =
    makeArc(5, 5, 3, 90, 270, True)

        Dim makeSemicircle As Func(Of GeoArc, GeoProfile) =
    Function(arc)

        Dim profile As New GeoProfile With {
            .IsClosed = True
        }

        Dim chord As New GeoLine With {
            .StartPoint = arc.EndPoint,
            .EndPoint = arc.StartPoint
        }

        addSegment(profile, arc, False)
        addSegment(profile, chord, False)

        Return profile

    End Function

        check("Negative angle semicircle CCW",
      makeSemicircle(negativeArc),
      Math.PI * 9.0 / 2.0)

        check("Wraparound semicircle CCW",
      makeSemicircle(wrapArc),
      Math.PI * 9.0 / 2.0)

        check("Wraparound semicircle CW",
      makeSemicircle(clockwiseArc),
      -Math.PI * 9.0 / 2.0)

        ' --------------------------------------------------
        ' Translation invariance
        ' --------------------------------------------------

        Dim translatedArc As GeoArc =
    makeArc(125, 75, 3, -90, 90, False)

        check("Translated semicircle CCW",
      makeSemicircle(translatedArc),
      Math.PI * 9.0 / 2.0)

        Dim translatedCWArc As GeoArc =
    makeArc(125, 75, 3, 90, -90, True)

        check("Translated semicircle CW",
      makeSemicircle(translatedCWArc),
      -Math.PI * 9.0 / 2.0)


        ' --------------------------------------------------
        ' Minor and major circular segments
        ' --------------------------------------------------

        Dim minorArc As GeoArc =
    makeArc(5, 5, 3, 0, 90, False)

        Dim majorArc As GeoArc =
    makeArc(5, 5, 3, 0, 270, False)

        Dim minorArea As Double =
    (Math.PI * 9.0 / 4.0) - 4.5

        Dim majorArea As Double =
    (Math.PI * 9.0 * 3.0 / 4.0) + 4.5

        check("90-degree circular segment CCW",
      makeSemicircle(minorArc),
      minorArea)

        check("270-degree circular segment CCW",
      makeSemicircle(majorArc),
      majorArea)

        check("90-degree circular segment CW",
      makeSemicircle(
          makeArc(5, 5, 3, 90, 0, True)),
      -minorArea)

        check("270-degree circular segment CW",
      makeSemicircle(
          makeArc(5, 5, 3, 270, 0, True)),
      -majorArea)

        ' --------------------------------------------------
        ' D-shaped profile
        ' Rectangle 6 x 8 plus semicircle R4
        ' --------------------------------------------------

        Dim dArc As GeoArc =
    makeArc(6, 4, 4, -90, 90, False)

        Dim dBottom As New GeoLine With {
    .StartPoint = makePoint(0, 0),
    .EndPoint = dArc.StartPoint
}

        Dim dTop As New GeoLine With {
    .StartPoint = dArc.EndPoint,
    .EndPoint = makePoint(0, 8)
}

        Dim dLeft As New GeoLine With {
    .StartPoint = dTop.EndPoint,
    .EndPoint = dBottom.StartPoint
}

        Dim dProfileCCW As New GeoProfile With {
    .IsClosed = True
}

        addSegment(dProfileCCW, dBottom, False)
        addSegment(dProfileCCW, dArc, False)
        addSegment(dProfileCCW, dTop, False)
        addSegment(dProfileCCW, dLeft, False)

        Dim dArea As Double =
    48.0 + Math.PI * 8.0

        check("D-shaped profile CCW",
      dProfileCCW,
      dArea)

        Dim dProfileCW As New GeoProfile With {
    .IsClosed = True
}

        addSegment(dProfileCW, dLeft, True)
        addSegment(dProfileCW, dTop, True)
        addSegment(dProfileCW, dArc, True)
        addSegment(dProfileCW, dBottom, True)

        check("D-shaped profile CW",
      dProfileCW,
      -dArea)

        ' --------------------------------------------------
        ' Circle path conversion direction
        ' --------------------------------------------------

        Dim checkCirclePath As Action(Of String,
                                GeoProfile,
                                Boolean) =
    Sub(description, profile, expectedCW)

        Dim path As ToolShapes.IPath2D =
            ConvertGeoProfileToPath(profile)

        Dim success As Boolean = False

        If path IsNot Nothing AndAlso
           path.IsClosed AndAlso
           path.Segments.Count() = 1 Then

            Dim convertedArc As ToolShapes.ArcSegment2D =
                TryCast(path.Segments.First(),
                        ToolShapes.ArcSegment2D)

            If convertedArc IsNot Nothing Then

                success =
                    convertedArc.IsClockwise = expectedCW AndAlso
                    Math.Abs(
                        Math.Abs(convertedArc.EndAngle -
                                 convertedArc.StartAngle) -
                        Math.PI * 2.0) <= tolerance

            End If

        End If

        If success Then
            passed += 1
        Else
            failed += 1
        End If

        Debug.WriteLine(
            $"{If(success, "PASS", "FAIL")} | " &
            $"{description} | ExpectedCW={expectedCW}")

    End Sub

        checkCirclePath("Circle path conversion CCW",
                circleCCW,
                False)

        checkCirclePath("Circle path conversion CW",
                circleCW,
                True)


        Debug.WriteLine("------------------------------------")
        Debug.WriteLine($"Passed: {passed}")
        Debug.WriteLine($"Failed: {failed}")
        Debug.WriteLine("====================================")

    End Sub

    Public Sub TestProfileBounds()

        Const tolerance As Double = 0.00001

        Dim passed As Integer = 0
        Dim failed As Integer = 0

        Debug.WriteLine("===== PROFILE BOUNDS TEST =====")

        Dim check As Action(Of String, GeoProfile, RectangleF) =
            Sub(description, profile, expected)

                Dim actual As RectangleF =
                    GetGeoProfileBounds(profile)

                Dim success As Boolean =
                    Math.Abs(actual.X - expected.X) <= tolerance AndAlso
                    Math.Abs(actual.Y - expected.Y) <= tolerance AndAlso
                    Math.Abs(actual.Width - expected.Width) <= tolerance AndAlso
                    Math.Abs(actual.Height - expected.Height) <= tolerance

                If success Then
                    passed += 1
                Else
                    failed += 1
                End If

                Debug.WriteLine(
                    $"{If(success, "PASS", "FAIL")} | " &
                    $"{description} | " &
                    $"Expected={expected} | Actual={actual}")

            End Sub

        ' --------------------------------------------------
        ' Local geometry construction helpers
        ' --------------------------------------------------

        Dim makePoint As Func(Of Double, Double, GeoPoint) =
            Function(x, y)
                Return New GeoPoint With {
                    .X = x,
                    .Y = y
                }
            End Function

        Dim addSegment As Action(Of GeoProfile,
                                     GeoEntity,
                                     Boolean) =
            Sub(profile, entity, reversed)

                profile.Entities.Add(entity)

                profile.Segments.Add(
                    New GeoProfileSegment With {
                        .Entity = entity,
                        .Reversed = reversed
                    })

            End Sub

        ' --------------------------------------------------
        ' Closed rectangular profile
        ' --------------------------------------------------

        Dim p1 As GeoPoint = makePoint(0, 0)
        Dim p2 As GeoPoint = makePoint(10, 0)
        Dim p3 As GeoPoint = makePoint(10, 6)
        Dim p4 As GeoPoint = makePoint(0, 6)

        Dim rectangle As New GeoProfile With {
            .IsClosed = True
        }

        addSegment(rectangle,
                   New GeoLine With {
                       .StartPoint = p1,
                       .EndPoint = p2
                   }, False)

        addSegment(rectangle,
                   New GeoLine With {
                       .StartPoint = p2,
                       .EndPoint = p3
                   }, False)

        addSegment(rectangle,
                   New GeoLine With {
                       .StartPoint = p3,
                       .EndPoint = p4
                   }, False)

        addSegment(rectangle,
                   New GeoLine With {
                       .StartPoint = p4,
                       .EndPoint = p1
                   }, False)

        check("Closed rectangle",
              rectangle,
              New RectangleF(0, 0, 10, 6))

        ' --------------------------------------------------
        ' Open line profile
        ' --------------------------------------------------

        Dim openProfile As New GeoProfile With {
            .IsClosed = False
        }

        addSegment(openProfile,
                   New GeoLine With {
                       .StartPoint = makePoint(2, 3),
                       .EndPoint = makePoint(8, 7)
                   }, False)

        check("Open line profile",
              openProfile,
              New RectangleF(2, 3, 6, 4))

        ' --------------------------------------------------
        ' Complete circles
        ' --------------------------------------------------

        Dim circle As New GeoCircle With {
            .Center = makePoint(5, 5),
            .Radius = 3.0
        }

        Dim circleCCW As New GeoProfile With {
            .IsClosed = True
        }

        addSegment(circleCCW, circle, False)

        check("Complete circle CCW",
              circleCCW,
              New RectangleF(2, 2, 6, 6))

        Dim circleCW As New GeoProfile With {
            .IsClosed = True
        }

        addSegment(circleCW, circle, True)

        check("Complete circle CW",
              circleCW,
              New RectangleF(2, 2, 6, 6))

        ' --------------------------------------------------
        ' Negative coordinates
        ' --------------------------------------------------

        Dim negativeCircle As New GeoProfile With {
            .IsClosed = True
        }

        addSegment(negativeCircle,
                   New GeoCircle With {
                       .Center = makePoint(-10, -5),
                       .Radius = 2.0
                   }, False)

        check("Circle with negative coordinates",
              negativeCircle,
              New RectangleF(-12, -7, 4, 4))

        ' --------------------------------------------------
        ' Empty and invalid profiles
        ' --------------------------------------------------

        check("Missing profile",
              Nothing,
              RectangleF.Empty)

        check("Empty profile",
              New GeoProfile,
              RectangleF.Empty)

        Dim invalidProfile As New GeoProfile With {
            .IsClosed = False
        }

        addSegment(invalidProfile,
                   New GeoLine With {
                       .StartPoint = makePoint(1, 1),
                       .EndPoint = Nothing
                   }, False)

        check("Invalid segment",
              invalidProfile,
              RectangleF.Empty)


        ' --------------------------------------------------
        ' Local arc construction helper
        ' --------------------------------------------------

        Dim makeArc As Func(Of Double, Double, Double,
                       Double, Double, Boolean, GeoArc) =
    Function(cx, cy, radius, startDegrees,
             endDegrees, clockwise)

        Dim a1 As Double =
            startDegrees * Math.PI / 180.0

        Dim a2 As Double =
            endDegrees * Math.PI / 180.0

        Return New GeoArc With {
            .Center = makePoint(cx, cy),
            .Radius = radius,
            .StartAngle = startDegrees,
            .EndAngle = endDegrees,
            .IsClockwise = clockwise,
            .StartPoint = makePoint(
                cx + radius * Math.Cos(a1),
                cy + radius * Math.Sin(a1)),
            .EndPoint = makePoint(
                cx + radius * Math.Cos(a2),
                cy + radius * Math.Sin(a2))
        }

    End Function

        Dim makeArcProfile As Func(Of GeoArc, Boolean, GeoProfile) =
    Function(arc, reversed)

        Dim profile As New GeoProfile With {
            .IsClosed = False
        }

        addSegment(profile, arc, reversed)

        Return profile

    End Function

        ' --------------------------------------------------
        ' Four cardinal extrema
        ' --------------------------------------------------

        check("Upper semicircle - maximum Y",
      makeArcProfile(
          makeArc(5, 5, 3, 0, 180, False), False),
      New RectangleF(2, 5, 6, 3))

        check("Lower semicircle - minimum Y",
      makeArcProfile(
          makeArc(5, 5, 3, 180, 360, False), False),
      New RectangleF(2, 2, 6, 3))

        check("Right semicircle - maximum X",
      makeArcProfile(
          makeArc(5, 5, 3, -90, 90, False), False),
      New RectangleF(5, 2, 3, 6))

        check("Left semicircle - minimum X",
      makeArcProfile(
          makeArc(5, 5, 3, 90, 270, False), False),
      New RectangleF(2, 2, 3, 6))

        ' --------------------------------------------------
        ' Angular wraparound and direction
        ' --------------------------------------------------

        Dim wrapArc As GeoArc =
    makeArc(5, 5, 3, 270, 90, False)

        check("CCW wraparound semicircle",
      makeArcProfile(wrapArc, False),
      New RectangleF(5, 2, 3, 6))

        check("Reversed wraparound semicircle",
      makeArcProfile(wrapArc, True),
      New RectangleF(5, 2, 3, 6))

        check("CW wraparound semicircle",
      makeArcProfile(
          makeArc(5, 5, 3, 90, 270, True), False),
      New RectangleF(5, 2, 3, 6))

        ' --------------------------------------------------
        ' Minor and major arcs
        ' --------------------------------------------------

        check("90-degree minor arc",
      makeArcProfile(
          makeArc(5, 5, 3, 0, 90, False), False),
      New RectangleF(5, 5, 3, 3))

        check("270-degree major arc",
      makeArcProfile(
          makeArc(5, 5, 3, 0, 270, False), False),
      New RectangleF(2, 2, 6, 6))

        ' Arc with neither endpoint at a cardinal extremum

        Dim diagonalOffset As Double =
    3.0 / Math.Sqrt(2.0)

        check("Arc crossing maximum X between endpoints",
      makeArcProfile(
          makeArc(5, 5, 3, -45, 45, False), False),
      New RectangleF(
          CSng(5.0 + diagonalOffset),
          CSng(5.0 - diagonalOffset),
          CSng(3.0 - diagonalOffset),
          CSng(2.0 * diagonalOffset)))

        ' --------------------------------------------------
        ' Mixed Line / Arc profile
        ' --------------------------------------------------

        Dim dArc As GeoArc =
    makeArc(6, 4, 4, -90, 90, False)

        Dim dBottom As New GeoLine With {
    .StartPoint = makePoint(0, 0),
    .EndPoint = dArc.StartPoint
}

        Dim dTop As New GeoLine With {
    .StartPoint = dArc.EndPoint,
    .EndPoint = makePoint(0, 8)
}

        Dim dLeft As New GeoLine With {
    .StartPoint = dTop.EndPoint,
    .EndPoint = dBottom.StartPoint
}

        Dim dProfileCCW As New GeoProfile With {
    .IsClosed = True
}

        addSegment(dProfileCCW, dBottom, False)
        addSegment(dProfileCCW, dArc, False)
        addSegment(dProfileCCW, dTop, False)
        addSegment(dProfileCCW, dLeft, False)

        check("D-shaped profile CCW",
      dProfileCCW,
      New RectangleF(0, 0, 10, 8))

        Dim dProfileCW As New GeoProfile With {
    .IsClosed = True
}

        addSegment(dProfileCW, dLeft, True)
        addSegment(dProfileCW, dTop, True)
        addSegment(dProfileCW, dArc, True)
        addSegment(dProfileCW, dBottom, True)

        check("D-shaped profile CW",
      dProfileCW,
      New RectangleF(0, 0, 10, 8))



        Debug.WriteLine("------------------------------------")
        Debug.WriteLine($"Passed: {passed}")
        Debug.WriteLine($"Failed: {failed}")
        Debug.WriteLine("====================================")

    End Sub


    Public Sub TestGeoProfileClassification()

        Dim passed As Integer = 0
        Dim failed As Integer = 0

        Debug.WriteLine("===== PROFILE CLASSIFICATION TEST =====")

        ' --------------------------------------------------
        ' Local rectangle construction helper
        ' --------------------------------------------------

        Dim makeRectangle As Func(Of Double, Double,
                                 Double, Double, GeoProfile) =
        Function(x, y, width, height)

            Dim profile As New GeoProfile With {
                .IsClosed = True
            }

            Dim points As GeoPoint() = {
                New GeoPoint With {.X = x, .Y = y},
                New GeoPoint With {.X = x + width, .Y = y},
                New GeoPoint With {.X = x + width, .Y = y + height},
                New GeoPoint With {.X = x, .Y = y + height}
            }

            For i As Integer = 0 To 3

                Dim line As New GeoLine With {
                    .StartPoint = points(i),
                    .EndPoint = points((i + 1) Mod 4)
                }

                profile.Entities.Add(line)

                profile.Segments.Add(
                    New GeoProfileSegment With {
                        .Entity = line,
                        .Reversed = False
                    })

            Next

            Return profile

        End Function

        ' --------------------------------------------------
        ' Result verification helper
        ' --------------------------------------------------

        Dim check As Action(Of String,
                           Dictionary(Of GeoProfile, GeoProfileClassification),
                           GeoProfile,
                           GeoProfileClassification) =
       Sub(description, classificationResults, profile, expected)

           Dim actual As GeoProfileClassification

           Dim found As Boolean =
    classificationResults.TryGetValue(profile, actual)

           Dim success As Boolean =
                found AndAlso actual = expected

           If success Then
               passed += 1
           Else
               failed += 1
           End If






           Debug.WriteLine(
                $"{If(success, "PASS", "FAIL")} | " &
                $"{description} | " &
                $"Expected={expected} | " &
                $"Actual={If(found, actual.ToString(), "Missing")}")

       End Sub

        ' --------------------------------------------------
        ' Test 1: Single outside profile
        ' --------------------------------------------------

        Dim outer As GeoProfile =
        makeRectangle(0, 0, 20, 20)

        Dim results As Dictionary(Of GeoProfile, GeoProfileClassification) =
        ClassifyGeoProfiles({outer})

        check("Single outside profile",
          results, outer,
          GeoProfileClassification.Outside)

        ' --------------------------------------------------
        ' Test 2: Four nesting levels
        ' --------------------------------------------------

        Dim inner As GeoProfile =
        makeRectangle(2, 2, 16, 16)

        Dim island As GeoProfile =
        makeRectangle(4, 4, 12, 12)

        Dim islandOpening As GeoProfile =
        makeRectangle(6, 6, 8, 8)

        results = ClassifyGeoProfiles(
        {outer, inner, island, islandOpening})

        check("Depth 0 - outside",
          results, outer,
          GeoProfileClassification.Outside)

        check("Depth 1 - inside",
          results, inner,
          GeoProfileClassification.Inside)

        check("Depth 2 - island",
          results, island,
          GeoProfileClassification.Outside)

        check("Depth 3 - island opening",
          results, islandOpening,
          GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 3: Input order independence
        ' --------------------------------------------------

        results = ClassifyGeoProfiles(
        {islandOpening, island, inner, outer})

        check("Reverse order - depth 0",
          results, outer,
          GeoProfileClassification.Outside)

        check("Reverse order - depth 1",
          results, inner,
          GeoProfileClassification.Inside)

        check("Reverse order - depth 2",
          results, island,
          GeoProfileClassification.Outside)

        check("Reverse order - depth 3",
          results, islandOpening,
          GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 4: Separate outside profiles
        ' --------------------------------------------------

        Dim separate As GeoProfile =
        makeRectangle(30, 30, 10, 10)

        results = ClassifyGeoProfiles({outer, separate})

        check("Separate profile A",
          results, outer,
          GeoProfileClassification.Outside)

        check("Separate profile B",
          results, separate,
          GeoProfileClassification.Outside)

        ' --------------------------------------------------
        ' Test 5: Intersecting profiles
        ' --------------------------------------------------

        Dim crossing As GeoProfile =
        makeRectangle(15, 15, 10, 10)

        results = ClassifyGeoProfiles(
        {outer, crossing, separate})

        check("Intersecting profile A",
          results, outer,
          GeoProfileClassification.Ambiguous)

        check("Intersecting profile B",
          results, crossing,
          GeoProfileClassification.Ambiguous)

        check("Unaffected separate profile",
          results, separate,
          GeoProfileClassification.Outside)

        ' --------------------------------------------------
        ' Test 6: Touching profiles
        ' --------------------------------------------------

        Dim touching As GeoProfile =
        makeRectangle(20, 5, 5, 5)

        results = ClassifyGeoProfiles({outer, touching})

        check("Touching profile A",
          results, outer,
          GeoProfileClassification.Ambiguous)

        check("Touching profile B",
          results, touching,
          GeoProfileClassification.Ambiguous)

        ' --------------------------------------------------
        ' Test 7: Coincident profiles
        ' --------------------------------------------------

        Dim coincident As GeoProfile =
        makeRectangle(0, 0, 20, 20)

        results = ClassifyGeoProfiles({outer, coincident})

        check("Coincident profile A",
          results, outer,
          GeoProfileClassification.Ambiguous)

        check("Coincident profile B",
          results, coincident,
          GeoProfileClassification.Ambiguous)

        ' --------------------------------------------------
        ' Test 8: Open and empty profiles
        ' --------------------------------------------------

        Dim openProfile As GeoProfile =
        makeRectangle(2, 2, 5, 5)

        openProfile.IsClosed = False

        Dim emptyProfile As New GeoProfile With {
        .IsClosed = True
    }

        results = ClassifyGeoProfiles(
        {outer, openProfile, emptyProfile})

        check("Open profile",
          results, openProfile,
          GeoProfileClassification.Invalid)

        check("Empty closed profile",
          results, emptyProfile,
          GeoProfileClassification.Invalid)


        check("Outside with excluded profiles",
          results, outer,
          GeoProfileClassification.Outside)

        ' --------------------------------------------------
        ' Local circle construction helper
        ' --------------------------------------------------

        Dim makeCircle As Func(Of Double, Double,
                          Double, GeoProfile) =
    Function(cx, cy, radius)

        Dim circleProfile As New GeoProfile With {
            .IsClosed = True
        }

        Dim circle As New GeoCircle With {
            .Center = New GeoPoint With {
                .X = cx,
                .Y = cy
            },
            .Radius = radius
        }

        circleProfile.Entities.Add(circle)

        circleProfile.Segments.Add(
            New GeoProfileSegment With {
                .Entity = circle,
                .Reversed = False
            })

        Return circleProfile

    End Function

        ' --------------------------------------------------
        ' Test 9: Nested circular profiles
        ' --------------------------------------------------

        Dim outerCircle As GeoProfile =
    makeCircle(10, 10, 10)

        Dim innerCircle As GeoProfile =
    makeCircle(10, 10, 8)

        Dim circleIsland As GeoProfile =
    makeCircle(10, 10, 6)

        Dim circleOpening As GeoProfile =
    makeCircle(10, 10, 4)

        results = ClassifyGeoProfiles(
    {outerCircle, innerCircle,
     circleIsland, circleOpening})

        check("Circle depth 0",
      results, outerCircle,
      GeoProfileClassification.Outside)

        check("Circle depth 1",
      results, innerCircle,
      GeoProfileClassification.Inside)

        check("Circle depth 2",
      results, circleIsland,
      GeoProfileClassification.Outside)

        check("Circle depth 3",
      results, circleOpening,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 10: Rectangle inside circle
        ' --------------------------------------------------

        Dim rectangleInCircle As GeoProfile =
    makeRectangle(7, 7, 6, 6)

        results = ClassifyGeoProfiles(
    {outerCircle, rectangleInCircle})

        check("Circle containing rectangle",
      results, outerCircle,
      GeoProfileClassification.Outside)

        check("Rectangle inside circle",
      results, rectangleInCircle,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 11: Circle inside rectangle
        ' --------------------------------------------------

        Dim circleInRectangle As GeoProfile =
    makeCircle(10, 10, 3)

        results = ClassifyGeoProfiles(
    {outer, circleInRectangle})

        check("Rectangle containing circle",
      results, outer,
      GeoProfileClassification.Outside)

        check("Circle inside rectangle",
      results, circleInRectangle,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 12: Mixed nested boundaries
        ' --------------------------------------------------

        Dim mixedRectangle As GeoProfile =
    makeRectangle(5, 5, 10, 10)

        Dim mixedCircle As GeoProfile =
    makeCircle(10, 10, 4)

        Dim mixedOpening As GeoProfile =
    makeRectangle(9, 9, 2, 2)

        results = ClassifyGeoProfiles(
    {outerCircle, mixedRectangle,
     mixedCircle, mixedOpening})

        check("Mixed depth 0 - circle",
      results, outerCircle,
      GeoProfileClassification.Outside)

        check("Mixed depth 1 - rectangle",
      results, mixedRectangle,
      GeoProfileClassification.Inside)

        check("Mixed depth 2 - circle",
      results, mixedCircle,
      GeoProfileClassification.Outside)

        check("Mixed depth 3 - rectangle",
      results, mixedOpening,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 13: Intersecting circles
        ' --------------------------------------------------

        Dim crossingCircle As GeoProfile =
    makeCircle(18, 10, 5)

        results = ClassifyGeoProfiles(
    {outerCircle, crossingCircle})

        check("Intersecting circle A",
      results, outerCircle,
      GeoProfileClassification.Ambiguous)

        check("Intersecting circle B",
      results, crossingCircle,
      GeoProfileClassification.Ambiguous)

        ' --------------------------------------------------
        ' Test 14: Internally tangent circles
        ' --------------------------------------------------

        Dim tangentCircle As GeoProfile =
    makeCircle(15, 10, 5)

        results = ClassifyGeoProfiles(
    {outerCircle, tangentCircle})

        check("Internally tangent circle A",
      results, outerCircle,
      GeoProfileClassification.Ambiguous)

        check("Internally tangent circle B",
      results, tangentCircle,
      GeoProfileClassification.Ambiguous)

        ' --------------------------------------------------
        ' Test 15: Coincident circles
        ' --------------------------------------------------

        Dim coincidentCircle As GeoProfile =
    makeCircle(10, 10, 10)

        results = ClassifyGeoProfiles(
    {outerCircle, coincidentCircle})

        check("Coincident circle A",
      results, outerCircle,
      GeoProfileClassification.Ambiguous)

        check("Coincident circle B",
      results, coincidentCircle,
      GeoProfileClassification.Ambiguous)

        ' --------------------------------------------------
        ' Test 16: Construct mixed Line / Arc boundary
        ' --------------------------------------------------

        Dim dProfile As New GeoProfile With {
    .IsClosed = True
}

        Dim dP1 As New GeoPoint With {.X = 0, .Y = 0}
        Dim dP2 As New GeoPoint With {.X = 6, .Y = 0}
        Dim dP3 As New GeoPoint With {.X = 6, .Y = 8}
        Dim dP4 As New GeoPoint With {.X = 0, .Y = 8}

        Dim dBottom As New GeoLine With {
    .StartPoint = dP1,
    .EndPoint = dP2
}

        Dim dArc As New GeoArc With {
    .Center = New GeoPoint With {.X = 6, .Y = 4},
    .Radius = 4.0,
    .StartAngle = -90.0,
    .EndAngle = 90.0,
    .IsClockwise = False,
    .StartPoint = dP2,
    .EndPoint = dP3
}

        Dim dTop As New GeoLine With {
    .StartPoint = dP3,
    .EndPoint = dP4
}

        Dim dLeft As New GeoLine With {
    .StartPoint = dP4,
    .EndPoint = dP1
}

        Dim dEntities As GeoEntity() = {
    dBottom, dArc, dTop, dLeft
}

        For Each entity As GeoEntity In dEntities

            dProfile.Entities.Add(entity)

            dProfile.Segments.Add(
        New GeoProfileSegment With {
            .Entity = entity,
            .Reversed = False
        })

        Next

        ' --------------------------------------------------
        ' Test 17: Rectangle inside straight portion
        ' --------------------------------------------------

        Dim dInnerRectangle As GeoProfile =
    makeRectangle(1, 1, 3, 2)

        results = ClassifyGeoProfiles(
    {dProfile, dInnerRectangle})

        check("D-profile containing rectangle",
      results, dProfile,
      GeoProfileClassification.Outside)

        check("Rectangle inside straight portion",
      results, dInnerRectangle,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 18: Rectangle inside curved portion
        ' --------------------------------------------------

        Dim dCurvedRectangle As GeoProfile =
    makeRectangle(7, 3, 2, 2)

        results = ClassifyGeoProfiles(
    {dProfile, dCurvedRectangle})

        check("D-profile containing curved-region rectangle",
      results, dProfile,
      GeoProfileClassification.Outside)

        check("Rectangle inside curved portion",
      results, dCurvedRectangle,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 19: Circle inside curved portion
        ' --------------------------------------------------

        Dim dInnerCircle As GeoProfile =
    makeCircle(8, 4, 1)

        results = ClassifyGeoProfiles(
    {dProfile, dInnerCircle})

        check("D-profile containing circle",
      results, dProfile,
      GeoProfileClassification.Outside)

        check("Circle inside curved portion",
      results, dInnerCircle,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 20: Rectangle outside curved boundary
        ' --------------------------------------------------

        Dim dOutsideRectangle As GeoProfile =
    makeRectangle(11, 3, 2, 2)

        results = ClassifyGeoProfiles(
    {dProfile, dOutsideRectangle})

        check("D-profile separate from rectangle",
      results, dProfile,
      GeoProfileClassification.Outside)

        check("Rectangle outside curved boundary",
      results, dOutsideRectangle,
      GeoProfileClassification.Outside)

        ' --------------------------------------------------
        ' Test 21: Rectangle crossing curved boundary
        ' --------------------------------------------------

        Dim dCrossingRectangle As GeoProfile =
    makeRectangle(9, 3, 2, 2)

        results = ClassifyGeoProfiles(
    {dProfile, dCrossingRectangle})

        check("D-profile intersected by rectangle",
      results, dProfile,
      GeoProfileClassification.Ambiguous)

        check("Rectangle crossing curved boundary",
      results, dCrossingRectangle,
      GeoProfileClassification.Ambiguous)

        ' --------------------------------------------------
        ' Test 22: Circle tangent to curved boundary
        ' --------------------------------------------------

        Dim dTangentCircle As GeoProfile =
    makeCircle(9, 4, 1)

        results = ClassifyGeoProfiles(
    {dProfile, dTangentCircle})

        check("D-profile tangent to circle",
      results, dProfile,
      GeoProfileClassification.Ambiguous)

        check("Circle tangent to curved boundary",
      results, dTangentCircle,
      GeoProfileClassification.Ambiguous)

        ' --------------------------------------------------
        ' Test 23: Classification independent of CAM layers
        ' --------------------------------------------------

        Dim layerA As New Layer With {
    .ID = 1,
    .Name = "Outside Geometry",
    .Color = Color.Cyan,
    .Visible = True
}

        Dim layerB As New Layer With {
    .ID = 2,
    .Name = "Inside Geometry",
    .Color = Color.Yellow,
    .Visible = True
}

        Dim layerC As New Layer With {
    .ID = 3,
    .Name = "Island Geometry",
    .Color = Color.Green,
    .Visible = True
}

        Dim layeredOuter As GeoProfile =
    makeRectangle(0, 0, 20, 20)

        Dim layeredInner As GeoProfile =
    makeRectangle(2, 2, 16, 16)

        Dim layeredIsland As GeoProfile =
    makeRectangle(4, 4, 12, 12)

        For Each entity As GeoEntity In layeredOuter.Entities
            entity.Association = layerA
        Next

        For Each entity As GeoEntity In layeredInner.Entities
            entity.Association = layerB
        Next

        For Each entity As GeoEntity In layeredIsland.Entities
            entity.Association = layerC
        Next

        results = ClassifyGeoProfiles(
    {layeredOuter, layeredInner, layeredIsland})

        check("Different layers - outside",
      results, layeredOuter,
      GeoProfileClassification.Outside)

        check("Different layers - inside",
      results, layeredInner,
      GeoProfileClassification.Inside)

        check("Different layers - island",
      results, layeredIsland,
      GeoProfileClassification.Outside)

        ' --------------------------------------------------
        ' Test 24: Hidden CAM layer
        ' --------------------------------------------------

        layerA.Visible = False

        results = ClassifyGeoProfiles(
    {layeredOuter, layeredInner, layeredIsland})

        check("Hidden outside layer",
      results, layeredOuter,
      GeoProfileClassification.Outside)

        check("Inside hidden outside boundary",
      results, layeredInner,
      GeoProfileClassification.Inside)

        check("Island inside hidden outside boundary",
      results, layeredIsland,
      GeoProfileClassification.Outside)

        ' --------------------------------------------------
        ' Test 25: Intersecting profiles on different layers
        ' --------------------------------------------------

        Dim layeredCrossing As GeoProfile =
    makeRectangle(15, 15, 10, 10)

        For Each entity As GeoEntity In layeredCrossing.Entities
            entity.Association = layerB
        Next

        results = ClassifyGeoProfiles(
    {layeredOuter, layeredCrossing})

        check("Different layers - intersecting outside",
      results, layeredOuter,
      GeoProfileClassification.Ambiguous)

        check("Different layers - intersecting rectangle",
      results, layeredCrossing,
      GeoProfileClassification.Ambiguous)





        ' --------------------------------------------------
        ' Test 26: Ambiguity propagation through descendants
        ' --------------------------------------------------

        Dim ambiguousOuter As GeoProfile =
    makeRectangle(0, 0, 20, 20)

        Dim ambiguousCrossing As GeoProfile =
    makeRectangle(15, 15, 10, 10)

        Dim ambiguousChild As GeoProfile =
    makeRectangle(2, 2, 10, 10)

        Dim ambiguousGrandchild As GeoProfile =
    makeRectangle(4, 4, 4, 4)

        Dim unaffectedOuter As GeoProfile =
    makeRectangle(30, 30, 20, 20)

        Dim unaffectedInner As GeoProfile =
    makeRectangle(32, 32, 10, 10)

        results = ClassifyGeoProfiles(
    {ambiguousOuter,
     ambiguousCrossing,
     ambiguousChild,
     ambiguousGrandchild,
     unaffectedOuter,
     unaffectedInner})

        check("Ambiguous parent boundary",
      results, ambiguousOuter,
      GeoProfileClassification.Ambiguous)

        check("Ambiguous crossing boundary",
      results, ambiguousCrossing,
      GeoProfileClassification.Ambiguous)

        check("Ambiguity propagated to child",
      results, ambiguousChild,
      GeoProfileClassification.Ambiguous)

        check("Ambiguity propagated to grandchild",
      results, ambiguousGrandchild,
      GeoProfileClassification.Ambiguous)

        check("Unaffected outside boundary",
      results, unaffectedOuter,
      GeoProfileClassification.Outside)

        check("Unaffected inside boundary",
      results, unaffectedInner,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 27: Reverse input order
        ' --------------------------------------------------

        results = ClassifyGeoProfiles(
    {unaffectedInner,
     unaffectedOuter,
     ambiguousGrandchild,
     ambiguousChild,
     ambiguousCrossing,
     ambiguousOuter})

        check("Reverse order - ambiguous parent",
      results, ambiguousOuter,
      GeoProfileClassification.Ambiguous)

        check("Reverse order - ambiguous crossing",
      results, ambiguousCrossing,
      GeoProfileClassification.Ambiguous)

        check("Reverse order - ambiguous child",
      results, ambiguousChild,
      GeoProfileClassification.Ambiguous)

        check("Reverse order - ambiguous grandchild",
      results, ambiguousGrandchild,
      GeoProfileClassification.Ambiguous)

        check("Reverse order - unaffected outside",
      results, unaffectedOuter,
      GeoProfileClassification.Outside)

        check("Reverse order - unaffected inside",
      results, unaffectedInner,
      GeoProfileClassification.Inside)

        ' --------------------------------------------------
        ' Test 28: Ambiguity does not propagate upward
        ' --------------------------------------------------

        Dim enclosingBoundary As GeoProfile =
    makeRectangle(-10, -10, 50, 50)

        results = ClassifyGeoProfiles(
    {enclosingBoundary,
     ambiguousOuter,
     ambiguousCrossing,
     ambiguousChild,
     ambiguousGrandchild})

        check("Containing ancestor remains outside",
      results, enclosingBoundary,
      GeoProfileClassification.Outside)

        check("Contained intersecting boundary A",
      results, ambiguousOuter,
      GeoProfileClassification.Ambiguous)

        check("Contained intersecting boundary B",
      results, ambiguousCrossing,
      GeoProfileClassification.Ambiguous)

        check("Contained ambiguous child",
      results, ambiguousChild,
      GeoProfileClassification.Ambiguous)

        check("Contained ambiguous grandchild",
      results, ambiguousGrandchild,
      GeoProfileClassification.Ambiguous)

        Debug.WriteLine("------------------------------------")
        Debug.WriteLine($"Passed: {passed}")
        Debug.WriteLine($"Failed: {failed}")
        Debug.WriteLine("====================================")

    End Sub
    Public Function ClassifyGeoProfiles(
    profiles As IEnumerable(Of GeoProfile),
    Optional gapTolerance As Double = 0.000001
    ) As Dictionary(Of GeoProfile, GeoProfileClassification)

        Dim results As New Dictionary(
            Of GeoProfile, GeoProfileClassification)

        Dim paths As New Dictionary(
            Of GeoProfile, ToolShapes.IPath2D)

        Dim depths As New Dictionary(Of GeoProfile, Integer)

        Dim containers As New Dictionary(
    Of GeoProfile, List(Of GeoProfile))


        If profiles Is Nothing Then Return results

        ' --------------------------------------------------
        ' Prepare valid closed profiles
        ' --------------------------------------------------

        For Each profile As GeoProfile In profiles

            If profile Is Nothing Then Continue For

            If results.ContainsKey(profile) Then Continue For

            results.Add(profile, GeoProfileClassification.Invalid)

            If Not profile.IsClosed Then Continue For

            Dim path As ToolShapes.IPath2D =
                ConvertGeoProfileToPath(profile)

            If path Is Nothing OrElse
               Not path.IsClosed Then Continue For

            paths.Add(profile, path)
            depths.Add(profile, 0)
            containers.Add(profile, New List(Of GeoProfile))

            results(profile) = GeoProfileClassification.Outside

        Next

        ' --------------------------------------------------
        ' Evaluate each unique pair
        ' --------------------------------------------------

        Dim validProfiles As List(Of GeoProfile) =
            paths.Keys.ToList()

        For i As Integer = 0 To validProfiles.Count - 2

            Dim profileA As GeoProfile = validProfiles(i)

            For j As Integer = i + 1 To validProfiles.Count - 1

                Dim profileB As GeoProfile = validProfiles(j)

                Dim relationship As ToolShapes.ProfileContainmentResult =
                    ToolShapes.Geometry2D.ClassifyProfileContainment(
                        paths(profileA),
                        paths(profileB))

                Select Case relationship

                    Case ToolShapes.ProfileContainmentResult.Inside

                        depths(profileA) += 1
                        containers(profileA).Add(profileB)

                    Case ToolShapes.ProfileContainmentResult.Intersecting

                        results(profileA) =
                            GeoProfileClassification.Ambiguous

                        results(profileB) =
                            GeoProfileClassification.Ambiguous

                    Case ToolShapes.ProfileContainmentResult.Invalid

                        results(profileA) =
                            GeoProfileClassification.Ambiguous

                        results(profileB) =
                            GeoProfileClassification.Ambiguous

                    Case ToolShapes.ProfileContainmentResult.Outside

                        ' A is not inside B.
                        ' Determine whether B is inside A.

                        Dim reverseRelationship As ToolShapes.ProfileContainmentResult =
    ToolShapes.Geometry2D.ClassifyProfileContainment(
        paths(profileB),
        paths(profileA))

                        Select Case reverseRelationship

                            Case ToolShapes.ProfileContainmentResult.Inside

                                depths(profileB) += 1
                                containers(profileB).Add(profileA)

                            Case ToolShapes.ProfileContainmentResult.Intersecting,
                                 ToolShapes.ProfileContainmentResult.Invalid

                                results(profileA) =
                                    GeoProfileClassification.Ambiguous

                                results(profileB) =
                                    GeoProfileClassification.Ambiguous

                        End Select

                End Select

            Next

        Next


        ' --------------------------------------------------
        ' Propagate ambiguity through containment relationships
        ' --------------------------------------------------

        Dim ambiguityChanged As Boolean

        Do

            ambiguityChanged = False

            For Each profile As GeoProfile In validProfiles

                If results(profile) =
            GeoProfileClassification.Ambiguous Then Continue For

                For Each containingProfile As GeoProfile In containers(profile)

                    If results(containingProfile) =
                GeoProfileClassification.Ambiguous Then

                        results(profile) =
                    GeoProfileClassification.Ambiguous

                        ambiguityChanged = True

                        Exit For

                    End If

                Next

            Next

        Loop While ambiguityChanged

        ' --------------------------------------------------
        ' Resolve classification from containment depth
        ' --------------------------------------------------

        For Each profile As GeoProfile In validProfiles

            If results(profile) =
                GeoProfileClassification.Ambiguous Then Continue For

            If depths(profile) Mod 2 = 0 Then

                results(profile) =
                    GeoProfileClassification.Outside

            Else

                results(profile) =
                    GeoProfileClassification.Inside

            End If

        Next

        Return results

    End Function


    Public Sub TestContainmentPath()

        Dim passed As Integer = 0
        Dim failed As Integer = 0

        Debug.WriteLine("===== CONTAINMENT PATH TEST =====")

        Dim check As Action(Of String, Boolean) =
            Sub(description, success)

                If success Then
                    passed += 1
                    Debug.WriteLine("PASS | " & description)
                Else
                    failed += 1
                    Debug.WriteLine("FAIL | " & description)
                End If

            End Sub

        ' Construct a rectangle with a 0.002 closure gap.

        Dim p1 As New GeoPoint With {.X = 0, .Y = 0}
        Dim p2 As New GeoPoint With {.X = 10, .Y = 0}
        Dim p3 As New GeoPoint With {.X = 10, .Y = 10}
        Dim p4 As New GeoPoint With {.X = 0, .Y = 10}
        Dim p5 As New GeoPoint With {.X = 0, .Y = 0.002}

        Dim profile As New GeoProfile With {
            .IsClosed = True
        }

        Dim lines As GeoLine() = {
            New GeoLine With {.StartPoint = p1, .EndPoint = p2},
            New GeoLine With {.StartPoint = p2, .EndPoint = p3},
            New GeoLine With {.StartPoint = p3, .EndPoint = p4},
            New GeoLine With {.StartPoint = p4, .EndPoint = p5}
        }

        For Each line As GeoLine In lines

            profile.Entities.Add(line)

            profile.Segments.Add(
                New GeoProfileSegment With {
                    .Entity = line,
                    .Reversed = False
                })

        Next

        ' Test 1: Original conversion remains open.

        Dim originalPath As ToolShapes.IPath2D =
            ConvertGeoProfileToPath(profile)

        check("Original path exists",
              originalPath IsNot Nothing)

        check("Original path is geometrically open",
              originalPath IsNot Nothing AndAlso
              Not originalPath.IsClosed)

        ' Test 2: Gap exceeds permitted tolerance.

        Dim rejectedPath As ToolShapes.IPath2D =
            CreateContainmentPath(profile, 0.001)

        check("Gap rejected at tolerance 0.001",
              rejectedPath Is Nothing)

        ' Test 3: Gap is within permitted tolerance.

        Dim acceptedPath As ToolShapes.IPath2D =
            CreateContainmentPath(profile, 0.005)

        check("Gap accepted at tolerance 0.005",
              acceptedPath IsNot Nothing)

        check("Analytical path is closed",
              acceptedPath IsNot Nothing AndAlso
              acceptedPath.IsClosed)

        check("Exactly one analytical connector added",
              acceptedPath IsNot Nothing AndAlso
              acceptedPath.Segments.Count() = 5)

        ' Test 4: Original geometry remains unchanged.

        check("Original entity count unchanged",
              profile.Entities.Count = 4)

        check("Original segment count unchanged",
              profile.Segments.Count = 4)

        check("Original endpoint unchanged",
              Math.Abs(p5.Y - 0.002) < 0.000000001)

        check("Original endpoint reference unchanged",
              Object.ReferenceEquals(lines(3).EndPoint, p5))

        ' Test 5: Exact closure requires no connector.

        lines(3).EndPoint = p1

        Dim exactPath As ToolShapes.IPath2D =
            CreateContainmentPath(profile, 0.005)

        check("Exactly closed profile accepted",
              exactPath IsNot Nothing AndAlso
              exactPath.IsClosed)

        check("No unnecessary connector added",
              exactPath IsNot Nothing AndAlso
              exactPath.Segments.Count() = 4)

        ' Test 6: Intentionally open profile.

        profile.IsClosed = False

        check("Open profile excluded",
              CreateContainmentPath(profile, 0.005) Is Nothing)

        ' Test 7: Empty closed profile.

        Dim emptyProfile As New GeoProfile With {
            .IsClosed = True
        }

        check("Empty closed profile rejected",
              CreateContainmentPath(emptyProfile, 0.005) Is Nothing)

        ' --------------------------------------------------
        ' Test 8: Internal gap between consecutive segments
        ' --------------------------------------------------

        profile.IsClosed = True

        Dim internalGapPoint As New GeoPoint With {
    .X = 10,
    .Y = 0.003
}

        lines(1).StartPoint = internalGapPoint

        Dim internalGapPath As ToolShapes.IPath2D =
    CreateContainmentPath(profile, 0.005)

        check("Internal gap accepted",
      internalGapPath IsNot Nothing)

        check("Internal gap path is closed",
      internalGapPath IsNot Nothing AndAlso
      internalGapPath.IsClosed)

        check("Internal gap connector added",
      internalGapPath IsNot Nothing AndAlso
      internalGapPath.Segments.Count() = 5)

        check("Internal gap rejected at smaller tolerance",
      CreateContainmentPath(profile, 0.001) Is Nothing)

        ' --------------------------------------------------
        ' Test 9: Multiple gaps within the same profile
        ' --------------------------------------------------

        lines(3).EndPoint = p5

        Dim multipleGapPath As ToolShapes.IPath2D =
    CreateContainmentPath(profile, 0.005)

        check("Multiple gaps accepted",
      multipleGapPath IsNot Nothing)

        check("Multiple gap path is closed",
      multipleGapPath IsNot Nothing AndAlso
      multipleGapPath.IsClosed)

        check("Exactly two analytical connectors added",
      multipleGapPath IsNot Nothing AndAlso
      multipleGapPath.Segments.Count() = 6)

        ' --------------------------------------------------
        ' Test 10: Actual point containment
        ' --------------------------------------------------

        check("Center point is inside",
      multipleGapPath IsNot Nothing AndAlso
      ToolShapes.Geometry2D.IsPointInsidePath(
          multipleGapPath,
          New PointF(5.0F, 5.0F)))

        check("Point outside boundary",
      multipleGapPath IsNot Nothing AndAlso
      Not ToolShapes.Geometry2D.IsPointInsidePath(
          multipleGapPath,
          New PointF(15.0F, 5.0F)))

        check("Inside point aligned with internal gap",
      multipleGapPath IsNot Nothing AndAlso
      ToolShapes.Geometry2D.IsPointInsidePath(
          multipleGapPath,
          New PointF(5.0F, 0.0015F)))

        check("Outside point aligned with internal gap",
      multipleGapPath IsNot Nothing AndAlso
      Not ToolShapes.Geometry2D.IsPointInsidePath(
          multipleGapPath,
          New PointF(-1.0F, 0.0015F)))
        ' --------------------------------------------------
        ' Test 11: Original geometry preservation
        ' --------------------------------------------------

        check("Internal gap endpoint reference preserved",
      Object.ReferenceEquals(
          lines(1).StartPoint,
          internalGapPoint))

        check("Internal gap coordinate preserved",
      Math.Abs(internalGapPoint.Y - 0.003) <
      0.000000001)

        check("Multiple gap entity count unchanged",
      profile.Entities.Count = 4)

        check("Multiple gap segment count unchanged",
      profile.Segments.Count = 4)


        ' --------------------------------------------------
        ' Test 12: Mixed Line / Arc gaps
        ' --------------------------------------------------

        Dim arcStart As New GeoPoint With {
    .X = 5,
    .Y = 0
}

        Dim arcEnd As New GeoPoint With {
    .X = -5,
    .Y = 0
}

        Dim arcCenter As New GeoPoint With {
    .X = 0,
    .Y = 0
}

        Dim gapArc As New GeoArc With {
    .Center = arcCenter,
    .Radius = 5.0,
    .StartAngle = 0.0,
    .EndAngle = 180.0,
    .IsClockwise = False,
    .StartPoint = arcStart,
    .EndPoint = arcEnd
}

        Dim gapLine As New GeoLine With {
    .StartPoint = New GeoPoint With {
        .X = -4.998,
        .Y = 0
    },
    .EndPoint = New GeoPoint With {
        .X = 4.998,
        .Y = 0
    }
}

        Dim mixedProfile As New GeoProfile With {
    .IsClosed = True
}

        mixedProfile.Entities.Add(gapArc)
        mixedProfile.Entities.Add(gapLine)

        mixedProfile.Segments.Add(
    New GeoProfileSegment With {
        .Entity = gapArc,
        .Reversed = False
    })

        mixedProfile.Segments.Add(
    New GeoProfileSegment With {
        .Entity = gapLine,
        .Reversed = False
    })

        Dim mixedPath As ToolShapes.IPath2D =
    CreateContainmentPath(mixedProfile, 0.005)

        check("Mixed Line/Arc gaps accepted",
      mixedPath IsNot Nothing)

        check("Mixed Line/Arc path closed",
      mixedPath IsNot Nothing AndAlso
      mixedPath.IsClosed)

        check("Mixed path contains four segments",
      mixedPath IsNot Nothing AndAlso
      mixedPath.Segments.Count() = 4)

        check("Mixed gaps rejected at smaller tolerance",
      CreateContainmentPath(
          mixedProfile, 0.001) Is Nothing)

        ' --------------------------------------------------
        ' Test 13: Mixed path point containment
        ' --------------------------------------------------

        check("Point inside semicircular boundary",
      mixedPath IsNot Nothing AndAlso
      ToolShapes.Geometry2D.IsPointInsidePath(
          mixedPath,
          New PointF(0.0F, 2.0F)))

        check("Point below semicircular boundary",
      mixedPath IsNot Nothing AndAlso
      Not ToolShapes.Geometry2D.IsPointInsidePath(
          mixedPath,
          New PointF(0.0F, -1.0F)))

        check("Point outside curved boundary",
      mixedPath IsNot Nothing AndAlso
      Not ToolShapes.Geometry2D.IsPointInsidePath(
          mixedPath,
          New PointF(4.0F, 4.0F)))

        ' --------------------------------------------------
        ' Test 14: Original arc preservation
        ' --------------------------------------------------

        check("Original arc radius unchanged",
      gapArc.Radius = 5.0)

        check("Original arc angles unchanged",
      gapArc.StartAngle = 0.0 AndAlso
      gapArc.EndAngle = 180.0)

        check("Original arc center reference unchanged",
      Object.ReferenceEquals(
          gapArc.Center, arcCenter))

        check("Original arc endpoint references unchanged",
      Object.ReferenceEquals(
          gapArc.StartPoint, arcStart) AndAlso
      Object.ReferenceEquals(
          gapArc.EndPoint, arcEnd))

        check("Mixed profile entity count unchanged",
      mixedProfile.Entities.Count = 2)

        check("Mixed profile segment count unchanged",
      mixedProfile.Segments.Count = 2)





        ' --------------------------------------------------
        ' Test 15: Reversed mixed Line / Arc traversal
        ' --------------------------------------------------

        mixedProfile.Segments.Reverse()

        For Each segment As GeoProfileSegment In mixedProfile.Segments

            segment.Reversed = Not segment.Reversed

        Next

        Dim reversedMixedPath As ToolShapes.IPath2D =
    CreateContainmentPath(mixedProfile, 0.005)

        check("Reversed mixed profile accepted",
      reversedMixedPath IsNot Nothing)

        check("Reversed mixed path closed",
      reversedMixedPath IsNot Nothing AndAlso
      reversedMixedPath.IsClosed)

        check("Reversed mixed path contains four segments",
      reversedMixedPath IsNot Nothing AndAlso
      reversedMixedPath.Segments.Count() = 4)

        check("Reversed mixed gaps rejected at smaller tolerance",
      CreateContainmentPath(
          mixedProfile, 0.001) Is Nothing)

        ' --------------------------------------------------
        ' Test 16: Reversed path point containment
        ' --------------------------------------------------

        check("Reversed path - inside point",
      reversedMixedPath IsNot Nothing AndAlso
      ToolShapes.Geometry2D.IsPointInsidePath(
          reversedMixedPath,
          New PointF(0.0F, 2.0F)))

        check("Reversed path - point below boundary",
      reversedMixedPath IsNot Nothing AndAlso
      Not ToolShapes.Geometry2D.IsPointInsidePath(
          reversedMixedPath,
          New PointF(0.0F, -1.0F)))

        check("Reversed path - point outside curved boundary",
      reversedMixedPath IsNot Nothing AndAlso
      Not ToolShapes.Geometry2D.IsPointInsidePath(
          reversedMixedPath,
          New PointF(4.0F, 4.0F)))

        ' --------------------------------------------------
        ' Test 17: Original geometry preservation
        ' --------------------------------------------------

        check("Reversed traversal preserves native arc direction",
      Not gapArc.IsClockwise)

        check("Reversed traversal preserves native arc angles",
      gapArc.StartAngle = 0.0 AndAlso
      gapArc.EndAngle = 180.0)

        check("Reversed traversal preserves native arc endpoints",
      Object.ReferenceEquals(
          gapArc.StartPoint, arcStart) AndAlso
      Object.ReferenceEquals(
          gapArc.EndPoint, arcEnd))

        check("Reversed traversal preserves entity count",
      mixedProfile.Entities.Count = 2)

        check("Reversed traversal preserves segment count",
      mixedProfile.Segments.Count = 2)

        Debug.WriteLine("------------------------------------")
        Debug.WriteLine("Passed: " & passed)
        Debug.WriteLine("Failed: " & failed)
        Debug.WriteLine("====================================")

    End Sub

    Public Sub TestReliableProfileBounds()

        Dim passed As Integer = 0
        Dim failed As Integer = 0

        Debug.WriteLine("===== RELIABLE PROFILE BOUNDS TEST =====")

        Dim check As Action(Of String, Boolean) =
            Sub(description, success)

                If success Then
                    passed += 1
                    Debug.WriteLine("PASS | " & description)
                Else
                    failed += 1
                    Debug.WriteLine("FAIL | " & description)
                End If

            End Sub

        Dim bounds As (
    MinX As Double,
    MinY As Double,
    MaxX As Double,
    MaxY As Double)

        ' --------------------------------------------------
        ' Test 1: Nothing and empty profiles
        ' --------------------------------------------------

        check("Nothing profile rejected",
              Not TryGetReliableProfileBounds(Nothing, bounds))

        Dim profile As New GeoProfile With {
            .IsClosed = True
        }

        check("Empty profile rejected",
              Not TryGetReliableProfileBounds(profile, bounds))

        ' --------------------------------------------------
        ' Test 2: Line geometry without ordered segments
        ' --------------------------------------------------

        Dim line As New GeoLine With {
            .StartPoint = New GeoPoint With {.X = -5, .Y = -2},
            .EndPoint = New GeoPoint With {.X = 10, .Y = 8}
        }

        profile.Entities.Add(line)

        check("Line bounds accepted without segments",
              TryGetReliableProfileBounds(profile, bounds))

        check("Line minimum coordinates correct",
            bounds.MinX = -5.0F AndAlso
              bounds.MinY = -2.0F)

        check("Line maximum coordinates correct",
              bounds.MaxX = 10.0F AndAlso
             bounds.MaxY = 8.0F)

        profile.IsClosed = False

        check("Bounds independent of closure status",
              TryGetReliableProfileBounds(profile, bounds))

        ' --------------------------------------------------
        ' Test 3: Circle bounds
        ' --------------------------------------------------

        profile.Entities.Clear()

        Dim circle As New GeoCircle With {
            .Center = New GeoPoint With {.X = 10, .Y = 20},
            .Radius = 5.0
        }

        profile.Entities.Add(circle)

        check("Circle bounds accepted",
              TryGetReliableProfileBounds(profile, bounds))

        check("Circle bounds correct",
           bounds.MinX = 5.0F AndAlso
              bounds.MinY = 15.0F AndAlso
              bounds.MaxX = 15.0F AndAlso
              bounds.MaxY = 25.0F)

        ' --------------------------------------------------
        ' Test 4: Conservative arc bounds
        ' --------------------------------------------------

        profile.Entities.Clear()

        Dim arc As New GeoArc With {
            .Center = New GeoPoint With {.X = 10, .Y = 20},
            .Radius = 5.0,
            .StartAngle = 0.0,
            .EndAngle = 90.0,
            .IsClockwise = False,
            .StartPoint = New GeoPoint With {.X = 15, .Y = 20},
            .EndPoint = New GeoPoint With {.X = 10, .Y = 25}
        }

        profile.Entities.Add(arc)

        check("Arc bounds accepted",
              TryGetReliableProfileBounds(profile, bounds))

        check("Arc uses conservative full-circle bounds",
           bounds.MinX = 5.0F AndAlso
              bounds.MinY = 15.0F AndAlso
              bounds.MaxX = 15.0F AndAlso
              bounds.MaxY = 25.0F)

        ' Deliberately inconsistent stored endpoint.

        arc.EndPoint = New GeoPoint With {
            .X = 18,
            .Y = 28
        }

        check("Inconsistent arc endpoint bounds accepted",
              TryGetReliableProfileBounds(profile, bounds))

        check("Bounds include inconsistent arc endpoint",
              bounds.MaxX = 18.0F AndAlso
              bounds.MaxY = 28.0F)

        ' --------------------------------------------------
        ' Test 5: Combined geometry bounds
        ' --------------------------------------------------

        profile.Entities.Add(line)

        check("Combined geometry bounds accepted",
              TryGetReliableProfileBounds(profile, bounds))

        check("Combined geometry bounds correct",
             bounds.MinX = -5.0F AndAlso
              bounds.MinY = -2.0F AndAlso
              bounds.MaxX = 18.0F AndAlso
              bounds.MaxY = 28.0F)

        profile.Entities.Reverse()

        check("Bounds independent of entity order",
              TryGetReliableProfileBounds(profile, bounds) AndAlso
             bounds.MinX = -5.0F AndAlso
              bounds.MinY = -2.0F AndAlso
              bounds.MaxX = 18.0F AndAlso
              bounds.MaxY = 28.0F)

        ' --------------------------------------------------
        ' Test 6: Degenerate but bounded geometry
        ' --------------------------------------------------

        profile.Entities.Clear()

        Dim verticalLine As New GeoLine With {
            .StartPoint = New GeoPoint With {.X = 5, .Y = 0},
            .EndPoint = New GeoPoint With {.X = 5, .Y = 10}
        }

        profile.Entities.Add(verticalLine)

        check("Zero-width bounds accepted",
              TryGetReliableProfileBounds(profile, bounds))

        check("Zero-width bounds correct",
             bounds.MinX = 5.0F AndAlso
              (bounds.MaxX - bounds.MinX) = 0.0F AndAlso
              (bounds.MaxY - bounds.MinY) = 10.0F)



        ' --------------------------------------------------
        ' Test 7: Missing geometry references
        ' --------------------------------------------------

        profile.Entities.Clear()
        profile.Entities.Add(Nothing)

        check("Nothing entity rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        profile.Entities.Clear()

        Dim invalidLine As New GeoLine With {
    .StartPoint = Nothing,
    .EndPoint = New GeoPoint With {.X = 10, .Y = 10}
}

        profile.Entities.Add(invalidLine)

        check("Missing line start point rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        invalidLine.StartPoint =
    New GeoPoint With {.X = 0, .Y = 0}

        invalidLine.EndPoint = Nothing

        check("Missing line end point rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        ' --------------------------------------------------
        ' Test 8: Unsupported boundary geometry
        ' --------------------------------------------------

        profile.Entities.Clear()

        profile.Entities.Add(
    New GeoPoint With {.X = 5, .Y = 5})

        check("Standalone point rejected as boundary",
      Not TryGetReliableProfileBounds(profile, bounds))

        ' --------------------------------------------------
        ' Test 9: Invalid circle geometry
        ' --------------------------------------------------

        profile.Entities.Clear()
        profile.Entities.Add(circle)

        circle.Radius = 0.0

        check("Zero circle radius rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        circle.Radius = -5.0

        check("Negative circle radius rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        circle.Radius = Double.NaN

        check("NaN circle radius rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        circle.Radius = Double.PositiveInfinity

        check("Infinite circle radius rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        circle.Radius = 5.0
        circle.Center = Nothing

        check("Missing circle center rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        ' --------------------------------------------------
        ' Test 10: Invalid arc geometry
        ' --------------------------------------------------

        profile.Entities.Clear()
        profile.Entities.Add(arc)

        arc.Radius = 0.0

        check("Zero arc radius rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        arc.Radius = 5.0
        arc.StartAngle = Double.NaN

        check("NaN arc angle rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        arc.StartAngle = 0.0
        arc.EndAngle = Double.PositiveInfinity

        check("Infinite arc angle rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        arc.EndAngle = 90.0
        arc.EndPoint = Nothing

        check("Missing arc endpoint rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        ' --------------------------------------------------
        ' Test 11: Non-finite coordinates
        ' --------------------------------------------------

        profile.Entities.Clear()

        Dim invalidPoint As New GeoPoint With {
    .X = Double.NaN,
    .Y = 0
}

        invalidLine.StartPoint = invalidPoint
        invalidLine.EndPoint =
    New GeoPoint With {.X = 10, .Y = 10}

        profile.Entities.Add(invalidLine)

        check("NaN endpoint coordinate rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        invalidPoint.X = Double.PositiveInfinity

        check("Infinite endpoint coordinate rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        ' --------------------------------------------------
        ' Test 12: Partial bounds must not be returned
        ' --------------------------------------------------

        profile.Entities.Clear()
        profile.Entities.Add(line)

        check("Valid geometry bounds established",
      TryGetReliableProfileBounds(profile, bounds))

        profile.Entities.Add(invalidLine)

        check("Combined valid and invalid geometry rejected",
      Not TryGetReliableProfileBounds(profile, bounds))

        check("Failed calculation clears previous bounds",
      bounds.MinX = 0.0 AndAlso
      bounds.MinY = 0.0 AndAlso
      bounds.MaxX = 0.0 AndAlso
      bounds.MaxY = 0.0)
        ' --------------------------------------------------
        ' Test 13: Single precision investigation
        ' --------------------------------------------------

        profile.Entities.Clear()

        Dim decimalLine As New GeoLine With {
    .StartPoint = New GeoPoint With {
        .X = 0.1,
        .Y = 0.1
    },
    .EndPoint = New GeoPoint With {
        .X = 10.3,
        .Y = 20.7
    }
}

        profile.Entities.Add(decimalLine)

        Dim decimalBoundsAccepted As Boolean =
    TryGetReliableProfileBounds(profile, bounds)

        Debug.WriteLine(
    "INFO | Decimal bounds accepted: " &
    decimalBoundsAccepted.ToString())

        Debug.WriteLine(
    "INFO | Stored minimum X: " &
    CDbl(decimalLine.StartPoint.X).ToString("G17"))

        Debug.WriteLine(
    "INFO | Stored maximum X: " &
    CDbl(decimalLine.EndPoint.X).ToString("G17"))

        Debug.WriteLine(
    "INFO | Returned bounds: " &
    bounds.ToString())



        Debug.WriteLine("------------------------------------")
        Debug.WriteLine("Passed: " & passed)
        Debug.WriteLine("Failed: " & failed)
        Debug.WriteLine("====================================")

    End Sub

End Module