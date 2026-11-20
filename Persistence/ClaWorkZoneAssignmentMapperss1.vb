
Public Class WorkZoneAssignmentMapper

    Public Shared Function ToRecord(
        assignment As MM3WorkZoneAssignment
    ) As WorkZoneAssignmentRecord


        If assignment Is Nothing OrElse
           assignment.Entity Is Nothing Then

            Throw New ArgumentException(
                "WorkZone assignment has no entity.")
        End If

        Dim record As New WorkZoneAssignmentRecord With {
            .WorkZoneIndex = assignment.WorkZoneIndex
        }

        If TypeOf assignment.Entity Is GeoEntity Then

            Dim geo = DirectCast(assignment.Entity, GeoEntity)

            record.EntityType = "GeoEntity"
            record.EntityID = geo.ID

        ElseIf TypeOf assignment.Entity Is MachiningFeature Then

            Dim feature = DirectCast(
                assignment.Entity,
                MachiningFeature)

            record.EntityType = "MachiningFeature"
            record.EntityID = feature.ID

        Else

            Throw New NotSupportedException(
                "Unsupported WorkZone assignment entity type: " &
                assignment.Entity.GetType().FullName)

        End If

        Return record

    End Function


    Public Shared Function FromRecord(
    record As WorkZoneAssignmentRecord,
    model As MM3
) As MM3WorkZoneAssignment

        If record Is Nothing Then
            Throw New ArgumentNullException(NameOf(record))
        End If

        If model Is Nothing Then
            Throw New ArgumentNullException(NameOf(model))
        End If

        Dim entity As isSelectable = Nothing

        Select Case record.EntityType

            Case "GeoEntity"

                entity = model.PartGeometry.
                FirstOrDefault(Function(g) g.ID = record.EntityID)

            Case "MachiningFeature"

                entity = model.ManufacturingGeometry.
                FirstOrDefault(Function(f) f.ID = record.EntityID)

            Case Else

                Throw New NotSupportedException(
                "Unsupported WorkZone assignment entity type: " &
                record.EntityType)

        End Select

        If entity Is Nothing Then
            Throw New InvalidOperationException(
            "WorkZone assignment references a missing entity: " &
            record.EntityType & " ID=" & record.EntityID)
        End If

        Return New MM3WorkZoneAssignment With {
        .WorkZoneIndex = record.WorkZoneIndex,
        .Entity = entity
    }

    End Function

End Class
