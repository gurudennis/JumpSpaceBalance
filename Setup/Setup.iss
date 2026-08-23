#define MyAppName "JumpSpaceBalance mod"
#define MyAppVersion "1.0.2.0"
#define MyAppPublisher "gurudennis"
#define MyAppURL "https://github.com/gurudennis/JumpSpaceBalance"
#define EscapeConstArgument(Value) StringChange(StringChange(StringChange(Value, "%", "%25"), ",", "%2c"), "}", "%7d")
#define SolutionDir ".."

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
CreateAppDir=no
;LicenseFile={#SolutionDir}\LICENSE.txt
; Uncomment the following line to run in non administrative install mode (install for current user only).
;PrivilegesRequired=lowest
OutputDir={#SolutionDir}\out\Setup
OutputBaseFilename=JumpSpaceBalance_Setup
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#SolutionDir}\out\Release\*"; DestDir: "{tmp}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#SolutionDir}\out\Release_Standalone\*"; DestDir: "{tmp}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#SolutionDir}\JumpSpaceBalance.Setup\bin\Release\JumpSpaceBalance.Setup.exe"; DestDir: "{tmp}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files.

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
begin
  // Trigger after all files are copied to the destination
  if CurStep = ssPostInstall then
  begin
    // Run the executable and wait for it to finish
    if Exec(ExpandConstant('{tmp}\JumpSpaceBalance.Setup.exe'), '', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
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