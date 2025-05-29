; diff-ext Inno Setup Installer Script

[Setup]
AppName=diff-ext Context Menu Extension
AppVersion=2.0
DefaultDirName={commonpf}\diff-ext
DisableProgramGroupPage=yes
ArchitecturesInstallIn64BitMode=x64compatible arm64
UninstallDisplayIcon={app}\diff-ext.dll
OutputDir=.
OutputBaseFilename=diff-ext-installer
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin
LicenseFile=LICENSE

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; Install the proper DLL for each architecture
Source: "x86\diff-ext-classic.dll"; DestDir: "{app}"; DestName: "diff-ext-classic.dll"; Check: IsX86
Source: "x86\diff-ext-setup.exe"; DestDir: "{app}"; DestName: "diff-ext-setup.exe"; Check: IsX86
Source: "x64\diff-ext-classic.dll"; DestDir: "{app}"; DestName: "diff-ext-classic.dll"; Check: IsX64
Source: "x64\diff-ext-setup.exe"; DestDir: "{app}"; DestName: "diff-ext-setup.exe"; Check: IsX64
Source: "arm64\diff-ext-classic.dll"; DestDir: "{app}"; DestName: "diff-ext-classic.dll"; Check: IsArm64
Source: "arm64\diff-ext-setup.exe"; DestDir: "{app}"; DestName: "diff-ext-setup.exe"; Check: IsArm64
Source: "LICENSE"; DestDir: "{app}"; DestName: "LICENSE";

[Run]
; Register DLL after install
Filename: "{sys}\regsvr32.exe"; Parameters: "/s ""{app}\diff-ext-classic.dll"""; Flags: runhidden waituntilterminated
Filename: {app}\diff_ext_setup.exe; Flags: postinstall

[UninstallRun]
; Unregister DLL on uninstall
Filename: "{sys}\regsvr32.exe"; Parameters: "/u /s ""{app}\diff-ext-classic.dll"""; Flags: runhidden waituntilterminated; RunOnceId: UnregDiffExt

[Code]
const
  MyCLSID = '{C42D835A-32CB-11F0-8E44-FAE5B572B91D}';

function IsX64: Boolean;
begin
  Result := (ProcessorArchitecture = paX64);
end;

function IsX86: Boolean;
begin
  Result := (ProcessorArchitecture = paX86);
end;

function IsArm64: Boolean;
begin
  Result := (ProcessorArchitecture = paArm64);
end;

