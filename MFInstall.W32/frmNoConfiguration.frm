VERSION 5.00
Begin VB.Form frmQuickStart 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "MailFilter/ax Installation Wizard"
   ClientHeight    =   5940
   ClientLeft      =   45
   ClientTop       =   330
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
   Icon            =   "frmNoConfiguration.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5940
   ScaleWidth      =   7995
   StartUpPosition =   2  'CenterScreen
   Begin VB.CheckBox chkInstallLibC 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Install LibC-based MailFilter/ax NLMs"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   2160
      TabIndex        =   19
      Top             =   4920
      Width           =   3975
   End
   Begin VB.OptionButton optGwiaVersion 
      BackColor       =   &H00FFFFFF&
      Caption         =   "GWIA Version 5.2  / 5.5 / 5.5EP"
      Height          =   285
      Index           =   1
      Left            =   3930
      TabIndex        =   6
      Top             =   4500
      Width           =   2835
   End
   Begin VB.OptionButton optGwiaVersion 
      BackColor       =   &H00FFFFFF&
      Caption         =   "GWIA Version 6"
      Height          =   285
      Index           =   0
      Left            =   2160
      TabIndex        =   5
      Top             =   4500
      Value           =   -1  'True
      Width           =   1545
   End
   Begin VB.TextBox txtHostname 
      ForeColor       =   &H00000000&
      Height          =   285
      Left            =   2640
      TabIndex        =   3
      Top             =   3330
      Width           =   4635
   End
   Begin VB.TextBox txtMFLicenseKey 
      ForeColor       =   &H00000000&
      Height          =   285
      Left            =   2640
      TabIndex        =   4
      Top             =   4020
      Width           =   4635
   End
   Begin VB.CheckBox chkLoadNLM 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Load MailFilter/ax Configuration"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   2160
      TabIndex        =   7
      Top             =   5070
      Value           =   1  'Checked
      Visible         =   0   'False
      Width           =   3975
   End
   Begin VB.CommandButton cmdChangeServer 
      Caption         =   "&Change Server ..."
      Height          =   315
      Left            =   2040
      TabIndex        =   9
      Top             =   5460
      Width           =   1455
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "E&xit"
      Height          =   315
      Left            =   900
      TabIndex        =   10
      Top             =   5460
      Width           =   1095
   End
   Begin VB.CommandButton cmdNext 
      Caption         =   "Install"
      Default         =   -1  'True
      Enabled         =   0   'False
      Height          =   315
      Left            =   6180
      TabIndex        =   8
      Top             =   5460
      Width           =   1095
   End
   Begin VB.TextBox txtInternetDomainName 
      ForeColor       =   &H00000000&
      Height          =   285
      Left            =   2640
      TabIndex        =   2
      Top             =   2610
      Width           =   4635
   End
   Begin VB.TextBox txtGwiaConfigFile 
      ForeColor       =   &H00000000&
      Height          =   285
      Left            =   2640
      TabIndex        =   0
      Top             =   1920
      Width           =   4605
   End
   Begin VB.CommandButton cmdBrowseGwiaConfigFile 
      BackColor       =   &H00FFFFFF&
      Caption         =   "..."
      Height          =   255
      Left            =   7320
      TabIndex        =   1
      Top             =   1920
      Width           =   375
   End
   Begin VB.PictureBox pctWizard 
      BackColor       =   &H00FFFFFF&
      BorderStyle     =   0  'None
      Height          =   5370
      Left            =   -30
      Picture         =   "frmNoConfiguration.frx":5112
      ScaleHeight     =   5370
      ScaleWidth      =   1950
      TabIndex        =   14
      Top             =   -60
      Width           =   1950
      Begin VB.Label lblDestinationServer 
         Alignment       =   2  'Center
         BackColor       =   &H00FFFFFF&
         ForeColor       =   &H00000000&
         Height          =   255
         Left            =   60
         TabIndex        =   18
         Top             =   5130
         Width           =   1875
      End
   End
   Begin VB.Label lblHostname 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Hostname:"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   2160
      TabIndex        =   17
      Top             =   3030
      Width           =   3975
   End
   Begin VB.Label lblMFLicenseKey 
      BackColor       =   &H00FFFFFF&
      Caption         =   "MailFilter/ax License Key:"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   2160
      TabIndex        =   16
      Top             =   3720
      Width           =   3975
   End
   Begin VB.Label lblIntroduction 
      BackColor       =   &H00FFFFFF&
      Caption         =   "MailFilter/ax has detected that you have no (valid) configuration."
      ForeColor       =   &H00000000&
      Height          =   375
      Left            =   2100
      TabIndex        =   15
      Top             =   180
      Width           =   5715
   End
   Begin VB.Label lblInternetDomainName 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Internet Domain Name:"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   2160
      TabIndex        =   13
      Top             =   2310
      Width           =   3975
   End
   Begin VB.Label lblGWIARoot 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Internet Agent Configuration File:"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   2160
      TabIndex        =   12
      Top             =   1620
      Width           =   3975
   End
   Begin VB.Label lblIntroduction2 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Enter information in the fields below to create a new configuration or click Cancel to exit the Installation."
      ForeColor       =   &H00000000&
      Height          =   615
      Left            =   2100
      TabIndex        =   11
      Top             =   780
      Width           =   5655
   End
   Begin VB.Line Line1 
      X1              =   0
      X2              =   9540
      Y1              =   5340
      Y2              =   5340
   End
