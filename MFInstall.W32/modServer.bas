Attribute VB_Name = "modServer"
Option Explicit

Public myConnection As Long

'minimum functions ...
Private Declare Function NWCallsInit Lib "calwin32" (reserved1 As Byte, reserved2 As Byte) As Long
Private Declare Function NWCallsTerm Lib "calwin32" (reserved As Byte) As Long
Private Declare Function NWCLXInit Lib "clxwin32" (reserved1 As Byte, reserved2 As Byte) As Long
Private Declare Function NWCLXTerm Lib "clxwin32" (reserved As Byte) As Long
Private Declare Function NWSMExecuteNCFFile Lib "calwin32" (ByVal connHandle As Long, ByVal ncfFileName As String) As Long
Private Declare Function NWSMLoadNLM Lib "calwin32" (ByVal connHandle As Long, ByVal loadCommand As String) As Long
Private Declare Function NWSMUnloadNLM Lib "calwin32" (ByVal connHandle As Long, ByVal loadCommand As String) As Long
Private Declare Function NWCheckConsolePrivileges Lib "calwin32" (ByVal conn As Long) As Long
Private Declare Function NWGetConnectionHandle Lib "calwin32" (pServerName As Byte, ByVal reserved1 As Integer, pConnHandle As Long, reserved2 As Integer) As Long
Private Declare Function NWGetPrimaryConnectionID Lib "calwin32" (pConnHandle As Long) As Long
Private Declare Function NWGetFileServerInformation Lib "calwin32" (ByVal conn As Long, ByVal serverName As String, majorVer As Byte, minVer As Byte, rev As Byte, maxConns As Integer, maxConnsUsed As Integer, connsInUse As Integer, numVolumes As Integer, SFTLevel As Byte, TTSLevel As Byte) As Long
Private Declare Function strToBytes Lib "kernel32.dll" Alias "lstrcpyn" (x As Byte, ByVal Y As String, Z As Long) As Long

Private Declare Function WNetGetConnection32 Lib "MPR.DLL" Alias _
      "WNetGetConnectionA" (ByVal lpszLocalName As String, ByVal _
      lpszRemoteName As String, lSize As Long) As Long
      
Type NWCCVersion
    major As Long
    minor As Long
    revision As Long
End Type

Public Const NW_MAX_TREE_NAME_LEN = 33
Public Const NW_MAX_SERVER_NAME_LEN = 49
Public Const NWCC_INFO_SERVER_VERSION = 12

Type NWCCConnInfo
    authenticationState As Long
    broadcastState As Long
    connRef As Long
    treeName(NW_MAX_TREE_NAME_LEN - 1) As Byte
    connNum As Long
    userID As Long
    serverName(NW_MAX_SERVER_NAME_LEN - 1) As Byte
    NDSState As Long
    maxPacketSize As Long
    licenseState As Long
    distance As Long
    serverVersion As NWCCVersion
    tranAddr As Long
End Type

Declare Function NWGetRequesterVersion Lib "calwin32" (majorVer As Byte, minorVer As Byte, revision As Byte) As Long
Declare Sub NWCCGetCLXVersion Lib "clxwin32" (majorVersion As Byte, minorVersion As Byte, revisionLevel As Byte, betaReleaseLevel As Byte)
Declare Function NWCCGetConnInfo Lib "clxwin32" (ByVal connHandle As Long, ByVal infoType As Long, ByVal len1 As Long, buffer As Any) As Long
'Private Declare Function NWGetNetWareProductVersion Lib "calwin32" (connHandle As Long, version As NETWARE_PRODUCT_VERSION) As Long

Public Function Server_GetServerNameFromDrive(DriveLtr As String) As String
    
    Dim lpszRemoteName As String
    Dim DriveLetter As String
    Dim bufSize As Long
    Dim rc As Long
    Dim serverName As String
    DriveLetter = Left(DriveLtr, 1) & ":"
    
    ' Specifies the size in charaters of the buffer.
    bufSize = 255
    ' Prepare a string variable by padding spaces.
    lpszRemoteName = Space(bufSize)
    ' Return the UNC path (\\Server\Share).
    rc = WNetGetConnection32(DriveLetter, lpszRemoteName, bufSize)
    
    ' Check to see if WNetGetConnection() succeeded. WNetGetConnection()
    ' returns 0 (NO_ERROR) if it succesfully retrieves the UNC path.
    If rc = 0 Then
        serverName = TrimStr(Mid(lpszRemoteName, 3))
        serverName = Left(serverName, InStr(1, serverName, "\") - 1)
        Server_GetServerNameFromDrive = serverName
    End If
    
End Function

Public Function Server_GetUNCPath(DriveLtr As String) As String
    
    Dim lpszRemoteName As String
    Dim DriveLetter As String
    Dim bufSize As Long
    Dim rc As Long
    Dim serverName As String
    DriveLetter = Left(DriveLtr, 1) & ":"
    
    ' Specifies the size in charaters of the buffer.
    bufSize = 255
    ' Prepare a string variable by padding spaces.
    lpszRemoteName = Space(bufSize)
    ' Return the UNC path (\\Server\Share).
    rc = WNetGetConnection32(DriveLetter, lpszRemoteName, bufSize)
    
    ' Check to see if WNetGetConnection() succeeded. WNetGetConnection()
    ' returns 0 (NO_ERROR) if it succesfully retrieves the UNC path.
    If rc = 0 Then
        Server_GetUNCPath = TrimStr(Mid(lpszRemoteName, 3))
    End If
    
End Function

Public Function Server_Login(serverName As String)
    
    Dim r1 As Byte, r2 As Byte
    NWCallsInit r1, r2
    NWCLXInit r1, r2
    
    Dim myServer() As Byte
    Dim myServerStr As String
    Dim res1 As Long, res2 As Integer
        
    myServerStr = serverName & Chr(0)
    
    'check priv
    myConnection = 0
    ReDim myServer(0 To Len(myServerStr))
    strToBytes myServer(0), myServerStr, Len(myServerStr)
    If NWGetConnectionHandle(myServer(0), res1, myConnection, res2) <> 0 Then
        MsgBox "Failed to get an active connection to the Server. Please check if you are logged in to Server " & serverName & ".", vbCritical
    End If


End Function

Public Function Server_Logout()
    Dim res As Byte
    NWCallsTerm res
    NWCLXTerm res
End Function

Public Function Server_LoadNLM()
    Dim rc As Integer
    'rc = NWSMLoadNLM(myConnection, "MailFlt.nlm")
    rc = NWSMExecuteNCFFile(myConnection, "SYS:\SYSTEM\MFSTART.NCF")
End Function
Public Function Server_LoadNLMAny(NLMfile As String)
    Dim rc As Integer
    rc = NWSMLoadNLM(myConnection, NLMfile)
    If (rc) Then MsgBox "Loading of " & NLMfile & " failed. Error Code = 0x" & Hex(rc) & ".", vbCritical
End Function
Public Function Server_UnLoadNLM()
    
    ' unload mfnrm first...
    NWSMUnloadNLM myConnection, "MFNRM.NLM"
    
    NWSMUnloadNLM myConnection, "MFCONFIG.NLM"
    NWSMUnloadNLM myConnection, "MAILFLT.NLM"

End Function

