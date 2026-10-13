Public Enum DynamicDataType
    StringType = 0
    IntegerType = 1
    DoubleType = 2
    BooleanType = 3
    Dropdown = 4
    FilePath = 5
    FolderPath = 6
    ColorType = 7
End Enum

Public Class DynamicPropertyType
    Public Property ID As Integer                  ' Unique ID for mapping
    Public Property Name As String                 ' Internal name / field
    Public Property Display As String              ' Displayed in PropertyGrid
    Public Property DataType As DynamicDataType
    Public Property Options As List(Of String)     ' Only used for dropdowns
End Class
