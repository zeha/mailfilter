VERSION 5.00
Begin VB.Form frmMain 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "MailFilter/ax Upgrade"
   ClientHeight    =   5940
   ClientLeft      =   45
   ClientTop       =   615
   ClientWidth     =   7995
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H00000000&
   Icon            =   "frmMain.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5940
   ScaleWidth      =   7995
   StartUpPosition =   2  'CenterScreen
   Begin VB.PictureBox pctUpdateLibCNLM 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   3090
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   16
      Top             =   2880
      Width           =   330
   End
   Begin VB.PictureBox pctUnloadNLM 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   3090
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   12
      Top             =   2040
      Width           =   330
   End
   Begin VB.PictureBox pctLoadNLM 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   3090
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   7
      Top             =   3300
      Width           =   330
   End
   Begin VB.PictureBox pct_SelectYes 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   7320
      Picture         =   "frmMain.frx":5112
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   6
      Top             =   2550
      Visible         =   0   'False
      Width           =   330
   End
   Begin VB.PictureBox pct_SelectNo 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   7320
      Picture         =   "frmMain.frx":5284
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   5
      Top             =   2910
      Visible         =   0   'False
      Width           =   330
   End
   Begin VB.PictureBox pctUpdateNLM 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   3090
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   4
      Top             =   2460
      Width           =   330
   End
   Begin VB.CommandButton cmdNext 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   315
      Left            =   6180
      TabIndex        =   3
      Top             =   5460
      Width           =   1095
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "E&xit"
      Height          =   315
      Left            =   900
      TabIndex        =   2
      Top             =   5460
      Width           =   1095
   End
   Begin VB.CommandButton cmdChangeServer 
      Caption         =   "&Change Server ..."
      Height          =   315
      Left            =   2040
      TabIndex        =   1
      Top             =   5460
      Width           =   1455
   End
   Begin VB.PictureBox pctWizard 
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      BorderStyle     =   0  'None
      Height          =   5250
      Left            =   0
      Picture         =   "frmMain.frx":53F6
      ScaleHeight     =   5250
      ScaleWidth      =   1950
      TabIndex        =   0
      Top             =   0
      Width           =   1950
   End
   Begin VB.Label lblUpdateLibCNLM 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Update MailFilter Components [LibC]"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   3510
      TabIndex        =   17
      Top             =   2940
      Width           =   3735
   End
   Begin VB.Label lblUpgrade 
      BackColor       =   &H00FFFFFF&
      BackStyle       =   0  'Transparent
      Caption         =   "Upgrade MailFilter/ax"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   14.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   525
      Left            =   2460
      TabIndex        =   15
      Top             =   240
      Width           =   3555
   End
   Begin VB.Label lblUnloadNLM 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Unload MailFilter Components"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   3510
      TabIndex        =   13
      Top             =   2100
      Width           =   3735
   End
   Begin VB.Label lblUpdateWarning 
      BackColor       =   &H00FFFFFF&
      Caption         =   $"frmMain.frx":5E65
      ForeColor       =   &H000000FF&
      Height          =   855
      Left            =   2490
      TabIndex        =   11
      Top             =   4020
      Width           =   5025
   End
   Begin VB.Label lblIntroduction 
      BackColor       =   &H00FFFFFF&
      Caption         =   "MailFilter/ax Upgrade has found a valid Installation of MailFilter/ax. You can update the MailFilter/ax NLM Components from here:"
      ForeColor       =   &H00000000&
      Height          =   735
      Left            =   2490
      TabIndex        =   10
      Top             =   990
      Width           =   5055
   End
   Begin VB.Label lblLoadNLM 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Load MailFilter Server/NLM"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   3510
      TabIndex        =   9
      Top             =   3360
      Width           =   3735
   End
   Begin VB.Label lblUpdateNLM 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Update MailFilter Components [Legacy]"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   3510
      TabIndex        =   8
      Top             =   2520
      Width           =   3735
   End
   Begin VB.Line Line1 
      X1              =   0
      X2              =   8400
      Y1              =   5280
      Y2              =   5280
   End
   Begin VB.Label lblServerName 
      Alignment       =   1  'Right Justify
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   315
      Left            =   5700
      TabIndex        =   14
      Top             =   90
      Width           =   1845
   End
   Begin VB.Menu mnuMailFilter 
      Caption         =   "Mail&Filter/ax"
      Begin VB.Menu mnuMailFilter_QuickStart 
         Caption         =   "&Quick Start Wizard ..."
      End
      Begin VB.Menu mnuMailFilter_Sep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuMailFilter_ChangeServer 
         Caption         =   "&Change Server ..."
      End
      Begin VB.Menu mnuMailFilter_RemoveServerComponents 
         Caption         =   "&Remove Server Components ..."
         Enabled         =   0   'False
         Visible         =   0   'False
      End
      Begin VB.Menu mnuMailFilter_Sep3 
         Caption         =   "-"
      End
      Begin VB.Menu mnuMailFilter_Exit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&?"
      Begin VB.Menu mnuHelp_AdminGuideDE 
         Caption         =   "MailFilter/ax Administrators Guide - Deutsch"
      End
      Begin VB.Menu mnuHelp_AdminGuideEN 
         Caption         =   "MailFilter/ax Administrators Guide - English"
      End
      Begin VB.Menu mnuHelp_Sep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuHelp_About 
         Caption         =   "About ..."
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private MF_FilesMissing As String
Private s_UnloadNLM As Boolean
Private s_UpdateNLM As Boolean
Private s_UpdateLibCNLM As Boolean
Private s_LoadNLM As Boolean


