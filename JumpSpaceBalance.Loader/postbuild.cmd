set SOLUTION_DIR=%1
set PROJECT_DIR=%2
set CONFIGURATION=%3

REM Copy to the native build directory
copy %PROJECT_DIR%bin\%CONFIGURATION%\net6.0\* %SOLUTION_DIR%x64\%CONFIGURATION%\

REM Melon package
mkdir %SOLUTION_DIR%out
mkdir %SOLUTION_DIR%out\%CONFIGURATION%
mkdir %SOLUTION_DIR%out\%CONFIGURATION%\Mods
copy %PROJECT_DIR%bin\%CONFIGURATION%\net6.0\JumpSpaceBalance.Loader.dll %SOLUTION_DIR%out\%CONFIGURATION%\Mods\
copy %PROJECT_DIR%JumpSpaceBalance_readme.txt %SOLUTION_DIR%out\%CONFIGURATION%\
