Imports System.Linq
Imports FabV25_WIN8.ToolShapes
Imports ToolShapes

Namespace ToolSelection

    ''' <summary>
    ''' Centralized authority for selecting the best tool for a given feature.
    ''' </summary>
    Public NotInheritable Class ToolSelector

        Private Sub New()
            ' Prevent instantiation
        End Sub

        ''' <summary>
        ''' Select the best compatible tool from a list of available tools.
        ''' </summary>
        ''' <param name="feature">The geometric feature to machine.</param>
        ''' <param name="availableTools">List of available ToolShape objects.</param>
        ''' <returns>The most suitable ToolShape, or Nothing if none are compatible.</returns>
        Public Shared Function SelectBestTool(
            feature As IGeometricFeature,
            availableTools As IEnumerable(Of ToolShape)
        ) As ToolShape

            If feature Is Nothing Then Throw New ArgumentNullException(NameOf(feature))
            If availableTools Is Nothing Then Throw New ArgumentNullException(NameOf(availableTools))

            ' Phase 1: Filter by capability (IsCompatibleWithFeature)
            Dim compatibleTools = availableTools.
                Where(Function(t) t.IsCompatibleWithFeature(feature)).
                ToList()

            If compatibleTools.Count = 0 Then
                ' No compatible tool found
                Return Nothing
            End If

            ' Phase 2: Rank tools by EffectiveDiameter (smallest preferred)
            ' This is a simple heuristic; could be enhanced later
            Dim bestTool = compatibleTools.
                OrderBy(Function(t) t.EffectiveDiameter).
                First()

            Return bestTool

        End Function

    End Class

End Namespace
