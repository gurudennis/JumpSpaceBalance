set SOLUTION_DIR=%1
set PROJECT_DIR=%2
set CONFIGURATION=%3

mkdir %SOLUTION_DIR%out
mkdir %SOLUTION_DIR%out\%CONFIGURATION%

echo %CONFIGURATION% | findstr /i "Standalone" >nul
if %ERRORLEVEL% neq 0 (
    REM Melon package (only when not Standalone)
    mkdir %SOLUTION_DIR%out\%CONFIGURATION%\UserLibs
    copy %SOLUTION_DIR%x64\%CONFIGURATION%\JumpSpaceBalance.dll %SOLUTION_DIR%out\%CONFIGURATION%\UserLibs\
) else (
    REM Standalone package
    copy %SOLUTION_DIR%x64\%CONFIGURATION%\JumpSpaceBalance.dll %SOLUTION_DIR%out\%CONFIGURATION%\
    copy %PROJECT_DIR%JumpSpaceBalance_readme.txt %SOLUTION_DIR%out\%CONFIGURATION%\
    copy %PROJECT_DIR%JumpSpaceBalance_uninstall.cmd %SOLUTION_DIR%out\%CONFIGURATION%\
)
