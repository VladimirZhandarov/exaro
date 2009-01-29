; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "eXaro"
!define PRODUCT_VERSION "1.0.1"
!define PRODUCT_PUBLISHER "OmniData S.R.L."
!define PRODUCT_WEB_SITE "http://exaro.sourceforge.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\exaro.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "exaro.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"


; License page
!insertmacro MUI_PAGE_LICENSE "COPYING"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
;!define MUI_FINISHPAGE_RUN "$INSTDIR\exaro.exe"
;!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Romanian"
!insertmacro MUI_LANGUAGE "Russian"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "eXaro_setup_1.0.0.exe"
InstallDir "$PROGRAMFILES\eXaro"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "exaro.exe"
  CreateDirectory "$SMPROGRAMS\eXaro"
  CreateShortCut "$SMPROGRAMS\eXaro\eXaro.lnk" "$INSTDIR\exaro.exe"
  CreateShortCut "$DESKTOP\eXaro.lnk" "$INSTDIR\exaro.exe"
  File "exaro.ico"
  File "Report.dll"
  File "QtZint1.dll"
  File "QtXml4.dll"
  File "QtWebKit4.dll"
  File "QtSql4.dll"
  File "QtScript4.dll"
  File "QtGui4.dll"
  File "QtDesigner4.dll"
  File "QtCore4.dll"
  File "PropertyEditor.dll"
  File "libpq.dll"
  File "ssleay32.dll"
  File "libeay32.dll"
  File "krb5_32.dll"
  File "comerr32.dll"
  File "libiconv-2.dll"
  File "libintl-2.dll"
  File "quazip.dll"
  File "mingwm10.dll"
  SetOutPath "$INSTDIR\popertyEditor"
  SetOverwrite on
  File "popertyEditor\BoolProperty.dll"
  File "popertyEditor\BrushProperty.dll"
  File "popertyEditor\ColorProperty.dll"
  File "popertyEditor\DateProperty.dll"
  File "popertyEditor\DateTimeProperty.dll"
  File "popertyEditor\EnumsProperty.dll"
  File "popertyEditor\FlagsProperty.dll"
  File "popertyEditor\FontProperty.dll"
  File "popertyEditor\NumbersProperty.dll"
  File "popertyEditor\PenProperty.dll"
  File "popertyEditor\PixmapProperty.dll"
  File "popertyEditor\RectFProperty.dll"
  File "popertyEditor\RectProperty.dll"
  File "popertyEditor\SizeFProperty.dll"
  File "popertyEditor\SizeProperty.dll"
  File "popertyEditor\StringListProperty.dll"
  File "popertyEditor\StringsProperty.dll"
  File "popertyEditor\TimeProperty.dll"
  SetOutPath "$INSTDIR\report"
  File "report\Arc.dll"
  File "report\Barcode.dll"
  File "report\Chord.dll"
  File "report\Detail.dll"
  File "report\DetailContainer.dll"
  File "report\DetailFooter.dll"
  File "report\DetailHeader.dll"
  File "report\Elipse.dll"
  File "report\Field.dll"
  File "report\HtmlScript.dll"
  File "report\Line.dll"
  File "report\Overlay.dll"
  File "report\Page.dll"
  File "report\PageFooter.dll"
  File "report\PageHeader.dll"
  File "report\Picture.dll"
  File "report\Pie.dll"
  File "report\Rectangle.dll"
  File "report\Report.dll"
  File "report\Script.dll"
  File "report\Summary.dll"
  File "report\Text.dll"
  File "report\Title.dll"
  SetOutPath "$INSTDIR\report\export"
  File "report\export\CsvTxt.dll"
  File "report\export\Html.dll"
  File "report\export\Ods.dll"

  SetOutPath "$INSTDIR\translations"
  File "translations\exaro_ro_RO.qm"
  File "translations\exaro_ru_RU.qm"
  File "translations\exaro_ru_UA.qm"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\eXaro\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\eXaro\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\exaro.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\exaro.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKCR ".bdrt" "" "eXaro.Document"
  WriteRegStr HKCR "eXaro.Document" "" "eXaro Document"
  WriteRegStr HKCR "eXaro.Document\DefaultIcon" "" "$INSTDIR\exaro.ico"
  WriteRegStr HKCR "eXaro.Document\shell\open\command" "" '"$INSTDIR\exaro.exe" "%1"'
  ;WriteRegStr HKCR "MyTextEditor.Document\shell\print\command" "" '"$INSTDIR\mytextedit.exe" /p "%1"'
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\report\export\Ods.dll"
  Delete "$INSTDIR\report\export\Html.dll"
  Delete "$INSTDIR\report\export\CsvTxt.dll"
  Delete "$INSTDIR\report\Title.dll"
  Delete "$INSTDIR\report\Text.dll"
  Delete "$INSTDIR\report\Summary.dll"
  Delete "$INSTDIR\report\Script.dll"
  Delete "$INSTDIR\report\Report.dll"
  Delete "$INSTDIR\report\Rectangle.dll"
  Delete "$INSTDIR\report\Pie.dll"
  Delete "$INSTDIR\report\Picture.dll"
  Delete "$INSTDIR\report\PageHeader.dll"
  Delete "$INSTDIR\report\PageFooter.dll"
  Delete "$INSTDIR\report\Page.dll"
  Delete "$INSTDIR\report\Overlay.dll"
  Delete "$INSTDIR\report\Line.dll"
  Delete "$INSTDIR\report\Field.dll"
  Delete "$INSTDIR\report\HtmlScript.dll"
  Delete "$INSTDIR\report\Elipse.dll"
  Delete "$INSTDIR\report\DetailHeader.dll"
  Delete "$INSTDIR\report\DetailFooter.dll"
  Delete "$INSTDIR\report\DetailContainer.dll"
  Delete "$INSTDIR\report\Detail.dll"
  Delete "$INSTDIR\report\Chord.dll"
  Delete "$INSTDIR\report\Barcode.dll"
  Delete "$INSTDIR\report\Arc.dll"
  Delete "$INSTDIR\popertyEditor\TimeProperty.dll"
  Delete "$INSTDIR\popertyEditor\StringsProperty.dll"
  Delete "$INSTDIR\popertyEditor\StringListProperty.dll"
  Delete "$INSTDIR\popertyEditor\SizeProperty.dll"
  Delete "$INSTDIR\popertyEditor\SizeFProperty.dll"
  Delete "$INSTDIR\popertyEditor\RectProperty.dll"
  Delete "$INSTDIR\popertyEditor\RectFProperty.dll"
  Delete "$INSTDIR\popertyEditor\PixmapProperty.dll"
  Delete "$INSTDIR\popertyEditor\PenProperty.dll"
  Delete "$INSTDIR\popertyEditor\NumbersProperty.dll"
  Delete "$INSTDIR\popertyEditor\FontProperty.dll"
  Delete "$INSTDIR\popertyEditor\FlagsProperty.dll"
  Delete "$INSTDIR\popertyEditor\EnumsProperty.dll"
  Delete "$INSTDIR\popertyEditor\DateTimeProperty.dll"
  Delete "$INSTDIR\popertyEditor\DateProperty.dll"
  Delete "$INSTDIR\popertyEditor\ColorProperty.dll"
  Delete "$INSTDIR\popertyEditor\BrushProperty.dll"
  Delete "$INSTDIR\popertyEditor\BoolProperty.dll"
  Delete "$INSTDIR\translations\exaro_ro_RO.qm"
  Delete "$INSTDIR\translations\exaro_ru_RU.qm"
  Delete "$INSTDIR\translations\exaro_ru_UA.qm"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\PropertyEditor.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtDesigner4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtScript4.dll"
  Delete "$INSTDIR\QtSql4.dll"
  Delete "$INSTDIR\QtWebKit4.dll"
  Delete "$INSTDIR\QtXml4.dll"
  Delete "$INSTDIR\QtZint1.dll"
  Delete "$INSTDIR\Report.dll"
  Delete "$INSTDIR\libpq.dll"
  Delete "$INSTDIR\ssleay32.dll"
  Delete "$INSTDIR\libeay32.dll"
  Delete "$INSTDIR\gssapi32.dll"
  Delete "$INSTDIR\krb5_32.dll"
  Delete "$INSTDIR\k5sprt32.dll"
  Delete "$INSTDIR\comerr32.dll"
  Delete "$INSTDIR\libiconv-2.dll"
  Delete "$INSTDIR\libintl-2.dll"
  Delete "$INSTDIR\quazip.dll"
  Delete "$INSTDIR\exaro.exe"
  Delete "$INSTDIR\exaro.ico"

  Delete "$SMPROGRAMS\eXaro\Uninstall.lnk"
  Delete "$SMPROGRAMS\eXaro\Website.lnk"
  Delete "$DESKTOP\eXaro.lnk"
  Delete "$SMPROGRAMS\eXaro\eXaro.lnk"

  RMDir "$SMPROGRAMS\eXaro"
  RMDir "$INSTDIR\report\export"
  RMDir "$INSTDIR\report"
  RMDir "$INSTDIR\translations"
  RMDir "$INSTDIR\popertyEditor"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  DeleteRegKey HKCR ".bdrt"
  DeleteRegKey HKCR "eXaro.Document"
  DeleteRegKey HKCR "eXaro.Document\DefaultIcon"
  DeleteRegKey HKCR "eXaro.Document\shell\open\command"

  SetAutoClose true
SectionEnd
