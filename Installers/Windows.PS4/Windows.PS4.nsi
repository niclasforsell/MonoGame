SetCompressor /SOLID /FINAL lzma

!include "header.nsh"
!define INSTALLERFILENAME "MonoGame.PS4"
!define APPNAME "MonoGame.PS4"

!include "Sections.nsh"
!include "MUI2.nsh"
!include "InstallOptions.nsh"

!define FrameworkPath "..\"
!define VERSION "3"
!define INSTALLERVERSION "3.0"
!define REVISION "0.0"

!define MUI_ICON "${FrameworkPath}\monogame.ico"
!define MUI_UNICON "${FrameworkPath}\monogame.ico"

Name '${APPNAME} ${INSTALLERVERSION}'
OutFile '${INSTALLERFILENAME}Installer-${INSTALLERVERSION}.exe'
InstallDir '$PROGRAMFILES\${APPNAME}\v${VERSION}'
MSBuildInstallDir '$PROGRAMFILES32\MSBuild\${APPNAME}\v${VERSION}'
VIProductVersion "${VERSION}.${REVISION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${APPNAME} Development SDK"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "MonoGame Team"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${INSTALLERVERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${INSTALLERVERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${APPNAME} Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "� Copyright MonoGame Team 2014"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;Interface Configuration

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${FrameworkPath}\monogame.bmp"
!define MUI_ABORTWARNING

!define MUI_WELCOMEFINISHPAGE_BITMAP "${FrameworkPath}\panel.bmp"
;Languages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "License.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------

; The stuff to install
Section "MonoGame Core Components" CoreComponents ;No components page, name is not important
  SectionIn RO

  ; TODO: This stuff below is a bit of a mess and i 
  ; suspect some of it can be removed now.  Some can
  ; be removed after we kill off the old XNA content 
  ; pipeline support.

  SetOutPath $MSBuildInstallDir
  File '..\monogame.ico'
  File '..\..\ThirdParty\Sony\*.targets'

  ; Install the MonoGame tools to individual subfolders both
  ; to avoid conflicting assemblies and to make it easy for end
  ; users to copy all the necessary files for distribution.

  SetOutPath $MSBuildInstallDir\2MGFX
  File /r '..\..\Tools\2MGFX\bin\x64\Release\*.exe'
  File /r '..\..\Tools\2MGFX\bin\x64\Release\*.dll'

  SetOutPath $MSBuildInstallDir\MGCB
  File /r '..\..\Tools\MGCB\bin\x64\Release\*.exe'
  File /r '..\..\Tools\MGCB\bin\x64\Release\*.dll'

  ; TODO: Add the new Pipeline GUI tool here along with
  ; registration of file extensions and shortcuts.


  ; Install PlayStation4 Assemblies
  SetOutPath '$InstallDir\Assemblies\PlayStation4'
  File '..\..\MonoGame.Framework\bin\PlayStation4\AnyCPU\Release\*.dll'
  File '..\..\MonoGame.Framework\bin\PlayStation4\AnyCPU\Release\*.prx'
  File '..\..\MonoGame.Framework\bin\PlayStation4\AnyCPU\Release\*.pdb'
  File '..\..\MonoGame.Framework\bin\PlayStation4\AnyCPU\Release\*.xml'
  

  ; Add remote programs
  WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}' 'DisplayName' '${APPNAME}'
  WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}' 'DisplayVersion' '${VERSION}'
  WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}' 'DisplayIcon' '$INSTDIR\monogame.ico'
  WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}' 'InstallLocation' '$INSTDIR\'
  WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}' 'Publisher' 'MonoGame'
  WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}' 'UninstallString' '$INSTDIR\uninstall.exe'

  SetOutPath '$InstallDir'
  File '..\monogame.ico'

  WriteUninstaller "uninstall.exe"

SectionEnd


Section "Visual Studio 2012 Templates" VS2012

  IfFileExists `$DOCUMENTS\Visual Studio 2012\Templates\ProjectTemplates\Visual C#\*.*` InstallTemplates CannotInstallTemplates
  InstallTemplates:
    SetOutPath "$DOCUMENTS\Visual Studio 2012\Templates\ProjectTemplates\Visual C#\MonoGame"
    File /r '..\..\ProjectTemplates\VisualStudio2012\PlayStation4.zip'
    GOTO EndTemplates
  CannotInstallTemplates:
    DetailPrint "Visual Studio 2012 not found"
  EndTemplates:

SectionEnd


Section "Visual Studio 2013 Templates" VS2013

  IfFileExists `$DOCUMENTS\Visual Studio 2013\Templates\ProjectTemplates\Visual C#\*.*` InstallTemplates CannotInstallTemplates
  InstallTemplates:
    SetOutPath "$DOCUMENTS\Visual Studio 2013\Templates\ProjectTemplates\Visual C#\MonoGame"
    File /r '..\..\ProjectTemplates\VisualStudio2012\PlayStation4.zip'
    GOTO EndTemplates
  CannotInstallTemplates:
    DetailPrint "Visual Studio 2013 not found"
  EndTemplates:

SectionEnd


Section "Start Menu Shortcuts" Menu

	CreateDirectory $SMPROGRAMS\${APPNAME}
	CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$InstallDir\uninstall.exe" "" "$InstallDir\uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\${APPNAME}\MGCB For PS4.lnk" "$MSBuildInstallDir\MGCB\MGCB.exe" "" "$MSBuildInstallDir\MGCB\MGCB.exe" 0
	WriteINIStr "$SMPROGRAMS\${APPNAME}\MonoGame Web Site.url" "InternetShortcut" "URL" "http://www.monogame.net"
	WriteINIStr "$SMPROGRAMS\${APPNAME}\MonoGame Web Site.url" "InternetShortcut" "IconFile" "$InstallDir\monogame.ico"
	WriteINIStr "$SMPROGRAMS\${APPNAME}\MonoGame Web Site.url" "InternetShortcut" "IconIndex" "0"

SectionEnd

LangString CoreComponentsDesc ${LANG_ENGLISH} "Install the runtime, tools, and MSBuild extensions for MonoGame.PS4"
LangString VS2012Desc ${LANG_ENGLISH} "Install the project templates for Visual Studio 2012"
LangString VS2013Desc ${LANG_ENGLISH} "Install the project templates for Visual Studio 2013"
LangString MenuDesc ${LANG_ENGLISH} "Add a link to the MonoGame website to your start menu"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${CoreComponents} $(CoreComponentsDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${OpenAL} $(OpenALDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${MonoDevelop} $(MonoDevelopDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${VS2012} $(VS2012Desc)
  !insertmacro MUI_DESCRIPTION_TEXT ${VS2013} $(VS2013Desc)
  !insertmacro MUI_DESCRIPTION_TEXT ${Menu} $(MenuDesc)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function .onInit
  IntOp $0 ${SF_SELECTED} | ${SF_RO}
  SectionSetFlags ${core_id} $0
FunctionEnd


Section "Uninstall"

  DeleteRegKey HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}'

  Delete "$DOCUMENTS\Visual Studio 2012\Templates\ProjectTemplates\Visual C#\MonoGame\PlayStation4.zip"
  Delete "$DOCUMENTS\Visual Studio 2013\Templates\ProjectTemplates\Visual C#\MonoGame\PlayStation4.zip"
  RMDir /r "$MSBuildInstallDir"
  RMDir /r "$SMPROGRAMS\${APPNAME}"

  Delete "$INSTDIR\Uninstall.exe"
  RMDir /r "$INSTDIR"

SectionEnd