End
Attribute VB_Name = "frmQuickStart"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Wizard_Validate()
    
    Dim isValid As Boolean
    On Local Error GoTo Wizard_Validate_Err
    
    isValid = True
    
    If Dir(txtGwiaConfigFile.Text) = "" Then isValid = False
    If txtInternetDomainName.Text = "" Then isValid = False
    
    Dim serverName As String
    serverName = UCase(Server_GetServerNameFromDrive(Left(txtGwiaConfigFile.Text, 1)))
    If (serverName <> "") Then
        lblDestinationServer.Caption = serverName
        Else
        lblDestinationServer.Caption = ""
    End If
    
    If isValid Then
        cmdNext.Enabled = True
        Else
        cmdNext.Enabled = False
    End If
    
Wizard_Validate_Exit:
    Exit Sub
    
Wizard_Validate_Err:
End Sub

Private Sub Wizard_ReadConfig()
    
    On Local Error Resume Next
    
    Dim logNum As Integer: logNum = FreeFile
    Open ValidatePath(App.Path) & "mfinst.log" For Append As #logNum

    Print #logNum, " --- QuickStart reading GWIA.CFG: " & Now
    Print #logNum, "  -- Selected GWIA.CFG is: " & txtGwiaConfigFile.Text
    
    Dim GwiaCfgFile As String
    
    GwiaCfgFile = txtGwiaConfigFile.Text
    
    If GwiaCfgFile = "" Then
        Print #logNum, "  -- Selected GWIA.CFG is EMPTY. Closing Log: " & Now
        Close #logNum
        Exit Sub
    End If
    If Dir(GwiaCfgFile) = "" Then
        Print #logNum, "  -- Selected GWIA.CFG is NOT FOUND. Closing Log: " & Now
        Close #logNum
        Exit Sub
    End If

    optGwiaVersion(0).Value = False
    optGwiaVersion(1).Value = True

    Dim thisLine As String
    Dim thisParameter As String, thisValue As String
    Dim i, j, x As Integer
    Dim fileNum As Integer: fileNum = FreeFile(0)
    Open GwiaCfgFile For Input As #fileNum
        Print #logNum, "  -- GWIA.CFG is OPEN. "
    
        Do
            Line Input #fileNum, thisLine
            If Left(thisLine, 1) = "/" Then
                i = 0
                i = InStr(1, thisLine, "=", vbBinaryCompare)
                If (i = 0) Then
                    i = InStr(1, thisLine, "-", vbBinaryCompare)
                End If
                If (i <> 0) Then
                    thisParameter = Mid(thisLine, 2, i - 2)
                    thisValue = Mid(thisLine, i + 1)
                    If Left(thisValue, 1) = """" Then
                        thisValue = Mid(thisValue, 2)
                    End If
                    If Right(thisValue, 1) = """" Then
                        thisValue = Left(thisValue, Len(thisValue) - 1)
                    End If
                    Select Case LCase(thisParameter)
                    Case "hn"
                        If (txtHostname.Text = "") Then
                            txtHostname.Text = thisValue
                        End If
                        j = InStr(1, thisValue, ".", vbBinaryCompare)
                        If (j <> 0) Then
                            If (txtInternetDomainName.Text = "") Then
                                txtInternetDomainName.Text = Mid(thisValue, j + 1)
                            End If
                        End If
                        Print #logNum, "  -- Hostname is " & txtHostname.Text
                        Print #logNum, "  -- InternetDomainName is " & txtInternetDomainName.Text
                    Case "dsnage"
                        'DSNAGE should only be available with GWIA6
                        optGwiaVersion(0).Value = True
                        Print #logNum, "  -- GWIA seems to be V6"
                    End Select
                End If
            End If
        
        Loop Until EOF(fileNum)
    
    Close #fileNum
    Print #logNum, "  -- GWIA.CFG is CLOSED."
    
    Print #logNum, " --- QuickStart waiting for User Input. Closing Log: " & Now
    Close #logNum
       
    
