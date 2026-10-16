Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.ComponentModel
Imports System.IO
Imports FabV25_WIN8.ConfigurationManagerForm
Imports FabV25_WIN8.AppData
'Imports WE_CIM_ConfigMan.JSon_Helper
Imports FabV25_WIN8.appMain
Imports System.Data.OleDb
Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports FabV25_WIN8.DynamicToolAttributes
Imports System.Dynamic
Imports System.Linq
Imports System.Drawing

Public Class ConfigurationManagerForm
    Inherits Form

    ' Path to the Data folder
    Public dataFolder As String = Path.Combine(Application.StartupPath, "data")


    ' --- Class-level variables to store JSON data ---

    ' Track current machine ID
    Public currentMachineID As Integer = -1

    ' Map Display → AttributeTypeID for the selected machine
    Private currentAttributeMap As Dictionary(Of String, Integer)
    Public Shared stationsTable As JsonTable(Of JObject)
    Private isLoadingStations As Boolean = False
    Private stationsDataTable As DataTable
    Private currentSelectedToolID As Integer

    Private toolSetupMembersTable As JsonTable(Of JObject)
    'Public Shared toolcribtable As List(Of Tool)
    Private toolcribtable As JsonTable(Of JObject)

    Private machineToolTypesTable As JsonTable(Of JObject)
    Private toolTypesTable As JsonTable(Of JObject)
    Private isLoadingToolSetup As Boolean = False
    Private currentSelectedStationID As Integer = -1
    Private toolDescriptionColumnIndex As Integer = -1
    Public Shared currenttoolSetupID As Integer = -1
    Private toolsetupid As Integer = -1
    '--- Declare this at the form level ---
    Private toolSetupParametersTable As JsonTable(Of JObject)
    Private toolSetupParamsFile As String = Path.Combine(dataFolder, "toolSetupParameters.json")
    Private toolSetupsTable As JsonTable(Of JObject)


    Private materialTypesTable As JsonTable(Of MaterialType)
    Private materialInventoryTable As JsonTable(Of MaterialSheet)
    Private mat As MaterialSheet

    ' Stores the currently edited MaterialSheet
    Private CurrentEditingSheet As MaterialSheet = Nothing
    Private currentMachine As String
    Private currentToolSetup As String
    Private newToolDescription As String
    Private currentToolTypeID As Integer

    Public CurrentSelectedNode As TreeNode

    Private newToolBeingAdded As Tool = Nothing
    Private currentEditingTool As Tool = Nothing



    Public Sub New()
        'Delimiters for formatting internal strings.
        CHR1 = Convert.ToChar(1)

        CHR2 = Convert.ToChar(2)
        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.

    End Sub

    ' --- Form Load ---
    Private Sub ConfigurationManagerForm_Load(sender As Object, e As EventArgs) Handles Me.Load


        ' Commit edits on ToolSetup grid immediately
        AddHandler dgvToolSetup.CurrentCellDirtyStateChanged, Sub(s, args)
                                                                  If dgvToolSetup.IsCurrentCellDirty Then
                                                                      dgvToolSetup.CommitEdit(DataGridViewDataErrorContexts.Commit)
                                                                  End If
                                                              End Sub

        Dim materialTypesPath = Path.Combine(dataFolder, "materialtypes.json")
        Dim materialInventoryPath = Path.Combine(dataFolder, "materialinventory.json")

        ' ----------------------------
        ' Load Material Types
        ' ----------------------------
        If File.Exists(materialTypesPath) Then
            Dim json = File.ReadAllText(materialTypesPath)
            materialTypesTable = JsonConvert.DeserializeObject(Of JsonTable(Of MaterialType))(json)
        Else
            materialTypesTable = New JsonTable(Of MaterialType)()
        End If

        ' ----------------------------
        ' Load Material Sheets / Inventory
        ' ----------------------------
        If File.Exists(materialInventoryPath) Then
            Dim materialInventoryJson = File.ReadAllText(materialInventoryPath)

            ' Deserialize as JsonTable(Of JObject) first
            Dim tmpTable = JsonConvert.DeserializeObject(Of JsonTable(Of JObject))(materialInventoryJson)

            Dim sheets As New List(Of MaterialSheet)
            For Each row In tmpTable.Rows
                Dim sheet As New MaterialSheet With {
                    .ID = row("ID")?.ToObject(Of Integer)(),
                    .TypeID = row("TypeID")?.ToObject(Of Integer)()
                }

                ' Populate Values array
                Dim vals = row("Values")?.ToObject(Of List(Of MaterialParameter))()
                If vals IsNot Nothing Then sheet.Values = vals

                ' Optional: populate convenience properties from Values for PropertyGrid
                sheet.Description = GetMaterialValue(sheet.Values, "Description")
                sheet.Length = CDbl(GetMaterialValue(sheet.Values, "Length", 0))
                sheet.Width = CDbl(GetMaterialValue(sheet.Values, "Width", 0))
                sheet.Thickness = CDbl(GetMaterialValue(sheet.Values, "Thickness", 0))

                sheets.Add(sheet)
            Next

            materialInventoryTable = New JsonTable(Of MaterialSheet) With {
                .TableName = tmpTable.TableName,
                .Columns = tmpTable.Columns,
                .Rows = sheets
            }

        Else
            materialInventoryTable = New JsonTable(Of MaterialSheet)()
        End If


        ' Synchronize AppData with the inventory loaded from JSON.
        AppData.MaterialSheets = materialInventoryTable.Rows

        ' Bind the first MaterialSheet to the property grid for testing
        If AppData.MaterialSheets.Any() Then
            propgridMaterial.SelectedObject = AppData.MaterialSheets(0)
        End If

        LoadToolTypes()
        PopulateTreeView()
        HookTreeViewEvents()

        With tabControlMain
            .Appearance = TabAppearance.FlatButtons
            .ItemSize = New Size(0, 1)
            .SizeMode = TabSizeMode.Fixed
            .Multiline = True          ' sometimes needed
        End With

        lstTools.BackColor = Color.LightGray


    End Sub

    Private Sub LoadToolTypes()
        ' Ensure ToolTypes are loaded
        If AppData.ToolTypes Is Nothing Then
            Dim filePath = Path.Combine(dataFolder, "ToolTypes.json")
            AppData.ToolTypes = If(File.Exists(filePath),
                              JObject.Parse(File.ReadAllText(filePath))("Rows").ToObject(Of List(Of ToolType))(),
                              New List(Of ToolType)())
        End If

        cboToolType.DataSource = AppData.ToolTypes
        cboToolType.DisplayMember = "Description" ' what user sees
        cboToolType.ValueMember = "ID"            '  ToolType.ID
        cboToolType.SelectedIndex = -1            ' optional: no selection initially
    End Sub


    Private Sub HookTreeViewEvents()
        AddHandler TreeViewMachines.AfterSelect, AddressOf TreeViewMachines_AfterSelect
    End Sub

    ' ----------------------------
    ' Helper function to read Value from Values list
    ' ----------------------------
    Private Function GetMaterialValue(values As List(Of MaterialParameter), name As String, Optional defaultValue As Object = Nothing) As Object
        Dim param = values.FirstOrDefault(Function(p) p.Name.Equals(name, StringComparison.OrdinalIgnoreCase))
        If param IsNot Nothing Then Return param.Value
        Return defaultValue
    End Function
    Private Sub PopulateTreeView()
        ' Clear the existing tree
        TreeViewMachines.Nodes.Clear()

        ' Map directives to each LayerSetup
        For Each ls In LayerSetups
            ls.Layers = Directives.Where(Function(d) d.LayerSetupID = ls.ID).ToList()
        Next


        ' -------------------------
        ' Root Node: Machine Types
        ' -------------------------
        Dim rootNode As TreeNode = TreeViewMachines.Nodes.Add("Machine Types")
        rootNode.Tag = EncodeConfigTreeTag(0, 0, 0, 0, 0, 0)

        ' Loop through Machine Types
        For Each mt As MachineType In AppData.MachineTypes
            Dim typeNode As TreeNode = rootNode.Nodes.Add(mt.Display)
            typeNode.Tag = EncodeConfigTreeTag(mt.ID, 0, 0, 0, 0, 1) ' nodetype = 1

            ' Add Machines under this type
            Dim machinesOfType = AppData.Machines.Where(Function(m) m.TypeID = mt.ID)
            For Each machine As Machine In machinesOfType
                Dim machineNode As TreeNode = typeNode.Nodes.Add(machine.Description)
                machineNode.Tag = EncodeConfigTreeTag(mt.ID, machine.ID, 0, 0, 0, 10)

                ' Add "Stations" node
                Dim stationsNode As TreeNode = machineNode.Nodes.Add("Stations")
                stationsNode.Tag = EncodeConfigTreeTag(mt.ID, machine.ID, 0, 0, 0, 11)

                ' Add "Tool Setups" node
                Dim toolSetupsNode As TreeNode = machineNode.Nodes.Add("Tool Setups")
                toolSetupsNode.Tag = EncodeConfigTreeTag(mt.ID, machine.ID, 0, 0, 0, 12)

                ' Add individual ToolSetup nodes under "Tool Setups"
                If AppData.ToolSetups IsNot Nothing Then
                    For Each ts As ToolSetup In AppData.ToolSetups
                        If ts.MachineID = machine.ID Then
                            Dim tsNode As TreeNode = toolSetupsNode.Nodes.Add(ts.Description)
                            tsNode.Tag = EncodeConfigTreeTag(mt.ID, machine.ID, ts.ID, 0, 0, 13) ' Store ToolSetupID

                            ' Layer Setups under Tool Setup
                            Dim lsForMachine = LayerSetups.Where(Function(ls) ls.MachineID = machine.ID).ToList()
                            Dim layerSetupsNode As New TreeNode("Layer Setups")
                            layerSetupsNode.Tag = EncodeConfigTreeTag(mt.ID, machine.ID, ts.ID, 0, 0, 14)


                            ' Add the "Layer Setups" node under the current Tool Setup node
                            tsNode.Nodes.Add(layerSetupsNode)

                            For Each ls In lsForMachine
                                'Dim lsNode As New TreeNode(ls.Description) With {.Tag = ls.ID}
                                Dim lsNode As New TreeNode(ls.Description) With {.Tag = EncodeConfigTreeTag(mt.ID, ts.MachineID, toolsetupid, ls.ID, 0, 15)}
                                layerSetupsNode.Nodes.Add(lsNode)   ' <- add to the Layer Setups node, not tsNode

                                ' Individual layers (directives) under LayerSetup
                                For Each layer In ls.Layers
                                    ' Dim layerNode As New TreeNode(layer.CADLayer) With {.Tag = layer.ID}
                                    Dim layerNode As New TreeNode(layer.CADLayer) With {.Tag = EncodeConfigTreeTag(mt.ID, ts.MachineID, toolsetupid, ls.ID, layer.ID, 16)}
                                    lsNode.Nodes.Add(layerNode)
                                Next
                            Next
                        End If
                    Next
                End If
            Next
        Next

        ' -------------------------
        ' Root Node: Material Inventory
        ' -------------------------
        Dim materialRoot As TreeNode = TreeViewMachines.Nodes.Add("Material Inventory")
        materialRoot.Tag = EncodeConfigTreeTag(0, 0, 0, 0, 0, 100) ' nodetype 100 = root

        ' Loop through Material Types
        If AppData.MaterialTypes IsNot Nothing Then
            For Each mt As MaterialType In AppData.MaterialTypes
                Dim typeNode As TreeNode = materialRoot.Nodes.Add(mt.Description)
                typeNode.Tag = EncodeConfigTreeTag(mt.ID, 0, 0, 0, 0, 101)

                ' Add MaterialSheets under this type
                Dim sheets = AppData.MaterialSheets.Where(Function(s) s.TypeID = mt.ID)
                For Each sheet As MaterialSheet In sheets
                    Dim desc As String = If(String.IsNullOrEmpty(sheet.Description), $"Sheet {sheet.ID}", sheet.Description)
                    Dim sheetNode As TreeNode = typeNode.Nodes.Add(desc)
                    sheetNode.Tag = sheet ' store the MaterialSheet object
                Next
            Next
        End If

        ' Expand the root node
        rootNode.Expand()
    End Sub



    'Private Sub RefreshMaterialInventoryTree(typeID As Integer)
    '    Dim rootNode = TreeViewMachines.Nodes.Cast(Of TreeNode)().FirstOrDefault(Function(n) GetNodeType(n.Tag) = 100)
    '    If rootNode Is Nothing Then Exit Sub

    '    Dim typeNode = rootNode.Nodes.Cast(Of TreeNode)().FirstOrDefault(Function(n) GetNodeID(n.Tag) = typeID)
    '    If typeNode Is Nothing Then Exit Sub

    '    typeNode.Nodes.Clear()

    '    Dim sheets = materialInventoryTable.Rows.Where(Function(s) s.TypeID = typeID)
    '    For Each sheet As MaterialSheet In sheets
    '        Dim sheetNode As TreeNode = typeNode.Nodes.Add($"{sheet.Thickness} x {sheet.Width} x {sheet.Length}")
    '        sheetNode.Tag = EncodeConfigTreeTag(-1, -1, sheet.ID, 102)
    '    Next

    '    typeNode.Expand()
    'End Sub

    Private Sub LoadSelectedToolSetup(toolSetupID As Integer, machineID As Integer)

        ' --- Load Stations for this machine if needed ---
        If AppData.Stations Is Nothing OrElse
       Not AppData.Stations.Any(Function(s) s.MachineID = machineID) Then

            Dim stationsFile = Path.Combine(dataFolder, "Stations.json")

            AppData.Stations = If(File.Exists(stationsFile),
            JObject.Parse(File.ReadAllText(stationsFile))("Rows").
                ToObject(Of List(Of Station))(),
            New List(Of Station)())
        End If

        ' --- Load ToolCrib if needed ---
        If AppData.ToolCrib Is Nothing Then
            Dim toolCribFile = Path.Combine(dataFolder, "ToolCrib.json")

            AppData.ToolCrib = If(File.Exists(toolCribFile),
            JObject.Parse(File.ReadAllText(toolCribFile))("Rows").
                ToObject(Of List(Of Tool))(),
            New List(Of Tool)())
        End If

        ' --- Load all ToolSetupMembers ---
        Dim toolSetupMembersFile =
        Path.Combine(dataFolder, "ToolSetupMembers.json")

        Dim allMembers As List(Of ToolSetupMember) =
        If(File.Exists(toolSetupMembersFile),
            JObject.Parse(File.ReadAllText(toolSetupMembersFile))("Rows").
                ToObject(Of List(Of ToolSetupMember))(),
            New List(Of ToolSetupMember)())

        ' --- Filter members for this ToolSetup ---
        Dim membersForSetup =
        allMembers.Where(Function(m) m.ToolSetupID = toolSetupID).ToList()

        ' --- Get stations involved in this setup ---
        Dim stationIDs =
        membersForSetup.Select(Function(m) m.StationID).Distinct().ToList()

        Dim stationsForMachine =
        AppData.Stations.Where(Function(s) stationIDs.Contains(s.ID)).ToList()

        ' --- Build DataTable for the grid ---
        Dim dt As New DataTable()

        dt.Columns.Add("NCCodeNumber", GetType(Integer))
        dt.Columns.Add("ToolDescription", GetType(String))
        dt.Columns.Add("IndexAngle", GetType(Double))
        dt.Columns.Add("FixedStation", GetType(Boolean))
        dt.Columns.Add("PrimaryCode", GetType(String))
        dt.Columns.Add("SecondaryCode", GetType(String))
        dt.Columns.Add("StationID", GetType(Integer)) ' Hidden ID column
        dt.Columns.Add("ToolSetupMemberID", GetType(Integer))

        ' --- Add global Tool Setup UDP columns ---
        If AppData.ToolSetupUDPDefinitions IsNot Nothing Then

            For Each udp In AppData.ToolSetupUDPDefinitions

                If String.IsNullOrWhiteSpace(udp.Name) Then Continue For

                ' Avoid duplicate DataTable column names.
                If dt.Columns.Contains(udp.Name) Then
                    MessageBox.Show(
                    "UDP name conflicts with an existing column: " & udp.Name)
                    Continue For
                End If

                Dim columnType As Type = GetType(String)

                Select Case If(udp.DataType, "").Trim().ToLowerInvariant()
                    Case "integer", "int"
                        columnType = GetType(Integer)

                    Case "double", "decimal"
                        columnType = GetType(Double)

                    Case "boolean", "bool"
                        columnType = GetType(Boolean)

                    Case Else
                        columnType = GetType(String)
                End Select

                dt.Columns.Add(udp.Name, columnType)
            Next

        End If

        ' --- Populate rows ---
        For Each member In membersForSetup

            Dim station =
            stationsForMachine.FirstOrDefault(
                Function(s) s.ID = member.StationID)

            If station Is Nothing Then Continue For

            Dim row = dt.NewRow()

            row("NCCodeNumber") = station.NCCodeNumber

            ' --- Get ToolDescription from ToolCrib ---
            Dim toolDesc As String = ""

            Dim tool =
            AppData.ToolCrib.FirstOrDefault(
                Function(t) t.ID = member.ToolID)

            If tool IsNot Nothing Then
                toolDesc = If(tool.Description, "")

                If String.IsNullOrWhiteSpace(toolDesc) AndAlso
               tool.Attributes IsNot Nothing Then

                    If tool.Attributes.ContainsKey("Tool Crib.Description") Then
                        toolDesc =
                        If(tool.Attributes("Tool Crib.Description")?.ToString(), "")
                    End If
                End If
            End If

            row("ToolDescription") = toolDesc
            row("IndexAngle") = member.IndexAngle
            row("FixedStation") = member.FixedStation
            row("PrimaryCode") = station.PrimaryCode
            row("SecondaryCode") = station.SecondaryCode
            row("StationID") = station.ID
            row("ToolSetupMemberID") = member.ID


            ' --- Initialize UDP cells from saved values or global defaults ---
            If AppData.ToolSetupUDPDefinitions IsNot Nothing Then

                For Each udp In AppData.ToolSetupUDPDefinitions

                    If String.IsNullOrWhiteSpace(udp.Name) Then Continue For
                    If Not dt.Columns.Contains(udp.Name) Then Continue For

                    ' Find the saved value for this specific tool row and UDP.
                    Dim savedValue = AppData.ToolSetupUDPValues.
            FirstOrDefault(
                Function(v) v.ToolSetupMemberID = member.ID AndAlso
                    v.ToolSetupID = toolSetupID AndAlso
                    String.Equals(
                        v.ParameterName,
                        udp.Name,
                        StringComparison.OrdinalIgnoreCase))

                    Dim valueToLoad As String =
            If(savedValue IsNot Nothing,
               savedValue.Value,
               udp.Default)

                    If String.IsNullOrWhiteSpace(valueToLoad) Then
                        row(udp.Name) = DBNull.Value
                    Else
                        Try
                            Dim columnType As Type = dt.Columns(udp.Name).DataType

                            If columnType Is GetType(Integer) Then
                                row(udp.Name) = Integer.Parse(valueToLoad)

                            ElseIf columnType Is GetType(Double) Then
                                row(udp.Name) = Double.Parse(
                        valueToLoad,
                        Globalization.NumberStyles.Any,
                        Globalization.CultureInfo.InvariantCulture)

                            ElseIf columnType Is GetType(Boolean) Then
                                row(udp.Name) = Boolean.Parse(valueToLoad)

                            Else
                                row(udp.Name) = valueToLoad
                            End If

                        Catch
                            ' Invalid stored/default value: leave the cell empty.
                            row(udp.Name) = DBNull.Value
                        End Try
                    End If

                Next

            End If


            dt.Rows.Add(row)

            ' Store the currently selected tool for lstTools integration.
            currentSelectedToolID = member.ToolID

        Next

        ' --- Bind DataTable to grid ---
        dgvToolSetup.DataSource = dt

        If dgvToolSetup.Columns.Contains("ToolSetupMemberID") Then
            dgvToolSetup.Columns("ToolSetupMemberID").Visible = False
        End If

        If dgvToolSetup.Columns.Contains("StationID") Then
            dgvToolSetup.Columns("StationID").Visible = False
        End If

        ' --- Use each UDP's Display value as the grid column caption ---
        If AppData.ToolSetupUDPDefinitions IsNot Nothing Then

            For Each udp In AppData.ToolSetupUDPDefinitions

                If String.IsNullOrWhiteSpace(udp.Name) Then Continue For
                If Not dgvToolSetup.Columns.Contains(udp.Name) Then Continue For

                With dgvToolSetup.Columns(udp.Name)
                    .HeaderText = If(String.IsNullOrWhiteSpace(udp.Display),
                              udp.Name, udp.Display)
                    .Visible = udp.Visible
                End With

            Next

        End If

    End Sub





    'Private Sub TreeViewMachines_AfterSelect(sender As Object, e As TreeViewEventArgs) Handles TreeViewMachines.AfterSelect
    '    Try
    '        Dim selectedNode As TreeNode = e.Node
    '        If selectedNode Is Nothing Then Return

    '        Dim WhichNode As Integer

    '        If TypeOf selectedNode.Tag Is String Then
    '            WhichNode = CInt(GetValueFromDelimitedData(CStr(selectedNode.Tag), "NodeType", "0"))
    '        ElseIf TypeOf selectedNode.Tag Is MaterialSheet Then
    '            WhichNode = 102 ' Material Sheet node
    '        End If

    '        Select Case WhichNode
    '            Case 0 To 9
    '                ' Machine Type nodes
    '                txtPreamble.Text = $"You are on a Node of type {WhichNode}"
    '                tabControlMain.SelectedTab = tabDefinition

    '            Case 10
    '                ' Actual Machine Node
    '                Dim machineID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "Machine_ID", 0)
    '                currentMachineID = machineID
    '                PopulatePropertyGridForMachine(machineID)
    '                dgvStations.DataSource = Nothing
    '                tabControlMain.SelectedTab = tabMachine

    '            Case 11
    '                ' Stations Node
    '                Dim machineID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "Machine_ID", 0)
    '                currentMachineID = machineID
    '                isLoadingStations = True
    '                PopulateStationsGrid(machineID)
    '                isLoadingStations = False
    '                tabControlMain.SelectedTab = tabStations

    '            Case 12
    '                ' Tool Setups Node
    '                Dim machineID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "Machine_ID", 0)
    '                currentMachineID = machineID
    '                txtPreamble.Text = "Select a Tool Setup to view details."
    '                dgvToolSetup.DataSource = Nothing
    '                tabControlMain.SelectedTab = tabDefinition

    '            Case 13
    '                ' Actual Tool Setup Node
    '                Dim machineID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "Machine_ID", 0)
    '                currentMachineID = machineID
    '                Dim toolSetupID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "ToolSetup_ID", -1)
    '                LoadSelectedToolSetup(toolSetupID)

    '                Dim machineTypeID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "MachineType_ID", 0)
    '                PopulateToolTypeComboBox(machineTypeID)
    '                InitializeToolDescriptionColumnIndex()
    '                tabControlMain.SelectedTab = tabToolSetup

    '            Case 100
    '                propgridMaterial.SelectedObject = Nothing

    '            Case 101
    '                propgridMaterial.SelectedObject = Nothing

    '            Case 102
    '                If TypeOf selectedNode.Tag Is MaterialSheet Then
    '                    propgridMaterial.SelectedObject = DirectCast(selectedNode.Tag, MaterialSheetWrapper)
    '                    tabControlMain.SelectedTab = tabMaterialInventory
    '                Else
    '                    propgridMaterial.SelectedObject = Nothing
    '                End If

    '        End Select

    '    Catch ex As Exception
    '        MessageBox.Show($"Error selecting node: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
    '    End Try
    'End Sub
    Private Sub TreeViewMachines_AfterSelect(sender As Object, e As TreeViewEventArgs) Handles TreeViewMachines.AfterSelect
        'Try

        CurrentSelectedNode = TreeViewMachines.SelectedNode

            Dim selectedNode = e.Node
            Dim WhichNode As Integer

            ' Determine node type

            ' MATERIAL MUST HAVE PRIORITY
            WhichNode = -1

            ' --- MATERIAL HAS PRIORITY ---
            If TypeOf selectedNode.Tag Is MaterialSheet Then

                WhichNode = 102

            ElseIf TypeOf selectedNode.Tag Is String Then

                WhichNode = CInt(GetValueFromDelimitedData(
                        CStr(selectedNode.Tag),
                        "NodeType",
                        "0"))
            End If


            Select Case WhichNode
                Case 0 To 9
                    If TypeOf selectedNode.Tag Is MaterialSheet Then Exit Select
                    txtPreamble.Text = $"You are on the {selectedNode.Text} Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 10 ' Actual Machine Node
                    Dim machineID = GetValueFromDelimitedData(selectedNode.Tag, "Machine_ID", 0)
                    currentMachineID = machineID
                    PopulatePropertyGridForMachine(machineID)
                    tabControlMain.SelectedTab = tabMachine
                    dgvStations.DataSource = Nothing ' clear stations until loaded

                Case 11 ' Stations Node
                    Dim machineID = GetValueFromDelimitedData(selectedNode.Tag, "Machine_ID", 0)
                    currentMachineID = machineID
                    PopulateStationsGrid(machineID)
                    tabControlMain.SelectedTab = tabStations

                Case 12 ' Tool Setups Node
                    Dim machineID = GetValueFromDelimitedData(selectedNode.Tag, "Machine_ID", 0)
                    currentMachineID = machineID
                    txtPreamble.Text = "Select a Tool Setup to view details."
                    tabControlMain.SelectedTab = tabDefinition
                    dgvToolSetup.DataSource = Nothing



                Case 13 ' Actual Tool Setup Node
                    Dim machineID = GetValueFromDelimitedData(selectedNode.Tag, "Machine_ID", 0)
                    currentMachineID = machineID
                    currentMachine = selectedNode.Parent.Parent.Text
                    Dim toolSetupID = GetValueFromDelimitedData(selectedNode.Tag, "ToolSetup_ID", -1)
                    currenttoolSetupID = toolSetupID
                    currentToolSetup = selectedNode.Text
                    LoadSelectedToolSetup(toolSetupID, machineID)

                    Dim machineTypeID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "MachineType_ID", 0)
                    PopulateToolTypeComboBox(machineTypeID)
                    tabControlMain.SelectedTab = tabToolSetup

                Case 14 ' Layer Setup Root Node
                    ' Set up the property grid for the Layer Setup root
                    propgrdLayerSetup.SelectedObject = Nothing



                    txtPreamble.Text = "Select a Layer Setup to view details."
                    tabControlMain.SelectedTab = tabDefinition

                Case 15 ' Layer Setup Node
                    ' Set up the property grid for the specific Layer Setup
                    CurrentSelectedNode = selectedNode
                    Dim layerSetupID = GetValueFromDelimitedData(selectedNode.Tag, "LayerSetup_ID", 0)
                    Dim layerSetup = LayerSetups.FirstOrDefault(Function(ls) ls.ID = layerSetupID)
                    propgrdLayerSetup.SelectedObject = layerSetup
                    txtPreamble.Text = $"Viewing Layer Setup: {selectedNode.Text}"
                    tabControlMain.SelectedTab = tabLayerSetup

                Case 16 ' Individual Layer Node
                    ' Set up the property grid for the individual layer
                    CurrentSelectedNode = selectedNode
                    Dim directiveID = GetValueFromDelimitedData(selectedNode.Tag, "Directive_ID", 0)
                    Dim directive = Directives.FirstOrDefault(Function(d) d.ID = directiveID)
                    propgrdLayer.SelectedObject = directive
                    txtPreamble.Text = $"Viewing Layer: {selectedNode.Text}"
                    tabControlMain.SelectedTab = tabLayer

                Case 100 To 102

                    ' Materials are independent from machine context
                    currentMachineID = 0
                    currenttoolSetupID = 0

                    mat = TryCast(selectedNode.Tag, MaterialSheet)

                    If mat IsNot Nothing Then
                        tabControlMain.SelectedTab = tabMaterialInventory

                        propgridMaterial.Dock = DockStyle.Fill
                        propgridMaterial.PropertySort = PropertySort.Alphabetical
                        propgridMaterial.SelectedObject = mat


                    Else
                        propgridMaterial.SelectedObject = Nothing
                    End If

                    propgridMaterial.Refresh()

            End Select

        'Catch ex As Exception
        '    MessageBox.Show($"Error selecting node: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        'End Try
    End Sub


    Private Sub GetNodeInfo(node As TreeNode, ByRef nodeType As Integer, ByRef machineID As Integer, ByRef toolSetupID As Integer)
        If TypeOf node.Tag Is String Then
            nodeType = CInt(GetValueFromDelimitedData(CStr(node.Tag), "NodeType", "0"))
            machineID = GetValueFromDelimitedData(CStr(node.Tag), "Machine_ID", 0)
            toolSetupID = GetValueFromDelimitedData(CStr(node.Tag), "ToolSetup_ID", 0)
        Else
            ' Object-based node (e.g., MaterialSheet)
            nodeType = 102
            machineID = 0
            toolSetupID = 0
        End If
    End Sub

    ' --- Populate the PropertyGrid dynamically for a machine ---
    Private Sub PopulatePropertyGridForMachine(machineID As Integer)
        ' Find machine in AppData
        Dim machine = AppData.Machines.FirstOrDefault(Function(m) m.ID = machineID)
        If machine Is Nothing Then
            propgrdMachine.SelectedObject = Nothing
            Return
        End If

        ' Get attributes for this machine
        Dim attrs As List(Of MachineAttribute) = AppData.MachineAttributes _
        .Where(Function(a) a.MachineID = machine.ID).ToList()

        ' Get types
        Dim types As List(Of MachineAttributeType) = AppData.MachineAttributeTypes

        currentAttributeMap = attrs _
                                        .Join(
                                            types,
                                            Function(a) a.AttributeTypeID,
                                            Function(t) t.ID,
                                            Function(a, t) New With {
                                                .Display = t.Display,
                                                .AttributeTypeID = a.AttributeTypeID
                                            }) _
                                        .ToDictionary(
                                            Function(x) x.Display,
                                            Function(x) x.AttributeTypeID)


        ' Create the dynamic property bag
        propgrdMachine.SelectedObject = Nothing
        propgrdMachine.SelectedObject = New DynamicMachinePropertyBag(attrs, types)


        propgrdMachine.Refresh()
    End Sub
    Private Sub PopulateStationsGrid(machineID As Integer)
        'Try
        Dim stationsForMachine As List(Of Station)
            ' Load the JSON file into a JObject
            Dim json As JObject = JObject.Parse(File.ReadAllText(Path.Combine(dataFolder, "Stations.json")))

            ' Extract the "Rows" array and deserialize to List(Of Station)
            Dim allStations As List(Of Station) = json("Rows") _
                                               .ToObject(Of List(Of Station))()

            If allStations Is Nothing Then
                stationsForMachine = New List(Of Station)()
            Else
                stationsForMachine = allStations.Where(Function(s) s.MachineID = machineID).ToList()
            End If


            ' Bind to grid
            dgvStations.DataSource = stationsForMachine

        'Catch ex As Exception
        '    MessageBox.Show($"Error populating stations: {ex.Message}")
        'End Try
    End Sub




    Private Sub propgrdMachine_PropertyValueChanged(s As Object, e As PropertyValueChangedEventArgs) Handles propgrdMachine.PropertyValueChanged
        ' Safety check
        If currentMachineID < 0 OrElse currentAttributeMap Is Nothing Then Exit Sub

        Dim displayName As String = e.ChangedItem.Label

        ' Ensure this display name is mapped to an AttributeTypeID
        If Not currentAttributeMap.ContainsKey(displayName) Then Exit Sub

        Dim attrTypeID As Integer = currentAttributeMap(displayName)
        Dim newValue As String = CStr(e.ChangedItem.Value)

        ' Find the matching MachineAttribute row
        Dim target = AppData.MachineAttributesTable.Rows _
                 .FirstOrDefault(Function(a) a.MachineID = currentMachineID AndAlso
                                             a.AttributeTypeID = attrTypeID)

        If target IsNot Nothing Then
            ' Update existing value
            target.Value = newValue
        Else
            ' Add new attribute if needed
            target = New MachineAttribute With {
            .MachineID = currentMachineID,
            .AttributeTypeID = attrTypeID,
            .Value = newValue
        }
            AppData.MachineAttributesTable.Rows.Add(target)
        End If

        ' --- Centralized save routine ---
        SaveMachineAttributes()
    End Sub



    Private Sub MoveToNextProperty()
        Dim grid As PropertyGrid = propgrdMachine
        Dim sel As GridItem = grid.SelectedGridItem

        If sel Is Nothing Then Return

        ' Get parent collection (all properties at this level)
        Dim parentCollection As GridItemCollection = If(sel.Parent IsNot Nothing, sel.Parent.GridItems, grid.SelectedGridItem.GridItems)

        ' Find the index of the current property
        Dim index As Integer = 0
        For i As Integer = 0 To parentCollection.Count - 1
            If parentCollection(i) Is sel Then
                index = i
                Exit For
            End If
        Next

        ' Move to next property, wrap around if at the end
        Dim nextIndex As Integer = If(index < parentCollection.Count - 1, index + 1, 0)
        grid.SelectedGridItem = parentCollection(nextIndex)

        ' Automatically start editing the value
        Dim editMethod As System.Reflection.MethodInfo = grid.GetType().GetMethod("FocusOnSelectedGridItem", System.Reflection.BindingFlags.Instance Or System.Reflection.BindingFlags.NonPublic)
        If editMethod IsNot Nothing Then
            editMethod.Invoke(grid, Nothing)
        End If
    End Sub


    Private Sub RefreshPropertyGridForMachine(machineID As Integer)
        ' Find the machine
        Dim machine = AppData.Machines.FirstOrDefault(
        Function(m) m.ID = machineID)

        If machine Is Nothing Then
            propgrdMachine.SelectedObject = Nothing
            currentAttributeMap = Nothing
            Return
        End If

        ' Get the machine's current attribute rows
        Dim attrs As List(Of MachineAttribute) =
        AppData.MachineAttributes.
        Where(Function(a) a.MachineID = machine.ID).ToList()

        ' Get the attribute type definitions
        Dim types As List(Of MachineAttributeType) =
        AppData.MachineAttributeTypes

        ' Rebuild the display-name to attribute-type-ID mapping
        currentAttributeMap = attrs.
        Join(
            types,
            Function(a) a.AttributeTypeID,
            Function(t) t.ID,
            Function(a, t) New With {
                .Display = t.Display,
                .AttributeTypeID = a.AttributeTypeID
            }).
        ToDictionary(
            Function(x) x.Display,
            Function(x) x.AttributeTypeID)

        ' Rebuild the PropertyGrid using the same model as initial population
        propgrdMachine.SelectedObject = Nothing
        propgrdMachine.SelectedObject =
        New DynamicMachinePropertyBag(attrs, types)

        propgrdMachine.Refresh()
    End Sub


    Private Sub btmRemoveMachineParam_Click(sender As Object, e As EventArgs) Handles btmRemoveMachineParam.Click

        ' Safety checks
        If currentMachineID < 0 OrElse currentAttributeMap Is Nothing Then
            MessageBox.Show("No machine selected or attribute map not initialized.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Get the currently selected property in the PropertyGrid
        Dim selectedProp As GridItem = propgrdMachine.SelectedGridItem

        If selectedProp Is Nothing OrElse String.IsNullOrWhiteSpace(selectedProp.Label) Then
            MessageBox.Show("No property selected to remove.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        Dim displayName As String = selectedProp.Label

        ' Ensure displayName maps to an AttributeTypeID
        If Not currentAttributeMap.ContainsKey(displayName) Then
            MessageBox.Show($"Attribute '{displayName}' cannot be removed because it is not recognized.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        Dim attrTypeID As Integer = currentAttributeMap(displayName)

        ' Find and remove the attribute from the table
        Dim targetAttr = AppData.MachineAttributesTable.Rows _
                     .FirstOrDefault(Function(a) a.MachineID = currentMachineID AndAlso a.AttributeTypeID = attrTypeID)

        If targetAttr IsNot Nothing Then
            AppData.MachineAttributesTable.Rows.Remove(targetAttr)
        Else
            MessageBox.Show($"Attribute '{displayName}' not found in table.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Refresh property grid
        RefreshPropertyGridForMachine(currentMachineID)

        SaveMachineAttributes()

    End Sub

    Private Sub dgvStations_CurrentCellDirtyStateChanged(sender As Object, e As EventArgs) Handles dgvStations.CurrentCellDirtyStateChanged

        If dgvStations.IsCurrentCellDirty Then
            dgvStations.CommitEdit(DataGridViewDataErrorContexts.Commit)
        End If

    End Sub
    Private Sub dgvStations_CellValueChanged(sender As Object, e As DataGridViewCellEventArgs) Handles dgvStations.CellValueChanged
        If isLoadingStations Then Return
        If e.RowIndex < 0 OrElse e.ColumnIndex < 0 Then Return
        If AppData.StationsTable Is Nothing Then Return

        Dim row = dgvStations.Rows(e.RowIndex)
        Dim idValue = row.Cells("ID").Value
        If idValue Is Nothing OrElse idValue Is DBNull.Value Then Return

        Dim stationID = idValue.ToString()
        Dim colField = dgvStations.Columns(e.ColumnIndex).Name
        Dim newValue = row.Cells(e.ColumnIndex).Value

        ' Find the authoritative station record.
        Dim jObj = AppData.StationsTable.Rows.FirstOrDefault(
        Function(r) r("ID").Value(Of String)() = stationID)

        If jObj Is Nothing OrElse jObj(colField) Is Nothing Then Return

        ' Preserve the existing JSON value type.
        Dim originalType = jObj(colField).Type

        Select Case originalType
            Case JTokenType.Integer
                jObj(colField) = Convert.ToInt32(newValue)
            Case JTokenType.Float
                jObj(colField) = Convert.ToDouble(newValue)
            Case JTokenType.Boolean
                jObj(colField) = Convert.ToBoolean(newValue)
            Case Else
                jObj(colField) = JToken.FromObject(newValue)
        End Select

        SaveStationsJson()
    End Sub
    Private Sub SaveStationsJson()
        If AppData.StationsTable Is Nothing Then Exit Sub

        Dim output = JsonConvert.SerializeObject(
        AppData.StationsTable, Formatting.Indented)

        File.WriteAllText(
        Path.Combine(dataFolder, "Stations.json"), output)
    End Sub

    ' --- Event to allow only numbers, decimal, and control keys in numeric columns ---
    Private Sub NumericColumn_KeyPress(sender As Object, e As KeyPressEventArgs)
        If Not Char.IsControl(e.KeyChar) AndAlso Not Char.IsDigit(e.KeyChar) AndAlso e.KeyChar <> "."c Then
            e.Handled = True
        End If
    End Sub
    Private Sub bbtnAddStation_Click(sender As Object, e As EventArgs) Handles bbtnAddStation.Click

        Dim table = AppData.StationsTable
        If table Is Nothing Then
            MessageBox.Show("Station data has not been loaded.", "Error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        If currentMachineID < 0 Then
            MessageBox.Show("Please select a machine first.", "Error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' Determine the next NCCodeNumber for this machine.
        Dim machineRows = table.Rows.Where(
        Function(r) r("MachineID").Value(Of Integer)() = currentMachineID).ToList()

        Dim newNCCode As Integer = 1
        If machineRows.Any() Then
            newNCCode = machineRows.Max(
            Function(r) r("NCCodeNumber").Value(Of Integer)()) + 1
        End If

        Dim input As String = InputBox(
        "Enter NCCodeNumber for new station:", "New Station", newNCCode.ToString())

        If Not Integer.TryParse(input, newNCCode) Then
            MessageBox.Show("Invalid NCCodeNumber entered. Operation cancelled.",
                        "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' Generate a unique station ID across all machines.
        Dim newID As Integer = If(
        table.Rows.Count = 0,
        1,
        table.Rows.Max(Function(r) r("ID").Value(Of Integer)()) + 1)

        ' Create the new station using the existing column definitions.
        Dim newStation As New JObject()

        For Each col In table.Columns
            Select Case col.Field
                Case "ID"
                    newStation(col.Field) = newID

                Case "MachineID"
                    newStation(col.Field) = currentMachineID

                Case "NCCodeNumber"
                    newStation(col.Field) = newNCCode

                Case "WorkplaneID"
                    newStation(col.Field) = 1

                Case Else
                    newStation(col.Field) =
            JToken.FromObject(GetDefaultValueForStationColumn(col.Field))
            End Select
        Next

        table.Rows.Add(newStation)
        SaveStationsJson()

        ' Refresh the grid and select the new station.
        PopulateStationsGrid(currentMachineID)

        Dim dgvRow = dgvStations.Rows.Cast(Of DataGridViewRow)().
        FirstOrDefault(
            Function(r) Not r.IsNewRow AndAlso
                r.Cells.Cast(Of DataGridViewCell)().Any(
                    Function(c) c.OwningColumn.Name.Equals(
                        "NCCodeNumber", StringComparison.OrdinalIgnoreCase) AndAlso
                        c.Value IsNot Nothing AndAlso
                        Convert.ToInt32(c.Value) = newNCCode))

        If dgvRow IsNot Nothing Then
            dgvRow.Selected = True
            dgvStations.CurrentCell = dgvRow.Cells(0)
        End If

    End Sub



    Private Sub btnRemoveStation_Click(sender As Object, e As EventArgs) Handles btnRemoveStation.Click

        If dgvStations.SelectedRows.Count = 0 Then
            MessageBox.Show("Please select at least one station to remove.", "Remove Station", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        Dim confirm = MessageBox.Show("Are you sure you want to remove the selected station(s)?", "Confirm Remove", MessageBoxButtons.YesNo, MessageBoxIcon.Question)
        If confirm <> DialogResult.Yes Then Return
        For Each dgvRow As DataGridViewRow In dgvStations.SelectedRows
            If dgvRow.IsNewRow Then Continue For

            Dim idValue = dgvRow.Cells("ID").Value
            If idValue Is Nothing OrElse idValue Is DBNull.Value Then Continue For

            Dim stationID As Integer = Convert.ToInt32(idValue)

            Dim jObj = AppData.StationsTable.Rows.FirstOrDefault(
        Function(r) r("ID").Value(Of Integer)() = stationID AndAlso
                    r("MachineID").Value(Of Integer)() = currentMachineID)

            If jObj IsNot Nothing Then
                AppData.StationsTable.Rows.Remove(jObj)
            End If
        Next

        SaveStationsJson()
        PopulateStationsGrid(currentMachineID)

        ' Save JSON
        'SaveStationsJson()

        ' Refresh grid
        'PopulateStationsGrid(currentMachineID)

    End Sub

    Private Sub btnAutoFill_Click(sender As Object, e As EventArgs) Handles btnAutoFill.Click

        Dim table = AppData.StationsTable

        If table Is Nothing Then
            MessageBox.Show("Station data has not been loaded.", "Error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        If currentMachineID < 0 Then
            MessageBox.Show("Please select a machine first.", "Error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' --- 1. Determine starting NCCodeNumber ---
        Dim machineStations = table.Rows.Where(
        Function(r) r("MachineID").Value(Of Integer)() = currentMachineID).ToList()

        Dim firstNCCode As Integer = 1

        If machineStations.Any() Then
            firstNCCode = machineStations.Max(
            Function(r) r("NCCodeNumber").Value(Of Integer)()) + 1
        End If

        ' --- 2. Ask user how many stations to add ---
        Dim input As String = InputBox(
        "Enter the number of stations to auto-fill for this machine:",
        "Auto-Fill Stations", "1")

        Dim count As Integer

        If Not Integer.TryParse(input, count) OrElse count <= 0 Then
            MessageBox.Show("Invalid number entered. Operation cancelled.",
                        "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' --- 3. Add stations ---
        For i As Integer = 0 To count - 1

            Dim newID As Integer =
            If(table.Rows.Count = 0, 1,
               table.Rows.Max(Function(r) r("ID").Value(Of Integer)()) + 1)

            Dim newStation As New JObject()

            For Each col In table.Columns
                Select Case col.Field
                    Case "ID"
                        newStation(col.Field) = newID
                    Case "MachineID"
                        newStation(col.Field) = currentMachineID
                    Case "NCCodeNumber"
                        newStation(col.Field) = firstNCCode + i
                    Case "WorkplaneID"
                        newStation(col.Field) = 1
                    Case Else
                        newStation(col.Field) =
                        JToken.FromObject(GetDefaultValueForStationColumn(col.Field))
                End Select
            Next

            table.Rows.Add(newStation)
        Next

        ' --- 4. Save JSON ---
        SaveStationsJson()

        ' --- 5. Refresh grid ---
        PopulateStationsGrid(currentMachineID)

        ' --- 6. Select first newly added station ---
        Dim firstRow = dgvStations.Rows.Cast(Of DataGridViewRow)().
        FirstOrDefault(
            Function(r) Not r.IsNewRow AndAlso
                r.Cells("NCCodeNumber").Value IsNot Nothing AndAlso
                Convert.ToInt32(r.Cells("NCCodeNumber").Value) = firstNCCode)

        If firstRow IsNot Nothing Then
            firstRow.Selected = True
            dgvStations.CurrentCell = firstRow.Cells(0)
        End If

        MessageBox.Show(
        $"{count} station(s) added and auto-filled for this machine.",
        "Auto-Fill Complete",
        MessageBoxButtons.OK, MessageBoxIcon.Information)

    End Sub



    Private Function GetDefaultValueForStationColumn(columnName As String) As Object
        Select Case columnName
            Case "NCCodeNumber"
                ' Normally assigned dynamically, so skip here
                Return 1
            Case "MachineID"
                Return currentMachineID
            Case "StationName"
                Return "New Station"
            Case "SomeBooleanColumn"
                Return False
            Case "SomeIntegerColumn"
                Return 0
            Case "SomeDoubleColumn"
                Return 0.0
            Case "SomeStringColumn"
                Return ""
            Case Else
                ' Fallback: empty string
                Return ""
        End Select
    End Function


    Public Function EncodeConfigTreeTag(MachineType_ID As Integer,
                                 Machine_ID As Integer,
                                 ToolSetup_ID As Integer,
                                 LayerSet_ID As Integer,
                                 Directive_ID As Integer,
                                 nodetype As Integer) As String

        'root("Machine Types") nodetype = 1
        '  └─ MachineType (Punch, Punch/Plasma,Laser,Punch/Laser, etc.) nodetype = 2
        '      └─ Machine name (Machine Description) nodetype = 3
        '          ├─ Stations (Just shows a table when selected) nodetype = 4
        '          └─ Tool Setups (Tool Setup Main Node nodetype = 5
        '              └─ ToolSetup Name (Tool Setup Description) nodetype = 6 



        EncodeConfigTreeTag = CHR1 & "MachineType_ID" & CHR2 & MachineType_ID &
                    CHR1 & "Machine_ID" & CHR2 & Machine_ID &
                    CHR1 & "ToolSetup_ID" & CHR2 & ToolSetup_ID &
                    CHR1 & "LayerSetup_ID" & CHR2 & LayerSet_ID &
                    CHR1 & "Directive_ID" & CHR2 & Directive_ID &
                    CHR1 & "NodeType" & CHR2 & nodetype



    End Function
    Public Function GetValueFromDelimitedData(
                            formattedString As String,
                            fieldName As String,
                            defval As String) As String

        If String.IsNullOrEmpty(formattedString) Then Return defval

        Dim parts() As String = Split(formattedString, CHR1)

        For Each part In parts
            If String.IsNullOrWhiteSpace(part) Then Continue For

            Dim fieldParts() As String = Split(part, CHR2)

            If fieldParts.Length = 2 Then
                If String.Equals(fieldParts(0), fieldName, StringComparison.OrdinalIgnoreCase) Then
                    Return fieldParts(1)
                End If
            End If
        Next

        Return defval

    End Function

    Private Sub PopulateToolTypeComboBox(machineTypeID As Integer)

        ' Safety checks
        If AppData.MachineToolTypes Is Nothing OrElse AppData.ToolTypes Is Nothing Then
            cboToolType.DataSource = Nothing
            Exit Sub
        End If

        ' Join MachineToolTypes → ToolTypes
        Dim allowedTypes =
        (From mtt In AppData.MachineToolTypes
         Join tt In AppData.ToolTypes
             On mtt.ToolTypeID Equals tt.ID
         Where mtt.MachineTypeID = machineTypeID
         Select New With {
             .ID = tt.ID,
             .Display = tt.Description
         }).Distinct().ToList()

        RemoveHandler cboToolType.SelectedIndexChanged, AddressOf cboToolType_SelectedIndexChanged

        cboToolType.DataSource = allowedTypes
        cboToolType.DisplayMember = "Display"
        cboToolType.ValueMember = "ID"

        If cboToolType.Items.Count > 0 Then
            cboToolType.SelectedIndex = 0
        End If

        AddHandler cboToolType.SelectedIndexChanged, AddressOf cboToolType_SelectedIndexChanged

        LoadToolsForSelectedToolType()
    End Sub





    Private Sub cboToolType_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboToolType.SelectedIndexChanged
        Dim selectedTypeID As Integer
        If cboToolType.SelectedValue IsNot Nothing AndAlso Integer.TryParse(cboToolType.SelectedValue.ToString(), selectedTypeID) Then
            LoadToolsForSelectedToolType()
            Dim gary As String = cboToolType.SelectedValue
        Else
            lstTools.DataSource = Nothing
        End If
    End Sub

    Private Sub LoadToolsForSelectedToolType()
        lstTools.DataSource = Nothing  ' reset first

        If cboToolType.SelectedItem Is Nothing Then Exit Sub

        Dim selectedToolTypeID As Integer =
        CInt(cboToolType.SelectedItem.GetType().GetProperty("ID").GetValue(cboToolType.SelectedItem))

        If AppData.ToolCrib Is Nothing Then
            Dim sfile = Path.Combine(dataFolder, "ToolCrib.json")
            AppData.ToolCrib = If(File.Exists(sfile),
            JObject.Parse(File.ReadAllText(sfile))("Rows").ToObject(Of List(Of Tool))(),
            New List(Of Tool)())
        End If

        Dim tools = AppData.ToolCrib _
        .Where(Function(t) t.TypeID = selectedToolTypeID) _
        .OrderBy(Function(t) t.Description) _
        .ToList()

        lstTools.DataSource = tools
        lstTools.DisplayMember = "Description"
        lstTools.ValueMember = "ID"
    End Sub


    Private Sub lstTools_DoubleClick(sender As Object, e As EventArgs) Handles lstTools.DoubleClick
        ' --- Ensure a station row is selected ---
        If dgvToolSetup.SelectedRows.Count = 0 Then Return
        Dim selectedRow As DataGridViewRow = dgvToolSetup.SelectedRows(0)

        ' --- Ensure StationID exists ---
        If Not dgvToolSetup.Columns.Contains("StationID") Then Return
        Dim stationID As Integer = CInt(selectedRow.Cells("StationID").Value)

        ' --- Ensure a tool is selected ---
        Dim tool = TryCast(lstTools.SelectedItem, Tool)
        If tool Is Nothing Then Return

        ' --- Update the grid's ToolDescription cell ---
        If dgvToolSetup.Columns.Contains("ToolDescription") Then
            selectedRow.Cells("ToolDescription").Value = tool.Description
        End If

        ' --- Update or add ToolSetupMember ---
        Dim existingMember = AppData.ToolSetupMembers _
        .FirstOrDefault(Function(m) m.ToolSetupID = currenttoolSetupID AndAlso m.StationID = stationID)

        If existingMember IsNot Nothing Then
            ' Update existing record
            existingMember.ToolID = tool.ID
        Else
            ' Add new record
            Dim newID As Integer = If(AppData.ToolSetupMembers.Any(),
                                  AppData.ToolSetupMembers.Max(Function(m) m.ID) + 1,
                                  1)

            Dim newMember As New ToolSetupMember With {
            .ID = newID,
            .ToolSetupID = currenttoolSetupID,
            .StationID = stationID,
            .ToolID = tool.ID,
            .FixedStation = False,
            .IndexAngle = 0.0
        }

            AppData.ToolSetupMembers.Add(newMember)
        End If

        ' --- Save changes ---
        SaveToolSetupMembers()
    End Sub



    Private Sub dgvToolSetup_SelectionChanged(sender As Object, e As EventArgs) Handles dgvToolSetup.SelectionChanged
        Try
            If dgvToolSetup.SelectedRows.Count = 0 Then
                currentSelectedStationID = -1
                Return
            End If

            If Not dgvToolSetup.Columns.Contains("StationID") Then
                currentSelectedStationID = -1
                Return
            End If

            Dim cellValue = dgvToolSetup.SelectedRows(0).Cells("StationID").Value
            currentSelectedStationID =
            If(cellValue IsNot Nothing AndAlso cellValue IsNot DBNull.Value,
               CInt(cellValue),
               -1)

        Catch
            currentSelectedStationID = -1
        End Try
    End Sub



    Private Sub InitializeToolDescriptionColumnIndex()
        toolDescriptionColumnIndex = (
        From c In dgvToolSetup.Columns.Cast(Of DataGridViewColumn)()
        Where c.Name = "ToolDescription"
        Select c.Index
    ).First()
    End Sub

    '==============================
    ' Auto-Save Setup
    '==============================

    '' 1. Hook the dirty state changed event so edits are committed immediately
    'Private Sub SetupAutoSave()
    '    AddHandler dgvToolSetup.CurrentCellDirtyStateChanged, Sub(s, args)
    '                                                              If dgvToolSetup.IsCurrentCellDirty Then
    '                                                                  dgvToolSetup.CommitEdit(DataGridViewDataErrorContexts.Commit)
    '                                                              End If
    '                                                          End Sub

    '    ' 2. Track cell value changes
    '    AddHandler dgvToolSetup.CellValueChanged, AddressOf dgvToolSetup_CellValueChanged
    'End Sub


    Private Sub SaveToolSetupMembers()
        Try
            Dim savePath = Path.Combine(dataFolder, "ToolSetupMembers.json")

            ' Wrap in Rows if needed
            Dim wrapper = New With {
            .TableName = "ToolSetupMembers",
            .Rows = AppData.ToolSetupMembers
        }

            File.WriteAllText(savePath, JsonConvert.SerializeObject(wrapper, Formatting.Indented))
        Catch ex As Exception
            MessageBox.Show($"Error saving ToolSetupMembers: {ex.Message}")
        End Try
    End Sub




    ' Helper to get next unique ID for ToolSetupMember
    Private Function GetNextToolSetupMemberID() As Integer
        If toolSetupMembersTable.Rows.Count = 0 Then Return 1
        Return toolSetupMembersTable.Rows _
            .OfType(Of JObject)() _
            .Max(Function(m) m("ID").Value(Of Integer)()) + 1
    End Function


    ' Save list to JSON
    Private Sub SaveToolCribJson()
        Dim path As String = "toolcrib.json"
        Dim json As String = JsonConvert.SerializeObject(toolcribtable, Formatting.Indented)
        File.WriteAllText(path, json)
    End Sub

    ' Refresh lstTools
    Private Sub RefreshToolList()
        lstTools.DataSource = Nothing
        lstTools.DataSource = toolcribtable
        lstTools.DisplayMember = "Description"
        lstTools.ValueMember = "ID"
    End Sub

    ' Select tool by ID
    Private Sub SelectToolInList(toolID As Integer)
        For i As Integer = 0 To lstTools.Items.Count - 1
            Dim t As Tool = CType(lstTools.Items(i), Tool)
            If t.ID = toolID Then
                lstTools.SelectedIndex = i
                Exit For
            End If
        Next
    End Sub

    Private Sub btnSaveToolProps_Click(sender As Object, e As EventArgs) Handles btnSaveToolProps.Click

        Dim dyn = TryCast(propgrdToolProps.SelectedObject, DynamicToolAttributes)
        If dyn Is Nothing Then Return

        Dim savedTool As Tool = Nothing

        If currentEditingTool IsNot Nothing Then

            ' EDIT: Update the existing tool.
            currentEditingTool.Description = lblCurrentDescription.Text
            currentEditingTool.Attributes = dyn._attributes

            savedTool = currentEditingTool

        ElseIf newToolBeingAdded IsNot Nothing Then

            ' ADD: Update and insert the new tool.
            newToolBeingAdded.Description = lblCurrentDescription.Text
            newToolBeingAdded.Attributes = dyn._attributes

            AppData.ToolCrib.Add(newToolBeingAdded)

            savedTool = newToolBeingAdded

        Else
            MessageBox.Show("Unable to determine whether this is an Add or Edit operation.")
            Return
        End If

        SaveToolCrib()

        ' Preserve the ID before clearing the operation state.
        Dim savedToolID As Integer = savedTool.ID

        currentEditingTool = Nothing
        newToolBeingAdded = Nothing

        ' Refresh the list for the currently selected tool type.
        LoadToolsForSelectedToolType()

        ' Reselect the saved tool.
        For i As Integer = 0 To lstTools.Items.Count - 1
            Dim tool = TryCast(lstTools.Items(i), Tool)

            If tool IsNot Nothing AndAlso tool.ID = savedToolID Then
                lstTools.SelectedIndex = i
                Exit For
            End If
        Next

    End Sub

    Private Sub SaveToolCrib()
        Try
            Dim savePath = Path.Combine(dataFolder, "ToolCrib.json")

            ' Ensure each Tool has its Description in the Attributes dictionary
            For Each tool In AppData.ToolCrib
                If tool.Attributes Is Nothing Then
                    tool.Attributes = New Dictionary(Of String, Object)()
                End If

                ' Sync dictionary with Description
                tool.Attributes("Tool Crib.Description") = tool.Description
            Next

            ' Serialize and save
            Dim json = JsonConvert.SerializeObject(AppData.ToolCrib, Formatting.Indented)
            File.WriteAllText(savePath, json)

        Catch ex As Exception
            MessageBox.Show($"Error saving ToolCrib: {ex.Message}")
        End Try
    End Sub


    Private Sub HighlightToolInList(toolID As Integer)
        For i = 0 To lstTools.Items.Count - 1
            Dim tool = TryCast(lstTools.Items(i), Tool)
            If tool IsNot Nothing AndAlso tool.ID = toolID Then
                lstTools.SelectedIndex = i
                Exit For
            End If
        Next
    End Sub


    Private Sub btnAddTool_Click(sender As Object, e As EventArgs) Handles btnAddTool.Click
        ' Ensure a ToolType is selected
        If cboToolType.SelectedItem Is Nothing Then
            MessageBox.Show("Please select a Tool Type first.")
            Return
        End If

        Dim currentToolTypeID As Integer =
        CInt(cboToolType.SelectedItem.GetType().GetProperty("ID").GetValue(cboToolType.SelectedItem))

        ' Prompt user for new tool name
        newToolDescription = InputBox("Enter the new tool name:", "Add New Tool")
        If String.IsNullOrWhiteSpace(newToolDescription) Then Return

        ' Explicitly establish Add operation state
        currentEditingTool = Nothing
        newToolBeingAdded = Nothing

        ' Switch to Tool Properties tab
        tabControlMain.SelectedTab = tabToolProperties

        ' Populate default properties from ToolTypeAttributes → ToolAttributeTypes
        Dim defaultProps = GetDefaultPropertiesForToolType(currentToolTypeID)

        propgrdToolProps.SelectedObject = New DynamicToolAttributes(defaultProps)


        newToolBeingAdded = New Tool With {
        .ID = GetNextToolCribID(),
        .TypeID = currentToolTypeID,
        .Description = newToolDescription,
        .Attributes = defaultProps
    }

        ' Update labels
        lblCurrentMachine.Text = currentMachine
        lblCurrentToolSetup.Text = currentToolSetup
        lblCurrentDescription.Text = newToolDescription
    End Sub





    Sub SaveTable(Of T)(list As List(Of T), filename As String)
        Dim spath As String = Path.Combine(dataFolder, filename)
        File.WriteAllText(spath, JsonConvert.SerializeObject(list, Formatting.Indented))
    End Sub

    Public Class DynamicPropertyGridObject
        Private _properties As Dictionary(Of String, Object)

        Public Sub New(properties As Dictionary(Of String, Object))
            _properties = properties
        End Sub

        Default Public Property Item(propertyName As String) As Object
            Get
                If _properties.ContainsKey(propertyName) Then
                    Return _properties(propertyName)
                End If
                Return Nothing
            End Get
            Set(value As Object)
                If _properties.ContainsKey(propertyName) Then
                    _properties(propertyName) = value
                End If
            End Set
        End Property

        Public Function GetProperties() As Dictionary(Of String, Object)
            Return _properties
        End Function
    End Class




    Public Class JsonSaver
        Public Shared Sub SaveObjectToJsonFile(ByVal obj As Object, ByVal filePath As String)
            Try
                ' Serialize the object to a JSON string
                Dim jsonString As String = JsonConvert.SerializeObject(obj, Formatting.Indented)

                ' Write the JSON string to a file
                File.WriteAllText(filePath, jsonString)

                ' Optional: log success to console
                Console.WriteLine($"Object successfully saved to {filePath}")

            Catch ex As Exception
                ' Show error to the user
                MessageBox.Show($"Error saving object to JSON file:{Environment.NewLine}{ex.Message}", "Save Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            End Try
        End Sub
    End Class

    Private Sub btnRemoveTool_Click(sender As Object, e As EventArgs) Handles btnRemoveTool.Click

        ' Ensure a tool is selected
        If lstTools.SelectedItem Is Nothing Then
            MessageBox.Show("Please select a tool to delete.")
            Return
        End If

        ' Get selected Tool
        Dim selectedTool = TryCast(lstTools.SelectedItem, Tool)
        If selectedTool Is Nothing Then
            MessageBox.Show("Unable to determine the selected tool.")
            Return
        End If

        ' Confirm deletion
        If MessageBox.Show($"Are you sure you want to delete tool '{selectedTool.Description}'?", "Confirm Delete", MessageBoxButtons.YesNo) <> DialogResult.Yes Then
            Return
        End If

        ' Determine the index of the selected tool
        Dim selectedIndex As Integer = lstTools.SelectedIndex

        ' Remove from ToolCrib
        AppData.ToolCrib.Remove(selectedTool)

        ' Remove all associated ToolSetupMembers
        Dim allMembersFile = Path.Combine(dataFolder, "ToolSetupMembers.json")
        Dim allMembers As List(Of ToolSetupMember) = If(File.Exists(allMembersFile),
        JObject.Parse(File.ReadAllText(allMembersFile))("Rows").ToObject(Of List(Of ToolSetupMember))(),
        New List(Of ToolSetupMember)())

        allMembers.RemoveAll(Function(m) m.ToolID = selectedTool.ID)

        ' Persist changes
        SaveToolCrib()
        File.WriteAllText(allMembersFile, JsonConvert.SerializeObject(New With {Key .Rows = allMembers}, Formatting.Indented))

        ' Refresh UI
        LoadToolsForSelectedToolType()

        ' Highlight next tool if available, otherwise previous
        If lstTools.Items.Count > 0 Then
            Dim newIndex As Integer = Math.Min(selectedIndex, lstTools.Items.Count - 1)
            lstTools.SelectedIndex = newIndex
        End If

        ' Optionally clear grid selection if deleted tool was selected
        dgvToolSetup.ClearSelection()
    End Sub

    Private Sub AddMachineTSM_Click(sender As Object, e As EventArgs) Handles AddMachineTSM.Click

        ' --- 1. Prompt for machine name ---
        Dim name As String = InputBox("Enter new machine name:", "Add Machine")
        If String.IsNullOrWhiteSpace(name) Then Return

        ' --- 2. Check for duplicate machine name ---
        If AppData.MachinesTable.Rows.Any(Function(m) String.Equals(m.Description, name, StringComparison.OrdinalIgnoreCase)) Then
            MessageBox.Show($"A machine with the name '{name}' already exists.", "Duplicate Name", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' --- 3. Get selected machine type ID ---
        Dim nTypeId As Integer = GetValueFromDelimitedData(TreeViewMachines.SelectedNode.Tag, "MachineType_ID", 0)

        ' --- 4. Determine next machine ID ---
        Dim nextID As Integer = If(AppData.MachinesTable.Rows.Count = 0, 1, AppData.MachinesTable.Rows.Max(Function(m) m.ID) + 1)

        ' --- 5. Create new machine ---
        Dim newMachine As New Machine With {
        .ID = nextID,
        .Description = name,
        .TypeID = nTypeId
    }
        AppData.MachinesTable.Rows.Add(newMachine)

        ' --- 6. Create default attributes ---
        For Each attrType In AppData.MachineAttributeTypesTable.Rows
            Dim nextAttrID As Integer = If(AppData.MachineAttributesTable.Rows.Count = 0, 1, AppData.MachineAttributesTable.Rows.Max(Function(a) a.ID) + 1)
            Dim defaultVal As Object = GetDefaultValueForDataType(attrType.DataType)

            Dim newAttr As New MachineAttribute With {
            .ID = nextAttrID,
            .MachineID = newMachine.ID,
            .AttributeTypeID = attrType.ID,
            .Value = CStr(defaultVal)
        }
            AppData.MachineAttributesTable.Rows.Add(newAttr)
        Next


        If AppData.StationsTable Is Nothing Then
            MessageBox.Show("AppData.StationsTable has not been initialized.")
            Return
        End If

        ' --- 7. Create a default station ---
        Dim stations = AppData.StationsTable

        Dim defaultStationID As Integer =
    If(stations.Rows.Count = 0, 1,
       stations.Rows.Max(Function(r) r("ID").Value(Of Integer)()) + 1)

        Dim defaultStation As New JObject()

        For Each col In stations.Columns
            Select Case col.Field
                Case "ID"
                    defaultStation(col.Field) = defaultStationID
                Case "MachineID"
                    defaultStation(col.Field) = newMachine.ID
                Case "NCCodeNumber"
                    defaultStation(col.Field) = 1
                Case "WorkplaneID"
                    defaultStation(col.Field) = 1
                Case Else
                    defaultStation(col.Field) =
                JToken.FromObject(GetDefaultValueForStationColumn(col.Field))
            End Select
        Next

        stations.Rows.Add(defaultStation)

        ' --- 8. Save JSON files ---
        JsonSaver.SaveObjectToJsonFile(AppData.MachinesTable, Path.Combine(dataFolder, "Machines.json"))
        JsonSaver.SaveObjectToJsonFile(AppData.MachineAttributesTable, Path.Combine(dataFolder, "MachineAttributes.json"))
        SaveStationsJson()

        ' --- 9. Refresh TreeView ---
        PopulateTreeView()
        ' --- 10. Select newly added machine in tree ---
        Dim machineNode As TreeNode = FindMachineNode(TreeViewMachines.Nodes, newMachine.ID)

        If machineNode IsNot Nothing Then
            If machineNode.Parent IsNot Nothing Then machineNode.Parent.Expand()
            machineNode.EnsureVisible()
            TreeViewMachines.SelectedNode = machineNode
            machineNode.Expand()
            TreeViewMachines.Focus()
            PopulatePropertyGridForMachine(newMachine.ID)
        End If

    End Sub

    Private Sub AddNewMachine(newMachine As Machine)
        ' --- 1. Find the parent node (e.g., Machine Type Node) ---
        Dim parentNode As TreeNode = FindMachineTypeNode(newMachine.TypeID)
        If parentNode Is Nothing Then

            ' Optionally, create the parent node if it doesn't exist
            parentNode = New TreeNode("Machine Type " & newMachine.TypeID)

            TreeViewMachines.Nodes.Add(parentNode)
        End If

        ' --- 2. Add the new machine node under the parent ---
        Dim machineNode As New TreeNode(newMachine.Description)
        machineNode.Tag = EncodeConfigTreeTag(newMachine.TypeID, newMachine.ID, 0, 0, 0, 10) '"NodeType", 10, "MachineID", newMachine.ID,) ' NodeType 10 = actual machine
        parentNode.Nodes.Add(machineNode)

        ' --- 3. Expand the parent so the new machine is visible ---
        parentNode.Expand()

        ' --- 4. Select the new machine node ---
        TreeViewMachines.SelectedNode = machineNode
        machineNode.EnsureVisible()

        ' --- 5. Populate the property grid ---
        currentMachineID = newMachine.ID
        PopulatePropertyGridForMachine(newMachine.ID)
    End Sub

    Private Sub DeleteMachineTSM_Click(sender As Object, e As EventArgs) Handles DeleteMachineTSM.Click
        If TreeViewMachines.SelectedNode Is Nothing Then
            MessageBox.Show("Please select a machine to delete.", "Delete Machine", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        ' Get the selected machine node
        Dim machineNode As TreeNode = TreeViewMachines.SelectedNode
        Dim machineID As Integer = GetValueFromDelimitedData(CStr(machineNode.Tag), "Machine_ID", -1)

        If machineID = -1 Then
            MessageBox.Show("Selected node is not a valid machine.", "Delete Machine", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Confirm deletion
        If MessageBox.Show($"Are you sure you want to delete machine '{machineNode.Text}' and all its associated data?", "Confirm Delete", MessageBoxButtons.YesNo, MessageBoxIcon.Question) <> DialogResult.Yes Then
            Return
        End If

        Try
            ' --- 1. Remove machine ---
            Dim machineRow = AppData.MachinesTable.Rows.FirstOrDefault(Function(m) m.ID = machineID)
            If machineRow IsNot Nothing Then AppData.MachinesTable.Rows.Remove(machineRow)

            ' --- 2. Remove machine attributes ---
            Dim attrsToRemove = AppData.MachineAttributesTable.Rows.Where(Function(a) a.MachineID = machineID).ToList()
            For Each attr In attrsToRemove
                AppData.MachineAttributesTable.Rows.Remove(attr)
            Next
            ' --- 3. Remove stations ---
            If AppData.StationsTable Is Nothing Then
                Throw New InvalidOperationException("AppData.StationsTable has not been initialized.")
            End If

            Dim stationsToRemove = AppData.StationsTable.Rows _
    .Where(Function(s) s("MachineID").Value(Of Integer)() = machineID) _
    .ToList()

            For Each station In stationsToRemove
                AppData.StationsTable.Rows.Remove(station)
            Next

            ' --- 4. Remove tool setups ---
            'Dim toolsToRemove = ToolSetups.Rows.Where(Function(t) t("MachineID").Value(Of Integer)() = machineID).ToList()
            'For Each tool In toolsToRemove
            '    ToolSetups.Rows.Remove(tool)
            'Next

            ' --- 5. Remove node from TreeView ---
            machineNode.Remove()

            ' --- 6. Clear property grid if this machine was selected ---
            propgridMaterial.SelectedObject = Nothing

            ' --- 7. Save JSON files ---
            JsonSaver.SaveObjectToJsonFile(AppData.MachinesTable, Path.Combine(dataFolder, "Machines.json"))
            JsonSaver.SaveObjectToJsonFile(AppData.MachineAttributesTable, Path.Combine(dataFolder, "MachineAttributes.json"))
            SaveStationsJson()
            SaveToolSetupsJson()

            MessageBox.Show($"Machine '{machineNode.Text}' and all associated data have been deleted.", "Delete Machine", MessageBoxButtons.OK, MessageBoxIcon.Information)

        Catch ex As Exception
            MessageBox.Show($"Error deleting machine: {ex.Message}", "Delete Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub


    Private Sub AddToolSetupTSM_Click(sender As Object, e As EventArgs) Handles AddToolSetupTSM.Click
        Dim name As String = InputBox("Enter new Tool Setup name:", "Add Tool Setup")
        If String.IsNullOrWhiteSpace(name) Then Return
        ' --- Refresh TreeView ---
        PopulateTreeView()
    End Sub

    Private Sub DeleteToolSetupTSM_Click(sender As Object, e As EventArgs) Handles DeleteToolSetupTSM.Click
        '' --- Find selected ToolSetup row ---
        'Dim tsRow As JObject = ToolSetups.Rows.FirstOrDefault(Function(ts) ts("ID").Value(Of Integer)() = currenttoolSetupID)
        'If tsRow Is Nothing Then
        '    MessageBox.Show("Tool Setup not found.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
        '    Return
        'End If

        '' --- Confirm deletion ---
        'If MessageBox.Show($"Are you sure you want to delete Tool Setup '{tsRow("Description")}'?",
        '                   "Confirm Delete", MessageBoxButtons.YesNo, MessageBoxIcon.Question) <> DialogResult.Yes Then
        '    Return
        'End If

        '' --- Remove ---
        'ToolSetups.Rows.Remove(tsRow)

        '' --- Save ---
        'JsonSaver.SaveObjectToJsonFile(ToolSetups, Path.Combine(dataFolder, "toolsetups.json"))

        ' --- Refresh TreeView ---
        PopulateTreeView()
    End Sub
    Private Sub TreeViewMachines_NodeMouseClick(sender As Object, e As TreeNodeMouseClickEventArgs) Handles TreeViewMachines.NodeMouseClick
        If e.Button <> MouseButtons.Right Then Return

        Dim selectedNode As TreeNode = e.Node
        If selectedNode Is Nothing Then Return
        TreeViewMachines.SelectedNode = selectedNode

        cmsTreeActions.Items.Clear()

        Dim WhichNode As Integer
        If TypeOf selectedNode.Tag Is String Then
            WhichNode = CInt(GetValueFromDelimitedData(CStr(selectedNode.Tag), "NodeType", "0"))
        ElseIf TypeOf selectedNode.Tag Is MaterialSheet Then
            WhichNode = 102
        End If

        Select Case WhichNode
            Case 1 To 9
                cmsTreeActions.Items.Add("Add Machine", Nothing, AddressOf AddMachineTSM_Click)

            Case 10
                cmsTreeActions.Items.Add("Delete Machine", Nothing, AddressOf DeleteMachineTSM_Click)

            Case 12
                cmsTreeActions.Items.Add("Add Tool Setup", Nothing, AddressOf AddToolSetupTSM_Click)

            Case 13
                cmsTreeActions.Items.Add("Delete Tool Setup", Nothing, AddressOf DeleteToolSetupTSM_Click)

            Case 14
                cmsTreeActions.Items.Add("Add Layer Setup", Nothing, AddressOf AddLayerSetupTSM_Click)
            Case 15
                cmsTreeActions.Items.Add("Add Layer", Nothing, AddressOf AddLayerTSM_Click)
                cmsTreeActions.Items.Add("Delete Layer Setup", Nothing, AddressOf DeleteLayerSetpTSM_Click)
            Case 16
                cmsTreeActions.Items.Add("Delete Layer ", Nothing, AddressOf DeleteLayerTSM_Click)
            Case 100
                cmsTreeActions.Items.Add("Add Material Type", Nothing, AddressOf AddMaterialTypeTSM_Click)

            Case 101
                cmsTreeActions.Items.Add("Add Material Size", Nothing, AddressOf AddMaterialSizeTSM_Click)
                cmsTreeActions.Items.Add("Delete Material Type", Nothing, AddressOf DeleteMaterialTypeTSM_Click)

            Case 102
                cmsTreeActions.Items.Add("Delete Material Size", Nothing, AddressOf DeleteMaterialSizeTSM_Click)

            Case Else
                selectedNode.ContextMenuStrip = Nothing
        End Select

        cmsTreeActions.Show(TreeViewMachines, e.Location)
    End Sub



    Private Sub btnTSRemoveParameter_Click(sender As Object, e As EventArgs) Handles btnTSRemoveParameter.Click
        If dgvToolSetup.CurrentCell Is Nothing Then Return

        Dim paramName As String = dgvToolSetup.Columns(dgvToolSetup.CurrentCell.ColumnIndex).Name
        RemoveToolSetupParameter(paramName)
    End Sub

    Private Sub btnAddTSParameter_Click(sender As Object, e As EventArgs) Handles btnAddTSParameter.Click

        Using dlg As New dlgAddParam()
            dlg.txtParamDefault.Text = ""
            dlg.chkIsVisible.Checked = True

            If dlg.ShowDialog() <> DialogResult.OK Then Return

            Dim paramName As String = dlg.txtParamName.Text.Trim()
            Dim paramDisplay As String = dlg.txtParamDisplay.Text.Trim()
            Dim dataType As String = dlg.cboParamDataType.Text.Trim()

            If String.IsNullOrWhiteSpace(paramName) Then
                MessageBox.Show("Parameter Name is required.")
                Return
            End If

            If String.IsNullOrWhiteSpace(dataType) Then
                MessageBox.Show("Select a parameter data type.")
                Return
            End If

            If AppData.ToolSetupUDPDefinitions.Any(
            Function(p) String.Equals(
                p.Name, paramName,
                StringComparison.OrdinalIgnoreCase)) Then

                MessageBox.Show("This parameter already exists.")
                Return
            End If

            Dim newDefinition As New AppData.ToolSetupUDPDefinition With {
            .Name = paramName,
            .Display = paramDisplay,
            .Default = dlg.txtParamDefault.Text,
            .DataType = dataType,
            .Visible = dlg.chkIsVisible.Checked
        }

            Dim table = AppData.ToolSetupUDPDefinitionsTable

            If table.Columns Is Nothing Then
                table.Columns = New List(Of AppData.JsonColumn)()
            End If

            ' Initialize wrapper metadata if necessary.
            If table.Columns.Count = 0 Then
                table.TableName = "ToolSetupUDPDefinitions"

                table.Columns.Add(New AppData.JsonColumn With {
                .Field = "Name",
                .Header = "Name",
                .Visible = True,
                .DataType = 0
            })

                table.Columns.Add(New AppData.JsonColumn With {
                .Field = "Display",
                .Header = "Display",
                .Visible = True,
                .DataType = 0
            })

                table.Columns.Add(New AppData.JsonColumn With {
                .Field = "Default",
                .Header = "Default",
                .Visible = True,
                .DataType = 0
            })

                table.Columns.Add(New AppData.JsonColumn With {
                .Field = "DataType",
                .Header = "Data Type",
                .Visible = True,
                .DataType = 0
            })

                table.Columns.Add(New AppData.JsonColumn With {
                .Field = "Visible",
                .Header = "Visible",
                .Visible = True,
                .DataType = 1
            })
            End If

            table.Rows.Add(newDefinition)

            Try
                AppData.SaveJsonTableWrapped(
                table, "ToolSetupUDPDefinitions.json")

                AppData.ToolSetupUDPDefinitions = table.Rows

                LoadSelectedToolSetup(currenttoolSetupID, currentMachineID)

            Catch ex As Exception
                table.Rows.Remove(newDefinition)

                MessageBox.Show(
                "Failed to save Tool Setup UDP definitions: " &
                ex.Message)
                Return
            End Try

            MessageBox.Show("Tool Setup parameter definition added.")
        End Using

    End Sub




    Private Sub RemoveToolSetupParameter(parameterName As String)
        If String.IsNullOrWhiteSpace(parameterName) Then Return

        ' Prevent removal of a fixed grid column.
        Dim fixedColumns As String() = {
        "NCCodeNumber", "ToolDescription", "IndexAngle",
        "FixedStation", "PrimaryCode", "SecondaryCode", "StationID"
    }

        If fixedColumns.Contains(parameterName, StringComparer.OrdinalIgnoreCase) Then
            MessageBox.Show("Only user-defined parameters can be removed.",
                        "Remove Parameter",
                        MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        If AppData.StationsTable Is Nothing Then
            MessageBox.Show("Station data has not been loaded.",
                        "Remove Parameter",
                        MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        If MessageBox.Show(
        $"Are you sure you want to remove the parameter '{parameterName}' from all stations?",
        "Confirm Delete",
        MessageBoxButtons.YesNo,
        MessageBoxIcon.Question) <> DialogResult.Yes Then
            Return
        End If

        Try
            Dim removedCount As Integer = 0

            ' Remove the UDP from every authoritative station record.
            For Each stationRow As JObject In AppData.StationsTable.Rows
                Dim propertiesToken = TryCast(stationRow("Properties"), JObject)

                If propertiesToken IsNot Nothing AndAlso
               propertiesToken.Property(parameterName) IsNot Nothing Then

                    propertiesToken.Remove(parameterName)
                    removedCount += 1
                End If
            Next

            If removedCount = 0 Then
                MessageBox.Show(
                $"Parameter '{parameterName}' was not found in any station.",
                "Remove Parameter",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information)
                Return
            End If

            ' Persist the complete wrapped Stations table.
            SaveStationsJson()

            ' Refresh the selected Tool Setup grid.
            If currenttoolSetupID > 0 Then
                LoadSelectedToolSetup(currenttoolSetupID, currentMachineID)
            End If

            MessageBox.Show(
            $"Parameter '{parameterName}' was removed from {removedCount} station(s).",
            "Remove Parameter",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)

        Catch ex As Exception
            MessageBox.Show(
            $"Error removing parameter: {ex.Message}",
            "Remove Error",
            MessageBoxButtons.OK,
            MessageBoxIcon.Error)
        End Try
    End Sub




    Private Sub btnAddMachineParam_Click(sender As Object, e As EventArgs) Handles btnAddMachineParam.Click

        ' Safety check
        If currentMachineID < 0 Then Return

        ' Show the Add Parameter dialog
        Using dlg As New dlgAddParam()
            dlg.txtParamDefault.Text = ""
            dlg.chkIsVisible.Checked = True

            If dlg.ShowDialog() <> DialogResult.OK Then Return

            Dim paramName As String = dlg.txtParamName.Text.Trim()
            Dim paramDisplay As String = dlg.txtParamDisplay.Text.Trim()
            Dim paramDataType As Integer = CInt(dlg.cboParamDataType.SelectedValue)
            Dim defaultValue As String = dlg.txtParamDefault.Text

            ' Find an existing attribute type by display name
            Dim existingAttrType = AppData.MachineAttributeTypesTable.Rows _
                .FirstOrDefault(Function(t) t.Display = paramDisplay)

            ' If the type exists, check whether this machine already has it
            If existingAttrType IsNot Nothing Then
                Dim existingAttrTypeID As Integer = existingAttrType.ID

                If AppData.MachineAttributesTable.Rows.Any(
                    Function(a) a.MachineID = currentMachineID AndAlso
                                a.AttributeTypeID = existingAttrTypeID) Then

                    MessageBox.Show("This attribute already exists for the machine.")
                    Return
                End If
            End If

            ' Reuse the existing type, or create a new type
            Dim attrTypeID As Integer
            Dim attrTypeCreated As Boolean = False

            If existingAttrType IsNot Nothing Then
                attrTypeID = existingAttrType.ID
            Else
                attrTypeID = If(
                    AppData.MachineAttributeTypesTable.Rows.Count = 0,
                    1,
                    AppData.MachineAttributeTypesTable.Rows.Max(Function(t) t.ID) + 1)

                Dim newAttrType As New MachineAttributeType With {
                    .ID = attrTypeID,
                    .Display = paramDisplay,
                    .DataType = paramDataType
                }

                AppData.MachineAttributeTypesTable.Rows.Add(newAttrType)
                attrTypeCreated = True
            End If

            ' Create the attribute row for the selected machine
            Dim nextAttrID As Integer = If(
                AppData.MachineAttributesTable.Rows.Count = 0,
                1,
                AppData.MachineAttributesTable.Rows.Max(Function(a) a.ID) + 1)

            Dim newAttr As New MachineAttribute With {
                .ID = nextAttrID,
                .MachineID = currentMachineID,
                .AttributeTypeID = attrTypeID,
                .Value = defaultValue
            }

            AppData.MachineAttributesTable.Rows.Add(newAttr)

            ' Refresh the PropertyGrid
            RefreshPropertyGridForMachine(currentMachineID)

            ' Save the machine attribute rows
            SaveMachineAttributes()

            ' Save attribute types only if a new type was created
            If attrTypeCreated Then
                Dim filePath As String = Path.Combine(dataFolder, "MachineAttributeTypes.json")

                File.WriteAllText(
                    filePath,
                    JsonConvert.SerializeObject(
                        AppData.MachineAttributeTypesTable,
                        Formatting.Indented))
            End If

        End Using

    End Sub

    Private Sub AddMachineParameter(paramDisplay As String, defaultValue As Object)
        ' --- Step 1: New attribute type ---
        Dim nextTypeID As Integer
        If AppData.MachineAttributeTypesTable.Rows.Count = 0 Then
            nextTypeID = 1
        Else
            nextTypeID = AppData.MachineAttributeTypesTable.Rows.Max(Function(t) t.ID) + 1
        End If

        Dim newType As New MachineAttributeType With {
            .ID = nextTypeID,
            .Display = paramDisplay
        }
        AppData.MachineAttributeTypesTable.Rows.Add(newType)

        ' --- Step 2: Add attribute for each machine ---
        Dim nextAttrID As Integer = If(AppData.MachineAttributesTable.Rows.Count = 0, 1, AppData.MachineAttributesTable.Rows.Max(Function(a) a.ID) + 1)
        For Each machine In AppData.MachinesTable.Rows
            Dim newAttr As New MachineAttribute With {
                .ID = nextAttrID,
                .MachineID = machine.ID,
                .AttributeTypeID = newType.ID,
                .Value = defaultValue
            }
            AppData.MachineAttributesTable.Rows.Add(newAttr)
            nextAttrID += 1
        Next

        ' --- Step 3: Save JSON ---
        JsonSaver.SaveObjectToJsonFile(AppData.MachineAttributesTable, Path.Combine(dataFolder, "MachineAttributes.json"))
        JsonSaver.SaveObjectToJsonFile(AppData.MachineAttributeTypesTable, Path.Combine(dataFolder, "MachineAttributeTypes.json"))

        ' --- Refresh property grid ---
        PopulatePropertyGridForMachine(AppData.MachinesTable.Rows.First().ID)
    End Sub

    Private Sub AddMaterialTypeTSM_Click(sender As Object, e As EventArgs) Handles AddMaterialTypeTSM.Click

        Dim typeName As String = InputBox("Enter new Material Type name:", "Add Material Type")
        If String.IsNullOrWhiteSpace(typeName) Then Return

        If IsDuplicateMaterialName(typeName) Then
            MessageBox.Show($"Material Type '{typeName}' already exists.", "Duplicate Type", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Generate new ID
        Dim newID As Integer = If(materialTypesTable.Rows.Count = 0, 1, materialTypesTable.Rows.Max(Function(t) t.ID) + 1)

        ' Create new MaterialType
        Dim newType As New MaterialType With {
            .ID = newID,
            .Description = typeName,
            .Display = typeName
        }
        materialTypesTable.Rows.Add(newType)

        ' Add to TreeView
        Dim rootNode As TreeNode = TreeViewMachines.Nodes.Cast(Of TreeNode)().FirstOrDefault(Function(n) n.Text = "Material Inventory")
        If rootNode IsNot Nothing Then
            Dim typeNode As TreeNode = rootNode.Nodes.Add(newType.Display)
            'typeNode.Tag = EncodeConfigTreeTag(newType.ID, -1, -1, 0, 101) ' nodetype 101
            typeNode.Tag = EncodeConfigTreeTag(newType.ID, 0, 0, 0, 0, 101)

        End If

        SaveMaterialTypesJson(dataFolder, materialTypesTable)

    End Sub
    Private Sub AddMaterialSizeTSM_Click(sender As Object, e As EventArgs) Handles AddMaterialSizeTSM.Click

        Dim selectedNode As TreeNode = TreeViewMachines.SelectedNode
        If selectedNode Is Nothing Then Return

        ' Confirm that the selected node is a Material Type node.
        Dim nodeType As Integer =
        CInt(GetValueFromDelimitedData(
            CStr(selectedNode.Tag), "NodeType", "0"))

        If nodeType <> 101 Then
            MessageBox.Show(
            "Please select a Material Type before adding a Material Sheet.",
            "Select Material Type",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information)
            Return
        End If

        ' Read the Material Type ID from the encoded tree-node tag.
        Dim typeID As Integer =
        CInt(GetValueFromDelimitedData(
            CStr(selectedNode.Tag), "MachineType_ID", "-1"))

        If typeID < 0 Then
            MessageBox.Show(
            "Unable to determine the selected Material Type ID.",
            "Invalid Material Type",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)
            Return
        End If

        ' Request the new Material Sheet description.
        Dim sheetName As String =
        InputBox("Enter new Material Sheet name:", "Add Material Sheet")

        If String.IsNullOrWhiteSpace(sheetName) Then Return

        sheetName = sheetName.Trim()

        ' Prevent duplicate sheet names within this Material Type.
        If IsDuplicateMaterialName(sheetName, typeID) Then
            MessageBox.Show(
            $"Material Sheet '{sheetName}' already exists under this Material Type.",
            "Duplicate Sheet",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)
            Return
        End If

        ' Generate a unique Material Sheet ID.
        Dim newID As Integer =
        If(materialInventoryTable.Rows.Count = 0,
           1,
           materialInventoryTable.Rows.Max(Function(s) s.ID) + 1)

        ' Create the new Material Sheet.
        Dim newSheet As New MaterialSheet With {
        .ID = newID,
        .TypeID = typeID
    }
        ' Initialize the Type ID parameter.
        newSheet.Values.Add(New MaterialParameter With {
    .Name = "Type ID",
    .Display = "Type ID",
    .Value = typeID,
    .DataType = 1,
    .DefaultValue = typeID,
    .Visible = True
})
        ' Initialize the Description parameter.
        newSheet.Values.Add(New MaterialParameter With {
        .Name = "Description",
        .Display = "Description",
        .Value = sheetName,
        .DataType = 0,
        .DefaultValue = sheetName,
        .Visible = True
    })

        ' Add the new sheet to the in-memory inventory.
        materialInventoryTable.Rows.Add(newSheet)

        ' Synchronize the application inventory.
        AppData.MaterialSheets = materialInventoryTable.Rows


        ' Persist the updated inventory.
        SaveMaterialInventoryJson(dataFolder, materialInventoryTable)

        ' Rebuild the tree from the synchronized AppData inventory.
        PopulateTreeView()

        ' Find the newly created Material Type node.
        Dim materialRoot As TreeNode =
            TreeViewMachines.Nodes.Cast(Of TreeNode)().
            FirstOrDefault(Function(n) GetNodeType(n.Tag) = 100)

        If materialRoot IsNot Nothing Then
            Dim typeNode As TreeNode =
                materialRoot.Nodes.Cast(Of TreeNode)().
                FirstOrDefault(
                    Function(n)
                        Return GetNodeType(n.Tag) = 101 AndAlso
                            CInt(GetValueFromDelimitedData(
                                CStr(n.Tag), "MachineType_ID", "-1")) = typeID
                    End Function)

            If typeNode IsNot Nothing Then
                ' Find the new sheet by its ID.
                Dim sheetNode As TreeNode =
                    typeNode.Nodes.Cast(Of TreeNode)().
                    FirstOrDefault(
                        Function(n)
                            Dim sheet As MaterialSheet =
                                TryCast(n.Tag, MaterialSheet)

                            Return sheet IsNot Nothing AndAlso
                                sheet.ID = newID
                        End Function)

                If sheetNode IsNot Nothing Then
                    TreeViewMachines.SelectedNode = sheetNode
                    sheetNode.EnsureVisible()
                End If
            End If
        End If

        ' Display the new sheet in the property grid.
        tabControlMain.SelectedTab = tabMaterialInventory
        propgridMaterial.SelectedObject = newSheet
        propgridMaterial.Refresh()


    End Sub

    Private Sub DeleteMaterialTypeTSM_Click(sender As Object, e As EventArgs) Handles DeleteMaterialTypeTSM.Click
        Dim selectedNode As TreeNode = TreeViewMachines.SelectedNode
        If selectedNode Is Nothing Then Return
        Dim NodeType As Integer = GetValueFromDelimitedData(selectedNode.Tag, "NodeType", -1)

        'If Not selectedNode.Tag.ToString().StartsWith("101") Then
        If NodeType <> 101 Then
            MessageBox.Show("Please select a Material Type node to delete.", "Delete Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        Dim typeID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "MachineType_ID", -1) ' Extract ID from your EncodeTag scheme

        If MessageBox.Show($"Are you sure you want to delete Material Type '{selectedNode.Text}'? This will also delete all its sheets.", "Confirm Delete", MessageBoxButtons.YesNo, MessageBoxIcon.Question) = DialogResult.No Then
            Return
        End If

        ' Remove all sheets of this type
        materialInventoryTable.Rows.RemoveAll(Function(s) s.TypeID = typeID)

        ' Remove the type itself
        materialTypesTable.Rows.RemoveAll(Function(t) t.ID = typeID)

        ' Remove node from TreeView
        selectedNode.Remove()

        ' Save changes
        SaveAllMaterialData(dataFolder, materialTypesTable, materialInventoryTable)
    End Sub


    Private Sub DeleteMaterialSizeTSM_Click(
    sender As Object,
    e As EventArgs
) Handles DeleteMaterialSizeTSM.Click

        Dim selectedNode As TreeNode = TreeViewMachines.SelectedNode
        If selectedNode Is Nothing Then Return

        ' Determine whether the selected node represents a Material Sheet.
        Dim selectedSheet As MaterialSheet =
        TryCast(selectedNode.Tag, MaterialSheet)

        Dim sheetID As Integer = -1

        If selectedSheet IsNot Nothing Then
            ' Current implementation: the node stores the MaterialSheet object.
            sheetID = selectedSheet.ID

        ElseIf TypeOf selectedNode.Tag Is String Then
            ' Legacy implementation: the node stores an encoded tag.
            Dim nodeType As Integer =
            CInt(GetValueFromDelimitedData(
                CStr(selectedNode.Tag), "NodeType", "-1"))

            If nodeType <> 102 Then
                MessageBox.Show(
                "Please select a Material Sheet node to delete.",
                "Delete Error",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning)
                Return
            End If

            ' Legacy encoded tags store the sheet ID in Machine_ID.
            sheetID = CInt(GetValueFromDelimitedData(
            CStr(selectedNode.Tag), "Machine_ID", "-1"))
        Else
            MessageBox.Show(
            "Please select a Material Sheet node to delete.",
            "Delete Error",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)
            Return
        End If

        If sheetID < 0 Then
            MessageBox.Show(
            "Unable to determine the Material Sheet ID.",
            "Delete Error",
            MessageBoxButtons.OK,
            MessageBoxIcon.Warning)
            Return
        End If

        ' Confirm deletion.
        If MessageBox.Show(
        $"Are you sure you want to delete Material Sheet '{selectedNode.Text}'?",
        "Confirm Delete",
        MessageBoxButtons.YesNo,
        MessageBoxIcon.Question) <> DialogResult.Yes Then
            Return
        End If

        ' Remove the sheet from the configuration manager inventory.
        materialInventoryTable.Rows.RemoveAll(
        Function(s) s.ID = sheetID)

        ' Synchronize the application inventory.
        AppData.MaterialSheets = materialInventoryTable.Rows

        ' Persist the updated inventory.
        SaveMaterialInventoryJson(dataFolder, materialInventoryTable)

        Dim typeID As Integer = selectedSheet.TypeID

        ' Rebuild the tree from the updated inventory.
        PopulateTreeView()

        ' Find the Material Inventory root.
        Dim materialRoot As TreeNode =
    TreeViewMachines.Nodes.Cast(Of TreeNode)().
    FirstOrDefault(Function(n) GetNodeType(n.Tag) = 100)

        If materialRoot IsNot Nothing Then
            ' Find the Material Type that contained the deleted sheet.
            Dim typeNode As TreeNode =
        materialRoot.Nodes.Cast(Of TreeNode)().
        FirstOrDefault(
            Function(n)
                Return GetNodeType(n.Tag) = 101 AndAlso
                    CInt(GetValueFromDelimitedData(
                        CStr(n.Tag), "MachineType_ID", "-1")) = typeID
            End Function)

            If typeNode IsNot Nothing Then
                materialRoot.Expand()
                typeNode.Expand()
                TreeViewMachines.SelectedNode = typeNode
            End If
        End If

        ' Clear the property grid because the sheet no longer exists.
        propgridMaterial.SelectedObject = Nothing

    End Sub


    Private Function GetNodeType(tag As Object) As Integer
        If tag Is Nothing Then Return -1

        Try
            'Dim tagParts() As String = tag.ToString().Split("|"c)
            'If tagParts.Length = 4 Then
            '    Return CInt(tagParts(3)) ' 4th part is nodetype
            'End If

            GetNodeType = GetValueFromDelimitedData(tag, "NodeType", -1)
        Catch ex As Exception
            ' Ignore, return -1
        End Try

        'Return -1
    End Function

    Private Function GetNodeID(tag As Object) As Integer
        If tag Is Nothing Then Return -1

        Try
            Dim tagParts() As String = tag.ToString().Split("|"c)
            If tagParts.Length = 4 Then
                Return CInt(tagParts(2)) ' 3rd part is ID (Tool/Material/Sheet ID)
            End If
        Catch ex As Exception
            ' Ignore, return -1
        End Try

        Return -1
    End Function

    Private Function GenerateNewMaterialTypeID() As Integer
        ' If table is empty or null, start at 1
        If materialTypesTable Is Nothing OrElse materialTypesTable.Rows Is Nothing OrElse materialTypesTable.Rows.Count = 0 Then
            Return 1
        End If

        ' Otherwise, return max existing ID + 1
        Return materialTypesTable.Rows.Max(Function(t) t.ID) + 1
    End Function


    Private Sub btnSaveMaterial_Click(sender As Object, e As EventArgs) Handles btnSaveMaterial.Click
        Try
            ' Loop through all sheets in the material inventory table
            For Each sheet As MaterialSheet In materialInventoryTable.Rows
                ' Ensure all columns exist as parameters
                For Each col In materialInventoryTable.Columns
                    ' Look for existing parameter by Name
                    Dim param = sheet.Values.FirstOrDefault(Function(p) p.Name = col.Field)

                    If param Is Nothing Then
                        ' If parameter does not exist, add it
                        sheet.Values.Add(New MaterialParameter With {
                        .Name = col.Field,
                        .Display = col.Header,
                        .Value = Nothing,            ' Default value can be set here if needed
                        .DataType = col.DataType,
                        .DefaultValue = Nothing,
                        .Visible = col.Visible
                    })
                    End If
                Next
            Next

            ' Serialize the entire table to JSON
            Dim materialInventoryJson As String = JsonConvert.SerializeObject(materialInventoryTable, Formatting.Indented)

            ' Save JSON file
            Dim materialInventoryPath = Path.Combine(dataFolder, "MaterialInventory.json")
            File.WriteAllText(materialInventoryPath, materialInventoryJson)

            MessageBox.Show("Material inventory saved successfully.", "Save Complete", MessageBoxButtons.OK, MessageBoxIcon.Information)

        Catch ex As Exception
            MessageBox.Show("Error saving material inventory: " & ex.Message, "Save Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    Private Sub btnSaveToolProps_BindingContextChanged(sender As Object, e As EventArgs) Handles btnSaveToolProps.BindingContextChanged

    End Sub


    Public Sub SaveMaterialInventoryJson(dataFolder As String, materialSheets As JsonTable(Of MaterialSheet))
        Try
            ' Ensure folder exists
            If Not Directory.Exists(dataFolder) Then Directory.CreateDirectory(dataFolder)

            Dim materialInventoryPath = Path.Combine(dataFolder, "MaterialInventory.json")

            ' Create backup if file exists
            If File.Exists(materialInventoryPath) Then
                Dim backupPath = Path.Combine(dataFolder, "MaterialInventory_backup_" & DateTime.Now.ToString("yyyyMMdd_HHmmss") & ".json")
                File.Copy(materialInventoryPath, backupPath)
            End If

            ' Serialize and save
            Dim json = JsonConvert.SerializeObject(materialSheets, Formatting.Indented)

            Debug.WriteLine("Default settings: " &
    JsonConvert.SerializeObject(materialSheets.Rows(0)))

            Debug.WriteLine("Explicit settings: " &
    JsonConvert.SerializeObject(materialSheets.Rows(0),
                                Formatting.Indented))

            Dim savedJson As String = File.ReadAllText(materialInventoryPath)

            Debug.WriteLine("Serialized JSON starts with: " &
                json.Substring(0, Math.Min(json.Length, 100)))

            Debug.WriteLine("File JSON starts with: " &
                savedJson.Substring(0, Math.Min(savedJson.Length, 100)))

            Debug.WriteLine($"Table type: {materialSheets.GetType().FullName}")
            Debug.WriteLine($"Rows type: {materialSheets.Rows.GetType().FullName}")
            Debug.WriteLine($"Row count: {materialSheets.Rows.Count}")

            For Each row In materialSheets.Rows
                Debug.WriteLine(If(row Is Nothing, "<Nothing>", row.GetType().FullName))
                Debug.WriteLine($"Description:{row.Description }")

            Next

            File.WriteAllText(materialInventoryPath, json)
        Catch ex As Exception
            MessageBox.Show("Error saving MaterialInventory.json:" & vbCrLf & ex.Message, "Save Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    Public Sub SaveMaterialTypesJson(dataFolder As String, materialTypes As JsonTable(Of MaterialType))
        Try
            ' Ensure folder exists
            If Not Directory.Exists(dataFolder) Then Directory.CreateDirectory(dataFolder)

            Dim materialTypesPath = Path.Combine(dataFolder, "MaterialTypes.json")

            ' Create backup if file exists
            If File.Exists(materialTypesPath) Then
                Dim backupPath = Path.Combine(dataFolder, "MaterialTypes_backup_" & DateTime.Now.ToString("yyyyMMdd_HHmmss") & ".json")
                File.Copy(materialTypesPath, backupPath)
            End If

            ' Serialize and save
            Dim json = JsonConvert.SerializeObject(materialTypes, Formatting.Indented)
            File.WriteAllText(materialTypesPath, json)
        Catch ex As Exception
            MessageBox.Show("Error saving MaterialTypes.json:" & vbCrLf & ex.Message, "Save Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    Public Sub SaveAllMaterialData(dataFolder As String,
                               materialTypes As JsonTable(Of MaterialType),
                               materialSheets As JsonTable(Of MaterialSheet))

        SaveMaterialTypesJson(dataFolder, materialTypes)
        SaveMaterialInventoryJson(dataFolder, materialSheets)

    End Sub

    ''' <summary>
    ''' Removes a parameter (column) from MaterialInventory.json.
    ''' Deletes column definition and removes the value from every row.
    ''' </summary>
    Public Function RemoveParameter(materialInventory As JsonTable(Of MaterialInventoryItem),
                                paramName As String) As Boolean
        Try
            '--- 1. Remove column definition ---
            Dim col = materialInventory.Columns.FirstOrDefault(
            Function(c) c.Field.Equals(paramName, StringComparison.OrdinalIgnoreCase))

            If col IsNot Nothing Then
                materialInventory.Columns.Remove(col)
            Else
                Return False  'parameter not found
            End If

            '--- 2. Remove value from all rows ---
            For Each row In materialInventory.Rows
                If row.Values.ContainsKey(paramName) Then
                    row.Values.Remove(paramName)
                End If
            Next

            Return True

        Catch ex As Exception
            MessageBox.Show("Error in RemoveParameter:" & vbCrLf & ex.Message)
            Return False
        End Try
    End Function

    ''' <summary>
    ''' Adds or updates a parameter (column) in MaterialInventory.json.
    ''' Automatically updates column definition and optionally updates all rows.
    ''' </summary>
    Public Function SaveParameter(materialInventory As JsonTable(Of MaterialInventoryItem),
                              paramName As String,
                              dataType As String,
                              Optional defaultValue As Object = Nothing) As Boolean
        Try
            '--- 1. Validate name ---
            If String.IsNullOrWhiteSpace(paramName) Then Return False

            '--- 2. Check if column exists ---
            Dim existingCol = materialInventory.Columns.FirstOrDefault(
            Function(c) c.Field.Equals(paramName, StringComparison.OrdinalIgnoreCase))

            If existingCol IsNot Nothing Then
                'Parameter already exists → update datatype only
                existingCol.DataType = dataType
            Else
                '--- 3. Add new column definition ---
                Dim newCol As New JsonColumn With {
                .Field = paramName,
                .Header = paramName,
                .DataType = dataType,
                .Visible = True
            }

                materialInventory.Columns.Add(newCol)

                '--- 4. Add default value to all existing rows ---
                For Each row In materialInventory.Rows
                    row.Values(paramName) = defaultValue
                Next
            End If

            Return True

        Catch ex As Exception
            MessageBox.Show("Error in SaveParameter:" & vbCrLf & ex.Message)
            Return False
        End Try
    End Function
    Public Sub ExportMaterialJsonWithColumns(accessDbPath As String, outputFolder As String)
        ' Ensure output folder exists
        If Not Directory.Exists(outputFolder) Then Directory.CreateDirectory(outputFolder)

        Dim connStr As String = $"Provider=Microsoft.ACE.OLEDB.12.0;Data Source={accessDbPath};Persist Security Info=False;"

        ' ----------------------------
        ' 1️⃣ Material Types JSON
        ' ----------------------------
        Dim materialTypes As New JsonTable(Of MaterialType) With {
        .TableName = "Material Types",
        .Columns = New List(Of JsonColumn) From {
            New JsonColumn With {.Field = "ID", .Header = "ID", .Visible = True},
            New JsonColumn With {.Field = "Description", .Header = "Description", .Visible = True}
        },
        .Rows = New List(Of MaterialType)()
    }

        Using conn As New OleDbConnection(connStr)
            conn.Open()
            Using cmd As New OleDbCommand("SELECT * FROM [Material Types]", conn)
                Using reader = cmd.ExecuteReader()
                    While reader.Read()
                        Dim mt As New MaterialType With {
                        .ID = If(IsDBNull(reader("ID")), 0, Convert.ToInt32(reader("ID"))),
                        .Description = If(IsDBNull(reader("Description")), "", reader("Description").ToString()),
                        .Display = If(IsDBNull(reader("Description")), "", reader("Description").ToString())
                    }
                        materialTypes.Rows.Add(mt)
                    End While
                End Using
            End Using
        End Using

        ' Save JSON
        Dim materialTypesPath = Path.Combine(outputFolder, "MaterialTypes.json")
        File.WriteAllText(materialTypesPath, JsonConvert.SerializeObject(materialTypes, Formatting.Indented))

        ' ----------------------------
        ' 2️⃣ Material Inventory JSON
        ' ----------------------------
        Dim materialInventory As New JsonTable(Of MaterialInventoryItem) With {
        .TableName = "Material Inventory",
        .Columns = New List(Of JsonColumn)(),
        .Rows = New List(Of MaterialInventoryItem)()
    }

        Using conn As New OleDbConnection(connStr)
            conn.Open()
            Using cmd As New OleDbCommand("SELECT * FROM [Material Inventory]", conn)
                Using reader = cmd.ExecuteReader()
                    ' Generate Columns dynamically from field names
                    For i As Integer = 0 To reader.FieldCount - 1
                        materialInventory.Columns.Add(New JsonColumn With {
                        .Field = reader.GetName(i),
                        .Header = reader.GetName(i),
                        .Visible = True
                    })
                    Next

                    ' Read rows
                    While reader.Read()
                        Dim item As New MaterialInventoryItem With {
                        .ID = If(IsDBNull(reader("ID")), 0, Convert.ToInt32(reader("ID"))),
                        .TypeID = If(IsDBNull(reader("Type ID")), 0, Convert.ToInt32(reader("Type ID"))),
                        .Values = New Dictionary(Of String, Object)()
                    }
                        '.MaterialSheetID = If(IsDBNull(reader("MaterialSheetID")), 0, Convert.ToInt32(reader("MaterialSheetID"))),


                        ' Populate Values dictionary with all fields
                        For i As Integer = 0 To reader.FieldCount - 1
                            Dim colName = reader.GetName(i)
                            item.Values(colName) = If(IsDBNull(reader(i)), Nothing, reader(i))
                        Next

                        materialInventory.Rows.Add(item)
                    End While
                End Using
            End Using
        End Using

        ' Save JSON
        Dim materialInventoryPath = Path.Combine(outputFolder, "MaterialInventory.json")
        File.WriteAllText(materialInventoryPath, JsonConvert.SerializeObject(materialInventory, Formatting.Indented))

        MessageBox.Show("Material JSON export with columns complete.", "Export Finished", MessageBoxButtons.OK, MessageBoxIcon.Information)
    End Sub


    Private Sub btnConvertCMDB_Click(sender As Object, e As EventArgs) Handles btnConvertCMDB.Click
        'Dim CMDBPath As String '= "C:\_Gary\AI\WE-CIM_ConfigMan\WE-CIM_ConfigMan_174\bin\Debug\net9.0-windows\Data\cmdb.mdb"
        'Dim outputfolder As String '= "C:\_Gary\AI\WE-CIM_ConfigMan\WE-CIM_ConfigMan_174\bin\Debug\net9.0-windows\Data\"

        'ExportMaterialJson(CMDBPath, outpurfolder)


        frmConvertCMDB.ShowDialog(Me)

    End Sub

    Private Sub btnMaterialAddUDP_Click(sender As Object, e As EventArgs) Handles btnMaterialAddUDP.Click

        ' Ensure a node is selected in the treeview
        Dim selectedNode As TreeNode = TreeViewMachines.SelectedNode
        If selectedNode Is Nothing Then
            MessageBox.Show("Please select a material sheet first.", "Add Parameter", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Only allow adding if it's a Material Sheet node (nodetype 102)
        Dim tagInfo = GetValueFromDelimitedData(selectedNode.Tag, "NodeType", -1)
        If tagInfo <> 102 Then
            MessageBox.Show("Please select a material sheet to add a parameter.", "Add Parameter", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        Dim SheetID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "MachineID", -1)
        ' Find the material sheet object from memory
        Dim sheet As MaterialSheet = materialInventoryTable.Rows.FirstOrDefault(Function(s) s.ID = SheetID)
        If sheet Is Nothing Then
            MessageBox.Show("Material sheet not found.", "Add Parameter", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' Prompt user for parameter details
        Dim paramName As String = InputBox("Enter internal parameter name (no spaces):", "Add Parameter")
        If String.IsNullOrWhiteSpace(paramName) Then Return

        ' Check if parameter already exists
        If sheet.Values.Any(Function(p) p.Name.Equals(paramName, StringComparison.OrdinalIgnoreCase)) Then
            MessageBox.Show("Parameter already exists.", "Add Parameter", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        Dim paramDisplay As String = InputBox("Enter display name for property grid:", "Add Parameter", paramName)
        If String.IsNullOrWhiteSpace(paramDisplay) Then paramDisplay = paramName

        ' Prompt for data type
        Dim dataTypeStr As String = InputBox("Enter DataType (0=String,1=Integer,4=Boolean,7=Decimal):", "Add Parameter", "0")
        Dim dataType As Integer
        If Not Integer.TryParse(dataTypeStr, dataType) Then dataType = 0

        ' Prompt for default value
        Dim defaultValueStr As String = InputBox("Enter default value:", "Add Parameter", "")
        Dim defaultValue As Object = defaultValueStr

        ' Prompt for visibility
        Dim visibleResult = MessageBox.Show("Should the parameter be visible in the property grid?", "Add Parameter", MessageBoxButtons.YesNo, MessageBoxIcon.Question)
        Dim visible As Boolean = (visibleResult = DialogResult.Yes)

        ' Create new MaterialParameter
        Dim newParam As New MaterialParameter With {
        .Name = paramName,
        .Display = paramDisplay,
        .Value = defaultValue,
        .DataType = dataType,
        .DefaultValue = defaultValue,
        .Visible = visible
    }

        ' Add to sheet
        sheet.Values.Add(newParam)

        ' Refresh property grid if showing this sheet
        If propgridMaterial.SelectedObject Is sheet Then
            propgridMaterial.SelectedObject = Nothing
            propgridMaterial.SelectedObject = sheet
        End If

        MessageBox.Show($"Parameter '{paramName}' added successfully.", "Add Parameter", MessageBoxButtons.OK, MessageBoxIcon.Information)


    End Sub
    Private Sub SaveParameter(paramName As String, paramDisplay As String, paramValue As Object,
                          dataType As Integer, Optional paramDefault As Object = Nothing,
                          Optional paramVisible As Boolean = True)

        Dim sheet = TryCast(propgridMaterial.SelectedObject, MaterialSheet)
        If sheet Is Nothing Then Return

        ' Check if parameter exists
        Dim param = sheet.Values.FirstOrDefault(Function(p) p.Name = paramName)
        If param IsNot Nothing Then
            ' Update existing
            param.Value = paramValue
            param.Display = paramDisplay
            param.DataType = dataType
            param.DefaultValue = paramDefault
            param.Visible = paramVisible
        Else
            ' Add new
            sheet.Values.Add(New MaterialParameter With {
            .Name = paramName,
            .Display = paramDisplay,
            .Value = paramValue,
            .DataType = dataType,
            .DefaultValue = paramDefault,
            .Visible = paramVisible
        })
        End If

        propgridMaterial.Refresh()
    End Sub
    Private Sub btnRemoveMaterialUDP_Click(sender As Object, e As EventArgs) Handles btnRemoveMaterialUDP.Click

        ' Ensure a node is selected in the treeview
        Dim selectedNode As TreeNode = TreeViewMachines.SelectedNode
        If selectedNode Is Nothing Then
            MessageBox.Show("Please select a material sheet first.", "Remove Parameter", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Only allow removal if it's a Material Sheet node (nodetype 102)
        Dim tagInfo = GetValueFromDelimitedData(selectedNode.Tag, "NodeType", -1)
        If tagInfo <> 102 Then
            MessageBox.Show("Please select a material sheet to remove a parameter.", "Remove Parameter", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' Find the material sheet object from memory
        Dim SheetID As Integer = GetValueFromDelimitedData(selectedNode.Tag, "MachineID", -1)
        Dim sheet As MaterialSheet = materialInventoryTable.Rows.FirstOrDefault(Function(s) s.ID = SheetID)
        If sheet Is Nothing Then
            MessageBox.Show("Material sheet not found.", "Remove Parameter", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' Prompt user for the parameter to remove
        Dim paramName As String = InputBox("Enter the name of the parameter to remove:", "Remove Parameter")
        If String.IsNullOrWhiteSpace(paramName) Then Return

        ' Remove the parameter from the sheet
        Dim param = sheet.Values.FirstOrDefault(Function(p) p.Name = paramName)
        If param IsNot Nothing Then
            sheet.Values.Remove(param)
            MessageBox.Show($"Parameter '{paramName}' removed successfully.", "Remove Parameter", MessageBoxButtons.OK, MessageBoxIcon.Information)

            ' Optionally refresh the property grid if you are showing sheet properties
            If propgridMaterial.SelectedObject Is sheet Then
                propgridMaterial.SelectedObject = Nothing
                propgridMaterial.SelectedObject = sheet
            End If
        Else
            MessageBox.Show($"Parameter '{paramName}' not found.", "Remove Parameter", MessageBoxButtons.OK, MessageBoxIcon.Warning)
        End If

    End Sub

    ' =========================================
    ' Helper: Generate a new unique ID
    ' =========================================
    Private Function GenerateNewMaterialSheetID() As Integer
        If materialInventoryTable.Rows Is Nothing OrElse materialInventoryTable.Rows.Count = 0 Then
            Return 1
        Else
            Return materialInventoryTable.Rows.Max(Function(s) s.ID) + 1
        End If
    End Function

    ' =========================================
    ' Helper: default values for DataType
    ' =========================================
    Private Function GetDefaultForDataType(dataType As Integer) As Object
        Select Case dataType
            Case 0 : Return ""
            Case 1 : Return ""
            Case 2 : Return False
            Case 3 : Return 0
            Case 4 : Return 0
            Case 5 : Return ""
            Case 6 : Return ""
            Case 7 : Return 0.0
            Case 8 : Return ""
            Case 9 : Return Color.White
            Case Else : Return Nothing
        End Select
    End Function

    Private Sub SaveMaterialParameter(sheet As MaterialSheet, paramName As String, paramValue As Object,
                                  Optional dataType As Integer = 0, Optional display As String = "",
                                  Optional defaultValue As Object = Nothing, Optional visible As Boolean = True)

        Dim param = sheet.Values.FirstOrDefault(Function(p) p.Name = paramName)
        If param IsNot Nothing Then
            ' Update existing parameter
            param.Value = paramValue
            param.DataType = dataType
            param.Display = If(display <> "", display, param.Display)
            param.DefaultValue = If(defaultValue IsNot Nothing, defaultValue, param.DefaultValue)
            param.Visible = visible
        Else
            ' Add new parameter
            sheet.Values.Add(New MaterialParameter With {
            .Name = paramName,
            .Display = If(display <> "", display, paramName),
            .Value = paramValue,
            .DataType = dataType,
            .DefaultValue = defaultValue,
            .Visible = visible
        })
        End If
    End Sub

    Private Sub RemoveMaterialParameter(sheet As MaterialSheet, paramName As String)
        Dim param = sheet.Values.FirstOrDefault(Function(p) p.Name = paramName)
        If param IsNot Nothing Then
            sheet.Values.Remove(param)
        End If
    End Sub

    Private Function IsMaterialTypeNode(node As TreeNode) As Boolean
        Return node.Tag IsNot Nothing AndAlso node.Tag.ToString().EndsWith("|101")
    End Function

    ''' <summary>
    ''' Checks if a name already exists in a collection of Material Types or Material Sheets.
    ''' </summary>
    ''' <param name="name">The name to check (Type Display or Sheet Description)</param>
    ''' <param name="typeID">Optional: For sheets, limit search to this TypeID</param>
    ''' <returns>True if duplicate exists, False otherwise</returns>
    Private Function IsDuplicateMaterialName(name As String, Optional typeID As Integer = -1) As Boolean
        ' Check Material Types
        If typeID = -1 Then
            Return materialTypesTable.Rows.Any(Function(t) String.Equals(t.Display, name, StringComparison.OrdinalIgnoreCase))
        Else
            ' Check Material Sheets for a given TypeID
            Return materialInventoryTable.Rows.Any(Function(s) s.TypeID = typeID AndAlso
            s.Values.Any(Function(p) p.Name = "Description" AndAlso
                String.Equals(p.Value.ToString(), name, StringComparison.OrdinalIgnoreCase)))
        End If
    End Function

    Private Sub ConfigurationManagerForm_FormClosing(sender As Object, e As FormClosingEventArgs) Handles Me.FormClosing
        Me.Dispose()
    End Sub

    Private Sub SaveMachineAttributes()
        Try
            Dim spath = Path.Combine(dataFolder, "MachineAttributes.json")
            Dim json = JsonConvert.SerializeObject(AppData.MachineAttributesTable, Formatting.Indented)
            File.WriteAllText(spath, json)
        Catch ex As Exception
            MessageBox.Show("Error saving MachineAttributes.json:" & vbCrLf & ex.Message)
        End Try
    End Sub


    ' Helper to provide default values based on DynamicDataType
    Private Function GetDefaultValueForDataType(dt As DynamicDataType) As Object
        Select Case dt
            Case DynamicDataType.StringType
                Return ""
            Case DynamicDataType.IntegerType
                Return 0
            Case DynamicDataType.DoubleType
                Return 0.0
            Case DynamicDataType.BooleanType
                Return False
            Case DynamicDataType.Dropdown
                Return Nothing
            Case DynamicDataType.ColorType
                Return Color.White
            Case DynamicDataType.File, DynamicDataType.Folder
                Return ""
            Case Else
                Return Nothing
        End Select
    End Function


    ' --- Recursive helper to find machine type node ---
    Private Function FindMachineTypeNode(machineTypeID As Integer) As TreeNode
        For Each node As TreeNode In TreeViewMachines.Nodes
            Dim nodeType As Integer = GetValueFromDelimitedData(CStr(node.Tag), "NodeType", -1)
            Dim typeID As Integer = GetValueFromDelimitedData(CStr(node.Tag), "MachineTypeID", -1)
            If nodeType = 1 AndAlso typeID = machineTypeID Then ' NodeType 1 = Punch Node (example)
                Return node
            End If

            ' Search children recursively
            Dim childResult As TreeNode = FindMachineTypeNodeRecursive(node.Nodes, machineTypeID)
            If childResult IsNot Nothing Then Return childResult
        Next
        Return Nothing
    End Function

    Private Function FindMachineTypeNodeRecursive(nodes As TreeNodeCollection, machineTypeID As Integer) As TreeNode
        For Each node As TreeNode In nodes
            Dim nodeType As Integer = GetValueFromDelimitedData(CStr(node.Tag), "NodeType", -1)
            Dim typeID As Integer = GetValueFromDelimitedData(CStr(node.Tag), "MachineTypeID", -1)
            If nodeType = 1 AndAlso typeID = machineTypeID Then
                Return node
            End If
            Dim childResult As TreeNode = FindMachineTypeNodeRecursive(node.Nodes, machineTypeID)
            If childResult IsNot Nothing Then Return childResult
        Next
        Return Nothing
    End Function

    ''' <summary>
    ''' Recursively searches the TreeView nodes for a machine node with the specified MachineID.
    ''' </summary>
    ''' <param name="nodes">The collection of TreeNodes to search.</param>
    ''' <param name="machineID">The MachineID to find.</param>
    ''' <returns>The TreeNode corresponding to the machine, or Nothing if not found.</returns>
    Private Function FindMachineNode(nodes As TreeNodeCollection, machineID As Integer) As TreeNode
        For Each node As TreeNode In nodes

            Dim nodeMachineID As Integer = GetValueFromDelimitedData(CStr(node.Tag), "Machine_ID", 0)

            ' Only match REAL machine nodes (ID > 0)
            If nodeMachineID > 0 AndAlso nodeMachineID = machineID Then
                Return node
            End If

            Dim childResult As TreeNode = FindMachineNode(node.Nodes, machineID)
            If childResult IsNot Nothing Then
                Return childResult
            End If
        Next

        Return Nothing
    End Function
    Public Function FindNodeByText(nodes As TreeNodeCollection, machineID As Integer) As TreeNode
        ' Dim comparison As StringComparison = If(caseSensitive, StringComparison.Ordinal, StringComparison.OrdinalIgnoreCase)

        For Each node As TreeNode In nodes
            Dim found As TreeNode = FindNodeRecursive(node, machineID)
            If found IsNot Nothing Then
                Return found
            End If
        Next
        Return Nothing
    End Function

    Private Function FindNodeRecursive(parentNode As TreeNode, machineID As Integer) As TreeNode
        Dim tagMachineID As Integer
        tagMachineID = GetValueFromDelimitedData(CInt(parentNode.Tag), "Machine_ID", -1)

        If tagMachineID = machineID Then
            Return parentNode
        End If

        For Each child As TreeNode In parentNode.Nodes
            tagMachineID = GetValueFromDelimitedData(CInt(child.Tag), "Machine_ID", -1)
            'Dim found As TreeNode = FindNodeRecursive(child, Text, Comparison)
            If tagMachineID = machineID Then
                Return child
            End If
        Next

        Return Nothing
    End Function
    Private Sub SaveToolSetupsJson()
        Try
            Dim filePath As String = Path.Combine(dataFolder, "ToolSetups.json")
            JsonSaver.SaveObjectToJsonFile(ToolSetups, filePath)

        Catch ex As Exception
            MessageBox.Show($"Error saving tool setups: {ex.Message}", "Save Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    Private Sub propgridMaterial_TextChanged(sender As Object, e As EventArgs)

    End Sub

    Private Sub btnCancelToolProps_Click(sender As Object, e As EventArgs) Handles btnCancelToolProps.Click
        tabControlMain.SelectedTab = tabToolSetup
    End Sub

    Private Function GetNextToolCribID() As Integer
        If AppData.ToolCrib.Any() Then
            Return AppData.ToolCrib.Max(Function(t) t.ID) + 1
        Else
            Return 1
        End If
    End Function
    ' Convert ToolTypeAttributes into dictionary for property grid
    Private Function GetDefaultPropertiesForToolType(toolTypeID As Integer) _
    As Dictionary(Of String, Object)

        Dim dict As New Dictionary(Of String, Object)

        ' Find all attribute mappings for this tool type
        Dim mappings = AppData.ToolTypeAttributes _
        .Where(Function(t) t.ToolTypeID = toolTypeID)

        For Each map In mappings

            ' Lookup the attribute definition
            Dim attrType = AppData.ToolAttributeTypes _
            .FirstOrDefault(Function(a) a.ID = map.AttributeTypeID)

            ' HARD GUARD — prevents your crash
            If attrType Is Nothing Then Continue For
            If String.IsNullOrWhiteSpace(attrType.Field) Then Continue For

            ' Initialize default based on datatype
            Dim defaultValue As Object = GetDefaultValueForDataType(attrType.DataType)

            dict(attrType.Field) = defaultValue
        Next

        Return dict
    End Function



    Private Function LoadToolAttributeTypes() As List(Of ToolAttributeType)
        Dim filePath = Path.Combine(dataFolder, "ToolAttributeTypes.json")
        If Not File.Exists(filePath) Then Return New List(Of ToolAttributeType)

        Dim json = JObject.Parse(File.ReadAllText(filePath))
        Return json("Rows").ToObject(Of List(Of ToolAttributeType))()
    End Function




    Private Function GetStationColumnNames(stations As List(Of Station)) As List(Of String)
        Dim cols As New HashSet(Of String)
        For Each s In stations
            cols.Add("ID")
            cols.Add("MachineID")
            cols.Add("Description")
            cols.Add("Position")
            For Each k In s.Properties.Keys
                cols.Add(k)
            Next
        Next
        Return cols.ToList()
    End Function

    Private Function GetSelectedMachineID() As Integer
        Dim node = TreeViewMachines.SelectedNode
        If node Is Nothing Then Return 0

        Dim MachineID = GetValueFromDelimitedData(node.Tag, "MachineID", 0)
        Return MachineID
    End Function

    ''' <summary>
    ''' Returns a default value based on the DataType integer.
    ''' Adjust as needed to match your DataType definitions.
    ''' </summary>
    Private Function GetDefaultValueForDataType(dataType As Integer) As Object
        Select Case dataType
            Case 1  ' Boolean
                Return False
            Case 4  ' Integer
                Return 0
            Case 7  ' Double / Decimal
                Return 0.0
            Case 10 ' String
                Return ""
            Case Else
                Return Nothing
        End Select
    End Function

    Private Sub btnEidtTool_Click(sender As Object, e As EventArgs) Handles btnEidtTool.Click

        ' Ensure a tool is selected
        Dim selectedTool = TryCast(lstTools.SelectedItem, Tool)
        If selectedTool Is Nothing Then
            MessageBox.Show("Please select a tool to edit.")
            Return
        End If

        ' Store for SaveToolProps
        currentEditingTool = selectedTool

        ' Update labels
        lblCurrentMachine.Text = currentMachine
        lblCurrentToolSetup.Text = currentToolSetup
        lblCurrentDescription.Text = selectedTool.Description

        ' Bind properties to PropertyGrid
        propgrdToolProps.SelectedObject = New DynamicToolAttributes(selectedTool.Attributes)

        ' Switch to properties tab
        tabControlMain.SelectedTab = tabToolProperties


    End Sub

    Private Sub propgrdLayer_Click(sender As Object, e As EventArgs) Handles propgrdLayer.Click

    End Sub

    Private Sub btnSaveLayerSetp_Click(sender As Object, e As EventArgs) Handles btnSaveLayerSetp.Click

        Dim editedLayerSetup = TryCast(propgrdLayerSetup.SelectedObject, LayerSetup)
        If editedLayerSetup Is Nothing Then
            MessageBox.Show("No Layer Setup selected.")
            Return
        End If

        ' Find existing LayerSetup
        Dim existing = AppData.LayerSetups _
        .FirstOrDefault(Function(ls) ls.ID = editedLayerSetup.ID)

        If existing Is Nothing Then
            MessageBox.Show("Layer Setup not found.")
            Return
        End If

        ' Copy editable fields
        existing.Description = editedLayerSetup.Description
        existing.ZLevelMode = editedLayerSetup.ZLevelMode
        existing.GapTolerance = editedLayerSetup.GapTolerance
        existing.CleanTolerance = editedLayerSetup.CleanTolerance
        existing.FilterTolerance = editedLayerSetup.FilterTolerance
        existing.SharpAngle = editedLayerSetup.SharpAngle
        existing.ProcessText = editedLayerSetup.ProcessText
        existing.Restrict_Offset = editedLayerSetup.Restrict_Offset

        SaveLayerSetups()

        MessageBox.Show("Layer Setup saved successfully.")


    End Sub

    Private Sub SaveLayerSetups()
        Dim filePath = Path.Combine(dataFolder, "LayerSetups.json")

        Dim wrapper As New JObject From {
        {"TableName", "Layer Setups"},
        {"Rows", JArray.FromObject(AppData.LayerSetups)}
    }

        File.WriteAllText(filePath, wrapper.ToString(Formatting.Indented))
    End Sub

    Private Sub SaveDirectives()
        Dim filePath = Path.Combine(dataFolder, "Directives.json")

        Dim wrapper As New JObject From {
        {"TableName", "Directives"},
        {"Rows", JArray.FromObject(AppData.Directives)}
    }

        File.WriteAllText(filePath, wrapper.ToString(Formatting.Indented))
    End Sub


    Private Sub btnLayerMapSave_Click(sender As Object, e As EventArgs) Handles btnLayerMapSave.Click

        Dim editedDirective = TryCast(propgrdLayer.SelectedObject, Directive)
        If editedDirective Is Nothing Then
            MessageBox.Show("No Layer selected.")
            Return
        End If

        Dim existing = AppData.Directives _
        .FirstOrDefault(Function(d) d.ID = editedDirective.ID)

        If existing Is Nothing Then
            MessageBox.Show("Layer not found.")
            Return
        End If

        ' Copy editable fields
        existing.CADLayer = editedDirective.CADLayer
        existing.CAMLayer = editedDirective.CAMLayer
        existing.Color = editedDirective.Color
        existing.Station = editedDirective.Station
        existing.ToolType = editedDirective.ToolType
        existing.ZLevel = editedDirective.ZLevel
        existing.CutSide = editedDirective.CutSide
        existing.Distance = editedDirective.Distance
        existing.CutDirection = editedDirective.CutDirection

        SaveDirectives()

        MessageBox.Show("Layer saved successfully.")


    End Sub

    ''' <summary>
    ''' Traverses up the tree to find the Tool Setup ID associated with this node.
    ''' </summary>
    Public Function GetToolSetupIDFromNode(node As TreeNode) As Integer
        Dim current As TreeNode = node
        While current IsNot Nothing
            If TypeOf current.Tag Is String Then
                Dim tagStr As String = CStr(current.Tag)
                Dim nodeType = CInt(GetValueFromDelimitedData(tagStr, "NodeType", "0"))

                ' Case 13 is Actual Tool Setup node
                If nodeType = 13 Then
                    Return GetValueFromDelimitedData(tagStr, "ToolSetup_ID", -1)
                End If
            End If
            current = current.Parent
        End While

        Return -1
    End Function
    ' --- Add new Layer Setup ---
    Private Sub AddLayerSetupTSM_Click(sender As Object, e As EventArgs) Handles AddLayerSetupTSM.Click
        Try
            Dim selectedToolSetupNode = TreeViewMachines.SelectedNode.Parent
            If selectedToolSetupNode Is Nothing Then Exit Sub

            ' Get MachineID from the parent Tool Setup node
            Dim machineID = GetValueFromDelimitedData(selectedToolSetupNode.Tag, "Machine_ID", 0)

            ' Create new LayerSetup
            Dim newLS As New LayerSetup With {
            .ID = GetNextLayerSetupID(),
            .Description = "New Layer Setup",
            .MachineID = machineID,
            .ZLevelMode = 0,
            .GapTolerance = 0,
            .CleanTolerance = 0,
            .FilterTolerance = 0,
            .SharpAngle = 0,
            .ProcessText = False,
            .Restrict_Offset = False,
            .Layers = New List(Of Directive)()
        }

            ' Add to AppData
            AppData.LayerSetups.Add(newLS)
            Dim MachineTyeID As Integer = GetValueFromDelimitedData(TreeViewMachines.SelectedNode.Parent.Parent.Parent.Tag, "MachineType_ID", 0)
            Dim ToolSetup_ID As Integer = GetValueFromDelimitedData(TreeViewMachines.SelectedNode.Parent.Tag, "MachineType_ID", 0)
            ' Create tree node
            Dim lsNode As New TreeNode(newLS.Description) With {
                .Tag = EncodeConfigTreeTag(MachineTyeID, machineID, ToolSetup_ID, newLS.ID, 0, 15)
        }

            selectedToolSetupNode.Nodes.Add(lsNode)
            selectedToolSetupNode.Expand()

            ' Auto-select the new node
            TreeViewMachines.SelectedNode = lsNode
            TreeViewMachines.Focus()

            ' Populate property grid
            propgrdLayerSetup.SelectedObject = newLS

        Catch ex As Exception
            MessageBox.Show($"Error adding Layer Setup: {ex.Message}")
        End Try
    End Sub

    Private Sub AddLayerTSM_Click(sender As Object, e As EventArgs) Handles AddLayerTSM.Click

        Dim selectedLSNode = TreeViewMachines.SelectedNode
        Dim layerSetupID = GetValueFromDelimitedData(selectedLSNode.Tag, "LayerSetup_ID", -1)
        toolsetupid = GetValueFromDelimitedData(TreeViewMachines.SelectedNode.Parent.Parent.Parent.Tag, "ToolSetup_ID", -1)
        Dim machinetypeid = GetValueFromDelimitedData(TreeViewMachines.SelectedNode.Parent.Parent.Parent.Parent.Parent.Parent.Tag, "MachineType_ID", -1)
        Dim machineid = GetValueFromDelimitedData(TreeViewMachines.SelectedNode.Parent.Parent.Parent.Parent.Parent.Tag, "Machine_ID", -1)

        If layerSetupID = -1 Then Return

        ' Create new Directive/Layer
        Dim newLayer As New Directive With {
        .ID = GetNextDirectiveID(),
        .LayerSetupID = layerSetupID,
        .CADLayer = "New Layer",
        .CAMLayer = "New Layer"
    }
        AppData.Directives.Add(newLayer)
        SaveDirectives()

        ' Add node
        Dim layerNode As New TreeNode(newLayer.CADLayer) With {
        .Tag = EncodeConfigTreeTag(machinetypeid, machineid, toolsetupid, layerSetupID, newLayer.ID, 16)
    }
        selectedLSNode.Nodes.Add(layerNode)
        selectedLSNode.Expand()

    End Sub

    Private Sub DeleteLayerSetpTSM_Click(sender As Object, e As EventArgs) Handles DeleteLayerSetpTSM.Click

        Dim lsNode = TreeViewMachines.SelectedNode
        Dim layerSetupID = GetValueFromDelimitedData(lsNode.Tag, "LayerSetup_ID", -1)
        If layerSetupID = -1 Then Return

        ' Remove associated directives
        AppData.Directives.RemoveAll(Function(d) d.LayerSetupID = layerSetupID)
        SaveDirectives()

        ' Remove LayerSetup
        AppData.LayerSetups.RemoveAll(Function(ls) ls.ID = layerSetupID)
        SaveLayerSetups()

        lsNode.Remove()

    End Sub

    Private Sub DeleteLayerTSM_Click(sender As Object, e As EventArgs) Handles DeleteLayerTSM.Click
        Dim layerNode = TreeViewMachines.SelectedNode
        Dim directiveID = GetValueFromDelimitedData(layerNode.Tag, "Directive_ID", -1)
        If directiveID = -1 Then Return

        AppData.Directives.RemoveAll(Function(d) d.ID = directiveID)
        SaveDirectives()

        layerNode.Remove()

    End Sub

    ' Returns the next available LayerSetup ID
    Private Function GetNextLayerSetupID() As Integer
        If AppData.LayerSetups Is Nothing OrElse AppData.LayerSetups.Count = 0 Then
            Return 1
        Else
            Return AppData.LayerSetups.Max(Function(ls) ls.ID) + 1
        End If
    End Function

    ' Returns the next available Directive ID
    Private Function GetNextDirectiveID() As Integer
        If AppData.Directives Is Nothing OrElse AppData.Directives.Count = 0 Then
            Return 1
        Else
            Return AppData.Directives.Max(Function(d) d.ID) + 1
        End If
    End Function

    Private Sub btnMaterialCancel_Click(sender As Object, e As EventArgs) Handles btnMaterialCancel.Click
        tabControlMain.SelectedTab = tabDefinition
        TreeViewMachines.SelectedNode = Nothing


    End Sub
    Private Sub dgvToolSetup_CellValueChanged(sender As Object, e As DataGridViewCellEventArgs) Handles dgvToolSetup.CellValueChanged

        If e.RowIndex < 0 OrElse e.ColumnIndex < 0 Then Return

        Dim grid = dgvToolSetup
        Dim columnName As String = grid.Columns(e.ColumnIndex).Name

        ' Ignore standard and internal columns.
        Select Case columnName
            Case "NCCodeNumber", "ToolDescription", "IndexAngle",
             "FixedStation", "PrimaryCode", "SecondaryCode",
             "StationID", "ToolSetupMemberID"
                Return
        End Select

        ' Confirm this column is a defined Tool Setup UDP.
        If AppData.ToolSetupUDPDefinitions Is Nothing Then Return

        Dim udp = AppData.ToolSetupUDPDefinitions.FirstOrDefault(
        Function(p) String.Equals(
            p.Name, columnName,
            StringComparison.OrdinalIgnoreCase))

        If udp Is Nothing Then Return

        If AppData.ToolSetupUDPValues Is Nothing OrElse
       AppData.ToolSetupUDPValuesTable Is Nothing Then
            MessageBox.Show("Tool Setup UDP values have not been loaded.")
            Return
        End If

        Dim gridRow As DataGridViewRow = grid.Rows(e.RowIndex)

        If gridRow.IsNewRow Then Return

        Dim memberID As Integer
        If Not Integer.TryParse(
        Convert.ToString(gridRow.Cells("ToolSetupMemberID").Value),
        memberID) Then
            MessageBox.Show("Unable to identify the Tool Setup member.")
            Return
        End If

        Dim cellValue As Object = gridRow.Cells(e.ColumnIndex).Value
        Dim valueText As String =
        If(cellValue Is Nothing OrElse cellValue Is DBNull.Value,
           "",
           Convert.ToString(
               cellValue,
               Globalization.CultureInfo.InvariantCulture))

        ' Find the existing value for this member and UDP.
        Dim udpValue = AppData.ToolSetupUDPValues.FirstOrDefault(
        Function(v) v.ToolSetupMemberID = memberID AndAlso
            v.ToolSetupID = currenttoolSetupID AndAlso
            String.Equals(
                v.ParameterName, udp.Name,
                StringComparison.OrdinalIgnoreCase))


        Dim isNew As Boolean = (udpValue Is Nothing)
        Dim previousValue As String = Nothing

        If isNew Then
            Dim nextID As Integer = 1

            If AppData.ToolSetupUDPValues.Count > 0 Then
                nextID = AppData.ToolSetupUDPValues.Max(Function(v) v.ID) + 1
            End If

            udpValue = New ToolSetupUDPValue With {
        .ID = nextID,
        .ToolSetupID = currenttoolSetupID,
        .ToolSetupMemberID = memberID,
        .ParameterName = udp.Name,
        .Value = valueText
    }

            AppData.ToolSetupUDPValues.Add(udpValue)
            AppData.ToolSetupUDPValuesTable.Rows.Add(udpValue)
        Else
            previousValue = udpValue.Value
            udpValue.Value = valueText
        End If

        Try
            AppData.SaveJsonTableWrapped(
        AppData.ToolSetupUDPValuesTable,
        "ToolSetupUDPValues.json")

        Catch ex As Exception

            If isNew Then
                AppData.ToolSetupUDPValues.Remove(udpValue)
                AppData.ToolSetupUDPValuesTable.Rows.Remove(udpValue)
            Else
                udpValue.Value = previousValue
            End If

            MessageBox.Show(
        "Failed to save Tool Setup UDP value: " & ex.Message)

        End Try


    End Sub

    Private Sub lstTools_SelectedIndexChanged(sender As Object, e As EventArgs) Handles lstTools.SelectedIndexChanged

    End Sub

    Private Sub lstTools_RegionChanged(sender As Object, e As EventArgs) Handles lstTools.RegionChanged

    End Sub

    Private Sub lstTools_DrawItem(
    sender As Object,
    e As DrawItemEventArgs
) Handles lstTools.DrawItem

        If e.Index < 0 OrElse e.Index >= lstTools.Items.Count Then Return

        ' Draw the normal background and selection.
        e.DrawBackground()

        Dim tool As Tool = TryCast(lstTools.Items(e.Index), Tool)
        If tool Is Nothing Then Return

        Dim textColor As Color = e.ForeColor
        Dim colorValue As Object = Nothing

        If tool.Attributes IsNot Nothing Then

            ' Prefer the canonical key; retain compatibility with legacy data.
            If Not tool.Attributes.TryGetValue("Color", colorValue) Then
                tool.Attributes.TryGetValue("COLOR", colorValue)
            End If

        End If

        If colorValue IsNot Nothing Then
            Try
                Dim oleColor As Integer = Convert.ToInt32(colorValue)
                textColor = ColorTranslator.FromOle(oleColor)
            Catch ex As Exception
                ' Retain the default foreground if the value is invalid.
            End Try
        End If

        Using brush As New SolidBrush(textColor)
            e.Graphics.DrawString(
            tool.Description,
            e.Font,
            brush,
            e.Bounds
        )
        End Using

        e.DrawFocusRectangle()

    End Sub

    Private Sub TreeViewMachines_CausesValidationChanged(sender As Object, e As EventArgs) Handles TreeViewMachines.CausesValidationChanged

    End Sub
End Class
