The full documentation of this project is available online at https://jeanbizeul.github.io/Gump/.

# Gump Setup Guide

## Windows Setup

### Prerequisites

#### 1. Visual Studio 2022 Community Edition
During install, select:
- “Desktop development with C++” workload
- CMake tools
- Windows SDK (latest)

#### 2. CMake (>= 3.16)
```powershell
winget install Kitware.CMake
````

#### 3. vcpkg

```powershell
# Clone & Bootstrap
git clone https://github.com/microsoft/vcpkg.git C:\Dev\vcpkg
C:\Dev\vcpkg\bootstrap-vcpkg.bat

# Integrate vcpkg with CMake/VS
C:\Dev\vcpkg\vcpkg integrate install
```

#### 4. Dependencies

Install GLFW3 via vcpkg:

```powershell
C:\Dev\vcpkg\vcpkg install glfw3:x64-windows

# Or you can use the provided dependencies-windows.txt

Get-Content dependencies-windows.txt | ForEach-Object { C:/Dev/vcpkg/vcpkg.exe install $_ }
```

### Clone the project

```powershell
git clone --recurse-submodules https://github.com/JeanBizeul/Gump.git
cd Gump
```

### Build Steps

#### 1. Create a build folder

```powershell
mkdir build
cd build
```

#### 2. Configure with CMake

```powershell
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/Dev/vcpkg/scripts/buildsystems/vcpkg.cmake
```

> If vcpkg is installed in a different location, adjust the path accordingly.

#### 3. Build the project

```powershell
cmake --build . --config Debug
```

For release build:

```powershell
cmake --build . --config Release
```

### Run

The executable will be located in `build/Debug/gump.exe` or `build/Release/gump.exe` depending on the configuration.

```powershell
# Debug build
./Debug/gump.exe
# Release build
./Release/gump.exe
```

---

## Linux / WSL Setup

### Prerequisites

#### 1. Essential tools

```bash
sudo apt update
xargs -a dependencies-linux.txt sudo apt install -y
```

#### 2. OpenGL development libraries

```bash
sudo apt install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev libglfw3-dev
```

### Clone the project

```bash
git clone --recurse-submodules https://github.com/JeanBizeul/Gump.git
cd Gump
```

### Build Steps

#### 1. Create a build folder

```bash
mkdir -p build
cd build
```

#### 2. Configure with CMake

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

#### 3. Build the project

```bash
cmake --build .
```

### Run

The executable will be located in the `build/` directory:

```bash
./gump
```

> Optionally, you can change `CMAKE_BUILD_TYPE` to `Release` for a release build:
>
> ```bash
> cmake .. -DCMAKE_BUILD_TYPE=Release
> cmake --build .
> ./gump
> ```

### Notes for WSL

* If using WSL2, ensure you have a working X server or WSLg enabled to run OpenGL applications.
* On headless setups without GPU forwarding, the program may compile but not display graphics.

---

## Cross-Platform Build Script

You can use the included **build script** to automate building and optionally running the app on both Windows and Linux/WSL.

They are located under the `/scripts` folder, you may need to use `chmod` to be able to run them on Linux/WSL:

```bash
chmod +x ./scripts/build-*-linux.sh
```
