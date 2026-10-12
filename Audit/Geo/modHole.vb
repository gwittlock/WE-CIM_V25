Imports DevExpress.XtraVerticalGrid.Rows

Module modHole
    Public Const HOLE_REPEAT_X = 0
    Public Const HOLE_REPEAT_Y = 1


    Public Structure tHoleParams

        Dim sncs As Long

        Dim hole_id As Long

        Dim feature_id As Long
        Dim tool_id As Long

        Dim xs As Double
        Dim ys As Double
        Dim xe As Double
        Dim ye As Double

        Dim Radius As Double

        Dim qty As Long
        Dim xqty As Long
        Dim yqty As Long

        Dim spacing As Double
        Dim xspacing As Double
        Dim yspacing As Double

        Dim prim As Long  'The primary axis for a grid (0) X / (1) Y

        Dim xmirror As Boolean
        Dim ymirror As Boolean
        Dim centered As Boolean
        Dim canned As Boolean

        Dim Ang As Double       'degrees
        Dim sang As Double      'degrees
        Dim iang As Double      'degrees

        Dim Auto_Index As Boolean

        Dim repeatdir As Long
        Dim repeatcnt As Long
        Dim gap As Double

        Dim Orient As Double    'degrees

    End Structure

    Private Function GetRowByCaption(ByVal s As String) As DevExpress.XtraVerticalGrid.Rows.BaseRow
        Dim r As DevExpress.XtraVerticalGrid.Rows.BaseRow
        For Each row As DevExpress.XtraVerticalGrid.Rows.BaseRow In dlgEntity.vgProplist.Rows
            r = GetRowByCaptionChild(row, s)
            If Not r Is Nothing Then
                Return r
            End If
        Next row
        Return Nothing
    End Function

    Private Function GetRowByCaptionChild(ByVal row As DevExpress.XtraVerticalGrid.Rows.BaseRow, ByVal s As String) As DevExpress.XtraVerticalGrid.Rows.BaseRow
        If row.Properties.Caption = s Then
            Return row
        End If
        Dim r As DevExpress.XtraVerticalGrid.Rows.BaseRow
        For Each cRow As DevExpress.XtraVerticalGrid.Rows.BaseRow In row.ChildRows
            r = GetRowByCaptionChild(cRow, s)
            If Not r Is Nothing Then
                Return r
            End If
        Next cRow
        Return Nothing
    End Function

    Private m_material_dx As Double
    Private m_material_dy As Double
    Private m_workplane_type As Long

    'The sncs numbers from CI
    'sncs_disp [ 301 | Single Hole ]
    'sncs_disp [ 302 | Row In X ]
    'sncs_disp [ 303 | Row In Y ]
    'sncs_disp [ 304 | At Angle / Dist Normal ]
    'sncs_disp [ 305 | At Angle / Dist Parallel ]
    'sncs_disp [ 306 | Grid ]
    'sncs_disp [ 307 | Bolt Hole Circle ]
    'sncs_disp [ 308 | Barrier ]
    Public Function CreateHole( _
                        entity_id As Long, _
                        sncs As Long, _
                        accumulator As clsPickAccumulator) As Long

        Dim params As tHoleParams
        Dim tool_data As ToolData
        Dim lToolID As Long
        Dim sValue As Object
        Dim childrow As BaseRow
        Dim name As String
        Dim status_bar_text As String

        tool_data = g_clsActiveToolData

        g_clsActiveToolData = FindToolByModelID(accumulator.tool)
        If (g_clsActiveToolData Is Nothing) Then

            childrow = GetRowByCaption("Tool")

            sValue = FindModelIDFromToolDescrip(childrow.Properties.Value)
            g_clsActiveToolData = FindToolByModelID(CLng(sValue))
            Name = GetEntityName(CLng(sValue))

            SetActiveLayer(name, g_clsActiveToolData.TlColor)

            status_bar_text = "Tool - " & g_clsActiveToolData.NC_Code_Number

            frmMain.statusActiveLayer.Caption = "Active: " & status_bar_text


            lToolID = GetActiveLayer().Id

            lReturn = UpdateAccumulatorDirect(dlgEntity.vgProplist, "tool", lToolID)

        End If

        'The hole_id is critical when editing an existing hole.
        'Without this parameter, we (wrongly) create another hole.
        params.hole_id = entity_id

        params.feature_id = 0
        params.tool_id = accumulator.tool

        params.xs = accumulator.ps.x
        params.ys = accumulator.ps.y

        params.Orient = accumulator.toolang

        'Single / RowInX / RowInY / Grid
        If ((sncs = 301) Or (sncs = 302) Or (sncs = 303) Or (sncs = 306)) Then
            params.xmirror = accumulator.soln_Get(0)
            params.ymirror = accumulator.soln_Get(1)
        Else
            params.xmirror = False
            params.ymirror = False
        End If

        'Single / RowInX / RowInY
        If ((sncs = 301) Or (sncs = 302) Or (sncs = 303)) Then
            params.centered = accumulator.soln_Get(2)
        Else
            params.centered = False
        End If

        'BHC
        If (sncs = 307) Then
            params.Auto_Index = accumulator.soln_Get(0)
        Else
            params.Auto_Index = False
        End If

        params.canned = False

        m_material_dx = HeaderDoubleGet("Length", 0)
        m_material_dy = HeaderDoubleGet("Width", 0)
        m_workplane_type = HeaderIntGet("WorkplaneType", 0)

        If (m_workplane_type = 2) Then

            m_material_dy = m_material_dy * -1

        End If

        UndoPrepare()

        Select Case sncs
            Case 301

                'Params used: feature_id, tool_id, xs, ys,
                '  centered, xmirror, ymirror, orient
                CreateHole = SingleHole(params)

            Case 302

                'Params used: feature_id, tool_id, xs, ys, xqty, xspacing
                '  centered, xmirror, ymirror, repeatdir, repeatcnt, gap, orient
                params.sncs = 302
                params.gap = accumulator.rptdst
                params.repeatdir = accumulator.rptdir
                params.repeatcnt = accumulator.rptcnt
                params.xqty = accumulator.qty_x
                params.xspacing = accumulator.spacing_x
                params.canned = accumulator.soln_Get(3)

                CreateHole = RowInX(params)

            Case 303

                'Params used: feature_id, tool_id, xs, ys, yqty, yspacing
                '  centered, xmirror, ymirror, repeatdir, repeatcnt, gap, orient
                params.sncs = 303
                params.gap = accumulator.rptdst
                params.repeatdir = accumulator.rptdir
                params.repeatcnt = accumulator.rptcnt
                params.yqty = accumulator.qty_y
                params.yspacing = accumulator.spacing_y
                params.canned = accumulator.soln_Get(3)

                CreateHole = RowInY(params)

            Case 304

                'Params used: feature_id, tool_id, xs, ys, qty, spacing, ang, orient
                params.sncs = 304
                params.qty = accumulator.qty
                params.spacing = accumulator.spacing
                params.Ang = accumulator.angincr
                params.canned = accumulator.soln_Get(0)

                CreateHole = LAA_Normal(params)

            Case 305

                'Params used: feature_id, tool_id, xs, ys, qty, xspacing,
                '  yspacing, orient
                params.sncs = 305
                params.qty = accumulator.qty
                params.xspacing = accumulator.spacing_x
                params.yspacing = accumulator.spacing_y
                params.canned = accumulator.soln_Get(0)

                CreateHole = LAA_Parallel(params)

            Case 306

                'Params used: feature_id, tool_id, xs, ys, xqty, xspacing,
                '  yqty, yspacing, orient
                params.sncs = 306
                params.prim = IIf((accumulator.soln_Get(3) = 1), 1, 0)  '(0) X / (1) Y
                params.xqty = accumulator.qty_x
                params.xspacing = accumulator.spacing_x
                params.yqty = accumulator.qty_y
                params.yspacing = accumulator.spacing_y
                params.canned = accumulator.soln_Get(2)

                'TODO: In the future, we may want to support a rotated grid
                'like those an Amada supports. For now, we fix the angle at zero.
                params.Ang = 0.0#

                CreateHole = Grid(params)

            Case 307

                'Params used: feature_id, tool_id, xs, ys, qty, sang, iang,
                '  radius, orient, auto_index
                params.sncs = 307
                params.qty = accumulator.qty
                params.sang = accumulator.Ang
                params.iang = accumulator.angincr
                params.Radius = accumulator.Rad
                params.canned = accumulator.soln_Get(1)

                CreateHole = BHC(params)

            Case 308

                'Params used: feature_id, tool_id, xs, ys, xe, ye, spacing, orient
                params.xe = accumulator.pe.x
                params.ye = accumulator.pe.y
                params.spacing = accumulator.spacing

                CreateHole = Barrier(params)

            Case Else
                CreateHole = 0

        End Select

        UndoCommit()

        g_clsActiveToolData = tool_data

    End Function

    Private Function SingleHole(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams

        this_hole_params = params

        If (params.centered) Then

            this_hole_params.xs = m_material_dx / 2
            this_hole_params.ys = m_material_dy / 2

            SingleHole = CreateOneHole(this_hole_params)

        Else

            SingleHole = CreateOneHole(this_hole_params)

            If (params.ymirror) Then

                this_hole_params.xs = params.xs
                this_hole_params.ys = m_material_dy - params.ys

                SingleHole = CreateOneHole(this_hole_params)

            End If

            If (params.xmirror) Then

                this_hole_params.xs = m_material_dx - params.xs
                this_hole_params.ys = params.ys

                SingleHole = CreateOneHole(this_hole_params)

            End If

            If (params.xmirror And params.ymirror) Then

                this_hole_params.xs = m_material_dx - params.xs
                this_hole_params.ys = m_material_dy - params.ys

                SingleHole = CreateOneHole(this_hole_params)

            End If

        End If

    End Function

    Private Function RowInX(params As tHoleParams) As Long

        Dim row_params As tHoleParams
        Dim funcParams As String

        'Create the recipient feature.
        lReturn = PortalExecute("feature:create:toolid=" & g_clsActiveToolData.Model_ID)
        params.feature_id = PortalGetInt("id", 0)

        funcParams = "function=rowinx" & _
            ",sncs=" & params.sncs & _
            ",startx=" & IIf(params.centered, CenteredXS(params), params.xs) & _
            ",starty=" & params.ys & _
            ",quantity=" & params.xqty & _
            ",spacing=" & params.xspacing & _
            ",repeatdir=" & params.repeatdir & _
            ",gap=" & params.gap & _
            ",repeats=" & params.repeatcnt & _
            ",domirrorx=" & IIf(params.xmirror, 1, 0) & _
            ",domirrory=" & IIf(params.ymirror, 1, 0) & _
            ",docenter=" & IIf(params.centered, 1, 0) & _
            ",canned=" & IIf(params.canned, 1, 0) & _
            ",index=" & params.Orient

        lReturn = EntityStringSet(params.feature_id, "function", funcParams)

        If (params.repeatdir = HOLE_REPEAT_X) Then

            'In this first case, however, we use all of the copied parameters.
            row_params = params
            If (params.centered) Then row_params.xs = CenteredXS(params)
            lReturn = HolesRepeatX(row_params)

            If (params.ymirror) Then
                row_params = params
                row_params.ys = m_material_dy - params.ys
                lReturn = HolesRepeatX(row_params)
            End If

            If (params.xmirror) Then
                row_params = params
                row_params.xs = m_material_dx - params.xs
                row_params.xspacing = -params.xspacing
                row_params.gap = -params.gap
                lReturn = HolesRepeatX(row_params)

                If (params.ymirror) Then
                    row_params = params
                    row_params.xs = m_material_dx - params.xs
                    row_params.ys = m_material_dy - params.ys
                    row_params.xspacing = -params.xspacing
                    row_params.gap = -params.gap
                    lReturn = HolesRepeatX(row_params)
                End If
            End If

        ElseIf (params.repeatdir = HOLE_REPEAT_Y) Then

            'In this first case, however, we use all of the copied parameters.
            row_params = params
            If (params.centered) Then row_params.xs = CenteredXS(params)
            lReturn = HolesRepeatX(row_params)

            If (params.ymirror) Then
                row_params = params
                row_params.ys = m_material_dy - params.ys
                row_params.gap = -params.gap
                lReturn = HolesRepeatX(row_params)
            End If

            If (params.xmirror) Then
                row_params = params
                row_params.xs = m_material_dx - params.xs
                row_params.xspacing = -params.xspacing
                lReturn = HolesRepeatX(row_params)

                If (params.ymirror) Then
                    row_params = params
                    row_params.xs = m_material_dx - params.xs
                    row_params.ys = m_material_dy - params.ys
                    row_params.xspacing = -params.xspacing
                    row_params.gap = -params.gap
                    lReturn = HolesRepeatX(row_params)
                End If
            End If

        End If

    End Function

    Private Function RowInY(params As tHoleParams) As Long

        Dim row_params As tHoleParams
        Dim funcParams As String

        'Create the recipient feature.
        lReturn = PortalExecute("feature:create:toolid=" & g_clsActiveToolData.Model_ID)
        params.feature_id = PortalGetInt("id", 0)

        funcParams = "function=rowiny" & _
            ",sncs=" & params.sncs & _
            ",startx=" & IIf(params.centered, CenteredYS(params), params.xs) & _
            ",starty=" & params.ys & _
            ",quantity=" & params.yqty & _
            ",spacing=" & params.yspacing & _
            ",repeatdir=" & params.repeatdir & _
            ",gap=" & params.gap & _
            ",repeats=" & params.repeatcnt & _
            ",domirrorx=" & IIf(params.xmirror, 1, 0) & _
            ",domirrory=" & IIf(params.ymirror, 1, 0) & _
            ",docenter=" & IIf(params.centered, 1, 0) & _
            ",canned=" & IIf(params.canned, 1, 0) & _
            ",index=" & params.Orient

        lReturn = EntityStringSet(params.feature_id, "function", funcParams)

        If (params.repeatdir = HOLE_REPEAT_X) Then

            'In this first case, however, we use all of the copied parameters.
            row_params = params
            If (params.centered) Then row_params.ys = CenteredYS(params)
            lReturn = HolesRepeatY(row_params)

            If (params.ymirror) Then
                row_params = params
                row_params.ys = m_material_dy - params.ys
                row_params.yspacing = -params.yspacing
                lReturn = HolesRepeatY(row_params)
            End If

            If (params.xmirror) Then
                row_params = params
                row_params.xs = m_material_dx - params.xs
                row_params.gap = -params.gap
                lReturn = HolesRepeatY(row_params)

                If (params.ymirror) Then
                    row_params = params
                    row_params.ys = m_material_dy - params.ys
                    row_params.xs = m_material_dx - params.xs
                    row_params.yspacing = -params.yspacing
                    row_params.gap = -params.gap
                    lReturn = HolesRepeatY(row_params)
                End If
            End If

        ElseIf (params.repeatdir = HOLE_REPEAT_Y) Then

            'In this first case, however, we use all of the copied parameters.
            row_params = params
            If (params.centered) Then row_params.ys = CenteredYS(params)
            lReturn = HolesRepeatY(row_params)

            If (params.ymirror) Then
                row_params = params
                row_params.ys = m_material_dy - params.ys
                row_params.yspacing = -params.yspacing
                row_params.gap = -params.gap
                lReturn = HolesRepeatY(row_params)
            End If

            If (params.xmirror) Then
                row_params = params
                row_params.xs = m_material_dx - params.xs
                row_params.xspacing = -params.xspacing
                lReturn = HolesRepeatY(row_params)

                If (params.ymirror) Then
                    row_params = params
                    row_params.ys = m_material_dy - params.ys
                    row_params.xs = m_material_dx - params.xs
                    row_params.yspacing = -params.yspacing
                    row_params.gap = -params.gap
                    lReturn = HolesRepeatY(row_params)
                End If
            End If

        End If

    End Function

    Private Function LAA_Parallel(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams
        Dim funcParams As String
        Dim cmd As String
        Dim vlen As Double
        Dim Ang As Double
        Dim indx As Long

        vlen = Math.Sqrt((params.xspacing * params.xspacing) + (params.yspacing + params.yspacing))
        If (vlen < 0.0001) Then  'arbtrarily is-zero test
            'Prevent divide by zero.
            LAA_Parallel = 0
            Exit Function
        End If

        'Create the recipient feature.
        lReturn = PortalExecute("feature:create:toolid=" & g_clsActiveToolData.Model_ID)
        params.feature_id = PortalGetInt("id", 0)

        funcParams = "function=laapar" & _
            ",sncs=" & params.sncs & _
            ",startx=" & params.xs & _
            ",starty=" & params.ys & _
            ",quantity=" & params.qty & _
            ",spacingx=" & params.xspacing & _
            ",spacingy=" & params.yspacing & _
            ",canned=" & IIf(params.canned, 1, 0) & _
            ",index=" & params.Orient

        lReturn = EntityStringSet(params.feature_id, "function", funcParams)

        If (params.canned) Then

            params.xspacing = params.xspacing / vlen
            params.yspacing = params.yspacing / vlen

            Ang = RAD2DEG * ATan2(params.yspacing, params.xspacing)

            cmd = "create:hole: sncs=305" & _
                  ",fid=" & params.feature_id & _
                  ",tool_id=" & params.tool_id & _
                  ",xc=" & params.xs & _
                  ",yc=" & params.ys & _
                  ",cnt=" & params.qty & _
                  ",dst=" & vlen & _
                  ",ang=" & Ang & _
                  ",orient=" & params.Orient

            lReturn = PortalExecute(cmd)

        Else

            this_hole_params = params

            For indx = 0 To (params.qty - 1)

                this_hole_params.xs = params.xs + (params.xspacing * indx)
                this_hole_params.ys = params.ys + (params.yspacing * indx)

                lReturn = CreateOneHole(this_hole_params)

            Next indx

        End If

    End Function

    Private Function LAA_Normal(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams
        Dim funcParams As String
        Dim cmd As String
        Dim DX As Double
        Dim DY As Double
        Dim indx As Long

        If (params.spacing < 0.0001) Then  'arbtrarily is-zero test
            'Prevent stupid stuff.
            LAA_Normal = 0
            Exit Function
        End If

        'Create the recipient feature.
        lReturn = PortalExecute("feature:create:toolid=" & g_clsActiveToolData.Model_ID)
        params.feature_id = PortalGetInt("id", 0)

        DX = params.spacing * Math.Cos(params.Ang * DEG2RAD)
        DY = params.spacing * Math.Sin(params.Ang * DEG2RAD)

        funcParams = "function=laanormal" & _
            ",sncs=" & params.sncs & _
            ",startx=" & params.xs & _
            ",starty=" & params.ys & _
            ",quantity=" & params.qty & _
            ",spacing=" & params.spacing & _
            ",angle=" & params.Ang & _
            ",canned=" & IIf(params.canned, 1, 0) & _
            ",index=" & params.Orient

        lReturn = EntityStringSet(params.feature_id, "function", funcParams)

        If (params.canned) Then

            cmd = "create:hole: sncs=304" & _
                  ",fid=" & params.feature_id & _
                  ",tool_id=" & params.tool_id & _
                  ",xc=" & params.xs & _
                  ",yc=" & params.ys & _
                  ",cnt=" & params.qty & _
                  ",dst=" & params.spacing & _
                  ",ang=" & params.Ang & _
                  ",orient=" & params.Orient

            lReturn = PortalExecute(cmd)

        Else

            this_hole_params = params

            For indx = 0 To (params.qty - 1)

                this_hole_params.xs = params.xs + (DX * indx)
                this_hole_params.ys = params.ys + (DY * indx)

                lReturn = CreateOneHole(this_hole_params)

            Next indx

        End If

    End Function

    Private Function Grid(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams
        Dim funcParams As String

        'Create the recipient feature.
        lReturn = PortalExecute("feature:create:toolid=" & g_clsActiveToolData.Model_ID)
        params.feature_id = PortalGetInt("id", 0)

        funcParams = "function=grid" & _
            ",sncs=" & params.sncs & _
            ",startx=" & params.xs & _
            ",starty=" & params.ys & _
            ",prim=" & params.prim & _
            ",quantityx =" & params.xqty & _
            ",spacingx=" & params.xspacing & _
            ",quantityy =" & params.yqty & _
            ",spacingy=" & params.yspacing & _
            ",ang=" & params.Ang & _
            ",domirrorx=" & IIf(params.xmirror, 1, 0) & _
            ",domirrory=" & IIf(params.ymirror, 1, 0) & _
            ",canned=" & IIf(params.canned, 1, 0) & _
            ",index=" & params.Orient

        lReturn = EntityStringSet(params.feature_id, "function", funcParams)

        this_hole_params = params
        lReturn = HolesGrid(this_hole_params)

        If (params.ymirror) Then

            this_hole_params = params
            this_hole_params.ys = m_material_dy - params.ys
            this_hole_params.yspacing = -params.yspacing
            lReturn = HolesGrid(this_hole_params)

        End If

        If (params.xmirror) Then

            this_hole_params = params
            this_hole_params.xs = m_material_dx - params.xs
            this_hole_params.xspacing = -params.xspacing
            lReturn = HolesGrid(this_hole_params)

            If (params.ymirror) Then

                this_hole_params = params
                this_hole_params.xs = m_material_dx - params.xs
                this_hole_params.ys = m_material_dy - params.ys
                this_hole_params.xspacing = -params.xspacing
                this_hole_params.yspacing = -params.yspacing
                lReturn = HolesGrid(this_hole_params)

            End If

        End If

    End Function

    Private Function BHC(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams
        Dim cmd As String
        Dim indx As Integer
        Dim funcParams As String
        Dim this_ang As Double
        Dim sang As Double
        Dim iang As Double
        Dim Orient As Double

        'Create the recipient feature.
        lReturn = PortalExecute("feature:create:toolid=" & g_clsActiveToolData.Model_ID)
        params.feature_id = PortalGetInt("id", 0)

        funcParams = "function=bhc" & _
            ",sncs=" & params.sncs & _
            ",startx=" & params.xs & _
            ",starty=" & params.ys & _
            ",quantity=" & params.qty & _
            ",incang=" & params.iang & _
            ",stang=" & params.sang & _
            ",rad=" & params.Radius & _
            ",keep=" & IIf(params.Auto_Index, 0, 1) & _
            ",canned=" & IIf(params.canned, 1, 0) & _
            ",index=" & params.Orient

        lReturn = EntityStringSet(params.feature_id, "function", funcParams)

        If (params.canned) Then

            cmd = "create:hole: sncs=307" & _
                  ",fid=" & params.feature_id & _
                  ",tool_id=" & params.tool_id & _
                  ",xc=" & params.xs & _
                  ",yc=" & params.ys & _
                  ",cnt=" & params.qty & _
                  ",rad=" & params.Radius & _
                  ",as=" & params.sang & _
                  ",ai=" & params.iang & _
                  ",orient=" & params.Orient

            lReturn = PortalExecute(cmd)

        Else

            'This statement copies the 'shared' data like tool_id, etc.
            'It also sets the default 'orient' for the case where auto_index is false.
            this_hole_params = params

            sang = params.sang * DEG2RAD
            iang = params.iang * DEG2RAD
            Orient = params.Orient * DEG2RAD

            For indx = 0 To (params.qty - 1)

                this_ang = sang + (iang * indx)
                this_hole_params.xs = (params.xs + (params.Radius * Math.Cos(this_ang)))
                this_hole_params.ys = (params.ys + (params.Radius * Math.Sin(this_ang)))

                If (params.Auto_Index) Then
                    this_hole_params.Orient = (sang + Orient + (iang * indx)) * RAD2DEG
                End If

                lReturn = CreateOneHole(this_hole_params)

            Next indx

        End If

    End Function

    Private Function Barrier(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams
        Dim funcParams As String
        Dim Dist As Double
        Dim xs As Double
        Dim ys As Double
        Dim indx As Long
        Dim TempQty As Long
        Dim TempQty2 As Long

        Dist = Dist2D(params.xs, params.xe, params.ys, params.ye)
        params.qty = Dist / params.spacing

        TempQty = params.qty
        TempQty2 = params.qty - TempQty

        If (TempQty2 > 0) Then

            lReturn = MsgBox( _
                "These settings will not create an equal spacing. " & _
                "There will be " & Str(TempQty + 1) & " holes created. " & _
                "Do you wish to continue?", vbYesNo + vbInformation, Application.ProductName)

            If lReturn = vbNo Then Exit Function

        End If

        'Create the recipient feature.
        lReturn = PortalExecute("feature:create:toolid=" & g_clsActiveToolData.Model_ID)
        params.feature_id = PortalGetInt("id", 0)

        Dist = Math.Abs(params.xs - params.xe)
        params.repeatdir = IIf((Dist > 0.0005), HOLE_REPEAT_X, HOLE_REPEAT_Y)

        funcParams = "function=barrier" & _
            ",sncs=" & params.sncs & _
            ",startx=" & params.xs & _
            ",starty=" & params.ys & _
            ",endx=" & params.xe & _
            ",endy=" & params.ye & _
            ",spacing=" & params.spacing & _
            ",domirrorx=" & IIf(params.xmirror, 1, 0) & _
            ",domirrory=" & IIf(params.ymirror, 1, 0) & _
            ",index=" & params.Orient

        lReturn = EntityStringSet(params.feature_id, "function", funcParams)

        If (params.repeatdir = HOLE_REPEAT_X) Then

            this_hole_params = params

            For indx = 0 To params.qty
                this_hole_params.xs = params.xs + (indx * params.spacing)
                lReturn = CreateOneHole(this_hole_params)
            Next

            If (params.ymirror) Then
                this_hole_params.ys = m_material_dy - params.ys
                For indx = 0 To (params.qty)
                    this_hole_params.xs = params.xs + (indx * params.spacing)
                    lReturn = CreateOneHole(this_hole_params)
                Next
            End If

            If (params.xmirror) Then
                xs = m_material_dx - params.xs
                this_hole_params.ys = params.ys

                For indx = 0 To params.qty
                    this_hole_params.xs = xs - (indx * params.spacing)
                    lReturn = CreateOneHole(this_hole_params)
                Next

                If (params.ymirror) Then
                    xs = m_material_dx - params.xs
                    this_hole_params.ys = m_material_dy - params.ys

                    For indx = 0 To params.qty
                        this_hole_params.xs = xs - (indx * params.spacing)
                        lReturn = CreateOneHole(this_hole_params)
                    Next
                End If
            End If

        ElseIf (params.repeatdir = HOLE_REPEAT_Y) Then

            this_hole_params = params

            For indx = 0 To params.qty
                this_hole_params.ys = params.ys + (indx * params.spacing)
                lReturn = CreateOneHole(this_hole_params)
            Next

            If (params.ymirror) Then
                this_hole_params.xs = params.xs
                ys = m_material_dy - params.ys

                For indx = 0 To params.qty
                    this_hole_params.ys = ys - (indx * params.spacing)
                    lReturn = CreateOneHole(this_hole_params)
                Next
            End If

            If (params.xmirror) Then
                this_hole_params.xs = m_material_dx - params.xs

                For indx = 0 To params.qty
                    this_hole_params.ys = params.ys + (indx * params.spacing)
                    lReturn = CreateOneHole(this_hole_params)
                Next

                If (params.ymirror) Then
                    this_hole_params.xs = m_material_dx - params.xs
                    ys = m_material_dy - params.ys

                    For indx = 0 To params.qty
                        this_hole_params.ys = ys - (indx * params.spacing)
                        lReturn = CreateOneHole(this_hole_params)
                    Next
                End If
            End If

        End If

    End Function

    Private Function CreateOneHole(params As tHoleParams) As Long

        Dim cmd As String
        Dim hole_id As Long

        cmd = "create:hole:"

        If (params.hole_id > 0) Then
            cmd = cmd & " id=" & params.hole_id & ","
        End If

        cmd = cmd & " fid=" & params.feature_id & _
              ",tool_id=" & params.tool_id & _
              ",xc=" & params.xs & _
              ",yc=" & params.ys & _
              ",orient=" & params.Orient

        lReturn = PortalExecute(cmd)

        hole_id = PortalGetInt("id", 0)
        If (hole_id > 0) Then
            modTooling.SetActiveToolAttributes(hole_id)
        End If

    End Function

    Private Function HolesRepeatX(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams
        Dim cmd As String
        Dim irepeat As Long
        Dim ispace As Long
        Dim xs As Double
        Dim ys As Double

        xs = params.xs
        ys = params.ys

        If (params.canned) Then

            cmd = "create:hole: sncs=302" & _
                  ",fid=" & params.feature_id & _
                  ",tool_id=" & params.tool_id & _
                  ",xc=" & params.xs & _
                  ",yc=" & params.ys & _
                  ",cnt=" & params.xqty & _
                  ",dst=" & params.xspacing & _
                  ",orient=" & params.Orient

            lReturn = PortalExecute(cmd)

        Else

            this_hole_params = params

            For irepeat = 0 To params.repeatcnt

                For ispace = 0 To (params.xqty - 1)

                    'NOTE: params.xspacing is signed.
                    this_hole_params.xs = xs + (ispace * params.xspacing)
                    this_hole_params.ys = ys

                    lReturn = CreateOneHole(this_hole_params)

                Next

                'NOTE: params.gap is signed.
                If (params.repeatdir = HOLE_REPEAT_Y) Then
                    ys = this_hole_params.ys + params.gap
                Else
                    xs = this_hole_params.xs + params.gap
                End If

            Next

        End If

    End Function

    Private Function HolesRepeatY(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams
        Dim cmd As String
        Dim irepeat As Long
        Dim ispace As Long
        Dim xs As Double
        Dim ys As Double

        xs = params.xs
        ys = params.ys

        If (params.canned) Then

            cmd = "create:hole: sncs=303" & _
                  ",fid=" & params.feature_id & _
                  ",tool_id=" & params.tool_id & _
                  ",xc=" & params.xs & _
                  ",yc=" & params.ys & _
                  ",cnt=" & params.yqty & _
                  ",dst=" & params.yspacing & _
                  ",orient=" & params.Orient

            lReturn = PortalExecute(cmd)

        Else

            this_hole_params = params

            For irepeat = 0 To params.repeatcnt

                For ispace = 0 To (params.yqty - 1)

                    'NOTE: params.yspacing is signed.
                    this_hole_params.xs = xs
                    this_hole_params.ys = ys + (ispace * params.yspacing)

                    lReturn = CreateOneHole(this_hole_params)

                Next

                'NOTE: params.gap is signed.
                If (params.repeatdir = HOLE_REPEAT_Y) Then
                    ys = this_hole_params.ys + params.gap
                Else
                    xs = this_hole_params.xs + params.gap
                End If

            Next

        End If

    End Function

    Private Function HolesGrid(params As tHoleParams) As Long

        Dim this_hole_params As tHoleParams
        Dim cmd As String
        Dim xndx As Long
        Dim yndx As Long

        'NOTE: params.xspacing and params.yspacing are signed.

        If (params.canned) Then

            cmd = "create:hole: sncs=306" & _
                  ",fid=" & params.feature_id & _
                  ",tool_id=" & params.tool_id & _
                  ",xc=" & params.xs & _
                  ",yc=" & params.ys & _
                  ",prim=" & params.prim & _
                  ",xcnt=" & params.xqty & _
                  ",xdst=" & params.xspacing & _
                  ",ycnt=" & params.yqty & _
                  ",ydst=" & params.yspacing & _
                  ",ang=" & params.Ang & _
                  ",orient=" & params.Orient

            lReturn = PortalExecute(cmd)

        Else

            this_hole_params = params

            'TODO: Generate points in manner consistent with the
            'G78 and G79 canned cycles? In particular, each row/col
            'should have opposite direction of preceding row/col.

            If (params.prim = 0) Then  'ie. X-Axis

                For yndx = 0 To (params.yqty - 1)

                    this_hole_params.ys = params.ys + (params.yspacing * yndx)

                    For xndx = 0 To (params.xqty - 1)

                        this_hole_params.xs = params.xs + (params.xspacing * xndx)
                        lReturn = CreateOneHole(this_hole_params)

                    Next

                Next

            Else

                For xndx = 0 To (params.xqty - 1)

                    this_hole_params.xs = params.xs + (params.xspacing * xndx)

                    For yndx = 0 To (params.yqty - 1)

                        this_hole_params.ys = params.ys + (params.yspacing * yndx)
                        lReturn = CreateOneHole(this_hole_params)

                    Next

                Next

            End If

        End If

    End Function

    Private Function CenteredXS(params As tHoleParams) As Double

        Dim repeatcnt As Long

        repeatcnt = IIf((params.repeatdir = HOLE_REPEAT_X), params.repeatcnt, 0)

        CenteredXS = (m_material_dx / 2) - _
            Math.Abs(((((params.xqty - 1) / 2) * params.xspacing) * _
                (repeatcnt + 1)) + ((params.gap * repeatcnt) / 2))

    End Function

    Private Function CenteredYS(params As tHoleParams) As Double

        Dim repeatcnt As Long

        repeatcnt = IIf((params.repeatdir = HOLE_REPEAT_Y), params.repeatcnt, 0)

        CenteredYS = (m_material_dy / 2) - _
            Math.Abs(((((params.yqty - 1) / 2) * params.yspacing) * _
                (repeatcnt + 1)) + ((params.gap * repeatcnt) / 2))

    End Function



End Module
