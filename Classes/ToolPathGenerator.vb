Namespace ToolShapes

    Public NotInheritable Class ToolpathGenerator

        Private Sub New()
        End Sub

        ' --------------------------------
        ' Legacy / low-level offsets
        ' --------------------------------
        Public Shared Function OffsetPath(path As IPath2D, offset As Double) As IEnumerable(Of IToolMotion)
            Dim motions As New List(Of IToolMotion)
            For Each seg In path.Segments
                motions.Add(New LinearToolMotion(seg.StartPoint, seg.EndPoint, 100.0))
            Next
            Return motions
        End Function

        Public Shared Function SweepSquare(path As IPath2D, dsize As Double) As IEnumerable(Of IToolMotion)
            Return OffsetPath(path, dsize / 2.0)
        End Function

        Public Shared Function SweepRectangle(path As IPath2D, width As Double, height As Double) As IEnumerable(Of IToolMotion)
            Return OffsetPath(path, width / 2.0)
        End Function

        Public Shared Function OffsetWithCornerFillets(path As IPath2D, toolRadius As Double, cornerRadius As Double) As IEnumerable(Of IToolMotion)
            Return OffsetPath(path, toolRadius)
        End Function

        ' --------------------------------
        ' NEW: Feature-based automatic toolpath generation
        ' --------------------------------
        Public Shared Function GenerateToolpath(
            feature As IGeometricFeature,
            availableTools As IEnumerable(Of ToolShape)
        ) As IEnumerable(Of IToolMotion)

            If feature Is Nothing Then Throw New ArgumentNullException(NameOf(feature))
            If availableTools Is Nothing Then Throw New ArgumentNullException(NameOf(availableTools))

            ' Step 1: Select the best tool for this feature
            Dim tool As ToolShape = ToolSelection.ToolSelector.SelectBestTool(feature, availableTools)

            If tool Is Nothing Then
                ' No compatible tool found
                Return Enumerable.Empty(Of IToolMotion)()
            End If

            ' Step 2: Generate toolpath via the tool's decomposition logic
            Return tool.DecomposePath(feature.SourcePath)
        End Function

    End Class

End Namespace