Private Sub cmdChangeServer_Click()
    Me.Hide
    frmChangeMF.Show vbModal
    Me.Show
End Sub

Private Sub pctLoadNLM_Click()
    s_LoadNLM = Not s_LoadNLM
    If s_LoadNLM Then
            pctLoadNLM.Picture = pct_SelectYes.Picture
        Else
            pctLoadNLM.Picture = pct_SelectNo.Picture
    End If
End Sub

Private Sub pctUpdateLibCNLM_Click()
    s_UpdateLibCNLM = Not s_UpdateLibCNLM
    If (s_UpdateLibCNLM) Then s_UpdateNLM = False
    If s_UpdateNLM Then
            pctUpdateNLM.Picture = pct_SelectYes.Picture
        Else
            pctUpdateNLM.Picture = pct_SelectNo.Picture
    End If
    If s_UpdateLibCNLM Then
            pctUpdateLibCNLM.Picture = pct_SelectYes.Picture
        Else
            pctUpdateLibCNLM.Picture = pct_SelectNo.Picture
    End If
End Sub

Private Sub pctUpdateNLM_Click()
    s_UpdateNLM = Not s_UpdateNLM
    If (s_UpdateNLM) Then s_UpdateLibCNLM = False
    If s_UpdateNLM Then
            pctUpdateNLM.Picture = pct_SelectYes.Picture
        Else
            pctUpdateNLM.Picture = pct_SelectNo.Picture
    End If
    If s_UpdateLibCNLM Then
            pctUpdateLibCNLM.Picture = pct_SelectYes.Picture
        Else
            pctUpdateLibCNLM.Picture = pct_SelectNo.Picture
    End If
End Sub

Private Sub pctUnloadNLM_Click()
    s_UnloadNLM = Not s_UnloadNLM
    If s_UnloadNLM Then
            pctUnloadNLM.Picture = pct_SelectYes.Picture
        Else
            pctUnloadNLM.Picture = pct_SelectNo.Picture
    End If
End Sub

Private Sub cmdExit_Click()
    End
End Sub

