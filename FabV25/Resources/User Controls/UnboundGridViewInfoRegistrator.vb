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
Imports DevExpress.XtraGrid.Registrator
Imports DevExpress.XtraGrid.Views.Base

Namespace CustomGrid.CustomGrid
    Public Class UnboundGridViewInfoRegistrator
        Inherits GridInfoRegistrator
        Public Overrides ReadOnly Property ViewName() As String
            Get
                Return "UnboundGridView"
            End Get
        End Property

        Public Overrides Function CreateView(ByVal grid As GridControl) As BaseView
            Return New UnboundGridView(TryCast(grid, GridControl))
        End Function
    End Class
End Namespace
