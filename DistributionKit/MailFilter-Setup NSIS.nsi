;
;  (c) Copyright 2001-2004 Christian Hofstaedtler
;
;  This is the MailFilter professional Installation Script. 
;
;  Use this script with NSIS v2.0.

; *** UPDATE BUILD NUMBERS *HERE* ***
  !define PROD_VERSION_MAJOR 1
  !define PROD_VERSION_MINOR 6
  !define PROD_VERSION_REVISION 1
;
;

; Product description
  !define PROD_NAME "MailFilter professional"
  !define PROD_VERSION ${PROD_VERSION_MAJOR}.${PROD_VERSION_MINOR}.${PROD_VERSION_REVISION}

  Name "${PROD_NAME}"
  BrandingText "MailFilter Local Installation"

  !include "MUI.nsh"

  SetCompressor lzma
  SetDateSave on
  SetOverwrite on
  SetCompress auto
  SetDatablockOptimize on
  ShowInstDetails hide
  InstType "${PROD_NAME} ${PROD_VERSION}"
  OutFile "MailFilter-Setup-${PROD_VERSION_MAJOR}${PROD_VERSION_MINOR}${PROD_VERSION_REVISION}.exe"
  InstallDir "$PROGRAMFILES\MailFilter professional ${PROD_VERSION}"
;  InstallDirRegKey HKLM "SOFTWARE\MailFilter\${PROD_VERSION}" "AdminInstallDir"

  !define MUI_ICON "nsis-src\install.ico"
  !define MUI_UNICON "nsis-src\uninst.ico"

  !define MUI_WELCOMEFINISHPAGE_BITMAP "..\Graphics\mailfilter-wizard-new-small.bmp"

;  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
;  !define MUI_LANGDLL_REGISTRY_KEY "Software\${PROD_NAME}" 
;  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

  !define MUI_ABORTWARNING
  !define MUI_LICENSEPAGE_RADIOBUTTONS
;  !define MUI_COMPONENTSPAGE_NODESC
  !define MUI_FINISHPAGE_NOAUTOCLOSE
;  !define MUI_FINISHPAGE_RUN "$INSTDIR\MFInstallWizard.exe"
;  !define MUI_FINISHPAGE_RUN_NOTCHECKED
  !define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\docs\notes\ReleaseNotes.en.html"

;  !insertmacro MUI_RESERVEFILE_WELCOMEFINISHPAGE
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_DIRECTORY
;  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
;  !insertmacro MUI_RESERVEFILE_LANGDLL

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES


  !insertmacro MUI_LANGUAGE "English"
;  LangString NAME_Section_Base ${LANG_English} "Server Binaries (NLMs)"
;  LangString NAME_Section_Installer ${LANG_English} "Installation Wizard"
;  LangString NAME_Section_Docs ${LANG_English} "Documentation"
;  LangString NAME_Section_ConfigW32 ${LANG_English} "Configuration Editor for Windows"

;  !insertmacro MUI_LANGUAGE "German"
;
;  LangString NAME_Section_Base ${LANG_German} "Server-Module (NLMs)"
;  LangString NAME_Section_Installer ${LANG_German} "Installationsassistent"
;  LangString NAME_Section_Docs ${LANG_German} "Dokumentation"
;  LangString NAME_Section_ConfigW32 ${LANG_German} "Konfigurationseditor für Windows"


; The stuff to install
;Section $(NAME_Section_Base) Section_Base
Section All
  SetShellVarContext all
  SectionIn 1 RO
  SetOverwrite on

  ;; Check for old MSI Installation
  DetailPrint "Uninstalling previous versions..."

  ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{9440BC33-927B-4721-AB64-A40D3500E16A}" "UninstallString"
  IfErrors msi_check_done
	ExecWait "MsiExec.exe /X{9440BC33-927B-4721-AB64-A40D3500E16A} /qn"
  msi_check_done:
	ClearErrors
  ;; Done

  ;; cleanup previous installation (-> same version only)
  RMDir /r "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}"
  RMDir /r "$INSTDIR"


  ; Set output path to the installation directory.
  SetOutPath "$INSTDIR"
  ; try again :/
  SetOutPath "$INSTDIR"

  File "..\MailFilter\out\FilterEdit.exe"
  File "..\MailFilter\out\MFFilterICE.exe"

  SetOutPath "$INSTDIR\SERVER"

;
; Copy MailFilter Default Configuration
;
  SetOutPath "$INSTDIR\SERVER\ETC"
  File ".\src\NLM\ETC\CONFIG.NUL"
  File ".\src\NLM\ETC\FILTERS.BIN"

  File ".\src\NLM\ETC\REPORT.TPL"
  File ".\src\NLM\ETC\RINSIDE.TPL"
  File ".\src\NLM\ETC\ROUTRCPT.TPL"
  File ".\src\NLM\ETC\ROUTSNDR.TPL"
  File ".\src\NLM\ETC\MAILCOPY.TPL"

