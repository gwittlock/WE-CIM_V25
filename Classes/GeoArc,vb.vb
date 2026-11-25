
Public Class GeoArc
    Inherits GeoEntity

    Public Sub New()
        MyBase.New()
    End Sub

    Friend Sub New(existingID As Integer)
        MyBase.New(existingID)
    End Sub

    Public Property StartPoint As GeoPoint
    Public Property EndPoint As GeoPoint
    Public Property Center As GeoPoint
    Public Property Radius As Double
    Public Property StartAngle As Double
    Public Property EndAngle As Double
    Public Property IsClockwise As Boolean

    <System.ComponentModel.ReadOnlyAttribute(True)>
    Public ReadOnly Property SweepAngle As Double
        Get
            Dim sweep As Double

            If IsClockwise Then
                sweep = (StartAngle - EndAngle) Mod 360.0
            Else
                sweep = (EndAngle - StartAngle) Mod 360.0
            End If

            If sweep < 0 Then
                sweep += 360.0
            End If

            Return sweep
        End Get
    End Property

    <System.ComponentModel.ReadOnlyAttribute(True)>
    Public ReadOnly Property ArcLength As Double
        Get
            Return Radius * SweepAngle * Math.PI / 180.0
        End Get
    End Property

End Class
