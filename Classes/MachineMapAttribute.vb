<AttributeUsage(AttributeTargets.Property, AllowMultiple:=False)>
Public Class MachineMapAttribute
    Inherits Attribute

    Public ReadOnly SourceProperty As String

    Public Sub New(sourceProperty As String)
        Me.SourceProperty = sourceProperty
    End Sub
End Class

