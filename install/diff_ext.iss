[Setup]
Compression=zip
AppName=diff_ext
AppId=A0482097-C69D-4DEC-8AB6-D3A259ACC151
AppVerName=diff_ext 0.25
AppPublisher=Z
DefaultDirName={pf}\Z\diff_ext
DefaultGroupName=Z\diff_ext
DisableProgramGroupPage=false
OutputDir=.
OutputBaseFilename=diff_ext-0.25

; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

PrivilegesRequired=none
ShowLanguageDialog=yes

LicenseFile=..\LICENSE.rtf
[Files]
Source: ..\build\diff_ext.dll; DestDir: {app}; Flags: regserver restartreplace uninsrestartdelete
Source: ..\build\diff_ext_setup.exe; DestDir: {app}; Flags: 
;restartreplace uninsrestartdelete ignoreversion
;regserver restartreplace uninsrestartdelete uninsremovereadonly
Source: ..\LICENSE; DestDir: {app}; Flags: uninsremovereadonly; Attribs: readonly

Source: ..\LICENSE_RU; DestDir: {app}; Attribs: readonly; Flags: uninsremovereadonly; Languages: ru
[Icons]
Name: {group}\setup; Filename: {app}\diff_ext_setup.exe; WorkingDir: {app}
Name: {group}\Uninstall diff-ext; Filename: {uninstallexe}

[Run]
Filename: {app}\diff_ext_setup.exe

[Registry]
Root: HKLM; Subkey: Software\Z\diff_ext; ValueType: string; ValueName: diff; ValueData: <path to diff>; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKLM; Subkey: Software\Z\diff_ext; ValueType: string; ValueName: home; ValueData: {app}; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKLM; Subkey: Software\Z\diff_ext; ValueType: dword; ValueName: language; ValueData: 1033; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKLM; Subkey: Software\Z; Flags: uninsdeletekeyifempty uninsdeletevalue

;Root: HKCR; Subkey: "*\shellex\ContextMenuHandlers\diff_ext"; ValueType: string; ValueName: ""; ValueData: "{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "CLSID\{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}"; ValueType: string; ValueName: ""; ValueData: "diff_ext"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "CLSID\{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}\InprocServer32"; ValueType: string; ValueName: ""; ValueData: "{app}\diff_ext.dll"
;Root: HKCR; Subkey: "CLSID\{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}\InprocServer32"; ValueType: string; ValueName: "ThreadingModel"; ValueData: "Apartment"
;Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved"; ValueType: string; ValueName: "{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}"; Flags: uninsdeletevalue

[Languages]
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: ..\LICENSE.rtf
Name: ru; MessagesFile: russian.isl; LicenseFile: ..\LICENSE_RU.rtf
[_ISTool]
Use7zip=false
