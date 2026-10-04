' ============================================
'   AppData - Global JSON and Application Data
'   Used by entire application
' ============================================
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.ComponentModel
Imports System.IO
Imports System.Drawing
Imports System.Drawing.Design
Imports System.Windows.Forms.Design


Public Class AppData

    ' Folder where JSON files are stored
    ' App data folder
    Public Shared DataFolder As String = Path.Combine(Application.StartupPath, "Data\")

    ' -------------------------------
    ' MACHINES / MATERIALS
    ' -------------------------------
    Public Shared MachineTypes As List(Of MachineType)
    Public Shared Machines As List(Of Machine)
    Public Shared MachineAttributeTypes As List(Of MachineAttributeType)
    Public Shared MachineAttributes As List(Of MachineAttribute)
    Public Shared MachineToolTypes As List(Of MachineToolType)

    Public Shared Stations As List(Of Station)
    Public Shared ToolSetups As List(Of ToolSetup)
    Public Shared ToolSetupMembers As List(Of ToolSetupMember)


    Public Shared MaterialTypes As List(Of MaterialType)
    Public Shared MaterialSheets As List(Of MaterialSheet)

    ' -------------------------------
    ' TOOLS
    ' -------------------------------
    Public Shared ToolCrib As List(Of ToolCribItem)

    Public Shared Function GetRuntimeTool(
    toolID As Integer) As WE_ENG_V25_0.Core.Models.Tool

        If ToolCrib Is Nothing Then
            Return Nothing
        End If

        Dim cribItem As ToolCribItem =
        ToolCrib.FirstOrDefault(
            Function(t) t.ID = toolID)

        If cribItem Is Nothing Then
            Return Nothing
        End If

        Dim runtimeTool As New WE_ENG_V25_0.Core.Models.Tool With {
        .ID = cribItem.ID,
        .Description = cribItem.Description,
        .ToolTypeID = cribItem.TypeID
    }

        If cribItem.Attributes IsNot Nothing Then

            Dim diameterValue As Object = Nothing

            If cribItem.Attributes.TryGetValue(
            "DIAMETER",
            diameterValue) Then

                If diameterValue IsNot Nothing Then
                    runtimeTool.Diameter =
                    Convert.ToDouble(diameterValue)
                End If

            End If

        End If

        Return runtimeTool

    End Function

    Public Shared ToolTypes As List(Of ToolType)
    Public Shared ToolAttributeTypes As List(Of ToolAttributeType)
    Public Shared ToolTypeAttributes As List(Of ToolTypeAttribute)
    Public Shared ToolTypePropertiesList As List(Of ToolTypeProperties)
    Public Shared LayerSetups As List(Of LayerSetup)
    Public Shared Directives As List(Of Directive)

    Public Shared panelWidth As Double
    Public Shared panelHeight As Double

    Public Shared _selectedDxfPath As String = String.Empty
    Public _dxfPath As String = String.Empty

    Public Enum CutSideEnum
        None = 0
        Left = -1
        Right = 1
    End Enum

    Public Enum CutDirectionEnum
        CW = 1
        CCW = -1
        Auto = 0
        As_Specified = 2
    End Enum

    '  LOAD ALL JSON FILES (once)
    ' ------------------------------------------
    Public Shared Sub LoadAllJson()

        ' ---- MACHINE DATA ----
        MachineTypes = LoadTableWrapped(Of MachineType)("MachineTypes.json")

        If MachineTypes.Count = 0 Then
            MessageBox.Show("Configuration Files Not Found! Make sure the you have converted your CMDB before continuing!", "Configuration File Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            End
            'ConfigurationManagerForm.Show()


        End If
        Machines = LoadTableWrapped(Of Machine)("Machines.json")
        MachineAttributeTypes = LoadTableWrapped(Of MachineAttributeType)("MachineAttributeTypes.json")
        MachineAttributes = LoadTableWrapped(Of MachineAttribute)("MachineAttributes.json")
        MachineToolTypes = LoadTableWrapped(Of MachineToolType)("MachineToolTypes.json")

        ' ---- TOOL DATA ----
        ToolSetups = LoadTableWrapped(Of ToolSetup)("ToolSetups.json")
        ToolCrib = LoadTableWrapped(Of ToolCribItem)("ToolCrib.json")
        ToolTypes = LoadTableWrapped(Of ToolType)("ToolTypes.json")
        ToolAttributeTypes = LoadTableWrapped(Of ToolAttributeType)("ToolAttributeTypes.json")
        ToolTypeAttributes = LoadTableWrapped(Of ToolTypeAttribute)("ToolTypeAttributes.json")
        ToolSetupMembers = LoadTableWrapped(Of ToolSetupMember)("ToolSetupMembers.json")

        ' ---- MATERIAL DATA ----
        MaterialTypes = LoadTableWrapped(Of MaterialType)("MaterialTypes.json")
        MaterialSheets = LoadMaterialSheets("MaterialInventory.json")

        ' ---- LAYER SETUPS / DIRECTIVES ----
        Dim layerSetupsFile = Path.Combine(DataFolder, "LayerSetups.json")
        Dim directivesFile = Path.Combine(DataFolder, "Directives.json")

        LayerSetups = If(File.Exists(layerSetupsFile),
        JObject.Parse(File.ReadAllText(layerSetupsFile))("Rows").ToObject(Of List(Of LayerSetup))(),
        New List(Of LayerSetup)())

        Directives = If(File.Exists(directivesFile),
        JObject.Parse(File.ReadAllText(directivesFile))("Rows").ToObject(Of List(Of Directive))(),
        New List(Of Directive)())

        ' Resolve directive colors ONCE
        For Each d In Directives
            d.ResolveColors()
        Next

        ' ------------------------------------------
        ' Bind directives to layer setups (NO DUPES)
        ' ------------------------------------------
        Dim directiveLookup = Directives.ToDictionary(Function(d) d.ID)

        For Each ls In LayerSetups
            ls.Layers.Clear()

            For Each d In Directives.Where(Function(x) x.LayerSetupID = ls.ID)
                ls.Layers.Add(directiveLookup(d.ID))
            Next
        Next

        ValidateConfigurationState()

    End Sub

    Public Shared Sub ValidateConfigurationState()

        ' ---- LayerSetup / Directive invariants ----
        For Each ls In LayerSetups

            If ls.Layers Is Nothing Then
                Throw New InvalidOperationException(
                $"LayerSetup ID {ls.ID} has a null Layers collection."
            )
            End If

            For Each d In ls.Layers

                If d Is Nothing Then
                    Throw New InvalidOperationException(
                    $"LayerSetup ID {ls.ID} contains a null directive."
                )
                End If

                If String.IsNullOrWhiteSpace(d.CADLayer) Then
                    Throw New InvalidOperationException(
                    $"Directive ID {d.ID} in LayerSetup ID {ls.ID} has an empty CADLayer."
                )
                End If

                If d.DisplayColor = Color.Empty Then
                    Throw New InvalidOperationException(
                    $"Directive ID {d.ID} in LayerSetup ID {ls.ID} has no resolved DisplayColor."
                )
                End If
            Next
        Next

    End Sub

    Private Function LoadToolTypeAttributes() As List(Of ToolTypeAttribute)
        Dim filePath = Path.Combine(DataFolder, "ToolTypeAttributes.json")
        If Not File.Exists(filePath) Then Return New List(Of ToolTypeAttribute)()

        Dim jsonText = File.ReadAllText(filePath)
        Dim rootObj = JObject.Parse(jsonText)

        ' Extract the "Rows" array
        Dim rows = rootObj("Rows").ToObject(Of JArray)()

        ' Map JSON rows to ToolTypeAttribute objects
        Dim result As New List(Of ToolTypeAttribute)
        For Each row In rows
            result.Add(New ToolTypeAttribute With {
            .ToolTypeID = row("ToolTypeID").Value(Of Integer)(),
            .AttributeTypeID = row("AttributeTypeID").Value(Of Integer)()
        })
        Next

        Return result
    End Function

    Private Shared Function LoadJObjectWrapped(fileName As String) As List(Of JObject)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Dim json = File.ReadAllText(spath)
            Dim wrapper = JsonConvert.DeserializeObject(Of JObject)(json)
            Dim rows = wrapper("Rows").ToString()
            Return JsonConvert.DeserializeObject(Of List(Of JObject))(rows)
        Else
            Return New List(Of JObject)()
        End If
    End Function

    Private Shared Function LoadMaterialSheetsWrapped(fileName As String) As List(Of MaterialSheet)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Dim json = File.ReadAllText(spath)
            Dim wrapper = JsonConvert.DeserializeObject(Of JObject)(json)
            Dim rows = wrapper("Rows").ToString()
            Dim sheets = JsonConvert.DeserializeObject(Of List(Of MaterialSheet))(rows)

            ' Populate Values collection for each sheet
            For Each sheet As MaterialSheet In sheets
                If sheet.Values Is Nothing Then
                    sheet.Values = New List(Of MaterialParameter)
                End If

                ' Add all properties from the original JObject except ID and TypeID
                Dim rowObj As JObject = JObject.Parse(JsonConvert.SerializeObject(sheet)) ' serialize to get full props
                For Each prop As JProperty In rowObj.Properties()
                    If prop.Name <> "ID" AndAlso prop.Name <> "TypeID" Then
                        Dim val As Object = Nothing
                        If prop.Value IsNot Nothing AndAlso prop.Value.Type <> JTokenType.Null Then
                            val = prop.Value
                        End If

                        sheet.Values.Add(New MaterialParameter With {
                        .Name = prop.Name,
                        .Display = prop.Name,
                        .Value = val,
                        .DataType = If(TypeOf val Is String, 0, 7),
                        .DefaultValue = val
                    })
                    End If
                Next
            Next

            Return sheets
        Else
            Return New List(Of MaterialSheet)()
        End If
    End Function

    ' ------------------------------------------
    ' GENERIC WRAPPER LOADER
    ' ------------------------------------------
    ' ------------------------------------------
    ' Generic loader for wrapped JSON tables
    ' Supports JSON objects with "TableName" and "Rows", or plain arrays
    ' Optional converter allows special handling (e.g., MaterialSheet)
    ' ------------------------------------------
    Public Shared Function LoadTableWrapped(Of T)(fileName As String, Optional converter As Func(Of JObject, T) = Nothing) As List(Of T)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        Dim result As New List(Of T)

        If Not File.Exists(spath) Then Return result

        Dim json = File.ReadAllText(spath)
        Dim jToken As JToken

        Try
            jToken = JToken.Parse(json)
        Catch ex As Exception
            MessageBox.Show($"Error parsing JSON {fileName}: {ex.Message}")
            Return result
        End Try

        Dim rows As JArray

        If TypeOf jToken Is JObject AndAlso jToken("Rows") IsNot Nothing Then
            rows = jToken("Rows")
        ElseIf TypeOf jToken Is JArray Then
            rows = jToken
        Else
            Return result
        End If

        For Each row As JObject In rows
            If converter IsNot Nothing Then
                result.Add(converter(row))
            Else
                result.Add(row.ToObject(Of T)())
            End If
        Next

        Return result
    End Function

    ' ------------------------------------------
    ' GENERIC JSON TABLE LOADER
    ' ------------------------------------------
    Private Shared Function LoadTable(Of T)(fileName As String) As List(Of T)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Return JsonConvert.DeserializeObject(Of List(Of T))(File.ReadAllText(spath))
        Else
            Return New List(Of T)()
        End If
    End Function

    Private Shared Function LoadJObjectList(fileName As String) As List(Of JObject)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If File.Exists(spath) Then
            Return JsonConvert.DeserializeObject(Of List(Of JObject))(File.ReadAllText(spath))
        Else
            Return New List(Of JObject)()
        End If
    End Function
    Private Shared Function LoadMaterialSheets(fileName As String) As List(Of MaterialSheet)
        Dim spath As String = Path.Combine(DataFolder, fileName)
        If Not File.Exists(spath) Then Return New List(Of MaterialSheet)()

        Dim json = File.ReadAllText(spath)
        Dim wrapper As JObject = JsonConvert.DeserializeObject(Of JObject)(json)
        Dim rows As JArray = wrapper("Rows")

        Dim sheets As New List(Of MaterialSheet)

        For Each row As JObject In rows
            Dim sheet As New MaterialSheet With {
            .ID = row("ID").Value(Of Integer)(),
            .TypeID = row("TypeID").Value(Of Integer)()
        }

            ' Deserialize Values array directly
            Dim valuesArray As JArray = row("Values")
            If valuesArray IsNot Nothing Then
                For Each v As JObject In valuesArray
                    sheet.Values.Add(New MaterialParameter With {
                    .Name = v("Name").ToString(),
                    .Display = v("Display").ToString(),
                    .Value = v("Value"),
                    .DataType = v("DataType").Value(Of Integer)(),
                    .DefaultValue = v("DefaultValue"),
                    .Visible = If(v("Visible") IsNot Nothing, v("Visible").Value(Of Boolean)(), True)
                })
                Next
            End If

            sheets.Add(sheet)
        Next

        Return sheets
    End Function


    Public Class Station
        Public Property ID As Integer
        Public Property MachineID As Integer
        Public Property WorkplaneID As Integer  ' <-- added
        Public Property Description As String
        Public Property Position As String

        ' JSON fields
        Public Property NCCodeNumber As Integer
        Public Property PrimaryCode As String
        Public Property SecondaryCode As String
        Public Property StationSize As Integer?

        ' User-defined properties (UDPs)
        Public Property Properties As Dictionary(Of String, Object)

        Public Sub New()
            Properties = New Dictionary(Of String, Object)()
        End Sub
    End Class

    Public Class StationJsonWrapper
        Public Property TableName As String
        Public Property Columns As JArray
        Public Property Rows As JArray
    End Class

    Public Class ToolSetupMembersWrapper
        Public Property TableName As String
        Public Property Rows As List(Of ToolSetupMember)
    End Class

    Public Class DictionaryPropertyGridWrapper
        Private _dict As Dictionary(Of String, Object)
        Public Sub New(dict As Dictionary(Of String, Object))
            _dict = dict
        End Sub

        Public Property Item(key As String) As Object
            Get
                Return _dict(key)
            End Get
            Set(value As Object)
                _dict(key) = value
            End Set
        End Property
    End Class

    ' Represents a single property for a tool type
    Public Class ToolTypeProperty
        Public Property Name As String               ' Property name shown in property grid
        Public Property DefaultValue As Object       ' Default value when creating a new tool
        Public Property Type As Type                 ' Data type of the property (e.g., Integer, Double, String)
    End Class

    ' Represents all properties for a specific ToolType
    Public Class ToolTypeProperties
        Public Property ToolTypeID As Integer                   ' ID of the ToolType
        Public Property Properties As List(Of ToolTypeProperty) ' List of all properties for this ToolType
    End Class

    ' LayerSetup class for WE-CIM v25
    Public Class layersetup
        ' Unique identifier for the LayerSetup
        Public Property ID As Integer
        Public Property Description As String
        Public Property MachineID As Integer

        ' CAM processing parameters
        Public Property ZLevelMode As Double
        Public Property GapTolerance As Double
        Public Property CleanTolerance As Double
        Public Property FilterTolerance As Double
        Public Property SharpAngle As Double
        Public Property ProcessText As Boolean
        Public Property Restrict_Offset As Boolean
        Public Property Color As Color = Color.Black
        Public Property Visible As Boolean = True

        ' Profile direction: Auto, CW, CCW
        Public Property Direction As ProfileDirectionModeEnum
        Public Enum ProfileDirectionModeEnum
            Auto
            CW
            CCW
        End Enum

        ' Optional: Dictionary for extra parameters/extensions
        Public Property Parameters As Dictionary(Of String, Object) = New Dictionary(Of String, Object)()

        ' Collection of directives/layers for this setup
        Public Property Layers As List(Of Directive) = New List(Of Directive)

        Public Sub Validate()
            If ID <= 0 Then
                Throw New InvalidOperationException("LayerSetup has invalid ID.")
            End If

            If Layers Is Nothing OrElse Layers.Count = 0 Then
                Throw New InvalidOperationException($"LayerSetup {ID} has no directives.")
            End If

            ' Ensure *default* exists
            If Not Layers.Any(Function(d) d.CADLayer.Equals("*default*", StringComparison.OrdinalIgnoreCase)) Then
                Throw New InvalidOperationException($"LayerSetup {ID} is missing *default* directive.")
            End If

            ' Ensure no unresolved directives
            For Each d In Layers
                d.Validate()

                If d.LayerSetupID <> ID Then
                    Throw New InvalidOperationException(
                $"Directive {d.ID} incorrectly assigned to LayerSetup {ID}."
            )
                End If
            Next
        End Sub

        Public Function FindRuleByLayer(layerName As String) As AppData.Directive
            If Layers Is Nothing Then Return Nothing
            Return Layers.FirstOrDefault(Function(r) String.Equals(r.CADLayer, layerName, StringComparison.OrdinalIgnoreCase))
        End Function

        ' Utility method to get the display color for a specific CAD layer
        Public Function GetColorForLayer(cadLayerName As String) As Color?
            If Layers Is Nothing Then Return Nothing

            ' Try to find directive matching the CAD layer
            Dim directive = Layers.FirstOrDefault(Function(d) String.Equals(d.CADLayer, cadLayerName, StringComparison.OrdinalIgnoreCase))

            ' Fallback to "*default*" if not found
            If directive Is Nothing Then
                directive = Layers.FirstOrDefault(Function(d) String.Equals(d.CADLayer, "*default*", StringComparison.OrdinalIgnoreCase))
            End If

            ' Return the DisplayColor if found
            If directive IsNot Nothing Then
                Return directive.DisplayColor
            End If

            ' Not found
            Return Nothing
        End Function

        ' Optional: Add helper to quickly get a parameter by name
        Public Function GetParameter(Of T)(paramName As String, Optional defaultValue As T = Nothing) As T
            If Parameters IsNot Nothing AndAlso Parameters.ContainsKey(paramName) Then
                Return CType(Parameters(paramName), T)
            End If
            Return defaultValue
        End Function

        ' Optional: Set or override a parameter
        Public Sub SetParameter(paramName As String, value As Object)
            If Parameters Is Nothing Then Parameters = New Dictionary(Of String, Object)()
            Parameters(paramName) = value
        End Sub

    End Class
    ' Directive class
    Public Class Directive
        Public Property ID As Integer
        Public Property LayerSetupID As Integer
        Public Property CADLayer As String
        Public Property CAMLayer As String

        ' Raw numeric color from JSON (optional / legacy)
        Public Property Color As Integer

        ' Human-readable color name from JSON
        <JsonProperty("DisplayColor")>
        Public Property DisplayColorName As String

        ' Actual runtime color used for rendering
        <JsonIgnore>
        Public Property DisplayColor As Color

        Public Property Station As String
        Public Property ToolType As Integer?
        Public Property ZLevel As Double
        Public Property CutSide As Integer
        Public Property Distance As Double
        Public Property CutDirection As Integer
        Public Property ToolID As Integer
        Public Sub Validate()
            If ID <= 0 Then
                Throw New InvalidOperationException("Directive ID is invalid.")
            End If

            If String.IsNullOrWhiteSpace(CADLayer) Then
                Throw New InvalidOperationException($"Directive {ID} has no CADLayer.")
            End If

            If LayerSetupID <= 0 Then
                Throw New InvalidOperationException($"Directive {ID} has invalid LayerSetupID.")
            End If

            If DisplayColor.IsEmpty Then
                Throw New InvalidOperationException($"Directive {ID} has unresolved DisplayColor.")
            End If
        End Sub



        Public Sub ResolveColors()

            ' 1. Prefer named color if provided
            If Not String.IsNullOrWhiteSpace(DisplayColorName) Then
                Dim c = System.Drawing.Color.FromName(DisplayColorName)
                If c.IsKnownColor OrElse c.IsNamedColor Then
                    DisplayColor = c
                    Return
                End If
            End If

            ' 2. Fallback to integer ARGB if present
            If Color <> 0 Then
                DisplayColor = ColorTranslator.FromWin32(Color)
                Return
            End If

            ' 3. Absolute fallback
            DisplayColor = System.Drawing.Color.Magenta

        End Sub

    End Class

End Class
