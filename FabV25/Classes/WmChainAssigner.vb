Imports FabV25_WIN8.ToolShapes

Friend Class WmChainAssigner

    Private Const ASSIGNER_NORMAL_USAGE As Integer = &H0
    Private Const ASSIGNER_PROPOGATE_IINDEX As Integer = &H1

    Private Const Small As Double = 0.000001
    Private Const VectorSmall As Double = 0.000001
    Private Const TwoPi As Double = 2.0 * Math.PI
    Private Const HalfPi As Double = 0.5 * Math.PI

    ' Corresponds to:
    '     static double FACTOR = 1.5e-4;
    '     static double DELTA = FACTOR * DEG2RAD;
    Private Const TangentAdjustFactor As Double = 0.00015

    Private ReadOnly _used As New List(Of WmChain)
    Private _flags As Integer

    Private Enum IntersectionType
        Undefined
        Engage
        Disengage
        Graze
        LeftExitSame
        LeftExitOpposite
        LeftEntrySame
        LeftEntryOpposite
        RightExitSame
        RightExitOpposite
        RightEntrySame
        RightEntryOpposite
    End Enum

    Private Class IntersectionMapEntry

        Public ReadOnly Property Pattern As String
        Public ReadOnly Property Type As IntersectionType

        Public Sub New(
        pattern As String,
        type As IntersectionType)

            Me.Pattern = pattern
            Me.Type = type
        End Sub

    End Class

    Private Shared ReadOnly IntersectionMap As IntersectionMapEntry() =
{
    New IntersectionMapEntry("adbc", IntersectionType.Engage),
    New IntersectionMapEntry("acbd", IntersectionType.Disengage),
    New IntersectionMapEntry("||||", IntersectionType.Graze),
    New IntersectionMapEntry("abdc", IntersectionType.Graze),
    New IntersectionMapEntry("abcd", IntersectionType.Graze),
    New IntersectionMapEntry("acdb", IntersectionType.Graze),
    New IntersectionMapEntry("adcb", IntersectionType.Graze),
    New IntersectionMapEntry("ab||", IntersectionType.Graze),
    New IntersectionMapEntry("||cd", IntersectionType.Graze),
    New IntersectionMapEntry("||dc", IntersectionType.Graze),
    New IntersectionMapEntry("a||b", IntersectionType.Graze),
    New IntersectionMapEntry("||bd", IntersectionType.LeftExitSame),
    New IntersectionMapEntry("||bc", IntersectionType.LeftExitOpposite),
    New IntersectionMapEntry("a||c", IntersectionType.LeftEntrySame),
    New IntersectionMapEntry("a||d", IntersectionType.LeftEntryOpposite),
    New IntersectionMapEntry("||db", IntersectionType.RightExitSame),
    New IntersectionMapEntry("||cb", IntersectionType.RightExitOpposite),
    New IntersectionMapEntry("ac||", IntersectionType.RightEntrySame),
    New IntersectionMapEntry("ad||", IntersectionType.RightEntryOpposite),
    New IntersectionMapEntry("", IntersectionType.Undefined)
    }

    Public Sub New()
        _flags = ASSIGNER_NORMAL_USAGE
    End Sub

    Public Sub FlagsSet(flags As Integer)
        _flags = flags
    End Sub

    '==========================================================
    ' Assign interference indices to all connected chains.
    '
    ' Corresponds to:
    '     CWmChainAssigner::InterferenceIndicesAssign()
    '==========================================================
    Public Function InterferenceIndicesAssign(
        chainList As IReadOnlyList(Of WmChain)) As Boolean

        If chainList Is Nothing Then
            Return False
        End If

        Dim status As Boolean = True

        Dim setNo As Integer = 0
        Dim nChains As Integer = chainList.Count

        _used.Clear()

        For index As Integer = 0 To nChains - 1

            Dim chain As WmChain = chainList(index)

            If chain Is Nothing Then
                Continue For
            End If

            If Not Used(chain) Then

                setNo += 1

                Dim intersectionSet As New List(Of WmChain)

                IntersectionSetBuild(
                    chain,
                    setNo,
                    intersectionSet)

                For jndx As Integer = 0 To intersectionSet.Count - 1

                    Dim iChain As WmChain =
                        intersectionSet(jndx)

                    If iChain Is Nothing Then
                        Continue For
                    End If

                    If Not InterferenceIndexAssign(iChain) Then
                        status = False
                        Exit For
                    End If

                    Mark(iChain)
                Next

                If Not status Then
                    Exit For
                End If
            End If
        Next

        If Not status Then
            Return False
        End If

        '======================================================
        ' Legacy special handling for closed chains.
        '======================================================
        For index As Integer = 0 To nChains - 1

            Dim chain As WmChain = chainList(index)

            If chain Is Nothing OrElse
               Not chain.IsClosed Then

                Continue For
            End If

            Dim lastSubchn As WmSubchn = chain.Last

            If lastSubchn Is Nothing OrElse
               lastSubchn.IsAssigned Then

                Continue For
            End If

            Dim previousSubchn As WmSubchn =
                lastSubchn.SubchnPrev

            If previousSubchn Is Nothing Then
                Continue For
            End If

            Dim firstSubchn As WmSubchn =
                chain.First

            If firstSubchn Is Nothing OrElse
               firstSubchn.IsAssigned Then

                Continue For
            End If

            firstSubchn.IIndex =
                previousSubchn.IIndex
        Next

        Return True
    End Function

    '==========================================================
    ' Build the connected set of chains linked through
    ' subchain intersections.
    '
    ' Corresponds to:
    '     CWmChainAssigner::IntersectionSetBuild()
    '==========================================================
    Private Sub IntersectionSetBuild(
        chain As WmChain,
        setNo As Integer,
        intersectionSet As List(Of WmChain))

        If chain Is Nothing OrElse
           intersectionSet Is Nothing Then

            Return
        End If

        intersectionSet.Clear()

        AddToIntersectionSet(
            chain,
            setNo,
            intersectionSet)

        Dim index As Integer = 0

        While index < intersectionSet.Count

            Dim candidate As WmChain =
                intersectionSet(index)

            If candidate IsNot Nothing Then

                Dim subchn As WmSubchn =
                    candidate.First

                While subchn IsNot Nothing

                    Dim other As WmSubchn =
                        subchn.Other()

                    If other IsNot Nothing Then

                        Dim owner As WmChain =
                            other.Owner

                        If owner IsNot Nothing Then

                            AddToIntersectionSet(
                                owner,
                                setNo,
                                intersectionSet)
                        End If
                    End If

                    subchn =
                        subchn.SubchnNext
                End While
            End If

            index += 1
        End While
    End Sub

    '==========================================================
    ' Corresponds to CWmChainList::ConditionalAppend().
    '==========================================================
    Private Sub AddToIntersectionSet(
        chain As WmChain,
        setNo As Integer,
        intersectionSet As List(Of WmChain))

        If chain Is Nothing OrElse
           intersectionSet Is Nothing Then

            Return
        End If

        chain.SetNo = setNo

        For Each existing As WmChain In intersectionSet

            If Object.ReferenceEquals(existing, chain) Then
                Return
            End If
        Next

        intersectionSet.Add(chain)
    End Sub

    '==========================================================
    ' Corresponds to CWmChainAssigner::Used().
    '==========================================================
    Private Function Used(chain As WmChain) As Boolean

        If chain Is Nothing Then
            Return False
        End If

        For Each usedChain As WmChain In _used

            If Object.ReferenceEquals(
                chain,
                usedChain) Then

                Return True
            End If
        Next

        Return False
    End Function

    '==========================================================
    ' Corresponds to CWmChainAssigner::Mark().
    '==========================================================
    Private Sub Mark(chain As WmChain)

        If chain Is Nothing Then
            Return
        End If

        If Not Used(chain) Then
            _used.Add(chain)
        End If
    End Sub

    '==========================================================
    ' Assign interference indices to one chain.
    '
    ' Corresponds to:
    '     CWmChainAssigner::InterferenceIndexAssign(CWmChain*)
    '==========================================================
    Private Function InterferenceIndexAssign(
        chain As WmChain) As Boolean

        If chain Is Nothing Then
            Return False
        End If

        Dim start As WmSubchn = Nothing
        Dim subchn As WmSubchn = Nothing

        FirstAssignedFind(
            chain,
            subchn)

        Dim iIndex As Integer = 0

        If subchn Is Nothing Then

            subchn = chain.First

            If subchn Is Nothing Then
                Return True
            End If

            subchn.IIndex = iIndex

        Else

            iIndex = subchn.IIndex
        End If

        start = subchn

        While True

            Dim nextSubchn As WmSubchn =
                subchn.SubchnNext

            If nextSubchn Is Nothing Then
                nextSubchn = chain.First
            End If

            subchn = nextSubchn

            If subchn Is Nothing Then
                Exit While
            End If

            If Not subchn.IsAssigned Then

                If Not InterferenceIndexAssign(
                    subchn,
                    iIndex) Then

                    Return False
                End If
            End If

            If Object.ReferenceEquals(
                subchn,
                start) Then

                Exit While
            End If

            iIndex = subchn.IIndex
        End While

        Return True
    End Function

    '==========================================================
    ' Assign an index to one subchain.
    '
    ' Corresponds to:
    '     CWmChainAssigner::InterferenceIndexAssign(CWmSubchn*)
    '==========================================================
    Private Function InterferenceIndexAssign(
        subchn As WmSubchn,
        iIndex As Integer) As Boolean

        If subchn Is Nothing Then
            Return False
        End If

        Dim count As Integer =
            subchn.Count

        If count = 0 Then

            If (_flags And ASSIGNER_PROPOGATE_IINDEX) <> 0 Then

                Dim owner As WmChain =
                    subchn.Owner

                If owner IsNot Nothing AndAlso
                   (Object.ReferenceEquals(
                        subchn,
                        owner.First) OrElse
                    Object.ReferenceEquals(
                        subchn,
                        owner.Last)) Then

                    subchn.IIndex = iIndex
                End If
            End If

        ElseIf count = 2 Then

            Return SimpleIindexAssign(
                subchn,
                iIndex)

        ElseIf (count Mod 2) = 0 Then

            Return ComplexIindexAssign(
                subchn,
                iIndex)

        Else

            ' Legacy returns STATUS_WARNING.
            ' No index change is made.
            Return True
        End If

        Return True
    End Function

    '==========================================================
    ' Corresponds to:
    '     CWmChainAssigner::SimpleIindexAssign()
    '==========================================================
    Private Function SimpleIindexAssign(
        subchn As WmSubchn,
        iIndex As Integer) As Boolean

        Dim aTan As Double = 0.0
        Dim bTan As Double = 0.0
        Dim cTan As Double = 0.0
        Dim dTan As Double = 0.0

        Dim tangentStatus As String = ""

        Dim status As Boolean = True

        Dim other As WmSubchn = Nothing

        If subchn IsNot Nothing Then
            other = subchn.Sister()
        End If

        Dim intersectionType As IntersectionType =
            IntersectionType.Undefined

        If subchn IsNot Nothing AndAlso
           other IsNot Nothing Then

            tangentStatus =
                TangentsGet(
                    subchn,
                    other,
                    aTan,
                    bTan,
                    cTan,
                    dTan)

            intersectionType =
                IntersectionType.Graze

            If tangentStatus = "ABCD" Then

                intersectionType =
                    IntersectionClassify(
                        aTan,
                        bTan,
                        cTan,
                        dTan)
            End If
        End If

        Select Case intersectionType

            Case IntersectionType.Engage

                subchn.IIndex = iIndex + 1
                other.IIndex = iIndex - 1

            Case IntersectionType.Disengage

                subchn.IIndex = iIndex - 1
                other.IIndex = iIndex + 1

            Case IntersectionType.Graze

                subchn.IIndex = iIndex
                other.IIndex = iIndex

            Case Else

                If subchn IsNot Nothing Then
                    subchn.IIndex = iIndex
                End If

                If other IsNot Nothing Then
                    other.IIndex = iIndex
                End If

        End Select

        Return status
    End Function

    '==========================================================
    ' Legacy implementation reports an internal error here.
    '
    ' We do not invent the missing algorithm.
    '==========================================================
    Private Function ComplexIindexAssign(
        subchn As WmSubchn,
        iIndex As Integer) As Boolean

        Return False
    End Function

    '==========================================================
    ' Corresponds to:
    '     CWmChainAssigner::FirstAssignedFind()
    '==========================================================
    Private Sub FirstAssignedFind(
        chain As WmChain,
        ByRef result As WmSubchn)

        result = Nothing

        If chain Is Nothing Then
            Return
        End If

        Dim subchn As WmSubchn =
            chain.First

        While subchn IsNot Nothing

            If subchn.IsAssigned Then
                result = subchn
                Return
            End If

            subchn =
                subchn.SubchnNext
        End While
    End Sub

    '==========================================================
    ' Corresponds to:
    '     CWmChainAssigner::TangentsGet()
    '==========================================================
    Private Function TangentsGet(
        subchnA As WmSubchn,
        subchnB As WmSubchn,
        ByRef aTan As Double,
        ByRef bTan As Double,
        ByRef cTan As Double,
        ByRef dTan As Double) As String

        Dim elemA As WmElem =
            Nothing

        Dim elemB As WmElem =
            Nothing

        Dim elemC As WmElem =
            Nothing

        Dim elemD As WmElem =
            Nothing

        If subchnA IsNot Nothing Then

            elemA =
                TryCast(
                    subchnA.Prev,
                    WmElem)

            elemB =
                TryCast(
                    subchnA.Next,
                    WmElem)
        End If

        If subchnB IsNot Nothing Then

            elemC =
                TryCast(
                    subchnB.Prev,
                    WmElem)

            elemD =
                TryCast(
                    subchnB.Next,
                    WmElem)
        End If

        Dim status As String =
            String.Concat(
                If(elemA Is Nothing, "-", "A"),
                If(elemB Is Nothing, "-", "B"),
                If(elemC Is Nothing, "-", "C"),
                If(elemD Is Nothing, "-", "D"))

        If status = "ABCD" Then

            aTan =
                TangentGet(
                    elemA,
                    True)

            bTan =
                TangentGet(
                    elemB,
                    False)

            cTan =
                TangentGet(
                    elemC,
                    True)

            dTan =
                TangentGet(
                    elemD,
                    False)
        End If

        Return status
    End Function

    '==========================================================
    ' Corresponds to:
    '     CWmChainAssigner::TangentGet()
    '
    ' Returns the tangent direction as a radians angle.
    '==========================================================
    Private Function TangentGet(
        node As WmElem,
        entry As Boolean) As Double

        If node Is Nothing OrElse
           node.Element Is Nothing Then

            Return 0.0
        End If

        Dim segment As IPathSegment =
            node.Element

        Dim tangentAngle As Double

        Dim line As LineSegment2D =
            TryCast(segment, LineSegment2D)

        If line IsNot Nothing Then

            Dim dx As Double =
                CDbl(line.EndPoint.X) -
                CDbl(line.StartPoint.X)

            Dim dy As Double =
                CDbl(line.EndPoint.Y) -
                CDbl(line.StartPoint.Y)

            tangentAngle =
                Math.Atan2(dy, dx)

        Else

            Dim arc As ArcSegment2D =
                TryCast(segment, ArcSegment2D)

            If arc Is Nothing Then
                Return 0.0
            End If

            Dim radialAngle As Double

            If entry Then

                radialAngle =
                    Math.Atan2(
                        CDbl(arc.EndPoint.Y) -
                        CDbl(arc.Center.Y),
                        CDbl(arc.EndPoint.X) -
                        CDbl(arc.Center.X))

            Else

                radialAngle =
                    Math.Atan2(
                        CDbl(arc.StartPoint.Y) -
                        CDbl(arc.Center.Y),
                        CDbl(arc.StartPoint.X) -
                        CDbl(arc.Center.X))

            End If

            Dim direction As Double

            If arc.IsClockwise Then
                direction = -1.0
            Else
                direction = 1.0
            End If

            tangentAngle =
                radialAngle +
                (direction * HalfPi)

            ' Legacy:
            '
            ' if (arc->Dir() > 0)
            '     adjust = ((entry) ? -1 : 1) * DELTA;
            ' else
            '     adjust = ((entry) ? 1 : -1) * DELTA;
            '
            ' Dir > 0 corresponds to CCW.
            Dim delta As Double =
                TangentAdjustFactor *
                (Math.PI / 180.0)

            If direction > 0.0 Then

                If entry Then
                    tangentAngle -= delta
                Else
                    tangentAngle += delta
                End If

            Else

                If entry Then
                    tangentAngle += delta
                Else
                    tangentAngle -= delta
                End If

            End If
        End If

        If entry Then
            tangentAngle += Math.PI
        End If

        Return NormalizeAngle(tangentAngle)
    End Function

    '==========================================================
    ' Corresponds to:
    '     CWmChainAssigner::IntersectionClassify()
    '==========================================================
    Private Function IntersectionClassify(
        aTan As Double,
        bTan As Double,
        cTan As Double,
        dTan As Double) As IntersectionType

        Dim angle() As Double =
        {
            aTan,
            bTan,
            cTan,
            dTan
        }

        Dim id() As Char =
        {
            "a"c,
            "b"c,
            "c"c,
            "d"c
        }

        '======================================================
        ' Normalize the other angles into the positive angular
        ' direction relative to angle A.
        '
        ' This is the direct equivalent of the legacy code.
        '======================================================
        For index As Integer = 1 To 3

            Dim delta As Double =
                angle(0) - angle(index)

            If angle(index) < angle(0) AndAlso
               delta > Small Then

                angle(index) += TwoPi
            End If
        Next

        '======================================================
        ' Sort the four tangent angles while preserving the
        ' legacy "|" treatment for coincident directions.
        '======================================================
        For i As Integer = 0 To 2

            For j As Integer = i + 1 To 3

                Dim delta As Double =
                    angle(j) - angle(i)

                If delta > VectorSmall Then
                    Continue For
                End If

                If Math.Abs(delta) < VectorSmall Then

                    id(i) = "|"c
                    id(j) = "|"c
                End If

                Dim tempAngle As Double =
                    angle(i)

                angle(i) =
                    angle(j)

                angle(j) =
                    tempAngle

                Dim tempId As Char =
                    id(i)

                id(i) =
                    id(j)

                id(j) =
                    tempId
            Next
        Next

        Dim pattern As String =
            New String(id)

        For Each mapEntry As IntersectionMapEntry In IntersectionMap

            If mapEntry.Type =
                IntersectionType.Undefined Then

                Exit For
            End If

            If String.Equals(
                pattern,
                mapEntry.Pattern,
                StringComparison.Ordinal) Then

                Return mapEntry.Type
            End If
        Next

        ' Legacy default:
        '     return Graze;
        Return IntersectionType.Graze
    End Function

    '==========================================================
    ' Normalize an angle into [0, 2PI).
    '==========================================================
    Private Function NormalizeAngle(
        angle As Double) As Double

        While angle < 0.0
            angle += TwoPi
        End While

        While angle >= TwoPi
            angle -= TwoPi
        End While

        Return angle
    End Function

End Class