Imports System
Imports System.Drawing

Namespace ToolShapes

    Public Enum ProfileSide
        Inside
        Outside
    End Enum

    Public Class Profile
        Public ReadOnly Property Path As IPath2D
        Public ReadOnly Property Side As ProfileSide
        Public ReadOnly Property SourceEntities As IReadOnlyList(Of ImportedEntity)

        ''' <summary>
        ''' Creates a nominal profile from an ordered path.
        ''' </summary>
        Public Sub New(
        path As IPath2D,
        side As ProfileSide,
        sourceEntities As IEnumerable(Of ImportedEntity))

            If path Is Nothing Then
                Throw New ArgumentNullException(NameOf(path))
            End If

            Me.Path = path
            Me.Side = side

            If sourceEntities Is Nothing Then
                Me.SourceEntities =
                New List(Of ImportedEntity)()
            Else
                Me.SourceEntities =
                sourceEntities.ToList()
            End If

        End Sub

        ''' <summary>
        ''' Indicates whether the profile geometry forms a closed path.
        ''' </summary>
        Public ReadOnly Property IsClosed As Boolean
            Get
                Return Path.IsClosed
            End Get
        End Property

        ''' <summary>
        ''' The profile bounding box in world coordinates.
        ''' </summary>
        Public ReadOnly Property BoundingBox As RectangleF
            Get
                Return Path.BoundingBox
            End Get
        End Property

    End Class

End Namespace