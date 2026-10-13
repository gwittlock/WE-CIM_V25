Imports System.Drawing

'==========================================================
' WmChainResolver
'
' Corresponds to legacy CWmChainResolver.
'
' Resolves intersecting/split chains by taking the subchains
' having the minimum interference index and coalescing
' contiguous subchains into integral chains.
'==========================================================
Public Class WmChainResolver

    Private ReadOnly _results As List(Of WmChain)
    Private _tol As Double

    Private Const Small As Double = 0.000001

    Public Sub New()

        _results = New List(Of WmChain)()
        _tol = Small

    End Sub

    '==========================================================
    ' Resolve
    '
    ' Corresponds to:
    '     CWmChainResolver::Resolve()
    '==========================================================
    Public Sub Resolve(
        chainList As List(Of WmChain),
        tol As Double)

        _results.Clear()

        _tol = tol

        ' The legacy resolver determines the number of
        ' intersection sets from the maximum SetNo.

        Dim maxSetNo As Integer = 0

        If chainList Is Nothing Then
            Return
        End If

        For Each chain As WmChain In chainList

            If chain Is Nothing Then
                Continue For
            End If

            If chain.SetNo > maxSetNo Then
                maxSetNo = chain.SetNo
            End If

        Next

        ' Resolve each intersection set independently.
        For setNo As Integer = 1 To maxSetNo

            Dim setChains As New List(Of WmChain)

            IntersectionSetBuild(
                setNo,
                chainList,
                setChains)

            MinimalLoopsBuild(setChains)

        Next

    End Sub

    '==========================================================
    ' Count
    '
    ' Corresponds to:
    '     CWmChainResolver::Count()
    '==========================================================
    Public ReadOnly Property Count As Integer
        Get
            Return _results.Count
        End Get
    End Property

    '==========================================================
    ' GetAt
    '
    ' Corresponds to:
    '     CWmChainResolver::GetAt()
    '==========================================================
    Public Function GetAt(index As Integer) As WmChain

        If index < 0 OrElse
           index >= _results.Count Then

            Return Nothing
        End If

        Return _results(index)

    End Function

    '==========================================================
    ' IntersectionSetBuild
    '
    ' Build the set of chains belonging to the specified
    ' intersection set.
    '==========================================================
    Private Sub IntersectionSetBuild(
        setNo As Integer,
        chains As List(Of WmChain),
        setChains As List(Of WmChain))

        If chains Is Nothing OrElse
           setChains Is Nothing Then

            Return
        End If

        For Each chain As WmChain In chains

            If chain Is Nothing Then
                Continue For
            End If

            If chain.SetNo = setNo Then
                setChains.Add(chain)
            End If

        Next

    End Sub

    '==========================================================
    ' MinimalLoopsBuild
    '
    ' Corresponds to:
    '     CWmChainResolver::MinimalLoopsBuild()
    '==========================================================
    Private Sub MinimalLoopsBuild(
        setChains As List(Of WmChain))

        If setChains Is Nothing OrElse
           setChains.Count = 0 Then

            Return
        End If

        Dim minIIndex As Integer =
            MinIIndexGet(setChains)

        ' Build the set of subchains having the minimum
        ' interference index.

        Dim subchns As New List(Of WmSubchn)

        MinSubchnsGet(
            setChains,
            minIIndex,
            subchns)

        MinChainsBuild(subchns)

    End Sub

    '==========================================================
    ' MinIIndexGet
    '
    ' Find the minimum interference index.
    '
    ' Terminal subchains are deliberately excluded, exactly
    ' as in the legacy implementation.
    '==========================================================
    Private Function MinIIndexGet(
        setChains As List(Of WmChain)) As Integer

        Dim minIIndex As Integer =
            Integer.MaxValue

        For Each chain As WmChain In setChains

            If chain Is Nothing Then
                Continue For
            End If

            Dim subchn As WmSubchn =
                chain.First

            ' First and Last are the chain sentinels.
            ' Actual subchains are linked through SubchnNext.

            While subchn IsNot Nothing AndAlso
                  Not Object.ReferenceEquals(subchn, chain.Last)

                If Not subchn.IsTerminal Then

                    Dim iIndex As Integer =
                        subchn.IIndex

                    If iIndex < minIIndex Then
                        minIIndex = iIndex
                    End If

                End If

                subchn = subchn.SubchnNext

            End While

        Next

        Return minIIndex

    End Function

    '==========================================================
    ' MinSubchnsGet
    '
    ' Collect all non-terminal subchains having the minimum
    ' interference index.
    '==========================================================
    Private Sub MinSubchnsGet(
        chains As List(Of WmChain),
        minIIndex As Integer,
        subchns As List(Of WmSubchn))

        If chains Is Nothing OrElse
           subchns Is Nothing Then

            Return
        End If

        For Each chain As WmChain In chains

            If chain Is Nothing Then
                Continue For
            End If

            Dim subchn As WmSubchn =
                chain.First

            While subchn IsNot Nothing AndAlso
                  Not Object.ReferenceEquals(subchn, chain.Last)

                If subchn.IIndex = minIIndex AndAlso
                   Not subchn.IsTerminal Then

                    subchns.Add(subchn)

                End If

                subchn = subchn.SubchnNext

            End While

        Next

    End Sub

    '==========================================================
    ' MinChainsBuild
    '
    ' Build integral resolved chains from the minimum-index
    ' subchains.
    '==========================================================
    Private Sub MinChainsBuild(
        subchns As List(Of WmSubchn))

        If subchns Is Nothing Then
            Return
        End If

        While subchns.Count > 0

            ' Legacy:
            '
            ' subchn = subchns->Remove(0);

            Dim subchn As WmSubchn =
                subchns(0)

            subchns.RemoveAt(0)

            If subchn Is Nothing Then
                Continue While
            End If

            ' Legacy only creates a result chain when Next()
            ' exists.
            If subchn.Next Is Nothing Then
                Continue While
            End If

            Dim chain As New WmChain()

            CopySubchnToChain(
                subchn,
                chain)

            _results.Add(chain)

            While True

                ' Find the next contiguous subchain within the
                ' specified tolerance.

                Dim nextSubchn As WmSubchn =
                    FindNextSubchn(
                        subchns,
                        subchn)

                If nextSubchn Is Nothing Then
                    Exit While
                End If

                Dim index As Integer =
                    subchns.IndexOf(nextSubchn)

                If index < 0 Then
                    Exit While
                End If

                subchns.RemoveAt(index)

                CopySubchnToChain(
                    nextSubchn,
                    chain)

                subchn = nextSubchn

            End While

        End While

    End Sub

    '==========================================================
    ' CopySubchnToChain
    '
    ' Equivalent to the useful portion of the legacy:
    '
    '     chain->CopyAppend((*subchn));
    '
    ' The current WmChain model does not have CopyAppend().
    ' Therefore copy the IPathSegment objects directly through
    ' the current WmChain.Append() API.
    '==========================================================
    Private Sub CopySubchnToChain(
        subchn As WmSubchn,
        chain As WmChain)

        If subchn Is Nothing OrElse
           chain Is Nothing Then

            Return
        End If

        Dim node As WmNode =
            subchn.Next

        While node IsNot Nothing AndAlso
              Not TypeOf node Is WmSubchn

            Dim element As WmElem =
                TryCast(node, WmElem)

            If element IsNot Nothing AndAlso
               element.Element IsNot Nothing Then

                chain.Append(element.Element)

            End If

            node = node.Next

        End While

    End Sub

    '==========================================================
    ' FindNextSubchn
    '
    ' Corresponds to:
    '     CWmChainResolver::FindNextSubchn()
    '
    ' IMPORTANT:
    '
    ' The legacy implementation intentionally does NOT use
    ' subchn->Other().

    ' It locates the next subchain by comparing the current
    ' EndPoint against each candidate StartPoint.
    '==========================================================
    Private Function FindNextSubchn(
        subchns As List(Of WmSubchn),
        subchn As WmSubchn) As WmSubchn

        If subchn Is Nothing OrElse
           subchns Is Nothing Then

            Return Nothing
        End If

        Dim endPoint As PointF =
            subchn.EndPoint

        For Each candidate As WmSubchn In subchns

            If candidate Is Nothing Then
                Continue For
            End If

            If Object.ReferenceEquals(
                candidate,
                subchn) Then

                Continue For
            End If

            Dim startPoint As PointF =
                candidate.StartPoint

            If PointsWithinTolerance(
                startPoint,
                endPoint,
                _tol) Then

                Return candidate

            End If

        Next

        Return Nothing

    End Function

    '==========================================================
    ' PointsWithinTolerance
    '==========================================================
    Private Shared Function PointsWithinTolerance(
        pointA As PointF,
        pointB As PointF,
        tolerance As Double) As Boolean

        Dim dx As Double =
            CDbl(pointA.X) - CDbl(pointB.X)

        Dim dy As Double =
            CDbl(pointA.Y) - CDbl(pointB.Y)

        Return (dx * dx + dy * dy) <=
               (tolerance * tolerance)

    End Function

End Class