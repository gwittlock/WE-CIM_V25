Imports FabV25_WIN8.ToolShapes

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
           profile.Segments.Count < 3 Then

            Return 0.0

        End If

        Dim area As Double = 0.0

        For i As Integer = 0 To profile.Segments.Count - 1

            Dim currentPoint As GeoPoint =
                GetSegmentStartPoint(profile.Segments(i))

            Dim nextPoint As GeoPoint =
                GetSegmentEndPoint(profile.Segments(i))

            If currentPoint Is Nothing OrElse
               nextPoint Is Nothing Then

                Return 0.0

            End If

            area +=
                (currentPoint.X * nextPoint.Y) -
                (nextPoint.X * currentPoint.Y)

        Next

        Return area / 2.0

    End Function

    Private Function GetGeoProfileBounds(
    profile As GeoProfile) As RectangleF

        If profile Is Nothing OrElse
           profile.Segments.Count = 0 Then

            Return RectangleF.Empty

        End If

        Dim minX As Double = Double.MaxValue
        Dim minY As Double = Double.MaxValue
        Dim maxX As Double = Double.MinValue
        Dim maxY As Double = Double.MinValue

        For Each segment As GeoProfileSegment In profile.Segments

            Dim startPt As GeoPoint =
                GetSegmentStartPoint(segment)

            Dim endPt As GeoPoint =
                GetSegmentEndPoint(segment)

            If startPt Is Nothing OrElse endPt Is Nothing Then
                Continue For
            End If

            minX = Math.Min(minX, Math.Min(startPt.X, endPt.X))
            minY = Math.Min(minY, Math.Min(startPt.Y, endPt.Y))

            maxX = Math.Max(maxX, Math.Max(startPt.X, endPt.X))
            maxY = Math.Max(maxY, Math.Max(startPt.Y, endPt.Y))

        Next

        If minX = Double.MaxValue Then
            Return RectangleF.Empty
        End If

        Return New RectangleF(
            CSng(minX),
            CSng(minY),
            CSng(maxX - minX),
            CSng(maxY - minY))

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
    profile As GeoProfile) As ToolShapes.IPath2D

        If profile Is Nothing OrElse Not profile.IsClosed Then
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

                path.Add(
                    New ToolShapes.ArcSegment2D(
                        New PointF(
                            CSng(circle.Center.X),
                            CSng(circle.Center.Y)),
                        circle.Radius,
                        0.0,
                        Math.PI * 2.0,
                        False))

            Else

                Return Nothing

            End If

        Next

        Return path

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


End Module