Attribute VB_Name = "modConfigurator"
Option Explicit

Public MFC_MF_InstallDir As String

Public Function MF_InstallFile(logNum As Integer, fileName As String, DestDir As String, Optional AlwaysOverwrite = False, Optional LocalSource = "") As Boolean

    On Local Error GoTo MF_InstallFile_Err
    
    Dim noErrors As Boolean
    Dim FileStr As String
    Dim DestStr As String
    
    If LocalSource = "" Then LocalSource = DestDir
    
    FileStr = "NLM\" & LocalSource & "\" & fileName
    DestStr = ValidatePath(MFC_MF_InstallDir) & DestDir & "\" & fileName
    
    noErrors = True

    On Error Resume Next
        MkDir ValidatePath(MFC_MF_InstallDir) & DestDir
        Err.Clear
    On Error GoTo MF_InstallFile_Err
    
    
    On Local Error Resume Next
    Err.Clear
    SetAttr DestStr, vbNormal
    If Err.Number = 0 Then
        
        If (AlwaysOverwrite = False) Then
            Print #logNum, "   * File already exists, Overwrite = False"
            GoTo MF_InstallFile_Done
        End If
    
        If (AlwaysOverwrite = True) Then
            Print #logNum, "   - File already exists, Overwrite = True"
            Kill DestStr
        End If
    
        Else
            Err.Clear
    End If
    
    If Dir(ValidatePath(App.Path) & FileStr, vbNormal) <> "" Then
            FileCopy ValidatePath(App.Path) & FileStr, DestStr
        Else
            noErrors = False
            Print #logNum, "   * Source File " & FileStr & " Missing!"
            Err.Raise vbObjectError + 4501, "MF_InstallFile", "Source File " & FileStr & " Missing!"
    End If
    
    
MF_InstallFile_Done:
    MF_InstallFile = noErrors
    Exit Function
    
MF_InstallFile_Err:
    noErrors = False
    Print #logNum, "   * ERROR: " & Err.Number & " - " & Err.Description
    Err.Clear
    Resume MF_InstallFile_Done
End Function