Private Sub cmdNext_Click()
    On Local Error Resume Next
    
    Me.Enabled = False
    
    Err.Clear
    
    Dim logNum As Integer: logNum = FreeFile
    Open ValidatePath(App.Path) & "mfinst.log" For Append As #logNum

    Print #logNum, " --- MF Update Win32 Starting: " & Now
    Print #logNum, "  -- App.Path: " & ValidatePath(App.Path)
    Print #logNum, "  -- Version: " & App.major & "." & App.minor & "." & App.revision
    
    Dim osVer As OSVERSIONINFO
    osVer.dwOSVersionInfoSize = Len(osVer)
    
    If GetVersionEx(osVer) Then
        Print #logNum, "  -- Windows Version: " & osVer.dwMajorVersion & "." & osVer.dwMinorVersion & " Build: " & osVer.dwBuildNumber
        Print #logNum, "  -- Windows Platform: " & osVer.dwPlatformId & ", " & osVer.szCSDVersion
    End If
    
    Print #logNum, "  -- InstallDir: " & MFC_MF_InstallDir
    Print #logNum, "  -- InstallDrive: " & Left(MFC_MF_InstallDir, 1)
    Print #logNum, "  -- InstallServer: " & Server_GetServerNameFromDrive(Left(MFC_MF_InstallDir, 1))
    
    Print #logNum, "  -- Connecting to Server..."
    Server_Logout
    Print #logNum, "  -- Getting Server Connection..."
    Server_Login Server_GetServerNameFromDrive(MFC_MF_InstallDir)
    
    Dim serverVersion As NWCCVersion
    NWCCGetConnInfo modServer.myConnection, NWCC_INFO_SERVER_VERSION, Len(serverVersion), serverVersion
    Print #logNum, "  -- Server Version: " & serverVersion.major & "." & serverVersion.minor & "." & serverVersion.revision
    
    Dim reqVersionMajor As Byte, reqVersionMinor As Byte, reqVersionRev As Byte
    NWGetRequesterVersion reqVersionMajor, reqVersionMinor, reqVersionRev
    Print #logNum, "  -- Requester Version: " & reqVersionMajor & "." & reqVersionMinor & "." & reqVersionRev
    
    Dim clxVersionMajor As Byte, clxVersionMinor As Byte, clxVersionRev As Byte, clxVersionBeta As Byte
    NWCCGetCLXVersion clxVersionMajor, clxVersionMinor, clxVersionRev, clxVersionBeta
    Print #logNum, "  -- CLX Version: " & clxVersionMajor & "." & clxVersionMinor & "." & clxVersionRev & " (Beta " & clxVersionBeta & ")"
    
    
    If s_UnloadNLM Or s_LoadNLM Then
        Dim serverName As String
'        Server_Logout
        serverName = Server_GetServerNameFromDrive(MFC_MF_InstallDir)
'        Server_Login serverName
    End If
    
    
    If s_UnloadNLM Then
        
        lblUnloadNLM.FontBold = True
        DoEvents
        
        Print #logNum, "  -- Unload Requested!"
        Server_UnLoadNLM
        
        lblUnloadNLM.FontBold = False
        DoEvents
        If Err.Number <> 0 Then
            pctUnloadNLM.Picture = pct_SelectNo.Picture
            s_UnloadNLM = False
            Print #logNum, "   * ERROR: " & Err.Number & " - " & Err.Description
            Err.Clear
        End If
        Print #logNum, "   - Done."
    End If
    
