Option Strict On
Option Explicit On

Public Enum ProfileShapeType
    Unknown
    Circle
    Rectangle
    Square
    Slot
End Enum

Public Class ProfileSignature

    Public ReadOnly Property ShapeType As ProfileShapeType
    Public ReadOnly Property Dim1 As Double
    Public ReadOnly Property Dim2 As Double

    Public Sub New(
        shapeType As ProfileShapeType,
        dim1 As Double,
        Optional dim2 As Double = 0
    )
        Me.ShapeType = shapeType
        Me.Dim1 = Math.Round(dim1, 4)
        Me.Dim2 = Math.Round(dim2, 4)
    End Sub

    Public Overrides Function Equals(obj As Object) As Boolean
        Dim other = TryCast(obj, ProfileSignature)
        If other Is Nothing Then Return False

        Return ShapeType = other.ShapeType AndAlso
               NearlyEqual(Dim1, other.Dim1) AndAlso
               NearlyEqual(Dim2, other.Dim2)
    End Function
    Public Overrides Function GetHashCode() As Integer
        Dim hash As Integer = 17
        hash = hash * 23 + ShapeType.GetHashCode()
        hash = hash * 23 + Dim1.GetHashCode()
        hash = hash * 23 + Dim2.GetHashCode()
        Return hash
    End Function


    Private Function NearlyEqual(a As Double, b As Double) As Boolean
        Return Math.Abs(a - b) < 0.0005
    End Function

End Class
