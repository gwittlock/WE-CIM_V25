Friend Class lblTextBox
    Public Event LockedChange()
    Public Event FieldWidthChange()
    Public Event MaxLengthChange()
    Public Event BackColorChange()
    Public Event EnabledChange()
    Public Event CaptionChange()
    Public Event FieldStyleChange()
    Public Event CaptionLocationChange()
    Public Event TextChange()
    Public Event PasswordCharChange()
    Public Event ForeColorChange()


    Dim bSkip As Boolean
    Dim nRight As Integer

    Event Change(ByVal Sender As System.Object, ByVal e As System.EventArgs) 'MappingInfo=txtInput,txtInput,-1,Change
    Shadows Event KeyPress(ByVal Sender As System.Object, ByVal e As KeyPressEventArgs) 'MappingInfo=txtInput,txtInput,-1,KeyPress
    Dim m_def_CaptionLocation As CaptionLocations = CaptionLocations.clLeft
    Const m_def_FieldWidth As Integer = 50

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
    Public Overrides Property BackColor() As System.Drawing.Color
        Get
            BackColor = MyBase.BackColor

        End Get
        Set(ByVal Value As System.Drawing.Color)

            MyBase.BackColor = Value
            RaiseEvent BackColorChange()

        End Set
    End Property
    Public Property Caption() As String
        Get

            On Error Resume Next

            Caption = lblCaption.Text

        End Get
        Set(ByVal Value As String)

            On Error Resume Next

            lblCaption.Text = Value

            bSkip = False

            LabelTextBox_Resize(Me, New System.EventArgs())
            RaiseEvent CaptionChange()

        End Set
    End Property
    Public Property CaptionLocation() As CaptionLocations
        Get

            On Error Resume Next

            CaptionLocation = m_CaptionLocation

        End Get
        Set(ByVal Value As CaptionLocations)

            On Error Resume Next

            m_CaptionLocation = Value
            LabelTextBox_Resize(Me, New System.EventArgs())
            RaiseEvent CaptionLocationChange()

        End Set
    End Property
    Public Shadows Property Enabled() As Boolean
        Get

            On Error Resume Next

            Enabled = MyBase.Enabled

        End Get
        Set(ByVal Value As Boolean)

            On Error Resume Next

            MyBase.Enabled = Value
            txtInput.Enabled = MyBase.Enabled
            lblCaption.Enabled = MyBase.Enabled
            RaiseEvent EnabledChange()

        End Set
    End Property
    Public Property FieldStyle() As FieldStyles
        Get

            On Error Resume Next

            FieldStyle = m_FieldStyle

        End Get
        Set(ByVal Value As FieldStyles)

            On Error Resume Next

            m_FieldStyle = Value
            LabelTextBox_Resize(Me, New System.EventArgs())
            RaiseEvent FieldStyleChange()

        End Set
    End Property
    Public Property FieldWidth() As Integer
        Get

            On Error Resume Next

            FieldWidth = m_FieldWidth

        End Get
        Set(ByVal Value As Integer)

            On Error Resume Next

            If Value < 50 Then Value = 50
            m_FieldWidth = Value
            LabelTextBox_Resize(Me, New System.EventArgs())
            RaiseEvent FieldWidthChange()

        End Set
    End Property

    Public Overrides Property ForeColor() As System.Drawing.Color
        Get

            On Error Resume Next

            ForeColor = txtInput.ForeColor

        End Get
        Set(ByVal Value As System.Drawing.Color)

            On Error Resume Next

            txtInput.ForeColor = Value
            RaiseEvent ForeColorChange()

        End Set
    End Property

    Public Property MaxLength() As Integer
        Get

            On Error Resume Next

            'UPGRADE_WARNING: TextBox property txtInput.MaxLength has a new behavior. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6BA9B8D2-2A32-4B6E-8D36-44949974A5B4"'
            MaxLength = txtInput.Maxlength

        End Get
        Set(ByVal Value As Integer)

            On Error Resume Next

            'UPGRADE_WARNING: TextBox property txtInput.MaxLength has a new behavior. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6BA9B8D2-2A32-4B6E-8D36-44949974A5B4"'
            txtInput.Maxlength = Value
            RaiseEvent MaxLengthChange()

        End Set
    End Property
    Public Overrides Property Text() As String
        Get

            Text = txtInput.Text

        End Get
        Set(ByVal Value As String)

            ' The following line tells Visual Basic the
            ' property has changed--if you omit this line,
            ' the data source will not be updated!
            txtInput.Text = Value
            RaiseEvent TextChange()

        End Set
    End Property

    Private Sub txtInput_Enter(ByVal sender As Object, ByVal e As System.EventArgs) Handles txtInput.Enter
        txtInput.SelectionStart = 0
        txtInput.SelectionLength = Len(txtInput.Text)
        HighLightCaption()
    End Sub

    Private Sub txtInput_KeyPress(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.KeyPressEventArgs) Handles txtInput.KeyPress
        Dim KeyAscii As Integer = Asc(eventArgs.KeyChar)

        On Error Resume Next

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

    Private Sub txtInput_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles txtInput.TextChanged
        RaiseEvent TextChange()
        RaiseEvent Change(Me, Nothing)
    End Sub

    Private Sub txtInput_Leave(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles txtInput.Leave

        On Error Resume Next

        NormalCaption()

    End Sub



    Private Sub LabelTextBox_Resize(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles MyBase.Resize
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

            txtInput.Left = nTemp
            txtInput.Top = 0

            If m_FieldStyle = FieldStyles.fsAuto Then
                nMinimum = nTemp + 50 + 2
                nTemp = nTemp + txtInput.Width + 2

                If Me.Width < nMinimum Then
                    Me.Width = nMinimum
                Else
                    If Len(lblCaption.Text) > 0 Then
                        txtInput.Width = Me.Width - (lblCaption.Width + 4)
                    Else
                        ' no caption so...
                        txtInput.Width = Me.Width
                    End If
                End If
            Else
                txtInput.Width = 50

                If Len(lblCaption.Text) > 0 Then
                    nMinimum = nTemp + txtInput.Width
                    Me.Width = nMinimum
                Else
                    ' no caption so...
                    Me.Width = txtInput.Width
                End If
            End If

            Me.Height = 25
            ' realign the control
        Else
            lblCaption.Left = 0
            lblCaption.Top = 0
            nMinimum = lblCaption.Width + 4

            If nMinimum < 50 Then nMinimum = 50

            txtInput.Left = 2
            txtInput.Top = lblCaption.Height + 2

            If m_FieldStyle = FieldStyles.fsAuto Then
                If Me.Width < nMinimum Then
                    Me.Width = nMinimum
                Else
                    txtInput.Width = Me.Width - 3

                    If Len(lblCaption.Text) > 0 Then
                        Me.Height = lblCaption.Height + txtInput.Height + 2
                    Else
                        ' no caption so...
                        txtInput.Top = 2
                        Me.Height = 25
                    End If
                End If
            Else
                txtInput.Width = m_def_FieldWidth - 3

                If nMinimum < txtInput.Width Then nMinimum = txtInput.Width

                Me.Width = nMinimum

                If Len(lblCaption.Text) > 0 Then
                    Me.Height = lblCaption.Height + txtInput.Height + 2
                Else
                    ' no caption so...
                    txtInput.Top = 2
                    Me.Height = 25
                End If
            End If
            'MessageBox.Show("the left is " + CStr(txtinput.Left) + _
            '               " and top is " + CStr(txtinput.Top) + _
            '              " width is " + CStr(txtinput.Width))

        End If

    End Sub
    Private Sub HighLightCaption()

        On Error Resume Next

        lblCaption.BackColor = System.Drawing.ColorTranslator.FromOle(&H8000000D)
        lblCaption.ForeColor = System.Drawing.ColorTranslator.FromOle(&H8000000E)

    End Sub


    Private Sub NormalCaption()

        On Error Resume Next

        lblCaption.BackColor = MyBase.BackColor
        lblCaption.ForeColor = MyBase.ForeColor

    End Sub

    Private Sub lblCaption_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lblCaption.Click

        HighLightCaption()
        txtInput.Focus()

    End Sub
End Class
