
Imports FabV25_WIN8.ToolShapes
Imports System.Drawing

Friend Class WmChainDegouger

    Private Const DEGOUGE_NORMAL_USAGE As Integer = &H0
    Private Const DEGOUGE_INCLUDE_ALL_ISLANDS As Integer = &H1
    Private Const DEGOUGE_PROPOGATE_IINDEX As Integer = &H2

    Private Const Small As Double = 0.000001

    Private _flags As Integer
    Private ReadOnly _results As List(Of WmChain)

    Public Sub New()

        _flags = DEGOUGE_NORMAL_USAGE
        _results = New List(Of WmChain)()

    End Sub

    Public Sub FlagsSet(flags As Integer)

        _flags = flags

    End Sub

    Public ReadOnly Property Count As Integer
        Get
            Return _results.Count
        End Get
    End Property

    Public Function GetAt(index As Integer) As WmChain

        If index < 0 OrElse
           index >= _results.Count Then

            Return Nothing
        End If

        Return _results(index)

    End Function

    '==========================================================
    ' Corresponds to:
    '
    ' CWmChainDegouger::Init(CWmChain* chain, double tol)
    '==========================================================
    Public Sub Init(
        chain As WmChain,
        tol As Double)

        If chain Is Nothing Then
            Throw New ArgumentNullException(NameOf(chain))
        End If

        _results.Clear()

        '======================================================
        ' Legacy:
        '
        ' intsctor.Intersect((*chain), (*chain), tol);
        '
        ' The current implementation builds the intersection
        ' records directly.
        '======================================================

        Dim segments As List(Of IPathSegment) =
            BuildSegmentList(chain)

        Dim intersections As New List(Of WmIntersectionRecord)
        Dim curveRecords As New List(Of WmCurveRecord)

        ConvexHullGeometry.BuildIntersectionRecords(
            segments,
            tol,
            intersections,
            curveRecords)

        '======================================================
        ' Legacy:
        '
        ' splitter.Split(&intsctor);
        '
        ' Current splitter operates directly on the records.
        '======================================================

        Dim splitter As New WmChainSplitter()

        splitter.Split(
            chain,
            curveRecords,
            intersections)

        '======================================================
        ' Legacy:
        '
        ' chainList.Append(chain);
        '======================================================

        Dim chainList As New List(Of WmChain)
        chainList.Add(chain)

        '======================================================
        ' Legacy:
        '
        ' if (m_flags & DEGOUGE_PROPOGATE_IINDEX)
        '     assigner.FlagsSet(ASSIGNER_PROPOGATE_IINDEX);
        '======================================================

        Dim assigner As New WmChainAssigner()

        If (_flags And DEGOUGE_PROPOGATE_IINDEX) <> 0 Then
            assigner.FlagsSet(&H1)
        End If

        If Not assigner.InterferenceIndicesAssign(chainList) Then
            Return
        End If

        '======================================================
        ' Legacy:
        '
        ' resolver.Resolve(chainList, (2*SMALL));
        '======================================================

        Dim resolver As New WmChainResolver()

        resolver.Resolve(
            chainList,
            2.0 * Small)

        '======================================================
        ' Copy the resolved chains into our result list.
        '======================================================

        CopyResults(resolver)

    End Sub

    '==========================================================
    ' Corresponds to:
    '
    ' CWmChainDegouger::Init(
    '     CWmChain* outer,
    '     CWmChainList* islands,
    '     double tol)
    '==========================================================
    Public Sub Init(
        outer As WmChain,
        islands As IReadOnlyList(Of WmChain),
        tol As Double)

        If outer Is Nothing Then
            Throw New ArgumentNullException(NameOf(outer))
        End If

        _results.Clear()

        '======================================================
        ' Legacy:
        '
        ' outer->PurgeSubchns();
        ' chainList.Append(outer);
        '======================================================

        outer.PurgeSubchns()

        Dim chainList As New List(Of WmChain)
        chainList.Add(outer)

        '======================================================
        ' We need one curve-record collection for every chain.
        '
        ' The legacy intersection constructor retained all of
        ' these records internally. The current splitter takes
        ' the records for one chain explicitly.
        '======================================================

        Dim chainRecords As New Dictionary(
            Of WmChain,
            List(Of WmCurveRecord))()

        Dim allIntersections As New List(Of WmIntersectionRecord)

        '======================================================
        ' Build the outer chain's initial record collection.
        '======================================================

        Dim outerSegments As List(Of IPathSegment) =
            BuildSegmentList(outer)

        Dim outerRecords As New List(Of WmCurveRecord)
        Dim outerIntersections As New List(Of WmIntersectionRecord)

        ConvexHullGeometry.BuildIntersectionRecords(
            outerSegments,
            tol,
            outerIntersections,
            outerRecords)

        chainRecords(outer) = outerRecords

        allIntersections.AddRange(outerIntersections)

        '======================================================
        ' Legacy:
        '
        ' for each island:
        '
        '     island->PurgeSubchns();
        '
        '     test intersection against every chain already
        '     accepted into chainList.
        '
        '     include island if it intersects, or if
        '     DEGOUGE_INCLUDE_ALL_ISLANDS is set.
        '======================================================

        If islands IsNot Nothing Then

            For Each island As WmChain In islands

                If island Is Nothing Then
                    Continue For
                End If

                island.PurgeSubchns()

                Dim doesIntersect As Boolean = False

                Dim islandSegments As List(Of IPathSegment) =
                    BuildSegmentList(island)

                Dim islandRecords As New List(Of WmCurveRecord)
                Dim islandIntersections As New List(Of WmIntersectionRecord)

                '==================================================
                ' We build the island's record list as we discover
                ' intersections. Each pair contributes records for
                ' both participating chains.
                '==================================================

                For Each existingChain As WmChain In chainList

                    If existingChain Is Nothing Then
                        Continue For
                    End If

                    Dim existingSegments As List(Of IPathSegment) =
                        BuildSegmentList(existingChain)

                    Dim recordsA As New List(Of WmCurveRecord)
                    Dim recordsB As New List(Of WmCurveRecord)
                    Dim pairIntersections As New List(Of WmIntersectionRecord)

                    Dim pairIntersects As Boolean =
                        ConvexHullGeometry.BuildIntersectionRecords(
                            existingSegments,
                            islandSegments,
                            tol,
                            pairIntersections,
                            recordsA,
                            recordsB)

                    If pairIntersects Then
                        doesIntersect = True
                    End If

                    '================================================
                    ' Merge records into the appropriate chain.
                    '================================================

                    MergeCurveRecords(
                        existingChain,
                        recordsA,
                        chainRecords)

                    MergeCurveRecords(
                        island,
                        recordsB,
                        chainRecords)

                    allIntersections.AddRange(pairIntersections)

                    islandIntersections.AddRange(pairIntersections)

                Next

                '==================================================
                ' Legacy:
                '
                ' if ((does_intersect == true) ||
                '     (m_flags & DEGOUGE_INCLUDE_ALL_ISLANDS))
                '     chainList.Append(island);
                '==================================================

                If doesIntersect OrElse
                   (_flags And DEGOUGE_INCLUDE_ALL_ISLANDS) <> 0 Then

                    chainList.Add(island)

                    If Not chainRecords.ContainsKey(island) Then
                        chainRecords(island) = islandRecords
                    End If

                End If

            Next

        End If

        '======================================================
        ' Split every chain using the records accumulated for
        ' that chain.
        '
        ' This replaces the legacy:
        '
        ' splitter.Split(&intsctor);
        '======================================================

        Dim splitter As New WmChainSplitter()

        For Each chain As WmChain In chainList

            If chain Is Nothing Then
                Continue For
            End If

            Dim records As List(Of WmCurveRecord) = Nothing

            If chainRecords.TryGetValue(chain, records) Then

                splitter.Split(
                    chain,
                    records,
                    allIntersections)

            End If

        Next

        '======================================================
        ' Assign interference indices.
        '======================================================

        Dim assigner As New WmChainAssigner()

        If (_flags And DEGOUGE_PROPOGATE_IINDEX) <> 0 Then
            assigner.FlagsSet(&H1)
        End If

        If Not assigner.InterferenceIndicesAssign(chainList) Then
            Return
        End If

        '======================================================
        ' Resolve the minimum loops.
        '======================================================

        Dim resolver As New WmChainResolver()

        resolver.Resolve(
            chainList,
            2.0 * Small)

        CopyResults(resolver)

    End Sub

    '==========================================================
    ' Build a simple segment list from the current WmChain.
    '==========================================================
    Private Function BuildSegmentList(
        chain As WmChain) As List(Of IPathSegment)

        Dim segments As New List(Of IPathSegment)

        If chain Is Nothing Then
            Return segments
        End If

        Dim index As Integer = 0

        While True

            Dim element As WmElem =
                chain.ElementAt(index)

            If element Is Nothing Then
                Exit While
            End If

            If element.Element IsNot Nothing Then
                segments.Add(element.Element)
            End If

            index += 1

        End While

        Return segments

    End Function

    '==========================================================
    ' Merge curve records belonging to a chain.
    '
    ' The intersection engine creates a fresh record list for
    ' each pair. A chain can participate in multiple pairwise
    ' intersections, so those records must be accumulated.
    '==========================================================
    Private Sub MergeCurveRecords(
        chain As WmChain,
        records As IReadOnlyList(Of WmCurveRecord),
        chainRecords As Dictionary(
            Of WmChain,
            List(Of WmCurveRecord)))

        If chain Is Nothing OrElse
           records Is Nothing Then

            Return
        End If

        Dim target As List(Of WmCurveRecord) = Nothing

        If Not chainRecords.TryGetValue(chain, target) Then

            target = New List(Of WmCurveRecord)

            chainRecords.Add(
                chain,
                target)

        End If

        For Each record As WmCurveRecord In records

            If record Is Nothing Then
                Continue For
            End If

            Dim existing As WmCurveRecord = Nothing

            For Each candidate As WmCurveRecord In target

                If candidate.SegmentIndex =
                   record.SegmentIndex Then

                    existing = candidate
                    Exit For

                End If

            Next

            If existing Is Nothing Then

                target.Add(record)

            Else

                '==================================================
                ' Merge the intersection extension data.
                '
                ' WmCurveRecord does not expose its internal list,
                ' so use Update() to reproduce the records without
                ' changing WmCurveRecord's encapsulation.
                '==================================================

                For index As Integer = 0 To record.Count - 1

                    Dim extData As WmCurveRecordExtData =
                        record.ExtData(index)

                    If extData Is Nothing Then
                        Continue For
                    End If

                    existing.Update(
                        extData.Intersection,
                        extData.UParam)

                Next

            End If

        Next

    End Sub

    '==========================================================
    ' Copy resolver results into our owned result list.
    '
    ' Corresponds to:
    '
    ' CWmChain* copy = new CWmChain();
    ' copy->AttribsPropogate(true);
    ' copy->CopyAppend(*resolved);
    ' m_results.Append(copy);
    '==========================================================
    Private Sub CopyResults(
        resolver As WmChainResolver)

        If resolver Is Nothing Then
            Return
        End If

        For index As Integer = 0 To resolver.Count - 1

            Dim resolved As WmChain =
                resolver.GetAt(index)

            If resolved Is Nothing Then
                Continue For
            End If

            Dim copy As New WmChain

            CopyChainElements(
                resolved,
                copy)

            _results.Add(copy)

        Next

    End Sub

    '==========================================================
    ' Copy all geometry elements from one chain to another.
    '==========================================================
    Private Sub CopyChainElements(
        source As WmChain,
        destination As WmChain)

        If source Is Nothing OrElse
           destination Is Nothing Then

            Return
        End If

        Dim index As Integer = 0

        While True

            Dim element As WmElem =
                source.ElementAt(index)

            If element Is Nothing Then
                Exit While
            End If

            If element.Element IsNot Nothing Then

                destination.Append(
                    element.Element)

            End If

            index += 1

        End While

    End Sub

End Class