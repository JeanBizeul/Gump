@echo off
setlocal

REM Go to project root
cd /d "%~dp0.."
set "PROJECT_ROOT=%cd%"

REM Ensure build directory exists
if not exist "build" mkdir "build"
cd build

REM Init & update git submodules
echo Initializing and updating git submodules...
git submodule init
git submodule update
echo Done

REM Configure using Clang + Ninja + vcpkg
cmake .. ^
    -G Ninja ^
    -DCMAKE_C_COMPILER=clang ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_TOOLCHAIN_FILE=C:/Dev/vcpkg/scripts/buildsystems/vcpkg.cmake ^
    -DCMAKE_BUILD_TYPE=Debug

if %errorlevel% neq 0 exit /b %errorlevel%

REM Build
cmake --build .
if %errorlevel% neq 0 exit /b %errorlevel%

REM Ask user if they want to run the app
echo.
echo Build completed successfully.
echo Program located at: %PROJECT_ROOT%\build\gump.exe
set /p runchoice="Do you want to run Gump directly? [Y/n] "

if /i "%runchoice%"=="n" (
    echo Skipping launch.
) else (
    echo Launching Gump...
    pushd "%PROJECT_ROOT%"
    ".\build\Debug\gump.exe"
    popd
)

endlocal

