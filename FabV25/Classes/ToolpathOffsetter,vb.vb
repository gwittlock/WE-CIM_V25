Imports System.Drawing

Namespace ToolShapes

    Public MustInherit Class ToolpathOffsetter

        Protected Sub New()
        End Sub

        Public MustOverride Function Offset(
            inputPath As IPath2D,
            toolShape As ToolShape,
            offsetDirection As ProfileSide) As IPath2D

    End Class

End Namespace