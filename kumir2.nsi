!include MUI2.nsh
 
# This include is used only by build system
# For manual build change this line to something line:
#  OutFile "kumir2-...exe"
#!include outfilename.nsh
OutFile kumir2-2.1.0-beta1.exe

Name "�����-2.x"
InstallDir "$PROGRAMFILES\Kumir2x"
RequestExecutionLevel admin


!insertmacro MUI_PAGE_LICENSE "LICENSE_RU.rtf"

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_LANGUAGE "Russian"

Section "Kumir" Kumir

	SetOutPath "$INSTDIR"
	File LICENSE_RU.rtf
	File vcredist_x86.exe
	SetOutPath "$INSTDIR\bin"
	File /r bin\*
	SetOutPath "$INSTDIR\lib"
	File /r lib\*
	SetOutPath "$INSTDIR\share"
	File /r share\*
	
	ExecWait '"$INSTDIR\vcredist_x86.exe" /passive'
	Delete /REBOOTOK "$INSTDIR\vcredist_x86.exe"

	CreateDirectory "$SMPROGRAMS\�����2"
	CreateShortCut "$SMPROGRAMS\�����2\�����-��������.lnk" "$INSTDIR\bin\kumir2-classic.exe"
	CreateShortCut "$SMPROGRAMS\�����2\����� ��� ������� �������.lnk" "$INSTDIR\bin\kumir2-highgrade.exe"
	CreateShortCut "$SMPROGRAMS\�����2\�����-���.lnk" "$INSTDIR\bin\kumir2-ide.exe"
	CreateShortCut "$SMPROGRAMS\�����2\����� ��� ��������.lnk" "$INSTDIR\bin\kumir2-teacher.exe"
	
SectionEnd