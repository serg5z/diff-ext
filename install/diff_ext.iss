[Setup]
Compression=zip
AppName=diff_ext
AppId=A0482097-C69D-4DEC-8AB6-D3A259ACC151
AppVerName=diff_ext 0.11
AppPublisher=Z
DefaultDirName={pf}\Z\diff_ext
DefaultGroupName=Z\diff_ext
DisableProgramGroupPage=no
OutputBaseFilename=diff_ext

; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Files]
Source: "..\build\diff_ext.dll"; DestDir: "{app}"; Flags: regserver restartreplace uninsrestartdelete
;restartreplace uninsrestartdelete ignoreversion
;regserver restartreplace uninsrestartdelete uninsremovereadonly
Source: "..\LICENSE"; DestDir: "{app}"; Flags: uninsremovereadonly

[Registry]
Root: HKLM; Subkey: "Software\Z\diff_ext"; ValueType: string; ValueName: "diff"; ValueData: "tkdiff"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Z"; Flags: uninsdeletekeyifempty uninsdeletevalue

;Root: HKCR; Subkey: "*\shellex\ContextMenuHandlers\diff_ext"; ValueType: string; ValueName: ""; ValueData: "{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "CLSID\{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}"; ValueType: string; ValueName: ""; ValueData: "diff_ext"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "CLSID\{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}\InprocServer32"; ValueType: string; ValueName: ""; ValueData: "{app}\diff_ext.dll"
;Root: HKCR; Subkey: "CLSID\{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}\InprocServer32"; ValueType: string; ValueName: "ThreadingModel"; ValueData: "Apartment"
;Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved"; ValueType: string; ValueName: "{{A0482097-C69D-4DEC-8AB6-D3A259ACC151}"; Flags: uninsdeletevalue

