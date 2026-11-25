Imports System.ComponentModel


Public Class GeoLine
        Inherits GeoEntity
    Public Sub New()
        MyBase.New()
    End Sub

    Friend Sub New(existingID As Integer)
        MyBase.New(existingID)
    End Sub
    <TypeConverter(GetType(ExpandableObjectConverter))>
        Public Property StartPoint As GeoPoint

    <TypeConverter(GetType(ExpandableObjectConverter))>
    Public Property EndPoint As GeoPoint

    <System.ComponentModel.ReadOnlyAttribute(True)>
    Public ReadOnly Property Length As Double
        Get
            If StartPoint Is Nothing OrElse EndPoint Is Nothing Then
                Return 0.0
            End If

            Dim dx As Double = EndPoint.X - StartPoint.X
            Dim dy As Double = EndPoint.Y - StartPoint.Y

            Return Math.Sqrt(dx * dx + dy * dy)
        End Get
    End Property

    <System.ComponentModel.ReadOnlyAttribute(True)>
    Public ReadOnly Property Angle As Double
        Get
            If StartPoint Is Nothing OrElse EndPoint Is Nothing Then
                Return 0.0
            End If

            Dim dx As Double = EndPoint.X - StartPoint.X
            Dim dy As Double = EndPoint.Y - StartPoint.Y

            Dim degrees As Double =
                Math.Atan2(dy, dx) * 180.0 / Math.PI

            If degrees < 0 Then
                degrees += 360.0
            End If

            Return degrees
        End Get
    End Property

End Class