''    If s_CreateDirectories Then
''        lblCreateDirectories.FontItalic = False
''        lblCreateDirectories.FontBold = True
''        DoEvents
''
''        MkDir ValidatePath(MkUNCPath(frmMain.txtGWIARoot.Text))
''        MkDir ValidatePath(MkUNCPath(frmMain.txtGWIASmtpHome.Text))
''
''        lblCreateDirectories.FontBold = False
''        lblCreateDirectories.FontItalic = True
''        DoEvents
''        If Err.Number <> 0 Then
''            pctCreateDirectories.Picture = pct_SelectNo.Picture
''            Err.Clear
''        End If
''    End If
''

    Print #logNum, "  -- Copying MFUPGR.NLM:"
    If MF_InstallFile(logNum, "MFUPGR.NLM", "SYSTEM", True) = False Then
        Err.Raise vbObjectError + 4002
    End If
    Server_LoadNLMAny "MFUPGR.NLM"

    If s_UpdateNLM Then
        lblUpdateNLM.FontBold = True
        DoEvents
    End If
    If s_UpdateLibCNLM Then
        lblUpdateLibCNLM.FontBold = True
        DoEvents
    End If
    
    If s_UpdateNLM Or s_UpdateLibCNLM Then
        'backup mailflt.nlm
        Print #logNum, "  -- Removing MailFlt.BAK: " & ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.BAK"
        Kill ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.BAK":    Err.Clear
        
        Print #logNum, "  -- Backing up MailFlt.NLM: " & ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.NLM"
        FileCopy ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.NLM", ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.BAK"
        
        'kill mfzap if it is found ... just to be sure ;)
        Kill ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MFZap.NLM":      Err.Clear
        
        Print #logNum, "  -- Copying New Files:"
        
        ' ************************** Install NLMs ...
        ' ** MailFilter/ax professional
        If s_UpdateNLM Then
            Print #logNum, "   - MailFlt.NLM [Legacy]:"
            If MF_InstallFile(logNum, "MailFlt.NLM", "SYSTEM", True) = False Then
                Err.Raise vbObjectError + 4001
            End If
        End If
        If s_UpdateLibCNLM Then
            Print #logNum, "   - MailFlt.NLM [LibC]:"
            If MF_InstallFile(logNum, "MailFlt.NLM", "SYSTEM", True, "LIBC") = False Then
                Err.Raise vbObjectError + 4001
            End If
        End If
        ' ** MailFilter/ax Configuration
        Print #logNum, "   - MFUpgr.NLM:"
        If MF_InstallFile(logNum, "MFUpgr.NLM", "SYSTEM", True) = False Then
            Err.Raise vbObjectError + 4002
        End If
        Print #logNum, "   - MFCONFIG.NLM:"
        If MF_InstallFile(logNum, "MFCONFIG.NLM", "SYSTEM", True) = False Then
            Err.Raise vbObjectError + 4003
        End If
        ' ** MailFilter/ax Restore
        Print #logNum, "   - MFRest.NLM:"
        If MF_InstallFile(logNum, "MFRest.NLM", "SYSTEM", True) = False Then
            Err.Raise vbObjectError + 4004
        End If
        ' ** MailFilter/NRM
        Print #logNum, "   - MFNRM.NLM:"
        If MF_InstallFile(logNum, "MFNRM.NLM", "SYSTEM", True) = False Then
            Err.Raise vbObjectError + 4005
        End If
        ' ** Load Scripts
        Print #logNum, "   - MFSTART.NCF"
        MF_InstallFile logNum, "MFSTART.NCF", "SYSTEM", False
        Print #logNum, "   - MFSTOP.NCF"
        MF_InstallFile logNum, "MFSTOP.NCF", "SYSTEM", False
        ' ** Defaults
        Print #logNum, "   - FILTERS.BIN"
        MF_InstallFile logNum, "FILTERS.BIN", "ETC\MAILFLT\DEFAULTS", True, "ETC"
        ' ** Default Templates
        Print #logNum, "   - REPORT.TPL"
        MF_InstallFile logNum, "REPORT.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC"
        Print #logNum, "   - RINSIDE.TPL"
        MF_InstallFile logNum, "RINSIDE.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC"
        Print #logNum, "   - ROUTRCPT.TPL"
        MF_InstallFile logNum, "ROUTRCPT.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC"
        Print #logNum, "   - ROUTSNDR.TPL"
        MF_InstallFile logNum, "ROUTSNDR.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC"
        Print #logNum, "   - MAILCOPY.TPL"
        MF_InstallFile logNum, "MAILCOPY.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC"
        ' **************************
        Print #logNum, "   - Done."
    
        lblUpdateNLM.FontBold = False
        lblUpdateLibCNLM.FontBold = False
        DoEvents
        If Err.Number <> 0 Then
            pctUpdateNLM.Picture = pct_SelectNo.Picture
            s_UnloadNLM = False
            Print #logNum, "   * ERROR: " & Err.Number & " - " & Err.Description
            Err.Clear
        End If
    End If
    
    If s_LoadNLM Then
        Print #logNum, "  -- Load Requested!"
        lblLoadNLM.FontBold = True
        DoEvents
        
        Print #logNum, "   - Executing MFSTART..."
        Server_LoadNLM
    
        lblLoadNLM.FontBold = False
        DoEvents
        If Err.Number <> 0 Then
            pctLoadNLM.Picture = pct_SelectNo.Picture
            s_LoadNLM = False
            Print #logNum, "   * ERROR: " & Err.Number & " - " & Err.Description
            Err.Clear
        End If
        Print #logNum, "   - Done."
    End If
    
    Print #logNum, " --- MF Update Win32 Complete: " & Now
    Print #logNum, ""
    Close #logNum
    MsgBox "Selected Actions have been performed. Check the Server's System Console.", vbInformation, "MailFilter/ax professional"
    
    Me.Enabled = True
    
