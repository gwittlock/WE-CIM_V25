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

Imports System.ComponentModel

Namespace CustomGrid.CustomGrid
    Public Class UnboundDataSource
        Inherits BindingList(Of Object())
        Implements ITypedList, IBindingList
        Public Sub New()
        End Sub
        Public Sub ClearAllItems()
            Me.ClearItems()
        End Sub
        Public Sub AddRow(ByVal rowData() As Object)
            Me.Add(rowData)

        End Sub

        Public Function GetValue(ByVal rowIndex As Integer, ByVal columnIndex As Integer) As Object
            If rowIndex >= Me.Count Then
                Return Nothing
            End If
            Dim row() As Object = Me(rowIndex)
            If columnIndex >= row.Length Then
                Return Nothing
            End If
            Return row(columnIndex)
        End Function

        Public Sub SetValue(ByVal rowIndex As Integer, ByVal columnIndex As Integer, ByVal value As Object)
            If rowIndex >= Me.Count Then
                Return
            End If
            Dim row() As Object = Me(rowIndex)
            If columnIndex >= row.Length Then
                row = New Object(columnIndex) {}
                Me(rowIndex).CopyTo(row, 0)
                Me(rowIndex) = row
            End If
            row(columnIndex) = value
        End Sub

#Region "ITypedList Members"

        Public Function GetItemProperties(ByVal listAccessors() As PropertyDescriptor) As PropertyDescriptorCollection Implements ITypedList.GetItemProperties
            Return New PropertyDescriptorCollection(New PropertyDescriptor() {})
        End Function

        Public Function GetListName(ByVal listAccessors() As PropertyDescriptor) As String Implements ITypedList.GetListName
            Return Me.GetType().Name
        End Function

#End Region

#Region "IBindingList Members"

        Private Overloads ReadOnly Property AllowNew() As Boolean Implements IBindingList.AllowNew
            Get
                Return True
            End Get
        End Property

        Private Overloads Function AddNew() As Object Implements IBindingList.AddNew
            Dim newRow(-1) As Object
            Me.Add(newRow)
            Return newRow
        End Function

#End Region

    End Class
End Namespace
