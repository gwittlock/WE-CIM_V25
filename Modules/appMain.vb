Imports System.IO
Imports System.Runtime.InteropServices
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.ComponentModel
Public Module appMain

    Public _SelectedMaterialID As Integer

    ' Path to the Data folder
    Public dataFolder As String = Path.Combine(Application.StartupPath, "data")

    Public fMainForm As frmMain

    Public lReturn As Integer 'used for return value of funtions using integer
    Public sReturn As String 'Used for return value of functions using strng

    'Delimiters for formatting internal strings.
    Public CHR1 As String = Convert.ToChar(1)

    Public CHR2 As String = Convert.ToChar(2)
    ' --- Class-level variables to store JSON data ---
    'Public machineTypesTable As JsonTable(Of MachineType)
    'Public machinesTable As JsonTable(Of Machine)
    'Public machineAttributesTable As JsonTable(Of MachineAttribute)
    'Public machineAttributeTypesTable As JsonTable(Of MachineAttributeType)
    ' Track current machine ID
    Public currentMachineID As Integer = -1



    Public Function EncodeTag(DatVarname As String,
                              DialogDisplay As String,
                              VBValue As String,
                              CValue As String,
                              CVarName As String,
                              VarDataType As Integer,
                              nRecordID As Integer,
                              sOptions As String,
                              nDefnID As Integer) As String

        EncodeTag = CHR1 & "DatVarName" & CHR2 & DatVarname &
                    CHR1 & "DialogDisplay" & CHR2 & DialogDisplay &
                    CHR1 & "VBValue" & CHR2 & VBValue &
                    CHR1 & "CValue" & CHR2 & CValue &
                    CHR1 & "CVarName" & CHR2 & CVarName &
                    CHR1 & "VarDataType" & CHR2 & VarDataType &
                    CHR1 & "nRecordID" & CHR2 & nRecordID &
                    CHR1 & "Options" & CHR2 & sOptions &
                    CHR1 & "DefnID" & CHR2 & nDefnID

    End Function

    Public Function ExecuteFunction(sncs As Long) As Long

        Dim status As Long
        Dim group As Long

        status = 0

        Cursor.Current = Cursors.WaitCursor

        group = (sncs \ 100) * 100

        'The selection filter icons require special consideration.
        'We want to avoid calling modModeMgmt.ModesReset() because
        'it resets the selection icon (arrow icon).
        'If (group <> 900 And group <> 2000) Then modModeMgmt.ModesReset

        'If (group <> 900) Then modModeMgmt.ModesReset()

        Select Case group
            Case 100 : status = ExecuteFile(sncs)
                'Case 200 : status = ExecuteView(sncs)
                '    'Case 300:     status = ExecuteStdView(sncs) -- see ExecuteStdView() in CI
                'Case 400 : status = ExecuteUndoRedo(sncs)
                'Case 500 : status = ExecuteProcess(sncs)
                '    'vb6 -  Case 600 : status = ExecuteHelp(sncs)
                '    'Case 700:     status = ExecuteDimension(sncs) -- see ExecuteDimension() in CI
                '    'Case 800:     status = ExecuteSnap(sncs) == see ExecuteSnap() in CI
                'Case 900 : status = ExecuteSelection(sncs)
                'Case 1100 : status = ExecuteGeometry(sncs)
                '    'vb6 - Case 1200 'Arrow icons (should never be an option)
                '    'vb6 - Case 1300 'Layer States (should never be an option)
                'Case 1400 : status = ExecuteTransform(sncs)
                '    ' Case 1500 : status = ExecuteUserMacro(sncs)
                'Case 2000 : status = ExecuteEdit(sncs)
                '    'vb6 - Case 9000 : status = ExecuteShapeLibrary(sncs)
                '    'Case 3000:    status = ExecuteMacro(sncs) -- see ExecuteMacro() in CI
                'Case 9500 : status = ExecuteEdit(9500)
                '    'Place Holder use Just to reference the id of 9500

                'Case 9501 : status = ExecuteProcess(sncs)

                'Case 9600 : status = ExecuteEdit(9600)

        End Select

        ExecuteFunction = status

        Cursor.Current = Cursors.Default


    End Function

    Public Function ExecuteFile(ByVal sncs As Long) As Long

        Dim status As Long

        Select Case sncs
            'Case 101 : status = modFile.FileNew()
            'Case 102 : status = modFile.FileOpen()
            'Case 103 : status = modFile.FileSave()
            'Case 104 : status = modFile.FileSaveAs()
            'Case 105 : status = modFile.FilePrintGraphics()
            'Case 106 : status = modFile.FileMacro()
            'Case 107 : status = modFile.FileMerge()
            'Case 108 : status = modFile.FileOpenPDB()
            'Case 109 : status = modFile.FilePrintNestreports
            'Case 110 : status = modFile.FilePrintToolList
            'Case 111 : status = modFile.FilePrintMaterial
            'Case 109 : status = modFile.FileMacro()

            Case Else : status = 0
        End Select

        ExecuteFile = status

    End Function

    Public Function GetValueFromDelimitedData(
                                formattedString As String,
                                fieldname As String,
                                defval As String) As String

        Dim theValue As String
        Dim pieces() As String
        Dim Count As Long
        Dim indx As Long

        If (formattedString IsNot Nothing) Then

            theValue = defval

            pieces = Split(formattedString, CHR1)
            Count = UBound(pieces)

            For indx = 1 To Count
                If (InStr(1, pieces(indx), fieldname) = 1) Then
                    pieces = Split(pieces(indx), CHR2)
                    Count = UBound(pieces)

                    If (Count = 1) Then
                        theValue = pieces(1)
                        Exit For
                    End If
                End If
            Next indx
        End If

        GetValueFromDelimitedData = theValue

    End Function
    ' --- Classes for JSON deserialization ---

    Public Class MachineToolType
        Public Property MachineTypeID As Integer
        Public Property ToolTypeID As Integer
    End Class
    '------------------------------
    '  MATERIAL TYPE CLASS
    '------------------------------

    ' Material Type
    Public Class MaterialType
        Public Property ID As Integer
        Public Property Description As String
        Public Property Display As String
        Public Property IsActive As Boolean = True
    End Class

    '------------------------------
    '  MATERIAL SHEET CLASS

    <TypeConverter(GetType(MaterialSheetTypeConverter))>
    Public Class MaterialSheet
        ' JSON-deserialized fields
        Public Property X As Double
        Public Property Y As Double
        Public Property ID As Integer
        Public Property TypeID As Integer
        Public Property Values As New List(Of MaterialParameter)

        ' Computed properties for common fields
        <JsonIgnore()>
        Public Property Description As String
            Get
                Return GetValue("Description")?.ToString()
            End Get
            Set(value As String)
                SetValue("Description", value)
            End Set
        End Property

        <JsonIgnore()>
        Public Property Length As Decimal
            Get
                Dim v = GetValue("Length")
                Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
            End Get
            Set(value As Decimal)
                SetValue("Length", value)
            End Set
        End Property

        <JsonIgnore()>
        Public Property Width As Decimal
            Get
                Dim v = GetValue("Width")
                Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
            End Get
            Set(value As Decimal)
                SetValue("Width", value)
            End Set
        End Property

        <JsonIgnore()>
        Public Property Thickness As Decimal
            Get
                Dim v = GetValue("Thickness")
                Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
            End Get
            Set(value As Decimal)
                SetValue("Thickness", value)
            End Set
        End Property

        <JsonIgnore()>
        Public Property Weight As Decimal
            Get
                Dim v = GetValue("Weight")
                Return If(v IsNot Nothing, Convert.ToDecimal(v), 0D)
            End Get
            Set(value As Decimal)
                SetValue("Weight", value)
            End Set
        End Property

        <JsonIgnore()>
        Public Property Quantity As Integer
            Get
                Dim v = GetValue("Quantity")
                Return If(v IsNot Nothing, Convert.ToInt32(v), 0)
            End Get
            Set(value As Integer)
                SetValue("Quantity", value)
            End Set
        End Property

        ' Helper to get a value from Values
        Private Function GetValue(name As String) As Object
            Dim param = Values.FirstOrDefault(Function(p) p.Name.Equals(name, StringComparison.OrdinalIgnoreCase))
            Return If(param IsNot Nothing, param.Value, Nothing)
        End Function

        ' Helper to set a value in Values
        Private Sub SetValue(name As String, val As Object)
            Dim param = Values.FirstOrDefault(Function(p) p.Name.Equals(name, StringComparison.OrdinalIgnoreCase))
            If param IsNot Nothing Then
                param.Value = val
            Else
                ' Add new param if missing
                Values.Add(New MaterialParameter With {
                .Name = name,
                .Display = name,
                .Value = val,
                .DataType = If(TypeOf val Is String, 0, If(TypeOf val Is Boolean, 4, 7)),
                .DefaultValue = val
            })
            End If
        End Sub
    End Class

    Public Class MaterialSheetTypeConverter
        Inherits ExpandableObjectConverter

        Public Overrides Function GetPropertiesSupported(context As ITypeDescriptorContext) As Boolean
            Return True
        End Function

        Public Overrides Function GetProperties(context As ITypeDescriptorContext, value As Object, attributes() As Attribute) As PropertyDescriptorCollection
            Dim sheet = TryCast(value, MaterialSheet)
            If sheet Is Nothing Then Return MyBase.GetProperties(context, value, attributes)

            Dim props As New List(Of PropertyDescriptor)

            ' Add all UDPs / Values
            For Each param In sheet.Values
                'props.Add(New MaterialParameterDescriptor(param))
            Next

            Return New PropertyDescriptorCollection(props.ToArray())
        End Function
    End Class

    Public Class MaterialInventoryItem
        Public Property ID As Integer
        Public Property TypeID As Integer
        Public Property Values As Dictionary(Of String, Object) = New Dictionary(Of String, Object)
    End Class

    Public Class MaterialParameter
        Public Property Name As String          ' Internal name / field
        Public Property Display As String       ' Display name for property grid
        Public Property Value As Object         ' Actual value
        Public Property DataType As Integer     ' 0=String, 1=Integer, 7=Decimal, 4=Boolean, etc.
        Public Property DefaultValue As Object  ' Default value if none is set
        Public Property Visible As Boolean = True
    End Class


    Public Class JsonTable(Of T)
        Public Property TableName As String
        Public Property Columns As List(Of JsonColumn)
        Public Property Rows As List(Of T)

        Public Sub New()
            Columns = New List(Of JsonColumn)()
            Rows = New List(Of T)()
        End Sub
    End Class

    '--------------------------------------
    ' JsonColumn - describes a column/property in the JSON table
    '--------------------------------------
    Public Class JsonColumn
        Public Property Field As String
        Public Property Header As String
        Public Property Visible As Boolean
        ' DataType: 0=String, 1=Boolean, 2=Integer, 7=Double/Decimal, etc.
        Public Property DataType As Integer
    End Class

    ' Wrapper to hold tool properties for the PropertyGrid
    Public Class ToolPropertyWrapper
        Public Property Description As String
        Public Property ToolTypeID As Integer
        Public Property Length As Double
        Public Property Diameter As Double
        Public Property Material As String
        ' Add other properties as needed
    End Class

    ' Tool object that is persisted to JSON
    Public Class Tool
        Public Enum ToolShape
            Circle
            Rectangle
            Square
            Custom
        End Enum

        Public Property Shape As ToolShape
        Public Property Size As Double ' For circle
        Public Property Width As Double ' For rectangle
        Public Property Height As Double
        Public Property Name As String
        Public Property ID As Integer
        Public Property Description As String
        Public Property ToolTypeID As Integer
        Public Property Length As Double
        Public Property Diameter As Double
        Public Property Material As String
        ' Add other properties as needed
    End Class

    ' --- JSON Classes ---
    Public Class MachineType
        Public Property ID As Integer
        Public Property Description As String
        Public Property Display As String
    End Class

    Public Class Machine
        Public Property ID As Integer
        Public Property Description As String
        Public Property TypeID As Integer
        Public Property WorkplaneTypeID As Integer
        Public Property Units As Integer
        Public Property CodePartProfile As Boolean
        Public Property HoldDown_CTG As String
        Public Property Clamp_CTG As String
        Public Property Table_CTG As String
        Public Property CNC_Folder As String
    End Class

    Public Class MachineAttribute
        Public Property ID As Integer
        Public Property MachineID As Integer
        Public Property AttributeTypeID As Integer
        Public Property Value As String
    End Class

    Public Class MachineAttributeType
        Public Property ID As Integer
        Public Property DataType As Integer        ' 0=String, 1=Integer, 7=Double, 8=Boolean, 9=Dropdown, 10=File, 11=Folder, 12=Color, etc.
        Public Property Display As String
        Public Property Description As String
        ' For dropdowns only
        Public Property Options As List(Of String)

        ' For File/Folder/Color, you can optionally store hints or default paths/colors
        Public Property DefaultValue As String
    End Class

End Module
