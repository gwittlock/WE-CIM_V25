Option Strict Off
Option Explicit On
Friend Class LabelComboBox
    Inherits System.Windows.Forms.UserControl
    Public Event FieldWidthChange()
    Public Event UseMnemonicChange()
    Public Event ListIndexChange()
    Public Event BackColorChange()
    Public Event EnabledChange()
    Public Event CaptionChange()
    Public Event FieldStyleChange()
    Public Event CaptionLocationChange()
    Public Event ItemDataChange()
    Public Event TextChange()
    Public Event ForeColorChange()
    Public Event ListChange()

    'For use with m_item_data()
    Private Const SIZE_INCREMENT As Integer = 10
    Dim m_item_data_ub As Integer
    Dim m_item_data() As Object

    Dim bSkip As Boolean
    Dim nRight As Integer

    'Event Declarations:
    Shadows Event Click(ByVal Sender As System.Object, ByVal e As System.EventArgs) 'MappingInfo=cboChoices,cboChoices,-1,Click
    Event Change(ByVal Sender As System.Object, ByVal e As System.EventArgs) 'MappingInfo=cboChoices,cboChoices,-1,Change
    Event DblClick(ByVal Sender As System.Object, ByVal e As System.EventArgs) 'MappingInfo=lblCaption,lblCaption,-1,DblClick
    Shadows Event KeyPress(ByVal Sender As System.Object, ByVal e As KeyPressEventArgs) 'MappingInfo=cboChoices,cboChoices,-1,KeyPress
    'Default Property Values:
    Dim m_def_CaptionLocation As CaptionLocations = CaptionLocations.clLeft
    Const m_def_FieldWidth As Integer = 100
    Dim m_def_FieldStyle As FieldStyles = FieldStyles.fsFixed
    'Property Variables:
    Dim m_CaptionLocation As CaptionLocations
    Dim m_FieldWidth As Integer
    Dim m_FieldStyle As FieldStyles

    ' enumerations
    Public Enum CaptionLocations
        clLeft = 0
        clTop = 1
    End Enum
    Public Enum FieldStyles
        fsFixed = 0
        fsAuto = 1
    End Enum

    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,AddItem
    Public Sub AddItem(ByVal Item As String, Optional ByVal Index As Object = Nothing)

        cboChoices.Items.Add(Item)

        If (cboChoices.Items.Count() >= m_item_data_ub) Then
            m_item_data_ub = m_item_data_ub + SIZE_INCREMENT
            ReDim Preserve m_item_data(m_item_data_ub)
        End If

    End Sub

    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,BackColor


    Public Overrides Property BackColor() As System.Drawing.Color
        Get

            BackColor = MyBase.BackColor

        End Get
        Set(ByVal Value As System.Drawing.Color)

            MyBase.BackColor = Value
            lblCaption.BackColor = MyBase.BackColor
            RaiseEvent BackColorChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=lblCaption,lblCaption,-1,Caption


    Public Property Caption() As String
        Get

            Caption = lblCaption.Text

        End Get
        Set(ByVal Value As String)

            lblCaption.Text = Value

            bSkip = False
            LabelComboBox_Resize(Me, New System.EventArgs())
            RaiseEvent CaptionChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=14,1,0,0


    Public Property CaptionLocation() As CaptionLocations
        Get

            CaptionLocation = m_CaptionLocation

        End Get
        Set(ByVal Value As CaptionLocations)

            m_CaptionLocation = Value
            LabelComboBox_Resize(Me, New System.EventArgs())
            RaiseEvent CaptionLocationChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=UserControl,UserControl,-1,Enabled


    Public Shadows Property Enabled() As Boolean
        Get

            Enabled = MyBase.Enabled

        End Get
        Set(ByVal Value As Boolean)

            MyBase.Enabled = Value
            cboChoices.Enabled = Value
            lblCaption.Enabled = Value
            RaiseEvent EnabledChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=14,1,0,0


    Public Property FieldStyle() As FieldStyles
        Get

            FieldStyle = m_FieldStyle

        End Get
        Set(ByVal Value As FieldStyles)

            m_FieldStyle = Value
            LabelComboBox_Resize(Me, New System.EventArgs())
            RaiseEvent FieldStyleChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=14,1,0,0


    Public Property FieldWidth() As Integer
        Get

            FieldWidth = m_FieldWidth

        End Get
        Set(ByVal Value As Integer)

            If Value < 495 Then Value = 495
            m_FieldWidth = Value
            LabelComboBox_Resize(Me, New System.EventArgs())
            RaiseEvent FieldWidthChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,ForeColor


    Public Overrides Property ForeColor() As System.Drawing.Color
        Get

            ForeColor = cboChoices.ForeColor

        End Get
        Set(ByVal Value As System.Drawing.Color)

            cboChoices.ForeColor = Value
            RaiseEvent ForeColorChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,ItemData
    'Public Property Get ItemData(ByVal Index As Integer) As Long


    'Public Property Let ItemData(ByVal Index As Integer, ByVal New_ItemData As Long)
    Public Property ItemData(ByVal Index As Integer) As Object

        Get
            If Index >= 0 Then
                ItemData = m_item_data(Index)
            Else
                ItemData = Nothing
            End If

        End Get

        Set(ByVal Value As Object)

            m_item_data(Index) = Value
            RaiseEvent ItemDataChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,List


    Public Property List(ByVal Index As Integer) As String
        Get


            List = cboChoices.Items(Index).ToString
        End Get
        Set(ByVal Value As String)

            cboChoices.Items(Index) = Value
            RaiseEvent ListChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,ListCount
    Public ReadOnly Property ListCount() As Integer
        Get

            ListCount = cboChoices.Items.Count

        End Get
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,ListIndex


    Public Property ListIndex() As Integer
        Get

            ListIndex = cboChoices.SelectedIndex

        End Get
        Set(ByVal Value As Integer)

            cboChoices.SelectedIndex = Value
            RaiseEvent ListIndexChange()

        End Set
    End Property

    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,Text



    Public Overrides Property Text() As String
        Get

            Text = cboChoices.Text

        End Get
        Set(ByVal Value As String)

            ' The following line tells Visual Basic the
            ' property has changed--if you omit this line,
            ' the data source will not be updated!
            cboChoices.Text = Value
            RaiseEvent TextChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=lblCaption,lblCaption,-1,UseMnemonic


    Public Property UseMnemonic() As Boolean
        Get

            UseMnemonic = lblCaption.UseMnemonic

        End Get
        Set(ByVal Value As Boolean)

            lblCaption.UseMnemonic = Value
            RaiseEvent UseMnemonicChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,Clear
    Public Sub Clear()

        cboChoices.Items.Clear()

    End Sub


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=cboChoices,cboChoices,-1,RemoveItem
    Public Sub RemoveItem(ByVal Index As Integer)

        cboChoices.Items.RemoveAt(Index)

    End Sub

    Private Sub cboChoices_GotFocus(ByVal sender As Object, ByVal e As System.EventArgs) Handles cboChoices.GotFocus
        HighLightCaption()
    End Sub


    Private Sub cboChoices_TextChanged(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles cboChoices.TextChanged

        RaiseEvent Change(Me, Nothing)

    End Sub

    Private Sub cboChoices_SelectedIndexChanged(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles cboChoices.SelectedIndexChanged

        RaiseEvent TextChange()
        RaiseEvent Click(Me, Nothing)

    End Sub


    Private Sub cboChoices_KeyPress(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.KeyPressEventArgs) Handles cboChoices.KeyPress
        Dim KeyAscii As Integer = Asc(eventArgs.KeyChar)

        RaiseEvent KeyPress(Me, New KeyPressEventArgs(KeyAscii))

        If KeyAscii = 13 Then
            KeyAscii = 0
            System.Windows.Forms.SendKeys.Send("{TAB}") ' treat as a tab to calculate
        End If

        eventArgs.KeyChar = Chr(KeyAscii)
        If KeyAscii = 0 Then
            eventArgs.Handled = True
        End If
    End Sub

    Private Sub UserControl_Initialize()
        m_item_data_ub = SIZE_INCREMENT - 1
        'UPGRADE_ISSUE: As Variant was removed from ReDim m_item_data(0 To m_item_data_ub) statement. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="19AFCB41-AA8E-4E6B-A441-A3E802E5FD64"'
        ReDim m_item_data(m_item_data_ub)
    End Sub

    Private Sub LabelComboBox_Resize(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles MyBase.Resize
        Dim nTemp As Integer
        Dim nMinimum As Integer

        If (CaptionLocation = CaptionLocations.clLeft) Then

            lblCaption.Left = 0
            lblCaption.Top = 2

            If Len(lblCaption.Text) > 0 Then
                nTemp = lblCaption.Width + 2
            Else
                nTemp = 0
            End If

            cboChoices.Left = nTemp
            cboChoices.Top = 0

            If m_FieldStyle = FieldStyles.fsAuto Then
                nMinimum = nTemp + 50 + 2
                nTemp = nTemp + cboChoices.Width + 2

                If Me.Width < nMinimum Then
                    Me.Width = nMinimum
                Else
                    If Len(lblCaption.Text) > 0 Then
                        cboChoices.Width = Me.Width - (lblCaption.Width + 4)
                    Else
                        ' no caption so...
                        cboChoices.Width = Me.Width
                    End If
                End If
            Else
                cboChoices.Width = 50

                If Len(lblCaption.Text) > 0 Then
                    nMinimum = nTemp + cboChoices.Width
                    Me.Width = nMinimum
                Else
                    ' no caption so...
                    Me.Width = cboChoices.Width
                End If
            End If

            Me.Height = 25
            ' realign the control
        Else
            lblCaption.Left = 0
            lblCaption.Top = 0
            nMinimum = lblCaption.Width + 4

            If nMinimum < 50 Then nMinimum = 50

            cboChoices.Left = 2
            cboChoices.Top = lblCaption.Height + 2

            If m_FieldStyle = FieldStyles.fsAuto Then
                If Me.Width < nMinimum Then
                    Me.Width = nMinimum
                Else
                    cboChoices.Width = Me.Width - 3

                    If Len(lblCaption.Text) > 0 Then
                        Me.Height = lblCaption.Height + cboChoices.Height + 2
                    Else
                        ' no caption so...
                        cboChoices.Top = 2
                        Me.Height = 25
                    End If
                End If
            Else
                cboChoices.Width = m_def_FieldWidth - 3

                If nMinimum < cboChoices.Width Then nMinimum = cboChoices.Width

                Me.Width = nMinimum

                If Len(lblCaption.Text) > 0 Then
                    Me.Height = lblCaption.Height + cboChoices.Height + 2
                Else
                    ' no caption so...
                    cboChoices.Top = 2
                    Me.Height = 25
                End If
            End If
            'MessageBox.Show("the left is " + CStr(cboChoices.Left) + _
            '               " and top is " + CStr(cboChoices.Top) + _
            '              " width is " + CStr(cboChoices.Width))

        End If


    End Sub


    Private Sub HighLightCaption()

        lblCaption.BackColor = System.Drawing.ColorTranslator.FromOle(&H8000000D)
        lblCaption.ForeColor = System.Drawing.ColorTranslator.FromOle(&H8000000E)

    End Sub


    Private Sub NormalCaption()

        lblCaption.BackColor = MyBase.BackColor
        lblCaption.ForeColor = Me.ForeColor

    End Sub

    Private Sub cboChoices_LostFocus(ByVal sender As Object, ByVal e As System.EventArgs) Handles cboChoices.LostFocus
        NormalCaption()
    End Sub

    Private Sub LabelComboBox_StyleChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.StyleChanged
        lblCaption.ForeColor = Me.ForeColor
    End Sub

    Private Sub lblCaption_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lblCaption.Click
        Dim curr_indx As Integer

        Try
            If (System.Windows.Forms.Keys.ShiftKey < 0) Then

                curr_indx = cboChoices.SelectedIndex - 1
                If (curr_indx < 0) Then
                    curr_indx = cboChoices.Items.Count - 1
                End If

            Else

                curr_indx = cboChoices.SelectedIndex + 1
                If (curr_indx >= cboChoices.Items.Count) Then
                    curr_indx = 0
                End If

            End If

            cboChoices.SelectedIndex = curr_indx

        Catch ex As Exception

        End Try
        cboChoices.Focus()

    End Sub

    Private Sub lblCaption_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles lblCaption.DoubleClick
        Dim nIndex As Integer

        nIndex = cboChoices.SelectedIndex
        nIndex = nIndex + 1
        Try
            If nIndex >= cboChoices.Items.Count Then
                cboChoices.SelectedIndex = 0
            Else
                cboChoices.SelectedIndex = nIndex
            End If
        Catch ex As Exception

        End Try


        RaiseEvent DblClick(Me, Nothing)

    End Sub

    Private Sub lblCaption_DragDrop(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles lblCaption.DragDrop

    End Sub
End Class