Imports FabV25_WIN8.WE_ENG_V25_0.Core.Models
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.ComponentModel
Imports System.IO
Imports System.Runtime.Remoting.Contexts

Public Class ConfigurationManagerForm
    Inherits Form

    ' --------------------------
    ' Stub Classes
    ' --------------------------
    Public Class MaterialType
        Public Property ID As Integer
        Public Property Display As String
    End Class

    Public Class MaterialSheet
        Public Property TypeID As Integer
        Public Property Thickness As String
        Public Property Width As Double
        Public Property Length As Double
        Public Property Notes As String
    End Class

    ' --------------------------
    ' Stub Collections
    ' --------------------------
    ' Using Lists to simulate DataTables for now
    Public materialTypesTable As New List(Of MaterialType)()
    Public materialInventoryTable As New List(Of MaterialSheet)()

    ' --------------------------
    ' Example placeholder data
    ' --------------------------
    Sub InitMaterialStubs()
        ' Material Types
        materialTypesTable.Add(New MaterialType With {.ID = 1, .Display = "CRS"})
        materialTypesTable.Add(New MaterialType With {.ID = 2, .Display = "SST"})
        materialTypesTable.Add(New MaterialType With {.ID = 3, .Display = "Aluminum"})

        ' Material Inventory
        materialInventoryTable.Add(New MaterialSheet With {.TypeID = 1, .Thickness = "11 GA", .Width = 48, .Length = 96})
        materialInventoryTable.Add(New MaterialSheet With {.TypeID = 2, .Thickness = "20 GA", .Width = 36, .Length = 96})
    End Sub



    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.

    End Sub

    ' --- Form Load ---
    Private Sub ConfigurationManagerForm_Load(sender As Object, e As EventArgs) Handles Me.Load
        ' TestStationsJson.TestStations()
        AddHandler dgvToolSetup.CurrentCellDirtyStateChanged, Sub(s, args)
                                                                  If dgvToolSetup.IsCurrentCellDirty Then
                                                                      dgvToolSetup.CommitEdit(DataGridViewDataErrorContexts.Commit)
                                                                  End If
                                                              End Sub

        LoadJsonData()
        PopulateTreeView()
    End Sub

    'Public Sub PopulateTreeView()

    '    Dim nodetype As Integer

    '    'root("Machine Types") nodetype = 1
    '    '  └─ MachineType (Punch, Punch/Plasma,Laser,Punch/Laser, etc.) nodetype = 2
    '    '      └─ Machine name (Machine Description) nodetype = 3
    '    '          ├─ Stations (Just shows a table when selected) nodetype = 4
    '    '          └─ Tool Setups (Tool Setup Main Node nodetype = 5
    '    '              └─ ToolSetup Name (Tool Setup Description) nodetype = 6 



    '    TreeViewMachines.Nodes.Clear()
    '    ' Root node
    '    Dim rootnode As TreeNode = TreeViewMachines.Nodes.Add("Machine Types")
    '    rootnode.Tag = EncodeTag(0, -1, -1, 0)

    '    ' Loop through machine types
    '    For Each mt As MachineType In machineTypesTable.Rows
    '        Dim typeNode As TreeNode = rootnode.Nodes.Add(mt.Display)
    '        Select Case UCase(mt.Display)
    '            Case "PUNCH"
    '                nodetype = 1
    '            Case "Punch/PLSAMA"
    '                nodetype = 2
    '            Case "Laser"
    '                nodetype = 3
    '            Case "Punch/Laser"
    '                nodetype = 4
    '            Case "BURNER"
    '                nodetype = 5
    '            Case "WATERJET"
    '                nodetype = 6
    '            Case "ROUTER"
    '                nodetype = 7
    '            Case "POINT-TO-POINT"
    '                nodetype = 8
    '            Case "MILL"
    '                nodetype = 9

    '            Case Else

    '        End Select
    '        typeNode.Tag = EncodeTag(mt.ID, -1, -1, nodetype)

    '        ' Add machines under this type
    '        Dim machinesOfType = machinesTable.Rows.Where(Function(m) m.TypeID = mt.ID)
    '        For Each machine In machinesOfType
    '            Dim machineNode As TreeNode = typeNode.Nodes.Add(machine.Description)
    '            machineNode.Tag = EncodeTag(mt.ID, machine.ID, -1, 10) ' Store MachineID for reliable lookup

    '            ' Add child nodes "Stations" and "Tool Setups"
    '            Dim stationsNode As TreeNode = machineNode.Nodes.Add("Stations")
    '            stationsNode.Tag = EncodeTag(mt.ID, machine.ID, -1, 11)

    '            Dim toolSetupsNode As TreeNode = machineNode.Nodes.Add("Tool Setups")
    '            toolSetupsNode.Tag = EncodeTag(mt.ID, machine.ID, -1, 12)

    '            ' Add Tool Setup nodes under "Tool Setups"
    '            If toolSetupsTable IsNot Nothing AndAlso toolSetupsTable.Rows.Count > 0 Then
    '                For Each ts As JObject In toolSetupsTable.Rows
    '                    Dim tsMachineID As Integer = ts("MachineID").Value(Of Integer)()
    '                    If tsMachineID = machine.ID Then
    '                        Dim tsNode As TreeNode = toolSetupsNode.Nodes.Add(ts("Description").Value(Of String)())
    '                        tsNode.Tag = EncodeTag(mt.ID, machine.ID, ts("ID").Value(Of Integer)(), 13) ' Store ToolSetupID
    '                    End If
    '                Next
    '            End If
    '        Next
    '    Next

    '    rootnode.Expand()
    'End Sub
    Public Sub PopulateTreeView()

        Dim nodetype As Integer

        TreeViewMachines.Nodes.Clear()

        ' --------------------------
        ' ROOT NODE: Machine Types
        ' --------------------------
        Dim machineRoot As TreeNode = TreeViewMachines.Nodes.Add("Machine Types")
        machineRoot.Tag = EncodeTag(0, -1, -1, 0)

        ' Loop through machine types (JsonTable(Of MachineType))
        For Each mt As MachineType In machineTypesTable.Rows
            Dim typeNode As TreeNode = machineRoot.Nodes.Add(mt.Display)
            Select Case UCase(mt.Display)
                Case "PUNCH" : nodetype = 1
                Case "PUNCH/PLSAMA" : nodetype = 2
                Case "LASER" : nodetype = 3
                Case "PUNCH/LASER" : nodetype = 4
                Case "BURNER" : nodetype = 5
                Case "WATERJET" : nodetype = 6
                Case "ROUTER" : nodetype = 7
                Case "POINT-TO-POINT" : nodetype = 8
                Case "MILL" : nodetype = 9
                Case Else : nodetype = 1
            End Select
            typeNode.Tag = EncodeTag(mt.ID, -1, -1, nodetype)

            ' Add machines under this type (JsonTable(Of Machine))
            Dim machinesOfType = machinesTable.Rows.Where(Function(m) m.TypeID = mt.ID)
            For Each machine In machinesOfType
                Dim machineNode As TreeNode = typeNode.Nodes.Add(machine.Description)
                machineNode.Tag = EncodeTag(mt.ID, machine.ID, -1, 10)

                ' Add child nodes "Stations" and "Tool Setups"
                Dim stationsNode As TreeNode = machineNode.Nodes.Add("Stations")
                stationsNode.Tag = EncodeTag(mt.ID, machine.ID, -1, 11)

                Dim toolSetupsNode As TreeNode = machineNode.Nodes.Add("Tool Setups")
                toolSetupsNode.Tag = EncodeTag(mt.ID, machine.ID, -1, 12)

                ' Add Tool Setup nodes (JsonTable(Of JObject))
                If toolSetupsTable IsNot Nothing AndAlso toolSetupsTable.Rows.Count > 0 Then
                    For Each ts As JObject In toolSetupsTable.Rows
                        Dim tsMachineID As Integer = ts("MachineID").Value(Of Integer)()
                        If tsMachineID = machine.ID Then
                            Dim tsDesc As String = ts("Description").Value(Of String)()
                            Dim tsID As Integer = ts("ID").Value(Of Integer)()
                            Dim tsNode As TreeNode = toolSetupsNode.Nodes.Add(tsDesc)
                            tsNode.Tag = EncodeTag(mt.ID, machine.ID, tsID, 13)
                        End If
                    Next
                End If
            Next
        Next

        machineRoot.Expand()

        ' --------------------------
        ' ROOT NODE: Material Inventory
        ' --------------------------
        Dim materialRoot As TreeNode = TreeViewMachines.Nodes.Add("Material Inventory")
        materialRoot.Tag = EncodeTag(-1, -1, -1, 100) ' nodetype 100 for root

        ' Loop through Material Types
        For Each mt As MaterialType In materialTypesTable
            Dim typeNode As TreeNode = materialRoot.Nodes.Add(mt.Display)
            typeNode.Tag = EncodeTag(-1, -1, -1, 101) ' nodetype 101 for Material Type

            ' Add sheets under each type
            For Each sheet As MaterialSheet In materialInventoryTable.Where(Function(s) s.TypeID = mt.ID)
                Dim sheetNode As TreeNode = typeNode.Nodes.Add($"{sheet.Thickness} {sheet.Width}x{sheet.Length}")
                sheetNode.Tag = EncodeTag(-1, -1, -1, 102) ' nodetype 102 for Material Sheet
            Next
        Next


        materialRoot.Expand()

    End Sub
    ' --- Populate the PropertyGrid dynamically for a machine ---
    Private Sub PopulatePropertyGridForMachine(machineID As Integer)
        Dim machine = machinesTable.Rows.FirstOrDefault(Function(m) m.ID = machineID)
        If machine Is Nothing Then
            propgrdMachine.SelectedObject = Nothing
            Return
        End If

        ' Filter attributes for this machine
        Dim attrs = machineAttributesTable.Rows.Where(Function(a) a.MachineID = machine.ID)

        ' Build dictionary for PropertyGrid
        Dim propDict As New Dictionary(Of String, Object)
        For Each attr In attrs
            Dim attrType = machineAttributeTypesTable.Rows.FirstOrDefault(Function(t) t.ID = attr.AttributeTypeID)
            If attrType IsNot Nothing Then
                propDict(attrType.Display) = attr.Value
            End If
        Next

        ' Populate PropertyGrid dynamically
        If propDict.Count = 0 Then
            propgrdMachine.SelectedObject = Nothing
        Else
            propgrdMachine.SelectedObject = New DynamicProperty.DynamicPropertyBag(propDict)
        End If
    End Sub

    ' --- Populate Stations grid dynamically ---
    Private Sub PopulateStationsGrid(machineID As Integer)

        If stationsTable Is Nothing Then Return

        isLoadingStations = True
        dgvStations.DataSource = Nothing
        dgvStations.Columns.Clear()

        ' Filter rows for the selected machine
        Dim stationRows = stationsTable.Rows.Where(Function(r) r("MachineID").Value(Of Integer)() = machineID).ToList()
        If stationRows.Count = 0 Then Return

        ' Dynamically create a DataTable for the grid
        Dim dt As New DataTable

        ' Add columns based on JSON Columns array
        For Each col In stationsTable.Columns
            If col.Visible Then
                dt.Columns.Add(col.Header, GetType(String)) ' All strings for simplicity
            End If
        Next

        ' Fill rows
        For Each row In stationRows
            Dim dr = dt.NewRow()
            For Each col In stationsTable.Columns
                If col.Visible Then
                    dr(col.Header) = If(row(col.Field) Is Nothing, "", row(col.Field).ToString())
                End If
            Next
            dt.Rows.Add(dr)
        Next

        dgvStations.DataSource = dt
        isLoadingStations = False

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
        Dim target = machineAttributesTable.Rows _
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
            machineAttributesTable.Rows.Add(target)
        End If

        ' Save JSON back to file
        Dim filePath = Path.Combine(dataFolder, "MachineAttributes.json")
        Dim jsonOut = JsonConvert.SerializeObject(machineAttributesTable, Formatting.Indented)
        File.WriteAllText(filePath, jsonOut)

        ' OPTIONAL: visual confirmation (comment out later)
        ' MessageBox.Show("Value saved.")
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

    'Private Sub btnAddMachineParameter_Click(sender As Object, e As EventArgs) Handles btnAddMachineParameter.Click
    '    ' Show the parameter dialog
    '    Using dlg As New dlgAddParam()
    '        If dlg.ShowDialog() <> DialogResult.OK Then Return

    '        ' Get parameter info from dialog
    '        Dim paramName As String = dlg.txtParamName.Text.Trim()
    '        Dim paramDisplay As String = dlg.txtParamDisplay.Text.Trim()
    '        Dim paramDataType As String = dlg.cboParamDataType.SelectedItem.ToString()
    '        Dim defaultValue As String = dlg.txtParamDefault.Text.Trim()
    '        Dim visible As Boolean = dlg.cbVisible.Checked

    '        ' --- Add new column to machinesTable ---
    '        Dim newCol As New JSon_Helper.JsonColumn With {
    '        .Field = paramName,
    '        .Header = paramDisplay,
    '        .Visible = visible,
    '        .DataType = paramDataType
    '    }
    '        machinesTable.Columns.Add(newCol)

    '        ' --- Set default value for all existing machines ---
    '        For Each machineRow In machinesTable.Rows
    '            ' Use reflection to set property dynamically or just add key/value if it's JObject
    '            Dim machineObj = machineRow ' machineRow is of type Machine
    '            machineObj(paramName) = defaultValue
    '        Next

    '        ' --- Save updated machines.json ---
    '        Dim filePath As String = Path.Combine(dataFolder, "machines.json")
    '        JsonSaver.SaveObjectToJsonFile(machinesTable, filePath)

    '        ' --- Refresh property grid ---
    '        propgrdMachine.SelectedObject = Nothing
    '        propgrdMachine.SelectedObject = machinesTable.Rows.FirstOrDefault()
    '    End Using
    'End Sub

    ''' <summary>
    ''' Refreshes the property grid for a given machine.
    ''' </summary>
    ''' <param name="machineID">The ID of the machine to refresh.</param>
    Private Sub RefreshPropertyGridForMachine(machineID As Integer)
        ' Find the machine
        Dim machine = machinesTable.Rows.FirstOrDefault(Function(m) m.ID = machineID)
        If machine Is Nothing Then
            propgrdMachine.SelectedObject = Nothing
            Return
        End If

        ' Filter attributes for this machine
        Dim attrs = machineAttributesTable.Rows.Where(Function(a) a.MachineID = machine.ID)

        ' Build dictionary for PropertyGrid
        Dim propDict As New Dictionary(Of String, Object)
        For Each attr In attrs
            Dim attrType = machineAttributeTypesTable.Rows.FirstOrDefault(Function(t) t.ID = attr.AttributeTypeID)
            If attrType IsNot Nothing Then
                propDict(attrType.Display) = attr.Value
            End If
        Next

        ' Populate PropertyGrid dynamically
        If propDict.Count = 0 Then
            propgrdMachine.SelectedObject = Nothing
        Else
            propgrdMachine.SelectedObject = New DynamicProperty.DynamicPropertyBag(propDict)
        End If
    End Sub

    Private Sub btmRemoveMachineParam_Click(sender As Object, e As EventArgs) Handles btmRemoveMachineParam.Click
        ' Ensure a property is selected
        If propgrdMachine.SelectedGridItem Is Nothing Then Return

        Dim propName As String = propgrdMachine.SelectedGridItem.Label

        ' Find the corresponding MachineAttributeType
        Dim attrType = machineAttributeTypesTable.Rows.FirstOrDefault(Function(t) t.Display = propName)
        If attrType Is Nothing Then Return

        ' Sanity check: confirm deletion
        If MessageBox.Show($"Are you sure you want to remove the parameter '{propName}'?", "Confirm Delete", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) = DialogResult.No Then
            Return
        End If

        ' Ask if removal is for all machines
        Dim removeForAll As Boolean = MessageBox.Show("Remove parameter for all machines?", "Remove Parameter", MessageBoxButtons.YesNo, MessageBoxIcon.Question) = DialogResult.Yes

        If removeForAll Then
            ' Remove from all machines
            machineAttributesTable.Rows.RemoveAll(Function(a) a.AttributeTypeID = attrType.ID)
            ' Optionally remove the attribute type globally
            machineAttributeTypesTable.Rows.Remove(attrType)
        Else
            ' Remove only for current machine
            machineAttributesTable.Rows.RemoveAll(Function(a) a.AttributeTypeID = attrType.ID AndAlso a.MachineID = currentMachineID)
        End If

        ' Save updated JSON files
        File.WriteAllText(Path.Combine(dataFolder, "MachineAttributes.json"), JsonConvert.SerializeObject(machineAttributesTable, Formatting.Indented))
        File.WriteAllText(Path.Combine(dataFolder, "MachineAttributeTypes.json"), JsonConvert.SerializeObject(machineAttributeTypesTable, Formatting.Indented))

        ' Refresh PropertyGrid for current machine
        RefreshPropertyGridForMachine(currentMachineID)

    End Sub

    Private Sub dgvStations_CurrentCellDirtyStateChanged(sender As Object, e As EventArgs) Handles dgvStations.CurrentCellDirtyStateChanged

        If dgvStations.IsCurrentCellDirty Then
            dgvStations.CommitEdit(DataGridViewDataErrorContexts.Commit)
        End If

    End Sub

    Private Sub dgvStations_CellValueChanged(sender As Object, e As DataGridViewCellEventArgs) Handles dgvStations.CellValueChanged
        ' --- Handle cell edits dynamically ---

        If isLoadingStations Then Return
        If e.RowIndex < 0 OrElse e.ColumnIndex < 0 Then Return

        Dim row = dgvStations.Rows(e.RowIndex)
        Dim drv As DataRowView = CType(row.DataBoundItem, DataRowView)
        Dim dataRow As DataRow = drv.Row

        ' Use column Name (not Header) for lookup
        Dim colField = dgvStations.Columns(e.ColumnIndex).Name
        Dim newValue = row.Cells(e.ColumnIndex).Value

        ' Lookup station in JSON table by ID
        Dim stationID = row.Cells("ID").Value.ToString()
        Dim jObj = stationsTable.Rows.FirstOrDefault(Function(r) r("ID").Value(Of String)() = stationID)
        If jObj IsNot Nothing Then
            ' Convert the new value back to original type
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
        End If

        ' Save immediately
        SaveStationsJson()

    End Sub

    Private Sub SaveStationsJson()
        ' --- Save JSON back to disk ---

        If stationsTable Is Nothing Then Exit Sub

        ' rebuild JSON rows from stationsTable (keeping all machines!)
        ' This assumes stationsTable.Rows already contains all machines; we're updating only the changed rows
        Dim output = JsonConvert.SerializeObject(stationsTable, Formatting.Indented)
        File.WriteAllText(Path.Combine(dataFolder, "Stations.json"), output)

    End Sub

    ' --- Event to allow only numbers, decimal, and control keys in numeric columns ---
    Private Sub NumericColumn_KeyPress(sender As Object, e As KeyPressEventArgs)
        If Not Char.IsControl(e.KeyChar) AndAlso Not Char.IsDigit(e.KeyChar) AndAlso e.KeyChar <> "."c Then
            e.Handled = True
        End If
    End Sub

    Private Sub bbtnAddStation_Click(sender As Object, e As EventArgs) Handles bbtnAddStation.Click

        If dgvStations.CurrentRow Is Nothing Then
            MessageBox.Show("Please select a station row first.", "Add Station", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        ' Get the currently selected row
        Dim selectedRow = dgvStations.CurrentRow
        Dim selectedNCCode As Integer
        If Not Integer.TryParse(selectedRow.Cells("NCCodeNumber").Value.ToString(), selectedNCCode) Then
            MessageBox.Show("Selected station has invalid NC Code Number.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        Dim newNCCode = selectedNCCode + 1

        ' Check if this NCCodeNumber already exists for this machine
        Dim exists = stationsTable.Rows.Any(Function(r) r("MachineID").Value(Of Integer)() = currentMachineID AndAlso r("NCCodeNumber").Value(Of Integer)() = newNCCode)
        If exists Then
            Dim input = InputBox($"Station with NCCodeNumber {newNCCode} already exists. Enter new NCCodeNumber:", "Duplicate NCCodeNumber", newNCCode.ToString())
            If Not Integer.TryParse(input, newNCCode) Then
                MessageBox.Show("Invalid NCCodeNumber entered. Operation cancelled.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
                Return
            End If
        End If

        ' Generate a new unique ID
        Dim newID = stationsTable.Rows.Max(Function(r) r("ID").Value(Of Integer)()) + 1

        ' Create the new station JObject using the columns dynamically
        Dim newStation As New JObject()
        For Each col In stationsTable.Columns
            Select Case col.Field
                Case "ID"
                    newStation(col.Field) = newID
                Case "MachineID"
                    newStation(col.Field) = currentMachineID
                Case "NCCodeNumber"
                    newStation(col.Field) = newNCCode
                Case Else
                    ' Set defaults based on JTokenType in first row if available, otherwise use simple defaults
                    Dim sampleValue As Object = Nothing
                    Dim jRow = stationsTable.Rows.FirstOrDefault()
                    If jRow IsNot Nothing AndAlso jRow.TryGetValue(col.Field, sampleValue) Then
                        ' sampleValue now contains the value
                    End If
                    'Dim sampleValue = stationsTable.Rows.FirstOrDefault()?.GetValueOrDefault(col.Field)
                    If sampleValue IsNot Nothing Then
                        newStation(col.Field) = JToken.FromObject(sampleValue.Type = JTokenType.Boolean AndAlso False OrElse 0 OrElse "")
                    Else
                        newStation(col.Field) = JToken.FromObject("")
                    End If
            End Select
        Next

        ' Add to in-memory table
        stationsTable.Rows.Add(newStation)

        ' Save to JSON
        SaveStationsJson()

        ' Refresh grid
        PopulateStationsGrid(currentMachineID)

        ' Optionally select the newly added row
        Dim dgvRow = dgvStations.Rows.Cast(Of DataGridViewRow)().FirstOrDefault(Function(r) r.Cells("NCCodeNumber").Value.ToString() = newNCCode.ToString())
        If dgvRow IsNot Nothing Then dgvRow.Selected = True

    End Sub

    Private Sub btnRemoveStation_Click(sender As Object, e As EventArgs) Handles btnRemoveStation.Click

        If dgvStations.SelectedRows.Count = 0 Then
            MessageBox.Show("Please select at least one station to remove.", "Remove Station", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        Dim confirm = MessageBox.Show("Are you sure you want to remove the selected station(s)?", "Confirm Remove", MessageBoxButtons.YesNo, MessageBoxIcon.Question)
        If confirm <> DialogResult.Yes Then Return

        For Each dgvRow As DataGridViewRow In dgvStations.SelectedRows
            Dim ncCode = dgvRow.Cells("NCCodeNumber").Value
            Dim jObj = stationsTable.Rows.FirstOrDefault(Function(r) r("MachineID").Value(Of Integer)() = currentMachineID AndAlso r("NCCodeNumber").Value(Of Integer)() = CInt(ncCode))
            If jObj IsNot Nothing Then
                stationsTable.Rows.Remove(jObj)
            End If
        Next

        ' Save JSON
        SaveStationsJson()

        ' Refresh grid
        PopulateStationsGrid(currentMachineID)

    End Sub

    Private Sub btnAutoFill_Click(sender As Object, e As EventArgs) Handles btnAutoFill.Click

        If dgvStations.SelectedRows.Count = 0 Then
            MessageBox.Show("Please select a starting station row first.", "Auto Fill Stations", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        Dim startRow As DataGridViewRow = dgvStations.SelectedRows(0)
        Dim startNCCode As Integer
        If Not Integer.TryParse(startRow.Cells("NCCodeNumber").Value.ToString(), startNCCode) Then
            MessageBox.Show("Selected row has invalid NCCodeNumber.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' Ask user how many stations to add
        Dim input As String = InputBox("Enter number of stations to add:", "Auto Fill Stations", "1")
        Dim count As Integer
        If Not Integer.TryParse(input, count) OrElse count <= 0 Then
            MessageBox.Show("Invalid number of stations.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' Add stations
        For i As Integer = 1 To count
            Dim newNCCode = startNCCode + i

            ' Check if station with this NCCodeNumber already exists
            If stationsTable.Rows.Any(Function(r) r("NCCodeNumber").Value(Of Integer)() = newNCCode) Then
                Dim userInput As String = InputBox($"Station with NCCodeNumber {newNCCode} already exists. Enter a new NCCodeNumber:", "Duplicate NCCodeNumber", newNCCode.ToString())
                If Not Integer.TryParse(userInput, newNCCode) Then
                    MessageBox.Show("Invalid NCCodeNumber. Skipping this station.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning)
                    Continue For
                End If
            End If

            ' Generate new unique ID
            Dim newID As Integer = If(stationsTable.Rows.Count > 0, stationsTable.Rows.Max(Function(r) r("ID").Value(Of Integer)()) + 1, 1)

            ' Create new JObject dynamically using columns
            Dim newStation As New JObject()
            For Each col In stationsTable.Columns
                Dim sampleValue As JToken = Nothing
                If stationsTable.Rows.FirstOrDefault() IsNot Nothing Then
                    stationsTable.Rows.First().TryGetValue(col.Field, sampleValue)
                End If

                If sampleValue IsNot Nothing Then
                    Select Case sampleValue.Type
                        Case JTokenType.Boolean : newStation(col.Field) = False
                        Case JTokenType.Integer : newStation(col.Field) = 0
                        Case JTokenType.Float : newStation(col.Field) = 0.0
                        Case Else : newStation(col.Field) = ""
                    End Select
                Else
                    newStation(col.Field) = ""
                End If
            Next

            ' Override essential fields
            newStation("ID") = newID
            newStation("MachineID") = currentMachineID
            newStation("NCCodeNumber") = newNCCode

            ' Add to in-memory table
            stationsTable.Rows.Add(newStation)
        Next

        ' Save JSON and refresh grid
        SaveStationsJson()
        PopulateStationsGrid(currentMachineID)

    End Sub

    Private Sub LoadToolSetup(toolSetupID As Integer)

        ' --- Find the ToolSetup object ---
        Dim ts As JObject = toolSetupsTable.Rows _
                    .OfType(Of JObject)() _
                    .FirstOrDefault(Function(t) t("ID").Value(Of Integer)() = toolSetupID)

        If ts Is Nothing Then
            MessageBox.Show("Tool Setup not found.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' --- Prepare DataTable for the grid ---
        Dim dt As New DataTable()

        ' --- Add columns in JSON order, NCCodeNumber & ToolDescription first ---
        dt.Columns.Add("NCCodeNumber", GetType(String))
        dt.Columns.Add("ToolDescription", GetType(String))

        For Each col As JsonColumn In stationsTable.Columns
            If col.Field <> "NCCodeNumber" AndAlso col.Visible Then
                dt.Columns.Add(col.Field, GetType(String))
            End If
        Next

        ' --- Loop through all stations for this machine ---
        Dim machineID As Integer = ts("MachineID").Value(Of Integer)()
        Dim machineStations = stationsTable.Rows _
                      .OfType(Of JObject)() _
                      .Where(Function(s) s("MachineID").Value(Of Integer)() = machineID)

        For Each station As JObject In machineStations
            Dim row = dt.NewRow()

            ' NCCodeNumber
            row("NCCodeNumber") = If(station("NCCodeNumber") IsNot Nothing, station("NCCodeNumber").ToString(), "")

            ' ToolDescription
            Dim mapping = toolSetupMembersTable.Rows _
                  .OfType(Of JObject)() _
                  .FirstOrDefault(Function(tm) tm("ToolSetupID").Value(Of Integer)() = toolSetupID AndAlso
                                                tm("StationID").Value(Of Integer)() = station("ID").Value(Of Integer)())
            If mapping IsNot Nothing Then
                Dim toolID As Integer = mapping("ToolID").Value(Of Integer)()
                Dim tool = toolcribtable.Rows _
                   .OfType(Of JObject)() _
                   .FirstOrDefault(Function(tc) tc("ID").Value(Of Integer)() = toolID)
                If tool IsNot Nothing Then
                    row("ToolDescription") = If(tool("Description") IsNot Nothing, tool("Description").ToString(), "")
                Else
                    row("ToolDescription") = ""
                End If
            Else
                row("ToolDescription") = ""
            End If

            ' Fill remaining visible columns in order
            For Each col As JsonColumn In stationsTable.Columns
                If col.Field <> "NCCodeNumber" AndAlso col.Visible Then
                    row(col.Field) = If(station(col.Field) IsNot Nothing, station(col.Field).ToString(), "")
                End If
            Next

            dt.Rows.Add(row)
        Next

        ' --- Bind DataTable to DataGridView ---
        dgvToolSetup.DataSource = dt

        ' --- Enforce column order and visibility ---
        Dim displayIndex As Integer = 0
        dgvToolSetup.Columns("NCCodeNumber").DisplayIndex = displayIndex : displayIndex += 1
        dgvToolSetup.Columns("ToolDescription").DisplayIndex = displayIndex : displayIndex += 1

        For Each col As JsonColumn In stationsTable.Columns
            If col.Field <> "NCCodeNumber" AndAlso dgvToolSetup.Columns.Contains(col.Field) Then
                dgvToolSetup.Columns(col.Field).DisplayIndex = displayIndex
                dgvToolSetup.Columns(col.Field).Visible = col.Visible
                displayIndex += 1
            End If
        Next


    End Sub

    Private Sub LoadSelectedToolSetup(toolSetupID As Integer)


        ' --- Find the ToolSetup object ---
        Dim ts As JObject = toolSetupsTable.Rows _
                        .OfType(Of JObject)() _
                        .FirstOrDefault(Function(t) t("ID").Value(Of Integer)() = toolSetupID)

        If ts Is Nothing Then
            MessageBox.Show("Tool Setup not found.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' --- Prepare DataTable for the grid ---
        Dim dt As New DataTable()

        ' Always include these first
        dt.Columns.Add("NCCodeNumber", GetType(String))
        dt.Columns.Add("ToolDescription", GetType(String))

        ' Add other columns dynamically
        For Each col In stationsTable.Columns
            If col.Field <> "NCCodeNumber" Then
                dt.Columns.Add(col.Field, GetType(String))
            End If
        Next

        Dim machineID As Integer = ts("MachineID").Value(Of Integer)()

        ' --- Build rows using LINQ ---
        Dim rows = stationsTable.Rows _
               .OfType(Of JObject)() _
               .Where(Function(s) s("MachineID").Value(Of Integer)() = machineID) _
               .Select(Function(station)
                           Dim mapping = toolSetupMembersTable.Rows _
                                         .OfType(Of JObject)() _
                                         .FirstOrDefault(Function(tm) tm("ToolSetupID").Value(Of Integer)() = toolSetupID AndAlso
                                                                       tm("StationID").Value(Of Integer)() = station("ID").Value(Of Integer)())
                           Dim toolDesc As String = ""
                           If mapping IsNot Nothing Then
                               Dim toolID = mapping("ToolID").Value(Of Integer)()
                               Dim tool = toolcribtable.Rows _
                                          .OfType(Of JObject)() _
                                          .FirstOrDefault(Function(tc) tc("ID").Value(Of Integer)() = toolID)
                               toolDesc = If(tool IsNot Nothing AndAlso tool("Description") IsNot Nothing, tool("Description").ToString(), "")
                           End If

                           Dim row = dt.NewRow()
                           row("NCCodeNumber") = If(station("NCCodeNumber") IsNot Nothing, station("NCCodeNumber").ToString(), "")
                           row("ToolDescription") = toolDesc

                           ' Fill other station fields
                           For Each col In stationsTable.Columns
                               If col.Field <> "NCCodeNumber" Then
                                   row(col.Field) = If(station(col.Field) IsNot Nothing, station(col.Field).ToString(), "")
                               End If
                           Next
                           Return row
                       End Function)

        ' Add all rows at once
        For Each r In rows
            dt.Rows.Add(r)
        Next

        ' --- Bind the grid ---
        dgvToolSetup.DataSource = dt

        ' --- Enforce column order ---
        dgvToolSetup.Columns("NCCodeNumber").DisplayIndex = 0
        dgvToolSetup.Columns("ToolDescription").DisplayIndex = 1

        ' --- Apply visibility from stationsTable.Columns ---
        For Each col In stationsTable.Columns
            If dgvToolSetup.Columns.Contains(col.Field) Then
                dgvToolSetup.Columns(col.Field).Visible = If(col.Field = "NCCodeNumber" OrElse col.Field = "ToolDescription", True, col.Visible)
            End If
        Next


    End Sub


    Public Function EncodeTag(MachineType_ID As Integer,
                                 Machine_ID As Integer,
                                 ToolSetup_ID As Integer,
                                 nodetype As Integer) As String

        'root("Machine Types") nodetype = 1
        '  └─ MachineType (Punch, Punch/Plasma,Laser,Punch/Laser, etc.) nodetype = 2
        '      └─ Machine name (Machine Description) nodetype = 3
        '          ├─ Stations (Just shows a table when selected) nodetype = 4
        '          └─ Tool Setups (Tool Setup Main Node nodetype = 5
        '              └─ ToolSetup Name (Tool Setup Description) nodetype = 6 



        EncodeTag = CHR1 & "MachineType_ID" & CHR2 & MachineType_ID &
                    CHR1 & "Machine_ID" & CHR2 & Machine_ID &
                    CHR1 & "ToolSetup_ID" & CHR2 & ToolSetup_ID &
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

    Private Function CreateToolSetupDataTable(toolSetupID As Integer) As DataTable

        Dim dt As New DataTable()

        ' --- First columns ---
        dt.Columns.Add("NCCodeNumber", GetType(String))
        dt.Columns.Add("ToolDescription", GetType(String))

        ' --- Add remaining station columns ---
        For Each col In stationsTable.Columns
            If col.Field <> "NCCodeNumber" Then
                dt.Columns.Add(col.Field, GetType(String))
            End If
        Next

        ' --- Find the ToolSetup object ---
        Dim ts As JObject = toolSetupsTable.Rows _
                    .OfType(Of JObject)() _
                    .FirstOrDefault(Function(t) t("ID").Value(Of Integer)() = toolSetupID)
        If ts Is Nothing Then Return dt

        Dim machineID As Integer = ts("MachineID").Value(Of Integer)()

        ' --- Loop through stations for this machine ---
        Dim machineStations = stationsTable.Rows _
                        .OfType(Of JObject)() _
                        .Where(Function(s) s("MachineID").Value(Of Integer)() = machineID)

        For Each station As JObject In machineStations
            Dim row = dt.NewRow()

            ' NCCodeNumber first
            If station("NCCodeNumber") IsNot Nothing Then
                row("NCCodeNumber") = station("NCCodeNumber").ToString()
            Else
                row("NCCodeNumber") = ""
            End If

            ' ToolDescription
            Dim mapping = toolSetupMembersTable.Rows _
                      .OfType(Of JObject)() _
                      .FirstOrDefault(Function(tm) tm("ToolSetupID").Value(Of Integer)() = toolSetupID AndAlso
                                                    tm("StationID").Value(Of Integer)() = station("ID").Value(Of Integer)())
            If mapping IsNot Nothing Then
                Dim toolID As Integer = mapping("ToolID").Value(Of Integer)()
                Dim tool = toolcribtable.Rows _
                       .OfType(Of JObject)() _
                       .FirstOrDefault(Function(tc) tc("ID").Value(Of Integer)() = toolID)
                If tool IsNot Nothing AndAlso tool("Description") IsNot Nothing Then
                    row("ToolDescription") = tool("Description").ToString()
                Else
                    row("ToolDescription") = ""
                End If
            Else
                row("ToolDescription") = ""
            End If

            ' Copy remaining station fields
            For Each col In stationsTable.Columns
                If col.Field <> "NCCodeNumber" Then
                    If station(col.Field) IsNot Nothing Then
                        row(col.Field) = station(col.Field).ToString()
                    Else
                        row(col.Field) = ""
                    End If
                End If
            Next

            dt.Rows.Add(row)
        Next

        Return dt
    End Function

    Private Sub PopulateToolTypeComboBox(machineTypeID As Integer)


        ' Filter list of tool types valid for this machine type
        Dim allowedTypes =
        (From mtt In machineToolTypesTable.Rows.OfType(Of JObject)()
         Join tt In toolTypesTable.Rows.OfType(Of JObject)()
            On mtt("ToolTypeID").Value(Of Integer)() Equals tt("ID").Value(Of Integer)()
         Where mtt("MachineTypeID").Value(Of Integer)() = machineTypeID
         Select New With {
             .ID = tt("ID").Value(Of Integer)(),
             .Display = tt("Display").Value(Of String)()
         }).ToList()

        ' Stop event firing during population
        RemoveHandler cboToolType.SelectedIndexChanged, AddressOf cboToolType_SelectedIndexChanged

        cboToolType.DataSource = allowedTypes
        cboToolType.DisplayMember = "Display"
        cboToolType.ValueMember = "ID"

        ' Force select first item if any
        If cboToolType.Items.Count > 0 Then
            cboToolType.SelectedIndex = 0
        End If

        ' Restore event
        AddHandler cboToolType.SelectedIndexChanged, AddressOf cboToolType_SelectedIndexChanged

        ' Immediately load tools for the first item
        LoadToolsForSelectedToolType()


    End Sub


    Private Sub LoadToolsForSelectedToolType()

        lstTools.Items.Clear()

        ' Ensure we have a valid selected item
        If cboToolType.SelectedItem Is Nothing Then Exit Sub

        ' Extract ID reliably from SelectedItem
        Dim selectedToolTypeID As Integer =
        cboToolType.SelectedItem.GetType().GetProperty("ID").GetValue(cboToolType.SelectedItem)

        ' Find all tools with matching Type ID
        Dim tools = toolcribtable.Rows _
        .OfType(Of JObject)() _
        .Where(Function(t) t("TypeID").Value(Of Integer)() = selectedToolTypeID) _
        .OrderBy(Function(t) t("Description").Value(Of String)())

        ' Populate list
        For Each tool In tools
            Dim desc As String = tool("Description").Value(Of String)()
            Dim id As Integer = tool("ID").Value(Of Integer)()
            lstTools.Items.Add($"{id}: {desc}")
        Next

    End Sub

    Private Sub cboToolType_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboToolType.SelectedIndexChanged

        LoadToolsForSelectedToolType()

    End Sub

    Private Sub lstTools_DoubleClick(sender As Object, e As EventArgs)
        ' Make sure a station row is selected
        If dgvToolSetup.SelectedRows.Count = 0 Then Return
        Dim selectedRow As DataGridViewRow = dgvToolSetup.SelectedRows(0)

        ' Make sure a tool is selected
        If lstTools.SelectedItem Is Nothing Then Return
        Dim toolInfo As String = lstTools.SelectedItem.ToString()
        ' Assuming the item is formatted as "ID: Description"
        Dim toolParts = toolInfo.Split(":"c)
        Dim toolID As Integer = CInt(toolParts(0).Trim())
        Dim toolDesc As String = toolParts(1).Trim()

        ' Update the grid's ToolDescription cell
        Dim toolDescriptionColumnIndex As Integer = dgvToolSetup.Columns("ToolDescription").Index
        selectedRow.Cells(toolDescriptionColumnIndex).Value = toolDesc

        ' Update the toolSetupMembersTable
        Dim stationID As Integer = CInt(selectedRow.Cells("ID").Value)
        Dim stationRowIndex As Integer = dgvToolSetup.CurrentRow.Index

        ' Check if a record already exists
        Dim existingMember = toolSetupMembersTable.Rows _
        .OfType(Of JObject)() _
        .FirstOrDefault(Function(t) t("ToolSetupID").Value(Of Integer)() = toolsetupid AndAlso
                                    t("StationID").Value(Of Integer)() = stationID)

        If existingMember IsNot Nothing Then
            ' Update existing record
            existingMember("ToolID") = toolID
        Else
            ' Add new record
            Dim newMember As New JObject
            newMember("ID") = toolSetupMembersTable.Rows.Max(Function(t) t("ID").Value(Of Integer)()) + 1
            newMember("ToolSetupID") = toolsetupid
            newMember("StationID") = stationID
            newMember("ToolID") = toolID
            newMember("FixedStation") = False
            newMember("IndexAngle") = 0.0

            toolSetupMembersTable.Rows.Add(newMember)
        End If

        ' Optionally autosave immediately
        SaveToolSetupMembers()

        ' Optional: update any other internal data structures here if needed

    End Sub

    Private Sub dgvToolSetup_SelectionChanged(sender As Object, e As EventArgs) Handles dgvToolSetup.SelectionChanged
        If dgvToolSetup.SelectedRows.Count = 0 Then

            currentSelectedStationID = -1
            Exit Sub
        End If

        currentSelectedStationID = CInt(dgvToolSetup.SelectedRows(0).Cells("ID").Value)
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


    ' Save ToolSetupMembers.json
    Private Sub SaveToolSetupMembers()
        Try
            Dim savePath = Path.Combine(dataFolder, "ToolSetupMembers.json")
            Dim json = JsonConvert.SerializeObject(toolSetupMembersTable, Formatting.Indented)
            File.WriteAllText(savePath, json)
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


    ' Get next ID
    Private Function GetNextToolID() As Integer

        ' If the table is empty, start at 1
        If toolcribtable.Rows.Count = 0 Then
            Return 1
        End If

        ' Find the maximum existing ID and add 1
        Dim maxID As Integer = toolcribtable.Rows _
                           .OfType(Of JObject)() _
                           .Max(Function(r) r("ID").Value(Of Integer)())

        Return maxID + 1

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

        ' --- 1. Get the selected tool from lstTools ---
        If lstTools.SelectedIndex < 0 Then
            MessageBox.Show("No tool selected.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        Dim selectedText As String = lstTools.SelectedItem.ToString()
        Dim selectedID As Integer = CInt(selectedText.Split(":"c)(0))

        Dim tool As JObject = toolcribtable.Rows _
                              .OfType(Of JObject)() _
                              .FirstOrDefault(Function(t) t("ID")?.Value(Of Integer)() = selectedID)

        If tool Is Nothing Then
            MessageBox.Show("Tool not found in memory.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' --- 2. Loop through all properties in the PropertyGrid ---
        ' --- Loop through all properties in the PropertyGrid ---
        Dim props As PropertyDescriptorCollection = TypeDescriptor.GetProperties(propgrdToolProps.SelectedObject)
        For Each prop As PropertyDescriptor In props
            Dim propName As String = prop.Name
            Dim propValue As Object = prop.GetValue(propgrdToolProps.SelectedObject)

            ' Add or update the property in the JObject
            If tool(propName) Is Nothing Then
                tool.Add(New JProperty(propName, If(propValue, "")))
            Else
                tool(propName) = If(propValue, "")
            End If
        Next

        ' --- 3. Save the updated toolCribTable back to JSON ---
        Dim filePath As String = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "toolcrib.json")
        JsonSaver.SaveObjectToJsonFile(toolcribtable, filePath)

        ' --- 4. Refresh lstTools to reflect any description changes ---
        lstTools.Items.Clear()
        For Each t As JObject In toolcribtable.Rows
            Dim desc As String = If(t("Description")?.ToString(), "")
            Dim id As String = If(t("ID")?.ToString(), "")
            lstTools.Items.Add($"{id}: {desc}")
        Next

        ' --- 5. Reselect the saved tool ---
        Dim newIndex As Integer = lstTools.Items.Cast(Of String)().ToList().FindIndex(Function(s) CInt(s.Split(":"c)(0)) = selectedID)
        If newIndex >= 0 Then
            lstTools.SelectedIndex = newIndex
            lstTools.TopIndex = newIndex
        End If

        MessageBox.Show("Tool properties saved successfully.", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information)

        tabControlMain.SelectedTab = tabToolSetup


    End Sub

    Private Sub btnAddTool_Click(sender As Object, e As EventArgs) Handles btnAddTool.Click

        ' --- 1. Generate a new unique Tool ID ---
        Dim newToolID As Integer = GetNextToolID()

        ' --- 2. Prepare a complete JObject for the new tool ---
        Dim newTool As JObject = New JObject() From {
        {"ID", newToolID},
        {"ToolTypeID", CInt(cboToolType.SelectedValue)},
        {"Description", cboToolType.Text},
        {"Length", 0.0},
        {"Diameter", 0.0},
        {"Material", ""}}
        ' Add any additional default fields your tools need here


        ' --- 3. Add the new tool to the in-memory table ---
        toolcribtable.Rows.Add(newTool)

        ' --- 4. Save the updated toolCribTable to JSON ---
        Dim filePath As String = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "toolcrib.json")
        JsonSaver.SaveObjectToJsonFile(toolcribtable, filePath)

        ' --- 5. Update lstTools safely ---
        lstTools.Items.Clear()
        For Each tool As JObject In toolcribtable.Rows
            Dim desc As String = If(tool("Description")?.ToString(), "")
            Dim id As String = If(tool("ID")?.ToString(), "")
            lstTools.Items.Add($"{id}: {desc}")
        Next

        ' --- 6. Select and highlight the new tool in the list ---
        Dim newIndex As Integer = lstTools.Items.Count - 1
        If newIndex >= 0 Then
            lstTools.SelectedIndex = newIndex
            lstTools.TopIndex = newIndex
        End If

        ' Show Tool Properties tab
        tabControlMain.SelectedTab = tabToolProperties

    End Sub

    Private Sub btnEidtTool_Click(sender As Object, e As EventArgs) Handles btnEidtTool.Click

        ' --- Make sure a tool is selected ---
        If lstTools.SelectedItem Is Nothing Then
            MessageBox.Show("Please select a tool to edit.", "Edit Tool", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        ' --- Parse selected item ID ---
        Dim selectedText As String = lstTools.SelectedItem.ToString()
        Dim colonIndex As Integer = selectedText.IndexOf(":")
        If colonIndex = -1 Then Return ' unexpected format

        Dim toolID As Integer
        If Not Integer.TryParse(selectedText.Substring(0, colonIndex), toolID) Then Return

        ' --- Find the tool in the in-memory table ---
        Dim toolToEdit As JObject = toolcribtable.Rows _
                                .OfType(Of JObject)() _
                                .FirstOrDefault(Function(t) t("ID").Value(Of Integer)() = toolID)

        If toolToEdit Is Nothing Then
            MessageBox.Show("Tool not found.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' --- Load into PropertyGrid ---
        propgrdToolProps.SelectedObject = toolToEdit

        ' --- Switch to Tool Properties tab ---
        tabControlMain.SelectedTab = tabToolProperties

    End Sub

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
        ' --- Make sure a tool is selected ---
        If lstTools.SelectedItem Is Nothing Then
            MessageBox.Show("Please select a tool to remove.", "Remove Tool", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        ' --- Confirm deletion ---
        Dim confirm As DialogResult = MessageBox.Show("Are you sure you want to remove the selected tool?",
                                                      "Confirm Remove",
                                                      MessageBoxButtons.YesNo,
                                                      MessageBoxIcon.Warning)
        If confirm <> DialogResult.Yes Then Return

        ' --- Parse selected tool ID ---
        Dim selectedText As String = lstTools.SelectedItem.ToString()
        Dim colonIndex As Integer = selectedText.IndexOf(":")
        If colonIndex = -1 Then Return

        Dim toolID As Integer
        If Not Integer.TryParse(selectedText.Substring(0, colonIndex), toolID) Then Return

        ' --- Find the tool in memory ---
        Dim toolToRemove As JObject = toolcribtable.Rows _
                                      .OfType(Of JObject)() _
                                      .FirstOrDefault(Function(t) t("ID").Value(Of Integer)() = toolID)

        If toolToRemove Is Nothing Then
            MessageBox.Show("Tool not found.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' --- Remove from the in-memory table ---
        toolcribtable.Rows.Remove(toolToRemove)

        ' --- Save the updated JSON ---
        Dim filePath As String = "toolcrib.json" ' adjust as needed
        JsonSaver.SaveObjectToJsonFile(toolcribtable, filePath)

        ' --- Update the list ---
        lstTools.Items.Remove(lstTools.SelectedItem)

        ' --- Clear PropertyGrid if it was showing the removed tool ---
        propgrdToolProps.SelectedObject = Nothing

    End Sub

    Private Sub AddMachineTSM_Click(sender As Object, e As EventArgs)
        MessageBox.Show("Hello")
    End Sub



    Private Sub DeleteMachineTSM_Click(sender As Object, e As EventArgs)
        ' --- Find the machine row in machinesTable ---
        Dim machineRow As Machine = machinesTable.Rows.FirstOrDefault(Function(m) m.ID = currentMachineID)
        If machineRow Is Nothing Then
            MessageBox.Show("Machine not found.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' --- Confirm deletion ---
        If MessageBox.Show($"Are you sure you want to delete machine '{machineRow.Description}' and all its Tool Setups?",
                       "Confirm Delete", MessageBoxButtons.YesNo, MessageBoxIcon.Question) <> DialogResult.Yes Then
            Return
        End If

        ' --- Remove associated Tool Setups ---
        For i As Integer = toolSetupsTable.Rows.Count - 1 To 0 Step -1
            Dim ts As JObject = toolSetupsTable.Rows(i)
            If ts("MachineID").Value(Of Integer)() = currentMachineID Then
                toolSetupsTable.Rows.RemoveAt(i)
            End If
        Next

        ' --- Remove the machine from machinesTable ---
        machinesTable.Rows.Remove(machineRow)

        ' --- Save changes ---
        JsonSaver.SaveObjectToJsonFile(machinesTable, Path.Combine(dataFolder, "machines.json"))
        JsonSaver.SaveObjectToJsonFile(toolSetupsTable, Path.Combine(dataFolder, "toolsetups.json"))

        ' --- Refresh TreeView ---
        PopulateTreeView() ' rebuilds tree from current machinesTable and toolSetupsTable
    End Sub

    Private Sub AddToolSetupTSM_Click(sender As Object, e As EventArgs)
        Dim name As String = InputBox("Enter new Tool Setup name:", "Add Tool Setup")
        If String.IsNullOrWhiteSpace(name) Then Return

        ' --- Determine next ID ---
        Dim nextID As Integer = If(toolSetupsTable.Rows.Count = 0, 1, toolSetupsTable.Rows.Max(Function(ts) ts("ID").Value(Of Integer)()) + 1)

        ' --- Create new ToolSetup ---
        Dim newTS As New JObject From {
        {"ID", nextID},
        {"MachineID", currentMachineID},
        {"Description", name}
    }

        ' --- Add to table ---
        toolSetupsTable.Rows.Add(newTS)

        ' --- Save ---
        JsonSaver.SaveObjectToJsonFile(toolSetupsTable, Path.Combine(dataFolder, "toolsetups.json"))

        ' --- Refresh TreeView ---
        PopulateTreeView()
    End Sub

    Private Sub DeleteToolSetupTSM_Click(sender As Object, e As EventArgs)
        ' --- Find selected ToolSetup row ---
        Dim tsRow As JObject = toolSetupsTable.Rows.FirstOrDefault(Function(ts) ts("ID").Value(Of Integer)() = currenttoolSetupID)
        If tsRow Is Nothing Then
            MessageBox.Show("Tool Setup not found.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            Return
        End If

        ' --- Confirm deletion ---
        If MessageBox.Show($"Are you sure you want to delete Tool Setup '{tsRow("Description")}'?",
                           "Confirm Delete", MessageBoxButtons.YesNo, MessageBoxIcon.Question) <> DialogResult.Yes Then
            Return
        End If

        ' --- Remove ---
        toolSetupsTable.Rows.Remove(tsRow)

        ' --- Save ---
        JsonSaver.SaveObjectToJsonFile(toolSetupsTable, Path.Combine(dataFolder, "toolsetups.json"))

        ' --- Refresh TreeView ---
        PopulateTreeView()
    End Sub


    Private Sub btnTSRemoveParameter_Click(sender As Object, e As EventArgs) Handles btnTSRemoveParameter.Click
        If dgvToolSetup.CurrentCell Is Nothing Then Return

        Dim paramName As String = dgvToolSetup.Columns(dgvToolSetup.CurrentCell.ColumnIndex).Name
        RemoveToolSetupParameter(paramName)
    End Sub

    Private Sub btnAddTSParameter_Click(sender As Object, e As EventArgs) Handles btnAddTSParameter.Click

        Dim paramForm As New dlgAddParam
        dlgAddParam.MyWhichNode = 12 ' Tool Setups Node

        dlgAddParam.IsAdding = True

        If dlgAddParam.ShowDialog() = DialogResult.OK Then
            Dim tsRow As JObject = toolSetupsTable.Rows.FirstOrDefault(Function(r) r("ID").Value(Of Integer)() = currenttoolSetupID)
            If tsRow Is Nothing Then Return

            Dim newParam As New JObject From {
            {"Name", dlgAddParam.txtParamName.Text},
            {"Display", dlgAddParam.txtParamDisplay.Text},
            {"Default", dlgAddParam.txtParamDefault.Text},
            {"DataType", dlgAddParam.cboParamDataType.SelectedItem.ToString()},
            {"Visible", dlgAddParam.cbVisible.Checked}
        }

            If tsRow("Parameters") Is Nothing Then tsRow("Parameters") = New JArray()
            tsRow("Parameters").Value(Of JArray)().Add(newParam)

            JsonSaver.SaveObjectToJsonFile(toolSetupsTable, Path.Combine(dataFolder, "toolsetups.json"))
            LoadSelectedToolSetup(currenttoolSetupID)
        End If


    End Sub

    Private Sub RemoveToolSetupParameter(parameterName As String)
        If String.IsNullOrWhiteSpace(parameterName) Then Return

        ' Confirm deletion
        If MessageBox.Show($"Are you sure you want to remove the parameter '{parameterName}' from all tool setups?",
                       "Confirm Delete", MessageBoxButtons.YesNo, MessageBoxIcon.Question) <> DialogResult.Yes Then
            Return
        End If

        ' --- Remove the column from Columns collection ---
        Dim colToRemove = stationsTable.Columns.FirstOrDefault(Function(c) c.Field = parameterName)
        If colToRemove IsNot Nothing Then
            stationsTable.Columns.Remove(colToRemove)
        End If

        ' --- Remove the field from all rows ---
        For Each row As JObject In stationsTable.Rows
            If row(parameterName) IsNot Nothing Then
                row.Remove(parameterName)
            End If
        Next

        ' --- Save changes ---
        Dim filePath = Path.Combine(dataFolder, "stations.json")
        JsonSaver.SaveObjectToJsonFile(stationsTable, filePath)

        ' --- Refresh the grid if a tool setup is selected ---
        If currenttoolSetupID > 0 Then
            LoadSelectedToolSetup(currenttoolSetupID)
        End If
    End Sub

    'Private Sub AddMachineParameter()
    '    ' Show dlgAddParam and get parameter details
    '    Using dlg As New dlgAddParam()
    '        If dlg.ShowDialog() <> DialogResult.OK Then Return

    '        Dim paramName = dlg.txtParamName.Text
    '        Dim displayName = dlg.txtParamDisplay.Text
    '        Dim defaultValue = dlg.txtParamDefault.Text
    '        Dim visible = dlg.cbVisible.Checked

    '        ' --- Add column to machinesTable.Columns ---
    '        If machinesTable.Columns.Any(Function(c) c.Field = paramName) Then
    '            MessageBox.Show("Parameter already exists.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning)
    '            Return
    '        End If

    '        machinesTable.Columns.Add(New JSon_Helper.JsonColumn With {
    '            .Field = paramName,
    '            .Header = displayName,
    '            .Visible = visible
    '        })

    '        ' --- Set default value for all Machine objects ---
    '        For Each machine As Machine In machinesTable.Rows
    '            machine.SetParameterValue(paramName, defaultValue)
    '        Next

    '        ' --- Save machinesTable to JSON ---
    '        JsonSaver.SaveObjectToJsonFile(machinesTable, Path.Combine(dataFolder, "machines.json"))

    '        ' --- Refresh PropertyGrid ---
    '        ' Rebind to currently selected machine object (if any)
    '        Dim currentMachine As Machine = propgrdMachine.SelectedObject
    '        propgrdMachine.SelectedObject = Nothing
    '        propgrdMachine.SelectedObject = currentMachine
    '    End Using
    'End Sub

    Private Sub btnAddMachineParam_Click(sender As Object, e As EventArgs) Handles btnAddMachineParam.Click
    End Sub
    Private Sub AddMachineParameter(paramDisplay As String, defaultValue As Object)
        ' --- Step 1: New attribute type ---
        Dim nextTypeID As Integer
        If machineAttributeTypesTable.Rows.Count = 0 Then
            nextTypeID = 1
        Else
            nextTypeID = machineAttributeTypesTable.Rows.Max(Function(t) t.ID) + 1
        End If

        Dim newType As New MachineAttributeType With {
            .ID = nextTypeID,
            .Display = paramDisplay
        }
        machineAttributeTypesTable.Rows.Add(newType)

        ' --- Step 2: Add attribute for each machine ---
        Dim nextAttrID As Integer = If(machineAttributesTable.Rows.Count = 0, 1, machineAttributesTable.Rows.Max(Function(a) a.ID) + 1)
        For Each machine In machinesTable.Rows
            Dim newAttr As New MachineAttribute With {
                .ID = nextAttrID,
                .MachineID = machine.ID,
                .AttributeTypeID = newType.ID,
                .Value = defaultValue
            }
            machineAttributesTable.Rows.Add(newAttr)
            nextAttrID += 1
        Next

        ' --- Step 3: Save JSON ---
        JsonSaver.SaveObjectToJsonFile(machineAttributesTable, Path.Combine(dataFolder, "MachineAttributes.json"))
        JsonSaver.SaveObjectToJsonFile(machineAttributeTypesTable, Path.Combine(dataFolder, "MachineAttributeTypes.json"))

        ' --- Refresh property grid ---
        PopulatePropertyGridForMachine(machinesTable.Rows.First().ID)
    End Sub

    Private Sub btnConvertCMDB_Click(sender As Object, e As EventArgs) Handles btnConvertCMDB.Click
        frmConvertCMDB.ShowDialog(Me)
    End Sub

    Private Sub TreeViewMachines_AfterSelect(sender As Object, e As TreeViewEventArgs) Handles TreeViewMachines.AfterSelect

        Dim Machine_ID = "Machine_ID"
        Dim ToolSetup_ID = "ToolSetup_ID"


        'EncodeTag = CHR1 & "MachineTypeD" & CHR2 & MachineTypeD &
        '            CHR1 & "MachinID" & CHR2 & MachinID &
        '            CHR1 & "ToolSetupID" & CHR2 & ToolSetupID &
        '            CHR1 & "NodeType" & CHR2 & nodetype

        Try

            Dim selectedNode = e.Node

            Dim whichNode As Integer = GetValueFromDelimitedData(CStr(selectedNode.Tag), "NodeType", "-1")

            Debug.WriteLine("TAG = " & selectedNode.Tag)
            Debug.WriteLine("NodeType = " & whichNode)


            Select Case whichNode
                Case 0 ' Root Node
                    txtPreamble.Text = "You are on the Root Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 1 'Punch Node
                    txtPreamble.Text = "You are on the Punch Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 2 'Punch/Plasma Node
                    txtPreamble.Text = "You are on the Punch/Plasma Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 3 'Laser Node
                    txtPreamble.Text = "You are on the Laser Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 4 'punch/laser Node
                    txtPreamble.Text = "You are on the Punch/Laser Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 5 'Burner Node
                    txtPreamble.Text = "You are on the Burner Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 6 'Waterjet Node
                    txtPreamble.Text = "You are on the Waterjet Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 7 'Router Node
                    txtPreamble.Text = "You are on the Router Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 8 'Point-to-Point Node
                    txtPreamble.Text = "You are on the Point-to-Point Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 9 'Mill Node
                    txtPreamble.Text = "You are on the Mill Node"
                    tabControlMain.SelectedTab = tabDefinition

                Case 10 'Actual Machine Node
                    ' Machine node selected
                    Dim machineID As Integer = GetValueFromDelimitedData(e.Node.Tag, Machine_ID, -1)
                    PopulatePropertyGridForMachine(machineID)
                    tabControlMain.SelectedTab = tabMachine
                    ' Do NOT populate stations grid yet
                    dgvStations.DataSource = Nothing
                    Return

                Case 11 'Stations Node
                    Dim machineID As Integer = GetValueFromDelimitedData(e.Node.Tag, Machine_ID, -1)
                    isLoadingStations = True
                    PopulateStationsGrid(machineID)
                    isLoadingStations = False
                    tabControlMain.SelectedTab = TabStations
                    Return

                Case 12 'Tool Setups Node
                Case 12 ' Tool Setups Node (main node, no specific tool selected)
                    txtPreamble.Text = "Select a Tool Setup to view details."
                    tabControlMain.SelectedTab = tabToolSetup
                    dgvToolSetup.DataSource = Nothing ' Clear grid

                Case 13 'Actual Tool Setup Node
                    Dim machineID As Integer = GetValueFromDelimitedData(e.Node.Tag, Machine_ID, -1)
                    toolsetupid = GetValueFromDelimitedData(e.Node.Tag, ToolSetup_ID, -1)

                    LoadSelectedToolSetup(toolsetupid)

                    Dim MachineToolTypeID As Integer = GetValueFromDelimitedData(e.Node.Tag, "MachineType_ID", -1)

                    PopulateToolTypeComboBox(MachineToolTypeID)

                    tabControlMain.SelectedTab = tabToolSetup
                    InitializeToolDescriptionColumnIndex()



                    Return

                Case 100

                Case 101

                Case 102


                Case Else
                    txtPreamble.Text = "Unknown Node Selected"
                    tabControlMain.SelectedTab = tabDefinition

            End Select

        Catch ex As Exception
            MessageBox.Show($"Error selecting node: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    Private Sub TreeViewMachines_NodeMouseClick(sender As Object, e As TreeNodeMouseClickEventArgs) Handles TreeViewMachines.NodeMouseClick

        ' Only act on right-click
        If e.Button <> MouseButtons.Right Then Return

        Dim selectedNode = e.Node
        TreeViewMachines.SelectedNode = selectedNode

        ' Clear any previous menu items
        cmsTreeActions.Items.Clear()

        ' Decode the NodeType from the encoded Tag
        Dim whichNode As Integer = GetValueFromDelimitedData(CStr(selectedNode.Tag), "NodeType", "-1")

        Select Case whichNode

            Case 0 ' Root Node
                txtPreamble.Text = "You are on the Root Node"
                tabControlMain.SelectedTab = tabDefinition

            Case 1, 2, 3, 4, 5, 6, 7, 8, 9 ' Machine Type Nodes
                cmsTreeActions.Items.Add("Add Machine", Nothing, AddressOf AddMachineTSM_Click)

            Case 10 'Actual Machine Node
                ' Delete Machine
                cmsTreeActions.Items.Add("Delete Machine", Nothing, AddressOf DeleteMachineTSM_Click)

            Case 12 'Tool Setups Node
                ' Add Tool Setup
                cmsTreeActions.Items.Add("Add Tool Setup", Nothing, AddressOf AddToolSetupTSM_Click)

            Case 13 'Actual Tool Setup Node
                ' Delete Tool Setup
                cmsTreeActions.Items.Add("Delete Tool Setup", Nothing, AddressOf DeleteToolSetupTSM_Click)

            Case 100
                'AddHandler Material type
                cmsTreeActions.Items.Add("Add Material Type", Nothing, AddressOf AddMaterialTypeToolStripMenuItem_Click)
            Case 101
                'Add material Sheet
                'AddHandler Material type
                cmsTreeActions.Items.Add("Add Material Size", Nothing, AddressOf AddMaterialSizeToolStripMenuItem_Click)
            Case 102


        End Select

        ' Show the context menu at mouse location
        cmsTreeActions.Show(TreeViewMachines, e.Location)
    End Sub

    Private Sub AddMaterialTypeToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles AddMaterialTypeToolStripMenuItem.Click

    End Sub

    Private Sub AddMaterialSizeToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles AddMaterialSizeToolStripMenuItem.Click

    End Sub

    Private Sub AddMachineToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles AddMachineToolStripMenuItem.Click
        Dim name As String = InputBox("Enter new machine name:", "Add Machine")
        If String.IsNullOrWhiteSpace(name) Then Return

        ' --- Determine next ID ---
        Dim nextID As Integer = If(machinesTable.Rows.Count = 0, 1, machinesTable.Rows.Max(Function(m) m.ID) + 1)

        ' --- Create new Machine ---
        Dim newMachine As New Machine With {
        .ID = nextID,
        .Description = name,
        .TypeID = 0
    }

        ' --- Add to machinesTable ---
        machinesTable.Rows.Add(newMachine)

        ' --- Save ---
        JsonSaver.SaveObjectToJsonFile(machinesTable, Path.Combine(dataFolder, "machines.json"))

        ' --- Refresh TreeView ---
        PopulateTreeView()
    End Sub
End Class
