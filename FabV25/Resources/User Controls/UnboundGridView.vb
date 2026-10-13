' Developer Express Code Central Example:
' How to use the Grid in the Unbound mode
' 
' The XtraGrid unlike the TreeList can't be used in the Unbound mode. It must
' always be bound to a datasource. This example illustrates how to emulate the
' Unbound mode for the XtraGrid.
' 
' As well as in case of using the TreeList, it
' is necessary to add unbound columns to the XtraGrid and use the AppendRow method
' to add a new record.
' 
' You can find sample updates and versions for different programming languages here:
' http://www.devexpress.com/example=E4434

Imports DevExpress.XtraGrid
Imports DevExpress.XtraGrid.Views.Grid
Imports System.ComponentModel

Namespace CustomGrid.CustomGrid
    Public Class UnboundGridView
        Inherits GridView
        Public Sub New()
            Me.New(Nothing)
        End Sub

        Public Sub New(ByVal grid As GridControl)
            MyBase.New(grid)
            AddHandler Me.CustomUnboundColumnData, AddressOf UnboundGridView_CustomUnboundColumnData
        End Sub

        Private Sub UnboundGridView_CustomUnboundColumnData(ByVal sender As Object, ByVal e As DevExpress.XtraGrid.Views.Base.CustomColumnDataEventArgs)
            If e.IsGetData Then
                e.Value = GridControl.DataSource.GetValue(e.ListSourceRowIndex, e.Column.AbsoluteIndex)
            Else
                GridControl.DataSource.SetValue(e.ListSourceRowIndex, e.Column.AbsoluteIndex, e.Value)
            End If
        End Sub

        Public Sub AppendRow(ByVal rowData() As Object)
            GridControl.DataSource.AddRow(rowData)
        End Sub

        Protected Overrides ReadOnly Property ViewName() As String
            Get
                Return "UnboundGridView"
            End Get
        End Property

        <Browsable(False)> _
        Public Shadows Property GridControl() As UnboundGridControl
            Get
                Return TryCast(MyBase.GridControl, UnboundGridControl)
            End Get
            Set(ByVal value As UnboundGridControl)
                MyBase.GridControl = value
            End Set
        End Property

        Public Overrides Sub AddNewRow()
            MyBase.AddNewRow()
        End Sub
    End Class
End Namespace
