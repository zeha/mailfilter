VERSION 5.00
Begin VB.Form frmSave 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   4  'Festes Werkzeugfenster
   Caption         =   "Save Changes"
   ClientHeight    =   4590
   ClientLeft      =   45
   ClientTop       =   285
   ClientWidth     =   5415
   ControlBox      =   0   'False
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
   Icon            =   "frmSave.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4590
   ScaleWidth      =   5415
   StartUpPosition =   1  'Fenstermitte
   Begin VB.PictureBox pctUpdateNLM 
      Appearance      =   0  '2D
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   720
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   14
      Top             =   2400
      Width           =   330
   End
   Begin VB.PictureBox pct_SelectNo 
      Appearance      =   0  '2D
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   4680
      Picture         =   "frmSave.frx":030A
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   13
      Top             =   2220
      Visible         =   0   'False
      Width           =   330
   End
   Begin VB.PictureBox pct_SelectYes 
      Appearance      =   0  '2D
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   4680
      Picture         =   "frmSave.frx":047C
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   12
      Top             =   1860
      Visible         =   0   'False
      Width           =   330
   End
   Begin VB.PictureBox pctQuitConfigurator 
      Appearance      =   0  '2D
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   720
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   10
      Top             =   3360
      Width           =   330
   End
   Begin VB.PictureBox pctLoadNLM 
      Appearance      =   0  '2D
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   720
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   8
      Top             =   2820
      Width           =   330
   End
   Begin VB.PictureBox pctCreateDirectories 
      Appearance      =   0  '2D
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   720
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   6
      Top             =   1980
      Width           =   330
   End
   Begin VB.PictureBox pctUnloadNLM 
      Appearance      =   0  '2D
      AutoSize        =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   330
      Left            =   720
      ScaleHeight     =   300
      ScaleWidth      =   300
      TabIndex        =   4
      Top             =   1560
      Width           =   330
   End
   Begin VB.CommandButton cmdCancel 
      BackColor       =   &H00FFFFFF&
      Cancel          =   -1  'True
      Caption         =   "&Don't Save"
      Height          =   375
      Left            =   3660
      TabIndex        =   3
      Top             =   4020
      Width           =   1575
   End
   Begin VB.CommandButton cmdSaveOnly 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Save &only"
      Height          =   375
      Left            =   1920
      TabIndex        =   2
      Top             =   4020
      Width           =   1575
   End
   Begin VB.CommandButton cmdSaveAct 
      BackColor       =   &H00FFFFFF&
      Caption         =   "&Save and Act"
      Default         =   -1  'True
      Height          =   375
      Left            =   180
      TabIndex        =   1
      Top             =   4020
      Width           =   1575
   End
   Begin VB.Label lblUpdateNLM 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Update MailFilter NLM"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   1140
      TabIndex        =   15
      Top             =   2460
      Width           =   2175
   End
   Begin VB.Image Image1 
      Height          =   405
      Left            =   3480
      Picture         =   "frmSave.frx":05EE
      Top             =   480
      Width           =   1800
   End
   Begin VB.Label lblQuitConfigurator 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Quit Configurator"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   1140
      TabIndex        =   11
      Top             =   3420
      Width           =   2175
   End
   Begin VB.Label lblLoadNLM 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Load MailFilter NLM"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   1140
      TabIndex        =   9
      Top             =   2880
      Width           =   2175
   End
   Begin VB.Label lblCreateDirectories 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Create Directories"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   1140
      TabIndex        =   7
      Top             =   2040
      Width           =   2175
   End
   Begin VB.Label lblUnloadNLM 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Unload MailFilter NLM"
      ForeColor       =   &H00000000&
      Height          =   255
      Left            =   1140
      TabIndex        =   5
      Top             =   1620
      Width           =   2175
   End
   Begin VB.Label lblIntroduction 
      BackColor       =   &H00FFFFFF&
      Caption         =   "You have chosen to save changes. Additionally the configurator can do the following things for you:"
      ForeColor       =   &H00000000&
      Height          =   495
      Left            =   180
      TabIndex        =   0
      Top             =   180
      Width           =   5055
   End
End
Attribute VB_Name = "frmSave"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private s_UnloadNLM As Boolean
Private s_UpdateNLM As Boolean
Private s_CreateDirectories As Boolean
Private s_LoadNLM As Boolean
Private s_QuitConfigurator As Boolean

Private Sub cmdCancel_Click()
    Unload Me
End Sub

