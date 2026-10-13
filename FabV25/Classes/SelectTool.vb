Imports FabV25_WIN8.ToolShapes

Public Class SelectTool
    Public Function SelectTool(
    feature As IGeometricFeature,
    tools As IEnumerable(Of ToolShape)
) As ToolShape

        Return tools.FirstOrDefault(
        Function(t) t.IsCompatibleWithFeature(feature)
    )

    End Function

End Class
