Imports System.Windows.Forms
Imports DevExpress.XtraGrid.Views.Base

Public Class frmWorkZones
    Public Event WorkZoneChanged()
    Private _workZoneBindingSource As New BindingSource()
    Public Event AssignWorkZoneRequested(zone As WorkZone)

    Private Sub frmWorkZones_Load(
    sender As Object,
    e As EventArgs) Handles MyBase.Load

        FillWorkZones()

    End Sub

    Private Sub grdWorkZonesView_CellValueChanged(
    sender As Object,
    e As DevExpress.XtraGrid.Views.Base.CellValueChangedEventArgs
) Handles grdWorkZonesView.CellValueChanged

        Dim zone As WorkZone = GetSelectedWorkZone()

        If zone Is Nothing Then Return

        RaiseEvent WorkZoneChanged()

    End Sub

    Public Sub FillWorkZones()

        _workZoneBindingSource.DataSource = Nothing

        If CurrentJob Is Nothing Then Return

        _workZoneBindingSource.DataSource =
        CurrentJob.WorkZones

        grdWorkZones.DataSource = _workZoneBindingSource

    End Sub

    Private Function GetSelectedWorkZone() As WorkZone

        Return TryCast(
        grdWorkZonesView.GetFocusedRow(),
        WorkZone)

    End Function

    Private Sub grdWorkZonesView_FocusedRowChanged(sender As Object, e As DevExpress.XtraGrid.Views.Base.FocusedRowChangedEventArgs) Handles grdWorkZonesView.FocusedRowChanged


    End Sub

    Private Sub cmdAdd_Click(sender As Object, e As EventArgs) Handles cmdAdd.Click

        If CurrentJob Is Nothing Then Return

        Dim newZone As New WorkZone With {
            .Index = CurrentJob.WorkZones.Count,
            .XMin = 0,
            .XMax = 0,
            .YMin = 0,
            .YMax = 0,
            .HoldX = 0,
            .HoldY = 0,
            .RepositionDistanceX = 0,
            .RepositionOffsetX = 0,
            .IsActive = True
        }

        CurrentJob.WorkZones.Add(newZone)

        _workZoneBindingSource.ResetBindings(False)

        grdWorkZonesView.FocusedRowHandle =
            grdWorkZonesView.RowCount - 1

        RaiseEvent WorkZoneChanged()

    End Sub

    Private Sub grdWorkZones_Click(sender As Object, e As EventArgs) Handles grdWorkZones.Click

    End Sub

    Private Sub grdWorkZonesView_CustomColumnDisplayText(
        sender As Object,
        e As DevExpress.XtraGrid.Views.Base.CustomColumnDisplayTextEventArgs
    ) Handles grdWorkZonesView.CustomColumnDisplayText

        If e.Column.FieldName <> "Index" Then Return

        If e.Value Is Nothing Then Return

        Dim zoneIndex As Integer = CInt(e.Value)

        e.DisplayText = (zoneIndex + 1).ToString()

    End Sub

    Private Sub cmdRemove_Click(sender As Object, e As EventArgs) Handles cmdRemove.Click

        If CurrentJob Is Nothing Then Return

        Dim zone As WorkZone = GetSelectedWorkZone()

        If zone Is Nothing Then Return

        Dim selectedIndex As Integer =
            CurrentJob.WorkZones.IndexOf(zone)

        If selectedIndex < 0 Then Return

        ' Remove the selected WorkZone.
        CurrentJob.WorkZones.Remove(zone)

        ' Renumber the remaining WorkZones.
        For i As Integer = 0 To CurrentJob.WorkZones.Count - 1
            CurrentJob.WorkZones(i).Index = i
        Next

        ' Refresh the grid.
        _workZoneBindingSource.ResetBindings(False)

        ' Select the next available WorkZone.
        If CurrentJob.WorkZones.Count > 0 Then

            Dim nextIndex As Integer =
                Math.Min(selectedIndex, CurrentJob.WorkZones.Count - 1)

            grdWorkZonesView.FocusedRowHandle = nextIndex

        End If

        ' Refresh the drawing.
        RaiseEvent WorkZoneChanged()

    End Sub
    Private Sub cmdAssign_Click(
    sender As Object,
    e As EventArgs) Handles cmdAssign.Click

        Dim selectedZone As WorkZone = GetSelectedWorkZone()

        If selectedZone Is Nothing Then Return

        RaiseEvent AssignWorkZoneRequested(selectedZone)

    End Sub
End Class