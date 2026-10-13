Imports System.Drawing

Namespace ToolShapes

    Public Class ConvexHullOffsetter
        Inherits ToolpathOffsetter

        Public Overrides Function Offset(
            inputPath As IPath2D,
            toolShape As ToolShape,
            offsetDirection As ProfileSide) As IPath2D

            If inputPath Is Nothing Then
                Throw New ArgumentNullException(NameOf(inputPath))
            End If

            If toolShape Is Nothing Then
                Throw New ArgumentNullException(NameOf(toolShape))
            End If

            Dim toolProfile As New ConvexToolProfile(
                toolShape.CuttingProfile)

            Dim partArcs As New List(Of ConvexHullArc)

            For Each segment As IPathSegment In inputPath.Segments
                partArcs.Add(
                    ConvexHullGeometry.ConvertSegment(segment))
            Next

            Dim offsetDirectionValue As Integer = 1

            Dim rawOffset As IReadOnlyList(Of ConvexHullArc) =
                OffsetConvexHull(
                    partArcs,
                    toolProfile.Arcs,
                    offsetDirectionValue)

            Throw New NotImplementedException(
                "Raw convex hull offset generated successfully. Finalize is not implemented yet.")

        End Function
    End Class

End Namespace