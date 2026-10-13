Imports FabV25_WIN8.ToolShapes

Public Class ProfileFeature
    Implements IGeometricFeature

    Public ReadOnly Property FeatureType As FeatureType _
    Implements IGeometricFeature.FeatureType
        Get
            Return FeatureType.Profile
        End Get
    End Property

    Public ReadOnly Property SourcePath As IPath2D _
    Implements IGeometricFeature.SourcePath

    Public ReadOnly Property MinCornerRadius As Double _
    Implements IGeometricFeature.MinCornerRadius

    Public ReadOnly Property MinSlotWidth As Double _
    Implements IGeometricFeature.MinSlotWidth

    Public ReadOnly Property IsClosed As Boolean _
    Implements IGeometricFeature.IsClosed
        Get
            Return SourcePath.IsClosed
        End Get
    End Property

    Public ReadOnly Property Area As Double _
    Implements IGeometricFeature.Area
        Get
            Return 0.0 ' stub for now
        End Get
    End Property

    Public Sub New(path As IPath2D, minCornerRadius As Double)
        SourcePath = path
        Me.MinCornerRadius = minCornerRadius
        Me.MinSlotWidth = 0.0
    End Sub
End Class