End Sub

Private Sub cmdBrowseGwiaConfigFile_Click()
    
    On Local Error Resume Next
    
    Dim GwiaCfgFile As String
    GwiaCfgFile = ComDlg_OpenFile("", "Open GWIA.CFG ...", Me.hWnd, "GWIA Configuration File (gwia.cfg)" & Chr(0) & "gwia.cfg" & Chr(0) & "Configuration Files (*.cfg)" & Chr(0) & "*.cfg" & Chr(0) & "All Files (*.*)" & Chr(0) & "*.*")
    
    If GwiaCfgFile = "" Then Exit Sub
    
    If Dir(GwiaCfgFile) = "" Then
        MsgBox "Could not find the specified file:" & vbCrLf & GwiaCfgFile
        Exit Sub
    End If

    txtGwiaConfigFile.Text = GwiaCfgFile
    
    Wizard_Validate
    Wizard_ReadConfig
    
End Sub

Private Sub cmdChangeServer_Click()
    frmChangeMF.Show vbModal, Me
    Shell ValidatePath(App.Path) & App.EXEName & ".exe", vbNormalFocus
    End
End Sub

Private Sub cmdExit_Click()
    End
End Sub

Private Sub cmdNext_Click()

    On Local Error GoTo cmdNext_Click_Err

Dim MFT_MFLT_ROOT As String
Dim MFT_GWIA_ROOT As String

Dim MFC_DOMAIN_Name As String
Dim MFC_DOMAIN_Hostname As String
Dim MFC_DOMAIN_EMail_PostMaster As String
Dim MFC_DOMAIN_EMail_MailFilter As String

Dim MFC_GWIA_Version As Integer

Dim MFL_Code As String

Dim MFC_NOTIFICATION_ENABLE_INTERNAL_RCPT As Boolean
Dim MFC_NOTIFICATION_ENABLE_INTERNAL_SNDR As Boolean