;
; Copy NLM Files
;

  SetOutPath "$INSTDIR\SERVER"
  File ".\src\NLM\SYSTEM\mfstart.ncf"
  File ".\src\NLM\SYSTEM\mfstop.ncf"

  SetOutPath "$INSTDIR\SERVER\BIN"
  File "..\MailFilter\out\MailFlt.nlm"	; LibC version
  File "..\MailFilter\out\MFLT50.nlm"	; CLib version
  File "..\MailFilter\out\MFAVA.nlm"
  File "..\MailFilter\out\MFConfig.nlm"

  File "..\MailFilter\out\EicarTst.nlm"
;  File "..\MailFilter\out\MFBUG.NLM"
;  File "..\MailFilter\out\MFPACK.NLM"


  SetOutPath "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}"
  SetOutPath "$INSTDIR"
  CreateShortCut "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}\Tools.lnk" %windir%\explorer.exe "$INSTDIR\" %windir%\explorer.exe 1
  CreateShortCut "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}\Filter Editor.lnk" '"$INSTDIR\FilterEdit.exe"'



  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "DisplayName" "${PROD_NAME} ${PROD_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "DisplayVersion" "${PROD_VERSION}"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "Publisher" "Christian Hofstaedtler"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "URLInfoAbout" "http://www.mailfilter.cc/"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "HelpLink" "http://www.mailfilter.cc/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "Readme" "file://$INSTDIR\docs\notes\ReleaseNotes.en.html"

  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "NoRepair" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}" "NoModify" 1


  WriteRegStr HKLM "SOFTWARE\MailFilter\${PROD_VERSION}" "" "$INSTDIR"
  WriteUninstaller "uninstall.exe"
;SectionEnd
;Section $(NAME_Section_Installer) Section_Installer
;  SectionIn 1
  SetOutPath "$INSTDIR"

  File "..\MailFilter\out\MFInstallWizard.exe"
;  File "..\MailFilter\out\MFUpgrWizard.exe"

  SetOutPath "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}"
  SetOutPath "$INSTDIR"
  CreateShortCut "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}\ Installation Wizard.lnk" '"$INSTDIR\MFInstallWizard.exe"'
;SectionEnd
;Section $(NAME_Section_Docs) Section_Docs
;  SectionIn 1
  SetOutPath "$INSTDIR"

;  File ".\src\Administrators Guide EN.pdf"
;  File ".\src\Administrators Guide DE.pdf"
  File /r "..\MailFilter\out\docs"

  SetOutPath "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}"
  SetOutPath "$INSTDIR"
  CreateShortCut "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}\Documentation.lnk" '"$INSTDIR\docs\index.en.html"'
;  CreateShortCut "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}\Administrators Guide (PDF - German).lnk"  '"$INSTDIR\Administrators Guide DE.pdf"'

  CreateShortCut "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}\Release Notes.lnk" '"$INSTDIR\docs\notes\ReleaseNotes.de.html"'
;  CreateShortCut "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}\Cluster Installation Note.lnk" '"$INSTDIR\docs\notes\ClusterNotes.de.html"'
SectionEnd


;Section $(NAME_Section_ConfigW32) Section_ConfigW32
;  SetOutPath "$INSTDIR"

;  File "..\MailFilter\out\MFConfig.exe"
;  File "..\MailFilter\out\MFConfig.chm"

;  File ".\src\msvcr70.dll"
;  File ".\src\mfc70.dll"
;  File ".\src\mfc70u.dll"

;  SetOutPath "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}"
;  SetOutPath "$INSTDIR"
;  CreateShortCut "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}\Configuration Editor.lnk" '"$INSTDIR\MFConfig.exe"'
;SectionEnd




; uninstall stuff

; special uninstall section.
Section "Uninstall"
  SetShellVarContext all

  DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROD_NAME} ${PROD_VERSION}"
  DeleteRegKey HKEY_CURRENT_USER "SOFTWARE\Hofstaedtler IE GmbH\MailFilter"
  DeleteRegKey /ifempty HKLM "SOFTWARE\Hofstaedtler IE GmbH"
  DeleteRegKey /ifempty HKLM "SOFTWARE\Hofstaedtler IE GmbH"

  DeleteRegKey HKLM "SOFTWARE\MailFilter\${PROD_VERSION}"
  DeleteRegKey /ifempty HKLM "SOFTWARE\MailFilter"

  RMDir /r "$SMPROGRAMS\MailFilter professional ${PROD_VERSION}"

  Delete $INSTDIR\uninstall.exe

  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR"

SectionEnd

;Function un.onInit
;  !insertmacro MUI_UNGETLANGUAGE
;FunctionEnd



; eof
