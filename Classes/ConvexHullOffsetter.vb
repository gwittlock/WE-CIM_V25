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

            ' Tool profile is now validated and available
            ' for the convex-hull offset algorithm.

            Throw New NotImplementedException(
        "Convex hull toolpath offset has not been implemented.")

        End Function

    End Class

End Namespace