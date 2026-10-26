
Imports System.ComponentModel
Imports System.Drawing.Design
Imports System.Windows.Forms
Imports System.Windows.Forms.Design

Public Class DirectiveStationEditor
    Inherits UITypeEditor

    Private Class StationChoice
        Public Property StoredValue As String
        Public Property DisplayText As String

        Public Overrides Function ToString() As String
            Return DisplayText
        End Function
    End Class

    Public Overrides Function GetEditStyle(
        context As ITypeDescriptorContext
    ) As UITypeEditorEditStyle

        Return UITypeEditorEditStyle.DropDown
    End Function

    Public Overrides Function EditValue(
        context As ITypeDescriptorContext,
        provider As IServiceProvider,
        value As Object
    ) As Object

        If context Is Nothing OrElse provider Is Nothing Then
            Return value
        End If

        Dim directive = TryCast(context.Instance, AppData.Directive)

        If directive Is Nothing Then
            Return value
        End If

        Dim editorService =
            TryCast(provider.GetService(
                GetType(IWindowsFormsEditorService)),
                IWindowsFormsEditorService)

        If editorService Is Nothing Then
            Return value
        End If

        Dim choices As New List(Of StationChoice) From {
            New StationChoice With {
                .StoredValue = "**Auto**",
                .DisplayText = "[**Auto**]"
            },
            New StationChoice With {
                .StoredValue = "None",
                .DisplayText = "[None]"
            }
        }

        ' Find the Tool Setup associated with this Directive.
        Dim layerSetup = AppData.LayerSetups.FirstOrDefault(
            Function(ls) ls.ID = directive.LayerSetupID)

        If layerSetup IsNot Nothing AndAlso
           AppData.ToolSetupMembers IsNot Nothing AndAlso
           AppData.StationList IsNot Nothing AndAlso
           AppData.ToolCrib IsNot Nothing Then

            ' Only include tools assigned to this Tool Setup.
            Dim members = AppData.ToolSetupMembers.Where(
                Function(m) m.ToolSetupID = layerSetup.ToolSetupID
            ).ToList()

            For Each member In members

                Dim station = AppData.StationList.FirstOrDefault(Function(s) s.ID = member.StationID)

                Dim tool = AppData.ToolCrib.FirstOrDefault(
                    Function(t) t.ID = member.ToolID)

                If station Is Nothing OrElse tool Is Nothing Then
                    Continue For
                End If

                choices.Add(New StationChoice With {
                    .StoredValue = station.NCCodeNumber.ToString(),
                    .DisplayText =
                        $"[{station.NCCodeNumber}] {tool.Description}"
                })
            Next
        End If

        ' Preserve an existing value if it is not in the current setup.
        Dim currentValue = If(value Is Nothing, "", value.ToString())

        If currentValue <> "" AndAlso
           Not choices.Any(
               Function(c) String.Equals(
                   c.StoredValue, currentValue,
                   StringComparison.OrdinalIgnoreCase)) Then

            choices.Add(New StationChoice With {
                .StoredValue = currentValue,
                .DisplayText = $"[{currentValue}] (not assigned to this Tool Setup)"
            })
        End If

        Dim listBox As New ListBox With {
            .BorderStyle = BorderStyle.FixedSingle,
            .IntegralHeight = False,
            .Width = 300,
            .Height = Math.Min(240, Math.Max(48, choices.Count * 22))
        }

        For Each choice In choices
            listBox.Items.Add(choice)
        Next

        Dim selectedChoice As StationChoice = Nothing

        For i As Integer = 0 To listBox.Items.Count - 1
            Dim choice = DirectCast(listBox.Items(i), StationChoice)

            If String.Equals(
                choice.StoredValue, currentValue,
                StringComparison.OrdinalIgnoreCase) Then

                listBox.SelectedIndex = i
                Exit For
            End If
        Next

        AddHandler listBox.Click,
            Sub(sender, e)
                If listBox.SelectedItem IsNot Nothing Then
                    selectedChoice =
                        DirectCast(listBox.SelectedItem, StationChoice)

                    editorService.CloseDropDown()
                End If
            End Sub

        AddHandler listBox.KeyDown,
            Sub(sender, e)
                If e.KeyCode = Keys.Enter AndAlso
                   listBox.SelectedItem IsNot Nothing Then

                    selectedChoice =
                        DirectCast(listBox.SelectedItem, StationChoice)

                    editorService.CloseDropDown()
                    e.Handled = True
                End If
            End Sub

        editorService.DropDownControl(listBox)

        If selectedChoice IsNot Nothing Then
            Return selectedChoice.StoredValue
        End If

        Return value
    End Function
End Class
