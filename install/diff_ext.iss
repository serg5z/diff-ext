[Setup]
Compression=lzma/normal
SolidCompression=yes
AppName=diff_ext
AppId=A0482097-C69D-4DEC-8AB6-D3A259ACC151
AppVerName=diff_ext 1.7.4
AppPublisher=Z
DefaultDirName={pf}\Z\diff_ext
DefaultGroupName=Z\diff_ext
DisableProgramGroupPage=false
OutputDir=.
OutputBaseFilename=diff_ext
ShowUndisplayableLanguages=yes

; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

PrivilegesRequired=none
ShowLanguageDialog=yes

[Components]
Name: "main"; Description: "diff-ext"; Types: full compact custom; Flags: fixed
Name: "i18n_ru"; Description: "Russian translation"; Types: full custom; Languages: en de ja
Name: "i18n_ru"; Description: "Русский перевод"; Types: full custom; Languages: ru
Name: "i18n_de"; Description: "German translation"; Types: full custom; Languages: en de ja
Name: "i18n_de"; Description: "Немецкий перевод"; Types: full custom; Languages: ru
Name: "i18n_ja"; Description: "Japanese translation"; Types: full custom; Languages: en de ja
Name: "i18n_ja"; Description: "Японский перевод"; Types: full custom; Languages: ru

[Files]
Source: ..\build\diff_ext.dll; DestDir: {app}; Flags: regserver restartreplace uninsrestartdelete
Source: ..\build\diff_ext_setup.exe; DestDir: {app}; Flags: 
Source: ..\build\diff_ext1049.dll; DestDir: {app}; Flags: ;Components: i18n_ru
Source: ..\build\diff_ext1031.dll; DestDir: {app}; Flags: ;Components: i18n_de
Source: ..\build\diff_ext1041.dll; DestDir: {app}; Flags: ;Components: i18n_ja
Source: ..\build\diff_ext_setup1049.dll; DestDir: {app}; Flags: ;Components: i18n_ru
Source: ..\build\diff_ext_setup1031.dll; DestDir: {app}; Flags: ;Components: i18n_de
Source: ..\build\diff_ext_setup1041.dll; DestDir: {app}; Flags: ;Components: i18n_ja
;restartreplace uninsrestartdelete ignoreversion
;regserver restartreplace uninsrestartdelete uninsremovereadonly
Source: ..\LICENSE; DestDir: {app}; Flags: uninsremovereadonly; Attribs: readonly
Source: ..\LICENSE_RU; DestDir: {app}; Attribs: readonly; Flags: uninsremovereadonly; Languages: ru

[Icons]
Name: {group}\setup; Filename: {app}\diff_ext_setup.exe; WorkingDir: {app}
Name: {group}\Uninstall diff-ext; Filename: {uninstallexe}

[Run]
Filename: {app}\diff_ext_setup.exe; Flags: postinstall

[Registry]
Root: HKCU; Subkey: Software\Z; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: Software\Z\diff-ext; Flags: uninsdeletekey 
Root: HKCU; Subkey: Software\Z\diff-ext\history; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Z\diff-ext; ValueType: string; ValueName: diff; ValueData: <path to diff>; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKCU; Subkey: Software\Z\diff-ext; ValueType: string; ValueName: diff3; ValueData: <path to diff3>; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKCU; Subkey: Software\Z\diff-ext; ValueType: dword; ValueName: language; ValueData: 1033; Flags: uninsdeletekey; Languages: en
Root: HKCU; Subkey: Software\Z\diff-ext; ValueType: dword; ValueName: language; ValueData: 1049; Flags: uninsdeletekey; Languages: ru
Root: HKCU; Subkey: Software\Z\diff-ext; ValueType: dword; ValueName: language; ValueData: 1031; Flags: uninsdeletekey; Languages: de
Root: HKCU; Subkey: Software\Z\diff-ext; ValueType: dword; ValueName: language; ValueData: 1041; Flags: uninsdeletekey; Languages: ja
Root: HKCU; Subkey: Software\Z\diff-ext; ValueType: dword; ValueName: 3way_compare_supported; ValueData: 0; Flags: uninsdeletekey createvalueifdoesntexist

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "..\LICENSE.rtf"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"; LicenseFile: "..\LICENSE_RU.rtf"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"; LicenseFile: "..\LICENSE.rtf"
Name: "ja"; MessagesFile: "japanese.isl"; LicenseFile: "..\LICENSE.rtf"

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
  dword_data: Cardinal;
  string_data: String;
  history: TArrayOfString;
  i: Integer;
begin
  if CurStep = ssPostInstall then begin
    if RegKeyExists(HKLM, 'Software\Z\diff-ext') then begin
      if RegQueryDWordValue(HKLM, 'Software\Z\diff-ext', 'language', dword_data) then begin
        RegWriteDWordValue(HKCU, 'Software\Z\diff-ext', 'language', dword_data);
      end;

      if RegQueryDWordValue(HKLM, 'Software\Z\diff-ext', '3way_compare_supported', dword_data) then begin
        RegWriteDWordValue(HKCU, 'Software\Z\diff-ext', '3way_compare_supported', dword_data);
      end;
      
      if RegQueryStringValue(HKLM, 'Software\Z\diff-ext', 'diff', string_data) then begin
        RegWriteStringValue(HKCU, 'Software\Z\diff-ext', 'diff', string_data);
      end;

      if RegQueryStringValue(HKLM, 'Software\Z\diff-ext', 'diff3', string_data) then begin
        RegWriteStringValue(HKCU, 'Software\Z\diff-ext', 'diff3', string_data);
      end;
      
      if RegGetValueNames(HKLM, 'Software\Z\diff-ext\history', history) then begin
        for i := 0 to GetArrayLength(history)-1 do begin
          RegQueryStringValue(HKLM, 'Software\Z\diff-ext\history', history[i], string_data);
          RegWriteStringValue(HKCU, 'Software\Z\diff-ext\history', history[i], string_data);
        end;
      end;
      
      RegDeleteKeyIncludingSubkeys(HKLM, 'Software\Z\diff-ext');
      RegDeleteKeyIfEmpty(HKLM, 'Software\Z');
    end;
  end;
end;

procedure CurUninstallStepChanged(CurStep: TUninstallStep);
begin
  if CurStep = usPostUninstall then begin
    if RegKeyExists(HKCR, '\Directory\shellex\ContextMenuHandlers\diff-ext') then begin
      RegDeleteKeyIncludingSubkeys(HKCR, '\Directory\shellex\ContextMenuHandlers\diff-ext');
    end;
    if RegKeyExists(HKCR, '\Folder\shellex\ContextMenuHandlers\diff-ext') then begin
      RegDeleteKeyIncludingSubkeys(HKCR, '\Folder\shellex\ContextMenuHandlers\diff-ext');
    end;
  end;
end;

