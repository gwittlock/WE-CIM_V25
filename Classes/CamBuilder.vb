Public Class CamBuilder
    Public Property CadEntityType As CadEntity.EntityType

    ''' <summary>
    ''' Build CAM profiles based on a layer setup.
    ''' </summary>
    ''' <param name="entities">List of CAD entities to process.</param>
    ''' <param name="layerSetup">The LayerSetup containing directives and parameters.</param>
    ''' <returns>List of CamProfile objects.</returns>
    Public Shared Function BuildProfilesV25(
        entities As List(Of CadEntity),
        layerSetup As AppData.LayerSetup
    ) As List(Of CamProfile)

        Dim profiles As New List(Of CamProfile)

        ' Track unprocessed entities
        Dim unprocessed = New HashSet(Of CadEntity)(entities)

        While unprocessed.Count > 0
            Dim profileEntities As New List(Of CadEntity)
            Dim current = unprocessed.First()
            profileEntities.Add(current)
            unprocessed.Remove(current)

            ' Expand the profile by chaining close entities
            Dim extended As Boolean = True
            While extended
                extended = False
                For Each candidate In unprocessed.ToList()
                    If AreEntitiesClose(profileEntities.Last(), candidate, layerSetup.GapTolerance) Then
                        profileEntities.Add(candidate)
                        unprocessed.Remove(candidate)
                        extended = True
                        Exit For
                    End If
                Next
            End While

            Dim firstEntity = profileEntities.FirstOrDefault()

            Dim camProfile As New CamProfile With {
    .Entities = profileEntities,
    .Direction = layerSetup.Direction,
    .ZLevelMode = layerSetup.ZLevelMode,
    .GapTolerance = layerSetup.GapTolerance,
    .CleanTolerance = layerSetup.CleanTolerance,
    .FilterTolerance = layerSetup.FilterTolerance,
    .SharpAngle = layerSetup.SharpAngle,
    .ProcessText = layerSetup.ProcessText,
    .RestrictOffset = layerSetup.Restrict_Offset,
    .LayerName = If(firstEntity IsNot Nothing, firstEntity.LayerName, ""),
    .DisplayColor = If(firstEntity IsNot Nothing, firstEntity.DisplayColor, Color.Black)
}

            ' Auto winding
            If camProfile.Direction = WindingDirectionEnum.Auto Then
                camProfile.Direction = ComputeAutoWindingDirection(camProfile)
            End If

            ' Geometry metrics
            camProfile.ComputeBoundingBox()
            camProfile.ComputeTotalLength()

            profiles.Add(camProfile)
        End While

        Return profiles
    End Function

    ''' <summary>
    ''' Placeholder function to compute Auto winding direction.
    ''' </summary>
    Private Shared Function ComputeAutoWindingDirection(profile As CamProfile) As WindingDirectionEnum
        Return WindingDirectionEnum.CW ' placeholder
    End Function

    ''' <summary>
    ''' Determines whether the endpoints of two CadEntities are within a given tolerance.
    ''' </summary>
    Public Shared Function AreEntitiesClose(e1 As CadEntity, e2 As CadEntity, tolerance As Double) As Boolean
        Dim points1 = GetEntityEndpoints(e1)   ' ((X,Y),(X,Y))
        Dim points2 = GetEntityEndpoints(e2)

        ' Compare start and end points of e1 with start and end points of e2
        Dim e1Points = {points1.Item1, points1.Item2}
        Dim e2Points = {points2.Item1, points2.Item2}

        For Each p1 In e1Points
            For Each p2 In e2Points
                Dim dx = p1.X - p2.X
                Dim dy = p1.Y - p2.Y
                If Math.Sqrt(dx * dx + dy * dy) <= tolerance Then
                    Return True
                End If
            Next
        Next

        Return False
    End Function


    ''' <summary>
    ''' Helper: returns start and end coordinates of a CadEntity.
    ''' </summary>
    Public Shared Function GetEntityEndpoints(entity As CadEntity) As ((X As Double, Y As Double), (X As Double, Y As Double))
        If entity Is Nothing Then
            Throw New ArgumentNullException(NameOf(entity))
        End If

        ' Return X and Y values as a tuple
        Dim startTuple = (X:=entity.StartPt.X, Y:=entity.StartPt.Y)
        Dim endTuple = (X:=entity.EndPt.X, Y:=entity.EndPt.Y)

        Return (startTuple, endTuple)
    End Function


    'Public Sub Validate()
    '    Select Case CadEntityType
    '        Case CadEntityType.Line
    '            If DxfLineReference Is Nothing Then
    '                Throw New InvalidOperationException("Line entity missing DxfLineReference")
    '            End If

    '        Case CadEntityType.Arc
    '            If DxfArcReference Is Nothing Then
    '                Throw New InvalidOperationException("Arc entity missing DxfArcReference")
    '            End If

    '        Case CadEntityType.Circle
    '            If DxfCircleReference Is Nothing Then
    '                Throw New InvalidOperationException("Circle entity missing DxfCircleReference")
    '            End If
    '    End Select
    'End Sub


End Class
