VERSION 5.00
Begin VB.Form frmRemoveInstallation 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Remove MailFilter/ax from Server"
   ClientHeight    =   3690
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   7035
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Icon            =   "frmRemoveInstall.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3690
   ScaleWidth      =   7035
   StartUpPosition =   1  'CenterOwner
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "C&ancel"
      Height          =   435
      Left            =   2700
      TabIndex        =   4
      Top             =   3000
      Width           =   1755
   End
   Begin VB.TextBox txtServerName 
      Height          =   255
      Left            =   1080
      Locked          =   -1  'True
      TabIndex        =   3
      Top             =   1260
      Width           =   2835
   End
   Begin VB.CheckBox chkRemove 
      Caption         =   "Yes, Remove MailFilter/ax and the Configuration Files."
      Height          =   315
      Left            =   1080
      TabIndex        =   2
      Top             =   1740
      Width           =   4815
   End
   Begin VB.CommandButton cmdRemove 
      Caption         =   "Remove MailFilter"
      Enabled         =   0   'False
      Height          =   435
      Left            =   4560
      TabIndex        =   1
      Top             =   3000
      Width           =   1755
   End
   Begin VB.Label lblIntroduction 
      Caption         =   "To remove MailFilter and associated Configuration Files from the selected Server, verify the Server Name and then check the box."
      Height          =   555
      Left            =   540
      TabIndex        =   0
      Top             =   240
      Width           =   5775
   End
End
Attribute VB_Name = "frmRemoveInstallation"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub chkRemove_Click()
    If txtServerName.Text <> "" Then
            cmdRemove.Enabled = True
        Else
            MsgBox "MailFilter was not found or the selected Installation was invalid. Removal cannot continue.", vbCritical, "Removal Failure"
    End If
End Sub

Private Sub cmdCancel_Click()
    Unload Me
End Sub

Private Sub cmdRemove_Click()
    
    On Error Resume Next
    
    Dim szMessage As String
    
    Server_Logout
    Server_Login Server_GetServerNameFromDrive(MFC_MF_InstallDir)
    Server_UnLoadNLM
    
    Err.Clear
    
    Kill ValidatePath(MFC_MF_InstallDir) & "SYSTEM\MailFlt.nlm"
    
    Kill ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT\MAILFLT.CFG"
    Kill ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT\MAILFLT.BAK"
    Kill ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT\ATTACH.FLT"
    Kill ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT\ATTACH.BAK"
    
    RmDir ValidatePath(MFC_MF_InstallDir) & "ETC\MAILFLT"
    
    szMessage = "Removal has finished." & vbCrLf & "Please check that the NLM is unloaded and all server files are removed." & vbCrLf & "If you want, you can now remove the local Configurator installation."
    
    If Err.Number <> 0 Then
        Err.Clear
        szMessage = szMessage & vbCrLf & vbCrLf & "There were errors while removing files!"
    End If
    
    MsgBox szMessage, vbInformation, "Removal"

    Unload Me
End Sub

Private Sub Form_Load()
    txtServerName.Text = Server_GetServerNameFromDrive(MFC_MF_InstallDir)
End Sub
