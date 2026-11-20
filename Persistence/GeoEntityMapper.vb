Public Class GeoEntityMapper


    Public Shared Function ToPointRecord(
    point As GeoPoint
) As GeoEntityRecord

        If point Is Nothing Then
            Throw New ArgumentNullException(NameOf(point))
        End If

        Return New GeoEntityRecord With {
        .ID = point.ID,
        .GeometryType = "GeoPoint",
        .X = point.X,
        .Y = point.Y
    }

    End Function


    Public Shared Function FromPointRecord(
    record As GeoEntityRecord
) As GeoPoint

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.GeometryType <> "GeoPoint" Then
            Throw New ArgumentException(
            "Record is not a GeoPoint.")
        End If

        Return New GeoPoint With {
        .ID = record.ID,
        .X = record.X,
        .Y = record.Y
    }

    End Function


    Public Shared Function ToLineRecord(
    line As GeoLine
) As GeoEntityRecord

        If line Is Nothing Then
            Throw New ArgumentNullException(NameOf(line))
        End If

        If line.StartPoint Is Nothing OrElse
       line.EndPoint Is Nothing Then

            Throw New ArgumentException(
            "GeoLine has a missing endpoint.")
        End If

        Return New GeoEntityRecord With {
        .ID = line.ID,
        .GeometryType = "GeoLine",
        .StartPointID = line.StartPoint.ID,
        .EndPointID = line.EndPoint.ID
    }

    End Function


    Public Shared Function FromLineRecord(
    record As GeoEntityRecord,
    pointsByID As Dictionary(Of Integer, GeoPoint)
) As GeoLine

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If record.GeometryType <> "GeoLine" Then
            Throw New ArgumentException(
            "Record is not a GeoLine.")
        End If

        If pointsByID Is Nothing Then
            Throw New ArgumentNullException(NameOf(pointsByID))
        End If

        If Not record.StartPointID.HasValue OrElse
       Not record.EndPointID.HasValue Then

            Throw New ArgumentException(
            "GeoLine record has missing endpoint IDs.")
        End If

        Dim startPoint As GeoPoint = Nothing
        Dim endPoint As GeoPoint = Nothing

        If Not pointsByID.TryGetValue(
        record.StartPointID.Value, startPoint) Then

            Throw New InvalidOperationException(
            "GeoLine start point could not be resolved.")
        End If

        If Not pointsByID.TryGetValue(
        record.EndPointID.Value, endPoint) Then

            Throw New InvalidOperationException(
            "GeoLine end point could not be resolved.")
        End If

        Return New GeoLine With {
        .ID = record.ID,
        .StartPoint = startPoint,
        .EndPoint = endPoint
    }

    End Function


    Public Shared Function ToCircleRecord(
    circle As GeoCircle
) As GeoEntityRecord

        If circle Is Nothing Then
            Throw New ArgumentNullException(NameOf(circle))
        End If

        If circle.Center Is Nothing Then
            Throw New ArgumentException(
            "GeoCircle has a missing center point.")
        End If

        Return New GeoEntityRecord With {
        .ID = circle.ID,
        .GeometryType = "GeoCircle",
        .CenterPointID = circle.Center.ID,
        .Radius = circle.Radius
    }

    End Function



End Class
