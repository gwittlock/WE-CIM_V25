Imports Microsoft.Win32
Imports System.Security.Permissions

Module modRegistry
    Public EntryName() As String

    Public EntryValue() As String

    Private root As Array = System.Enum.GetValues(GetType(RegistryHive))
    Private rootNames() As String
    Const localMachineRoot As String = "HKEY_LOCAL_MACHINE"

    Private Const ERROR_SUCCESS = 0&

    Private CURRENT_VERSION As String
    Public Const App_Title = "WE-CIM Advanced Fabrication"
    Public Const PRODUCT_NAME = "WE-CIM"

    Public Function RegGetString(
                    ByVal folder As String,
                    ByVal entry As String,
                    ByVal def_value As Object) As String
        Dim subkey As String

        subkey = RegAppKey() & "\" & folder

        RegGetString = ReadRegistryValue(Registry.CurrentUser, subkey, entry, def_value)

    End Function

    Public Function RegGetInt(
                    ByVal folder As String,
                    ByVal entry As String,
                    ByVal def_value As Object) As Integer
        Dim subkey As String

        subkey = RegAppKey() & "\" & folder

        RegGetInt = CInt(ReadRegistryValue(Registry.CurrentUser, subkey, entry, def_value))

    End Function
    Public Function RegPutJavaEx(ByVal sMajorKey As String, ByVal sMinorKey As String, _
                               ByVal sSubKey As String, ByVal sValue As String)

        ' lRetVal = RegPutStringEx("PanFront", "Preferences", "Demo Mode", "1")
        '                      or
        ' lRetVal = RegPutStringEx("PanFront", "Preferences\Display", "Color", "1")

        Dim sKey As String

        RegPutJavaEx = Nothing


        sMajorKey = Trim$(sMajorKey)
        sMinorKey = Trim$(sMinorKey)
        sSubKey = Trim$(sSubKey)
        sValue = Trim$(sValue)

        If Len(sValue) < 1 Then
            sValue = ""
        End If

        sKey = "SOFTWARE\" & sMajorKey & "\" & sMinorKey
        ' using RegCreateKey instead of RegOpenKey
        ' in case the key does not currently exist

        WriteSubKeyValue(Registry.CurrentUser, sSubKey, sKey, sValue)



    End Function
    Public Function RegGetJavaEx( _
                            ByVal subkey As String, _
                            ByVal entry As String, _
                            ByVal def_value As Object) As String

        RegGetJavaEx = ReadRegistryValue(Registry.CurrentUser, subkey, entry, def_value)

    End Function
    Public Function RegAppKey() As String

        Dim app_key As String

        app_key = "Software\" & PRODUCT_NAME

        'Set the CurrentVersion string value in the registry.
        'This may be required by Java macros and backgroung applications.
        '
        'Also, by setting the CurrentVersion each time RegAppKey() is called,
        'we should be able to run different versions of the app concurrently.
        '
        CURRENT_VERSION = My.Application.Info.Version.Major & "." & My.Application.Info.Version.Minor

        RegAppKey = app_key & "\" & CURRENT_VERSION


    End Function

    Private Function RegGetString_core( _
                    ByVal root_key As String, _
                    ByVal sub_key As String, _
                    ByVal Item As String, _
                    ByVal def_value As Object) As String

        Dim instance As RegistryKey
        Dim keyName As String
        Dim sKeyNames() As String
        Dim nAppMajor As Long

        RegGetString_core = Nothing

        Try

            'Assume Failure
            RegGetString_core = ""
            Dim exists As Boolean = False

            Dim regKey As Object
            regKey = Nothing

            'Gets specific registry settings   

            If (UCase(root_key) = "HKEY_CURRENT_USER") Then
                nAppMajor = My.Application.Info.Version.Major


                'instance = Registry.CurrentUser.OpenSubKey(sub_key)
                keyName = "HKEY_CURRENT_USER\\Software\\WE-CIM\\" & nAppMajor & "\\" & sub_key

            Else
                ' instance = Registry.LocalMachine.OpenSubKey(sub_key)
                nAppMajor = My.Application.Info.Version.Major

                keyName = "HKEY_LOCAL_MACHINE\\Software\\WE-CIM\\" & nAppMajor & "\\" & sub_key

            End If

            'Grant Read, Write and Create permissions for the key
            Dim f As New RegistryPermission( _
            RegistryPermissionAccess.Read Or RegistryPermissionAccess.Write Or RegistryPermissionAccess.Create, _
            keyName)


            instance = Registry.CurrentUser.OpenSubKey(sub_key)
            sKeyNames = instance.GetValueNames

            If (sKeyNames.Count = 0) Then

                If (UCase(root_key) = "HKEY_CURRENT_USER") Then
                    regKey = My.Computer.Registry.CurrentUser.CreateSubKey(sub_key)
                Else
                    regKey = My.Computer.Registry.LocalMachine.CreateSubKey(sub_key)
                End If
                ' It doesn't exist here. Create the key.

                ' Next, set the key name and value.
                regKey.SetValue(Item, def_value)

            Else

                My.Computer.Registry.SetValue(keyName, Item, "1")

            End If
        Catch ex As Exception

            MessageBox.Show("RegGetString_core: " & ex.Message)

        End Try

    End Function

    Public Sub GetSettings()
        'Gets specific registry settings   
        Dim regKey As RegistryKey
        regKey = Registry.CurrentUser.OpenSubKey("Software\Name", True)
        Try
            'Sets value of registry key   
            regKey.SetValue("APPName", "Application Name")
        Catch ex As NullReferenceException
            'If key does not already exists creates key and populates data   
            regKey = Registry.CurrentUser.OpenSubKey("SOFTWARE", True)
            regKey.CreateSubKey("Name Of SubKey")
            regKey.SetValue("APPName", "Name of Application")
        End Try
        ' myValue = regKey.GetValue("Key Name", "Default Value")
    End Sub
    Public Sub SetSettings()
        Dim regKey As RegistryKey
        Dim PhoneIP As String = String.Empty
        Try
            regKey = Registry.CurrentUser.OpenSubKey("Software\Name", True)
            regKey.SetValue("APPName", "Name of Application")
            regKey.SetValue("Registry Key Name", "Value")
            regKey.Close()
        Catch ex As Exception
            'do something   
        End Try
    End Sub


    Public Function ReadRegistryValue(ByVal MainKey As RegistryKey, ByVal sKey As String, ByVal sKeyName As String, _
                                ByRef oNameValue As Object) As Object
        Dim rkKey As RegistryKey
        Dim Value As New String("")

        Try
            'open the given subkey
            'rkKey = MainKey.OpenSubKey(sKey, True)
            rkKey = MainKey.CreateSubKey(sKey, RegistryKeyPermissionCheck.Default)
            Value = rkKey.GetValue(sKeyName)

            'check to see if the subkey exists
            If Value Is Nothing Then 'it doesnt exist

                rkKey.SetValue(sKeyName, oNameValue, RegistryValueKind.String)
                Value = oNameValue
            Else
                oNameValue = rkKey.GetValue(sKeyName)
                Value = oNameValue.ToString

            End If



        Catch ex As Exception
            MessageBox.Show(ex.Message, "Error: Reading Registry Value", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
        Return oNameValue
    End Function

    ''' Writes a value in the Registry
    Public Function WriteSubKeyValue(ByVal MainKey As RegistryKey, ByVal sKey As String, ByVal sKeyName As String, _
                                ByRef oNameValue As Object) As Boolean
        Dim rkKey As RegistryKey


        Try
            'open the given subkey

            rkKey = MainKey.CreateSubKey(sKey, RegistryKeyPermissionCheck.Default)

            rkKey.SetValue(sKeyName, oNameValue, RegistryValueKind.String)

        Catch ex As Exception

            MessageBox.Show(ex.Message, "Error: Writing Registry Value", MessageBoxButtons.OK, MessageBoxIcon.Error)

        End Try
    End Function


    Public Function RegPutString( _
                        ByVal folder As String, _
                        ByVal entry As String, _
                        ByVal sValue As String) As String
        RegPutString = ""
        Dim subkey As String

        subkey = RegAppKey() & "\" & folder

        WriteSubKeyValue(Registry.CurrentUser, subkey, entry, sValue)
    End Function
    Public Function EnumerateRegValues( _
                    ByVal sSubKey As String, _
                    Optional ByVal sDefault As String = "") As String()

        Dim instance As RegistryKey = Registry.CurrentUser.OpenSubKey(RegAppKey() & "\\" & sSubKey)

        EnumerateRegValues = instance.GetValueNames

    End Function

    Public Function DynatorchGetString( _
                    ByVal folder As String, _
                    ByVal entry As String, _
                    ByVal def_value As Object) As String

        Dim subkey As String


        'subkey = ClemSoft & "\" & folder
        subkey = folder

        DynatorchGetString = RegGetString_core("HKEY_CURRENT_USER", subkey, entry, def_value)

    End Function

    Public Function DynatorchPutString( _
                        ByVal folder As String, _
                        ByVal entry As String, _
                        ByVal sValue As String) As String

        Dim status As String

        DynatorchPutString = "failed"

        Try
            status = RegPrimitivePutString("HKEY_CURRENT_USER", folder, entry, sValue)
            DynatorchPutString = "succeeded"

        Catch ex As Exception

            status = "Error in DynatorchPutString( " & folder & ", " & entry & " )"

            lReturn = MsgBox("Warning", status, vbOKOnly)

        End Try

    End Function

    Public Function RegPrimitivePutString( _
                        ByVal root_key As String, _
                        ByVal sub_key As String, _
                        ByVal sItem As String, _
                        ByVal sValue As String) As String

        Dim regKey As Object

        ' using RegCreateKey instead of RegOpenKey
        ' in case the key does not currently exist

        RegPrimitivePutString = "succeeded"

        If (UCase(root_key) = "HKEY_CURRENT_USER") Then
            regKey = My.Computer.Registry.CurrentUser.CreateSubKey(sub_key)
        Else
            regKey = My.Computer.Registry.LocalMachine.CreateSubKey(sub_key)
        End If

        regKey.SetValue(sItem, sValue)

    End Function

    Public Sub RegDeleteSetting(ByVal sMajorKey As String, ByVal sMinorKey As String, _
                                   ByVal sSubKey As String, ByVal sValue As String)

        Dim sKey As String
        sKey = RegAppKey() & "\" & sMajorKey & "\" & sMinorKey & "\" & sSubKey

        Dim foundKey As RegistryKey = My.Computer.Registry.CurrentUser.OpenSubKey(sKey, True)

        If Not (foundKey Is Nothing) Then
            foundKey.DeleteValue(sValue)
        End If


    End Sub

    Public Function RegClearWECADImage( _
                        ByVal sKey As String, _
                        ByVal sSubKey As String, _
                        ByVal sValue As String) As String

        Dim lValLen As Long


        lReturn = WriteSubKeyValue(Registry.CurrentUser, sKey, "DXF_Filename", sValue)


        ' using RegCreateKey instead of RegOpenKey
        ' in case the key does not currently exist
        'lReturn = RegCreateKey(HKEY_CURRENT_USER, sKey, lHandle)

        If (lReturn = True) Then

            If (Trim(sValue) = "") Then
                sValue = ""
                lValLen = 0
            Else
                sValue = Trim(sValue)
                lValLen = CLng(Len(sValue))
            End If

            lReturn = RegPutString("SOFTWARE\\WE-CIM\\20.0\\Fabrication\\FileOpen\\WECAD", "DXF_Filename", sValue)


            RegClearWECADImage = "succeeded"

        Else

            RegClearWECADImage = "failed"

        End If


    End Function
    Public Function RegClearArborImage( _
                        ByVal sKey As String, _
                        ByVal sSubKey As String, _
                        ByVal sValue As String) As String

        Dim lValLen As Long

        '' using RegCreateKey instead of RegOpenKey
        '' in case the key does not currently exist
        'lReturn = RegCreateKey(HKEY_CURRENT_USER, sKey, lHandle)

        lReturn = WriteSubKeyValue(Registry.CurrentUser, sKey, "DXF_Filename", sValue)

        If (lReturn = True) Then

            If (Trim(sValue) = "") Then
                sValue = ""
                lValLen = 0
            Else
                sValue = Trim(sValue)
                lValLen = CLng(Len(sValue))
            End If

            lReturn = RegPutString("Software\Arbor Image Corporation\Cutting Shop\", "Last DXF Output", sValue)
            RegClearArborImage = "succeeded"

        Else

            RegClearArborImage = "failed"

        End If

    End Function
    Public Function GetSavedDateInReg() As String

        GetSavedDateInReg = RegGetString("Customizations", "SavedDate", "")

    End Function

    Public Function SetSaveDateInReg(sDefaulteValue As String) As String

        SetSaveDateInReg = RegPutString("Customizations", "SavedDate", sDefaulteValue)
    End Function
End Module
