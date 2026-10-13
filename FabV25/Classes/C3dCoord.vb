Public Class C3dCoord
    Public Property X As Double
    Public Property Y As Double
    Public Property Z As Double

    ' Constructor
    Public Sub New()
        X = 0
        Y = 0
        Z = 0
    End Sub

    Public Sub New(x As Double, y As Double, z As Double)
        Me.X = x
        Me.Y = y
        Me.Z = z
    End Sub

    ' Copy constructor
    Public Sub New(other As C3dCoord)
        Me.X = other.X
        Me.Y = other.Y
        Me.Z = other.Z
    End Sub

    ' Distance to another point
    Public Function DistanceTo(other As C3dCoord) As Double
        Dim dx As Double = Me.X - other.X
        Dim dy As Double = Me.Y - other.Y
        Dim dz As Double = Me.Z - other.Z
        Return Math.Sqrt(dx * dx + dy * dy + dz * dz)
    End Function

    ' Vector addition
    Public Shared Operator +(a As C3dCoord, b As C3dCoord) As C3dCoord
        Return New C3dCoord(a.X + b.X, a.Y + b.Y, a.Z + b.Z)
    End Operator

    ' Vector subtraction
    Public Shared Operator -(a As C3dCoord, b As C3dCoord) As C3dCoord
        Return New C3dCoord(a.X - b.X, a.Y - b.Y, a.Z - b.Z)
    End Operator

    ' Scale by factor
    Public Shared Operator *(a As C3dCoord, factor As Double) As C3dCoord
        Return New C3dCoord(a.X * factor, a.Y * factor, a.Z * factor)
    End Operator

    ' ToString override for debugging
    Public Overrides Function ToString() As String
        Return $"({X}, {Y}, {Z})"
    End Function
End Class
