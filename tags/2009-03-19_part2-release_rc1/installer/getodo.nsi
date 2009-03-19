!define VERSION "rev92"
!define VERSIONDATE "2009-03-19"

; The name of the installer
Name "GeToDo ${VERSION} ${VERSIONDATE}"

; The file to write
OutFile "getodo-setup-${VERSION}.exe"

; The default installation directory
InstallDir $PROGRAMFILES\getodo

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\getodo" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "GeToDo (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "getodo.exe"
  File "sqlite3.dll"
  File "main-window.glade"
  File "filter-dialog.glade"
  File "recurrence-dialog.glade"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\getodo "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\getodo" "DisplayName" GeToDo
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\getodo" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\getodo" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\getodo" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\getodo"
  CreateShortCut "$SMPROGRAMS\getodo\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\getodo\getodo.lnk" "$INSTDIR\getodo.exe" "" "$INSTDIR\getodo.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\getodo"
  DeleteRegKey HKLM SOFTWARE\getodo

  ; Remove files and uninstaller
  Delete $INSTDIR\getodo.exe
  Delete $INSTDIR\sqlite3.dll
  Delete $INSTDIR\main-window.glade
  Delete $INSTDIR\filter-dialog.glade
  Delete $INSTDIR\recurrence-dialog.glade
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\getodo\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\getodo"
  RMDir "$INSTDIR"

SectionEnd