Private Sub cmdSaveAct_Click()

    On Local Error Resume Next
    
    Me.Enabled = False
    
    Err.Clear
    
    If s_UnloadNLM Or s_LoadNLM Then
        Dim serverName As String
        Server_Logout
        serverName = Server_GetServerNameFromDrive(MFC_MF_InstallDir)
        Server_Login serverName
    End If
    
    
    If s_UnloadNLM Then
        lblUnloadNLM.FontItalic = False
        lblUnloadNLM.FontBold = True
        DoEvents
        
        Server_UnLoadNLM
        
        lblUnloadNLM.FontBold = False
        lblUnloadNLM.FontItalic = True
        DoEvents
        If Err.Number <> 0 Then
            pctUnloadNLM.Picture = pct_SelectNo.Picture
            Err.Clear
        End If
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
    If s_UpdateNLM Then
        lblUpdateNLM.FontItalic = False
        lblUpdateNLM.FontBold = True
        DoEvents
        
        Kill ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.BAK":    Err.Clear
        FileCopy ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.NLM", ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.BAK"
        Kill ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.NLM":    Err.Clear
        FileCopy ValidatePath(App.Path) & "NLM\SYSTEM\MailFlt.NLM", ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.NLM"
        
        Kill ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MFNRM.NLM":    Err.Clear
        FileCopy ValidatePath(App.Path) & "NLM\SYSTEM\MFNRM.NLM", ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MFNRM.NLM"
        Kill ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.NLM":    Err.Clear
        FileCopy ValidatePath(App.Path) & "NLM\SYSTEM\MFCONFIG.NLM", ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MFCONFIG.NLM"
        
    
        lblUpdateNLM.FontBold = False
        lblUpdateNLM.FontItalic = True
        DoEvents
        If Err.Number <> 0 Then
            pctUpdateNLM.Picture = pct_SelectNo.Picture
            Err.Clear
        End If
    End If
    
    If s_LoadNLM Then
        lblLoadNLM.FontItalic = False
        lblLoadNLM.FontBold = True
        DoEvents
        
        Server_LoadNLM
    
        lblLoadNLM.FontBold = False
        lblLoadNLM.FontItalic = True
        DoEvents
        If Err.Number <> 0 Then
            pctLoadNLM.Picture = pct_SelectNo.Picture
            Err.Clear
        End If
    End If
    
    If s_QuitConfigurator Then
        lblQuitConfigurator.FontItalic = False
        lblQuitConfigurator.FontBold = True
        DoEvents
        
        Me.Enabled = True
        End
        '' The code below will never be reached,
        '' So it is commented out ...
        'lblQuitConfigurator.FontBold = False
        'lblQuitConfigurator.FontItalic = True
        'DoEvents
    End If
    
    Me.Enabled = True
    
    Unload Me

End Sub

Private Sub cmdSaveOnly_Click()
    
    Unload Me
End Sub

Private Sub Form_Load()
    s_CreateDirectories = True
    pctCreateDirectories.Picture = pct_SelectYes.Picture
    
    s_LoadNLM = True
    pctLoadNLM.Picture = pct_SelectYes.Picture
    
    s_UnloadNLM = True
    pctUnloadNLM.Picture = pct_SelectYes.Picture
    
    s_UpdateNLM = True
    pctUpdateNLM.Picture = pct_SelectYes.Picture
    
    s_QuitConfigurator = False
    pctQuitConfigurator.Picture = pct_SelectNo.Picture
End Sub

Private Sub pctCreateDirectories_Click()
    s_CreateDirectories = Not s_CreateDirectories
    If s_CreateDirectories Then
            pctCreateDirectories.Picture = pct_SelectYes.Picture
        Else
            pctCreateDirectories.Picture = pct_SelectNo.Picture
    End If
End Sub

Private Sub pctLoadNLM_Click()
    s_LoadNLM = Not s_LoadNLM
    If s_LoadNLM Then
            pctLoadNLM.Picture = pct_SelectYes.Picture
        Else
            pctLoadNLM.Picture = pct_SelectNo.Picture
    End If
End Sub

Private Sub pctUpdateNLM_Click()
    s_UpdateNLM = Not s_UpdateNLM
    If s_UpdateNLM Then
            pctUpdateNLM.Picture = pct_SelectYes.Picture
        Else
            pctUpdateNLM.Picture = pct_SelectNo.Picture
    End If
End Sub

Private Sub pctQuitConfigurator_Click()
    s_QuitConfigurator = Not s_QuitConfigurator
    If s_QuitConfigurator Then
            pctQuitConfigurator.Picture = pct_SelectYes.Picture
        Else
            pctQuitConfigurator.Picture = pct_SelectNo.Picture
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
