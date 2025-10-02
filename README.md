# Windows Setup

## Prerequisites
### 1. Visual Studio 2022 Community Edition
During install, selct:
- “Desktop development with C++” workload
- CMake tools
- Windows SDK (latest)

### 2. CMAKE (>= 3.16)
```powerwhell
winget install Kitware.CMake
```

### 3. vcpkg
```powershell
# Clone & Bootstrap
git clone https://github.com/microsoft/vcpkg.git C:\Dev\vcpkg
C:\Dev\vcpkg\bootstrap-vcpkg.bat

# Integrate vcpkg with CMake/VS
C:\Dev\vcpkg\vcpkg integrate install
```

### 4. Dependencies

Install GLFW3 via vcpkg
```powershell
C:\Dev\vcpkg\vcpkg install glfw3:x64-windows
```

## Clone the project

```powershell
git clone --recurse-submodules <your-repo-url>
cd Gump
```

## Build Steps

### 1. Create a build folder:
```powershell
mkdir build
cd build
```

### 2. Configure with CMake:
```powershell
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/Dev/vcpkg/scripts/buildsystems/vcpkg.cmake
```
If you have installed vcpkg in another place, specify it here.

### 3. Build the project
```powershell
cmake --build . --config Debug
```

For release build:
```powershell
cmake --build . --config Release
```

## Run
The executable will be located in `build/Debug/gump.exe` or in `build/Release/gump.exe` depending on the build config you choose.
```powershell
# If you have built with 'cmake --build . --config Debug'
./Debug/gemp.exe
# If you have built with 'cmake --build . --config Release'
./Release/gemp.exe
```

