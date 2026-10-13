Option Strict Off
Option Explicit On
Friend Class MathTextBox
    Inherits System.Windows.Forms.UserControl
    Public Event FieldStyleChange()
    Public Event TextChange()
    Public Event EvaluatorUsedChange()
    Public Event CaptionChange()
    Public Event MinValueChange()
    Public Event FormatStringChange()
    Public Event MinValueUsedChange()
    Public Event ForeColorChange()
    Public Event MaxValueUsedChange()
    Public Event MaxValueChange()
    Public Event LockedChange()
    Public Event CaptionLocationChange()
    Public Event MaxLengthChange()
    Public Event EnabledChange()
    Public Event SuppressedChange()
    Public Event FieldWidthChange()
    Public Event UseMnemonicChange()
    Public Event BackColorChange()
    Dim bSkip As Boolean
    Dim nRight As Integer
    ' Flags settings:
    Const bit00 As Integer = &H1S
    Const bit01 As Integer = &H2S
    Const bit02 As Integer = &H4S
    Const bit03 As Integer = &H8S
    Const bit04 As Integer = &H10S
    ' Flags for arithmetic
    Const push1 As Boolean = bit01 Or bit02 Or bit03 Or bit04 ' +-
    Const push2 As Boolean = bit02 Or bit03 Or bit04 ' */
    Const push3 As Boolean = bit03 Or bit04 ' ^
    Const push4 As Integer = bit04 ' negation -
    ' mathematic operators
    Const plus As Integer = 43
    Const minus As Integer = 45 '+-
    Const Multiply As Integer = 42
    Const Divide As Integer = 47 '*/
    Const Exponent As Integer = 94 '^
    Const OpenParen As Integer = 40
    Const CloseParen As Integer = 41 '()
    Const DecPoint As Integer = 46
    ' enumerations
    Public Enum CaptionLocations
        clLeft = 0
        clTop = 1
    End Enum
    Public Enum FieldStyles
        fsFixed = 0
        fsAuto = 1
    End Enum
    ' shared variables
    Private nCurrentCharPos As Integer
    Private nEvalError As Integer
    Private sExpression As String
    'Event Declarations:
    Event Change(ByVal Sender As System.Object, ByVal e As System.EventArgs) 'MappingInfo=txtMath,txtMath,-1,Change
    Shadows Event KeyPress(ByVal Sender As System.Object, ByVal e As KeyPressEventArgs) 'MappingInfo=txtMath,txtMath,-1,KeyPress
    'Default Property Values:
    'UPGRADE_NOTE: m_def_CaptionLocation was changed from a Constant to a Variable. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="C54B49D7-5804-4D48-834B-B3D81E4C2F13"'
    Dim m_def_CaptionLocation As CaptionLocations = CaptionLocations.clLeft
    Const m_def_EvaluatorUsed As Boolean = True
    Const m_def_FieldWidth As Integer = 100
    'UPGRADE_NOTE: m_def_FieldStyle was changed from a Constant to a Variable. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="C54B49D7-5804-4D48-834B-B3D81E4C2F13"'
    Dim m_def_FieldStyle As FieldStyles = FieldStyles.fsFixed
    Const m_def_FormatString As String = "0.0#####;;0"
    Const m_def_MaxValue As Integer = 0
    Const m_def_MaxValueUsed As Boolean = False
    Const m_def_MinValue As Integer = 0
    Const m_def_MinValueUsed As Boolean = False
    'Property Variables:
    Dim m_CaptionLocation As CaptionLocations
    Dim m_EvaluatorUsed As Boolean
    Dim m_FieldWidth As Integer
    Dim m_FieldStyle As FieldStyles
    Dim m_FormatString As String
    Dim m_MaxValue As Object
    Dim m_MaxValueUsed As Boolean
    Dim m_MinValue As Object
    Dim m_MinValueUsed As Boolean
    Dim m_suppressed As Boolean


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=txtMath,txtMath,-1,BackColor


    Public Overrides Property BackColor() As System.Drawing.Color
        Get

            'On Error Resume Next

            BackColor = MyBase.BackColor

        End Get
        Set(ByVal Value As System.Drawing.Color)

            'On Error Resume Next

            MyBase.BackColor = Value
            lblCaption.BackColor = MyBase.BackColor
            RaiseEvent BackColorChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=lblCaption,lblCaption,-1,Caption


    Public Property Caption() As String
        Get

            'On Error Resume Next

            Caption = lblCaption.Text

        End Get
        Set(ByVal Value As String)

            lblCaption.Text = Value
            bSkip = False
            MathTextBox_Resize(Me, New System.EventArgs())
            RaiseEvent CaptionChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=14,1,0,0


    Public Property CaptionLocation() As CaptionLocations
        Get

            'On Error Resume Next

            CaptionLocation = m_CaptionLocation

        End Get
        Set(ByVal Value As CaptionLocations)

            'On Error Resume Next

            m_CaptionLocation = Value
            MathTextBox_Resize(Me, New System.EventArgs())
            RaiseEvent CaptionLocationChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=UserControl,UserControl,-1,Enabled


    Public Shadows Property Enabled() As Boolean
        Get

            'On Error Resume Next

            Enabled = MyBase.Enabled

        End Get
        Set(ByVal Value As Boolean)

            'On Error Resume Next

            MyBase.Enabled = Value
            txtMath.Enabled = Value
            lblCaption.Enabled = Value
            RaiseEvent EnabledChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=0,0,0,True


    Public Property EvaluatorUsed() As Boolean
        Get

            'On Error Resume Next

            EvaluatorUsed = m_EvaluatorUsed

        End Get
        Set(ByVal Value As Boolean)

            'On Error Resume Next

            m_EvaluatorUsed = Value
            RaiseEvent EvaluatorUsedChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=14,1,0,0


    Public Property FieldStyle() As FieldStyles
        Get

            'On Error Resume Next

            FieldStyle = m_FieldStyle

        End Get
        Set(ByVal Value As FieldStyles)

            'On Error Resume Next

            m_FieldStyle = Value
            MathTextBox_Resize(Me, New System.EventArgs())
            RaiseEvent FieldStyleChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=14,1,0,0


    Public Property FieldWidth() As Integer
        Get

            'On Error Resume Next

            FieldWidth = m_FieldWidth

        End Get
        Set(ByVal Value As Integer)

            'On Error Resume Next

            If Value < 255 Then Value = 255
            m_FieldWidth = Value
            MathTextBox_Resize(Me, New System.EventArgs())
            RaiseEvent FieldWidthChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=txtMath,txtMath,-1,ForeColor


    Public Overrides Property ForeColor() As System.Drawing.Color
        Get

            'On Error Resume Next

            ForeColor = txtMath.ForeColor

        End Get
        Set(ByVal Value As System.Drawing.Color)

            'On Error Resume Next

            txtMath.ForeColor = Value
            RaiseEvent ForeColorChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=13,0,0,0.0###;;0


    Public Property FormatString() As String
        Get

            'On Error Resume Next

            FormatString = m_FormatString

        End Get
        Set(ByVal Value As String)

            'On Error Resume Next

            m_FormatString = Value
            RaiseEvent FormatStringChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=txtMath,txtMath,-1,Locked


    Public Property Locked() As Boolean
        Get

            'On Error Resume Next

            Locked = txtMath.ReadOnly

        End Get
        Set(ByVal Value As Boolean)

            'On Error Resume Next

            txtMath.ReadOnly = Value
            RaiseEvent LockedChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=txtMath,txtMath,-1,Suppressed


    Public Property Suppressed() As Boolean
        Get

            Suppressed = m_suppressed

        End Get
        Set(ByVal Value As Boolean)

            m_suppressed = Value
            RaiseEvent SuppressedChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=txtMath,txtMath,-1,MaxLength


    Public Property MaxLength() As Integer
        Get

            'On Error Resume Next

            'UPGRADE_WARNING: TextBox property txtMath.MaxLength has a new behavior. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6BA9B8D2-2A32-4B6E-8D36-44949974A5B4"'
            MaxLength = txtMath.Maxlength

        End Get
        Set(ByVal Value As Integer)

            'On Error Resume Next

            'UPGRADE_WARNING: TextBox property txtMath.MaxLength has a new behavior. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6BA9B8D2-2A32-4B6E-8D36-44949974A5B4"'
            txtMath.Maxlength = Value
            RaiseEvent MaxLengthChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=14,0,0,0


    Public Property MaxValue() As Object
        Get

            'On Error Resume Next

            'UPGRADE_WARNING: Couldn't resolve default property of object m_MaxValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
            'UPGRADE_WARNING: Couldn't resolve default property of object MaxValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
            MaxValue = m_MaxValue

        End Get
        Set(ByVal Value As Object)

            'On Error Resume Next

            'UPGRADE_WARNING: Couldn't resolve default property of object New_MaxValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
            'UPGRADE_WARNING: Couldn't resolve default property of object m_MaxValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
            m_MaxValue = Value
            RaiseEvent MaxValueChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=0,0,0,False


    Public Property MaxValueUsed() As Boolean
        Get

            'On Error Resume Next

            MaxValueUsed = m_MaxValueUsed

        End Get
        Set(ByVal Value As Boolean)

            'On Error Resume Next

            m_MaxValueUsed = Value
            RaiseEvent MaxValueUsedChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=14,0,0,0


    Public Property MinValue() As Object
        Get

            'On Error Resume Next

            'UPGRADE_WARNING: Couldn't resolve default property of object m_MinValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
            'UPGRADE_WARNING: Couldn't resolve default property of object MinValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
            MinValue = m_MinValue

        End Get
        Set(ByVal Value As Object)

            'On Error Resume Next

            'UPGRADE_WARNING: Couldn't resolve default property of object New_MinValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
            'UPGRADE_WARNING: Couldn't resolve default property of object m_MinValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
            m_MinValue = Value
            RaiseEvent MinValueChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MemberInfo=0,0,0,False


    Public Property MinValueUsed() As Boolean
        Get

            'On Error Resume Next

            MinValueUsed = m_MinValueUsed

        End Get
        Set(ByVal Value As Boolean)

            'On Error Resume Next

            m_MinValueUsed = Value
            RaiseEvent MinValueUsedChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=txtMath,txtMath,-1,Text


    Public Overrides Property Text() As String
        Get

            Text = txtMath.Text

        End Get
        Set(ByVal Value As String)

            txtMath.Text = Value
            txtMath.Tag = Value ' update tag if setting through code
            RaiseEvent TextChange()

        End Set
    End Property


    'WARNING! DO NOT REMOVE OR MODIFY THE FOLLOWING COMMENTED LINES!
    'MappingInfo=lblCaption,lblCaption,-1,UseMnemonic


    Public Property UseMnemonic() As Boolean
        Get

            'On Error Resume Next

            UseMnemonic = lblCaption.UseMnemonic

        End Get
        Set(ByVal Value As Boolean)

            'On Error Resume Next

            lblCaption.UseMnemonic = Value
            RaiseEvent UseMnemonicChange()

        End Set
    End Property


    'UPGRADE_WARNING: Event txtMath.TextChanged may fire when form is initialized. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="88B12AE1-6DE0-48A0-86F1-60C0686C026A"'
    Private Sub txtMath_TextChanged(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles txtMath.TextChanged

        'On Error Resume Next

        RaiseEvent TextChange()
        If (Not Suppressed()) Then RaiseEvent Change(Me, Nothing)

    End Sub


    Private Sub txtMath_Enter(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles txtMath.Enter

        'On Error Resume Next

        If Len(txtMath.Tag) > 0 Then
            txtMath.Text = txtMath.Tag ' restore from tagged information
        End If

        txtMath.SelectionStart = 0
        txtMath.SelectionLength = Len(txtMath.Text)
        HighLightCaption()

    End Sub


    Private Sub txtMath_KeyPress(ByVal eventSender As System.Object, ByVal eventArgs As System.Windows.Forms.KeyPressEventArgs) Handles txtMath.KeyPress
        Dim KeyAscii As Integer = Asc(eventArgs.KeyChar)

        'On Error Resume Next

        RaiseEvent KeyPress(Me, New KeyPressEventArgs(KeyAscii))

        If m_EvaluatorUsed = True Then
            If KeyAscii = 13 Then
                KeyAscii = 0
                System.Windows.Forms.SendKeys.Send("{TAB}") ' treat as a tab to calculate
            End If
        Else
            If KeyAscii < 48 Or KeyAscii > 57 Then
                Select Case KeyAscii

                    Case 8 ' backspace

                    Case 13 ' return
                        KeyAscii = 0
                        System.Windows.Forms.SendKeys.Send("{TAB}") ' treat as a tab to calculate

                    Case 27 ' escape

                    Case 32 ' space

                    Case 40 ' (

                    Case 41 ' )

                    Case 42 ' *

                    Case 43 ' +

                    Case 45 ' -

                    Case 46 ' .

                    Case 47 ' /

                    Case 94 ' ^

                    Case Else
                        KeyAscii = 0

                End Select
            End If
        End If

        eventArgs.KeyChar = Chr(KeyAscii)
        If KeyAscii = 0 Then
            eventArgs.Handled = True
        End If
    End Sub


    Public Sub txtMath_Leave(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles txtMath.Leave

        'On Error Resume Next

        If (Len(txtMath.Text) > 0) Then

            txtMath.Text = Calculate(txtMath.Text)

            If (nEvalError <> 0) Then

                'NOTE: At this point, txtMath.text should be the empty string.
                nEvalError = 0 ' reset

                txtMath.Text = txtMath.Tag 'restore the original input
                txtMath.Focus()

            Else

                txtMath.Tag = txtMath.Text 'save for restoration

                NormalCaption()

            End If

        Else

            'Hmmmmm, should we really allow an *empty* math input?

        End If

    End Sub

    Private Sub UserControl_Initialize()
        m_EvaluatorUsed = True
        m_suppressed = False
    End Sub

    'Initialize Properties for User Control
    'UPGRADE_ISSUE: UserControl event UserControl.InitProperties was not upgraded. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="ABD9AF39-7E24-4AFF-AD8D-3675C1AA3054"'
    Private Sub UserControl_InitProperties()

        'On Error Resume Next

        m_CaptionLocation = m_def_CaptionLocation
        m_EvaluatorUsed = m_def_EvaluatorUsed
        m_FieldStyle = m_def_FieldStyle
        m_FieldWidth = m_def_FieldWidth
        m_FormatString = m_def_FormatString
        'UPGRADE_WARNING: Couldn't resolve default property of object m_MaxValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
        m_MaxValue = m_def_MaxValue
        m_MaxValueUsed = m_def_MaxValueUsed
        'UPGRADE_WARNING: Couldn't resolve default property of object m_MinValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
        m_MinValue = m_def_MinValue
        m_MinValueUsed = m_def_MinValueUsed
        m_suppressed = False

    End Sub


    'Load property values from storage
    Private Sub UserControl_ReadProperties(ByRef PropBag As Object)

        txtMath.BackColor = System.Drawing.ColorTranslator.FromOle(PropBag.ReadProperty("BackColor", &H80000005))
        lblCaption.Text = PropBag.ReadProperty("Caption", "Caption")
        m_CaptionLocation = PropBag.ReadProperty("CaptionLocation", m_def_CaptionLocation)
        MyBase.Enabled = PropBag.ReadProperty("Enabled", True)
        m_EvaluatorUsed = PropBag.ReadProperty("EvaluatorUsed", m_def_EvaluatorUsed)
        m_FieldStyle = PropBag.ReadProperty("FieldStyle", m_def_FieldStyle)
        m_FieldWidth = PropBag.ReadProperty("FieldWidth", m_def_FieldWidth)
        Font = PropBag.ReadProperty("Font", ParentForm.Font)
        txtMath.ForeColor = System.Drawing.ColorTranslator.FromOle(PropBag.ReadProperty("ForeColor", &H80000008))
        m_FormatString = PropBag.ReadProperty("FormatString", m_def_FormatString)
        txtMath.ReadOnly = PropBag.ReadProperty("Locked", False)
        txtMath.MaxLength = PropBag.ReadProperty("MaxLength", 0)
        m_MaxValue = PropBag.ReadProperty("MaxValue", m_def_MaxValue)
        m_MaxValueUsed = PropBag.ReadProperty("MaxValueUsed", m_def_MaxValueUsed)
        m_MinValue = PropBag.ReadProperty("MinValue", m_def_MinValue)
        m_MinValueUsed = PropBag.ReadProperty("MinValueUsed", m_def_MinValueUsed)
        txtMath.Text = PropBag.ReadProperty("Text", "")
        m_suppressed = PropBag.ReadProperty("Suppressed", False)
        lblCaption.UseMnemonic = PropBag.ReadProperty("UseMnemonic", True)
        ' update control properties from parent
        txtMath.Enabled = MyBase.Enabled
        lblCaption.Enabled = MyBase.Enabled

    End Sub


    Private Sub MathTextBox_Resize(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles MyBase.Resize
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

            txtMath.Left = nTemp
            txtMath.Top = 0

            If m_FieldStyle = FieldStyles.fsAuto Then
                nMinimum = nTemp + 100 + 2
                nTemp = nTemp + txtMath.Width + 2

                If Me.Width < nMinimum Then
                    Me.Width = nMinimum
                Else
                    If Len(lblCaption.Text) > 0 Then
                        txtMath.Width = Me.Width - (lblCaption.Width + 4)
                    Else
                        ' no caption so...
                        txtMath.Width = Me.Width
                    End If
                End If
            Else
                txtMath.Width = 100

                If Len(lblCaption.Text) > 0 Then
                    nMinimum = nTemp + txtMath.Width
                    Me.Width = nMinimum
                Else
                    ' no caption so...
                    Me.Width = txtMath.Width
                End If
            End If

            Me.Height = 25
            ' realign the control
        Else
            lblCaption.Left = 0
            lblCaption.Top = 0
            nMinimum = lblCaption.Width + 4

            If nMinimum < 100 Then nMinimum = 100

            txtMath.Left = 2
            txtMath.Top = lblCaption.Height + 2

            If m_FieldStyle = FieldStyles.fsAuto Then
                If Me.Width < nMinimum Then
                    Me.Width = nMinimum
                Else
                    txtMath.Width = Me.Width - 3

                    If Len(lblCaption.Text) > 0 Then
                        Me.Height = lblCaption.Height + txtMath.Height + 2
                    Else
                        ' no caption so...
                        txtMath.Top = 2
                        Me.Height = 25
                    End If
                End If
            Else
                txtMath.Width = m_def_FieldWidth - 3

                If nMinimum < txtMath.Width Then nMinimum = txtMath.Width

                Me.Width = nMinimum

                If Len(lblCaption.Text) > 0 Then
                    Me.Height = lblCaption.Height + txtMath.Height + 2
                Else
                    ' no caption so...
                    txtMath.Top = 2
                    Me.Height = 25
                End If
            End If
            'MessageBox.Show("the left is " + CStr(txtmath.Left) + _
            '               " and top is " + CStr(txtmath.Top) + _
            '              " width is " + CStr(txtmath.Width))

        End If

    End Sub



    'UPGRADE_ISSUE: UserControl event UserControl.Show was not upgraded. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="ABD9AF39-7E24-4AFF-AD8D-3675C1AA3054"'
    Private Sub UserControl_WriteProperties(ByRef PropBag As Object)
        Call PropBag.WriteProperty("BackColor", System.Drawing.ColorTranslator.ToOle(txtMath.BackColor), &H80000005)
        Call PropBag.WriteProperty("Caption", lblCaption.Text, "Caption")
        Call PropBag.WriteProperty("CaptionLocation", m_CaptionLocation, m_def_CaptionLocation)
        Call PropBag.WriteProperty("Enabled", MyBase.Enabled, True)
        Call PropBag.WriteProperty("EvaluatorUsed", m_EvaluatorUsed, m_def_EvaluatorUsed)
        Call PropBag.WriteProperty("FieldStyle", m_FieldStyle, m_def_FieldStyle)
        Call PropBag.WriteProperty("FieldWidth", m_FieldWidth, m_def_FieldWidth)
        Call PropBag.WriteProperty("Font", Font, ParentForm.Font)
        Call PropBag.WriteProperty("ForeColor", System.Drawing.ColorTranslator.ToOle(txtMath.ForeColor), &H80000008)
        Call PropBag.WriteProperty("FormatString", m_FormatString, m_def_FormatString)
        Call PropBag.WriteProperty("Locked", txtMath.ReadOnly, False)
        Call PropBag.WriteProperty("MaxLength", txtMath.MaxLength, 0)
        Call PropBag.WriteProperty("MaxValue", m_MaxValue, m_def_MaxValue)
        Call PropBag.WriteProperty("MaxValueUsed", m_MaxValueUsed, m_def_MaxValueUsed)
        Call PropBag.WriteProperty("MinValue", m_MinValue, m_def_MinValue)
        Call PropBag.WriteProperty("MinValueUsed", m_MinValueUsed, m_def_MinValueUsed)
        Call PropBag.WriteProperty("Text", txtMath.Text, "")
        Call PropBag.WriteProperty("Suppressed", m_suppressed, False)
        Call PropBag.WriteProperty("UseMnemonic", lblCaption.UseMnemonic, True)

    End Sub


    Private Function Calculate(ByVal sFormula As String) As String

        Dim lReturn As Integer
        Dim dTemp As Double

        'On Error Resume Next
        Calculate = "" 'assume failure.

        sExpression = sFormula
        nCurrentCharPos = 0
        nEvalError = 0

        If (m_EvaluatorUsed = True) Then
            'lReturn = PortalExecute("*solve:eval:exp=" & QStr(sFormula))

            If (lReturn = 0) Then nEvalError = 1

            'dTemp = PortalGetDouble("val", 0)
        Else
            dTemp = Evaluate(0.0#, 0)
        End If

        If (nEvalError = 0) Then

            If (m_MinValueUsed = True) Then
                'UPGRADE_WARNING: Couldn't resolve default property of object m_MinValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
                If (dTemp < CDbl(m_MinValue)) Then
                    'UPGRADE_WARNING: Couldn't resolve default property of object m_MinValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
                    MsgBox("Invalid value - must be greater than or equal to: " & CStr(m_MinValue), MsgBoxStyle.Exclamation, My.Application.Info.Title)
                    nEvalError = 1
                End If
            End If

            If ((nEvalError = 0) And (m_MaxValueUsed = True)) Then
                'UPGRADE_WARNING: Couldn't resolve default property of object m_MaxValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
                If (dTemp > CDbl(m_MaxValue)) Then
                    'UPGRADE_WARNING: Couldn't resolve default property of object m_MaxValue. Click for more: 'ms-help://MS.VSExpressCC.v80/dv_commoner/local/redirect.htm?keyword="6A50421D-15FE-4896-8A1B-2EC21E9037B2"'
                    MsgBox("Invalid value - must be less than or equal to: " & CStr(m_MaxValue), MsgBoxStyle.Exclamation, My.Application.Info.Title)
                    nEvalError = 1
                End If
            End If

            Calculate = FormatNumber(dTemp, m_FormatString)

        Else

            MsgBox("Invalid expresion.", MsgBoxStyle.Exclamation, My.Application.Info.Title)
            txtMath.Focus()

        End If

    End Function


    Private Function Evaluate(ByRef dItem As Object, ByRef decFlag As Object) As Double

        ' This function Evaluates the shared sExpression
        ' holding an arithmetic expression.

        Static nChar As Integer
        Static sTemp As String
        Static sCollection As String
        Static dTest As Double

        dTest = 0

        If nEvalError Then Exit Function
        Do While nCurrentCharPos < Len(sExpression)
            nCurrentCharPos = nCurrentCharPos + 1
            sTemp = Mid(sExpression, nCurrentCharPos, 1)
            nChar = Asc(sTemp)

            Select Case nChar

                Case 48 To 57 ' ignore blanks
                    sCollection = sCollection & sTemp

                Case DecPoint
                    If decFlag And bit00 Then
                        ' we've got a fractional part already
                        nEvalError = 2
                        Exit Do
                    Else
                        ' turn on bit00 to indicate we're collecting the fractional part
                        decFlag = decFlag Or bit00
                        sCollection = sCollection & sTemp
                    End If

                Case plus

                    If decFlag And push1 Then Exit Do

                    GoTo collectItem
                    dItem = dItem + Evaluate(0.0#, bit01)

                Case minus
                    If decFlag And push1 Then Exit Do

                    GoTo collectItem

                    dItem = dItem - Evaluate(0.0#, bit01)

                Case Multiply
                    If decFlag And push2 Then Exit Do

                    GoTo collectItem

                    dItem = dItem * Evaluate(0.0#, bit02)

                Case Divide
                    If decFlag And push2 Then Exit Do

                    GoTo collectItem
                    dTest = Evaluate(0.0#, bit02)

                    If dTest Then

                        dItem = dItem / dTest
                    Else
                        nEvalError = 11
                    End If

                Case Exponent
                    If decFlag And push3 Then Exit Do


                    GoTo collectItem

                    dItem = dItem ^ Evaluate(0.0#, bit03)

                Case OpenParen

                    dItem = Evaluate(0.0#, 0)

                Case CloseParen
                    nCurrentCharPos = nCurrentCharPos + 1
                    Exit Do

                Case Else
                    nEvalError = 2
                    Exit Do

            End Select
        Loop

push:

        If Len(sCollection) Then
            Evaluate = Val(sCollection)
            sCollection = ""
        Else
            Evaluate = dItem
        End If

        If nCurrentCharPos < Len(sExpression) Then nCurrentCharPos = nCurrentCharPos - 1

        Exit Function
collectItem:

        If Len(sCollection) Then dItem = Val(sCollection)

        sCollection = ""


    End Function


    Public Sub HighLightCaption()

        'On Error Resume Next

        lblCaption.BackColor = System.Drawing.ColorTranslator.FromOle(&H8000000D)
        lblCaption.ForeColor = System.Drawing.ColorTranslator.FromOle(&H8000000E)

    End Sub


    Public Sub NormalCaption()

        lblCaption.BackColor = MyBase.BackColor
        lblCaption.ForeColor = MyBase.ForeColor

    End Sub

    Private Sub lblCaption_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lblCaption.Click
        txtMath.Focus()
    End Sub
End Class