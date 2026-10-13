Imports System.Drawing
Imports WE_ENG_V25_0.Core.Models

Namespace ToolShapes

    Public Enum ToolPathOffset
        None
        Left
        Right
    End Enum

    Public Class TooledProfile

        ''' <summary>
        ''' The nominal Profile from which this tooled profile is derived.
        ''' </summary>
        Public ReadOnly Property SourceProfile As Profile

        ''' <summary>
        ''' Indicates the relationship of the tool path
        ''' to the nominal profile geometry.
        ''' </summary>
        Public Property Offset As ToolPathOffset

        ''' <summary>
        ''' The generated tool-center path.
        ''' This remains separate from the nominal SourceProfile.Path.
        ''' </summary>
        Public Property ToolPath As IPath2D

        ''' <summary>
        ''' The tool used to generate this profile.
        ''' </summary>
        Public Property Tool As WE_ENG_V25_0.Core.Models.Tool

        Public Sub New(
            sourceProfile As Profile,
            offset As ToolPathOffset
        )

            If sourceProfile Is Nothing Then
                Throw New ArgumentNullException(NameOf(sourceProfile))
            End If

            Me.SourceProfile = sourceProfile
            Me.Offset = offset

        End Sub

        ''' <summary>
        ''' The inside/outside classification comes from the
        ''' nominal source profile.
        ''' </summary>
        Public ReadOnly Property Side As ProfileSide
            Get
                Return SourceProfile.Side
            End Get
        End Property

        ''' <summary>
        ''' Indicates whether the nominal source profile is closed.
        ''' </summary>
        Public ReadOnly Property IsClosed As Boolean
            Get
                Return SourceProfile.IsClosed
            End Get
        End Property

        ''' <summary>
        ''' Bounding box of the generated tool path.
        ''' Returns the nominal profile bounds until a tool path exists.
        ''' </summary>
        Public ReadOnly Property BoundingBox As RectangleF
            Get
                If ToolPath IsNot Nothing Then
                    Return ToolPath.BoundingBox
                End If

                Return SourceProfile.BoundingBox
            End Get
        End Property

    End Class

End Namespace