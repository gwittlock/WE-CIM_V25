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
Imports System.ComponentModel

Namespace CustomGrid.CustomGrid
    <ToolboxItem(True)> _
    Public Class UnboundGridControl
        Inherits GridControl
        Friend WithEvents GridView1 As DevExpress.XtraGrid.Views.Grid.GridView
        Public Sub New()
            Me.DataSource = New UnboundDataSource()
        End Sub

        Protected Overrides Function CreateDefaultView() As BaseView
            Return CreateView("UnboundGridView")
        End Function

        Protected Overrides Sub RegisterAvailableViewsCore(ByVal collection As InfoCollection)
            MyBase.RegisterAvailableViewsCore(collection)
            collection.Add(New UnboundGridViewInfoRegistrator())
        End Sub

        <Browsable(False)> _
        Public Shadows Property DataSource() As UnboundDataSource
            Get
                Return TryCast(MyBase.DataSource, UnboundDataSource)
            End Get
            Set(ByVal value As UnboundDataSource)
                MyBase.DataSource = value
            End Set
        End Property

        Private Sub InitializeComponent()
            Me.GridView1 = New DevExpress.XtraGrid.Views.Grid.GridView()
            CType(Me.GridView1, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.SuspendLayout()
            '
            'GridView1
            '
            Me.GridView1.GridControl = Me
            Me.GridView1.Name = "GridView1"
            '
            'UnboundGridControl
            '
            Me.MainView = Me.GridView1
            Me.ViewCollection.AddRange(New DevExpress.XtraGrid.Views.Base.BaseView() {Me.GridView1})
            CType(Me.GridView1, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me, System.ComponentModel.ISupportInitialize).EndInit()
            Me.ResumeLayout(False)

        End Sub
    End Class
End Namespace
