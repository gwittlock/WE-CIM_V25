<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ConfigurationManagerForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.SplitContainer1 = New System.Windows.Forms.SplitContainer()
        Me.SplitContainer2 = New System.Windows.Forms.SplitContainer()
        Me.TreeViewMachines = New System.Windows.Forms.TreeView()
        Me.btnConvertCMDB = New System.Windows.Forms.Button()
        Me.tabControlMain = New System.Windows.Forms.TabControl()
        Me.tabMachine = New System.Windows.Forms.TabPage()
        Me.btmRemoveMachineParam = New System.Windows.Forms.Button()
        Me.btnAddMachineParam = New System.Windows.Forms.Button()
        Me.propgrdMachine = New System.Windows.Forms.PropertyGrid()
        Me.TabStations = New System.Windows.Forms.TabPage()
        Me.btnRemoveStation = New System.Windows.Forms.Button()
        Me.bbtnAddStation = New System.Windows.Forms.Button()
        Me.btnAutoFill = New System.Windows.Forms.Button()
        Me.dgvStations = New System.Windows.Forms.DataGridView()
        Me.tabToolSetup = New System.Windows.Forms.TabPage()
        Me.SplitContainer3 = New System.Windows.Forms.SplitContainer()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.btnTSRemoveParameter = New System.Windows.Forms.Button()
        Me.btnAddTSParameter = New System.Windows.Forms.Button()
        Me.dgvToolSetup = New System.Windows.Forms.DataGridView()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.lstTools = New System.Windows.Forms.ListBox()
        Me.btnEidtTool = New System.Windows.Forms.Button()
        Me.btnRemoveTool = New System.Windows.Forms.Button()
        Me.btnAddTool = New System.Windows.Forms.Button()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cboToolType = New System.Windows.Forms.ComboBox()
        Me.tabDefinition = New System.Windows.Forms.TabPage()
        Me.txtPreamble = New System.Windows.Forms.TextBox()
        Me.tabToolProperties = New System.Windows.Forms.TabPage()
        Me.Panel3 = New System.Windows.Forms.Panel()
        Me.btnCancelToolProps = New System.Windows.Forms.Button()
        Me.btnSaveToolProps = New System.Windows.Forms.Button()
        Me.Panel2 = New System.Windows.Forms.Panel()
        Me.lblCurrentDescription = New System.Windows.Forms.Label()
        Me.lblCurrentToolSetup = New System.Windows.Forms.Label()
        Me.lblCurrentMachine = New System.Windows.Forms.Label()
        Me.propgrdToolProps = New System.Windows.Forms.PropertyGrid()
        Me.cmsTreeActions = New System.Windows.Forms.ContextMenuStrip(Me.components)
        Me.AddMachineToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.DeleteMachineToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.DeleteToolSetupToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.AddToolSetupToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.AddMaterialTypeToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.AddMaterialSizeToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.RemoveMaterialTypeToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.RemoveMaterialSizeToolStripItem = New System.Windows.Forms.ToolStripMenuItem()
        CType(Me.SplitContainer1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainer1.Panel1.SuspendLayout()
        Me.SplitContainer1.Panel2.SuspendLayout()
        Me.SplitContainer1.SuspendLayout()
        CType(Me.SplitContainer2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainer2.Panel1.SuspendLayout()
        Me.SplitContainer2.Panel2.SuspendLayout()
        Me.SplitContainer2.SuspendLayout()
        Me.tabControlMain.SuspendLayout()
        Me.tabMachine.SuspendLayout()
        Me.TabStations.SuspendLayout()
        CType(Me.dgvStations, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tabToolSetup.SuspendLayout()
        CType(Me.SplitContainer3, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainer3.Panel1.SuspendLayout()
        Me.SplitContainer3.Panel2.SuspendLayout()
        Me.SplitContainer3.SuspendLayout()
        Me.Panel1.SuspendLayout()
        CType(Me.dgvToolSetup, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        Me.tabDefinition.SuspendLayout()
        Me.tabToolProperties.SuspendLayout()
        Me.Panel3.SuspendLayout()
        Me.Panel2.SuspendLayout()
        Me.cmsTreeActions.SuspendLayout()
        Me.SuspendLayout()
        '
        'SplitContainer1
        '
        Me.SplitContainer1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainer1.Location = New System.Drawing.Point(0, 0)
        Me.SplitContainer1.Name = "SplitContainer1"
        '
        'SplitContainer1.Panel1
        '
        Me.SplitContainer1.Panel1.Controls.Add(Me.SplitContainer2)
        '
        'SplitContainer1.Panel2
        '
        Me.SplitContainer1.Panel2.Controls.Add(Me.tabControlMain)
        Me.SplitContainer1.Size = New System.Drawing.Size(1204, 605)
        Me.SplitContainer1.SplitterDistance = 401
        Me.SplitContainer1.TabIndex = 0
        '
        'SplitContainer2
        '
        Me.SplitContainer2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainer2.Location = New System.Drawing.Point(0, 0)
        Me.SplitContainer2.Name = "SplitContainer2"
        Me.SplitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal
        '
        'SplitContainer2.Panel1
        '
        Me.SplitContainer2.Panel1.Controls.Add(Me.TreeViewMachines)
        Me.SplitContainer2.Panel1.Margin = New System.Windows.Forms.Padding(5)
        '
        'SplitContainer2.Panel2
        '
        Me.SplitContainer2.Panel2.Controls.Add(Me.btnConvertCMDB)
        Me.SplitContainer2.Panel2.Margin = New System.Windows.Forms.Padding(5)
        Me.SplitContainer2.Size = New System.Drawing.Size(401, 605)
        Me.SplitContainer2.SplitterDistance = 537
        Me.SplitContainer2.TabIndex = 0
        '
        'TreeViewMachines
        '
        Me.TreeViewMachines.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TreeViewMachines.Location = New System.Drawing.Point(0, 0)
        Me.TreeViewMachines.Name = "TreeViewMachines"
        Me.TreeViewMachines.Size = New System.Drawing.Size(401, 537)
        Me.TreeViewMachines.TabIndex = 0
        '
        'btnConvertCMDB
        '
        Me.btnConvertCMDB.AutoSize = True
        Me.btnConvertCMDB.Location = New System.Drawing.Point(141, 14)
        Me.btnConvertCMDB.Name = "btnConvertCMDB"
        Me.btnConvertCMDB.Size = New System.Drawing.Size(88, 23)
        Me.btnConvertCMDB.TabIndex = 0
        Me.btnConvertCMDB.Text = "Convert CMDB"
        Me.btnConvertCMDB.UseVisualStyleBackColor = True
        '
        'tabControlMain
        '
        Me.tabControlMain.Controls.Add(Me.tabMachine)
        Me.tabControlMain.Controls.Add(Me.TabStations)
        Me.tabControlMain.Controls.Add(Me.tabToolSetup)
        Me.tabControlMain.Controls.Add(Me.tabDefinition)
        Me.tabControlMain.Controls.Add(Me.tabToolProperties)
        Me.tabControlMain.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tabControlMain.Location = New System.Drawing.Point(0, 0)
        Me.tabControlMain.Name = "tabControlMain"
        Me.tabControlMain.SelectedIndex = 0
        Me.tabControlMain.Size = New System.Drawing.Size(799, 605)
        Me.tabControlMain.TabIndex = 0
        '
        'tabMachine
        '
        Me.tabMachine.Controls.Add(Me.btmRemoveMachineParam)
        Me.tabMachine.Controls.Add(Me.btnAddMachineParam)
        Me.tabMachine.Controls.Add(Me.propgrdMachine)
        Me.tabMachine.Location = New System.Drawing.Point(4, 22)
        Me.tabMachine.Name = "tabMachine"
        Me.tabMachine.Padding = New System.Windows.Forms.Padding(3)
        Me.tabMachine.Size = New System.Drawing.Size(791, 579)
        Me.tabMachine.TabIndex = 0
        Me.tabMachine.Text = "Machines"
        Me.tabMachine.UseVisualStyleBackColor = True
        '
        'btmRemoveMachineParam
        '
        Me.btmRemoveMachineParam.AutoSize = True
        Me.btmRemoveMachineParam.Location = New System.Drawing.Point(235, 535)
        Me.btmRemoveMachineParam.Name = "btmRemoveMachineParam"
        Me.btmRemoveMachineParam.Size = New System.Drawing.Size(108, 23)
        Me.btmRemoveMachineParam.TabIndex = 2
        Me.btmRemoveMachineParam.Text = "Remove Parameter"
        Me.btmRemoveMachineParam.UseVisualStyleBackColor = True
        '
        'btnAddMachineParam
        '
        Me.btnAddMachineParam.AutoSize = True
        Me.btnAddMachineParam.Location = New System.Drawing.Point(57, 535)
        Me.btnAddMachineParam.Name = "btnAddMachineParam"
        Me.btnAddMachineParam.Size = New System.Drawing.Size(87, 23)
        Me.btnAddMachineParam.TabIndex = 1
        Me.btnAddMachineParam.Text = "Add Parameter"
        Me.btnAddMachineParam.UseVisualStyleBackColor = True
        '
        'propgrdMachine
        '
        Me.propgrdMachine.Dock = System.Windows.Forms.DockStyle.Fill
        Me.propgrdMachine.Location = New System.Drawing.Point(3, 3)
        Me.propgrdMachine.Name = "propgrdMachine"
        Me.propgrdMachine.Size = New System.Drawing.Size(785, 573)
        Me.propgrdMachine.TabIndex = 0
        '
        'TabStations
        '
        Me.TabStations.Controls.Add(Me.btnRemoveStation)
        Me.TabStations.Controls.Add(Me.bbtnAddStation)
        Me.TabStations.Controls.Add(Me.btnAutoFill)
        Me.TabStations.Controls.Add(Me.dgvStations)
        Me.TabStations.Location = New System.Drawing.Point(4, 22)
        Me.TabStations.Name = "TabStations"
        Me.TabStations.Padding = New System.Windows.Forms.Padding(3)
        Me.TabStations.Size = New System.Drawing.Size(791, 579)
        Me.TabStations.TabIndex = 1
        Me.TabStations.Text = "Stations"
        Me.TabStations.UseVisualStyleBackColor = True
        '
        'btnRemoveStation
        '
        Me.btnRemoveStation.AutoSize = True
        Me.btnRemoveStation.Location = New System.Drawing.Point(550, 548)
        Me.btnRemoveStation.Name = "btnRemoveStation"
        Me.btnRemoveStation.Size = New System.Drawing.Size(93, 23)
        Me.btnRemoveStation.TabIndex = 3
        Me.btnRemoveStation.Text = "Remove Station"
        Me.btnRemoveStation.UseVisualStyleBackColor = True
        '
        'bbtnAddStation
        '
        Me.bbtnAddStation.AutoSize = True
        Me.bbtnAddStation.Location = New System.Drawing.Point(438, 548)
        Me.bbtnAddStation.Name = "bbtnAddStation"
        Me.bbtnAddStation.Size = New System.Drawing.Size(75, 23)
        Me.bbtnAddStation.TabIndex = 2
        Me.bbtnAddStation.Text = "Add Station"
        Me.bbtnAddStation.UseVisualStyleBackColor = True
        '
        'btnAutoFill
        '
        Me.btnAutoFill.AutoSize = True
        Me.btnAutoFill.Location = New System.Drawing.Point(51, 548)
        Me.btnAutoFill.Name = "btnAutoFill"
        Me.btnAutoFill.Size = New System.Drawing.Size(98, 23)
        Me.btnAutoFill.TabIndex = 1
        Me.btnAutoFill.Text = "AUto FIll Stations"
        Me.btnAutoFill.UseVisualStyleBackColor = True
        '
        'dgvStations
        '
        Me.dgvStations.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvStations.Dock = System.Windows.Forms.DockStyle.Top
        Me.dgvStations.Location = New System.Drawing.Point(3, 3)
        Me.dgvStations.Name = "dgvStations"
        Me.dgvStations.Size = New System.Drawing.Size(785, 527)
        Me.dgvStations.TabIndex = 0
        '
        'tabToolSetup
        '
        Me.tabToolSetup.Controls.Add(Me.SplitContainer3)
        Me.tabToolSetup.Location = New System.Drawing.Point(4, 22)
        Me.tabToolSetup.Name = "tabToolSetup"
        Me.tabToolSetup.Size = New System.Drawing.Size(791, 579)
        Me.tabToolSetup.TabIndex = 2
        Me.tabToolSetup.Text = "Tool Setups"
        Me.tabToolSetup.UseVisualStyleBackColor = True
        '
        'SplitContainer3
        '
        Me.SplitContainer3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainer3.Location = New System.Drawing.Point(0, 0)
        Me.SplitContainer3.Name = "SplitContainer3"
        '
        'SplitContainer3.Panel1
        '
        Me.SplitContainer3.Panel1.Controls.Add(Me.Panel1)
        Me.SplitContainer3.Panel1.Controls.Add(Me.dgvToolSetup)
        '
        'SplitContainer3.Panel2
        '
        Me.SplitContainer3.Panel2.Controls.Add(Me.GroupBox1)
        Me.SplitContainer3.Size = New System.Drawing.Size(791, 579)
        Me.SplitContainer3.SplitterDistance = 390
        Me.SplitContainer3.TabIndex = 0
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.btnTSRemoveParameter)
        Me.Panel1.Controls.Add(Me.btnAddTSParameter)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.Panel1.Location = New System.Drawing.Point(0, 519)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(390, 60)
        Me.Panel1.TabIndex = 1
        '
        'btnTSRemoveParameter
        '
        Me.btnTSRemoveParameter.AutoSize = True
        Me.btnTSRemoveParameter.Location = New System.Drawing.Point(239, 29)
        Me.btnTSRemoveParameter.Name = "btnTSRemoveParameter"
        Me.btnTSRemoveParameter.Size = New System.Drawing.Size(108, 23)
        Me.btnTSRemoveParameter.TabIndex = 1
        Me.btnTSRemoveParameter.Text = "Remove Parameter"
        Me.btnTSRemoveParameter.UseVisualStyleBackColor = True
        '
        'btnAddTSParameter
        '
        Me.btnAddTSParameter.AutoSize = True
        Me.btnAddTSParameter.Location = New System.Drawing.Point(51, 29)
        Me.btnAddTSParameter.Name = "btnAddTSParameter"
        Me.btnAddTSParameter.Size = New System.Drawing.Size(87, 23)
        Me.btnAddTSParameter.TabIndex = 0
        Me.btnAddTSParameter.Text = "Add Parameter"
        Me.btnAddTSParameter.UseVisualStyleBackColor = True
        '
        'dgvToolSetup
        '
        Me.dgvToolSetup.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvToolSetup.Dock = System.Windows.Forms.DockStyle.Top
        Me.dgvToolSetup.Location = New System.Drawing.Point(0, 0)
        Me.dgvToolSetup.Name = "dgvToolSetup"
        Me.dgvToolSetup.Size = New System.Drawing.Size(390, 513)
        Me.dgvToolSetup.TabIndex = 0
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.lstTools)
        Me.GroupBox1.Controls.Add(Me.btnEidtTool)
        Me.GroupBox1.Controls.Add(Me.btnRemoveTool)
        Me.GroupBox1.Controls.Add(Me.btnAddTool)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.cboToolType)
        Me.GroupBox1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.GroupBox1.Location = New System.Drawing.Point(0, 0)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(397, 579)
        Me.GroupBox1.TabIndex = 0
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Tool Inventory"
        '
        'lstTools
        '
        Me.lstTools.FormattingEnabled = True
        Me.lstTools.Location = New System.Drawing.Point(3, 74)
        Me.lstTools.Name = "lstTools"
        Me.lstTools.Size = New System.Drawing.Size(386, 459)
        Me.lstTools.TabIndex = 6
        '
        'btnEidtTool
        '
        Me.btnEidtTool.Location = New System.Drawing.Point(305, 548)
        Me.btnEidtTool.Name = "btnEidtTool"
        Me.btnEidtTool.Size = New System.Drawing.Size(75, 23)
        Me.btnEidtTool.TabIndex = 5
        Me.btnEidtTool.Text = "Edit Tool"
        Me.btnEidtTool.UseVisualStyleBackColor = True
        '
        'btnRemoveTool
        '
        Me.btnRemoveTool.AutoSize = True
        Me.btnRemoveTool.Location = New System.Drawing.Point(218, 548)
        Me.btnRemoveTool.Name = "btnRemoveTool"
        Me.btnRemoveTool.Size = New System.Drawing.Size(81, 23)
        Me.btnRemoveTool.TabIndex = 4
        Me.btnRemoveTool.Text = "Remove Tool"
        Me.btnRemoveTool.UseVisualStyleBackColor = True
        '
        'btnAddTool
        '
        Me.btnAddTool.AutoSize = True
        Me.btnAddTool.Location = New System.Drawing.Point(27, 548)
        Me.btnAddTool.Name = "btnAddTool"
        Me.btnAddTool.Size = New System.Drawing.Size(75, 23)
        Me.btnAddTool.TabIndex = 3
        Me.btnAddTool.Text = "Add Tool"
        Me.btnAddTool.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(6, 31)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(55, 13)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "Tool Type"
        '
        'cboToolType
        '
        Me.cboToolType.FormattingEnabled = True
        Me.cboToolType.Location = New System.Drawing.Point(6, 47)
        Me.cboToolType.Name = "cboToolType"
        Me.cboToolType.Size = New System.Drawing.Size(383, 21)
        Me.cboToolType.TabIndex = 0
        '
        'tabDefinition
        '
        Me.tabDefinition.Controls.Add(Me.txtPreamble)
        Me.tabDefinition.Location = New System.Drawing.Point(4, 22)
        Me.tabDefinition.Name = "tabDefinition"
        Me.tabDefinition.Size = New System.Drawing.Size(791, 579)
        Me.tabDefinition.TabIndex = 3
        Me.tabDefinition.Text = "Definition"
        Me.tabDefinition.UseVisualStyleBackColor = True
        '
        'txtPreamble
        '
        Me.txtPreamble.Dock = System.Windows.Forms.DockStyle.Fill
        Me.txtPreamble.Location = New System.Drawing.Point(0, 0)
        Me.txtPreamble.Multiline = True
        Me.txtPreamble.Name = "txtPreamble"
        Me.txtPreamble.Size = New System.Drawing.Size(791, 579)
        Me.txtPreamble.TabIndex = 0
        Me.txtPreamble.Text = "This is where we would put some guidance for the user. this text will change base" &
    "d on with node is highlighted."
        '
        'tabToolProperties
        '
        Me.tabToolProperties.Controls.Add(Me.Panel3)
        Me.tabToolProperties.Controls.Add(Me.Panel2)
        Me.tabToolProperties.Controls.Add(Me.propgrdToolProps)
        Me.tabToolProperties.Location = New System.Drawing.Point(4, 22)
        Me.tabToolProperties.Name = "tabToolProperties"
        Me.tabToolProperties.Size = New System.Drawing.Size(791, 579)
        Me.tabToolProperties.TabIndex = 4
        Me.tabToolProperties.Text = "Tool Properties"
        Me.tabToolProperties.UseVisualStyleBackColor = True
        '
        'Panel3
        '
        Me.Panel3.Controls.Add(Me.btnCancelToolProps)
        Me.Panel3.Controls.Add(Me.btnSaveToolProps)
        Me.Panel3.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.Panel3.Location = New System.Drawing.Point(300, 518)
        Me.Panel3.Name = "Panel3"
        Me.Panel3.Size = New System.Drawing.Size(491, 61)
        Me.Panel3.TabIndex = 2
        '
        'btnCancelToolProps
        '
        Me.btnCancelToolProps.Location = New System.Drawing.Point(397, 30)
        Me.btnCancelToolProps.Name = "btnCancelToolProps"
        Me.btnCancelToolProps.Size = New System.Drawing.Size(75, 23)
        Me.btnCancelToolProps.TabIndex = 1
        Me.btnCancelToolProps.Text = "Cancel"
        Me.btnCancelToolProps.UseVisualStyleBackColor = True
        '
        'btnSaveToolProps
        '
        Me.btnSaveToolProps.Location = New System.Drawing.Point(316, 30)
        Me.btnSaveToolProps.Name = "btnSaveToolProps"
        Me.btnSaveToolProps.Size = New System.Drawing.Size(75, 23)
        Me.btnSaveToolProps.TabIndex = 0
        Me.btnSaveToolProps.Text = "Save Tool"
        Me.btnSaveToolProps.UseVisualStyleBackColor = True
        '
        'Panel2
        '
        Me.Panel2.Controls.Add(Me.lblCurrentDescription)
        Me.Panel2.Controls.Add(Me.lblCurrentToolSetup)
        Me.Panel2.Controls.Add(Me.lblCurrentMachine)
        Me.Panel2.Dock = System.Windows.Forms.DockStyle.Top
        Me.Panel2.Location = New System.Drawing.Point(300, 0)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(491, 100)
        Me.Panel2.TabIndex = 1
        '
        'lblCurrentDescription
        '
        Me.lblCurrentDescription.AutoSize = True
        Me.lblCurrentDescription.Location = New System.Drawing.Point(41, 70)
        Me.lblCurrentDescription.Name = "lblCurrentDescription"
        Me.lblCurrentDescription.Size = New System.Drawing.Size(104, 13)
        Me.lblCurrentDescription.TabIndex = 2
        Me.lblCurrentDescription.Text = "lblCurrentDescription"
        '
        'lblCurrentToolSetup
        '
        Me.lblCurrentToolSetup.AutoSize = True
        Me.lblCurrentToolSetup.Location = New System.Drawing.Point(41, 46)
        Me.lblCurrentToolSetup.Name = "lblCurrentToolSetup"
        Me.lblCurrentToolSetup.Size = New System.Drawing.Size(100, 13)
        Me.lblCurrentToolSetup.TabIndex = 1
        Me.lblCurrentToolSetup.Text = "lblCurrentToolSetup"
        '
        'lblCurrentMachine
        '
        Me.lblCurrentMachine.AutoSize = True
        Me.lblCurrentMachine.Location = New System.Drawing.Point(41, 17)
        Me.lblCurrentMachine.Name = "lblCurrentMachine"
        Me.lblCurrentMachine.Size = New System.Drawing.Size(92, 13)
        Me.lblCurrentMachine.TabIndex = 0
        Me.lblCurrentMachine.Text = "lblCurrentMachine"
        '
        'propgrdToolProps
        '
        Me.propgrdToolProps.Dock = System.Windows.Forms.DockStyle.Left
        Me.propgrdToolProps.Location = New System.Drawing.Point(0, 0)
        Me.propgrdToolProps.Name = "propgrdToolProps"
        Me.propgrdToolProps.Size = New System.Drawing.Size(300, 579)
        Me.propgrdToolProps.TabIndex = 0
        '
        'cmsTreeActions
        '
        Me.cmsTreeActions.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.AddMachineToolStripMenuItem, Me.DeleteMachineToolStripMenuItem, Me.DeleteToolSetupToolStripMenuItem, Me.AddToolSetupToolStripMenuItem, Me.AddMaterialTypeToolStripMenuItem, Me.AddMaterialSizeToolStripMenuItem, Me.RemoveMaterialTypeToolStripMenuItem, Me.RemoveMaterialSizeToolStripItem})
        Me.cmsTreeActions.Name = "cmsTreeActions"
        Me.cmsTreeActions.Size = New System.Drawing.Size(207, 180)
        '
        'AddMachineToolStripMenuItem
        '
        Me.AddMachineToolStripMenuItem.Name = "AddMachineToolStripMenuItem"
        Me.AddMachineToolStripMenuItem.Size = New System.Drawing.Size(206, 22)
        Me.AddMachineToolStripMenuItem.Text = "Add Machine"
        '
        'DeleteMachineToolStripMenuItem
        '
        Me.DeleteMachineToolStripMenuItem.Name = "DeleteMachineToolStripMenuItem"
        Me.DeleteMachineToolStripMenuItem.Size = New System.Drawing.Size(206, 22)
        Me.DeleteMachineToolStripMenuItem.Text = "Delete Machine"
        '
        'DeleteToolSetupToolStripMenuItem
        '
        Me.DeleteToolSetupToolStripMenuItem.Name = "DeleteToolSetupToolStripMenuItem"
        Me.DeleteToolSetupToolStripMenuItem.Size = New System.Drawing.Size(206, 22)
        Me.DeleteToolSetupToolStripMenuItem.Text = "Delete Tool Setup"
        '
        'AddToolSetupToolStripMenuItem
        '
        Me.AddToolSetupToolStripMenuItem.Name = "AddToolSetupToolStripMenuItem"
        Me.AddToolSetupToolStripMenuItem.Size = New System.Drawing.Size(206, 22)
        Me.AddToolSetupToolStripMenuItem.Text = "Add Tool Setup"
        '
        'AddMaterialTypeToolStripMenuItem
        '
        Me.AddMaterialTypeToolStripMenuItem.Name = "AddMaterialTypeToolStripMenuItem"
        Me.AddMaterialTypeToolStripMenuItem.Size = New System.Drawing.Size(206, 22)
        Me.AddMaterialTypeToolStripMenuItem.Text = "Add Material Type"
        '
        'AddMaterialSizeToolStripMenuItem
        '
        Me.AddMaterialSizeToolStripMenuItem.Name = "AddMaterialSizeToolStripMenuItem"
        Me.AddMaterialSizeToolStripMenuItem.Size = New System.Drawing.Size(206, 22)
        Me.AddMaterialSizeToolStripMenuItem.Text = "Add Material Size"
        '
        'RemoveMaterialTypeToolStripMenuItem
        '
        Me.RemoveMaterialTypeToolStripMenuItem.Name = "RemoveMaterialTypeToolStripMenuItem"
        Me.RemoveMaterialTypeToolStripMenuItem.Size = New System.Drawing.Size(206, 22)
        Me.RemoveMaterialTypeToolStripMenuItem.Text = "Remove Material Type"
        '
        'RemoveMaterialSizeToolStripItem
        '
        Me.RemoveMaterialSizeToolStripItem.Name = "RemoveMaterialSizeToolStripItem"
        Me.RemoveMaterialSizeToolStripItem.Size = New System.Drawing.Size(206, 22)
        Me.RemoveMaterialSizeToolStripItem.Text = "Remove Material "
        '
        'ConfigurationManagerForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1204, 605)
        Me.Controls.Add(Me.SplitContainer1)
        Me.Name = "ConfigurationManagerForm"
        Me.Text = "ConfigurationManagerForm"
        Me.SplitContainer1.Panel1.ResumeLayout(False)
        Me.SplitContainer1.Panel2.ResumeLayout(False)
        CType(Me.SplitContainer1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer1.ResumeLayout(False)
        Me.SplitContainer2.Panel1.ResumeLayout(False)
        Me.SplitContainer2.Panel2.ResumeLayout(False)
        Me.SplitContainer2.Panel2.PerformLayout()
        CType(Me.SplitContainer2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer2.ResumeLayout(False)
        Me.tabControlMain.ResumeLayout(False)
        Me.tabMachine.ResumeLayout(False)
        Me.tabMachine.PerformLayout()
        Me.TabStations.ResumeLayout(False)
        Me.TabStations.PerformLayout()
        CType(Me.dgvStations, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tabToolSetup.ResumeLayout(False)
        Me.SplitContainer3.Panel1.ResumeLayout(False)
        Me.SplitContainer3.Panel2.ResumeLayout(False)
        CType(Me.SplitContainer3, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer3.ResumeLayout(False)
        Me.Panel1.ResumeLayout(False)
        Me.Panel1.PerformLayout()
        CType(Me.dgvToolSetup, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.tabDefinition.ResumeLayout(False)
        Me.tabDefinition.PerformLayout()
        Me.tabToolProperties.ResumeLayout(False)
        Me.Panel3.ResumeLayout(False)
        Me.Panel2.ResumeLayout(False)
        Me.Panel2.PerformLayout()
        Me.cmsTreeActions.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents SplitContainer1 As SplitContainer
    Friend WithEvents SplitContainer2 As SplitContainer
    Friend WithEvents TreeViewMachines As TreeView
    Friend WithEvents btnConvertCMDB As Button
    Friend WithEvents tabControlMain As TabControl
    Friend WithEvents tabMachine As TabPage
    Friend WithEvents TabStations As TabPage
    Friend WithEvents tabToolSetup As TabPage
    Friend WithEvents tabDefinition As TabPage
    Friend WithEvents tabToolProperties As TabPage
    Friend WithEvents btmRemoveMachineParam As Button
    Friend WithEvents btnAddMachineParam As Button
    Friend WithEvents propgrdMachine As PropertyGrid
    Friend WithEvents btnRemoveStation As Button
    Friend WithEvents bbtnAddStation As Button
    Friend WithEvents btnAutoFill As Button
    Friend WithEvents dgvStations As DataGridView
    Friend WithEvents SplitContainer3 As SplitContainer
    Friend WithEvents Panel1 As Panel
    Friend WithEvents btnTSRemoveParameter As Button
    Friend WithEvents btnAddTSParameter As Button
    Friend WithEvents dgvToolSetup As DataGridView
    Friend WithEvents GroupBox1 As GroupBox
    Friend WithEvents btnEidtTool As Button
    Friend WithEvents btnRemoveTool As Button
    Friend WithEvents btnAddTool As Button
    Friend WithEvents Label1 As Label
    Friend WithEvents cboToolType As Windows.Forms.ComboBox
    Friend WithEvents txtPreamble As TextBox
    Friend WithEvents Panel3 As Panel
    Friend WithEvents btnCancelToolProps As Button
    Friend WithEvents btnSaveToolProps As Button
    Friend WithEvents Panel2 As Panel
    Friend WithEvents lblCurrentDescription As Label
    Friend WithEvents lblCurrentToolSetup As Label
    Friend WithEvents lblCurrentMachine As Label
    Friend WithEvents propgrdToolProps As PropertyGrid
    Friend WithEvents cmsTreeActions As ContextMenuStrip
    Friend WithEvents AddMachineToolStripMenuItem As ToolStripMenuItem
    Friend WithEvents DeleteMachineToolStripMenuItem As ToolStripMenuItem
    Friend WithEvents DeleteToolSetupToolStripMenuItem As ToolStripMenuItem
    Friend WithEvents AddToolSetupToolStripMenuItem As ToolStripMenuItem
    Friend WithEvents lstTools As ListBox
    Friend WithEvents AddMaterialTypeToolStripMenuItem As ToolStripMenuItem
    Friend WithEvents AddMaterialSizeToolStripMenuItem As ToolStripMenuItem
    Friend WithEvents RemoveMaterialTypeToolStripMenuItem As ToolStripMenuItem
    Friend WithEvents RemoveMaterialSizeToolStripItem As ToolStripMenuItem
End Class
