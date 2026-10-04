Module modSelection
    Public Const c_INVALID As Integer = -1
    Public Const c_DBLAYER As Integer = 0
    Public Const c_DBWORKPLANE As Integer = 1
    Public Const c_DBTOOL As Integer = 2
    Public Const c_DBPOINT As Integer = 3
    Public Const c_DBLINE As Integer = 4
    Public Const c_DBARC As Integer = 5
    Public Const c_DBHOLE As Integer = 6
    Public Const c_DBPROFILE As Integer = 7
    Public Const c_DBCOMMAND As Integer = 8
    Public Const c_DBFEATURE As Integer = 9
    Public Const c_DBPATTERN As Integer = 10
    Public Const c_DBSEQUENCE As Integer = 11
    Public Const c_DBTERMINAL As Integer = 12

    Public Const PI As Double = 3.14159265358979
    Public Const TWOPI As Double = 2 * PI
    Public Const HALFPI As Double = PI / 2
    Public Const QUARTERPI As Double = PI / 2
    Public Const RAD2DEG As Double = 180.0# / PI
    Public Const DEG2RAD As Double = PI / 180.0#

    Public Const DIR_CW As Integer = -1
    Public Const DIR_CCW As Integer = 1
    Public Enum SELECTION_FILTER
        SEL_NONE = &H0
        SEL_LAYER = &H1         'OBSOLETE
        SEL_WORK = &H2
        SEL_TOOL = &H4
        SEL_POINT = &H8
        SEL_LINE = &H10
        SEL_ARC = &H20
        SEL_HOLE = &H40
        SEL_PROFILE = &H80
        SEL_COMMAND = &H100
        SEL_FEATURE = &H200
        SEL_SEQUENCE = &H400
        SEL_PATTERN = &H800
        SEL_ALL = &HFFF
    End Enum

    Public Enum SELECTION_FILTER_ACTION
        SEL_DISABLE = 0
        SEL_ENABLE = 1
        SEL_GET = 2
    End Enum

    Public Enum SELECTOR_STATE_OBJECT
        SEL_TOOLBAR = 0
        SEL_APPLICATION = 1
    End Enum

    Private m_state(0 To 1) As Long

    Public Const SELECT_BY_TOOL = 1
    Public Const SELECT_BY_LAYER = 2

    Public Const BUTTON_PROFILE = 908
    Public Const BUTTON_FEATURE = 909
    Public Const BUTTON_SELECT_BY_BOX = 920
    Public Const BUTTON_SELECT_MODE = 912
    Public Const BUTTON_HOT_SPOT = 930


    'ie. Are we in selection-mode or not.
    Public m_selection_mode As Boolean

    'ie. Is select-by-box active.
    Public m_select_by_box As Boolean

    'Bitwise selection-filter state.
    Public m_selection_filter As Long

    Public Function IsActive() As Boolean
        IsActive = m_selection_mode
    End Function

    Public Function SelectionMode() As Long
        SelectionMode = ButtonSet(BUTTON_SELECT_MODE, ButtonFind(BUTTON_SELECT_MODE))
    End Function

    Public Function SelectedCount() As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'lReturn = PortalExecute("*selector:count:")
        'SelectedCount = PortalGetInt("count", 0)
    End Function

    'Get the id of the Ith entity in the selection set.
    'Returns zero if indx is out-of-bounds.
    Public Function GetAt(ByVal indx As Long) As Long
        'This ia legacy code. We nolonger use portalexecute
        'lReturn = PortalExecute("*selector:get:index=" & indx)
        'GetAt = PortalGetInt("id", 0)
    End Function

    Public Function FilterSet(ByVal sncs As Long) As Long
        FilterSet = ButtonSet(sncs, ButtonFind(sncs))

        Exit Function

        'See also frmWorkZones.cmdAssign()


        'SNCS [903..909] is entity-type [SEL_LINE..SEL_FEATURE]
        Select Case sncs
            Case 901
                'see tbSelect_Layers_Click

            Case 902
                'see tbSelect_Tools_Click

            Case 903
                If (m_selection_filter And SELECTION_FILTER.SEL_LINE) Then
                    m_selection_filter = (m_selection_filter And SELECTION_FILTER.SEL_LINE)
                Else
                    m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_LINE)

                End If

            Case 904
                If (m_selection_filter And SELECTION_FILTER.SEL_ARC) Then
                    m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_ARC))
                    '
                Else
                    m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_ARC)

                End If

            Case 905 'aka Hole
                If (m_selection_filter And SELECTION_FILTER.SEL_HOLE) Then
                    m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_HOLE))
                    '
                Else
                    m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_HOLE)

                End If

            Case 906 'Point
                If (m_selection_filter And SELECTION_FILTER.SEL_POINT) Then
                    m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_POINT))

                Else
                    m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_POINT)

                End If

            Case 907 'Command
                If (m_selection_filter And SELECTION_FILTER.SEL_COMMAND) Then
                    m_selection_filter = (m_selection_filter And SELECTION_FILTER.SEL_COMMAND)

                Else
                    m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_COMMAND)

                End If

            Case 908 'aka. BUTTON_PROFILE
                If (m_selection_filter And SELECTION_FILTER.SEL_PROFILE) Then
                    m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_PROFILE))

                Else
                    m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_PROFILE)

                End If

            Case 909 'aka. BUTTON_FEATURE
                If (m_selection_filter And SELECTION_FILTER.SEL_FEATURE) Then
                    m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_FEATURE))

                Else
                    m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_FEATURE)

                End If

            Case 920 'Select by box

                'See tbSelect_ByWindow_Click

            Case 912
                'see tbSelect_Enable_Click

            Case 930 'Hot Spot
                'see tbSelect_HotDot_Click

        End Select


        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        ' lReturn = PortalExecute("selector:filter: encoded=" & m_selection_filter)


    End Function
    Public Function ButtonFind(sncs As Long) As Long

        Dim indx As Integer

        'For indx = 0 To m_toolbar.ToolCount
        For indx = 0 To frmMain.tbSelect.Items.Count
            If (frmMain.tbSelect.Items(indx).Tag = sncs) Then
                Exit For
            End If

        Next

        ButtonFind = IIf((indx < frmMain.tbSelect.Items.Count), indx, -1)

    End Function
    Public Function ButtonsSet(buttons As String, State As Boolean) As Long

        Dim pieces() As String
        Dim sncs As Long
        Dim button_indx As Integer
        Dim indx As Long
        Dim tb As ToolStripButton


        pieces = Split(buttons, "|")
        For indx = 0 To UBound(pieces)
            sncs = CLng(pieces(indx))
            button_indx = ButtonFind(sncs)

            tb = frmMain.tbSelect.Items(button_indx)

            If (tb.Checked <> State) Then

                lReturn = ButtonSet(sncs, button_indx)

            End If
        Next

        ButtonsSet = 1 'fodder

    End Function

    Public Function ButtonSet(sncs As Long, indx As Integer) As Long
        Dim tb As ToolStripButton

        Try

            If (indx >= 0) Then

                tb = frmMain.tbSelect.Items(indx)

                'See also frmWorkZones.cmdAssign()
                'odView.SelectionColorReset()

                'SNCS [903..909] is entity-type [SEL_LINE.SEL_FEATURE]
                Select Case sncs
                    Case 901

                        'we do not want the layer Icon to stay depressed so set it back
                        'tb.CheckState = CheckState.Unchecked
                        Windows.Forms.Cursor.Current = Cursors.Default

                        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
                        'frmSelectLayer.ShowDialog(fMainForm)

                    Case 902

                        'we do not want the tool Icon to stay depressed so set it back
                        'tb.CheckState = CheckState.Unchecked
                        Windows.Forms.Cursor.Current = Cursors.Default

                        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
                        'frmSelectTools.ShowDialog(fMainForm)

                    Case 903
                        If (m_selection_filter And SELECTION_FILTER.SEL_LINE) Then
                            m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_LINE))
                            tb.Checked = False
                        Else
                            m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_LINE)
                            tb.Checked = True

                            lReturn = ButtonsSet("906|907|908|909", False)
                        End If
                        If (tb.Checked = True) Then
                            tb.Image = My.Resources.Blue_Select_By_Line
                        Else
                            tb.Image = My.Resources.Orange_Select_By_Line
                        End If

                    Case 904
                        If (m_selection_filter And SELECTION_FILTER.SEL_ARC) Then
                            m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_ARC))
                            tb.Checked = False
                        Else
                            m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_ARC)
                            tb.Checked = True
                            lReturn = ButtonsSet("906|907|908|909", False)
                        End If

                        If (tb.Checked = True) Then
                            tb.Image = My.Resources.Blue_Select_By_Arc
                        Else
                            tb.Image = My.Resources.Orange_Select_By_Arc
                        End If

                    Case 905 'aka Hole
                        If (m_selection_filter And SELECTION_FILTER.SEL_HOLE) Then
                            m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_HOLE))
                            tb.Checked = False
                        Else
                            m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_HOLE)
                            tb.Checked = True
                            lReturn = ButtonsSet("906|907|908|909", False)
                        End If

                        If (tb.Checked = True) Then
                            tb.Image = My.Resources.Blue_Select_By_Hole
                        Else
                            tb.Image = My.Resources.Orange_Select_BY_Hole

                        End If

                    Case 906 'Point
                        If (m_selection_filter And SELECTION_FILTER.SEL_POINT) Then
                            m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_POINT))
                            tb.Checked = False
                        Else
                            m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_POINT)
                            tb.Checked = True
                            lReturn = ButtonsSet("903|904|905|907|908|909", False)
                        End If

                        If (tb.Checked = True) Then
                            tb.Image = My.Resources.Blue_Select_By_Point
                        Else
                            tb.Image = My.Resources.Orange_Select_By_Point
                        End If

                    Case 907 'Command
                        If (m_selection_filter And SELECTION_FILTER.SEL_COMMAND) Then
                            m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_COMMAND))
                            tb.Checked = False
                        Else
                            m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_COMMAND)
                            tb.Checked = True
                            lReturn = ButtonsSet("903|904|905|906|908|909", False)
                        End If

                        If (tb.Checked = True) Then
                            tb.Image = My.Resources.Blue_Select_By_Command
                        Else
                            tb.Image = My.Resources.Orange_Select_By_Command
                        End If


                    Case 908 'aka. BUTTON_PROFILE
                        If (m_selection_filter And SELECTION_FILTER.SEL_PROFILE) Then
                            m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_PROFILE))
                            tb.Checked = False
                        Else
                            m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_PROFILE)
                            tb.Checked = True
                            lReturn = ButtonsSet("903|904|905|906|907|909", False)
                        End If

                        If (tb.Checked = True) Then
                            tb.Image = My.Resources.Blue_Select_By_Profile
                        Else
                            tb.Image = My.Resources.Orange_Select_By_Profile
                        End If

                    Case 909 'aka. BUTTON_FEATURE
                        If (m_selection_filter And SELECTION_FILTER.SEL_FEATURE) Then
                            m_selection_filter = (m_selection_filter And (Not SELECTION_FILTER.SEL_FEATURE))
                            tb.Checked = False
                        Else
                            m_selection_filter = (m_selection_filter Or SELECTION_FILTER.SEL_FEATURE)
                            tb.Checked = True
                            lReturn = ButtonsSet("903|904|905|906|907|908", False)
                        End If

                        If (tb.Checked = True) Then
                            tb.Image = My.Resources.Blue_Select_By_Feature
                        Else
                            tb.Image = My.Resources.Orange_Select_By_Features
                        End If

                    Case 920 'Select by box

                        tb.Checked = (Not tb.Checked)
                        modSelection.m_select_by_box = tb.Checked
                        If (tb.Checked = True) Then

                            'Turn off the "Enable Selection" button.
                            lReturn = SingleSelectionDisable()

                            Dim ms As New System.IO.MemoryStream(My.Resources.window)
                            frmMain.pnlPicmodeler.Cursor = New Cursor(ms)

                            'Disable the hot-dot.  Otherwise, the window can snap to
                            'nearby entity, resulting in an undesirable selection.  The
                            'current hot-dot state is re-instated after the window.
                            'modModeMgmt.HotDotEnable(False)
                            frmMain.tbSelect_HotDot.Image = My.Resources.Orange_HotDot
                            tb.Image = My.Resources.Blue_Select_By_Window
                            frmMain.tbSelect_Enable.Image = My.Resources.Orange_Select_Arrow
                        Else

                            'modModeMgmt.HotDotEnable(True)
                            frmMain.tbSelect_HotDot.Image = My.Resources.Blue_HotDot
                            tb.Image = My.Resources.Orange_Select_By_Window
                            Windows.Forms.Cursor.Current = Cursors.Default

                        End If


                    Case 912 'TODO: ?????

                        tb.Checked = (Not tb.Checked)
                        m_selection_mode = tb.Checked

                        If (m_selection_mode) Then
                            'lReturn = ModeBegin(MODE_SELECT)
                            BoxSelectCancel()
                            tb.Image = My.Resources.Blue_Select_Arrow

                            Dim ms As New System.IO.MemoryStream(My.Resources.SelectCur)
                            frmMain.pnlPicmodeler.Cursor = New Cursor(ms)

                        Else
                            tb.Image = My.Resources.Orange_Select_Arrow
                            frmMain.pnlPicmodeler.Cursor = Cursors.Default
                            ' lReturn = ModeEnd(MODE_SELECT)
                        End If


                    Case 930 'Hot Spot
                        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
                        'If (modModeMgmt.IsHotDotEnabled()) Then
                        '    tb.CheckState = CheckState.Checked
                        'Else
                        '    tb.CheckState = CheckState.Unchecked
                        'End If

                End Select

            End If

            'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
            'lReturn = PortalExecute("selector:filter: encoded=" & m_selection_filter)

            ButtonSet = indx

        Catch ex As Exception
            MessageBox.Show("Selection ButtonSet: " & ex.Message)
        End Try


    End Function
    Private Function SingleSelectionDisable() As Long
        Dim indx As Integer
        Dim tb As ToolStripButton

        indx = ButtonFind(BUTTON_SELECT_MODE)

        tb = frmMain.tbSelect.Items(indx)

        If (tb.Checked = True) Then
            tb.Checked = False
        End If

        SingleSelectionDisable = 1  'fodder

    End Function

    Public Function HotSpotSet(ByVal enable As Boolean) As Long
        HotSpotSet = ButtonSet(BUTTON_HOT_SPOT, ButtonFind(BUTTON_HOT_SPOT))
    End Function

    Public Function SelectEntity(ByVal entity_id As Long) As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new

        'If (m_selection_filter > 0) Then
        '    SelectEntity = PortalExecute("selector:select:id=" & entity_id)
        '    If (SelectEntity > 0) Then

        '        frmMain.SelectionStatusUpdate(SelectedCount)

        '        If (bIsSeqMoveLoaded = True) Then
        '            frmSeqMove.SelectSynchronize()
        '        End If
        '        'TODO: Do we really want to regen the graphics everytime/everywhere
        '        'we call SelectEntity()?  Doing so can cause a great deal of flicker.
        '        'I think not because frmPicModeler.SynchronizeUI() does the regen.
        '        'modView.ViewRegen

        '    End If
        'End If

    End Function

    Public Function DeselectEntity(ByVal entity_id As Long) As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new

        'DeselectEntity = PortalExecute("*selector:unselect:id=" & entity_id)

        'frmMain.SelectionStatusUpdate(SelectedCount)

        ''See also SelectEntity()

    End Function

    Public Function BoxSelectBegin() As Long

        'NOTE: This was Changed by Gary because usrs were getting confused
        'by the selection arrow being turned of when in box mode
        'Turn off the selection (cursor-looking) icon.
        'If (modSelection.IsActive() = True) Then modSelection.SelectionMode

        modSelection.m_select_by_box = True

        lReturn = ButtonSet(BUTTON_SELECT_BY_BOX, ButtonFind(BUTTON_SELECT_BY_BOX))


        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'frmMain.SelectionStatusUpdate(SelectedCount)

        'lReturn = ModeBegin(MODE_WINDOW)

    End Function

    Public Sub BoxSelectEnd()
        modSelection.m_select_by_box = False
    End Sub

    Public Function BoxSelectCancel() As Long
        If (modSelection.m_select_by_box = True) Then
            lReturn = ButtonSet(BUTTON_SELECT_BY_BOX, ButtonFind(BUTTON_SELECT_BY_BOX))
            frmMain.tbSelect_ByWindow.CheckState = CheckState.Unchecked
            BoxSelectEnd()
        End If
    End Function

    Public Function IsBoxSelectActive() As Boolean
        IsBoxSelectActive = m_select_by_box
    End Function

    Public Function BoxSelect(ByVal view_ps As Point, ByVal view_pe As Point) As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'Dim local_ps As PortalPoint
        'Dim local_pe As PortalPoint
        'Dim cmd As String

        ''Currently, this is the only way to convert coordinates ...
        'local_ps = MouseMove(view_ps, 0)
        'local_pe = MouseMove(view_pe, 0)

        'cmd = "selector:selectbox:" & _
        '      " xmin=" & local_ps.dx & _
        '      ",ymin=" & local_ps.dy & _
        '      ",zmin=-99" & _
        '      ",xmax=" & local_pe.dx & _
        '      ",ymax=" & local_pe.dy & _
        '      ",zmax=99"

        'BoxSelect = PortalExecute(cmd)

        'frmMain.SynchronizeUI(modSelection.SelectedCount())

    End Function

    Public Function SelectAllByFilter() As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'SelectAllByFilter = PortalExecute("*selector:selectall:")

        'frmMain.SynchronizeUI(modSelection.SelectedCount)


    End Function

    Public Function DeselectAllByFilter() As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'DeselectAllByFilter = PortalExecute("*selector:clear:")

        'frmMain.SynchronizeUI(modSelection.SelectedCount)

    End Function

    Public Function SelectAll() As Long
        Dim Filter As Long
        Dim old_color As Long
        Dim sExt As String

        'sExt = UCase$(PathExt(PathCurrent()))

        'NOTE: We do not use modView.SelectionColorReset() here because the
        'subsequent call to frmPicModeler.SynchronizeUI() refreshes the view.
        '  See also frmWorkZones.cmdAssign()

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'old_color = modView.SelectionColorSet(System.Drawing.ColorTranslator.ToWin32(System.Drawing.Color.Red), False)

        Filter = SELECTION_FILTER.SEL_LINE Or
                SELECTION_FILTER.SEL_ARC Or
                SELECTION_FILTER.SEL_HOLE Or
                SELECTION_FILTER.SEL_POINT Or
                SELECTION_FILTER.SEL_COMMAND Or
                SELECTION_FILTER.SEL_PROFILE Or
                SELECTION_FILTER.SEL_FEATURE

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'lReturn = PortalExecute("*selector:filter: encoded=" & Filter)

        'SelectAll = PortalExecute("*selector:selectall:")

        'If (sExt <> "TBG") Then

        '    lReturn = DeSelectTableGraphics()

        'End If


        ''Restore the previous selection filter state.
        'lReturn = PortalExecute("*selector:filter: encoded=" & m_selection_filter)

        'frmMain.SynchronizeUI(modSelection.SelectedCount)

    End Function

    Public Function DeselectAll() As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new

        'Dim old_color As Long

        ''NOTE: We do not use modView.SelectionColorReset() here because the
        ''subsequent call to frmPicModeler.SynchronizeUI() refreshes the view.
        ''  See also frmWorkZones.cmdAssign()
        'old_color = modView.SelectionColorSet(System.Drawing.ColorTranslator.ToWin32(System.Drawing.Color.Red), False)

        'DeselectAll = PortalExecute("*selector:clear:")

        'frmMain.SynchronizeUI(modSelection.SelectedCount)

    End Function
    Public Function SelectHolesByLayer(
                        ByVal layer_id As Long,
                        ByVal hole_diam As Double) As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        ''Dim cmd As String
        'Dim old_color As Long

        ''TODO:PUT BACK AS WE PROGRESS IN ADDING OTHER CODE
        ''cmd = "ciEntity:SelectByLayer:" & _
        ''" action=" & CIUPDATE & _
        ''",id=" & layer_id & _
        ''",filter=" & CIARC & _
        ''",diam=" & hole_diam & _
        ''",val=1" & _
        ''",draw=1"

        ''SelectHolesByLayer = PortalExecute(cmd)

        ''See also frmWorkZones.cmdAssign()
        'old_color = modView.SelectionColorSet(System.Drawing.ColorTranslator.ToWin32(System.Drawing.Color.Red), False)

        ''Clean-up partially rendered 'selection arrows'.
        'modView.ViewRegen()

    End Function

    'NOTE: This low-level function was introduced for the file importing
    'process.  See also modConversion.FilteredSelection() for usage.
    'Returns the former selection-filter state.
    Public Function SelectionFilterSet(ByVal Filter As Long) As Long

        SelectionFilterSet = Filter
        m_selection_filter = Filter

    End Function

    Public Function IsSelected(ByVal entityID As Long) As Boolean

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'lReturn = PortalExecute("*selector:isselected:id=" & entityID)
        'IsSelected = (PortalGetInt("bool", 0) <> 0)
    End Function

    Public Function SelectorRestrictions(ByVal enable As Boolean) As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'SelectorRestrictions = PortalExecute( _
        '                "*selector:restrictions:" & _
        '                " active=" & IIf(enable, 1, 0))
    End Function


    'NOTE: Introduced for use by frmEntityList
    Public Sub SetSelectionFilterByType(ByVal entity_type As Long)

        Dim the_filter As SELECTION_FILTER

        Select Case entity_type
            Case c_DBWORKPLANE : the_filter = SELECTION_FILTER.SEL_WORK
            Case c_DBTOOL : the_filter = SELECTION_FILTER.SEL_TOOL
            Case c_DBPOINT : the_filter = SELECTION_FILTER.SEL_POINT
            Case c_DBLINE : the_filter = SELECTION_FILTER.SEL_LINE
            Case c_DBARC : the_filter = SELECTION_FILTER.SEL_ARC
            Case c_DBHOLE : the_filter = SELECTION_FILTER.SEL_HOLE
            Case c_DBPROFILE : the_filter = SELECTION_FILTER.SEL_PROFILE
            Case c_DBCOMMAND : the_filter = SELECTION_FILTER.SEL_COMMAND
            Case c_DBFEATURE : the_filter = SELECTION_FILTER.SEL_FEATURE
            Case c_DBPATTERN : the_filter = SELECTION_FILTER.SEL_PATTERN
            Case c_DBSEQUENCE : the_filter = SELECTION_FILTER.SEL_SEQUENCE
            Case Else : the_filter = SELECTION_FILTER.SEL_NONE
        End Select

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        ''In case the user has been futzing around in frmWorkZones.
        ''  See also frmWorkZones.cmdAssign()
        'modView.SelectionColorReset()

        'If (the_filter <> SELECTION_FILTER.SEL_NONE) Then
        '    lReturn = PortalExecute("*selector:filter: encoded=" & the_filter)
        'End If
        SelectionFilterSet(the_filter)
    End Sub

    'NOTE: the_filters is the booleaned result of some number of SELECTION_FILTER(s)
    Public Function SetSelectionFilters(ByVal the_filters As Long) As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'SetSelectionFilters = PortalExecute("*selector:filter: encoded=" & the_filters)
    End Function

    Public Function SelectorPush() As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'SelectorPush = PortalExecute("*selector:push:")
    End Function

    Public Function SelectorPop() As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'SelectorPop = PortalExecute("*selector:pop:")
    End Function

    Public Function SelectAllRefsTo(ByVal entityID As Long)


        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'lReturn = PortalExecute("selector:selectallrefsto:id=" & entityID)

        'SelectAllRefsTo = modSelection.SelectedCount()

    End Function

    Public Function DeSelectTableGraphics() As Long

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'Dim indx As Long
        'Dim lCount As Long
        'Dim lEntityID As Long

        'Dim lToolID As Long
        'Dim sLayerName As String
        'Dim lEntities() As Long
        'Dim entindex As Long


        'lCount = modSelection.SelectedCount()
        'ReDim lEntities(0 To lCount)
        'entindex = 0
        'For indx = 0 To (lCount - 1)

        '    lEntityID = modSelection.GetAt(indx)
        '    lReturn = PortalExecute("*create:extract:id=" & lEntityID)
        '    lToolID = PortalGetInt("toolid", 0) ' Getting the tool ID if any

        '    sLayerName = EntityStringGet(lToolID, "Name", "<error>")
        '    'lReturn = DeselectEntity(lEntityID)

        '    If (sLayerName = "TableGraphics") Then
        '        lEntities(entindex) = lEntityID

        '        'lReturn = DeselectEntity(lEntityID)
        '        'lCount = modSelection.SelectedCount()
        '        entindex = entindex + 1
        '    End If
        'Next

        'For indx = 0 To UBound(lEntities)
        '    lReturn = DeselectEntity(lEntities(indx))


        'Next
    End Function
    Public Function GetSelectionExtents() As String

        'legacy code. We nolonger use PotalExecute, so we will have to implemnt something new
        'Dim cnt As Integer
        'Dim ni As Integer = 0
        'Dim lSelectionID As Long
        'Dim lType As Long
        'Dim xs As Double
        'Dim ys As Double
        'Dim xe As Double
        'Dim ye As Double
        'Dim Xc As Double
        'Dim Yc As Double
        'Dim dTempX As Double
        'Dim dTempY As Double
        'Dim dir As Long
        'Dim diam As Double
        'Dim depth As Double
        'Dim dMaxX As Double = 0
        'Dim dMaxY As Double = 0
        'Dim workplane_type As Long
        'Dim nQuadAdjust As Integer
        'Dim cmd As String

        'workplane_type = HeaderIntGet("WorkplaneType", 0)

        'If workplane_type = 1 Then
        '    nQuadAdjust = 0
        'Else
        '    nQuadAdjust = -1
        'End If

        'cnt = SelectedCount()

        'For ni = 0 To cnt


        '    lSelectionID = modSelection.GetAt(ni)

        '    lReturn = PortalExecute("*create:extract:id=" & lSelectionID)
        '    lType = PortalGetInt("type", c_INVALID)

        '    Select Case lType
        '        Case 3 'Point

        '            xs = PortalGetDouble("x", 0.0#)
        '            ys = PortalGetDouble("y", 0.0#)

        '            ys = ys * nQuadAdjust

        '            If (xs > dMaxX) Then
        '                dMaxX = xs

        '            End If

        '            If (ys > dMaxY) Then
        '                dMaxX = ys

        '            End If

        '        Case 4 'Line

        '            ' extract data from entity
        '            xs = PortalGetDouble("sx", 0.0#)
        '            ys = PortalGetDouble("sy", 0.0#)
        '            xe = PortalGetDouble("ex", 0.0#)
        '            ye = PortalGetDouble("ey", 0.0#)

        '            ys = ys * nQuadAdjust
        '            ye = ye * nQuadAdjust

        '            If (xs > xe) Then
        '                dTempX = xs
        '            ElseIf (xe > dMaxX) Then
        '                dTempX = xe
        '            End If

        '            If dTempX > dMaxX Then
        '                dMaxX = dTempX
        '            End If

        '            If ys > ye Then
        '                dTempY = ys
        '            ElseIf ye > dMaxY Then
        '                dTempY = ye
        '            End If

        '            If dTempY > dMaxY Then
        '                dMaxY = dTempY

        '            End If


        '        Case 5 'Arc

        '            ' extract data from entity
        '            xs = PortalGetDouble("sx", 0.0#)
        '            ys = PortalGetDouble("sy", 0.0#)
        '            xe = PortalGetDouble("ex", 0.0#)
        '            ye = PortalGetDouble("ey", 0.0#)
        '            Xc = PortalGetDouble("cx", 0.0#)
        '            Yc = PortalGetDouble("cy", 0.0#)
        '            dir = PortalGetInt("dir", 1)

        '            ys = ys * nQuadAdjust
        '            ye = ye * nQuadAdjust
        '            Yc = Yc * nQuadAdjust


        '            If (xs > xe) Then
        '                dTempX = xs
        '            ElseIf (xe > dMaxX) Then
        '                dTempX = xe
        '            End If

        '            If Xc > dTempX Then
        '                dTempX = Xc
        '            End If

        '            If dTempX > dMaxX Then
        '                dMaxX = dTempX
        '            End If

        '        Case 6 'Hole
        '            ' extract data from entity
        '            xs = PortalGetDouble("x", 0.0#)
        '            ys = PortalGetDouble("y", 0.0#)
        '            diam = PortalGetDouble("dia", 0.0#)
        '            depth = PortalGetDouble("depth", 0.0#)

        '            ys = ys * nQuadAdjust

        '            If xs > dMaxX Then
        '                dMaxX = xs

        '            End If

        '            If ys > dMaxY Then
        '                dMaxY = ys
        '            End If

        '        Case 8 'pattern

        '            cnt = 0

        '            lReturn = PortalExecute("*Entity:Container:Count:id=" & lSelectionID)

        '            If (lReturn > 0) Then
        '                cnt = PortalGetInt("count", 0)
        '            End If

        '            'MessageBox.Show("the pattern id is < " & lSelectionID & " > and the count is < " & cnt & " >")

        '    End Select
        'Next
        'GetSelectionExtents = "X=" & Round(dMaxX, 4) & ", Y=" & Round(dMaxY, 4)
        ''MessageBox.Show(CalculateDimensionsJustXY, "Part Extents", MessageBoxButtons.OK, MessageBoxIcon.Warning)
    End Function
End Module
