Public Module PartGeometryBuilder

    Public Function CreatePartGeometry(
    cadEntities As List(Of CadEntity),
    layerSetup As AppData.layersetup) As List(Of GeoEntity)

        Dim geometry As New List(Of GeoEntity)()

        Dim layers As New Dictionary(Of String, Layer)(
        StringComparer.OrdinalIgnoreCase)

        Dim points As New Dictionary(Of (Double, Double), GeoPoint)()



        For Each cad As CadEntity In cadEntities

            Debug.WriteLine(
    "NATIVE INPUT: " &
    cad.GetType().Name &
    " | CAD Layer: " &
    cad.LayerName)

            If Not TypeOf cad Is CadEntity.CadLine Then
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
        System.Drawing.Color.FromArgb(directive.Color),
        layers)

            Dim line As GeoLine = ConvertCadLine(
        DirectCast(cad, CadEntity.CadLine),
        layer,
        points)

            geometry.Add(line)

            Debug.WriteLine(
                "CAD Layer: " & cad.LayerName &
                " → CAM Layer: " & layer.Name)

        Next


        Dim geoProfiles As List(Of GeoProfile) =
    BuildGeoProfiles(geometry)

        Debug.WriteLine(
    "Native GeoProfile Count: " & geoProfiles.Count)

        Return geometry

    End Function

    Private Function BuildGeoProfiles(
    geometry As List(Of GeoEntity)) As List(Of GeoProfile)

        Dim profiles As New List(Of GeoProfile)()

        If geometry Is Nothing OrElse geometry.Count = 0 Then
            Return profiles
        End If

        '-----------------------------------------
        ' Step 1 - Create an initial profile
        ' for each native line.
        '-----------------------------------------

        For Each entity As GeoEntity In geometry

            If Not TypeOf entity Is GeoLine Then
                Continue For
            End If

            Dim profile As New GeoProfile With {
            .ID = profiles.Count + 1,
            .IsClosed = False
        }

            profile.Entities.Add(entity)
            profiles.Add(profile)

        Next


        '-----------------------------------------
        ' Step 2 - Merge compatible profiles.
        '
        ' TryMergeGeoProfiles validates:
        '   - Same Layer/Tool association
        '   - Shared free endpoint
        '   - No branching
        '-----------------------------------------

        Dim i As Integer = 0

        While i < profiles.Count

            Dim j As Integer = i + 1

            While j < profiles.Count

                If TryMergeGeoProfiles(
                profiles(i),
                profiles(j)) Then

                    profiles(i).Entities.AddRange(
                    profiles(j).Entities)

                    profiles.RemoveAt(j)

                    Continue While

                End If

                j += 1

            End While

            i += 1

        End While


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
            GetProfileEndpoints(profile)

            profile.IsClosed =
            (profile.Entities.Count >= 3 AndAlso
             endpoints.Count = 0)

        Next


        '-----------------------------------------
        ' Step 4 - Diagnostic output.
        '-----------------------------------------

        For Each profile As GeoProfile In profiles

            Dim endpoints As List(Of GeoPoint) =
            GetProfileEndpoints(profile)

            Debug.WriteLine(
            "GeoProfile " & profile.ID &
            " | Lines: " & profile.Entities.Count &
            " | Free Endpoints: " & endpoints.Count &
            " | IsClosed: " & profile.IsClosed)

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
    profile As GeoProfile) As List(Of GeoPoint)

        Dim counts As New Dictionary(Of GeoPoint, Integer)()

        For Each entity As GeoEntity In profile.Entities

            If Not TypeOf entity Is GeoLine Then
                Continue For
            End If

            Dim line As GeoLine = DirectCast(entity, GeoLine)

            For Each pt As GeoPoint In {
            line.StartPoint,
            line.EndPoint
        }

                If Not counts.ContainsKey(pt) Then
                    counts(pt) = 0
                End If

                counts(pt) += 1

            Next

        Next

        Return counts.
        Where(Function(item) item.Value = 1).
        Select(Function(item) item.Key).
        ToList()

    End Function

    Private Function CanMergeProfiles(
    profile1 As GeoProfile,
    profile2 As GeoProfile) As Boolean

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

        Dim pointCounts As New Dictionary(Of GeoPoint, Integer)()

        For Each profile As GeoProfile In {profile1, profile2}

            For Each entity As GeoEntity In profile.Entities

                If Not TypeOf entity Is GeoLine Then
                    Return False
                End If

                Dim line As GeoLine = DirectCast(entity, GeoLine)

                For Each pt As GeoPoint In {
                line.StartPoint,
                line.EndPoint
            }

                    If Not pointCounts.ContainsKey(pt) Then
                        pointCounts(pt) = 0
                    End If

                    pointCounts(pt) += 1

                    If pointCounts(pt) > 2 Then
                        Return False
                    End If

                Next

            Next

        Next

        Return True

    End Function


    Private Function TryMergeGeoProfiles(
    profile1 As GeoProfile,
    profile2 As GeoProfile) As Boolean

        If Not CanMergeProfiles(profile1, profile2) Then
            Return False
        End If

        Dim endpoints1 As List(Of GeoPoint) =
        GetProfileEndpoints(profile1)

        Dim endpoints2 As List(Of GeoPoint) =
        GetProfileEndpoints(profile2)

        If endpoints1.Count <> 2 OrElse
       endpoints2.Count <> 2 Then
            Return False
        End If

        For Each point1 As GeoPoint In endpoints1

            For Each point2 As GeoPoint In endpoints2

                If Object.ReferenceEquals(point1, point2) Then
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

    Private Function GetOrCreatePoint(
    x As Double,
    y As Double,
    points As Dictionary(Of (Double, Double), GeoPoint)) As GeoPoint

        Dim key = (x, y)
        Dim existingPoint As GeoPoint = Nothing

        If points.TryGetValue(key, existingPoint) Then
            Return existingPoint
        End If

        Dim newPoint As New GeoPoint With {
            .X = x,
            .Y = y
        }

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


End Module