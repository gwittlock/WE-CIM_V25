Imports System.Runtime.InteropServices
Imports System
Imports System.Text

''' <summary>Class for calling the native 32 bit Instant PLUS DLL.</summary>
Friend Class IP2Lib32Managed

    ''' <summary>DEPRECATED - Use IP2LibManaged for flags instead</summary>    
    Friend Const FLAGS_NONE As Int32 = IP2LibManaged.FLAGS_NONE
    Friend Const FLAGS_USE_THREAD As Int32 = IP2LibManaged.FLAGS_USE_THREAD
    Friend Const FLAGS_SILENT_MODE As Int32 = IP2LibManaged.FLAGS_SILENT_MODE
    Friend Const FLAGS_USE_XML_PATH As Int32 = IP2LibManaged.FLAGS_USE_XML_PATH
    Friend Const RESULT_FILE_NOT_FOUND As Int32 = IP2LibManaged.RESULT_FILE_NOT_FOUND
    Friend Const RESULT_CANNOT_CREATE_THREAD As Int32 = IP2LibManaged.RESULT_CANNOT_CREATE_THREAD
    Friend Const RESULT_INCOMPATIBLE_FILE_VERSION As Int32 = IP2LibManaged.RESULT_INCOMPATIBLE_FILE_VERSION

    ''' <summary>
    ''' (DEPRECATED - Use IP2LibManaged instead) Calls the appropriate native Instant PLUS DLL.
    ''' </summary>
    ''' <param name="flags">Flags.</param>
    ''' <param name="key">Decryption Key.</param>
    ''' <param name="path">Path to XML config file.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function CallInstantPLUS(ByVal flags As Int32, ByVal key As String, ByVal path As String) As Int32
        Return IP2LibManaged.CallInstantPLUS(flags, key, path)
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall)> _
    Friend Shared Function CallIpEx(ByVal hWnd As IntPtr, ByVal flags As Int32, ByVal key As String, ByVal path As String, ByVal res1 As String, ByVal res2 As String, ByVal res3 As String, ByVal res4 As Int32, ByVal res5 As Int32, ByVal res6 As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n1")> _
    Friend Shared Function WR_LFGetString(ByVal flags As Int32, ByVal var_no As Int32, ByVal buffer As StringBuilder) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n2")> _
    Friend Shared Function WR_LFGetNum(ByVal flags As Int32, ByVal var_no As Int32, ByRef value As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n3")> _
    Friend Shared Function WR_LFGetDate(ByVal flags As Int32, ByVal var_no As Int32, ByRef month_hours As Int32, ByRef day_minutes As Int32, ByRef year_seconds As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n4")> _
    Friend Shared Function WR_LFSetString(ByVal flags As Int32, ByVal var_no As Int32, ByVal buffer As String) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n5")> _
    Friend Shared Function WR_LFSetNum(ByVal flags As Int32, ByVal var_no As Int32, ByVal value As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n6")> _
    Friend Shared Function WR_LFSetDate(ByVal flags As Int32, ByVal var_no As Int32, ByVal month_hours As Int32, ByVal day_minutes As Int32, ByVal year_seconds As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n7")> _
    Friend Shared Function WR_Activate(ByVal flags As Int32, ByVal action As Int32, ByVal action_flags As Int32, ByVal hWnd As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n8")> _
    Friend Shared Function WR_TrialRunsLeft(ByVal flags As Int32, ByRef runsleft As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n9")> _
    Friend Shared Function WR_TrialDaysLeft(ByVal flags As Int32, ByRef daysleft As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n10")> _
    Friend Shared Function WR_IsTrial(ByVal flags As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n17")> _
    Friend Shared Function WR_GetLFHandle(ByRef lRetVal As Int32) As IntPtr
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n18")> _
    Friend Shared Function WR_GetLicenseID(ByRef licenseid As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n19")> _
    Friend Shared Function WR_GetPassword(ByVal password As StringBuilder) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r14")> _
    Friend Shared Function WR_ExpireType(ByVal flags As Int32, ByVal str1 As StringBuilder) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n25")> _
    Friend Shared Function WR_DeactivateLicense(ByVal flags As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r4")> _
    Friend Shared Function WR_GetProductID(ByVal flags As Int32, ByRef number As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r5")> _
    Friend Shared Function WR_GetProdOptionID(ByVal flags As Int32, ByRef number As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r15")> _
    Friend Shared Function WR_GetPurchaseUrl(ByVal flags As Int32, ByVal str1 As StringBuilder) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r2")> _
    Friend Shared Function WR_IsJustActivated(ByVal flags As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r1")> _
    Friend Shared Function WR_Close(ByVal flags As Int32) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r3")> _
    Friend Shared Function WR_DeactivateInstallation(ByVal flags As Int32, ByVal hWnd As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n13")> _
    Friend Shared Function WR_SemOpen(ByVal flags As Int32, ByVal prefix_server As String, ByVal name As String, ByRef sem_handle As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n14")> _
    Friend Shared Function WR_SemClose(ByVal sem_handle As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n16")> _
    Friend Shared Function WR_SemTest(ByVal sem_handle As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib32.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r6")> _
    Friend Shared Function WR_GetInstantPLUSLastError(ByVal errorType As Int32, ByRef errorCode As Int32) As Int32
    End Function
End Class

''' <summary>Class for calling the native 64 bit Instant PLUS DLL</summary>
Friend Class IP2Lib64Managed

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall)> _
    Friend Shared Function CallIpEx(ByVal hWnd As IntPtr, ByVal flags As Int32, ByVal key As String, ByVal path As String, ByVal res1 As String, ByVal res2 As String, ByVal res3 As String, ByRef res4 As Int32, ByRef res5 As Int32, ByRef res6 As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n1")> _
    Friend Shared Function WR_LFGetString(ByVal flags As Int32, ByVal var_no As Int32, ByVal buffer As StringBuilder) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n2")> _
    Friend Shared Function WR_LFGetNum(ByVal flags As Int32, ByVal var_no As Int32, ByRef value As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n3")> _
    Friend Shared Function WR_LFGetDate(ByVal flags As Int32, ByVal var_no As Int32, ByRef month_hours As Int32, ByRef day_minutes As Int32, ByRef year_seconds As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n4")> _
    Friend Shared Function WR_LFSetString(ByVal flags As Int32, ByVal var_no As Int32, ByVal buffer As String) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n5")> _
    Friend Shared Function WR_LFSetNum(ByVal flags As Int32, ByVal var_no As Int32, ByVal value As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n6")> _
    Friend Shared Function WR_LFSetDate(ByVal flags As Int32, ByVal var_no As Int32, ByVal month_hours As Int32, ByVal day_minutes As Int32, ByVal year_seconds As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n7")> _
    Friend Shared Function WR_Activate(ByVal flags As Int32, ByVal action As Int32, ByVal action_flags As Int32, ByVal hWnd As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n8")> _
    Friend Shared Function WR_TrialRunsLeft(ByVal flags As Int32, ByRef runsleft As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n9")> _
    Friend Shared Function WR_TrialDaysLeft(ByVal flags As Int32, ByRef daysleft As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n10")> _
    Friend Shared Function WR_IsTrial(ByVal flags As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n17")> _
    Friend Shared Function WR_GetLFHandle(ByRef lRetVal As Int32) As IntPtr
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n18")> _
    Friend Shared Function WR_GetLicenseID(ByRef licenseid As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n19")> _
    Friend Shared Function WR_GetPassword(ByVal password As StringBuilder) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r14")> _
    Friend Shared Function WR_ExpireType(ByVal flags As Int32, ByVal str1 As StringBuilder) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n25")> _
    Friend Shared Function WR_DeactivateLicense(ByVal flags As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r4")> _
    Friend Shared Function WR_GetProductID(ByVal flags As Int32, ByRef number As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r5")> _
    Friend Shared Function WR_GetProdOptionID(ByVal flags As Int32, ByRef number As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r15")> _
    Friend Shared Function WR_GetPurchaseUrl(ByVal flags As Int32, ByVal str1 As StringBuilder) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r2")> _
    Friend Shared Function WR_IsJustActivated(ByVal flags As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r1")>
    Friend Shared Function WR_Close(ByVal flags As Int32) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r3")> _
    Friend Shared Function WR_DeactivateInstallation(ByVal flags As Int32, ByVal hWnd As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n13")> _
    Friend Shared Function WR_SemOpen(ByVal flags As Int32, ByVal prefix_server As String, ByVal name As String, ByRef sem_handle As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n14")> _
    Friend Shared Function WR_SemClose(ByVal sem_handle As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="n16")> _
    Friend Shared Function WR_SemTest(ByVal sem_handle As IntPtr) As Int32
    End Function

    <DllImport("IP2Lib64.dll", CallingConvention:=CallingConvention.StdCall, EntryPoint:="r6")> _
    Friend Shared Function WR_GetInstantPLUSLastError(ByVal errorType As Int32, ByRef errorCode As Int32) As Int32
    End Function
End Class

''' <summary>Class for calling the appropriate native Instant PLUS DLL.</summary>
Friend Class IP2LibManaged

    Friend Enum TargetArchitecture
        x86 = 0
        x64 = 1
    End Enum

    ''' <summary>
    ''' Constants for flags
    ''' </summary>    
    Friend Const FLAGS_NONE As Int32 = 0
    Friend Const FLAGS_USE_THREAD As Int32 = 1
    Friend Const FLAGS_SILENT_MODE As Int32 = 2
    Friend Const FLAGS_USE_XML_PATH As Int32 = 4
    Friend Const RESULT_FILE_NOT_FOUND As Int32 = 7
    Friend Const RESULT_CANNOT_CREATE_THREAD As Int32 = -99
    Friend Const RESULT_INCOMPATIBLE_FILE_VERSION As Int32 = -98
    Friend Const INSTANTPLUS_ERRORS_ACTIVATIONDEACTIVATION = 1

    ''' <summary>
    ''' Constants for exception messages
    ''' </summary>    
    Private Const EXCEPTION_STARTUP_TITLE As String = "Start-up Error"
    Private Const EXCEPTION_STARTUP_BADIMAGEFORMAT_MESSAGE As String = "The application encountered an error during start-up.  This error is most likely occurring because the application has not been configured to target only x86 architecture.  Please contact technical support for assistance.  The error description is as follows:\n\n"
    Private Const EXCEPTION_STARTUP_DLLNOTFOUNDEXCEPTION As String = "The application encountered an error during start-up.  This error is most likely occurring because a file this program requires to run correctly is not present.  Please contact technical support for assistance.  The error description is as follows:\n\n"
    Private Const EXCEPTION_STARTUP_OTHER As String = "The application encountered an error during start-up.  Please contact technical support for assistance.  The error description is as follows:\n\n"
    Private Const EXCEPTION_STARTUP_RESULT_FILE_NOT_FOUND = "The application encountered an error during start-up because a required file is missing.  Please contact technical support for further assistance."
    Private Const EXCEPTION_STARTUP_INCOMPATIBLE_FILE_VERSION = "Incompatible XML file version"
    Private Const EXCEPTION_RUNTIME_TITLE As String = "Error"
    Private Const EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE As String = "The application encountered an error.  This error is most likely occurring because the application has not been configured to target only x86 architecture.  Please contact technical support for assistance.  The error description is as follows:\n\n"
    Private Const EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION As String = "The application encountered an error.  This error is most likely occurring because a file this program requires to run correctly is not present.  Please contact technical support for assistance.  The error description is as follows:\n\n"
    Private Const EXCEPTION_RUNTIME_OTHER As String = "The application encountered an error.  Please contact technical support for assistance.  The error description is as follows:\n\n"

    ''' <summary>
    ''' Calls the appropriate native Instant PLUS DLL. Use CallInstantPLUSEx() for additional arguments.
    ''' </summary>
    ''' <param name="flags">Flags.</param>
    ''' <param name="key">Decryption Key.</param>
    ''' <param name="path">Path to XML config file.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function CallInstantPLUS(ByVal flags As Int32, ByVal key As String, ByVal path As String) As Int32

        Return CallInstantPLUSEx(IntPtr.Zero, flags, key, path, "", "", "", 0, 0, 0)

    End Function

    ''' <summary>
    ''' Calls the appropriate native Instant PLUS DLL.
    ''' </summary>
    ''' <param name="hWnd">A handle to the application's window.</param>
    ''' <param name="flags">Flags.</param>
    ''' <param name="key">Decryption Key.</param>
    ''' <param name="path">Path to XML config file.</param>
    ''' <param name="res1">Reserved.</param>
    ''' <param name="res2">Reserved.</param>
    ''' <param name="res3">Reserved.</param>
    ''' <param name="res4">Reserved.</param>
    ''' <param name="res5">Reserved.</param>
    ''' <param name="res6">Reserved.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function CallInstantPLUSEx(ByVal hWnd As IntPtr, ByVal flags As Int32, ByVal key As String, ByVal path As String, ByVal res1 As String, ByVal res2 As String, ByVal res3 As String, ByVal res4 As Int32, ByVal res5 As Int32, ByVal res6 As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.CallIpEx(hWnd, flags, key, path, res1, res2, res3, res4, res5, res6)
                Case Else
                    result = IP2Lib32Managed.CallIpEx(hWnd, flags, key, path, res1, res2, res3, res4, res5, res6)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_STARTUP_BADIMAGEFORMAT_MESSAGE.Replace("\n", System.Environment.NewLine) + bEx.Message, EXCEPTION_STARTUP_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_STARTUP_DLLNOTFOUNDEXCEPTION.Replace("\n", System.Environment.NewLine) + dEx.Message, EXCEPTION_STARTUP_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_STARTUP_OTHER.Replace("\n", System.Environment.NewLine) + ex.Message, EXCEPTION_STARTUP_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        If RESULT_FILE_NOT_FOUND = result Then
            MessageBox.Show(EXCEPTION_STARTUP_RESULT_FILE_NOT_FOUND.Replace("\n", System.Environment.NewLine), EXCEPTION_STARTUP_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        ElseIf RESULT_INCOMPATIBLE_FILE_VERSION = result Then
            MessageBox.Show(EXCEPTION_STARTUP_INCOMPATIBLE_FILE_VERSION.Replace("\n", System.Environment.NewLine), EXCEPTION_STARTUP_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End If

        Return result

    End Function

    ''' <summary>
    ''' Determines if the current assembly is running under a 32 bit process or a 64 bit process.
    ''' </summary>
    ''' <returns>TargetArchitecture.</returns>
    Friend Shared Function GetTargetArchitecture() As TargetArchitecture

        Dim arch As TargetArchitecture = TargetArchitecture.x86

        Select Case IntPtr.Size
            Case 8
                arch = TargetArchitecture.x64
            Case 4
                arch = TargetArchitecture.x86
            Case Else
                arch = TargetArchitecture.x86
        End Select

        Return arch

    End Function

    ''' <summary>
    ''' Use this function to obtain the string values of the variables stored in the License File.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="var_no">Determines which variable in the license file is being retrieved.</param>
    ''' <param name="value">A string which will contain the string value from the license file when this method is successful.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_LFGetString(ByVal flags As Int32, ByVal var_no As Int32, ByRef value As String) As Int32

        Dim str1 As New StringBuilder(512)
        Dim result As Int32 = 0

        result = WR_LFGetString(flags, var_no, str1)
        If (result = 1) Then
            value = str1.ToString()
        Else
            value = String.Empty
        End If
        Return result
    End Function


    ''' <summary>
    ''' Use this function to obtain the string values of the variables stored in the License File.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="var_no">Determines which variable in the license file is being retrieved.</param>
    ''' <param name="buffer">The location in which to place the variable data.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_LFGetString(ByVal flags As Int32, ByVal var_no As Int32, ByVal buffer As StringBuilder) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_LFGetString(flags, var_no, buffer)
                Case Else
                    result = IP2Lib32Managed.WR_LFGetString(flags, var_no, buffer)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", System.Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", System.Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", System.Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Use this function to obtain the values of the numeric variables stored in the License File.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="var_no">Determines which variable in the license file is being retrieved.</param>
    ''' <param name="value">The location in which to place the variable data.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_LFGetNum(ByVal flags As Int32, ByVal var_no As Int32, ByRef value As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_LFGetNum(flags, var_no, value)
                Case Else
                    result = IP2Lib32Managed.WR_LFGetNum(flags, var_no, value)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", System.Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", System.Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", System.Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Use this function to obtain the values of the date and time variables stored in the License File.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="var_no">Determines which variable is being retrieved.</param>
    ''' <param name="month_hours">Returns the month or hours of the variable.</param>
    ''' <param name="day_minutes">Returns the day or minutes of the variable.</param>
    ''' <param name="year_seconds">Returns the year or seconds of the variable.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_LFGetDate(ByVal flags As Int32, ByVal var_no As Int32, ByRef month_hours As Int32, ByRef day_minutes As Int32, ByRef year_seconds As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_LFGetDate(flags, var_no, month_hours, day_minutes, year_seconds)
                Case Else
                    result = IP2Lib32Managed.WR_LFGetDate(flags, var_no, month_hours, day_minutes, year_seconds)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' This function is used to set the string values of the variables stored in the License File.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="var_no">Determines which variable in the license file is being changed.</param>
    ''' <param name="buffer">The value to which to set the License File variable.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_LFSetString(ByVal flags As Int32, ByVal var_no As Int32, ByVal buffer As String) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_LFSetString(flags, var_no, buffer)
                Case Else
                    result = IP2Lib32Managed.WR_LFSetString(flags, var_no, buffer)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Use this function to set the values of the numeric variables in the License File.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="var_no">Determines which variable in the license file is being set.</param>
    ''' <param name="value">The value to write into the license file.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_LFSetNum(ByVal flags As Int32, ByVal var_no As Int32, ByVal value As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_LFSetNum(flags, var_no, value)
                Case Else
                    result = IP2Lib32Managed.WR_LFSetNum(flags, var_no, value)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' This function is used to change the date and time values of the variables stored in the License File.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="var_no">Determines which variable is being retrieved.</param>
    ''' <param name="month_hours">Sets the month or hours of the variable in the license file.</param>
    ''' <param name="day_minutes">Sets the day or minutes of the variable in the license file.</param>
    ''' <param name="year_seconds">Sets the year or seconds of the variable in the license file.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_LFSetDate(ByVal flags As Int32, ByVal var_no As Int32, ByVal month_hours As Int32, ByVal day_minutes As Int32, ByVal year_seconds As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_LFSetDate(flags, var_no, month_hours, day_minutes, year_seconds)
                Case Else
                    result = IP2Lib32Managed.WR_LFSetDate(flags, var_no, month_hours, day_minutes, year_seconds)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' This function is used to call into the wrapper from the application in order to perform an activation.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="action">Reserved - Always set to 1.</param>
    ''' <param name="action_flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="hWnd">A handle to the application's window.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_Activate(ByVal flags As Int32, ByVal action As Int32, ByVal action_flags As Int32, ByVal hWnd As IntPtr) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_Activate(flags, action, action_flags, hWnd)
                Case Else
                    result = IP2Lib32Managed.WR_Activate(flags, action, action_flags, hWnd)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' This function retrieves the number of executions remaining in a trial.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="runsleft">Variable to place the number of executions remaining in the trial.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_TrialRunsLeft(ByVal flags As Int32, ByRef runsleft As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_TrialRunsLeft(flags, runsleft)
                Case Else
                    result = IP2Lib32Managed.WR_TrialRunsLeft(flags, runsleft)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' This function retrieves the number of days remaining in a trial.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="daysleft">Variable to place the number of days remaining in the trial.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_TrialDaysLeft(ByVal flags As Int32, ByRef daysleft As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_TrialDaysLeft(flags, daysleft)
                Case Else
                    result = IP2Lib32Managed.WR_TrialDaysLeft(flags, daysleft)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' This function returns the trial status of an application.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_IsTrial(ByVal flags As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_IsTrial(flags)
                Case Else
                    result = IP2Lib32Managed.WR_IsTrial(flags)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Use this function to retrieve the handle to the license file.  This handle may then be used with any of the Protection PLUS API functions.
    ''' </summary>
    ''' <param name="lRetVal">The output parameter for result.</param>
    ''' <returns>IntPtr.</returns>
    Friend Shared Function WR_GetLFHandle(ByRef lRetVal As Int32) As IntPtr

        Dim lfhandle As IntPtr = IntPtr.Zero

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    lfhandle = IP2Lib64Managed.WR_GetLFHandle(lRetVal)
                Case Else
                    lfhandle = IP2Lib32Managed.WR_GetLFHandle(lRetVal)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return lfhandle

    End Function

    ''' <summary>
    ''' Retrieves the License ID from the license file.  If the application has yet to be activated the License ID will be set to 0.
    ''' </summary>
    ''' <param name="licenseid">Variable to place the License ID.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_GetLicenseID(ByRef licenseid As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_GetLicenseID(licenseid)
                Case Else
                    result = IP2Lib32Managed.WR_GetLicenseID(licenseid)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Retrieves the activation password from the license file.  If the application has yet to be activated the password will be blank.
    ''' </summary>
    ''' <param name="value">A string which will contain the string value from the license file when this method is successful.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_GetPassword(ByRef value As String) As Int32
        Dim str1 As New StringBuilder(512)
        Dim result As Int32
        result = WR_GetPassword(str1)
        If (result = 1) Then
            value = str1.ToString()
        Else
            value = String.Empty
        End If
        Return result
    End Function


    ''' <summary>
    ''' Retrieves the activation password from the license file.  If the application has yet to be activated the password will be blank.
    ''' </summary>
    ''' <param name="password">Variable to place the activation password.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_GetPassword(ByVal password As StringBuilder) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_GetPassword(password)
                Case Else
                    result = IP2Lib32Managed.WR_GetPassword(password)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Use this function to retrieve the expiration type from the license file.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="value">A string which will contain the string value from the license file when this method is successful.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_ExpireType(ByVal flags As Int32, ByRef value As String) As Int32
        Dim str1 As New StringBuilder(512)
        Dim result As Int32 = 0

        result = WR_ExpireType(flags, str1)
        If (result = 1) Then
            value = str1.ToString()
        Else
            value = String.Empty
        End If
        Return result
    End Function


    ''' <summary>
    ''' Use this function to retrieve the expiration type from the license file.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="str1">Variable to place the expire type of the application.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_ExpireType(ByVal flags As Int32, ByVal str1 As StringBuilder) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_ExpireType(flags, str1)
                Case Else
                    result = IP2Lib32Managed.WR_ExpireType(flags, str1)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' This function will reset a license to a deactivated status.  Upon next execution, the user will need to reactive the application.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_DeactivateLicense(ByVal flags As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_DeactivateLicense(flags)
                Case Else
                    result = IP2Lib32Managed.WR_DeactivateLicense(flags)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Use this function to retrieve the SOLO Product ID from the wrapper.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="number">Variable to place the SOLO Product ID.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_GetProductID(ByVal flags As Int32, ByRef number As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_GetProductID(flags, number)
                Case Else
                    result = IP2Lib32Managed.WR_GetProductID(flags, number)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Use this function to retrieve the SOLO Product Option ID from the wrapper.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="number">Variable to place the SOLO Product Option ID.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_GetProdOptionID(ByVal flags As Int32, ByRef number As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_GetProdOptionID(flags, number)
                Case Else
                    result = IP2Lib32Managed.WR_GetProdOptionID(flags, number)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function
    ''' <summary>
    ''' Retrieves the url used to purchase the application.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="value">A string which will contain the string value from the license file when this method is successful.</param>
    ''' <returns>Int32.</returns>

    Friend Shared Function WR_GetPurchaseUrl(ByVal flags As Int32, ByRef value As String) As Int32
        Dim str1 As New StringBuilder
        Dim result As Int32 = 0

        result = WR_GetPurchaseUrl(flags, str1)
        If (result = 1) Then
            value = str1.ToString()
        Else
            value = String.Empty
        End If
        Return result
    End Function

    ''' <summary>
    ''' Retrieves the url used to purchase the application.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <param name="str1">Variable to place the url used for purchasing.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_GetPurchaseUrl(ByVal flags As Int32, ByVal str1 As StringBuilder) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_GetPurchaseUrl(flags, str1)
                Case Else
                    result = IP2Lib32Managed.WR_GetPurchaseUrl(flags, str1)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Use this function to determine if the application was just activated during the current execution.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_IsJustActivated(ByVal flags As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_IsJustActivated(flags)
                Case Else
                    result = IP2Lib32Managed.WR_IsJustActivated(flags)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Shuts down the Instant PLUS API.
    ''' </summary>
    ''' <param name="flags">Reserved - Always use zero or FLAGS_NONE.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_Close(ByVal flags As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_Close(flags)
                Case Else
                    result = IP2Lib32Managed.WR_Close(flags)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Deactivates installation using SOLO Server's XML Activation Service.
    ''' </summary>
    ''' <param name="flags">Flags.</param>
    ''' <param name="hWnd">A handle to the application's window.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_DeactivateInstallation(ByVal flags As Int32, ByVal hWnd As IntPtr) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_DeactivateInstallation(flags, hWnd)
                Case Else
                    result = IP2Lib32Managed.WR_DeactivateInstallation(flags, hWnd)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Opens the Network Semaphore File in exclusive mode.
    ''' </summary>
    ''' <param name="flags">Flags.</param>
    ''' <param name="prefix_server">The complete path, including a trailing backslash, to the Network Semaphore Files of the server counting the number of simultaneous users.</param>
    ''' <param name="name">A four-character identifier to use to count the workstations.</param>
    ''' <param name="sem_handle">The pointer in which to place the Network Semaphore File handle.</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_SemOpen(ByVal flags As Int32, ByVal prefix_server As String, ByVal name As String, ByRef sem_handle As IntPtr) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_SemOpen(flags, prefix_server, name, sem_handle)
                Case Else
                    result = IP2Lib32Managed.WR_SemOpen(flags, prefix_server, name, sem_handle)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Closes the Network Semaphore File.
    ''' </summary>
    ''' <param name="sem_handle">The handle to the semaphore file given by WR_SemOpen().</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_SemClose(ByVal sem_handle As IntPtr) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_SemClose(sem_handle)
                Case Else
                    result = IP2Lib32Managed.WR_SemClose(sem_handle)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Verifies the Network Semaphore File is still open.
    ''' </summary>
    ''' <param name="sem_handle">The handle to the semaphore file given by WR_SemOpen().</param>
    ''' <returns>Int32.</returns>
    Friend Shared Function WR_SemTest(ByVal sem_handle As IntPtr) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_SemTest(sem_handle)
                Case Else
                    result = IP2Lib32Managed.WR_SemTest(sem_handle)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function

    ''' <summary>
    ''' Gets the last error encountered by the Instant PLUS runtime.
    ''' </summary>
    ''' <param name="errorType">The type of error to retreive.  Presently, only a value of 1 (INSTANTPLUS_ERRORS_ACTIVATIONDEACTIVATION) is supported.</param>
    ''' <param name="errorCode">The last error code encountered by the Instant PLUS runtime.</param>
    ''' <returns>Returns a 1 if the errorCode was retrieved successfully.  0 indicates errorCode was not initialized before calling this method.  -3 inidicates an unsupported errorType value.</returns>
    Friend Shared Function WR_GetInstantPLUSLastError(ByVal errorType As Int32, ByRef errorCode As Int32) As Int32

        Dim result As Int32 = 0

        Try
            Select Case GetTargetArchitecture()
                Case TargetArchitecture.x64
                    result = IP2Lib64Managed.WR_GetInstantPLUSLastError(errorType, errorCode)
                Case Else
                    result = IP2Lib32Managed.WR_GetInstantPLUSLastError(errorType, errorCode)
            End Select
        Catch bEx As BadImageFormatException
            MessageBox.Show(EXCEPTION_RUNTIME_BADIMAGEFORMAT_MESSAGE.Replace("\n", Environment.NewLine) + bEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch dEx As DllNotFoundException
            MessageBox.Show(EXCEPTION_RUNTIME_DLLNOTFOUNDEXCEPTION.Replace("\n", Environment.NewLine) + dEx.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception
            MessageBox.Show(EXCEPTION_RUNTIME_OTHER.Replace("\n", Environment.NewLine) + ex.Message, EXCEPTION_RUNTIME_TITLE, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try

        Return result

    End Function
End Class
