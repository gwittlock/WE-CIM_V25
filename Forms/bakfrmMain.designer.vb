Imports DevExpress.Skins
Imports DevExpress.LookAndFeel
Imports DevExpress.UserSkins
Imports DevExpress.XtraBars
Imports DevExpress.XtraBars.Ribbon
Imports DevExpress.XtraBars.Helpers

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmMain
    Inherits RibbonForm

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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Dim SuperToolTip1 As DevExpress.Utils.SuperToolTip = New DevExpress.Utils.SuperToolTip()
        Dim ToolTipTitleItem1 As DevExpress.Utils.ToolTipTitleItem = New DevExpress.Utils.ToolTipTitleItem()
        Dim ToolTipItem1 As DevExpress.Utils.ToolTipItem = New DevExpress.Utils.ToolTipItem()
        Dim SuperToolTip2 As DevExpress.Utils.SuperToolTip = New DevExpress.Utils.SuperToolTip()
        Dim ToolTipTitleItem2 As DevExpress.Utils.ToolTipTitleItem = New DevExpress.Utils.ToolTipTitleItem()
        Dim ToolTipItem2 As DevExpress.Utils.ToolTipItem = New DevExpress.Utils.ToolTipItem()
        Dim SuperToolTip3 As DevExpress.Utils.SuperToolTip = New DevExpress.Utils.SuperToolTip()
        Dim ToolTipTitleItem3 As DevExpress.Utils.ToolTipTitleItem = New DevExpress.Utils.ToolTipTitleItem()
        Dim ToolTipItem3 As DevExpress.Utils.ToolTipItem = New DevExpress.Utils.ToolTipItem()
        Dim SuperToolTip4 As DevExpress.Utils.SuperToolTip = New DevExpress.Utils.SuperToolTip()
        Dim ToolTipItem4 As DevExpress.Utils.ToolTipItem = New DevExpress.Utils.ToolTipItem()
        Dim SuperToolTip5 As DevExpress.Utils.SuperToolTip = New DevExpress.Utils.SuperToolTip()
        Dim ToolTipTitleItem4 As DevExpress.Utils.ToolTipTitleItem = New DevExpress.Utils.ToolTipTitleItem()
        Dim ToolTipItem5 As DevExpress.Utils.ToolTipItem = New DevExpress.Utils.ToolTipItem()
        Me.navbarImageListLarge = New System.Windows.Forms.ImageList(Me.components)
        Me.navbarImageList = New System.Windows.Forms.ImageList(Me.components)
        Me.RibbonControl = New DevExpress.XtraBars.Ribbon.RibbonControl()
        Me.appMenu = New DevExpress.XtraBars.Ribbon.ApplicationMenu(Me.components)
        Me.iNew = New DevExpress.XtraBars.BarButtonItem()
        Me.iOpen = New DevExpress.XtraBars.BarButtonItem()
        Me.iSave = New DevExpress.XtraBars.BarButtonItem()
        Me.iSaveAs = New DevExpress.XtraBars.BarButtonItem()
        Me.iExit = New DevExpress.XtraBars.BarButtonItem()
        Me.barRecentFiles = New DevExpress.XtraBars.BarSubItem()
        Me.ribbonImageCollection = New DevExpress.Utils.ImageCollection(Me.components)
        Me.iHelp = New DevExpress.XtraBars.BarButtonItem()
        Me.iAbout = New DevExpress.XtraBars.BarButtonItem()
        Me.statusLocation = New DevExpress.XtraBars.BarStaticItem()
        Me.statusActiveLayer = New DevExpress.XtraBars.BarStaticItem()
        Me.rgbiSkins = New DevExpress.XtraBars.RibbonGalleryBarItem()
        Me.statusSelection = New DevExpress.XtraBars.BarStaticItem()
        Me.statusDate = New DevExpress.XtraBars.BarStaticItem()
        Me.statusTime = New DevExpress.XtraBars.BarStaticItem()
        Me.iOpenPDB = New DevExpress.XtraBars.BarButtonItem()
        Me.iMerge = New DevExpress.XtraBars.BarButtonItem()
        Me.iExportCNC = New DevExpress.XtraBars.BarButtonItem()
        Me.iPrintGraphics = New DevExpress.XtraBars.BarButtonItem()
        Me.iPrintTools = New DevExpress.XtraBars.BarButtonItem()
        Me.iPrintNest = New DevExpress.XtraBars.BarButtonItem()
        Me.iUndo = New DevExpress.XtraBars.BarButtonItem()
        Me.iRedo = New DevExpress.XtraBars.BarButtonItem()
        Me.iDelete = New DevExpress.XtraBars.BarButtonItem()
        Me.iPurge = New DevExpress.XtraBars.BarButtonItem()
        Me.iChangeTool = New DevExpress.XtraBars.BarButtonItem()
        Me.iChangeAttrib = New DevExpress.XtraBars.BarButtonItem()
        Me.iChangeEntities = New DevExpress.XtraBars.BarButtonItem()
        Me.iHolePattern = New DevExpress.XtraBars.BarButtonItem()
        Me.iLabels = New DevExpress.XtraBars.BarButtonItem()
        Me.iDeletePattern = New DevExpress.XtraBars.BarButtonItem()
        Me.iExplodePattern = New DevExpress.XtraBars.BarButtonItem()
        Me.iDisAssociate = New DevExpress.XtraBars.BarButtonItem()
        Me.iFeatureAdd = New DevExpress.XtraBars.BarButtonItem()
        Me.iFeatureExtract = New DevExpress.XtraBars.BarButtonItem()
        Me.iFeatureInsert = New DevExpress.XtraBars.BarButtonItem()
        Me.iFeatureRemove = New DevExpress.XtraBars.BarButtonItem()
        Me.chkShowStock = New DevExpress.XtraBars.BarEditItem()
        Me.repShowStock = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowZones = New DevExpress.XtraBars.BarEditItem()
        Me.repShowWorkzones = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowTable = New DevExpress.XtraBars.BarEditItem()
        Me.repShowTableGraphics = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowToolpath = New DevExpress.XtraBars.BarEditItem()
        Me.repShowToolpahDashed = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowProfiles = New DevExpress.XtraBars.BarEditItem()
        Me.ShowProfileMarkers = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowEndPoints = New DevExpress.XtraBars.BarEditItem()
        Me.repShowEndPoints = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowInstance = New DevExpress.XtraBars.BarEditItem()
        Me.repShowInstanceText = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowLegend = New DevExpress.XtraBars.BarEditItem()
        Me.repShowLegendText = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowHandles = New DevExpress.XtraBars.BarEditItem()
        Me.repShowHandles = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkHighlightProf = New DevExpress.XtraBars.BarEditItem()
        Me.repHighlightProfiles = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowRapids = New DevExpress.XtraBars.BarEditItem()
        Me.repShowRapidMoves = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowNibble = New DevExpress.XtraBars.BarEditItem()
        Me.repShowNibbleHits = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkShowSolid = New DevExpress.XtraBars.BarEditItem()
        Me.repShowSolidHits = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.chkWhitBackground = New DevExpress.XtraBars.BarEditItem()
        Me.repWhiteBG = New DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit()
        Me.bbViewFull = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewWindow = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewZoomIn = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewZoomOut = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewPrevious = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewRefresh = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewEntityList = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewCodeViewer = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewHotspot = New DevExpress.XtraBars.BarButtonItem()
        Me.bbViewTravel = New DevExpress.XtraBars.BarButtonItem()
        Me.sbViewOptions = New DevExpress.XtraBars.BarSubItem()
        Me.BarShowStock = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowWorkzones = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowTableGraphics = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowToolpathDashed = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowProfileMarkers = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowEndpoints = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowInstanceText = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowLegend = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowHandles = New DevExpress.XtraBars.BarCheckItem()
        Me.BarHighlightProfiles = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowRapidMoves = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowNibbleHits = New DevExpress.XtraBars.BarCheckItem()
        Me.BarShowSolidHits = New DevExpress.XtraBars.BarCheckItem()
        Me.BarSaveLastUsed = New DevExpress.XtraBars.BarCheckItem()
        Me.BarUseWhiteBG = New DevExpress.XtraBars.BarCheckItem()
        Me.barSnapResolution = New DevExpress.XtraBars.BarEditItem()
        Me.repSnapResolution = New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()
        Me.bbCreateLine = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCreateArc = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCreateHole = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCreatePoint = New DevExpress.XtraBars.BarButtonItem()
        Me.bbRubberBand = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCreateBoundingBox = New DevExpress.XtraBars.BarButtonItem()
        Me.bbAssociate = New DevExpress.XtraBars.BarButtonItem()
        Me.bbAutoPunch = New DevExpress.XtraBars.BarButtonItem()
        Me.bbManualLead = New DevExpress.XtraBars.BarButtonItem()
        Me.bbAutoLead = New DevExpress.XtraBars.BarButtonItem()
        Me.bbNotch = New DevExpress.XtraBars.BarButtonItem()
        Me.bbShakerTab = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCommad = New DevExpress.XtraBars.BarButtonItem()
        Me.bbSlit = New DevExpress.XtraBars.BarButtonItem()
        Me.bbLinearCler = New DevExpress.XtraBars.BarButtonItem()
        Me.bbAreaClear = New DevExpress.XtraBars.BarButtonItem()
        Me.bbShapeLIbrary = New DevExpress.XtraBars.BarButtonItem()
        Me.bbTransformMove = New DevExpress.XtraBars.BarButtonItem()
        Me.bbTransFormCopy = New DevExpress.XtraBars.BarButtonItem()
        Me.bbTransFormScale = New DevExpress.XtraBars.BarButtonItem()
        Me.bbTRansformMirror = New DevExpress.XtraBars.BarButtonItem()
        Me.bbTransformRotate = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModTrimExtend = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModSplit = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModFillet = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModChampher = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModDropStop = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModChainCut = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModCutBack = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModZonesManage = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModZonesExtract = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModClamps = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModReposition = New DevExpress.XtraBars.BarButtonItem()
        Me.bbEditProject = New DevExpress.XtraBars.BarButtonItem()
        Me.bbModIndvHIts = New DevExpress.XtraBars.BarButtonItem()
        Me.bbSequenceChain = New DevExpress.XtraBars.BarButtonItem()
        Me.bbSequemceUnchain = New DevExpress.XtraBars.BarButtonItem()
        Me.bbSequenceRevOrder = New DevExpress.XtraBars.BarButtonItem()
        Me.bbSeqManOrder = New DevExpress.XtraBars.BarButtonItem()
        Me.bbConfigMan = New DevExpress.XtraBars.BarButtonItem()
        Me.bbNesting = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCodeView = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCadToCode = New DevExpress.XtraBars.BarButtonItem()
        Me.bbRemnant = New DevExpress.XtraBars.BarButtonItem()
        Me.bbExport = New DevExpress.XtraBars.BarButtonItem()
        Me.bbMacroExecute = New DevExpress.XtraBars.BarButtonItem()
        Me.BarDockingMenuItem1 = New DevExpress.XtraBars.BarDockingMenuItem()
        Me.BarSubItem1 = New DevExpress.XtraBars.BarSubItem()
        Me.BarSubItem2 = New DevExpress.XtraBars.BarSubItem()
        Me.BarStaticItem1 = New DevExpress.XtraBars.BarStaticItem()
        Me.BarStaticItem2 = New DevExpress.XtraBars.BarStaticItem()
        Me.BarButtonItem1 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem2 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem3 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem5 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem6 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem7 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem8 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem9 = New DevExpress.XtraBars.BarButtonItem()
        Me.ribG_NestingParts = New DevExpress.XtraBars.RibbonGalleryBarItem()
        Me.bbWallOfset = New DevExpress.XtraBars.BarButtonItem()
        Me.StstausPlaceHoilder = New DevExpress.XtraBars.BarStaticItem()
        Me.BarButtonItem11 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem12 = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCreatePattern = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCreateInstance = New DevExpress.XtraBars.BarButtonItem()
        Me.bbTrueShape = New DevExpress.XtraBars.BarButtonItem()
        Me.bbManulNestingGrid = New DevExpress.XtraBars.BarButtonItem()
        Me.bbStaggerNest = New DevExpress.XtraBars.BarButtonItem()
        Me.bbSkeltonCutOff = New DevExpress.XtraBars.BarButtonItem()
        Me.bbCMDB = New DevExpress.XtraBars.BarEditItem()
        Me.repCMDBFile = New DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit()
        Me.bbEWM = New DevExpress.XtraBars.BarButtonItem()
        Me.bbAdminDumpModel = New DevExpress.XtraBars.BarButtonItem()
        Me.bbLogFileRecord = New DevExpress.XtraBars.BarButtonItem()
        Me.bbLogFileStop = New DevExpress.XtraBars.BarButtonItem()
        Me.bbbLogFilePlay = New DevExpress.XtraBars.BarButtonItem()
        Me.bbExecutePortal = New DevExpress.XtraBars.BarButtonItem()
        Me.BarEditItem1 = New DevExpress.XtraBars.BarEditItem()
        Me.RepositoryItemMRUEdit1 = New DevExpress.XtraEditors.Repository.RepositoryItemMRUEdit()
        Me.BarListItem1 = New DevExpress.XtraBars.BarListItem()
        Me.BarListItem2 = New DevExpress.XtraBars.BarListItem()
        Me.BarSubItem4 = New DevExpress.XtraBars.BarSubItem()
        Me.BarStaticItem3 = New DevExpress.XtraBars.BarStaticItem()
        Me.BarSubItem3 = New DevExpress.XtraBars.BarSubItem()
        Me.mnuMeasue = New DevExpress.XtraBars.BarStaticItem()
        Me.mnuProperties = New DevExpress.XtraBars.BarStaticItem()
        Me.BarStaticItem6 = New DevExpress.XtraBars.BarStaticItem()
        Me.BarStaticItem7 = New DevExpress.XtraBars.BarStaticItem()
        Me.BarSubItem5 = New DevExpress.XtraBars.BarSubItem()
        Me.bbInquire = New DevExpress.XtraBars.BarButtonItem()
        Me.bbNestingDefaults = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem13 = New DevExpress.XtraBars.BarButtonItem()
        Me.bbManualNestAddPart = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiTools = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiSetLayer = New DevExpress.XtraBars.BarButtonItem()
        Me.bbResetAppdb = New DevExpress.XtraBars.BarEditItem()
        Me.repAPPDBFile = New DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit()
        Me.bbiForTesting = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem14 = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiZoomToPart = New DevExpress.XtraBars.BarEditItem()
        Me.BarEditItem3 = New DevExpress.XtraBars.BarEditItem()
        Me.bbiMeasure = New DevExpress.XtraBars.BarButtonItem()
        Me.puContainerEdit = New DevExpress.XtraBars.BarEditItem()
        Me.RepositoryItemPopupContainerEdit1 = New DevExpress.XtraEditors.Repository.RepositoryItemPopupContainerEdit()
        Me.BarEditItem4 = New DevExpress.XtraBars.BarEditItem()
        Me.RepositoryItemTextEdit6 = New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()
        Me.bbPatternBump = New DevExpress.XtraBars.BarButtonItem()
        Me.bbShowWelcome = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiWECAD = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiCutShop = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiDeactivate = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiBenchMark = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem16 = New DevExpress.XtraBars.BarButtonItem()
        Me.beGraphicsPref = New DevExpress.XtraBars.BarEditItem()
        Me.repGraphicsPref = New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()
        Me.BarHeaderItem1 = New DevExpress.XtraBars.BarHeaderItem()
        Me.BarButtonItem17 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem18 = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiFreightCarAmerica = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiDiamondLife = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiTechConnect = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem19 = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiMRP = New DevExpress.XtraBars.BarButtonItem()
        Me.bbi_Rittal = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiForm1 = New DevExpress.XtraBars.BarButtonItem()
        Me.btmHeatTransfer = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem20 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem21 = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiFileManage = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem22 = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiQuickSave = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiSaveAs = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiPartExtents = New DevExpress.XtraBars.BarButtonItem()
        Me.ProfileBlend = New DevExpress.XtraBars.BarButtonItem()
        Me.PrintEntityList = New DevExpress.XtraBars.BarButtonItem()
        Me.PrintCrossData = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiProfileBlend = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiFlipSheet = New DevExpress.XtraBars.BarButtonItem()
        Me.bbiMaterialList = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem23 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarStaticItem4 = New DevExpress.XtraBars.BarStaticItem()
        Me.bbZoomToPart = New DevExpress.XtraBars.BarButtonItem()
        Me.ribbonImageCollectionLarge = New DevExpress.Utils.ImageCollection(Me.components)
        Me.homeRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.ConfigPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.fileRibbonPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.exportRibbonPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgImport = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.printRibbonPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgSetToolLayer = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.skinsRibbonPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.exitRibbonPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgForTesting = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgBenchMark = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ribNesting = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.rgTrueShape = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rbgManualNesting = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rbgSkeletonCutOff = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rbNestingDefaults = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.EditRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.UndoPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.RedoPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.RemovePageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ChnagePageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.PatternRibbonPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.FeaturesPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ProjectPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ViewRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.ViewZoomPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ViewDisplayPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ViewOptionsPageGroup1 = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.CreateRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.CreateGeoPageGroup1 = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ToolpathPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.PartOutlinePageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.PatternGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ModifyRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.TransformPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ModGeoPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ModToolpathPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ZonesPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.SequenceRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.SequencePageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ManualSeqPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.ribProcess = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.CadToCodePageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.RemnantPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgOptions = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgCustomization = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.MacroRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.MacroPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgCustomMacros = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.AdminRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.rpgEWM = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgLogFile = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgPortal = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgCMDB = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgGraphicsPref = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.helpRibbonPage = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.helpRibbonPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.rpgTechSupport = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.repHandleX = New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()
        Me.repHandleY = New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()
        Me.repOrientation = New DevExpress.XtraEditors.Repository.RepositoryItemCalcEdit()
        Me.repMoveCopy = New DevExpress.XtraEditors.Repository.RepositoryItemRadioGroup()
        Me.RepositoryItemTextEdit2 = New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()
        Me.RepositoryItemTextEdit3 = New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()
        Me.repSheetsGridLookUp = New DevExpress.XtraEditors.Repository.RepositoryItemGridLookUpEdit()
        Me.RepositoryItemGridLookUpEdit1View = New DevExpress.XtraGrid.Views.Grid.GridView()
        Me.RepositoryItemTextEdit1 = New DevExpress.XtraEditors.Repository.RepositoryItemTextEdit()
        Me.ribbonStatusBar = New DevExpress.XtraBars.Ribbon.RibbonStatusBar()
        Me.NestingImageCollection = New DevExpress.Utils.ImageCollection(Me.components)
        Me.dlgFileSaveDialog = New System.Windows.Forms.SaveFileDialog()
        Me.DefaultLookAndFeel1 = New DevExpress.LookAndFeel.DefaultLookAndFeel(Me.components)
        Me.ToolStripContainer1 = New System.Windows.Forms.ToolStripContainer()
        Me.splitContainerControl = New DevExpress.XtraEditors.SplitContainerControl()
        Me.pnlPicmodeler = New System.Windows.Forms.Panel()
        Me.pnlNestCombos = New System.Windows.Forms.Panel()
        Me.cboSheets = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblSheets = New DevExpress.XtraEditors.LabelControl()
        Me.cboPatterns = New DevExpress.XtraEditors.ComboBoxEdit()
        Me.lblPatterns = New DevExpress.XtraEditors.LabelControl()
        Me.defaultToolTipController1 = New DevExpress.Utils.DefaultToolTipController(Me.components)
        Me.tbView = New System.Windows.Forms.ToolStrip()
        Me.tbViewFull = New System.Windows.Forms.ToolStripButton()
        Me.tbViewWindow = New System.Windows.Forms.ToolStripButton()
        Me.tbViewZoomIn = New System.Windows.Forms.ToolStripButton()
        Me.tbViewZoomOut = New System.Windows.Forms.ToolStripButton()
        Me.tbViewPrevious = New System.Windows.Forms.ToolStripButton()
        Me.tbViewRefresh = New System.Windows.Forms.ToolStripButton()
        Me.tbViewSetTools = New System.Windows.Forms.ToolStripButton()
        Me.tbViewSetLayers = New System.Windows.Forms.ToolStripButton()
        Me.tbViewSelHide = New System.Windows.Forms.ToolStripButton()
        Me.tbViewSelShow = New System.Windows.Forms.ToolStripButton()
        Me.tbUndo = New System.Windows.Forms.ToolStrip()
        Me.tbUndo_Undo = New System.Windows.Forms.ToolStripButton()
        Me.tbUndo_Redo = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect = New System.Windows.Forms.ToolStrip()
        Me.tbSelect_Enable = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Layers = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Tools = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Lines = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Arcs = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Holes = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Points = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Command = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Profiles = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_Features = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_ByWindow = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_AddAllByFilter = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_RemoveAllByFilter = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_All = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_RemoveAll = New System.Windows.Forms.ToolStripButton()
        Me.tbSelect_HotDot = New System.Windows.Forms.ToolStripButton()
        Me.picPattern = New DevExpress.XtraEditors.PanelControl()
        Me.rgbSkins = New DevExpress.Utils.ImageCollection(Me.components)
        Me.imgCursors = New DevExpress.Utils.ImageCollection(Me.components)
        Me.dlgFileOpenDialog = New System.Windows.Forms.OpenFileDialog()
        Me.BarButtonItem4 = New DevExpress.XtraBars.BarButtonItem()
        Me.BarButtonItem10 = New DevExpress.XtraBars.BarButtonItem()
        Me.PopupMenu1 = New DevExpress.XtraBars.PopupMenu(Me.components)
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.PopupMenu2 = New DevExpress.XtraBars.PopupMenu(Me.components)
        Me.mnuPopUp3 = New DevExpress.XtraBars.PopupMenu(Me.components)
        Me.HelpProvider1 = New System.Windows.Forms.HelpProvider()
        Me.ribbonPageGroup1 = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.galleryDropDown1 = New DevExpress.XtraBars.Ribbon.GalleryDropDown(Me.components)
        Me.BarButtonItem15 = New DevExpress.XtraBars.BarButtonItem()
        Me.Timer2 = New System.Windows.Forms.Timer(Me.components)
        Me.RibbonPage2 = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.RibbonPage3 = New DevExpress.XtraBars.Ribbon.RibbonPage()
        Me.CodeViewPageGroup = New DevExpress.XtraBars.Ribbon.RibbonPageGroup()
        Me.BehaviorManager1 = New DevExpress.Utils.Behaviors.BehaviorManager(Me.components)
        CType(Me.RibbonControl, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.appMenu, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ribbonImageCollection, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowStock, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowWorkzones, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowTableGraphics, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowToolpahDashed, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ShowProfileMarkers, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowEndPoints, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowInstanceText, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowLegendText, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowHandles, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repHighlightProfiles, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowRapidMoves, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowNibbleHits, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repShowSolidHits, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repWhiteBG, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repSnapResolution, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repCMDBFile, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.RepositoryItemMRUEdit1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repAPPDBFile, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.RepositoryItemPopupContainerEdit1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.RepositoryItemTextEdit6, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repGraphicsPref, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ribbonImageCollectionLarge, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repHandleX, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repHandleY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repOrientation, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repMoveCopy, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.RepositoryItemTextEdit2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.RepositoryItemTextEdit3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.repSheetsGridLookUp, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.RepositoryItemGridLookUpEdit1View, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.RepositoryItemTextEdit1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.NestingImageCollection, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.ToolStripContainer1.ContentPanel.SuspendLayout()
        Me.ToolStripContainer1.TopToolStripPanel.SuspendLayout()
        Me.ToolStripContainer1.SuspendLayout()
        CType(Me.splitContainerControl, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.splitContainerControl.SuspendLayout()
        Me.pnlNestCombos.SuspendLayout()
        CType(Me.cboSheets.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.cboPatterns.Properties, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tbView.SuspendLayout()
        Me.tbUndo.SuspendLayout()
        Me.tbSelect.SuspendLayout()
        CType(Me.picPattern, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.rgbSkins, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.imgCursors, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.PopupMenu1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.PopupMenu2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.mnuPopUp3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.galleryDropDown1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.BehaviorManager1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'navbarImageListLarge
        '
        Me.navbarImageListLarge.ImageStream = CType(resources.GetObject("navbarImageListLarge.ImageStream"), System.Windows.Forms.ImageListStreamer)
        Me.navbarImageListLarge.TransparentColor = System.Drawing.Color.Transparent
        Me.navbarImageListLarge.Images.SetKeyName(0, "Mail_16x16.png")
        Me.navbarImageListLarge.Images.SetKeyName(1, "Organizer_16x16.png")
        '
        'navbarImageList
        '
        Me.navbarImageList.ImageStream = CType(resources.GetObject("navbarImageList.ImageStream"), System.Windows.Forms.ImageListStreamer)
        Me.navbarImageList.TransparentColor = System.Drawing.Color.Transparent
        Me.navbarImageList.Images.SetKeyName(0, "Inbox_16x16.png")
        Me.navbarImageList.Images.SetKeyName(1, "Outbox_16x16.png")
        Me.navbarImageList.Images.SetKeyName(2, "Drafts_16x16.png")
        Me.navbarImageList.Images.SetKeyName(3, "Trash_16x16.png")
        Me.navbarImageList.Images.SetKeyName(4, "Calendar_16x16.png")
        Me.navbarImageList.Images.SetKeyName(5, "Tasks_16x16.png")
        '
        'RibbonControl
        '
        Me.RibbonControl.AllowKeyTips = False
        Me.RibbonControl.AllowMinimizeRibbon = False
        Me.RibbonControl.AllowTrimPageText = False
        Me.RibbonControl.ApplicationButtonDropDownControl = Me.appMenu
        Me.RibbonControl.ApplicationButtonText = Nothing
        Me.RibbonControl.CausesValidation = False
        Me.RibbonControl.ExpandCollapseItem.Id = 0
        Me.RibbonControl.Images = Me.ribbonImageCollection
        Me.RibbonControl.Items.AddRange(New DevExpress.XtraBars.BarItem() {Me.RibbonControl.ExpandCollapseItem, Me.iNew, Me.iOpen, Me.iSave, Me.iSaveAs, Me.iExit, Me.iHelp, Me.iAbout, Me.statusLocation, Me.statusActiveLayer, Me.rgbiSkins, Me.statusSelection, Me.statusDate, Me.statusTime, Me.iOpenPDB, Me.iMerge, Me.iExportCNC, Me.iPrintGraphics, Me.iPrintTools, Me.iPrintNest, Me.iUndo, Me.iRedo, Me.iDelete, Me.iPurge, Me.iChangeTool, Me.iChangeAttrib, Me.iChangeEntities, Me.iHolePattern, Me.iLabels, Me.iDeletePattern, Me.iExplodePattern, Me.iDisAssociate, Me.iFeatureAdd, Me.iFeatureExtract, Me.iFeatureInsert, Me.iFeatureRemove, Me.chkShowStock, Me.chkShowZones, Me.chkShowTable, Me.chkShowToolpath, Me.chkShowProfiles, Me.chkShowEndPoints, Me.chkShowInstance, Me.chkShowLegend, Me.chkShowHandles, Me.chkHighlightProf, Me.chkShowRapids, Me.chkShowNibble, Me.chkShowSolid, Me.chkWhitBackground, Me.bbViewFull, Me.bbViewWindow, Me.bbViewZoomIn, Me.bbViewZoomOut, Me.bbViewPrevious, Me.bbViewRefresh, Me.bbViewEntityList, Me.bbViewCodeViewer, Me.bbViewHotspot, Me.bbViewTravel, Me.sbViewOptions, Me.BarShowStock, Me.BarShowWorkzones, Me.BarShowTableGraphics, Me.BarShowToolpathDashed, Me.BarShowProfileMarkers, Me.BarShowEndpoints, Me.BarShowInstanceText, Me.BarShowLegend, Me.BarShowHandles, Me.BarHighlightProfiles, Me.BarShowRapidMoves, Me.BarShowNibbleHits, Me.BarShowSolidHits, Me.BarSaveLastUsed, Me.BarUseWhiteBG, Me.bbCreateLine, Me.bbCreateArc, Me.bbCreateHole, Me.bbCreatePoint, Me.bbRubberBand, Me.bbCreateBoundingBox, Me.bbAssociate, Me.bbAutoPunch, Me.bbManualLead, Me.bbAutoLead, Me.bbNotch, Me.bbShakerTab, Me.bbCommad, Me.bbSlit, Me.bbLinearCler, Me.bbAreaClear, Me.bbShapeLIbrary, Me.bbTransformMove, Me.bbTransFormCopy, Me.bbTransFormScale, Me.bbTRansformMirror, Me.bbTransformRotate, Me.bbModTrimExtend, Me.bbModSplit, Me.bbModFillet, Me.bbModChampher, Me.bbModDropStop, Me.bbModChainCut, Me.bbModCutBack, Me.bbModZonesManage, Me.bbModZonesExtract, Me.bbModClamps, Me.bbModReposition, Me.bbEditProject, Me.bbModIndvHIts, Me.bbSequenceChain, Me.bbSequemceUnchain, Me.bbSequenceRevOrder, Me.bbSeqManOrder, Me.bbConfigMan, Me.bbNesting, Me.bbCodeView, Me.bbCadToCode, Me.bbRemnant, Me.bbExport, Me.bbMacroExecute, Me.BarDockingMenuItem1, Me.BarSubItem1, Me.BarSubItem2, Me.BarStaticItem1, Me.BarStaticItem2, Me.BarButtonItem1, Me.BarButtonItem2, Me.BarButtonItem3, Me.BarButtonItem5, Me.BarButtonItem6, Me.BarButtonItem7, Me.BarButtonItem8, Me.BarButtonItem9, Me.ribG_NestingParts, Me.barSnapResolution, Me.bbWallOfset, Me.StstausPlaceHoilder, Me.BarButtonItem11, Me.BarButtonItem12, Me.bbCreatePattern, Me.bbCreateInstance, Me.bbTrueShape, Me.bbManulNestingGrid, Me.bbStaggerNest, Me.bbSkeltonCutOff, Me.bbCMDB, Me.bbEWM, Me.bbAdminDumpModel, Me.bbLogFileRecord, Me.bbLogFileStop, Me.bbbLogFilePlay, Me.bbExecutePortal, Me.BarEditItem1, Me.BarListItem1, Me.barRecentFiles, Me.BarListItem2, Me.BarSubItem4, Me.BarStaticItem3, Me.BarSubItem3, Me.mnuMeasue, Me.mnuProperties, Me.BarStaticItem6, Me.BarStaticItem7, Me.BarSubItem5, Me.bbInquire, Me.bbNestingDefaults, Me.BarButtonItem13, Me.bbManualNestAddPart, Me.bbiTools, Me.bbiSetLayer, Me.bbResetAppdb, Me.bbiForTesting, Me.BarButtonItem14, Me.bbiZoomToPart, Me.BarEditItem3, Me.bbiMeasure, Me.puContainerEdit, Me.BarEditItem4, Me.bbPatternBump, Me.bbShowWelcome, Me.bbiWECAD, Me.bbiCutShop, Me.bbiDeactivate, Me.bbiBenchMark, Me.BarButtonItem16, Me.beGraphicsPref, Me.BarHeaderItem1, Me.BarButtonItem17, Me.BarButtonItem18, Me.bbiFreightCarAmerica, Me.bbiDiamondLife, Me.bbiTechConnect, Me.BarButtonItem19, Me.bbiMRP, Me.bbi_Rittal, Me.bbiForm1, Me.btmHeatTransfer, Me.BarButtonItem20, Me.BarButtonItem21, Me.bbiFileManage, Me.BarButtonItem22, Me.bbiQuickSave, Me.bbiSaveAs, Me.bbiPartExtents, Me.ProfileBlend, Me.PrintEntityList, Me.PrintCrossData, Me.bbiProfileBlend, Me.bbiFlipSheet, Me.bbiMaterialList, Me.BarButtonItem23, Me.BarStaticItem4, Me.bbZoomToPart})
        Me.RibbonControl.LargeImages = Me.ribbonImageCollectionLarge
        Me.RibbonControl.Location = New System.Drawing.Point(0, 0)
        Me.RibbonControl.MaxItemId = 435
        Me.RibbonControl.Name = "RibbonControl"
        Me.RibbonControl.OptionsCustomizationForm.FormIcon = CType(resources.GetObject("resource.FormIcon"), System.Drawing.Icon)
        Me.RibbonControl.PageHeaderItemLinks.Add(Me.iAbout)
        Me.RibbonControl.Pages.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPage() {Me.homeRibbonPage, Me.ribNesting, Me.EditRibbonPage, Me.ViewRibbonPage, Me.CreateRibbonPage, Me.ModifyRibbonPage, Me.SequenceRibbonPage, Me.ribProcess, Me.MacroRibbonPage, Me.AdminRibbonPage, Me.helpRibbonPage})
        Me.RibbonControl.QuickToolbarItemLinks.Add(Me.iNew)
        Me.RibbonControl.QuickToolbarItemLinks.Add(Me.iOpen)
        Me.RibbonControl.QuickToolbarItemLinks.Add(Me.iSave)
        Me.RibbonControl.QuickToolbarItemLinks.Add(Me.iSaveAs)
        Me.RibbonControl.QuickToolbarItemLinks.Add(Me.iHelp)
        Me.RibbonControl.QuickToolbarItemLinks.Add(Me.bbExport)
        Me.RibbonControl.RepositoryItems.AddRange(New DevExpress.XtraEditors.Repository.RepositoryItem() {Me.repHandleX, Me.repHandleY, Me.repOrientation, Me.repMoveCopy, Me.RepositoryItemTextEdit2, Me.RepositoryItemTextEdit3, Me.repSheetsGridLookUp, Me.RepositoryItemPopupContainerEdit1, Me.RepositoryItemTextEdit6, Me.RepositoryItemTextEdit1, Me.repGraphicsPref})
        Me.RibbonControl.RibbonStyle = DevExpress.XtraBars.Ribbon.RibbonControlStyle.Office2010
        Me.RibbonControl.ShowApplicationButton = DevExpress.Utils.DefaultBoolean.[True]
        Me.RibbonControl.ShowItemCaptionsInPageHeader = True
        Me.RibbonControl.ShowItemCaptionsInQAT = True
        Me.RibbonControl.Size = New System.Drawing.Size(1352, 147)
        Me.RibbonControl.StatusBar = Me.ribbonStatusBar
        '
        'appMenu
        '
        Me.appMenu.ItemLinks.Add(Me.iNew)
        Me.appMenu.ItemLinks.Add(Me.iOpen)
        Me.appMenu.ItemLinks.Add(Me.iSave)
        Me.appMenu.ItemLinks.Add(Me.iSaveAs)
        Me.appMenu.ItemLinks.Add(Me.iExit)
        Me.appMenu.ItemLinks.Add(Me.barRecentFiles)
        Me.appMenu.Name = "appMenu"
        Me.appMenu.Ribbon = Me.RibbonControl
        '
        'iNew
        '
        Me.iNew.Caption = "New"
        Me.iNew.Description = "Creates a new, blank file."
        Me.iNew.Hint = "Creates a new, blank file"
        Me.iNew.Id = 1
        Me.iNew.ImageOptions.ImageIndex = 0
        Me.iNew.ImageOptions.LargeImageIndex = 0
        Me.iNew.Name = "iNew"
        '
        'iOpen
        '
        Me.iOpen.Caption = "&Open"
        Me.iOpen.Description = "Opens a file."
        Me.iOpen.Hint = "Opens a file"
        Me.iOpen.Id = 2
        Me.iOpen.ImageOptions.ImageIndex = 1
        Me.iOpen.ImageOptions.LargeImageIndex = 1
        Me.iOpen.ItemShortcut = New DevExpress.XtraBars.BarShortcut((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.O))
        Me.iOpen.Name = "iOpen"
        Me.iOpen.ShowItemShortcut = DevExpress.Utils.DefaultBoolean.[True]
        '
        'iSave
        '
        Me.iSave.Caption = "&Save"
        Me.iSave.Description = "Saves the active MM2 file."
        Me.iSave.Hint = "Saves the active MM2 File."
        Me.iSave.Id = 16
        Me.iSave.ImageOptions.ImageIndex = 4
        Me.iSave.ImageOptions.LargeImageIndex = 4
        Me.iSave.ItemShortcut = New DevExpress.XtraBars.BarShortcut((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.S))
        Me.iSave.Name = "iSave"
        '
        'iSaveAs
        '
        Me.iSaveAs.Caption = "Save As"
        Me.iSaveAs.Description = "Saves the active MM2 file in a different location."
        Me.iSaveAs.Hint = "Saves the active MM2 file in a different location."
        Me.iSaveAs.Id = 17
        Me.iSaveAs.ImageOptions.ImageIndex = 5
        Me.iSaveAs.ImageOptions.LargeImageIndex = 5
        Me.iSaveAs.Name = "iSaveAs"
        '
        'iExit
        '
        Me.iExit.Caption = "Exit"
        Me.iExit.Description = "Closes this program after prompting you to save unsaved data."
        Me.iExit.Hint = "Closes this program after prompting you to save unsaved data"
        Me.iExit.Id = 20
        Me.iExit.ImageOptions.ImageIndex = 6
        Me.iExit.ImageOptions.LargeImageIndex = 6
        Me.iExit.Name = "iExit"
        '
        'barRecentFiles
        '
        Me.barRecentFiles.AllowRightClickInMenu = False
        Me.barRecentFiles.Caption = "Recent Files"
        Me.barRecentFiles.Id = 284
        Me.barRecentFiles.ItemClickFireMode = DevExpress.XtraBars.BarItemEventFireMode.Immediate
        Me.barRecentFiles.MenuDrawMode = DevExpress.XtraBars.MenuDrawMode.SmallImagesText
        Me.barRecentFiles.Name = "barRecentFiles"
        '
        'ribbonImageCollection
        '
        Me.ribbonImageCollection.ImageStream = CType(resources.GetObject("ribbonImageCollection.ImageStream"), DevExpress.Utils.ImageCollectionStreamer)
        Me.ribbonImageCollection.Images.SetKeyName(0, "Ribbon_New_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(1, "Ribbon_Open_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(2, "Ribbon_Close_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(3, "Ribbon_Find_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(4, "Ribbon_Save_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(5, "Ribbon_SaveAs_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(6, "Ribbon_Exit_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(7, "Ribbon_Content_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(8, "Ribbon_Info_16x16.png")
        Me.ribbonImageCollection.Images.SetKeyName(9, "1_13.png")
        Me.ribbonImageCollection.Images.SetKeyName(10, "opentoolbox.png")
        Me.ribbonImageCollection.Images.SetKeyName(11, "imagesCA00WBPL.jpg")
        Me.ribbonImageCollection.Images.SetKeyName(12, "modifyEntity.png")
        Me.ribbonImageCollection.Images.SetKeyName(13, "dropdoor.png")
        Me.ribbonImageCollection.Images.SetKeyName(14, "circle-red.png")
        Me.ribbonImageCollection.Images.SetKeyName(15, "circle-green.png")
        Me.ribbonImageCollection.Images.SetKeyName(16, "green_circle16.png")
        Me.ribbonImageCollection.Images.SetKeyName(17, "diskette.gif")
        Me.ribbonImageCollection.Images.SetKeyName(18, "Relationship.png")
        Me.ribbonImageCollection.Images.SetKeyName(19, "flowchart.jpg")
        Me.ribbonImageCollection.Images.SetKeyName(20, "entitylist.png")
        Me.ribbonImageCollection.Images.SetKeyName(21, "entitylist16.png")
        Me.ribbonImageCollection.Images.SetKeyName(22, "delete.png")
        Me.ribbonImageCollection.Images.SetKeyName(23, "delete16.png")
        Me.ribbonImageCollection.Images.SetKeyName(24, "properties.png")
        Me.ribbonImageCollection.Images.SetKeyName(25, "properties16.png")
        Me.ribbonImageCollection.Images.SetKeyName(26, "measure.png")
        Me.ribbonImageCollection.Images.SetKeyName(27, "measure16.png")
        Me.ribbonImageCollection.Images.SetKeyName(28, "hotdot16.png")
        Me.ribbonImageCollection.Images.SetKeyName(29, "hotdot.png")
        Me.ribbonImageCollection.Images.SetKeyName(30, "zoomin16.png")
        Me.ribbonImageCollection.Images.SetKeyName(31, "zoomprevious16.png")
        Me.ribbonImageCollection.Images.SetKeyName(32, "zoomout16.png")
        Me.ribbonImageCollection.Images.SetKeyName(33, "zoomrefresh16.png")
        Me.ribbonImageCollection.Images.SetKeyName(34, "zoomwindow16.png")
        Me.ribbonImageCollection.Images.SetKeyName(35, "zoomfull16.png")
        Me.ribbonImageCollection.Images.SetKeyName(36, "zoomfull.png")
        Me.ribbonImageCollection.Images.SetKeyName(37, "zoomin.png")
        Me.ribbonImageCollection.Images.SetKeyName(38, "zoomout.png")
        Me.ribbonImageCollection.Images.SetKeyName(39, "zoomprevious.png")
        Me.ribbonImageCollection.Images.SetKeyName(40, "zoomrefresh.png")
        Me.ribbonImageCollection.Images.SetKeyName(41, "zoomwindow.png")
        Me.ribbonImageCollection.Images.SetKeyName(42, "zones.png")
        Me.ribbonImageCollection.Images.SetKeyName(43, "projects.png")
        Me.ribbonImageCollection.Images.SetKeyName(44, "PartOutline.png")
        Me.ribbonImageCollection.Images.SetKeyName(45, "chamfer.png")
        Me.ribbonImageCollection.Images.SetKeyName(46, "tools.png")
        Me.ribbonImageCollection.Images.SetKeyName(47, "layers.png")
        Me.ribbonImageCollection.Images.SetKeyName(48, "dropStop.jpg")
        Me.ribbonImageCollection.Images.SetKeyName(49, "FeatureRemove.png")
        Me.ribbonImageCollection.Images.SetKeyName(50, "featureInsert.png")
        Me.ribbonImageCollection.Images.SetKeyName(51, "FeatureExtract.png")
        Me.ribbonImageCollection.Images.SetKeyName(52, "FeatureAdd.png")
        Me.ribbonImageCollection.Images.SetKeyName(53, "tnt.png")
        Me.ribbonImageCollection.Images.SetKeyName(54, "editlabel.png")
        Me.ribbonImageCollection.Images.SetKeyName(55, "hole_pattern.png")
        Me.ribbonImageCollection.Images.SetKeyName(56, "edit_entity.png")
        Me.ribbonImageCollection.Images.SetKeyName(57, "attributes.png")
        Me.ribbonImageCollection.Images.SetKeyName(58, "Tool_Layer.png")
        Me.ribbonImageCollection.Images.SetKeyName(59, "trash_full.png")
        Me.ribbonImageCollection.Images.SetKeyName(60, "redo.png")
        Me.ribbonImageCollection.Images.SetKeyName(61, "undo.png")
        Me.ribbonImageCollection.Images.SetKeyName(62, "redo1 (2).png")
        Me.ribbonImageCollection.Images.SetKeyName(63, "DocumentImport.png")
        Me.ribbonImageCollection.Images.SetKeyName(64, "Undo1 (1).png")
        Me.ribbonImageCollection.Images.SetKeyName(65, "RecordDel.png")
        Me.ribbonImageCollection.Images.SetKeyName(66, "blX.gif")
        Me.ribbonImageCollection.Images.SetKeyName(67, "blY.gif")
        Me.ribbonImageCollection.Images.SetKeyName(68, "brX.gif")
        Me.ribbonImageCollection.Images.SetKeyName(69, "brY.gif")
        Me.ribbonImageCollection.Images.SetKeyName(70, "tlX.gif")
        Me.ribbonImageCollection.Images.SetKeyName(71, "tlY.gif")
        Me.ribbonImageCollection.Images.SetKeyName(72, "trX.gif")
        Me.ribbonImageCollection.Images.SetKeyName(73, "trY.gif")
        Me.ribbonImageCollection.Images.SetKeyName(74, "zones.ico")
        Me.ribbonImageCollection.Images.SetKeyName(75, "PartOutline.ico")
        Me.ribbonImageCollection.Images.SetKeyName(76, "chamfer.ico")
        Me.ribbonImageCollection.Images.SetKeyName(77, "undo.ico")
        Me.ribbonImageCollection.Images.SetKeyName(78, "redo.ico")
        Me.ribbonImageCollection.Images.SetKeyName(79, "Print_tool_Report.ico")
        Me.ribbonImageCollection.Images.SetKeyName(80, "16x16_Open_Pdb.ico")
        Me.ribbonImageCollection.Images.SetKeyName(81, "24x24_openPDB.ico")
        Me.ribbonImageCollection.Images.SetKeyName(82, "zoomout.ico")
        Me.ribbonImageCollection.Images.SetKeyName(83, "ZoomIn.ico")
        Me.ribbonImageCollection.Images.SetKeyName(84, "offset.ico")
        Me.ribbonImageCollection.Images.SetKeyName(85, "Unchain.ico")
        Me.ribbonImageCollection.Images.SetKeyName(86, "Split.ico")
        Me.ribbonImageCollection.Images.SetKeyName(87, "Fillet.ico")
        Me.ribbonImageCollection.Images.SetKeyName(88, "TrimExtend.ico")
        Me.ribbonImageCollection.Images.SetKeyName(89, "delete.ico")
        Me.ribbonImageCollection.Images.SetKeyName(90, "editholepattern.ico")
        Me.ribbonImageCollection.Images.SetKeyName(91, "explode.ico")
        Me.ribbonImageCollection.Images.SetKeyName(92, "Change_tool.ico")
        Me.ribbonImageCollection.Images.SetKeyName(93, "Copy.ico")
        Me.ribbonImageCollection.Images.SetKeyName(94, "array.ico")
        Me.ribbonImageCollection.Images.SetKeyName(95, "mirror.ico")
        Me.ribbonImageCollection.Images.SetKeyName(96, "changelayer.ico")
        Me.ribbonImageCollection.Images.SetKeyName(97, "Scale.ico")
        Me.ribbonImageCollection.Images.SetKeyName(98, "reverseorder.ico")
        Me.ribbonImageCollection.Images.SetKeyName(99, "Rotate.ico")
        Me.ribbonImageCollection.Images.SetKeyName(100, "Chain.ico")
        Me.ribbonImageCollection.Images.SetKeyName(101, "Move.ico")
        Me.ribbonImageCollection.Images.SetKeyName(102, "SelectByWindow.ico")
        Me.ribbonImageCollection.Images.SetKeyName(103, "SelectByFeature.ico")
        Me.ribbonImageCollection.Images.SetKeyName(104, "SelectProfileFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(105, "HotSpot.ico")
        Me.ribbonImageCollection.Images.SetKeyName(106, "deselectall.ico")
        Me.ribbonImageCollection.Images.SetKeyName(107, "EnableSelection.ico")
        Me.ribbonImageCollection.Images.SetKeyName(108, "SelectAll.ico")
        Me.ribbonImageCollection.Images.SetKeyName(109, "deselectAllByFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(110, "SelectAllbyFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(111, "SelectCommandFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(112, "SelectPointFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(113, "SelectToolFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(114, "SelectLayerFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(115, "SelectLineFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(116, "SelectArcFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(117, "SelectHoleFilter.ico")
        Me.ribbonImageCollection.Images.SetKeyName(118, "Arborimage.ico")
        Me.ribbonImageCollection.Images.SetKeyName(119, "Code Preview.ico")
        Me.ribbonImageCollection.Images.SetKeyName(120, "ShapeLibrary.ico")
        Me.ribbonImageCollection.Images.SetKeyName(121, "Code Generation.ico")
        Me.ribbonImageCollection.Images.SetKeyName(122, "Configuration Manager.ico")
        Me.ribbonImageCollection.Images.SetKeyName(123, "CreateHole.ico")
        Me.ribbonImageCollection.Images.SetKeyName(124, "CreateArc.ico")
        Me.ribbonImageCollection.Images.SetKeyName(125, "Create Line.ico")
        Me.ribbonImageCollection.Images.SetKeyName(126, "Nesting.ico")
        Me.ribbonImageCollection.Images.SetKeyName(127, "Barcode (2).ico")
        Me.ribbonImageCollection.Images.SetKeyName(128, "Hammer (3).ico")
        Me.ribbonImageCollection.Images.SetKeyName(129, "Folder (4).ico")
        Me.ribbonImageCollection.Images.SetKeyName(130, "Chart Down.ico")
        Me.ribbonImageCollection.Images.SetKeyName(131, "New.ico")
        Me.ribbonImageCollection.Images.SetKeyName(132, "Eraser (3).ico")
        Me.ribbonImageCollection.Images.SetKeyName(133, "OK (6).ico")
        Me.ribbonImageCollection.Images.SetKeyName(134, "Book Search (2).ico")
        Me.ribbonImageCollection.Images.SetKeyName(135, "Gear (2).ico")
        Me.ribbonImageCollection.Images.SetKeyName(136, "Floppy - 3½ Disk.ico")
        Me.ribbonImageCollection.Images.SetKeyName(137, "Blocks.ico")
        Me.ribbonImageCollection.Images.SetKeyName(138, "Molecule Box.ico")
        Me.ribbonImageCollection.Images.SetKeyName(139, "Flash Drive.ico")
        Me.ribbonImageCollection.Images.SetKeyName(140, "config.ico")
        Me.ribbonImageCollection.Images.SetKeyName(141, "Gear (32).ico")
        Me.ribbonImageCollection.Images.SetKeyName(142, "Folder (1) (open).ico")
        Me.ribbonImageCollection.Images.SetKeyName(143, "LayoutParagraph.ico")
        Me.ribbonImageCollection.Images.SetKeyName(144, "Notepad (4).ico")
        Me.ribbonImageCollection.Images.SetKeyName(145, "Lightning.ico")
        Me.ribbonImageCollection.Images.SetKeyName(146, "Explorer.ico")
        Me.ribbonImageCollection.Images.SetKeyName(147, "Flash Drive.ico")
        Me.ribbonImageCollection.InsertImage(Global.FabV25_WIN8.My.Resources.Resources.MRP_Round, "MRP_Round", GetType(Global.FabV25_WIN8.My.Resources.Resources), 148)
        Me.ribbonImageCollection.Images.SetKeyName(148, "MRP_Round")
        Me.ribbonImageCollection.Images.SetKeyName(149, "apilogo2.png")
        '
        'iHelp
        '
        Me.iHelp.Caption = "Help"
        Me.iHelp.Description = "Start the program help system."
        Me.iHelp.Hint = "Start the program help system"
        Me.iHelp.Id = 22
        Me.iHelp.ImageOptions.ImageIndex = 7
        Me.iHelp.ImageOptions.LargeImageIndex = 7
        Me.iHelp.Name = "iHelp"
        '
        'iAbout
        '
        Me.iAbout.Caption = "About"
        Me.iAbout.Description = "Displays general program information."
        Me.iAbout.Hint = "Displays general program information"
        Me.iAbout.Id = 24
        Me.iAbout.ImageOptions.ImageIndex = 8
        Me.iAbout.ImageOptions.LargeImageIndex = 8
        Me.iAbout.Name = "iAbout"
        '
        'statusLocation
        '
        Me.statusLocation.Caption = "Location:"
        Me.statusLocation.Id = 31
        Me.statusLocation.Name = "statusLocation"
        Me.statusLocation.Width = 250
        '
        'statusActiveLayer
        '
        Me.statusActiveLayer.Caption = "Active:"
        Me.statusActiveLayer.Id = 32
        Me.statusActiveLayer.Name = "statusActiveLayer"
        Me.statusActiveLayer.Width = 200
        '
        'rgbiSkins
        '
        Me.rgbiSkins.Caption = "Skins"
        '
        '
        '
        Me.rgbiSkins.Gallery.AllowHoverImages = True
        Me.rgbiSkins.Gallery.Appearance.ItemCaptionAppearance.Normal.Options.UseFont = True
        Me.rgbiSkins.Gallery.Appearance.ItemCaptionAppearance.Normal.Options.UseTextOptions = True
        Me.rgbiSkins.Gallery.Appearance.ItemCaptionAppearance.Normal.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center
        Me.rgbiSkins.Gallery.ColumnCount = 8
        Me.rgbiSkins.Gallery.HoverImageSize = New System.Drawing.Size(64, 64)
        Me.rgbiSkins.Gallery.ImageSize = New System.Drawing.Size(64, 64)
        Me.rgbiSkins.Gallery.ItemImageLocation = DevExpress.Utils.Locations.Top
        Me.rgbiSkins.Gallery.RowCount = 4
        Me.rgbiSkins.Gallery.UseMaxImageSize = True
        Me.rgbiSkins.Hint = "Select a Theme to change the apperance of the application."
        Me.rgbiSkins.Id = 60
        Me.rgbiSkins.Name = "rgbiSkins"
        '
        'statusSelection
        '
        Me.statusSelection.Caption = "Selection"
        Me.statusSelection.Id = 62
        Me.statusSelection.Name = "statusSelection"
        Me.statusSelection.Width = 250
        '
        'statusDate
        '
        Me.statusDate.Caption = "Date:"
        Me.statusDate.Id = 63
        Me.statusDate.Name = "statusDate"
        Me.statusDate.Width = 150
        '
        'statusTime
        '
        Me.statusTime.Caption = "Time:"
        Me.statusTime.Id = 64
        Me.statusTime.Name = "statusTime"
        Me.statusTime.Width = 100
        '
        'iOpenPDB
        '
        Me.iOpenPDB.Caption = "Open PDB From Model"
        Me.iOpenPDB.Description = "Opens the nesting database asscociated with the current file."
        Me.iOpenPDB.Hint = "Opens the nesting database asscociated with the current file."
        Me.iOpenPDB.Id = 65
        Me.iOpenPDB.ImageOptions.LargeImageIndex = 48
        Me.iOpenPDB.Name = "iOpenPDB"
        '
        'iMerge
        '
        Me.iMerge.Caption = "Merge 1 MM2 Into Another"
        Me.iMerge.Hint = "Insert A MM2 Into Another"
        Me.iMerge.Id = 66
        Me.iMerge.ImageOptions.LargeImageIndex = 124
        Me.iMerge.Name = "iMerge"
        '
        'iExportCNC
        '
        Me.iExportCNC.Caption = "Create CNC File"
        Me.iExportCNC.Description = "Create The File Needed To Run The Machne"
        Me.iExportCNC.Hint = "Create The File Needed To Run The Machne"
        Me.iExportCNC.Id = 67
        Me.iExportCNC.ImageOptions.LargeImageIndex = 116
        Me.iExportCNC.Name = "iExportCNC"
        '
        'iPrintGraphics
        '
        Me.iPrintGraphics.Caption = "Graphics"
        Me.iPrintGraphics.Description = "Prints a Image of the current File"
        Me.iPrintGraphics.Hint = "Prints a Image of the current File"
        Me.iPrintGraphics.Id = 68
        Me.iPrintGraphics.ImageOptions.LargeImageIndex = 120
        Me.iPrintGraphics.ItemShortcut = New DevExpress.XtraBars.BarShortcut((System.Windows.Forms.Keys.Alt Or System.Windows.Forms.Keys.F8))
        Me.iPrintGraphics.Name = "iPrintGraphics"
        '
        'iPrintTools
        '
        Me.iPrintTools.Caption = "Tool List"
        Me.iPrintTools.Description = "Print A Tool List For The Current File"
        Me.iPrintTools.Hint = "Print A Tool List For The Current File"
        Me.iPrintTools.Id = 69
        Me.iPrintTools.ImageOptions.LargeImageIndex = 56
        Me.iPrintTools.Name = "iPrintTools"
        '
        'iPrintNest
        '
        Me.iPrintNest.Caption = "Nest Report"
        Me.iPrintNest.Description = "Print A Report For The Current Nest."
        Me.iPrintNest.Hint = "Print A Report For The Current Nest."
        Me.iPrintNest.Id = 70
        Me.iPrintNest.ImageOptions.LargeImageIndex = 125
        Me.iPrintNest.Name = "iPrintNest"
        '
        'iUndo
        '
        Me.iUndo.Caption = "Undo"
        Me.iUndo.Hint = "Undo Last Operations"
        Me.iUndo.Id = 71
        Me.iUndo.ImageOptions.LargeImageIndex = 54
        Me.iUndo.ItemShortcut = New DevExpress.XtraBars.BarShortcut((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.Z))
        Me.iUndo.Name = "iUndo"
        Me.iUndo.ShortcutKeyDisplayString = "Ctrl+Z"
        '
        'iRedo
        '
        Me.iRedo.Caption = "Redo"
        Me.iRedo.Hint = "Redo The Last Operation"
        Me.iRedo.Id = 72
        Me.iRedo.ImageOptions.LargeImageIndex = 45
        Me.iRedo.ItemShortcut = New DevExpress.XtraBars.BarShortcut((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.Y))
        Me.iRedo.Name = "iRedo"
        Me.iRedo.ShortcutKeyDisplayString = "Ctrl+Y"
        '
        'iDelete
        '
        Me.iDelete.AllowRightClickInMenu = False
        Me.iDelete.Caption = "Delete"
        Me.iDelete.DropDownEnabled = False
        Me.iDelete.Hint = "Delete Selcted Entities"
        Me.iDelete.Id = 73
        Me.iDelete.ImageOptions.Image = Global.FabV25_WIN8.My.Resources.Resources.delete
        Me.iDelete.ImageOptions.LargeImageIndex = 126
        Me.iDelete.Name = "iDelete"
        '
        'iPurge
        '
        Me.iPurge.Caption = "Purge"
        Me.iPurge.Hint = "Purge EnptyFeature And/Or Unused Tooling"
        Me.iPurge.Id = 74
        Me.iPurge.ImageOptions.LargeImageIndex = 44
        Me.iPurge.Name = "iPurge"
        '
        'iChangeTool
        '
        Me.iChangeTool.Caption = "Tool/Layer"
        Me.iChangeTool.Hint = "Change Tool Or Layer Properties"
        Me.iChangeTool.Id = 75
        Me.iChangeTool.ImageOptions.LargeImageIndex = 43
        Me.iChangeTool.Name = "iChangeTool"
        '
        'iChangeAttrib
        '
        Me.iChangeAttrib.Caption = "Attributes"
        Me.iChangeAttrib.Hint = "Change, Add Or Remove Attributes Of a Entity Or Selection"
        Me.iChangeAttrib.Id = 76
        Me.iChangeAttrib.ImageOptions.LargeImageIndex = 42
        Me.iChangeAttrib.Name = "iChangeAttrib"
        '
        'iChangeEntities
        '
        Me.iChangeEntities.Caption = "Entities"
        Me.iChangeEntities.Hint = "Modify The Properties Of A Given Entity"
        Me.iChangeEntities.Id = 77
        Me.iChangeEntities.ImageOptions.LargeImageIndex = 127
        Me.iChangeEntities.Name = "iChangeEntities"
        '
        'iHolePattern
        '
        Me.iHolePattern.Caption = "Hole Pattern"
        Me.iHolePattern.Hint = "Modify The Properties Of a Hole Pattern"
        Me.iHolePattern.Id = 78
        Me.iHolePattern.ImageOptions.LargeImageIndex = 40
        Me.iHolePattern.Name = "iHolePattern"
        '
        'iLabels
        '
        Me.iLabels.Caption = "Labels"
        Me.iLabels.Hint = "Modify The Label Of A Pattern"
        Me.iLabels.Id = 79
        Me.iLabels.ImageOptions.LargeImageIndex = 39
        Me.iLabels.Name = "iLabels"
        '
        'iDeletePattern
        '
        Me.iDeletePattern.Caption = "Delete Pattern"
        Me.iDeletePattern.Hint = "Remove The Selected Pattern"
        Me.iDeletePattern.Id = 80
        Me.iDeletePattern.ImageOptions.LargeImageIndex = 50
        Me.iDeletePattern.Name = "iDeletePattern"
        '
        'iExplodePattern
        '
        Me.iExplodePattern.Caption = "Explode"
        Me.iExplodePattern.Hint = "Explode The Pattern Back To It's Primitive Entities"
        Me.iExplodePattern.Id = 81
        Me.iExplodePattern.ImageOptions.LargeImageIndex = 38
        Me.iExplodePattern.Name = "iExplodePattern"
        '
        'iDisAssociate
        '
        Me.iDisAssociate.Caption = "Disassociate"
        Me.iDisAssociate.Hint = "Remove The Tool Association Of The Selection"
        Me.iDisAssociate.Id = 82
        Me.iDisAssociate.ImageOptions.LargeImageIndex = 128
        Me.iDisAssociate.Name = "iDisAssociate"
        '
        'iFeatureAdd
        '
        Me.iFeatureAdd.Caption = "Add"
        Me.iFeatureAdd.Hint = "Create A New Feature"
        Me.iFeatureAdd.Id = 83
        Me.iFeatureAdd.ImageOptions.LargeImageIndex = 37
        Me.iFeatureAdd.Name = "iFeatureAdd"
        '
        'iFeatureExtract
        '
        Me.iFeatureExtract.Caption = "Extract"
        Me.iFeatureExtract.Hint = "Remove the Enties Within The Feature"
        Me.iFeatureExtract.Id = 85
        Me.iFeatureExtract.ImageOptions.LargeImageIndex = 36
        Me.iFeatureExtract.Name = "iFeatureExtract"
        '
        'iFeatureInsert
        '
        Me.iFeatureInsert.Caption = "Insert"
        Me.iFeatureInsert.Hint = "Insert A Feature Into The Current Model"
        Me.iFeatureInsert.Id = 86
        Me.iFeatureInsert.ImageOptions.LargeImageIndex = 35
        Me.iFeatureInsert.Name = "iFeatureInsert"
        '
        'iFeatureRemove
        '
        Me.iFeatureRemove.Caption = "Remove"
        Me.iFeatureRemove.Hint = "Remove The Complete Feature From the Model"
        Me.iFeatureRemove.Id = 87
        Me.iFeatureRemove.ImageOptions.LargeImageIndex = 34
        Me.iFeatureRemove.Name = "iFeatureRemove"
        '
        'chkShowStock
        '
        Me.chkShowStock.Alignment = DevExpress.XtraBars.BarItemLinkAlignment.Right
        Me.chkShowStock.Caption = "Show Stock Layer (F10)"
        Me.chkShowStock.CaptionAlignment = DevExpress.Utils.HorzAlignment.Far
        Me.chkShowStock.Edit = Me.repShowStock
        Me.chkShowStock.Id = 107
        Me.chkShowStock.Name = "chkShowStock"
        '
        'repShowStock
        '
        Me.repShowStock.AutoHeight = False
        Me.repShowStock.Caption = "Check"
        Me.repShowStock.Name = "repShowStock"
        '
        'chkShowZones
        '
        Me.chkShowZones.Caption = "Show Workzones and Clamps"
        Me.chkShowZones.CaptionAlignment = DevExpress.Utils.HorzAlignment.Far
        Me.chkShowZones.Edit = Me.repShowWorkzones
        Me.chkShowZones.Id = 108
        Me.chkShowZones.Name = "chkShowZones"
        '
        'repShowWorkzones
        '
        Me.repShowWorkzones.AutoHeight = False
        Me.repShowWorkzones.Caption = "Check"
        Me.repShowWorkzones.Name = "repShowWorkzones"
        '
        'chkShowTable
        '
        Me.chkShowTable.Caption = "Show Table Graphics"
        Me.chkShowTable.Edit = Me.repShowTableGraphics
        Me.chkShowTable.Id = 109
        Me.chkShowTable.Name = "chkShowTable"
        '
        'repShowTableGraphics
        '
        Me.repShowTableGraphics.AutoHeight = False
        Me.repShowTableGraphics.Caption = "Check"
        Me.repShowTableGraphics.Name = "repShowTableGraphics"
        '
        'chkShowToolpath
        '
        Me.chkShowToolpath.Caption = "Show Toolpath Geometry Dashed"
        Me.chkShowToolpath.Edit = Me.repShowToolpahDashed
        Me.chkShowToolpath.Id = 110
        Me.chkShowToolpath.Name = "chkShowToolpath"
        '
        'repShowToolpahDashed
        '
        Me.repShowToolpahDashed.AutoHeight = False
        Me.repShowToolpahDashed.Caption = "Check"
        Me.repShowToolpahDashed.Name = "repShowToolpahDashed"
        '
        'chkShowProfiles
        '
        Me.chkShowProfiles.Caption = "Show Profile Markers"
        Me.chkShowProfiles.Edit = Me.ShowProfileMarkers
        Me.chkShowProfiles.Id = 111
        Me.chkShowProfiles.Name = "chkShowProfiles"
        '
        'ShowProfileMarkers
        '
        Me.ShowProfileMarkers.AutoHeight = False
        Me.ShowProfileMarkers.Caption = "Check"
        Me.ShowProfileMarkers.Name = "ShowProfileMarkers"
        '
        'chkShowEndPoints
        '
        Me.chkShowEndPoints.Caption = "Show End Point Markers (F9)"
        Me.chkShowEndPoints.Edit = Me.repShowEndPoints
        Me.chkShowEndPoints.Id = 112
        Me.chkShowEndPoints.Name = "chkShowEndPoints"
        '
        'repShowEndPoints
        '
        Me.repShowEndPoints.AutoHeight = False
        Me.repShowEndPoints.Caption = "Check"
        Me.repShowEndPoints.Name = "repShowEndPoints"
        '
        'chkShowInstance
        '
        Me.chkShowInstance.Caption = "Show Instance Text"
        Me.chkShowInstance.Edit = Me.repShowInstanceText
        Me.chkShowInstance.Id = 113
        Me.chkShowInstance.Name = "chkShowInstance"
        '
        'repShowInstanceText
        '
        Me.repShowInstanceText.AutoHeight = False
        Me.repShowInstanceText.Caption = "Check"
        Me.repShowInstanceText.Name = "repShowInstanceText"
        '
        'chkShowLegend
        '
        Me.chkShowLegend.Caption = "Show Legend Text"
        Me.chkShowLegend.Edit = Me.repShowLegendText
        Me.chkShowLegend.Id = 114
        Me.chkShowLegend.Name = "chkShowLegend"
        '
        'repShowLegendText
        '
        Me.repShowLegendText.AutoHeight = False
        Me.repShowLegendText.Caption = "Check"
        Me.repShowLegendText.Name = "repShowLegendText"
        '
        'chkShowHandles
        '
        Me.chkShowHandles.Caption = "Show Handles (F11)"
        Me.chkShowHandles.Edit = Me.repShowHandles
        Me.chkShowHandles.Id = 115
        Me.chkShowHandles.Name = "chkShowHandles"
        '
        'repShowHandles
        '
        Me.repShowHandles.AutoHeight = False
        Me.repShowHandles.Caption = "Check"
        Me.repShowHandles.Name = "repShowHandles"
        '
        'chkHighlightProf
        '
        Me.chkHighlightProf.Caption = "Highlight Profiles and Features"
        Me.chkHighlightProf.Edit = Me.repHighlightProfiles
        Me.chkHighlightProf.Id = 116
        Me.chkHighlightProf.Name = "chkHighlightProf"
        '
        'repHighlightProfiles
        '
        Me.repHighlightProfiles.AutoHeight = False
        Me.repHighlightProfiles.Caption = "Check"
        Me.repHighlightProfiles.Name = "repHighlightProfiles"
        '
        'chkShowRapids
        '
        Me.chkShowRapids.Caption = "Show Rapid Moves"
        Me.chkShowRapids.Edit = Me.repShowRapidMoves
        Me.chkShowRapids.Id = 117
        Me.chkShowRapids.Name = "chkShowRapids"
        '
        'repShowRapidMoves
        '
        Me.repShowRapidMoves.AutoHeight = False
        Me.repShowRapidMoves.Caption = "Check"
        Me.repShowRapidMoves.Name = "repShowRapidMoves"
        '
        'chkShowNibble
        '
        Me.chkShowNibble.Caption = "Show Nibble Hits"
        Me.chkShowNibble.Edit = Me.repShowNibbleHits
        Me.chkShowNibble.Id = 118
        Me.chkShowNibble.Name = "chkShowNibble"
        '
        'repShowNibbleHits
        '
        Me.repShowNibbleHits.AutoHeight = False
        Me.repShowNibbleHits.Caption = "Check"
        Me.repShowNibbleHits.Name = "repShowNibbleHits"
        '
        'chkShowSolid
        '
        Me.chkShowSolid.Caption = "Show Solid Hits"
        Me.chkShowSolid.Edit = Me.repShowSolidHits
        Me.chkShowSolid.Id = 119
        Me.chkShowSolid.Name = "chkShowSolid"
        '
        'repShowSolidHits
        '
        Me.repShowSolidHits.AutoHeight = False
        Me.repShowSolidHits.Caption = "Check"
        Me.repShowSolidHits.Name = "repShowSolidHits"
        '
        'chkWhitBackground
        '
        Me.chkWhitBackground.Caption = "White Background"
        Me.chkWhitBackground.Edit = Me.repWhiteBG
        Me.chkWhitBackground.Id = 120
        Me.chkWhitBackground.Name = "chkWhitBackground"
        '
        'repWhiteBG
        '
        Me.repWhiteBG.AutoHeight = False
        Me.repWhiteBG.Caption = "Check"
        Me.repWhiteBG.Name = "repWhiteBG"
        '
        'bbViewFull
        '
        Me.bbViewFull.Caption = "Full"
        Me.bbViewFull.Hint = "View Full Screen"
        Me.bbViewFull.Id = 122
        Me.bbViewFull.ImageOptions.ImageIndex = 38
        Me.bbViewFull.ImageOptions.LargeImageIndex = 129
        Me.bbViewFull.Name = "bbViewFull"
        '
        'bbViewWindow
        '
        Me.bbViewWindow.Caption = "Window"
        Me.bbViewWindow.Hint = "View By Window"
        Me.bbViewWindow.Id = 123
        Me.bbViewWindow.ImageOptions.ImageIndex = 34
        Me.bbViewWindow.ImageOptions.LargeImageIndex = 26
        Me.bbViewWindow.Name = "bbViewWindow"
        '
        'bbViewZoomIn
        '
        Me.bbViewZoomIn.Caption = "Zoom In"
        Me.bbViewZoomIn.Hint = "Zoom In"
        Me.bbViewZoomIn.Id = 124
        Me.bbViewZoomIn.ImageOptions.LargeImageIndex = 22
        Me.bbViewZoomIn.Name = "bbViewZoomIn"
        '
        'bbViewZoomOut
        '
        Me.bbViewZoomOut.Caption = "Zoom Out"
        Me.bbViewZoomOut.Hint = "ZoomOut"
        Me.bbViewZoomOut.Id = 125
        Me.bbViewZoomOut.ImageOptions.LargeImageIndex = 23
        Me.bbViewZoomOut.Name = "bbViewZoomOut"
        '
        'bbViewPrevious
        '
        Me.bbViewPrevious.Caption = "Previous"
        Me.bbViewPrevious.Hint = "Zoom Previous"
        Me.bbViewPrevious.Id = 126
        Me.bbViewPrevious.ImageOptions.ImageIndex = 31
        Me.bbViewPrevious.ImageOptions.LargeImageIndex = 24
        Me.bbViewPrevious.ItemShortcut = New DevExpress.XtraBars.BarShortcut(System.Windows.Forms.Keys.F3)
        Me.bbViewPrevious.Name = "bbViewPrevious"
        '
        'bbViewRefresh
        '
        Me.bbViewRefresh.Caption = "Refresh"
        Me.bbViewRefresh.Hint = "Refresh The Current View"
        Me.bbViewRefresh.Id = 128
        Me.bbViewRefresh.ImageOptions.ImageIndex = 33
        Me.bbViewRefresh.ImageOptions.LargeImageIndex = 25
        Me.bbViewRefresh.ItemShortcut = New DevExpress.XtraBars.BarShortcut(System.Windows.Forms.Keys.F8)
        Me.bbViewRefresh.Name = "bbViewRefresh"
        Me.bbViewRefresh.ShortcutKeyDisplayString = "F8"
        '
        'bbViewEntityList
        '
        Me.bbViewEntityList.Caption = "Entity List"
        Me.bbViewEntityList.Hint = "Display The Entity List"
        Me.bbViewEntityList.Id = 142
        Me.bbViewEntityList.ImageOptions.ImageIndex = 21
        Me.bbViewEntityList.ImageOptions.LargeImageIndex = 16
        Me.bbViewEntityList.ItemShortcut = New DevExpress.XtraBars.BarShortcut(System.Windows.Forms.Keys.F7)
        Me.bbViewEntityList.Name = "bbViewEntityList"
        Me.bbViewEntityList.ShortcutKeyDisplayString = "F7"
        '
        'bbViewCodeViewer
        '
        Me.bbViewCodeViewer.Caption = "Code Viewer"
        Me.bbViewCodeViewer.Hint = "Display The Code Viewer To Simulate Toolpath"
        Me.bbViewCodeViewer.Id = 143
        Me.bbViewCodeViewer.ImageOptions.LargeImageIndex = 131
        Me.bbViewCodeViewer.ItemShortcut = New DevExpress.XtraBars.BarShortcut((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.D))
        Me.bbViewCodeViewer.Name = "bbViewCodeViewer"
        '
        'bbViewHotspot
        '
        Me.bbViewHotspot.ButtonStyle = DevExpress.XtraBars.BarButtonStyle.Check
        Me.bbViewHotspot.Caption = "Hot Spot"
        Me.bbViewHotspot.Hint = "Toggle The Hot Spot Indicatior"
        Me.bbViewHotspot.Id = 144
        Me.bbViewHotspot.ImageOptions.LargeImageIndex = 20
        Me.bbViewHotspot.ItemShortcut = New DevExpress.XtraBars.BarShortcut((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.H))
        Me.bbViewHotspot.Name = "bbViewHotspot"
        Me.bbViewHotspot.ShortcutKeyDisplayString = "CTRL-H"
        '
        'bbViewTravel
        '
        Me.bbViewTravel.Caption = "Travel Distance"
        Me.bbViewTravel.Hint = "Display Tool Travel Distance and Number Of Pierces/Holes"
        Me.bbViewTravel.Id = 145
        Me.bbViewTravel.ImageOptions.LargeImageIndex = 133
        Me.bbViewTravel.Name = "bbViewTravel"
        '
        'sbViewOptions
        '
        Me.sbViewOptions.Caption = "Options"
        Me.sbViewOptions.Hint = "Specify General Properties When Viewing"
        Me.sbViewOptions.Id = 148
        Me.sbViewOptions.ImageOptions.Image = CType(resources.GetObject("sbViewOptions.ImageOptions.Image"), System.Drawing.Image)
        Me.sbViewOptions.ImageOptions.LargeImageIndex = 134
        Me.sbViewOptions.LinksPersistInfo.AddRange(New DevExpress.XtraBars.LinkPersistInfo() {New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowStock), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowWorkzones), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowTableGraphics), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowToolpathDashed), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowProfileMarkers, True), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowEndpoints), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowInstanceText), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowLegend), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowHandles), New DevExpress.XtraBars.LinkPersistInfo(Me.BarHighlightProfiles), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowRapidMoves), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowNibbleHits), New DevExpress.XtraBars.LinkPersistInfo(Me.BarShowSolidHits), New DevExpress.XtraBars.LinkPersistInfo(Me.BarSaveLastUsed), New DevExpress.XtraBars.LinkPersistInfo(Me.BarUseWhiteBG), New DevExpress.XtraBars.LinkPersistInfo(Me.barSnapResolution)})
        Me.sbViewOptions.Name = "sbViewOptions"
        Me.sbViewOptions.ShowItemShortcut = DevExpress.Utils.DefaultBoolean.[True]
        '
        'BarShowStock
        '
        Me.BarShowStock.Caption = "Show Stock (F4)"
        Me.BarShowStock.Id = 151
        Me.BarShowStock.Name = "BarShowStock"
        '
        'BarShowWorkzones
        '
        Me.BarShowWorkzones.Caption = "Show Workzones/Clamps"
        Me.BarShowWorkzones.Id = 152
        Me.BarShowWorkzones.Name = "BarShowWorkzones"
        '
        'BarShowTableGraphics
        '
        Me.BarShowTableGraphics.Caption = "Show Table Graphics"
        Me.BarShowTableGraphics.Id = 153
        Me.BarShowTableGraphics.Name = "BarShowTableGraphics"
        '
        'BarShowToolpathDashed
        '
        Me.BarShowToolpathDashed.Caption = "Show Toolpath Dashed"
        Me.BarShowToolpathDashed.Id = 154
        Me.BarShowToolpathDashed.Name = "BarShowToolpathDashed"
        '
        'BarShowProfileMarkers
        '
        Me.BarShowProfileMarkers.Caption = "Show Profile Markers"
        Me.BarShowProfileMarkers.Id = 155
        Me.BarShowProfileMarkers.Name = "BarShowProfileMarkers"
        '
        'BarShowEndpoints
        '
        Me.BarShowEndpoints.Caption = "Show End Point Markers"
        Me.BarShowEndpoints.Id = 156
        Me.BarShowEndpoints.Name = "BarShowEndpoints"
        '
        'BarShowInstanceText
        '
        Me.BarShowInstanceText.Caption = "Show Instance Text"
        Me.BarShowInstanceText.Id = 157
        Me.BarShowInstanceText.Name = "BarShowInstanceText"
        '
        'BarShowLegend
        '
        Me.BarShowLegend.Caption = "Show Legend Text"
        Me.BarShowLegend.Id = 158
        Me.BarShowLegend.Name = "BarShowLegend"
        '
        'BarShowHandles
        '
        Me.BarShowHandles.Caption = "Show Handles"
        Me.BarShowHandles.Id = 159
        Me.BarShowHandles.Name = "BarShowHandles"
        '
        'BarHighlightProfiles
        '
        Me.BarHighlightProfiles.Caption = "Highlight Profile/Features"
        Me.BarHighlightProfiles.Id = 160
        Me.BarHighlightProfiles.Name = "BarHighlightProfiles"
        '
        'BarShowRapidMoves
        '
        Me.BarShowRapidMoves.Caption = "Show Rapid Moves"
        Me.BarShowRapidMoves.Id = 161
        Me.BarShowRapidMoves.Name = "BarShowRapidMoves"
        '
        'BarShowNibbleHits
        '
        Me.BarShowNibbleHits.Caption = "Show Nibble Hits"
        Me.BarShowNibbleHits.Id = 162
        Me.BarShowNibbleHits.Name = "BarShowNibbleHits"
        '
        'BarShowSolidHits
        '
        Me.BarShowSolidHits.Caption = "Show Solid Hits"
        Me.BarShowSolidHits.Id = 163
        Me.BarShowSolidHits.Name = "BarShowSolidHits"
        '
        'BarSaveLastUsed
        '
        Me.BarSaveLastUsed.Caption = "Save Last Used"
        Me.BarSaveLastUsed.Id = 164
        Me.BarSaveLastUsed.Name = "BarSaveLastUsed"
        '
        'BarUseWhiteBG
        '
        Me.BarUseWhiteBG.Caption = "Use White Background"
        Me.BarUseWhiteBG.Id = 166
        Me.BarUseWhiteBG.Name = "BarUseWhiteBG"
        '
        'barSnapResolution
        '
        Me.barSnapResolution.Caption = "Snap Resolution"
        Me.barSnapResolution.Edit = Me.repSnapResolution
        Me.barSnapResolution.EditValue = "0.0001"
        Me.barSnapResolution.Id = 237
        Me.barSnapResolution.Name = "barSnapResolution"
        '
        'repSnapResolution
        '
        Me.repSnapResolution.AutoHeight = False
        Me.repSnapResolution.MaxLength = 25
        Me.repSnapResolution.Name = "repSnapResolution"
        '
        'bbCreateLine
        '
        Me.bbCreateLine.Caption = "Line"
        Me.bbCreateLine.Hint = "Ceate Line Geometry"
        Me.bbCreateLine.Id = 167
        Me.bbCreateLine.ImageOptions.LargeImageIndex = 135
        Me.bbCreateLine.Name = "bbCreateLine"
        '
        'bbCreateArc
        '
        Me.bbCreateArc.Caption = "Arc/Circle"
        Me.bbCreateArc.Hint = "Create Arc/Circle Geometry"
        Me.bbCreateArc.Id = 168
        Me.bbCreateArc.ImageOptions.LargeImageIndex = 136
        Me.bbCreateArc.Name = "bbCreateArc"
        '
        'bbCreateHole
        '
        Me.bbCreateHole.Caption = "Hole"
        Me.bbCreateHole.Hint = "Create Punched/Drilled or Pierce Hole"
        Me.bbCreateHole.Id = 169
        Me.bbCreateHole.ImageOptions.LargeImageIndex = 137
        Me.bbCreateHole.Name = "bbCreateHole"
        '
        'bbCreatePoint
        '
        Me.bbCreatePoint.Caption = "Rapid Point"
        Me.bbCreatePoint.Hint = "Create a Rapid To Point"
        Me.bbCreatePoint.Id = 170
        Me.bbCreatePoint.ImageOptions.LargeImageIndex = 138
        Me.bbCreatePoint.Name = "bbCreatePoint"
        '
        'bbRubberBand
        '
        Me.bbRubberBand.Caption = "Rubberband"
        Me.bbRubberBand.Hint = "Create A Closed Profile Using The Selection Set Using Rubberband"
        Me.bbRubberBand.Id = 171
        Me.bbRubberBand.ImageOptions.LargeImageIndex = 139
        Me.bbRubberBand.Name = "bbRubberBand"
        '
        'bbCreateBoundingBox
        '
        Me.bbCreateBoundingBox.Caption = "Bounding Box"
        Me.bbCreateBoundingBox.Hint = "Create A Closed Profile Using The Selection Set Using A Boundiung Box"
        Me.bbCreateBoundingBox.Id = 172
        Me.bbCreateBoundingBox.ImageOptions.LargeImageIndex = 140
        Me.bbCreateBoundingBox.Name = "bbCreateBoundingBox"
        '
        'bbAssociate
        '
        Me.bbAssociate.Caption = "Associate Tool"
        Me.bbAssociate.Hint = "Associate A Tool to Geometry"
        Me.bbAssociate.Id = 173
        Me.bbAssociate.ImageOptions.LargeImageIndex = 141
        Me.bbAssociate.ItemShortcut = New DevExpress.XtraBars.BarShortcut(System.Windows.Forms.Keys.F12)
        Me.bbAssociate.Name = "bbAssociate"
        Me.bbAssociate.ShortcutKeyDisplayString = "F12"
        '
        'bbAutoPunch
        '
        Me.bbAutoPunch.Caption = "AutoPunch"
        Me.bbAutoPunch.Hint = "Perform Auto Punching Using The Selection Set"
        Me.bbAutoPunch.Id = 174
        Me.bbAutoPunch.ImageOptions.LargeImageIndex = 142
        Me.bbAutoPunch.Name = "bbAutoPunch"
        '
        'bbManualLead
        '
        Me.bbManualLead.Caption = "Manual Lead In/Out"
        Me.bbManualLead.Hint = "Create A Manual Lead In/Out"
        Me.bbManualLead.Id = 176
        Me.bbManualLead.ImageOptions.LargeImageIndex = 144
        Me.bbManualLead.Name = "bbManualLead"
        '
        'bbAutoLead
        '
        Me.bbAutoLead.Caption = "Auto Lead In/Out"
        Me.bbAutoLead.Hint = "Create Auto Lead In/Out Using Selection Set"
        Me.bbAutoLead.Id = 177
        Me.bbAutoLead.ImageOptions.LargeImageIndex = 145
        Me.bbAutoLead.Name = "bbAutoLead"
        '
        'bbNotch
        '
        Me.bbNotch.Caption = "Notch"
        Me.bbNotch.Hint = "Create A Notch In A Profile"
        Me.bbNotch.Id = 178
        Me.bbNotch.ImageOptions.LargeImageIndex = 146
        Me.bbNotch.Name = "bbNotch"
        '
        'bbShakerTab
        '
        Me.bbShakerTab.Caption = "Shaker Tab"
        Me.bbShakerTab.Hint = "Create Shaker Tabs "
        Me.bbShakerTab.Id = 179
        Me.bbShakerTab.ImageOptions.LargeImageIndex = 147
        Me.bbShakerTab.Name = "bbShakerTab"
        '
        'bbCommad
        '
        Me.bbCommad.Caption = "Create Command"
        Me.bbCommad.Hint = "Create CNC Command Or Text Label"
        Me.bbCommad.Id = 180
        Me.bbCommad.ImageOptions.LargeImageIndex = 148
        Me.bbCommad.Name = "bbCommad"
        '
        'bbSlit
        '
        Me.bbSlit.Caption = "Slit"
        Me.bbSlit.Hint = "Cretae Slit Lines For Cutting Skeleton"
        Me.bbSlit.Id = 181
        Me.bbSlit.ImageOptions.LargeImageIndex = 149
        Me.bbSlit.Name = "bbSlit"
        '
        'bbLinearCler
        '
        Me.bbLinearCler.Caption = "Linear Clear"
        Me.bbLinearCler.Hint = "Create Linear Toolpath To Clear A Closed Profile"
        Me.bbLinearCler.Id = 182
        Me.bbLinearCler.ImageOptions.LargeImageIndex = 150
        Me.bbLinearCler.Name = "bbLinearCler"
        '
        'bbAreaClear
        '
        Me.bbAreaClear.Caption = "Area Clear"
        Me.bbAreaClear.Hint = "Create Circular Toolpath To Clear A Closed Profile"
        Me.bbAreaClear.Id = 183
        Me.bbAreaClear.ImageOptions.LargeImageIndex = 151
        Me.bbAreaClear.Name = "bbAreaClear"
        '
        'bbShapeLIbrary
        '
        Me.bbShapeLIbrary.Caption = "Shape Library"
        Me.bbShapeLIbrary.Hint = "Create A Profile From The Shape Library"
        Me.bbShapeLIbrary.Id = 184
        Me.bbShapeLIbrary.ImageOptions.LargeImageIndex = 152
        Me.bbShapeLIbrary.Name = "bbShapeLIbrary"
        '
        'bbTransformMove
        '
        Me.bbTransformMove.Caption = "Move"
        Me.bbTransformMove.Hint = "Move Selected Geometry"
        Me.bbTransformMove.Id = 185
        Me.bbTransformMove.ImageOptions.LargeImageIndex = 165
        Me.bbTransformMove.Name = "bbTransformMove"
        '
        'bbTransFormCopy
        '
        Me.bbTransFormCopy.Caption = "Copy"
        Me.bbTransFormCopy.Hint = "Copy Selected Geometry"
        Me.bbTransFormCopy.Id = 186
        Me.bbTransFormCopy.ImageOptions.LargeImageIndex = 164
        Me.bbTransFormCopy.Name = "bbTransFormCopy"
        '
        'bbTransFormScale
        '
        Me.bbTransFormScale.Caption = "Scale"
        Me.bbTransFormScale.Hint = "Scale Selected Geometry"
        Me.bbTransFormScale.Id = 187
        Me.bbTransFormScale.ImageOptions.LargeImageIndex = 166
        Me.bbTransFormScale.Name = "bbTransFormScale"
        '
        'bbTRansformMirror
        '
        Me.bbTRansformMirror.Caption = "Mirror"
        Me.bbTRansformMirror.Hint = "Mirror Selected Geometry"
        Me.bbTRansformMirror.Id = 188
        Me.bbTRansformMirror.ImageOptions.LargeImageIndex = 161
        Me.bbTRansformMirror.Name = "bbTRansformMirror"
        '
        'bbTransformRotate
        '
        Me.bbTransformRotate.Caption = "Rotate"
        Me.bbTransformRotate.Hint = "Rotate Selected Geometry"
        Me.bbTransformRotate.Id = 189
        Me.bbTransformRotate.ImageOptions.LargeImageIndex = 159
        Me.bbTransformRotate.Name = "bbTransformRotate"
        '
        'bbModTrimExtend
        '
        Me.bbModTrimExtend.Caption = "Trim/Extend"
        Me.bbModTrimExtend.Hint = "Trim Or Extend Entities"
        Me.bbModTrimExtend.Id = 191
        Me.bbModTrimExtend.ImageOptions.LargeImageIndex = 163
        Me.bbModTrimExtend.Name = "bbModTrimExtend"
        '
        'bbModSplit
        '
        Me.bbModSplit.Caption = "Split"
        Me.bbModSplit.Hint = "Split A Entity"
        Me.bbModSplit.Id = 192
        Me.bbModSplit.ImageOptions.LargeImageIndex = 162
        Me.bbModSplit.Name = "bbModSplit"
        '
        'bbModFillet
        '
        Me.bbModFillet.Caption = "Fillet"
        Me.bbModFillet.Hint = "Apply Fillet To a Corner"
        Me.bbModFillet.Id = 193
        Me.bbModFillet.ImageOptions.LargeImageIndex = 157
        Me.bbModFillet.Name = "bbModFillet"
        '
        'bbModChampher
        '
        Me.bbModChampher.Caption = "Chamfer"
        Me.bbModChampher.Hint = "Apply A Champher To Two Entities"
        Me.bbModChampher.Id = 194
        Me.bbModChampher.ImageOptions.LargeImageIndex = 156
        Me.bbModChampher.Name = "bbModChampher"
        '
        'bbModDropStop
        '
        Me.bbModDropStop.Caption = "Drop/Stop"
        Me.bbModDropStop.Hint = "Specify A Drop Or Stop Command On A Entity"
        Me.bbModDropStop.Id = 195
        Me.bbModDropStop.ImageOptions.LargeImageIndex = 154
        Me.bbModDropStop.Name = "bbModDropStop"
        '
        'bbModChainCut
        '
        Me.bbModChainCut.Caption = "Set Chain Cut"
        Me.bbModChainCut.Hint = "Specify An Entity To Be Used For Chain Cutting"
        Me.bbModChainCut.Id = 196
        Me.bbModChainCut.ImageOptions.LargeImageIndex = 155
        Me.bbModChainCut.Name = "bbModChainCut"
        '
        'bbModCutBack
        '
        Me.bbModCutBack.Caption = "Cutback Line"
        Me.bbModCutBack.Hint = "Modify The Cut Back Toolpath Created By Nesting"
        Me.bbModCutBack.Id = 197
        Me.bbModCutBack.ImageOptions.LargeImageIndex = 153
        Me.bbModCutBack.Name = "bbModCutBack"
        '
        'bbModZonesManage
        '
        Me.bbModZonesManage.Caption = "Manage"
        Me.bbModZonesManage.Hint = "Modify Properties Of A WorkZone And Assign Entities To A Workzone"
        Me.bbModZonesManage.Id = 201
        Me.bbModZonesManage.ImageOptions.LargeImageIndex = 160
        Me.bbModZonesManage.Name = "bbModZonesManage"
        '
        'bbModZonesExtract
        '
        Me.bbModZonesExtract.Caption = "Extract"
        Me.bbModZonesExtract.Hint = "Extract All Entities From A workzone."
        Me.bbModZonesExtract.Id = 202
        Me.bbModZonesExtract.ImageOptions.LargeImageIndex = 167
        Me.bbModZonesExtract.Name = "bbModZonesExtract"
        '
        'bbModClamps
        '
        Me.bbModClamps.Caption = "Clamp Locations"
        Me.bbModClamps.Hint = "Specify or Modify the clamp locations of the current model"
        Me.bbModClamps.Id = 203
        Me.bbModClamps.ImageOptions.LargeImageIndex = 168
        Me.bbModClamps.Name = "bbModClamps"
        '
        'bbModReposition
        '
        Me.bbModReposition.Caption = "Manual Reposition"
        Me.bbModReposition.Hint = "Specify The Clamp Locations For Reposition"
        Me.bbModReposition.Id = 204
        Me.bbModReposition.ImageOptions.LargeImageIndex = 169
        Me.bbModReposition.Name = "bbModReposition"
        '
        'bbEditProject
        '
        Me.bbEditProject.Caption = "Project"
        Me.bbEditProject.Hint = "Modify The Propertis Of The Current Project"
        Me.bbEditProject.Id = 205
        Me.bbEditProject.ImageOptions.LargeImageIndex = 170
        Me.bbEditProject.Name = "bbEditProject"
        '
        'bbModIndvHIts
        '
        Me.bbModIndvHIts.Caption = "Explode To Individual Hits"
        Me.bbModIndvHIts.Hint = "Explode Profile Into Indivual Punch Hits(For Machines That Do Not Support Canned " &
    "Cycles)"
        Me.bbModIndvHIts.Id = 206
        Me.bbModIndvHIts.ImageOptions.LargeImageIndex = 171
        Me.bbModIndvHIts.Name = "bbModIndvHIts"
        '
        'bbSequenceChain
        '
        Me.bbSequenceChain.Caption = "Chain"
        Me.bbSequenceChain.Hint = "Chain Entities Into A Profile"
        Me.bbSequenceChain.Id = 207
        Me.bbSequenceChain.ImageOptions.LargeImageIndex = 173
        Me.bbSequenceChain.Name = "bbSequenceChain"
        '
        'bbSequemceUnchain
        '
        Me.bbSequemceUnchain.Caption = "UnChain"
        Me.bbSequemceUnchain.Hint = "Unchain Entitiesa From A profile Into Thier Primative Type"
        Me.bbSequemceUnchain.Id = 208
        Me.bbSequemceUnchain.ImageOptions.LargeImageIndex = 172
        Me.bbSequemceUnchain.Name = "bbSequemceUnchain"
        '
        'bbSequenceRevOrder
        '
        Me.bbSequenceRevOrder.Caption = "Reverse Order"
        Me.bbSequenceRevOrder.Hint = "Reverse The direction Order of Geometry"
        Me.bbSequenceRevOrder.Id = 209
        Me.bbSequenceRevOrder.ImageOptions.LargeImageIndex = 174
        Me.bbSequenceRevOrder.Name = "bbSequenceRevOrder"
        '
        'bbSeqManOrder
        '
        Me.bbSeqManOrder.Caption = "Select Toolpath Order"
        Me.bbSeqManOrder.Hint = "Manually  Specify How The toolpath Should Be Sequenced"
        Me.bbSeqManOrder.Id = 210
        Me.bbSeqManOrder.ImageOptions.LargeImageIndex = 175
        Me.bbSeqManOrder.Name = "bbSeqManOrder"
        '
        'bbConfigMan
        '
        Me.bbConfigMan.Caption = "Configuration Manager"
        Me.bbConfigMan.Hint = "Modify Machine, Tooling, Layer Mapping, Material, And Lead Parameters"
        Me.bbConfigMan.Id = 211
        Me.bbConfigMan.ImageOptions.LargeImageIndex = 176
        Me.bbConfigMan.Name = "bbConfigMan"
        ToolTipTitleItem1.Text = "Configuration Manager"
        ToolTipItem1.LeftIndent = 6
        SuperToolTip1.Items.Add(ToolTipTitleItem1)
        SuperToolTip1.Items.Add(ToolTipItem1)
        Me.bbConfigMan.SuperTip = SuperToolTip1
        '
        'bbNesting
        '
        Me.bbNesting.Caption = "Automatic Nesting"
        Me.bbNesting.Hint = "Create Nesated Sheets of Parts"
        Me.bbNesting.Id = 212
        Me.bbNesting.ImageOptions.LargeImageIndex = 177
        Me.bbNesting.Name = "bbNesting"
        '
        'bbCodeView
        '
        Me.bbCodeView.Caption = "Preview CNC Code"
        Me.bbCodeView.Hint = "Simulate The CNC Code"
        Me.bbCodeView.Id = 213
        Me.bbCodeView.ImageOptions.LargeImageIndex = 178
        Me.bbCodeView.Name = "bbCodeView"
        '
        'bbCadToCode
        '
        Me.bbCadToCode.Caption = "CAD To Code"
        Me.bbCadToCode.Hint = "Batrch Convert Cad Files To Model Files"
        Me.bbCadToCode.Id = 214
        Me.bbCadToCode.ImageOptions.LargeImageIndex = 179
        Me.bbCadToCode.Name = "bbCadToCode"
        '
        'bbRemnant
        '
        Me.bbRemnant.Caption = "Remnant Manager"
        Me.bbRemnant.Hint = "Manage Remnants Created By Nesting"
        Me.bbRemnant.Id = 215
        Me.bbRemnant.ImageOptions.LargeImageIndex = 180
        Me.bbRemnant.Name = "bbRemnant"
        '
        'bbExport
        '
        Me.bbExport.Caption = "Export CNC Code"
        Me.bbExport.Id = 216
        Me.bbExport.ImageOptions.ImageIndex = 147
        Me.bbExport.Name = "bbExport"
        '
        'bbMacroExecute
        '
        Me.bbMacroExecute.Caption = "Macro Execute"
        Me.bbMacroExecute.Hint = "Execute A User Macro"
        Me.bbMacroExecute.Id = 217
        Me.bbMacroExecute.ImageOptions.LargeImageIndex = 181
        Me.bbMacroExecute.Name = "bbMacroExecute"
        '
        'BarDockingMenuItem1
        '
        Me.BarDockingMenuItem1.Caption = "BarDockingMenuItem1"
        Me.BarDockingMenuItem1.Id = 219
        Me.BarDockingMenuItem1.Name = "BarDockingMenuItem1"
        '
        'BarSubItem1
        '
        Me.BarSubItem1.Caption = "View Full"
        Me.BarSubItem1.Id = 220
        Me.BarSubItem1.Name = "BarSubItem1"
        '
        'BarSubItem2
        '
        Me.BarSubItem2.Caption = "BarSubItem2"
        Me.BarSubItem2.Id = 221
        Me.BarSubItem2.Name = "BarSubItem2"
        '
        'BarStaticItem1
        '
        Me.BarStaticItem1.Caption = "View Full"
        Me.BarStaticItem1.Id = 222
        Me.BarStaticItem1.Name = "BarStaticItem1"
        '
        'BarStaticItem2
        '
        Me.BarStaticItem2.Caption = "BarStaticItem2"
        Me.BarStaticItem2.Id = 223
        Me.BarStaticItem2.Name = "BarStaticItem2"
        '
        'BarButtonItem1
        '
        Me.BarButtonItem1.Caption = "Vew All of the picture"
        Me.BarButtonItem1.Id = 224
        Me.BarButtonItem1.Name = "BarButtonItem1"
        '
        'BarButtonItem2
        '
        Me.BarButtonItem2.Caption = "View Window"
        Me.BarButtonItem2.Id = 225
        Me.BarButtonItem2.Name = "BarButtonItem2"
        '
        'BarButtonItem3
        '
        Me.BarButtonItem3.Caption = "BarButtonItem3"
        Me.BarButtonItem3.Id = 226
        Me.BarButtonItem3.Name = "BarButtonItem3"
        '
        'BarButtonItem5
        '
        Me.BarButtonItem5.Caption = "View Full"
        Me.BarButtonItem5.Id = 228
        Me.BarButtonItem5.Name = "BarButtonItem5"
        '
        'BarButtonItem6
        '
        Me.BarButtonItem6.Caption = "View Window"
        Me.BarButtonItem6.Id = 229
        Me.BarButtonItem6.Name = "BarButtonItem6"
        '
        'BarButtonItem7
        '
        Me.BarButtonItem7.Caption = "View Previous"
        Me.BarButtonItem7.Id = 230
        Me.BarButtonItem7.Name = "BarButtonItem7"
        '
        'BarButtonItem8
        '
        Me.BarButtonItem8.Caption = "BarButtonItem8"
        Me.BarButtonItem8.Id = 232
        Me.BarButtonItem8.Name = "BarButtonItem8"
        '
        'BarButtonItem9
        '
        Me.BarButtonItem9.Caption = "BarButtonItem9"
        Me.BarButtonItem9.Id = 233
        Me.BarButtonItem9.Name = "BarButtonItem9"
        '
        'ribG_NestingParts
        '
        Me.ribG_NestingParts.Caption = "RibbonGalleryBarItem1"
        '
        '
        '
        Me.ribG_NestingParts.Gallery.ImageSize = New System.Drawing.Size(100, 100)
        Me.ribG_NestingParts.Id = 235
        Me.ribG_NestingParts.Name = "ribG_NestingParts"
        '
        'bbWallOfset
        '
        Me.bbWallOfset.Caption = "WallOffset"
        Me.bbWallOfset.Hint = "Creates a New Profile which is offset from the currently selected Profile"
        Me.bbWallOfset.Id = 239
        Me.bbWallOfset.ImageOptions.Image = CType(resources.GetObject("bbWallOfset.ImageOptions.Image"), System.Drawing.Image)
        Me.bbWallOfset.ImageOptions.LargeImage = CType(resources.GetObject("bbWallOfset.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.bbWallOfset.Name = "bbWallOfset"
        '
        'StstausPlaceHoilder
        '
        Me.StstausPlaceHoilder.Caption = "PlaceHolder"
        Me.StstausPlaceHoilder.Id = 241
        Me.StstausPlaceHoilder.Name = "StstausPlaceHoilder"
        Me.StstausPlaceHoilder.Visibility = DevExpress.XtraBars.BarItemVisibility.Never
        '
        'BarButtonItem11
        '
        Me.BarButtonItem11.Caption = "Pattern"
        Me.BarButtonItem11.Id = 249
        Me.BarButtonItem11.ImageOptions.LargeImage = CType(resources.GetObject("BarButtonItem11.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.BarButtonItem11.ImageOptions.LargeImageIndex = 183
        Me.BarButtonItem11.Name = "BarButtonItem11"
        '
        'BarButtonItem12
        '
        Me.BarButtonItem12.Caption = "Pattern"
        Me.BarButtonItem12.Id = 250
        Me.BarButtonItem12.ImageOptions.LargeImageIndex = 183
        Me.BarButtonItem12.Name = "BarButtonItem12"
        '
        'bbCreatePattern
        '
        Me.bbCreatePattern.Caption = "Pattern"
        Me.bbCreatePattern.Hint = "Creates a pattern fro the current selection, or from a selected MM2 file."
        Me.bbCreatePattern.Id = 251
        Me.bbCreatePattern.ImageOptions.LargeImageIndex = 183
        Me.bbCreatePattern.Name = "bbCreatePattern"
        '
        'bbCreateInstance
        '
        Me.bbCreateInstance.Caption = "Instance"
        Me.bbCreateInstance.Hint = "Inserts a pattern into the current model."
        Me.bbCreateInstance.Id = 252
        Me.bbCreateInstance.ImageOptions.LargeImageIndex = 184
        Me.bbCreateInstance.Name = "bbCreateInstance"
        '
        'bbTrueShape
        '
        Me.bbTrueShape.Caption = "Automatic Nesting"
        Me.bbTrueShape.Hint = "Show the Nesting Dialog in order to either Create or Ooen a nesting database."
        Me.bbTrueShape.Id = 253
        Me.bbTrueShape.ImageOptions.LargeImageIndex = 205
        Me.bbTrueShape.Name = "bbTrueShape"
        '
        'bbManulNestingGrid
        '
        Me.bbManulNestingGrid.Caption = "Grid"
        Me.bbManulNestingGrid.Hint = "Use this button in order to manually create a Grid of the current selection."
        Me.bbManulNestingGrid.Id = 254
        Me.bbManulNestingGrid.ImageOptions.LargeImageIndex = 185
        Me.bbManulNestingGrid.Name = "bbManulNestingGrid"
        '
        'bbStaggerNest
        '
        Me.bbStaggerNest.Caption = "Staggered"
        Me.bbStaggerNest.Hint = "Use this button to create a Staggered Nest of the current selection."
        Me.bbStaggerNest.Id = 255
        Me.bbStaggerNest.ImageOptions.LargeImageIndex = 186
        Me.bbStaggerNest.Name = "bbStaggerNest"
        '
        'bbSkeltonCutOff
        '
        Me.bbSkeltonCutOff.Caption = "Skeleton Cutoff"
        Me.bbSkeltonCutOff.Hint = "Use this button to cut up the skeleton /Web cretead by nesting parts on a sheet. " &
    "This will cut the skeleton in both the X and Y axis."
        Me.bbSkeltonCutOff.Id = 256
        Me.bbSkeltonCutOff.ImageOptions.LargeImageIndex = 187
        Me.bbSkeltonCutOff.Name = "bbSkeltonCutOff"
        '
        'bbCMDB
        '
        Me.bbCMDB.Caption = "  CMDB File:"
        Me.bbCMDB.Edit = Me.repCMDBFile
        Me.bbCMDB.EditWidth = 400
        Me.bbCMDB.Id = 257
        Me.bbCMDB.Name = "bbCMDB"
        '
        'repCMDBFile
        '
        Me.repCMDBFile.AutoHeight = False
        Me.repCMDBFile.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton()})
        Me.repCMDBFile.Name = "repCMDBFile"
        '
        'bbEWM
        '
        Me.bbEWM.Caption = "EWM Settings"
        Me.bbEWM.Id = 274
        Me.bbEWM.ImageOptions.LargeImageIndex = 189
        Me.bbEWM.Name = "bbEWM"
        '
        'bbAdminDumpModel
        '
        Me.bbAdminDumpModel.Caption = "Dump Model"
        Me.bbAdminDumpModel.Id = 277
        Me.bbAdminDumpModel.ImageOptions.LargeImageIndex = 190
        Me.bbAdminDumpModel.Name = "bbAdminDumpModel"
        Me.bbAdminDumpModel.PaintStyle = DevExpress.XtraBars.BarItemPaintStyle.CaptionGlyph
        '
        'bbLogFileRecord
        '
        Me.bbLogFileRecord.Caption = "Record"
        Me.bbLogFileRecord.Id = 278
        Me.bbLogFileRecord.ImageOptions.LargeImageIndex = 193
        Me.bbLogFileRecord.Name = "bbLogFileRecord"
        '
        'bbLogFileStop
        '
        Me.bbLogFileStop.Caption = "Stop"
        Me.bbLogFileStop.Id = 279
        Me.bbLogFileStop.ImageOptions.LargeImageIndex = 192
        Me.bbLogFileStop.Name = "bbLogFileStop"
        '
        'bbbLogFilePlay
        '
        Me.bbbLogFilePlay.Caption = "Play"
        Me.bbbLogFilePlay.Id = 280
        Me.bbbLogFilePlay.ImageOptions.LargeImageIndex = 191
        Me.bbbLogFilePlay.Name = "bbbLogFilePlay"
        '
        'bbExecutePortal
        '
        Me.bbExecutePortal.Caption = "Execute Portal"
        Me.bbExecutePortal.Id = 281
        Me.bbExecutePortal.ImageOptions.LargeImageIndex = 194
        Me.bbExecutePortal.Name = "bbExecutePortal"
        '
        'BarEditItem1
        '
        Me.BarEditItem1.Caption = "MRU"
        Me.BarEditItem1.Edit = Me.RepositoryItemMRUEdit1
        Me.BarEditItem1.Id = 282
        Me.BarEditItem1.Name = "BarEditItem1"
        '
        'RepositoryItemMRUEdit1
        '
        Me.RepositoryItemMRUEdit1.AutoHeight = False
        Me.RepositoryItemMRUEdit1.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.RepositoryItemMRUEdit1.Name = "RepositoryItemMRUEdit1"
        '
        'BarListItem1
        '
        Me.BarListItem1.Caption = "Recent"
        Me.BarListItem1.Id = 283
        Me.BarListItem1.Name = "BarListItem1"
        '
        'BarListItem2
        '
        Me.BarListItem2.Caption = "BarListItem2"
        Me.BarListItem2.Id = 285
        Me.BarListItem2.Name = "BarListItem2"
        '
        'BarSubItem4
        '
        Me.BarSubItem4.Caption = "BarSubItem4"
        Me.BarSubItem4.Id = 286
        Me.BarSubItem4.Name = "BarSubItem4"
        '
        'BarStaticItem3
        '
        Me.BarStaticItem3.Id = 287
        Me.BarStaticItem3.Name = "BarStaticItem3"
        '
        'BarSubItem3
        '
        Me.BarSubItem3.Caption = "BarSubItem3"
        Me.BarSubItem3.Id = 288
        Me.BarSubItem3.Name = "BarSubItem3"
        '
        'mnuMeasue
        '
        Me.mnuMeasue.Caption = "Measure"
        Me.mnuMeasue.Id = 289
        Me.mnuMeasue.Name = "mnuMeasue"
        Me.mnuMeasue.RibbonStyle = DevExpress.XtraBars.Ribbon.RibbonItemStyles.SmallWithText
        Me.mnuMeasue.ShowImageInToolbar = False
        '
        'mnuProperties
        '
        Me.mnuProperties.Caption = "Properties"
        Me.mnuProperties.Id = 290
        Me.mnuProperties.Name = "mnuProperties"
        '
        'BarStaticItem6
        '
        Me.BarStaticItem6.Caption = "Delete"
        Me.BarStaticItem6.Id = 291
        Me.BarStaticItem6.Name = "BarStaticItem6"
        '
        'BarStaticItem7
        '
        Me.BarStaticItem7.Caption = "Entity List"
        Me.BarStaticItem7.Id = 292
        Me.BarStaticItem7.Name = "BarStaticItem7"
        '
        'BarSubItem5
        '
        Me.BarSubItem5.Caption = "BarSubItem5"
        Me.BarSubItem5.Id = 293
        Me.BarSubItem5.Name = "BarSubItem5"
        '
        'bbInquire
        '
        Me.bbInquire.Caption = "Properties"
        Me.bbInquire.Id = 294
        Me.bbInquire.ImageOptions.ImageIndex = 25
        Me.bbInquire.Name = "bbInquire"
        '
        'bbNestingDefaults
        '
        Me.bbNestingDefaults.Caption = "Set Nest Settings Defaults"
        Me.bbNestingDefaults.Description = "Nesting Defaults"
        Me.bbNestingDefaults.Hint = "Sets the defaults values for the Parts Database Setup"
        Me.bbNestingDefaults.Id = 296
        Me.bbNestingDefaults.ImageOptions.LargeImageIndex = 188
        Me.bbNestingDefaults.Name = "bbNestingDefaults"
        '
        'BarButtonItem13
        '
        Me.BarButtonItem13.Caption = "Report a Problem"
        Me.BarButtonItem13.Hint = "Report a problem/bug to Wittlock Engineering Development."
        Me.BarButtonItem13.Id = 298
        Me.BarButtonItem13.ImageOptions.LargeImageIndex = 195
        Me.BarButtonItem13.Name = "BarButtonItem13"
        '
        'bbManualNestAddPart
        '
        Me.bbManualNestAddPart.Caption = "Add Part"
        Me.bbManualNestAddPart.Hint = "This button creates a Pattern as well as adds a part the currently selected sheet" &
    " of nested parts."
        Me.bbManualNestAddPart.Id = 299
        Me.bbManualNestAddPart.ImageOptions.LargeImageIndex = 196
        Me.bbManualNestAddPart.Name = "bbManualNestAddPart"
        '
        'bbiTools
        '
        Me.bbiTools.Caption = "Set Active Tool"
        Me.bbiTools.Hint = "Select a tool to use for drawing geometry. This tool will stay active until you s" &
    "elect another tool or Layer."
        Me.bbiTools.Id = 301
        Me.bbiTools.ImageOptions.LargeImageIndex = 199
        Me.bbiTools.Name = "bbiTools"
        '
        'bbiSetLayer
        '
        Me.bbiSetLayer.Caption = "Set Active Layer"
        Me.bbiSetLayer.Hint = "Select a Layer use for drawing geometry. This Layer will stay active until you se" &
    "lect another Layer or Tool."
        Me.bbiSetLayer.Id = 302
        Me.bbiSetLayer.ImageOptions.LargeImageIndex = 198
        Me.bbiSetLayer.Name = "bbiSetLayer"
        '
        'bbResetAppdb
        '
        Me.bbResetAppdb.Caption = "App DB File:"
        Me.bbResetAppdb.Edit = Me.repAPPDBFile
        Me.bbResetAppdb.EditWidth = 400
        Me.bbResetAppdb.Id = 303
        Me.bbResetAppdb.Name = "bbResetAppdb"
        '
        'repAPPDBFile
        '
        Me.repAPPDBFile.AutoHeight = False
        Me.repAPPDBFile.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton()})
        Me.repAPPDBFile.Name = "repAPPDBFile"
        '
        'bbiForTesting
        '
        Me.bbiForTesting.Caption = "For Testing Only"
        Me.bbiForTesting.Id = 304
        Me.bbiForTesting.ImageOptions.Image = CType(resources.GetObject("bbiForTesting.ImageOptions.Image"), System.Drawing.Image)
        Me.bbiForTesting.ImageOptions.LargeImage = CType(resources.GetObject("bbiForTesting.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.bbiForTesting.Name = "bbiForTesting"
        '
        'BarButtonItem14
        '
        Me.BarButtonItem14.Caption = "BarButtonItem14"
        Me.BarButtonItem14.Id = 320
        Me.BarButtonItem14.Name = "BarButtonItem14"
        '
        'bbiZoomToPart
        '
        Me.bbiZoomToPart.Edit = Nothing
        Me.bbiZoomToPart.Id = 326
        Me.bbiZoomToPart.Name = "bbiZoomToPart"
        '
        'BarEditItem3
        '
        Me.BarEditItem3.Edit = Nothing
        Me.BarEditItem3.Id = 332
        Me.BarEditItem3.Name = "BarEditItem3"
        '
        'bbiMeasure
        '
        Me.bbiMeasure.Caption = "Measure"
        Me.bbiMeasure.Id = 368
        Me.bbiMeasure.ImageOptions.ImageIndex = 27
        Me.bbiMeasure.Name = "bbiMeasure"
        '
        'puContainerEdit
        '
        Me.puContainerEdit.Caption = "Pattern:"
        Me.puContainerEdit.Edit = Me.RepositoryItemPopupContainerEdit1
        Me.puContainerEdit.EditWidth = 150
        Me.puContainerEdit.Id = 381
        Me.puContainerEdit.Name = "puContainerEdit"
        '
        'RepositoryItemPopupContainerEdit1
        '
        Me.RepositoryItemPopupContainerEdit1.AutoHeight = False
        Me.RepositoryItemPopupContainerEdit1.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.RepositoryItemPopupContainerEdit1.Name = "RepositoryItemPopupContainerEdit1"
        '
        'BarEditItem4
        '
        Me.BarEditItem4.Caption = "BarEditItem4"
        Me.BarEditItem4.Edit = Me.RepositoryItemTextEdit6
        Me.BarEditItem4.Id = 382
        Me.BarEditItem4.Name = "BarEditItem4"
        '
        'RepositoryItemTextEdit6
        '
        Me.RepositoryItemTextEdit6.AutoHeight = False
        Me.RepositoryItemTextEdit6.Name = "RepositoryItemTextEdit6"
        '
        'bbPatternBump
        '
        Me.bbPatternBump.Caption = "Pattern Bump"
        Me.bbPatternBump.Hint = "Use this button to Drag and Drop Patterns on a sheet."
        Me.bbPatternBump.Id = 383
        Me.bbPatternBump.ImageOptions.LargeImage = CType(resources.GetObject("bbPatternBump.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.bbPatternBump.Name = "bbPatternBump"
        '
        'bbShowWelcome
        '
        Me.bbShowWelcome.AllowDrawArrow = False
        Me.bbShowWelcome.AllowDrawArrowInMenu = False
        Me.bbShowWelcome.AllowRightClickInMenu = False
        Me.bbShowWelcome.Caption = "Show Welcome Dialog"
        Me.bbShowWelcome.Id = 385
        Me.bbShowWelcome.ImageOptions.LargeImageIndex = 203
        Me.bbShowWelcome.Name = "bbShowWelcome"
        ToolTipTitleItem2.Text = "Show Welcome Dilaog"
        ToolTipItem2.LeftIndent = 6
        ToolTipItem2.Text = "This button displys the Welcome dialog that appears when you first execute the ap" &
    "plicatuon."
        SuperToolTip2.Items.Add(ToolTipTitleItem2)
        SuperToolTip2.Items.Add(ToolTipItem2)
        Me.bbShowWelcome.SuperTip = SuperToolTip2
        '
        'bbiWECAD
        '
        Me.bbiWECAD.Caption = "WE-CAD"
        Me.bbiWECAD.Id = 386
        Me.bbiWECAD.ImageOptions.LargeImage = CType(resources.GetObject("bbiWECAD.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.bbiWECAD.Name = "bbiWECAD"
        '
        'bbiCutShop
        '
        Me.bbiCutShop.Caption = "Cutting Shop"
        Me.bbiCutShop.Id = 387
        Me.bbiCutShop.ImageOptions.LargeImage = CType(resources.GetObject("bbiCutShop.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.bbiCutShop.Name = "bbiCutShop"
        '
        'bbiDeactivate
        '
        Me.bbiDeactivate.Caption = "Deactivate License"
        Me.bbiDeactivate.Id = 389
        Me.bbiDeactivate.ImageOptions.LargeImageIndex = 206
        Me.bbiDeactivate.Name = "bbiDeactivate"
        ToolTipTitleItem3.Text = "DeActivate"
        ToolTipItem3.LeftIndent = 6
        ToolTipItem3.Text = "Deactivate the licnse on This PC in order to instal WE-CIMl and license a new PC"
        SuperToolTip3.Items.Add(ToolTipTitleItem3)
        SuperToolTip3.Items.Add(ToolTipItem3)
        Me.bbiDeactivate.SuperTip = SuperToolTip3
        '
        'bbiBenchMark
        '
        Me.bbiBenchMark.Caption = "BenchMark Fab"
        Me.bbiBenchMark.Enabled = False
        Me.bbiBenchMark.Id = 391
        Me.bbiBenchMark.ImageOptions.LargeImageIndex = 207
        Me.bbiBenchMark.Name = "bbiBenchMark"
        '
        'BarButtonItem16
        '
        Me.BarButtonItem16.Caption = "Reset PDB in Reg"
        Me.BarButtonItem16.Id = 393
        Me.BarButtonItem16.ImageOptions.Image = CType(resources.GetObject("BarButtonItem16.ImageOptions.Image"), System.Drawing.Image)
        Me.BarButtonItem16.ImageOptions.LargeImage = Global.FabV25_WIN8.My.Resources.Resources.reset
        Me.BarButtonItem16.Name = "BarButtonItem16"
        ToolTipItem4.Text = "Rsets the last PDB setting in the registry"
        SuperToolTip4.Items.Add(ToolTipItem4)
        Me.BarButtonItem16.SuperTip = SuperToolTip4
        '
        'beGraphicsPref
        '
        Me.beGraphicsPref.Caption = "Graphics Delay"
        Me.beGraphicsPref.Edit = Me.repGraphicsPref
        Me.beGraphicsPref.Id = 397
        Me.beGraphicsPref.Name = "beGraphicsPref"
        '
        'repGraphicsPref
        '
        Me.repGraphicsPref.AutoHeight = False
        Me.repGraphicsPref.Name = "repGraphicsPref"
        '
        'BarHeaderItem1
        '
        Me.BarHeaderItem1.Caption = "BarHeaderItem1"
        Me.BarHeaderItem1.Id = 398
        Me.BarHeaderItem1.Name = "BarHeaderItem1"
        '
        'BarButtonItem17
        '
        Me.BarButtonItem17.Caption = "Extra Form4"
        Me.BarButtonItem17.Id = 399
        Me.BarButtonItem17.Name = "BarButtonItem17"
        '
        'BarButtonItem18
        '
        Me.BarButtonItem18.Caption = "Set PartDefaults"
        Me.BarButtonItem18.Hint = "Set the part default values whne adding new parts and the ""ADD PART"" button."
        Me.BarButtonItem18.Id = 400
        Me.BarButtonItem18.ImageOptions.LargeImageIndex = 208
        Me.BarButtonItem18.Name = "BarButtonItem18"
        '
        'bbiFreightCarAmerica
        '
        Me.bbiFreightCarAmerica.Caption = "FreightCar America"
        Me.bbiFreightCarAmerica.Id = 404
        Me.bbiFreightCarAmerica.ImageOptions.LargeImageIndex = 209
        Me.bbiFreightCarAmerica.Name = "bbiFreightCarAmerica"
        '
        'bbiDiamondLife
        '
        Me.bbiDiamondLife.Caption = "diamondLife"
        Me.bbiDiamondLife.Id = 405
        Me.bbiDiamondLife.ImageOptions.LargeImage = Global.FabV25_WIN8.My.Resources.Resources.Diamond_Logo
        Me.bbiDiamondLife.Name = "bbiDiamondLife"
        '
        'bbiTechConnect
        '
        Me.bbiTechConnect.Caption = "Tech Connect"
        Me.bbiTechConnect.Description = "Connect to technical support"
        Me.bbiTechConnect.Id = 406
        Me.bbiTechConnect.ImageOptions.LargeImage = CType(resources.GetObject("bbiTechConnect.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.bbiTechConnect.Name = "bbiTechConnect"
        '
        'BarButtonItem19
        '
        Me.BarButtonItem19.Caption = "Form1 test"
        Me.BarButtonItem19.Id = 409
        Me.BarButtonItem19.Name = "BarButtonItem19"
        '
        'bbiMRP
        '
        Me.bbiMRP.Caption = "MRP"
        Me.bbiMRP.Id = 410
        Me.bbiMRP.ImageOptions.LargeImage = Global.FabV25_WIN8.My.Resources.Resources.MRP_Round
        Me.bbiMRP.Name = "bbiMRP"
        '
        'bbi_Rittal
        '
        Me.bbi_Rittal.Caption = "Rittal"
        Me.bbi_Rittal.Id = 411
        Me.bbi_Rittal.ImageOptions.LargeImage = Global.FabV25_WIN8.My.Resources.Resources.Rittal_logo_1
        Me.bbi_Rittal.Name = "bbi_Rittal"
        '
        'bbiForm1
        '
        Me.bbiForm1.Caption = "Form1"
        Me.bbiForm1.Id = 412
        Me.bbiForm1.Name = "bbiForm1"
        '
        'btmHeatTransfer
        '
        Me.btmHeatTransfer.Caption = "Api Heat Transfer"
        Me.btmHeatTransfer.Id = 413
        Me.btmHeatTransfer.Name = "btmHeatTransfer"
        '
        'BarButtonItem20
        '
        Me.BarButtonItem20.Caption = "extraform3"
        Me.BarButtonItem20.Id = 414
        Me.BarButtonItem20.Name = "BarButtonItem20"
        '
        'BarButtonItem21
        '
        Me.BarButtonItem21.Caption = "MRP Interface"
        Me.BarButtonItem21.Id = 415
        Me.BarButtonItem21.ImageOptions.LargeImageIndex = 212
        Me.BarButtonItem21.Name = "BarButtonItem21"
        '
        'bbiFileManage
        '
        Me.bbiFileManage.Caption = "File Management"
        Me.bbiFileManage.Id = 416
        Me.bbiFileManage.ImageOptions.ImageIndex = 149
        Me.bbiFileManage.ImageOptions.LargeImageIndex = 216
        Me.bbiFileManage.Name = "bbiFileManage"
        '
        'BarButtonItem22
        '
        Me.BarButtonItem22.Caption = "BarButtonItem22"
        Me.BarButtonItem22.Id = 421
        Me.BarButtonItem22.Name = "BarButtonItem22"
        '
        'bbiQuickSave
        '
        Me.bbiQuickSave.Caption = "Quick Save"
        Me.bbiQuickSave.Id = 422
        Me.bbiQuickSave.ImageOptions.ImageIndex = 4
        Me.bbiQuickSave.Name = "bbiQuickSave"
        '
        'bbiSaveAs
        '
        Me.bbiSaveAs.Caption = "Save As"
        Me.bbiSaveAs.Id = 423
        Me.bbiSaveAs.ImageOptions.ImageIndex = 5
        Me.bbiSaveAs.Name = "bbiSaveAs"
        '
        'bbiPartExtents
        '
        Me.bbiPartExtents.Caption = "Part Extents"
        Me.bbiPartExtents.Description = "VIew the Extents of the part"
        Me.bbiPartExtents.Id = 424
        Me.bbiPartExtents.ImageOptions.LargeImage = Global.FabV25_WIN8.My.Resources.Resources.ruler
        Me.bbiPartExtents.Name = "bbiPartExtents"
        '
        'ProfileBlend
        '
        Me.ProfileBlend.Caption = "Profile blend"
        Me.ProfileBlend.Id = 425
        Me.ProfileBlend.Name = "ProfileBlend"
        '
        'PrintEntityList
        '
        Me.PrintEntityList.Caption = "Entity List"
        Me.PrintEntityList.Id = 426
        Me.PrintEntityList.ImageOptions.Image = CType(resources.GetObject("PrintEntityList.ImageOptions.Image"), System.Drawing.Image)
        Me.PrintEntityList.ImageOptions.LargeImage = CType(resources.GetObject("PrintEntityList.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.PrintEntityList.Name = "PrintEntityList"
        '
        'PrintCrossData
        '
        Me.PrintCrossData.Caption = "Print Cross Data"
        Me.PrintCrossData.Id = 427
        Me.PrintCrossData.Name = "PrintCrossData"
        '
        'bbiProfileBlend
        '
        Me.bbiProfileBlend.Caption = "Profile Blend"
        Me.bbiProfileBlend.Id = 428
        Me.bbiProfileBlend.ImageOptions.LargeImageIndex = 219
        Me.bbiProfileBlend.Name = "bbiProfileBlend"
        '
        'bbiFlipSheet
        '
        Me.bbiFlipSheet.Caption = "Flip Sheet"
        Me.bbiFlipSheet.Id = 429
        Me.bbiFlipSheet.ImageOptions.LargeImageIndex = 221
        Me.bbiFlipSheet.Name = "bbiFlipSheet"
        '
        'bbiMaterialList
        '
        Me.bbiMaterialList.Caption = "Material List"
        Me.bbiMaterialList.Id = 430
        Me.bbiMaterialList.ImageOptions.LargeImageIndex = 222
        Me.bbiMaterialList.Name = "bbiMaterialList"
        '
        'BarButtonItem23
        '
        Me.BarButtonItem23.Caption = "BarButtonItem23"
        Me.BarButtonItem23.Id = 432
        Me.BarButtonItem23.Name = "BarButtonItem23"
        '
        'BarStaticItem4
        '
        Me.BarStaticItem4.Caption = "BarStaticItem4"
        Me.BarStaticItem4.Id = 433
        Me.BarStaticItem4.Name = "BarStaticItem4"
        '
        'bbZoomToPart
        '
        Me.bbZoomToPart.Caption = "Zoom To Part"
        Me.bbZoomToPart.Id = 434
        Me.bbZoomToPart.ImageOptions.Image = CType(resources.GetObject("bbZoomToPart.ImageOptions.Image"), System.Drawing.Image)
        Me.bbZoomToPart.ImageOptions.LargeImage = CType(resources.GetObject("bbZoomToPart.ImageOptions.LargeImage"), System.Drawing.Image)
        Me.bbZoomToPart.Name = "bbZoomToPart"
        '
        'ribbonImageCollectionLarge
        '
        Me.ribbonImageCollectionLarge.ImageSize = New System.Drawing.Size(32, 32)
        Me.ribbonImageCollectionLarge.ImageStream = CType(resources.GetObject("ribbonImageCollectionLarge.ImageStream"), DevExpress.Utils.ImageCollectionStreamer)
        Me.ribbonImageCollectionLarge.Images.SetKeyName(0, "Ribbon_New_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(1, "Ribbon_Open_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(2, "Ribbon_Close_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(3, "Ribbon_Find_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(4, "Ribbon_Save_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(5, "Ribbon_SaveAs_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(6, "Ribbon_Exit_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(7, "Ribbon_Content_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(8, "Ribbon_Info_32x32.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(9, "1_13.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(10, "opentoolbox.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(11, "imagesCA00WBPL.jpg")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(12, "modifyEntity.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(13, "dropdoor.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(14, "Relationship.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(15, "flowchart.jpg")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(16, "entitylist.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(17, "delete.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(18, "properties.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(19, "measure.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(20, "hotdot.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(21, "zoomfull.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(22, "zoomin.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(23, "zoomout.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(24, "zoomprevious.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(25, "zoomrefresh.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(26, "zoomwindow.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(27, "zones.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(28, "projects.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(29, "PartOutline.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(30, "chamfer.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(31, "tools.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(32, "layers.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(33, "dropStop.jpg")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(34, "FeatureRemove.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(35, "featureInsert.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(36, "FeatureExtract.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(37, "FeatureAdd.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(38, "tnt.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(39, "editlabel.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(40, "hole_pattern.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(41, "edit_entity.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(42, "attributes.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(43, "Tool_Layer.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(44, "trash_full.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(45, "redo.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(46, "undo.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(47, "redo1 (2).png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(48, "DocumentImport.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(49, "Undo1 (1).png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(50, "RecordDel.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(51, "zones.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(52, "PartOutline.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(53, "chamfer.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(54, "undo.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(55, "redo.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(56, "Print_tool_Report.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(57, "16x16_Open_Pdb.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(58, "24x24_openPDB.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(59, "zoomout.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(60, "ZoomIn.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(61, "offset.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(62, "Unchain.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(63, "Split.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(64, "Fillet.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(65, "TrimExtend.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(66, "delete.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(67, "editholepattern.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(68, "explode.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(69, "Change_tool.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(70, "Copy.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(71, "array.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(72, "mirror.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(73, "changelayer.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(74, "Scale.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(75, "reverseorder.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(76, "Rotate.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(77, "Chain.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(78, "Move.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(79, "SelectByWindow.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(80, "SelectByFeature.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(81, "SelectProfileFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(82, "HotSpot.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(83, "deselectall.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(84, "EnableSelection.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(85, "SelectAll.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(86, "deselectAllByFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(87, "SelectAllbyFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(88, "SelectCommandFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(89, "SelectPointFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(90, "SelectToolFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(91, "SelectLayerFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(92, "SelectLineFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(93, "SelectArcFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(94, "SelectHoleFilter.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(95, "Arborimage.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(96, "Code Preview.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(97, "ShapeLibrary.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(98, "Code Generation.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(99, "Configuration Manager.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(100, "CreateHole.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(101, "CreateArc.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(102, "Nesting.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(103, "Barcode (2).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(104, "line.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(105, "Hammer (3).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(106, "Folder (4).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(107, "Chart Down.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(108, "New.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(109, "Eraser (3).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(110, "OK (6).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(111, "Book Search (2).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(112, "Gear (2).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(113, "Floppy - 3½ Disk.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(114, "Blocks.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(115, "Molecule Box.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(116, "Flash Drive.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(117, "config.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(118, "Gear (32).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(119, "Folder (1) (open).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(120, "LayoutParagraph.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(121, "Notepad (4).ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(122, "Lightning.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(123, "Explorer.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(124, "DocumentFromTemplate.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(126, "delete.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(127, "icon_magic[1].png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(128, "disassociate.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(129, "Zoom11.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(130, "zoomnext.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(131, "SearchDocument.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(132, "TravelDistance.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(133, "highway_icon.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(134, "monitor.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(135, "line.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(136, "arc.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(137, "hole.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(138, "point.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(139, "rubberband.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(140, "boundingbox.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(141, "Associate.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(142, "autotool.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(143, "LayerMap.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(144, "manual_leadin.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(145, "AutoLead.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(146, "notch.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(147, "shakertab.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(148, "Command.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(149, "Slit.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(150, "linearclear.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(151, "areaclear.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(152, "ShapeLibrary.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(153, "cutback.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(154, "dropstop.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(155, "chaincut.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(156, "champher.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(157, "fillert.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(158, "bump.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(159, "Rotate.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(160, "zones.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(161, "mirror.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(162, "Split.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(163, "TrimExtend.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(164, "Copy.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(165, "Move.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(166, "Scale.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(167, "extract.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(168, "clamps.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(169, "reposition.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(170, "editProject.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(171, "IndvHits.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(172, "Unchain.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(173, "Chain.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(174, "reverseorder.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(175, "sequenceorder.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(176, "ConfigMan.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(177, "Nesting.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(178, "CodePreview.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(179, "CadToCode.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(180, "Remnant.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(181, "MacExecute.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(182, "MacConfig.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(183, "CreatePattern.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(184, "CreateInstance.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(185, "GridNest.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(186, "StaggerNest.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(187, "SkeletonCutOff.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(188, "NestSettings.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(189, "EWM.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(190, "48X48DumpTruck.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(191, "player_play (1).png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(192, "player_stop.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(193, "player_record.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(194, "portal.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(195, "softwarebug.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(196, "ManPushingPuzzle.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(197, "bump4.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(198, "Layers.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(199, "ToolBox.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(200, "welcome.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(201, "welcome_title.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(202, "welcome_icon.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(203, "Wood_Welcome.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(204, "450px-imagem_2.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(205, "Auto_nesting3.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(206, "DeActivateLicense.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(207, "bm.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(208, "PartDefaults.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(209, "FreightCar_logo.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(210, "Diamond_Logo.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(211, "Tech_Connect.png")
        Me.ribbonImageCollectionLarge.InsertImage(Global.FabV25_WIN8.My.Resources.Resources.MRP_Round, "MRP_Round", GetType(Global.FabV25_WIN8.My.Resources.Resources), 212)
        Me.ribbonImageCollectionLarge.Images.SetKeyName(212, "MRP_Round")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(213, "apilogo2.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(214, "FileCabinet")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(215, "cabinet-icon-9272.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(216, "FileCab_48X48.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(217, "ruler.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(218, "ProfilefilletB.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(219, "ProfBlend2.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(220, "coin-toss.png")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(221, "flipsheet.ico")
        Me.ribbonImageCollectionLarge.Images.SetKeyName(222, "SteelCoil.png")
        '
        'homeRibbonPage
        '
        Me.homeRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.ConfigPageGroup, Me.fileRibbonPageGroup, Me.exportRibbonPageGroup, Me.rpgImport, Me.printRibbonPageGroup, Me.rpgSetToolLayer, Me.skinsRibbonPageGroup, Me.exitRibbonPageGroup, Me.rpgForTesting, Me.rpgBenchMark})
        Me.homeRibbonPage.Name = "homeRibbonPage"
        Me.homeRibbonPage.Text = "Home"
        '
        'ConfigPageGroup
        '
        Me.ConfigPageGroup.ItemLinks.Add(Me.bbConfigMan)
        Me.ConfigPageGroup.Name = "ConfigPageGroup"
        Me.ConfigPageGroup.ShowCaptionButton = False
        Me.ConfigPageGroup.Text = "Configuration"
        '
        'fileRibbonPageGroup
        '
        Me.fileRibbonPageGroup.ItemLinks.Add(Me.iNew)
        Me.fileRibbonPageGroup.ItemLinks.Add(Me.iOpen)
        Me.fileRibbonPageGroup.ItemLinks.Add(Me.iMerge)
        Me.fileRibbonPageGroup.ItemLinks.Add(Me.iOpenPDB)
        Me.fileRibbonPageGroup.Name = "fileRibbonPageGroup"
        Me.fileRibbonPageGroup.ShowCaptionButton = False
        Me.fileRibbonPageGroup.Text = "File"
        '
        'exportRibbonPageGroup
        '
        Me.exportRibbonPageGroup.ItemLinks.Add(Me.iSave)
        Me.exportRibbonPageGroup.ItemLinks.Add(Me.iSaveAs)
        Me.exportRibbonPageGroup.ItemLinks.Add(Me.iExportCNC)
        Me.exportRibbonPageGroup.Name = "exportRibbonPageGroup"
        Me.exportRibbonPageGroup.ShowCaptionButton = False
        Me.exportRibbonPageGroup.Text = "Export"
        '
        'rpgImport
        '
        Me.rpgImport.ItemLinks.Add(Me.BarButtonItem21)
        Me.rpgImport.Name = "rpgImport"
        Me.rpgImport.Text = "Import"
        '
        'printRibbonPageGroup
        '
        Me.printRibbonPageGroup.ItemLinks.Add(Me.iPrintGraphics)
        Me.printRibbonPageGroup.ItemLinks.Add(Me.iPrintTools)
        Me.printRibbonPageGroup.ItemLinks.Add(Me.iPrintNest)
        Me.printRibbonPageGroup.ItemLinks.Add(Me.bbiMaterialList)
        Me.printRibbonPageGroup.ItemLinks.Add(Me.PrintEntityList)
        Me.printRibbonPageGroup.Name = "printRibbonPageGroup"
        Me.printRibbonPageGroup.ShowCaptionButton = False
        Me.printRibbonPageGroup.Text = "Printing"
        '
        'rpgSetToolLayer
        '
        Me.rpgSetToolLayer.ItemLinks.Add(Me.bbiTools)
        Me.rpgSetToolLayer.ItemLinks.Add(Me.bbiSetLayer)
        Me.rpgSetToolLayer.Name = "rpgSetToolLayer"
        Me.rpgSetToolLayer.ShowCaptionButton = False
        Me.rpgSetToolLayer.Text = "Set Active Tool/Layer"
        '
        'skinsRibbonPageGroup
        '
        Me.skinsRibbonPageGroup.ItemLinks.Add(Me.rgbiSkins)
        Me.skinsRibbonPageGroup.Name = "skinsRibbonPageGroup"
        Me.skinsRibbonPageGroup.ShowCaptionButton = False
        Me.skinsRibbonPageGroup.Text = "Skins"
        '
        'exitRibbonPageGroup
        '
        Me.exitRibbonPageGroup.ItemLinks.Add(Me.iExit)
        Me.exitRibbonPageGroup.Name = "exitRibbonPageGroup"
        Me.exitRibbonPageGroup.ShowCaptionButton = False
        Me.exitRibbonPageGroup.Text = "Exit"
        '
        'rpgForTesting
        '
        Me.rpgForTesting.AllowTextClipping = False
        Me.rpgForTesting.ItemLinks.Add(Me.bbiForTesting)
        Me.rpgForTesting.Name = "rpgForTesting"
        Me.rpgForTesting.ShowCaptionButton = False
        Me.rpgForTesting.Text = "For Testing Only"
        Me.rpgForTesting.Visible = False
        '
        'rpgBenchMark
        '
        Me.rpgBenchMark.AllowTextClipping = False
        Me.rpgBenchMark.ItemLinks.Add(Me.bbiBenchMark)
        Me.rpgBenchMark.Name = "rpgBenchMark"
        Me.rpgBenchMark.ShowCaptionButton = False
        Me.rpgBenchMark.State = DevExpress.XtraBars.Ribbon.RibbonPageGroupState.Collapsed
        Me.rpgBenchMark.Text = "BenchMark Fab"
        Me.rpgBenchMark.Visible = False
        '
        'ribNesting
        '
        Me.ribNesting.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.rgTrueShape, Me.rbgManualNesting, Me.rbgSkeletonCutOff, Me.rbNestingDefaults})
        Me.ribNesting.Name = "ribNesting"
        Me.ribNesting.Text = "Nesting"
        '
        'rgTrueShape
        '
        Me.rgTrueShape.AllowTextClipping = False
        Me.rgTrueShape.ItemLinks.Add(Me.bbTrueShape, "N")
        Me.rgTrueShape.Name = "rgTrueShape"
        Me.rgTrueShape.ShowCaptionButton = False
        Me.rgTrueShape.Text = "True Shape"
        '
        'rbgManualNesting
        '
        Me.rbgManualNesting.ItemLinks.Add(Me.bbManulNestingGrid)
        Me.rbgManualNesting.ItemLinks.Add(Me.bbStaggerNest)
        Me.rbgManualNesting.ItemLinks.Add(Me.bbManualNestAddPart)
        Me.rbgManualNesting.ItemLinks.Add(Me.bbPatternBump, "B")
        Me.rbgManualNesting.Name = "rbgManualNesting"
        Me.rbgManualNesting.ShowCaptionButton = False
        Me.rbgManualNesting.Text = "Manual Nesting"
        '
        'rbgSkeletonCutOff
        '
        Me.rbgSkeletonCutOff.AllowTextClipping = False
        Me.rbgSkeletonCutOff.ItemLinks.Add(Me.bbSkeltonCutOff)
        Me.rbgSkeletonCutOff.Name = "rbgSkeletonCutOff"
        Me.rbgSkeletonCutOff.ShowCaptionButton = False
        Me.rbgSkeletonCutOff.State = DevExpress.XtraBars.Ribbon.RibbonPageGroupState.Expanded
        Me.rbgSkeletonCutOff.Text = "Skeleton"
        '
        'rbNestingDefaults
        '
        Me.rbNestingDefaults.AllowTextClipping = False
        Me.rbNestingDefaults.ItemLinks.Add(Me.bbNestingDefaults)
        Me.rbNestingDefaults.ItemLinks.Add(Me.BarButtonItem18)
        Me.rbNestingDefaults.Name = "rbNestingDefaults"
        Me.rbNestingDefaults.ShowCaptionButton = False
        Me.rbNestingDefaults.State = DevExpress.XtraBars.Ribbon.RibbonPageGroupState.Expanded
        Me.rbNestingDefaults.Text = "Set Nesting Defaults"
        '
        'EditRibbonPage
        '
        Me.EditRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.UndoPageGroup, Me.RedoPageGroup, Me.RemovePageGroup, Me.ChnagePageGroup, Me.PatternRibbonPageGroup, Me.FeaturesPageGroup, Me.ProjectPageGroup})
        Me.EditRibbonPage.Name = "EditRibbonPage"
        Me.EditRibbonPage.Text = "Edit"
        '
        'UndoPageGroup
        '
        Me.UndoPageGroup.ItemLinks.Add(Me.iUndo)
        Me.UndoPageGroup.Name = "UndoPageGroup"
        Me.UndoPageGroup.ShowCaptionButton = False
        Me.UndoPageGroup.Text = "Undo"
        '
        'RedoPageGroup
        '
        Me.RedoPageGroup.ItemLinks.Add(Me.iRedo)
        Me.RedoPageGroup.Name = "RedoPageGroup"
        Me.RedoPageGroup.ShowCaptionButton = False
        Me.RedoPageGroup.Text = "Redo"
        '
        'RemovePageGroup
        '
        Me.RemovePageGroup.ItemLinks.Add(Me.iDelete)
        Me.RemovePageGroup.ItemLinks.Add(Me.iPurge)
        Me.RemovePageGroup.Name = "RemovePageGroup"
        Me.RemovePageGroup.ShowCaptionButton = False
        Me.RemovePageGroup.Text = "Remove Entities"
        '
        'ChnagePageGroup
        '
        Me.ChnagePageGroup.ItemLinks.Add(Me.iChangeTool)
        Me.ChnagePageGroup.ItemLinks.Add(Me.iChangeAttrib)
        Me.ChnagePageGroup.ItemLinks.Add(Me.iChangeEntities)
        Me.ChnagePageGroup.ItemLinks.Add(Me.iDisAssociate)
        Me.ChnagePageGroup.Name = "ChnagePageGroup"
        Me.ChnagePageGroup.ShowCaptionButton = False
        Me.ChnagePageGroup.Text = "Change"
        '
        'PatternRibbonPageGroup
        '
        Me.PatternRibbonPageGroup.ItemLinks.Add(Me.iHolePattern)
        Me.PatternRibbonPageGroup.ItemLinks.Add(Me.iLabels)
        Me.PatternRibbonPageGroup.ItemLinks.Add(Me.iDeletePattern)
        Me.PatternRibbonPageGroup.ItemLinks.Add(Me.iExplodePattern)
        Me.PatternRibbonPageGroup.Name = "PatternRibbonPageGroup"
        Me.PatternRibbonPageGroup.ShowCaptionButton = False
        Me.PatternRibbonPageGroup.Text = "Patterns"
        '
        'FeaturesPageGroup
        '
        Me.FeaturesPageGroup.ItemLinks.Add(Me.iFeatureAdd)
        Me.FeaturesPageGroup.ItemLinks.Add(Me.iFeatureExtract)
        Me.FeaturesPageGroup.ItemLinks.Add(Me.iFeatureInsert)
        Me.FeaturesPageGroup.ItemLinks.Add(Me.iFeatureRemove)
        Me.FeaturesPageGroup.Name = "FeaturesPageGroup"
        Me.FeaturesPageGroup.ShowCaptionButton = False
        Me.FeaturesPageGroup.Text = "Features"
        '
        'ProjectPageGroup
        '
        Me.ProjectPageGroup.ItemLinks.Add(Me.bbEditProject)
        Me.ProjectPageGroup.Name = "ProjectPageGroup"
        Me.ProjectPageGroup.ShowCaptionButton = False
        Me.ProjectPageGroup.Text = "Project"
        '
        'ViewRibbonPage
        '
        Me.ViewRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.ViewZoomPageGroup, Me.ViewDisplayPageGroup, Me.ViewOptionsPageGroup1})
        Me.ViewRibbonPage.Name = "ViewRibbonPage"
        Me.ViewRibbonPage.Text = "View"
        '
        'ViewZoomPageGroup
        '
        Me.ViewZoomPageGroup.ItemLinks.Add(Me.bbViewFull)
        Me.ViewZoomPageGroup.ItemLinks.Add(Me.bbViewWindow)
        Me.ViewZoomPageGroup.ItemLinks.Add(Me.bbViewZoomIn)
        Me.ViewZoomPageGroup.ItemLinks.Add(Me.bbViewZoomOut)
        Me.ViewZoomPageGroup.ItemLinks.Add(Me.bbViewPrevious)
        Me.ViewZoomPageGroup.ItemLinks.Add(Me.bbViewRefresh)
        Me.ViewZoomPageGroup.Name = "ViewZoomPageGroup"
        Me.ViewZoomPageGroup.ShowCaptionButton = False
        Me.ViewZoomPageGroup.Text = "Zoom"
        '
        'ViewDisplayPageGroup
        '
        Me.ViewDisplayPageGroup.ItemLinks.Add(Me.bbViewEntityList)
        Me.ViewDisplayPageGroup.ItemLinks.Add(Me.bbViewCodeViewer)
        Me.ViewDisplayPageGroup.ItemLinks.Add(Me.bbViewHotspot)
        Me.ViewDisplayPageGroup.ItemLinks.Add(Me.sbViewOptions)
        Me.ViewDisplayPageGroup.Name = "ViewDisplayPageGroup"
        Me.ViewDisplayPageGroup.ShowCaptionButton = False
        Me.ViewDisplayPageGroup.Text = "Display"
        '
        'ViewOptionsPageGroup1
        '
        Me.ViewOptionsPageGroup1.ItemLinks.Add(Me.bbViewTravel)
        Me.ViewOptionsPageGroup1.ItemLinks.Add(Me.bbShowWelcome)
        Me.ViewOptionsPageGroup1.ItemLinks.Add(Me.bbiPartExtents)
        Me.ViewOptionsPageGroup1.Name = "ViewOptionsPageGroup1"
        Me.ViewOptionsPageGroup1.ShowCaptionButton = False
        Me.ViewOptionsPageGroup1.Text = "Misc."
        '
        'CreateRibbonPage
        '
        Me.CreateRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.CreateGeoPageGroup1, Me.ToolpathPageGroup, Me.PartOutlinePageGroup, Me.PatternGroup})
        Me.CreateRibbonPage.Name = "CreateRibbonPage"
        Me.CreateRibbonPage.Text = "Create"
        '
        'CreateGeoPageGroup1
        '
        Me.CreateGeoPageGroup1.ItemLinks.Add(Me.bbCreateLine)
        Me.CreateGeoPageGroup1.ItemLinks.Add(Me.bbCreateArc)
        Me.CreateGeoPageGroup1.ItemLinks.Add(Me.bbCreateHole)
        Me.CreateGeoPageGroup1.ItemLinks.Add(Me.bbCreatePoint)
        Me.CreateGeoPageGroup1.ItemLinks.Add(Me.bbShapeLIbrary)
        Me.CreateGeoPageGroup1.ItemLinks.Add(Me.bbWallOfset)
        Me.CreateGeoPageGroup1.Name = "CreateGeoPageGroup1"
        Me.CreateGeoPageGroup1.ShowCaptionButton = False
        Me.CreateGeoPageGroup1.Text = "Geometry"
        '
        'ToolpathPageGroup
        '
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbAssociate)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbAutoPunch)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbManualLead)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbAutoLead)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbNotch)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbShakerTab)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbCommad)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbSlit)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbLinearCler)
        Me.ToolpathPageGroup.ItemLinks.Add(Me.bbAreaClear)
        Me.ToolpathPageGroup.Name = "ToolpathPageGroup"
        Me.ToolpathPageGroup.ShowCaptionButton = False
        Me.ToolpathPageGroup.Text = "Auto Toolpath"
        '
        'PartOutlinePageGroup
        '
        Me.PartOutlinePageGroup.ItemLinks.Add(Me.bbRubberBand)
        Me.PartOutlinePageGroup.ItemLinks.Add(Me.bbCreateBoundingBox)
        Me.PartOutlinePageGroup.Name = "PartOutlinePageGroup"
        Me.PartOutlinePageGroup.ShowCaptionButton = False
        Me.PartOutlinePageGroup.Text = "Part Outline"
        '
        'PatternGroup
        '
        Me.PatternGroup.ItemLinks.Add(Me.bbCreatePattern)
        Me.PatternGroup.ItemLinks.Add(Me.bbCreateInstance)
        Me.PatternGroup.Name = "PatternGroup"
        Me.PatternGroup.ShowCaptionButton = False
        Me.PatternGroup.Text = "Patterns"
        '
        'ModifyRibbonPage
        '
        Me.ModifyRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.TransformPageGroup, Me.ModGeoPageGroup, Me.ModToolpathPageGroup, Me.ZonesPageGroup})
        Me.ModifyRibbonPage.Name = "ModifyRibbonPage"
        Me.ModifyRibbonPage.Text = "Modify"
        '
        'TransformPageGroup
        '
        Me.TransformPageGroup.ItemLinks.Add(Me.bbTransformMove)
        Me.TransformPageGroup.ItemLinks.Add(Me.bbTransFormCopy)
        Me.TransformPageGroup.ItemLinks.Add(Me.bbTransFormScale)
        Me.TransformPageGroup.ItemLinks.Add(Me.bbTRansformMirror)
        Me.TransformPageGroup.ItemLinks.Add(Me.bbTransformRotate)
        Me.TransformPageGroup.Name = "TransformPageGroup"
        Me.TransformPageGroup.ShowCaptionButton = False
        Me.TransformPageGroup.Text = "Transform"
        '
        'ModGeoPageGroup
        '
        Me.ModGeoPageGroup.ItemLinks.Add(Me.bbModTrimExtend)
        Me.ModGeoPageGroup.ItemLinks.Add(Me.bbModSplit)
        Me.ModGeoPageGroup.ItemLinks.Add(Me.bbModFillet)
        Me.ModGeoPageGroup.ItemLinks.Add(Me.bbModChampher)
        Me.ModGeoPageGroup.ItemLinks.Add(Me.bbiProfileBlend)
        Me.ModGeoPageGroup.Name = "ModGeoPageGroup"
        Me.ModGeoPageGroup.ShowCaptionButton = False
        Me.ModGeoPageGroup.Text = "Geometry"
        '
        'ModToolpathPageGroup
        '
        Me.ModToolpathPageGroup.ItemLinks.Add(Me.bbModDropStop)
        Me.ModToolpathPageGroup.ItemLinks.Add(Me.bbModChainCut)
        Me.ModToolpathPageGroup.ItemLinks.Add(Me.bbModCutBack)
        Me.ModToolpathPageGroup.ItemLinks.Add(Me.bbModIndvHIts)
        Me.ModToolpathPageGroup.ItemLinks.Add(Me.bbiFlipSheet)
        Me.ModToolpathPageGroup.Name = "ModToolpathPageGroup"
        Me.ModToolpathPageGroup.ShowCaptionButton = False
        Me.ModToolpathPageGroup.Text = "Toolpath"
        '
        'ZonesPageGroup
        '
        Me.ZonesPageGroup.ItemLinks.Add(Me.bbModZonesManage)
        Me.ZonesPageGroup.ItemLinks.Add(Me.bbModZonesExtract)
        Me.ZonesPageGroup.ItemLinks.Add(Me.bbModClamps)
        Me.ZonesPageGroup.Name = "ZonesPageGroup"
        Me.ZonesPageGroup.ShowCaptionButton = False
        Me.ZonesPageGroup.Text = "Zones"
        '
        'SequenceRibbonPage
        '
        Me.SequenceRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.SequencePageGroup, Me.ManualSeqPageGroup})
        Me.SequenceRibbonPage.Name = "SequenceRibbonPage"
        Me.SequenceRibbonPage.Text = "Sequence"
        '
        'SequencePageGroup
        '
        Me.SequencePageGroup.ItemLinks.Add(Me.bbSequenceChain)
        Me.SequencePageGroup.ItemLinks.Add(Me.bbSequemceUnchain)
        Me.SequencePageGroup.ItemLinks.Add(Me.bbSequenceRevOrder)
        Me.SequencePageGroup.Name = "SequencePageGroup"
        Me.SequencePageGroup.ShowCaptionButton = False
        Me.SequencePageGroup.Text = "Sequence"
        '
        'ManualSeqPageGroup
        '
        Me.ManualSeqPageGroup.ItemLinks.Add(Me.bbSeqManOrder)
        Me.ManualSeqPageGroup.Name = "ManualSeqPageGroup"
        Me.ManualSeqPageGroup.ShowCaptionButton = False
        Me.ManualSeqPageGroup.Text = "Manual"
        '
        'ribProcess
        '
        Me.ribProcess.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.CadToCodePageGroup, Me.RemnantPageGroup, Me.rpgOptions, Me.rpgCustomization})
        Me.ribProcess.Name = "ribProcess"
        Me.ribProcess.Text = "Process"
        '
        'CadToCodePageGroup
        '
        Me.CadToCodePageGroup.ItemLinks.Add(Me.bbCadToCode)
        Me.CadToCodePageGroup.Name = "CadToCodePageGroup"
        Me.CadToCodePageGroup.ShowCaptionButton = False
        Me.CadToCodePageGroup.Text = "Convert"
        '
        'RemnantPageGroup
        '
        Me.RemnantPageGroup.ItemLinks.Add(Me.bbRemnant)
        Me.RemnantPageGroup.Name = "RemnantPageGroup"
        Me.RemnantPageGroup.ShowCaptionButton = False
        Me.RemnantPageGroup.Text = "Remnants"
        '
        'rpgOptions
        '
        Me.rpgOptions.ItemLinks.Add(Me.bbiWECAD)
        Me.rpgOptions.ItemLinks.Add(Me.bbiCutShop)
        Me.rpgOptions.Name = "rpgOptions"
        Me.rpgOptions.Text = "Options"
        '
        'rpgCustomization
        '
        Me.rpgCustomization.AllowTextClipping = False
        Me.rpgCustomization.ItemLinks.Add(Me.bbiFreightCarAmerica)
        Me.rpgCustomization.ItemLinks.Add(Me.bbiDiamondLife)
        Me.rpgCustomization.ItemLinks.Add(Me.bbi_Rittal)
        Me.rpgCustomization.ItemLinks.Add(Me.bbiFileManage)
        Me.rpgCustomization.Name = "rpgCustomization"
        Me.rpgCustomization.Text = "Customization"
        '
        'MacroRibbonPage
        '
        Me.MacroRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.MacroPageGroup, Me.rpgCustomMacros})
        Me.MacroRibbonPage.Name = "MacroRibbonPage"
        Me.MacroRibbonPage.Text = "Macro"
        '
        'MacroPageGroup
        '
        Me.MacroPageGroup.AllowTextClipping = False
        Me.MacroPageGroup.ItemLinks.Add(Me.bbMacroExecute)
        Me.MacroPageGroup.Name = "MacroPageGroup"
        Me.MacroPageGroup.ShowCaptionButton = False
        Me.MacroPageGroup.Text = "Macro Execute"
        '
        'rpgCustomMacros
        '
        Me.rpgCustomMacros.Name = "rpgCustomMacros"
        ToolTipTitleItem4.Text = "Add an Icon"
        ToolTipItem5.LeftIndent = 6
        ToolTipItem5.Text = "Click the arrows in the lower right corner to add " & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Macros to the ToolBar"
        SuperToolTip5.Items.Add(ToolTipTitleItem4)
        SuperToolTip5.Items.Add(ToolTipItem5)
        Me.rpgCustomMacros.SuperTip = SuperToolTip5
        Me.rpgCustomMacros.Text = "Custom Macros"
        '
        'AdminRibbonPage
        '
        Me.AdminRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.rpgEWM, Me.rpgLogFile, Me.rpgPortal, Me.rpgCMDB, Me.rpgGraphicsPref})
        Me.AdminRibbonPage.Name = "AdminRibbonPage"
        Me.AdminRibbonPage.Text = "Admin"
        '
        'rpgEWM
        '
        Me.rpgEWM.ItemLinks.Add(Me.bbEWM)
        Me.rpgEWM.ItemLinks.Add(Me.bbAdminDumpModel)
        Me.rpgEWM.Name = "rpgEWM"
        Me.rpgEWM.ShowCaptionButton = False
        Me.rpgEWM.Text = "Settings"
        '
        'rpgLogFile
        '
        Me.rpgLogFile.ItemLinks.Add(Me.bbLogFileRecord)
        Me.rpgLogFile.ItemLinks.Add(Me.bbLogFileStop)
        Me.rpgLogFile.ItemLinks.Add(Me.bbbLogFilePlay)
        Me.rpgLogFile.Name = "rpgLogFile"
        Me.rpgLogFile.ShowCaptionButton = False
        Me.rpgLogFile.Text = "Log File"
        '
        'rpgPortal
        '
        Me.rpgPortal.AllowTextClipping = False
        Me.rpgPortal.ItemLinks.Add(Me.bbExecutePortal)
        Me.rpgPortal.ItemLinks.Add(Me.BarButtonItem17)
        Me.rpgPortal.ItemLinks.Add(Me.BarButtonItem19)
        Me.rpgPortal.ItemLinks.Add(Me.bbiForm1)
        Me.rpgPortal.ItemLinks.Add(Me.btmHeatTransfer)
        Me.rpgPortal.ItemLinks.Add(Me.BarButtonItem20)
        Me.rpgPortal.ItemLinks.Add(Me.ProfileBlend)
        Me.rpgPortal.ItemLinks.Add(Me.PrintCrossData)
        Me.rpgPortal.Name = "rpgPortal"
        Me.rpgPortal.ShowCaptionButton = False
        Me.rpgPortal.State = DevExpress.XtraBars.Ribbon.RibbonPageGroupState.Expanded
        Me.rpgPortal.Text = "Portal"
        '
        'rpgCMDB
        '
        Me.rpgCMDB.AllowTextClipping = False
        Me.rpgCMDB.ItemLinks.Add(Me.bbCMDB)
        Me.rpgCMDB.ItemLinks.Add(Me.bbResetAppdb)
        Me.rpgCMDB.ItemLinks.Add(Me.BarButtonItem16)
        Me.rpgCMDB.Name = "rpgCMDB"
        Me.rpgCMDB.ShowCaptionButton = False
        Me.rpgCMDB.Text = "Reset CMDB"
        '
        'rpgGraphicsPref
        '
        Me.rpgGraphicsPref.ItemLinks.Add(Me.beGraphicsPref)
        Me.rpgGraphicsPref.Name = "rpgGraphicsPref"
        Me.rpgGraphicsPref.ShowCaptionButton = False
        Me.rpgGraphicsPref.State = DevExpress.XtraBars.Ribbon.RibbonPageGroupState.Expanded
        Me.rpgGraphicsPref.Text = "Graphics Preferences"
        '
        'helpRibbonPage
        '
        Me.helpRibbonPage.Groups.AddRange(New DevExpress.XtraBars.Ribbon.RibbonPageGroup() {Me.helpRibbonPageGroup, Me.rpgTechSupport})
        Me.helpRibbonPage.Name = "helpRibbonPage"
        Me.helpRibbonPage.Text = "Help"
        '
        'helpRibbonPageGroup
        '
        Me.helpRibbonPageGroup.ItemLinks.Add(Me.iHelp)
        Me.helpRibbonPageGroup.ItemLinks.Add(Me.iAbout)
        Me.helpRibbonPageGroup.ItemLinks.Add(Me.bbiDeactivate)
        Me.helpRibbonPageGroup.Name = "helpRibbonPageGroup"
        Me.helpRibbonPageGroup.ShowCaptionButton = False
        Me.helpRibbonPageGroup.Text = "Help"
        '
        'rpgTechSupport
        '
        Me.rpgTechSupport.AllowTextClipping = False
        Me.rpgTechSupport.ItemLinks.Add(Me.BarButtonItem13)
        Me.rpgTechSupport.ItemLinks.Add(Me.bbiTechConnect)
        Me.rpgTechSupport.Name = "rpgTechSupport"
        Me.rpgTechSupport.ShowCaptionButton = False
        Me.rpgTechSupport.State = DevExpress.XtraBars.Ribbon.RibbonPageGroupState.Expanded
        Me.rpgTechSupport.Text = "Technical Support"
        '
        'repHandleX
        '
        Me.repHandleX.AutoHeight = False
        Me.repHandleX.DisplayFormat.FormatString = "#####.#####"
        Me.repHandleX.DisplayFormat.FormatType = DevExpress.Utils.FormatType.Numeric
        Me.repHandleX.EditFormat.FormatString = "#####.#####"
        Me.repHandleX.EditFormat.FormatType = DevExpress.Utils.FormatType.Numeric
        Me.repHandleX.Name = "repHandleX"
        '
        'repHandleY
        '
        Me.repHandleY.AutoHeight = False
        Me.repHandleY.Name = "repHandleY"
        '
        'repOrientation
        '
        Me.repOrientation.AutoHeight = False
        Me.repOrientation.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.repOrientation.Name = "repOrientation"
        '
        'repMoveCopy
        '
        Me.repMoveCopy.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.Style3D
        Me.repMoveCopy.EnableFocusRect = True
        Me.repMoveCopy.GlyphAlignment = DevExpress.Utils.HorzAlignment.[Default]
        Me.repMoveCopy.Items.AddRange(New DevExpress.XtraEditors.Controls.RadioGroupItem() {New DevExpress.XtraEditors.Controls.RadioGroupItem(True, "Move"), New DevExpress.XtraEditors.Controls.RadioGroupItem(False, "Copy")})
        Me.repMoveCopy.Name = "repMoveCopy"
        '
        'RepositoryItemTextEdit2
        '
        Me.RepositoryItemTextEdit2.Appearance.BackColor = System.Drawing.Color.Transparent
        Me.RepositoryItemTextEdit2.Appearance.ForeColor = System.Drawing.Color.Transparent
        Me.RepositoryItemTextEdit2.Appearance.Options.UseBackColor = True
        Me.RepositoryItemTextEdit2.Appearance.Options.UseForeColor = True
        Me.RepositoryItemTextEdit2.AutoHeight = False
        Me.RepositoryItemTextEdit2.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.Simple
        Me.RepositoryItemTextEdit2.Name = "RepositoryItemTextEdit2"
        '
        'RepositoryItemTextEdit3
        '
        Me.RepositoryItemTextEdit3.Appearance.BackColor = System.Drawing.Color.Transparent
        Me.RepositoryItemTextEdit3.Appearance.ForeColor = System.Drawing.Color.Transparent
        Me.RepositoryItemTextEdit3.Appearance.Options.UseBackColor = True
        Me.RepositoryItemTextEdit3.Appearance.Options.UseForeColor = True
        Me.RepositoryItemTextEdit3.AutoHeight = False
        Me.RepositoryItemTextEdit3.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.Simple
        Me.RepositoryItemTextEdit3.Name = "RepositoryItemTextEdit3"
        '
        'repSheetsGridLookUp
        '
        Me.repSheetsGridLookUp.BestFitMode = DevExpress.XtraEditors.Controls.BestFitMode.BestFitResizePopup
        Me.repSheetsGridLookUp.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.repSheetsGridLookUp.Name = "repSheetsGridLookUp"
        Me.repSheetsGridLookUp.PopupView = Me.RepositoryItemGridLookUpEdit1View
        '
        'RepositoryItemGridLookUpEdit1View
        '
        Me.RepositoryItemGridLookUpEdit1View.FocusRectStyle = DevExpress.XtraGrid.Views.Grid.DrawFocusRectStyle.RowFocus
        Me.RepositoryItemGridLookUpEdit1View.Name = "RepositoryItemGridLookUpEdit1View"
        Me.RepositoryItemGridLookUpEdit1View.OptionsSelection.EnableAppearanceFocusedCell = False
        Me.RepositoryItemGridLookUpEdit1View.OptionsView.ShowGroupPanel = False
        '
        'RepositoryItemTextEdit1
        '
        Me.RepositoryItemTextEdit1.AutoHeight = False
        Me.RepositoryItemTextEdit1.Name = "RepositoryItemTextEdit1"
        '
        'ribbonStatusBar
        '
        Me.ribbonStatusBar.ItemLinks.Add(Me.statusLocation)
        Me.ribbonStatusBar.ItemLinks.Add(Me.statusActiveLayer)
        Me.ribbonStatusBar.ItemLinks.Add(Me.statusSelection)
        Me.ribbonStatusBar.ItemLinks.Add(Me.statusDate)
        Me.ribbonStatusBar.ItemLinks.Add(Me.statusTime)
        Me.ribbonStatusBar.Location = New System.Drawing.Point(0, 595)
        Me.ribbonStatusBar.Name = "ribbonStatusBar"
        Me.ribbonStatusBar.Ribbon = Me.RibbonControl
        Me.ribbonStatusBar.Size = New System.Drawing.Size(1352, 23)
        '
        'NestingImageCollection
        '
        Me.NestingImageCollection.ImageSize = New System.Drawing.Size(64, 64)
        Me.NestingImageCollection.ImageStream = CType(resources.GetObject("NestingImageCollection.ImageStream"), DevExpress.Utils.ImageCollectionStreamer)
        Me.NestingImageCollection.Images.SetKeyName(0, ".Drawing16_90.png")
        '
        'DefaultLookAndFeel1
        '
        Me.DefaultLookAndFeel1.LookAndFeel.SkinName = "Dark Side"
        '
        'ToolStripContainer1
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.ToolStripContainer1, DevExpress.Utils.DefaultBoolean.[Default])
        '
        'ToolStripContainer1.BottomToolStripPanel
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.ToolStripContainer1.BottomToolStripPanel, DevExpress.Utils.DefaultBoolean.[Default])
        '
        'ToolStripContainer1.ContentPanel
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.ToolStripContainer1.ContentPanel, DevExpress.Utils.DefaultBoolean.[Default])
        Me.ToolStripContainer1.ContentPanel.Controls.Add(Me.splitContainerControl)
        Me.ToolStripContainer1.ContentPanel.Margin = New System.Windows.Forms.Padding(0)
        Me.ToolStripContainer1.ContentPanel.Size = New System.Drawing.Size(1352, 416)
        Me.ToolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill
        '
        'ToolStripContainer1.LeftToolStripPanel
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.ToolStripContainer1.LeftToolStripPanel, DevExpress.Utils.DefaultBoolean.[Default])
        Me.ToolStripContainer1.Location = New System.Drawing.Point(0, 147)
        Me.ToolStripContainer1.Margin = New System.Windows.Forms.Padding(0)
        Me.ToolStripContainer1.Name = "ToolStripContainer1"
        '
        'ToolStripContainer1.RightToolStripPanel
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.ToolStripContainer1.RightToolStripPanel, DevExpress.Utils.DefaultBoolean.[Default])
        Me.ToolStripContainer1.Size = New System.Drawing.Size(1352, 448)
        Me.ToolStripContainer1.TabIndex = 19
        Me.ToolStripContainer1.Text = "ToolStripContainer1"
        '
        'ToolStripContainer1.TopToolStripPanel
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.ToolStripContainer1.TopToolStripPanel, DevExpress.Utils.DefaultBoolean.[Default])
        Me.ToolStripContainer1.TopToolStripPanel.BackColor = System.Drawing.SystemColors.ControlDark
        Me.ToolStripContainer1.TopToolStripPanel.Controls.Add(Me.tbView)
        Me.ToolStripContainer1.TopToolStripPanel.Controls.Add(Me.tbSelect)
        Me.ToolStripContainer1.TopToolStripPanel.Controls.Add(Me.tbUndo)
        Me.ToolStripContainer1.TopToolStripPanel.MaximumSize = New System.Drawing.Size(0, 33)
        '
        'splitContainerControl
        '
        Me.splitContainerControl.AlwaysScrollActiveControlIntoView = False
        Me.splitContainerControl.CausesValidation = False
        Me.splitContainerControl.Dock = System.Windows.Forms.DockStyle.Fill
        Me.splitContainerControl.Location = New System.Drawing.Point(0, 0)
        Me.splitContainerControl.Margin = New System.Windows.Forms.Padding(0)
        Me.splitContainerControl.Name = "splitContainerControl"
        Me.splitContainerControl.Panel1.Text = "Panel1"
        Me.splitContainerControl.Panel2.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.Simple
        Me.splitContainerControl.Panel2.Controls.Add(Me.pnlPicmodeler)
        Me.splitContainerControl.Panel2.Controls.Add(Me.pnlNestCombos)
        Me.splitContainerControl.Panel2.Padding = New System.Windows.Forms.Padding(5)
        Me.splitContainerControl.Panel2.Text = "Panel2"
        Me.splitContainerControl.PanelVisibility = DevExpress.XtraEditors.SplitPanelVisibility.Panel2
        Me.splitContainerControl.Size = New System.Drawing.Size(1352, 416)
        Me.splitContainerControl.SplitterPosition = 21
        Me.splitContainerControl.TabIndex = 28
        Me.splitContainerControl.Text = "splitContainerControl1"
        Me.splitContainerControl.ToolTipController = Me.defaultToolTipController1.DefaultController
        '
        'pnlPicmodeler
        '
        Me.pnlPicmodeler.AccessibleRole = System.Windows.Forms.AccessibleRole.Graphic
        Me.defaultToolTipController1.SetAllowHtmlText(Me.pnlPicmodeler, DevExpress.Utils.DefaultBoolean.[Default])
        Me.pnlPicmodeler.AutoSize = True
        Me.pnlPicmodeler.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.pnlPicmodeler.BackColor = System.Drawing.Color.White
        Me.pnlPicmodeler.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlPicmodeler.Location = New System.Drawing.Point(5, 38)
        Me.pnlPicmodeler.Margin = New System.Windows.Forms.Padding(50)
        Me.pnlPicmodeler.MinimumSize = New System.Drawing.Size(150, 150)
        Me.pnlPicmodeler.Name = "pnlPicmodeler"
        Me.pnlPicmodeler.Size = New System.Drawing.Size(1338, 369)
        Me.pnlPicmodeler.TabIndex = 5
        '
        'pnlNestCombos
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.pnlNestCombos, DevExpress.Utils.DefaultBoolean.[Default])
        Me.pnlNestCombos.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlNestCombos.Controls.Add(Me.cboSheets)
        Me.pnlNestCombos.Controls.Add(Me.lblSheets)
        Me.pnlNestCombos.Controls.Add(Me.cboPatterns)
        Me.pnlNestCombos.Controls.Add(Me.lblPatterns)
        Me.pnlNestCombos.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnlNestCombos.Location = New System.Drawing.Point(5, 5)
        Me.pnlNestCombos.Name = "pnlNestCombos"
        Me.pnlNestCombos.Size = New System.Drawing.Size(1338, 33)
        Me.pnlNestCombos.TabIndex = 54
        Me.pnlNestCombos.Visible = False
        '
        'cboSheets
        '
        Me.cboSheets.Location = New System.Drawing.Point(55, 5)
        Me.cboSheets.MenuManager = Me.RibbonControl
        Me.cboSheets.Name = "cboSheets"
        Me.cboSheets.Properties.AllowMouseWheel = False
        Me.cboSheets.Properties.AutoComplete = False
        Me.cboSheets.Properties.PopupSizeable = True
        Me.cboSheets.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboSheets.Size = New System.Drawing.Size(397, 20)
        Me.cboSheets.TabIndex = 2
        '
        'lblSheets
        '
        Me.lblSheets.Location = New System.Drawing.Point(12, 8)
        Me.lblSheets.Name = "lblSheets"
        Me.lblSheets.Size = New System.Drawing.Size(37, 13)
        Me.lblSheets.TabIndex = 0
        Me.lblSheets.Text = "Sheets:"
        '
        'cboPatterns
        '
        Me.cboPatterns.Location = New System.Drawing.Point(509, 5)
        Me.cboPatterns.MenuManager = Me.RibbonControl
        Me.cboPatterns.Name = "cboPatterns"
        Me.cboPatterns.Properties.AllowMouseWheel = False
        Me.cboPatterns.Properties.Buttons.AddRange(New DevExpress.XtraEditors.Controls.EditorButton() {New DevExpress.XtraEditors.Controls.EditorButton(DevExpress.XtraEditors.Controls.ButtonPredefines.Combo)})
        Me.cboPatterns.Properties.TextEditStyle = DevExpress.XtraEditors.Controls.TextEditStyles.DisableTextEditor
        Me.cboPatterns.Size = New System.Drawing.Size(271, 20)
        Me.cboPatterns.TabIndex = 4
        '
        'lblPatterns
        '
        Me.lblPatterns.Location = New System.Drawing.Point(458, 8)
        Me.lblPatterns.Name = "lblPatterns"
        Me.lblPatterns.Size = New System.Drawing.Size(45, 13)
        Me.lblPatterns.TabIndex = 3
        Me.lblPatterns.Text = "Patterns:"
        '
        'defaultToolTipController1
        '
        '
        '
        '
        Me.defaultToolTipController1.DefaultController.Appearance.BackColor = System.Drawing.SystemColors.Info
        Me.defaultToolTipController1.DefaultController.Appearance.ForeColor = System.Drawing.Color.Black
        Me.defaultToolTipController1.DefaultController.Appearance.Options.UseBackColor = True
        Me.defaultToolTipController1.DefaultController.Appearance.Options.UseForeColor = True
        Me.defaultToolTipController1.DefaultController.Appearance.Options.UseTextOptions = True
        Me.defaultToolTipController1.DefaultController.AutoPopDelay = 8000
        Me.defaultToolTipController1.DefaultController.CloseOnClick = DevExpress.Utils.DefaultBoolean.[True]
        Me.defaultToolTipController1.DefaultController.Rounded = True
        Me.defaultToolTipController1.DefaultController.ShowBeak = True
        Me.defaultToolTipController1.DefaultController.ToolTipType = DevExpress.Utils.ToolTipType.SuperTip
        '
        'tbView
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.tbView, DevExpress.Utils.DefaultBoolean.[Default])
        Me.tbView.AllowMerge = False
        Me.tbView.BackColor = System.Drawing.SystemColors.ControlDarkDark
        Me.tbView.CanOverflow = False
        Me.tbView.Dock = System.Windows.Forms.DockStyle.None
        Me.tbView.ImageScalingSize = New System.Drawing.Size(24, 24)
        Me.tbView.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.tbViewFull, Me.tbViewWindow, Me.tbViewZoomIn, Me.tbViewZoomOut, Me.tbViewPrevious, Me.tbViewRefresh, Me.tbViewSetTools, Me.tbViewSetLayers, Me.tbViewSelHide, Me.tbViewSelShow})
        Me.tbView.Location = New System.Drawing.Point(6, 0)
        Me.tbView.Name = "tbView"
        Me.tbView.Size = New System.Drawing.Size(292, 31)
        Me.tbView.TabIndex = 3
        '
        'tbViewFull
        '
        Me.tbViewFull.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewFull.Image = CType(resources.GetObject("tbViewFull.Image"), System.Drawing.Image)
        Me.tbViewFull.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewFull.Name = "tbViewFull"
        Me.tbViewFull.Size = New System.Drawing.Size(28, 28)
        Me.tbViewFull.Text = "ToolStripButton1"
        Me.tbViewFull.ToolTipText = "Full View"
        '
        'tbViewWindow
        '
        Me.tbViewWindow.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewWindow.Image = CType(resources.GetObject("tbViewWindow.Image"), System.Drawing.Image)
        Me.tbViewWindow.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewWindow.Name = "tbViewWindow"
        Me.tbViewWindow.Size = New System.Drawing.Size(28, 28)
        Me.tbViewWindow.Text = "ToolStripButton1"
        Me.tbViewWindow.ToolTipText = "Zoom Window"
        '
        'tbViewZoomIn
        '
        Me.tbViewZoomIn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewZoomIn.Image = CType(resources.GetObject("tbViewZoomIn.Image"), System.Drawing.Image)
        Me.tbViewZoomIn.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewZoomIn.Name = "tbViewZoomIn"
        Me.tbViewZoomIn.Size = New System.Drawing.Size(28, 28)
        Me.tbViewZoomIn.Text = "ToolStripButton1"
        Me.tbViewZoomIn.ToolTipText = "Zoom In"
        '
        'tbViewZoomOut
        '
        Me.tbViewZoomOut.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewZoomOut.Image = CType(resources.GetObject("tbViewZoomOut.Image"), System.Drawing.Image)
        Me.tbViewZoomOut.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewZoomOut.Name = "tbViewZoomOut"
        Me.tbViewZoomOut.Size = New System.Drawing.Size(28, 28)
        Me.tbViewZoomOut.Text = "ToolStripButton2"
        Me.tbViewZoomOut.ToolTipText = "Zoom Out"
        '
        'tbViewPrevious
        '
        Me.tbViewPrevious.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewPrevious.Image = CType(resources.GetObject("tbViewPrevious.Image"), System.Drawing.Image)
        Me.tbViewPrevious.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewPrevious.Name = "tbViewPrevious"
        Me.tbViewPrevious.Size = New System.Drawing.Size(28, 28)
        Me.tbViewPrevious.Text = "ToolStripButton3"
        Me.tbViewPrevious.ToolTipText = "View Previous"
        '
        'tbViewRefresh
        '
        Me.tbViewRefresh.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewRefresh.Image = CType(resources.GetObject("tbViewRefresh.Image"), System.Drawing.Image)
        Me.tbViewRefresh.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewRefresh.Name = "tbViewRefresh"
        Me.tbViewRefresh.Size = New System.Drawing.Size(28, 28)
        Me.tbViewRefresh.Text = "ToolStripButton4"
        Me.tbViewRefresh.ToolTipText = "View Refresh"
        '
        'tbViewSetTools
        '
        Me.tbViewSetTools.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewSetTools.Image = CType(resources.GetObject("tbViewSetTools.Image"), System.Drawing.Image)
        Me.tbViewSetTools.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewSetTools.Name = "tbViewSetTools"
        Me.tbViewSetTools.Size = New System.Drawing.Size(28, 28)
        Me.tbViewSetTools.Text = "ToolStripButton1"
        Me.tbViewSetTools.ToolTipText = "Select and Show/Hide Tools"
        '
        'tbViewSetLayers
        '
        Me.tbViewSetLayers.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewSetLayers.Image = CType(resources.GetObject("tbViewSetLayers.Image"), System.Drawing.Image)
        Me.tbViewSetLayers.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewSetLayers.Name = "tbViewSetLayers"
        Me.tbViewSetLayers.Size = New System.Drawing.Size(28, 28)
        Me.tbViewSetLayers.Text = "ToolStripButton1"
        Me.tbViewSetLayers.ToolTipText = "Set and Show/Hide Layers"
        '
        'tbViewSelHide
        '
        Me.tbViewSelHide.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewSelHide.Image = CType(resources.GetObject("tbViewSelHide.Image"), System.Drawing.Image)
        Me.tbViewSelHide.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewSelHide.Name = "tbViewSelHide"
        Me.tbViewSelHide.Size = New System.Drawing.Size(28, 28)
        Me.tbViewSelHide.Text = "Show Selection"
        Me.tbViewSelHide.ToolTipText = "Hide Selection"
        '
        'tbViewSelShow
        '
        Me.tbViewSelShow.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbViewSelShow.Image = CType(resources.GetObject("tbViewSelShow.Image"), System.Drawing.Image)
        Me.tbViewSelShow.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbViewSelShow.Name = "tbViewSelShow"
        Me.tbViewSelShow.Size = New System.Drawing.Size(28, 28)
        Me.tbViewSelShow.Text = "Show Selection"
        '
        'tbUndo
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.tbUndo, DevExpress.Utils.DefaultBoolean.[Default])
        Me.tbUndo.AllowMerge = False
        Me.tbUndo.BackColor = System.Drawing.SystemColors.ControlDarkDark
        Me.tbUndo.CanOverflow = False
        Me.tbUndo.Dock = System.Windows.Forms.DockStyle.None
        Me.tbUndo.GripMargin = New System.Windows.Forms.Padding(6)
        Me.tbUndo.ImageScalingSize = New System.Drawing.Size(24, 24)
        Me.tbUndo.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.tbUndo_Undo, Me.tbUndo_Redo})
        Me.tbUndo.Location = New System.Drawing.Point(773, 0)
        Me.tbUndo.Name = "tbUndo"
        Me.HelpProvider1.SetShowHelp(Me.tbUndo, True)
        Me.tbUndo.Size = New System.Drawing.Size(76, 31)
        Me.tbUndo.TabIndex = 8
        '
        'tbUndo_Undo
        '
        Me.tbUndo_Undo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbUndo_Undo.Image = CType(resources.GetObject("tbUndo_Undo.Image"), System.Drawing.Image)
        Me.tbUndo_Undo.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbUndo_Undo.Name = "tbUndo_Undo"
        Me.tbUndo_Undo.Size = New System.Drawing.Size(28, 28)
        Me.tbUndo_Undo.Text = "ToolStripButton1"
        Me.tbUndo_Undo.ToolTipText = "Undo Last Operation (Ctrl+Z)"
        '
        'tbUndo_Redo
        '
        Me.tbUndo_Redo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbUndo_Redo.Image = CType(resources.GetObject("tbUndo_Redo.Image"), System.Drawing.Image)
        Me.tbUndo_Redo.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbUndo_Redo.Name = "tbUndo_Redo"
        Me.tbUndo_Redo.Size = New System.Drawing.Size(28, 28)
        Me.tbUndo_Redo.Text = "ToolStripButton2"
        Me.tbUndo_Redo.ToolTipText = "Redo Last Undo (Ctrl+Y)"
        '
        'tbSelect
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.tbSelect, DevExpress.Utils.DefaultBoolean.[Default])
        Me.tbSelect.AllowMerge = False
        Me.tbSelect.BackColor = System.Drawing.SystemColors.ControlDarkDark
        Me.tbSelect.CanOverflow = False
        Me.tbSelect.Dock = System.Windows.Forms.DockStyle.None
        Me.tbSelect.GripMargin = New System.Windows.Forms.Padding(6)
        Me.tbSelect.ImageScalingSize = New System.Drawing.Size(24, 24)
        Me.tbSelect.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.tbSelect_Enable, Me.tbSelect_Layers, Me.tbSelect_Tools, Me.tbSelect_Lines, Me.tbSelect_Arcs, Me.tbSelect_Holes, Me.tbSelect_Points, Me.tbSelect_Command, Me.tbSelect_Profiles, Me.tbSelect_Features, Me.tbSelect_ByWindow, Me.tbSelect_AddAllByFilter, Me.tbSelect_RemoveAllByFilter, Me.tbSelect_All, Me.tbSelect_RemoveAll, Me.tbSelect_HotDot})
        Me.tbSelect.Location = New System.Drawing.Point(298, 0)
        Me.tbSelect.Name = "tbSelect"
        Me.tbSelect.Size = New System.Drawing.Size(472, 32)
        Me.tbSelect.TabIndex = 3
        '
        'tbSelect_Enable
        '
        Me.tbSelect_Enable.CheckOnClick = True
        Me.tbSelect_Enable.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Enable.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_Arrow
        Me.tbSelect_Enable.ImageTransparentColor = System.Drawing.Color.DarkOrange
        Me.tbSelect_Enable.Margin = New System.Windows.Forms.Padding(2)
        Me.tbSelect_Enable.Name = "tbSelect_Enable"
        Me.tbSelect_Enable.Size = New System.Drawing.Size(28, 28)
        Me.tbSelect_Enable.Tag = "912"
        Me.tbSelect_Enable.Text = "tbSelect_Enabled"
        Me.tbSelect_Enable.ToolTipText = "Enable Selection"
        '
        'tbSelect_Layers
        '
        Me.tbSelect_Layers.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Layers.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Layer
        Me.tbSelect_Layers.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Layers.Name = "tbSelect_Layers"
        Me.tbSelect_Layers.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Layers.Tag = "901"
        Me.tbSelect_Layers.Text = "tbSlect_Layers"
        Me.tbSelect_Layers.ToolTipText = "Select By Layer"
        '
        'tbSelect_Tools
        '
        Me.tbSelect_Tools.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Tools.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Tool
        Me.tbSelect_Tools.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Tools.Name = "tbSelect_Tools"
        Me.tbSelect_Tools.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Tools.Tag = "902"
        Me.tbSelect_Tools.Text = "tbSelectTools"
        Me.tbSelect_Tools.ToolTipText = "Select By Tool"
        '
        'tbSelect_Lines
        '
        Me.tbSelect_Lines.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Lines.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Line
        Me.tbSelect_Lines.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Lines.Name = "tbSelect_Lines"
        Me.tbSelect_Lines.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Lines.Tag = "903"
        Me.tbSelect_Lines.Text = "tbSelect_Lines"
        Me.tbSelect_Lines.ToolTipText = "Select Lines"
        '
        'tbSelect_Arcs
        '
        Me.tbSelect_Arcs.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Arcs.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Arc
        Me.tbSelect_Arcs.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Arcs.Name = "tbSelect_Arcs"
        Me.tbSelect_Arcs.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Arcs.Tag = "904"
        Me.tbSelect_Arcs.Text = "tbSelect_Arcs"
        Me.tbSelect_Arcs.ToolTipText = "Select Arc"
        '
        'tbSelect_Holes
        '
        Me.tbSelect_Holes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Holes.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_BY_Hole
        Me.tbSelect_Holes.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Holes.Name = "tbSelect_Holes"
        Me.tbSelect_Holes.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Holes.Tag = "905"
        Me.tbSelect_Holes.Text = "tbSelect_Holes"
        Me.tbSelect_Holes.ToolTipText = "Select Holes"
        '
        'tbSelect_Points
        '
        Me.tbSelect_Points.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Points.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Point
        Me.tbSelect_Points.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Points.Name = "tbSelect_Points"
        Me.tbSelect_Points.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Points.Tag = "906"
        Me.tbSelect_Points.Text = "tbSelect_Points"
        Me.tbSelect_Points.ToolTipText = "Select Points"
        '
        'tbSelect_Command
        '
        Me.tbSelect_Command.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Command.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Command
        Me.tbSelect_Command.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Command.Name = "tbSelect_Command"
        Me.tbSelect_Command.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Command.Tag = "907"
        Me.tbSelect_Command.Text = "tbSelect_Command"
        Me.tbSelect_Command.ToolTipText = "Select Command/Patterns"
        '
        'tbSelect_Profiles
        '
        Me.tbSelect_Profiles.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Profiles.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Profile
        Me.tbSelect_Profiles.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Profiles.Name = "tbSelect_Profiles"
        Me.tbSelect_Profiles.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Profiles.Tag = "908"
        Me.tbSelect_Profiles.Text = "tbSelect_Profiles"
        Me.tbSelect_Profiles.ToolTipText = "Select Profiles"
        '
        'tbSelect_Features
        '
        Me.tbSelect_Features.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_Features.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Features
        Me.tbSelect_Features.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_Features.Name = "tbSelect_Features"
        Me.tbSelect_Features.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_Features.Tag = "909"
        Me.tbSelect_Features.Text = "tbSelect_Features"
        Me.tbSelect_Features.ToolTipText = "Select Features"
        '
        'tbSelect_ByWindow
        '
        Me.tbSelect_ByWindow.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_ByWindow.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_By_Window
        Me.tbSelect_ByWindow.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_ByWindow.Name = "tbSelect_ByWindow"
        Me.tbSelect_ByWindow.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_ByWindow.Tag = "920"
        Me.tbSelect_ByWindow.Text = "tbSelect_ByWindow"
        Me.tbSelect_ByWindow.ToolTipText = "Select By Window"
        '
        'tbSelect_AddAllByFilter
        '
        Me.tbSelect_AddAllByFilter.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_AddAllByFilter.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_All_By_Filter
        Me.tbSelect_AddAllByFilter.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_AddAllByFilter.Name = "tbSelect_AddAllByFilter"
        Me.tbSelect_AddAllByFilter.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_AddAllByFilter.Tag = "921"
        Me.tbSelect_AddAllByFilter.Text = "tbSelect_AddAllByFilter"
        Me.tbSelect_AddAllByFilter.ToolTipText = "Select All By Filters"
        '
        'tbSelect_RemoveAllByFilter
        '
        Me.tbSelect_RemoveAllByFilter.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_RemoveAllByFilter.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_Remove_By_Filter
        Me.tbSelect_RemoveAllByFilter.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_RemoveAllByFilter.Name = "tbSelect_RemoveAllByFilter"
        Me.tbSelect_RemoveAllByFilter.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_RemoveAllByFilter.Tag = "922"
        Me.tbSelect_RemoveAllByFilter.Text = "tbSelect_RemoveAllByFilter"
        Me.tbSelect_RemoveAllByFilter.ToolTipText = "Remove All By Filter From Selection"
        '
        'tbSelect_All
        '
        Me.tbSelect_All.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_All.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_All
        Me.tbSelect_All.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_All.Name = "tbSelect_All"
        Me.tbSelect_All.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_All.Tag = "923"
        Me.tbSelect_All.Text = "tbSelect_All"
        Me.tbSelect_All.ToolTipText = "Select All "
        '
        'tbSelect_RemoveAll
        '
        Me.tbSelect_RemoveAll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_RemoveAll.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_Select_Remove_All
        Me.tbSelect_RemoveAll.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_RemoveAll.Name = "tbSelect_RemoveAll"
        Me.tbSelect_RemoveAll.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_RemoveAll.Tag = "924"
        Me.tbSelect_RemoveAll.Text = "tbSelect_RemoveAll"
        Me.tbSelect_RemoveAll.ToolTipText = "Clear The Selection Set"
        '
        'tbSelect_HotDot
        '
        Me.tbSelect_HotDot.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tbSelect_HotDot.Image = Global.FabV25_WIN8.My.Resources.Resources.Orange_HotDot
        Me.tbSelect_HotDot.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tbSelect_HotDot.Name = "tbSelect_HotDot"
        Me.tbSelect_HotDot.Size = New System.Drawing.Size(28, 29)
        Me.tbSelect_HotDot.Tag = "930"
        Me.tbSelect_HotDot.Text = "tbSelect_HotDot"
        Me.tbSelect_HotDot.ToolTipText = "Toggle Hot Spot Indicator"
        '
        'picPattern
        '
        Me.defaultToolTipController1.SetAllowHtmlText(Me.picPattern, DevExpress.Utils.DefaultBoolean.[Default])
        Me.picPattern.Appearance.BackColor = System.Drawing.Color.White
        Me.picPattern.Appearance.Image = CType(resources.GetObject("picPattern.Appearance.Image"), System.Drawing.Image)
        Me.picPattern.Appearance.Options.UseBackColor = True
        Me.picPattern.Appearance.Options.UseImage = True
        Me.picPattern.Location = New System.Drawing.Point(1164, 52)
        Me.picPattern.LookAndFeel.UseDefaultLookAndFeel = False
        Me.picPattern.Name = "picPattern"
        Me.picPattern.Padding = New System.Windows.Forms.Padding(3)
        Me.picPattern.Size = New System.Drawing.Size(64, 64)
        Me.picPattern.TabIndex = 47
        Me.picPattern.UseWaitCursor = True
        Me.picPattern.Visible = False
        '
        'rgbSkins
        '
        Me.rgbSkins.ImageSize = New System.Drawing.Size(32, 32)
        Me.rgbSkins.ImageStream = CType(resources.GetObject("rgbSkins.ImageStream"), DevExpress.Utils.ImageCollectionStreamer)
        Me.rgbSkins.Images.SetKeyName(0, "PinkGalleryW.png")
        Me.rgbSkins.Images.SetKeyName(1, "SilverGalleryW.png")
        '
        'imgCursors
        '
        Me.imgCursors.ImageSize = New System.Drawing.Size(32, 32)
        Me.imgCursors.ImageStream = CType(resources.GetObject("imgCursors.ImageStream"), DevExpress.Utils.ImageCollectionStreamer)
        Me.imgCursors.Images.SetKeyName(0, "draw.ico")
        Me.imgCursors.Images.SetKeyName(1, "dynamic.ico")
        Me.imgCursors.Images.SetKeyName(2, "info.ico")
        Me.imgCursors.Images.SetKeyName(3, "measure.ico")
        Me.imgCursors.Images.SetKeyName(4, "Pan.ico")
        Me.imgCursors.Images.SetKeyName(5, "Select.ico")
        Me.imgCursors.Images.SetKeyName(6, "Window.ico")
        '
        'dlgFileOpenDialog
        '
        Me.dlgFileOpenDialog.FileName = "OpenFileDialog1"
        '
        'BarButtonItem4
        '
        Me.BarButtonItem4.Caption = "Automatic Nesting"
        Me.BarButtonItem4.Hint = "Create Nesated Sheets of Parts"
        Me.BarButtonItem4.Id = 212
        Me.BarButtonItem4.ImageOptions.LargeImageIndex = 177
        Me.BarButtonItem4.Name = "BarButtonItem4"
        '
        'BarButtonItem10
        '
        Me.BarButtonItem10.Caption = "Automatic Nesting"
        Me.BarButtonItem10.Hint = "Create Nesated Sheets of Parts"
        Me.BarButtonItem10.Id = 212
        Me.BarButtonItem10.ImageOptions.LargeImageIndex = 177
        Me.BarButtonItem10.Name = "BarButtonItem10"
        '
        'PopupMenu1
        '
        Me.PopupMenu1.Name = "PopupMenu1"
        Me.PopupMenu1.Ribbon = Me.RibbonControl
        '
        'Timer1
        '
        Me.Timer1.Enabled = True
        Me.Timer1.Interval = 1000
        '
        'PopupMenu2
        '
        Me.PopupMenu2.Name = "PopupMenu2"
        Me.PopupMenu2.Ribbon = Me.RibbonControl
        '
        'mnuPopUp3
        '
        Me.mnuPopUp3.AllowRibbonQATMenu = False
        Me.mnuPopUp3.ItemLinks.Add(Me.bbViewWindow, True)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbViewFull)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbViewPrevious)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbViewRefresh)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbZoomToPart)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbInquire, True)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbiMeasure)
        Me.mnuPopUp3.ItemLinks.Add(Me.iDelete, True)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbViewEntityList, True)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbiQuickSave, True)
        Me.mnuPopUp3.ItemLinks.Add(Me.bbiSaveAs)
        Me.mnuPopUp3.Name = "mnuPopUp3"
        Me.mnuPopUp3.Ribbon = Me.RibbonControl
        '
        'HelpProvider1
        '
        Me.HelpProvider1.HelpNamespace = "C:\_Dev\FabV21\FabV21_WIN8\WE_ENG_V21_0\bin\x86\Debug\WECIM_V21.chm"
        '
        'ribbonPageGroup1
        '
        Me.ribbonPageGroup1.Name = "ribbonPageGroup1"
        Me.ribbonPageGroup1.Text = "ribbonPageGroup1"
        '
        'galleryDropDown1
        '
        '
        '
        '
        Me.galleryDropDown1.Gallery.AutoSize = DevExpress.XtraBars.Ribbon.GallerySizeMode.None
        Me.galleryDropDown1.Gallery.ColumnCount = 8
        Me.galleryDropDown1.Gallery.ImageSize = New System.Drawing.Size(140, 115)
        Me.galleryDropDown1.Gallery.ShowItemText = True
        Me.galleryDropDown1.Manager = Nothing
        Me.galleryDropDown1.Name = "galleryDropDown1"
        '
        'BarButtonItem15
        '
        Me.BarButtonItem15.Caption = "Transfer From Sheet"
        Me.BarButtonItem15.Hint = "This allows for Transferring 1a part from the selected sheet to the sheet current" &
    "ly show in the graphics area."
        Me.BarButtonItem15.Id = 359
        Me.BarButtonItem15.Name = "BarButtonItem15"
        Me.BarButtonItem15.Tag = "DrawButton"
        '
        'Timer2
        '
        Me.Timer2.Interval = 5000
        '
        'RibbonPage2
        '
        Me.RibbonPage2.Name = "RibbonPage2"
        Me.RibbonPage2.Text = "RibbonPage2"
        '
        'RibbonPage3
        '
        Me.RibbonPage3.Name = "RibbonPage3"
        Me.RibbonPage3.Text = "RibbonPage3"
        '
        'CodeViewPageGroup
        '
        Me.CodeViewPageGroup.ItemLinks.Add(Me.bbCodeView)
        Me.CodeViewPageGroup.Name = "CodeViewPageGroup"
        Me.CodeViewPageGroup.ShowCaptionButton = False
        Me.CodeViewPageGroup.Text = "Code Viewer"
        '
        'frmMain
        '
        Me.AllowFormGlass = DevExpress.Utils.DefaultBoolean.[False]
        Me.defaultToolTipController1.SetAllowHtmlText(Me, DevExpress.Utils.DefaultBoolean.[Default])
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(1352, 618)
        Me.Controls.Add(Me.ToolStripContainer1)
        Me.Controls.Add(Me.picPattern)
        Me.Controls.Add(Me.ribbonStatusBar)
        Me.Controls.Add(Me.RibbonControl)
        Me.DoubleBuffered = False
        Me.HelpProvider1.SetHelpKeyword(Me, "Contents")
        Me.HelpProvider1.SetHelpString(Me, "Welcome")
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.KeyPreview = True
        Me.Name = "frmMain"
        Me.Ribbon = Me.RibbonControl
        Me.RibbonVisibility = DevExpress.XtraBars.Ribbon.RibbonVisibility.Visible
        Me.HelpProvider1.SetShowHelp(Me, True)
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.StatusBar = Me.ribbonStatusBar
        Me.Text = "WE-CIM Advanced Fabrication"
        Me.WindowState = System.Windows.Forms.FormWindowState.Maximized
        CType(Me.RibbonControl, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.appMenu, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ribbonImageCollection, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowStock, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowWorkzones, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowTableGraphics, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowToolpahDashed, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ShowProfileMarkers, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowEndPoints, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowInstanceText, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowLegendText, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowHandles, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repHighlightProfiles, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowRapidMoves, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowNibbleHits, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repShowSolidHits, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repWhiteBG, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repSnapResolution, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repCMDBFile, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.RepositoryItemMRUEdit1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repAPPDBFile, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.RepositoryItemPopupContainerEdit1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.RepositoryItemTextEdit6, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repGraphicsPref, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ribbonImageCollectionLarge, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repHandleX, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repHandleY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repOrientation, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repMoveCopy, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.RepositoryItemTextEdit2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.RepositoryItemTextEdit3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.repSheetsGridLookUp, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.RepositoryItemGridLookUpEdit1View, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.RepositoryItemTextEdit1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.NestingImageCollection, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ToolStripContainer1.ContentPanel.ResumeLayout(False)
        Me.ToolStripContainer1.TopToolStripPanel.ResumeLayout(False)
        Me.ToolStripContainer1.TopToolStripPanel.PerformLayout()
        Me.ToolStripContainer1.ResumeLayout(False)
        Me.ToolStripContainer1.PerformLayout()
        CType(Me.splitContainerControl, System.ComponentModel.ISupportInitialize).EndInit()
        Me.splitContainerControl.ResumeLayout(False)
        Me.pnlNestCombos.ResumeLayout(False)
        Me.pnlNestCombos.PerformLayout()
        CType(Me.cboSheets.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.cboPatterns.Properties, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tbView.ResumeLayout(False)
        Me.tbView.PerformLayout()
        Me.tbUndo.ResumeLayout(False)
        Me.tbUndo.PerformLayout()
        Me.tbSelect.ResumeLayout(False)
        Me.tbSelect.PerformLayout()
        CType(Me.picPattern, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.rgbSkins, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.imgCursors, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.PopupMenu1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.PopupMenu2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.mnuPopUp3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.galleryDropDown1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.BehaviorManager1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents RibbonControl As DevExpress.XtraBars.Ribbon.RibbonControl
    Private WithEvents statusLocation As DevExpress.XtraBars.BarStaticItem
    Private WithEvents homeRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Private WithEvents fileRibbonPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Private WithEvents iNew As DevExpress.XtraBars.BarButtonItem
    Private WithEvents iOpen As DevExpress.XtraBars.BarButtonItem
    Private WithEvents iSave As DevExpress.XtraBars.BarButtonItem
    Private WithEvents iSaveAs As DevExpress.XtraBars.BarButtonItem
    Private WithEvents exportRibbonPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Private WithEvents skinsRibbonPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Private WithEvents rgbiSkins As DevExpress.XtraBars.RibbonGalleryBarItem
    Private WithEvents exitRibbonPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Private WithEvents iExit As DevExpress.XtraBars.BarButtonItem
    Private WithEvents helpRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Private WithEvents helpRibbonPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Private WithEvents iHelp As DevExpress.XtraBars.BarButtonItem
    Private WithEvents iAbout As DevExpress.XtraBars.BarButtonItem
    Private WithEvents ribbonStatusBar As DevExpress.XtraBars.Ribbon.RibbonStatusBar
    Private WithEvents ribbonImageCollection As DevExpress.Utils.ImageCollection
    Private WithEvents ribbonImageCollectionLarge As DevExpress.Utils.ImageCollection
    Private WithEvents navbarImageList As System.Windows.Forms.ImageList
    Private WithEvents navbarImageListLarge As System.Windows.Forms.ImageList
    Friend WithEvents statusSelection As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents statusDate As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents statusTime As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents iOpenPDB As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iMerge As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iExportCNC As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iPrintGraphics As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents printRibbonPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents iPrintTools As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iPrintNest As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents EditRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents ViewRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents CreateRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents iUndo As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents UndoPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents ModifyRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents SequenceRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents ribProcess As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents MacroRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents AdminRibbonPage As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents iRedo As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iDelete As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents RedoPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents RemovePageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents iPurge As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iChangeTool As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iChangeAttrib As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iChangeEntities As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ChnagePageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents iHolePattern As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iLabels As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iDeletePattern As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iExplodePattern As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents PatternRibbonPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents iDisAssociate As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iFeatureAdd As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iFeatureExtract As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iFeatureInsert As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents iFeatureRemove As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents FeaturesPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents ViewDisplayPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents chkShowStock As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowStock As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowZones As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowWorkzones As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowTable As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowTableGraphics As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowToolpath As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowToolpahDashed As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowProfiles As DevExpress.XtraBars.BarEditItem
    Friend WithEvents ShowProfileMarkers As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowEndPoints As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowEndPoints As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowInstance As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowInstanceText As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowLegend As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowLegendText As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowHandles As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowHandles As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkHighlightProf As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repHighlightProfiles As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowRapids As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowRapidMoves As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowNibble As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowNibbleHits As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkShowSolid As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repShowSolidHits As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents chkWhitBackground As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repWhiteBG As DevExpress.XtraEditors.Repository.RepositoryItemCheckEdit
    Friend WithEvents bbViewFull As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ViewZoomPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbViewWindow As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbViewZoomIn As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbViewZoomOut As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbViewPrevious As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbViewRefresh As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbViewEntityList As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbViewCodeViewer As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbViewHotspot As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbViewTravel As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ViewOptionsPageGroup1 As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents sbViewOptions As DevExpress.XtraBars.BarSubItem
    Friend WithEvents BarShowStock As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowWorkzones As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowTableGraphics As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowToolpathDashed As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowProfileMarkers As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowEndpoints As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowInstanceText As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowLegend As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowHandles As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarHighlightProfiles As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowRapidMoves As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowNibbleHits As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarShowSolidHits As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarSaveLastUsed As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents BarUseWhiteBG As DevExpress.XtraBars.BarCheckItem
    Friend WithEvents CreateGeoPageGroup1 As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbCreateLine As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbCreateArc As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbCreateHole As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbCreatePoint As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbRubberBand As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents PartOutlinePageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbCreateBoundingBox As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ToolpathPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbAssociate As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbAutoPunch As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbManualLead As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbAutoLead As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbNotch As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbShakerTab As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbCommad As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbSlit As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbLinearCler As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbAreaClear As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbShapeLIbrary As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbTransformMove As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents TransformPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbTransFormCopy As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbTransFormScale As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbTRansformMirror As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbTransformRotate As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbModTrimExtend As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbModSplit As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbModFillet As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ModGeoPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbModChampher As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbModDropStop As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbModChainCut As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbModCutBack As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ModToolpathPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbModZonesManage As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ZonesPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbModZonesExtract As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbModClamps As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbModReposition As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbEditProject As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ProjectPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbModIndvHIts As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents SequencePageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbSequenceChain As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbSequemceUnchain As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbSequenceRevOrder As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbSeqManOrder As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents ManualSeqPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbConfigMan As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbNesting As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbCodeView As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbCadToCode As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents CadToCodePageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbRemnant As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents RemnantPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbExport As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents MacroPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbMacroExecute As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarDockingMenuItem1 As DevExpress.XtraBars.BarDockingMenuItem
    Friend WithEvents ToolStripContainer1 As System.Windows.Forms.ToolStripContainer
    Friend WithEvents imgCursors As DevExpress.Utils.ImageCollection
    Private WithEvents rgbSkins As DevExpress.Utils.ImageCollection
    Public WithEvents DefaultLookAndFeel1 As DevExpress.LookAndFeel.DefaultLookAndFeel
    Friend WithEvents dlgFileOpenDialog As System.Windows.Forms.OpenFileDialog
    Friend WithEvents BarSubItem1 As DevExpress.XtraBars.BarSubItem
    Friend WithEvents BarSubItem2 As DevExpress.XtraBars.BarSubItem
    Friend WithEvents BarStaticItem1 As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents BarStaticItem2 As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents BarButtonItem1 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem2 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem3 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem5 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem6 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem7 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem8 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem9 As DevExpress.XtraBars.BarButtonItem
    Private WithEvents appMenu As DevExpress.XtraBars.Ribbon.ApplicationMenu
    Friend WithEvents ribG_NestingParts As DevExpress.XtraBars.RibbonGalleryBarItem
    Friend WithEvents ribNesting As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents BarButtonItem4 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem10 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents barSnapResolution As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repSnapResolution As DevExpress.XtraEditors.Repository.RepositoryItemTextEdit
    Friend WithEvents PopupMenu1 As DevExpress.XtraBars.PopupMenu
    Friend WithEvents bbWallOfset As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    Public WithEvents statusActiveLayer As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents StstausPlaceHoilder As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents PopupMenu2 As DevExpress.XtraBars.PopupMenu
    Friend WithEvents BarButtonItem11 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents PatternGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents tbUndo As System.Windows.Forms.ToolStrip
    Friend WithEvents tbUndo_Undo As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbUndo_Redo As System.Windows.Forms.ToolStripButton
    Friend WithEvents BarButtonItem12 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbCreatePattern As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbCreateInstance As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbTrueShape As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rgTrueShape As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbManulNestingGrid As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbStaggerNest As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rbgManualNesting As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbSkeltonCutOff As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rbgSkeletonCutOff As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbCMDB As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repCMDBFile As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit
    Friend WithEvents rpgCMDB As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbEWM As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rpgEWM As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents rpgLogFile As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbAdminDumpModel As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbLogFileRecord As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbLogFileStop As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbbLogFilePlay As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbExecutePortal As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rpgPortal As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents BarEditItem1 As DevExpress.XtraBars.BarEditItem
    Friend WithEvents RepositoryItemMRUEdit1 As DevExpress.XtraEditors.Repository.RepositoryItemMRUEdit
    Friend WithEvents BarListItem1 As DevExpress.XtraBars.BarListItem
    Friend WithEvents barRecentFiles As DevExpress.XtraBars.BarSubItem
    Friend WithEvents BarListItem2 As DevExpress.XtraBars.BarListItem
    Friend WithEvents BarSubItem4 As DevExpress.XtraBars.BarSubItem
    Friend WithEvents mnuPopUp3 As DevExpress.XtraBars.PopupMenu
    Friend WithEvents BarStaticItem3 As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents BarSubItem3 As DevExpress.XtraBars.BarSubItem
    Friend WithEvents mnuMeasue As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents mnuProperties As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents BarStaticItem6 As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents BarStaticItem7 As DevExpress.XtraBars.BarStaticItem
    Friend WithEvents BarSubItem5 As DevExpress.XtraBars.BarSubItem
    Friend WithEvents bbInquire As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents HelpProvider1 As System.Windows.Forms.HelpProvider
    Friend WithEvents bbNestingDefaults As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rbNestingDefaults As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents rpgCustomMacros As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Private WithEvents ribbonPageGroup1 As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Public WithEvents NestingImageCollection As DevExpress.Utils.ImageCollection
    Friend WithEvents BarButtonItem13 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rpgTechSupport As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbManualNestAddPart As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rpgSetToolLayer As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbiTools As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbiSetLayer As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbResetAppdb As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repAPPDBFile As DevExpress.XtraEditors.Repository.RepositoryItemButtonEdit
    Friend WithEvents bbiForTesting As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rpgForTesting As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Public WithEvents galleryDropDown1 As DevExpress.XtraBars.Ribbon.GalleryDropDown
    Friend WithEvents BarButtonItem14 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbiZoomToPart As DevExpress.XtraBars.BarEditItem
    Friend WithEvents BarEditItem3 As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repHandleX As DevExpress.XtraEditors.Repository.RepositoryItemTextEdit
    Friend WithEvents repHandleY As DevExpress.XtraEditors.Repository.RepositoryItemTextEdit
    Friend WithEvents repOrientation As DevExpress.XtraEditors.Repository.RepositoryItemCalcEdit
    Friend WithEvents repMoveCopy As DevExpress.XtraEditors.Repository.RepositoryItemRadioGroup
    Friend WithEvents picPattern As DevExpress.XtraEditors.PanelControl
    Private WithEvents defaultToolTipController1 As DevExpress.Utils.DefaultToolTipController
    Friend WithEvents bbiMeasure As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents RepositoryItemTextEdit2 As DevExpress.XtraEditors.Repository.RepositoryItemTextEdit
    Friend WithEvents RepositoryItemTextEdit3 As DevExpress.XtraEditors.Repository.RepositoryItemTextEdit
    Friend WithEvents repSheetsGridLookUp As DevExpress.XtraEditors.Repository.RepositoryItemGridLookUpEdit
    Friend WithEvents RepositoryItemGridLookUpEdit1View As DevExpress.XtraGrid.Views.Grid.GridView
    Friend WithEvents BarButtonItem15 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents puContainerEdit As DevExpress.XtraBars.BarEditItem
    Friend WithEvents RepositoryItemPopupContainerEdit1 As DevExpress.XtraEditors.Repository.RepositoryItemPopupContainerEdit
    Friend WithEvents BarEditItem4 As DevExpress.XtraBars.BarEditItem
    Friend WithEvents RepositoryItemTextEdit6 As DevExpress.XtraEditors.Repository.RepositoryItemTextEdit
    Friend WithEvents bbPatternBump As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents Timer2 As System.Windows.Forms.Timer
    Friend WithEvents tbSelect As System.Windows.Forms.ToolStrip
    Friend WithEvents tbSelect_Enable As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Layers As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Tools As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Lines As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Arcs As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Holes As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Points As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Command As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Profiles As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_Features As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_ByWindow As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_AddAllByFilter As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_RemoveAllByFilter As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_All As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_RemoveAll As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbSelect_HotDot As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbView As System.Windows.Forms.ToolStrip
    Friend WithEvents tbViewFull As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbViewWindow As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbViewZoomIn As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbViewZoomOut As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbViewPrevious As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbViewRefresh As System.Windows.Forms.ToolStripButton
    Friend WithEvents pnlNestCombos As System.Windows.Forms.Panel
    Friend WithEvents cboPatterns As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents lblPatterns As DevExpress.XtraEditors.LabelControl
    Friend WithEvents cboSheets As DevExpress.XtraEditors.ComboBoxEdit
    Friend WithEvents lblSheets As DevExpress.XtraEditors.LabelControl
    Friend WithEvents ConfigPageGroup As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbShowWelcome As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbiWECAD As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbiCutShop As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rpgOptions As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents pnlPicmodeler As System.Windows.Forms.Panel
    Friend WithEvents bbiDeactivate As DevExpress.XtraBars.BarButtonItem


    Protected Overrides Sub Finalize()
        MyBase.Finalize()
    End Sub
    Friend WithEvents rpgBenchMark As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents bbiBenchMark As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents tbViewSetTools As System.Windows.Forms.ToolStripButton
    Friend WithEvents tbViewSetLayers As System.Windows.Forms.ToolStripButton
    Friend WithEvents BarButtonItem16 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents splitContainerControl As DevExpress.XtraEditors.SplitContainerControl
    Friend WithEvents RepositoryItemTextEdit1 As DevExpress.XtraEditors.Repository.RepositoryItemTextEdit
    Friend WithEvents rpgGraphicsPref As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents beGraphicsPref As DevExpress.XtraBars.BarEditItem
    Friend WithEvents repGraphicsPref As DevExpress.XtraEditors.Repository.RepositoryItemTextEdit
    Friend WithEvents BarHeaderItem1 As DevExpress.XtraBars.BarHeaderItem
    Friend WithEvents BarButtonItem17 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents BarButtonItem18 As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbiFreightCarAmerica As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents rpgCustomization As DevExpress.XtraBars.Ribbon.RibbonPageGroup
    Friend WithEvents RibbonPage2 As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents RibbonPage3 As DevExpress.XtraBars.Ribbon.RibbonPage
    Friend WithEvents bbiDiamondLife As DevExpress.XtraBars.BarButtonItem
    Friend WithEvents bbiTechConnect As BarButtonItem
    Friend WithEvents BarButtonItem19 As BarButtonItem
    Friend WithEvents bbiMRP As BarButtonItem
    Friend WithEvents CodeViewPageGroup As RibbonPageGroup
    Friend WithEvents bbi_Rittal As BarButtonItem
    Friend WithEvents bbiForm1 As BarButtonItem
    Friend WithEvents btmHeatTransfer As BarButtonItem
    Friend WithEvents BarButtonItem20 As BarButtonItem
    Friend WithEvents BarButtonItem21 As BarButtonItem
    Friend WithEvents rpgImport As RibbonPageGroup
    Friend WithEvents bbiFileManage As BarButtonItem
    Friend WithEvents BarButtonItem22 As BarButtonItem
    Friend WithEvents bbiQuickSave As BarButtonItem
    Friend WithEvents bbiSaveAs As BarButtonItem
    Friend WithEvents BehaviorManager1 As DevExpress.Utils.Behaviors.BehaviorManager
    Public WithEvents dlgFileSaveDialog As SaveFileDialog
    Friend WithEvents bbiPartExtents As BarButtonItem
    Friend WithEvents ProfileBlend As BarButtonItem
    Friend WithEvents PrintEntityList As BarButtonItem
    Friend WithEvents PrintCrossData As BarButtonItem
    Friend WithEvents bbiProfileBlend As BarButtonItem
    Friend WithEvents tbViewSelHide As ToolStripButton
    Friend WithEvents tbViewSelShow As ToolStripButton
    Friend WithEvents bbiFlipSheet As BarButtonItem
    Friend WithEvents bbiMaterialList As BarButtonItem
    Friend WithEvents BarButtonItem23 As BarButtonItem
    Friend WithEvents BarStaticItem4 As BarStaticItem
    Friend WithEvents bbZoomToPart As BarButtonItem
End Class
