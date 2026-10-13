Module modProject

    Public Sub InitMachineList(
    ByVal cbeMachine As DevExpress.XtraEditors.ComboBoxEdit,
    ByVal selectedMachineID As Integer)

        Dim selectedIndex As Integer = -1
        Dim idx As Integer = 0

        cbeMachine.Properties.Items.Clear()

        ' ----------------------------------
        ' Add "None" entry
        ' ----------------------------------
        cbeMachine.Properties.Items.Add(New myItemData("None", 0))

        idx = 1

        ' ----------------------------------
        ' Load Machines from JSON
        ' ----------------------------------
        For Each m In AppData.Machines.OrderBy(Function(x) x.Description)

            Dim item As New myItemData(m.Description, m.ID)

            cbeMachine.Properties.Items.Add(item)

            If m.ID = selectedMachineID Then
                selectedIndex = idx
            End If

            idx += 1
        Next

        ' ----------------------------------
        ' Select current machine if provided
        ' ----------------------------------
        cbeMachine.SelectedIndex =
        If(selectedIndex >= 0, selectedIndex, 0)

    End Sub

    Public Class myItemData
        Private sName As String
        Private iID As Integer
        Private sData As String

        Public Sub New()
            sName = ""
            iID = 0
            sData = ""
        End Sub

        Public Sub New(ByVal Name As String, ByVal ID As Integer, Optional ByVal sItemData As String = Nothing)
            sName = Name
            iID = ID
            sData = sItemData
        End Sub

        Public Property sItemData() As String
            Get
                Return sData
            End Get

            Set(ByVal sValue As String)
                sData = sValue
            End Set
        End Property
        Public Property Name() As String
            Get
                Return sName
            End Get

            Set(ByVal sValue As String)
                sName = sValue
            End Set
        End Property
        Public Property ItemData() As Integer
            Get
                Return iID
            End Get

            Set(ByVal iValue As Integer)
                iID = iValue
            End Set
        End Property

        Public Overrides Function ToString() As String
            Return sName
        End Function

    End Class

    Public Function GetMachineID() As Long
        GetMachineID = RegGetString("Global", "MachineID", "0")
    End Function

    Public Function GetToolSetupID() As Long
        GetToolSetupID = RegGetString("Global", "ToolSetupID", "0")
    End Function

    Public Function GetMaterialID() As Long
        GetMaterialID = RegGetString("Global", "MaterialID", "0")
    End Function
End Module
