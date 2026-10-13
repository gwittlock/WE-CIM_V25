Imports System.ComponentModel

Namespace WindowsApplication234
    <System.ComponentModel.Designer("System.Windows.Forms.Design.ParentControlDesigner, System.Design", GetType(Design.IDesigner))> _
    Partial Public Class ChkBoxFrame
        Inherits UserControl

        Public Event ValueChange()
        Public Shadows Event ForeColorChanged()
        Public Event EnabledChange()
        Public Event ToggleEffectChange()
        Public Event CaptionChange()
        Public Event CheckedChanged()
        Public Shadows Event Paint()


        Public Shadows Event SystemColorsChanged()


        Shadows Event Click(ByVal Sender As System.Object, ByVal e As System.EventArgs)
        Public Sub New()
            InitializeComponent()

        End Sub

        Private Sub UserControl1_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
            'checkBoxX1.BackColor = panelEx1.Style.BackColor1.Color
            EnableDisableControls()
            Me.BackColor = ParentForm.BackColor
        End Sub

        Public Property CaptionColor() As System.Drawing.Color
            Get
                On Error Resume Next

                CaptionColor = CheckEdit1.ForeColor
            End Get
            Set(ByVal value As System.Drawing.Color)

                On Error Resume Next

                CheckEdit1.ForeColor = value

                CheckEdit1.Refresh()

            End Set
        End Property
        ' Delegate to CheckBox.Checked.
        Public Property Checked() As Boolean
            Get
                Return CheckEdit1.Checked
            End Get
            Set(ByVal Value As Boolean)
                CheckEdit1.Checked = Value

            End Set
        End Property

        Public Property Caption() As String

            Get

                On Error Resume Next

                Caption = CheckEdit1.Text

            End Get
            Set(ByVal Value As String)

                On Error Resume Next

                CheckEdit1.Text = Value
                RaiseEvent CaptionChange()



            End Set
        End Property

        ' Fixes Z-Order of child controls so they appear on top of the panel.
        Protected Overrides Sub OnControlRemoved(ByVal e As System.Windows.Forms.ControlEventArgs)
            If (Me.DesignMode) Then panelEx1.SendToBack()
            MyBase.OnControlRemoved(e)
        End Sub
        Protected Overrides Sub OnControlAdded(ByVal e As System.Windows.Forms.ControlEventArgs)
            panelEx1.SendToBack()
            MyBase.OnControlAdded(e)
            e.Control.Enabled = Me.Enabled
        End Sub

        Protected Overrides Sub OnLayout(ByVal e As System.Windows.Forms.LayoutEventArgs)
            If Me.DesignMode AndAlso Controls.IndexOf(panelEx1) <> 0 Then panelEx1.SendToBack()

            MyBase.OnLayout(e)
        End Sub


        Private Sub checkBoxX1_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)
            EnableDisableControls()
        End Sub

        Private Sub EnableDisableControls()
            For Each item As Control In Me.Controls
                If item IsNot CheckEdit1 And item IsNot panelEx1 Then
                    item.Enabled = CheckEdit1.Checked
                End If
            Next item

            'For Each ctl As Control In Me.Controls
            'If Not (ctl.Name Is "CheckEdit1" Or ctl.Name Is "Panelex1") Then
            'Try
            'ctl.Enabled = CheckEdit1.Checked
            'Catch ex As Exception
            'End Try
            'End If
            'Next ctl
        End Sub

        Private Sub CheckEdit1_CheckStateChanged(sender As Object, e As EventArgs) Handles CheckEdit1.CheckStateChanged
            RaiseEvent CheckedChanged()
        End Sub

        Private Sub CheckEdit1_EditValueChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles CheckEdit1.EditValueChanged
            EnableDisableControls()
        End Sub

        Private Sub CheckEdit1_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles CheckEdit1.Paint
            Dim myFont As New Font(CheckEdit1.Font, CheckEdit1.Font.Size)
            Dim StringSize As New SizeF

            StringSize = e.Graphics.MeasureString(CheckEdit1.Text, myFont)
            CheckEdit1.Width = StringSize.Width + 20
        End Sub

        Private Sub panelEx1_Paint(ByVal sender As System.Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles panelEx1.Paint

        End Sub

        Private Sub ChkBoxFrame_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Resize
            panelEx1.Width = MyBase.Width - 5
            panelEx1.Height = MyBase.Height - 5

        End Sub

    End Class
End Namespace
