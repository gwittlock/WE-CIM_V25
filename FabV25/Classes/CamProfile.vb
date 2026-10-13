Public Enum WindingDirectionEnum
    Auto
    CW
    CCW
End Enum

Public Class CamProfile
    ' Versioning
    Public Const Version As Integer = 25

    ' Layer/Directive info
    Public Property LayerName As String
    Public Property Direction As WindingDirectionEnum

    ' List of CAD entities that make up this profile
    Public Property Entities As List(Of CadEntity) = New List(Of CadEntity)

    ' Optional processing flags (from LayerSetup)
    Public Property ZLevelMode As Double
    Public Property GapTolerance As Double
    Public Property CleanTolerance As Double
    Public Property FilterTolerance As Double
    Public Property SharpAngle As Double
    Public Property ProcessText As Boolean
    Public Property RestrictOffset As Boolean
    Public Property DisplayColor As Color

    ' Bounding box coordinates
    Public Property MinX As Double
    Public Property MinY As Double
    Public Property MaxX As Double
    Public Property MaxY As Double

    ' Total length of the profile
    Public Property TotalLength As Double

    ''' <summary>
    ''' Computes the bounding box for this profile.
    ''' </summary>
    Public Sub ComputeBoundingBox()
        If Entities Is Nothing OrElse Entities.Count = 0 Then
            MinX = 0
            MinY = 0
            MaxX = 0
            MaxY = 0
            Return
        End If

        MinX = Entities.Min(Function(e) e.MinX)
        MinY = Entities.Min(Function(e) e.MinY)
        MaxX = Entities.Max(Function(e) e.MaxX)
        MaxY = Entities.Max(Function(e) e.MaxY)
    End Sub

    ''' <summary>
    ''' Computes the total length of this profile.
    ''' </summary>
    Public Sub ComputeTotalLength()
        TotalLength = 0
        If Entities Is Nothing Then Return

        For Each e In Entities
            TotalLength += e.Length
        Next
    End Sub
End Class
