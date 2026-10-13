
Public Class SelectionSet

    ' =========================
    ' Internal State
    ' =========================

    'Private ReadOnly _selected As New HashSet(Of DxfDrawable)

    ' =========================
    ' Events
    ' =========================
    Private ReadOnly _items As HashSet(Of ISelectable)

    Public Sub New(items As HashSet(Of ISelectable))
        _items = items
    End Sub


    Public Event SelectionChanged()

    ' =========================
    ' Properties
    ' =========================

    Public ReadOnly Property Count As Integer
        Get
            Return _selected.Count
        End Get
    End Property

    Public ReadOnly Property HasSelection As Boolean
        Get
            Return _selected.Count > 0
        End Get
    End Property

    Public ReadOnly Property SelectedEntities As IEnumerable(Of DxfDrawable)
        Get
            Return _selected
        End Get
    End Property

    Public Function IsSelected(ent As DxfDrawable) As Boolean
        If ent Is Nothing Then Return False
        Return _selected.Contains(ent)
    End Function

    ' =========================
    ' Core Operations
    ' =========================

    Public Sub Add(ent As DxfDrawable)

        If ent Is Nothing Then Exit Sub

        If _selected.Add(ent) Then
            ent.IsSelected = True

            RaiseEvent SelectionChanged()
        End If

    End Sub

    Public Sub Remove(ent As DxfDrawable)

        If ent Is Nothing Then Exit Sub

        If _selected.Remove(ent) Then
            ent.IsSelected = False
            RaiseEvent SelectionChanged()
        End If

    End Sub
    Public Sub Toggle(d As DxfDrawable)

        If d Is Nothing Then Exit Sub

        If _selected.Contains(d) Then
            _selected.Remove(d)
            d.IsSelected = False
        Else
            _selected.Add(d)
            d.IsSelected = True
        End If

        RaiseEvent SelectionChanged()

    End Sub

    Public Sub Clear()

        If _selected.Count = 0 Then Exit Sub

        For Each ent In _selected
            ent.IsSelected = False
        Next

        _selected.Clear()

        RaiseEvent SelectionChanged()

    End Sub

    ' =========================
    ' Bulk Operations
    ' =========================

    Public Sub SelectAll(entities As IEnumerable(Of DxfDrawable))

        If entities Is Nothing Then Exit Sub

        Dim changed As Boolean = False

        ' Clear existing
        If _selected.Count > 0 Then
            For Each ent In _selected
                ent.IsSelected = False
            Next
            _selected.Clear()
            changed = True
        End If

        ' Add all
        For Each ent In entities
            If ent Is Nothing Then Continue For

            If _selected.Add(ent) Then
                ent.IsSelected = True
                changed = True
            End If
        Next

        If changed Then RaiseEvent SelectionChanged()

    End Sub

    Public Sub AddRange(entities As IEnumerable(Of DxfDrawable))

        If entities Is Nothing Then Exit Sub

        Dim changed As Boolean = False

        For Each ent In entities
            If ent Is Nothing Then Continue For

            If _selected.Add(ent) Then
                ent.IsSelected = True
                changed = True
            End If
        Next

        If changed Then RaiseEvent SelectionChanged()

    End Sub

    Public Sub RemoveRange(entities As IEnumerable(Of DxfDrawable))

        If entities Is Nothing Then Exit Sub

        Dim changed As Boolean = False

        For Each ent In entities
            If ent Is Nothing Then Continue For

            If _selected.Remove(ent) Then
                ent.IsSelected = False
                changed = True
            End If
        Next

        If changed Then RaiseEvent SelectionChanged()

    End Sub

    ' =========================
    ' Filter-Based Operations
    ' =========================

    Public Sub SelectByFilter(allEntities As IEnumerable(Of DxfDrawable),
                             filterFunc As Func(Of DxfDrawable, Boolean))

        If allEntities Is Nothing OrElse filterFunc Is Nothing Then Exit Sub

        Dim changed As Boolean = False

        For Each ent In allEntities
            If ent Is Nothing Then Continue For

            If filterFunc(ent) Then
                If _selected.Add(ent) Then
                    ent.IsSelected = True
                    changed = True
                End If
            End If
        Next

        If changed Then RaiseEvent SelectionChanged()

    End Sub

    Public Sub RemoveByFilter(filterFunc As Func(Of DxfDrawable, Boolean))

        If filterFunc Is Nothing Then Exit Sub

        Dim toRemove As New List(Of DxfDrawable)

        For Each ent In _selected
            If filterFunc(ent) Then
                toRemove.Add(ent)
            End If
        Next

        If toRemove.Count = 0 Then Exit Sub

        For Each ent In toRemove
            _selected.Remove(ent)
            ent.IsSelected = False
        Next

        RaiseEvent SelectionChanged()

    End Sub

    ' =========================
    ' Utility
    ' =========================

    Public Function GetSelectionSnapshot() As List(Of DxfDrawable)
        Return _selected.ToList()
    End Function

    'Public Sub SelectDrawable(d As DxfDrawable)
    '    If d Is Nothing Then Exit Sub

    '    _selected.Add(d)
    'End Sub

    'Public Sub ClearSelection()
    '    _selected.Clear()
    'End Sub
End Class