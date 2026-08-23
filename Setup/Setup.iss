#define MyAppName "JumpSpaceBalance mod"
#define MyAppVersion "1.0.2.0"
#define MyAppPublisher "gurudennis"
#define MyAppURL "https://github.com/gurudennis/JumpSpaceBalance"
#define EscapeConstArgument(Value) StringChange(StringChange(StringChange(Value, "%", "%25"), ",", "%2c"), "}", "%7d")
#define SolutionDir ".."

; "Debug" or "Release"
#define SetupFlavor "Debug"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{8F4B1EA8-7769-4C6C-A5E8-2B5671A49EA4}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={cm:NameAndVersion,{#EscapeConstArgument(MyAppName)},{#EscapeConstArgument(MyAppVersion)}}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
CreateAppDir=yes
;LicenseFile={#SolutionDir}\LICENSE.txt
; Uncomment the following line to run in non administrative install mode (install for current user only).
;PrivilegesRequired=lowest
OutputDir={#SolutionDir}\out\Setup
OutputBaseFilename=JumpSpaceBalance_Setup
SolidCompression=yes
WizardStyle=modern
DefaultDirName={code:FindInstallDir}
DisableDirPage=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#SolutionDir}\out\Release\*"; DestDir: "{tmp}\Release"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#SolutionDir}\out\Release_Standalone\*"; DestDir: "{tmp}\Release_Standalone"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#SolutionDir}\JumpSpaceBalance.Setup\bin\{#SetupFlavor}\JumpSpaceBalance.Setup.exe"; DestDir: "{tmp}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files.

[Code]
function GetSteamDirectory(Param: string): string;
var
  I: Integer;
  SteamPath: string;
  DriveLetter: char;
  TestPath: string;
begin
  Result := '';

  // 1. Check Current User Registry (HKCU)
  if RegQueryStringValue(HKCU, 'Software\Valve\Steam', 'SteamPath', SteamPath) then
  begin
    StringChangeEx(SteamPath, '/', '\', True);
    if DirExists(SteamPath) then
    begin
      Result := SteamPath;
      Exit;
    end;
  end;

  // 2. Check Local Machine Registry (HKLM 32-bit/64-bit views)
  // Inno Setup automatically handles Wow6432Node depending on the installer architecture,
  // but an explicit check won't hurt.
  if RegQueryStringValue(HKLM, 'SOFTWARE\Valve\Steam', 'InstallPath', SteamPath) or
     RegQueryStringValue(HKLM, 'SOFTWARE\Wow6432Node\Valve\Steam', 'InstallPath', SteamPath) then
  begin
    StringChangeEx(SteamPath, '/', '\', True);
    if DirExists(SteamPath) then
    begin
      Result := SteamPath;
      Exit;
    end;
  end;

  // 3. Fallback: Loop through ASCII values for drives C (67) to H (72)
  for I := 67 to 72 do
  begin
    DriveLetter := Chr(I);
    TestPath := DriveLetter + ':\Program Files (x86)\Steam';
    if DirExists(TestPath) then
    begin
      Result := TestPath;
      Exit;
    end;
  end;

  // 4. Fallback if Steam isn't found anywhere
  Result := ExpandConstant('{pf}\Steam'); 
end;
function FindInstallDir(Param: string): string;
begin
  Result := GetSteamDirectory('') + '\steamapps\common\JumpSpace';
end;
procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  ShowType: Integer;
  Args: String;
begin
  // Trigger after all files are copied to the destination
  if CurStep = ssPostInstall then
  begin
    // Run the executable and wait for it to finish
    if '{#SetupFlavor}' = 'Debug' then
    begin
      ShowType := SW_SHOW;
    end
    else
    begin
      ShowType := SW_HIDE;
    end;
    Args := '"' + ExpandConstant('{app}') + '"';
    if Exec(ExpandConstant('{tmp}\JumpSpaceBalance.Setup.exe'), Args, '', ShowType, ewWaitUntilTerminated, ResultCode) then
    begin
      // Check if the exit code indicates a failure (anything other than 0)
      if ResultCode <> 0 then
      begin
        // Inform the user of the failure
        MsgBox('Installation failed (Error Code: ' + IntToStr(ResultCode) + ').', mbCriticalError, MB_OK);
        Abort();
      end;
    end
    else
    begin
      // Handles cases where the file failed to launch entirely
      MsgBox('Critical error: Could not execute installation tool.', mbCriticalError, MB_OK);
      Abort();
    end;
  end;
end;