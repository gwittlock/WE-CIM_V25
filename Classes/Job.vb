Imports System.Collections.Generic

Public Class Job

    '------------------------------------------------------------
    ' Job identification
    '------------------------------------------------------------
    Public Property ID As Integer
    Public Property Description As String

    '------------------------------------------------------------
    ' Job-level configuration
    '------------------------------------------------------------
    Public Property MachineID As Integer
    Public Property ToolSetupID As Integer
    Public Property LayerSetupID As Integer
    Public Property MaterialTypeID As Integer
    Public Property JobSheets As New List(Of JobSheet)

    '------------------------------------------------------------
    ' Customer / Job UDP values
    '
    ' Examples:
    '   Due Date
    '   PO Number
    '   Customer Name
    '   etc.
    '
    ' These are intentionally dynamic.
    '------------------------------------------------------------
    Public Property UDPs As New Dictionary(Of String, Object)

End Class