Dim errNum As Integer
Dim errDesc As String
Dim szServerName As String

    Dim logNum As Integer: logNum = FreeFile
    Open ValidatePath(App.Path) & "mfinst.log" For Append As #logNum
    MFC_MF_InstallDir = ValidatePath(Left(txtGwiaConfigFile.Text, 2))

    Print #logNum, " --- MF Install Win32 Starting: " & Now
    Print #logNum, "  -- App.Path: " & ValidatePath(App.Path)
    Print #logNum, "  -- Version: " & App.major & "." & App.minor & "." & App.revision
    
    Dim osVer As OSVERSIONINFO
    osVer.dwOSVersionInfoSize = Len(osVer)
    
    If GetVersionEx(osVer) Then
        Print #logNum, "  -- Windows Version: " & osVer.dwMajorVersion & "." & osVer.dwMinorVersion & " Build: " & osVer.dwBuildNumber
        Print #logNum, "  -- Windows Platform: " & osVer.dwPlatformId & ", " & osVer.szCSDVersion
    End If
    
    szServerName = Server_GetServerNameFromDrive(Left(MFC_MF_InstallDir, 1))
    
    Print #logNum, "  -- InstallDir: " & MFC_MF_InstallDir
    Print #logNum, "  -- InstallDrive: " & Left(MFC_MF_InstallDir, 1)
    Print #logNum, "  -- InstallServer: " & szServerName
    
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
    
    cmdNext.Caption = "Working..."
    cmdNext.Enabled = False
    Me.Enabled = False
    
    Dim fileNum As Integer: fileNum = FreeFile
    Dim curLine As String
    Dim parAr() As String
    
    Dim new_GWIARoot As String
    Dim new_GWIASmtpHome As String
    
    Dim noErrors As Boolean
    Dim tmpStr As String
    noErrors = True
    
    If (optGwiaVersion(0).Value = True) Then
        MFC_GWIA_Version = 600
    Else
        MFC_GWIA_Version = 550
    End If

    Print #logNum, "  -- GWIA Version: " & MFC_GWIA_Version

    Print #logNum, "  -- GWIA ConfigFile: " & txtGwiaConfigFile.Text
    Open txtGwiaConfigFile.Text For Input As #fileNum
        Do
            'If EOF(fileNum) Then Exit Do
            Line Input #fileNum, curLine
            parAr = Split(curLine, "-", 2, vbTextCompare)
            If Not UBound(parAr) > 0 Then
                parAr = Split(curLine, "=", 2, vbTextCompare)
            End If

            If UBound(parAr) > 0 Then
                Select Case LCase(parAr(0))
                Case "/dhome"
                    new_GWIARoot = TrimStr(parAr(1))
                    Print #logNum, "  -- GWIA Root: " & new_GWIARoot
                Case "/smtphome"
                    new_GWIASmtpHome = TrimStr(parAr(1))
                    Print #logNum, "  -- GWIA SMTP: " & new_GWIASmtpHome
                End Select
            
                If (new_GWIARoot <> "") And (new_GWIASmtpHome <> "") Then
                    Exit Do
                End If
            End If
        Loop Until EOF(fileNum)
    Close #fileNum
    Print #logNum, "  -- GWIA Config File Closed."
    
    Dim pos As Integer
    
    If (new_GWIARoot = "") Then
            MsgBox "No valid GWIA configuration was found." & vbCrLf & _
                   "Please check that your GWIA.CFG includes the Home, DHome and SMTPHome switches.", vbCritical, "(D)Home Switch Not Found"
            noErrors = False
            Print #logNum, "  ** GWIARoot is Empty, Aborting! " & Now
            Close #logNum
            Exit Sub
    End If
    
    tmpStr = MkNetwarePath(MkUNCPath(new_GWIARoot, szServerName))
    If tmpStr <> "" Then new_GWIARoot = tmpStr
    Print #logNum, "  -- GWIA NetWare Path: " & new_GWIARoot
    
    tmpStr = MkNetwarePath(MkUNCPath(new_GWIASmtpHome, szServerName))
    If tmpStr <> "" Then new_GWIASmtpHome = tmpStr
    Print #logNum, "  -- SMTP NetWare Path: " & new_GWIASmtpHome
    
    If (new_GWIASmtpHome = "") Then
        Print #logNum, "  -- GWIASmtpHome is Empty, Asking User..."
        
        new_GWIASmtpHome = new_GWIARoot & "\MF"
        If MsgBox("The Wizard did not find a configured SMTP home for the selected GWIA." & vbCrLf & _
            "The SMTP Home will be set to """ & new_GWIASmtpHome & """." & vbCrLf & _
            "Please update your GWIA Configuration to reflect this." & vbCrLf & _
            vbCrLf & _
            "MailFilter will load, but will not scan E-Mails until the GWIA Configuration is updated and GWIA is restarted." & vbCrLf & _
            vbCrLf & _
            "Say Yes if you want the Configuration Wizard to update the selected GWIA.CFG.", vbInformation + vbYesNo, "GWIA SMTP Home") = vbYes Then
            
            Print #logNum, "  -- Updating GWIA.CFG on User's request"
            
            On Local Error Resume Next
            SetAttr txtGwiaConfigFile.Text, vbNormal
            Err.Clear
            On Local Error GoTo cmdNext_Click_Err
            
            fileNum = FreeFile()
            Open txtGwiaConfigFile.Text For Append As #fileNum
                Print #fileNum, "/SmtpHome=" & new_GWIASmtpHome
            Close #fileNum
        End If
        DoEvents
        Me.Refresh
        DoEvents
    End If
    

    tmpStr = DeValidatePath(MkUNCPath(new_GWIARoot, szServerName))
    Print #logNum, "  -- Checking for Dir [GWIA]: " & tmpStr
    If Dir(tmpStr, vbDirectory) = "" Then
        Print #logNum, "  -- Creating Dir: " & tmpStr
        MkDir tmpStr
    End If
    
    tmpStr = DeValidatePath(MkUNCPath(new_GWIASmtpHome, szServerName))
    Print #logNum, "  -- Checking for Dir [SMTP]: " & tmpStr
    If Dir(tmpStr, vbDirectory) = "" Then
        Print #logNum, "  -- Creating Dir: " & tmpStr
        MkDir tmpStr
    End If
    
    
'    Print #logNum, "  -- Checking for Dir: " & tmpStr
    
    Print #logNum, "  -- Check ETC\MAILFLT: " & MFC_MF_InstallDir & "ETC\MAILFLT"
    If Dir(MFC_MF_InstallDir & "ETC\MAILFLT", vbDirectory) = "" Then
        Print #logNum, "  -- Create ETC\MAILFLT: " & MFC_MF_InstallDir & "ETC\MAILFLT"
        MkDir MFC_MF_InstallDir & "ETC\MAILFLT"
    End If
    Print #logNum, "  -- Check ETC\MAILFLT\DEFAULTS: " & MFC_MF_InstallDir & "ETC\MAILFLT\DEFAULTS"
    If Dir(MFC_MF_InstallDir & "ETC\MAILFLT\DEFAULTS", vbDirectory) = "" Then
        Print #logNum, "  -- Create ETC\MAILFLT\DEFAULTS: " & MFC_MF_InstallDir & "ETC\MAILFLT\DEFAULTS"
        MkDir MFC_MF_InstallDir & "ETC\MAILFLT\DEFAULTS"
    End If
    
    Print #logNum, "  -- Copying Files:"
    ' ************************** Install NLMs ...
    ' ** MailFilter/ax professional
    If chkInstallLibC.Value = vbChecked Then
        Print #logNum, "   - LibC\MailFlt.NLM"
        If MF_InstallFile(logNum, "MailFlt.nlm", "SYSTEM", True, "LIBC") = False Then noErrors = False
    Else
        Print #logNum, "   - MailFlt.NLM"
        If MF_InstallFile(logNum, "MailFlt.nlm", "SYSTEM", True) = False Then noErrors = False
    End If
    ' ** MailFilter/ax Configuration
    Print #logNum, "   - MFUpgr.NLM"
    If MF_InstallFile(logNum, "MFUpgr.nlm", "SYSTEM", True) = False Then noErrors = False
    Print #logNum, "   - MFCONFIG.NLM"
    If MF_InstallFile(logNum, "MFCONFIG.NLM", "SYSTEM", True) = False Then noErrors = False
    ' ** MailFilter/ax Restore
    Print #logNum, "   - MFRest.NLM"
    If MF_InstallFile(logNum, "MFRest.nlm", "SYSTEM", True) = False Then noErrors = False
    ' ** MailFilter/NRM
    Print #logNum, "   - MFNRM.NLM"
    If MF_InstallFile(logNum, "MFNRM.NLM", "SYSTEM", True) = False Then noErrors = False
    ' ** Load Scripts
    Print #logNum, "   - MFSTART.NCF"
    If MF_InstallFile(logNum, "MFSTART.NCF", "SYSTEM", False) = False Then noErrors = False
    Print #logNum, "   - MFSTOP.NCF"
    If MF_InstallFile(logNum, "MFSTOP.NCF", "SYSTEM", False) = False Then noErrors = False
    ' ** Defaults
    Print #logNum, "   - FILTERS.BIN"
    If MF_InstallFile(logNum, "FILTERS.BIN", "ETC\MAILFLT\DEFAULTS", True, "ETC") = False Then noErrors = False
    ' ** Default Templates
    Print #logNum, "   - REPORT.TPL"
    If MF_InstallFile(logNum, "REPORT.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC") = False Then noErrors = False
    If MF_InstallFile(logNum, "REPORT.TPL", "ETC\MAILFLT", True, "ETC") = False Then noErrors = False
    Print #logNum, "   - RINSIDE.TPL"
    If MF_InstallFile(logNum, "RINSIDE.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC") = False Then noErrors = False
    If MF_InstallFile(logNum, "RINSIDE.TPL", "ETC\MAILFLT", True, "ETC") = False Then noErrors = False
    Print #logNum, "   - ROUTRCPT.TPL"
    If MF_InstallFile(logNum, "ROUTRCPT.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC") = False Then noErrors = False
    If MF_InstallFile(logNum, "ROUTRCPT.TPL", "ETC\MAILFLT", True, "ETC") = False Then noErrors = False
    Print #logNum, "   - ROUTSNDR.TPL"
    If MF_InstallFile(logNum, "ROUTSNDR.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC") = False Then noErrors = False
    If MF_InstallFile(logNum, "ROUTSNDR.TPL", "ETC\MAILFLT", True, "ETC") = False Then noErrors = False
    Print #logNum, "   - MAILCOPY.TPL"
    If MF_InstallFile(logNum, "MAILCOPY.TPL", "ETC\MAILFLT\DEFAULTS", True, "ETC") = False Then noErrors = False
    If MF_InstallFile(logNum, "MAILCOPY.TPL", "ETC\MAILFLT", True, "ETC") = False Then noErrors = False
    ' **************************
    
    Print #logNum, "  -- Configuration:"
    MFC_DOMAIN_Name = txtInternetDomainName.Text
    MFC_DOMAIN_EMail_MailFilter = "<MailFilter@" & MFC_DOMAIN_Name & ">"
    MFC_DOMAIN_EMail_PostMaster = "<PostMaster@" & MFC_DOMAIN_Name & ">"
    MFT_GWIA_ROOT = new_GWIARoot
    MFT_MFLT_ROOT = new_GWIASmtpHome
    MFL_Code = txtMFLicenseKey.Text
    
    Print #logNum, "   - MFC_DOMAIN_Name: " & MFC_DOMAIN_Name
    Print #logNum, "   - MFC_DOMAIN_EMail_MailFilter: " & MFC_DOMAIN_EMail_MailFilter
    Print #logNum, "   - MFC_DOMAIN_EMail_PostMaster: " & MFC_DOMAIN_EMail_PostMaster
    Print #logNum, "   - MFT_GWIA_ROOT: " & MFT_GWIA_ROOT
    Print #logNum, "   - MFT_MFLT_ROOT: " & MFT_MFLT_ROOT
    Print #logNum, "   - MFL_Code: " & MFL_Code
  
    Dim inpStr As String
    Dim fN As Integer: fN = FreeFile
    
    Print #logNum, "  -- Writing Configuration " & MFC_MF_InstallDir & "ETC\MAILFLT\EXEPATH.CFG"
    
    Open MFC_MF_InstallDir & "ETC\MAILFLT\EXEPATH.CFG" For Output As #fN
        Print #fN, "\\" & szServerName & "\SYS\SYSTEM"
    Close #fN
    
    Print #logNum, "  -- Writing Configuration " & MFC_MF_InstallDir & "ETC\MAILFLT\CONFIG.BIN"
    
    Dim tmpBuf As String
    Open MFC_MF_InstallDir & "ETC\MAILFLT\CONFIG.BIN" For Binary As #fN

        tmpBuf = "MAILFILTER_R001_008" & Chr(0) & Chr(0) & Chr(0) & "FRESH" & String(329 - 90 - 20 - 7, Chr(0)) & Chr(0) & MFL_Code & Chr(0) & String(88 - Len(MFL_Code), Chr(0)) & Chr(0)
        Put #fN, , tmpBuf
        
        tmpBuf = MFT_GWIA_ROOT & Chr(0) & String(328 - Len(MFT_GWIA_ROOT), Chr(0)) & Chr(0)
        Put #fN, , tmpBuf
        
        tmpBuf = MFT_MFLT_ROOT & Chr(0) & String(328 - Len(MFT_MFLT_ROOT), Chr(0)) & Chr(0)
        Put #fN, , tmpBuf
        
        tmpBuf = MFC_DOMAIN_Name & Chr(0) & String(328 - Len(MFC_DOMAIN_Name), Chr(0)) & Chr(0)
        Put #fN, , tmpBuf
        
        tmpBuf = MFC_DOMAIN_EMail_MailFilter & Chr(0) & String(328 - Len(MFC_DOMAIN_EMail_MailFilter), Chr(0)) & Chr(0)
        Put #fN, , tmpBuf
        
        tmpBuf = MFC_DOMAIN_EMail_PostMaster & Chr(0) & String(328 - Len(MFC_DOMAIN_EMail_PostMaster), Chr(0)) & Chr(0)
        Put #fN, , tmpBuf
        
        tmpBuf = MFC_DOMAIN_Hostname & Chr(0) & String(328 - Len(MFC_DOMAIN_Hostname), Chr(0)) & Chr(0)
        Put #fN, , tmpBuf
        
        tmpBuf = String(3500 - Loc(2), Chr(0))
        Put #fN, , tmpBuf
        
        
        tmpBuf = "1"
        tmpBuf = tmpBuf & String(9, Chr(0))
        Put #fN, , tmpBuf
        
        tmpBuf = "0"
        tmpBuf = tmpBuf & String(24, Chr(0)) & MFC_GWIA_Version & Chr(0) & String(455, Chr(0))
        Put #fN, , tmpBuf
        
        Dim curItem As Integer
        
'        inpStr = ""
'        fN = FreeFile(1)
'        Open ValidatePath(App.Path) & "NLM\ETC\MailFlt\DEFAULTS\Attach.flt" For Input As #fN
'            curItem = 0
'            Do
'                curItem = curItem + 1
'                If EOF(fN) Then Exit Do
'
'                Line Input #fN, inpStr
'                tmpBuf = inpStr & String(49 - Len(inpStr), Chr(0)) & Chr(0)
'                Put #2, , tmpBuf
'            Loop Until EOF(fN)
'        Close #fN
'
'        tmpBuf = String(50, Chr(0))
'        For curItem = curItem To 150
'            Put #2, , tmpBuf
'        Next curItem
        
        
'        inpStr = ""
'        fN = FreeFile(1)
'        Open ValidatePath(App.Path) & "NLM\ETC\MailFlt\DEFAULTS\Subject.flt" For Input As #fN
'            curItem = 0
'            Do
'                curItem = curItem + 1
'                If EOF(fN) Then Exit Do
'
'                Line Input #fN, inpStr
'                tmpBuf = inpStr & String(74 - Len(inpStr), Chr(0)) & Chr(0)
'                Put #2, , tmpBuf
'            Loop Until EOF(fN)
'        Close #fN
        
'        tmpBuf = String(75, Chr(0))
'        For curItem = curItem To 1000
'            Put #2, , tmpBuf
'        Next curItem
        
        ' fill the initial configuration file with zeros. mfupgr.nlm will then import the filter lists ...

        tmpBuf = String(50, Chr(0))
        For curItem = 1 To 200
            Put #fN, , tmpBuf
        Next curItem
    Close #fN
    Print #logNum, "  -- Done."
    
    '
    '>> Update Autoexec.NCF
    On Local Error Resume Next
    SetAttr ValidatePath(MFC_MF_InstallDir) & "SYSTEM\AUTOEXEC.NCF", vbNormal
    Err.Clear
    On Local Error GoTo cmdNext_Click_Err
    
    Dim updateAutoexecNCF As Boolean: updateAutoexecNCF = True
    Print #logNum, "  -- Updating " & MFC_MF_InstallDir & "SYSTEM\AUTOEXEC.NCF"
    fN = FreeFile
    Open ValidatePath(MFC_MF_InstallDir) & "SYSTEM\AUTOEXEC.NCF" For Input As #fN
        Do
            If (EOF(fN)) Then Exit Do
            Line Input #fN, tmpStr
            If InStr(1, tmpStr, "MFSTART", vbBinaryCompare) <> 0 Then
                updateAutoexecNCF = False
            End If
        Loop Until EOF(fN)
    Close #fN
    
    If updateAutoexecNCF = True Then
        Open ValidatePath(MFC_MF_InstallDir) & "SYSTEM\AUTOEXEC.NCF" For Append As #fN
        Print #fN, ""
        Print #fN, "#"
        Print #fN, "# The following lines have beend added by"
        Print #fN, "# the MailFilter/ax Installation Wizard."
        Print #fN, "#"
        Print #fN, "MFSTART"
        Print #fN, "#"
        Print #fN, "# --- MFI: " & Now & " ---"
        Print #fN, "#"
        Print #fN, ""
        Close #fN
        Else
        Print #logNum, "  -- Not updating, MFSTART is already there..."
    End If
    Print #logNum, "  -- Done."
    
    Print #logNum, "  -- Unloading MailFilter NLMs..."
    Server_UnLoadNLM
    Print #logNum, "  -- Load MFUPGR.NLM..."
    Server_LoadNLMAny "MFUPGR.NLM"
    Print #logNum, "  -- Done."

    cmdNext.Caption = "Save"
    Me.Enabled = True
    
    MsgBox "MailFilter/ax has been installed on the selected Server." & vbCrLf & vbCrLf & _
            "You now have to complete the Configuration:" & vbCrLf & _
            "* Go to Server " & Server_GetServerNameFromDrive(MFC_MF_InstallDir) & ", or use RConsole." & vbCrLf & _
            "* Verify the Configuration with MFConfig.NLM." & vbCrLf & _
            "* Enter MFSTART to start MailFilter/ax." & vbCrLf & _
            vbCrLf & _
            "Thank you for using MailFilter/ax!", vbInformation, "MailFilter/ax Installation"
    
    Print #logNum, " --- MF Install Win32 Complete: " & Now
    Print #logNum, ""
    Close #logNum
    
    End
    
cmdNext_Click_Exit:
    cmdNext.Caption = "Install"
    Me.Enabled = True
    cmdNext.Enabled = True
    Exit Sub

cmdNext_Click_Err:
    errNum = Err.Number
    errDesc = Err.Description
    DoLogError logNum, errNum, errDesc
    MsgBox "The following error occoured:" & vbCrLf & _
        errNum & ": " & errDesc, vbCritical, "MailFilter/ax Installation"
    Err.Clear
    Resume cmdNext_Click_Exit

End Sub

Private Sub DoLogError(logNum As Integer, errNum As Integer, errDesc As String)
    On Local Error Resume Next
    Print #logNum, " *** ERROR: " & errNum & " - " & errDesc
End Sub


Private Sub Form_Load()
    On Local Error Resume Next
    
    Dim logNum As Integer: logNum = FreeFile
    Open ValidatePath(App.Path) & "mfinst.log" For Append As #logNum

    Print #logNum, " --- QuickStart Win32 Starting: " & Now
    Print #logNum, "  -- App.Path: " & ValidatePath(App.Path)
    Print #logNum, "  -- Version: " & App.major & "." & App.minor & "." & App.revision
    
    Dim osVer As OSVERSIONINFO
    osVer.dwOSVersionInfoSize = Len(osVer)
    
    If GetVersionEx(osVer) Then
        Print #logNum, "  -- Windows Version: " & osVer.dwMajorVersion & "." & osVer.dwMinorVersion & " Build: " & osVer.dwBuildNumber
        Print #logNum, "  -- Windows Platform: " & osVer.dwPlatformId & ", " & osVer.szCSDVersion
    End If
    
    Dim szServerName As String
    Print #logNum, "  -- Connecting to Server..."
    Server_Logout
    Print #logNum, "  -- Getting Server Connection..."
    Server_Login Server_GetServerNameFromDrive(MFC_MF_InstallDir)
    szServerName = Server_GetServerNameFromDrive(Left(MFC_MF_InstallDir, 1))
    
    Dim serverVersion As NWCCVersion
    NWCCGetConnInfo modServer.myConnection, NWCC_INFO_SERVER_VERSION, Len(serverVersion), serverVersion
    Print #logNum, "  -- Server Version: " & serverVersion.major & "." & serverVersion.minor & "." & serverVersion.revision
    chkInstallLibC.Value = vbUnchecked
    If ((serverVersion.major > 4) And (serverVersion.minor = 1) And (serverVersion.revision > 2)) Then
            chkInstallLibC.Value = vbChecked
            Print #logNum, "  -- Selecting LibC Version for 5.1 SP3"
    End If
    If ((serverVersion.major = 5) And (serverVersion.minor = 6) And (serverVersion.revision > 2)) Then
            chkInstallLibC.Value = vbChecked
            Print #logNum, "  -- Selecting LibC Version for 6.0 SP3"
    End If
    If ((serverVersion.major = 5) And (serverVersion.minor > 6)) Then
            chkInstallLibC.Value = vbChecked
            Print #logNum, "  -- Selecting LibC Version for 6.5 and newer"
    End If

    Print #logNum, " --- QuickStart waiting for User Input. Closing Log: " & Now
    Close #logNum
    
End Sub

Private Sub txtGwiaConfigFile_Change()
    Wizard_Validate
End Sub

Private Sub txtGwiaConfigFile_LostFocus()
    Wizard_ReadConfig
End Sub

Private Sub txtInternetDomainName_Change()
    Wizard_Validate
End Sub
