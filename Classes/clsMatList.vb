Public Class clsMatList

    Private _smatcategoryname As String
    Private _nmatcategoryid As Integer
    Private _smaterialname As String
    Private _nmaterialid As Integer
    Private _dmateriallength As Decimal
    Private _dmaterialwidth As Decimal
    Private _dmaterialthickness As Decimal
    Private _smatlistindex As Long


    Public Property sMatListIndex As Long
        Get
            Return _smatlistindex
        End Get
        Set(value As Long)
            _smatlistindex = value
        End Set
    End Property
    Public Property sMatCategoryName As String
        Get
            Return _smatcategoryname
        End Get
        Set(value As String)
            _smatcategoryname = value
        End Set
    End Property
    Public Property nMatCategoryID As Integer
        Get
            Return _nmatcategoryid
        End Get
        Set(value As Integer)
            _nmatcategoryid = value
        End Set
    End Property
    Public Property sMaterialName As String
        Get
            Return _smaterialname
        End Get
        Set(value As String)
            _smaterialname = value
        End Set
    End Property
    Public Property nMaterialID As Integer
        Get
            Return _nmaterialid
        End Get
        Set(value As Integer)
            _nmaterialid = value
        End Set
    End Property
    Public Property dMaterialLength As Decimal
        Get
            Return _dmateriallength
        End Get
        Set(value As Decimal)
            _dmateriallength = value
        End Set
    End Property
    Public Property dMaterialWidth As Decimal
        Get
            Return _dmaterialwidth
        End Get
        Set(value As Decimal)
            _dmaterialwidth = value
        End Set
    End Property
    Public Property dMaterialThickness As Decimal
        Get
            Return _dmaterialthickness
        End Get
        Set(value As Decimal)
            _dmaterialthickness = value
        End Set
    End Property
    Public Sub New()
        dMaterialLength = 0
        dMaterialWidth = 0
        dMaterialThickness = 0
        nMaterialID = -1
        sMatCategoryName = ""
        sMaterialName = ""
        nMatCategoryID = -1
    End Sub
    Public Sub New(__smatcategoryname As String, __nmatcategoryid As Integer, __smaterialname As String, __nmaterialid As Integer, __dmateriallength As Decimal, __dmaterialwidth As Decimal, __dmaterialthickness As Decimal)
        sMatCategoryName = __smatcategoryname
        nMatCategoryID = __nmatcategoryid
        sMaterialName = __smaterialname
        nMaterialID = __nmaterialid
        dMaterialLength = __dmateriallength
        dMaterialWidth = __dmaterialwidth
        dMaterialThickness = __dmaterialthickness
    End Sub

End Class