End Sub

Private Function MF_CheckFileExists(logNum As Integer, fileName As String, localDir As String)
    If (Dir(ValidatePath(App.Path) & "NLM\" & localDir & "\" & fileName, vbNormal) = "") Then
        MF_FilesMissing = MF_FilesMissing & localDir & "\" & fileName & vbCrLf
        Print #logNum, "   * Missing File: " & localDir & "\" & fileName & " (" & ValidatePath(App.Path) & "NLM\" & localDir & "\" & fileName & ")"
    End If
End Function

Private Sub Form_Load()
    
    On Local Error Resume Next
    
    Dim logNum As Integer: logNum = FreeFile
    Open ValidatePath(App.Path) & "mfinst.log" For Append As #logNum

    Print #logNum, " --- Install Win32 Starting: " & Now
    Print #logNum, "  -- App.Path: " & ValidatePath(App.Path)
    Print #logNum, "  -- Version: " & App.major & "." & App.minor & "." & App.revision
    
    Dim osVer As OSVERSIONINFO
    osVer.dwOSVersionInfoSize = Len(osVer)
    
    If GetVersionEx(osVer) Then
        Print #logNum, "  -- Windows Version: " & osVer.dwMajorVersion & "." & osVer.dwMinorVersion & " Build: " & osVer.dwBuildNumber
        Print #logNum, "  -- Windows Platform: " & osVer.dwPlatformId & ", " & osVer.szCSDVersion
    End If
    
    Print #logNum, "  -- Checking package files:"
    
    ' Check for valid package ...
    MF_FilesMissing = ""
    MF_CheckFileExists logNum, "MailFlt.nlm", "SYSTEM"
    MF_CheckFileExists logNum, "MailFlt.nlm", "LIBC"
    MF_CheckFileExists logNum, "MFUpgr.nlm", "SYSTEM"
    MF_CheckFileExists logNum, "MFCONFIG.NLM", "SYSTEM"
    MF_CheckFileExists logNum, "MFRest.nlm", "SYSTEM"
    MF_CheckFileExists logNum, "MFNRM.NLM", "SYSTEM"
    MF_CheckFileExists logNum, "MFSTART.NCF", "SYSTEM"
    MF_CheckFileExists logNum, "MFSTOP.NCF", "SYSTEM"
    MF_CheckFileExists logNum, "FILTERS.BIN", "ETC"
    MF_CheckFileExists logNum, "REPORT.TPL", "ETC"
    MF_CheckFileExists logNum, "RINSIDE.TPL", "ETC"
    MF_CheckFileExists logNum, "ROUTRCPT.TPL", "ETC"
    MF_CheckFileExists logNum, "ROUTSNDR.TPL", "ETC"
    Print #logNum, "  -- Done."
    
    
    If MF_FilesMissing <> "" Then
        Print #logNum, " --- Package check results: Files missing. Aborting..."
        Close #logNum
        MsgBox "The following files are missing from the local package:" & vbCrLf & MF_FilesMissing & vbCrLf & "Install cannot continue. Please re-install the MailFilter/ax package.", vbCritical, "MailFilter/ax Installation Wizard"
        End
    End If
    
    Print #logNum, " --- Package check results: Success."
    '
    
    
    MFC_MF_InstallDir = Registry.GetRegStr(HKEY_LOCAL_MACHINE, "SOFTWARE\Hofstaedtler IE GmbH\MailFilter", "InstallDir")
    
    If Dir(ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT\CONFIG.BIN") = "" Then
            MFC_MF_InstallDir = ""
        ElseIf FileLen(ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT\CONFIG.BIN") < 1000 Then
            MFC_MF_InstallDir = ""
    End If
    
    
    If MFC_MF_InstallDir = "" Then
    
        Print #logNum, " --- Querying user for MailFilter/ax Installation Directory... " & Now
        Close #logNum
        
        frmChangeMF.Show vbModal
        If MFC_MF_InstallDir = "" Then
            End
        End If
        
        logNum = FreeFile
        Open ValidatePath(App.Path) & "mfinst.log" For Append As #logNum
        Print #logNum, " --- New MailFilter/ax Installation Directory: " & MFC_MF_InstallDir
        
    End If
    
    MFC_MF_InstallDir = ValidatePath(MFC_MF_InstallDir)
    
    s_UnloadNLM = True
    pctUnloadNLM.Picture = pct_SelectYes.Picture
    
    s_UpdateNLM = True
    pctUpdateNLM.Picture = pct_SelectYes.Picture

    s_LoadNLM = True
    pctLoadNLM.Picture = pct_SelectYes.Picture
    
    Dim szServerName As String
    
    Print #logNum, "  -- Connecting to Server..."
    Server_Logout
    Print #logNum, "  -- Getting Server Connection..."
    szServerName = Server_GetServerNameFromDrive(MFC_MF_InstallDir)
    lblServerName.Caption = szServerName
    Server_Login szServerName
    
    Dim serverVersion As NWCCVersion
    NWCCGetConnInfo modServer.myConnection, NWCC_INFO_SERVER_VERSION, Len(serverVersion), serverVersion
    Print #logNum, "  -- Server Version: " & serverVersion.major & "." & serverVersion.minor & "." & serverVersion.revision
    If ((serverVersion.major > 4) And (serverVersion.minor > 1)) Then
        If (serverVersion.revision > 2) Then
            'chkInstallLibC.Value = vbChecked
            Print #logNum, "  -- Selecting LibC Version"
            s_UpdateNLM = False
            s_UpdateLibCNLM = True
            pctUpdateNLM.Picture = pct_SelectNo.Picture
            pctUpdateLibCNLM.Picture = pct_SelectYes.Picture
        End If
    End If
    
    
    Print #logNum, " --- Read InstallDir: " & MFC_MF_InstallDir
    Print #logNum, " --- Closing Log, Waiting for user input: " & Now
    Close #logNum
    
    
    Me.Show
    cmdNext.SetFocus
    
End Sub

Private Sub mnuHelp_About_Click()
    frmAbout.Show
End Sub

Private Sub mnuHelp_AdminGuideEN_Click()
    OpenFile ValidatePath(App.Path) & "MailFilter Administrators Guide EN.pdf"
End Sub

Private Sub mnuHelp_AdminGuideDE_Click()
    OpenFile ValidatePath(App.Path) & "MailFilter Administrators Guide DE.pdf"
End Sub

Private Sub mnuMailFilter_ChangeServer_Click()
    
    frmChangeMF.Show vbModal, Me
    MFC_MF_InstallDir = ValidatePath(MFC_MF_InstallDir)
    
End Sub

Private Sub mnuMailFilter_Exit_Click()
    End
End Sub

Private Sub mnuMailFilter_QuickStart_Click()
    Me.Hide
    frmQuickStart.Show vbModal, Me
    Me.Show
End Sub

Private Sub mnuMailFilter_RemoveServerComponents_Click()
    Me.Hide
    frmRemoveInstallation.Show vbModal, Me
    Me.Show
End Sub

