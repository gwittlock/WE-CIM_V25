Imports System.ComponentModel

<TypeConverter(GetType(ExpandableObjectConverter))>
Public Class GeoPoint
    Inherits GeoEntity

    Public Sub New()
        MyBase.New()
    End Sub

    Friend Sub New(existingID As Integer)
        MyBase.New(existingID)
    End Sub

    Public Property X As Double
    Public Property Y As Double

End Class