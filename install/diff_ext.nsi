;--------------------------------
;Include Modern UI

;!include "MUI.nsh"

; The name of the installer
Name "diff-ext"

; The file to write
OutFile "diff-ext-0.20.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Z\diff-ext

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Z\diff_ext" ""

;--------------------------------
; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;!insertmacro MUI_PAGE_COMPONENTS
;!insertmacro MUI_PAGE_DIRECTORY
;!insertmacro MUI_PAGE_INSTFILES
;  
;!insertmacro MUI_UNPAGE_CONFIRM
;!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------

; The stuff to install
Section "diff-ext (required)"
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "..\build\diff_ext.dll"
  File "..\build\diff_ext_setup.exe"
  File "..\LICENSE"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Z\diff_ext "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM SOFTWARE\Z\diff_ext "diff" "<path to diff>"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\diff-ext" "DisplayName" "diff-ext 0.20"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\diff-ext" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\diff-ext" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\diff-ext" "NoRepair" 1
  WriteUninstaller "uninstall.exe"  

  RegDLL "$INSTDIR\diff_ext.dll"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\Z\diff-ext"
  CreateShortCut "$SMPROGRAMS\Z\diff-ext\setup.lnk" "$INSTDIR\diff_ext_setup.exe" "" "$INSTDIR\diff_ext_setup.exe" 0
  CreateShortCut "$SMPROGRAMS\Z\diff-ext\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Section "Initial setup"
  ExecWait "$INSTDIR\diff_ext_setup.exe"
SectionEnd

;--------------------------------
; Uninstaller
Section "Uninstall"
    UnRegDLL "$INSTDIR\diff_ext.dll"
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\diff-ext"
  DeleteRegKey HKLM SOFTWARE\Z\diff_ext
  DeleteRegKey /ifempty HKLM SOFTWARE\Z

  ; Remove files and uninstaller
  Delete /REBOOTOK $INSTDIR\diff_ext.dll
  Delete $INSTDIR\LICENSE
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Z\diff-ext\*.*"

  ; Remove directories used
  RMDir /REBOOTOK "$SMPROGRAMS\Z\diff-ext"
  RMDir /REBOOTOK "$INSTDIR"
SectionEnd