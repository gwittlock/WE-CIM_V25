Public MustInherit Class GeoEntity
    Implements isSelectable

    Public Property ID As Integer
    Public Property Selected As Boolean
    Public Property Association As IGeoAssociation

    Public Property IsSelected As Boolean Implements isSelectable.IsSelected
        Get
            Return Selected
        End Get
        Set(value As Boolean)
            Selected = value
        End Set
    End Property

    Public Property IsHovered As Boolean Implements isSelectable.IsHovered

    Public ReadOnly Property Color As System.Drawing.Color
        Get
            If Association Is Nothing Then
                Return System.Drawing.Color.White
            End If

            If TypeOf Association Is Layer Then
                Return DirectCast(Association, Layer).Color
            End If

            If TypeOf Association Is FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool Then
                Return DirectCast(
                    Association,
                    FabV25_WIN8.WE_ENG_V25_0.Core.Models.Tool).Color
            End If

            Return System.Drawing.Color.White
        End Get
    End Property

End Class