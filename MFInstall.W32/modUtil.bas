Attribute VB_Name = "modUtil"
Option Explicit
Public Declare Function GetVersionEx Lib "kernel32" Alias "GetVersionExA" (lpVersionInformation As OSVERSIONINFO) As Long

Public Type OSVERSIONINFO
        dwOSVersionInfoSize As Long
        dwMajorVersion As Long
        dwMinorVersion As Long
        dwBuildNumber As Long
        dwPlatformId As Long
        szCSDVersion As String * 128      '  Maintenance string for PSS usage
End Type

Private Declare Function ShellExecute Lib "shell32.dll" Alias "ShellExecuteA" (ByVal hWnd As Long, ByVal lpOperation As String, ByVal lpFile As String, ByVal lpParameters As String, ByVal lpDirectory As String, ByVal nShowCmd As Long) As Long
Public Declare Function InitCommonControls Lib "comctl32" ()

Public Function ValidatePath(inPath As String) As String

    If Right(inPath, 1) = "\" Then
        ValidatePath = inPath
    Else
        ValidatePath = inPath & "\"
    End If
    
End Function

Public Function DeValidatePath(inPath As String) As String

    If Right(inPath, 1) = "\" Then
        DeValidatePath = Left(inPath, Len(inPath) - 1)
    Else
        DeValidatePath = inPath
    End If
    
End Function

Public Function TrimStr(inText As String) As String

    TrimStr = Trim(Replace(inText, Chr(0), ""))

End Function

Public Function CutStr(inText As String) As String

    Dim tmpStr As String
    tmpStr = Left(inText, InStr(1, inText, Chr(0), vbTextCompare) - 1)
    CutStr = Trim(tmpStr)

End Function

Public Function OpenUrl(szUrl As String)

    ShellExecute &O0, "open", szUrl, "", "", 0

End Function

Public Function OpenFile(szFile As String)

    ShellExecute &O0, "open", szFile, "", "", 0

End Function

Public Function MkNetwarePath(szPath As String)
    Dim outPath As String
    Dim pos As Integer

    outPath = szPath
    If Left(szPath, 2) = "\\" Then  ' UNC Path?
        outPath = Mid(szPath, 3)
        
        pos = InStr(1, outPath, "\")
        pos = InStr(pos + 1, outPath, "\")
        outPath = UCase(Left(outPath, pos - 1)) & ":" & Mid(outPath, pos)
    
    ElseIf Mid(szPath, 2, 1) = ":" Then 'DOS Path?
        outPath = MkNetwarePath(Server_GetUNCPath(szPath) & ":" & Mid(szPath, 3))
    End If

    MkNetwarePath = outPath
End Function

Public Function MkUNCPath(szPath As String, szServerName As String)
    Dim outPath As String
    Dim pos As Integer
    Dim pos2 As Integer

    outPath = szPath
    If InStr(3, szPath, ":") > 2 Then ' Netware Path?
        pos = InStr(1, szPath, ":")
        pos2 = InStr(1, szPath, "/")
        If pos2 = 0 Then pos2 = InStr(1, szPath, "\")
        
        If Not (pos2 > pos) Then
            
            outPath = "\\" & Left(szPath, pos - 1)
            If Mid(szPath, pos + 1, 1) = "\" Then
                outPath = outPath & Mid(szPath, pos + 1)
                Else
                outPath = outPath & "\" & Mid(szPath, pos + 1)
            End If
            
        Else
        
            outPath = "\\" & szServerName & "\" & Left(szPath, pos - 1)
            If Mid(szPath, pos + 1, 1) = "\" Then
                outPath = outPath & Mid(szPath, pos + 1)
                Else
                outPath = outPath & "\" & Mid(szPath, pos + 1)
            End If
            
        End If
    ElseIf Mid(szPath, 2, 1) = ":" Then 'DOS Path?
        outPath = Server_GetUNCPath(szPath) & Mid(szPath, 3)
    End If

    MkUNCPath = outPath
End Function

