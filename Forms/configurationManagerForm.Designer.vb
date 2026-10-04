<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class ConfigurationManagerForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(ConfigurationManagerForm))
        Me.SplitContainer1 = New System.Windows.Forms.SplitContainer()
        Me.Panel4 = New System.Windows.Forms.Panel()
        Me.btnConvertCMDB = New System.Windows.Forms.Button()
        Me.TreeViewMachines = New System.Windows.Forms.TreeView()
        Me.tabControlMain = New System.Windows.Forms.TabControl()
        Me.tabMachine = New System.Windows.Forms.TabPage()
        Me.btmRemoveMachineParam = New System.Windows.Forms.Button()
        Me.btnAddMachineParam = New System.Windows.Forms.Button()
        Me.propgrdMachine = New System.Windows.Forms.PropertyGrid()
        Me.tabStations = New System.Windows.Forms.TabPage()
        Me.btnAutoFill = New System.Windows.Forms.Button()
        Me.btnRemoveStation = New System.Windows.Forms.Button()
        Me.bbtnAddStation = New System.Windows.Forms.Button()
        Me.dgvStations = New System.Windows.Forms.DataGridView()
        Me.tabToolSetup = New System.Windows.Forms.TabPage()
        Me.SplitContainer2 = New System.Windows.Forms.SplitContainer()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.btnTSRemoveParameter = New System.Windows.Forms.Button()
        Me.btnAddTSParameter = New System.Windows.Forms.Button()
        Me.dgvToolSetup = New System.Windows.Forms.DataGridView()
        Me.grpboxToolInventory = New System.Windows.Forms.GroupBox()
        Me.btnEidtTool = New System.Windows.Forms.Button()
        Me.btnRemoveTool = New System.Windows.Forms.Button()
        Me.btnAddTool = New System.Windows.Forms.Button()
        Me.lstTools = New System.Windows.Forms.ListBox()
        Me.lblToolTypes = New System.Windows.Forms.Label()
        Me.cboToolType = New System.Windows.Forms.ComboBox()
        Me.tabDefinition = New System.Windows.Forms.TabPage()
        Me.txtPreamble = New System.Windows.Forms.TextBox()
        Me.tabToolProperties = New System.Windows.Forms.TabPage()
        Me.PanelBtnsToolProps = New System.Windows.Forms.Panel()
        Me.btnCancelToolProps = New System.Windows.Forms.Button()
        Me.btnSaveToolProps = New System.Windows.Forms.Button()
        Me.panelToolProps = New System.Windows.Forms.Panel()
        Me.lblCurrentDescription = New System.Windows.Forms.Label()
        Me.lblCurrentToolSetup = New System.Windows.Forms.Label()
        Me.lblCurrentMachine = New System.Windows.Forms.Label()
        Me.propgrdToolProps = New System.Windows.Forms.PropertyGrid()
        Me.tabMaterialInventory = New System.Windows.Forms.TabPage()
        Me.Panel3 = New System.Windows.Forms.Panel()
        Me.propgridMaterial = New System.Windows.Forms.PropertyGrid()
        Me.Panel2 = New System.Windows.Forms.Panel()
        Me.btnRemoveMaterialUDP = New System.Windows.Forms.Button()
        Me.btnMaterialAddUDP = New System.Windows.Forms.Button()
        Me.btnMaterialCancel = New System.Windows.Forms.Button()
        Me.btnSaveMaterial = New System.Windows.Forms.Button()
        Me.tabLayerSetup = New System.Windows.Forms.TabPage()
        Me.pnlLayerSetSaveCancel = New System.Windows.Forms.Panel()
        Me.btnSaveLayerSetp = New System.Windows.Forms.Button()
        Me.propgrdLayerSetup = New System.Windows.Forms.PropertyGrid()
        Me.tabLayer = New System.Windows.Forms.TabPage()
        Me.pnlLayerMapSave = New System.Windows.Forms.Panel()
        Me.btnLayerMapSave = New System.Windows.Forms.Button()
        Me.propgrdLayer = New System.Windows.Forms.PropertyGrid()
        Me.cmsTreeActions = New System.Windows.Forms.ContextMenuStrip(Me.components)
        Me.AddMachineTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.DeleteMachineTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.DeleteToolSetupTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.AddToolSetupTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.RenameToolSetupTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.AddLayerSetupTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.AddLayerTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.DeleteLayerSetpTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.AddMaterialTypeTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.AddMaterialSizeTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.DeleteMaterialTypeTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.DeleteMaterialSizeTSM = New System.Windows.Forms.ToolStripMenuItem()
        Me.DeleteLayerTSM = New System.Windows.Forms.ToolStripMenuItem()
        CType(Me.SplitContainer1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainer1.Panel1.SuspendLayout()
        Me.SplitContainer1.Panel2.SuspendLayout()
        Me.SplitContainer1.SuspendLayout()
        Me.Panel4.SuspendLayout()
        Me.tabControlMain.SuspendLayout()
        Me.tabMachine.SuspendLayout()
        Me.tabStations.SuspendLayout()
        CType(Me.dgvStations, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tabToolSetup.SuspendLayout()
        CType(Me.SplitContainer2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainer2.Panel1.SuspendLayout()
        Me.SplitContainer2.Panel2.SuspendLayout()
        Me.SplitContainer2.SuspendLayout()
        Me.Panel1.SuspendLayout()
        CType(Me.dgvToolSetup, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpboxToolInventory.SuspendLayout()
        Me.tabDefinition.SuspendLayout()
        Me.tabToolProperties.SuspendLayout()
        Me.PanelBtnsToolProps.SuspendLayout()
        Me.panelToolProps.SuspendLayout()
        Me.tabMaterialInventory.SuspendLayout()
        Me.Panel3.SuspendLayout()
        Me.Panel2.SuspendLayout()
        Me.tabLayerSetup.SuspendLayout()
        Me.pnlLayerSetSaveCancel.SuspendLayout()
        Me.tabLayer.SuspendLayout()
        Me.pnlLayerMapSave.SuspendLayout()
        Me.cmsTreeActions.SuspendLayout()
        Me.SuspendLayout()
        '
        'SplitContainer1
        '
        Me.SplitContainer1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainer1.Location = New System.Drawing.Point(0, 0)
        Me.SplitContainer1.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.SplitContainer1.Name = "SplitContainer1"
        '
        'SplitContainer1.Panel1
        '
        Me.SplitContainer1.Panel1.AutoScroll = True
        Me.SplitContainer1.Panel1.Controls.Add(Me.Panel4)
        Me.SplitContainer1.Panel1.Controls.Add(Me.TreeViewMachines)
        Me.SplitContainer1.Panel1.Margin = New System.Windows.Forms.Padding(3)
        '
        'SplitContainer1.Panel2
        '
        Me.SplitContainer1.Panel2.Controls.Add(Me.tabControlMain)
        Me.SplitContainer1.Size = New System.Drawing.Size(976, 531)
        Me.SplitContainer1.SplitterDistance = 236
        Me.SplitContainer1.SplitterWidth = 3
        Me.SplitContainer1.TabIndex = 0
        '
        'Panel4
        '
        Me.Panel4.Controls.Add(Me.btnConvertCMDB)
        Me.Panel4.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.Panel4.Location = New System.Drawing.Point(0, 476)
        Me.Panel4.Name = "Panel4"
        Me.Panel4.Size = New System.Drawing.Size(236, 55)
        Me.Panel4.TabIndex = 2
        '
        'btnConvertCMDB
        '
        Me.btnConvertCMDB.AutoSize = True
        Me.btnConvertCMDB.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnConvertCMDB.Location = New System.Drawing.Point(34, 14)
        Me.btnConvertCMDB.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnConvertCMDB.Name = "btnConvertCMDB"
        Me.btnConvertCMDB.Size = New System.Drawing.Size(117, 27)
        Me.btnConvertCMDB.TabIndex = 2
        Me.btnConvertCMDB.Text = "Convert CMDB"
        Me.btnConvertCMDB.UseVisualStyleBackColor = True
        '
        'TreeViewMachines
        '
        Me.TreeViewMachines.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TreeViewMachines.Location = New System.Drawing.Point(0, 0)
        Me.TreeViewMachines.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.TreeViewMachines.Name = "TreeViewMachines"
        Me.TreeViewMachines.Size = New System.Drawing.Size(236, 531)
        Me.TreeViewMachines.TabIndex = 1
        '
        'tabControlMain
        '
        Me.tabControlMain.Controls.Add(Me.tabMachine)
        Me.tabControlMain.Controls.Add(Me.tabStations)
        Me.tabControlMain.Controls.Add(Me.tabToolSetup)
        Me.tabControlMain.Controls.Add(Me.tabDefinition)
        Me.tabControlMain.Controls.Add(Me.tabToolProperties)
        Me.tabControlMain.Controls.Add(Me.tabMaterialInventory)
        Me.tabControlMain.Controls.Add(Me.tabLayerSetup)
        Me.tabControlMain.Controls.Add(Me.tabLayer)
        Me.tabControlMain.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tabControlMain.Location = New System.Drawing.Point(0, 0)
        Me.tabControlMain.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabControlMain.Name = "tabControlMain"
        Me.tabControlMain.SelectedIndex = 0
        Me.tabControlMain.Size = New System.Drawing.Size(737, 531)
        Me.tabControlMain.TabIndex = 2
        '
        'tabMachine
        '
        Me.tabMachine.Controls.Add(Me.btmRemoveMachineParam)
        Me.tabMachine.Controls.Add(Me.btnAddMachineParam)
        Me.tabMachine.Controls.Add(Me.propgrdMachine)
        Me.tabMachine.Location = New System.Drawing.Point(4, 22)
        Me.tabMachine.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabMachine.Name = "tabMachine"
        Me.tabMachine.Padding = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabMachine.Size = New System.Drawing.Size(729, 505)
        Me.tabMachine.TabIndex = 0
        Me.tabMachine.Text = "Machine"
        Me.tabMachine.UseVisualStyleBackColor = True
        '
        'btmRemoveMachineParam
        '
        Me.btmRemoveMachineParam.AutoSize = True
        Me.btmRemoveMachineParam.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btmRemoveMachineParam.Location = New System.Drawing.Point(210, 468)
        Me.btmRemoveMachineParam.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btmRemoveMachineParam.Name = "btmRemoveMachineParam"
        Me.btmRemoveMachineParam.Size = New System.Drawing.Size(140, 27)
        Me.btmRemoveMachineParam.TabIndex = 2
        Me.btmRemoveMachineParam.Text = "Remove Parameter"
        Me.btmRemoveMachineParam.UseVisualStyleBackColor = True
        '
        'btnAddMachineParam
        '
        Me.btnAddMachineParam.AutoSize = True
        Me.btnAddMachineParam.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnAddMachineParam.Location = New System.Drawing.Point(20, 468)
        Me.btnAddMachineParam.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnAddMachineParam.Name = "btnAddMachineParam"
        Me.btnAddMachineParam.Size = New System.Drawing.Size(117, 27)
        Me.btnAddMachineParam.TabIndex = 1
        Me.btnAddMachineParam.Text = "Add Parameter"
        Me.btnAddMachineParam.UseVisualStyleBackColor = True
        '
        'propgrdMachine
        '
        Me.propgrdMachine.BackColor = System.Drawing.SystemColors.Control
        Me.propgrdMachine.Dock = System.Windows.Forms.DockStyle.Top
        Me.propgrdMachine.Location = New System.Drawing.Point(3, 2)
        Me.propgrdMachine.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.propgrdMachine.Name = "propgrdMachine"
        Me.propgrdMachine.Size = New System.Drawing.Size(723, 450)
        Me.propgrdMachine.TabIndex = 0
        '
        'tabStations
        '
        Me.tabStations.Controls.Add(Me.btnAutoFill)
        Me.tabStations.Controls.Add(Me.btnRemoveStation)
        Me.tabStations.Controls.Add(Me.bbtnAddStation)
        Me.tabStations.Controls.Add(Me.dgvStations)
        Me.tabStations.Location = New System.Drawing.Point(4, 22)
        Me.tabStations.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabStations.Name = "tabStations"
        Me.tabStations.Padding = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabStations.Size = New System.Drawing.Size(729, 505)
        Me.tabStations.TabIndex = 1
        Me.tabStations.Text = "Stations"
        Me.tabStations.UseVisualStyleBackColor = True
        '
        'btnAutoFill
        '
        Me.btnAutoFill.AutoSize = True
        Me.btnAutoFill.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnAutoFill.Location = New System.Drawing.Point(59, 454)
        Me.btnAutoFill.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnAutoFill.Name = "btnAutoFill"
        Me.btnAutoFill.Size = New System.Drawing.Size(123, 27)
        Me.btnAutoFill.TabIndex = 3
        Me.btnAutoFill.Text = "Auto Fill Stations"
        Me.btnAutoFill.UseVisualStyleBackColor = True
        '
        'btnRemoveStation
        '
        Me.btnRemoveStation.AutoSize = True
        Me.btnRemoveStation.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnRemoveStation.Location = New System.Drawing.Point(543, 454)
        Me.btnRemoveStation.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnRemoveStation.Name = "btnRemoveStation"
        Me.btnRemoveStation.Size = New System.Drawing.Size(118, 27)
        Me.btnRemoveStation.TabIndex = 2
        Me.btnRemoveStation.Text = "Remove Station"
        Me.btnRemoveStation.UseVisualStyleBackColor = True
        '
        'bbtnAddStation
        '
        Me.bbtnAddStation.AutoSize = True
        Me.bbtnAddStation.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.bbtnAddStation.Location = New System.Drawing.Point(432, 454)
        Me.bbtnAddStation.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.bbtnAddStation.Name = "bbtnAddStation"
        Me.bbtnAddStation.Size = New System.Drawing.Size(105, 27)
        Me.bbtnAddStation.TabIndex = 1
        Me.bbtnAddStation.Text = "Add Station"
        Me.bbtnAddStation.UseVisualStyleBackColor = True
        '
        'dgvStations
        '
        Me.dgvStations.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvStations.Dock = System.Windows.Forms.DockStyle.Top
        Me.dgvStations.Location = New System.Drawing.Point(3, 2)
        Me.dgvStations.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.dgvStations.Name = "dgvStations"
        Me.dgvStations.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect
        Me.dgvStations.Size = New System.Drawing.Size(723, 398)
        Me.dgvStations.TabIndex = 0
        '
        'tabToolSetup
        '
        Me.tabToolSetup.Controls.Add(Me.SplitContainer2)
        Me.tabToolSetup.Location = New System.Drawing.Point(4, 22)
        Me.tabToolSetup.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabToolSetup.Name = "tabToolSetup"
        Me.tabToolSetup.Size = New System.Drawing.Size(729, 505)
        Me.tabToolSetup.TabIndex = 2
        Me.tabToolSetup.Text = "ToolSetups"
        Me.tabToolSetup.UseVisualStyleBackColor = True
        '
        'SplitContainer2
        '
        Me.SplitContainer2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SplitContainer2.Location = New System.Drawing.Point(0, 0)
        Me.SplitContainer2.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.SplitContainer2.Name = "SplitContainer2"
        '
        'SplitContainer2.Panel1
        '
        Me.SplitContainer2.Panel1.Controls.Add(Me.Panel1)
        Me.SplitContainer2.Panel1.Controls.Add(Me.dgvToolSetup)
        '
        'SplitContainer2.Panel2
        '
        Me.SplitContainer2.Panel2.Controls.Add(Me.grpboxToolInventory)
        Me.SplitContainer2.Size = New System.Drawing.Size(729, 505)
        Me.SplitContainer2.SplitterDistance = 410
        Me.SplitContainer2.SplitterWidth = 3
        Me.SplitContainer2.TabIndex = 6
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.btnTSRemoveParameter)
        Me.Panel1.Controls.Add(Me.btnAddTSParameter)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.Panel1.Location = New System.Drawing.Point(0, 461)
        Me.Panel1.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(410, 44)
        Me.Panel1.TabIndex = 4
        '
        'btnTSRemoveParameter
        '
        Me.btnTSRemoveParameter.AutoSize = True
        Me.btnTSRemoveParameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnTSRemoveParameter.Location = New System.Drawing.Point(249, 11)
        Me.btnTSRemoveParameter.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnTSRemoveParameter.Name = "btnTSRemoveParameter"
        Me.btnTSRemoveParameter.Size = New System.Drawing.Size(140, 27)
        Me.btnTSRemoveParameter.TabIndex = 5
        Me.btnTSRemoveParameter.Text = "Remove Parameter"
        Me.btnTSRemoveParameter.UseVisualStyleBackColor = True
        '
        'btnAddTSParameter
        '
        Me.btnAddTSParameter.AutoSize = True
        Me.btnAddTSParameter.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnAddTSParameter.Location = New System.Drawing.Point(54, 11)
        Me.btnAddTSParameter.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnAddTSParameter.Name = "btnAddTSParameter"
        Me.btnAddTSParameter.Size = New System.Drawing.Size(113, 27)
        Me.btnAddTSParameter.TabIndex = 4
        Me.btnAddTSParameter.Text = "Add Parameter"
        Me.btnAddTSParameter.UseVisualStyleBackColor = True
        '
        'dgvToolSetup
        '
        Me.dgvToolSetup.AllowUserToOrderColumns = True
        Me.dgvToolSetup.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvToolSetup.Dock = System.Windows.Forms.DockStyle.Top
        Me.dgvToolSetup.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnEnter
        Me.dgvToolSetup.Location = New System.Drawing.Point(0, 0)
        Me.dgvToolSetup.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.dgvToolSetup.Name = "dgvToolSetup"
        Me.dgvToolSetup.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect
        Me.dgvToolSetup.Size = New System.Drawing.Size(410, 460)
        Me.dgvToolSetup.TabIndex = 3
        '
        'grpboxToolInventory
        '
        Me.grpboxToolInventory.Controls.Add(Me.btnEidtTool)
        Me.grpboxToolInventory.Controls.Add(Me.btnRemoveTool)
        Me.grpboxToolInventory.Controls.Add(Me.btnAddTool)
        Me.grpboxToolInventory.Controls.Add(Me.lstTools)
        Me.grpboxToolInventory.Controls.Add(Me.lblToolTypes)
        Me.grpboxToolInventory.Controls.Add(Me.cboToolType)
        Me.grpboxToolInventory.Dock = System.Windows.Forms.DockStyle.Fill
        Me.grpboxToolInventory.Location = New System.Drawing.Point(0, 0)
        Me.grpboxToolInventory.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.grpboxToolInventory.Name = "grpboxToolInventory"
        Me.grpboxToolInventory.Padding = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.grpboxToolInventory.Size = New System.Drawing.Size(316, 505)
        Me.grpboxToolInventory.TabIndex = 4
        Me.grpboxToolInventory.TabStop = False
        Me.grpboxToolInventory.Text = "Tool Inventory"
        '
        'btnEidtTool
        '
        Me.btnEidtTool.AutoSize = True
        Me.btnEidtTool.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnEidtTool.Location = New System.Drawing.Point(217, 476)
        Me.btnEidtTool.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnEidtTool.Name = "btnEidtTool"
        Me.btnEidtTool.Size = New System.Drawing.Size(82, 27)
        Me.btnEidtTool.TabIndex = 5
        Me.btnEidtTool.Text = "Edit Tool"
        Me.btnEidtTool.UseVisualStyleBackColor = True
        '
        'btnRemoveTool
        '
        Me.btnRemoveTool.AutoSize = True
        Me.btnRemoveTool.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnRemoveTool.Location = New System.Drawing.Point(109, 476)
        Me.btnRemoveTool.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnRemoveTool.Name = "btnRemoveTool"
        Me.btnRemoveTool.Size = New System.Drawing.Size(102, 27)
        Me.btnRemoveTool.TabIndex = 4
        Me.btnRemoveTool.Text = "Remove Tool"
        Me.btnRemoveTool.UseVisualStyleBackColor = True
        '
        'btnAddTool
        '
        Me.btnAddTool.AutoSize = True
        Me.btnAddTool.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnAddTool.Location = New System.Drawing.Point(21, 476)
        Me.btnAddTool.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnAddTool.Name = "btnAddTool"
        Me.btnAddTool.Size = New System.Drawing.Size(82, 27)
        Me.btnAddTool.TabIndex = 3
        Me.btnAddTool.Text = "Add Tool"
        Me.btnAddTool.UseVisualStyleBackColor = True
        '
        'lstTools
        '
        Me.lstTools.BackColor = System.Drawing.Color.White
        Me.lstTools.FormattingEnabled = True
        Me.lstTools.Location = New System.Drawing.Point(21, 57)
        Me.lstTools.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.lstTools.Name = "lstTools"
        Me.lstTools.Size = New System.Drawing.Size(278, 394)
        Me.lstTools.TabIndex = 2
        '
        'lblToolTypes
        '
        Me.lblToolTypes.AutoSize = True
        Me.lblToolTypes.Location = New System.Drawing.Point(106, 15)
        Me.lblToolTypes.Name = "lblToolTypes"
        Me.lblToolTypes.Size = New System.Drawing.Size(60, 13)
        Me.lblToolTypes.TabIndex = 1
        Me.lblToolTypes.Text = "Tool Types"
        '
        'cboToolType
        '
        Me.cboToolType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboToolType.FormattingEnabled = True
        Me.cboToolType.Location = New System.Drawing.Point(25, 31)
        Me.cboToolType.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.cboToolType.Name = "cboToolType"
        Me.cboToolType.Size = New System.Drawing.Size(257, 21)
        Me.cboToolType.Sorted = True
        Me.cboToolType.TabIndex = 0
        '
        'tabDefinition
        '
        Me.tabDefinition.Controls.Add(Me.txtPreamble)
        Me.tabDefinition.Location = New System.Drawing.Point(4, 22)
        Me.tabDefinition.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabDefinition.Name = "tabDefinition"
        Me.tabDefinition.Padding = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabDefinition.Size = New System.Drawing.Size(729, 505)
        Me.tabDefinition.TabIndex = 3
        Me.tabDefinition.Text = "Definition"
        Me.tabDefinition.UseVisualStyleBackColor = True
        '
        'txtPreamble
        '
        Me.txtPreamble.Dock = System.Windows.Forms.DockStyle.Fill
        Me.txtPreamble.Location = New System.Drawing.Point(3, 2)
        Me.txtPreamble.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.txtPreamble.Multiline = True
        Me.txtPreamble.Name = "txtPreamble"
        Me.txtPreamble.Size = New System.Drawing.Size(723, 501)
        Me.txtPreamble.TabIndex = 0
        Me.txtPreamble.Text = "This is where we would put some guidance for the user. this text will change base" &
    "d on with node is highlighted."
        '
        'tabToolProperties
        '
        Me.tabToolProperties.Controls.Add(Me.PanelBtnsToolProps)
        Me.tabToolProperties.Controls.Add(Me.panelToolProps)
        Me.tabToolProperties.Controls.Add(Me.propgrdToolProps)
        Me.tabToolProperties.Location = New System.Drawing.Point(4, 22)
        Me.tabToolProperties.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabToolProperties.Name = "tabToolProperties"
        Me.tabToolProperties.Padding = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.tabToolProperties.Size = New System.Drawing.Size(729, 505)
        Me.tabToolProperties.TabIndex = 4
        Me.tabToolProperties.Text = "Tool Properties"
        Me.tabToolProperties.UseVisualStyleBackColor = True
        '
        'PanelBtnsToolProps
        '
        Me.PanelBtnsToolProps.Controls.Add(Me.btnCancelToolProps)
        Me.PanelBtnsToolProps.Controls.Add(Me.btnSaveToolProps)
        Me.PanelBtnsToolProps.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.PanelBtnsToolProps.Location = New System.Drawing.Point(222, 427)
        Me.PanelBtnsToolProps.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.PanelBtnsToolProps.Name = "PanelBtnsToolProps"
        Me.PanelBtnsToolProps.Size = New System.Drawing.Size(504, 76)
        Me.PanelBtnsToolProps.TabIndex = 4
        '
        'btnCancelToolProps
        '
        Me.btnCancelToolProps.AutoSize = True
        Me.btnCancelToolProps.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnCancelToolProps.Location = New System.Drawing.Point(107, 30)
        Me.btnCancelToolProps.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnCancelToolProps.Name = "btnCancelToolProps"
        Me.btnCancelToolProps.Size = New System.Drawing.Size(64, 27)
        Me.btnCancelToolProps.TabIndex = 5
        Me.btnCancelToolProps.Text = "Cancel"
        Me.btnCancelToolProps.UseVisualStyleBackColor = True
        '
        'btnSaveToolProps
        '
        Me.btnSaveToolProps.AutoSize = True
        Me.btnSaveToolProps.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnSaveToolProps.Location = New System.Drawing.Point(19, 30)
        Me.btnSaveToolProps.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.btnSaveToolProps.Name = "btnSaveToolProps"
        Me.btnSaveToolProps.Size = New System.Drawing.Size(82, 27)
        Me.btnSaveToolProps.TabIndex = 4
        Me.btnSaveToolProps.Text = "Save Tool"
        Me.btnSaveToolProps.UseVisualStyleBackColor = True
        '
        'panelToolProps
        '
        Me.panelToolProps.Controls.Add(Me.lblCurrentDescription)
        Me.panelToolProps.Controls.Add(Me.lblCurrentToolSetup)
        Me.panelToolProps.Controls.Add(Me.lblCurrentMachine)
        Me.panelToolProps.Dock = System.Windows.Forms.DockStyle.Top
        Me.panelToolProps.Location = New System.Drawing.Point(222, 2)
        Me.panelToolProps.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.panelToolProps.Name = "panelToolProps"
        Me.panelToolProps.Size = New System.Drawing.Size(504, 98)
        Me.panelToolProps.TabIndex = 1
        '
        'lblCurrentDescription
        '
        Me.lblCurrentDescription.AutoSize = True
        Me.lblCurrentDescription.Location = New System.Drawing.Point(22, 64)
        Me.lblCurrentDescription.Name = "lblCurrentDescription"
        Me.lblCurrentDescription.Size = New System.Drawing.Size(104, 13)
        Me.lblCurrentDescription.TabIndex = 2
        Me.lblCurrentDescription.Text = "lblCurrentDescription"
        '
        'lblCurrentToolSetup
        '
        Me.lblCurrentToolSetup.AutoSize = True
        Me.lblCurrentToolSetup.Location = New System.Drawing.Point(22, 41)
        Me.lblCurrentToolSetup.Name = "lblCurrentToolSetup"
        Me.lblCurrentToolSetup.Size = New System.Drawing.Size(100, 13)
        Me.lblCurrentToolSetup.TabIndex = 1
        Me.lblCurrentToolSetup.Text = "lblCurrentToolSetup"
        '
        'lblCurrentMachine
        '
        Me.lblCurrentMachine.AutoSize = True
        Me.lblCurrentMachine.Location = New System.Drawing.Point(22, 15)
        Me.lblCurrentMachine.Name = "lblCurrentMachine"
        Me.lblCurrentMachine.Size = New System.Drawing.Size(92, 13)
        Me.lblCurrentMachine.TabIndex = 0
        Me.lblCurrentMachine.Text = "lblCurrentMachine"
        '
        'propgrdToolProps
        '
        Me.propgrdToolProps.BackColor = System.Drawing.SystemColors.Control
        Me.propgrdToolProps.Dock = System.Windows.Forms.DockStyle.Left
        Me.propgrdToolProps.Location = New System.Drawing.Point(3, 2)
        Me.propgrdToolProps.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.propgrdToolProps.Name = "propgrdToolProps"
        Me.propgrdToolProps.Size = New System.Drawing.Size(219, 501)
        Me.propgrdToolProps.TabIndex = 0
        '
        'tabMaterialInventory
        '
        Me.tabMaterialInventory.Controls.Add(Me.Panel3)
        Me.tabMaterialInventory.Controls.Add(Me.Panel2)
        Me.tabMaterialInventory.Location = New System.Drawing.Point(4, 22)
        Me.tabMaterialInventory.Name = "tabMaterialInventory"
        Me.tabMaterialInventory.Padding = New System.Windows.Forms.Padding(3)
        Me.tabMaterialInventory.Size = New System.Drawing.Size(729, 505)
        Me.tabMaterialInventory.TabIndex = 5
        Me.tabMaterialInventory.Text = "Material Inventory"
        Me.tabMaterialInventory.UseVisualStyleBackColor = True
        '
        'Panel3
        '
        Me.Panel3.Controls.Add(Me.propgridMaterial)
        Me.Panel3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel3.Location = New System.Drawing.Point(3, 3)
        Me.Panel3.Name = "Panel3"
        Me.Panel3.Size = New System.Drawing.Size(723, 449)
        Me.Panel3.TabIndex = 1
        '
        'propgridMaterial
        '
        Me.propgridMaterial.Location = New System.Drawing.Point(0, 0)
        Me.propgridMaterial.Name = "propgridMaterial"
        Me.propgridMaterial.Size = New System.Drawing.Size(556, 285)
        Me.propgridMaterial.TabIndex = 0
        '
        'Panel2
        '
        Me.Panel2.Controls.Add(Me.btnRemoveMaterialUDP)
        Me.Panel2.Controls.Add(Me.btnMaterialAddUDP)
        Me.Panel2.Controls.Add(Me.btnMaterialCancel)
        Me.Panel2.Controls.Add(Me.btnSaveMaterial)
        Me.Panel2.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.Panel2.Location = New System.Drawing.Point(3, 452)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(723, 50)
        Me.Panel2.TabIndex = 0
        '
        'btnRemoveMaterialUDP
        '
        Me.btnRemoveMaterialUDP.AutoSize = True
        Me.btnRemoveMaterialUDP.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnRemoveMaterialUDP.Location = New System.Drawing.Point(137, 6)
        Me.btnRemoveMaterialUDP.Name = "btnRemoveMaterialUDP"
        Me.btnRemoveMaterialUDP.Size = New System.Drawing.Size(140, 27)
        Me.btnRemoveMaterialUDP.TabIndex = 3
        Me.btnRemoveMaterialUDP.Text = "Remove Parameter"
        Me.btnRemoveMaterialUDP.UseVisualStyleBackColor = True
        '
        'btnMaterialAddUDP
        '
        Me.btnMaterialAddUDP.AutoSize = True
        Me.btnMaterialAddUDP.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnMaterialAddUDP.Location = New System.Drawing.Point(18, 6)
        Me.btnMaterialAddUDP.Name = "btnMaterialAddUDP"
        Me.btnMaterialAddUDP.Size = New System.Drawing.Size(113, 27)
        Me.btnMaterialAddUDP.TabIndex = 2
        Me.btnMaterialAddUDP.Text = "Add Parameter"
        Me.btnMaterialAddUDP.UseVisualStyleBackColor = True
        '
        'btnMaterialCancel
        '
        Me.btnMaterialCancel.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnMaterialCancel.Location = New System.Drawing.Point(612, 6)
        Me.btnMaterialCancel.Name = "btnMaterialCancel"
        Me.btnMaterialCancel.Size = New System.Drawing.Size(104, 27)
        Me.btnMaterialCancel.TabIndex = 1
        Me.btnMaterialCancel.Text = "Cancel"
        Me.btnMaterialCancel.UseVisualStyleBackColor = True
        '
        'btnSaveMaterial
        '
        Me.btnSaveMaterial.AutoSize = True
        Me.btnSaveMaterial.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnSaveMaterial.Location = New System.Drawing.Point(502, 6)
        Me.btnSaveMaterial.Name = "btnSaveMaterial"
        Me.btnSaveMaterial.Size = New System.Drawing.Size(104, 27)
        Me.btnSaveMaterial.TabIndex = 0
        Me.btnSaveMaterial.Text = "Save Material"
        Me.btnSaveMaterial.UseVisualStyleBackColor = True
        '
        'tabLayerSetup
        '
        Me.tabLayerSetup.Controls.Add(Me.pnlLayerSetSaveCancel)
        Me.tabLayerSetup.Controls.Add(Me.propgrdLayerSetup)
        Me.tabLayerSetup.Location = New System.Drawing.Point(4, 22)
        Me.tabLayerSetup.Name = "tabLayerSetup"
        Me.tabLayerSetup.Padding = New System.Windows.Forms.Padding(3)
        Me.tabLayerSetup.Size = New System.Drawing.Size(729, 505)
        Me.tabLayerSetup.TabIndex = 6
        Me.tabLayerSetup.Text = "Layer Setup"
        Me.tabLayerSetup.UseVisualStyleBackColor = True
        '
        'pnlLayerSetSaveCancel
        '
        Me.pnlLayerSetSaveCancel.Controls.Add(Me.btnSaveLayerSetp)
        Me.pnlLayerSetSaveCancel.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.pnlLayerSetSaveCancel.Location = New System.Drawing.Point(3, 447)
        Me.pnlLayerSetSaveCancel.Name = "pnlLayerSetSaveCancel"
        Me.pnlLayerSetSaveCancel.Size = New System.Drawing.Size(723, 55)
        Me.pnlLayerSetSaveCancel.TabIndex = 1
        '
        'btnSaveLayerSetp
        '
        Me.btnSaveLayerSetp.Location = New System.Drawing.Point(624, 21)
        Me.btnSaveLayerSetp.Name = "btnSaveLayerSetp"
        Me.btnSaveLayerSetp.Size = New System.Drawing.Size(75, 23)
        Me.btnSaveLayerSetp.TabIndex = 0
        Me.btnSaveLayerSetp.Text = "Save"
        Me.btnSaveLayerSetp.UseVisualStyleBackColor = True
        '
        'propgrdLayerSetup
        '
        Me.propgrdLayerSetup.Dock = System.Windows.Forms.DockStyle.Top
        Me.propgrdLayerSetup.Location = New System.Drawing.Point(3, 3)
        Me.propgrdLayerSetup.Name = "propgrdLayerSetup"
        Me.propgrdLayerSetup.Size = New System.Drawing.Size(723, 438)
        Me.propgrdLayerSetup.TabIndex = 0
        '
        'tabLayer
        '
        Me.tabLayer.Controls.Add(Me.pnlLayerMapSave)
        Me.tabLayer.Controls.Add(Me.propgrdLayer)
        Me.tabLayer.Location = New System.Drawing.Point(4, 22)
        Me.tabLayer.Name = "tabLayer"
        Me.tabLayer.Padding = New System.Windows.Forms.Padding(3)
        Me.tabLayer.Size = New System.Drawing.Size(729, 505)
        Me.tabLayer.TabIndex = 7
        Me.tabLayer.Text = "Layer Map"
        Me.tabLayer.UseVisualStyleBackColor = True
        '
        'pnlLayerMapSave
        '
        Me.pnlLayerMapSave.Controls.Add(Me.btnLayerMapSave)
        Me.pnlLayerMapSave.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.pnlLayerMapSave.Location = New System.Drawing.Point(3, 451)
        Me.pnlLayerMapSave.Name = "pnlLayerMapSave"
        Me.pnlLayerMapSave.Size = New System.Drawing.Size(723, 51)
        Me.pnlLayerMapSave.TabIndex = 1
        '
        'btnLayerMapSave
        '
        Me.btnLayerMapSave.Location = New System.Drawing.Point(619, 17)
        Me.btnLayerMapSave.Name = "btnLayerMapSave"
        Me.btnLayerMapSave.Size = New System.Drawing.Size(75, 23)
        Me.btnLayerMapSave.TabIndex = 0
        Me.btnLayerMapSave.Text = "Save"
        Me.btnLayerMapSave.UseVisualStyleBackColor = True
        '
        'propgrdLayer
        '
        Me.propgrdLayer.Location = New System.Drawing.Point(3, 3)
        Me.propgrdLayer.Name = "propgrdLayer"
        Me.propgrdLayer.Size = New System.Drawing.Size(733, 445)
        Me.propgrdLayer.TabIndex = 0
        '
        'cmsTreeActions
        '
        Me.cmsTreeActions.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.AddMachineTSM, Me.DeleteMachineTSM, Me.DeleteToolSetupTSM, Me.AddToolSetupTSM, Me.RenameToolSetupTSM, Me.AddLayerSetupTSM, Me.AddLayerTSM, Me.DeleteLayerSetpTSM, Me.AddMaterialTypeTSM, Me.AddMaterialSizeTSM, Me.DeleteMaterialTypeTSM, Me.DeleteMaterialSizeTSM, Me.DeleteLayerTSM})
        Me.cmsTreeActions.Name = "ContextMenuStrip1"
        Me.cmsTreeActions.Size = New System.Drawing.Size(197, 290)
        '
        'AddMachineTSM
        '
        Me.AddMachineTSM.Name = "AddMachineTSM"
        Me.AddMachineTSM.Size = New System.Drawing.Size(196, 22)
        Me.AddMachineTSM.Text = "Add Machine"
        '
        'DeleteMachineTSM
        '
        Me.DeleteMachineTSM.Name = "DeleteMachineTSM"
        Me.DeleteMachineTSM.Size = New System.Drawing.Size(196, 22)
        Me.DeleteMachineTSM.Text = "Delete Machine"
        '
        'DeleteToolSetupTSM
        '
        Me.DeleteToolSetupTSM.Name = "DeleteToolSetupTSM"
        Me.DeleteToolSetupTSM.Size = New System.Drawing.Size(196, 22)
        Me.DeleteToolSetupTSM.Text = "Delete Tool Setup"
        '
        'AddToolSetupTSM
        '
        Me.AddToolSetupTSM.Name = "AddToolSetupTSM"
        Me.AddToolSetupTSM.Size = New System.Drawing.Size(196, 22)
        Me.AddToolSetupTSM.Text = "Add Tool Setup"
        '
        'RenameToolSetupTSM
        '
        Me.RenameToolSetupTSM.Name = "RenameToolSetupTSM"
        Me.RenameToolSetupTSM.Size = New System.Drawing.Size(196, 22)
        Me.RenameToolSetupTSM.Text = "Rename Tool Setup"
        '
        'AddLayerSetupTSM
        '
        Me.AddLayerSetupTSM.Name = "AddLayerSetupTSM"
        Me.AddLayerSetupTSM.Size = New System.Drawing.Size(196, 22)
        Me.AddLayerSetupTSM.Text = "Add Layer Setup"
        '
        'AddLayerTSM
        '
        Me.AddLayerTSM.Name = "AddLayerTSM"
        Me.AddLayerTSM.Size = New System.Drawing.Size(196, 22)
        Me.AddLayerTSM.Text = "Add Layer"
        '
        'DeleteLayerSetpTSM
        '
        Me.DeleteLayerSetpTSM.Name = "DeleteLayerSetpTSM"
        Me.DeleteLayerSetpTSM.Size = New System.Drawing.Size(196, 22)
        Me.DeleteLayerSetpTSM.Text = "Delete Layer Setup"
        '
        'AddMaterialTypeTSM
        '
        Me.AddMaterialTypeTSM.Name = "AddMaterialTypeTSM"
        Me.AddMaterialTypeTSM.Size = New System.Drawing.Size(196, 22)
        Me.AddMaterialTypeTSM.Text = "Add Material Type"
        '
        'AddMaterialSizeTSM
        '
        Me.AddMaterialSizeTSM.Name = "AddMaterialSizeTSM"
        Me.AddMaterialSizeTSM.Size = New System.Drawing.Size(196, 22)
        Me.AddMaterialSizeTSM.Text = "Add Material Size"
        '
        'DeleteMaterialTypeTSM
        '
        Me.DeleteMaterialTypeTSM.Name = "DeleteMaterialTypeTSM"
        Me.DeleteMaterialTypeTSM.Size = New System.Drawing.Size(196, 22)
        Me.DeleteMaterialTypeTSM.Text = "Delete Material Type"
        '
        'DeleteMaterialSizeTSM
        '
        Me.DeleteMaterialSizeTSM.Name = "DeleteMaterialSizeTSM"
        Me.DeleteMaterialSizeTSM.Size = New System.Drawing.Size(196, 22)
        Me.DeleteMaterialSizeTSM.Text = "Delete Material Size"
        '
        'DeleteLayerTSM
        '
        Me.DeleteLayerTSM.Name = "DeleteLayerTSM"
        Me.DeleteLayerTSM.Size = New System.Drawing.Size(196, 22)
        Me.DeleteLayerTSM.Text = "Delete Layer"
        '
        'ConfigurationManagerForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(976, 531)
        Me.Controls.Add(Me.SplitContainer1)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Margin = New System.Windows.Forms.Padding(3, 2, 3, 2)
        Me.Name = "ConfigurationManagerForm"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Formtest"
        Me.SplitContainer1.Panel1.ResumeLayout(False)
        Me.SplitContainer1.Panel2.ResumeLayout(False)
        CType(Me.SplitContainer1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer1.ResumeLayout(False)
        Me.Panel4.ResumeLayout(False)
        Me.Panel4.PerformLayout()
        Me.tabControlMain.ResumeLayout(False)
        Me.tabMachine.ResumeLayout(False)
        Me.tabMachine.PerformLayout()
        Me.tabStations.ResumeLayout(False)
        Me.tabStations.PerformLayout()
        CType(Me.dgvStations, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tabToolSetup.ResumeLayout(False)
        Me.SplitContainer2.Panel1.ResumeLayout(False)
        Me.SplitContainer2.Panel2.ResumeLayout(False)
        CType(Me.SplitContainer2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer2.ResumeLayout(False)
        Me.Panel1.ResumeLayout(False)
        Me.Panel1.PerformLayout()
        CType(Me.dgvToolSetup, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpboxToolInventory.ResumeLayout(False)
        Me.grpboxToolInventory.PerformLayout()
        Me.tabDefinition.ResumeLayout(False)
        Me.tabDefinition.PerformLayout()
        Me.tabToolProperties.ResumeLayout(False)
        Me.PanelBtnsToolProps.ResumeLayout(False)
        Me.PanelBtnsToolProps.PerformLayout()
        Me.panelToolProps.ResumeLayout(False)
        Me.panelToolProps.PerformLayout()
        Me.tabMaterialInventory.ResumeLayout(False)
        Me.Panel3.ResumeLayout(False)
        Me.Panel2.ResumeLayout(False)
        Me.Panel2.PerformLayout()
        Me.tabLayerSetup.ResumeLayout(False)
        Me.pnlLayerSetSaveCancel.ResumeLayout(False)
        Me.tabLayer.ResumeLayout(False)
        Me.pnlLayerMapSave.ResumeLayout(False)
        Me.cmsTreeActions.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents SplitContainer1 As SplitContainer
    Friend WithEvents TreeViewMachines As TreeView
    Friend WithEvents tabControlMain As TabControl
    Friend WithEvents tabMachine As TabPage
    Friend WithEvents btmRemoveMachineParam As Button
    Friend WithEvents btnAddMachineParam As Button
    Friend WithEvents tabStations As TabPage
    Friend WithEvents btnAutoFill As Button
    Friend WithEvents btnRemoveStation As Button
    Friend WithEvents bbtnAddStation As Button
    Friend WithEvents dgvStations As DataGridView
    Friend WithEvents tabToolSetup As TabPage
    Friend WithEvents tabDefinition As TabPage
    Friend WithEvents txtPreamble As TextBox
    Friend WithEvents tabToolProperties As TabPage
    Friend WithEvents panelToolProps As Panel
    Friend WithEvents propgrdToolProps As PropertyGrid
    Friend WithEvents lblCurrentToolSetup As Label
    Friend WithEvents lblCurrentMachine As Label
    Friend WithEvents PanelBtnsToolProps As Panel
    Friend WithEvents btnCancelToolProps As Button
    Friend WithEvents btnSaveToolProps As Button
    Friend WithEvents AddMachineTSM As ToolStripMenuItem
    Friend WithEvents DeleteMachineTSM As ToolStripMenuItem
    Friend WithEvents DeleteToolSetupTSM As ToolStripMenuItem
    Friend WithEvents SplitContainer2 As SplitContainer
    Friend WithEvents Panel1 As Panel
    Friend WithEvents btnTSRemoveParameter As Button
    Friend WithEvents btnAddTSParameter As Button
    Friend WithEvents dgvToolSetup As DataGridView
    Friend WithEvents grpboxToolInventory As GroupBox
    Friend WithEvents btnEidtTool As Button
    Friend WithEvents btnRemoveTool As Button
    Friend WithEvents btnAddTool As Button
    Friend WithEvents lstTools As ListBox
    Friend WithEvents lblToolTypes As Label
    Friend WithEvents cboToolType As System.Windows.Forms.ComboBox
    Friend WithEvents lblCurrentDescription As Label
    Private WithEvents cmsTreeActions As ContextMenuStrip
    Friend WithEvents AddToolSetupTSM As ToolStripMenuItem
    Friend WithEvents AddMaterialTypeTSM As ToolStripMenuItem
    Friend WithEvents AddMaterialSizeTSM As ToolStripMenuItem
    Friend WithEvents DeleteMaterialTypeTSM As ToolStripMenuItem
    Friend WithEvents DeleteMaterialSizeTSM As ToolStripMenuItem
    Friend WithEvents tabMaterialInventory As TabPage
    Friend WithEvents Panel3 As Panel
    Friend WithEvents Panel2 As Panel
    Friend WithEvents btnMaterialCancel As Button
    Friend WithEvents btnSaveMaterial As Button
    Friend WithEvents btnRemoveMaterialUDP As Button
    Friend WithEvents btnMaterialAddUDP As Button
    Friend WithEvents Panel4 As Panel
    Friend WithEvents btnConvertCMDB As Button
    Friend WithEvents AddLayerSetupTSM As ToolStripMenuItem
    Friend WithEvents AddLayerTSM As ToolStripMenuItem
    Friend WithEvents DeleteLayerSetpTSM As ToolStripMenuItem
    Friend WithEvents DeleteLayerTSM As ToolStripMenuItem
    Friend WithEvents RenameToolSetupTSM As ToolStripMenuItem
    Friend WithEvents tabLayerSetup As TabPage
    Friend WithEvents tabLayer As TabPage
    Friend WithEvents propgrdLayerSetup As PropertyGrid
    Friend WithEvents propgrdLayer As PropertyGrid
    Friend WithEvents pnlLayerSetSaveCancel As Panel
    Friend WithEvents btnSaveLayerSetp As Button
    Friend WithEvents pnlLayerMapSave As Panel
    Friend WithEvents btnLayerMapSave As Button
    Friend WithEvents propgrdMachine As PropertyGrid
    Friend WithEvents propgridMaterial As PropertyGrid
End Class
