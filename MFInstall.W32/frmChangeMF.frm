VERSION 5.00
Begin VB.Form frmChangeMF 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Select MailFilter/ax Installation"
   ClientHeight    =   5940
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   7995
   Icon            =   "frmChangeMF.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5940
   ScaleWidth      =   7995
   StartUpPosition =   2  'CenterScreen
   Begin VB.PictureBox pctWizard 
      AutoSize        =   -1  'True
      BorderStyle     =   0  'None
      Height          =   5250
      Left            =   0
      Picture         =   "frmChangeMF.frx":5112
      ScaleHeight     =   5250
      ScaleWidth      =   1950
      TabIndex        =   5
      Top             =   0
      Width           =   1950
   End
   Begin VB.CommandButton cmdOK 
      BackColor       =   &H00FFFFFF&
      Caption         =   "O&K"
      Default         =   -1  'True
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   315
      Left            =   6180
      TabIndex        =   4
      Top             =   5460
      Width           =   1095
   End
   Begin VB.CommandButton cmdCancel 
      BackColor       =   &H00FFFFFF&
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   315
      Left            =   4980
      TabIndex        =   3
      Top             =   5460
      Width           =   1095
   End
   Begin VB.CommandButton cmdBrowse 
      BackColor       =   &H00FFFFFF&
      Caption         =   "..."
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   6720
      TabIndex        =   2
      Top             =   2520
      Width           =   375
   End
   Begin VB.TextBox txtPath 
      BackColor       =   &H00FFFFFF&
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   285
      Left            =   2640
      TabIndex        =   1
      Text            =   "S:\"
      Top             =   2520
      Width           =   3975
   End
   Begin VB.Label lblIntroduction 
      BackColor       =   &H00FFFFFF&
      Caption         =   $"frmChangeMF.frx":5B81
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   855
      Left            =   2100
      TabIndex        =   6
      Top             =   180
      Width           =   5460
   End
   Begin VB.Line Line1 
      X1              =   0
      X2              =   9540
      Y1              =   5280
      Y2              =   5280
   End
   Begin VB.Label lblSelect 
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      Caption         =   "Select MailFilter/ax Installation Server: ( SYS Drive )"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   195
      Left            =   2160
      TabIndex        =   0
      Top             =   2220
      Width           =   3720
   End
End
Attribute VB_Name = "frmChangeMF"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit


Private Sub cmdBrowse_Click()
''    cdgBrowse.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNPathMustExist Or cdlOFNShareAware
''
''    If txtPath.Text = "" Then
''        cdgBrowse.InitDir = MFC_MF_InstallDir
''        Else
''        cdgBrowse.InitDir = txtPath.Text
''    End If
''
''    cdgBrowse.ShowOpen
''
''    Dim pL As Integer
''    Dim pR As Integer
''    Dim str As String, searchstr As String
''
''    str = cdgBrowse.FileName
''
''    searchstr = "\mailflt.nlm"
''    pL = InStr(1, str, searchstr, vbTextCompare)
''    If pL > 1 Then
''        pR = pL + Len(searchstr)
''        str = Left(str, pL - 1) & Mid(str, pR + 1)
''    End If
''
''    searchstr = "system"
''    pL = InStr(1, str, searchstr, vbTextCompare)
''    If pL > 1 Then
''        pR = pL + Len(searchstr)
''        str = Left(str, pL - 1) & Mid(str, pR + 1)
''    End If
''
''    txtPath.Text = str
    
    Dim szResult As String
    szResult = ComDlg_OpenDir("Select the server's SYS: drive where MailFilter is installed:", Me.hWnd)
    If szResult <> "" Then
        txtPath.Text = szResult
    End If
    
End Sub

Private Sub cmdCancel_Click()
    Unload Me
End Sub

Private Sub cmdOK_Click()
    On Local Error Resume Next
    
    MFC_MF_InstallDir = txtPath.Text
    
    Dim logNum As Integer: logNum = FreeFile
    Open ValidatePath(App.Path) & "mfinst.log" For Append As #logNum
    Print #logNum, " --- User selected: " & MFC_MF_InstallDir
    Print #logNum, " --- Writing Reg Key"
    
    Registry.SetRegStr HKEY_LOCAL_MACHINE, "SOFTWARE\Hofstaedtler IE GmbH\MailFilter", "InstallDir", CStr(MFC_MF_InstallDir)

    Print #logNum, " --- Checking for new installation:"

    If Dir(ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT\CONFIG.BIN") = "" Then
        Print #logNum, " --- Yes. Closing Log. " & Now
        Close #logNum
        frmQuickStart.Show vbModal
        Unload Me
        Exit Sub
    End If
    Print #logNum, " --- Checking for corrupt installation:"
    If FileLen(ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT\CONFIG.BIN") < 1000 Then
        Print #logNum, " --- Yes. Closing Log. " & Now
        Close #logNum
        frmQuickStart.Show vbModal
        Unload Me
        Exit Sub
    End If
    Print #logNum, " --- Done. Closing Log. " & Now
    Close #logNum

    Unload Me
End Sub

Private Sub Form_Load()
    If MFC_MF_InstallDir <> "" Then
        txtPath.Text = MFC_MF_InstallDir
    End If
End Sub
