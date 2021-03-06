#define AppName "Fubble"
#define UpdaterName "Updater"

[Setup]
AppName={#AppName}
AppPublisher=Fubble OG
AppVersion=
AppVerName={#AppName}
WizardStyle=modern
OutputBaseFilename={#AppName}Setup
DefaultDirName={userpf}\Fubble
DefaultGroupName=Fubble
UninstallDisplayIcon={app}\Fubble.exe
Compression=lzma2
SolidCompression=yes

; Disable all user prompts
DisableProgramGroupPage=yes
DisableReadyPage=yes
DirExistsWarning=no

[Files]
Source: "Updater.exe"; DestDir: "{app}"
Source: "rclone.exe"; DestDir: "{app}"
Source: "Updater.exe.config"; DestDir: "{app}"
Source: "de\Updater.resources.dll"; DestDir: "{app}\de"

[Dirs]
Name: {app}; Permissions: users-full

[UninstallDelete]
Type: filesandordirs; Name: "{app}\fubble"

[Run]
Filename: {app}\{#UpdaterName}.exe; Description: {cm:LaunchProgram,{#AppName}}; Flags: nowait postinstall skipifsilent

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#UpdaterName}.exe"
Name: "{userdesktop}\{#AppName}"; Filename: "{app}\{#UpdaterName}"