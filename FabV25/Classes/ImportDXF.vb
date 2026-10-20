Imports IxMilia.Dxf
Imports IxMilia.Dxf.Entities

Public Class ImportDXF

    Public Function ImportDxf(path As String) As List(Of GeoEntity)
        Dim entities As New List(Of GeoEntity)
        Dim creator As New CreateGeo()
        Dim dxf = DxfFile.Load(path)

        For Each e In dxf.Entities

            Select Case e.EntityType

                Case DxfEntityType.Line


                    Dim l = CType(e, DxfLine)

                    Dim geoLine As GeoLine =
                        creator.CreateLine(
                            l.P1.X,
                            l.P1.Y,
                            l.P2.X,
                            l.P2.Y)

                    entities.Add(geoLine)

                Case DxfEntityType.Arc

                    Dim a = CType(e, DxfArc)

                    Dim startRadians As Double = a.StartAngle * Math.PI / 180.0
                    Dim endRadians As Double = a.EndAngle * Math.PI / 180.0

                    Dim startX As Double =
                        a.Center.X + a.Radius * Math.Cos(startRadians)
                    Dim startY As Double =
                        a.Center.Y + a.Radius * Math.Sin(startRadians)

                    Dim endX As Double =
                        a.Center.X + a.Radius * Math.Cos(endRadians)
                    Dim endY As Double =
                        a.Center.Y + a.Radius * Math.Sin(endRadians)

                    Dim geoArc As GeoArc =
                        creator.CreateArc(
                            startX,
                            startY,
                            endX,
                            endY,
                            a.Center.X,
                            a.Center.Y,
                            a.Radius,
                            a.StartAngle,
                            a.EndAngle,
                            False)

                    entities.Add(geoArc)

                Case DxfEntityType.Circle

                    Dim c = CType(e, DxfCircle)

                    Dim geoCircle As GeoCircle =
                        creator.CreateCircle(
                            c.Center.X,
                            c.Center.Y,
                            c.Radius)

                    entities.Add(geoCircle)



                Case DxfEntityType.Point

                    Dim p = CType(e, DxfModelPoint)

                    Dim geoPoint As GeoPoint =
                        creator.CreatePoint(
                            p.Location.X,
                            p.Location.Y)

                    entities.Add(geoPoint)


            End Select

        Next

        Return entities

    End Function


End Class