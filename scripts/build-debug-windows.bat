@echo off
setlocal

REM Go to project root
cd /d "%~dp0.."

REM Ensure build directory exists
if not exist "build" mkdir "build"
cd build

REM Configure with vcpkg toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/Dev/vcpkg/scripts/buildsystems/vcpkg.cmake
if %errorlevel% neq 0 exit /b %errorlevel%

REM Build (Debug by default)
cmake --build . --config Debug
if %errorlevel% neq 0 exit /b %errorlevel%

REM Ask user if they want to run the app
echo.
echo Build completed successfully.
echo Program located at: %cd%\Debug\gump.exe
set /p runchoice="Do you want to run Gump directly ? [Y/n] "

if /i "%runchoice%"=="n" (
    echo Skipping launch.
) else (
    echo Launching Gump...
    ".\Debug\gump.exe"
)

endlocal
