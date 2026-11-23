Public Class GeoCircle
    Inherits GeoEntity

    Public Property Center As GeoPoint
    Public Property Radius As Double

    <System.ComponentModel.ReadOnlyAttribute(True)>
    Public ReadOnly Property Diameter As Double
        Get
            Return Radius * 2.0
        End Get
    End Property


    <System.ComponentModel.ReadOnlyAttribute(True)>
    Public ReadOnly Property Circumference As Double
        Get
            Return 2.0 * Math.PI * Radius
        End Get
    End Property

    <System.ComponentModel.ReadOnlyAttribute(True)>
    Public ReadOnly Property Area As Double
        Get
            Return Math.PI * Radius * Radius
        End Get
    End Property

End Class